enum Rotation : int
{
    ROTATION_ST_FROSTFIRE = 0,
    ROTATION_ST_AB_AM = 10,
    ROTATION_ST_AB_AM_BARRAGE = 11,
    ROTATION_ST_FIRE = 20,
    ROTATION_ST_FROST = 30,
    ROTATION_AOE_AE = 100,
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
};

enum MetaGem : int {
    META_NONE = 0,
    META_CHAOTIC_SKYFLARE = 41285,
    META_EMBER_SKYFLARE = 41333,
    META_BEAMING_EARTHSIEGE = 41389,
    META_INSIGHTFUL_EARTHSIEGE = 41389,
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
};

enum Drums : int
{
    DRUMS_NONE = 0,
    DRUMS_OF_WAR = 29528,
    DRUMS_OF_RESTORATION = 29531,
    DRUMS_OF_BATTLE = 29529,
    DRUMS_OF_FORGOTTEN_KINGS = 49633,
};

enum Potion : int
{
    POTION_NONE = 0,
    POTION_MANA = 33448,
    POTION_SPEED = 40211,
    POTION_WILD_MAGIC = 40212,
};

enum Conjured : int
{
    CONJURED_NONE = 0,
    CONJURED_MANA_GEM = 33312,
    CONJURED_FLAME_CAP = 22788,
};

enum EventType : int
{
    EVENT_CAST_START,
    EVENT_CAST_FINISH,
    EVENT_SPELL_IMPACT,
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
    EVENT_WAIT
};

struct SpellStats
{
    std::string source;
    std::string name;
    int casts = 0;
    int misses = 0;
    int hits = 0;
    int crits = 0;
    double dmg = 0;
    double min_dmg = 0;
    double max_dmg = 0;
};

struct SimulationResult
{
    int dmg;
    double t;
    double dps;
    double evocated_at;
    double t_gcd_capped = 0;
    std::string log;
    std::string spells;
};

struct SimulationsResult
{
    double min_dps, avg_dps, max_dps;
    int iterations;
    std::string histogram;
    std::string all_results;
    std::string stats;
};

double critRatingToChance(double rating)
{
    return rating / 45.91;
}

double hitRatingToChance(double rating)
{
    return rating / 26.232;
}

double hasteRatingToHaste(double rating)
{
    return rating / 32.79;
}

bool isTimerReady(std::vector<double>& v, double t)
{
    for (int i=0; i<v.size(); i++) {
        if (v.at(i) > t)
            return false;
        // We give it a 20 second window to pop, otherwise it's an old timer
        if (v.at(i) <= t && v.at(i) + 20 > t)
            return true;
    }

    return true;
}

bool isTimerReadyExplicit(std::vector<double>& v, double t)
{
    for (int i=0; i<v.size(); i++) {
        if (v.at(i) > t)
            return false;
        // We give it a 20 second window to pop, otherwise it's an old timer
        if (v.at(i) <= t && v.at(i) + 20 > t)
            return true;
    }

    return false;
}

// Global rng generator
thread_local static std::mt19937 g_rng(std::random_device{}());

template<typename Numeric>
Numeric random(Numeric from, Numeric to)
{
    using dist_type = typename std::conditional
    <
        std::is_integral<Numeric>::value
        , std::uniform_int_distribution<Numeric>
        , std::uniform_real_distribution<Numeric>
    >::type;

    thread_local static dist_type dist;

    return dist(g_rng, typename dist_type::param_type{from, to});
}

void setRNGSeed(int seed)
{
    g_rng.seed(seed);
}
