namespace unit {
    class Unit;
}

namespace action
{

    enum Type : int
    {
        TYPE_WAIT,
        TYPE_SPELL,
        TYPE_BUFF,
        TYPE_BUFF_EXPIRE,
        TYPE_DEBUFF,
        TYPE_COOLDOWN,
        TYPE_COOLDOWN_EXPIRE,
        TYPE_POTION,
        TYPE_CONJURED,
        TYPE_TRINKET,
        TYPE_MANA,
        TYPE_UNIT,
    };

    class Action
    {

    public:
        Type type;
        double value;
        double gcd = 0;
        std::string str;
        bool primary_action = false;

        Potion potion;
        Conjured conjured;
        Trinket trinket;

        shared_ptr<spell::Spell> spell;
        shared_ptr<buff::Buff> buff;
        shared_ptr<debuff::Debuff> debuff;
        shared_ptr<cooldown::Cooldown> cooldown;
        shared_ptr<unit::Unit> unit;

        Action(Type _type)
        {
            type = _type;
        }
    };

}