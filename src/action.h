#pragma once

namespace unit {
class Unit;
}

#include <memory>

namespace action
{

    enum Type : int
    {
        TYPE_NONE,
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

    struct Action
    {
        Type type = TYPE_NONE;
        double value = 0;
        double gcd = 0;
        std::string str = "";
        bool primary_action = false;

        Potion potion = Potion::POTION_NONE;
        Conjured conjured = Conjured::CONJURED_NONE;
        Trinket trinket = Trinket::TRINKET_NONE;

        std::shared_ptr<spell::Spell> spell;
        std::shared_ptr<buff::Buff> buff;
        std::shared_ptr<debuff::Debuff> debuff;
        std::shared_ptr<cooldown::Cooldown> cooldown;
        std::shared_ptr<unit::Unit> unit;

        Action() = default;
        Action(const Action&) = delete;
        Action(Action&&) = default;
    };
}