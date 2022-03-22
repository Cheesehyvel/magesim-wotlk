namespace buff
{

    enum ID : int
    {
        ARCANE_BLAST = 30451,
        ARCANE_POWER = 12042,
        PRESENCE_OF_MIND = 12043,
        ICY_VEINS = 12472,
        COMBUSTION = 29977,
        EVOCATION = 12051,
        BLOODLUST = 2825,
        POWER_INFUSION = 10060,
        INNERVATE = 29166,
        CLEARCAST = 12536,
        SERPENT_COIL = 30720,
        MANA_TIDE = 39609,
        BERSERKING = 20554,
        MQG = 23723,
        ETERNAL_SAGE = 35084,
        SKULL_GULDAN = 40396,
        SHRUNKEN_HEAD = 43712,
        NAARU_SLIVER = 45042,
        DRUMS_OF_BATTLE = 35476,
        DRUMS_OF_WAR = 35475,
        DRUMS_OF_RESTORATION = 35478,
        FLAME_CAP = 28714,
        ASHTONGUE_TALISMAN = 40482,
        MISSILE_BARRAGE = 44401,
    };


    class Buff
    {

    public:
        ID id;
        std::string name;
        double duration;
        int stacks = 1;
        int max_stacks = 1;
        bool hidden = false;
        double cost = 0;

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
        }

    };

    class ArcanePower : public Buff
    {

    public:
        ArcanePower()
        {
            id = ARCANE_POWER;
            name = "Arcane Power";
            duration = 15;
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

    class SerpentCoil : public Buff
    {

    public:
        SerpentCoil()
        {
            id = SERPENT_COIL;
            name = "Serpent Coil";
            duration = 15;
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

    class MindQuickening : public Buff
    {

    public:
        MindQuickening()
        {
            id = MQG;
            name = "MQG";
            duration = 20;
        }

    };

    class EternalSage : public Buff
    {

    public:
        EternalSage()
        {
            id = ETERNAL_SAGE;
            name = "Band of Eternal Sage";
            duration = 10;
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
        }

    };

    class DrumsOfBattle : public Buff
    {

    public:
        DrumsOfBattle()
        {
            id = DRUMS_OF_BATTLE;
            name = "Drums of Battle";
            duration = 30;
        }

    };

    class DrumsOfWar : public Buff
    {

    public:
        DrumsOfWar()
        {
            id = DRUMS_OF_WAR;
            name = "Drums of War";
            duration = 30;
        }

    };

    class DrumsOfRestoration : public Buff
    {

    public:
        DrumsOfRestoration()
        {
            id = DRUMS_OF_RESTORATION;
            name = "Drums of Restoration";
            duration = 30;
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

    class AshtongueTalisman : public Buff
    {

    public:
        AshtongueTalisman()
        {
            id = ASHTONGUE_TALISMAN;
            name = "Insight of the Ashtongue";
            duration = 5;
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

}