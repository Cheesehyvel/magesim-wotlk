#pragma once

#include <random>

#define MANA_TICK_T 1.0
#define IGNITE_MUNCH_WINDOW 0.01
#define IGNITE_BLEED_WINDOW 0.04

enum Rotation : int
{
    ROTATION_ST_FROSTFIRE = 0,
    ROTATION_ST_AB_AM = 10,
    ROTATION_ST_AB_AM_BARRAGE = 11,
    ROTATION_ST_FIRE = 20,
    ROTATION_ST_FIRE_SC = 21,
    ROTATION_ST_FROST = 30,
    ROTATION_AOE_AE = 100,
    ROTATION_AOE_AE_FS = 101,
    ROTATION_AOE_BLIZZ = 110,
    ROTATION_AOE_BLIZZ_FS = 111,
    ROTATION_AOE_FIRE = 120,
};

enum Race : int
{
    RACE_BLOOD_ELF,
    RACE_DRAENEI,
    RACE_GNOME,
    RACE_HUMAN,
    RACE_TROLL,
    RACE_UNDEAD
};

enum Faction : int
{
    FACTION_HORDE,
    FACTION_ALLIANCE,
};

enum School : int
{
    SCHOOL_NONE,
    SCHOOL_ARCANE,
    SCHOOL_FROST,
    SCHOOL_FIRE,
    SCHOOL_NATURE,
    SCHOOL_SHADOW,
    SCHOOL_FROSTFIRE,
    SCHOOL_HOLY,
};

enum MetaGem : int
{
    META_NONE = 0,
    META_CHAOTIC_SKYFLARE = 41285,
    META_EMBER_SKYFLARE = 41333,
    META_BEAMING_EARTHSIEGE = 41389,
    META_INSIGHTFUL_EARTHSIEGE = 41389,
};

enum Enchant : int
{
    ENCHANT_NONE = 0,
    ENCHANT_WEAPON_GREATER_SPELLPOWER = 62948,
    ENCHANT_WEAPON_MIGHTY_SPELLPOWER = 60714,
    ENCHANT_WEAPON_ACCURACY = 59619,
};

enum Trinket : int
{
    TRINKET_NONE = 0,
    // Vanilla
    TRINKET_MQG = 19339,
    // Tbc
    TRINKET_SERPENT_COIL = 30720,
    TRINKET_SKULL_GULDAN = 32483,
    TRINKET_SHRUNKEN_HEAD = 33829,
    TRINKET_NAARU_SLIVER = 34429,
    TRINKET_ASHTONGUE_TALISMAN = 32488,
    // Wotlk
    TRINKET_MERCURIAL_ALCHEMIST_STONE = 44322,
    TRINKET_TWILIGHT_SERPENT = 42395,
    TRINKET_RUNE_INFINITE_POWER = 38765,
    TRINKET_TOME_ARCANE_PHENOMENA = 36972,
    TRINKET_CANNONEERS_FUSELIGHTER = 44013,
    TRINKET_MARK_WAR_PRISONER = 37873,
    TRINKET_FORGE_EMBER = 37660,
    TRINKET_PENDULUM_TELLURIC_CURRENTS = 37264,
    TRINKET_DARKMOON_DEATH = 42990,
    TRINKET_DARKMOON_ILLUSION = 42988,
    TRINKET_SUNDIAL_EXILED = 40682,
    TRINKET_ABYSSAL_RUNE = 47213,
    TRINKET_EMBRACE_SPIDER = 39229,
    TRINKET_DYING_CURSE = 40255,
    TRINKET_EXTRACT_NECROMANTIC_POWER = 40373,
    TRINKET_SOUL_DEAD = 40382,
    TRINKET_ILLUSTRATION_DRAGON_SOUL = 40432,
    TRINKET_ELEMENTAL_FOCUS_STONE = 45866,
    TRINKET_EYE_BROODMOTHER = 45308,
    TRINKET_SIFS_REMEMBERANCE = 45929,
    TRINKET_SPARK_HOPE = 45703,
    TRINKET_SCALE_FATES = 45466,
    TRINKET_FLARE_HEAVENS = 45518,
    TRINKET_SHOW_FAITH = 45535,
    TRINKET_PANDORAS_PLEA = 45490,
    TRINKET_LIVING_FLAME = 45148,
    TRINKET_FETISH_VOLATILE_POWER_HC = 48018,
    TRINKET_FETISH_VOLATILE_POWER_NM = 47879,
    TRINKET_TALISMAN_VOLATILE_POWER_HC = 47946,
    TRINKET_TALISMAN_VOLATILE_POWER_NM = 47726,
    TRINKET_SOLACE_FALLEN_HC = 47432,
    TRINKET_SOLACE_FALLEN_NM = 47271,
    TRINKET_SOLACE_DEFEATED_HC = 47059,
    TRINKET_SOLACE_DEFEATED_NM = 47041,
    TRINKET_REIGN_DEAD_HC = 47477,
    TRINKET_REIGN_DEAD_NM = 47316,
    TRINKET_REIGN_UNLIVING_HC = 47188,
    TRINKET_REIGN_UNLIVING_NM = 47182,
    TRINKET_MURADINS_SPYGLASS_HC = 50345,
    TRINKET_MURADINS_SPYGLASS_NM = 50340,
    TRINKET_SLIVER_PURE_ICE_HC = 50346,
    TRINKET_SLIVER_PURE_ICE_NM = 50339,
    TRINKET_DISLODGED_OBJECT_HC = 50348,
    TRINKET_DISLODGED_OBJECT_NM = 50353,
    TRINKET_NAMELESS_LICH_HC = 50365,
    TRINKET_NAMELESS_LICH_NM = 50360,
    TRINKET_SHARD_CRYSTAL_HEART = 48722,
    TRINKET_TALISMAN_RESURGENCE = 48724,
    TRINKET_CHARRED_TWILIGHT_SCALE_HC = 54588,
    TRINKET_CHARRED_TWILIGHT_SCALE_NM = 54572,
    TRINKET_MISGUIDED_QUILL = 50357,
    TRINKET_PURIFIED_LUNAR_DUST = 50358,
};

enum Potion : int
{
    POTION_NONE = 0,
    POTION_MANA = 33448,
    POTION_SPEED = 40211,
    POTION_WILD_MAGIC = 40212,
    POTION_FLAME_CAP = 22788,
};

enum Conjured : int
{
    CONJURED_NONE = 0,
    CONJURED_DARK_RUNE = 20520,
};

enum EventType : int
{
    EVENT_NONE,
    EVENT_CAST_START,
    EVENT_CAST_FINISH,
    EVENT_SPELL_IMPACT,
    EVENT_SPELL_TICK,
    EVENT_MANA_REGEN,
    EVENT_MANA_GAIN,
    EVENT_BUFF_GAIN,
    EVENT_BUFF_EXPIRE,
    EVENT_BUFF_GAIN_ALL,
    EVENT_DEBUFF_GAIN,
    EVENT_DEBUFF_EXPIRE,
    EVENT_DOT,
    EVENT_CD_GAIN,
    EVENT_CD_EXPIRE,
    EVENT_UNIT,
    EVENT_UNIT_DESPAWN,
    EVENT_WAIT,
    EVENT_INTERRUPTION,
    EVENT_INTERRUPTION_END
};

struct SpellStats
{
    std::string source;
    std::string name;
    double casts = 0;
    double misses = 0;
    double hits = 0;
    double crits = 0;
    double dmg = 0;
    double min_dmg = 0;
    double max_dmg = 0;
};

struct SimulationResult
{
    int dmg;
    double t;
    double dps;
    double t_gcd_capped;
    std::string log;
    std::string spells;
};

struct SimulationsResult
{
    double min_dps = 0;
    double max_dps = 0;
    double avg_dps = 0;
    double t_gcd_capped = 0;
    int iterations;
    std::string histogram;
    std::string all_results;
    std::string spells;
};

double critRatingToChance(double rating);

double hitRatingToChance(double rating);

double hasteRatingToHaste(double rating);

// Global rng generator
extern thread_local std::mt19937 g_rng;

template <typename Numeric>
Numeric random(Numeric from, Numeric to)
{
    using dist_type = typename std::conditional<
        std::is_integral<Numeric>::value, std::uniform_int_distribution<Numeric>, std::uniform_real_distribution<Numeric>>::type;

    thread_local static dist_type dist;

    return dist(g_rng, typename dist_type::param_type{from, to});
}

void setRNGSeed(int seed);
