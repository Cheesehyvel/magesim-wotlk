using namespace std;

namespace unit
{

    class Unit
    {

    public:
        shared_ptr<Config> config;
        Stats stats;
        string name;
        double base_mana = 2241;
        double mana;
        double t_gcd;
        double t_gcd_capped = 0;
        int mana_sapphire;
        int mana_emerald;

        map<cooldown::ID, shared_ptr<cooldown::Cooldown>> cooldowns;
        map<buff::ID, shared_ptr<buff::Buff>> buffs;
        map<debuff::ID, shared_ptr<debuff::Debuff>> debuffs;

        Unit(shared_ptr<Config> _config)
        {
            config = _config;
        }

        void reset()
        {
            mana = maxMana();
            t_gcd = 0;
            t_gcd_capped = 0;

            mana_sapphire = 0;
            mana_emerald = 0;

            buffs.clear();
            debuffs.clear();
            cooldowns.clear();
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

        int buffStacks(buff::ID id)
        {
            if (hasBuff(id))
                return buffs[id]->stacks;
            return 0;
        }

        bool hasBuff(buff::ID id)
        {
            return buffs.find(id) != buffs.end();
        }

        int addBuff(shared_ptr<buff::Buff> buff)
        {
            if (hasBuff(buff->id))
                return buffs[buff->id]->addStack();
            else
                buffs[buff->id] = buff;

            return 1;
        }

        void removeBuff(buff::ID id)
        {
            buffs.erase(id);
        }

        shared_ptr<buff::Buff> getBuff(buff::ID id)
        {
            if (hasBuff(id))
                return buffs[id];
            return NULL;
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
            double mps = 0.01 * base_mana;

            if (stats.mp5)
                mps+= stats.mp5/5.0;

            return mps;
        }

        virtual double manaPerSecond()
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

        double manaPerTick()
        {
            return manaPerSecond() * 2;
        }

        double gcd(double t = 1.5)
        {
            double cap = 1.0;

            t*= castHaste();

            if (t < cap)
                t = cap;

            return t;
        }

        virtual double manaGem()
        {
            if (mana_sapphire > 0) {
                mana_sapphire--;
                return round(random<double>(3330, 3500));
            }
            else if (mana_emerald > 0) {
                mana_emerald--;
                mana = round(random<double>(2340, 2460));
            }

            return 0;
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
            double rating = stats.haste_rating;

            if (hasBuff(buff::SKULL_GULDAN))
                rating+= 175;
            if (hasBuff(buff::MQG))
                rating+= 330;
            if (hasBuff(buff::ASHTONGUE_TALISMAN))
                rating+= 145;
            if (hasBuff(buff::DRUMS_OF_BATTLE))
                rating+= 80;

            if (rating)
                haste+= hasteRatingToHaste(rating) / 100.0;

            if (stats.haste)
                haste*= 1+stats.haste/100.0;

            if (hasBuff(buff::BLOODLUST))
                haste*= 1.3;
            else if (hasBuff(buff::POWER_INFUSION))
                haste*= 1.2;
            else if (hasBuff(buff::ICY_VEINS))
                haste*= 1.2;
            else if (hasBuff(buff::BERSERKING))
                haste*= 1.2;

            return 1.0 / haste;
        }

        virtual double hitChance(shared_ptr<spell::Spell> spell, double dlevel = 3)
        {
            return stats.hit;
        }

        virtual double critChance(shared_ptr<spell::Spell> spell)
        {
            return stats.crit;
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
            return 1;
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
            return stats.intellect * intellectMultiplier();
        }

        virtual double getSpirit()
        {
            return stats.spirit * spiritMultiplier();
        }

        virtual double getSpellPower()
        {
            return stats.spell_power * spellPowerMultiplier();
        }

        virtual double intellectMultiplier()
        {
            return 1;
        }

        virtual double spiritMultiplier()
        {
            return 1;
        }

        virtual double spellPowerMultiplier()
        {
            return 1;
        }

        virtual double cooldownMod(shared_ptr<cooldown::Cooldown> cooldown)
        {
            return 0;
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

        virtual shared_ptr<action::Action> spellAction(shared_ptr<spell::Spell> spell)
        {
            shared_ptr<action::Action> action = make_shared<action::Action>(action::TYPE_SPELL);
            action->spell = spell;
            action->primary_action = spell->active_use;
            return action;
        }

        virtual shared_ptr<action::Action> manaAction(double mana, string str = "")
        {
            shared_ptr<action::Action> action = make_shared<action::Action>(action::TYPE_MANA);
            action->value = mana;
            action->str = str;
            return action;
        }

        virtual shared_ptr<action::Action> nextAction(shared_ptr<State> state)
        {
            return NULL;
        }

    };

}