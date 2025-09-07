/*
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

enum Bgs
{
    // self explanatory
    BG_TEXT = 0,
    // The actual background made of tiles
    BG_BACKDROP_BLACK,
    BG_BACKDROP_FIRE,
    BG_BACKDROP_FIELD,
    BG_COUNT,
};

enum WindowIds
{
    WINDOW_TOP = 0,
    WINDOW_MID,
    WINDOW_BOT,
    WINDOW_COUNT,
};

enum SpeechLoadStates
{
    STATE_RESET = 0,
    STATE_ALLOC,
    STATE_REGS,
    STATE_BGS,
    STATE_BG_GFX,
    STATE_WINDOWS,
    STATE_FINISH,
    STATE_COUNT,
};

struct IntroSequence
{
    MainCallback savedCallback;
    u16 blackTilemapBuffer[0x800];
    u16 fireTilemapBuffer[0x800];
    u16 fieldTilemapBuffer[0x800];
    s16 alphaCoeff;
    s16 alphaCoeff2;
    s16 timer;
    s16 fadeTimer;
    s16 counter;
    u8 loadState;
    u8 winPrint;
    bool32 fadeFinished:1;
};

// EWRAM data
static EWRAM_DATA struct IntroSequenceState *sIntroSequence = NULL;

// Function declarations

//Constants
static const u8 sFireBgText_Window1[] = _(
    "In a land far away, a world is under tyranny\n"
    "under demonic lords called the Bai'Narii."
);

static const u8 sFireBgText_Window2[] = _(
    "They ruled with an iron fist over\n"
    "the peaceful denizens of R'mhakking."
);

static const u8 sFireBgText_Window3[] = _(
    "The world was put in a persistent form\n"
    "of stasis, never to change."
);
/*
static const u8 sFieldBgText_Window1[] = _(
    "At the edges of the world, however,\n"
    "there was a whisper among the wind."
);

static const u8 sFieldBgText_Window2[] = _(
    "\n"
    "the peaceful denizens of R'mhakking."
);

static const u8 sFieldBgText_Window3[] = _(
    "The world was put in a persistent form\n"
    "of stasis, never to change."
);

static const u16 sBlackBg_BgGfx[] = INCBIN_U16("graphics/intro_sequence/black_bg.4bpp");
// static const u16 sBlackBg_BgPal[] = INCBIN_U16("graphics/intro_sequence/black_bg.gbapal");
// static const u32 sBlackBg_BgMap[] = INCBIN_U32("graphics/intro_sequence/black_bg.bin.smolTM");

static const u16 sFireBg_BgGfx[] = INCBIN_U16("graphics/intro_sequence/fire_bg.4bpp");
// static const u16 sFireBg_BgPal[] = INCBIN_U16("graphics/intro_sequence/fire_bg.gbapal");
// static const u32 sFireBg_BgMap[] = INCBIN_U32("graphics/intro_sequence/fire_bg.bin.smolTM");

static const u16 sFieldBg_BgGfx[] = INCBIN_U16("graphics/intro_sequence/field_bg.4bpp");
// static const u16 sFieldBg_BgPal[] = INCBIN_U16("graphics/intro_sequence/field_bg.gbapal");
// static const u32 sFieldBg_BgMap[] = INCBIN_U32("graphics/intro_sequence/field_bg.bin.smolTM");

static const struct BgTemplate sIntroSequence_BgTemplates[BG_COUNT] =
{
    [BG_TEXT] = {
        .bg = 0,
        .charBaseIndex = 0,
        .mapBaseIndex = 31,
        .priority = 0,
        // other unspecified attributes defaults to 0
    },
    [BG_BACKDROP_BLACK] = {
        .bg = 1,
        .charBaseIndex = 1,
        .mapBaseIndex = 30,
        .priority = 1,
    },
    [BG_BACKDROP_FIRE] = {
        .bg = 1,
        .charBaseIndex = 1,
        .mapBaseIndex = 30,
        .priority = 1,
    },
    [BG_BACKDROP_FIELD] = {
        .bg = 1,
        .charBaseIndex = 1,
        .mapBaseIndex = 30,
        .priority = 1,
    },
};

static const struct WindowTemplate sIntroSequence_WindowTemplates[] =
{
    [WINDOW_TOP] =
    {
        .bg = BG_TEXT,
        .tilemapLeft = 1,
        .tilemapTop = 1,
        .width = 28,
        .height = 5,
        .paletteNum = 15,
        .baseBlock = 1,
    },
    [WINDOW_MID] =
    {
        .bg = BG_TEXT,
        .tilemapLeft = 1,
        .tilemapTop = 7,
        .width = 28,
        .height = 5,
        .paletteNum = 15,
        .baseBlock = 0x8D, //1 + (28 * 5)
    },
    [WINDOW_BOT] =
    {
        .bg = BG_TEXT,
        .tilemapLeft = 1,
        .tilemapTop = 7,
        .width = 28,
        .height = 6,
        .paletteNum = 15,
        .baseBlock = 0x119, //0x8D + (28 * 5),
    },
    DUMMY_WINDOW_TEMPLATE,
};

// Callbacks for the sample UI
static void IntroSequence_SetupCB(void);
static void IntroSequence_MainCB(void);
static void IntroSequence_VBlankCB(void);

// Sample UI tasks
static void Task_IntroSequenceWaitFadeIn(u8 taskId);
static void Task_IntroSequenceMainInput(u8 taskId);
static void Task_IntroSequenceWaitFadeAndBail(u8 taskId);
static void Task_IntroSequenceWaitFadeAndExitGracefully(u8 taskId);

// Sample UI helper functions
static void IntroSequence_Init(MainCallback callback);
static void IntroSequence_ResetGpuRegsAndBgs(void);
static bool8 IntroSequence_InitBgs(void);
static void IntroSequence_FadeAndBail(void);
static bool8 IntroSequence_LoadGraphics(void);
static void IntroSequence_InitWindows(void);
static void IntroSequence_PrintUiWindowText(void);
static void IntroSequence_FreeResources(void);

// Declared in sample_ui.h
void Task_OpenIntroSequence_BlankTemplate(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        CleanupOverworldWindowsAndTilemaps();
        IntroSequence_Init(CB2_NewGame);
        DestroyTask(taskId);
    }
}

static void IntroSequence_Init(MainCallback callback)
{
    sIntroSequence = AllocZeroed(sizeof(struct IntroSequenceState));
    if (sIntroSequence == NULL)
    {
        SetMainCallback2(callback);
        return;
    }

    sIntroSequence->loadState = 0;
    sIntroSequence->savedCallback = callback;

    SetMainCallback2(IntroSequence_SetupCB);
}

// Credit: Jaizu, pret
static void IntroSequence_ResetGpuRegsAndBgs(void)
{
    
     * TODO : these settings are overkill, and seem to be clearing some
     * important values. I need to come back and investigate this. For now, they
     * are disabled. Note: by not resetting the various BG and GPU regs, we are
     * effectively assuming that the user of this UI is entering from the
     * overworld. If this UI is entered from a different screen, it's possible
     * some regs won't be set correctly. In that case, you'll need to figure
     * out which ones you need.
    
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

static void IntroSequence_SetupCB(void)
{
    switch (gMain.state)
    {
    case 0:
        IntroSequence_ResetGpuRegsAndBgs();
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
        if (IntroSequence_InitBgs())
        {
            sIntroSequence->loadState = 0;
            gMain.state++;
        }
        else
        {
            IntroSequence_FadeAndBail();
            return;
        }
        break;
    case 3:
        if (IntroSequence_LoadGraphics() == TRUE)
        {
            gMain.state++;
        }
        break;
    case 4:
        IntroSequence_InitWindows();
        gMain.state++;
        break;
    case 5:
        IntroSequence_PrintUiWindowText();
        CreateTask(Task_IntroSequenceWaitFadeIn, 0);
        gMain.state++;
        break;
    case 6:
        BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
        gMain.state++;
        break;
    case 7:
        SetVBlankCallback(IntroSequence_VBlankCB);
        SetMainCallback2(IntroSequence_MainCB);
        break;
    }
}

static void IntroSequence_MainCB(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    DoScheduledBgTilemapCopiesToVram();
    UpdatePaletteFade();
}

static void IntroSequence_VBlankCB(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static void Task_IntroSequenceWaitFadeIn(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        gTasks[taskId].func = Task_IntroSequenceMainInput;
    }
}

static void Task_IntroSequenceMainInput(u8 taskId)
{
    if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_PC_OFF);
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
        gTasks[taskId].func = Task_IntroSequenceWaitFadeAndExitGracefully;
    }
    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
    }
}

static void Task_IntroSequenceWaitFadeAndBail(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        SetMainCallback2(sIntroSequence->savedCallback);
        IntroSequence_FreeResources();
        DestroyTask(taskId);
    }
}

static void Task_IntroSequenceWaitFadeAndExitGracefully(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        SetMainCallback2(sIntroSequence->savedCallback);
        IntroSequence_FreeResources();
        DestroyTask(taskId);
    }
}
#define TILEMAP_BUFFER_SIZE (1024 * 2)
static bool8 IntroSequence_InitBgs(void)
{
    ResetAllBgsCoordinates();

    sBg1TilemapBuffer = AllocZeroed(TILEMAP_BUFFER_SIZE);
    if (sBg1TilemapBuffer == NULL)
    {
        return FALSE;
    }

    ResetBgsAndClearDma3BusyFlags(0);
    InitBgsFromTemplates(0, sIntroSequenceBgTemplates, NELEMS(sIntroSequenceBgTemplates));

    SetBgTilemapBuffer(1, sBg1TilemapBuffer);
    ScheduleBgCopyTilemapToVram(1);

    ShowBg(0);
    ShowBg(1);

    return TRUE;
}
#undef TILEMAP_BUFFER_SIZE

static void IntroSequence_FadeAndBail(void)
{
    BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
    CreateTask(Task_IntroSequenceWaitFadeAndBail, 0);
    SetVBlankCallback(IntroSequence_VBlankCB);
    SetMainCallback2(IntroSequence_MainCB);
}

static bool8 IntroSequence_LoadGraphics(void)
{
    switch (sIntroSequence->loadState)
    {
    case 0:
        ResetTempTileDataBuffers();
        DecompressAndCopyTileDataToVram(1, sIntroSequenceTiles, 0, 0, 0);
        sIntroSequence->loadState++;
        break;
    case 1:
        if (FreeTempTileDataBuffersIfPossible() != TRUE)
        {
            DecompressDataWithHeaderWram(sIntroSequenceTilemap, sBg1TilemapBuffer);
            sIntroSequence->loadState++;
        }
        break;
    case 2:
        LoadPalette(sIntroSequencePalette, BG_PLTT_ID(0), PLTT_SIZE_4BPP);
        LoadPalette(gMessageBox_Pal, BG_PLTT_ID(15), PLTT_SIZE_4BPP);
        sIntroSequence->loadState++;
    default:
        sIntroSequence->loadState = 0;
        return TRUE;
    }
    return FALSE;
}

static void IntroSequence_InitWindows(void)
{
    InitWindows(sIntroSequenceWindowTemplates);
    DeactivateAllTextPrinters();
    ScheduleBgCopyTilemapToVram(0);
    FillWindowPixelBuffer(WINDOW_TOP, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    PutWindowTilemap(WINDOW_TOP);
    CopyWindowToVram(WINDOW_TOP, 3);
}

static inline void IntroSequence_PrintMessageBox(const u8 *str, enum WindowIds window)
{
    FillWindowPixelBuffer(window, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));

    AddTextPrinterParameterized2(window, 
                                 FONT_NORMAL, 
                                 str, 
                                 GetPlayerTextSpeed(), 
                                 NULL, 
                                 TEXT_COLOR_DARK_GRAY, 
                                 TEXT_COLOR_WHITE, 
                                 TEXT_COLOR_LIGHT_GRAY
                                );

    CopyWindowToVram(window, COPYWIN_FULL);
}

static void IntroSequence_FreeResources(void)
{
    if (sIntroSequence != NULL)
    {
        Free(sIntroSequence);
    }
    if (sBg1TilemapBuffer != NULL)
    {
        Free(sBg1TilemapBuffer);
    }
    FreeAllWindowBuffers();
    ResetSpriteData();
}
*/