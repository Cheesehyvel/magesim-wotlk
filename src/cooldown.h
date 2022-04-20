namespace cooldown
{

    enum ID : int
    {
        NONE = 0,
        POTION = 1,
        CONJURED = 2,
        TRINKET1 = 3,
        TRINKET2 = 4,
        TRINKET_SHARED = 5,
        EVOCATION = 12051,
        COLD_SNAP = 11958,
        WATER_ELEMENTAL = 31687,
        DEEP_FREEZE = 44572,
        BERSERKING = 20554,
        ICY_VEINS = 12472,
        FIRE_BLAST = 42873,
        BLAST_WAVE = 42945,
        DRAGONS_BREATH = 42950,
        CONE_OF_COLD = 42931,
        ARCANE_BARRAGE = 44781,
        COMBUSTION = 29977,
        ARCANE_POWER = 12042,
        PRESENCE_OF_MIND = 12043,
        MIRROR_IMAGE = 55342,
        POWER_INFUSION = 10060,
        DRUMS = 35476,
        BLACK_MAGIC = 59626,
        LIGHTWEAVE = 55637,
        DARKGLOW = 55767,
        HYPERSPEED_ACCELERATION = 54758,
        MANA_GEM = 42987,
        PRAXIS = 64867,
    };


    class Cooldown
    {

    public:
        ID id;
        double duration;

        Cooldown(ID _id = NONE, double _duration = 0)
        {
            if (_id)
                id = _id;
            if (_duration > 0)
                duration = _duration;
        }

    };

    class TrinketShared : public Cooldown
    {

    public:
        TrinketShared(double _duration = 20)
        {
            id = TRINKET_SHARED;
            duration = _duration;
        }

    };

    class Potion : public Cooldown
    {

    public:
        Potion(bool in_combat)
        {
            id = POTION;
            duration = in_combat ? 100000 : 120;
        }

    };

    class Conjured : public Cooldown
    {

    public:
        Conjured(double _duration = 120)
        {
            id = CONJURED;
            duration = _duration;
        }

    };

    class IcyVeins : public Cooldown
    {

    public:
        IcyVeins()
        {
            id = ICY_VEINS;
            duration = 180;
        }

    };

    class ArcanePower : public Cooldown
    {

    public:
        ArcanePower()
        {
            id = ARCANE_POWER;
            duration = 120;
        }

    };

    class PresenceOfMind : public Cooldown
    {

    public:
        PresenceOfMind()
        {
            id = PRESENCE_OF_MIND;
            duration = 180;
        }

    };

    class MirrorImage : public Cooldown
    {

    public:
        MirrorImage()
        {
            id = MIRROR_IMAGE;
            duration = 180;
        }

    };

    class FireBlast : public Cooldown
    {

    public:
        FireBlast(int imp_fire_blast = 0)
        {
            id = FIRE_BLAST;
            duration = 8 - imp_fire_blast * 1.0;
        }

    };

    class ArcaneBarrage : public Cooldown
    {

    public:
        ArcaneBarrage()
        {
            id = ARCANE_BARRAGE;
            duration = 3;
        }

    };

    class ConeOfCold : public Cooldown
    {

    public:
        ConeOfCold()
        {
            id = CONE_OF_COLD;
            duration = 10;
        }

    };

    class DragonsBreath : public Cooldown
    {

    public:
        DragonsBreath()
        {
            id = DRAGONS_BREATH;
            duration = 20;
        }

    };

    class BlastWave : public Cooldown
    {

    public:
        BlastWave()
        {
            id = BLAST_WAVE;
            duration = 30;
        }

    };

    class Combustion : public Cooldown
    {

    public:
        Combustion()
        {
            id = COMBUSTION;
            duration = 180;
        }

    };

    class Berserking : public Cooldown
    {

    public:
        Berserking()
        {
            id = BERSERKING;
            duration = 180;
        }

    };

    class ColdSnap : public Cooldown
    {

    public:
        ColdSnap()
        {
            id = COLD_SNAP;
            duration = 480;
        }

    };

    class WaterElemental : public Cooldown
    {

    public:
        WaterElemental()
        {
            id = WATER_ELEMENTAL;
            duration = 180;
        }

    };

    class DeepFreeze : public Cooldown
    {

    public:
        DeepFreeze()
        {
            id = DEEP_FREEZE;
            duration = 30;
        }

    };

    class Evocation : public Cooldown
    {

    public:
        Evocation()
        {
            id = EVOCATION;
            duration = 480;
        }

    };

    class PowerInfusion : public Cooldown
    {

    public:
        PowerInfusion()
        {
            id = POWER_INFUSION;
            duration = 180;
        }

    };

    class Drums : public Cooldown
    {

    public:
        Drums()
        {
            id = DRUMS;
            duration = 120;
        }

    };

    class BlackMagic : public Cooldown
    {

    public:
        BlackMagic()
        {
            id = BLACK_MAGIC;
            duration = 35;
        }

    };

    class Lightweave : public Cooldown
    {

    public:
        Lightweave()
        {
            id = LIGHTWEAVE;
            duration = 45;
        }

    };

    class Darkglow : public Cooldown
    {

    public:
        Darkglow()
        {
            id = DARKGLOW;
            duration = 45;
        }

    };

    class HyperspeedAcceleration : public Cooldown
    {

    public:
        HyperspeedAcceleration()
        {
            id = HYPERSPEED_ACCELERATION;
            duration = 60;
        }

    };

    class ManaGem : public Cooldown
    {

    public:
        ManaGem()
        {
            id = MANA_GEM;
            duration = 120;
        }

    };

    class Praxis : public Cooldown
    {

    public:
        Praxis()
        {
            id = PRAXIS;
            duration = 45;
        }

    };

}
