using namespace std;

class Event
{

public:
    double t;
    double mana;
    double tick;
    string source;
    EventType type;
    shared_ptr<unit::Unit> unit;
    shared_ptr<spell::Spell> spell;
    shared_ptr<spell::SpellInstance> instance;
    shared_ptr<buff::Buff> buff;
    shared_ptr<debuff::Debuff> debuff;
    shared_ptr<cooldown::Cooldown> cooldown;

};

class Simulation
{

public:
    bool logging = true;
    list<shared_ptr<Event>> queue;
    vector<shared_ptr<LogEntry>> log;
    shared_ptr<State> state;
    shared_ptr<unit::Player> player;
    shared_ptr<Config> config;

    Simulation(shared_ptr<Config> _config, shared_ptr<unit::Player> _player)
    {
        config = _config;
        player = _player;
        state = make_shared<State>(config);
    }

    void reset()
    {
        clearLog();
        state->reset();
        player->reset();

        for (int i=0; i<config->timings.size(); i++)
            config->timings[i]->used = false;
    }

    SimulationsResult runMultiple(int iterations)
    {
        SimulationResult r;
        SimulationsResult result;

        logging = false;

        double bin_size = 20;
        int bin;
        map<int, int> histogram;
        ostringstream results;

        if (config->additional_data)
            results << "DPS,Duration\n";

        for (int i=0; i<iterations; i++) {
            if (config->rng_seed)
                setRNGSeed(config->rng_seed+i);

            r = run();

            if (i == 0 || r.dps < result.min_dps)
                result.min_dps = r.dps;
            if (i == 0 || r.dps > result.max_dps)
                result.max_dps = r.dps;
            result.avg_dps+= (r.dps - result.avg_dps) / (i+1);

            result.t_gcd_capped+= (r.t_gcd_capped - result.t_gcd_capped) / (i+1);

            bin = floor(r.dps/bin_size)*bin_size;
            if (histogram.find(bin) != histogram.end())
                histogram[bin]++;
            else
                histogram[bin] = 1;

            if (config->additional_data)
                results << r.dps << "," << r.t << "\n";
        }

        result.iterations = iterations;

        if (config->additional_data)
            result.all_results = results.str();

        // Histogram json string
        ostringstream ss;
        ss << "{";
        for (auto itr = histogram.begin(); itr != histogram.end(); itr++) {
            if (itr != histogram.begin())
                ss << ",";
            ss << "\"" << itr->first << "\":" << itr->second;
        }
        ss << "}";
        result.histogram = ss.str();

        return result;
    }

    SimulationResult run(bool single = false)
    {
        if (single && config->rng_seed)
            setRNGSeed(config->rng_seed);

        reset();

        runPrecombat();

        onManaRegen(player);

        for (int i=0; i<config->timings.size(); i++) {
            if (config->timings[i]->name == "bloodlust")
                pushBuffGainAll(make_shared<buff::Bloodlust>(), config->timings[i]->t);
            else if (config->timings[i]->name == "innervate")
                pushBuffGain(player, make_shared<buff::Innervate>(), config->timings[i]->t);
            else if (config->timings[i]->name == "power_infusion")
                pushBuffGain(player, make_shared<buff::PowerInfusion>(), config->timings[i]->t);
            else if (config->timings[i]->name == "mana_tide")
                pushBuffGain(player, make_shared<buff::ManaTide>(), config->timings[i]->t);
        }

        if (player->talents.focus_magic) {
            pushBuffGain(player, make_shared<buff::FocusMagic>(), 5.0);
        }

        workCurrent();
        nextAction(player);
        work();

        SimulationResult result;
        result.dmg = state->dmg;
        result.t = state->t;
        result.dps = state->dmg/state->t;
        result.t_gcd_capped = player->t_gcd_capped;

        if (logging) {
            result.log = jsonLog();
            result.spells = spellStats();
        }

        return result;
    }

    void runPrecombat()
    {
        double t = 0;

        if (config->pre_mirror_image)
            t-= 1.5;
        if (player->talents.water_elemental && config->pre_water_elemental)
            t-= 1.5;
        if (player->talents.incanters_absorption && config->pre_incanters_absorption)
            t-= 1.5;
        if (t >= 0)
            t-= 1.5;

        state->t = player->t_gcd = t;
        int i = 0;
        while (nextAction(player)) {
            workCurrent();
            if (player->t_gcd != state->t)
                state->t = player->t_gcd;
            workCurrent();

            if (state->t >= 0)
                break;
        }

        state->t = player->t_gcd = 0;
    }

    void workCurrent()
    {
        shared_ptr<Event> event;

        while (true) {
            event = queue.front();
            if (event->t != state->t)
                return;
            queue.pop_front();
            tick(event);
        }
    }

    void work()
    {
        shared_ptr<Event> event;

        while (true) {
            event = queue.front();
            queue.pop_front();

            if (event->t >= state->duration) {
                state->t = state->duration;
                break;
            }

            tick(event);
        }

        queue.clear();
    }

    void tick(shared_ptr<Event> event)
    {
        state->t = event->t;

        if (event->type == EVENT_CAST_START)
            cast(event->unit, event->spell);
        else if (event->type == EVENT_CAST_FINISH)
            onCastFinish(event->unit, event->spell);
        else if (event->type == EVENT_SPELL_IMPACT)
            onSpellImpact(event->unit, event->instance);
        else if (event->type == EVENT_SPELL_TICK)
            onSpellTick(event->unit, event->spell, event->tick);
        else if (event->type == EVENT_MANA_REGEN)
            onManaRegen(event->unit);
        else if (event->type == EVENT_MANA_GAIN)
            onManaGain(event->unit, event->mana, event->source);
        else if (event->type == EVENT_BUFF_GAIN)
            onBuffGain(event->unit, event->buff);
        else if (event->type == EVENT_BUFF_EXPIRE)
            onBuffExpire(event->unit, event->buff);
        else if (event->type == EVENT_BUFF_GAIN_ALL)
            onBuffGainAll(event->buff);
        else if (event->type == EVENT_DEBUFF_GAIN)
            onDebuffGain(event->debuff);
        else if (event->type == EVENT_DEBUFF_EXPIRE)
            onDebuffExpire(event->debuff);
        else if (event->type == EVENT_CD_GAIN)
            onCooldownGain(event->unit, event->cooldown);
        else if (event->type == EVENT_CD_EXPIRE)
            onCooldownExpire(event->unit, event->cooldown);
        else if (event->type == EVENT_UNIT)
            onUnitSpawn(event->unit);
        else if (event->type == EVENT_UNIT_DESPAWN)
            onUnitDespawn(event->unit);
        else if (event->type == EVENT_WAIT)
            onWait(event->unit, event->spell);
    }

    void push(shared_ptr<Event> event)
    {
        event->t+= state->t;

        for (auto itr = queue.begin(); itr != queue.end(); itr++) {
            if (event->t < (*itr)->t) {
                queue.insert(itr, event);
                return;
            }
        }

        queue.push_back(event);
    }

    void pushCastStart(shared_ptr<unit::Unit> unit, shared_ptr<spell::Spell> spell, double t)
    {
        shared_ptr<Event> event(new Event());
        event->type = EVENT_CAST_START;
        event->spell = spell;
        event->unit = unit;
        event->t = t;

        push(event);
    }

    void pushCastFinish(shared_ptr<unit::Unit> unit, shared_ptr<spell::Spell> spell, double t)
    {
        shared_ptr<Event> event(new Event());
        event->type = EVENT_CAST_FINISH;
        event->spell = spell;
        event->unit = unit;
        event->t = t;

        push(event);
    }

    void pushSpellImpact(shared_ptr<unit::Unit> unit, shared_ptr<spell::Spell> spell, double t = 0)
    {
        shared_ptr<spell::SpellInstance> instance = getSpellInstance(unit, spell);

        shared_ptr<Event> event(new Event());
        event->type = EVENT_SPELL_IMPACT;
        event->instance = instance;
        event->unit = unit;
        event->t = t;

        push(event);
    }

    void pushSpellImpact(shared_ptr<unit::Unit> unit, shared_ptr<spell::SpellInstance> instance, double t = 0)
    {
        shared_ptr<Event> event(new Event());
        event->type = EVENT_SPELL_IMPACT;
        event->instance = instance;
        event->unit = unit;
        event->t = t;

        push(event);
    }

    void pushChannelingTick(shared_ptr<unit::Unit> unit, shared_ptr<spell::Spell> spell, double t, int tick)
    {
        shared_ptr<Event> event(new Event());
        event->type = EVENT_SPELL_TICK;
        event->spell = spell;
        event->unit = unit;
        event->t = t;
        event->tick = tick;

        push(event);
    }

    void pushDot(shared_ptr<unit::Unit> unit, shared_ptr<spell::Spell> spell, int tick)
    {
        shared_ptr<spell::SpellInstance> instance = getSpellInstance(unit, spell);
        instance->tick = tick;

        shared_ptr<Event> event(new Event());
        event->type = EVENT_SPELL_IMPACT;
        event->instance = instance;
        event->unit = unit;
        event->t = tick * spell->t_interval;

        push(event);
    }

    void pushDotTick(shared_ptr<unit::Unit> unit, shared_ptr<spell::SpellInstance> instance)
    {
        shared_ptr<Event> event(new Event());
        event->type = EVENT_SPELL_IMPACT;
        event->instance = instance;
        event->unit = unit;
        event->t = instance->tick * instance->spell->t_interval;

        push(event);
    }

    void pushManaRegen(shared_ptr<unit::Unit> unit)
    {
        shared_ptr<Event> event(new Event());
        event->type = EVENT_MANA_REGEN;
        event->unit = unit;
        event->t = MANA_TICK_T;
        push(event);
    }

    void pushManaGain(shared_ptr<unit::Unit> unit, double t, double mana, string source = "")
    {
        shared_ptr<Event> event(new Event());
        event->type = EVENT_MANA_GAIN;
        event->t = t;
        event->unit = unit;
        event->mana = mana;
        event->source = source;

        push(event);
    }

    void pushBuffGain(shared_ptr<unit::Unit> unit, shared_ptr<buff::Buff> buff, double t)
    {
        shared_ptr<Event> event(new Event());
        event->type = EVENT_BUFF_GAIN;
        event->t = t;
        event->unit = unit;
        event->buff = buff;

        push(event);
    }

    void pushBuffGainAll(shared_ptr<buff::Buff> buff, double t)
    {
        shared_ptr<Event> event(new Event());
        event->type = EVENT_BUFF_GAIN_ALL;
        event->t = t;
        event->buff = buff;

        push(event);
    }

    void pushBuffExpire(shared_ptr<unit::Unit> unit, shared_ptr<buff::Buff> buff, double t = 0)
    {
        shared_ptr<Event> event(new Event());
        event->type = EVENT_BUFF_EXPIRE;
        event->t = t == 0 ? buff->duration : t;
        event->unit = unit;
        event->buff = buff;

        push(event);
    }

    void pushDebuffGain(shared_ptr<debuff::Debuff> debuff, double t)
    {
        shared_ptr<Event> event(new Event());
        event->type = EVENT_DEBUFF_GAIN;
        event->t = t;
        event->debuff = debuff;

        push(event);
    }

    void pushDebuffExpire(shared_ptr<debuff::Debuff> debuff)
    {
        shared_ptr<Event> event(new Event());
        event->type = EVENT_DEBUFF_EXPIRE;
        event->t = debuff->duration;
        event->debuff = debuff;

        push(event);
    }

    void pushCooldownGain(shared_ptr<unit::Unit> unit, shared_ptr<cooldown::Cooldown> cooldown, double t)
    {
        shared_ptr<Event> event(new Event());
        event->type = EVENT_CD_GAIN;
        event->t = t;
        event->unit = unit;
        event->cooldown = cooldown;

        push(event);
    }

    void pushCooldownExpire(shared_ptr<unit::Unit> unit, shared_ptr<cooldown::Cooldown> cooldown)
    {
        shared_ptr<Event> event(new Event());
        event->type = EVENT_CD_EXPIRE;
        event->t = cooldown->duration;
        event->unit = unit;
        event->cooldown = cooldown;

        push(event);
    }

    void pushUnitDespawn(shared_ptr<unit::Unit> unit, double t)
    {
        shared_ptr<Event> event(new Event());
        event->type = EVENT_UNIT_DESPAWN;
        event->t = t;
        event->unit = unit;

        push(event);
    }

    void pushWait(shared_ptr<unit::Unit> unit, double t, std::string str = "", shared_ptr<spell::Spell> spell = NULL)
    {
        shared_ptr<Event> event(new Event());
        event->type = EVENT_WAIT;
        event->unit = unit;
        event->t = t;
        event->spell = spell;

        push(event);

        if (str.length()) {
            ostringstream s;
            s << std::fixed << std::setprecision(2);
            s << str << ", " << unit->name << " waiting " << t << " seconds...";
            addLog(unit, LOG_WAIT, s.str());
        }
    }

    void onAction(shared_ptr<unit::Unit> unit, shared_ptr<action::Action> action)
    {
        if (action->type == action::TYPE_WAIT) {
            pushWait(unit, action->value, action->str);
        }
        else if (action->type == action::TYPE_SPELL) {
            cast(unit, action->spell);
            if (action->cooldown)
                onCooldownGain(unit, action->cooldown);
        }
        else if (action->type == action::TYPE_BUFF) {
            onBuffGain(unit, action->buff);
            if (action->cooldown)
                onCooldownGain(unit, action->cooldown);
        }
        else if (action->type == action::TYPE_BUFF_EXPIRE) {
            onBuffExpire(unit, action->buff);
            if (action->cooldown)
                onCooldownGain(unit, action->cooldown);
        }
        else if (action->type == action::TYPE_DEBUFF) {
            onDebuffGain(action->debuff);
            if (action->cooldown)
                onCooldownGain(unit, action->cooldown);
        }
        else if (action->type == action::TYPE_COOLDOWN) {
            onCooldownGain(unit, action->cooldown);
        }
        else if (action->type == action::TYPE_COOLDOWN_EXPIRE) {
            onCooldownExpire(unit, action->cooldown);
        }
        else if (action->type == action::TYPE_POTION) {
            usePotion(unit, action->potion);
        }
        else if (action->type == action::TYPE_CONJURED) {
            useConjured(unit, action->conjured);
        }
        else if (action->type == action::TYPE_TRINKET) {
            useTrinket(unit, action->trinket, action->cooldown);
        }
        else if (action->type == action::TYPE_MANA) {
            onManaGain(unit, action->value, action->str);
            if (action->cooldown)
                onCooldownGain(unit, action->cooldown);
        }
        else if (action->type == action::TYPE_UNIT) {
            onUnitSpawn(action->unit);
        }

        if (action->primary_action)
            nextAction(unit);
    }

    bool nextAction(shared_ptr<unit::Unit> unit)
    {
        shared_ptr<action::Action> action = unit->nextAction(state);
        if (action == NULL)
            return false;

        onAction(unit, action);

        return true;
    }

    void processActions(shared_ptr<unit::Unit> unit, std::list<shared_ptr<action::Action>> actions)
    {
        for (auto itr = actions.begin(); itr != actions.end(); itr++) {
            onAction(unit, *itr);
        }
    }

    void cast(shared_ptr<unit::Unit> unit, shared_ptr<spell::Spell> spell)
    {
        if (canCast(unit, spell)) {
            if (spell->active_use && !spell->off_gcd && unit->t_gcd > state->t)
                pushWait(unit, unit->t_gcd - state->t, "GCD", spell);
            else
                onCastStart(unit, spell);
        }
        else {
            pushWait(unit, 0.5, "Out of mana", spell);
        }
    }

    void onCastStart(shared_ptr<unit::Unit> unit, shared_ptr<spell::Spell> spell)
    {
        spell->actual_cast_time = castTime(unit, spell);

        logCastStart(unit, spell);

        if (spell->active_use) {
            unit->t_gcd = state->t + unit->gcd(spell->gcd);
            if (spell->gcd)
                unit->last_spell_cast_time = spell->actual_cast_time;
        }

        if (spell->channeling)
            onCastSuccess(unit, spell);
        else
            pushCastFinish(unit, spell, spell->actual_cast_time);
    }

    void onCastFinish(shared_ptr<unit::Unit> unit, shared_ptr<spell::Spell> spell)
    {
        if (spell->tick || canCast(unit, spell))
            onCastSuccess(unit, spell);
        else
            nextAction(unit);
    }

    void onCastSuccess(shared_ptr<unit::Unit> unit, shared_ptr<spell::Spell> spell)
    {
        int targets = spell->aoe ? config->targets : 1;

        spell->actual_cost = manaCost(unit, spell);
        unit->mana-= spell->actual_cost;
        logCastSuccess(unit, spell);

        if (spell->channeling && !spell->tick)
            unit->is_channeling = true;

        for (int t=0; t<targets; t++) {
            if (spell->channeling && !spell->tick) {
                for (int i=1; i<=spell->ticks; i++)
                    pushChannelingTick(unit, spell, spell->actual_cast_time / spell->ticks * i, i);
            }
            else if (spell->dot) {
                dotApply(unit, spell);
            }
            else if (!spell->is_trigger) {
                pushSpellImpact(unit, spell, travelTime(unit, spell));
            }
        }

        if (spell->active_use) {
            onCastSuccessProc(unit, spell);
        }

        if (spell->active_use) {
            if (state->inCombat()) {
                if (spell->channeling && spell->actual_cast_time > 0)
                    pushWait(unit, spell->actual_cast_time);
                else
                    nextAction(unit);
            }

            // Log spell use
            if (logging && spell->min_dmg) {
                initSpellStats(unit, spell);
                state->spells[spell->id].casts++;
            }
        }
    }

    void onSpellImpact(shared_ptr<unit::Unit> unit, shared_ptr<spell::SpellInstance> instance)
    {
        if (instance->spell->dot) {
            instance->resist = spellDmgResist(unit, instance);
            instance->dmg-= instance->resist;
        }

        state->dmg+= instance->dmg;
        logSpellImpact(unit, instance);
        onSpellImpactProc(unit, instance);

        // Log spell use
        if (logging && state->spells.find(instance->spell->id) != state->spells.end()) {
            if (instance->result == spell::MISS)
                state->spells[instance->spell->id].misses++;
            else if (instance->result == spell::CRIT)
                state->spells[instance->spell->id].crits++;
            else
                state->spells[instance->spell->id].hits++;

            state->spells[instance->spell->id].dmg+= instance->dmg;
            if (instance->dmg > state->spells[instance->spell->id].max_dmg)
                state->spells[instance->spell->id].max_dmg = instance->dmg;
            if (instance->dmg < state->spells[instance->spell->id].min_dmg || state->spells[instance->spell->id].min_dmg == 0)
                state->spells[instance->spell->id].min_dmg = instance->dmg;
        }
    }

    void onSpellTick(shared_ptr<unit::Unit> unit, shared_ptr<spell::Spell> spell, int tick)
    {
        if (!spell->is_trigger) {
            shared_ptr<spell::SpellInstance> instance = getSpellInstance(unit, spell);
            instance->tick = tick;
            pushSpellImpact(unit, instance, travelTime(unit, spell));
        }

        onSpellTickProc(unit, spell, tick);
    }

    void dotApply(shared_ptr<unit::Unit> unit, shared_ptr<spell::Spell> spell)
    {
        // Ignite special case
        if (spell->id == spell::IGNITE) {
            list<shared_ptr<spell::SpellInstance>> dots = getDots(unit, spell);
            shared_ptr<spell::SpellInstance> dot = NULL;
            removeSpellImpacts(unit, spell);

            double leftover = 0;
            while (!dots.empty()) {
                leftover+= dots.front()->dmg;
                dots.pop_front();
            }

            for (int i=1; i<=spell->ticks; i++) {
                dot = getSpellInstance(unit, spell);
                dot->tick = i;
                dot->dmg+= round(leftover / 2.0);
                if (config->ignite_munching && state->ignite_dmg > 0 && state->t - state->ignite_t <= IGNITE_MUNCH_WINDOW)
                    dot->dmg-= state->ignite_dmg;
                pushDotTick(unit, dot);
            }

            if (config->ignite_munching) {
                state->ignite_t = state->t;
                state->ignite_dmg = spell->min_dmg;
            }
        }
        else {
            if (!spell->overlap)
                removeSpellImpacts(unit, spell);
            for (int i=1; i<=spell->ticks; i++)
                pushDot(unit, spell, i);

            // Living bomb special
            if (spell->id == spell::LIVING_BOMB) {
                shared_ptr<spell::Spell> bomb = make_shared<spell::LivingBombExplosion>();
                removeSpellImpact(unit, bomb);
                pushSpellImpact(unit, bomb, spell->ticks * spell->t_interval);
            }
        }
    }

    void onCastSuccessProc(shared_ptr<unit::Unit> unit, shared_ptr<spell::Spell> spell)
    {
        std::list<shared_ptr<action::Action>> actions = unit->onCastSuccessProc(state, spell);
        processActions(unit, actions);
    }

    void onSpellImpactProc(shared_ptr<unit::Unit> unit, shared_ptr<spell::SpellInstance> instance)
    {
        std::list<shared_ptr<action::Action>> actions = unit->onSpellImpactProc(state, instance);
        processActions(unit, actions);
    }

    void onSpellTickProc(shared_ptr<unit::Unit> unit, shared_ptr<spell::Spell> spell, int tick)
    {
        std::list<shared_ptr<action::Action>> actions = unit->onSpellTickProc(state, spell, tick);
        processActions(unit, actions);
    }

    void onUnitSpawn(shared_ptr<unit::Unit> unit)
    {
        if (unit->unique)
            despawnDuplicate(unit);

        unit->reset();
        logUnitSpawn(unit);
        state->addUnit(unit);

        if (unit->duration)
            pushUnitDespawn(unit, unit->duration);

        if (state->inCombat())
            pushWait(unit, 0.5);
        else
            pushWait(unit, -state->t);
    }

    void onUnitDespawn(shared_ptr<unit::Unit> unit)
    {
        logUnitDespawn(unit);
        removeUnitEvents(unit);
        state->removeUnit(unit);
    }

    void despawnDuplicate(shared_ptr<unit::Unit> unit)
    {
        for (auto itr = state->units.begin(); itr != state->units.end(); itr++) {
            if ((*itr)->id == unit->id) {
                onUnitDespawn(*itr);
                break;
            }
        }
    }

    void onManaRegen(shared_ptr<unit::Unit> unit, bool next = true)
    {
        onManaGain(unit, unit->manaPerTick(state), "Mana Regen");
        if (next)
            pushManaRegen(unit);
    }

    void onManaGain(shared_ptr<unit::Unit> unit, double mana, string source = "")
    {
        unit->mana = min(player->maxMana(), unit->mana + mana);
        logManaGain(unit, mana, source);
    }

    void onWait(shared_ptr<unit::Unit> unit, shared_ptr<spell::Spell> spell = NULL)
    {
        if (unit->is_channeling) {
            unit->removeSnapshots();
            unit->is_channeling = false;
        }

        if (spell != NULL)
            cast(unit, spell);
        else
            nextAction(unit);
    }

    void onBuffGain(shared_ptr<unit::Unit> unit, shared_ptr<buff::Buff> buff)
    {
        if (buff->cost) {
            if (!canBuff(unit, buff))
                return;
            unit->mana-= manaCost(unit, buff);
        }

        int old_stacks = unit->buffStacks(buff->id);

        buff->t_refreshed = state->t;
        if (old_stacks < 1)
            buff->t_gained = state->t;

        int stacks = unit->addBuff(buff);

        if (old_stacks < 1 || buff->stack_refresh) {
            removeBuffExpiration(unit, buff);
            pushBuffExpire(unit, buff);
        }

        if (buff->id == buff::MANA_TIDE) {
            for (double t=3; t<=12; t+= 3)
                pushManaGain(unit, t, unit->maxMana() * 0.06, "Mana Tide");
        }
        else if (buff->id == buff::INNERVATE) {
            for (double t=1; t<=10; t+= 1)
                pushManaGain(unit, t, 3496 * .225, "Innervate");
        }
        else if (buff->id == buff::DISLODGED_OBJECT_HC && stacks < 10) {
            pushBuffGain(unit, buff, 2);
        }
        else if (buff->id == buff::DISLODGED_OBJECT_NM && stacks < 10) {
            pushBuffGain(unit, buff, 2);
        }

        if (stacks > old_stacks)
            logBuffGain(unit, buff, stacks);

        std::list<shared_ptr<action::Action>> actions = unit->onBuffGain(state, buff);
        processActions(unit, actions);
    }

    void onBuffExpire(shared_ptr<unit::Unit> unit, shared_ptr<buff::Buff> buff)
    {
        bool snapshot = false;
        if (buff->snapshot && unit->is_channeling && buffDuration(unit, buff->id) > 0)
            snapshot = true;

        removeBuffExpiration(unit, buff);
        logBuffExpire(unit, buff);
        unit->removeBuff(buff->id, snapshot);

        std::list<shared_ptr<action::Action>> actions = unit->onBuffExpire(state, buff);
        processActions(unit, actions);
    }

    void onBuffGainAll(shared_ptr<buff::Buff> buff)
    {
        onBuffGain(player, buff);

        for (auto itr = state->units.begin(); itr != state->units.end(); itr++)
            onBuffGain(*itr, buff);
    }

    void onDebuffGain(shared_ptr<debuff::Debuff> debuff)
    {
        int stacks = state->addDebuff(debuff);
        removeDebuffExpiration(debuff);
        pushDebuffExpire(debuff);

        if (stacks)
            logDebuffGain(debuff, stacks);
    }

    void onDebuffExpire(shared_ptr<debuff::Debuff> debuff)
    {
        removeDebuffExpiration(debuff);
        logDebuffExpire(debuff);
        state->removeDebuff(debuff->id);
    }

    void onCooldownGain(shared_ptr<unit::Unit> unit, shared_ptr<cooldown::Cooldown> cooldown, bool mod = true)
    {
        if (mod)
            cooldown->duration+= unit->cooldownMod(cooldown);

        removeCooldownExpiration(unit, cooldown);
        unit->addCooldown(cooldown);
        pushCooldownExpire(unit, cooldown);
    }

    void onCooldownExpire(shared_ptr<unit::Unit> unit, shared_ptr<cooldown::Cooldown> cooldown)
    {
        unit->removeCooldown(cooldown->id);
    }

    void usePotion(shared_ptr<unit::Unit> unit, Potion potion)
    {
        std::list<shared_ptr<action::Action>> actions = unit->usePotion(potion, state->inCombat());
        processActions(unit, actions);
    }

    void useConjured(shared_ptr<unit::Unit> unit, Conjured conjured)
    {
        std::list<shared_ptr<action::Action>> actions = unit->useConjured(conjured);
        processActions(unit, actions);
    }

    void useTrinket(shared_ptr<unit::Unit> unit, Trinket trinket, shared_ptr<cooldown::Cooldown> cooldown)
    {
        std::list<shared_ptr<action::Action>> actions = unit->useTrinket(trinket, cooldown);
        processActions(unit, actions);
    }

    bool hasDot(shared_ptr<unit::Unit> unit, shared_ptr<spell::Spell> spell)
    {
       return getDot(unit, spell) != NULL;
    }

    shared_ptr<spell::SpellInstance> getDot(shared_ptr<unit::Unit> unit, shared_ptr<spell::Spell> spell)
    {
        for (auto itr = queue.begin(); itr != queue.end(); itr++) {
            if ((*itr)->type == EVENT_SPELL_IMPACT && (*itr)->instance->spell->id == spell->id && (*itr)->unit == unit) {
                return (*itr)->instance;
            }
        }

        return NULL;
    }

    list<shared_ptr<spell::SpellInstance>> getDots(shared_ptr<unit::Unit> unit, shared_ptr<spell::Spell> spell)
    {
        list<shared_ptr<spell::SpellInstance>> dots;

        for (auto itr = queue.begin(); itr != queue.end(); itr++) {
            if ((*itr)->type == EVENT_SPELL_IMPACT && (*itr)->instance->spell->id == spell->id && (*itr)->unit == unit) {
                dots.push_back((*itr)->instance);
            }
        }

        return dots;
    }

    void removeSpellImpact(shared_ptr<unit::Unit> unit, shared_ptr<spell::Spell> spell)
    {
        for (auto itr = queue.begin(); itr != queue.end(); itr++) {
            if ((*itr)->type == EVENT_SPELL_IMPACT && (*itr)->instance->spell->id == spell->id && (*itr)->unit == unit) {
                queue.erase(itr);
                return;
            }
        }
    }

    void removeSpellImpacts(shared_ptr<unit::Unit> unit, shared_ptr<spell::Spell> spell)
    {
        for (auto itr = queue.begin(); itr != queue.end();) {
            if ((*itr)->type == EVENT_SPELL_IMPACT && (*itr)->instance->spell->id == spell->id && (*itr)->unit == unit)
                itr = queue.erase(itr);
            else
                itr++;
        }
    }

    void removeBuffExpiration(shared_ptr<unit::Unit> unit, shared_ptr<buff::Buff> buff)
    {
        for (auto itr = queue.begin(); itr != queue.end(); itr++) {
            if ((*itr)->type == EVENT_BUFF_EXPIRE && (*itr)->buff->id == buff->id && (*itr)->unit == unit) {
                queue.erase(itr);
                return;
            }
        }
    }

    void removeDebuffExpiration(shared_ptr<debuff::Debuff> debuff)
    {
        for (auto itr = queue.begin(); itr != queue.end(); itr++) {
            if ((*itr)->type == EVENT_DEBUFF_EXPIRE && (*itr)->debuff->id == debuff->id) {
                queue.erase(itr);
                return;
            }
        }
    }

    void removeCooldownExpiration(shared_ptr<unit::Unit> unit, shared_ptr<cooldown::Cooldown> cooldown)
    {
        for (auto itr = queue.begin(); itr != queue.end(); itr++) {
            if ((*itr)->type == EVENT_CD_EXPIRE && (*itr)->cooldown->id == cooldown->id && (*itr)->unit == unit) {
                queue.erase(itr);
                return;
            }
        }
    }

    double buffDuration(shared_ptr<unit::Unit> unit, buff::ID id)
    {
        for (auto itr = queue.begin(); itr != queue.end(); itr++) {
            if ((*itr)->type == EVENT_BUFF_EXPIRE && (*itr)->buff->id == id && (*itr)->unit == unit)
                return (*itr)->t - state->t;
        }

        return 0;
    }

    double debuffDuration(debuff::ID id)
    {
        for (auto itr = queue.begin(); itr != queue.end(); itr++) {
            if ((*itr)->type == EVENT_DEBUFF_EXPIRE && (*itr)->debuff->id == id)
                return (*itr)->t - state->t;
        }

        return 0;
    }

    void removeUnitEvents(shared_ptr<unit::Unit> unit)
    {
        for (auto itr = queue.begin(); itr != queue.end();) {
            if ((*itr)->unit == unit && (*itr)->type != EVENT_SPELL_IMPACT)
                itr = queue.erase(itr);
            else
                itr++;
        }
    }

    bool canCast(shared_ptr<unit::Unit> unit, shared_ptr<spell::Spell> spell)
    {
        return unit->mana >= manaCost(unit, spell);
    }

    bool canBuff(shared_ptr<unit::Unit> unit, shared_ptr<buff::Buff> buff)
    {
        return unit->mana >= manaCost(unit, buff);
    }

    double manaCost(shared_ptr<unit::Unit> unit, shared_ptr<spell::Spell> spell)
    {
        return unit->manaCost(spell);
    }

    double manaCost(shared_ptr<unit::Unit> unit, shared_ptr<buff::Buff> buff)
    {
        return unit->manaCost(buff);
    }

    double travelTime(shared_ptr<unit::Unit> unit, shared_ptr<spell::Spell> spell)
    {
        if (!spell->has_travel_time)
            return 0;

        double t = config->spell_travel_time / 1000.0;

        if (spell->travel_time_factor)
            t*= spell->travel_time_factor;

        return t;
    }

    double castTime(shared_ptr<unit::Unit> unit, shared_ptr<spell::Spell> spell)
    {
        if (unit->hasBuff(buff::PRESENCE_OF_MIND) && !spell->channeling)
            return 0;

        return unit->castTime(spell);
    }

    double hitChance(shared_ptr<unit::Unit> unit, shared_ptr<spell::Spell> spell)
    {
        int dlevel = config->target_level - 80;

        double hit = 96.0 - dlevel;

        if (dlevel > 2)
            hit-= (dlevel - 2) * 10.0;

        hit+= unit->hitChance(spell);

        if (config->debuff_spell_hit)
            hit+= 3.0;

        return min(hit, 100.0);
    }

    double critChance(shared_ptr<unit::Unit> unit, shared_ptr<spell::Spell> spell)
    {
        double crit = unit->critChance(spell);

        if (config->debuff_spell_crit || state->hasDebuff(debuff::IMPROVED_SCORCH))
            crit+= 5.0;
        else if (state->hasDebuff(debuff::WINTERS_CHILL))
            crit+= state->debuffStacks(debuff::WINTERS_CHILL);
        if (config->debuff_crit)
            crit+= 3.0;

        return crit;
    }

    double critMultiplier(shared_ptr<unit::Unit> unit, shared_ptr<spell::Spell> spell)
    {
        double base = 1.5;
        double multi = 1;

        base*= unit->baseCritMultiplier(spell);

        if (spell->proc)
            return base;

        multi*= unit->critMultiplierMod(spell);

        return (base - 1) * multi + 1;
    }

    double buffDmgMultiplier(shared_ptr<unit::Unit> unit, shared_ptr<spell::Spell> spell)
    {
        double multi = 1;

        multi*= unit->buffDmgMultiplier(spell, state);

        return multi;
    }

    double debuffDmgMultiplier(shared_ptr<unit::Unit> unit, shared_ptr<spell::Spell> spell)
    {
        double multi = 1;

        if (config->debuff_spell_dmg)
            multi*= 1.13;

        return multi;
    }

    double spellDmg(shared_ptr<unit::Unit> unit, shared_ptr<spell::Spell> spell)
    {
        double dmg;

        if (config->avg_spell_dmg)
            dmg = spell->avgDmg();
        else
            dmg = random<double>(spell->min_dmg, spell->max_dmg);

        if (spell->fixed_dmg)
            return dmg;

        if (spell->coeff) {
            double sp = unit->getSpellPower(spell->school);
            double coeff = spell->coeff;

            coeff+= unit->spellCoeffMod(spell);

            if (spell->channeling)
                coeff/= spell->ticks;

            dmg+= sp*coeff;
        }

        dmg*= buffDmgMultiplier(unit, spell);

        if (spell->aoe && spell->aoe_capped && config->targets > 10)
            dmg*= 10 / config->targets;

        dmg*= debuffDmgMultiplier(unit, spell);

        return dmg;
    }

    double spellDmgResist(shared_ptr<unit::Unit> unit, shared_ptr<spell::SpellInstance> instance)
    {
        if (instance->spell->binary || instance->spell->fixed_dmg)
            return 0.0;

        // No confirmed formulas or resistance tables can be found
        // This resistance table is based on data from Naxxramas in Wotlk Beta uploaded to WCL
        // It results in 5.9% mitigation

        int resist[3] = {55, 31, 14};
        int roll = random<int>(0, 99);

        double resistance_multiplier = 0.0;
        for (int i=0; i<3; i++) {
            if (roll < resist[i]) {
                resistance_multiplier = ((float) i) * 0.1;
                break;
            }

            roll-= resist[i];
        }

        if (!resistance_multiplier)
            return 0.0;

        return round(instance->dmg * resistance_multiplier);
    }

    spell::Result getSpellResult(shared_ptr<unit::Unit> unit, shared_ptr<spell::Spell> spell)
    {
        if (unit->canMiss(spell) && random<double>(0, 100) > hitChance(unit, spell))
            return spell::MISS;

        if (unit->canCrit(spell) && random<double>(0, 100) <= critChance(unit, spell))
            return spell::CRIT;

        return spell::HIT;
    }

    shared_ptr<spell::SpellInstance> getSpellInstance(shared_ptr<unit::Unit> unit, shared_ptr<spell::Spell> spell)
    {
        shared_ptr<spell::SpellInstance> instance = make_shared<spell::SpellInstance>(spell);

        if (spell->max_dmg > 0) {
            instance->result = getSpellResult(unit, spell);

            if (instance->result != spell::MISS) {
                instance->dmg = spellDmg(unit, spell);

                if (instance->result == spell::CRIT)
                    instance->dmg*= critMultiplier(unit, spell);

                if (unit->canResist(spell)) {
                    instance->resist = spellDmgResist(unit, instance);
                    instance->dmg-= instance->resist;
                }

                instance->dmg = round(instance->dmg);
            }
        }

        return instance;
    }

    void logCastStart(shared_ptr<unit::Unit> unit, shared_ptr<spell::Spell> spell)
    {
        if (!logging || !spell->active_use || !spell->cast_time)
            return;

        ostringstream s;

        s << unit->name << " started casting " << spell->name;

        addLog(unit, LOG_CAST_START, s.str());
    }

    void logCastSuccess(shared_ptr<unit::Unit> unit, shared_ptr<spell::Spell> spell)
    {
        if (!logging || !spell->active_use)
            return;

        ostringstream s;

        s << unit->name << " successfully cast " << spell->name;

        addLog(unit, LOG_CAST_SUCCESS, s.str());
    }

    void logSpellImpact(shared_ptr<unit::Unit> unit, shared_ptr<spell::SpellInstance> instance)
    {
        if (!logging)
            return;

        ostringstream s;

        s << unit->name << "'s " << instance->spell->name;
        if (instance->spell->dot)
            s << " (dot)";
        if (instance->result == spell::MISS)
            s << " was resisted";
        else if (instance->result == spell::CRIT)
            s << " crit for " << instance->dmg;
        else
            s << " hit for " << instance->dmg;

        if (instance->resist)
            s << " (" << instance->resist << " resisted)";

        addLog(unit, LOG_SPELL_IMPACT, s.str());
    }

    void logBuffGain(shared_ptr<unit::Unit> unit, shared_ptr<buff::Buff> buff, int stacks = 1)
    {
        if (!logging || buff->hidden)
            return;

        ostringstream s;

        s << unit->name << " gained " << buff->name;
        if (buff->max_stacks > 1)
            s << " (" << stacks << ")";

        addLog(unit, LOG_BUFF, s.str());
    }

    void logBuffExpire(shared_ptr<unit::Unit> unit, shared_ptr<buff::Buff> buff)
    {
        if (!logging || buff->hidden)
            return;

        ostringstream s;

        s << unit->name << " lost " << buff->name;

        addLog(unit, LOG_BUFF, s.str());
    }

    void logDebuffGain(shared_ptr<debuff::Debuff> debuff, int stacks = 1)
    {
        if (!logging || debuff->hidden)
            return;

        ostringstream s;

        s << "Target gained " << debuff->name;
        if (debuff->max_stacks > 1)
            s << " (" << stacks << ")";

        addLog(player, LOG_BUFF, s.str());
    }

    void logDebuffExpire(shared_ptr<debuff::Debuff> debuff)
    {
        if (!logging || debuff->hidden)
            return;

        ostringstream s;

        s << "Target lost " << debuff->name;

        addLog(player, LOG_BUFF, s.str());
    }

    void logManaGain(shared_ptr<unit::Unit> unit, double mana, string source)
    {
        if (!logging)
            return;

        ostringstream s;

        s << fixed << setprecision(0);
        if (mana < 0)
            s << unit->name << " lost " << (0-mana) << " mana from " << source;
        else
            s << unit->name << " gained " << mana << " mana from " << source;

        addLog(unit, LOG_MANA, s.str());
    }

    void logUnitSpawn(shared_ptr<unit::Unit> unit)
    {
        if (!logging)
            return;

        ostringstream s;

        s << unit->name << " spawned";

        addLog(unit, LOG_UNIT, s.str());
    }

    void logUnitDespawn(shared_ptr<unit::Unit> unit)
    {
        if (!logging)
            return;

        ostringstream s;

        s << unit->name << " despawned";

        addLog(unit, LOG_UNIT, s.str());
    }

    string jsonLog()
    {
        ostringstream s;

        s << "[";

        for (int i=0; i<log.size(); i++) {
            if (i > 0)
                s << ",";
            s << "{";
            if (log[i]->unit)
                s << "\"unit\":\"" << log[i]->unit->name << "\"";
            else
                s << "\"unit\":\"Unknown\"";
            s << ",\"text\":\"" << log[i]->text << "\"";
            s << ",\"t\":" << log[i]->t;
            s << ",\"type\":" << log[i]->type;
            s << ",\"dmg\":" << log[i]->dmg;
            s << ",\"mana\":" << log[i]->mana;
            s << ",\"mana_percent\":" << log[i]->mana_percent;
            s << "}";
        }

        s << "]";

        return s.str();
    }

    void addLog(shared_ptr<unit::Unit> unit, LogType type, string text)
    {
        if (!logging)
            return;

        shared_ptr<LogEntry> entry(new LogEntry);
        entry->unit = unit;
        entry->type = type;
        entry->text = text;
        entry->t = state->t;
        entry->dmg = state->dmg;
        entry->mana = unit->mana;
        entry->mana_percent = unit->manaPercent();

        log.push_back(entry);
    }

    void clearLog()
    {
        log.clear();
    }

    void initSpellStats(shared_ptr<unit::Unit> unit, shared_ptr<spell::Spell> spell)
    {
        if (state->spells.find(spell->id) == state->spells.end()) {
            state->spells[spell->id].name = spell->name;
            state->spells[spell->id].source = unit->name;
        }
    }

    string spellStats()
    {
        ostringstream s;

        s << "[";

        for (auto itr = state->spells.begin(); itr != state->spells.end(); itr++) {
            if (itr != state->spells.begin())
                s << ",";
            s << "{";
            s << "\"id\":" << itr->first << ",";
            s << "\"name\":\"" << itr->second.name << "\",";
            s << "\"source\":\"" << itr->second.source << "\",";
            s << "\"casts\":" << itr->second.casts << ",";
            s << "\"misses\":" << itr->second.misses << ",";
            s << "\"hits\":" << itr->second.hits << ",";
            s << "\"crits\":" << itr->second.crits << ",";
            s << "\"min_dmg\":" << itr->second.min_dmg << ",";
            s << "\"max_dmg\":" << itr->second.max_dmg << ",";
            s << "\"dmg\":" << itr->second.dmg;
            s << "}";
        }

        s << "]";

        return s.str();
    }

};
