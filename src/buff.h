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
        ASHEN_BAND = 72416,
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


    class Buff
    {

    public:
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

        int addStack()
        {
            if (stacks < max_stacks)
                stacks++;
            else
                return 0;

            return stacks;
        }

    };

    class ArcaneBlast : public Buff
    {

    public:
        ArcaneBlast()
        {
            id = ARCANE_BLAST;
            name = "Arcane Blast";
            duration = 8;
            max_stacks = 4;
            snapshot = true;
        }

    };

    class ArcanePower : public Buff
    {

    public:
        ArcanePower(bool glyph = false)
        {
            id = ARCANE_POWER;
            name = "Arcane Power";
            duration = 15;
            if (glyph)
                duration+= 3;
        }

    };

    class PresenceOfMind : public Buff
    {

    public:
        PresenceOfMind()
        {
            id = PRESENCE_OF_MIND;
            name = "Presence of Mind";
            duration = 100;
            snapshot = true;
        }

    };

    class ArcanePotency : public Buff
    {

    public:
        ArcanePotency()
        {
            id = ARCANE_POTENCY;
            name = "Arcane Potency";
            duration = 1000; // unlimited
        }

    };

    class IcyVeins : public Buff
    {

    public:
        IcyVeins()
        {
            id = ICY_VEINS;
            name = "Icy Veins";
            duration = 20;
            cost = 3;
        }

    };

    class Combustion : public Buff
    {

    public:
        Combustion()
        {
            id = COMBUSTION;
            name = "Combustion";
            duration = 2000;
            max_stacks = 20;
        }

    };

    class Evocation : public Buff
    {

    public:
        Evocation(double haste = 1, int ticks = 4)
        {
            id = EVOCATION;
            name = "Evocation";
            duration = 2.0 * ticks * haste;
        }

    };

    class Clearcast : public Buff
    {

    public:
        Clearcast()
        {
            id = CLEARCAST;
            name = "Clearcast";
            duration = 15;
            snapshot = true;
        }

    };

    class Bloodlust : public Buff
    {

    public:
        Bloodlust()
        {
            id = BLOODLUST;
            name = "Bloodlust";
            duration = 40;
        }

    };

    class PowerInfusion : public Buff
    {

    public:
        PowerInfusion()
        {
            id = POWER_INFUSION;
            name = "Power Infusion";
            duration = 15;
        }

    };

    class Innervate : public Buff
    {

    public:
        Innervate()
        {
            id = INNERVATE;
            name = "Innervate";
            duration = 20;
        }

    };

    class ManaTide : public Buff
    {

    public:
        ManaTide()
        {
            id = MANA_TIDE;
            name = "Mana Tide";
            duration = 12;
        }

    };

    class Berserking : public Buff
    {

    public:
        Berserking()
        {
            id = BERSERKING;
            name = "Berserking";
            duration = 10;
        }

    };

    class MissileBarrage : public Buff
    {

    public:
        MissileBarrage()
        {
            id = MISSILE_BARRAGE;
            name = "Missile Barrage";
            duration = 15;
        }

    };

    class HotStreak : public Buff
    {

    public:
        HotStreak()
        {
            id = HOT_STREAK;
            name = "Hot Streak";
            duration = 10;
        }

    };

    class Firestarter : public Buff
    {

    public:
        Firestarter()
        {
            id = FIRESTARTER;
            name = "Firestarter";
            duration = 10;
        }

    };

    class BrainFreeze : public Buff
    {

    public:
        BrainFreeze()
        {
            id = BRAIN_FREEZE;
            name = "Brain Freeze";
            duration = 15;
        }

    };

    class FingersOfFrost : public Buff
    {

    public:
        FingersOfFrost()
        {
            id = FINGERS_OF_FROST;
            name = "Fingers of Frost";
            duration = 15;
        }

    };

    class GhostFingers : public Buff
    {

    public:
        GhostFingers()
        {
            id = GHOST_FINGERS;
            name = "Fingers of Frost (Ghost proc)";
            duration = 0.01;
            hidden = true;
        }

    };

    class FocusMagic : public Buff
    {

    public:
        FocusMagic()
        {
            id = FOCUS_MAGIC;
            name = "Focus Magic";
            duration = 6000; // This is actually 10 sec but assume it gets refreshed
            stats.crit = 3;
        }

    };

    class FireWard : public Buff
    {

    public:
        FireWard()
        {
            id = FIRE_WARD;
            name = "Fire Ward";
            duration = 30;
        }

    };

    class ManaShield : public Buff
    {

    public:
        ManaShield()
        {
            id = MANA_SHIELD;
            name = "Mana Shield";
            duration = 60;
        }

    };

    class IncantersAbsorption : public Buff
    {

    public:
        IncantersAbsorption(double sp)
        {
            id = INCANTERS_ABSORPTION;
            duration = 10;
            stats.spell_power = sp;
            show_refresh = true;

            std::ostringstream s;
            s << std::fixed << std::setprecision(0);
            s << "Incanter's Absorption (" << sp << ")";
            name = s.str();
        }

    };

    class HyperspeedAcceleration : public Buff
    {

    public:
        HyperspeedAcceleration()
        {
            id = HYPERSPEED_ACCELERATION;
            name = "Hyperspeed Acceleration";
            duration = 12;
            stats.haste_rating = 340;
        }

    };

    class BlackMagic : public Buff
    {

    public:
        BlackMagic()
        {
            id = BLACK_MAGIC;
            name = "Black Magic";
            duration = 10;
            stats.haste_rating = 250;
        }

    };

    class Lightweave : public Buff
    {

    public:
        Lightweave()
        {
            id = LIGHTWEAVE;
            name = "Lightweave";
            duration = 15;
            stats.spell_power = 295;
        }

    };

    class Speed : public Buff
    {

    public:
        Speed()
        {
            id = SPEED;
            name = "Speed";
            duration = 15;
            stats.haste_rating = 500;
        }

    };

    class WildMagic : public Buff
    {

    public:
        WildMagic()
        {
            id = WILD_MAGIC;
            name = "Wild Magic";
            duration = 15;
            stats.spell_power = 200;
            stats.crit = critRatingToChance(200);
        }

    };

    class ManaSurge : public Buff
    {

    public:
        ManaSurge()
        {
            id = MANA_SURGE;
            name = "Mana Surge";
            duration = 15;
            stats.spell_power = 225;
        }

    };

    class ManaSurges : public Buff
    {

    public:
        ManaSurges()
        {
            id = MANA_SURGES;
            name = "Mana Surges";
            duration = 15;
            stats.spell_power = 225;
        }

    };

    class Praxis : public Buff
    {

    public:
        Praxis()
        {
            id = PRAXIS;
            name = "Praxis";
            duration = 15;
            stats.spell_power = 350;
        }

    };

    class QuadCore : public Buff
    {

    public:
        QuadCore()
        {
            id = QUAD_CORE;
            name = "Quad Core";
            duration = 30;
        }

    };

    class PushingTheLimit : public Buff
    {

    public:
        PushingTheLimit()
        {
            id = PUSHING_THE_LIMIT;
            name = "Pushing the Limit";
            duration = 5;
        }

    };

    class AshenBand : public Buff
    {

    public:
        AshenBand()
        {
            id = ASHEN_BAND;
            name = "Frostforged Sage";
            duration = 10;
            stats.spell_power = 285;
        }

    };

    class TwilightSerpent : public Buff
    {

    public:
        TwilightSerpent()
        {
            id = TWILIGHT_SERPENT;
            name = "Twilight Serpent";
            duration = 20;
            stats.spell_power = 292;
        }

    };

    class InfinitePower : public Buff
    {

    public:
        InfinitePower()
        {
            id = INFINITE_POWER;
            name = "Infinite Power";
            duration = 20;
            stats.spell_power = 202;
        }

    };

    class TomeArcanePhenomena : public Buff
    {

    public:
        TomeArcanePhenomena()
        {
            id = TOME_ARCANE_PHENOMENA;
            name = "Tome of Arcane Phenomena";
            duration = 20;
            stats.haste_rating = 256;
        }

    };

    class ArgentValor : public Buff
    {

    public:
        ArgentValor()
        {
            id = ARGENT_VALOR;
            name = "Argent Valor";
            duration = 20;
            stats.spell_power = 281;
        }

    };

    class MarkWarPrisoner : public Buff
    {

    public:
        MarkWarPrisoner()
        {
            id = MARK_WAR_PRISONER;
            name = "Mark of the War Prisoner";
            duration = 20;
            stats.spell_power = 346;
        }

    };

    class ForgeEmber : public Buff
    {

    public:
        ForgeEmber()
        {
            id = FORGE_EMBER;
            name = "Forge Ember";
            duration = 10;
            stats.spell_power = 512;
        }

    };

    class NowIsTheTime : public Buff
    {

    public:
        NowIsTheTime()
        {
            id = NOW_IS_THE_TIME;
            name = "Now is the Time!";
            duration = 10;
            stats.spell_power = 590;
        }

    };

    class EmbraceSpider : public Buff
    {

    public:
        EmbraceSpider()
        {
            id = EMBRACE_SPIDER;
            name = "Embrace of the Spider";
            duration = 10;
            stats.haste_rating = 505;
        }

    };

    class DyingCurse : public Buff
    {

    public:
        DyingCurse()
        {
            id = DYING_CURSE;
            name = "Dying Curse";
            duration = 10;
            stats.spell_power = 765;
        }

    };

    class IllustrationDragonSoul : public Buff
    {

    public:
        IllustrationDragonSoul()
        {
            id = ILLUSTRATION_DRAGON_SOUL;
            name = "Illustration of the Dragon Soul";
            duration = 10;
            stats.spell_power = 20;
            max_stacks = 10;
        }

    };

    class AlacrityElements : public Buff
    {

    public:
        AlacrityElements()
        {
            id = ALACRITY_ELEMENTS;
            name = "Alacrity of the Elements";
            duration = 10;
            stats.haste_rating = 552;
        }

    };

    class EyeBroodmother : public Buff
    {

    public:
        EyeBroodmother()
        {
            id = EYE_BROODMOTHER;
            name = "Eye of the Broodmother";
            duration = 10;
            max_stacks = 5;
            stats.spell_power = 26;
        }

    };

    class MemoriesLove : public Buff
    {

    public:
        MemoriesLove()
        {
            id = MEMORIES_LOVE;
            name = "Memories of Love";
            duration = 15;
            stats.mp5 = 220;
        }

    };

    class ScaleFates : public Buff
    {

    public:
        ScaleFates()
        {
            id = SCALE_FATES;
            name = "Scale of Fates";
            duration = 20;
            stats.haste_rating = 457;
        }

    };

    class FlareHeavens : public Buff
    {

    public:
        FlareHeavens()
        {
            id = FLARE_HEAVENS;
            name = "Flame of the Heavens";
            duration = 10;
            stats.spell_power = 959;
        }

    };

    class ShowFaith : public Buff
    {

    public:
        ShowFaith()
        {
            id = SHOW_FAITH;
            name = "Show of Faith";
            duration = 15;
            stats.mp5 = 272;
        }

    };

    class LivingFlame : public Buff
    {

    public:
        LivingFlame()
        {
            id = LIVING_FLAME;
            name = "Living Flame";
            duration = 20;
            stats.spell_power = 534;
        }

    };

    class PandorasPlea : public Buff
    {

    public:
        PandorasPlea()
        {
            id = PANDORAS_PLEA;
            name = "Pandora's Plea";
            duration = 10;
            stats.spell_power = 794;
        }

    };

    class VolatilePowerHc : public Buff
    {

    public:
        VolatilePowerHc()
        {
            id = VOLATILE_POWER_HC;
            name = "Volatile Power";
            duration = 20;
        }

    };

    class VolatilePowerNm : public Buff
    {

    public:
        VolatilePowerNm()
        {
            id = VOLATILE_POWER_NM;
            name = "Volatile Power";
            duration = 20;
        }

    };

    class VolatilityHc : public Buff
    {

    public:
        VolatilityHc()
        {
            id = VOLATILITY_HC;
            name = "Volatility";
            duration = 20;
            max_stacks = 8;
            stats.haste_rating = 64;
        }

    };

    class VolatilityNm : public Buff
    {

    public:
        VolatilityNm()
        {
            id = VOLATILITY_NM;
            name = "Volatility";
            duration = 20;
            max_stacks = 8;
            stats.haste_rating = 57;
        }

    };

    class EnergizedHc : public Buff
    {

    public:
        EnergizedHc()
        {
            id = ENERGIZED_HC;
            name = "Energized";
            duration = 10;
            max_stacks = 8;
            stats.mp5 = 18;
        }

    };

    class EnergizedNm : public Buff
    {

    public:
        EnergizedNm()
        {
            id = ENERGIZED_NM;
            name = "Energized";
            duration = 10;
            max_stacks = 8;
            stats.mp5 = 16;
        }

    };

    class ReignUnlivingHc : public Buff
    {

    public:
        ReignUnlivingHc()
        {
            id = REIGN_UNLIVING_HC;
            name = "Shard of Flame";
            duration = 10000;
            max_stacks = 3;
        }

    };

    class ReignUnlivingNm : public Buff
    {

    public:
        ReignUnlivingNm()
        {
            id = REIGN_UNLIVING_NM;
            name = "Mote of Flame";
            duration = 10000;
            max_stacks = 3;
        }

    };

    class MuradinsSpyglassHc : public Buff
    {

    public:
        MuradinsSpyglassHc()
        {
            id = MURADINS_SPYGLASS_HC;
            name = "Cultivated Power";
            duration = 10;
            max_stacks = 10;
            stats.spell_power = 20;
        }

    };

    class MuradinsSpyglassNm : public Buff
    {

    public:
        MuradinsSpyglassNm()
        {
            id = MURADINS_SPYGLASS_NM;
            name = "Cultivated Power";
            duration = 10;
            max_stacks = 10;
            stats.spell_power = 18;
        }

    };

    class DislodgedObjectHc : public Buff
    {

    public:
        DislodgedObjectHc()
        {
            id = DISLODGED_OBJECT_HC;
            name = "Surge of Power";
            duration = 20;
            max_stacks = 10;
            stack_refresh = false;
            stats.spell_power = 121;
        }

    };

    class DislodgedObjectNm : public Buff
    {

    public:
        DislodgedObjectNm()
        {
            id = DISLODGED_OBJECT_NM;
            name = "Surge of Power";
            duration = 20;
            max_stacks = 10;
            stack_refresh = false;
            stats.spell_power = 105;
        }

    };

    class NamelessLichHc : public Buff
    {

    public:
        NamelessLichHc()
        {
            id = NAMELESS_LICH_HC;
            name = "Siphoned Power";
            duration = 20;
            stats.spell_power = 1207;
        }

    };

    class NamelessLichNm : public Buff
    {

    public:
        NamelessLichNm()
        {
            id = NAMELESS_LICH_NM;
            name = "Siphoned Power";
            duration = 20;
            stats.spell_power = 1074;
        }

    };

    class FlameCap : public Buff
    {

    public:
        FlameCap()
        {
            id = FLAME_CAP;
            name = "Flame Cap";
            duration = 60;
        }

    };

    class SkullGuldan : public Buff
    {

    public:
        SkullGuldan()
        {
            id = SKULL_GULDAN;
            name = "Fel Infusion";
            duration = 20;
            stats.haste_rating = 175;
        }

    };

    class ShrunkenHead : public Buff
    {

    public:
        ShrunkenHead()
        {
            id = SHRUNKEN_HEAD;
            name = "Mojo Madness";
            duration = 20;
            stats.spell_power = 211;
        }

    };

    class NaaruSliver : public Buff
    {

    public:
        NaaruSliver()
        {
            id = NAARU_SLIVER;
            name = "Power Circle";
            duration = 15;
            stats.spell_power = 210;
        }

    };

    class AshtongueTalisman : public Buff
    {

    public:
        AshtongueTalisman()
        {
            id = ASHTONGUE_TALISMAN;
            name = "Insight of the Ashtongue";
            duration = 5;
            stats.haste_rating = 145;
        }

    };

    class MindQuickening : public Buff
    {

    public:
        MindQuickening()
        {
            id = MQG;
            name = "MQG";
            duration = 20;
            stats.haste_rating = 330;
        }

    };

    class ArcaneMadness : public Buff
    {

    public:
        ArcaneMadness()
        {
            id = ARCANE_MADNESS;
            name = "Arcane Madness";
            duration = 6;
            stats.spell_power = 70;
        }

    };

}