using namespace std;

namespace unit
{

    class Player : public Unit
    {

    public:
        Talents talents;
        Glyphs glyphs;
        Race race = RACE_UNDEAD;

        int combustion = 0;

        Player(shared_ptr<Config> _config) : Unit(_config)
        {
            base_mana = 3268;
            mana_sapphire = 3;
            mana_emerald = 3;
            name = "The player";
        }

        Stats getStats()
        {
            return stats;
        }

        void setStats(Stats _stats)
        {
            stats = _stats;
        }

        void loadTalentsFromString(std::string str)
        {
            int tree = 0, t = 0, p = 0;

            for (int i=0; i<str.length(); i++) {
                if (str[i] == '-') {
                    tree++;
                    t = 0;
                }
                else {
                    p = str[i] - '0';

                    if (tree == 0 && t == 1) talents.arcane_focus = p;
                    else if (tree == 0 && t == 5) talents.clearcast = p;
                    else if (tree == 0 && t == 7) talents.spell_impact = p;
                    else if (tree == 0 && t == 8) talents.student_of_the_mind = p;
                    else if (tree == 0 && t == 9) talents.focus_magic = p;
                    else if (tree == 0 && t == 12) talents.arcane_meditation = p;
                    else if (tree == 0 && t == 13) talents.torment_of_the_weak = p;
                    else if (tree == 0 && t == 15) talents.presence_of_mind = p;
                    else if (tree == 0 && t == 16) talents.arcane_mind = p;
                    else if (tree == 0 && t == 18) talents.arcane_instability = p;
                    else if (tree == 0 && t == 19) talents.arcane_potency = p;
                    else if (tree == 0 && t == 20) talents.arcane_empowerment = p;
                    else if (tree == 0 && t == 21) talents.arcane_power = p;
                    else if (tree == 0 && t == 23) talents.arcane_floes = p;
                    else if (tree == 0 && t == 24) talents.mind_mastery = p;
                    else if (tree == 0 && t == 26) talents.missile_barrage = p;
                    else if (tree == 0 && t == 27) talents.netherwind_presence = p;
                    else if (tree == 0 && t == 28) talents.spell_power = p;
                    else if (tree == 0 && t == 29) talents.arcane_barrage = p;

                    else if (tree == 1 && t == 0) talents.imp_fire_blast = p;
                    else if (tree == 1 && t == 1) talents.incineration = p;
                    else if (tree == 1 && t == 2) talents.imp_fireball = p;
                    else if (tree == 1 && t == 3) talents.ignite = p;
                    else if (tree == 1 && t == 5) talents.world_in_flames = p;
                    else if (tree == 1 && t == 8) talents.pyroblast = p;
                    else if (tree == 1 && t == 10) talents.imp_scorch = p;
                    else if (tree == 1 && t == 12) talents.master_of_elements = p;
                    else if (tree == 1 && t == 13) talents.playing_with_fire = p;
                    else if (tree == 1 && t == 14) talents.critical_mass = p;
                    else if (tree == 1 && t == 15) talents.blast_weave = p;
                    else if (tree == 1 && t == 17) talents.fire_power = p;
                    else if (tree == 1 && t == 18) talents.pyromaniac = p;
                    else if (tree == 1 && t == 19) talents.combustion = p;
                    else if (tree == 1 && t == 20) talents.molten_fury = p;
                    else if (tree == 1 && t == 22) talents.empowered_fire = p;
                    else if (tree == 1 && t == 23) talents.firestarter = p;
                    else if (tree == 1 && t == 24) talents.dragons_breath = p;
                    else if (tree == 1 && t == 25) talents.hot_streak = p;
                    else if (tree == 1 && t == 26) talents.burnout = p;
                    else if (tree == 1 && t == 27) talents.living_bomb = p;

                    else if (tree == 2 && t == 1) talents.imp_frostbolt = p;
                    else if (tree == 2 && t == 2) talents.ice_floes = p;
                    else if (tree == 2 && t == 3) talents.ice_shards = p;
                    else if (tree == 2 && t == 5) talents.precision = p;
                    else if (tree == 2 && t == 7) talents.piercing_ice = p;
                    else if (tree == 2 && t == 8) talents.icy_veins = p;
                    else if (tree == 2 && t == 11) talents.frost_channeling = p;
                    else if (tree == 2 && t == 12) talents.shatter = p;
                    else if (tree == 2 && t == 13) talents.cold_snap = p;
                    else if (tree == 2 && t == 14) talents.imp_cone_of_cold = p;
                    else if (tree == 2 && t == 16) talents.cold_as_ice = p;
                    else if (tree == 2 && t == 17) talents.winters_chill = p;
                    else if (tree == 2 && t == 20) talents.arctic_winds = p;
                    else if (tree == 2 && t == 21) talents.empowered_frostbolt = p;
                    else if (tree == 2 && t == 22) talents.fingers_of_frost = p;
                    else if (tree == 2 && t == 23) talents.brain_freeze = p;
                    else if (tree == 2 && t == 24) talents.water_elemental = p;
                    else if (tree == 2 && t == 25) talents.enduring_winter = p;
                    else if (tree == 2 && t == 26) talents.chilled_to_the_bone = p;
                    else if (tree == 2 && t == 27) talents.deep_freeze = p;

                    t++;
                }
            }
        }

        Faction faction()
        {
            if (race == RACE_GNOME || race == RACE_HUMAN || race == RACE_DRAENEI)
                return FACTION_ALLIANCE;
            return FACTION_HORDE;
        }

        double manaPerSecond()
        {
            double mps = staticManaPerSecond();
            double spi = spiritManaPerSecond();

            double while_casting = 0;
            if (talents.arcane_meditation) {
                if (talents.arcane_meditation == 1)
                    while_casting+= 0.17;
                else if (talents.arcane_meditation == 2)
                    while_casting+= 0.33;
                else if (talents.arcane_meditation == 3)
                    while_casting+= 0.5;
            }
            if (talents.pyromaniac) {
                if (talents.pyromaniac == 1)
                    while_casting+= 0.17;
                else if (talents.pyromaniac == 2)
                    while_casting+= 0.33;
                else if (talents.pyromaniac == 3)
                    while_casting+= 0.5;
            }
            if (config->mage_armor) {
                while_casting+= 0.5;
                if (glyphs.mage_armor)
                    while_casting+= 0.2;
            }

            mps+= while_casting * spi;

            return mps;
        }

        double manaGem()
        {
            double mana = Unit::manaGem();

            if (glyphs.mana_gem)
                mana*= 1.4;

            return mana;
        }

        double baseCastTime(shared_ptr<spell::Spell> spell)
        {
            double t = Unit::baseCastTime(spell);

            if (spell->id == spell::FROSTBOLT && talents.imp_frostbolt)
                t-= talents.imp_frostbolt*0.1;

            if (spell->id == spell::FIREBALL && talents.imp_fireball)
                t-= talents.imp_fireball*0.1;

            if (spell->id == spell::ARCANE_MISSILES && hasBuff(buff::MISSILE_BARRAGE))
                t-= 2.5;

            return t;
        }

        double castHaste(shared_ptr<spell::Spell> spell)
        {
            double haste = 1;

            if (talents.netherwind_presence)
                haste*= talents.netherwind_presence*0.02;

            return haste;
        }

        double hitChance(shared_ptr<spell::Spell> spell, double dlevel = 3)
        {
            double hit = stats.hit;

            if (spell->school == SCHOOL_ARCANE && talents.arcane_focus)
                hit+= talents.arcane_focus*1.0;

            if (talents.precision)
                hit+= talents.precision;

            return hit;
        }

        double critChance(shared_ptr<spell::Spell> spell)
        {
            double crit = stats.crit;

            if (talents.arcane_potency) {
                if (hasBuff(buff::CLEARCAST) || hasBuff(buff::PRESENCE_OF_MIND))
                    crit+= talents.arcane_potency*15.0;
            }

            if (talents.spell_impact) {
                if (spell->id == spell::ARCANE_BLAST ||
                    spell->id == spell::ARCANE_EXPLOSION ||
                    spell->id == spell::BLAST_WAVE ||
                    spell->id == spell::CONE_OF_COLD ||
                    spell->id == spell::FIREBALL ||
                    spell->id == spell::FIRE_BLAST ||
                    spell->id == spell::ICE_LANCE ||
                    spell->id == spell::SCORCH)
                {
                    crit+= talents.spell_impact*2.0;
                }
            }

            if (talents.incineration) {
                if (spell->id == spell::ARCANE_BLAST ||
                    spell->id == spell::CONE_OF_COLD ||
                    spell->id == spell::FIRE_BLAST ||
                    spell->id == spell::SCORCH)
                {
                    crit+= talents.incineration*2.0;
                }
            }

            if (talents.world_in_flames) {
                if (spell->id == spell::ARCANE_EXPLOSION ||
                    spell->id == spell::BLAST_WAVE ||
                    spell->id == spell::DRAGONS_BREATH ||
                    spell->id == spell::FLAMESTRIKE ||
                    spell->id == spell::LIVING_BOMB ||
                    spell->id == spell::PYROBLAST)
                {
                    crit+= talents.world_in_flames*2.0;
                }
            }

            if ((spell->school == SCHOOL_FIRE || spell->school == SCHOOL_FROSTFIRE) && talents.critical_mass)
                crit+= talents.critical_mass*2.0;

            if (hasBuff(buff::COMBUSTION) && (spell->school == SCHOOL_FIRE || spell->school == SCHOOL_FROSTFIRE))
                crit+= buffStacks(buff::COMBUSTION)*10.0;

            if (talents.pyromaniac)
                crit+= talents.pyromaniac*1.0;

            return crit;
        }

        double baseCritMultiplier(shared_ptr<spell::Spell> spell)
        {
            double base = 1;

            if (config->meta_gem == META_CHAOTIC_SKYFLARE)
                base*= 1.03;

            return base;
        }

        double critMultiplierMod(shared_ptr<spell::Spell> spell)
        {
            double multi = Unit::critMultiplierMod(spell);

            if (talents.spell_power)
                multi+= talents.spell_power*0.25;

            if ((spell->school == SCHOOL_FROST || spell->school == SCHOOL_FROSTFIRE) && talents.ice_shards) {
                if (talents.ice_shards == 3)
                    multi+= 1;
                else
                    multi+= talents.ice_shards*0.33;
            }

            if (hasBuff(buff::COMBUSTION) && (spell->school == SCHOOL_FIRE || spell->school == SCHOOL_FROSTFIRE))
                multi+= 0.5;

            return multi;
        }

        double spellCoeffMod(shared_ptr<spell::Spell> spell)
        {
            double coeff = Unit::spellCoeffMod(spell);

            if (talents.arcane_empowerment) {
                if (spell->id == spell::ARCANE_MISSILES)
                    coeff+= talents.arcane_empowerment * 0.15;
                if (spell->id == spell::ARCANE_BLAST)
                    coeff+= talents.arcane_empowerment * 0.03;
            }

            if (talents.empowered_fire) {
                if (spell->id == spell::FIREBALL ||
                    spell->id == spell::FROSTFIRE_BOLT ||
                    spell->id == spell::PYROBLAST)
                {
                    coeff+= talents.empowered_fire * 0.05;
                }
            }

            if (talents.empowered_frostbolt && spell->id == spell::FROSTBOLT)
                coeff+= talents.empowered_frostbolt * 0.05;

            return coeff;
        }

        double buffDmgMultiplier(shared_ptr<spell::Spell> spell, shared_ptr<State> state)
        {
            double multi = 1;

            if (talents.arcane_instability)
                multi*= 1 + (talents.arcane_instability * 0.01);
            if (talents.playing_with_fire)
                multi*= 1 + (talents.playing_with_fire * 0.01);
            if (talents.piercing_ice && (spell->school == SCHOOL_FROST || spell->school == SCHOOL_FROSTFIRE))
                multi*= 1 + (talents.piercing_ice * 0.02);
            if (talents.arctic_winds && (spell->school == SCHOOL_FROST || spell->school == SCHOOL_FROSTFIRE))
                multi*= 1 + (talents.arctic_winds * 0.01);
            if (talents.fire_power && (spell->school == SCHOOL_FIRE || spell->school == SCHOOL_FROSTFIRE))
                multi*= 1 + (talents.fire_power * 0.02);
            if (talents.torment_of_the_weak)
                multi*= 1 + (0.04 * talents.torment_of_the_weak);

            // Below 35% - We'll estimate that to last 35% of duration
            if (talents.molten_fury && state->t / state->duration >= 0.8)
                multi*= 1 + (talents.molten_fury * 0.06);

            if (hasBuff(buff::ARCANE_POWER) && !spell->proc)
                multi*= 1.2;

            if (spell->school == SCHOOL_ARCANE && hasBuff(buff::ARCANE_BLAST))
                multi*= 1.15 * buffStacks(buff::ARCANE_BLAST);

            return multi;
        }

        double manaCostMod(shared_ptr<spell::Spell> spell)
        {
            double base_cost = baseManaCost(spell);
            double cost = Unit::manaCostMod(spell);

            if (talents.frost_channeling && (spell->school == SCHOOL_FROST || spell->school == SCHOOL_FROSTFIRE)) {
                if (talents.frost_channeling == 1)
                    cost-= base_cost * talents.frost_channeling*0.04;
                else if (talents.frost_channeling == 2)
                    cost-= base_cost * talents.frost_channeling*0.07;
                else if (talents.frost_channeling == 3)
                    cost-= base_cost * talents.frost_channeling*0.1;
            }

            if (spell->id == spell::ARCANE_BLAST)
                cost+= base_cost * 1.75 * buffStacks(buff::ARCANE_BLAST);

            return cost;
        }

        double manaCostMultiplier(shared_ptr<spell::Spell> spell)
        {
            double multi = Unit::manaCostMultiplier(spell);

            if (talents.precision)
                multi*= (1.0 - talents.precision*0.01);

            return multi;
        }

        double intellectMultiplier()
        {
            double multi = Unit::intellectMultiplier();

            if (race == RACE_GNOME)
                multi*= 1.05;

            if (talents.arcane_mind)
                multi*= (1 + talents.arcane_mind * 0.03);

            return multi;
        }

        double spiritMultiplier()
        {
            double multi = Unit::spiritMultiplier();

            if (race == RACE_HUMAN)
                multi*= 1.1;

            if (talents.student_of_the_mind) {
                if (talents.student_of_the_mind == 1)
                    multi*= 1.04;
                else if (talents.student_of_the_mind == 2)
                    multi*= 1.07;
                else if (talents.student_of_the_mind == 3)
                    multi*= 1.1;
            }

            return multi;
        }

        double spellPowerMultiplier()
        {
            double multi = Unit::spellPowerMultiplier();

            if (talents.mind_mastery)
                multi*= (1 + getIntellect() * talents.mind_mastery * 0.03);

            return multi;
        }

        double cooldownMod(shared_ptr<cooldown::Cooldown> cooldown)
        {
            double mod = Unit::cooldownMod(cooldown);

            if (talents.arcane_floes) {
                if (cooldown->id == cooldown::PRESENCE_OF_MIND || cooldown->id == cooldown::ARCANE_POWER)
                    mod-= cooldown->duration * 0.15 * talents.arcane_floes;
                if (cooldown->id == cooldown::EVOCATION)
                    mod-= 60 * talents.arcane_floes;
            }

            if (talents.ice_floes) {
                if (cooldown->id == cooldown::CONE_OF_COLD || cooldown->id == cooldown::ICY_VEINS) {
                    if (talents.ice_floes == 1)
                        mod-= cooldown->duration * 0.07 * talents.ice_floes;
                    else if (talents.ice_floes == 2)
                        mod-= cooldown->duration * 0.14 * talents.ice_floes;
                    else if (talents.ice_floes == 3)
                        mod-= cooldown->duration * 0.2 * talents.ice_floes;
                }
            }

            return mod;
        }

        std::list<shared_ptr<action::Action>> onCastSuccessProc(shared_ptr<State> state, shared_ptr<spell::Spell> spell)
        {
            std::list<shared_ptr<action::Action>> actions = Unit::onCastSuccessProc(state, spell);

            // Clearcast
            if (random<double>(0, 100) <= talents.clearcast * 2)
                actions.push_back(buffAction(make_shared<buff::Clearcast>()));

            return actions;
        }

        std::list<shared_ptr<action::Action>> onSpellImpactProc(shared_ptr<State> state, shared_ptr<spell::SpellInstance> instance)
        {
            std::list<shared_ptr<action::Action>> actions = Unit::onSpellImpactProc(state, instance);

            if (instance->result == spell::CRIT) {
                //
            }

            return actions;
        }

        shared_ptr<action::Action> nextAction(shared_ptr<State> state)
        {
            shared_ptr<action::Action> action = NULL;

            // TODO: Next action logic
            action = spellAction(make_shared<spell::Fireball>());

            return action;
        }

    };

}