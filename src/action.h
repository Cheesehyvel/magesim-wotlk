namespace action
{

    enum Type : int
    {
        TYPE_WAIT,
        TYPE_SPELL,
        TYPE_BUFF,
        TYPE_DEBUFF,
        TYPE_POTION,
        TYPE_CONJURED,
        TYPE_MANA,
    };

    class Action
    {

    public:
        Type type;
        double value;
        double gcd = 0;
        std::string str;
        bool self_action = false;

        Potion potion;
        Conjured conjured;

        shared_ptr<spell::Spell> spell;
        shared_ptr<buff::Buff> buff;
        shared_ptr<debuff::Debuff> debuff;
        shared_ptr<cooldown::Cooldown> cooldown;

        Action(Type _type)
        {
            type = _type;
        }
    };

}