#pragma once

#include "unit.h"
#include "stats.h"

namespace unit
{

constexpr int WATER_ELEMENTAL = 37994;

class WaterElemental : public Unit
{

public:

    WaterElemental(const Config& config, const Stats& stats);

    double maxMana() const;

    action::Action nextAction(const State& state);
};
}