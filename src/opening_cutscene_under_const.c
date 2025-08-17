/*
#include "global.h"
#include "main.h"
#include "bg.h"
#include "gpu_regs.h"
#include "sprite.h"
#include "window.h"
#include "decompress.h"
#include "text.h"
#include "constants/characters.h"
#include "string_util.h"
#include "palette.h"
#include "palette_util.h"
#include "malloc.h"
#include "task.h"
#include "menu.h"
#include "scanline_effect.h"
#include "sound.h"
#include "strings.h"
#include "trainer_pokemon_sprites.h"
#include "pokeball.h"
#include "naming_screen.h"
#include "overworld.h"
#include "constants/rgb.h"
#include "constants/songs.h"

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
    WIN_TOP = 0,
    WIN_MID,
    WIN_BOT,
    WIN_COUNT,
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
    u16 blackTilemapBuffer[0x800];
    u16 fireTilemapBuffer[0x800];
    u16 fieldTilemapBuffer[0x800];
    s16 alphaCoeff;
    s16 alphaCoeff2;
    s16 timer;
    s16 fadeTimer;
    s16 counter;
    bool32 fadeFinished:1;
};

// EWRAM data
static EWRAM_DATA struct IntroSequence *sIntroSequence = NULL;

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
static const u16 sBlackBg_BgPal[] = INCBIN_U16("graphics/intro_sequence/black_bg.gbapal");
static const u32 sBlackBg_BgMap[] = INCBIN_U32("graphics/intro_sequence/black_bg.bin.smolTM");

static const u16 sFireBg_BgGfx[] = INCBIN_U16("graphics/intro_sequence/fire_bg.4bpp");
static const u16 sFireBg_BgPal[] = INCBIN_U16("graphics/intro_sequence/fire_bg.gbapal");
static const u32 sFireBg_BgMap[] = INCBIN_U32("graphics/intro_sequence/fire_bg.bin.smolTM");

static const u16 sFieldBg_BgGfx[] = INCBIN_U16("graphics/intro_sequence/field_bg.4bpp");
static const u16 sFieldBg_BgPal[] = INCBIN_U16("graphics/intro_sequence/field_bg.gbapal");
static const u32 sFieldBg_BgMap[] = INCBIN_U32("graphics/intro_sequence/field_bg.bin.smolTM");

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
    [WIN_TOP] =
    {
        .bg = BG_TEXT,
        .tilemapLeft = 1,
        .tilemapTop = 1,
        .width = 28,
        .height = 5,
        .paletteNum = 15,
        .baseBlock = 1,
    },
    [WIN_MID] =
    {
        .bg = BG_TEXT,
        .tilemapLeft = 1,
        .tilemapTop = 7,
        .width = 28,
        .height = 5,
        .paletteNum = 15,
        .baseBlock = 0x8D, //1 + (28 * 5)
    },
    [WIN_BOT] =
    {
        .bg = BG_TEXT,
        .tilemapLeft = 1,
        .tilemapTop = 7,
        .width = 28,
        .height = 6,
        .paletteNum = 15,
        .baseBlock = 0x119, //0x8D + (28 * 5),
    },
    DUMMY_WIN_TEMPLATE,
};
*/