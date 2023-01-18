#include "water_elemental.h"
#include "unit.h"
#include "state.h"

namespace
{
Stats CorrectStats(const Stats& stats)
{
    Stats result = stats;
    result.haste_rating = 0;
    result.haste = 0;
    result.spell_power *= 0.33;
    result.intellect = 369 + stats.intellect * 0.3;
    return result;
}
}

namespace unit
{
WaterElemental::WaterElemental(const Config& _config, const Stats &_stats)
    : Unit(_config, CorrectStats(_stats))
{
    base_mana = 1082;
    name = "Water Elemental";
    id = WATER_ELEMENTAL;
    duration = 45;
}

double WaterElemental::maxMana() const
{
    return base_mana + stats.intellect * 4.95;
}

action::Action WaterElemental::nextAction(const State& state)
{
    auto action = gcdAction(state.t);

    if (action.type != action::TYPE_NONE)
        return action;

    if (state.isInterrupted(false)) {
        action::Action action;
        action.type = action::TYPE_WAIT;
        action.value = state.interruptedFor(false);
        return action;
    }

    return spellAction<spell::Waterbolt>();
}
}