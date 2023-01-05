#pragma once

#include "stats.h"
#include "cooldown.h"
#include "config.h"
#include "buff.h"
#include "debuff.h"
#include "config.h"
#include "spell.h"
#include "action.h"

#include <string>
#include <unordered_map>
#include <memory>

struct State;

namespace unit
{

class Unit
{
protected:
    std::shared_ptr<Config> config;
    Stats stats;
    Stats buff_stats;

public:
    std::string name;
    double base_mana = 2241;
    double mana;
    double t_gcd;
    double t_gcd_capped = 0;
    double duration = 0;
    bool unique = true;
    bool get_raid_buffs = true;
    bool is_channeling = false;
    double last_spell_cast_time = 0;
    int id;

    std::unordered_map<cooldown::ID, std::shared_ptr<cooldown::Cooldown>> cooldowns;
    std::unordered_map<buff::ID, std::shared_ptr<buff::Buff>> buffs;
    std::unordered_map<buff::ID, std::shared_ptr<buff::Buff>> snapshot_buffs;
    std::unordered_map<debuff::ID, std::shared_ptr<debuff::Debuff>> debuffs;

    Unit(std::shared_ptr<Config> _config);

    virtual void reset();

    virtual Stats getStats();
    virtual void setStats(Stats _stats);

    bool hasCooldown(cooldown::ID id) const;
    void addCooldown(std::shared_ptr<cooldown::Cooldown> cooldown);
    void removeCooldown(cooldown::ID id);

    int buffStacks(buff::ID id, bool snapshot = false) const;
    bool hasBuff(buff::ID id, bool snapshot = false) const;
    int addBuff(std::shared_ptr<buff::Buff> buff);
    void removeBuff(buff::ID id, bool snapshot = false);

    bool canReactTo(buff::ID id, double t) const;
    bool canReactTo(cooldown::ID id, double t) const;

    bool hasSnapshot(buff::ID id) const;
    void removeSnapshot(buff::ID id);
    void removeSnapshots();

    void addBuffStats(const Stats& _stats);
    void removeBuffStats(const Stats& _stats, int stacks = 1);

    int debuffStacks(debuff::ID id) const;
    bool hasDebuff(debuff::ID id) const;
    int addDebuff(std::shared_ptr<debuff::Debuff> debuff);
    void removeDebuff(debuff::ID id);

    virtual void applyMana(const State& state, double _mana);

    virtual double spiritManaPerSecond() const;

    virtual double staticManaPerSecond() const;

    virtual double manaPerSecond(const State&) const;

    virtual double maxMana() const;

    double manaPercent() const;

    double manaPerTick(const State& state) const;

    double gcd(double t = 1.5) const;

    virtual double baseCastTime(std::shared_ptr<spell::Spell> spell) const;

    virtual double castTime(std::shared_ptr<spell::Spell> spell) const;

    virtual double castHaste() const;

    virtual double hitChance(std::shared_ptr<spell::Spell> spell, double dlevel = 3) const;

    virtual double critChance(std::shared_ptr<spell::Spell> spell) const;

    bool canCast(std::shared_ptr<spell::Spell> spell) const;

    bool canBuff(const buff::Buff &buff) const;

    bool canMiss(std::shared_ptr<spell::Spell> spell) const;

    virtual bool canCrit(std::shared_ptr<spell::Spell> spell) const;

    bool canResist(std::shared_ptr<spell::Spell> spell) const;

    virtual double baseCritMultiplier(std::shared_ptr<spell::Spell> spell) const;

    virtual double critMultiplierMod(std::shared_ptr<spell::Spell> spell) const;

    virtual double spellCoeffMod(std::shared_ptr<spell::Spell> spell) const;

    virtual double buffDmgMultiplier(std::shared_ptr<spell::Spell> spell, const State& state) const;

    virtual double baseManaCost(std::shared_ptr<spell::Spell> spell) const;

    virtual double baseManaCost(const buff::Buff& buff) const;

    virtual double manaCost(std::shared_ptr<spell::Spell> spell) const;

    virtual double manaCost(const buff::Buff &buff) const;

    virtual double manaCostMod(std::shared_ptr<spell::Spell> spell, double mana_cost) const;

    virtual double manaCostMultiplier(std::shared_ptr<spell::Spell> spell) const;

    virtual double getIntellect() const;

    virtual double getSpirit() const;

    virtual double getSpellPower(School school = SCHOOL_NONE) const;

    virtual double getHasteRating() const;

    virtual double getMp5() const;

    virtual double cooldownMod(const cooldown::Cooldown &cooldown) const;

    virtual void interrupt(const Interruption& interruption);

    virtual std::vector<action::Action> onBuffGain(const State &state, std::shared_ptr<buff::Buff> buff);

    virtual std::vector<action::Action> onBuffExpire(const State &state, std::shared_ptr<buff::Buff> buff);

    virtual std::vector<action::Action> onCastSuccessProc(const State &state, std::shared_ptr<spell::Spell> spell);

    virtual std::vector<action::Action> onSpellImpactProc(const State &state, const spell::SpellInstance &instance);

    virtual std::vector<action::Action> onSpellTickProc(const State &state, std::shared_ptr<spell::Spell> spell, int tick);

    virtual std::vector<action::Action> usePotion(Potion potion, bool in_combat);

    virtual std::vector<action::Action> useConjured(const State &state, Conjured conjured);

    virtual std::vector<action::Action> useTrinket(Trinket trinket, std::shared_ptr<cooldown::Cooldown> cooldown);

    action::Action buffAction(std::shared_ptr<buff::Buff> buff, bool primary_action = false) const;

    template <typename T, typename... Args>
    action::Action buffAction(bool primary_action = false, Args... args) const;

    action::Action buffExpireAction(std::shared_ptr<buff::Buff> buff, bool primary_action = false) const;

    template <typename T, typename... Args>
    action::Action buffExpireAction(bool primary_action = false, Args... args) const;

    template <typename T, typename... Args>
    action::Action debuffAction(bool primary_action = false, Args... args) const;

    action::Action cooldownAction(std::shared_ptr<cooldown::Cooldown> cooldown) const;

    template <typename T, typename... Args>
    action::Action cooldownAction(Args... args) const;

    template <typename B, typename C>
    action::Action buffCooldownAction(bool primary_action = false) const;

    template <typename T, typename... Args>
    action::Action cooldownExpireAction(Args... args) const;

    action::Action spellAction(std::shared_ptr<spell::Spell> spell) const;

    template <typename T, typename... Args>
    action::Action spellAction(Args... args) const;

    template <typename S, typename C>
    action::Action spellCooldownAction() const;

    virtual action::Action manaAction(double mana, const std::string& str = "") const;

    virtual action::Action gcdAction(double t);
    virtual action::Action nextAction(const State &state);
};

template <typename T, typename... Args>
action::Action Unit::buffAction(bool primary_action, Args... args) const
{
    action::Action action{ action::TYPE_BUFF };
    action.buff = std::make_shared<T>(std::forward<Args>(args)...);
    action.primary_action = primary_action;
    return action;
}

template <typename T, typename... Args>
action::Action Unit::buffExpireAction(bool primary_action, Args... args) const
{
    action::Action action{ action::TYPE_BUFF_EXPIRE };
    action.buff = std::make_shared<T>(std::forward<Args>(args)...);
    action.primary_action = primary_action;
    return action;
}

template <typename T, typename... Args>
action::Action Unit::debuffAction(bool primary_action, Args... args) const
{
    action::Action action{ action::TYPE_DEBUFF };
    action.debuff = std::make_shared<T>(std::forward<Args>(args)...);
    action.primary_action = primary_action;
    return action;
}

template <typename T, typename... Args>
action::Action Unit::cooldownAction(Args... args) const
{
    action::Action action{ action::TYPE_COOLDOWN };
    action.cooldown = std::make_shared<T>(std::forward<Args>(args)...);
    return action;
}

template <typename B, typename C>
action::Action Unit::buffCooldownAction(bool primary_action) const
{
    auto action = buffAction<B>(primary_action);
    action.cooldown = std::make_shared<C>();
    return action;
}

template <typename T, typename... Args>
action::Action Unit::cooldownExpireAction(Args... args) const
{
    action::Action action{ action::TYPE_COOLDOWN_EXPIRE };
    action.cooldown = std::make_shared<T>(std::forward<Args>(args)...);
    return action;
}

template <typename T, typename... Args>
action::Action Unit::spellAction(Args... args) const
{
    action::Action action{ action::TYPE_SPELL };
    action.spell = std::make_shared<T>(std::forward<Args>(args)...);
    return action;
}

template <typename S, typename C>
action::Action Unit::spellCooldownAction() const
{
    auto action = spellAction<S>();
    action.cooldown = std::make_shared<C>();
    return action;
}
}