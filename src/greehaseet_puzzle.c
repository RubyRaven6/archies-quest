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
#include "event_data.h"
#include "math_util.h"

//Sprite Callbacks
static void CursorCallback(struct Sprite *sprite);
static void NumberCallback(struct Sprite *sprite);
static u32 CreateNumberSpriteAt(u32 x, u32 y, u32 number, u16 *numberSpriteId);
static void DestroyNumberSpriteAt(u32 x, u32 y, u16 *numberSpriteId);

struct GreehaseetPuzzleState
{
    MainCallback savedCallback;
    u8 loadState;
    u8 tenPearlShell;
    u8 sevenPearlShell;
    u8 threePearlShell;
    u8 cursorX;
    u8 cursorY;
    u16 cursorSpriteId;
    u16 selectedSpriteId;
    u8 inputMode;
    u8 selectedShell;
    u16 tenShellSpriteId;
    u16 sevenShellSpriteId;
    u16 threeShellSpriteId;
};

enum WindowIds
{
    WINDOW_INSTRUCTIONS,
    WINDOW_CONTROLS
};

enum ShellStates
{
    INPUT_SELECT_SHELL,
    INPUT_TAKE_SHELL_AMOUNT,
    INPUT_POUR_INTO_SHELL,
    INPUT_SELECTED_TEN_SHELL,
    INPUT_SELECTED_SEVEN_SHELL,
    INPUT_SELECTED_THREE_SHELL
};

enum FontColor
{
    FONT_WHITE,
    FONT_RED
};

static const u8 sGreehaseetPuzzleWindowFontColors[][3] =
{
    [FONT_WHITE]  = {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_WHITE,      TEXT_COLOR_DARK_GRAY},
};


static EWRAM_DATA struct GreehaseetPuzzleState *sGreehaseetPuzzleState = NULL;
static EWRAM_DATA u8 *sBg1TilemapBuffer = NULL;

#define TAG_CURSOR                  30004
#define TAG_NUMBERS                 30005
#define TAG_SELECTEDCURSOR          30006

#define MAX_TEN_SHELL       10
#define MAX_SEVEN_SHELL     7
#define MAX_THREE_SHELL     3

static const u16 sCursor_Pal[] = INCBIN_U16("graphics/greehaseet_puzzle/cursor.gbapal");
static const u32 sCursor_Gfx[] = INCBIN_U32("graphics/greehaseet_puzzle/cursor.4bpp.smol");

static const u16 sSelectedCursor_Pal[] = INCBIN_U16("graphics/greehaseet_puzzle/selected_arrow.gbapal");
static const u32 sSelectedCursor_Gfx[] = INCBIN_U32("graphics/greehaseet_puzzle/selected_arrow.4bpp.smol");

static const u16 sNumbers_Pal[] = INCBIN_U16("graphics/greehaseet_puzzle/num_spritesheet.gbapal");
static const u32 sNumbers_Gfx[] = INCBIN_U32("graphics/greehaseet_puzzle/num_spritesheet.4bpp.smol");

static const u32 sGreehaseetPuzzleTiles[] = INCBIN_U32("graphics/greehaseet_puzzle/puzzle_tiles.4bpp.smol");
static const u32 sGreehaseetPuzzleTilemap[] = INCBIN_U32("graphics/greehaseet_puzzle/puzzle_tiles.bin.smolTM");
static const u16 sGreehaseetPuzzlePalette[] = INCBIN_U16("graphics/greehaseet_puzzle/puzzle_tiles.gbapal");

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

static const struct OamData sOamData_SelectedCursor =
{
    .size = SPRITE_SIZE(16x32),
    .shape = SPRITE_SHAPE(16x32),
    .priority = 0,
};

static const struct CompressedSpriteSheet sSpriteSheet_SelectedCursor =
{
    .data = sSelectedCursor_Gfx,
    .size = 16*32/2,
    .tag = TAG_SELECTEDCURSOR,
};

static const struct SpritePalette sSpritePal_SelectedCursor =
{
    .data = sSelectedCursor_Pal,
    .tag = TAG_SELECTEDCURSOR
};

static const struct SpriteTemplate sSpriteTemplate_SelectedCursor =
{
    .tileTag = TAG_SELECTEDCURSOR,
    .paletteTag = TAG_SELECTEDCURSOR,
    .oam = &sOamData_SelectedCursor,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .callback = NumberCallback
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
    .size = 32*32*11/2,
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

static const union AnimCmd sSpriteAnimNumbers9[] =
{
    ANIMCMD_FRAME(NUMBER_CONSTANT * 9, 32),
    ANIMCMD_FRAME(NUMBER_CONSTANT * 9, 32),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sSpriteAnimNumbers10[] =
{
    ANIMCMD_FRAME(NUMBER_CONSTANT * 10, 32),
    ANIMCMD_FRAME(NUMBER_CONSTANT * 10, 32),
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
    sSpriteAnimNumbers9,
    sSpriteAnimNumbers10,
};

static const struct SpriteTemplate sSpriteTemplateNumbers =
{
    .tileTag = TAG_NUMBERS,
    .paletteTag = TAG_NUMBERS,
    .oam = &sOamDataNumbers,
    .anims = sSpriteAnimTableNumbers,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy
};

static const struct BgTemplate sGreehaseetPuzzleBgTemplates[] =
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
    [WINDOW_INSTRUCTIONS] =
    {
        .bg = 0,
        .tilemapLeft = 1,
        .tilemapTop = 1,
        .width = 12,
        .height = 4,
        .paletteNum = 15,
        .baseBlock = 1
    },
    [WINDOW_CONTROLS] =
    {
        .bg = 0,
        .tilemapLeft = 1,
        .tilemapTop = 16,
        .width = 13,
        .height = 4,
        .paletteNum = 15,
        .baseBlock = 49
    },
    DUMMY_WIN_TEMPLATE
};

// Callbacks for the sample UI
static void GreehaseetPuzzle_SetupCB(void);
static void GreehaseetPuzzle_MainCB(void);
static void GreehaseetPuzzle_VBlankCB(void);

// Sample UI tasks
static void Task_GreehaseetPuzzleWaitFadeIn(u8 taskId);
static void Task_GreehaseetPuzzleMainInput(u8 taskId);
static void Task_GreehaseetPuzzleWaitFadeAndBail(u8 taskId);
static void Task_GreehaseetPuzzleWaitFadeAndExitGracefully(u8 taskId);
static void Task_GreehaseetWaitForPuzzleFade(u8 taskId);

// Sample UI helper functions
static void GreehaseetPuzzle_Init(MainCallback callback);
static void GreehaseetPuzzle_ResetGpuRegsAndBgs(void);
static void GreehaseetPuzzle_InitWindows(void);
static bool8 GreehaseetPuzzle_InitBgs(void);
static void GreehaseetPuzzle_PrintWindowText(void);
static void GreehaseetPuzzle_FadeAndBail(void);
static bool8 GreehaseetPuzzle_LoadGraphics(void);
static void GreehaseetPuzzle_FreeResources(void);


// More stuff
static u8 CreateCursor(void);
static void DestroyCursor(void);
static void GreehaseetPuzzle_HandleShellContents(void);
static void TransferPearls(u8 * sourceShell, u8 * targetShell, u8 maxTarget);
static void GreehaseetPuzzle_SelectShell(void);

// Declared in sample_ui.h
void Task_OpenGreehaseetPuzzle(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        CleanupOverworldWindowsAndTilemaps();
        GreehaseetPuzzle_Init(CB2_ReturnToFieldContinueScript);
        DestroyTask(taskId);
    }
}

static void GreehaseetPuzzle_Init(MainCallback callback)
{
    sGreehaseetPuzzleState = AllocZeroed(sizeof(struct GreehaseetPuzzleState));
    if (sGreehaseetPuzzleState == NULL)
    {
        SetMainCallback2(callback);
        return;
    }

    sGreehaseetPuzzleState->loadState = 0;
    sGreehaseetPuzzleState->savedCallback = callback;
    sGreehaseetPuzzleState->cursorSpriteId = 0xFF;
    sGreehaseetPuzzleState->selectedSpriteId = 0xFF;
    sGreehaseetPuzzleState->tenShellSpriteId = 0xFF;
    sGreehaseetPuzzleState->sevenShellSpriteId = 0xFF;
    sGreehaseetPuzzleState->threeShellSpriteId = 0xFF;
    sGreehaseetPuzzleState->tenPearlShell = 10;
    sGreehaseetPuzzleState->sevenPearlShell = 0;
    sGreehaseetPuzzleState->threePearlShell = 0;
    sGreehaseetPuzzleState->inputMode = INPUT_SELECT_SHELL;

    SetMainCallback2(GreehaseetPuzzle_SetupCB);
}

// Credit: Jaizu, pret
static void GreehaseetPuzzle_ResetGpuRegsAndBgs(void)
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

static void GreehaseetPuzzle_SetupCB(void)
{
    switch (gMain.state)
    {
    case 0:
        GreehaseetPuzzle_ResetGpuRegsAndBgs();
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
        if (GreehaseetPuzzle_InitBgs())
        {
            sGreehaseetPuzzleState->loadState = 0;
            gMain.state++;
        }
        else
        {
            GreehaseetPuzzle_FadeAndBail();
            return;
        }
        break;
    case 3:
        if (GreehaseetPuzzle_LoadGraphics() == TRUE)
        {
            gMain.state++;
        }
        break;
    case 4:
        LoadCompressedSpriteSheet(&sSpriteSheet_Cursor);
        LoadSpritePalette(&sSpritePal_Cursor);
        LoadCompressedSpriteSheet(&sSpriteSheet_SelectedCursor);
        LoadSpritePalette(&sSpritePal_SelectedCursor);
        LoadCompressedSpriteSheet(&sSpriteSheet_Numbers);
        LoadSpritePalette(&sSpritePal_Numbers);
        gMain.state++;
        break;
    case 5:
        GreehaseetPuzzle_InitWindows();
        CreateCursor();
        CreateNumberSpriteAt(32, 56, sGreehaseetPuzzleState->tenPearlShell, &sGreehaseetPuzzleState->tenShellSpriteId); // Ten Shell
        CreateNumberSpriteAt(176, 16, sGreehaseetPuzzleState->sevenPearlShell, &sGreehaseetPuzzleState->sevenShellSpriteId); // Seven Shell
        CreateNumberSpriteAt(176, 96, sGreehaseetPuzzleState->threePearlShell, &sGreehaseetPuzzleState->threeShellSpriteId); // Three Shell
        gMain.state++;
        break;
    case 6:
        GreehaseetPuzzle_PrintWindowText();
        CreateTask(Task_GreehaseetPuzzleWaitFadeIn, 0);
        gMain.state++;
        break;
    case 7:
        BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
        gMain.state++;
        break;
    default:
        SetVBlankCallback(GreehaseetPuzzle_VBlankCB);
        SetMainCallback2(GreehaseetPuzzle_MainCB);
        break;
    }
}

static void GreehaseetPuzzle_MainCB(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    DoScheduledBgTilemapCopiesToVram();
    UpdatePaletteFade();
}

static void GreehaseetPuzzle_VBlankCB(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static void Task_GreehaseetPuzzleWaitFadeIn(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        gTasks[taskId].func = Task_GreehaseetPuzzleMainInput;
    }
}

struct SpriteCoordsStruct {
    u8 x;
    u8 y;
};

static void CursorCallback(struct Sprite *sprite)
{
    struct SpriteCoordsStruct spriteCoords[3][1] = { //Thanks Jaizu
        {{88, 64}}, //INPUT_SELECTED_TEN_SHELL
        {{224, 17}}, //INPUT_SELECTED_SEVEN_SHELL
        {{224, 97}}, //INPUT_SELECTED_THREE_SHELL
    };

    u32 cursorY = sGreehaseetPuzzleState->cursorY;
    u32 cursorX = sGreehaseetPuzzleState->cursorX;
    sprite->x   = spriteCoords[cursorY][cursorX].x + 8;
    sprite->y   = spriteCoords[cursorY][cursorX].y + 15;
}

static void NumberCallback(struct Sprite *sprite)
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
    if (sGreehaseetPuzzleState->cursorSpriteId == 0xFF)
        sGreehaseetPuzzleState->cursorSpriteId = CreateSprite(&sSpriteTemplate_Cursor, 0, 0, 0);

    gSprites[sGreehaseetPuzzleState->cursorSpriteId].invisible = FALSE;
    StartSpriteAnim(&gSprites[sGreehaseetPuzzleState->cursorSpriteId], 0);
    return sGreehaseetPuzzleState->cursorSpriteId;
}

static void DestroyCursor(void)
{
    if (sGreehaseetPuzzleState->cursorSpriteId != 0xFF)
        DestroySprite(&gSprites[sGreehaseetPuzzleState->cursorSpriteId]);
    sGreehaseetPuzzleState->cursorSpriteId = 0xFF;
}

static void Task_GreehaseetPuzzleMainInput(u8 taskId)
{
    u8 *cursorY = &sGreehaseetPuzzleState->cursorY;
    u8 *inputMode = &sGreehaseetPuzzleState->inputMode;
    u8 *selectedShell = &sGreehaseetPuzzleState->selectedShell;

    if (JOY_NEW(B_BUTTON))
    {
        if(*inputMode == INPUT_POUR_INTO_SHELL){
            *inputMode = INPUT_SELECT_SHELL;
            if (sGreehaseetPuzzleState->selectedSpriteId != 0xFF)
                DestroySprite(&gSprites[sGreehaseetPuzzleState->selectedSpriteId]);
            sGreehaseetPuzzleState->selectedSpriteId = 0xFF;
            PlaySE(SE_PC_OFF);
        }
        else
        {
            PlaySE(SE_M_DIVE);
            BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
            gTasks[taskId].func = Task_GreehaseetPuzzleWaitFadeAndExitGracefully;
        }
    }
    if (JOY_NEW(A_BUTTON))
    {
        if(*inputMode == INPUT_SELECT_SHELL)
        {
            GreehaseetPuzzle_SelectShell();
            if (sGreehaseetPuzzleState->selectedSpriteId == 0xFF)
            {
                switch(*selectedShell)
                {
                    case INPUT_SELECTED_TEN_SHELL:
                        sGreehaseetPuzzleState->selectedSpriteId = CreateSprite(&sSpriteTemplate_SelectedCursor, 1, 80, 0);
                        break;
                    case INPUT_SELECTED_SEVEN_SHELL:
                        sGreehaseetPuzzleState->selectedSpriteId = CreateSprite(&sSpriteTemplate_SelectedCursor, 144, 35, 0);
                        break;
                    case INPUT_SELECTED_THREE_SHELL:
                        sGreehaseetPuzzleState->selectedSpriteId = CreateSprite(&sSpriteTemplate_SelectedCursor, 144, 115, 0);
                        break;
                }
            }
        }
        else if(*inputMode == INPUT_POUR_INTO_SHELL)
        {
            GreehaseetPuzzle_HandleShellContents();

            if (sGreehaseetPuzzleState->selectedSpriteId != 0xFF)
                DestroySprite(&gSprites[sGreehaseetPuzzleState->selectedSpriteId]);
            sGreehaseetPuzzleState->selectedSpriteId = 0xFF;
            //destroy sprites
        }

        CreateNumberSpriteAt(32, 56, sGreehaseetPuzzleState->tenPearlShell, &sGreehaseetPuzzleState->tenShellSpriteId); // Ten Shell
        CreateNumberSpriteAt(176, 16, sGreehaseetPuzzleState->sevenPearlShell, &sGreehaseetPuzzleState->sevenShellSpriteId); // Seven Shell
        CreateNumberSpriteAt(176, 96, sGreehaseetPuzzleState->threePearlShell, &sGreehaseetPuzzleState->threeShellSpriteId); // Three Shell


        if (sGreehaseetPuzzleState->sevenPearlShell == 5 && sGreehaseetPuzzleState->tenPearlShell == 5){
            FlagSet(FLAG_GREEHASEET_PUZZLE_SOLVED);
            PlaySE(SE_SELECT);
            gTasks[taskId].func = Task_GreehaseetWaitForPuzzleFade;
        }
    }
    if (JOY_NEW(DPAD_DOWN)){
        if (*cursorY == 1) {
            sGreehaseetPuzzleState->cursorY++;
        }
    }
    if (JOY_NEW(DPAD_UP)){
        if (*cursorY == 2) {
            sGreehaseetPuzzleState->cursorY--;
        }
    }
    if (JOY_NEW(DPAD_RIGHT)){
        if(*cursorY == 2){
            sGreehaseetPuzzleState->cursorY = 0;
        }
        else {
            sGreehaseetPuzzleState->cursorY++;
        }
    }
    if (JOY_NEW(DPAD_LEFT)){
        if(*cursorY == 0){
            sGreehaseetPuzzleState->cursorY = 2;
        }
        else {
            sGreehaseetPuzzleState->cursorY--;
        }
    }
    // if(JOY_NEW(SELECT_BUTTON)){
    //     u8 *inputMode = &sGreehaseetPuzzleState->inputMode;
    //     u8 *selectedShell = &sGreehaseetPuzzleState->selectedShell;
    //     u8 *tenPearlShell = &sGreehaseetPuzzleState->tenPearlShell;
    //     u8 *sevenPearlShell = &sGreehaseetPuzzleState->sevenPearlShell;
    //     u8 *threePearlShell = &sGreehaseetPuzzleState->threePearlShell;
    // }
}

static void Task_GreehaseetWaitForPuzzleFade(u8 taskId)
{
    if (gTasks[taskId].data[0] < 120){ // thanks hedara
        gTasks[taskId].data[0]++;
    }
    else {
        PlaySE(SE_M_ROCK_THROW);
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
        gTasks[taskId].func = Task_GreehaseetPuzzleWaitFadeAndExitGracefully;
    }
}

static void Task_GreehaseetPuzzleWaitFadeAndBail(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        SetMainCallback2(sGreehaseetPuzzleState->savedCallback);
        GreehaseetPuzzle_FreeResources();
        DestroyTask(taskId);
    }
}

static void Task_GreehaseetPuzzleWaitFadeAndExitGracefully(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        SetMainCallback2(sGreehaseetPuzzleState->savedCallback);
        GreehaseetPuzzle_FreeResources();
        DestroyTask(taskId);
    }
}

#define TILEMAP_BUFFER_SIZE (1024 * 2)
static bool8 GreehaseetPuzzle_InitBgs(void)
{
    ResetAllBgsCoordinates();

    sBg1TilemapBuffer = AllocZeroed(TILEMAP_BUFFER_SIZE);
    if (sBg1TilemapBuffer == NULL)
    {
        return FALSE;
    }

    ResetBgsAndClearDma3BusyFlags(0);
    InitBgsFromTemplates(0, sGreehaseetPuzzleBgTemplates, NELEMS(sGreehaseetPuzzleBgTemplates));

    SetBgTilemapBuffer(1, sBg1TilemapBuffer);
    ScheduleBgCopyTilemapToVram(1);

    ShowBg(0);
    ShowBg(1);

    return TRUE;
}
#undef TILEMAP_BUFFER_SIZE

static void GreehaseetPuzzle_FadeAndBail(void)
{
    BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
    CreateTask(Task_GreehaseetPuzzleWaitFadeAndBail, 0);
    SetVBlankCallback(GreehaseetPuzzle_VBlankCB);
    SetMainCallback2(GreehaseetPuzzle_MainCB);
}

static bool8 GreehaseetPuzzle_LoadGraphics(void)
{
    switch (sGreehaseetPuzzleState->loadState)
    {
    case 0:
        ResetTempTileDataBuffers();
        DecompressAndCopyTileDataToVram(1, sGreehaseetPuzzleTiles, 0, 0, 0);
        sGreehaseetPuzzleState->loadState++;
        break;
    case 1:
        if (FreeTempTileDataBuffersIfPossible() != TRUE)
        {
            DecompressDataWithHeaderWram(sGreehaseetPuzzleTilemap, sBg1TilemapBuffer);
            sGreehaseetPuzzleState->loadState++;
        }
        break;
    case 2:
        LoadPalette(sGreehaseetPuzzlePalette, BG_PLTT_ID(0), PLTT_SIZE_4BPP);
        LoadPalette(gMessageBox_Pal, BG_PLTT_ID(15), PLTT_SIZE_4BPP);
        sGreehaseetPuzzleState->loadState++;
    default:
        sGreehaseetPuzzleState->loadState = 0;
        return TRUE;
    }
    return FALSE;
}

static void GreehaseetPuzzle_InitWindows(void)
{
    InitWindows(sGigageshoolPuzzleWindowTemplates);
    DeactivateAllTextPrinters();
    ScheduleBgCopyTilemapToVram(0);
    FillWindowPixelBuffer(WINDOW_INSTRUCTIONS, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    PutWindowTilemap(WINDOW_INSTRUCTIONS);
    CopyWindowToVram(WINDOW_INSTRUCTIONS, 3);
    FillWindowPixelBuffer(WINDOW_CONTROLS, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    PutWindowTilemap(WINDOW_CONTROLS);
    CopyWindowToVram(WINDOW_CONTROLS, 3);
}

static void GreehaseetPuzzle_PrintWindowText(void)
{
    FillWindowPixelBuffer(WINDOW_INSTRUCTIONS, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));
    FillWindowPixelBuffer(WINDOW_CONTROLS, PIXEL_FILL(TEXT_COLOR_TRANSPARENT));

    AddTextPrinterParameterized4(WINDOW_INSTRUCTIONS, FONT_NARROW, 0, 3, 0, 0,
        sGreehaseetPuzzleWindowFontColors[FONT_WHITE], TEXT_SKIP_DRAW, COMPOUND_STRING("Divide 10 pearls into\ntwo sets of 5."));

    AddTextPrinterParameterized4(WINDOW_CONTROLS, FONT_SMALL_NARROWER, 0, 3, 0, 0,
        sGreehaseetPuzzleWindowFontColors[FONT_WHITE], TEXT_SKIP_DRAW, COMPOUND_STRING("{A_BUTTON} Choose a shell. {B_BUTTON} Exit.\n{A_BUTTON} Choose shell to put\npearls in."));

    CopyWindowToVram(WINDOW_INSTRUCTIONS, COPYWIN_GFX);
    CopyWindowToVram(WINDOW_CONTROLS, COPYWIN_GFX);
}
static void GreehaseetPuzzle_FreeResources(void)
{
    DestroyCursor();
    if (sGreehaseetPuzzleState != NULL)
    {
        Free(sGreehaseetPuzzleState);
    }
    if (sBg1TilemapBuffer != NULL)
    {
        Free(sBg1TilemapBuffer);
    }
    FreeAllWindowBuffers();
    ResetSpriteData();
}

static void GreehaseetPuzzle_SelectShell(void)
{
    u8 *cursorY = &sGreehaseetPuzzleState->cursorY;
    u8 *selectedShell = &sGreehaseetPuzzleState->selectedShell;

    PlaySE(SE_SELECT);
    sGreehaseetPuzzleState->inputMode = INPUT_POUR_INTO_SHELL;
    switch(*cursorY){
        case 0:
            *selectedShell = INPUT_SELECTED_TEN_SHELL;
            break;
        case 1:
            *selectedShell = INPUT_SELECTED_SEVEN_SHELL;
            break;
        case 2:
            *selectedShell = INPUT_SELECTED_THREE_SHELL;
            break;
    }
}

static void TransferPearls(u8 * sourceShell, u8 * targetShell, u8 maxTarget) {
    u8 rest = (*sourceShell + *targetShell <= maxTarget) ?
        0 : MathUtil_Max(maxTarget, *sourceShell + *targetShell) - MathUtil_Min(maxTarget, *sourceShell + *targetShell);
    *targetShell = (*sourceShell + *targetShell) - rest;
    *sourceShell = rest;
}

static void GreehaseetPuzzle_HandleShellContents(void)
{
    u8 *selectedShell = &sGreehaseetPuzzleState->selectedShell;
    u8 *tenShell = &sGreehaseetPuzzleState->tenPearlShell;
    u8 *sevenShell = &sGreehaseetPuzzleState->sevenPearlShell;
    u8 *threeShell = &sGreehaseetPuzzleState->threePearlShell;
    u8 *cursorY = &sGreehaseetPuzzleState->cursorY;
    u8 *inputMode = &sGreehaseetPuzzleState->inputMode;

    PlaySE(SE_SELECT);
    switch(*selectedShell){
        case INPUT_SELECTED_TEN_SHELL:
            if(*cursorY == 1){ // For when Ten Shell to Seven Shell
                TransferPearls(tenShell, sevenShell, MAX_SEVEN_SHELL);

                *inputMode = INPUT_SELECT_SHELL;
            }
            else if (*cursorY == 2){ //For when Ten Shell to Three Shell
                TransferPearls(tenShell, threeShell, MAX_THREE_SHELL);

                *inputMode = INPUT_SELECT_SHELL;
            }
            else { //For Ten Shell back to Ten
                *inputMode = INPUT_SELECT_SHELL;
            }
            break;
        case INPUT_SELECTED_SEVEN_SHELL:
            if(*cursorY == 0){ //For when Seven Shell to Ten Shell
                TransferPearls(sevenShell, tenShell, MAX_TEN_SHELL);
            }
            else if (*cursorY == 2){ //For when Seven Shell to Three Shell
                TransferPearls(sevenShell, threeShell, MAX_THREE_SHELL);

                *inputMode = INPUT_SELECT_SHELL;
            }
            else { //For Seven Shell back to Seven
                *inputMode = INPUT_SELECT_SHELL;
            }
            break;
        case INPUT_SELECTED_THREE_SHELL:
            if(*cursorY == 2){ //For Three Shell back to Three
                *inputMode = INPUT_SELECT_SHELL;
            }
            else if (*cursorY == 1){//Three Shell to Seven Shell
                TransferPearls(threeShell, sevenShell, MAX_SEVEN_SHELL);
                *inputMode = INPUT_SELECT_SHELL;
            }
            else
            { //For Three Shell to Ten
                if(*cursorY == 0)
                {
                    TransferPearls(threeShell, tenShell, MAX_TEN_SHELL);
                }
                *inputMode = INPUT_SELECT_SHELL;
            }
            break;
    }

    DestroyNumberSpriteAt(32, 56, &sGreehaseetPuzzleState->tenShellSpriteId); // Destroys ten shell
    DestroyNumberSpriteAt(176, 16, &sGreehaseetPuzzleState->sevenShellSpriteId); // Destroys seven shell
    DestroyNumberSpriteAt(176, 96, &sGreehaseetPuzzleState->threeShellSpriteId); // Destroys three shell
}
