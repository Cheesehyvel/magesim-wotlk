#include "state.h"
#include "config.h"
#include "unit.h"
#include "target.h"

#include <memory>

State::State(const Config& _config) : config(_config)
{
    active_interruptions.resize(config.interruptions.size());
    reset();
}

void State::reset()
{
    t = 0;
    duration = config.duration;
    duration += -config.duration_variance + random<double>(0, config.duration_variance * 2);

    spells.clear();
    units.clear();

    for (auto const& i : targets)
        i->reset();

    std::fill(active_interruptions.begin(), active_interruptions.end(), false);
}

bool State::inCombat() const
{
    return t >= 0;
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

void State::addTarget(int id)
{
    if (id < 1)
        id = targets.size()+1;
    auto target = std::make_shared<target::Target>(config, id);
    targets.push_back(target);
}

double State::timeRemain() const
{
    return duration - t;
}

double State::hpRemain() const
{
    if (t <= 0)
        return config.target_hp;

    return timeRemain() / duration * config.target_hp;
}

unsigned long long State::totalDmg() const
{
    unsigned long long dmg = 0;
    for (auto const& i : targets)
        dmg+= i->dmg;
    return dmg;
}

unsigned long long State::mainDmg() const
{
    return targets[0]->dmg;
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
