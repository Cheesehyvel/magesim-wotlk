#include "unit.h"
#include "valkyr_protector.h"
#include "state.h"

#include <vector>

namespace
{
Stats CorrectStats(const Stats& stats)
{
    Stats result = stats;
    result.haste_rating = 0;
    result.haste = 0;
    result.spell_power = 0;
    result.hit = 100;
    result.crit = 5;
    return result;
}
}

namespace unit
{

ValkyrProtector::ValkyrProtector(const Config& _config, const Stats& _stats)
    : Unit(_config, CorrectStats(_stats))
{
    base_mana = 3000; // Unknown
    name = "Val'kyr Protector";
    id = VALKYR_PROTECTOR;
    duration = 30;
    get_raid_buffs = false;
    get_raid_debuffs = false;
    unique = false;
}

action::Action ValkyrProtector::nextAction(const State& state)
{
    auto gcd = gcdAction(state.t);
    if (gcd.type != action::TYPE_NONE)
        return gcd;

    if (state.isInterrupted(false)) {
        action::Action action;
        action.type = action::TYPE_WAIT;
        action.value = state.interruptedFor(false);
        return action;
    }

    auto target = state.targets[0];

    return spellAction<spell::ValkyrProtectorSmite>(target);
}
}