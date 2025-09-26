#include "constants/event_objects.h"

static const u32 sFieldMugshotGfx_TestNormal[] = INCBIN_U32("graphics/field_mugshots/test/normal.4bpp.lz");
static const u32 sFieldMugshotGfx_TestAlt[] = INCBIN_U32("graphics/field_mugshots/test/alt.4bpp.lz");
static const u16 sFieldMugshotPal_TestNormal[] = INCBIN_U16("graphics/field_mugshots/test/normal.gbapal");
static const u16 sFieldMugshotPal_TestAlt[] = INCBIN_U16("graphics/field_mugshots/test/alt.gbapal");

static const u32 sFieldMugshot_CoolArchie[] = INCBIN_U32("graphics/field_mugshots/cool_archie.4bpp.lz");
static const u32 sFieldMugshot_OldArchie[] = INCBIN_U32("graphics/field_mugshots/old_archie.4bpp.lz");
static const u32 sFieldMugshot_Greehaseet[] = INCBIN_U32("graphics/field_mugshots/greehaseet.4bpp.lz");
static const u32 sFieldMugshot_Nessereign[] = INCBIN_U32("graphics/field_mugshots/nessereign.4bpp.lz");
static const u32 sFieldMugshot_Sapprilon[] = INCBIN_U32("graphics/field_mugshots/sapprilon.4bpp.lz");
static const u32 sFieldMugshot_Addisamap[] = INCBIN_U32("graphics/field_mugshots/addisamap.4bpp.lz");
static const u32 sFieldMugshot_Maxie[] = INCBIN_U32("graphics/field_mugshots/maxie.4bpp.lz");

static const u16 sFieldMugshotPal_CoolArchie[] = INCBIN_U16("graphics/field_mugshots/cool_archie.gbapal");

struct MugshotGfx
{
    const u32 *gfx;
    const u16 *pal;
    u32 tag;        // object event-based palette (tag)
};

static const struct MugshotGfx sFieldMugshots[MUGSHOT_COUNT][EMOTE_COUNT] =
{
    [MUGSHOT_TEST] =
    {
        [EMOTE_NORMAL] =
        {
            .gfx = sFieldMugshotGfx_TestNormal,
            .pal = sFieldMugshotPal_TestNormal,
        },

        [EMOTE_ALT] =
        {
            .gfx = sFieldMugshotGfx_TestAlt,
            .pal = sFieldMugshotPal_TestAlt,
        },
    },

    [MUGSHOT_MC] =
        {
            [EMOTE_NORMAL] =
            {
                .gfx = sFieldMugshot_CoolArchie,
                .pal = sFieldMugshotPal_CoolArchie,
            }
        },
        [MUGSHOT_OLD_ARCHIE] =
        {
            [EMOTE_NORMAL] =
            {
                .gfx = sFieldMugshot_OldArchie,
                .tag = OBJ_EVENT_PAL_TAG_COOL_ARCHIE,
            }
        },
        [MUGSHOT_GREEHASEET] =
        {
            [EMOTE_NORMAL] =
            {
                .gfx = sFieldMugshot_Greehaseet,
                .tag = OBJ_EVENT_PAL_TAG_GREEHASEET,
            }
        },
        [MUGSHOT_NESSIE] =
        {
            [EMOTE_NORMAL] =
            {
                .gfx = sFieldMugshot_Nessereign,
                .tag = OBJ_EVENT_PAL_TAG_NESSEREIGN,
            }
        },
        [MUGSHOT_SAPPY] =
        {
            [EMOTE_NORMAL] =
            {
                .gfx = sFieldMugshot_Sapprilon,
                .tag = OBJ_EVENT_PAL_TAG_SAPPRILON,
            }
        },
        [MUGSHOT_ADDISAMAP] =
        {
            [EMOTE_NORMAL] =
            {
                .gfx = sFieldMugshot_Addisamap,
                .tag = OBJ_EVENT_PAL_TAG_ADDISAMAP,
            }
        },
        [MUGSHOT_MAXIE] =
        {
            [EMOTE_NORMAL] =
            {
                .gfx = sFieldMugshot_Maxie,
                .tag = OBJ_EVENT_PAL_TAG_MAXIE,
            }
        },
};
