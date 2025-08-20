#ifdef __INTELLISENSE__
const struct SpeciesInfo gSpeciesInfoGen9[] =
{
#endif
    [SPECIES_NESSEREIGN] =
    {
        .baseHP        = 50,
        .baseAttack    = 190,
        .baseDefense   = 20,
        .baseSpeed     = 160,
        .baseSpAttack  = 190,
        .baseSpDefense = 20,
        .types = MON_TYPES(TYPE_NORMAL, TYPE_DARK),
        .catchRate = 255,
        .expYield = 0,
        .evYield_SpAttack = 3,
        .genderRatio = MON_GENDERLESS,
        .eggCycles = 20,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_NO_EGGS_DISCOVERED),
        .abilities = { ABILITY_PROTEAN, ABILITY_CURSED_BODY, ABILITY_DAMP },
        .bodyColor = BODY_COLOR_BLACK,
        .speciesName = _("Nessereign"),
        .cryId = CRY_NONE,
        .natDexNum = NATIONAL_DEX_NONE,
        .categoryName = _("DemonLord"),
        .height = 0,
        .weight = 0,
        .description = COMPOUND_STRING(
            "This is a newly discovered Pokémon.\n"
            "It is currently under investigation.\n"
            "No detailed information is available\n"
            "at this time."),
        .pokemonScale = 256,
        .pokemonOffset = 0,
        .trainerScale = 256,
        .trainerOffset = 0,
        .frontPic = gMonFrontPic_Nessereign,
        .frontPicSize = MON_COORDS_SIZE(64, 64),
        .frontPicYOffset = 0,
        .frontAnimFrames = sAnims_SingleFramePlaceHolder,
        //.frontAnimId = ANIM_V_SQUISH_AND_BOUNCE,
        .backPic = gMonBackPic_CircledQuestionMark,
        .backPicSize = MON_COORDS_SIZE(64, 64),
        .backPicYOffset = 7,
        .backAnimId = BACK_ANIM_NONE,
        .palette = gMonPalette_Nessereign,
        .shinyPalette = gMonShinyPalette_CircledQuestionMark,
        .iconSprite = gMonIcon_QuestionMark,
        .iconPalIndex = 0,
        .enemyMonElevation = 11,
        FOOTPRINT(QuestionMark)
        SHADOW(0, 20, SHADOW_SIZE_XL_BATTLE_ONLY)
        // OVERWORLD(
        //     sPicTable_Pecharunt,
        //     SIZE_32x32,
        //     SHADOW_SIZE_M,
        //     sAnimTable_Following,
        //     gOverworldPalette_Pecharunt,
        //     gShinyOverworldPalette_Pecharunt
        // )
        .levelUpLearnset = sNoneLevelUpLearnset,
    },
    /*
    [SPECIES_GREEHASEET] =
    {
        .baseHP        = 103,
        .baseAttack    = 57,
        .baseDefense   = 85,
        .baseSpeed     = 79,
        .baseSpAttack  = 81,
        .baseSpDefense = 98,
        .types = MON_TYPES(TYPE_WATER, TYPE_ICE),
        .catchRate = 255,
        .expYield = 0,
        .evYield_SpAttack = 3,
        .genderRatio = MON_GENDERLESS,
        .eggCycles = 20,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_NO_EGGS_DISCOVERED),
        .abilities = { ABILITY_SNOW_WARNING, ABILITY_CURSED_BODY, ABILITY_DAMP },
        .bodyColor = BODY_COLOR_BLACK,
        .speciesName = _("Greehaseet"),
        .cryId = CRY_NONE,
        .natDexNum = NATIONAL_DEX_NONE,
        .categoryName = _("DemonLord"),
        .height = 0,
        .weight = 0,
        .description = COMPOUND_STRING(
            "This is a newly discovered Pokémon.\n"
            "It is currently under investigation.\n"
            "No detailed information is available\n"
            "at this time."),
        .pokemonScale = 256,
        .pokemonOffset = 0,
        .trainerScale = 256,
        .trainerOffset = 0,
        .frontPic = gMonFrontPic_Nessereign,
        .frontPicSize = MON_COORDS_SIZE(64, 64),
        .frontPicYOffset = 0,
        .frontAnimFrames = sAnims_SingleFramePlaceHolder,
        //.frontAnimId = ANIM_V_SQUISH_AND_BOUNCE,
        .backPic = gMonBackPic_CircledQuestionMark,
        .backPicSize = MON_COORDS_SIZE(64, 64),
        .backPicYOffset = 7,
        .backAnimId = BACK_ANIM_NONE,
        .palette = gMonPalette_Nessereign,
        .shinyPalette = gMonShinyPalette_CircledQuestionMark,
        .iconSprite = gMonIcon_QuestionMark,
        .iconPalIndex = 0,
        .enemyMonElevation = 11,
        FOOTPRINT(QuestionMark)
        SHADOW(0, 20, SHADOW_SIZE_XL_BATTLE_ONLY)
        // OVERWORLD(
        //     sPicTable_Pecharunt,
        //     SIZE_32x32,
        //     SHADOW_SIZE_M,
        //     sAnimTable_Following,
        //     gOverworldPalette_Pecharunt,
        //     gShinyOverworldPalette_Pecharunt
        // )
        .levelUpLearnset = sNoneLevelUpLearnset,
    },
    
    [SPECIES_SAPPRILON] =
    {
        .baseHP        = 73,
        .baseAttack    = 67,
        .baseDefense   = 95,
        .baseSpeed     = 119,
        .baseSpAttack  = 81,
        .baseSpDefense = 110,
        .types = MON_TYPES(TYPE_PSYCHIC, TYPE_STEEL),
        .catchRate = 255,
        .expYield = 0,
        .evYield_SpAttack = 3,
        .genderRatio = MON_GENDERLESS,
        .eggCycles = 20,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_NO_EGGS_DISCOVERED),
        .abilities = { ABILITY_NONE, ABILITY_CURSED_BODY, ABILITY_DAMP },
        .bodyColor = BODY_COLOR_BLACK,
        .speciesName = _("Sapprilon"),
        .cryId = CRY_NONE,
        .natDexNum = NATIONAL_DEX_NONE,
        .categoryName = _("DemonLord"),
        .height = 0,
        .weight = 0,
        .description = COMPOUND_STRING(
            "This is a newly discovered Pokémon.\n"
            "It is currently under investigation.\n"
            "No detailed information is available\n"
            "at this time."),
        .pokemonScale = 256,
        .pokemonOffset = 0,
        .trainerScale = 256,
        .trainerOffset = 0,
        .frontPic = gMonFrontPic_Nessereign,
        .frontPicSize = MON_COORDS_SIZE(64, 64),
        .frontPicYOffset = 0,
        .frontAnimFrames = sAnims_SingleFramePlaceHolder,
        //.frontAnimId = ANIM_V_SQUISH_AND_BOUNCE,
        .backPic = gMonBackPic_CircledQuestionMark,
        .backPicSize = MON_COORDS_SIZE(64, 64),
        .backPicYOffset = 7,
        .backAnimId = BACK_ANIM_NONE,
        .palette = gMonPalette_Nessereign,
        .shinyPalette = gMonShinyPalette_CircledQuestionMark,
        .iconSprite = gMonIcon_QuestionMark,
        .iconPalIndex = 0,
        .enemyMonElevation = 11,
        FOOTPRINT(QuestionMark)
        SHADOW(0, 20, SHADOW_SIZE_XL_BATTLE_ONLY)
        // OVERWORLD(
        //     sPicTable_Pecharunt,
        //     SIZE_32x32,
        //     SHADOW_SIZE_M,
        //     sAnimTable_Following,
        //     gOverworldPalette_Pecharunt,
        //     gShinyOverworldPalette_Pecharunt
        // )
        .levelUpLearnset = sNoneLevelUpLearnset,
    },

    */
    [SPECIES_ADDISAMAP] =
    {
        .baseHP        = 255,
        .baseAttack    = 50,
        .baseDefense   = 50,
        .baseSpeed     = 80,
        .baseSpAttack  = 75,
        .baseSpDefense = 135,
        .types = MON_TYPES(TYPE_GROUND, TYPE_GRASS),
        .catchRate = 255,
        .expYield = 0,
        .evYield_SpAttack = 3,
        .genderRatio = MON_GENDERLESS,
        .eggCycles = 20,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_NO_EGGS_DISCOVERED),
        .abilities = { ABILITY_GRASSY_SURGE, ABILITY_CURSED_BODY, ABILITY_DAMP },
        .bodyColor = BODY_COLOR_BLACK,
        .speciesName = _("Addisamap"),
        .cryId = CRY_NONE,
        .natDexNum = NATIONAL_DEX_NONE,
        .categoryName = _("DemonLord"),
        .height = 0,
        .weight = 0,
        .description = COMPOUND_STRING(
            "This is a newly discovered Pokémon.\n"
            "It is currently under investigation.\n"
            "No detailed information is available\n"
            "at this time."),
        .pokemonScale = 256,
        .pokemonOffset = 0,
        .trainerScale = 256,
        .trainerOffset = 0,
        .frontPic = gMonFrontPic_Addisamap,
        .frontPicSize = MON_COORDS_SIZE(64, 64),
        .frontPicYOffset = 0,
        .frontAnimFrames = sAnims_SingleFramePlaceHolder,
        //.frontAnimId = ANIM_V_SQUISH_AND_BOUNCE,
        .backPic = gMonBackPic_CircledQuestionMark,
        .backPicSize = MON_COORDS_SIZE(64, 64),
        .backPicYOffset = 2,
        .backAnimId = BACK_ANIM_NONE,
        .palette = gMonPalette_Addisamap,
        .shinyPalette = gMonShinyPalette_CircledQuestionMark,
        .iconSprite = gMonIcon_QuestionMark,
        .iconPalIndex = 0,
        .enemyMonElevation = 0,
        FOOTPRINT(QuestionMark)
        SHADOW(1, 7, SHADOW_SIZE_XL_BATTLE_ONLY)
        .levelUpLearnset = sNoneLevelUpLearnset,
    },
#ifdef __INTELLISENSE__
};
#endif
