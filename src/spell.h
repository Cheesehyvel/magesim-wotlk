using namespace std;

namespace spell
{

    enum ID : int
    {
        ARCANE_BLAST = 42897,
        ARCANE_EXPLOSION = 42921,
        ARCANE_MISSILES = 42846,
        FROSTBOLT = 42842,
        FIREBALL = 42833,
        FROSTFIRE_BOLT = 47610,
        SCORCH = 42859,
        FIRE_BLAST = 42873,
        PYROBLAST = 42891,
        LIVING_BOMB = 55360,
        FLAMESTRIKE = 42926,
        BLAST_WAVE = 42945,
        DRAGONS_BREATH = 42950,
        ICE_LANCE = 42914,
        CONE_OF_COLD = 42931,
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
        bool channeling = false;
        bool proc = false;
        bool binary = false;
        bool aoe = false;
        bool has_travel_time = false;
        int ticks = 0;
        School school;

        double actual_cost = 0;
        int tick = 0;
        bool done = false;

        int misses = 0;
        int hits = 0;
        int crits = 0;

        double avgDmg()
        {
            return (min_dmg + max_dmg)/2.0;
        }

    };

    class SpellInstance
    {

    public:
        shared_ptr<Spell> spell;
        Result result;
        double dmg = 0;
        double resist = 0;

        SpellInstance(shared_ptr<Spell> _spell)
        {
            spell = _spell;
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
            min_dmg = 360;
            max_dmg = 360;
            cast_time = 5;
            coeff = 5/3.5;
            school = SCHOOL_ARCANE;
            channeling = true;
            ticks = 5;
            has_travel_time = true;
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
            min_dmg = 799;
            max_dmg = 861;
            cast_time = 3;
            coeff = 3/3.5 * 0.95;
            school = SCHOOL_FROST;
            binary = true;
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
            min_dmg = 888;
            max_dmg = 1132;
            cast_time = 3.5;
            coeff = 1;
            school = SCHOOL_FIRE;
            has_travel_time = true;
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
            min_dmg = 376;
            max_dmg = 444;
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

    class Pyroblast : public Spell
    {

    public:
        Pyroblast()
        {
            id = PYROBLAST;
            name = "Pyroblast";
            cost = 22;
            min_dmg = 1190;
            max_dmg = 1510;
            cast_time = 6;
            coeff = 1.15;
            school = SCHOOL_FIRE;
            has_travel_time = true;
        }

    };

    class Flamestrike : public Spell
    {

    public:
        Flamestrike()
        {
            id = PYROBLAST;
            name = "Flamestrike";
            cost = 30;
            min_dmg = 873;
            max_dmg = 1067;
            cast_time = 2;
            coeff = 0.2357;
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
            id = PYROBLAST;
            name = "Dragon's Breath";
            cost = 7;
            min_dmg = 1101;
            max_dmg = 1279;
            cast_time = 0;
            coeff = 0.1936;
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
            min_dmg = 690;
            max_dmg = 690;
            cast_time = 0;
            delay = 12;
            coeff = 0.4286;
            school = SCHOOL_FIRE;
        }

    };

}