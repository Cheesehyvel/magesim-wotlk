#include "state.h"
#include "config.h"
#include "unit.h"

State::State(const Config& _config) : config(_config)
{
    active_interruptions.resize(config.interruptions.size());
    reset();
}

void State::reset()
{
    t = 0;
    dmg = 0;
    duration = config.duration;
    duration += -config.duration_variance + random<double>(0, config.duration_variance * 2);

    ignite_dmg = 0;
    ignite_t = -20;

    spells.clear();
    debuffs.clear();
    units.clear();

    std::fill(active_interruptions.begin(), active_interruptions.end(), false);
}

bool State::inCombat() const
{
    return t >= 0;
}

double State::dps() const
{
    return t == 0 ? 0 : dmg / t;
}

int State::debuffStacks(debuff::ID id) const
{
    auto const itr = debuffs.find(id);
    if (itr != debuffs.end())
        return itr->second->stacks;
    return 0;
}

bool State::hasDebuff(debuff::ID id) const
{
    return debuffs.find(id) != debuffs.end();
}

int State::addDebuff(std::shared_ptr<debuff::Debuff> debuff)
{
    if (hasDebuff(debuff->id))
        return debuffs[debuff->id]->addStack();
    else
        debuffs[debuff->id] = debuff;

    return 1;
}

void State::removeDebuff(debuff::ID id)
{
    debuffs.erase(id);
}

bool State::hasUnit(int id) const
{
    for (auto const& i : units)
        if (i->id == id)
            return true;

    return false;
}

void State::addUnit(std::shared_ptr<unit::Unit> unit)
{
    units.push_back(unit);
}

void State::removeUnit(std::shared_ptr<unit::Unit> unit)
{
    for (auto itr = units.begin(); itr != units.end(); itr++) {
        if ((*itr) == unit) {
            units.erase(itr);
            break;
        }
    }
}

double State::timeRemain() const
{
    return duration - t;
}

void State::activateInterruption(int index)
{
    active_interruptions[index] = true;
}

void State::deactivateInterruption(int index)
{
    active_interruptions[index] = false;
}

double State::interruptedFor(bool is_player) const
{
    double max = 0;
    double d = 0;

    for (int i=0; i<active_interruptions.size(); i++) {
        if (active_interruptions[i] && (is_player || config.interruptions[i].affects_all)) {
            d = config.interruptions[i].t + config.interruptions[i].duration;
            if (d > max)
                max = d;
        }
    }

    return max - t;
}

bool State::isInterrupted(bool is_player) const
{
    for (int i=0; i<active_interruptions.size(); i++) {
        if (active_interruptions[i] && (is_player || config.interruptions[i].affects_all))
            return true;
    }

    return false;
}

bool State::isSilenced(bool is_player) const
{
    for (int i=0; i<active_interruptions.size(); i++) {
        if (active_interruptions[i] && (is_player || config.interruptions[i].affects_all) && config.interruptions[i].silence)
            return true;
    }

    return false;
}

bool State::isMoving(bool is_player) const
{
    bool is_moving = false;

    for (int i=0; i<active_interruptions.size(); i++) {
        if (active_interruptions[i] && (is_player || config.interruptions[i].affects_all)) {
            if (config.interruptions[i].silence)
                return false;
            is_moving = true;
        }
    }

    return is_moving;
}
