namespace dot
{

    enum ID : int
    {
        IGNITE = 12848,
        FIREBALL = 42833,
        PYROBLAST = 42891,
        FROSTFIRE_BOLT = 47610,
        LIVING_BOMB = 55360,
        FLAMESTRIKE = 42926,
    };


    class Dot
    {

    public:
        ID id;
        std::string name;
        int ticks = 1;
        int tick = 0;
        double base_dmg = 0;
        double dmg;
        double t_interval = 3;
        double coeff = 0;
        bool aoe = false;
        bool stackable = false;
        School school;

        virtual void onTick()
        {
            tick++;
        }

        virtual void stack(double _dmg){}

    };

    class Ignite : public Dot
    {

    public:
        double dmg2;
        double dmg3;
        double dmg4;

        Ignite(double _dmg)
        {
            id = IGNITE;
            name = "Ignite";
            t_interval = 1;
            ticks = 4;
            stackable = true;
            school = SCHOOL_FIRE;
            stack(_dmg);
        }

        void onTick()
        {
            Dot::onTick();
            dmg = dmg2;
            dmg2 = dmg3;
            dmg3 = dmg4;
            dmg4 = 0;
        }

        void stack(double _dmg)
        {
            dmg+= _dmg;
            dmg2+= _dmg;
            dmg3+= _dmg;
            dmg4+= _dmg;

            tick = 0;
        }

    };

    class Fireball : public Dot
    {

    public:
        Fireball()
        {
            id = FIREBALL;
            name = "Fireball";
            t_interval = 2;
            ticks = 4;
            base_dmg = 29;
            school = SCHOOL_FIRE;
        }

    };

    class FrostfireBolt : public Dot
    {

    public:
        FrostfireBolt()
        {
            id = FROSTFIRE_BOLT;
            name = "Frostfire Bolt";
            t_interval = 3;
            ticks = 3;
            base_dmg = 30;
            school = SCHOOL_FROSTFIRE;
        }

    };

    class Pyroblast : public Dot
    {

    public:
        Pyroblast()
        {
            id = PYROBLAST;
            name = "Pyroblast";
            t_interval = 3;
            ticks = 4;
            base_dmg = 113;
            coeff = 0.05;
            school = SCHOOL_FIRE;
        }

    };

    class LivingBomb : public Dot
    {

    public:
        LivingBomb()
        {
            id = LIVING_BOMB;
            name = "Living Bomb";
            t_interval = 3;
            ticks = 4;
            base_dmg = 345;
            coeff = 0.2;
            school = SCHOOL_FIRE;
        }

    };

    class Flamestrike : public Dot
    {

    public:
        Flamestrike()
        {
            id = FLAMESTRIKE;
            name = "Flamestrike";
            t_interval = 2;
            ticks = 4;
            base_dmg = 306;
            aoe = true;
            coeff = 0.122;
            school = SCHOOL_FIRE;
        }

    };

}