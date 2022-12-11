#pragma once

#include "state.h"
#include "log.h"

#include <string>
#include <memory>
#include <vector>
#include <list>

namespace spell
{
struct Spell;
struct SpellInstance;
};

namespace buff
{
struct Buff;
}

namespace debuff
{
struct Debuff;
}

namespace cooldown
{
struct Cooldown;
}

struct Event
{
    double t = 0;
    double mana = 0;
    int tick = 0;
    std::string source = "";
    EventType type = EventType::EVENT_NONE;

    // ownership is shared between this, actions, and possibly the simulator itself
    std::shared_ptr<unit::Unit> unit;

    // must be a pointer because some child classes may be larger than base
    // must be shared because multiple actions may refer to it
    std::shared_ptr<spell::Spell> spell;

    spell::SpellInstance instance;

    // ownership shared between this and Unit::buffs
    std::shared_ptr<buff::Buff> buff;

    // ownership shared between this, State, and Unit::debuffs
    std::shared_ptr<debuff::Debuff> debuff;

    std::shared_ptr<cooldown::Cooldown> cooldown;
};

class Simulation
{
private:
    std::shared_ptr<Config> config;

public:
    bool logging = true;
    std::list<Event> queue;
    std::vector<LogEntry> log;
    std::shared_ptr<unit::Player> player;
    State state;

    Simulation(std::shared_ptr<Config> _config, std::shared_ptr<unit::Player> _player);

    void reset();

    SimulationsResult runMultiple(int iterations);
    SimulationResult run(bool single = false);

    void runPrecombat();

    void workCurrent();

    void work();

    void tick(Event& event);

    void push(Event& event);

    void pushCastStart(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell, double t);

    void pushCastFinish(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell, double t);

    void pushSpellImpact(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell, double t = 0);

    void pushSpellImpact(std::shared_ptr<unit::Unit> unit, spell::SpellInstance& instance, double t = 0);

    void pushChannelingTick(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell, double t, int tick);

    void pushDot(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell, int tick);

    void pushDotTick(std::shared_ptr<unit::Unit> unit, spell::SpellInstance& instance);

    void pushManaRegen(std::shared_ptr<unit::Unit> unit);

    void pushManaGain(std::shared_ptr<unit::Unit> unit, double t, double mana, const std::string &source = "");

    void pushBuffGain(std::shared_ptr<unit::Unit> unit, std::shared_ptr<buff::Buff> buff, double t);

    void pushBuffGainAll(std::shared_ptr<buff::Buff> buff, double t);

    void pushBuffExpire(std::shared_ptr<unit::Unit> unit, std::shared_ptr<buff::Buff> buff, double t = 0);

    void pushDebuffGain(std::shared_ptr<debuff::Debuff> debuff, double t);

    void pushDebuffExpire(std::shared_ptr<debuff::Debuff> debuff);

    void pushCooldownGain(std::shared_ptr<unit::Unit> unit, std::shared_ptr<cooldown::Cooldown> cooldown, double t);

    void pushCooldownExpire(std::shared_ptr<unit::Unit> unit, std::shared_ptr<cooldown::Cooldown> cooldown);

    void pushUnitDespawn(std::shared_ptr<unit::Unit> unit, double t);

    void pushWait(std::shared_ptr<unit::Unit> unit, double t, const std::string &str = "", std::shared_ptr<spell::Spell> spell = NULL);

    void onAction(std::shared_ptr<unit::Unit> unit, action::Action & action);

    bool nextAction(std::shared_ptr<unit::Unit> unit);

    void processActions(std::shared_ptr<unit::Unit> unit, std::vector<action::Action> &actions);

    void cast(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell);

    void onCastStart(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell);

    void onCastFinish(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell);

    void onCastSuccess(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell);

    void onSpellImpact(std::shared_ptr<unit::Unit> unit, spell::SpellInstance &instance);

    void onSpellTick(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell, int tick);

    void dotApply(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell);

    void onCastSuccessProc(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell);

    void onSpellImpactProc(std::shared_ptr<unit::Unit> unit, const spell::SpellInstance &instance);

    void onSpellTickProc(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell, int tick);

    void onUnitSpawn(std::shared_ptr<unit::Unit> unit);

    void onUnitDespawn(std::shared_ptr<unit::Unit> unit);

    void despawnDuplicate(std::shared_ptr<unit::Unit> unit);

    void onManaRegen(std::shared_ptr<unit::Unit> unit, bool next = true);

    void onManaGain(std::shared_ptr<unit::Unit> unit, double mana, const std::string &source = "");

    void onWait(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell = NULL);

    void onBuffGain(std::shared_ptr<unit::Unit> unit, std::shared_ptr<buff::Buff> buff);

    void onBuffExpire(std::shared_ptr<unit::Unit> unit, std::shared_ptr<buff::Buff> buff);

    void onBuffGainAll(std::shared_ptr<buff::Buff> buff);

    void onDebuffGain(std::shared_ptr<debuff::Debuff> debuff);

    void onDebuffExpire(std::shared_ptr<debuff::Debuff> debuff);

    void onCooldownGain(std::shared_ptr<unit::Unit> unit, std::shared_ptr<cooldown::Cooldown> cooldown, bool mod = true);

    void onCooldownExpire(std::shared_ptr<unit::Unit> unit, std::shared_ptr<cooldown::Cooldown> cooldown);

    void usePotion(std::shared_ptr<unit::Unit> unit, Potion potion);

    void useConjured(std::shared_ptr<unit::Unit> unit, Conjured conjured);

    void useTrinket(std::shared_ptr<unit::Unit> unit, Trinket trinket, std::shared_ptr<cooldown::Cooldown> cooldown);

    bool hasDot(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell) const;

    double getDotDamage(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell) const;

    void removeSpellImpact(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell);

    void removeSpellImpacts(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell);

    void removeBuffExpiration(std::shared_ptr<unit::Unit> unit, const buff::Buff &buff);

    void removeDebuffExpiration(const debuff::Debuff &debuff);

    void removeCooldownExpiration(std::shared_ptr<unit::Unit> unit, const cooldown::Cooldown &cooldown);

    double buffDuration(std::shared_ptr<unit::Unit> unit, buff::ID id) const;

    double debuffDuration(debuff::ID id) const;

    double cooldownDuration(std::shared_ptr<unit::Unit> unit, cooldown::ID id) const;

    void removeUnitEvents(std::shared_ptr<unit::Unit> unit);

    double travelTime(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell);

    double hitChance(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell) const;

    double critChance(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell) const;

    double critMultiplier(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell) const;

    double buffDmgMultiplier(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell) const;

    double debuffDmgMultiplier(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell) const;

    double spellDmg(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell);

    double spellDmgResist(std::shared_ptr<unit::Unit> unit, const spell::SpellInstance& instance);

    spell::Result getSpellResult(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell) const;

    spell::SpellInstance getSpellInstance(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell);

    void logCastStart(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell);

    void logCastSuccess(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell);

    void logCastMiss(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell);

    void logSpellImpact(std::shared_ptr<unit::Unit> unit, const spell::SpellInstance &instance);

    void logBuffGain(std::shared_ptr<unit::Unit> unit, const buff::Buff& buff, int stacks = 1);

    void logBuffExpire(std::shared_ptr<unit::Unit> unit, const buff::Buff& buff);

    void logDebuffGain(const debuff::Debuff& debuff, int stacks = 1);

    void logDebuffExpire(const debuff::Debuff& debuff);

    void logManaGain(std::shared_ptr<unit::Unit> unit, double mana, const std::string &source);

    void logUnitSpawn(std::shared_ptr<unit::Unit> unit);

    void logUnitDespawn(std::shared_ptr<unit::Unit> unit);

    std::string jsonLog() const;

    std::string getLog() const;

    void addLog(std::shared_ptr<unit::Unit> unit, LogType type, const std::string& text);

    void clearLog();

    void initSpellStats(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell);

    std::string spellStats();
};
