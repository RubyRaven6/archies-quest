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

#include "event_data.h"

static void DaggerCallback(struct Sprite *sprite);
static u8 CreateDagger();
static void Destroydagger();
struct NessiePuzzleState
{
    MainCallback savedCallback;
    u8 loadState;
    u16 daggerSpriteId;
    u16 dagger_x;
    u16 dagger_y;
};

enum WindowIds
{
    WINDOW_NO_STAB, // Instructions without stab
    WINDOW_STAB, // Instructions with stab
    WINDOW_STAB_AREA  // Window for where to stab
};

static EWRAM_DATA struct NessiePuzzleState *sNessiePuzzleState = NULL;
static EWRAM_DATA u8 *sBg1TilemapBuffer = NULL;

#define TAG_DAGGER 30004

static const u16 sDaggerCursor_Pal[] = INCBIN_U16("graphics/nessie_puzzle/dagger.gbapal");
static const u32 sDaggerCursor_Gfx[] = INCBIN_U32("graphics/nessie_puzzle/dagger.4bpp.lz");

static const struct OamData sOamData_Dagger =
{
    .size = SPRITE_SIZE(32x32),
    .shape = SPRITE_SHAPE(32x32),
    .priority = 1,
};

static const struct CompressedSpriteSheet sSpriteSheet_Dagger =
{
    .data = sDaggerCursor_Gfx,
    .size = 32*32/2,
    .tag = TAG_DAGGER,
};

static const struct SpritePalette sSpritePal_Dagger =
{
    .data = sDaggerCursor_Pal,
    .tag = TAG_DAGGER
};

static const struct SpriteTemplate sSpriteTemplate_Dagger =
{
    .tileTag = TAG_DAGGER,
    .paletteTag = TAG_DAGGER,
    .oam = &sOamData_Dagger,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .callback = DaggerCallback
};

static const struct BgTemplate sNessiePuzzleBgTemplates[] =
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

static const struct WindowTemplate sNessiePuzzleWindowTemplates[] =
{
    [WINDOW_NO_STAB] =
    {
        .bg = 0,
        .tilemapLeft = 13,
        .tilemapTop = 16,
        .width = 8,
        .height = 2,
        .paletteNum = 15,
        .baseBlock = 1
    },
    [WINDOW_STAB] =
    {
        .bg = 0,
        .tilemapLeft = 11,
        .tilemapTop = 16,
        .width = 8,
        .height = 2,
        .paletteNum = 15,
        .baseBlock = 1
    },
    [WINDOW_STAB_AREA] =
    {
        .bg = 1,
        .tilemapLeft = 2,
        .tilemapTop = 2,
        .width = 26,
        .height = 14,
        .paletteNum = 15,
        .baseBlock = 17
    },
    DUMMY_WIN_TEMPLATE
};

static const u32 sNessiePaintings[] = INCBIN_U32("graphics/nessie_puzzle/nessie_painting_map.4bpp.lz");
static const u32 sNessiePaintingMap[] = INCBIN_U32("graphics/nessie_puzzle/nessie_painting_map.bin.lz");
static const u16 sNessiePaintingPalette[] = INCBIN_U16("graphics/nessie_puzzle/nessie_painting.gbapal");

enum FontColor
{
    FONT_WHITE,
    FONT_RED
};
static const u8 sNessiePuzzleWindowFontColors[][3] =
{
    [FONT_WHITE]  = {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_WHITE,      TEXT_COLOR_DARK_GRAY},
    [FONT_RED]    = {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_RED,        TEXT_COLOR_LIGHT_GRAY},
};

// Callbacks for the sample UI
static void NessiePuzzle_SetupCB(void);
static void NessiePuzzle_MainCB(void);
static void NessiePuzzle_VBlankCB(void);

// Sample UI tasks
static void Task_NessiePuzzleWaitFadeIn(u8 taskId);
static void Task_NessiePuzzleMainInput(u8 taskId);
static void Task_NessiePuzzleWaitFadeAndBail(u8 taskId);
static void Task_NessiePuzzleWaitFadeAndExitGracefully(u8 taskId);

// Sample UI helper functions
static void NessiePuzzle_Init(MainCallback callback);
static void NessiePuzzle_ResetGpuRegsAndBgs(void);
static bool8 NessiePuzzle_InitBgs(void);
static void NessiePuzzle_FadeAndBail(void);
static bool8 NessiePuzzle_LoadGraphics(void);
static void NessiePuzzle_InitWindows(void);
static void NessiePuzzle_PrintUiSampleWindowText(void);
static void NessiePuzzle_FreeResources(void);

// Declared in nessie_puzzle.h
void Task_OpenNessiePainting(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        CleanupOverworldWindowsAndTilemaps();
        NessiePuzzle_Init(CB2_ReturnToFieldContinueScript);
        DestroyTask(taskId);
    }
}

static void NessiePuzzle_Init(MainCallback callback)
{
    sNessiePuzzleState = AllocZeroed(sizeof(struct NessiePuzzleState));
    if (sNessiePuzzleState == NULL)
    {
        SetMainCallback2(callback);
        return;
    }

    sNessiePuzzleState->loadState = 0;
    sNessiePuzzleState->savedCallback = callback;
    sNessiePuzzleState->daggerSpriteId = 0xFF;

    SetMainCallback2(NessiePuzzle_SetupCB);
}

// Credit: Jaizu, pret
static void NessiePuzzle_ResetGpuRegsAndBgs(void)
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

static void NessiePuzzle_SetupCB(void)
{
    switch (gMain.state)
    {
    case 0:
        NessiePuzzle_ResetGpuRegsAndBgs();
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
        if (NessiePuzzle_InitBgs())
        {
            sNessiePuzzleState->loadState = 0;
            gMain.state++;
        }
        else
        {
            NessiePuzzle_FadeAndBail();
            return;
        }
        break;
    case 3:
        if (NessiePuzzle_LoadGraphics() == TRUE)
        {
            gMain.state++;
        }
        break;
    case 4:
        LoadCompressedSpriteSheet(&sSpriteSheet_Dagger);
        LoadSpritePalette(&sSpritePal_Dagger);
        gMain.state++;
        break;
    case 5:
        NessiePuzzle_InitWindows();
        if(FlagGet(FLAG_NESSIE_READ_BOOK) && FlagGet(FLAG_NESSIE_GOT_DAGGER)){
            CreateDagger();
        };
        gMain.state++;
        break;
    case 6:
        NessiePuzzle_PrintUiSampleWindowText();
        CreateTask(Task_NessiePuzzleWaitFadeIn, 0);
        gMain.state++;
        break;
    case 7:
        BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
        gMain.state++;
        break;
    default:
        SetVBlankCallback(NessiePuzzle_VBlankCB);
        SetMainCallback2(NessiePuzzle_MainCB);
        break;
    }
}

static void NessiePuzzle_MainCB(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    DoScheduledBgTilemapCopiesToVram();
    UpdatePaletteFade();
}

static void NessiePuzzle_VBlankCB(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static void Task_NessiePuzzleWaitFadeIn(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        gTasks[taskId].func = Task_NessiePuzzleMainInput;
    }
}

#define DAGGER_MULTIPLIER   

static void DaggerCallback(struct Sprite *sprite)
{
    // Reminder of harsher times.
    // struct SpriteCordsStruct spriteCords[14][27] = { //Thanks Jaizu
    //     {{32, 32},	{40, 32},	{48, 32},	{56, 32},	{64, 32},	{72, 32},	{80, 32},	{88, 32},	{96, 32},	{104, 32},	{112, 32},	{120, 32},	{128, 32},	{136, 32},	{144, 32},	{152, 32},	{160, 32},	{168, 32},	{176, 32},	{184, 32},	{192, 32},	{200, 32},	{208, 32},	{216, 32},	{224, 32},	{232, 32},	{240, 32}},
    //     {{32, 40},	{40, 40},	{48, 40},	{56, 40},	{64, 40},	{72, 40},	{80, 40},	{88, 40},	{96, 40},	{104, 40},	{112, 40},	{120, 40},	{128, 40},	{136, 40},	{144, 40},	{152, 40},	{160, 40},	{168, 40},	{176, 40},	{184, 40},	{192, 40},	{200, 40},	{208, 40},	{216, 40},	{224, 40},	{232, 40},	{240, 40}},
    //     {{32, 48},	{40, 48},	{48, 48},	{56, 48},	{64, 48},	{72, 48},	{80, 48},	{88, 48},	{96, 48},	{104, 48},	{112, 48},	{120, 48},	{128, 48},	{136, 48},	{144, 48},	{152, 48},	{160, 48},	{168, 48},	{176, 48},	{184, 48},	{192, 48},	{200, 48},	{208, 48},	{216, 48},	{224, 48},	{232, 48},	{240, 48}},
    //     {{32, 56},	{40, 56},	{48, 56},	{56, 56},	{64, 56},	{72, 56},	{80, 56},	{88, 56},	{96, 56},	{104, 56},	{112, 56},	{120, 56},	{128, 56},	{136, 56},	{144, 56},	{152, 56},	{160, 56},	{168, 56},	{176, 56},	{184, 56},	{192, 56},	{200, 56},	{208, 56},	{216, 56},	{224, 56},	{232, 56},	{240, 56}},
    //     {{32, 64},	{40, 64},	{48, 64},	{56, 64},	{64, 64},	{72, 64},	{80, 64},	{88, 64},	{96, 64},	{104, 64},	{112, 64},	{120, 64},	{128, 64},	{136, 64},	{144, 64},	{152, 64},	{160, 64},	{168, 64},	{176, 64},	{184, 64},	{192, 64},	{200, 64},	{208, 64},	{216, 64},	{224, 64},	{232, 64},	{240, 64}},
    //     {{32, 72},	{40, 72},	{48, 72},	{56, 72},	{64, 72},	{72, 72},	{80, 72},	{88, 72},	{96, 72},	{104, 72},	{112, 72},	{120, 72},	{128, 72},	{136, 72},	{144, 72},	{152, 72},	{160, 72},	{168, 72},	{176, 72},	{184, 72},	{192, 72},	{200, 72},	{208, 72},	{216, 72},	{224, 72},	{232, 72},	{240, 72}},
    //     {{32, 80},	{40, 80},	{48, 80},	{56, 80},	{64, 80},	{72, 80},	{80, 80},	{88, 80},	{96, 80},	{104, 80},	{112, 80},	{120, 80},	{128, 80},	{136, 80},	{144, 80},	{152, 80},	{160, 80},	{168, 80},	{176, 80},	{184, 80},	{192, 80},	{200, 80},	{208, 80},	{216, 80},	{224, 80},	{232, 80},	{240, 80}},
    //     {{32, 88},	{40, 88},	{48, 88},	{56, 88},	{64, 88},	{72, 88},	{80, 88},	{88, 88},	{96, 88},	{104, 88},	{112, 88},	{120, 88},	{128, 88},	{136, 88},	{144, 88},	{152, 88},	{160, 88},	{168, 88},	{176, 88},	{184, 88},	{192, 88},	{200, 88},	{208, 88},	{216, 88},	{224, 88},	{232, 88},	{240, 88}},
    //     {{32, 96},	{40, 96},	{48, 96},	{56, 96},	{64, 96},	{72, 96},	{80, 96},	{88, 96},	{96, 96},	{104, 96},	{112, 96},	{120, 96},	{128, 96},	{136, 96},	{144, 96},	{152, 96},	{160, 96},	{168, 96},	{176, 96},	{184, 96},	{192, 96},	{200, 96},	{208, 96},	{216, 96},	{224, 96},	{232, 96},	{240, 96}},
    //     {{32, 104},	{40, 104},	{48, 104},	{56, 104},	{64, 104},	{72, 104},	{80, 104},	{88, 104},	{96, 104},	{104, 104},	{112, 104},	{120, 104},	{128, 104},	{136, 104},	{144, 104},	{152, 104},	{160, 104},	{168, 104},	{176, 104},	{184, 104},	{192, 104},	{200, 104},	{208, 104},	{216, 104},	{224, 104},	{232, 104},	{240, 104}},
    //     {{32, 112},	{40, 112},	{48, 112},	{56, 112},	{64, 112},	{72, 112},	{80, 112},	{88, 112},	{96, 112},	{104, 112},	{112, 112},	{120, 112},	{128, 112},	{136, 112},	{144, 112},	{152, 112},	{160, 112},	{168, 112},	{176, 112},	{184, 112},	{192, 112},	{200, 112},	{208, 112},	{216, 112},	{224, 112},	{232, 112},	{240, 112}},
    //     {{32, 120},	{40, 120},	{48, 120},	{56, 120},	{64, 120},	{72, 120},	{80, 120},	{88, 120},	{96, 120},	{104, 120},	{112, 120},	{120, 120},	{128, 120},	{136, 120},	{144, 120},	{152, 120},	{160, 120},	{168, 120},	{176, 120},	{184, 120},	{192, 120},	{200, 120},	{208, 120},	{216, 120},	{224, 120},	{232, 120},	{240, 120}},
    //     {{32, 128},	{40, 128},	{48, 128},	{56, 128},	{64, 128},	{72, 128},	{80, 128},	{88, 128},	{96, 128},	{104, 128},	{112, 128},	{120, 128},	{128, 128},	{136, 128},	{144, 128},	{152, 128},	{160, 128},	{168, 128},	{176, 128},	{184, 128},	{192, 128},	{200, 128},	{208, 128},	{216, 128},	{224, 128},	{232, 128},	{240, 128}},
    //     {{32, 136},	{40, 136},	{48, 136},	{56, 136},	{64, 136},	{72, 136},	{80, 136},	{88, 136},	{96, 136},	{104, 136},	{112, 136},	{120, 136},	{128, 136},	{136, 136},	{144, 136},	{152, 136},	{160, 136},	{168, 136},	{176, 136},	{184, 136},	{192, 136},	{200, 136},	{208, 136},	{216, 136},	{224, 136},	{232, 136},	{240, 136}},
    // };

    sprite->x = 32 + sNessiePuzzleState->dagger_x * 8;
    sprite->y = 32 + sNessiePuzzleState->dagger_y * 8;
}

static void Task_NessiePuzzleMainInput(u8 taskId)
{
    if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_PC_OFF);
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
        gTasks[taskId].func = Task_NessiePuzzleWaitFadeAndExitGracefully;
    }
    if (JOY_NEW(A_BUTTON))
    {
        if
        (
            ((sNessiePuzzleState->dagger_x == 20) && (sNessiePuzzleState->dagger_y == 10)) ||
            ((sNessiePuzzleState->dagger_x == 21) && (sNessiePuzzleState->dagger_y == 10)) ||
            ((sNessiePuzzleState->dagger_x == 20) && (sNessiePuzzleState->dagger_y == 11)) ||
            ((sNessiePuzzleState->dagger_x == 21) && (sNessiePuzzleState->dagger_y == 11))
        )
        {
            FlagSet(FLAG_NESSIE_PUZZLE_SOLVED);
            PlaySE(SE_SELECT);
            BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
            gTasks[taskId].func = Task_NessiePuzzleWaitFadeAndExitGracefully;
        }
        
        else
        {
            PlaySE(SE_WALL_HIT);
        }
    }
    if (JOY_NEW(DPAD_LEFT)|| JOY_HELD(DPAD_LEFT))
    {
        if (sNessiePuzzleState->dagger_x == 0)
            sNessiePuzzleState->dagger_x = 26;
        else
            sNessiePuzzleState->dagger_x--;
    }
    if (JOY_NEW(DPAD_RIGHT)|| JOY_HELD(DPAD_RIGHT))
    {
        if (sNessiePuzzleState->dagger_x == 26)
            sNessiePuzzleState->dagger_x = 0;
        else
            sNessiePuzzleState->dagger_x++;
    }
    if (JOY_NEW(DPAD_UP)|| JOY_HELD(DPAD_UP))
    {
        if (sNessiePuzzleState->dagger_y == 0)
            sNessiePuzzleState->dagger_y = 13;
        else
            sNessiePuzzleState->dagger_y--;
    }
    if (JOY_NEW(DPAD_DOWN)|| JOY_HELD(DPAD_DOWN))
    {
        if (sNessiePuzzleState->dagger_y == 13)
            sNessiePuzzleState->dagger_y = 0;
        else
            sNessiePuzzleState->dagger_y++;
    }
}

static void Task_NessiePuzzleWaitFadeAndBail(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        SetMainCallback2(sNessiePuzzleState->savedCallback);
        NessiePuzzle_FreeResources();
        DestroyTask(taskId);
    }
}

static void Task_NessiePuzzleWaitFadeAndExitGracefully(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        SetMainCallback2(sNessiePuzzleState->savedCallback);
        NessiePuzzle_FreeResources();
        DestroyTask(taskId);
    }
}
#define TILEMAP_BUFFER_SIZE (1024 * 2)
static bool8 NessiePuzzle_InitBgs(void)
{
    ResetAllBgsCoordinates();

    sBg1TilemapBuffer = AllocZeroed(TILEMAP_BUFFER_SIZE);
    if (sBg1TilemapBuffer == NULL)
    {
        return FALSE;
    }

    ResetBgsAndClearDma3BusyFlags(0);
    InitBgsFromTemplates(0, sNessiePuzzleBgTemplates, NELEMS(sNessiePuzzleBgTemplates));

    SetBgTilemapBuffer(1, sBg1TilemapBuffer);
    ScheduleBgCopyTilemapToVram(1);

    ShowBg(0);
    ShowBg(1);

    return TRUE;
}
#undef TILEMAP_BUFFER_SIZE

static void NessiePuzzle_FadeAndBail(void)
{
    BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
    CreateTask(Task_NessiePuzzleWaitFadeAndBail, 0);
    SetVBlankCallback(NessiePuzzle_VBlankCB);
    SetMainCallback2(NessiePuzzle_MainCB);
}

static bool8 NessiePuzzle_LoadGraphics(void)
{
    switch (sNessiePuzzleState->loadState)
    {
    case 0:
        ResetTempTileDataBuffers();
        DecompressAndCopyTileDataToVram(1, sNessiePaintings, 0, 0, 0);
        sNessiePuzzleState->loadState++;
        break;
    case 1:
        if (FreeTempTileDataBuffersIfPossible() != TRUE)
        {
            LZDecompressWram(sNessiePaintingMap, sBg1TilemapBuffer);
            sNessiePuzzleState->loadState++;
        }
        break;
    case 2:
        LoadPalette(sNessiePaintingPalette, BG_PLTT_ID(0), PLTT_SIZE_4BPP);
        LoadPalette(gMessageBox_Pal, BG_PLTT_ID(15), PLTT_SIZE_4BPP);
        sNessiePuzzleState->loadState++;
    default:
        sNessiePuzzleState->loadState = 0;
        return TRUE;
    }
    return FALSE;
}

static void NessiePuzzle_InitWindows(void)
{
    InitWindows(sNessiePuzzleWindowTemplates);
    DeactivateAllTextPrinters();
    ScheduleBgCopyTilemapToVram(0);
    if (!(FlagGet(FLAG_NESSIE_READ_BOOK) && FlagGet(FLAG_NESSIE_GOT_DAGGER)))
    {
        FillWindowPixelBuffer(WINDOW_NO_STAB, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
        PutWindowTilemap(WINDOW_NO_STAB);
        CopyWindowToVram(WINDOW_NO_STAB, COPYWIN_FULL);
    }
    else
    {
        FillWindowPixelBuffer(WINDOW_STAB, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
        PutWindowTilemap(WINDOW_STAB);
        CopyWindowToVram(WINDOW_STAB, COPYWIN_FULL);
    }
}

static const u8 sText_Instructions1[] = _("{B_BUTTON} Exit");
static const u8 sText_Instructions2[] = _("{A_BUTTON} Stab {B_BUTTON} Exit");
static void NessiePuzzle_PrintUiSampleWindowText(void)
{
    /* prints with no stabby */
    if (!(FlagGet(FLAG_NESSIE_READ_BOOK) && FlagGet(FLAG_NESSIE_GOT_DAGGER)))
    {
        FillWindowPixelBuffer(WINDOW_NO_STAB, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
        AddTextPrinterParameterized4(WINDOW_NO_STAB, FONT_SMALL, 0, 0, 0, 0,
            sNessiePuzzleWindowFontColors[FONT_WHITE], TEXT_SKIP_DRAW, sText_Instructions1);
        CopyWindowToVram(WINDOW_NO_STAB, COPYWIN_GFX);
    }
    /* prints with stabby */
    else
    {
        FillWindowPixelBuffer(WINDOW_STAB, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
        AddTextPrinterParameterized4(WINDOW_STAB, FONT_SMALL, 0, 0, 0, 0,
            sNessiePuzzleWindowFontColors[FONT_WHITE], TEXT_SKIP_DRAW, sText_Instructions2);
        CopyWindowToVram(WINDOW_STAB, COPYWIN_GFX);
    }
}

static void NessiePuzzle_FreeResources(void)
{
    Destroydagger();
    if (sNessiePuzzleState != NULL)
    {
        Free(sNessiePuzzleState);
    }
    if (sBg1TilemapBuffer != NULL)
    {
        Free(sBg1TilemapBuffer);
    }
    FreeAllWindowBuffers();
    ResetSpriteData();
}

static u8 CreateDagger()
{
    if (sNessiePuzzleState->daggerSpriteId == 0xFF)
        sNessiePuzzleState->daggerSpriteId = CreateSprite(&sSpriteTemplate_Dagger, 0, 0, 0);

    gSprites[sNessiePuzzleState->daggerSpriteId].invisible = FALSE;
    return sNessiePuzzleState->daggerSpriteId;
}

static void Destroydagger()
{
    if (sNessiePuzzleState->daggerSpriteId != 0xFF)
        DestroySprite(&gSprites[sNessiePuzzleState->daggerSpriteId]);
    sNessiePuzzleState->daggerSpriteId = 0xFF;
}