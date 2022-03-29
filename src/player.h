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
        bool on_hot_streak;
        double t_living_bomb;
        int mana_sapphire;
        int mana_emerald;

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
            on_hot_streak = false;
            t_living_bomb = -20;
            mana_sapphire = 3;
            mana_emerald = 3;
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

            if (spell->id == spell::FROSTFIRE_BOLT && glyphs.frostfire)
                crit+= 2;

            if (hasBuff(buff::COMBUSTION) && (spell->school == SCHOOL_FIRE || spell->school == SCHOOL_FROSTFIRE))
                crit+= buffStacks(buff::COMBUSTION)*10.0;

            if (talents.shatter && isFrozen()) {
                crit+= talents.shatter * 17;
                if (talents.shatter == 3)
                    crit-= 1;
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
                multi+= talents.spell_power*0.1;

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

            if (spell->school == SCHOOL_ARCANE && hasBuff(buff::ARCANE_BLAST)) {
                double ab = 0.15;
                if (glyphs.arcane_blast)
                    ab+= 0.03;
                multi*= 1 + ab * buffStacks(buff::ARCANE_BLAST);
            }

            if (spell->id == spell::ICE_LANCE && isFrozen()) {
                if (glyphs.ice_lance)
                    multi*= 4;
                else
                    multi*= 3;
            }

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

            if (talents.mind_mastery)
                sp+= getIntellect() * talents.mind_mastery * 0.03;

            return sp;
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

        std::list<shared_ptr<action::Action>> onCastSuccessProc(shared_ptr<State> state, shared_ptr<spell::Spell> spell)
        {
            std::list<shared_ptr<action::Action>> actions = Unit::onCastSuccessProc(state, spell);
            shared_ptr<action::Action> action = NULL;

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
            if (spell->id == spell::COLD_SNAP) {
                actions.push_back(cooldownAction(make_shared<cooldown::ColdSnap>()));
                actions.push_back(cooldownExpireAction(make_shared<cooldown::IcyVeins>()));
                actions.push_back(cooldownExpireAction(make_shared<cooldown::WaterElemental>()));
                actions.push_back(cooldownExpireAction(make_shared<cooldown::ConeOfCold>()));
                actions.push_back(cooldownExpireAction(make_shared<cooldown::DeepFreeze>()));
            }

            if (hasBuff(buff::PRESENCE_OF_MIND) && spell->cast_time && !spell->channeling)
                actions.push_back(buffExpireAction(make_shared<buff::PresenceOfMind>()));

            if (spell->id == spell::ARCANE_BLAST)
                actions.push_back(buffAction(make_shared<buff::ArcaneBlast>()));
            else if (hasBuff(buff::ARCANE_BLAST) && spell->school == SCHOOL_ARCANE)
                actions.push_back(buffExpireAction(make_shared<buff::ArcaneBlast>()));

            if (hasBuff(buff::BRAIN_FREEZE) && (spell->id == spell::FROSTFIRE_BOLT || spell->id == spell::FIREBALL))
                actions.push_back(buffExpireAction(make_shared<buff::BrainFreeze>()));

            if (spell->id == spell::ARCANE_MISSILES && hasBuff(buff::MISSILE_BARRAGE))
                actions.push_back(buffExpireAction(make_shared<buff::MissileBarrage>()));
            if (spell->id == spell::PYROBLAST && hasBuff(buff::HOT_STREAK))
                actions.push_back(buffExpireAction(make_shared<buff::HotStreak>()));

            if (talents.firestarter && (spell->id == spell::BLAST_WAVE || spell->id == spell::DRAGONS_BREATH))
                actions.push_back(buffAction(make_shared<buff::Firestarter>()));

            if (config->black_magic && !spell->is_trigger && !hasCooldown(cooldown::BLACK_MAGIC) && random<int>(0, 99) < 35) {
                action = buffAction(make_shared<buff::BlackMagic>());
                action->cooldown = make_shared<cooldown::BlackMagic>();
                actions.push_back(action);
            }
            if (config->lightweave_embroidery && !hasCooldown(cooldown::LIGHTWEAVE) && random<int>(0, 99) < 35) {
                action = buffAction(make_shared<buff::Lightweave>());
                action->cooldown = make_shared<cooldown::Lightweave>();
                actions.push_back(action);
            }
            if (config->darkglow_embroidery && !hasCooldown(cooldown::DARKGLOW) && random<int>(0, 99) < 35) {
                action = manaAction(400, "Darkglow");
                action->cooldown = make_shared<cooldown::Darkglow>();
                actions.push_back(action);
            }

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
                    actions.push_back(action);
                }
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

                if (random<int>(0, 99) < talents.clearcast * 2)
                    actions.push_back(buffAction(make_shared<buff::Clearcast>()));

                if (instance->spell->id == spell::FIREBALL && !glyphs.fireball)
                    actions.push_back(spellAction(make_shared<spell::FireballDot>()));
                if (instance->spell->id == spell::PYROBLAST)
                    actions.push_back(spellAction(make_shared<spell::PyroblastDot>()));
                if (instance->spell->id == spell::FROSTFIRE_BOLT)
                    actions.push_back(spellAction(make_shared<spell::FrostfireBoltDot>()));

                if (!instance->spell->dot) {
                    // 50% chance, no icd ? unconfirmed
                    if (config->judgement_of_wisdom && random<int>(0,1) == 1)
                        actions.push_back(manaAction(base_mana * 0.02, "Judgement of Wisdom"));

                    if (hasBuff(buff::COMBUSTION)) {
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
                }
            }

            if (instance->result == spell::CRIT) {
                if (hasTrinket(TRINKET_ASHTONGUE_TALISMAN) && random<int>(0, 1))
                    actions.push_back(buffAction(make_shared<buff::AshtongueTalisman>()));

                if (talents.master_of_elements)
                    actions.push_back(manaAction(baseManaCost(instance->spell) * 0.1 * talents.master_of_elements, "Master of Elements"));

                // Ignite
                if (talents.ignite && (instance->spell->school == SCHOOL_FIRE || instance->spell->school == SCHOOL_FROSTFIRE) && !instance->spell->dot) {
                    shared_ptr<action::Action> ignite = make_shared<action::Action>(action::TYPE_SPELL);
                    // 40% over for ticks = 10%
                    ignite->spell = make_shared<spell::Ignite>(round(instance->dmg * 0.1));
                    actions.push_back(ignite);
                }

                if (talents.burnout)
                    actions.push_back(manaAction(instance->spell->actual_cost * -0.01 * talents.burnout, "Burnout"));
            }

            if (talents.hot_streak && !instance->spell->dot) {
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

        bool hasManaGem()
        {
            return mana_sapphire > 0 || mana_emerald > 0;
        }

        bool shouldUseManaGem(shared_ptr<State> state)
        {
            if (config->conjured != CONJURED_MANA_GEM)
                return false;

            if (hasCooldown(cooldown::CONJURED))
                return false;

            // Check for planned mana gem timings
            for (int i=0; i<config->conjured_t.size(); i++) {
                if (state->t - config->conjured_t.at(i) < 10)
                    return false;
            }

            double max = 0;
            if (mana_sapphire > 0)
                max = 3500;
            else if (mana_emerald > 0)
                max = 2460;
            else
                return false;

            if (hasTrinket(TRINKET_SERPENT_COIL))
                max*= 1.25;
            if (glyphs.mana_gem)
                max*= 1.4;

            // If tide is running, add a tick as buffer
            if (hasBuff(buff::MANA_TIDE))
                max+= maxMana() * 0.06;

            return maxMana() - mana >= max;
        }

        bool shouldUseManaPotion(shared_ptr<State> state)
        {
            if (config->potion != POTION_MANA)
                return false;

            if (hasCooldown(cooldown::POTION) || hasBuff(buff::INNERVATE))
                return false;

            if (!hasCooldown(cooldown::CONJURED) && config->conjured == CONJURED_MANA_GEM && config->conjured_t.size() > 0 && hasManaGem())
                return false;

            double max = 4400;

            if (hasTrinket(TRINKET_MERCURIAL_ALCHEMIST_STONE))
                mana*= 1.4;

            // If tide is running, add a tick as buffer
            if (hasBuff(buff::MANA_TIDE))
                max+= maxMana() * 0.06;

                // If gem is configured to be used within 15 sec, count with the mana gain to avoid overcapping
            if (!hasCooldown(cooldown::CONJURED) && config->conjured == CONJURED_MANA_GEM) {
                bool gem_soon = false;
                for (int i=0; i<config->conjured_t.size(); i++) {
                    // 10 second margin for when it was supposed to be used
                    if (state->t - config->conjured_t.at(i) < 10 && config->conjured_t.at(i) - state->t < 15) {
                        gem_soon = true;
                        break;
                    }
                }

                if (gem_soon) {
                    double gem = 3500;
                    if (hasTrinket(TRINKET_SERPENT_COIL))
                        gem*= 1.25;
                    if (glyphs.mana_gem)
                        gem*= 1.4;
                    max+= gem;
                }
            }

            return maxMana() - mana >= max;
        }

        list<shared_ptr<action::Action>> useConjured(Conjured conjured)
        {
            list<shared_ptr<action::Action>> actions = Unit::useConjured(conjured);

            double cd = 120;

            if (conjured == CONJURED_MANA_GEM) {
                double mana = 0;

                if (mana_sapphire > 0) {
                    mana_sapphire--;
                    mana = random<int>(3330, 3500);
                }
                else if (mana_emerald > 0) {
                    mana_emerald--;
                    mana = random<int>(2340, 2460);
                }

                if (hasTrinket(TRINKET_SERPENT_COIL))
                    mana*= 1.25;
                if (glyphs.mana_gem)
                    mana*= 1.4;

                actions.push_back(manaAction(mana, "Mana Gem"));

                if (hasTrinket(TRINKET_SERPENT_COIL))
                    actions.push_back(buffAction(make_shared<buff::SerpentCoil>()));
            }
            else if (conjured == CONJURED_FLAME_CAP) {
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
            if (trinket == TRINKET_MERCURIAL_ALCHEMIST_STONE)
                return false;

            // TBC
            if (trinket == TRINKET_ASHTONGUE_TALISMAN)
                return false;
            if (trinket == TRINKET_SERPENT_COIL)
                return false;

            return true;
        }

        list<shared_ptr<action::Action>> useTrinket(Trinket trinket, shared_ptr<cooldown::Cooldown> cooldown)
        {
            list<shared_ptr<action::Action>> actions = Unit::useTrinket(trinket, cooldown);
            shared_ptr<buff::Buff> buff = NULL;

            cooldown->duration = 120;

            if (trinket == TRINKET_SKULL_GULDAN)
               buff = make_shared<buff::SkullGuldan>();
            if (trinket == TRINKET_SHRUNKEN_HEAD)
               buff = make_shared<buff::ShrunkenHead>();

            if (trinket == TRINKET_MQG) {
                buff = make_shared<buff::MindQuickening>();
                cooldown->duration = 300;
            }
            if (trinket == TRINKET_NAARU_SLIVER) {
                buff = make_shared<buff::NaaruSliver>();
                cooldown->duration = 90;
            }

            if (buff != NULL) {
                actions.push_back(buffAction(buff));
                actions.push_back(cooldownAction(cooldown));
                if (trinketSharesCD(trinket))
                    actions.push_back(cooldownAction(make_shared<cooldown::TrinketShared>(buff->duration)));
            }

            return actions;
        }

        shared_ptr<action::Action> useCooldown(shared_ptr<State> state)
        {
            shared_ptr<action::Action> action = NULL;

            if (isTimerReady(config->arcane_power_t, state->t) && !hasCooldown(cooldown::ARCANE_POWER) && talents.arcane_power) {
                action = buffAction(make_shared<buff::ArcanePower>(glyphs.arcane_power));
                action->cooldown = make_shared<cooldown::ArcanePower>();
            }
            else if (isTimerReady(config->combustion_t, state->t) && !hasCooldown(cooldown::COMBUSTION) && !hasBuff(buff::COMBUSTION) && talents.combustion) {
                action = buffAction(make_shared<buff::Combustion>());
            }
            else if (isTimerReady(config->presence_of_mind_t, state->t) && !hasCooldown(cooldown::PRESENCE_OF_MIND) && talents.presence_of_mind) {
                action = buffAction(make_shared<buff::PresenceOfMind>());
                action->cooldown = make_shared<cooldown::PresenceOfMind>();
            }
            else if (isTimerReady(config->icy_veins_t, state->t) && !hasCooldown(cooldown::ICY_VEINS) && talents.icy_veins) {
                action = buffAction(make_shared<buff::IcyVeins>());
                action->cooldown = make_shared<cooldown::IcyVeins>();
            }
            else if (isTimerReady(config->cold_snap_t, state->t) && !hasCooldown(cooldown::COLD_SNAP) && talents.cold_snap) {
                action = spellAction(make_shared<spell::ColdSnap>());
                action->cooldown = make_shared<cooldown::ColdSnap>();
            }
            else if (isTimerReady(config->berserking_t, state->t) && !hasCooldown(cooldown::BERSERKING) && race == RACE_TROLL) {
                action = buffAction(make_shared<buff::Berserking>());
                action->cooldown = make_shared<cooldown::Berserking>();
            }
            else if (isTimerReady(config->hyperspeed_t, state->t) && !hasCooldown(cooldown::HYPERSPEED_ACCELERATION) && config->hyperspeed_accelerators) {
                action = buffAction(make_shared<buff::HyperspeedAcceleration>());
                action->cooldown = make_shared<cooldown::HyperspeedAcceleration>();
            }
            else if (!hasCooldown(cooldown::POTION) && config->potion != POTION_NONE && config->potion != POTION_MANA && isTimerReady(config->potion_t, state->t)) {
                action = make_shared<action::Action>(action::TYPE_POTION);
                action->potion = config->potion;
            }
            else if (
                !hasCooldown(cooldown::CONJURED) && config->conjured != CONJURED_NONE && (
                    config->conjured == CONJURED_MANA_GEM && isTimerReadyExplicit(config->conjured_t, state->t) ||
                    config->conjured != CONJURED_MANA_GEM && isTimerReady(config->conjured_t, state->t)))
            {
                action = make_shared<action::Action>(action::TYPE_CONJURED);
                action->conjured = config->conjured;
            }
            else if (isUseTrinket(config->trinket1) && !hasCooldown(cooldown::TRINKET1) && !isTrinketOnSharedCD(config->trinket1) && isTimerReady(config->trinket1_t, state->t)) {
                action = make_shared<action::Action>(action::TYPE_TRINKET);
                action->cooldown = make_shared<cooldown::Cooldown>(cooldown::TRINKET1);
                action->trinket = config->trinket1;
            }
            else if (isUseTrinket(config->trinket2) && !hasCooldown(cooldown::TRINKET2) && !isTrinketOnSharedCD(config->trinket2) && isTimerReady(config->trinket2_t, state->t)) {
                action = make_shared<action::Action>(action::TYPE_TRINKET);
                action->cooldown = make_shared<cooldown::Cooldown>(cooldown::TRINKET2);
                action->trinket = config->trinket2;
            }

            if (action != NULL)
                action->primary_action = true;

            return action;
        }

        shared_ptr<action::Action> preCombat(shared_ptr<State> state)
        {
            shared_ptr<action::Action> action = NULL;

            return action;
        }

        shared_ptr<action::Action> nextAction(shared_ptr<State> state)
        {
            shared_ptr<action::Action> action = NULL;

            if (!state->inCombat())
                return preCombat(state);

            action = useCooldown(state);
            if (action)
                return action;

            // Mana consumes
            if (shouldUseManaGem(state)) {
                action = make_shared<action::Action>(action::TYPE_CONJURED);
                action->conjured = config->conjured;
                action->primary_action = true;
                return action;
            }
            else if (shouldUseManaPotion(state)) {
                action = make_shared<action::Action>(action::TYPE_POTION);
                action->potion = config->potion;
                action->primary_action = true;
                return action;
            }

            // TODO: TESTING ONLY
            if (!hasCooldown(cooldown::MIRROR_IMAGE)) {
                action = spellAction(make_shared<spell::MirrorImage>());
                action->cooldown = make_shared<cooldown::MirrorImage>();
                return action;
            }
            // TODO: TESTING ONLY
            if (!hasCooldown(cooldown::WATER_ELEMENTAL) && !state->hasUnit(unit::WATER_ELEMENTAL)) {
                action = spellAction(make_shared<spell::WaterElemental>());
                action->cooldown = make_shared<cooldown::WaterElemental>();
                return action;
            }

            // Frostfire bolt
            if (config->rotation == ROTATION_ST_FROSTFIRE) {
                action = spellAction(make_shared<spell::FrostfireBolt>());
            }

            // AB -> AM rotation
            else if (config->rotation == ROTATION_ST_AB_AM) {
                int ab_stacks = config->rot_ab_stacks_three ? 3 : 4;
                if (buffStacks(buff::ARCANE_BLAST) >= ab_stacks)
                    action = spellAction(make_shared<spell::ArcaneMissiles>());
                else
                    action = spellAction(make_shared<spell::ArcaneBlast>());
            }

            // AB -> AM + Barrage rotation
            else if (config->rotation == ROTATION_ST_AB_AM_BARRAGE) {
                int ab_stacks = config->rot_ab_stacks_three ? 3 : 4;
                if (buffStacks(buff::ARCANE_BLAST) >= ab_stacks) {
                    if (hasBuff(buff::MISSILE_BARRAGE))
                        action = spellAction(make_shared<spell::ArcaneMissiles>());
                    else
                        action = spellAction(make_shared<spell::ArcaneBarrage>());
                }
                else {
                    action = spellAction(make_shared<spell::ArcaneBlast>());
                }
            }

            // Fire rotation
            else if (config->rotation == ROTATION_ST_FIRE) {
                if (hasBuff(buff::HOT_STREAK)) {
                    action = spellAction(make_shared<spell::Pyroblast>());
                }
                else if (t_living_bomb+12.0 <= state->t) {
                    action = spellAction(make_shared<spell::LivingBomb>());
                    t_living_bomb = state->t + 12;
                }
                else {
                    action = spellAction(make_shared<spell::Fireball>());
                }
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
                else if (hasBuff(buff::BRAIN_FREEZE)) {
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

            // Undefined rotation
            else {
                action = spellAction(make_shared<spell::Fireball>());
            }

            return action;
        }

    };

}