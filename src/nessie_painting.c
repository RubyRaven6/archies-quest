#include "sample_ui.h"

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

static void SelectorCallback(struct Sprite *sprite);
static u8 CreateSelector();
static void DestroySelector();
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
    WINDOW_STAB_AREA  // Window for cursor
};

struct SpriteCordsStruct {
    u8 x;
    u8 y;
};

static EWRAM_DATA struct NessiePuzzleState *sNessiePuzzleState = NULL;
static EWRAM_DATA u8 *sBg1TilemapBuffer = NULL;

#define TAG_SELECTOR 30004

static const u16 sDaggerCursor_Pal[] = INCBIN_U16("graphics/sample_ui/dagger.gbapal");
static const u32 sDaggerCursor_Gfx[] = INCBIN_U16("graphics/sample_ui/dagger.4bpp.lz");

static const struct OamData sOamData_Dagger =
{
    .size = SPRITE_SIZE(32x32),
    .shape = SPRITE_SHAPE(32x32),
    .priority = 0,
};

static const struct CompressedSpriteSheet sSpriteSheet_Dagger =
{
    .data = sDaggerCursor_Gfx,
    .size = 32*32*4/2,
    .tag = TAG_SELECTOR,
};

static const struct SpritePalette sSpritePal_Dagger =
{
    .data = sDaggerCursor_Pal,
    .tag = TAG_SELECTOR
};

static const struct SpriteTemplate sSpriteTemplate_Dagger =
{
    .tileTag = TAG_SELECTOR,
    .paletteTag = TAG_SELECTOR,
    .oam = &sOamData_Dagger,
    .images = NULL,
    .callback = SelectorCallback
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
        .bg = 0,
        .tilemapLeft = 2,
        .tilemapTop = 2,
        .width = 26,
        .height = 14,
        .paletteNum = 15,
        .baseBlock = 17
    },
    DUMMY_WIN_TEMPLATE
};

static const u32 sNessiePaintings[] = INCBIN_U32("graphics/sample_ui/nessie_painting_map.4bpp.lz");

static const u32 sNessiePaintingMap[] = INCBIN_U32("graphics/sample_ui/nessie_painting_map.bin.lz");

static const u16 sNessiePaintingPalette[] = INCBIN_U16("graphics/sample_ui/nessie_painting.gbapal");

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

// Declared in sample_ui.h
void Task_OpenNessiePainting(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        CleanupOverworldWindowsAndTilemaps();
        NessiePuzzle_Init(CB2_ReturnToField);
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
        if(FlagGet(FLAG_NESSIE_FOUND_SOLUTION)){
            CreateSelector();
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
        PlaySE(SE_SELECT);
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
    if (!FlagGet(FLAG_NESSIE_FOUND_SOLUTION))
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
    if (!FlagGet(FLAG_NESSIE_FOUND_SOLUTION))
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

static u8 CreateSelector()
{
    if (sNessiePuzzleState->daggerSpriteId == 0xFF)
        sNessiePuzzleState->daggerSpriteId = CreateSprite(&sSpriteTemplate_Dagger, 183, 30, 0);

    gSprites[sNessiePuzzleState->daggerSpriteId].invisible = FALSE;
    return sNessiePuzzleState->daggerSpriteId;
}

static void DestroySelector()
{
    if (sNessiePuzzleState->daggerSpriteId != 0xFF)
        DestroySprite(&gSprites[sNessiePuzzleState->daggerSpriteId]);
    sNessiePuzzleState->daggerSpriteId = 0xFF;
}

static void SelectorCallback(struct Sprite *sprite)
{
    struct SpriteCordsStruct spriteCords[6][2] = {
        {{183, 30 + 20},  {215, 30 + 20}},
        {{183, 46 + 20},  {215, 46 + 20}},
        {{183, 62 + 20},  {215, 62 + 20}},
        {{183, 78 + 20},  {215, 78 + 20}},
        {{183, 94 + 20},  {215, 94 + 20}},
        {{183, 110 + 20}, {215, 110 + 20}}, // Thanks Jaizu
    };
  
    if(sprite->data[0] == 32)
    {
        sprite->invisible = TRUE;
    }
    if(sprite->data[0] >= 48)
    {
        sprite->invisible = FALSE;
        sprite->data[0] = 0;
    }
    sprite->data[0]++;

    sprite->x = spriteCords[sNessiePuzzleState->dagger_y][sNessiePuzzleState->dagger_x].x;
    sprite->y = spriteCords[sNessiePuzzleState->dagger_y][sNessiePuzzleState->dagger_x].y;
}