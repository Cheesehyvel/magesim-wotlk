#pragma once

#include "common.h"
#include "config.h"
#include "spell.h"
#include "debuff.h"

#include <unordered_map>
#include <vector>

namespace unit
{
class Unit;
}

struct State
{
private:
    std::shared_ptr<Config> config;

public:
    double t;
    unsigned long long dmg;
    double duration;
    double ignite_dmg;
    double ignite_t;

    std::unordered_map<spell::ID, SpellStats> spells;
    std::unordered_map<debuff::ID, std::shared_ptr<debuff::Debuff>> debuffs;
    std::vector<std::shared_ptr<unit::Unit>> units;
    std::vector<bool> active_interruptions;

    State(std::shared_ptr<Config> _config);

    void reset();

    bool inCombat() const;

    double dps() const;

    int debuffStacks(debuff::ID id) const;
    bool hasDebuff(debuff::ID id) const;
    int addDebuff(std::shared_ptr<debuff::Debuff> debuff);
    void removeDebuff(debuff::ID id);

    bool hasUnit(int id) const;
    void addUnit(std::shared_ptr<unit::Unit> unit);
    void removeUnit(std::shared_ptr<unit::Unit> unit);

    double timeRemain() const;

    void activateInterruption(int index);
    void deactivateInterruption(int index);
    double interruptedFor(bool is_player = true) const;
    bool isInterrupted(bool is_player = true) const;
    bool isSilenced(bool is_player = true) const;
    bool isMoving(bool is_player = true) const;
};
