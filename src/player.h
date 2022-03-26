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
        bool on_hot_streak = false;

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

        Talents getTalents()
        {
            return talents;
        }

        void setTalents(Talents _talents)
        {
            talents = _talents;
        }

        Glyphs getGlyphs()
        {
            return glyphs;
        }

        void setGlyphs(Glyphs _glyphs)
        {
            glyphs = _glyphs;
        }

        Faction faction()
        {
            if (race == RACE_GNOME || race == RACE_HUMAN || race == RACE_DRAENEI)
                return FACTION_ALLIANCE;
            return FACTION_HORDE;
        }

        bool hasTrinket(Trinket trinket)
        {
            return config->trinket1 == trinket || config->trinket2 == trinket;
        }

        double manaPerSecond()
        {
            double mps = staticManaPerSecond();
            double spi = spiritManaPerSecond();

            if (config->mana_replenishment)
                mps+= maxMana() * 0.01 / 5.0;

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

        double maxMana()
        {
            double mana = Unit::maxMana();

            if (config->meta_gem == META_BEAMING_EARTHSIEGE)
                mana*= 1.02;

            return mana;
        }

        double baseCastTime(shared_ptr<spell::Spell> spell)
        {
            double t = Unit::baseCastTime(spell);

            if (spell->id == spell::FROSTBOLT && talents.imp_frostbolt)
                t-= talents.imp_frostbolt*0.1;
            if (spell->id == spell::FROSTBOLT && talents.empowered_frostbolt)
                t-= talents.empowered_frostbolt*0.1;

            if (spell->id == spell::FIREBALL && talents.imp_fireball)
                t-= talents.imp_fireball*0.1;

            if (spell->id == spell::FIREBALL && glyphs.fireball)
                t-= 0.15;

            if (spell->id == spell::ARCANE_MISSILES && hasBuff(buff::MISSILE_BARRAGE))
                t-= 2.5;

            if (spell->id == spell::PYROBLAST && hasBuff(buff::HOT_STREAK))
                t = 0;
            if (spell->id == spell::FLAMESTRIKE && hasBuff(buff::FIRESTARTER))
                t = 0;

            return t;
        }

        double castHaste()
        {
            double haste = 1.0 / Unit::castHaste();

            if (talents.netherwind_presence)
                haste*= 1 + talents.netherwind_presence*0.02;

            return 1.0 / haste;
        }

        double hitChance(shared_ptr<spell::Spell> spell, double dlevel = 3)
        {
            double hit = stats.hit;

            if (spell->school == SCHOOL_ARCANE && talents.arcane_focus)
                hit+= talents.arcane_focus*1.0;

            return hit;
        }

        double critChance(shared_ptr<spell::Spell> spell)
        {
            double crit = stats.crit;

            if (config->molten_armor) {
                double multi = 0.35;
                if (glyphs.molten_armor)
                    multi+= 0.2;
                crit+= critRatingToChance(round(getSpirit() * multi));
            }

            if (hasBuff(buff::FOCUS_MAGIC))
                crit+= 3.0;

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
                    spell->id == spell::LIVING_BOMB_EXPLOSION ||
                    spell->id == spell::PYROBLAST)
                {
                    crit+= talents.world_in_flames*2.0;
                }
            }

            if ((spell->school == SCHOOL_FIRE || spell->school == SCHOOL_FROSTFIRE) && talents.critical_mass)
                crit+= talents.critical_mass*2.0;

            if (hasBuff(buff::COMBUSTION) && (spell->school == SCHOOL_FIRE || spell->school == SCHOOL_FROSTFIRE))
                crit+= buffStacks(buff::COMBUSTION)*10.0;

            return crit;
        }

        bool canCrit(shared_ptr<spell::Spell> spell)
        {
            if (glyphs.living_bomb && spell->id == spell::LIVING_BOMB)
                return true;

            return Unit::canCrit(spell);
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

            if (talents.burnout)
                multi+= talents.spell_power*0.1;

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

            if (talents.torment_of_the_weak) {
                if (spell->id == spell::FROSTBOLT ||
                    spell->id == spell::FIREBALL ||
                    spell->id == spell::FROSTFIRE_BOLT ||
                    spell->id == spell::PYROBLAST ||
                    spell->id == spell::ARCANE_MISSILES ||
                    spell->id == spell::ARCANE_BLAST ||
                    spell->id == spell::ARCANE_BARRAGE)
                {
                    multi*= 1 + (0.04 * talents.torment_of_the_weak);
                }
            }

            // Below 35% - We'll estimate that to last 35% of duration
            if (talents.molten_fury && state->t / state->duration >= 0.65)
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

            if (spell->id == spell::FLAMESTRIKE && hasBuff(buff::FIRESTARTER))
                return 0;

            if (talents.precision)
                multi*= (1.0 - talents.precision*0.01);

            if (talents.arcane_focus && spell->school == SCHOOL_ARCANE)
                multi*= (1.0 - talents.arcane_focus*0.01);

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

            // Cooldowns
            if (spell->id == spell::FIRE_BLAST)
                actions.push_back(cooldownAction(make_shared<cooldown::FireBlast>(talents.imp_fire_blast)));
            if (spell->id == spell::ARCANE_BARRAGE)
                actions.push_back(cooldownAction(make_shared<cooldown::ArcaneBarrage>()));
            if (spell->id == spell::BLAST_WAVE)
                actions.push_back(cooldownAction(make_shared<cooldown::BlastWave>()));
            if (spell->id == spell::CONE_OF_COLD)
                actions.push_back(cooldownAction(make_shared<cooldown::ConeOfCold>()));
            if (spell->id == spell::DRAGONS_BREATH)
                actions.push_back(cooldownAction(make_shared<cooldown::DragonsBreath>()));

            if (random<int>(0, 99) < talents.clearcast * 2)
                actions.push_back(buffAction(make_shared<buff::Clearcast>()));

            if (spell->id == spell::ARCANE_MISSILES && hasBuff(buff::MISSILE_BARRAGE))
                actions.push_back(buffExpireAction(make_shared<buff::MissileBarrage>()));
            if (spell->id == spell::PYROBLAST && hasBuff(buff::HOT_STREAK))
                actions.push_back(buffExpireAction(make_shared<buff::HotStreak>()));

            if (talents.firestarter && (spell->id == spell::BLAST_WAVE || spell->id == spell::DRAGONS_BREATH))
                actions.push_back(buffAction(make_shared<buff::Firestarter>()));

            if (talents.missile_barrage) {
                if (spell->id == spell::ARCANE_BLAST ||
                    spell->id == spell::ARCANE_BARRAGE ||
                    spell->id == spell::FIREBALL ||
                    spell->id == spell::FROSTBOLT ||
                    spell->id == spell::FROSTFIRE_BOLT)
                {
                    int chance = talents.missile_barrage*4;
                    if (spell->id == spell::ARCANE_BLAST)
                        chance*= 2;

                    if (random<int>(0, 99) < chance)
                        actions.push_back(buffAction(make_shared<buff::MissileBarrage>()));
                }
            }

            return actions;
        }

        std::list<shared_ptr<action::Action>> onSpellImpactProc(shared_ptr<State> state, shared_ptr<spell::SpellInstance> instance)
        {
            std::list<shared_ptr<action::Action>> actions = Unit::onSpellImpactProc(state, instance);

            if (instance->result != spell::MISS) {
                if (talents.imp_scorch && instance->spell->id == spell::SCORCH)
                    actions.push_back(debuffAction(make_shared<debuff::ImprovedScorch>()));

                if (instance->spell->id == spell::IGNITE && talents.empowered_fire) {
                    double chance = talents.empowered_fire * 33;
                    if (talents.empowered_fire > 1)
                        chance+= 1;
                    if (chance == 100 || random<int>(0, 99) < chance)
                        actions.push_back(manaAction(base_mana * 0.02, "Ignite"));
                }
            }

            if (instance->result == spell::CRIT) {
                if (hasTrinket(TRINKET_ASHTONGUE_TALISMAN) && random<int>(0, 1))
                    actions.push_back(buffAction(make_shared<buff::AshtongueTalisman>()));

                if (talents.master_of_elements)
                    actions.push_back(manaAction(baseManaCost(instance->spell) * 0.1 * talents.master_of_elements, "Master of Elements"));

                // Ignite
                if (talents.ignite && instance->spell->school == SCHOOL_FIRE && !instance->spell->dot) {
                    shared_ptr<action::Action> ignite = make_shared<action::Action>(action::TYPE_SPELL);
                    // 40% over for ticks = 10%
                    ignite->spell = make_shared<spell::Ignite>(round(instance->dmg * 0.1));
                    actions.push_back(ignite);
                }

                if (talents.burnout)
                    actions.push_back(manaAction(instance->spell->actual_cost * -0.01 * talents.burnout, "Burnout"));
            }

            if (talents.hot_streak) {
                double hot_streaked = false;

                if (instance->spell->id == spell::FIREBALL ||
                    instance->spell->id == spell::SCORCH ||
                    instance->spell->id == spell::LIVING_BOMB_EXPLOSION ||
                    instance->spell->id == spell::FROSTFIRE_BOLT)
                {
                    if (instance->result == spell::CRIT) {
                        if (on_hot_streak)
                            actions.push_back(buffAction(make_shared<buff::HotStreak>()));
                        else
                            hot_streaked = true;
                    }
                }

                on_hot_streak = hot_streaked;
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