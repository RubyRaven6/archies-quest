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

struct ArchieCardState
{
    MainCallback savedCallback;
    u8 loadState;
    u8 mode;
};

enum WindowIds
{
    WINDOW_0
};

static EWRAM_DATA struct ArchieCardState *sArchieCardState = NULL;
static EWRAM_DATA u8 *sBg1TilemapBuffer = NULL;

static const struct BgTemplate sArchieCardBgTemplates[] =
{
    {
        .bg = 0,
        .charBaseIndex = 0,
        .mapBaseIndex = 31,
        .priority = 1
    },
    {
        .bg = 1,
        .charBaseIndex = 3,
        .mapBaseIndex = 30,
        .priority = 2
    }
};

static const struct WindowTemplate sArchieCardWindowTemplates[] =
{
    [WINDOW_0] =
    {
        .bg = 0,
        .tilemapLeft = 14,
        .tilemapTop = 0,
        .width = 16,
        .height = 10,
        .paletteNum = 15,
        .baseBlock = 1
    },
    DUMMY_WIN_TEMPLATE
};

static const u32 sArchieCardTiles[] = INCBIN_U32("graphics/sample_ui/tiles.4bpp.lz");

static const u32 sArchieCardTilemap[] = INCBIN_U32("graphics/sample_ui/tilemap.bin.lz");

static const u16 sArchieCardPalette[] = INCBIN_U16("graphics/sample_ui/00.gbapal");

enum FontColor
{
    FONT_WHITE,
    FONT_RED
};
static const u8 sArchieCardWindowFontColors[][3] =
{
    [FONT_WHITE]  = {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_WHITE,      TEXT_COLOR_DARK_GRAY},
    [FONT_RED]    = {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_RED,        TEXT_COLOR_LIGHT_GRAY},
};

// Callbacks for the sample UI
static void ArchieCard_SetupCB(void);
static void ArchieCard_MainCB(void);
static void ArchieCard_VBlankCB(void);

// Sample UI tasks
static void Task_ArchieCardWaitFadeIn(u8 taskId);
static void Task_ArchieCardMainInput(u8 taskId);
static void Task_ArchieCardWaitFadeAndBail(u8 taskId);
static void Task_ArchieCardWaitFadeAndExitGracefully(u8 taskId);

// Sample UI helper functions
static void ArchieCard_Init(MainCallback callback);
static void ArchieCard_ResetGpuRegsAndBgs(void);
static bool8 ArchieCard_InitBgs(void);
static void ArchieCard_FadeAndBail(void);
static bool8 ArchieCard_LoadGraphics(void);
static void ArchieCard_InitWindows(void);
static void ArchieCard_PrintUiSampleWindowText(void);
static void ArchieCard_FreeResources(void);

// Declared in sample_ui.h
void Task_OpenArchieCard_BlankTemplate(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        CleanupOverworldWindowsAndTilemaps();
        ArchieCard_Init(CB2_ReturnToFieldWithOpenMenu);
        DestroyTask(taskId);
    }
}

static void ArchieCard_Init(MainCallback callback)
{
    sArchieCardState = AllocZeroed(sizeof(struct ArchieCardState));
    if (sArchieCardState == NULL)
    {
        SetMainCallback2(callback);
        return;
    }

    sArchieCardState->loadState = 0;
    sArchieCardState->savedCallback = callback;

    SetMainCallback2(ArchieCard_SetupCB);
}

// Credit: Jaizu, pret
static void ArchieCard_ResetGpuRegsAndBgs(void)
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

static void ArchieCard_SetupCB(void)
{
    switch (gMain.state)
    {
    case 0:
        ArchieCard_ResetGpuRegsAndBgs();
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
        if (ArchieCard_InitBgs())
        {
            sArchieCardState->loadState = 0;
            gMain.state++;
        }
        else
        {
            ArchieCard_FadeAndBail();
            return;
        }
        break;
    case 3:
        if (ArchieCard_LoadGraphics() == TRUE)
        {
            gMain.state++;
        }
        break;
    case 4:
        ArchieCard_InitWindows();
        gMain.state++;
        break;
    case 5:
        ArchieCard_PrintUiSampleWindowText();
        CreateTask(Task_ArchieCardWaitFadeIn, 0);
        gMain.state++;
        break;
    case 6:
        BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
        gMain.state++;
        break;
    case 7:
        SetVBlankCallback(ArchieCard_VBlankCB);
        SetMainCallback2(ArchieCard_MainCB);
        break;
    }
}

static void ArchieCard_MainCB(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    DoScheduledBgTilemapCopiesToVram();
    UpdatePaletteFade();
}

static void ArchieCard_VBlankCB(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static void Task_ArchieCardWaitFadeIn(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        gTasks[taskId].func = Task_ArchieCardMainInput;
    }
}

static void Task_ArchieCardMainInput(u8 taskId)
{
    if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_PC_OFF);
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
        gTasks[taskId].func = Task_ArchieCardWaitFadeAndExitGracefully;
    }
    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
    }
}

static void Task_ArchieCardWaitFadeAndBail(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        SetMainCallback2(sArchieCardState->savedCallback);
        ArchieCard_FreeResources();
        DestroyTask(taskId);
    }
}

static void Task_ArchieCardWaitFadeAndExitGracefully(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        SetMainCallback2(sArchieCardState->savedCallback);
        ArchieCard_FreeResources();
        DestroyTask(taskId);
    }
}
#define TILEMAP_BUFFER_SIZE (1024 * 2)
static bool8 ArchieCard_InitBgs(void)
{
    ResetAllBgsCoordinates();

    sBg1TilemapBuffer = AllocZeroed(TILEMAP_BUFFER_SIZE);
    if (sBg1TilemapBuffer == NULL)
    {
        return FALSE;
    }

    ResetBgsAndClearDma3BusyFlags(0);
    InitBgsFromTemplates(0, sArchieCardBgTemplates, NELEMS(sArchieCardBgTemplates));

    SetBgTilemapBuffer(1, sBg1TilemapBuffer);
    ScheduleBgCopyTilemapToVram(1);

    ShowBg(0);
    ShowBg(1);

    return TRUE;
}
#undef TILEMAP_BUFFER_SIZE

static void ArchieCard_FadeAndBail(void)
{
    BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
    CreateTask(Task_ArchieCardWaitFadeAndBail, 0);
    SetVBlankCallback(ArchieCard_VBlankCB);
    SetMainCallback2(ArchieCard_MainCB);
}

static bool8 ArchieCard_LoadGraphics(void)
{
    switch (sArchieCardState->loadState)
    {
    case 0:
        ResetTempTileDataBuffers();
        DecompressAndCopyTileDataToVram(1, sArchieCardTiles, 0, 0, 0);
        sArchieCardState->loadState++;
        break;
    case 1:
        if (FreeTempTileDataBuffersIfPossible() != TRUE)
        {
            LZDecompressWram(sArchieCardTilemap, sBg1TilemapBuffer);
            sArchieCardState->loadState++;
        }
        break;
    case 2:
        LoadPalette(sArchieCardPalette, BG_PLTT_ID(0), PLTT_SIZE_4BPP);
        LoadPalette(gMessageBox_Pal, BG_PLTT_ID(15), PLTT_SIZE_4BPP);
        sArchieCardState->loadState++;
    default:
        sArchieCardState->loadState = 0;
        return TRUE;
    }
    return FALSE;
}

static void ArchieCard_InitWindows(void)
{
    InitWindows(sArchieCardWindowTemplates);
    DeactivateAllTextPrinters();
    ScheduleBgCopyTilemapToVram(0);
    FillWindowPixelBuffer(WINDOW_0, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    PutWindowTilemap(WINDOW_0);
    CopyWindowToVram(WINDOW_0, 3);
}

static const u8 sText_Text1[] = _("Hello, world!");
static const u8 sText_Text2[] = _("Press {A_BUTTON} to make a sound!");
static void ArchieCard_PrintUiSampleWindowText(void)
{
    FillWindowPixelBuffer(WINDOW_0, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));

    AddTextPrinterParameterized4(WINDOW_0, FONT_NORMAL, 0, 3, 0, 0,
        sArchieCardWindowFontColors[FONT_WHITE], TEXT_SKIP_DRAW, sText_Text1);
    AddTextPrinterParameterized4(WINDOW_0, FONT_SMALL, 0, 15, 0, 0,
        sArchieCardWindowFontColors[FONT_RED], TEXT_SKIP_DRAW, sText_Text2);

    CopyWindowToVram(WINDOW_0, COPYWIN_GFX);
}

static void ArchieCard_FreeResources(void)
{
    if (sArchieCardState != NULL)
    {
        Free(sArchieCardState);
    }
    if (sBg1TilemapBuffer != NULL)
    {
        Free(sBg1TilemapBuffer);
    }
    FreeAllWindowBuffers();
    ResetSpriteData();
}
