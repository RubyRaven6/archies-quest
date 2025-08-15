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
#include "string_util.h"
#include "pokemon_icon.h"
#include "puzzles.h"
#include "graphics.h"
#include "data.h"
#include "pokedex.h"
#include "gpu_regs.h"

//Sprite Callbacks
static void CursorCallback(struct Sprite *sprite);
static void NumberCallback(struct Sprite *);
static u32 CreateNumberSpriteAt(u32 x, u32 y, u32 number, u16 *numberSpriteId);
static void DestroyNumberSpriteAt(u32 x, u32 y, u16 *numberSpriteId);

struct GigageshoolPuzzleState
{
    MainCallback savedCallback;
    u8 loadState;
    u8 eightPearlShell;
    u8 fivePearlShell;
    u8 threePearlShell;
    u8 cursorX;
    u8 cursorY;
    u16 cursorSpriteId;
    u8 inputMode;
    u8 selectedShell;
    u16 eightShellSpriteId;
    u16 fiveShellSpriteId;
    u16 threeShellSpriteId;
};

enum WindowIds
{
    WINDOW_EIGHT,
    WINDOW_FIVE,
    WINDOW_THREE
};

enum ShellStates
{
    INPUT_SELECT_SHELL,
    INPUT_TAKE_SHELL_AMOUNT,
    INPUT_POUR_INTO_SHELL,
    INPUT_SELECTED_EIGHT_SHELL,
    INPUT_SELECTED_FIVE_SHELL,
    INPUT_SELECTED_THREE_SHELL
};

enum NumberSprites
{
    SPRITE_NUM_ZERO,
    SPRITE_NUM_ONE,
    SPRITE_NUM_TWO,
    SPRITE_NUM_THREE,
    SPRITE_NUM_FOUR,
    SPRITE_NUM_FIVE,
    SPRITE_NUM_SIX,
    SPRITE_NUM_SEVEN,
    SPRITE_NUM_EIGHT,
};

static EWRAM_DATA struct GigageshoolPuzzleState *sGigageshoolPuzzleState = NULL;
static EWRAM_DATA u8 *sBg1TilemapBuffer = NULL;

#define TAG_CURSOR          30004
#define TAG_NUMBERS         30005
/*
#define TAG_ZERO_SPRITE     30005
#define TAG_ONE_SPRITE      30006
#define TAG_TWO_SPRITE      30007
#define TAG_THREE_SPRITE    30008
#define TAG_FOUR_SPRITE     30009
#define TAG_FIVE_SPRITE     30010
#define TAG_SIX_SPRITE      30011
#define TAG_SEVEN_SPRITE    30012
#define TAG_EIGHT_SPRITE    30013
*/
#define MAX_EIGHT_SHELL     8
#define MAX_FIVE_SHELL      5
#define MAX_THREE_SHELL     3

static const u16 sCursor_Pal[] = INCBIN_U16("graphics/gigageshool_puzzle/cursor.gbapal");
static const u32 sCursor_Gfx[] = INCBIN_U32("graphics/gigageshool_puzzle/cursor.4bpp.lz");

static const u16 sNumbers_Pal[] = INCBIN_U16("graphics/gigageshool_puzzle/num_spritesheet.gbapal");
static const u32 sNumbers_Gfx[] = INCBIN_U32("graphics/gigageshool_puzzle/num_spritesheet.4bpp.lz");

/*
static const u16 sZeroSprite_Pal[] = INCBIN_U16("graphics/gigageshool_puzzle/num_0.gbapal");
static const u32 sZeroSprite_Gfx[] = INCBIN_U32("graphics/gigageshool_puzzle/num_0.4bpp.lz");

static const u16 sOneSprite_Pal[] = INCBIN_U16("graphics/gigageshool_puzzle/num_1.gbapal");
static const u32 sOneSprite_Gfx[] = INCBIN_U32("graphics/gigageshool_puzzle/num_1.4bpp.lz");

static const u16 sTwoSprite_Pal[] = INCBIN_U16("graphics/gigageshool_puzzle/num_2.gbapal");
static const u32 sTwoSprite_Gfx[] = INCBIN_U32("graphics/gigageshool_puzzle/num_2.4bpp.lz");

static const u16 sThreeSprite_Pal[] = INCBIN_U16("graphics/gigageshool_puzzle/num_3.gbapal");
static const u32 sThreeSprite_Gfx[] = INCBIN_U32("graphics/gigageshool_puzzle/num_3.4bpp.lz");

static const u16 sFourSprite_Pal[] = INCBIN_U16("graphics/gigageshool_puzzle/num_4.gbapal");
static const u32 sFourSprite_Gfx[] = INCBIN_U32("graphics/gigageshool_puzzle/num_4.4bpp.lz");

static const u16 sFiveSprite_Pal[] = INCBIN_U16("graphics/gigageshool_puzzle/num_5.gbapal");
static const u32 sFiveSprite_Gfx[] = INCBIN_U32("graphics/gigageshool_puzzle/num_5.4bpp.lz");

static const u16 sSixSprite_Pal[] = INCBIN_U16("graphics/gigageshool_puzzle/num_6.gbapal");
static const u32 sSixSprite_Gfx[] = INCBIN_U32("graphics/gigageshool_puzzle/num_6.4bpp.lz");

static const u16 sSevenSprite_Pal[] = INCBIN_U16("graphics/gigageshool_puzzle/num_7.gbapal");
static const u32 sSevenSprite_Gfx[] = INCBIN_U32("graphics/gigageshool_puzzle/num_7.4bpp.lz");

static const u16 sEightSprite_Pal[] = INCBIN_U16("graphics/gigageshool_puzzle/num_8.gbapal");
static const u32 sEightSprite_Gfx[] = INCBIN_U32("graphics/gigageshool_puzzle/num_8.4bpp.lz");

static const struct OamData sOamData_ZeroSprite =
{
    .size = SPRITE_SIZE(32x32),
    .shape = SPRITE_SHAPE(32x32),
    .priority = 1,
};

static const struct CompressedSpriteSheet sSpriteSheet_ZeroSprite =
{
    .data = sZeroSprite_Gfx,
    .size = 32*32/2,
    .tag = TAG_ZERO_SPRITE,
};

static const struct SpritePalette sSpritePal_ZeroSprite =
{
    .data = sZeroSprite_Pal,
    .tag = TAG_ZERO_SPRITE
};

static const struct SpriteTemplate sSpriteTemplate_ZeroSprite =
{
    .tileTag = TAG_ZERO_SPRITE,
    .paletteTag = TAG_ZERO_SPRITE,
    .oam = &sOamData_ZeroSprite,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .callback = NumberCallback
};

static const struct OamData sOamData_OneSprite =
{
    .size = SPRITE_SIZE(32x32),
    .shape = SPRITE_SHAPE(32x32),
    .priority = 1,
};

static const struct CompressedSpriteSheet sSpriteSheet_OneSprite =
{
    .data = sOneSprite_Gfx,
    .size = 32*32/2,
    .tag = TAG_ONE_SPRITE,
};

static const struct SpritePalette sSpritePal_OneSprite =
{
    .data = sOneSprite_Pal,
    .tag = TAG_ONE_SPRITE
};

static const struct SpriteTemplate sSpriteTemplate_OneSprite =
{
    .tileTag = TAG_ONE_SPRITE,
    .paletteTag = TAG_ONE_SPRITE,
    .oam = &sOamData_OneSprite,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .callback = NumberCallback
};

static const struct OamData sOamData_TwoSprite =
{
    .size = SPRITE_SIZE(32x32),
    .shape = SPRITE_SHAPE(32x32),
    .priority = 1,
};

static const struct CompressedSpriteSheet sSpriteSheet_TwoSprite =
{
    .data = sTwoSprite_Gfx,
    .size = 32*32/2,
    .tag = TAG_TWO_SPRITE,
};

static const struct SpritePalette sSpritePal_TwoSprite =
{
    .data = sTwoSprite_Pal,
    .tag = TAG_TWO_SPRITE
};

static const struct SpriteTemplate sSpriteTemplate_TwoSprite =
{
    .tileTag = TAG_TWO_SPRITE,
    .paletteTag = TAG_TWO_SPRITE,
    .oam = &sOamData_TwoSprite,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .callback = NumberCallback
};

static const struct OamData sOamData_ThreeSprite =
{
    .size = SPRITE_SIZE(32x32),
    .shape = SPRITE_SHAPE(32x32),
    .priority = 1,
};

static const struct CompressedSpriteSheet sSpriteSheet_ThreeSprite =
{
    .data = sThreeSprite_Gfx,
    .size = 32*32/2,
    .tag = TAG_THREE_SPRITE,
};

static const struct SpritePalette sSpritePal_ThreeSprite =
{
    .data = sThreeSprite_Pal,
    .tag = TAG_THREE_SPRITE
};

static const struct SpriteTemplate sSpriteTemplate_ThreeSprite =
{
    .tileTag = TAG_THREE_SPRITE,
    .paletteTag = TAG_THREE_SPRITE,
    .oam = &sOamData_ThreeSprite,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .callback = NumberCallback
};

static const struct OamData sOamData_FourSprite =
{
    .size = SPRITE_SIZE(32x32),
    .shape = SPRITE_SHAPE(32x32),
    .priority = 1,
};

static const struct CompressedSpriteSheet sSpriteSheet_FourSprite =
{
    .data = sFourSprite_Gfx,
    .size = 32*32/2,
    .tag = TAG_FOUR_SPRITE,
};

static const struct SpritePalette sSpritePal_FourSprite =
{
    .data = sFourSprite_Pal,
    .tag = TAG_FOUR_SPRITE
};

static const struct SpriteTemplate sSpriteTemplate_FourSprite =
{
    .tileTag = TAG_FOUR_SPRITE,
    .paletteTag = TAG_FOUR_SPRITE,
    .oam = &sOamData_FourSprite,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .callback = NumberCallback
};

static const struct OamData sOamData_FiveSprite =
{
    .size = SPRITE_SIZE(32x32),
    .shape = SPRITE_SHAPE(32x32),
    .priority = 1,
};

static const struct CompressedSpriteSheet sSpriteSheet_FiveSprite =
{
    .data = sFiveSprite_Gfx,
    .size = 32*32/2,
    .tag = TAG_FIVE_SPRITE,
};

static const struct SpritePalette sSpritePal_FiveSprite =
{
    .data = sFiveSprite_Pal,
    .tag = TAG_FIVE_SPRITE
};

static const struct SpriteTemplate sSpriteTemplate_FiveSprite =
{
    .tileTag = TAG_FIVE_SPRITE,
    .paletteTag = TAG_FIVE_SPRITE,
    .oam = &sOamData_FiveSprite,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .callback = NumberCallback
};

static const struct OamData sOamData_SixSprite =
{
    .size = SPRITE_SIZE(32x32),
    .shape = SPRITE_SHAPE(32x32),
    .priority = 1,
};

static const struct CompressedSpriteSheet sSpriteSheet_SixSprite =
{
    .data = sSixSprite_Gfx,
    .size = 32*32/2,
    .tag = TAG_SIX_SPRITE,
};

static const struct SpritePalette sSpritePal_SixSprite =
{
    .data = sSixSprite_Pal,
    .tag = TAG_SIX_SPRITE
};

static const struct SpriteTemplate sSpriteTemplate_SixSprite =
{
    .tileTag = TAG_SIX_SPRITE,
    .paletteTag = TAG_SIX_SPRITE,
    .oam = &sOamData_SixSprite,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .callback = NumberCallback
};

static const struct OamData sOamData_SevenSprite =
{
    .size = SPRITE_SIZE(32x32),
    .shape = SPRITE_SHAPE(32x32),
    .priority = 1,
};

static const struct CompressedSpriteSheet sSpriteSheet_SevenSprite =
{
    .data = sSevenSprite_Gfx,
    .size = 32*32/2,
    .tag = TAG_SEVEN_SPRITE,
};

static const struct SpritePalette sSpritePal_SevenSprite =
{
    .data = sSevenSprite_Pal,
    .tag = TAG_SEVEN_SPRITE
};

static const struct SpriteTemplate sSpriteTemplate_SevenSprite =
{
    .tileTag = TAG_SEVEN_SPRITE,
    .paletteTag = TAG_SEVEN_SPRITE,
    .oam = &sOamData_SevenSprite,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .callback = NumberCallback
};

static const struct OamData sOamData_EightSprite =
{
    .size = SPRITE_SIZE(32x32),
    .shape = SPRITE_SHAPE(32x32),
    .priority = 1,
};

static const struct CompressedSpriteSheet sSpriteSheet_EightSprite =
{
    .data = sEightSprite_Gfx,
    .size = 32*32/2,
    .tag = TAG_EIGHT_SPRITE,
};

static const struct SpritePalette sSpritePal_EightSprite =
{
    .data = sEightSprite_Pal,
    .tag = TAG_EIGHT_SPRITE
};

static const struct SpriteTemplate sSpriteTemplate_EightSprite =
{
    .tileTag = TAG_EIGHT_SPRITE,
    .paletteTag = TAG_EIGHT_SPRITE,
    .oam = &sOamData_EightSprite,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .callback = NumberCallback
};
*/
static const struct OamData sOamData_Cursor =
{
    .size = SPRITE_SIZE(16x32),
    .shape = SPRITE_SHAPE(16x32),
    .priority = 0,
};

static const struct CompressedSpriteSheet sSpriteSheet_Cursor =
{
    .data = sCursor_Gfx,
    .size = 16*32/2,
    .tag = TAG_CURSOR,
};

static const struct SpritePalette sSpritePal_Cursor =
{
    .data = sCursor_Pal,
    .tag = TAG_CURSOR
};

static const struct SpriteTemplate sSpriteTemplate_Cursor =
{
    .tileTag = TAG_CURSOR,
    .paletteTag = TAG_CURSOR,
    .oam = &sOamData_Cursor,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .callback = CursorCallback
};

static const struct OamData sOamDataNumbers =
{
    .size = SPRITE_SIZE(32x32),
    .shape = SPRITE_SHAPE(32x32),
    .priority = 0,
};

static const struct CompressedSpriteSheet sSpriteSheet_Numbers =
{
    .data = sNumbers_Gfx,
    .size = 32*32*9/2,
    .tag = TAG_NUMBERS,
};

static const struct SpritePalette sSpritePal_Numbers =
{
    .data = sNumbers_Pal,
    .tag = TAG_NUMBERS
};

#define NUMBER_CONSTANT 16

static const union AnimCmd sSpriteAnimNumbers0[] =
{
    ANIMCMD_FRAME(NUMBER_CONSTANT * 0, 32),
    ANIMCMD_FRAME(NUMBER_CONSTANT * 0, 32),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sSpriteAnimNumbers1[] =
{
    ANIMCMD_FRAME(NUMBER_CONSTANT * 1, 32),
    ANIMCMD_FRAME(NUMBER_CONSTANT * 1, 32),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sSpriteAnimNumbers2[] =
{
    ANIMCMD_FRAME(NUMBER_CONSTANT * 2, 32),
    ANIMCMD_FRAME(NUMBER_CONSTANT * 2, 32),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sSpriteAnimNumbers3[] =
{
    ANIMCMD_FRAME(NUMBER_CONSTANT * 3, 32),
    ANIMCMD_FRAME(NUMBER_CONSTANT * 3, 32),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sSpriteAnimNumbers4[] =
{
    ANIMCMD_FRAME(NUMBER_CONSTANT * 4, 32),
    ANIMCMD_FRAME(NUMBER_CONSTANT * 4, 32),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sSpriteAnimNumbers5[] =
{
    ANIMCMD_FRAME(NUMBER_CONSTANT * 5, 32),
    ANIMCMD_FRAME(NUMBER_CONSTANT * 5, 32),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sSpriteAnimNumbers6[] =
{
    ANIMCMD_FRAME(NUMBER_CONSTANT * 6, 32),
    ANIMCMD_FRAME(NUMBER_CONSTANT * 6, 32),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sSpriteAnimNumbers7[] =
{
    ANIMCMD_FRAME(NUMBER_CONSTANT * 7, 32),
    ANIMCMD_FRAME(NUMBER_CONSTANT * 7, 32),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sSpriteAnimNumbers8[] =
{
    ANIMCMD_FRAME(NUMBER_CONSTANT * 8, 32),
    ANIMCMD_FRAME(NUMBER_CONSTANT * 8, 32),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd *const sSpriteAnimTableNumbers[] =
{
    sSpriteAnimNumbers0,
    sSpriteAnimNumbers1,
    sSpriteAnimNumbers2,
    sSpriteAnimNumbers3,
    sSpriteAnimNumbers4,
    sSpriteAnimNumbers5,
    sSpriteAnimNumbers6,
    sSpriteAnimNumbers7,
    sSpriteAnimNumbers8,
};

static const struct SpriteTemplate sSpriteTemplateNumbers =
{
    .tileTag = TAG_NUMBERS,
    .paletteTag = TAG_NUMBERS,
    .oam = &sOamDataNumbers,
    .anims = sSpriteAnimTableNumbers,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = NumberCallback
};

static const struct BgTemplate sGigageshoolPuzzleBgTemplates[] =
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

static const struct WindowTemplate sGigageshoolPuzzleWindowTemplates[] =
{
    [WINDOW_EIGHT] =
    {
        .bg = 0,
        .tilemapLeft = 5,
        .tilemapTop = 7,
        .width = 2,
        .height = 3,
        .paletteNum = 15,
        .baseBlock = 1
    },
    DUMMY_WIN_TEMPLATE
};

static const u32 sGigageshoolPuzzleTiles[] = INCBIN_U32("graphics/gigageshool_puzzle/puzzle_tiles.4bpp.lz");

static const u32 sGigageshoolPuzzleTilemap[] = INCBIN_U32("graphics/gigageshool_puzzle/puzzle_tiles.bin.lz");

static const u16 sGigageshoolPuzzlePalette[] = INCBIN_U16("graphics/gigageshool_puzzle/puzzle_palette.gbapal");

// Callbacks for the sample UI
static void GigageshoolPuzzle_SetupCB(void);
static void GigageshoolPuzzle_MainCB(void);
static void GigageshoolPuzzle_VBlankCB(void);

// Sample UI tasks
static void Task_GigageshoolPuzzleWaitFadeIn(u8 taskId);
static void Task_GigageshoolPuzzleMainInput(u8 taskId);
static void Task_GigageshoolPuzzleWaitFadeAndBail(u8 taskId);
static void Task_GigageshoolPuzzleWaitFadeAndExitGracefully(u8 taskId);

// Sample UI helper functions
static void GigageshoolPuzzle_Init(MainCallback callback);
static void GigageshoolPuzzle_ResetGpuRegsAndBgs(void);
static bool8 GigageshoolPuzzle_InitBgs(void);
static void GigageshoolPuzzle_FadeAndBail(void);
static bool8 GigageshoolPuzzle_LoadGraphics(void);
static void GigageshoolPuzzle_InitWindows(void);
static void GigageshoolPuzzle_FreeResources(void);

// More stuff
static u8 CreateCursor(void);
static void DestroyCursor(void);
static void GigageshoolPuzzle_HandleShellContents(void);
static void GigageshoolPuzzle_SelectShell(void);

// Declared in sample_ui.h
void Task_OpenGigageshoolPuzzle(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        CleanupOverworldWindowsAndTilemaps();
        GigageshoolPuzzle_Init(CB2_ReturnToFieldWithOpenMenu);
        DestroyTask(taskId);
    }
}

static void GigageshoolPuzzle_Init(MainCallback callback)
{
    sGigageshoolPuzzleState = AllocZeroed(sizeof(struct GigageshoolPuzzleState));
    if (sGigageshoolPuzzleState == NULL)
    {
        SetMainCallback2(callback);
        return;
    }

    sGigageshoolPuzzleState->loadState = 0;
    sGigageshoolPuzzleState->cursorSpriteId = 0xFF;
    sGigageshoolPuzzleState->eightShellSpriteId = 0xFF;
    sGigageshoolPuzzleState->fiveShellSpriteId = 0xFF;
    sGigageshoolPuzzleState->threeShellSpriteId = 0xFF;
    sGigageshoolPuzzleState->savedCallback = callback;
    sGigageshoolPuzzleState->eightPearlShell = 8;
    sGigageshoolPuzzleState->fivePearlShell = 0;
    sGigageshoolPuzzleState->threePearlShell = 0;
    sGigageshoolPuzzleState->inputMode = INPUT_SELECT_SHELL;

    SetMainCallback2(GigageshoolPuzzle_SetupCB);
}

// Credit: Jaizu, pret
static void GigageshoolPuzzle_ResetGpuRegsAndBgs(void)
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

static void GigageshoolPuzzle_SetupCB(void)
{
    switch (gMain.state)
    {
    case 0:
        GigageshoolPuzzle_ResetGpuRegsAndBgs();
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
        if (GigageshoolPuzzle_InitBgs())
        {
            sGigageshoolPuzzleState->loadState = 0;
            gMain.state++;
        }
        else
        {
            GigageshoolPuzzle_FadeAndBail();
            return;
        }
        break;
    case 3:
        if (GigageshoolPuzzle_LoadGraphics() == TRUE)
        {
            gMain.state++;
        }
        break;
    case 4:
        LoadCompressedSpriteSheet(&sSpriteSheet_Cursor);
        LoadSpritePalette(&sSpritePal_Cursor);
        LoadCompressedSpriteSheet(&sSpriteSheet_Numbers);
        LoadSpritePalette(&sSpritePal_Numbers);
        gMain.state++;
        break;
    case 5:
        GigageshoolPuzzle_InitWindows();
        CreateCursor();
        CreateNumberSpriteAt(32, 56, sGigageshoolPuzzleState->eightPearlShell, &sGigageshoolPuzzleState->eightShellSpriteId); // Eight Shell
        CreateNumberSpriteAt(176, 16, sGigageshoolPuzzleState->fivePearlShell, &sGigageshoolPuzzleState->fiveShellSpriteId); // Five Shell
        CreateNumberSpriteAt(176, 96, sGigageshoolPuzzleState->threePearlShell, &sGigageshoolPuzzleState->threeShellSpriteId); // Three Shell
        gMain.state++;
        break;
    case 6:
        CreateTask(Task_GigageshoolPuzzleWaitFadeIn, 0);
        gMain.state++;
        break;
    case 7:
        BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
        gMain.state++;
        break;
    default:
        SetVBlankCallback(GigageshoolPuzzle_VBlankCB);
        SetMainCallback2(GigageshoolPuzzle_MainCB);
        break;
    }
}

static void GigageshoolPuzzle_MainCB(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    DoScheduledBgTilemapCopiesToVram();
    UpdatePaletteFade();
}

static void GigageshoolPuzzle_VBlankCB(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static void Task_GigageshoolPuzzleWaitFadeIn(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        gTasks[taskId].func = Task_GigageshoolPuzzleMainInput;
    }
}

struct SpriteCoordsStruct {
    u8 x;
    u8 y;
};

static void CursorCallback(struct Sprite *sprite)
{
    struct SpriteCoordsStruct spriteCoords[3][1] = { //Thanks Jaizu
        {{88, 64}}, //INPUT_SELECTED_EIGHT_SHELL
        {{224, 17}}, //INPUT_SELECTED_FIVE_SHELL
        {{224, 97}}, //INPUT_SELECTED_THREE_SHELL
    };

    u32 cursorY = sGigageshoolPuzzleState->cursorY;
    u32 cursorX = sGigageshoolPuzzleState->cursorX;
    sprite->x   = spriteCoords[cursorY][cursorX].x + 8;
    sprite->y   = spriteCoords[cursorY][cursorX].y + 15;
}

static void NumberCallback(struct Sprite *)
{
  // nothing in here
}

static u32 CreateNumberSpriteAt(u32 x, u32 y, u32 number, u16 *numberSpriteId)
{    
    if (*numberSpriteId == 0xFF)
        *numberSpriteId = CreateSprite(&sSpriteTemplateNumbers, x, y, 0);

    gSprites[*numberSpriteId].x = x + 16;
    gSprites[*numberSpriteId].y = y + 16;
    StartSpriteAnim(&gSprites[*numberSpriteId], number);
    return *numberSpriteId;
}

static void DestroyNumberSpriteAt(u32 x, u32 y, u16 *numberSpriteId)
{
    if (*numberSpriteId != 0xFF)
        DestroySprite(&gSprites[*numberSpriteId]);
    *numberSpriteId = 0xFF;
}

static u8 CreateCursor(void)
{
    if (sGigageshoolPuzzleState->cursorSpriteId == 0xFF)
        sGigageshoolPuzzleState->cursorSpriteId = CreateSprite(&sSpriteTemplate_Cursor, 0, 0, 0);

    gSprites[sGigageshoolPuzzleState->cursorSpriteId].invisible = FALSE;
    StartSpriteAnim(&gSprites[sGigageshoolPuzzleState->cursorSpriteId], 0);
    return sGigageshoolPuzzleState->cursorSpriteId;
}

static void DestroyCursor(void)
{
    if (sGigageshoolPuzzleState->cursorSpriteId != 0xFF)
        DestroySprite(&gSprites[sGigageshoolPuzzleState->cursorSpriteId]);
    sGigageshoolPuzzleState->cursorSpriteId = 0xFF;
}

static void Task_GigageshoolPuzzleMainInput(u8 taskId)
{
    u8 *cursorY = &sGigageshoolPuzzleState->cursorY;
    u8 *inputMode = &sGigageshoolPuzzleState->inputMode;

    if (JOY_NEW(B_BUTTON))
    {
        if(*inputMode == INPUT_POUR_INTO_SHELL){
            *inputMode = INPUT_SELECT_SHELL;
            PlaySE(SE_PC_OFF);
        }
        else
        {
            PlaySE(SE_M_DIVE);
            BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
            gTasks[taskId].func = Task_GigageshoolPuzzleWaitFadeAndExitGracefully;
        }
    }
    if (JOY_NEW(A_BUTTON))
    {
        if(*inputMode == INPUT_SELECT_SHELL)
            GigageshoolPuzzle_SelectShell();
        else if(*inputMode == INPUT_POUR_INTO_SHELL)
            GigageshoolPuzzle_HandleShellContents();
        
        CreateNumberSpriteAt(32, 56, sGigageshoolPuzzleState->eightPearlShell, &sGigageshoolPuzzleState->eightShellSpriteId); // Eight Shell
        CreateNumberSpriteAt(176, 16, sGigageshoolPuzzleState->fivePearlShell, &sGigageshoolPuzzleState->fiveShellSpriteId); // Five Shell
        CreateNumberSpriteAt(176, 96, sGigageshoolPuzzleState->threePearlShell, &sGigageshoolPuzzleState->threeShellSpriteId); // Three Shell
    }
    if (JOY_NEW(DPAD_RIGHT)){
        if(*cursorY == 2){
            sGigageshoolPuzzleState->cursorY = 0;
        } 
        else {
            sGigageshoolPuzzleState->cursorY++;
        } 
    }
    if (JOY_NEW(DPAD_LEFT)){
        if(*cursorY == 0){
            sGigageshoolPuzzleState->cursorY = 2;
        } 
        else {
            sGigageshoolPuzzleState->cursorY--;
        } 
    }
    if(JOY_NEW(SELECT_BUTTON)){
        u8 *inputMode = &sGigageshoolPuzzleState->inputMode;
        u8 *selectedShell = &sGigageshoolPuzzleState->selectedShell;
        u8 *eightPearlShell = &sGigageshoolPuzzleState->eightPearlShell;
        u8 *fivePearlShell = &sGigageshoolPuzzleState->fivePearlShell;
        u8 *threePearlShell = &sGigageshoolPuzzleState->threePearlShell;
        //u8 *cursorY = &sGigageshoolPuzzleState->cursorY;

        DebugPrintf("*inputMode: %u", *inputMode);
        DebugPrintf("*selectedShell: %u", *selectedShell);
        DebugPrintf("*eightPearlShell: %u", *eightPearlShell);
        DebugPrintf("*fivePearlShell: %u", *fivePearlShell);
        DebugPrintf("*threePearlShell: %u", *threePearlShell);
        //DebugPrintf("*cursorY: %u", *cursorY);
    }
}

static void Task_GigageshoolPuzzleWaitFadeAndBail(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        SetMainCallback2(sGigageshoolPuzzleState->savedCallback);
        GigageshoolPuzzle_FreeResources();
        DestroyTask(taskId);
    }
}

static void Task_GigageshoolPuzzleWaitFadeAndExitGracefully(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        SetMainCallback2(sGigageshoolPuzzleState->savedCallback);
        GigageshoolPuzzle_FreeResources();
        DestroyTask(taskId);
    }
}

#define TILEMAP_BUFFER_SIZE (1024 * 2)
static bool8 GigageshoolPuzzle_InitBgs(void)
{
    ResetAllBgsCoordinates();

    sBg1TilemapBuffer = AllocZeroed(TILEMAP_BUFFER_SIZE);
    if (sBg1TilemapBuffer == NULL)
    {
        return FALSE;
    }

    ResetBgsAndClearDma3BusyFlags(0);
    InitBgsFromTemplates(0, sGigageshoolPuzzleBgTemplates, NELEMS(sGigageshoolPuzzleBgTemplates));

    SetBgTilemapBuffer(1, sBg1TilemapBuffer);
    ScheduleBgCopyTilemapToVram(1);

    ShowBg(0);
    ShowBg(1);

    return TRUE;
}
#undef TILEMAP_BUFFER_SIZE

static void GigageshoolPuzzle_FadeAndBail(void)
{
    BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
    CreateTask(Task_GigageshoolPuzzleWaitFadeAndBail, 0);
    SetVBlankCallback(GigageshoolPuzzle_VBlankCB);
    SetMainCallback2(GigageshoolPuzzle_MainCB);
}

static bool8 GigageshoolPuzzle_LoadGraphics(void)
{
    switch (sGigageshoolPuzzleState->loadState)
    {
    case 0:
        ResetTempTileDataBuffers();
        DecompressAndCopyTileDataToVram(1, sGigageshoolPuzzleTiles, 0, 0, 0);
        sGigageshoolPuzzleState->loadState++;
        break;
    case 1:
        if (FreeTempTileDataBuffersIfPossible() != TRUE)
        {
            LZDecompressWram(sGigageshoolPuzzleTilemap, sBg1TilemapBuffer);
            sGigageshoolPuzzleState->loadState++;
        }
        break;
    case 2:
        LoadPalette(sGigageshoolPuzzlePalette, BG_PLTT_ID(0), PLTT_SIZE_4BPP);
        LoadPalette(gMessageBox_Pal, BG_PLTT_ID(15), PLTT_SIZE_4BPP);
        sGigageshoolPuzzleState->loadState++;
    default:
        sGigageshoolPuzzleState->loadState = 0;
        return TRUE;
    }
    return FALSE;
}

static void GigageshoolPuzzle_InitWindows(void)
{
    InitWindows(sGigageshoolPuzzleWindowTemplates);
    DeactivateAllTextPrinters();
    ScheduleBgCopyTilemapToVram(0);
    FillWindowPixelBuffer(WINDOW_EIGHT, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    PutWindowTilemap(WINDOW_EIGHT);
    CopyWindowToVram(WINDOW_EIGHT, 3);
}

static void GigageshoolPuzzle_FreeResources(void)
{
    DestroyCursor();
    if (sGigageshoolPuzzleState != NULL)
    {
        Free(sGigageshoolPuzzleState);
    }
    if (sBg1TilemapBuffer != NULL)
    {
        Free(sBg1TilemapBuffer);
    }
    FreeAllWindowBuffers();
    ResetSpriteData();
}

static void GigageshoolPuzzle_SelectShell(void)
{
    u8 *cursorY = &sGigageshoolPuzzleState->cursorY;
    u8 *selectedShell = &sGigageshoolPuzzleState->selectedShell;

    PlaySE(SE_SELECT);
    sGigageshoolPuzzleState->inputMode = INPUT_POUR_INTO_SHELL;
    switch(*cursorY){
        case 0:
            *selectedShell = INPUT_SELECTED_EIGHT_SHELL;
            break;
        case 1:
            *selectedShell = INPUT_SELECTED_FIVE_SHELL;
            break;
        case 2:
            *selectedShell = INPUT_SELECTED_THREE_SHELL;
            break;
    }
}

static void GigageshoolPuzzle_HandleShellContents(void)
{  
    u8 temp = 0;
    u8 *selectedShell = &sGigageshoolPuzzleState->selectedShell;
    u8 *eightShell = &sGigageshoolPuzzleState->eightPearlShell;
    u8 *fiveShell = &sGigageshoolPuzzleState->fivePearlShell;
    u8 *threeShell = &sGigageshoolPuzzleState->threePearlShell;
    u8 *cursorY = &sGigageshoolPuzzleState->cursorY;
    u8 *inputMode = &sGigageshoolPuzzleState->inputMode;

    PlaySE(SE_SELECT);
    
    switch(*selectedShell){
        case INPUT_SELECTED_EIGHT_SHELL:
            if(*cursorY == 1){ //For when Eight Shell to Five Shell
                if (*eightShell == 0 || *fiveShell == MAX_FIVE_SHELL){
                }
                else if (*eightShell > MAX_FIVE_SHELL && *fiveShell == 0){
                    *eightShell = *eightShell - MAX_FIVE_SHELL;
                    *fiveShell = MAX_FIVE_SHELL;
                }
                else if (*eightShell > MAX_FIVE_SHELL && *fiveShell > 0){
                    temp = MAX_FIVE_SHELL - *fiveShell;
                    *fiveShell += temp;
                    *eightShell -= temp;
                }
                else {
                    *fiveShell += *eightShell;
                    *eightShell = 0;
                }
                *inputMode = INPUT_SELECT_SHELL;
            }
            else if (*cursorY == 2){ //For when Eight Shell to Three Shell
                if (*eightShell == 0 || *threeShell == MAX_THREE_SHELL){
                }
                else if (*eightShell > MAX_THREE_SHELL && *threeShell == 0){
                    *eightShell = *eightShell - MAX_THREE_SHELL;
                    *threeShell = MAX_THREE_SHELL;
                }
                else if (*eightShell > MAX_THREE_SHELL && *threeShell > 0){
                    temp = MAX_THREE_SHELL - *threeShell;
                    *threeShell += temp;
                    *eightShell -= temp;
                }
                else {
                    *threeShell += *eightShell;
                    *eightShell = 0;
                }
                *inputMode = INPUT_SELECT_SHELL;
            }
            else { //For Eight Shell back to Eight
                *inputMode = INPUT_SELECT_SHELL;
            }
            break;
        case INPUT_SELECTED_FIVE_SHELL:
            if(*cursorY == 0){ //For when Five Shell to Eight Shell
                *eightShell += *fiveShell;
                *fiveShell = 0;
                *inputMode = INPUT_SELECT_SHELL;
            }
            else if (*cursorY == 2){ //For when Five Shell to Three Shell
                if (*fiveShell == 0 || *threeShell == MAX_THREE_SHELL){
                }
                else if (*fiveShell > MAX_THREE_SHELL && *threeShell == 0){
                    *fiveShell = *fiveShell - MAX_THREE_SHELL;
                    *threeShell = MAX_THREE_SHELL;
                }
                else if (*fiveShell > MAX_THREE_SHELL && *threeShell > 0){
                    temp = MAX_THREE_SHELL - *threeShell;
                    *threeShell += temp;
                    *fiveShell -= temp;
                }
                else {
                    *threeShell += *fiveShell;
                    *fiveShell = 0;
                }                
                *inputMode = INPUT_SELECT_SHELL;
            }
            else { //For Five Shell back to Five
                *inputMode = INPUT_SELECT_SHELL;
            }
            break;
        case INPUT_SELECTED_THREE_SHELL:
            if(*cursorY == 2){ //For Three Shell back to Three
                *inputMode = INPUT_SELECT_SHELL;
            }
            else if (*cursorY == 1)
            {
                if (*fiveShell == MAX_FIVE_SHELL){}
                else //*cursorY == 1
                {                    
                    *fiveShell += *threeShell;
                    *threeShell = 0;
                }
            }
            else 
            { //For Three Shell to Eight
                if(*cursorY == 0)
                {
                    *eightShell += *threeShell;
                    *threeShell = 0;
                }
                *inputMode = INPUT_SELECT_SHELL;
            }
            break;
    }
    
    DestroyNumberSpriteAt(32, 56, &sGigageshoolPuzzleState->eightShellSpriteId); // Destroys eight shell
    DestroyNumberSpriteAt(176, 16, &sGigageshoolPuzzleState->fiveShellSpriteId); // Destroys five shell
    DestroyNumberSpriteAt(176, 96, &sGigageshoolPuzzleState->threeShellSpriteId); // Destroys three shell
}