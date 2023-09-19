#pragma once

#include "unit.h"
#include "stats.h"

#include <vector>

namespace action
{
struct Action;
}

namespace unit
{

constexpr int VALKYR_GUARDIAN = 38391;

class ValkyrGuardian : public Unit
{
public:

    ValkyrGuardian(const Config& config, const Stats& stats);

    action::Action nextAction(const State& state);
};

}