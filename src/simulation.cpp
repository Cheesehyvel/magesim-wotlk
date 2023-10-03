#include "common.h"
#include "spell.h"
#include "buff.h"
#include "debuff.h"
#include "cooldown.h"
#include "unit.h"
#include "target.h"
#include "log.h"
#include "player.h"
#include "state.h"
#include "config.h"
#include "simulation.h"

#include <string>
#include <memory>
#include <vector>
#include <iomanip>
#include <unordered_map>
#include <sstream>
#include <algorithm>

Simulation::Simulation(const Config& _config, std::shared_ptr<unit::Player> _player)
    : config(_config), player(_player), state(_config)
{
    for (int i=1; i<=config.targets; i++)
        state.addTarget(i);
}

void Simulation::reset()
{
    clearLog();
    state.reset();
    player->reset();
}

SimulationsResult Simulation::runMultiple(int iterations)
{
    SimulationResult r;
    SimulationsResult result;

    logging = iterations == 1;
    logging_spells = true;

    constexpr double bin_size = 50;

    std::unordered_map<spell::ID, SpellStats> spells;
    std::unordered_map<int, int> histogram;
    std::string results{ "" };

    if (config.additional_data)
        results += "DPS,Duration\n";

    for (int i = 0; i < iterations; i++) {
        if (config.rng_seed)
            setRNGSeed(config.rng_seed + i);

        r = run();

        if (i == 0 || r.dps < result.min_dps)
            result.min_dps = r.dps;
        if (i == 0 || r.dps > result.max_dps)
            result.max_dps = r.dps;
        result.avg_dps += (r.dps - result.avg_dps) / (i + 1);

        result.t_gcd_capped += (r.t_gcd_capped - result.t_gcd_capped) / (i + 1);

        auto const bin = static_cast<int>(floor(r.dps / bin_size) * bin_size);
        if (histogram.find(bin) != histogram.end())
            histogram[bin]++;
        else
            histogram[bin] = 1;

        if (logging_spells) {
            for (auto itr = state.spells.begin(); itr != state.spells.end(); itr++) {
                if (spells.find(itr->first) == spells.end()) {
                    spells[itr->first].name = itr->second.name;
                    spells[itr->first].source = itr->second.source;
                    spells[itr->first].min_dmg = itr->second.min_dmg;
                    spells[itr->first].max_dmg = itr->second.max_dmg;
                }
                spells[itr->first].casts += (itr->second.casts - spells[itr->first].casts) / (i+1.0);
                spells[itr->first].misses += (itr->second.misses - spells[itr->first].misses) / (i+1.0);
                spells[itr->first].hits += (itr->second.hits - spells[itr->first].hits) / (i+1.0);
                spells[itr->first].crits += (itr->second.crits - spells[itr->first].crits) / (i+1.0);
                spells[itr->first].dmg += (itr->second.dmg - spells[itr->first].dmg) / (i+1.0);
                if (itr->second.max_dmg > spells[itr->first].max_dmg)
                    spells[itr->first].max_dmg = itr->second.max_dmg;
                if (itr->second.min_dmg < spells[itr->first].min_dmg)
                    spells[itr->first].min_dmg = itr->second.min_dmg;
            }
        }

        if (config.additional_data)
            results += std::to_string(r.dps) + "," + std::to_string(r.t) + "\n";
    }

    result.iterations = iterations;

    if (config.additional_data)
        result.all_results = std::move(results);

    if (logging_spells)
        result.spells = spellStats(spells);

    // Histogram json string
    std::string ss{ "{" };
    for (auto itr = histogram.begin(); itr != histogram.end(); itr++) {
        if (itr != histogram.begin())
            ss += ",";
        ss += "\"" + std::to_string(itr->first) + "\":" + std::to_string(itr->second);
    }
    ss += "}";
    result.histogram = std::move(ss);

    return result;
}

SimulationResult Simulation::run(bool single)
{
    if (single && config.rng_seed)
        setRNGSeed(config.rng_seed);

    reset();

    runPrecombat();

    onManaRegen(player);

    for (auto &timing : config.timings)
    {
        if (timing.name == "bloodlust")
            pushBuffGainAll(std::make_shared<buff::Bloodlust>(), timing.t);
        else if (timing.name == "innervate")
            pushBuffGain(player, std::make_shared<buff::Innervate>(), timing.t);
        else if (timing.name == "power_infusion")
            pushBuffGain(player, std::make_shared<buff::PowerInfusion>(), timing.t);
        else if (timing.name == "mana_tide")
            pushBuffGain(player, std::make_shared<buff::ManaTide>(), timing.t);
        else if (timing.name == "hodir_storm_power")
            pushBuffGain(player, std::make_shared<buff::HodirStormPower>(), timing.t);
        else if (timing.name == "hodir_starlight")
            pushBuffGain(player, std::make_shared<buff::HodirStarlight>(), timing.t);
        else if (timing.name == "hodir_singed")
            pushDebuffGain(state.targets[0], std::make_shared<debuff::HodirSinged>(), timing.t);
        else if (timing.name == "iron_council_shield_of_runes")
            pushBuffGain(player, std::make_shared<buff::IronCouncilShieldOfRunes>(), timing.t);
        else if (timing.name == "icehowl_staggered_daze")
            pushDebuffGain(state.targets[0], std::make_shared<debuff::IcehowlStaggeredDaze>(), timing.t);
    }

    for (int i=0; i<config.interruptions.size(); i++) {
        pushInterruption(i);
    }

    if (player->talents.focus_magic) {
        pushBuffGain(player, std::make_shared<buff::FocusMagic>(), 5.0);
    }

    workCurrent();

    if (!state.isSilenced() && !player->shouldPreCast())
        nextAction(player);

    work();

    SimulationResult result;
    if (config.only_main_dmg)
        result.dmg = state.mainDmg();
    else
        result.dmg = state.totalDmg();
    result.t = state.t;
    result.dps = result.dmg / state.t;
    result.t_gcd_capped = player->t_gcd_capped;

    if (logging) {
        result.log = jsonLog();
        result.spells = spellStats(state.spells);
    }

    return result;
}

void Simulation::runPrecombat()
{
    double t = player->preCombatDuration(state);
    if (t <= 0)
        t += player->gcd();

    state.t = player->t_gcd = 0.0 - t;
    int i = 0;
    while (nextAction(player)) {
        workCurrent();

        if (player->t_gcd != state.t)
            state.t = player->t_gcd;

        if (state.t >= 0 && config.pre_potion && !player->hasCooldown(cooldown::POTION)) {
            state.t = -0.1;
            usePotion(player, config.pre_potion);
        }

        if (state.t >= 0)
            break;
    }

    state.t = player->t_gcd = 0;
}

void Simulation::workCurrent()
{
    while (true)
    {
        auto event = queue.front();
        if (event.t != state.t)
            return;
        queue.pop_front();
        tick(event);
    }
}

void Simulation::work()
{
    while (true)
    {
        auto event = queue.front();
        queue.pop_front();

        if (event.t >= state.duration)
        {
            state.t = state.duration;
            break;
        }

        tick(event);
    }

    queue.clear();
}

void Simulation::tick(Event& event)
{
    state.t = event.t;

    switch (event.type)
    {
        case EVENT_NONE:
        case EVENT_DOT:
            break;
        case EVENT_CAST_START:
            cast(event.unit, event.spell, event.target);
            break;
        case EVENT_CAST_FINISH:
            onCastFinish(event.unit, event.spell, event.target);
            break;
        case EVENT_SPELL_IMPACT:
            onSpellImpact(event.unit, event.instance, event.target);
            break;
        case EVENT_SPELL_TICK:
            onSpellTick(event.unit, event.spell, event.target, event.tick);
            break;
        case EVENT_MANA_REGEN:
            onManaRegen(event.unit);
            break;
        case EVENT_MANA_GAIN:
            onManaGain(event.unit, event.mana, event.source);
            break;
        case EVENT_BUFF_GAIN:
            onBuffGain(event.unit, event.buff);
            break;
        case EVENT_BUFF_EXPIRE:
            onBuffExpire(event.unit, event.buff);
            break;
        case EVENT_BUFF_GAIN_ALL:
            onBuffGainAll(event.buff);
            break;
        case EVENT_DEBUFF_GAIN:
            onDebuffGain(event.target, event.debuff);
            break;
        case EVENT_DEBUFF_EXPIRE:
            onDebuffExpire(event.target, event.debuff);
            break;
        case EVENT_CD_GAIN:
            onCooldownGain(event.unit, event.cooldown);
            break;
        case EVENT_CD_EXPIRE:
            onCooldownExpire(event.unit, event.cooldown);
            break;
        case EVENT_UNIT:
            onUnitSpawn(event.unit);
            break;
        case EVENT_UNIT_DESPAWN:
            onUnitDespawn(event.unit);
            break;
        case EVENT_WAIT:
            onWait(event.unit, event.spell, event.target);
            break;
        case EVENT_INTERRUPTION:
            onInterruption(event.interruption_index);
            break;
        case EVENT_INTERRUPTION_END:
            onInterruptionEnd(event.interruption_index);
            break;
    }
}

void Simulation::push(Event& event)
{
    event.t += state.t;

    for (auto itr = queue.begin(); itr != queue.end(); itr++) {
        if (event.t < itr->t) {
            queue.insert(itr, event);
            return;
        }
    }

    queue.push_back(event);
}

void Simulation::pushCastStart(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target, double t)
{
    Event event;
    event.type = EVENT_CAST_START;
    event.spell = spell;
    event.unit = unit;
    event.target = target;
    event.t = t;

    push(event);
}

void Simulation::pushCastFinish(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target, double t)
{
    Event event;
    event.type = EVENT_CAST_FINISH;
    event.spell = spell;
    event.unit = unit;
    event.target = target;
    event.t = t;

    push(event);
}

void Simulation::pushSpellImpact(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target, double t)
{
    Event event;
    event.type = EVENT_SPELL_IMPACT;
    event.instance = getSpellInstance(unit, spell, target);
    event.unit = unit;
    event.target = target;
    event.t = t;

    push(event);
}

void Simulation::pushSpellImpact(std::shared_ptr<unit::Unit> unit, spell::SpellInstance &instance, std::shared_ptr<target::Target> target, double t)
{
    Event event;
    event.type = EVENT_SPELL_IMPACT;
    event.instance = instance;
    event.unit = unit;
    event.target = target;
    event.t = t;

    push(event);
}

void Simulation::pushChannelingTick(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target, double t, int tick)
{
    Event event;
    event.type = EVENT_SPELL_TICK;
    event.spell = spell;
    event.unit = unit;
    event.target = target;
    event.t = t;
    event.tick = tick;

    push(event);
}

void Simulation::pushDot(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target, int tick)
{
    auto instance = getSpellInstance(unit, spell, target);
    instance.tick = tick;

    Event event;
    event.type = EVENT_SPELL_IMPACT;
    event.instance = instance;
    event.unit = unit;
    event.target = target;
    event.t = tick * spell->t_interval;

    push(event);
}

void Simulation::pushDotTick(std::shared_ptr<unit::Unit> unit, spell::SpellInstance &instance, std::shared_ptr<target::Target> target)
{
    Event event;
    event.type = EVENT_SPELL_IMPACT;
    event.instance = instance;
    event.unit = unit;
    event.target = target;
    event.t = instance.tick * instance.spell->t_interval;

    push(event);
}

void Simulation::pushManaRegen(std::shared_ptr<unit::Unit> unit)
{
    Event event;
    event.type = EVENT_MANA_REGEN;
    event.unit = unit;
    event.t = MANA_TICK_T;
    push(event);
}

void Simulation::pushManaGain(std::shared_ptr<unit::Unit> unit, double t, double mana, const std::string &source)
{
    Event event;
    event.type = EVENT_MANA_GAIN;
    event.t = t;
    event.unit = unit;
    event.mana = mana;
    event.source = source;

    push(event);
}

void Simulation::pushBuffGain(std::shared_ptr<unit::Unit> unit, std::shared_ptr<buff::Buff> buff, double t)
{
    Event event;
    event.type = EVENT_BUFF_GAIN;
    event.t = t;
    event.unit = unit;
    event.buff = buff;

    push(event);
}

void Simulation::pushBuffGainAll(std::shared_ptr<buff::Buff> buff, double t)
{
    Event event;
    event.type = EVENT_BUFF_GAIN_ALL;
    event.t = t;
    event.buff = buff;

    push(event);
}

void Simulation::pushBuffExpire(std::shared_ptr<unit::Unit> unit, std::shared_ptr<buff::Buff> buff, double t)
{
    Event event;
    event.type = EVENT_BUFF_EXPIRE;
    event.t = t == 0 ? buff->duration : t;
    event.unit = unit;
    event.buff = buff;

    push(event);
}

void Simulation::pushDebuffGain(std::shared_ptr<target::Target> target, std::shared_ptr<debuff::Debuff> debuff, double t)
{
    Event event;
    event.type = EVENT_DEBUFF_GAIN;
    event.t = t;
    event.target = target;
    event.debuff = debuff;

    push(event);
}

void Simulation::pushDebuffExpire(std::shared_ptr<target::Target> target, std::shared_ptr<debuff::Debuff> debuff)
{
    Event event;
    event.type = EVENT_DEBUFF_EXPIRE;
    event.t = debuff->duration;
    event.target = target;
    event.debuff = debuff;

    push(event);
}

void Simulation::pushCooldownGain(std::shared_ptr<unit::Unit> unit, std::shared_ptr<cooldown::Cooldown> cooldown, double t)
{
    Event event;
    event.type = EVENT_CD_GAIN;
    event.t = t;
    event.unit = unit;
    event.cooldown = cooldown;

    push(event);
}

void Simulation::pushCooldownExpire(std::shared_ptr<unit::Unit> unit, std::shared_ptr<cooldown::Cooldown> cooldown)
{
    Event event;
    event.type = EVENT_CD_EXPIRE;
    event.t = cooldown->duration;
    event.unit = unit;
    event.cooldown = cooldown;

    push(event);
}

void Simulation::pushUnitDespawn(std::shared_ptr<unit::Unit> unit, double t)
{
    Event event;
    event.type = EVENT_UNIT_DESPAWN;
    event.t = t;
    event.unit = unit;

    push(event);
}

void Simulation::pushWait(std::shared_ptr<unit::Unit> unit, double t, const std::string &str, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target)
{
    Event event;
    event.type = EVENT_WAIT;
    event.unit = unit;
    event.t = t;
    event.spell = spell;
    event.target = target;

    push(event);

    if (!str.empty())
        addLog(unit, LOG_WAIT, str + ", " + unit->name + " waiting " + std::to_string(t) + " seconds...");
}

void Simulation::pushInterruption(int index)
{
    Event start;
    start.type = EVENT_INTERRUPTION;
    start.t = config.interruptions[index].t;
    start.interruption_index = index;
    push(start);

    Event end;
    end.type = EVENT_INTERRUPTION_END;
    end.t = config.interruptions[index].t + config.interruptions[index].duration;
    end.interruption_index = index;
    push(end);
}

void Simulation::onAction(std::shared_ptr<unit::Unit> unit, action::Action &action)
{
    if (action.type == action::TYPE_WAIT) {
        pushWait(unit, action.value, action.str, action.spell, action.target);
    }
    else if (action.type == action::TYPE_SPELL) {
        cast(unit, action.spell, action.target);
        if (action.cooldown)
            onCooldownGain(unit, action.cooldown);
    }
    else if (action.type == action::TYPE_BUFF) {
        onBuffGain(unit, action.buff);
        if (action.cooldown)
            onCooldownGain(unit, action.cooldown);
    }
    else if (action.type == action::TYPE_BUFF_EXPIRE) {
        onBuffExpire(unit, action.buff);
        if (action.cooldown)
            onCooldownGain(unit, action.cooldown);
    }
    else if (action.type == action::TYPE_DEBUFF) {
        onDebuffGain(action.target, action.debuff);
        if (action.cooldown)
            onCooldownGain(unit, action.cooldown);
    }
    else if (action.type == action::TYPE_COOLDOWN) {
        onCooldownGain(unit, action.cooldown);
    }
    else if (action.type == action::TYPE_COOLDOWN_EXPIRE) {
        onCooldownExpire(unit, action.cooldown);
    }
    else if (action.type == action::TYPE_POTION) {
        usePotion(unit, action.potion);
    }
    else if (action.type == action::TYPE_CONJURED) {
        useConjured(unit, action.conjured);
    }
    else if (action.type == action::TYPE_TRINKET) {
        useTrinket(unit, action.trinket, action.cooldown);
    }
    else if (action.type == action::TYPE_MANA) {
        onManaGain(unit, action.value, action.str);
        if (action.cooldown)
            onCooldownGain(unit, action.cooldown);
    }
    else if (action.type == action::TYPE_UNIT) {
        onUnitSpawn(action.unit);
    }

    if (action.primary_action)
        nextAction(unit);
}

bool Simulation::nextAction(std::shared_ptr<unit::Unit> unit)
{
    auto action = unit->nextAction(state);

    if (action.type == action::TYPE_NONE)
        return false;

    onAction(unit, action);

    return true;
}

void Simulation::processActions(std::shared_ptr<unit::Unit> unit, std::vector<action::Action> &actions)
{
    for (auto & i : actions)
        onAction(unit, i);
}

void Simulation::cast(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target)
{
    if (unit->canCast(spell)) {
        if (spell->active_use && !spell->off_gcd && unit->t_gcd > state.t)
            pushWait(unit, unit->t_gcd - state.t, "GCD", spell, target);
        else
            onCastStart(unit, spell, target);
    }
    else {
        pushWait(unit, 0.5, "Out of mana", spell);
    }
}

void Simulation::onCastStart(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target)
{
    spell->actual_cast_time = unit->castTime(spell);

    logCastStart(unit, spell, target);

    if (spell->active_use) {
        unit->t_gcd = state.t + unit->gcd(spell->gcd);
        if (spell->gcd)
            unit->last_spell_cast_time = spell->actual_cast_time;
    }

    if (spell->channeling)
        onCastSuccess(unit, spell, target);
    else
        pushCastFinish(unit, spell, target, spell->actual_cast_time);
}

void Simulation::onCastFinish(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target)
{
    if (spell->tick || unit->canCast(spell))
        onCastSuccess(unit, spell, target);
    else
        nextAction(unit);
}

void Simulation::onCastSuccess(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target)
{
    spell->actual_cost = unit->manaCost(spell);
    unit->applyMana(state, -spell->actual_cost);

    if (spell->dot && spell->active_use && random<double>(0, 100) > hitChance(unit, spell, target)) {
        logCastMiss(unit, spell, target);
    }
    else {
        logCastSuccess(unit, spell, target);

        if (spell->channeling && !spell->tick)
            unit->is_channeling = true;

        if (spell->aoe) {
            for (auto const& tar : state.targets) {
                if (spell->channeling && !spell->tick) {
                    for (int i = 1; i <= spell->ticks; i++)
                        pushChannelingTick(unit, spell, tar, spell->actual_cast_time / spell->ticks * i, i);
                }
                else if (spell->dot) {
                    dotApply(unit, spell, tar);
                }
                else if (!spell->is_trigger) {
                    pushSpellImpact(unit, spell, tar, unit->travelTime(spell));
                }
            }
        }
        else {
            if (spell->channeling && !spell->tick) {
                for (int i = 1; i <= spell->ticks; i++)
                    pushChannelingTick(unit, spell, target, spell->actual_cast_time / spell->ticks * i, i);
            }
            else if (spell->dot) {
                dotApply(unit, spell, target);
            }
            else if (!spell->is_trigger) {
                pushSpellImpact(unit, spell, target, unit->travelTime(spell));
            }
        }

        if (spell->active_use)
            onCastSuccessProc(unit, spell, target);
        if (spell->proc)
            onProcSuccess(unit, spell, target);
    }

    if (spell->active_use) {
        if (state.inCombat()) {
            if (spell->channeling && spell->actual_cast_time > 0)
                pushWait(unit, spell->actual_cast_time);
            else
                nextAction(unit);
        }
    }
}

void Simulation::onSpellImpact(std::shared_ptr<unit::Unit> unit, spell::SpellInstance &instance, std::shared_ptr<target::Target> target)
{
    if (instance.spell->dot) {
        instance.resist = spellDmgResist(unit, instance);
        instance.dmg -= instance.resist;
    }

    target->dmg += static_cast<unsigned long long>(instance.dmg);
    logSpellImpact(unit, instance, target);
    onSpellImpactProc(unit, instance, target);

    // Log spell use
    if (logging_spells) {
        initSpellStats(unit, instance.spell);
        state.spells[instance.spell->id].casts++;
        if (instance.result == spell::MISS)
            state.spells[instance.spell->id].misses++;
        else if (instance.result == spell::CRIT)
            state.spells[instance.spell->id].crits++;
        else
            state.spells[instance.spell->id].hits++;

        state.spells[instance.spell->id].dmg += instance.dmg;
        if (instance.dmg > state.spells[instance.spell->id].max_dmg)
            state.spells[instance.spell->id].max_dmg = instance.dmg;
        if (instance.dmg < state.spells[instance.spell->id].min_dmg || state.spells[instance.spell->id].min_dmg == 0)
            state.spells[instance.spell->id].min_dmg = instance.dmg;
    }
}

void Simulation::onSpellTick(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target, int tick)
{
    if (!spell->is_trigger) {
        auto instance = getSpellInstance(unit, spell, target);
        instance.tick = tick;
        pushSpellImpact(unit, instance, target, unit->travelTime(spell));
    }

    onSpellTickProc(unit, spell, target, tick);
}

void Simulation::dotApply(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target)
{
    // Ignite special case
    if (spell->id == spell::IGNITE) {
        auto leftover = getDotDamage(unit, spell, target);

        // Remove pending ignite ticks
        for (auto i = queue.begin(); i != queue.end();) {
            if (i->type == EVENT_SPELL_IMPACT && i->instance.spell->id == spell->id && i->unit == unit && i->target == target)
                i = queue.erase(i);
            else
                i++;
        }

        for (int i = 1; i <= spell->ticks; i++) {
            auto dot = getSpellInstance(unit, spell, target);
            dot.tick = i;
            dot.dmg += round(leftover / 2.0);
            pushDotTick(unit, dot, target);
        }
    }
    else {
        removeSpellImpacts(unit, spell, target);

        for (int i = 1; i <= spell->ticks; i++)
            pushDot(unit, spell, target, i);
    }
}

void Simulation::onCastSuccessProc(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target)
{
    auto actions = unit->onCastSuccessProc(state, spell, target);
    processActions(unit, actions);

    // Special for fetish of volatile power
    if (unit->id != 1) {
        if (player->hasTrinkets(TRINKET_FETISH_VOLATILE_POWER_HC, TRINKET_TALISMAN_VOLATILE_POWER_HC) && player->hasBuff(buff::VOLATILE_POWER_HC) && spell->max_dmg > 0) {
            onBuffGain(player, std::make_shared<buff::VolatilityHc>());
        }
        if (player->hasTrinkets(TRINKET_FETISH_VOLATILE_POWER_NM, TRINKET_TALISMAN_VOLATILE_POWER_NM) && player->hasBuff(buff::VOLATILE_POWER_NM) && spell->max_dmg > 0) {
            onBuffGain(player, std::make_shared<buff::VolatilityNm>());
        }
    }
}

void Simulation::onSpellImpactProc(std::shared_ptr<unit::Unit> unit, const spell::SpellInstance &instance, std::shared_ptr<target::Target> target)
{
    std::vector<action::Action> actions = unit->onSpellImpactProc(state, instance, target);
    processActions(unit, actions);
}

void Simulation::onSpellTickProc(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target, int tick)
{
    std::vector<action::Action> actions = unit->onSpellTickProc(state, spell, target, tick);
    processActions(unit, actions);
}

void Simulation::onProcSuccess(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target)
{
    std::vector<action::Action> actions = unit->onProcSuccess(state, spell, target);
    processActions(unit, actions);
}

void Simulation::onUnitSpawn(std::shared_ptr<unit::Unit> unit)
{
    if (unit->unique)
        despawnDuplicate(unit);

    unit->reset();
    logUnitSpawn(unit);
    state.addUnit(unit);

    if (unit->duration)
        pushUnitDespawn(unit, unit->duration);

    if (state.inCombat())
        pushWait(unit, 0.25);
    else
        pushWait(unit, -state.t);
}

void Simulation::onUnitDespawn(std::shared_ptr<unit::Unit> unit)
{
    logUnitDespawn(unit);
    removeUnitEvents(unit);
    state.removeUnit(unit);
}

void Simulation::despawnDuplicate(std::shared_ptr<unit::Unit> unit)
{
    for (auto const& i : state.units)
        if (i->id == unit->id)
        {
            onUnitDespawn(i);
            break;
        }
}

void Simulation::onManaRegen(std::shared_ptr<unit::Unit> unit, bool next)
{
    onManaGain(unit, unit->manaPerTick(state), "Mana Regen");
    if (next)
        pushManaRegen(unit);
}

void Simulation::onManaGain(std::shared_ptr<unit::Unit> unit, double mana, const std::string &source)
{
    unit->applyMana(state, mana);
    logManaGain(unit, mana, source);
}

void Simulation::onWait(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target)
{
    if (unit->is_channeling) {
        unit->removeSnapshots();
        unit->is_channeling = false;
    }

    if (spell != NULL)
        cast(unit, spell, target);
    else
        nextAction(unit);
}

void Simulation::onInterruption(int index)
{
    logInterruption(&config.interruptions[index]);

    state.activateInterruption(index);

    player->interrupt(config.interruptions[index]);

    if (config.interruptions[index].affects_all) {
        for (auto i = state.units.begin(); i != state.units.end(); ++i)
            (*i)->interrupt(config.interruptions[index]);
    }

    for (auto i = queue.begin(); i != queue.end();) {
        if ((i->type == EVENT_CAST_FINISH || i->type == EVENT_SPELL_TICK || i->type == EVENT_WAIT) &&
            (i->unit->id == player->id || config.interruptions[index].affects_all))
        {
            i = queue.erase(i);
        }
        else {
            ++i;
        }
    }

    if (!config.interruptions[index].silence && state.t > 0) {
        nextAction(player);

        if (config.interruptions[index].affects_all) {
            for (auto i = state.units.begin(); i != state.units.end(); ++i)
                nextAction(*i);
        }
    }
}

void Simulation::onInterruptionEnd(int index)
{
    state.deactivateInterruption(index);

    if (config.interruptions[index].silence && state.t > 0 && !state.isSilenced()) {
        nextAction(player);

        if (config.interruptions[index].affects_all && !state.isSilenced(false)) {
            for (auto i = state.units.begin(); i != state.units.end(); ++i)
                nextAction(*i);
        }
    }
}

void Simulation::onBuffGain(std::shared_ptr<unit::Unit> unit, std::shared_ptr<buff::Buff> buff)
{
    if (buff->cost) {
        if (!unit->canBuff(*buff))
            return;
        unit->applyMana(state, -unit->manaCost(*buff));
    }

    int old_stacks = unit->buffStacks(buff->id);

    buff->t_refreshed = state.t;
    if (old_stacks < 1)
        buff->t_gained = state.t;

    int stacks = unit->addBuff(buff);

    if (old_stacks < 1 || buff->stack_refresh) {
        removeBuffExpiration(unit, buff);
        pushBuffExpire(unit, buff);
    }

    if (buff->id == buff::MANA_TIDE) {
        for (double t = 3; t <= 12; t += 3)
            pushManaGain(unit, t, unit->maxMana() * 0.06, "Mana Tide");
    }
    else if (buff->id == buff::INNERVATE) {
        for (double t = 1; t <= 10; t += 1)
            pushManaGain(unit, t, 3496 * .225, "Innervate");
    }
    else if (buff->id == buff::DISLODGED_OBJECT_HC && stacks < 10) {
        pushBuffGain(unit, buff, 2);
    }
    else if (buff->id == buff::DISLODGED_OBJECT_NM && stacks < 10) {
        pushBuffGain(unit, buff, 2);
    }

    if (stacks != old_stacks || buff->show_refresh)
        logBuffGain(unit, buff, stacks);

    auto actions = unit->onBuffGain(state, buff);
    processActions(unit, actions);
}

void Simulation::onBuffExpire(std::shared_ptr<unit::Unit> unit, std::shared_ptr<buff::Buff> buff)
{
    bool snapshot = false;
    if (buff->snapshot && unit->is_channeling && buffDuration(unit, buff->id) > 0)
        snapshot = true;

    removeBuffExpiration(unit, buff);
    logBuffExpire(unit, buff);
    unit->removeBuff(buff->id, snapshot);

    auto actions = unit->onBuffExpire(state, buff);
    processActions(unit, actions);
}

void Simulation::onBuffGainAll(std::shared_ptr<buff::Buff> buff)
{
    onBuffGain(player, buff);

    for (auto itr = state.units.begin(); itr != state.units.end(); itr++) {
        if ((*itr)->get_raid_buffs)
            onBuffGain(*itr, buff);
    }
}

void Simulation::onDebuffGain(std::shared_ptr<target::Target> target, std::shared_ptr<debuff::Debuff> debuff)
{
    int stacks = target->addDebuff(debuff);
    removeDebuffExpiration(target, debuff);
    pushDebuffExpire(target, debuff);

    if (stacks)
        logDebuffGain(target, debuff, stacks);
}

void Simulation::onDebuffExpire(std::shared_ptr<target::Target> target, std::shared_ptr<debuff::Debuff> debuff)
{
    removeDebuffExpiration(target, debuff);
    logDebuffExpire(target, debuff);
    target->removeDebuff(debuff->id);
}

void Simulation::onCooldownGain(std::shared_ptr<unit::Unit> unit, std::shared_ptr<cooldown::Cooldown> cooldown, bool mod)
{
    if (mod)
        cooldown->duration += unit->cooldownMod(*cooldown);

    cooldown->t_gained = state.t;
    unit->addCooldown(cooldown);

    if (cooldownDuration(unit, cooldown->id) < cooldown->duration) {
        removeCooldownExpiration(unit, *cooldown);
        pushCooldownExpire(unit, cooldown);
    }
}

void Simulation::onCooldownExpire(std::shared_ptr<unit::Unit> unit, std::shared_ptr<cooldown::Cooldown> cooldown)
{
    unit->removeCooldown(cooldown->id);
}

void Simulation::usePotion(std::shared_ptr<unit::Unit> unit, Potion potion)
{
    std::vector<action::Action> actions = unit->usePotion(potion, state.inCombat());
    processActions(unit, actions);
}

void Simulation::useConjured(std::shared_ptr<unit::Unit> unit, Conjured conjured)
{
    std::vector<action::Action> actions = unit->useConjured(state, conjured);
    processActions(unit, actions);
}

void Simulation::useTrinket(std::shared_ptr<unit::Unit> unit, Trinket trinket, std::shared_ptr<cooldown::Cooldown> cooldown)
{
    std::vector<action::Action> actions = unit->useTrinket(trinket, cooldown);
    processActions(unit, actions);
}

double Simulation::getDotDamage(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target) const
{
    double ret = 0;
    for (auto const& i : queue)
        if (i.type == EVENT_SPELL_IMPACT && i.instance.spell->id == spell->id && i.unit == unit && i.target == target)
            ret += i.instance.dmg;
    return ret;
}

void Simulation::removeSpellImpact(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target)
{
    for (auto i = queue.begin(); i != queue.end(); ++i)
        if (i->type == EVENT_SPELL_IMPACT && i->instance.spell->id == spell->id && i->unit == unit && i->target == target)
        {
            queue.erase(i);
            return;
        }
}

void Simulation::removeSpellImpacts(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target)
{
    for (auto i = queue.begin(); i != queue.end();) {
        if (i->type == EVENT_SPELL_IMPACT && i->instance.spell->id == spell->id && i->unit == unit && i->target == target)
            i = queue.erase(i);
        else
            ++i;
    }
}

void Simulation::removeBuffExpiration(std::shared_ptr<unit::Unit> unit, std::shared_ptr<buff::Buff> buff)
{
    for (auto i = queue.begin(); i != queue.end(); ++i) {
        if (i->type == EVENT_BUFF_EXPIRE && i->buff->id == buff->id && i->unit == unit) {
            queue.erase(i);
            return;
        }
    }
}

void Simulation::removeDebuffExpiration(std::shared_ptr<target::Target> target, std::shared_ptr<debuff::Debuff> debuff)
{
    for (auto i = queue.begin(); i != queue.end(); ++i) {
        if (i->type == EVENT_DEBUFF_EXPIRE && i->debuff->id == debuff->id && i->target == target) {
            queue.erase(i);
            return;
        }
    }
}

void Simulation::removeCooldownExpiration(std::shared_ptr<unit::Unit> unit, const cooldown::Cooldown &cooldown)
{
    for (auto i = queue.begin(); i != queue.end(); ++i) {
        if (i->type == EVENT_CD_EXPIRE && i->cooldown->id == cooldown.id && i->unit == unit) {
            queue.erase(i);
            return;
        }
    }
}

double Simulation::buffDuration(std::shared_ptr<unit::Unit> unit, buff::ID id) const
{
    for (auto const& i : queue)
        if (i.type == EVENT_BUFF_EXPIRE && i.buff->id == id && i.unit == unit)
            return i.t - state.t;

    return 0;
}

double Simulation::cooldownDuration(std::shared_ptr<unit::Unit> unit, cooldown::ID id) const
{
    for (auto const& i : queue)
        if (i.type == EVENT_CD_EXPIRE && i.cooldown->id == id && i.unit == unit)
            return i.t - state.t;

    return 0;
}

void Simulation::removeUnitEvents(std::shared_ptr<unit::Unit> unit)
{
    for (auto i = queue.begin(); i != queue.end();)
    {
        if (i->unit == unit && i->type != EVENT_SPELL_IMPACT)
            i = queue.erase(i);
        else
            ++i;
    }
}

double Simulation::hitChance(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target) const
{
    int dlevel = config.target_level - 80;

    double hit = 96.0 - dlevel;

    if (dlevel > 2)
        hit -= (dlevel - 2) * 10.0;

    hit += unit->hitChance(spell);

    if (config.debuff_spell_hit && unit->get_raid_debuffs)
        hit += 3.0;

    return std::min(hit, 100.0);
}

double Simulation::critChance(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target) const
{
    double crit = unit->critChance(spell);
    double crit_debuff = 0;

    if (!unit->get_raid_debuffs)
        return crit;

    if (config.debuff_spell_crit || target->hasDebuff(debuff::IMPROVED_SCORCH))
        crit_debuff += 5.0;
    else if (target->hasDebuff(debuff::WINTERS_CHILL))
        crit_debuff += target->debuffStacks(debuff::WINTERS_CHILL);

    if (config.debuff_crit)
        crit_debuff += 3.0;

    if (spell->id == spell::FROSTFIRE_BOLT)
        crit_debuff *= 2;

    crit += crit_debuff;

    crit = std::min(crit, 100.0);

    // Crit suppression
    if (unit->crit_suppression) {
        if (config.target_level == 83)
            crit -= 2.1;
        if (config.target_level == 82)
            crit -= 0.3;
    }

    return crit;
}

double Simulation::critMultiplier(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell) const
{
    double base = 1.5;
    double multi = 1;

    base *= unit->baseCritMultiplier(spell);

    if (spell->proc)
        return base;

    multi *= unit->critMultiplierMod(spell);

    return (base - 1) * multi + 1;
}

double Simulation::buffDmgMultiplier(const std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell) const
{
    double multi = 1;

    multi *= unit->buffDmgMultiplier(spell, state);

    return multi;
}

double Simulation::debuffDmgMultiplier(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target) const
{
    double multi = 1;

    if (!unit->get_raid_debuffs)
        return multi;

    if (config.debuff_spell_dmg)
        multi *= 1.13;

    if (target->hasDebuff(debuff::HODIR_SINGED))
        multi *= 1.5;
    if (target->hasDebuff(debuff::ICEHOWL_STAGGERED_DAZE))
        multi *= 2;

    return multi;
}

double Simulation::spellDmg(const std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target)
{
    double dmg;

    if (config.avg_spell_dmg)
        dmg = spell->avgDmg();
    else
        dmg = random<double>(spell->min_dmg, spell->max_dmg);

    if (spell->fixed_dmg)
        return dmg;

    if (spell->coeff) {
        auto const sp = unit->getSpellPower(spell->school);
        double coeff = spell->coeff;

        coeff += unit->spellCoeffMod(spell);

        if (spell->channeling)
            coeff /= spell->ticks;

        dmg += sp * coeff;
    }

    dmg *= buffDmgMultiplier(unit, spell);

    if (spell->aoe && spell->aoe_capped && config.targets > 10)
        dmg *= 10.0 / config.targets;

    dmg *= debuffDmgMultiplier(unit, spell, target);

    return dmg;
}

double Simulation::spellDmgResist(std::shared_ptr<unit::Unit> unit, const spell::SpellInstance &instance)
{
    if (instance.spell->binary)
        return 0.0;

    // No confirmed formulas or resistance tables can be found
    // This resistance table is based on data from Naxxramas in Wotlk Beta uploaded to WCL
    // It results in 5.9% mitigation

    int resist[3] = { 55, 31, 14 };
    int roll = random<int>(0, 99);

    double resistance_multiplier = 0.0;
    for (int i = 0; i < 3; i++) {
        if (roll < resist[i]) {
            resistance_multiplier = ((float)i) * 0.1;
            break;
        }

        roll -= resist[i];
    }

    if (!resistance_multiplier)
        return 0.0;

    return round(instance.dmg * resistance_multiplier);
}

spell::Result Simulation::getSpellResult(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target) const
{
    if (unit->canMiss(spell) && random<double>(0, 100) > hitChance(unit, spell, target))
        return spell::MISS;

    if (unit->canCrit(spell) && random<double>(0, 100) <= critChance(unit, spell, target))
        return spell::CRIT;

    return spell::HIT;
}

spell::SpellInstance Simulation::getSpellInstance(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target)
{
    spell::SpellInstance instance;

    instance.spell = spell;

    if (spell->max_dmg > 0) {
        instance.result = getSpellResult(unit, spell, target);

        if (instance.result != spell::MISS) {
            instance.dmg = spellDmg(unit, spell, target);

            if (instance.result == spell::CRIT)
                instance.dmg *= critMultiplier(unit, spell);

            if (unit->canResist(spell)) {
                instance.resist = spellDmgResist(unit, instance);
                instance.dmg -= instance.resist;
            }

            instance.dmg = round(instance.dmg);
        }
    }

    return instance;
}

void Simulation::logCastStart(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target)
{
    if (!logging || !spell->active_use || !spell->cast_time)
        return;

    if (target)
        addLog(unit, LOG_CAST_START, unit->name + " started casting " + spell->name + " on "+target->name+".");
    else
        addLog(unit, LOG_CAST_START, unit->name + " started casting " + spell->name + ".");
}

void Simulation::logCastSuccess(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target)
{
    if (!logging || !spell->active_use)
        return;

    if (target)
        addLog(unit, LOG_CAST_SUCCESS, unit->name + " successfully cast " + spell->name + " on "+target->name+".");
    else
        addLog(unit, LOG_CAST_SUCCESS, unit->name + " successfully cast " + spell->name + ".");
}

void Simulation::logCastMiss(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target)
{
    if (!logging || !spell->active_use)
        return;

    if (target)
        addLog(unit, LOG_CAST_SUCCESS, unit->name + "'s " + spell->name+" missed "+target->name+".");
    else
        addLog(unit, LOG_CAST_SUCCESS, unit->name + "'s " + spell->name+" missed.");
}

void Simulation::logSpellImpact(std::shared_ptr<unit::Unit> unit, const spell::SpellInstance &instance, std::shared_ptr<target::Target> target)
{
    if (!logging)
        return;

    std::string s = unit->name + "'s " + instance.spell->name;

    if (instance.spell->dot)
        s += " (dot)";
    if (instance.result == spell::MISS)
        s += " missed";
    else if (instance.result == spell::CRIT)
        s += " crit for " + std::to_string(static_cast<unsigned int>(instance.dmg));
    else
        s += " hit for " + std::to_string(static_cast<unsigned int>(instance.dmg));

    if (instance.resist)
        s += " (" + std::to_string(static_cast<unsigned int>(instance.resist)) + " resisted)";

    if (target)
        s += " on "+target->name;

    s += ".";

    addLog(unit, LOG_SPELL_IMPACT, s);
}

void Simulation::logBuffGain(std::shared_ptr<unit::Unit> unit, std::shared_ptr<buff::Buff> buff, int stacks)
{
    if (!logging || buff->hidden)
        return;

    std::string s = unit->name + " gained " + buff->name;
    if (buff->max_stacks > 1)
        s += " (" + std::to_string(stacks) + ")";
    s += ".";

    addLog(unit, LOG_BUFF, s);
}

void Simulation::logBuffExpire(std::shared_ptr<unit::Unit> unit, std::shared_ptr<buff::Buff> buff)
{
    if (!logging || buff->hidden)
        return;

    addLog(unit, LOG_BUFF, unit->name + " lost " + buff->name + ".");
}

void Simulation::logDebuffGain(std::shared_ptr<target::Target> target, std::shared_ptr<debuff::Debuff> debuff, int stacks)
{
    if (!logging || debuff->hidden)
        return;

    std::string s = target->name+" gained " + debuff->name;
    if (debuff->max_stacks > 1)
        s += " (" + std::to_string(stacks) + ")";
    s += ".";

    addLog(player, LOG_BUFF, s);
}

void Simulation::logDebuffExpire(std::shared_ptr<target::Target> target, std::shared_ptr<debuff::Debuff> debuff)
{
    if (!logging || debuff->hidden)
        return;

    addLog(player, LOG_BUFF, target->name+" lost " + debuff->name + ".");
}

void Simulation::logManaGain(std::shared_ptr<unit::Unit> unit, double mana, const std::string &source)
{
    if (!logging)
        return;

    std::string s = unit->name;

    if (mana < 0)
        s += " lost " + std::to_string(static_cast<unsigned int>(0 - mana));
    else
        s += " gained " + std::to_string(static_cast<unsigned int>(mana));

    s += " mana from " + source + ".";

    addLog(unit, LOG_MANA, s);
}

void Simulation::logUnitSpawn(std::shared_ptr<unit::Unit> unit)
{
    if (!logging)
        return;

    addLog(unit, LOG_UNIT, unit->name + " spawned");
}

void Simulation::logUnitDespawn(std::shared_ptr<unit::Unit> unit)
{
    if (!logging)
        return;

    addLog(unit, LOG_UNIT, unit->name + " despawned");
}

void Simulation::logInterruption(const Interruption* interruption)
{
    if (!logging)
        return;

    std::string s = "Interruption ";

    if (interruption->silence)
        s+= "(silence)";
    else
        s+= "(movement)";

    s+= " on player";

    if (interruption->affects_all)
        s+= " and pets";

    s+= " for "+std::to_string(static_cast<unsigned int>(interruption->duration))+" seconds.";

    addLog(player, LOG_INTERRUPTION, s);
}

std::string Simulation::jsonLog() const
{
    std::ostringstream s;

    s << "[";

    for (int i = 0; i < log.size(); i++) {
        if (i > 0)
            s << ",";
        s << "{";
        if (log[i].unit)
            s << "\"unit\":\"" << log[i].unit->name << "\"";
        else
            s << "\"unit\":\"Unknown\"";
        s << ",\"text\":\"" << log[i].text << "\"";
        s << ",\"t\":" << log[i].t;
        s << ",\"type\":" << log[i].type;
        s << ",\"dmg\":" << log[i].dmg;
        s << ",\"mana\":" << log[i].mana;
        s << ",\"mana_percent\":" << log[i].mana_percent;
        s << "}";
    }

    s << "]";

    return s.str();
}

void Simulation::addLog(std::shared_ptr<unit::Unit> unit, LogType type, const std::string &text)
{
    if (!logging)
        return;

    log.emplace_back(type, unit, text, state.t, unit->mana, unit->manaPercent(), state.totalDmg());
}

void Simulation::clearLog()
{
    log.clear();
}

void Simulation::initSpellStats(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell)
{
    if (state.spells.find(spell->id) == state.spells.end()) {
        state.spells[spell->id].name = spell->name;
        if (spell->dot)
            state.spells[spell->id].name += " (dot)";
        state.spells[spell->id].source = unit->name;
    }
}

std::string Simulation::spellStats(std::unordered_map<spell::ID, SpellStats> spells)
{
    std::string s{ "[" };

    for (auto itr = spells.begin(); itr != spells.end(); itr++) {
        if (itr != spells.begin())
            s += ",";
        s += "{";
        s += "\"id\":" + std::to_string(itr->first) + ",";
        s += "\"name\":\"" + itr->second.name + "\",";
        s += "\"source\":\"" + itr->second.source + "\",";
        s += "\"casts\":" + std::to_string(itr->second.casts) + ",";
        s += "\"misses\":" + std::to_string(itr->second.misses) + ",";
        s += "\"hits\":" + std::to_string(itr->second.hits) + ",";
        s += "\"crits\":" + std::to_string(itr->second.crits) + ",";
        s += "\"min_dmg\":" + std::to_string(itr->second.min_dmg) + ",";
        s += "\"max_dmg\":" + std::to_string(itr->second.max_dmg) + ",";
        s += "\"dmg\":" + std::to_string(itr->second.dmg);
        s += "}";
    }

    s += "]";

    return s;
}

