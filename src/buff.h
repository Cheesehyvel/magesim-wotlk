#pragma once

#include "stats.h"

#include <string>
#include <sstream>

namespace buff
{
enum ID : int
{
    ARCANE_BLAST = 30451,
    ARCANE_POWER = 12042,
    PRESENCE_OF_MIND = 12043,
    ARCANE_POTENCY = 57531,
    ICY_VEINS = 12472,
    COMBUSTION = 29977,
    EVOCATION = 12051,
    BLOODLUST = 2825,
    POWER_INFUSION = 10060,
    INNERVATE = 29166,
    CLEARCAST = 12536,
    MANA_TIDE = 39609,
    BERSERKING = 20554,
    MISSILE_BARRAGE = 44401,
    HOT_STREAK = 48108,
    FIRESTARTER = 54741,
    BRAIN_FREEZE = 57761,
    FINGERS_OF_FROST = 44544,
    GHOST_FINGERS = 4454411,
    FOCUS_MAGIC = 54648,
    FIRE_WARD = 43010,
    MANA_SHIELD = 43020,
    INCANTERS_ABSORPTION = 44413,
    HYPERSPEED_ACCELERATION = 54758,
    BLACK_MAGIC = 59626,
    LIGHTWEAVE = 55637,
    SPEED = 53908,
    WILD_MAGIC = 53909,
    MANA_SURGE = 37447,
    MANA_SURGES = 61062,
    PRAXIS = 64867,
    QUAD_CORE = 70747,
    PUSHING_THE_LIMIT = 70753,
    // Trinkets
    TWILIGHT_SERPENT = 56184,
    INFINITE_POWER = 61428,
    TOME_ARCANE_PHENOMENA = 60471,
    ARGENT_VALOR = 59657,
    MARK_WAR_PRISONER = 60480,
    FORGE_EMBER = 60479,
    NOW_IS_THE_TIME = 60064,
    EMBRACE_SPIDER = 60492,
    DYING_CURSE = 60494,
    ILLUSTRATION_DRAGON_SOUL = 60486,
    ALACRITY_ELEMENTS = 65004,
    EYE_BROODMOTHER = 65006,
    MEMORIES_LOVE = 65003,
    SCALE_FATES = 64707,
    FLARE_HEAVENS = 64713,
    SHOW_FAITH = 64739,
    LIVING_FLAME = 64712,
    PANDORAS_PLEA = 64741,
    VOLATILE_POWER_HC = 67744,
    VOLATILE_POWER_NM = 67736,
    VOLATILITY_HC = 67743,
    VOLATILITY_NM = 67735,
    ENERGIZED_HC = 67750,
    ENERGIZED_NM = 67696,
    REIGN_UNLIVING_HC = 67759,
    REIGN_UNLIVING_NM = 67713,
    MURADINS_SPYGLASS_HC = 71572,
    MURADINS_SPYGLASS_NM = 71570,
    DISLODGED_OBJECT_HC = 71644,
    DISLODGED_OBJECT_NM = 71601,
    NAMELESS_LICH_HC = 71636,
    NAMELESS_LICH_NM = 71605,
    // TBC Stuff
    FLAME_CAP = 28714,
    SKULL_GULDAN = 40396,
    SHRUNKEN_HEAD = 43712,
    NAARU_SLIVER = 45042,
    ASHTONGUE_TALISMAN = 40482,
    MQG = 23723,
    ARCANE_MADNESS = 37444,
};


// Prolonged ownership by Event and Unit
// TODO: Do Event and Unit really need the same copy (i.e. pointer)?
struct Buff
{
    ID id;
    std::string name;
    double duration;
    int stacks = 1;
    int max_stacks = 1;
    bool stack_refresh = true;
    bool show_refresh = false;
    bool hidden = false;
    bool snapshot = false;
    double cost = 0;
    double t_gained = 0;
    double t_refreshed = 0;
    Stats stats;

    int addStack();
};

class ArcaneBlast : public Buff
{

public:
    ArcaneBlast();

};

class ArcanePower : public Buff
{

public:
    ArcanePower(bool glyph = false);

};

class PresenceOfMind : public Buff
{

public:
    PresenceOfMind();

};

class ArcanePotency : public Buff
{

public:
    ArcanePotency();

};

class IcyVeins : public Buff
{

public:
    IcyVeins();

};

class Combustion : public Buff
{

public:
    Combustion();
};

class Evocation : public Buff
{

public:
    Evocation(double haste = 1, int ticks = 4);
};

class Clearcast : public Buff
{

public:
    Clearcast();
};

class Bloodlust : public Buff
{

public:
    Bloodlust();
};

class PowerInfusion : public Buff
{

public:
    PowerInfusion();
};

class Innervate : public Buff
{

public:
    Innervate();

};

class ManaTide : public Buff
{

public:
    ManaTide();
};

class Berserking : public Buff
{

public:
    Berserking();
};

class MissileBarrage : public Buff
{

public:
    MissileBarrage();
};

class HotStreak : public Buff
{

public:
    HotStreak();
};

class Firestarter : public Buff
{

public:
    Firestarter();
};

class BrainFreeze : public Buff
{

public:
    BrainFreeze();
};

class FingersOfFrost : public Buff
{

public:
    FingersOfFrost();
};

class GhostFingers : public Buff
{

public:
    GhostFingers();
};

class FocusMagic : public Buff
{

public:
    FocusMagic();
};

class FireWard : public Buff
{

public:
    FireWard();
};

class ManaShield : public Buff
{

public:
    ManaShield();
};

class IncantersAbsorption : public Buff
{

public:
    IncantersAbsorption(double sp);

};

class HyperspeedAcceleration : public Buff
{

public:
    HyperspeedAcceleration();
};

class BlackMagic : public Buff
{

public:
    BlackMagic();
};

class Lightweave : public Buff
{

public:
    Lightweave();
};

class Speed : public Buff
{

public:
    Speed();
};

class WildMagic : public Buff
{

public:
    WildMagic();
};

class ManaSurge : public Buff
{

public:
    ManaSurge();
};

class ManaSurges : public Buff
{

public:
    ManaSurges();
};

class Praxis : public Buff
{

public:
    Praxis();
};

class QuadCore : public Buff
{

public:
    QuadCore();
};

class PushingTheLimit : public Buff
{

public:
    PushingTheLimit();
};

class TwilightSerpent : public Buff
{

public:
    TwilightSerpent();
};

class InfinitePower : public Buff
{

public:
    InfinitePower();
};

class TomeArcanePhenomena : public Buff
{

public:
    TomeArcanePhenomena();
};

class ArgentValor : public Buff
{

public:
    ArgentValor();
};

class MarkWarPrisoner : public Buff
{

public:
    MarkWarPrisoner();
};

class ForgeEmber : public Buff
{

public:
    ForgeEmber();
};

class NowIsTheTime : public Buff
{

public:
    NowIsTheTime();
};

class EmbraceSpider : public Buff
{

public:
    EmbraceSpider();
};

class DyingCurse : public Buff
{

public:
    DyingCurse();
};

class IllustrationDragonSoul : public Buff
{

public:
    IllustrationDragonSoul();
};

class AlacrityElements : public Buff
{

public:
    AlacrityElements();
};

class EyeBroodmother : public Buff
{

public:
    EyeBroodmother();
};

class MemoriesLove : public Buff
{

public:
    MemoriesLove();
};

class ScaleFates : public Buff
{

public:
    ScaleFates();
};

class FlareHeavens : public Buff
{

public:
    FlareHeavens();
};

class ShowFaith : public Buff
{

public:
    ShowFaith();
};

class LivingFlame : public Buff
{

public:
    LivingFlame();
};

class PandorasPlea : public Buff
{

public:
    PandorasPlea();
};

class VolatilePowerHc : public Buff
{

public:
    VolatilePowerHc();
};

class VolatilePowerNm : public Buff
{

public:
    VolatilePowerNm();
};

class VolatilityHc : public Buff
{

public:
    VolatilityHc();
};

class VolatilityNm : public Buff
{

public:
    VolatilityNm();
};

class EnergizedHc : public Buff
{

public:
    EnergizedHc();
};

class EnergizedNm : public Buff
{

public:
    EnergizedNm();
};

class ReignUnlivingHc : public Buff
{

public:
    ReignUnlivingHc();
};

class ReignUnlivingNm : public Buff
{

public:
    ReignUnlivingNm();
};

class MuradinsSpyglassHc : public Buff
{

public:
    MuradinsSpyglassHc();
};

class MuradinsSpyglassNm : public Buff
{

public:
    MuradinsSpyglassNm();
};

class DislodgedObjectHc : public Buff
{

public:
    DislodgedObjectHc();
};

class DislodgedObjectNm : public Buff
{

public:
    DislodgedObjectNm();
};

class NamelessLichHc : public Buff
{

public:
    NamelessLichHc();
};

class NamelessLichNm : public Buff
{

public:
    NamelessLichNm();
};

class FlameCap : public Buff
{

public:
    FlameCap();
};

class SkullGuldan : public Buff
{

public:
    SkullGuldan();
};

class ShrunkenHead : public Buff
{

public:
    ShrunkenHead();
};

class NaaruSliver : public Buff
{

public:
    NaaruSliver();
};

class AshtongueTalisman : public Buff
{

public:
    AshtongueTalisman();
};

class MindQuickening : public Buff
{

public:
    MindQuickening();
};

class ArcaneMadness : public Buff
{

public:
    ArcaneMadness();
};

}