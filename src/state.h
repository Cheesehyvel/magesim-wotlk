#pragma once

#include "common.h"
#include "config.h"
#include "spell.h"
#include "debuff.h"

#include <unordered_map>
#include <vector>

namespace unit { class Unit; }
namespace target { class Target; }

struct State
{
private:
    const Config config;

public:
    double t;
    double duration;

    std::unordered_map<spell::ID, SpellStats> spells;
    std::unordered_map<debuff::ID, std::shared_ptr<debuff::Debuff>> debuffs;
    std::vector<std::shared_ptr<unit::Unit>> units;
    std::vector<std::shared_ptr<target::Target>> targets;
    std::vector<bool> active_interruptions;

    State(const Config& config);

    void reset();

    bool inCombat() const;

    bool hasUnit(int id) const;
    void addUnit(std::shared_ptr<unit::Unit> unit);
    void removeUnit(std::shared_ptr<unit::Unit> unit);

    void addTarget(int id = 0);

    double timeRemain() const;

    unsigned long long totalDmg() const;
    unsigned long long mainDmg() const;

    void activateInterruption(int index);
    void deactivateInterruption(int index);
    double interruptedFor(bool is_player = true) const;
    bool isInterrupted(bool is_player = true) const;
    bool isSilenced(bool is_player = true) const;
    bool isMoving(bool is_player = true) const;
};
