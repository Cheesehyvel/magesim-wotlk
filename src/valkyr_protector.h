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

constexpr int VALKYR_PROTECTOR = 38392;

class ValkyrProtector : public Unit
{
public:

    ValkyrProtector(const Config& config, const Stats& stats);

    action::Action nextAction(const State& state);
};

}