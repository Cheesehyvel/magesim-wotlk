#include "unit.h"
#include "mirror_image.h"
#include "state.h"

#include <vector>

namespace
{
Stats CorrectStats(const Stats& stats)
{
    Stats result = stats;
    result.haste_rating = 0;
    result.haste = 0;
    result.spell_power *= 0.33;
    return result;
}
}

namespace unit
{
MirrorImage::MirrorImage(const Config& _config, const Stats& _stats)
    : Unit(_config, CorrectStats(_stats))
{
    base_mana = 3000; // Unknown
    name = "Mirror Image";
    id = MIRROR_IMAGE;
    duration = 30;
    get_raid_buffs = false;
    unique = false;
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
    auto action = gcdAction(state.t);
    if (action.type != action::TYPE_NONE)
        return action;

    if (!hasCooldown(cooldown::FIRE_BLAST) && random<int>(0, 2) == 0)
        return spellAction<spell::MirrorFireBlast>();
    else
        return spellAction<spell::MirrorFrostbolt>();

    return { action::TYPE_NONE };
}
}