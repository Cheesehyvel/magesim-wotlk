#pragma once

#include <string>
#include <memory>

namespace unit
{
class Unit;
}

enum LogType : int
{
    LOG_NONE,
    LOG_CAST_START,
    LOG_CAST_SUCCESS,
    LOG_SPELL_IMPACT,
    LOG_MANA,
    LOG_BUFF,
    LOG_DOT,
    LOG_DEBUG,
    LOG_WAIT,
    LOG_UNIT,
    LOG_INTERRUPTION
};

struct LogEntry
{
    LogType type;
    std::shared_ptr<unit::Unit> unit;
    std::string text;
    double t;
    double mana;
    double mana_percent;
    unsigned long long dmg;

    LogEntry(LogType _type, std::shared_ptr<unit::Unit> _unit, const std::string& _text, double _t,
        double _mana, double _mana_percent, unsigned long long _dmg) :
        type(_type), unit(_unit), text(_text), t(_t), mana(_mana), mana_percent(_mana_percent),
        dmg(_dmg) {}
};