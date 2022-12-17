#include "water_elemental.h"
#include "unit.h"
#include "state.h"

namespace unit
{
WaterElemental::WaterElemental(std::shared_ptr<Config> _config) : Unit(_config)
{
    base_mana = 1082;
    name = "Water Elemental";
    id = WATER_ELEMENTAL;
    duration = 45;
}

void WaterElemental::setStats(Stats _stats)
{
    stats = _stats;
    stats.haste_rating = 0;
    stats.haste = 0;
    stats.spell_power *= 0.33;
    stats.intellect = 369 + stats.intellect * 0.3;
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