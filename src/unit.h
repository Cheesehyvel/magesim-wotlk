using namespace std;

class State;

namespace unit
{

    class Unit
    {

    public:
        shared_ptr<Config> config;
        Stats stats;
        Stats buff_stats;
        string name;
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

        map<cooldown::ID, shared_ptr<cooldown::Cooldown>> cooldowns;
        map<buff::ID, shared_ptr<buff::Buff>> buffs;
        map<buff::ID, shared_ptr<buff::Buff>> snapshot_buffs;
        map<debuff::ID, shared_ptr<debuff::Debuff>> debuffs;

        Unit(shared_ptr<Config> _config)
        {
            config = _config;
        }

        virtual void reset()
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

        virtual Stats getStats()
        {
            return stats;
        }

        virtual void setStats(Stats _stats)
        {
            stats = _stats;
        }

        bool hasCooldown(cooldown::ID id)
        {
            return cooldowns.find(id) != cooldowns.end();
        }

        void addCooldown(shared_ptr<cooldown::Cooldown> cooldown)
        {
            cooldowns[cooldown->id] = cooldown;
        }

        void removeCooldown(cooldown::ID id)
        {
            cooldowns.erase(id);
        }

        int buffStacks(buff::ID id, bool snapshot = false)
        {
            if (hasBuff(id))
                return buffs[id]->stacks;
            if (snapshot && hasSnapshot(id))
                return snapshot_buffs[id]->stacks;
            return 0;
        }

        bool hasBuff(buff::ID id, bool snapshot = false)
        {
            if (buffs.find(id) != buffs.end())
                return true;

            if (snapshot && hasSnapshot(id))
                return true;

            return false;
        }

        int addBuff(shared_ptr<buff::Buff> buff)
        {
            int stacks = buffStacks(buff->id);

            if (stacks < buff->max_stacks)
                addBuffStats(buff->stats);

            if (stacks)
                buffs[buff->id]->addStack();
            else
                buffs[buff->id] = buff;

            return buffs[buff->id]->stacks;
        }

        void removeBuff(buff::ID id, bool snapshot = false)
        {
            if (hasBuff(id)) {
                auto buff = getBuff(id);
                if (snapshot)
                    snapshot_buffs[buff->id] = buff;
                else
                    removeBuffStats(buff->stats, buff->stacks);
            }

            buffs.erase(id);
        }

        shared_ptr<buff::Buff> getBuff(buff::ID id)
        {
            if (hasBuff(id))
                return buffs[id];
            return NULL;
        }

        bool canReactTo(buff::ID id, double t)
        {
            if (t < t_gcd)
                t = t_gcd;
            auto buff = getBuff(id);
            if (buff != NULL && t - buff->t_gained >= config->reaction_time/1000.0)
                return true;
            return false;
        }

        bool hasSnapshot(buff::ID id)
        {
            return snapshot_buffs.find(id) != snapshot_buffs.end();
        }

        shared_ptr<buff::Buff> getSnapshot(buff::ID id)
        {
            if (hasSnapshot(id))
                return snapshot_buffs[id];
            return NULL;
        }

        void removeSnapshot(buff::ID id)
        {
            if (hasSnapshot(id)) {
                auto buff = getSnapshot(id);
                removeBuffStats(buff->stats, buff->stacks);
            }

            snapshot_buffs.erase(id);
        }

        void removeSnapshots()
        {
            for (auto it = snapshot_buffs.begin(); it != snapshot_buffs.end(); it++)
                removeBuffStats(it->second->stats, it->second->stacks);
            snapshot_buffs.clear();
        }

        void addBuffStats(Stats _stats)
        {
            buff_stats.intellect+= _stats.intellect;
            buff_stats.spirit+= _stats.spirit;
            buff_stats.mp5+= _stats.mp5;
            buff_stats.crit+= _stats.crit;
            buff_stats.hit+= _stats.hit;
            buff_stats.haste+= _stats.haste;
            buff_stats.haste_rating+= _stats.haste_rating;
            buff_stats.spell_power+= _stats.spell_power;
        }

        void removeBuffStats(Stats _stats, int stacks = 1)
        {
            buff_stats.intellect-= _stats.intellect * stacks;
            buff_stats.spirit-= _stats.spirit * stacks;
            buff_stats.mp5-= _stats.mp5 * stacks;
            buff_stats.crit-= _stats.crit * stacks;
            buff_stats.hit-= _stats.hit * stacks;
            buff_stats.haste-= _stats.haste * stacks;
            buff_stats.haste_rating-= _stats.haste_rating * stacks;
            buff_stats.spell_power-= _stats.spell_power * stacks;
        }

        int debuffStacks(debuff::ID id)
        {
            if (hasDebuff(id))
                return debuffs[id]->stacks;
            return 0;
        }

        bool hasDebuff(debuff::ID id)
        {
            return debuffs.find(id) != debuffs.end();
        }

        int addDebuff(shared_ptr<debuff::Debuff> debuff)
        {
            if (hasDebuff(debuff->id))
                return debuffs[debuff->id]->addStack();
            else
                debuffs[debuff->id] = debuff;

            return 1;
        }

        void removeDebuff(debuff::ID id)
        {
            debuffs.erase(id);
        }

        virtual double spiritManaPerSecond()
        {
            return 0.001 + getSpirit()*0.003345 * sqrt(getIntellect());
        }

        virtual double staticManaPerSecond()
        {
            return getMp5() / 5.0;
        }

        virtual double manaPerSecond(shared_ptr<State>)
        {
            return staticManaPerSecond();
        }

        virtual double maxMana()
        {
            // Subtract 280 because the first 20 intellect only gives 1 mana instead of 15
            return base_mana + stats.intellect*15 - 280;
        }

        double manaPercent()
        {
            return mana / maxMana() * 100.0;
        }

        double manaPerTick(shared_ptr<State> state)
        {
            return manaPerSecond(state) * MANA_TICK_T;
        }

        double gcd(double t = 1.5)
        {
            if (t == 0)
                return 0;

            double cap = 1.0;

            t*= castHaste();

            if (t < cap)
                t = cap;

            return t;
        }

        virtual double baseCastTime(shared_ptr<spell::Spell> spell)
        {
            return spell->cast_time;
        }

        virtual double castTime(shared_ptr<spell::Spell> spell)
        {
            return baseCastTime(spell) * castHaste();
        }

        virtual double castHaste()
        {
            double haste = 1;
            double rating = getHasteRating();

            if (rating)
                haste+= hasteRatingToHaste(rating) / 100.0;

            if (stats.haste)
                haste*= 1+stats.haste/100.0;

            if (hasBuff(buff::BLOODLUST))
                haste*= 1.3;
            else if (hasBuff(buff::POWER_INFUSION))
                haste*= 1.2;
            if (hasBuff(buff::ICY_VEINS))
                haste*= 1.2;
            if (hasBuff(buff::BERSERKING))
                haste*= 1.2;

            if (get_raid_buffs) {
                if (config->buff_haste)
                    haste*= 1.03;
                if (config->buff_spell_haste)
                    haste*= 1.05;
            }

            return 1.0 / haste;
        }

        virtual double hitChance(shared_ptr<spell::Spell> spell, double dlevel = 3)
        {
            return stats.hit + buff_stats.hit;
        }

        virtual double critChance(shared_ptr<spell::Spell> spell)
        {
            double crit = stats.crit + buff_stats.crit;

            if (get_raid_buffs) {
                if (config->buff_spell_crit)
                    crit+= 5;
            }

            return crit;
        }

        virtual bool canMiss(shared_ptr<spell::Spell> spell)
        {
            return !spell->dot;
        }

        virtual bool canCrit(shared_ptr<spell::Spell> spell)
        {
            return !spell->dot;
        }

        virtual bool canResist(shared_ptr<spell::Spell> spell)
        {
            return !spell->dot;
        }

        virtual double baseCritMultiplier(shared_ptr<spell::Spell> spell)
        {
            return 1;
        }

        virtual double critMultiplierMod(shared_ptr<spell::Spell> spell)
        {
            return 1;
        }

        virtual double spellCoeffMod(shared_ptr<spell::Spell> spell)
        {
            return 0;
        }

        virtual double buffDmgMultiplier(shared_ptr<spell::Spell> spell, shared_ptr<State> state)
        {
            double multi = 1;

            if (get_raid_buffs) {
                if (config->buff_dmg)
                    multi*= 1.03;
            }

            return multi;
        }

        virtual double baseManaCost(shared_ptr<spell::Spell> spell)
        {
            return spell->cost/100.0 * base_mana;
        }

        virtual double baseManaCost(shared_ptr<buff::Buff> buff)
        {
            return buff->cost/100.0 * base_mana;
        }

        virtual double manaCost(shared_ptr<spell::Spell> spell)
        {
            double cost = baseManaCost(spell) + manaCostMod(spell);

            return round(cost * manaCostMultiplier(spell));
        }

        virtual double manaCost(shared_ptr<buff::Buff> buff)
        {
            double cost = baseManaCost(buff);

            return round(cost);
        }

        virtual double manaCostMod(shared_ptr<spell::Spell> spell)
        {
            double base_cost = baseManaCost(spell);
            double cost = 0;

            if (hasBuff(buff::ARCANE_POWER))
                cost+= 0.2 * base_cost;

            return cost;
        }

        virtual double manaCostMultiplier(shared_ptr<spell::Spell> spell)
        {
            double multi = 1;

            if (hasBuff(buff::POWER_INFUSION))
                multi*= 0.8;

            return multi;
        }

        virtual double getIntellect()
        {
            return stats.intellect + buff_stats.intellect;
        }

        virtual double getSpirit()
        {
            return stats.spirit + buff_stats.spirit;
        }

        virtual double getSpellPower(School school = SCHOOL_NONE)
        {
            double sp = stats.spell_power + buff_stats.spell_power;

            if (get_raid_buffs) {
                if (config->demonic_pact || config->totem_of_wrath || config->flametongue) {
                    double x = 0;
                    if (config->totem_of_wrath)
                        x = 280;
                    else if (config->flametongue)
                        x = 144;
                    if (config->demonic_pact && config->demonic_pact_bonus > x)
                        x = config->demonic_pact_bonus;
                    sp+= x;
                }
            }

            return sp;
        }

        virtual double getHasteRating()
        {
            return stats.haste_rating + buff_stats.haste_rating;
        }

        virtual double getMp5()
        {
            return stats.mp5 + buff_stats.mp5;
        }

        virtual double cooldownMod(shared_ptr<cooldown::Cooldown> cooldown)
        {
            return 0;
        }

        virtual list<shared_ptr<action::Action>> onBuffGain(shared_ptr<State> state, shared_ptr<buff::Buff> buff)
        {
            list<shared_ptr<action::Action>> actions;

            return actions;
        }

        virtual list<shared_ptr<action::Action>> onBuffExpire(shared_ptr<State> state, shared_ptr<buff::Buff> buff)
        {
            list<shared_ptr<action::Action>> actions;

            return actions;
        }

        virtual list<shared_ptr<action::Action>> onCastSuccessProc(shared_ptr<State> state, shared_ptr<spell::Spell> spell)
        {
            list<shared_ptr<action::Action>> actions;

            return actions;
        }

        virtual list<shared_ptr<action::Action>> onSpellImpactProc(shared_ptr<State> state, shared_ptr<spell::SpellInstance> instance)
        {
            list<shared_ptr<action::Action>> actions;

            return actions;
        }

        virtual list<shared_ptr<action::Action>> onSpellTickProc(shared_ptr<State> state, shared_ptr<spell::Spell> spell, int tick)
        {
            list<shared_ptr<action::Action>> actions;

            return actions;
        }

        virtual list<shared_ptr<action::Action>> usePotion(Potion potion, bool in_combat)
        {
            list<shared_ptr<action::Action>> actions;

            return actions;
        }

        virtual list<shared_ptr<action::Action>> useConjured(shared_ptr<State> state, Conjured conjured)
        {
            list<shared_ptr<action::Action>> actions;

            return actions;
        }

        virtual list<shared_ptr<action::Action>> useTrinket(Trinket trinket, shared_ptr<cooldown::Cooldown> cooldown)
        {
            list<shared_ptr<action::Action>> actions;

            return actions;
        }

        virtual shared_ptr<action::Action> buffAction(shared_ptr<buff::Buff> buff, bool primary_action = false)
        {
            shared_ptr<action::Action> action = make_shared<action::Action>(action::TYPE_BUFF);
            action->buff = buff;
            action->primary_action = primary_action;
            return action;
        }

        virtual shared_ptr<action::Action> buffExpireAction(shared_ptr<buff::Buff> buff, bool primary_action = false)
        {
            shared_ptr<action::Action> action = make_shared<action::Action>(action::TYPE_BUFF_EXPIRE);
            action->buff = buff;
            action->primary_action = primary_action;
            return action;
        }

        virtual shared_ptr<action::Action> debuffAction(shared_ptr<debuff::Debuff> debuff, bool primary_action = false)
        {
            shared_ptr<action::Action> action = make_shared<action::Action>(action::TYPE_DEBUFF);
            action->debuff = debuff;
            action->primary_action = primary_action;
            return action;
        }

        virtual shared_ptr<action::Action> cooldownAction(shared_ptr<cooldown::Cooldown> cooldown)
        {
            shared_ptr<action::Action> action = make_shared<action::Action>(action::TYPE_COOLDOWN);
            action->cooldown = cooldown;
            return action;
        }

        virtual shared_ptr<action::Action> cooldownExpireAction(shared_ptr<cooldown::Cooldown> cooldown)
        {
            shared_ptr<action::Action> action = make_shared<action::Action>(action::TYPE_COOLDOWN_EXPIRE);
            action->cooldown = cooldown;
            return action;
        }

        virtual shared_ptr<action::Action> spellAction(shared_ptr<spell::Spell> spell)
        {
            shared_ptr<action::Action> action = make_shared<action::Action>(action::TYPE_SPELL);
            action->spell = spell;
            return action;
        }

        virtual shared_ptr<action::Action> manaAction(double mana, string str = "")
        {
            shared_ptr<action::Action> action = make_shared<action::Action>(action::TYPE_MANA);
            action->value = mana;
            action->str = str;
            return action;
        }

        virtual shared_ptr<action::Action> gcdAction(double t)
        {
            if (t < t_gcd) {
                shared_ptr<action::Action> action = make_shared<action::Action>(action::TYPE_WAIT);
                action->value = t_gcd - t;
                action->str = "GCD";

                if (last_spell_cast_time > 0)
                    t_gcd_capped+= action->value;

                return action;
            }

            return NULL;
        }

        virtual shared_ptr<action::Action> nextAction(shared_ptr<State> state)
        {
            return NULL;
        }

    };

}