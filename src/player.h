using namespace std;

namespace unit
{

    class Player : public Unit
    {

    public:
        Stats summon_stats;
        Talents talents;
        Glyphs glyphs;
        Race race = RACE_UNDEAD;

        int combustion;
        int fingers_of_frost;
        bool heating_up;
        double t_living_bomb;
        double t_flamestrike;
        double t_scorch;
        double t_mana_spent;
        double fire_ward;
        double mana_shield;
        double frozen_rune;
        int mana_sapphire;
        int ab_streak;
        bool waited;
        bool should_wait;

        Player(shared_ptr<Config> _config) : Unit(_config)
        {
            base_mana = 3268;
            name = "Player";
            id = 1;
        }

        void reset()
        {
            Unit::reset();
            combustion = 0;
            fingers_of_frost = 0;
            heating_up = false;
            t_living_bomb = -20;
            t_flamestrike = -20;
            t_scorch = -60;
            t_mana_spent = 0;
            fire_ward = 0;
            mana_shield = 0;
            frozen_rune = 0;
            mana_sapphire = 3;
            ab_streak = 0;
            waited = false;
            should_wait = false;
        }

        Stats getStats()
        {
            return stats;
        }

        void setStats(Stats _stats)
        {
            stats = _stats;
        }

        Stats getSummonStats()
        {
            return summon_stats;
        }

        void setSummonStats(Stats _stats)
        {
            summon_stats = _stats;
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

        double manaPerSecond(shared_ptr<State> state)
        {
            double mps = staticManaPerSecond();
            double spi = spiritManaPerSecond();

            if (config->mana_replenishment)
                mps+= maxMana() * 0.01 / 5.0;

            double while_casting = 0;
            if (t_mana_spent + 5 <= state->t) {
                while_casting = 1;
            }
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
                if (config->t9_2set)
                    while_casting+= 0.1;
            }

            mps+= min(1.0, while_casting) * spi;

            return mps;
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
            if ((spell->id == spell::FIREBALL || spell->id == spell::FROSTFIRE_BOLT) && hasBuff(buff::BRAIN_FREEZE))
                t = 0;

            return t;
        }

        double castHaste()
        {
            double haste = 1.0 / Unit::castHaste();

            if (talents.netherwind_presence)
                haste*= 1 + talents.netherwind_presence*0.02;

            if (hasBuff(buff::PUSHING_THE_LIMIT))
                haste*= 1.12;

            return 1.0 / haste;
        }

        double hitChance(shared_ptr<spell::Spell> spell, double dlevel = 3)
        {
            double hit = Unit::hitChance(spell, dlevel);

            if (spell->school == SCHOOL_ARCANE && talents.arcane_focus)
                hit+= talents.arcane_focus*1.0;

            return hit;
        }

        double critChance(shared_ptr<spell::Spell> spell)
        {
            double crit = Unit::critChance(spell);

            if (config->molten_armor) {
                double multi = 0.35;
                if (glyphs.molten_armor)
                    multi+= 0.2;
                if (config->t9_2set)
                    multi+= 0.15;
                crit+= critRatingToChance(round(getSpirit() * multi));
            }

            if (talents.arcane_potency && hasBuff(buff::ARCANE_POTENCY))
                crit+= talents.arcane_potency*15.0;

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

            if (talents.imp_scorch) {
                if (spell->id == spell::SCORCH ||
                    spell->id == spell::FIREBALL ||
                    spell->id == spell::FROSTFIRE_BOLT)
                {
                    crit+= talents.imp_scorch*1.0;
                }
            }

            if ((spell->school == SCHOOL_FIRE || spell->school == SCHOOL_FROSTFIRE) && talents.critical_mass)
                crit+= talents.critical_mass*2.0;

            if (spell->id == spell::FROSTFIRE_BOLT && glyphs.frostfire)
                crit+= 2;

            if (hasBuff(buff::COMBUSTION) && (spell->school == SCHOOL_FIRE || spell->school == SCHOOL_FROSTFIRE))
                crit+= buffStacks(buff::COMBUSTION)*10.0;

            if (talents.shatter && isFrozen()) {
                crit+= talents.shatter * 17;
                if (talents.shatter == 3)
                    crit-= 1;
            }

            if (config->t9_4set) {
                if (spell->id == spell::ARCANE_BLAST ||
                    spell->id == spell::ARCANE_MISSILES ||
                    spell->id == spell::FIREBALL ||
                    spell->id == spell::FROSTBOLT ||
                    spell->id == spell::FROSTFIRE_BOLT)
                {
                    crit+= 5.0;
                }
            }

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
                multi+= talents.burnout*0.1;
            if (config->t7_4set)
                multi+= 0.05;

            if (spell->id == spell::ARCANE_MISSILES && glyphs.arcane_missiles)
                multi+= 0.25;

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
            double multi = Unit::buffDmgMultiplier(spell, state);

            if (talents.arcane_instability)
                multi*= 1 + talents.arcane_instability * 0.01;
            if (talents.playing_with_fire)
                multi*= 1 + talents.playing_with_fire * 0.01;
            if (talents.piercing_ice && (spell->school == SCHOOL_FROST || spell->school == SCHOOL_FROSTFIRE))
                multi*= 1 + talents.piercing_ice * 0.02;
            if (talents.arctic_winds && (spell->school == SCHOOL_FROST || spell->school == SCHOOL_FROSTFIRE))
                multi*= 1 + talents.arctic_winds * 0.01;
            if (talents.fire_power && (spell->school == SCHOOL_FIRE || spell->school == SCHOOL_FROSTFIRE))
                multi*= 1 + talents.fire_power * 0.02;
            if (talents.chilled_to_the_bone && (spell->id == spell::FROSTBOLT || spell->id == spell::FROSTFIRE_BOLT || spell->id == spell::ICE_LANCE))
                multi*= 1 + talents.chilled_to_the_bone * 0.01;
            if (spell->id == spell::FROSTFIRE_BOLT && glyphs.frostfire)
                multi*= 1.02;
            if (spell->id == spell::FROSTBOLT && glyphs.frostbolt)
                multi*= 1.05;
            if (spell->id == spell::SCORCH && glyphs.scorch)
                multi*= 1.2;
            if (spell->id == spell::CONE_OF_COLD && talents.imp_cone_of_cold)
                multi*= 1.05 + talents.imp_cone_of_cold * 0.1;
            if (config->t6_4set && (spell->id == spell::FIREBALL || spell->id == spell::FROSTBOLT || spell->id == spell::ARCANE_MISSILES))
                multi*= 1.05;
            if (hasBuff(buff::QUAD_CORE))
                multi*= 1.18;
            if (config->udc_2set)
                multi*= 1.01;
            if (config->udc_3set)
                multi*= 1.02;
            if (config->udc_4set)
                multi*= 1.03;
            if (config->cudc_3set)
                multi*= 1.02;

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
                    multi*= 1 + talents.spell_impact*0.02;
                }
            }

            // Below 35% - We'll estimate that to last 35% of duration
            if (talents.molten_fury && state->t / state->duration >= 0.65)
                multi*= 1 + (talents.molten_fury * 0.06);

            if (hasBuff(buff::ARCANE_POWER) && !spell->proc)
                multi*= 1.2;

            if (spell->school == SCHOOL_ARCANE && hasBuff(buff::ARCANE_BLAST, true)) {
                double ab = 0.15;
                if (glyphs.arcane_blast)
                    ab+= 0.03;
                multi*= 1 + ab * buffStacks(buff::ARCANE_BLAST, true);
            }

            if (config->t5_2set && spell->id == spell::ARCANE_BLAST)
                multi*= 1.05;

            if (spell->id == spell::ICE_LANCE && isFrozen()) {
                if (glyphs.ice_lance)
                    multi*= 4;
                else
                    multi*= 3;
            }

            return multi;
        }

        double baseManaCost(shared_ptr<spell::Spell> spell)
        {
            double cost = Unit::baseManaCost(spell);

            if (hasTrinket(TRINKET_SPARK_HOPE)) {
                cost-= 42;
                if (cost < 0)
                    cost = 0;
            }

            return cost;
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

            if (spell->id == spell::ARCANE_BLAST) {
                cost+= base_cost * 1.75 * buffStacks(buff::ARCANE_BLAST);
                if (config->t5_2set)
                    cost+= base_cost * 0.05;
            }

            return cost;
        }

        double manaCostMultiplier(shared_ptr<spell::Spell> spell)
        {
            double multi = Unit::manaCostMultiplier(spell);

            if (hasBuff(buff::CLEARCAST))
                return 0;

            if (spell->id == spell::FLAMESTRIKE && hasBuff(buff::FIRESTARTER))
                return 0;

            if (spell->id == spell::ARCANE_MISSILES && hasBuff(buff::MISSILE_BARRAGE))
                return 0;

            if (talents.precision)
                multi*= (1.0 - talents.precision*0.01);
            if (talents.arcane_focus && spell->school == SCHOOL_ARCANE)
                multi*= (1.0 - talents.arcane_focus*0.01);
            if (spell->id == spell::ARCANE_BARRAGE && glyphs.arcane_barrage)
                multi*= 0.8;
            if (spell->id == spell::ARCANE_EXPLOSION && glyphs.arcane_explosion)
                multi*= 0.9;
            if (glyphs.blast_wave && spell->id == spell::BLAST_WAVE)
                multi*= 0.85;

            return multi;
        }

        double getSpellPower(School school = SCHOOL_NONE)
        {
            double sp = Unit::getSpellPower(school);

            if (hasBuff(buff::FLAME_CAP) && (school == SCHOOL_FIRE || school == SCHOOL_FROSTFIRE))
                sp+= 80.0;

            if (talents.mind_mastery)
                sp+= getIntellect() * talents.mind_mastery * 0.03;

            return sp;
        }

        double getHasteRating()
        {
            double rating = Unit::getHasteRating();

            return rating;
        }

        double shouldConsumeClearcast(shared_ptr<spell::Spell> spell)
        {
            if (spell->is_trigger || spell->tick)
                return false;

            if (spell->id == spell::ARCANE_MISSILES && hasBuff(buff::MISSILE_BARRAGE))
                return false;

            return true;
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
                        mod-= cooldown->duration * 0.07;
                    else if (talents.ice_floes == 2)
                        mod-= cooldown->duration * 0.14;
                    else if (talents.ice_floes == 3)
                        mod-= cooldown->duration * 0.2;
                }
            }

            if (cooldown->id == cooldown::WATER_ELEMENTAL && glyphs.water_elemental)
                mod-= 30;

            if (talents.cold_as_ice) {
                if (cooldown->id == cooldown::COLD_SNAP || cooldown->id == cooldown::WATER_ELEMENTAL)
                    mod*= 1.0 - talents.cold_as_ice * 0.1;
            }

            if (cooldown->id == cooldown::EVOCATION && config->t3_2set)
                mod-= 60;

            return mod;
        }

        bool isFrozen()
        {
            return hasBuff(buff::FINGERS_OF_FROST) || hasBuff(buff::GHOST_FINGERS);
        }

        bool hasChillEffect(shared_ptr<spell::Spell> spell)
        {
            // Fingers of frost still seem to proc even with the glyph? unconfirmed
            // if (spell->id == spell::FROSTBOLT)
                // return !glyphs.frostbolt;
            if (spell->id == spell::CONE_OF_COLD || spell->id == spell::FROSTFIRE_BOLT || spell->id == spell::FROSTBOLT)
                return true;

            return false;
        }

        list<shared_ptr<action::Action>> onBuffGain(shared_ptr<State> state, shared_ptr<buff::Buff> buff)
        {
            list<shared_ptr<action::Action>> actions = Unit::onBuffGain(state, buff);

            if (buff->id == buff::REIGN_UNLIVING_HC && buffStacks(buff->id) == 3) {
                actions.push_back(spellAction(make_shared<spell::PillarOfFlameHc>()));
                actions.push_back(buffExpireAction(make_shared<buff::ReignUnlivingHc>()));
            }
            else if (buff->id == buff::REIGN_UNLIVING_NM && buffStacks(buff->id) == 3) {
                actions.push_back(spellAction(make_shared<spell::PillarOfFlameNm>()));
                actions.push_back(buffExpireAction(make_shared<buff::ReignUnlivingNm>()));
            }
            else if (buff->id == buff::FIRE_WARD) {
                fire_ward = 1950.0 + getSpellPower(SCHOOL_FIRE)*0.1;
            }
            else if (buff->id == buff::MANA_SHIELD) {
                mana_shield = 1330.0 + getSpellPower(SCHOOL_ARCANE)*0.8053;
            }
            else if (buff->id == buff::FROZEN_RUNE) {
                frozen_rune = random<double>(1500, 2500);
                actions.push_back(cooldownAction(make_shared<cooldown::Conjured>(60*3)));
            }
            else if ((buff->id == buff::CLEARCAST || buff->id == buff::PRESENCE_OF_MIND) && talents.arcane_potency) {
                actions.push_back(buffAction(make_shared<buff::ArcanePotency>()));
            }
            // else if (buff->id == buff::HYPERSPEED_ACCELERATION) {
                // actions.push_back(cooldownAction(make_shared<cooldown::TrinketShared>(buff->duration)));
            // }
            else if (buff->id == buff::ARCANE_BLAST) {
                ab_streak++;
            }

            return actions;
        }

        list<shared_ptr<action::Action>> onBuffExpire(shared_ptr<State> state, shared_ptr<buff::Buff> buff)
        {
            list<shared_ptr<action::Action>> actions = Unit::onBuffExpire(state, buff);

            if (buff->id == buff::VOLATILE_POWER_HC)
                actions.push_back(buffExpireAction(make_shared<buff::VolatilityHc>()));
            if (buff->id == buff::VOLATILE_POWER_NM)
                actions.push_back(buffExpireAction(make_shared<buff::VolatilityNm>()));
            if (buff->id == buff::FIRE_WARD)
                fire_ward = 0;
            if (buff->id == buff::ARCANE_BLAST)
                ab_streak = 0;

            return actions;
        }

        list<shared_ptr<action::Action>> onCastSuccessProc(shared_ptr<State> state, shared_ptr<spell::Spell> spell)
        {
            list<shared_ptr<action::Action>> actions = Unit::onCastSuccessProc(state, spell);
            shared_ptr<action::Action> action = NULL;

            if (spell->actual_cost)
                t_mana_spent = state->t;

            if (spell->id == spell::MANA_GEM)
                return useManaGem();

            if (spell->id == spell::SAPPER_CHARGE)
                return actions;

            bool is_harmful = spell->max_dmg > 0;

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
            if (spell->id == spell::DEEP_FREEZE)
                actions.push_back(cooldownAction(make_shared<cooldown::DeepFreeze>()));
            if (spell->id == spell::MIRROR_IMAGE)
                actions.push_back(cooldownAction(make_shared<cooldown::MirrorImage>()));
            if (spell->id == spell::WATER_ELEMENTAL)
                actions.push_back(cooldownAction(make_shared<cooldown::WaterElemental>()));
            if (spell->id == spell::EVOCATION) {
                actions.push_back(cooldownAction(make_shared<cooldown::Evocation>()));
                actions.push_back(buffAction(make_shared<buff::Evocation>(castHaste(), evocationTicks())));
            }
            if (spell->id == spell::COLD_SNAP) {
                actions.push_back(cooldownAction(make_shared<cooldown::ColdSnap>()));
                actions.push_back(cooldownExpireAction(make_shared<cooldown::IcyVeins>()));
                actions.push_back(cooldownExpireAction(make_shared<cooldown::WaterElemental>()));
                actions.push_back(cooldownExpireAction(make_shared<cooldown::ConeOfCold>()));
                actions.push_back(cooldownExpireAction(make_shared<cooldown::DeepFreeze>()));
            }
            if (spell->id == spell::FIRE_WARD) {
                actions.push_back(cooldownAction(make_shared<cooldown::FireWard>()));
                actions.push_back(buffAction(make_shared<buff::FireWard>()));
            }
            if (spell->id == spell::MANA_SHIELD) {
                actions.push_back(buffAction(make_shared<buff::ManaShield>()));
            }
            if (spell->id == spell::FROZEN_RUNE) {
                actions.push_back(buffAction(make_shared<buff::FrozenRune>()));
            }

            if (hasBuff(buff::ARCANE_POTENCY) && !spell->channeling)
                actions.push_back(buffExpireAction(make_shared<buff::ArcanePotency>()));

            if (spell->id == spell::LIVING_BOMB)
                t_living_bomb = state->t;
            if (spell->id == spell::SCORCH)
                t_scorch = state->t;
            if (spell->id == spell::FLAMESTRIKE) {
                t_flamestrike = state->t;
                actions.push_back(spellAction(make_shared<spell::FlamestrikeDot>()));
            }

            if (hasBuff(buff::GHOST_FINGERS))
                actions.push_back(buffExpireAction(make_shared<buff::GhostFingers>()));

            if (hasBuff(buff::PRESENCE_OF_MIND) && spell->cast_time && !spell->channeling)
                actions.push_back(buffExpireAction(make_shared<buff::PresenceOfMind>()));

            if (hasBuff(buff::CLEARCAST) && shouldConsumeClearcast(spell))
                actions.push_back(buffExpireAction(make_shared<buff::Clearcast>()));

            if (spell->id == spell::ARCANE_BLAST)
                actions.push_back(buffAction(make_shared<buff::ArcaneBlast>()));
            else if (hasBuff(buff::ARCANE_BLAST) && spell->school == SCHOOL_ARCANE && spell->min_dmg > 0)
                actions.push_back(buffExpireAction(make_shared<buff::ArcaneBlast>()));

            if (hasBuff(buff::BRAIN_FREEZE) && spell->actual_cast_time == 0 && (spell->id == spell::FROSTFIRE_BOLT || spell->id == spell::FIREBALL)) {
                // 20% chance - Sorta confirmed from elitist jerks
                if (!config->t8_4set || random<int>(0, 4) != 0) {
                    actions.push_back(buffExpireAction(make_shared<buff::BrainFreeze>()));
                    if (config->t10_2set)
                        actions.push_back(buffAction(make_shared<buff::PushingTheLimit>()));
                }
            }

            if (spell->id == spell::ARCANE_MISSILES && hasBuff(buff::MISSILE_BARRAGE)) {
                // 20% chance - Sorta confirmed from elitist jerks
                if (!config->t8_4set || random<int>(0, 4) != 0) {
                    actions.push_back(buffExpireAction(make_shared<buff::MissileBarrage>()));
                    if (config->t10_2set)
                        actions.push_back(buffAction(make_shared<buff::PushingTheLimit>()));
                }
            }
            if (spell->id == spell::PYROBLAST && hasBuff(buff::HOT_STREAK)) {
                // 20% chance - Sorta confirmed from elitist jerks
                if (!config->t8_4set || random<int>(0, 4) != 0) {
                    actions.push_back(buffExpireAction(make_shared<buff::HotStreak>()));
                    if (config->t10_2set)
                        actions.push_back(buffAction(make_shared<buff::PushingTheLimit>()));
                }
            }

            if (talents.firestarter && (spell->id == spell::BLAST_WAVE || spell->id == spell::DRAGONS_BREATH))
                actions.push_back(buffAction(make_shared<buff::Firestarter>()));

            if (talents.fingers_of_frost && hasBuff(buff::FINGERS_OF_FROST)) {
                fingers_of_frost--;
                if (fingers_of_frost <= 0) {
                    actions.push_back(buffExpireAction(make_shared<buff::FingersOfFrost>()));
                    actions.push_back(buffAction(make_shared<buff::GhostFingers>()));
                }
            }

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

            if (spell->id == spell::MIRROR_IMAGE) {
                int images = 3;
                if (glyphs.mirror_image)
                    images++;
                for (int i=0; i<images; i++) {
                    action = make_shared<action::Action>(action::TYPE_UNIT);
                    action->unit = make_shared<unit::MirrorImage>(config);
                    action->unit->setStats(stats);
                    action->unit->name+= " #"+to_string(i+1);
                    actions.push_back(action);
                }
                if (config->t10_4set)
                    actions.push_back(buffAction(make_shared<buff::QuadCore>()));
            }

            if (spell->id == spell::WATER_ELEMENTAL) {
                action = make_shared<action::Action>(action::TYPE_UNIT);
                action->unit = make_shared<unit::WaterElemental>(config);
                action->unit->setStats(stats);
                if (glyphs.eternal_water)
                    action->unit->duration = 0;
                else if (talents.enduring_winter)
                    action->unit->duration+= talents.enduring_winter * 5.0;
                actions.push_back(action);
            }

            if (config->t8_2set && !hasCooldown(cooldown::PRAXIS)) {
                if (spell->id == spell::ARCANE_BLAST ||
                    spell->id == spell::FIREBALL ||
                    spell->id == spell::FROSTBOLT ||
                    spell->id == spell::FROSTFIRE_BOLT)
                {
                    if (random<int>(0, 3) == 0) {
                        action = buffAction(make_shared<buff::Praxis>());
                        action->cooldown = make_shared<cooldown::Praxis>();
                        actions.push_back(action);
                    }
                }
            }

            if (spell->can_proc) {

                // Unconfirmed - on spell cast ?
                if (config->black_magic && !spell->is_trigger && !hasCooldown(cooldown::BLACK_MAGIC) && random<int>(0, 99) < 35) {
                    action = buffAction(make_shared<buff::BlackMagic>());
                    action->cooldown = make_shared<cooldown::BlackMagic>();
                    actions.push_back(action);
                }

                // Unconfirmed - on spell cast ?
                if (config->darkglow_embroidery && !hasCooldown(cooldown::DARKGLOW) && random<int>(0, 99) < 35) {
                    action = manaAction(400, "Darkglow");
                    action->cooldown = make_shared<cooldown::Darkglow>();
                    actions.push_back(action);
                }

                // Confirmed - on harmful spell cast
                if (hasTrinket(TRINKET_FORGE_EMBER) && !hasCooldown(cooldown::FORGE_EMBER) && is_harmful && random<int>(0, 9) == 0) {
                    action = buffAction(make_shared<buff::ForgeEmber>());
                    action->cooldown = make_shared<cooldown::ForgeEmber>();
                    actions.push_back(action);
                }

                // Unconfirmed - on harmful spell cast ?
                if (hasTrinket(TRINKET_PENDULUM_TELLURIC_CURRENTS) && !hasCooldown(cooldown::PENDULUM_TELLURIC_CURRENTS) && is_harmful && random<int>(0, 19) < 3) {
                    action = spellAction(make_shared<spell::PendulumTelluricCurrents>());
                    action->cooldown = make_shared<cooldown::PendulumTelluricCurrents>();
                    actions.push_back(action);
                }

                // Unconfirmed - on spell cast ?
                if (hasTrinket(TRINKET_EMBRACE_SPIDER) && !hasCooldown(cooldown::EMBRACE_SPIDER) && random<int>(0, 9) == 0) {
                    action = buffAction(make_shared<buff::EmbraceSpider>());
                    action->cooldown = make_shared<cooldown::EmbraceSpider>();
                    actions.push_back(action);
                }

                // Unconfirmed - on harmful spell cast ?
                if (hasTrinket(TRINKET_FLARE_HEAVENS) && !hasCooldown(cooldown::FLARE_HEAVENS) && is_harmful && random<int>(0, 9) == 0) {
                    action = buffAction(make_shared<buff::FlareHeavens>());
                    action->cooldown = make_shared<cooldown::FlareHeavens>();
                    actions.push_back(action);
                }

                // Unconfirmed - on spell cast ?
                if (hasTrinket(TRINKET_PANDORAS_PLEA) && !hasCooldown(cooldown::PANDORAS_PLEA) && random<int>(0, 9) == 0) {
                    action = buffAction(make_shared<buff::PandorasPlea>());
                    action->cooldown = make_shared<cooldown::PandorasPlea>();
                    actions.push_back(action);
                }

                // Confirmed - on harmful spell cast
                if (hasTrinket(TRINKET_SUNDIAL_EXILED) && !hasCooldown(cooldown::NOW_IS_THE_TIME) && is_harmful && random<int>(0, 9) == 0) {
                    action = buffAction(make_shared<buff::NowIsTheTime>());
                    action->cooldown = make_shared<cooldown::NowIsTheTime>();
                    actions.push_back(action);
                }

                // Unconfirmed - on spell cast ?
                if (hasTrinket(TRINKET_DYING_CURSE) && !hasCooldown(cooldown::DYING_CURSE) && random<int>(0, 19) < 3) {
                    action = buffAction(make_shared<buff::DyingCurse>());
                    action->cooldown = make_shared<cooldown::DyingCurse>();
                    actions.push_back(action);
                }

                // Unconfirmed - on spell cast ?
                if (hasTrinket(TRINKET_ILLUSTRATION_DRAGON_SOUL)) {
                    action = buffAction(make_shared<buff::IllustrationDragonSoul>());
                    actions.push_back(action);
                }

                // Unconfirmed - on harmful spell cast ?
                if (hasTrinket(TRINKET_ELEMENTAL_FOCUS_STONE) && !hasCooldown(cooldown::ALACRITY_ELEMENTS) && is_harmful && random<int>(0, 9) == 0) {
                    action = buffAction(make_shared<buff::AlacrityElements>());
                    action->cooldown = make_shared<cooldown::AlacrityElements>();
                    actions.push_back(action);
                }

                // Unconfirmed - on spell cast ?
                if (hasTrinket(TRINKET_EYE_BROODMOTHER)) {
                    action = buffAction(make_shared<buff::EyeBroodmother>());
                    actions.push_back(action);
                }

                // Unconfirmed - on spell cast ?
                if (hasTrinket(TRINKET_SIFS_REMEMBERANCE) && !hasCooldown(cooldown::MEMORIES_LOVE) && random<int>(0, 9) == 0) {
                    action = buffAction(make_shared<buff::MemoriesLove>());
                    action->cooldown = make_shared<cooldown::MemoriesLove>();
                    actions.push_back(action);
                }

                // Unconfirmed - on spell cast ?
                if (hasTrinket(TRINKET_SHOW_FAITH) && !hasCooldown(cooldown::SHOW_FAITH) && random<int>(0, 9) == 0) {
                    action = buffAction(make_shared<buff::ShowFaith>());
                    action->cooldown = make_shared<cooldown::ShowFaith>();
                    actions.push_back(action);
                }

                // Unconfirmed - on spell cast ?
                if (hasTrinket(TRINKET_VOLATILE_POWER_HC) && hasBuff(buff::VOLATILE_POWER_HC)) {
                    action = buffAction(make_shared<buff::VolatilityHc>());
                    actions.push_back(action);
                }
                if (hasTrinket(TRINKET_VOLATILE_POWER_NM) && hasBuff(buff::VOLATILE_POWER_NM)) {
                    action = buffAction(make_shared<buff::VolatilityNm>());
                    actions.push_back(action);
                }

                // Unconfirmed - on spell cast ?
                if (hasTrinket(TRINKET_SOLACE_DEFEATED_HC)) {
                    action = buffAction(make_shared<buff::EnergizedHc>());
                    actions.push_back(action);
                }
                if (hasTrinket(TRINKET_SOLACE_DEFEATED_NM)) {
                    action = buffAction(make_shared<buff::EnergizedNm>());
                    actions.push_back(action);
                }

                // Unconfirmed - on harmful spell cast ?
                if (hasTrinket(TRINKET_DISLODGED_OBJECT_HC) && !hasCooldown(cooldown::DISLODGED_OBJECT_HC) && is_harmful && random<int>(0, 9) == 0) {
                    action = buffAction(make_shared<buff::DislodgedObjectHc>());
                    action->cooldown = make_shared<cooldown::DislodgedObjectHc>();
                    actions.push_back(action);
                }
                if (hasTrinket(TRINKET_DISLODGED_OBJECT_NM) && !hasCooldown(cooldown::DISLODGED_OBJECT_NM) && is_harmful && random<int>(0, 9) == 0) {
                    action = buffAction(make_shared<buff::DislodgedObjectNm>());
                    action->cooldown = make_shared<cooldown::DislodgedObjectNm>();
                    actions.push_back(action);
                }

            }

            return actions;
        }

        list<shared_ptr<action::Action>> onSpellImpactProc(shared_ptr<State> state, shared_ptr<spell::SpellInstance> instance)
        {
            list<shared_ptr<action::Action>> actions = Unit::onSpellImpactProc(state, instance);
            shared_ptr<action::Action> action = NULL;

            // Special case for sapper
            if (instance->spell->id == spell::SAPPER_CHARGE) {
                double absorb = 0;
                double abs;
                double dmg = random<double>(2188, 2812);

                if (hasBuff(buff::MANA_SHIELD)) {
                    abs = dmg;
                    if (abs > mana_shield)
                        abs = mana_shield;
                    mana_shield-= abs;
                    absorb+= abs;
                    dmg-= abs;

                    if (abs) {
                        double mana_loss = -abs*1.5;
                        if (talents.arcane_shielding == 1)
                            mana_loss*= 0.83;
                        else if (talents.arcane_shielding == 2)
                            mana_loss*= 0.67;
                        actions.push_back(manaAction(mana_loss, "Mana Shield"));
                    }

                    if (mana_shield <= 0)
                        actions.push_back(buffExpireAction(make_shared<buff::ManaShield>()));
                }

                if (hasBuff(buff::FIRE_WARD)) {
                    abs = dmg;
                    if (abs > fire_ward)
                        abs = fire_ward;
                    fire_ward-= abs;
                    absorb+= abs;
                    dmg-= abs;

                    if (fire_ward <= 0)
                        actions.push_back(buffExpireAction(make_shared<buff::FireWard>()));
                }

                if (hasBuff(buff::FROZEN_RUNE)) {
                    abs = dmg;
                    if (abs > frozen_rune)
                        abs = frozen_rune;
                    frozen_rune-= abs;
                    absorb+= abs;
                    dmg-= abs;

                    if (frozen_rune <= 0)
                        actions.push_back(buffExpireAction(make_shared<buff::ManaShield>()));
                }

                if (absorb && talents.incanters_absorption) {
                    absorb*= talents.incanters_absorption * 0.05;
                    absorb = round(absorb);
                    actions.push_back(buffAction(make_shared<buff::IncantersAbsorption>(absorb)));
                }

                return actions;
            }

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

                if (instance->spell->id == spell::FIREBALL && !glyphs.fireball)
                    actions.push_back(spellAction(make_shared<spell::FireballDot>()));
                if (instance->spell->id == spell::PYROBLAST)
                    actions.push_back(spellAction(make_shared<spell::PyroblastDot>()));
                if (instance->spell->id == spell::FROSTFIRE_BOLT)
                    actions.push_back(spellAction(make_shared<spell::FrostfireBoltDot>()));

                if (instance->spell->dot) {
                    if (hasTrinket(TRINKET_EXTRACT_NECROMANTIC_POWER) && !hasCooldown(cooldown::EXTRACT_NECROMANTIC_POWER) && random<int>(0, 9) == 0) {
                        action = spellAction(make_shared<spell::ExtractNecromanticPower>());
                        action->cooldown = make_shared<cooldown::ExtractNecromanticPower>();
                        actions.push_back(action);
                    }

                    if (hasTrinket(TRINKET_NAMELESS_LICH_HC) && !hasCooldown(cooldown::NAMELESS_LICH_HC) && random<int>(0, 9) < 3) {
                        action = buffAction(make_shared<buff::NamelessLichHc>());
                        action->cooldown = make_shared<cooldown::NamelessLichHc>();
                        actions.push_back(action);
                    }
                    if (hasTrinket(TRINKET_NAMELESS_LICH_NM) && !hasCooldown(cooldown::NAMELESS_LICH_NM) && random<int>(0, 9) < 3) {
                        action = buffAction(make_shared<buff::NamelessLichNm>());
                        action->cooldown = make_shared<cooldown::NamelessLichNm>();
                        actions.push_back(action);
                    }
                }
                else {
                    if (talents.clearcast) {
                        double chance = talents.clearcast * 2.0;
                        // Less chance per tick for channelled spells
                        if (instance->spell->ticks)
                            chance/= instance->spell->ticks;
                        if (random<double>(0, 100) < chance)
                            actions.push_back(buffAction(make_shared<buff::Clearcast>()));
                    }

                    // 50% chance, no icd ? unconfirmed
                    if (config->judgement_of_wisdom && random<int>(0,1) == 1)
                        actions.push_back(manaAction(base_mana * 0.02, "Judgement of Wisdom"));

                    if (hasBuff(buff::COMBUSTION) && (instance->spell->school == SCHOOL_FIRE || instance->spell->school == SCHOOL_FROSTFIRE)) {
                        if (instance->result == spell::CRIT)
                            combustion++;
                        if (combustion == 3) {
                            actions.push_back(buffExpireAction(make_shared<buff::Combustion>()));
                            actions.push_back(cooldownAction(make_shared<cooldown::Combustion>()));
                        }
                        else {
                            actions.push_back(buffAction(make_shared<buff::Combustion>()));
                        }
                    }

                    // Confirmed - on spell impact
                    // Unconfirmed - does it proc on resist?
                    if (config->lightweave_embroidery && !hasCooldown(cooldown::LIGHTWEAVE) && random<int>(0, 99) < 35) {
                        action = buffAction(make_shared<buff::Lightweave>());
                        action->cooldown = make_shared<cooldown::Lightweave>();
                        actions.push_back(action);
                    }
                }

                if (hasTrinket(TRINKET_DARKMOON_DEATH) && !hasCooldown(cooldown::DARKMOON_DEATH) && random<int>(0, 19) < 3) {
                    action = spellAction(make_shared<spell::DarkmoonDeath>());
                    action->cooldown = make_shared<cooldown::DarkmoonDeath>();
                    actions.push_back(action);
                }

                // Unconfirmed - on spell dmg ?
                if (hasTrinket(TRINKET_MURADINS_SPYGLASS_HC)) {
                    action = buffAction(make_shared<buff::MuradinsSpyglassHc>());
                    actions.push_back(action);
                }
                if (hasTrinket(TRINKET_MURADINS_SPYGLASS_NM)) {
                    action = buffAction(make_shared<buff::MuradinsSpyglassNm>());
                    actions.push_back(action);
                }
            }

            if (instance->result == spell::CRIT) {
                // Ignite
                if (talents.ignite && (instance->spell->school == SCHOOL_FIRE || instance->spell->school == SCHOOL_FROSTFIRE)) {
                    shared_ptr<action::Action> ignite = make_shared<action::Action>(action::TYPE_SPELL);
                    // 40% over 2 ticks = 20%
                    ignite->spell = make_shared<spell::Ignite>(round(instance->dmg * 0.2));
                    actions.push_back(ignite);
                }

                if (!instance->spell->dot) {
                    if (talents.master_of_elements) {
                        double mana = baseManaCost(instance->spell) * 0.1 * talents.master_of_elements;
                        if ((instance->spell->channeling || instance->spell->dot) && instance->spell->ticks)
                            mana = mana / instance->spell->ticks;
                        actions.push_back(manaAction(mana, "Master of Elements"));
                    }

                    if (talents.burnout && instance->spell->actual_cost)
                        actions.push_back(manaAction(instance->spell->actual_cost * -0.01 * talents.burnout, "Burnout"));

                    if (hasTrinket(TRINKET_SOUL_DEAD) && !hasCooldown(cooldown::SOUL_DEAD) && random<int>(0, 3) == 0) {
                        action = manaAction(900, "Soul of the Dead");
                        action->cooldown = make_shared<cooldown::SoulDead>();
                        actions.push_back(action);
                    }

                    if (hasTrinket(TRINKET_ASHTONGUE_TALISMAN) && random<int>(0, 1))
                        actions.push_back(buffAction(make_shared<buff::AshtongueTalisman>()));

                    if (hasTrinket(TRINKET_REIGN_UNLIVING_HC) && !hasCooldown(cooldown::REIGN_UNLIVING_HC)) {
                        action = buffAction(make_shared<buff::ReignUnlivingHc>());
                        action->cooldown = make_shared<cooldown::ReignUnlivingHc>();
                        actions.push_back(action);
                    }
                    if (hasTrinket(TRINKET_REIGN_UNLIVING_NM) && !hasCooldown(cooldown::REIGN_UNLIVING_NM)) {
                        action = buffAction(make_shared<buff::ReignUnlivingNm>());
                        action->cooldown = make_shared<cooldown::ReignUnlivingNm>();
                        actions.push_back(action);
                    }

                    if (config->t5_4set)
                        actions.push_back(buffAction(make_shared<buff::ArcaneMadness>()));
                }
            }

            if (talents.hot_streak && !instance->spell->dot) {
                if (instance->spell->id == spell::FIREBALL ||
                    instance->spell->id == spell::SCORCH ||
                    instance->spell->id == spell::LIVING_BOMB_EXPLOSION ||
                    instance->spell->id == spell::FROSTFIRE_BOLT ||
                    instance->spell->id == spell::FIRE_BLAST)
                {
                    double heating = false;
                    if (instance->result == spell::CRIT) {
                        if (heating_up)
                            actions.push_back(buffAction(make_shared<buff::HotStreak>()));
                        else
                            heating = true;
                    }
                    heating_up = heating;
                }
            }

            if (talents.fingers_of_frost) {
                if (hasChillEffect(instance->spell)) {
                    int chance = 7;
                    if (talents.fingers_of_frost == 2)
                        chance = 15;
                    if (random<int>(0,99) < chance) {
                        fingers_of_frost = 2;
                        actions.push_back(buffAction(make_shared<buff::FingersOfFrost>()));
                    }
                }
            }

            if (talents.brain_freeze) {
                if (hasChillEffect(instance->spell)) {
                    int chance = 5 * talents.brain_freeze;
                    if (random<int>(0,99) < chance)
                        actions.push_back(buffAction(make_shared<buff::BrainFreeze>()));
                }
            }

            return actions;
        }

        list<shared_ptr<action::Action>> onSpellTickProc(shared_ptr<State> state, shared_ptr<spell::Spell> spell, int tick)
        {
            list<shared_ptr<action::Action>> actions = Unit::onSpellTickProc(state, spell, tick);

            if (spell->id == spell::EVOCATION)
                actions.push_back(manaAction(maxMana() * 0.15, "Evocation"));

            if (hasBuff(buff::ARCANE_POTENCY)) {
                // Special case for blizzard
                if (spell->min_dmg && (spell->id != spell::BLIZZARD || tick == spell->ticks))
                    actions.push_back(buffExpireAction(make_shared<buff::ArcanePotency>()));
            }

            return actions;
        }

        bool hasManaGem()
        {
            return mana_sapphire > 0;
        }

        double manaGemMax()
        {
            double max = 0;
            if (mana_sapphire > 0)
                max = 3500;
            else
                return 0;

            double imp = 1;
            if (config->t7_2set)
                imp+= 0.25;
            if (hasTrinket(TRINKET_SERPENT_COIL))
                imp+= 0.25;
            max*= imp;

            if (glyphs.mana_gem)
                max*= 1.4;

            return max;
        }

        bool shouldUseManaGem(shared_ptr<State> state)
        {
            if (hasCooldown(cooldown::MANA_GEM) || !hasManaGem())
                return false;

            // Check for planned mana gem timings
            shared_ptr<Timing> timing = getNextTiming("mana_gem");
            if (timing) {
                if (isTimingReady(timing, state)) {
                    useTiming(timing);
                    return true;
                }
                if (timing->t < state->t + 120)
                    return false;
            }

            double max = manaGemMax();

            // If tide is running, add a tick as buffer
            if (hasBuff(buff::MANA_TIDE))
                max+= maxMana() * 0.06;

            return maxMana() - mana >= max;
        }

        bool shouldUseManaPotion(shared_ptr<State> state)
        {
            if (config->potion != POTION_MANA || hasCooldown(cooldown::POTION))
                return false;

            // Check for planned potions timings
            shared_ptr<Timing> timing = getNextTiming("potion");
            if (timing) {
                if (isTimingReady(timing, state)) {
                    useTiming(timing);
                    return true;
                }
                return false;
            }

            if (hasBuff(buff::INNERVATE))
                return false;

            double max = 4400;

            if (hasTrinket(TRINKET_MERCURIAL_ALCHEMIST_STONE))
                max*= 1.4;
            if (config->prof_engineer)
                max*= 1.25;

            // If tide is running, add a tick as buffer
            if (hasBuff(buff::MANA_TIDE))
                max+= maxMana() * 0.06;

            // If gem is configured to be used within 10 sec, count with the mana gain to avoid overcapping
            if (!hasCooldown(cooldown::MANA_GEM) && hasManaGem()) {
                bool gem_soon = false;
                shared_ptr<Timing> timing = getNextTiming("mana_gem");
                if (timing != NULL && timing->t - state->t < 10)
                    max+= manaGemMax();
            }

            return maxMana() - mana >= max;
        }

        bool shouldEvocate(shared_ptr<State> state)
        {
            if (hasCooldown(cooldown::EVOCATION))
                return false;

            // Check for planned evocation timings
            shared_ptr<Timing> timing = getNextTiming("evocation");
            if (timing) {
                if (isTimingReady(timing, state)) {
                    useTiming(timing);
                    return true;
                }
                if (timing->t < state->t + 480)
                    return false;
            }

            if (hasBuff(buff::INNERVATE) || hasBuff(buff::MANA_TIDE))
                return false;

            if (manaPercent() > 20.0)
                return false;

            if (hasBuff(buff::BLOODLUST) && manaPercent() > 10.0)
                return false;

            if (state->duration - state->t < 12)
                return false;

            return true;
        }

        list<shared_ptr<action::Action>> useManaGem()
        {
            list<shared_ptr<action::Action>> actions;

            double mana = 0;

            if (mana_sapphire > 0) {
                mana_sapphire--;
                mana = random<int>(3330, 3500);
            }

            double imp = 1;
            if (config->t7_2set)
                imp+= 0.25;
            if (hasTrinket(TRINKET_SERPENT_COIL))
                imp+= 0.25;
            mana*= imp;

            if (glyphs.mana_gem)
                mana*= 1.4;

            actions.push_back(manaAction(mana, "Mana Gem"));

            if (config->t7_2set)
                actions.push_back(buffAction(make_shared<buff::ManaSurges>()));
            if (hasTrinket(TRINKET_SERPENT_COIL))
                actions.push_back(buffAction(make_shared<buff::ManaSurge>()));

            actions.push_back(cooldownAction(make_shared<cooldown::ManaGem>()));

            return actions;
        }

        list<shared_ptr<action::Action>> useConjured(Conjured conjured)
        {
            list<shared_ptr<action::Action>> actions = Unit::useConjured(conjured);

            double cd = 120;

            if (conjured == CONJURED_FLAME_CAP) {
                cd = 180;
                actions.push_back(buffAction(make_shared<buff::FlameCap>()));
            }
            else {
                return actions;
            }

            actions.push_back(cooldownAction(make_shared<cooldown::Conjured>(cd)));

            return actions;
        }

        list<shared_ptr<action::Action>> usePotion(Potion potion, bool in_combat)
        {
            list<shared_ptr<action::Action>> actions = Unit::usePotion(potion, in_combat);

            if (potion == POTION_MANA) {
                double mana = round(random<double>(4200, 4400));

                if (hasTrinket(TRINKET_MERCURIAL_ALCHEMIST_STONE))
                    mana*= 1.4;
                if (config->prof_engineer)
                    mana*= 1.25;

                actions.push_back(manaAction(mana, "Mana Potion"));
            }
            else if (potion == POTION_SPEED) {
                actions.push_back(buffAction(make_shared<buff::Speed>()));
            }
            else if (potion == POTION_WILD_MAGIC) {
                actions.push_back(buffAction(make_shared<buff::WildMagic>()));
            }
            else {
                return actions;
            }

            actions.push_back(cooldownAction(make_shared<cooldown::Potion>(in_combat)));

            return actions;
        }

        bool isTrinketOnSharedCD(Trinket trinket)
        {
            if (!trinketSharesCD(trinket))
                return false;
            return hasCooldown(cooldown::TRINKET_SHARED);
        }

        bool trinketSharesCD(Trinket trinket)
        {
            return true;
        }

        bool isUseTrinket(Trinket trinket)
        {
            if (trinket == TRINKET_SLIVER_PURE_ICE_HC)
                return true;
            if (trinket == TRINKET_SLIVER_PURE_ICE_NM)
                return true;
            if (trinket == TRINKET_VOLATILE_POWER_HC)
                return true;
            if (trinket == TRINKET_VOLATILE_POWER_NM)
                return true;
            if (trinket == TRINKET_SCALE_FATES)
                return true;
            if (trinket == TRINKET_LIVING_FLAME)
                return true;
            if (trinket == TRINKET_MARK_WAR_PRISONER)
                return true;
            if (trinket == TRINKET_CANNONEERS_FUSELIGHTER)
                return true;
            if (trinket == TRINKET_TOME_ARCANE_PHENOMENA)
                return true;
            if (trinket == TRINKET_TWILIGHT_SERPENT)
                return true;
            if (trinket == TRINKET_RUNE_INFINITE_POWER)
                return true;

            // TBC
            if (trinket == TRINKET_NAARU_SLIVER)
                return true;
            if (trinket == TRINKET_SKULL_GULDAN)
                return true;
            if (trinket == TRINKET_SHRUNKEN_HEAD)
                return true;
            if (trinket == TRINKET_MQG)
                return true;

            return false;
        }

        shared_ptr<Timing> getNextTiming(string name)
        {
            for (int i=0; i<config->timings.size(); i++) {
                if (!config->timings[i]->used && config->timings[i]->name == name)
                    return config->timings[i];
            }

            return NULL;
        }

        bool isTimingReady(shared_ptr<Timing> timing, shared_ptr<State> state)
        {
            if (timing->used || timing->t > state->t)
                return false;

            if (timing->wait_for_buff != 0 && timing->wait_t + timing->t > state->t) {
                auto buff_id = static_cast<buff::ID>(timing->wait_for_buff);
                if (!hasBuff(buff_id))
                    return false;
            }

            return true;
        }

        void useTiming(shared_ptr<Timing> timing = NULL)
        {
            if (timing != NULL)
                timing->used = true;
        }

        bool useTimingIfPossible(string name, shared_ptr<State> state, bool expl = false)
        {
            shared_ptr<Timing> timing = getNextTiming(name);

            if (timing == NULL && !expl || timing != NULL && isTimingReady(timing, state)) {
                useTiming(timing);
                return true;
            }

            return false;
        }

        list<shared_ptr<action::Action>> useTrinket(Trinket trinket, shared_ptr<cooldown::Cooldown> cooldown)
        {
            list<shared_ptr<action::Action>> actions = Unit::useTrinket(trinket, cooldown);
            shared_ptr<buff::Buff> buff = NULL;

            cooldown->duration = 120;

            if (trinket == TRINKET_TWILIGHT_SERPENT) {
                buff = make_shared<buff::TwilightSerpent>();
            }
            else if (trinket == TRINKET_RUNE_INFINITE_POWER) {
                buff = make_shared<buff::InfinitePower>();
            }
            else if (trinket == TRINKET_SLIVER_PURE_ICE_HC) {
                actions.push_back(manaAction(1830, "Sliver of Pure Ice"));
                actions.push_back(cooldownAction(cooldown));
            }
            else if (trinket == TRINKET_SLIVER_PURE_ICE_NM) {
                actions.push_back(manaAction(1625, "Sliver of Pure Ice"));
                actions.push_back(cooldownAction(cooldown));
            }
            else if (trinket == TRINKET_VOLATILE_POWER_HC) {
                buff = make_shared<buff::VolatilePowerHc>();
            }
            else if (trinket == TRINKET_VOLATILE_POWER_NM) {
                buff = make_shared<buff::VolatilePowerNm>();
            }
            else if (trinket == TRINKET_SCALE_FATES) {
                buff = make_shared<buff::ScaleFates>();
            }
            else if (trinket == TRINKET_LIVING_FLAME) {
                buff = make_shared<buff::LivingFlame>();
            }
            else if (trinket == TRINKET_TOME_ARCANE_PHENOMENA) {
                buff = make_shared<buff::TomeArcanePhenomena>();
            }
            else if (trinket == TRINKET_CANNONEERS_FUSELIGHTER) {
                buff = make_shared<buff::ArgentValor>();
            }
            else if (trinket == TRINKET_MARK_WAR_PRISONER) {
                buff = make_shared<buff::MarkWarPrisoner>();
            }
            else if (trinket == TRINKET_NAARU_SLIVER) {
                buff = make_shared<buff::NaaruSliver>();
                cooldown->duration = 90;
            }
            else if (trinket == TRINKET_SKULL_GULDAN) {
               buff = make_shared<buff::SkullGuldan>();
            }
            else if (trinket == TRINKET_SHRUNKEN_HEAD) {
               buff = make_shared<buff::ShrunkenHead>();
            }
            else if (trinket == TRINKET_MQG) {
                buff = make_shared<buff::MindQuickening>();
                cooldown->duration = 300;
            }

            if (buff != NULL)
                actions.push_back(buffAction(buff));

            if (actions.size() > 0) {
                actions.push_back(cooldownAction(cooldown));
                if (trinketSharesCD(trinket))
                    actions.push_back(cooldownAction(make_shared<cooldown::TrinketShared>(buff->duration)));
            }

            return actions;
        }

        int evocationTicks()
        {
            int ticks = 4;

            if (config->t6_2set)
                ticks++;

            if (config->evo_ticks && config->evo_ticks <= ticks)
                ticks = config->evo_ticks;

            return ticks;
        }

        shared_ptr<action::Action> useCooldown(shared_ptr<State> state)
        {
            shared_ptr<action::Action> action = NULL;

            if (talents.arcane_power && !hasCooldown(cooldown::ARCANE_POWER) && !hasBuff(buff::PRESENCE_OF_MIND) && useTimingIfPossible("arcane_power", state)) {
                action = buffAction(make_shared<buff::ArcanePower>(glyphs.arcane_power));
                action->cooldown = make_shared<cooldown::ArcanePower>();
            }
            else if (talents.combustion && !hasCooldown(cooldown::COMBUSTION) && !hasBuff(buff::COMBUSTION) && useTimingIfPossible("combustion", state)) {
                combustion = 0;
                action = buffAction(make_shared<buff::Combustion>());
            }
            else if (talents.presence_of_mind && !hasCooldown(cooldown::PRESENCE_OF_MIND) && !hasBuff(buff::ARCANE_POWER) && useTimingIfPossible("presence_of_mind", state)) {
                action = buffAction(make_shared<buff::PresenceOfMind>());
                action->cooldown = make_shared<cooldown::PresenceOfMind>();
            }
            else if (talents.icy_veins && !hasCooldown(cooldown::ICY_VEINS) && useTimingIfPossible("icy_veins", state)) {
                action = buffAction(make_shared<buff::IcyVeins>());
                action->cooldown = make_shared<cooldown::IcyVeins>();
            }
            else if (talents.cold_snap && !hasCooldown(cooldown::COLD_SNAP) && useTimingIfPossible("cold_snap", state, true)) {
                action = spellAction(make_shared<spell::ColdSnap>());
                action->cooldown = make_shared<cooldown::ColdSnap>();
            }
            else if (race == RACE_TROLL && !hasCooldown(cooldown::BERSERKING) && useTimingIfPossible("berserking", state)) {
                action = buffAction(make_shared<buff::Berserking>());
                action->cooldown = make_shared<cooldown::Berserking>();
            }
            else if (config->hyperspeed_accelerators && !hasCooldown(cooldown::HYPERSPEED_ACCELERATION) && useTimingIfPossible("hyperspeed_accelerators", state)) {
                action = buffAction(make_shared<buff::HyperspeedAcceleration>());
                action->cooldown = make_shared<cooldown::HyperspeedAcceleration>();
            }
            else if (config->potion != POTION_NONE && config->potion != POTION_MANA && !hasCooldown(cooldown::POTION) && useTimingIfPossible("potion", state)) {
                action = make_shared<action::Action>(action::TYPE_POTION);
                action->potion = config->potion;
            }
            else if (config->conjured != CONJURED_NONE && !hasCooldown(cooldown::CONJURED) && useTimingIfPossible("conjured", state)) {
                action = make_shared<action::Action>(action::TYPE_CONJURED);
                action->conjured = config->conjured;
            }
            else if (isUseTrinket(config->trinket1) && !hasCooldown(cooldown::TRINKET1) && !isTrinketOnSharedCD(config->trinket1) && useTimingIfPossible("trinket1", state)) {
                action = make_shared<action::Action>(action::TYPE_TRINKET);
                action->cooldown = make_shared<cooldown::Cooldown>(cooldown::TRINKET1);
                action->trinket = config->trinket1;
            }
            else if (isUseTrinket(config->trinket2) && !hasCooldown(cooldown::TRINKET2) && !isTrinketOnSharedCD(config->trinket2) && useTimingIfPossible("trinket2", state)) {
                action = make_shared<action::Action>(action::TYPE_TRINKET);
                action->cooldown = make_shared<cooldown::Cooldown>(cooldown::TRINKET2);
                action->trinket = config->trinket2;
            }
            else if (!hasCooldown(cooldown::EVOCATION) && useTimingIfPossible("evocation", state, true)) {
                action = spellAction(make_shared<spell::Evocation>(evocationTicks()));
            }
            else if (talents.water_elemental && !hasCooldown(cooldown::WATER_ELEMENTAL) && !state->hasUnit(unit::WATER_ELEMENTAL) && useTimingIfPossible("water_elemental", state)) {
                action = spellAction(make_shared<spell::WaterElemental>());
                action->cooldown = make_shared<cooldown::WaterElemental>();
            }
            else if (!hasCooldown(cooldown::MIRROR_IMAGE) && useTimingIfPossible("mirror_image", state, true)) {
                action = spellAction(make_shared<spell::MirrorImage>());
                action->cooldown = make_shared<cooldown::MirrorImage>();
            }

            if (action != NULL && action->type != action::TYPE_SPELL)
                action->primary_action = true;

            return action;
        }

        bool canBlast(shared_ptr<State> state)
        {
            // Lets assume we cant blast for 30+ seconds to save computing time
            if (state->timeRemain() > 30)
                return false;

            shared_ptr<spell::Spell> ab = make_shared<spell::ArcaneBlast>();
            double cast_time = castTime(ab);
            double base_cost = baseManaCost(ab);
            double cur_mana = mana;
            double t = state->t;
            int stacks = buffStacks(buff::ARCANE_BLAST);

            for (t = state->t; t<=state->duration; t+= cast_time) {
                if (stacks < 4)
                    stacks++;
                cur_mana-= base_cost * 1.75 * stacks;
                if (cur_mana < 0)
                    return false;
            }

            return true;
        }

        shared_ptr<action::Action> preCombat(shared_ptr<State> state)
        {
            shared_ptr<action::Action> action = NULL;

            if (talents.incanters_absorption && config->pre_incanters_absorption && config->pre_rune_incanters_absorption && !hasBuff(buff::FROZEN_RUNE)) {
                action = spellAction(make_shared<spell::FrozenRune>());
            }
            else if (talents.incanters_absorption && config->pre_incanters_absorption && config->pre_mana_incanters_absorption && !hasBuff(buff::MANA_SHIELD)) {
                action = spellAction(make_shared<spell::ManaShield>());
            }
            else if (talents.incanters_absorption && config->pre_incanters_absorption && !hasBuff(buff::FIRE_WARD)) {
                action = spellAction(make_shared<spell::FireWard>());
            }
            else if (config->pre_potion && !hasCooldown(cooldown::POTION) && state->t >= -2.0) {
                action = make_shared<action::Action>(action::TYPE_POTION);
                action->potion = config->pre_potion;
            }
            else if (talents.water_elemental && config->pre_water_elemental && !state->hasUnit(unit::WATER_ELEMENTAL)) {
                action = spellAction(make_shared<spell::WaterElemental>());
                action->cooldown = make_shared<cooldown::WaterElemental>();
            }
            else if (config->pre_mirror_image && !hasCooldown(cooldown::MIRROR_IMAGE)) {
                action = spellAction(make_shared<spell::MirrorImage>());
                action->cooldown = make_shared<cooldown::MirrorImage>();
            }

            if (action != NULL)
                action->primary_action = false;

            return action;
        }

        shared_ptr<action::Action> offGcd(shared_ptr<State> state)
        {
            shared_ptr<action::Action> action = NULL;

            if (talents.incanters_absorption && hasBuff(buff::FIRE_WARD) && !hasCooldown(cooldown::SAPPER_CHARGE) && (useTimingIfPossible("sapper_charge", state, true) || hasBuff(buff::ARCANE_POWER) && getNextTiming("sapper_charge") == NULL)) {
                action = spellAction(make_shared<spell::SapperCharge>());
                action->cooldown = make_shared<cooldown::SapperCharge>();
            }

            return action;
        }

        shared_ptr<action::Action> nextAction(shared_ptr<State> state)
        {
            shared_ptr<action::Action> action = NULL;

            if (!state->inCombat())
                return preCombat(state);

            action = offGcd(state);
            if (action)
                return action;

            action = gcdAction(state->t);
            if (action)
                return action;

            action = useCooldown(state);
            if (action)
                return action;

            // Mana consumes
            if (shouldUseManaGem(state)) {
                return spellAction(make_shared<spell::ManaGem>());
            }
            else if (shouldUseManaPotion(state)) {
                action = make_shared<action::Action>(action::TYPE_POTION);
                action->potion = config->potion;
                action->primary_action = true;
                return action;
            }
            else if (shouldEvocate(state)) {
                return spellAction(make_shared<spell::Evocation>(evocationTicks()));
            }

            if (config->maintain_imp_scorch && talents.imp_scorch && t_scorch+27.0 <= state->t) {
                action = spellAction(make_shared<spell::Scorch>());
                return action;
            }

            // FFB / Scorch
            if (config->rotation == ROTATION_ST_FROSTFIRE || config->rotation == ROTATION_ST_FIRE_SC) {
                bool no_bomb = talents.living_bomb && t_living_bomb+12.0 <= state->t && state->t + 12.0 < state->duration;
                if (no_bomb && !heating_up) {
                    action = spellAction(make_shared<spell::LivingBomb>());
                }
                else if (canReactTo(buff::HOT_STREAK, state->t)) {
                    action = spellAction(make_shared<spell::Pyroblast>());
                }
                else if (no_bomb) {
                    action = spellAction(make_shared<spell::LivingBomb>());
                }
                else {
                    if (config->rotation == ROTATION_ST_FIRE_SC)
                        action = spellAction(make_shared<spell::Scorch>());
                    else
                        action = spellAction(make_shared<spell::FrostfireBolt>());
                }
            }

            // Fire rotation
            else if (config->rotation == ROTATION_ST_FIRE) {
                bool no_bomb = talents.living_bomb && t_living_bomb+12.0 <= state->t && state->t + 12.0 < state->duration;
                if (no_bomb && !heating_up) {
                    action = spellAction(make_shared<spell::LivingBomb>());
                    should_wait = false;
                }
                else if (canReactTo(buff::HOT_STREAK, state->t)) {
                    if (waited || !should_wait || !config->hot_streak_cqs) {
                        action = spellAction(make_shared<spell::Pyroblast>());
                        waited = false;
                        should_wait = false;
                    }
                    else {
                        action = make_shared<action::Action>(action::TYPE_WAIT);
                        action->value = config->hot_streak_cqs_time / 1000.0;
                        waited = true;
                    }
                }
                else if (no_bomb) {
                    action = spellAction(make_shared<spell::LivingBomb>());
                    should_wait = false;
                }
                else {
                    action = spellAction(make_shared<spell::Fireball>());
                    should_wait = true;
                }
            }

            // Arcane rotations
            else if (config->rotation == ROTATION_ST_AB_AM || config->rotation == ROTATION_ST_AB_AM_BARRAGE) {
                int ab_stacks = 4;
                if (config->rot_ab3_mana > 0 && manaPercent() < config->rot_ab3_mana)
                    ab_stacks = 3;
                bool has_mb = canReactTo(buff::MISSILE_BARRAGE, state->t);

                // AB until the end
                if (canBlast(state))
                    action = spellAction(make_shared<spell::ArcaneBlast>());
                // Extra ABs during AP
                else if (hasBuff(buff::ARCANE_POWER) && config->rot_abs_ap+4 > ab_streak && state->t < 60)
                    action = spellAction(make_shared<spell::ArcaneBlast>());
                // AM if we have MB and below n AB stacks
                else if (config->rot_mb_below_ab && has_mb && buffStacks(buff::ARCANE_BLAST) < config->rot_mb_below_ab)
                    action = spellAction(make_shared<spell::ArcaneMissiles>());
                // AM if we have MB and below mana %
                else if (config->rot_mb_mana && has_mb && manaPercent() < config->rot_mb_mana)
                    action = spellAction(make_shared<spell::ArcaneMissiles>());
                // AB if we don't have barrage and over mana %
                else if (!has_mb && config->rot_ab_no_mb_mana < manaPercent())
                    action = spellAction(make_shared<spell::ArcaneBlast>());
                // AM / Abarr if we have AB stacks
                else if (buffStacks(buff::ARCANE_BLAST) >= ab_stacks) {
                    // Arcane Barrage if we don't have Missile Barrage proc
                    if (!has_mb && config->rotation == ROTATION_ST_AB_AM_BARRAGE)
                        action = spellAction(make_shared<spell::ArcaneBarrage>());
                    else
                        action = spellAction(make_shared<spell::ArcaneMissiles>());
                }
                else
                    action = spellAction(make_shared<spell::ArcaneBlast>());
            }

            // Frost
            else if (config->rotation == ROTATION_ST_FROST) {
                if (hasBuff(buff::GHOST_FINGERS)) {
                    if (talents.deep_freeze && !hasCooldown(cooldown::DEEP_FREEZE))
                        action = spellAction(make_shared<spell::DeepFreeze>());
                    else if (hasBuff(buff::BRAIN_FREEZE))
                        action = spellAction(make_shared<spell::FrostfireBolt>());
                    else if (config->rot_ice_lance)
                        action = spellAction(make_shared<spell::IceLance>());
                }
                else if (canReactTo(buff::BRAIN_FREEZE, state->t)) {
                    action = spellAction(make_shared<spell::FrostfireBolt>());
                }

                if (action == NULL) {
                    action = spellAction(make_shared<spell::Frostbolt>());
                }
            }

            // Arcane Explosion
            else if (config->rotation == ROTATION_AOE_AE) {
                action = spellAction(make_shared<spell::ArcaneExplosion>());
            }

            // Blizzard
            else if (config->rotation == ROTATION_AOE_BLIZZ) {
                action = spellAction(make_shared<spell::Blizzard>());
            }

            // Blizzard + Flamestrike
            else if (config->rotation == ROTATION_AOE_BLIZZ_FS) {
                shared_ptr<spell::Spell> fs = make_shared<spell::Flamestrike>();
                if (t_flamestrike+8.0 + castTime(fs) <= state->t)
                    action = spellAction(fs);
                else
                    action = spellAction(make_shared<spell::Blizzard>());
            }

            // Undefined rotation
            else {
                action = spellAction(make_shared<spell::Fireball>());
            }

            return action;
        }

    };

}