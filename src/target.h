#pragma once

#include "config.h"
#include "debuff.h"
#include "spell.h"
#include <unordered_map>
#include <string>

namespace target
{

class Target
{
protected:
    const Config config;

public:
    int id;
    std::string name;
    unsigned long long dmg;
    double ignite_dmg;
    double t_ignite;
    double t_living_bomb;

    std::unordered_map<debuff::ID, std::shared_ptr<debuff::Debuff>> debuffs;

    Target(const Config& config, int id);

    void reset();

    int debuffStacks(debuff::ID id) const;
    bool hasDebuff(debuff::ID id) const;
    int addDebuff(std::shared_ptr<debuff::Debuff> debuff);
    void removeDebuff(debuff::ID id);
};

}