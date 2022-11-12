using namespace std;

namespace spell
{

    enum ID : int
    {
        ARCANE_BARRAGE = 44781,
        ARCANE_BLAST = 42897,
        ARCANE_EXPLOSION = 42921,
        ARCANE_MISSILES = 42846,
        BLIZZARD = 42940,
        FROSTBOLT = 42842,
        FIREBALL = 42833,
        FIREBALL_DOT = 4283300,
        FROSTFIRE_BOLT = 47610,
        FROSTFIRE_BOLT_DOT = 4761000,
        SCORCH = 42859,
        FIRE_BLAST = 42873,
        PYROBLAST = 42891,
        PYROBLAST_DOT = 4289100,
        LIVING_BOMB = 55360,
        LIVING_BOMB_EXPLOSION = 5536000,
        FLAMESTRIKE = 42926,
        FLAMESTRIKE_DOT = 4292600,
        BLAST_WAVE = 42945,
        DRAGONS_BREATH = 42950,
        ICE_LANCE = 42914,
        CONE_OF_COLD = 42931,
        DEEP_FREEZE = 44572,
        COLD_SNAP = 11958,
        FIRE_WARD = 43010,
        MANA_SHIELD = 43020,
        IGNITE = 12848,
        MANA_GEM = 42987,
        EVOCATION = 12051,
        SAPPER_CHARGE = 56488,
        PENDULUM_TELLURIC_CURRENTS = 60483,
        DARKMOON_DEATH = 60203,
        EXTRACT_NECROMANTIC_POWER = 60488,
        PILLAR_OF_FLAME_HC = 67760,
        PILLAR_OF_FLAME_NM = 67714,

        MIRROR_IMAGE = 55342,
        MIRROR_FIRE_BLAST = 59637,
        MIRROR_FROSTBOLT = 59638,

        WATER_ELEMENTAL = 31687,
        WATERBOLT = 31707,
    };

    enum Result : int
    {
        HIT,
        CRIT,
        MISS
    };

    class Spell
    {

    public:
        ID id;
        string name;
        double cost = 0;
        double min_dmg = 0;
        double max_dmg = 0;
        double cast_time = 0;
        double delay = 0;
        double coeff = 1;
        double gcd = 1.5;
        double travel_time_factor = 1;
        bool channeling = false;
        bool dot = false;
        bool overlap = false;
        bool proc = false;
        bool binary = false;
        bool aoe = false;
        bool aoe_capped = true;
        bool has_travel_time = false;
        bool fixed_dmg = false;
        bool active_use = true;
        bool off_gcd = false;
        bool is_trigger = false;
        bool can_proc = true;
        int ticks = 0;
        int t_interval = 1;
        School school;

        double actual_cost = 0;
        double actual_cast_time = 0;
        int tick = 0;
        bool done = false;

        double avgDmg()
        {
            return (min_dmg + max_dmg)/2.0;
        }

        shared_ptr<Spell> clone() const
        {
            return make_shared<Spell>(*this);
        }

    };

    class SpellInstance
    {

    public:
        shared_ptr<Spell> spell;
        Result result;
        double dmg = 0;
        double resist = 0;
        int tick = 0;

        SpellInstance(shared_ptr<Spell> _spell)
        {
            spell = _spell;
        }
    };


    class ArcaneBarrage : public Spell
    {

    public:
        ArcaneBarrage()
        {
            id = ARCANE_BARRAGE;
            name = "Arcane Barrage";
            cost = 18;
            min_dmg = 936;
            max_dmg = 1144;
            cast_time = 0;
            coeff = 2.5/3.5;
            school = SCHOOL_ARCANE;
            has_travel_time = true;
        }

    };


    class ArcaneBlast : public Spell
    {

    public:
        ArcaneBlast()
        {
            id = ARCANE_BLAST;
            name = "Arcane Blast";
            cost = 7;
            min_dmg = 1185;
            max_dmg = 1377;
            cast_time = 2.5;
            coeff = 2.5/3.5;
            school = SCHOOL_ARCANE;
        }

    };

    class ArcaneExplosion : public Spell
    {

    public:
        ArcaneExplosion()
        {
            id = ARCANE_EXPLOSION;
            name = "Arcane Explosion";
            cost = 22;
            min_dmg = 538;
            max_dmg = 582;
            cast_time = 0;
            coeff = 1.5/3.5/2;
            school = SCHOOL_ARCANE;
            aoe = true;
        }

    };

    class ArcaneMissiles : public Spell
    {

    public:
        ArcaneMissiles()
        {
            id = ARCANE_MISSILES;
            name = "Arcane Missiles";
            cost = 31;
            min_dmg = 362;
            max_dmg = 362;
            cast_time = 5;
            coeff = 5.0/3.5;
            school = SCHOOL_ARCANE;
            channeling = true;
            ticks = 5;
            has_travel_time = true;
            travel_time_factor = 0.5;
        }

    };

    class Blizzard : public Spell
    {

    public:
        Blizzard()
        {
            id = BLIZZARD;
            name = "Blizzard";
            cost = 74;
            min_dmg = 426;
            max_dmg = 426;
            cast_time = 8;
            coeff = 4.0/3.5;
            school = SCHOOL_FROST;
            channeling = true;
            ticks = 8;
        }

    };

    class Frostbolt : public Spell
    {

    public:
        Frostbolt()
        {
            id = FROSTBOLT;
            name = "Frostbolt";
            cost = 11;
            min_dmg = 804;
            max_dmg = 866;
            cast_time = 3;
            coeff = 3/3.5;
            school = SCHOOL_FROST;
            has_travel_time = true;
        }

    };

    class Fireball : public Spell
    {

    public:
        Fireball()
        {
            id = FIREBALL;
            name = "Fireball";
            cost = 19;
            min_dmg = 898;
            max_dmg = 1143;
            cast_time = 3.5;
            coeff = 1;
            school = SCHOOL_FIRE;
            has_travel_time = true;
        }

    };

    class FireballDot : public Spell
    {

    public:
        FireballDot()
        {
            id = FIREBALL_DOT;
            name = "Fireball";
            dot = true;
            active_use = false;
            coeff = 0;
            t_interval = 2;
            ticks = 4;
            min_dmg = 29;
            max_dmg = 29;
            school = SCHOOL_FIRE;
        }

    };

    class FrostfireBolt : public Spell
    {

    public:
        FrostfireBolt()
        {
            id = FROSTFIRE_BOLT;
            name = "Frostfire Bolt";
            cost = 14;
            min_dmg = 722;
            max_dmg = 838;
            cast_time = 3;
            coeff = 3/3.5;
            school = SCHOOL_FROSTFIRE;
            has_travel_time = true;
            travel_time_factor = 0.85;
        }

    };

    class FrostfireBoltDot : public Spell
    {

    public:
        FrostfireBoltDot()
        {
            id = FROSTFIRE_BOLT_DOT;
            name = "Frostfire Bolt";
            dot = true;
            active_use = false;
            t_interval = 3;
            ticks = 3;
            coeff = 0;
            min_dmg = 30;
            max_dmg = 30;
            school = SCHOOL_FROSTFIRE;
        }

    };

    class Scorch : public Spell
    {

    public:
        Scorch()
        {
            id = SCORCH;
            name = "Scorch";
            cost = 8;
            min_dmg = 382;
            max_dmg = 451;
            cast_time = 1.5;
            coeff = 1.5/3.5;
            school = SCHOOL_FIRE;
        }

    };

    class FireBlast : public Spell
    {

    public:
        FireBlast()
        {
            id = FIRE_BLAST;
            name = "Fire Blast";
            cost = 21;
            min_dmg = 925;
            max_dmg = 1095;
            cast_time = 0;
            coeff = 1.5/3.5;
            school = SCHOOL_FIRE;
        }

    };

    class IceLance : public Spell
    {

    public:
        IceLance()
        {
            id = ICE_LANCE;
            name = "Ice Lance";
            cost = 6;
            min_dmg = 224;
            max_dmg = 258;
            cast_time = 0;
            coeff = 1.5/3.5/3.0;
            has_travel_time = true;
            school = SCHOOL_FROST;
        }

    };

    class Pyroblast : public Spell
    {

    public:
        Pyroblast()
        {
            id = PYROBLAST;
            name = "Pyroblast";
            cost = 22;
            min_dmg = 1210;
            max_dmg = 1531;
            cast_time = 5;
            coeff = 1.15;
            school = SCHOOL_FIRE;
            has_travel_time = true;
        }

    };

    class PyroblastDot : public Spell
    {

    public:
        PyroblastDot()
        {
            id = PYROBLAST_DOT;
            name = "Pyroblast";
            dot = true;
            active_use = false;
            t_interval = 3;
            ticks = 4;
            min_dmg = 113;
            max_dmg = 113;
            coeff = 0.05;
            school = SCHOOL_FIRE;
        }

    };

    class Flamestrike : public Spell
    {

    public:
        Flamestrike()
        {
            id = FLAMESTRIKE;
            name = "Flamestrike";
            aoe = true;
            cost = 30;
            min_dmg = 876;
            max_dmg = 1071;
            cast_time = 2;
            coeff = 0.2357;
            school = SCHOOL_FIRE;
        }

    };

    class FlamestrikeDot : public Spell
    {

    public:
        FlamestrikeDot()
        {
            id = FLAMESTRIKE_DOT;
            name = "Flamestrike";
            aoe = true;
            aoe_capped = false;
            overlap = true;
            dot = true;
            active_use = false;
            t_interval = 2;
            ticks = 4;
            min_dmg = 195;
            max_dmg = 195;
            coeff = 0.122;
            school = SCHOOL_FIRE;
        }

    };

    class LivingBomb : public Spell
    {

    public:
        LivingBomb()
        {
            id = LIVING_BOMB;
            name = "Living Bomb";
            cost = 22;
            dot = true;
            t_interval = 3;
            ticks = 4;
            min_dmg = 345;
            max_dmg = 345;
            coeff = 0.2;
            school = SCHOOL_FIRE;
        }

    };

    class LivingBombExplosion : public Spell
    {

    public:
        LivingBombExplosion()
        {
            id = LIVING_BOMB_EXPLOSION;
            name = "Living Bomb";
            aoe = true;
            active_use = false;
            min_dmg = 690;
            max_dmg = 690;
            coeff = 0.4286;
            school = SCHOOL_FIRE;
        }

    };

    class BlastWave : public Spell
    {

    public:
        BlastWave()
        {
            id = BLAST_WAVE;
            name = "Blast Wave";
            aoe = true;
            cost = 7;
            min_dmg = 1047;
            max_dmg = 1233;
            cast_time = 0;
            coeff = 0.1936;
            school = SCHOOL_FIRE;
        }

    };

    class DragonsBreath : public Spell
    {

    public:
        DragonsBreath()
        {
            id = DRAGONS_BREATH;
            name = "Dragon's Breath";
            aoe = true;
            cost = 7;
            min_dmg = 1101;
            max_dmg = 1279;
            cast_time = 0;
            coeff = 0.1936;
            school = SCHOOL_FIRE;
        }

    };

    class ConeOfCold : public Spell
    {

    public:
        ConeOfCold()
        {
            id = CONE_OF_COLD;
            name = "Cone of Cold";
            aoe = true;
            cost = 25;
            min_dmg = 710;
            max_dmg = 776;
            cast_time = 0;
            coeff = 0.214;
            school = SCHOOL_FROST;
        }

    };

    class DeepFreeze : public Spell
    {

    public:
        DeepFreeze()
        {
            id = DEEP_FREEZE;
            name = "Deep Freeze";
            cost = 9;
            min_dmg = 2369;
            max_dmg = 2641;
            cast_time = 0;
            coeff = 7.5/3.5;
            school = SCHOOL_FROST;
        }

    };

    class ColdSnap : public Spell
    {

    public:
        ColdSnap()
        {
            id = COLD_SNAP;
            name = "Cold Snap";
            cost = 0;
            is_trigger = true;
            gcd = 0;
            school = SCHOOL_FROST;
        }

    };

    class FireWard : public Spell
    {

    public:
        FireWard()
        {
            id = FIRE_WARD;
            name = "Fire Ward";
            cost = 16;
            is_trigger = true;
            can_proc = false;
            school = SCHOOL_FIRE;
        }

    };

    class ManaShield : public Spell
    {

    public:
        ManaShield()
        {
            id = MANA_SHIELD;
            name = "Mana Shield";
            cost = 7;
            is_trigger = true;
            can_proc = false;
            school = SCHOOL_ARCANE;
        }

    };

    class Ignite : public Spell
    {

    public:
        double dmg2;
        double dmg3;
        double dmg4;

        Ignite(double _dmg)
        {
            id = IGNITE;
            name = "Ignite";
            dot = true;
            active_use = false;
            t_interval = 2;
            ticks = 2;
            coeff = 0;
            min_dmg = max_dmg = _dmg;
            fixed_dmg = true;
            school = SCHOOL_FIRE;
        }

    };

    class ManaGem : public Spell
    {

    public:
        ManaGem()
        {
            id = MANA_GEM;
            name = "Mana Gem";
            is_trigger = true;
            gcd = 0;
            school = SCHOOL_ARCANE;
        }

    };

    class Evocation : public Spell
    {

    public:
        Evocation(int _ticks = 4)
        {
            id = EVOCATION;
            name = "Evocation";
            is_trigger = true;
            school = SCHOOL_ARCANE;
            channeling = true;
            ticks = _ticks;
            cast_time = _ticks*2;
        }

    };

    class SapperCharge : public Spell
    {

    public:
        SapperCharge()
        {
            id = SAPPER_CHARGE;
            name = "Global Thermal Sapper Charge";
            school = SCHOOL_FIRE;
            min_dmg = 2188;
            max_dmg = 2812;
            coeff = 0;
            gcd = 0;
            off_gcd = true;
        }

    };

    class PendulumTelluricCurrents : public Spell
    {

    public:
        PendulumTelluricCurrents()
        {
            id = PENDULUM_TELLURIC_CURRENTS;
            name = "Pendulum of Telluric Currents";
            school = SCHOOL_SHADOW;
            proc = true;
            min_dmg = 1168;
            max_dmg = 1752;
            coeff = 0;
            gcd = 0;
            active_use = false;
        }
    };

    class DarkmoonDeath : public Spell
    {

    public:
        DarkmoonDeath()
        {
            id = DARKMOON_DEATH;
            name = "Darkmoon Card: Death";
            school = SCHOOL_SHADOW;
            proc = true;
            min_dmg = 1750;
            max_dmg = 2250;
            coeff = 0;
            gcd = 0;
            active_use = false;
            has_travel_time = true;
        }
    };

    class ExtractNecromanticPower : public Spell
    {

    public:
        ExtractNecromanticPower()
        {
            id = EXTRACT_NECROMANTIC_POWER;
            name = "Extract of Necromatic Power";
            school = SCHOOL_SHADOW;
            proc = true;
            min_dmg = 788;
            max_dmg = 1312;
            coeff = 0;
            gcd = 0;
            active_use = false;
            has_travel_time = true;
        }
    };

    class PillarOfFlameHc : public Spell
    {

    public:
        PillarOfFlameHc()
        {
            id = PILLAR_OF_FLAME_HC;
            name = "Pillar of Flame";
            school = SCHOOL_FIRE;
            proc = true;
            min_dmg = 1959;
            max_dmg = 2275;
            coeff = 0;
            gcd = 0;
            active_use = false;
            has_travel_time = true;
        }
    };

    class PillarOfFlameNm : public Spell
    {

    public:
        PillarOfFlameNm()
        {
            id = PILLAR_OF_FLAME_NM;
            name = "Pillar of Flame";
            school = SCHOOL_FIRE;
            proc = true;
            min_dmg = 1741;
            max_dmg = 2023;
            coeff = 0;
            gcd = 0;
            active_use = false;
            has_travel_time = true;
        }
    };


    class MirrorImage : public Spell
    {

    public:
        MirrorImage()
        {
            id = MIRROR_IMAGE;
            name = "Mirror Image";
            cost = 10;
            is_trigger = true;
            school = SCHOOL_ARCANE;
        }

    };

    class MirrorFrostbolt : public Spell
    {

    public:
        MirrorFrostbolt()
        {
            id = MIRROR_FROSTBOLT;
            name = "Frostbolt";
            cost = 0;
            min_dmg = 163;
            max_dmg = 169;
            cast_time = 3;
            coeff = 0.3;
            school = SCHOOL_FROST;
            has_travel_time = true;
        }

    };

    class MirrorFireBlast : public Spell
    {

    public:
        MirrorFireBlast()
        {
            id = MIRROR_FIRE_BLAST;
            name = "Fire Blast";
            cost = 0;
            min_dmg = 88;
            max_dmg = 98;
            cast_time = 0;
            coeff = 0.15;
            school = SCHOOL_FIRE;
        }

    };


    class WaterElemental : public Spell
    {

    public:
        WaterElemental()
        {
            id = WATER_ELEMENTAL;
            name = "Water Elemental";
            cost = 16;
            is_trigger = true;
            school = SCHOOL_FROST;
        }

    };

    class Waterbolt : public Spell
    {

    public:
        Waterbolt()
        {
            id = WATERBOLT;
            name = "Waterbolt";
            cost = 1;
            min_dmg = 601;
            max_dmg = 673;
            cast_time = 2.5;
            coeff = 2.5/3.0;
            school = SCHOOL_FROST;
        }

    };

}