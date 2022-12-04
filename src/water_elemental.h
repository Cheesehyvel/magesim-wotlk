#pragma once

#include "unit.h"

namespace unit
{

constexpr int WATER_ELEMENTAL = 37994;

class WaterElemental : public Unit
{

public:

    WaterElemental(std::shared_ptr<Config> _config);

    void setStats(Stats _stats);

    double maxMana() const;

    action::Action nextAction(const State& state);
};
}