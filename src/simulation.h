using namespace std;

class Event
{

public:
    double t;
    double mana;
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

        double evocated = 0;
        double evocated_at = 0;
        double t_gcd_capped = 0;

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

            if (r.evocated_at != -1) {
                evocated++;
                evocated_at+= (r.evocated_at - evocated_at) / evocated;
            }

            t_gcd_capped+= (r.t_gcd_capped - t_gcd_capped) / (i+1);

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

        // Stats json string
        ss.str("");
        ss.clear();
        ss << "{";
        ss << "\"evocated\":{\"t\":" << evocated_at << ",\"n\":" << evocated << "},";
        ss << "\"t_gcd_capped\":" << t_gcd_capped;
        ss << "}";
        result.stats = ss.str();

        return result;
    }

    SimulationResult run()
    {
        reset();

        pushManaRegen(player);

        if (config->innervate) {
            for (int i=0; i<config->innervate_t.size() && i<config->innervate; i++)
                pushBuffGain(player, make_shared<buff::Innervate>(), config->innervate_t.at(i));
        }
        if (config->bloodlust) {
            for (int i=0; i<config->bloodlust_t.size(); i++)
                pushBuffGain(player, make_shared<buff::Bloodlust>(), config->bloodlust_t.at(i));
        }
        if (config->power_infusion) {
            for (int i=0; i<config->power_infusion_t.size(); i++)
                pushBuffGain(player, make_shared<buff::PowerInfusion>(), config->power_infusion_t.at(i));
        }
        if (config->mana_tide) {
            for (int i=0; i<config->mana_tide_t.size(); i++)
                pushBuffGain(player, make_shared<buff::ManaTide>(), config->mana_tide_t.at(i));
        }
        if (player->talents.focus_magic) {
            pushBuffGain(player, make_shared<buff::FocusMagic>(), 5.0);
        }
        if (config->drums && config->drums_friend) {
            double t = 0;
            for (int i=0; i<config->drums_t.size(); i++) {
                if (config->drums_t.at(i) >= t)
                    t = config->drums_t.at(i);
                pushBuffGain(player, getDrumsBuff(), t);
            }
            for (t+= 120; t<state->duration; t+= 120)
                pushBuffGain(player, getDrumsBuff(), t);
        }

        nextAction(player);

        work();

        SimulationResult result;
        result.dmg = state->dmg;
        result.t = state->t;
        result.dps = state->dmg/state->t;
        result.evocated_at = state->evocated_at;

        if (logging) {
            result.log = jsonLog();
            result.spells = spellStats();
        }

        return result;
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
        else if (event->type == EVENT_MANA_REGEN)
            onManaRegen(event->unit);
        else if (event->type == EVENT_MANA_GAIN)
            onManaGain(event->unit, event->mana, event->source);
        else if (event->type == EVENT_BUFF_GAIN)
            onBuffGain(event->unit, event->buff);
        else if (event->type == EVENT_BUFF_EXPIRE)
            onBuffExpire(event->unit, event->buff);
        else if (event->type == EVENT_DEBUFF_GAIN)
            onDebuffGain(event->debuff);
        else if (event->type == EVENT_DEBUFF_EXPIRE)
            onDebuffExpire(event->debuff);
        else if (event->type == EVENT_CD_GAIN)
            onCooldownGain(event->unit, event->cooldown);
        else if (event->type == EVENT_CD_EXPIRE)
            onCooldownExpire(event->unit, event->cooldown);
        else if (event->type == EVENT_WAIT)
            onWait(event->unit);
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
        event->t = 2;
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

    void pushWait(shared_ptr<unit::Unit> unit, double t, std::string str = "")
    {
        shared_ptr<Event> event(new Event());
        event->type = EVENT_WAIT;
        event->unit = unit;
        event->t = t;

        push(event);

        ostringstream s;
        s << std::fixed << std::setprecision(2);
        if (str.length())
            s << str << ", waiting ";
        else
            s << "Waiting ";
        s << t << " seconds...";

        addLog(unit, LOG_WAIT, s.str());
    }

    void onAction(shared_ptr<unit::Unit> unit, shared_ptr<action::Action> action)
    {
        bool go_next = false;

        if (action->type == action::TYPE_WAIT) {
            pushWait(unit, action->value);
        }
        else if (action->type == action::TYPE_SPELL) {
            cast(unit, action->spell);
        }
        else if (action->type == action::TYPE_BUFF) {
            onBuffGain(unit, action->buff);
            if (action->cooldown)
                onCooldownGain(unit, action->cooldown);
            go_next = action->primary_action;
        }
        else if (action->type == action::TYPE_BUFF_EXPIRE) {
            onBuffExpire(unit, action->buff);
            if (action->cooldown)
                onCooldownGain(unit, action->cooldown);
            go_next = action->primary_action;
        }
        else if (action->type == action::TYPE_DEBUFF) {
            onDebuffGain(action->debuff);
            if (action->cooldown)
                onCooldownGain(unit, action->cooldown);
        }
        else if (action->type == action::TYPE_COOLDOWN) {
            onCooldownGain(unit, action->cooldown);
        }
        else if (action->type == action::TYPE_POTION) {
            usePotion(unit, action->potion);
            go_next = true;
        }
        else if (action->type == action::TYPE_CONJURED) {
            useConjured(unit, action->conjured);
            go_next = true;
        }
        else if (action->type == action::TYPE_MANA) {
            onManaGain(unit, action->value, action->str);
        }

        if (go_next)
            nextAction(unit);
    }

    void nextAction(shared_ptr<unit::Unit> unit)
    {
        onAction(unit, unit->nextAction(state));
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
            if (spell->active_use && unit->t_gcd > state->t)
                pushWait(unit, unit->t_gcd - state->t, "GCD");
            else
                onCastStart(unit, spell);
        }
        else {
            pushWait(unit, 0.5, "Out of mana");
        }
    }

    void onCastStart(shared_ptr<unit::Unit> unit, shared_ptr<spell::Spell> spell)
    {
        logCastStart(unit, spell);

        if (spell->active_use)
            unit->t_gcd = state->t + unit->gcd(spell->gcd);

        if (spell->channeling)
            onCastSuccess(unit, spell);
        else
            pushCastFinish(unit, spell, castTime(unit, spell));
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
        if (!spell->tick) {
            spell->actual_cost = manaCost(unit, spell);
            unit->mana-= spell->actual_cost;
            logCastSuccess(unit, spell);
        }

        if (spell->channeling && !spell->tick) {
            double cast_time = castTime(unit, spell);
            for (int i=1; i<=spell->ticks; i++)
                pushCastFinish(unit, spell, cast_time / spell->ticks * i);

            spell->tick++;
        }
        else if (spell->dot) {
            dotApply(unit, spell);
        }
        else {
            pushSpellImpact(unit, spell, travelTime(unit, spell));
        }

        if (!spell->tick && spell->active_use) {
            if (unit->hasBuff(buff::CLEARCAST))
                unit->removeBuff(buff::CLEARCAST);
            onCastSuccessProc(unit, spell);
        }

        if (spell->channeling) {
            spell->done = spell->tick == spell->ticks;
            spell->tick++;
        }
        else {
            spell->done = true;
        }

        if (spell->done && spell->active_use)
            nextAction(unit);
    }

    void onSpellImpact(shared_ptr<unit::Unit> unit, shared_ptr<spell::SpellInstance> instance)
    {
        state->dmg+= instance->dmg;
        logSpellImpact(unit, instance);
        onSpellImpactProc(unit, instance);
    }

    void dotApply(shared_ptr<unit::Unit> unit, shared_ptr<spell::Spell> spell)
    {
        // Stackable dot dmg, ie Ignite
        if (spell->stackable) {
            list<shared_ptr<spell::SpellInstance>> dots = getDots(unit, spell);
            shared_ptr<spell::SpellInstance> dot = NULL;
            removeSpellImpacts(unit, spell);
            for (int i=1; i<=spell->ticks; i++) {
                dot = getSpellInstance(unit, spell);
                dot->tick = i;
                if (!dots.empty()) {
                    dot->dmg+= dots.front()->dmg;
                    dots.pop_front();
                }
                pushDotTick(unit, dot);
            }
        }
        else {
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

    void onManaRegen(shared_ptr<unit::Unit> unit)
    {
        onManaGain(unit, unit->manaPerTick(), "Mana Regen");
        pushManaRegen(unit);
    }

    void onManaGain(shared_ptr<unit::Unit> unit, double mana, string source = "")
    {
        unit->mana = min(player->maxMana(), unit->mana + mana);
        logManaGain(unit, mana, source);
    }

    void onWait(shared_ptr<unit::Unit> unit)
    {
        nextAction(unit);
    }

    void onBuffGain(shared_ptr<unit::Unit> unit, shared_ptr<buff::Buff> buff)
    {
        if (buff->cost) {
            if (!canBuff(unit, buff))
                return;
            unit->mana-= manaCost(unit, buff);
        }

        int stacks = unit->addBuff(buff);
        removeBuffExpiration(unit, buff);
        pushBuffExpire(unit, buff);

        if (buff->id == buff::MANA_TIDE) {
            for (double t=3; t<=12; t+= 3)
                pushManaGain(unit, t, unit->maxMana() * 0.06, "Mana Tide");
        }

        if (buff->id == buff::DRUMS_OF_RESTORATION) {
            for (double t = 3; t<=15; t+= 3)
                pushManaGain(unit, t, 120, "Drums of Restoration");
        }

        if (stacks)
            logBuffGain(unit, buff, stacks);
    }

    void onBuffExpire(shared_ptr<unit::Unit> unit, shared_ptr<buff::Buff> buff)
    {
        removeBuffExpiration(unit, buff);
        logBuffExpire(unit, buff);
        unit->removeBuff(buff->id);
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

    shared_ptr<buff::Buff> getDrumsBuff()
    {
        if (config->drums == DRUMS_OF_BATTLE)
            return make_shared<buff::DrumsOfBattle>();
        else if (config->drums == DRUMS_OF_WAR)
            return make_shared<buff::DrumsOfWar>();
        else if (config->drums == DRUMS_OF_RESTORATION)
            return make_shared<buff::DrumsOfRestoration>();

        return NULL;
    }

    void usePotion(shared_ptr<unit::Unit> unit, Potion potion)
    {
        if (potion == POTION_MANA) {
            double mana = round(random<double>(4200, 4400));

            if (hasTrinket(TRINKET_MERCURIAL_ALCHEMIST_STONE))
                mana*= 1.4;

            onManaGain(unit, mana, "Mana Potion");
        }
        else if (potion == POTION_SPEED) {

        }
        else if (potion == POTION_WILD_MAGIC) {

        }
        else {
            return;
        }

        onCooldownGain(unit, make_shared<cooldown::Potion>());
    }

    void useConjured(shared_ptr<unit::Unit> unit, Conjured conjured)
    {
        double cd = 120;

        if (conjured == CONJURED_FLAME_CAP) {
            cd = 180;
            onBuffGain(unit, make_shared<buff::FlameCap>());
        }
        else if (conjured == CONJURED_MANA_GEM) {
            double mana = unit->manaGem();

            if (hasTrinket(TRINKET_SERPENT_COIL))
                mana*= 1.25;

            onManaGain(unit, mana, "Mana Gem");

            // TODO: Check for t7 bonus
            if (hasTrinket(TRINKET_SERPENT_COIL))
                onBuffGain(unit, make_shared<buff::SerpentCoil>());
        }
        else {
            return;
        }

        onCooldownGain(unit, make_shared<cooldown::Conjured>(cd));
    }

    bool hasTrinket(Trinket trinket)
    {
        return config->trinket1 == trinket || config->trinket2 == trinket;
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
        for (auto itr = queue.begin(); itr != queue.end();) {
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
        if (unit->hasBuff(buff::CLEARCAST))
            return 0;

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
        return config->spell_travel_time / 1000.0;
    }

    double castTime(shared_ptr<unit::Unit> unit, shared_ptr<spell::Spell> spell)
    {
        if (unit->hasBuff(buff::PRESENCE_OF_MIND) && !spell->channeling)
            return 0;

        return unit->castTime(spell);
    }

    double hitChance(shared_ptr<unit::Unit> unit, shared_ptr<spell::Spell> spell)
    {
        int dlevel = 3;
        if (config->targets > 1)
            dlevel = 2;

        double hit = 96.0 - dlevel;

        if (dlevel > 2)
            hit-= 10.0;

        hit+= unit->hitChance(spell);

        return min(hit, 99.0);
    }

    double critChance(shared_ptr<unit::Unit> unit, shared_ptr<spell::Spell> spell)
    {
        double crit = unit->critChance(spell);

        if (config->debuff_spell_crit || state->hasDebuff(debuff::IMPROVED_SCORCH))
            crit+= 5.0;
        else if (state->hasDebuff(debuff::WINTERS_CHILL))
            crit+= state->debuffStacks(debuff::WINTERS_CHILL);

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

        if (config->buff_dmg)
            multi*= 1.03;

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
            double sp = unit->getSpellPower();
            double coeff = spell->coeff;

            if (unit->hasBuff(buff::FLAME_CAP) && (spell->school == SCHOOL_FIRE || spell->school == SCHOOL_FROSTFIRE))
                sp+= 80.0;
            if (unit->hasBuff(buff::SERPENT_COIL))
                sp+= 225.0;
            if (unit->hasBuff(buff::SHRUNKEN_HEAD))
                sp+= 211.0;
            if (unit->hasBuff(buff::NAARU_SLIVER))
                sp+= 320.0;
            if (unit->hasBuff(buff::DRUMS_OF_WAR))
                sp+= 30.0;

            coeff+= unit->spellCoeffMod(spell);

            if (spell->channeling)
                coeff/= spell->ticks;

            dmg+= sp*coeff;
        }

        dmg*= buffDmgMultiplier(unit, spell);

        if (spell->aoe && config->targets > 10)
            dmg*= 10 / config->targets;

        dmg*= debuffDmgMultiplier(unit, spell);

        return dmg;
    }

    double spellDmgResist(shared_ptr<unit::Unit> unit, shared_ptr<spell::SpellInstance> instance)
    {
        if (instance->spell->binary)
            return 0.0;

        // No confirmed formulas or resistance tables can be found
        // This resistance table is based on data from Karazhan in TBC Beta uploaded to WCL
        // It results in about 6% mitigation

        int resist[4] = {83, 11, 5, 1};
        int roll = random<int>(0, 99);

        double resistance_multiplier = 0.0;
        for (int i=0; i<4; i++) {
            if (roll < resist[i]) {
                resistance_multiplier = ((float) i) * 0.25;
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

        s << unit->name << " finished casting " << spell->name;

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

    string jsonLog()
    {
        ostringstream s;

        s << "[";

        for (int i=0; i<log.size(); i++) {
            if (i > 0)
                s << ",";
            s << "{";
            s << "\"unit\":\"" << log[i]->unit->name << "\"";
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
