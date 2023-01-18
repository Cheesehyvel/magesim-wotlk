#include "unit.h"
#include "state.h"
#include "config.h"
#include "common.h"
#include "spell.h"
#include "action.h"

#include <memory>
#include <string>
#include <exception>

namespace unit
{
Unit::Unit(const Config& _config, const Stats& _stats)
    : config(_config), stats(_stats) {}

void Unit::reset()
{
    mana = maxMana();
    t_gcd = 0;
    t_gcd_capped = 0;
    is_channeling = false;
    last_spell_cast_time = 0;

    buffs.clear();
    snapshot_buffs.clear();
    debuffs.clear();
    cooldowns.clear();

    buff_stats.intellect = 0;
    buff_stats.spirit = 0;
    buff_stats.mp5 = 0;
    buff_stats.crit = 0;
    buff_stats.hit = 0;
    buff_stats.haste = 0;
    buff_stats.haste_rating = 0;
    buff_stats.spell_power = 0;
}

bool Unit::hasCooldown(cooldown::ID id) const
{
    return cooldowns.find(id) != cooldowns.end();
}

void Unit::addCooldown(std::shared_ptr<cooldown::Cooldown> cooldown)
{
    cooldowns[cooldown->id] = cooldown;
}

void Unit::removeCooldown(cooldown::ID id)
{
    cooldowns.erase(id);
}

int Unit::buffStacks(buff::ID id, bool snapshot) const
{
    auto const itr = buffs.find(id);
    if (itr != buffs.end())
        return itr->second->stacks;

    if (snapshot)
    {
        auto const snapshot_itr = snapshot_buffs.find(id);
        if (snapshot_itr != snapshot_buffs.end())
            return snapshot_itr->second->stacks;
    }

    return 0;
}

bool Unit::hasBuff(buff::ID id, bool snapshot) const
{
    if (buffs.find(id) != buffs.end())
        return true;

    if (snapshot && hasSnapshot(id))
        return true;

    return false;
}

int Unit::addBuff(std::shared_ptr<buff::Buff> buff)
{
    auto const stacks = buffStacks(buff->id);

    if (stacks < buff->max_stacks)
        addBuffStats(buff->stats);

    if (stacks)
        buffs[buff->id]->addStack();
    else
        buffs[buff->id] = buff;

    return buffs[buff->id]->stacks;
}

void Unit::removeBuff(buff::ID id, bool snapshot)
{
    auto const i = buffs.find(id);
    if (i != buffs.end())
    {
        auto const& buff = i->second;

        if (snapshot)
            snapshot_buffs[id] = buff;
        else
            removeBuffStats(buff->stats, buff->stacks);

        buffs.erase(id);
    }
}

bool Unit::canReactTo(buff::ID id, double t) const
{
    if (t < t_gcd)
        t = t_gcd;

    auto const i = buffs.find(id);

    return i != buffs.end() && t - i->second->t_gained >= config.reaction_time / 1000.0;
}

bool Unit::canReactTo(cooldown::ID id, double t) const
{
    if (t < t_gcd)
        t = t_gcd;

    auto const i = cooldowns.find(id);

    return i != cooldowns.end() && t - i->second->t_gained >= config.reaction_time / 1000.0;
}

bool Unit::hasSnapshot(buff::ID id) const
{
    return snapshot_buffs.find(id) != snapshot_buffs.end();
}

void Unit::removeSnapshot(buff::ID id)
{
    auto const i = snapshot_buffs.find(id);
    if (i != snapshot_buffs.end())
    {
        removeBuffStats(i->second->stats, i->second->stacks);
        snapshot_buffs.erase(i);
    }
}

void Unit::removeSnapshots()
{
    for (auto it = snapshot_buffs.begin(); it != snapshot_buffs.end(); ++it)
        removeBuffStats(it->second->stats, it->second->stacks);
    snapshot_buffs.clear();
}

void Unit::addBuffStats(const Stats& _stats)
{
    buff_stats.intellect += _stats.intellect;
    buff_stats.spirit += _stats.spirit;
    buff_stats.mp5 += _stats.mp5;
    buff_stats.crit += _stats.crit;
    buff_stats.hit += _stats.hit;
    buff_stats.haste += _stats.haste;
    buff_stats.haste_rating += _stats.haste_rating;
    buff_stats.spell_power += _stats.spell_power;
}

void Unit::removeBuffStats(const Stats& _stats, int stacks)
{
    buff_stats.intellect -= _stats.intellect * stacks;
    buff_stats.spirit -= _stats.spirit * stacks;
    buff_stats.mp5 -= _stats.mp5 * stacks;
    buff_stats.crit -= _stats.crit * stacks;
    buff_stats.hit -= _stats.hit * stacks;
    buff_stats.haste -= _stats.haste * stacks;
    buff_stats.haste_rating -= _stats.haste_rating * stacks;
    buff_stats.spell_power -= _stats.spell_power * stacks;
}

int Unit::debuffStacks(debuff::ID id) const
{
    auto const itr = debuffs.find(id);
    return itr == debuffs.end() ? 0 : itr->second->stacks;
}

bool Unit::hasDebuff(debuff::ID id) const
{
    return debuffs.find(id) != debuffs.end();
}

int Unit::addDebuff(std::shared_ptr<debuff::Debuff> debuff)
{
    if (hasDebuff(debuff->id))
        return debuffs[debuff->id]->addStack();
    else
        debuffs[debuff->id] = debuff;

    return 1;
}

void Unit::removeDebuff(debuff::ID id)
{
    debuffs.erase(id);
}

void Unit::applyMana(const State& state, double _mana)
{
    mana+= _mana;

    if (mana > maxMana())
        mana = maxMana();
    else if (mana < 0)
        mana = 0;
}

double Unit::spiritManaPerSecond() const
{
    return 0.001 + getSpirit() * 0.003345 * sqrt(getIntellect());
}

double Unit::staticManaPerSecond() const
{
    return getMp5() / 5.0;
}

double Unit::manaPerSecond(const State&) const
{
    return staticManaPerSecond();
}

double Unit::maxMana() const
{
    // Subtract 280 because the first 20 intellect only gives 1 mana instead of 15
    return base_mana + stats.intellect * 15 - 280;
}

double Unit::manaPercent() const
{
    return mana / maxMana() * 100.0;
}

double Unit::manaPerTick(const State& state) const
{
    return manaPerSecond(state) * MANA_TICK_T;
}

double Unit::gcd(double t) const
{
    if (t == 0)
        return 0;

    double cap = 1.0;

    t *= castHaste();

    if (t < cap)
        t = cap;

    return t;
}

double Unit::baseCastTime(std::shared_ptr<spell::Spell> spell) const
{
    if (hasBuff(buff::PRESENCE_OF_MIND) && !spell->channeling)
        return 0;

    return spell->cast_time;
}

double Unit::castTime(std::shared_ptr<spell::Spell> spell) const
{
    return baseCastTime(spell) * castHaste();
}

double Unit::castHaste() const
{
    double haste = 1;
    double rating = getHasteRating();

    if (rating)
        haste += hasteRatingToHaste(rating) / 100.0;

    if (stats.haste)
        haste *= 1 + stats.haste / 100.0;

    if (hasBuff(buff::BLOODLUST))
        haste *= 1.3;
    else if (hasBuff(buff::POWER_INFUSION))
        haste *= 1.2;
    if (hasBuff(buff::ICY_VEINS))
        haste *= 1.2;
    if (hasBuff(buff::BERSERKING))
        haste *= 1.2;

    if (get_raid_buffs) {
        if (config.buff_haste)
            haste *= 1.03;
        if (config.buff_spell_haste)
            haste *= 1.05;
    }

    return 1.0 / haste;
}

double Unit::hitChance(std::shared_ptr<spell::Spell>, double dlevel) const
{
    return stats.hit + buff_stats.hit;
}

double Unit::critChance(std::shared_ptr<spell::Spell>) const
{
    double crit = stats.crit + buff_stats.crit;

    if (get_raid_buffs) {
        if (config.buff_spell_crit)
            crit += 5;
    }

    return crit;
}

bool Unit::canCast(std::shared_ptr<spell::Spell> spell) const
{
    return mana >= manaCost(spell);
}

bool Unit::canBuff(const buff::Buff &buff) const
{
    return mana >= manaCost(buff);
}

bool Unit::canMiss(std::shared_ptr<spell::Spell> spell) const
{
    return !spell->dot;
}

bool Unit::canCrit(std::shared_ptr<spell::Spell> spell) const
{
    return !spell->dot;
}

bool Unit::canResist(std::shared_ptr<spell::Spell> spell) const
{
    return !spell->dot;
}

double Unit::baseCritMultiplier(std::shared_ptr<spell::Spell> spell) const
{
    return 1;
}

double Unit::critMultiplierMod(std::shared_ptr<spell::Spell> spell) const
{
    return 1;
}

double Unit::spellCoeffMod(std::shared_ptr<spell::Spell> spell) const
{
    return 0;
}

double Unit::buffDmgMultiplier(std::shared_ptr<spell::Spell>, const State&) const
{
    double multi = 1;

    if (get_raid_buffs) {
        if (config.buff_dmg)
            multi *= 1.03;
    }

    return multi;
}

double Unit::baseManaCost(std::shared_ptr<spell::Spell> spell) const
{
    return spell->cost / 100.0 * base_mana;
}

double Unit::baseManaCost(const buff::Buff& buff) const
{
    return buff.cost / 100.0 * base_mana;
}

double Unit::manaCost(std::shared_ptr<spell::Spell> spell) const
{
    double cost = floor(baseManaCost(spell) * manaCostMultiplier(spell));

    cost+= manaCostMod(spell, cost);

    return cost;
}

double Unit::manaCost(const buff::Buff& buff) const
{
    double cost = baseManaCost(buff);

    return round(cost);
}

double Unit::manaCostMod(std::shared_ptr<spell::Spell> spell, double mana_cost) const
{
    return 0;
}

double Unit::manaCostMultiplier(std::shared_ptr<spell::Spell> spell) const
{
    double multi = 1;

    if (hasBuff(buff::POWER_INFUSION))
        multi *= 0.8;

    return multi;
}

double Unit::getIntellect() const
{
    return stats.intellect + buff_stats.intellect;
}

double Unit::getSpirit() const
{
    return stats.spirit + buff_stats.spirit;
}

double Unit::getSpellPower(School school) const
{
    double sp = stats.spell_power + buff_stats.spell_power;

    if (get_raid_buffs) {
        if (config.demonic_pact || config.totem_of_wrath || config.flametongue) {
            double x = 0;
            if (config.totem_of_wrath)
                x = 280;
            else if (config.flametongue)
                x = 144;
            if (config.demonic_pact && config.demonic_pact_bonus > x)
                x = config.demonic_pact_bonus;
            sp += x;
        }
    }

    return sp;
}

double Unit::getHasteRating() const
{
    return stats.haste_rating + buff_stats.haste_rating;
}

double Unit::getMp5() const
{
    return stats.mp5 + buff_stats.mp5;
}

double Unit::cooldownMod(const cooldown::Cooldown &cooldown) const
{
    return 0;
}

void Unit::interrupt(const Interruption& interruption)
{
    is_channeling = false;
}

std::vector<action::Action> Unit::onBuffGain(const State &state, std::shared_ptr<buff::Buff> buff)
{
    return {};
}

std::vector<action::Action> Unit::onBuffExpire(const State &state, std::shared_ptr<buff::Buff> buff)
{
    return {};
}

std::vector<action::Action> Unit::onCastSuccessProc(const State &state, std::shared_ptr<spell::Spell> spell)
{
    return {};
}

std::vector<action::Action> Unit::onSpellImpactProc(const State &state, const spell::SpellInstance &instance)
{
    return {};
}

std::vector<action::Action> Unit::onSpellTickProc(const State &state, std::shared_ptr<spell::Spell> spell, int tick)
{
    return {};
}

std::vector<action::Action> Unit::usePotion(Potion potion, bool in_combat)
{
    return {};
}

std::vector<action::Action> Unit::useConjured(const State &state, Conjured conjured)
{
    return {};
}

std::vector<action::Action> Unit::useTrinket(Trinket trinket, std::shared_ptr<cooldown::Cooldown> cooldown)
{
    return {};
}

action::Action Unit::buffAction(std::shared_ptr<buff::Buff> buff, bool primary_action) const
{
    action::Action action{ action::TYPE_BUFF };
    action.buff = buff;
    action.primary_action = primary_action;
    return action;
}

action::Action Unit::buffExpireAction(std::shared_ptr<buff::Buff> buff, bool primary_action) const
{
    action::Action action{ action::TYPE_BUFF_EXPIRE };
    action.buff = buff;
    action.primary_action = primary_action;
    return action;
}

action::Action Unit::cooldownAction(std::shared_ptr<cooldown::Cooldown> cooldown) const
{
    action::Action action{ action::TYPE_COOLDOWN };
    action.cooldown = cooldown;
    return action;
}

action::Action Unit::spellAction(std::shared_ptr<spell::Spell> spell) const
{
    action::Action action{ action::TYPE_SPELL };
    action.spell = spell;
    return action;
}

action::Action Unit::manaAction(double mana, const std::string &str) const
{
    action::Action action{ action::TYPE_MANA };
    action.value = mana;
    action.str = str;
    return action;
}

action::Action Unit::Unit::gcdAction(double t)
{
    if (t < t_gcd) {
        action::Action action{ action::TYPE_WAIT };
        action.value = t_gcd - t;
        action.str = "GCD";

        if (last_spell_cast_time > 0)
            t_gcd_capped += action.value;

        return action;
    }

    return { action::TYPE_NONE };
}

action::Action Unit::nextAction(const State &state)
{
    return { action::TYPE_NONE };
}

}