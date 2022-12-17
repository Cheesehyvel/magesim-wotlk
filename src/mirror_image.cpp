#include "unit.h"
#include "mirror_image.h"
#include "state.h"

#include <vector>

namespace unit
{

MirrorImage::MirrorImage(std::shared_ptr<Config> _config) : Unit(_config)
{
    base_mana = 3000; // Unknown
    name = "Mirror Image";
    id = MIRROR_IMAGE;
    duration = 30;
    get_raid_buffs = false;
    unique = false;
}

void MirrorImage::setStats(Stats _stats)
{
    stats = _stats;
    stats.haste_rating = 0;
    stats.haste = 0;
    stats.spell_power *= 0.33;
}

std::vector<action::Action> MirrorImage::onCastSuccessProc(const State& state, std::shared_ptr<spell::Spell> spell)
{
    auto actions = Unit::onCastSuccessProc(state, spell);

    // Cooldowns
    if (spell->id == spell::MIRROR_FIRE_BLAST)
        actions.emplace_back(cooldownAction<cooldown::FireBlast>());

    return actions;
}

action::Action MirrorImage::nextAction(const State& state)
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

    if (!hasCooldown(cooldown::FIRE_BLAST) && random<int>(0, 2) == 0)
        return spellAction<spell::MirrorFireBlast>();
    else
        return spellAction<spell::MirrorFrostbolt>();

    return { action::TYPE_NONE };
}
}