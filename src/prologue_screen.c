#include "puzzles.h"

#include "gba/types.h"
#include "gba/defines.h"
#include "global.h"
#include "main.h"
#include "bg.h"
#include "text_window.h"
#include "window.h"
#include "constants/characters.h"
#include "palette.h"
#include "task.h"
#include "overworld.h"
#include "malloc.h"
#include "gba/macro.h"
#include "menu_helpers.h"
#include "menu.h"
#include "scanline_effect.h"
#include "sprite.h"
#include "constants/rgb.h"
#include "decompress.h"
#include "constants/songs.h"
#include "sound.h"
#include "sprite.h"
#include "string_util.h"
#include "pokemon_icon.h"
#include "graphics.h"
#include "data.h"
#include "pokedex.h"
#include "gpu_regs.h"
#include "strings.h"
#include "field_screen_effect.h"

static void Task_Prologue_Cleanup(u8 taskId);
static bool32 PrintPrologueMessage(u8 taskId, const u8 *text, u32 x, u32 y);

struct PrologueScreenState
{
    MainCallback savedCallback;
    u8 loadState;
    u8 mode;
};

enum WindowIds
{
    WINDOW_0
};

static EWRAM_DATA struct PrologueScreenState *sPrologueScreenState = NULL;
static EWRAM_DATA u8 *sBg1TilemapBuffer = NULL;

static const struct BgTemplate sPrologueScreenBgTemplates[] =
    {
        {.bg = 0,
         .charBaseIndex = 0,
         .mapBaseIndex = 31,
         .priority = 1},
        {.bg = 1,
         .charBaseIndex = 3,
         .mapBaseIndex = 30,
         .priority = 2}};

static const struct WindowTemplate sPrologueScreenWindowTemplates[] =
    {
        [WINDOW_0] =
            {
                .bg = 0,
                .tilemapLeft = 14,
                .tilemapTop = 0,
                .width = 16,
                .height = 10,
                .paletteNum = 15,
                .baseBlock = 1},
        DUMMY_WIN_TEMPLATE};

static const u32 sPrologueScreenTiles[] = INCBIN_U32("graphics/sample_ui/tiles.4bpp.smol");

static const u32 sPrologueScreenTilemap[] = INCBIN_U32("graphics/sample_ui/tilemap.bin.smolTM");

static const u16 sPrologueScreenPalette[] = INCBIN_U16("graphics/sample_ui/00.gbapal");

enum FontColor
{
    FONT_WHITE,
    FONT_RED
};
static const u8 sPrologueScreenWindowFontColors[][3] =
    {
        [FONT_WHITE] = {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_WHITE, TEXT_COLOR_DARK_GRAY},
        [FONT_RED] = {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_RED, TEXT_COLOR_LIGHT_GRAY},
};

// Callbacks for the sample UI
static void PrologueScreen_SetupCB(void);
static void PrologueScreen_MainCB(void);
static void PrologueScreen_VBlankCB(void);

// Sample UI tasks
static void Task_PrologueScreenWaitFadeIn(u8 taskId);
static void Task_PrologueScreenMainInput(u8 taskId);
static void Task_PrologueScreenWaitFadeAndBail(u8 taskId);
static void Task_PrologueScreenWaitFadeAndExitGracefully(u8 taskId);

// Sample UI helper functions
static void PrologueScreen_Init(MainCallback callback);
static void PrologueScreen_ResetGpuRegsAndBgs(void);
static bool8 PrologueScreen_InitBgs(void);
static void PrologueScreen_FadeAndBail(void);
static bool8 PrologueScreen_LoadGraphics(void);
static void PrologueScreen_InitWindows(void);
static void PrologueScreen_PrintUiSampleWindowText(void);
static void PrologueScreen_FreeResources(void);

enum
{
    PROLOGUE_ENTER_MSG_SCREEN,
    PROLOGUE_PRINT_MSG,
    PROLOGUE_LEAVE_MSG_SCREEN,
    PROLOGUE_HEAL_SCRIPT,
};

#define tState data[0]
#define tWindowId data[1]
#define tPrintState data[2]

static const struct WindowTemplate sWindowTemplate_PrologueText =
    {
        .bg = 0,
        .tilemapLeft = 0,
        .tilemapTop = 5,
        .width = 30,
        .height = 11,
        .paletteNum = 15,
        .baseBlock = 1,
};

static const u8 sPrologueTextColors[] = {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_WHITE, TEXT_COLOR_DARK_GRAY};

static bool32 PrintPrologueMessage(u8 taskId, const u8 *text, u32 x, u32 y)
{
    u32 windowId = gTasks[taskId].tWindowId;

    switch (gTasks[taskId].tPrintState)
    {
    case 0:
        FillWindowPixelBuffer(windowId, PIXEL_FILL(0));
        StringExpandPlaceholders(gStringVar4, text);
        AddTextPrinterParameterized4(windowId, FONT_NORMAL, x, y, 1, 0, sPrologueTextColors, 1, gStringVar4);
        gTextFlags.canABSpeedUpPrint = FALSE;
        gTasks[taskId].tPrintState = 1;
        break;
    case 1:
        RunTextPrinters();
        if (!IsTextPrinterActive(windowId))
        {
            gTasks[taskId].tPrintState = 0;
            return TRUE;
        }
        break;
    }
    return FALSE;
}

void Task_OpenPrologueScreen(u8 taskId)
{
    u32 windowId;

    switch (gTasks[taskId].tState)
    {
    case PROLOGUE_ENTER_MSG_SCREEN:
        windowId = AddWindow(&sWindowTemplate_PrologueText);
        gTasks[taskId].tWindowId = windowId;
        Menu_LoadStdPalAt(BG_PLTT_ID(15));
        FillWindowPixelBuffer(windowId, PIXEL_FILL(0));
        PutWindowTilemap(windowId);
        CopyWindowToVram(windowId, COPYWIN_FULL);

        gTasks[taskId].tState = PROLOGUE_PRINT_MSG;
        break;
    case PROLOGUE_PRINT_MSG:
    {
        const u8 *msg = gText_Prologue;

        if (PrintPrologueMessage(taskId, msg, 2, 8))
        {
            gTasks[taskId].tState = PROLOGUE_LEAVE_MSG_SCREEN;
        }
        break;
    }
    case PROLOGUE_LEAVE_MSG_SCREEN:
        windowId = gTasks[taskId].tWindowId;
        ClearWindowTilemap(windowId);
        CopyWindowToVram(windowId, COPYWIN_MAP);
        RemoveWindow(windowId);
        FadeInFromBlack();
        gTasks[taskId].func = Task_Prologue_Cleanup;
        break;
    }
}

static void Task_Prologue_Cleanup(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        FreeAllWindowBuffers();
        SetMainCallback2(CB2_NewGame);
        DestroyTask(taskId);
    }
}

static void PrologueScreen_Init(MainCallback callback)
{
    sPrologueScreenState = AllocZeroed(sizeof(struct PrologueScreenState));
    if (sPrologueScreenState == NULL)
    {
        SetMainCallback2(callback);
        return;
    }

    sPrologueScreenState->loadState = 0;
    sPrologueScreenState->savedCallback = callback;

    SetMainCallback2(PrologueScreen_SetupCB);
}

// Credit: Jaizu, pret
static void PrologueScreen_ResetGpuRegsAndBgs(void)
{
    /*
     * TODO : these settings are overkill, and seem to be clearing some
     * important values. I need to come back and investigate this. For now, they
     * are disabled. Note: by not resetting the various BG and GPU regs, we are
     * effectively assuming that the user of this UI is entering from the
     * overworld. If this UI is entered from a different screen, it's possible
     * some regs won't be set correctly. In that case, you'll need to figure
     * out which ones you need.
     */
    // SetGpuReg(REG_OFFSET_DISPCNT, 0);
    // SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_OBJ_ON);
    // SetGpuReg(REG_OFFSET_BG3CNT, 0);
    // SetGpuReg(REG_OFFSET_BG2CNT, 0);
    // SetGpuReg(REG_OFFSET_BG1CNT, 0);
    // SetGpuReg(REG_OFFSET_BG0CNT, 0);
    // ChangeBgX(0, 0, BG_COORD_SET);
    // ChangeBgY(0, 0, BG_COORD_SET);
    // ChangeBgX(1, 0, BG_COORD_SET);
    // ChangeBgY(1, 0, BG_COORD_SET);
    // ChangeBgX(2, 0, BG_COORD_SET);
    // ChangeBgY(2, 0, BG_COORD_SET);
    // ChangeBgX(3, 0, BG_COORD_SET);
    // ChangeBgY(3, 0, BG_COORD_SET);
    // SetGpuReg(REG_OFFSET_BLDCNT, 0);
    // SetGpuReg(REG_OFFSET_BLDY, 0);
    // SetGpuReg(REG_OFFSET_BLDALPHA, 0);
    // SetGpuReg(REG_OFFSET_WIN0H, 0);
    // SetGpuReg(REG_OFFSET_WIN0V, 0);
    // SetGpuReg(REG_OFFSET_WIN1H, 0);
    // SetGpuReg(REG_OFFSET_WIN1V, 0);
    // SetGpuReg(REG_OFFSET_WININ, 0);
    // SetGpuReg(REG_OFFSET_WINOUT, 0);
    // CpuFill16(0, (void *)VRAM, VRAM_SIZE);
    // CpuFill32(0, (void *)OAM, OAM_SIZE);
}

static void PrologueScreen_SetupCB(void)
{
    switch (gMain.state)
    {
    case 0:
        PrologueScreen_ResetGpuRegsAndBgs();
        SetVBlankHBlankCallbacksToNull();
        ClearScheduledBgCopiesToVram();
        gMain.state++;
        break;
    case 1:
        ScanlineEffect_Stop();
        FreeAllSpritePalettes();
        ResetPaletteFade();
        ResetSpriteData();
        ResetTasks();
        gMain.state++;
        break;
    case 2:
        if (PrologueScreen_InitBgs())
        {
            sPrologueScreenState->loadState = 0;
            gMain.state++;
        }
        else
        {
            PrologueScreen_FadeAndBail();
            return;
        }
        break;
    case 3:
        if (PrologueScreen_LoadGraphics() == TRUE)
        {
            gMain.state++;
        }
        break;
    case 4:
        PrologueScreen_InitWindows();
        gMain.state++;
        break;
    case 5:
        PrologueScreen_PrintUiSampleWindowText();
        CreateTask(Task_PrologueScreenWaitFadeIn, 0);
        gMain.state++;
        break;
    case 6:
        BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
        gMain.state++;
        break;
    case 7:
        SetVBlankCallback(PrologueScreen_VBlankCB);
        SetMainCallback2(PrologueScreen_MainCB);
        break;
    }
}

static void PrologueScreen_MainCB(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    DoScheduledBgTilemapCopiesToVram();
    UpdatePaletteFade();
}

static void PrologueScreen_VBlankCB(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static void Task_PrologueScreenWaitFadeIn(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        gTasks[taskId].func = Task_PrologueScreenMainInput;
    }
}

static void Task_PrologueScreenMainInput(u8 taskId)
{
    if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_PC_OFF);
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
        gTasks[taskId].func = Task_PrologueScreenWaitFadeAndExitGracefully;
    }
    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
    }
}

static void Task_PrologueScreenWaitFadeAndBail(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        SetMainCallback2(sPrologueScreenState->savedCallback);
        PrologueScreen_FreeResources();
        DestroyTask(taskId);
    }
}

static void Task_PrologueScreenWaitFadeAndExitGracefully(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        SetMainCallback2(sPrologueScreenState->savedCallback);
        PrologueScreen_FreeResources();
        DestroyTask(taskId);
    }
}
#define TILEMAP_BUFFER_SIZE (1024 * 2)
static bool8 PrologueScreen_InitBgs(void)
{
    ResetAllBgsCoordinates();

    sBg1TilemapBuffer = AllocZeroed(TILEMAP_BUFFER_SIZE);
    if (sBg1TilemapBuffer == NULL)
    {
        return FALSE;
    }

    ResetBgsAndClearDma3BusyFlags(0);
    InitBgsFromTemplates(0, sPrologueScreenBgTemplates, NELEMS(sPrologueScreenBgTemplates));

    SetBgTilemapBuffer(1, sBg1TilemapBuffer);
    ScheduleBgCopyTilemapToVram(1);

    ShowBg(0);
    ShowBg(1);

    return TRUE;
}
#undef TILEMAP_BUFFER_SIZE

static void PrologueScreen_FadeAndBail(void)
{
    BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
    CreateTask(Task_PrologueScreenWaitFadeAndBail, 0);
    SetVBlankCallback(PrologueScreen_VBlankCB);
    SetMainCallback2(PrologueScreen_MainCB);
}

static bool8 PrologueScreen_LoadGraphics(void)
{
    switch (sPrologueScreenState->loadState)
    {
    case 0:
        ResetTempTileDataBuffers();
        DecompressAndCopyTileDataToVram(1, sPrologueScreenTiles, 0, 0, 0);
        sPrologueScreenState->loadState++;
        break;
    case 1:
        if (FreeTempTileDataBuffersIfPossible() != TRUE)
        {
            DecompressDataWithHeaderWram(sPrologueScreenTilemap, sBg1TilemapBuffer);
            sPrologueScreenState->loadState++;
        }
        break;
    case 2:
        LoadPalette(sPrologueScreenPalette, BG_PLTT_ID(0), PLTT_SIZE_4BPP);
        LoadPalette(gMessageBox_Pal, BG_PLTT_ID(15), PLTT_SIZE_4BPP);
        sPrologueScreenState->loadState++;
    default:
        sPrologueScreenState->loadState = 0;
        return TRUE;
    }
    return FALSE;
}

static void PrologueScreen_InitWindows(void)
{
    InitWindows(sPrologueScreenWindowTemplates);
    DeactivateAllTextPrinters();
    ScheduleBgCopyTilemapToVram(0);
    FillWindowPixelBuffer(WINDOW_0, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    PutWindowTilemap(WINDOW_0);
    CopyWindowToVram(WINDOW_0, 3);
}

static const u8 sText_Text1[] = _("Hello, world!");
static const u8 sText_Text2[] = _("Press {A_BUTTON} to make a sound!");
static void PrologueScreen_PrintUiSampleWindowText(void)
{
    FillWindowPixelBuffer(WINDOW_0, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));

    AddTextPrinterParameterized4(WINDOW_0, FONT_NORMAL, 0, 3, 0, 0,
                                 sPrologueScreenWindowFontColors[FONT_WHITE], TEXT_SKIP_DRAW, sText_Text1);
    AddTextPrinterParameterized4(WINDOW_0, FONT_SMALL, 0, 15, 0, 0,
                                 sPrologueScreenWindowFontColors[FONT_RED], TEXT_SKIP_DRAW, sText_Text2);

    CopyWindowToVram(WINDOW_0, COPYWIN_GFX);
}

static void PrologueScreen_FreeResources(void)
{
    if (sPrologueScreenState != NULL)
    {
        Free(sPrologueScreenState);
    }
    if (sBg1TilemapBuffer != NULL)
    {
        Free(sBg1TilemapBuffer);
    }
    FreeAllWindowBuffers();
    ResetSpriteData();
}
