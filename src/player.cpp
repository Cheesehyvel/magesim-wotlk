#include "player.h"
#include "config.h"
#include "unit.h"
#include "talents.h"
#include "state.h"
#include "mirror_image.h"
#include "water_elemental.h"

#include <algorithm>
#include <string>
#include <exception>

namespace unit
{

Player::Player(const Config& _config, const Stats& _stats, const Talents& _talents, const Glyphs& _glyphs)
    : Unit(_config, _stats), talents(_talents), glyphs(_glyphs)
{
    base_mana = 3268;
    name = "Player";
    id = 1;
    used_timings.resize(_config.timings.size());
}

void Player::reset()
{
    Unit::reset();
    combustion = 0;
    fingers_of_frost = 0;
    heating_up = false;
    t_living_bomb = -20;
    t_flamestrike = -20;
    t_scorch = -60;
    t_brain_freeze = 0;
    t_incanters_absorption = 0;
    t_mana_spent = 0;
    fire_ward = 0;
    mana_shield = 0;
    mana_sapphire = 3;
    ab_streak = 0;
    used_dark_rune = false;
    waited = false;
    should_wait = false;
    black_magic = false;

    if (config.rot_black_magic)
        swapWeapons();

    std::fill(used_timings.begin(), used_timings.end(), false);
}

Faction Player::faction() const
{
    if (race == RACE_GNOME || race == RACE_HUMAN || race == RACE_DRAENEI)
        return FACTION_ALLIANCE;
    return FACTION_HORDE;
}

bool Player::hasTrinket(Trinket trinket) const
{
    return config.trinket1 == trinket || config.trinket2 == trinket;
}

void Player::applyMana(const State& state, double _mana)
{
    Unit::applyMana(state, _mana);

    if (_mana < 0)
        t_mana_spent = state.t;
}

double Player::manaPerSecond(const State& state) const
{
    double mps = staticManaPerSecond();
    double spi = spiritManaPerSecond();

    if (config.mana_replenishment)
        mps += maxMana() * 0.01 / 5.0;

    double while_casting = 0;
    if (t_mana_spent + 5 <= state.t) {
        while_casting = 1;
    }
    if (talents.arcane_meditation) {
        if (talents.arcane_meditation == 1)
            while_casting += 0.17;
        else if (talents.arcane_meditation == 2)
            while_casting += 0.33;
        else if (talents.arcane_meditation == 3)
            while_casting += 0.5;
    }
    if (talents.pyromaniac) {
        if (talents.pyromaniac == 1)
            while_casting += 0.17;
        else if (talents.pyromaniac == 2)
            while_casting += 0.33;
        else if (talents.pyromaniac == 3)
            while_casting += 0.5;
    }
    if (config.mage_armor) {
        while_casting += 0.5;
        if (glyphs.mage_armor)
            while_casting += 0.2;
        if (config.t9_2set)
            while_casting += 0.1;
    }

    mps += std::min(1.0, while_casting) * spi;

    return mps;
}

double Player::maxMana() const
{
    double mana = Unit::maxMana();

    if (config.meta_gem == META_BEAMING_EARTHSIEGE)
        mana *= 1.02;

    return mana;
}

double Player::baseCastTime(std::shared_ptr<spell::Spell> spell) const
{
    double t = Unit::baseCastTime(spell);

    if (spell->id == spell::FROSTBOLT && talents.imp_frostbolt)
        t -= talents.imp_frostbolt * 0.1;
    if (spell->id == spell::FROSTBOLT && talents.empowered_frostbolt)
        t -= talents.empowered_frostbolt * 0.1;

    if (spell->id == spell::FIREBALL && talents.imp_fireball)
        t -= talents.imp_fireball * 0.1;

    if (spell->id == spell::FIREBALL && glyphs.fireball)
        t -= 0.15;

    if (spell->id == spell::ARCANE_MISSILES && hasBuff(buff::MISSILE_BARRAGE))
        t -= 2.5;

    if (spell->id == spell::PYROBLAST && hasBuff(buff::HOT_STREAK))
        t = 0;
    if (spell->id == spell::FLAMESTRIKE && hasBuff(buff::FIRESTARTER))
        t = 0;
    if ((spell->id == spell::FIREBALL || spell->id == spell::FROSTFIRE_BOLT) && hasBuff(buff::BRAIN_FREEZE))
        t = 0;

    return t;
}

double Player::castHaste() const
{
    double haste = 1.0 / Unit::castHaste();

    if (talents.netherwind_presence)
        haste *= 1 + talents.netherwind_presence * 0.02;

    if (hasBuff(buff::PUSHING_THE_LIMIT))
        haste *= 1.12;

    if (hasBuff(buff::HODIR_STARLIGHT))
        haste *= 1.5;

    return 1.0 / haste;
}

double Player::hitChance(std::shared_ptr<spell::Spell> spell, double dlevel) const
{
    double hit = Unit::hitChance(spell, dlevel);

    if (spell->school == SCHOOL_ARCANE && talents.arcane_focus)
        hit += talents.arcane_focus * 1.0;

    // Precision double dip for ffb
    if (spell->id == spell::FROSTFIRE_BOLT && talents.precision)
        hit += talents.precision;

    return hit;
}

double Player::critChance(std::shared_ptr<spell::Spell> spell) const
{
    double crit = Unit::critChance(spell);

    if (config.molten_armor) {
        double multi = 0.35;
        if (glyphs.molten_armor)
            multi += 0.2;
        if (config.t9_2set)
            multi += 0.15;
        crit += critRatingToChance(round(getSpirit() * multi));
    }

    if (talents.arcane_potency && hasBuff(buff::ARCANE_POTENCY))
        crit += talents.arcane_potency * 15.0;

    if (talents.incineration) {
        if (spell->id == spell::ARCANE_BLAST ||
            spell->id == spell::CONE_OF_COLD ||
            spell->id == spell::FIRE_BLAST ||
            spell->id == spell::SCORCH)
        {
            crit += talents.incineration * 2.0;
        }
    }

    if (talents.world_in_flames) {
        if (spell->id == spell::ARCANE_EXPLOSION ||
            spell->id == spell::BLAST_WAVE ||
            spell->id == spell::DRAGONS_BREATH ||
            spell->id == spell::FLAMESTRIKE ||
            spell->id == spell::LIVING_BOMB_EXPLOSION ||
            spell->id == spell::PYROBLAST)
        {
            crit += talents.world_in_flames * 2.0;
        }
    }

    if (talents.imp_scorch) {
        if (spell->id == spell::SCORCH ||
            spell->id == spell::FIREBALL ||
            spell->id == spell::FROSTFIRE_BOLT)
        {
            crit += talents.imp_scorch * 1.0;
        }
    }

    if ((spell->school == SCHOOL_FIRE || spell->school == SCHOOL_FROSTFIRE) && talents.critical_mass)
        crit += talents.critical_mass * 2.0;

    if (spell->id == spell::FROSTFIRE_BOLT && glyphs.frostfire)
        crit += 2;

    if (hasBuff(buff::COMBUSTION) && (spell->school == SCHOOL_FIRE || spell->school == SCHOOL_FROSTFIRE))
        crit += buffStacks(buff::COMBUSTION) * 10.0;

    if (talents.shatter && isFrozen()) {
        crit += talents.shatter * 17;
        if (talents.shatter == 3)
            crit -= 1;
    }

    if (config.t9_4set) {
        if (spell->id == spell::ARCANE_BLAST ||
            spell->id == spell::ARCANE_MISSILES ||
            spell->id == spell::FIREBALL ||
            spell->id == spell::FROSTBOLT ||
            spell->id == spell::FROSTFIRE_BOLT)
        {
            crit += 5.0;
        }
    }

    return crit;
}

bool Player::canCrit(std::shared_ptr<spell::Spell> spell) const
{
    if (glyphs.living_bomb && spell->id == spell::LIVING_BOMB)
        return true;

    return Unit::canCrit(spell);
}

double Player::baseCritMultiplier(std::shared_ptr<spell::Spell> spell) const
{
    double base = 1;

    if (config.meta_gem == META_CHAOTIC_SKYFLARE)
        base *= 1.03;
    if (hasBuff(buff::HODIR_STORM_POWER))
        base *= 2.35;

    return base;
}

double Player::critMultiplierMod(std::shared_ptr<spell::Spell> spell) const
{
    double multi = Unit::critMultiplierMod(spell);

    if (talents.spell_power)
        multi += talents.spell_power * 0.25;
    if (talents.burnout)
        multi += talents.burnout * 0.1;
    if (config.t7_4set)
        multi += 0.05;

    if (spell->id == spell::ARCANE_MISSILES && glyphs.arcane_missiles)
        multi += 0.25;

    if ((spell->school == SCHOOL_FROST || spell->school == SCHOOL_FROSTFIRE) && talents.ice_shards) {
        if (talents.ice_shards == 3)
            multi += 1;
        else
            multi += talents.ice_shards * 0.33;
    }

    if (hasBuff(buff::COMBUSTION) && (spell->school == SCHOOL_FIRE || spell->school == SCHOOL_FROSTFIRE))
        multi += 0.5;

    return multi;
}

double Player::spellCoeffMod(std::shared_ptr<spell::Spell> spell) const
{
    double coeff = Unit::spellCoeffMod(spell);

    if (talents.arcane_empowerment) {
        if (spell->id == spell::ARCANE_MISSILES)
            coeff += talents.arcane_empowerment * 0.15;
        if (spell->id == spell::ARCANE_BLAST)
            coeff += talents.arcane_empowerment * 0.03;
    }

    if (talents.empowered_fire) {
        if (spell->id == spell::FIREBALL ||
            spell->id == spell::FROSTFIRE_BOLT ||
            spell->id == spell::PYROBLAST_DOT ||
            spell->id == spell::PYROBLAST)
        {
            coeff += talents.empowered_fire * 0.05;
        }
    }

    if (talents.empowered_frostbolt && spell->id == spell::FROSTBOLT)
        coeff += talents.empowered_frostbolt * 0.05;

    return coeff;
}

double Player::buffDmgMultiplier(std::shared_ptr<spell::Spell> spell, const State& state) const
{
    double multi = Unit::buffDmgMultiplier(spell, state);
    double additive = 1;

    if (talents.arcane_instability)
        multi *= 1 + talents.arcane_instability * 0.01;
    if (talents.playing_with_fire)
        multi *= 1 + talents.playing_with_fire * 0.01;
    if (talents.piercing_ice && (spell->school == SCHOOL_FROST || spell->school == SCHOOL_FROSTFIRE))
        multi *= 1 + talents.piercing_ice * 0.02;
    if (talents.arctic_winds && (spell->school == SCHOOL_FROST || spell->school == SCHOOL_FROSTFIRE))
        multi *= 1 + talents.arctic_winds * 0.01;
    if (spell->id == spell::CONE_OF_COLD && talents.imp_cone_of_cold)
        multi *= 1.05 + talents.imp_cone_of_cold * 0.1;
    if (hasBuff(buff::QUAD_CORE))
        multi *= 1.18;
    if (config.udc_2set)
        multi *= 1.01;
    if (config.udc_3set)
        multi *= 1.02;
    if (config.udc_4set)
        multi *= 1.03;
    if (config.cudc_3set)
        multi *= 1.02;

    if (hasBuff(buff::IRON_COUNCIL_SHIELD_OF_RUNES))
        multi *= 1.5;

    if (talents.torment_of_the_weak) {
        if (spell->id == spell::FROSTBOLT ||
            spell->id == spell::FIREBALL ||
            spell->id == spell::FIREBALL_DOT ||
            spell->id == spell::FROSTFIRE_BOLT ||
            spell->id == spell::FROSTFIRE_BOLT_DOT ||
            spell->id == spell::PYROBLAST ||
            spell->id == spell::PYROBLAST_DOT ||
            spell->id == spell::ARCANE_MISSILES ||
            spell->id == spell::ARCANE_BLAST ||
            spell->id == spell::ARCANE_BARRAGE)
        {
            multi *= 1 + (0.04 * talents.torment_of_the_weak);
        }
    }

    // Below 35% - We'll estimate that to last 35% of duration
    // Living Bomb dot is not affected by Molten Fury for some reason
    if (talents.molten_fury && state.t / state.duration >= 0.65 && spell->id != spell::LIVING_BOMB)
        multi *= 1 + (talents.molten_fury * 0.06);

    if (spell->school == SCHOOL_ARCANE && hasBuff(buff::ARCANE_BLAST, true)) {
        double ab = 0.15;
        if (glyphs.arcane_blast)
            ab += 0.03;
        multi *= 1 + ab * buffStacks(buff::ARCANE_BLAST, true);
    }

    if (config.t5_2set && spell->id == spell::ARCANE_BLAST)
        multi *= 1.05;

    if (spell->id == spell::ICE_LANCE && isFrozen()) {
        if (glyphs.ice_lance)
            multi *= 4;
        else
            multi *= 3;
    }

    // Additive category
    additive = 1;

    if (talents.fire_power && (spell->school == SCHOOL_FIRE || spell->school == SCHOOL_FROSTFIRE))
        additive += talents.fire_power * 0.02;
    if (spell->id == spell::SCORCH && glyphs.scorch)
        additive += 0.2;
    if (spell->id == spell::FROSTFIRE_BOLT && glyphs.frostfire)
        additive += 0.02;
    if (config.t6_4set && (spell->id == spell::FIREBALL || spell->id == spell::FROSTBOLT || spell->id == spell::ARCANE_MISSILES))
        additive += 0.05;
    if (hasBuff(buff::ARCANE_POWER) && !spell->proc)
        additive += 0.2;

    if (talents.spell_impact) {
        if (spell->id == spell::ARCANE_BLAST ||
            spell->id == spell::ARCANE_EXPLOSION ||
            spell->id == spell::BLAST_WAVE ||
            spell->id == spell::CONE_OF_COLD ||
            spell->id == spell::FIREBALL ||
            spell->id == spell::FIREBALL_DOT ||
            spell->id == spell::FIRE_BLAST ||
            spell->id == spell::ICE_LANCE ||
            spell->id == spell::SCORCH)
        {
            additive += talents.spell_impact * 0.02;
        }
    }

    multi *= additive;

    // Additive category
    additive = 1;

    if (spell->id == spell::FROSTBOLT && glyphs.frostbolt)
        additive += 0.05;

    if (talents.chilled_to_the_bone) {
        if (spell->id == spell::FROSTBOLT ||
            spell->id == spell::FROSTFIRE_BOLT ||
            spell->id == spell::FROSTFIRE_BOLT_DOT ||
            spell->id == spell::ICE_LANCE)
        {
            additive += talents.chilled_to_the_bone * 0.01;
        }
    }

    multi *= additive;

    return multi;
}

double Player::baseManaCost(std::shared_ptr<spell::Spell> spell) const
{
    double cost = Unit::baseManaCost(spell);

    if (hasTrinket(TRINKET_SPARK_HOPE)) {
        cost -= 44;
        if (cost < 0)
            cost = 0;
    }

    return cost;
}

double Player::manaCostMultiplier(std::shared_ptr<spell::Spell> spell) const
{
    double multi = Unit::manaCostMultiplier(spell);

    if (hasBuff(buff::CLEARCAST))
        return 0;

    if (spell->id == spell::FLAMESTRIKE && hasBuff(buff::FIRESTARTER))
        return 0;

    if (spell->id == spell::ARCANE_MISSILES && hasBuff(buff::MISSILE_BARRAGE))
        return 0;

    if (talents.precision)
        multi *= (1.0 - talents.precision * 0.01);
    if (talents.arcane_focus && spell->school == SCHOOL_ARCANE)
        multi *= (1.0 - talents.arcane_focus * 0.01);
    if (spell->id == spell::ARCANE_BARRAGE && glyphs.arcane_barrage)
        multi *= 0.8;
    if (spell->id == spell::ARCANE_EXPLOSION && glyphs.arcane_explosion)
        multi *= 0.9;
    if (glyphs.blast_wave && spell->id == spell::BLAST_WAVE)
        multi *= 0.85;

    if (talents.frost_channeling) {
        if (talents.frost_channeling == 1)
            multi*= 0.96;
        else if (talents.frost_channeling == 2)
            multi*= 0.93;
        else if (talents.frost_channeling == 3)
            multi*= 0.9;
    }

    return multi;
}

double Player::manaCostMod(std::shared_ptr<spell::Spell> spell, double mana_cost) const
{
    double mod = Unit::manaCostMod(spell, mana_cost);

    if (hasBuff(buff::ARCANE_POWER))
        mod+= 0.2 * mana_cost;

    if (spell->id == spell::ARCANE_BLAST) {
        double multi = 1.75 * buffStacks(buff::ARCANE_BLAST);
        if (config.t5_2set)
            multi += 0.05;
        mod+= ceil(mana_cost * multi - 0.5);
    }

    return mod;
}

double Player::getSpellPower(School school) const
{
    double sp = Unit::getSpellPower(school);

    if (hasBuff(buff::FLAME_CAP) && (school == SCHOOL_FIRE || school == SCHOOL_FROSTFIRE))
        sp += 80.0;

    if (talents.mind_mastery)
        sp += getIntellect() * talents.mind_mastery * 0.03;

    return sp;
}

bool Player::shouldConsumeClearcast(std::shared_ptr<spell::Spell> spell) const
{
    if (spell->is_trigger || spell->tick)
        return false;

    if (spell->id == spell::ARCANE_MISSILES && hasBuff(buff::MISSILE_BARRAGE))
        return false;

    return true;
}

double Player::cooldownMod(const cooldown::Cooldown& cooldown) const
{
    if (cooldown.id == cooldown::ARCANE_POWER && cooldown.duration == 1.5)
        return 0;

    double mod = Unit::cooldownMod(cooldown);

    if (talents.arcane_floes) {
        if (cooldown.id == cooldown::PRESENCE_OF_MIND || cooldown.id == cooldown::ARCANE_POWER)
            mod -= cooldown.duration * 0.15 * talents.arcane_floes;
        if (cooldown.id == cooldown::EVOCATION)
            mod -= 60 * talents.arcane_floes;
    }

    if (talents.ice_floes) {
        if (cooldown.id == cooldown::CONE_OF_COLD || cooldown.id == cooldown::ICY_VEINS) {
            if (talents.ice_floes == 1)
                mod -= cooldown.duration * 0.07;
            else if (talents.ice_floes == 2)
                mod -= cooldown.duration * 0.14;
            else if (talents.ice_floes == 3)
                mod -= cooldown.duration * 0.2;
        }
    }

    if (cooldown.id == cooldown::WATER_ELEMENTAL && glyphs.water_elemental)
        mod -= 30;

    if (talents.cold_as_ice) {
        if (cooldown.id == cooldown::COLD_SNAP || cooldown.id == cooldown::WATER_ELEMENTAL)
            mod *= 1.0 - talents.cold_as_ice * 0.1;
    }

    if (cooldown.id == cooldown::EVOCATION && config.t3_2set)
        mod -= 60;

    return mod;
}

bool Player::isFrozen() const
{
    return hasBuff(buff::FINGERS_OF_FROST) || hasBuff(buff::GHOST_FINGERS);
}

bool Player::hasChillEffect(std::shared_ptr<spell::Spell> spell) const
{
    // Fingers of frost still seem to proc even with the glyph? unconfirmed
    // if (spell->id == spell::FROSTBOLT)
        // return !glyphs.frostbolt;
    if (spell->id == spell::CONE_OF_COLD || spell->id == spell::FROSTFIRE_BOLT || spell->id == spell::FROSTBOLT)
        return true;

    return false;
}

void Player::interrupt(const Interruption& interruption)
{
    Unit::interrupt(interruption);

    if (interruption.duration > 0) {
        waited = false;
        should_wait = false;
    }
}

void Player::swapWeapons()
{
    black_magic = !black_magic;

    if (config.rot_black_magic_ench != ENCHANT_NONE) {
        Stats ench_stats;
        if (config.rot_black_magic_ench == ENCHANT_WEAPON_GREATER_SPELLPOWER) {
            ench_stats.spell_power = 63;
        }
        else if (config.rot_black_magic_ench == ENCHANT_WEAPON_MIGHTY_SPELLPOWER) {
            ench_stats.spell_power = 81;
        }
        else if (config.rot_black_magic_ench == ENCHANT_WEAPON_ACCURACY) {
            ench_stats.hit = hitRatingToChance(25);
            ench_stats.crit = critRatingToChance(25);
        }

        // Buff stats are reset every sim
        if (black_magic)
            removeBuffStats(ench_stats);
        else
            addBuffStats(ench_stats);
    }
}

std::vector<action::Action> Player::onBuffGain(const State& state, std::shared_ptr<buff::Buff> buff)
{
    auto actions = Unit::onBuffGain(state, buff);

    if (buff->id == buff::REIGN_UNLIVING_HC && buffStacks(buff->id) == 3) {
        actions.push_back(spellAction<spell::PillarOfFlameHc>());
        actions.push_back(buffExpireAction<buff::ReignUnlivingHc>());
    }
    else if (buff->id == buff::REIGN_UNLIVING_NM && buffStacks(buff->id) == 3) {
        actions.push_back(spellAction<spell::PillarOfFlameNm>());
        actions.push_back(buffExpireAction<buff::ReignUnlivingNm>());
    }
    else if (buff->id == buff::FIRE_WARD) {
        fire_ward = 1950.0 + getSpellPower(SCHOOL_FIRE) * 0.8053;
    }
    else if (buff->id == buff::MANA_SHIELD) {
        mana_shield = 1330.0 + getSpellPower(SCHOOL_ARCANE) * 0.8053;
    }
    else if ((buff->id == buff::CLEARCAST || buff->id == buff::PRESENCE_OF_MIND) && talents.arcane_potency) {
        actions.push_back(buffAction<buff::ArcanePotency>());
    }
    // else if (buff->id == buff::HYPERSPEED_ACCELERATION) {
        // actions.push_back(cooldownAction(make_shared<cooldown::TrinketShared>(buff->duration)));
    // }
    else if (buff->id == buff::ARCANE_BLAST) {
        ab_streak++;
    }
    else if (buff->id == buff::BRAIN_FREEZE) {
        t_brain_freeze = state.t;
    }
    else if (buff->id == buff::INCANTERS_ABSORPTION) {
        t_incanters_absorption = state.t;
    }

    return actions;
}

std::vector<action::Action> Player::onBuffExpire(const State& state, std::shared_ptr<buff::Buff> buff)
{
    std::vector<action::Action> actions = Unit::onBuffExpire(state, buff);

    if (buff->id == buff::VOLATILE_POWER_HC)
        actions.push_back(buffExpireAction<buff::VolatilityHc>());
    if (buff->id == buff::VOLATILE_POWER_NM)
        actions.push_back(buffExpireAction< buff::VolatilityNm>());
    if (buff->id == buff::FIRE_WARD)
        fire_ward = 0;
    if (buff->id == buff::ARCANE_BLAST)
        ab_streak = 0;

    return actions;
}

std::vector<action::Action> Player::onCastSuccessProc(const State& state, std::shared_ptr<spell::Spell> spell)
{
    auto actions = Unit::onCastSuccessProc(state, spell);

    if (spell->id == spell::MANA_GEM)
        return useManaGem();

    if (spell->id == spell::SAPPER_CHARGE)
        return actions;

    bool is_harmful = spell->max_dmg > 0;

    // Cooldowns
    if (spell->id == spell::FIRE_BLAST)
        actions.push_back(cooldownAction<cooldown::FireBlast>(talents.imp_fire_blast));
    if (spell->id == spell::ARCANE_BARRAGE)
        actions.push_back(cooldownAction<cooldown::ArcaneBarrage>());
    if (spell->id == spell::BLAST_WAVE)
        actions.push_back(cooldownAction<cooldown::BlastWave>());
    if (spell->id == spell::CONE_OF_COLD)
        actions.push_back(cooldownAction<cooldown::ConeOfCold>());
    if (spell->id == spell::DRAGONS_BREATH)
        actions.push_back(cooldownAction<cooldown::DragonsBreath>());
    if (spell->id == spell::DEEP_FREEZE)
        actions.push_back(cooldownAction<cooldown::DeepFreeze>());
    if (spell->id == spell::MIRROR_IMAGE)
        actions.push_back(cooldownAction<cooldown::MirrorImage>());
    if (spell->id == spell::WATER_ELEMENTAL)
        actions.push_back(cooldownAction<cooldown::WaterElemental>());
    if (spell->id == spell::EVOCATION) {
        actions.push_back(cooldownAction<cooldown::Evocation>());
        actions.push_back(buffAction<buff::Evocation>(false, castHaste(), evocationTicks()));
    }
    if (spell->id == spell::COLD_SNAP) {
        actions.push_back(cooldownAction<cooldown::ColdSnap>());
        actions.push_back(cooldownExpireAction<cooldown::IcyVeins>());
        actions.push_back(cooldownExpireAction<cooldown::WaterElemental>());
        actions.push_back(cooldownExpireAction<cooldown::ConeOfCold>());
        actions.push_back(cooldownExpireAction<cooldown::DeepFreeze>());
    }
    if (spell->id == spell::FIRE_WARD) {
        actions.push_back(cooldownAction<cooldown::FireWard>());
        actions.push_back(buffAction<buff::FireWard>());
    }
    if (spell->id == spell::MANA_SHIELD) {
        actions.push_back(buffAction<buff::ManaShield>());
    }

    if (hasBuff(buff::ARCANE_POTENCY) && !spell->channeling)
        actions.push_back(buffExpireAction<buff::ArcanePotency>());

    if (spell->id == spell::LIVING_BOMB)
        t_living_bomb = state.t;
    if (spell->id == spell::SCORCH)
        t_scorch = state.t;
    if (spell->id == spell::FLAMESTRIKE) {
        t_flamestrike = state.t;
        actions.push_back(spellAction<spell::FlamestrikeDot>());
    }

    if (hasBuff(buff::GHOST_FINGERS))
        actions.push_back(buffExpireAction<buff::GhostFingers>());

    if (hasBuff(buff::PRESENCE_OF_MIND) && spell->cast_time && !spell->channeling) {
        actions.push_back(buffExpireAction<buff::PresenceOfMind>());
        actions.push_back(cooldownAction<cooldown::ArcanePower>(1.5));
    }

    if (hasBuff(buff::CLEARCAST) && shouldConsumeClearcast(spell))
        actions.push_back(buffExpireAction<buff::Clearcast>());

    if (spell->id == spell::ARCANE_BLAST)
        actions.push_back(buffAction<buff::ArcaneBlast>());
    else if (hasBuff(buff::ARCANE_BLAST) && spell->school == SCHOOL_ARCANE && spell->min_dmg > 0)
        actions.push_back(buffExpireAction<buff::ArcaneBlast>());

    if (hasBuff(buff::BRAIN_FREEZE) && spell->actual_cast_time == 0 && (spell->id == spell::FROSTFIRE_BOLT || spell->id == spell::FIREBALL)) {
        // 20% chance - Sorta confirmed from elitist jerks
        if (!config.t8_4set || random<int>(0, 4) != 0) {
            actions.push_back(buffExpireAction<buff::BrainFreeze>());
            if (config.t10_2set)
                actions.push_back(buffAction<buff::PushingTheLimit>());
        }
    }

    if (spell->id == spell::ARCANE_MISSILES && hasBuff(buff::MISSILE_BARRAGE)) {
        // 20% chance - Sorta confirmed from elitist jerks
        if (!config.t8_4set || random<int>(0, 4) != 0) {
            actions.push_back(buffExpireAction<buff::MissileBarrage>());
            if (config.t10_2set)
                actions.push_back(buffAction<buff::PushingTheLimit>());
        }
    }
    if (spell->id == spell::PYROBLAST && hasBuff(buff::HOT_STREAK)) {
        // 20% chance - Sorta confirmed from elitist jerks
        if (!config.t8_4set || random<int>(0, 4) != 0) {
            actions.push_back(buffExpireAction<buff::HotStreak>());
            if (config.t10_2set)
                actions.push_back(buffAction<buff::PushingTheLimit>());
        }
    }

    if (talents.firestarter && (spell->id == spell::BLAST_WAVE || spell->id == spell::DRAGONS_BREATH))
        actions.push_back(buffAction<buff::Firestarter>());

    if (talents.fingers_of_frost && hasBuff(buff::FINGERS_OF_FROST) && is_harmful) {
        fingers_of_frost--;
        if (fingers_of_frost <= 0) {
            actions.push_back(buffExpireAction<buff::FingersOfFrost>());
            actions.push_back(buffAction<buff::GhostFingers>());
        }
    }

    if (talents.missile_barrage) {
        if (spell->id == spell::ARCANE_BLAST ||
            spell->id == spell::ARCANE_BARRAGE ||
            spell->id == spell::FIREBALL ||
            spell->id == spell::FROSTBOLT ||
            spell->id == spell::FROSTFIRE_BOLT)
        {
            int chance = talents.missile_barrage * 4;
            if (spell->id == spell::ARCANE_BLAST)
                chance *= 2;

            if (random<int>(0, 99) < chance)
                actions.push_back(buffAction<buff::MissileBarrage>());
        }
    }

    if (spell->id == spell::MIRROR_IMAGE) {
        int images = 3;
        if (glyphs.mirror_image)
            images++;
        for (int i = 0; i < images; i++) {
            action::Action action;
            action.type = action::TYPE_UNIT;
            action.unit = std::make_shared<unit::MirrorImage>(config, stats);
            action.unit->name += " #" + std::to_string(i + 1);
            actions.push_back(std::move(action));
        }
        if (config.t10_4set)
            actions.push_back(buffAction<buff::QuadCore>());
    }

    if (spell->id == spell::WATER_ELEMENTAL) {
        action::Action action{ action::TYPE_UNIT };
        action.unit = std::make_shared<unit::WaterElemental>(config, stats);
        if (glyphs.eternal_water)
            action.unit->duration = 0;
        else if (talents.enduring_winter)
            action.unit->duration += talents.enduring_winter * 5.0;
        actions.push_back(std::move(action));
    }

    if (config.t8_2set && !hasCooldown(cooldown::PRAXIS)) {
        if (spell->id == spell::ARCANE_BLAST ||
            spell->id == spell::FIREBALL ||
            spell->id == spell::FROSTBOLT ||
            spell->id == spell::FROSTFIRE_BOLT)
        {
            if (random<int>(0, 3) == 0) {
                actions.push_back(buffCooldownAction<buff::Praxis, cooldown::Praxis>());
            }
        }
    }

    if (spell->can_proc) {
        for (auto& i : onCastOrTick(state, spell))
            actions.push_back(std::move(i));

        // Confirmed - on spell impact and channeled harmful spell cast
        // Does not proc on blizzard(aoe?) cast start (only am?)
        if ((config.black_magic || black_magic) && !hasCooldown(cooldown::BLACK_MAGIC) && spell->channeling && is_harmful && spell->id != spell::BLIZZARD && random<int>(0, 99) < 35) {
            actions.push_back(buffCooldownAction<buff::BlackMagic, cooldown::BlackMagic>());
        }

        // Unconfirmed - on spell cast ?
        if (config.darkglow_embroidery && !hasCooldown(cooldown::DARKGLOW) && random<int>(0, 99) < 35) {
            auto action = manaAction(400, "Darkglow");
            action.cooldown = std::make_shared<cooldown::Darkglow>();
            actions.push_back(std::move(action));
        }

        // Unconfirmed - on spell cast ?
        if (hasTrinket(TRINKET_SIFS_REMEMBERANCE) && !hasCooldown(cooldown::MEMORIES_LOVE) && random<int>(0, 9) == 0) {
            actions.push_back(buffCooldownAction<buff::MemoriesLove, cooldown::MemoriesLove>());
        }

        // Unconfirmed - on spell cast ?
        if (hasTrinket(TRINKET_SHOW_FAITH) && !hasCooldown(cooldown::SHOW_FAITH) && random<int>(0, 9) == 0) {
            actions.push_back(buffCooldownAction<buff::ShowFaith, cooldown::ShowFaith>());
        }

        // Unconfirmed - on harmful spell cast ?
        if (hasTrinket(TRINKET_VOLATILE_POWER_HC) && hasBuff(buff::VOLATILE_POWER_HC) && is_harmful) {
            actions.push_back(buffAction<buff::VolatilityHc>());
        }
        if (hasTrinket(TRINKET_VOLATILE_POWER_NM) && hasBuff(buff::VOLATILE_POWER_NM) && is_harmful) {
            actions.push_back(buffAction<buff::VolatilityNm>());
        }

        // Unconfirmed - on spell cast ?
        if (hasTrinket(TRINKET_SOLACE_DEFEATED_HC)) {
            actions.push_back(buffAction<buff::EnergizedHc>());
        }
        if (hasTrinket(TRINKET_SOLACE_DEFEATED_NM)) {
            actions.push_back(buffAction<buff::EnergizedNm>());
        }

    }

    if (config.rot_black_magic && spell->actual_cast_time == 0 && is_harmful) {
        if (black_magic && canReactTo(cooldown::BLACK_MAGIC, state.t) || !black_magic && !hasCooldown(cooldown::BLACK_MAGIC)) {
            swapWeapons();
        }
    }

    return actions;
}

std::vector<action::Action> Player::onSelfDmg(const State& state, double dmg, School school)
{
    std::vector<action::Action> actions;

    double absorb = 0;
    double abs;

    /**
        * Dark rune / Sapper scaling with amp magic
        * Currently disabled
    if (config.amplify_magic) {
        dmg+= 240.0;
        if (talents.magic_attunement)
            dmg+= talents.magic_attunement * 60.0;
    }
    */

    if (hasBuff(buff::FIRE_WARD) && school == SCHOOL_FIRE) {
        abs = dmg;
        if (abs > fire_ward)
            abs = fire_ward;
        fire_ward -= abs;
        absorb += abs;
        dmg -= abs;

        if (fire_ward <= 0)
            actions.push_back(buffExpireAction<buff::FireWard>());
    }

    if (hasBuff(buff::MANA_SHIELD)) {
        abs = dmg;
        if (abs > mana_shield)
            abs = mana_shield;
        mana_shield -= abs;
        absorb += abs;
        dmg -= abs;

        if (abs) {
            double mana_loss = -abs * 1.5;
            if (talents.arcane_shielding == 1)
                mana_loss *= 0.83;
            else if (talents.arcane_shielding == 2)
                mana_loss *= 0.67;
            actions.push_back(manaAction(mana_loss, "Mana Shield"));
        }

        if (mana_shield <= 0)
            actions.push_back(buffExpireAction<buff::ManaShield>());
    }

    if (absorb && talents.incanters_absorption) {
        absorb *= talents.incanters_absorption * 0.05;
        absorb = round(absorb);

        auto const i = buffs.find(buff::INCANTERS_ABSORPTION);
        if (i != buffs.end()) {
            absorb += (10.0 - (state.t - t_incanters_absorption)) / 10.0 * i->second->stats.spell_power;
            actions.push_back(buffExpireAction(i->second));
        }

        absorb = std::min(absorb, 1000.0); // Assume 20k health, 5% cap = 1000 sp
        actions.push_back(buffAction<buff::IncantersAbsorption>(false, absorb));
    }

    return actions;
}

std::vector<action::Action> Player::onSpellImpactProc(const State& state, const spell::SpellInstance &instance)
{
    std::vector<action::Action> actions = Unit::onSpellImpactProc(state, instance);

    // Special case for Sapper
    if (instance.spell->id == spell::SAPPER_CHARGE) {
        double dmg = random<double>(2188, 2812);
        for (auto &i : onSelfDmg(state, dmg, instance.spell->school))
            actions.push_back(std::move(i));
    }

    if (instance.spell->id == spell::LIVING_BOMB && instance.tick == instance.spell->ticks) {
        actions.push_back(spellAction<spell::LivingBombExplosion>());
    }

    if (instance.result != spell::MISS) {
        if (talents.imp_scorch && instance.spell->id == spell::SCORCH)
            actions.push_back(debuffAction<debuff::ImprovedScorch>());

        if (instance.spell->id == spell::IGNITE && talents.empowered_fire) {
            double chance = talents.empowered_fire * 33;
            if (talents.empowered_fire > 1)
                chance += 1;
            if (chance == 100 || random<int>(0, 99) < chance)
                actions.push_back(manaAction(base_mana * 0.02, "Ignite"));
        }

        if (instance.spell->id == spell::FIREBALL && !glyphs.fireball)
            actions.push_back(spellAction<spell::FireballDot>());
        if (instance.spell->id == spell::PYROBLAST)
            actions.push_back(spellAction<spell::PyroblastDot>());
        if (instance.spell->id == spell::FROSTFIRE_BOLT)
            actions.push_back(spellAction<spell::FrostfireBoltDot>());

        if (instance.spell->dot) {
            if (hasTrinket(TRINKET_EXTRACT_NECROMANTIC_POWER) && !hasCooldown(cooldown::EXTRACT_NECROMANTIC_POWER) && random<int>(0, 9) == 0) {
                actions.push_back(spellCooldownAction<spell::ExtractNecromanticPower, cooldown::ExtractNecromanticPower>());
            }

            if (hasTrinket(TRINKET_NAMELESS_LICH_HC) && !hasCooldown(cooldown::NAMELESS_LICH_HC) && random<int>(0, 9) < 3) {
                actions.push_back(buffCooldownAction<buff::NamelessLichHc, cooldown::NamelessLichHc>());
            }
            if (hasTrinket(TRINKET_NAMELESS_LICH_NM) && !hasCooldown(cooldown::NAMELESS_LICH_NM) && random<int>(0, 9) < 3) {
                actions.push_back(buffCooldownAction<buff::NamelessLichNm, cooldown::NamelessLichNm>());
            }
        }
        else {
            if (talents.clearcast) {
                double chance = talents.clearcast * 2.0;
                // Less chance per tick for channelled spells
                if (instance.spell->ticks)
                    chance /= instance.spell->ticks;
                if (random<double>(0, 100) < chance)
                    actions.push_back(buffAction<buff::Clearcast>());
            }

            // 50% chance, no icd ? unconfirmed
            if (config.judgement_of_wisdom && random<int>(0, 1) == 1)
                actions.push_back(manaAction(base_mana * 0.02, "Judgement of Wisdom"));

            if (hasBuff(buff::COMBUSTION) && (instance.spell->school == SCHOOL_FIRE || instance.spell->school == SCHOOL_FROSTFIRE)) {
                if (instance.result == spell::CRIT)
                    combustion++;
                if (combustion == 3) {
                    actions.push_back(buffExpireAction<buff::Combustion>());
                    actions.push_back(cooldownAction<cooldown::Combustion>());
                }
                else {
                    actions.push_back(buffAction<buff::Combustion>());
                }
            }

            // Confirmed - on spell impact
            // Confirmed - does not proc on channeled spells
            // Unconfirmed - does it proc on resist?
            if (config.lightweave_embroidery && !instance.spell->channeling && !hasCooldown(cooldown::LIGHTWEAVE) && random<int>(0, 99) < 35) {
                actions.push_back(buffCooldownAction<buff::Lightweave, cooldown::Lightweave>());
            }

            // Confirmed - on spell impact and channeled cast success
            if ((config.black_magic || black_magic) && !hasCooldown(cooldown::BLACK_MAGIC) && random<int>(0, 99) < 35) {
                actions.push_back(buffCooldownAction<buff::BlackMagic, cooldown::BlackMagic>());
            }

            // Unconfirmed - on spell impact
            if (config.ashen_band && !hasCooldown(cooldown::ASHEN_BAND) && random<int>(0, 9) == 0) {
                actions.push_back(buffCooldownAction<buff::AshenBand, cooldown::AshenBand>());
            }
        }

        if (hasTrinket(TRINKET_DARKMOON_DEATH) && !hasCooldown(cooldown::DARKMOON_DEATH) && random<int>(0, 19) < 3) {
            actions.push_back(spellCooldownAction<spell::DarkmoonDeath, cooldown::DarkmoonDeath>());
        }

        // Unconfirmed - on spell dmg ?
        if (hasTrinket(TRINKET_MURADINS_SPYGLASS_HC)) {
            actions.push_back(buffAction<buff::MuradinsSpyglassHc>());
        }
        if (hasTrinket(TRINKET_MURADINS_SPYGLASS_NM)) {
            actions.push_back(buffAction<buff::MuradinsSpyglassNm>());
        }
    }

    if (instance.result == spell::CRIT) {
        // Ignite
        if (talents.ignite && (instance.spell->school == SCHOOL_FIRE || instance.spell->school == SCHOOL_FROSTFIRE)) {
            // 40% over 2 ticks = 20%
            actions.push_back(spellAction<spell::Ignite>(round(instance.dmg * 0.2)));
        }

        if (!instance.spell->dot) {
            if (talents.master_of_elements) {
                double mana = baseManaCost(instance.spell) * 0.1 * talents.master_of_elements;
                if ((instance.spell->channeling || instance.spell->dot) && instance.spell->ticks)
                    mana = mana / instance.spell->ticks;
                actions.push_back(manaAction(mana, "Master of Elements"));
            }

            if (talents.burnout && instance.spell->actual_cost)
                actions.push_back(manaAction(instance.spell->actual_cost * -0.01 * talents.burnout, "Burnout"));

            if (hasTrinket(TRINKET_SOUL_DEAD) && !hasCooldown(cooldown::SOUL_DEAD) && random<int>(0, 3) == 0) {
                auto action = manaAction(900, "Soul of the Dead");
                action.cooldown = std::make_shared<cooldown::SoulDead>();
                actions.push_back(std::move(action));
            }

            if (hasTrinket(TRINKET_ASHTONGUE_TALISMAN) && random<int>(0, 1))
                actions.push_back(buffAction<buff::AshtongueTalisman>());

            if (hasTrinket(TRINKET_REIGN_UNLIVING_HC) && !hasCooldown(cooldown::REIGN_UNLIVING_HC)) {
                actions.push_back(buffCooldownAction<buff::ReignUnlivingHc, cooldown::ReignUnlivingHc>());
            }
            if (hasTrinket(TRINKET_REIGN_UNLIVING_NM) && !hasCooldown(cooldown::REIGN_UNLIVING_NM)) {
                actions.push_back(buffCooldownAction<buff::ReignUnlivingNm, cooldown::ReignUnlivingNm>());
            }

            if (config.t5_4set)
                actions.push_back(buffAction<buff::ArcaneMadness>());
        }
    }

    if (talents.hot_streak && !instance.spell->dot) {
        if (instance.spell->id == spell::FIREBALL ||
            instance.spell->id == spell::SCORCH ||
            instance.spell->id == spell::LIVING_BOMB_EXPLOSION ||
            instance.spell->id == spell::FROSTFIRE_BOLT ||
            instance.spell->id == spell::FIRE_BLAST)
        {
            double heating = false;
            if (instance.result == spell::CRIT) {
                if (heating_up)
                    actions.push_back(buffAction<buff::HotStreak>());
                else
                    heating = true;
            }
            heating_up = heating;
        }
    }

    if (talents.fingers_of_frost) {
        if (hasChillEffect(instance.spell)) {
            int chance = 7;
            if (talents.fingers_of_frost == 2)
                chance = 15;
            if (random<int>(0, 99) < chance) {
                fingers_of_frost = 2;
                actions.push_back(buffAction<buff::FingersOfFrost>());
            }
        }
    }

    if (talents.brain_freeze) {
        if (hasChillEffect(instance.spell)) {
            int chance = 5 * talents.brain_freeze;
            if (random<int>(0, 99) < chance)
                actions.push_back(buffAction<buff::BrainFreeze>());
        }
    }

    return actions;
}

std::vector<action::Action> Player::onSpellTickProc(const State& state, std::shared_ptr<spell::Spell> spell, int tick)
{
    std::vector<action::Action> actions = Unit::onSpellTickProc(state, spell, tick);

    if (spell->id == spell::EVOCATION)
        actions.push_back(manaAction(maxMana() * 0.15, "Evocation"));

    if (hasBuff(buff::ARCANE_POTENCY)) {
        // Special case for blizzard (maybe all channeled aoe?)
        if (spell->min_dmg && (spell->id != spell::BLIZZARD || tick == spell->ticks))
            actions.push_back(buffExpireAction<buff::ArcanePotency>());
    }

    if (spell->can_proc) {
        for (auto &i : onCastOrTick(state, spell, tick))
            actions.push_back(std::move(i));
    }

    return actions;
}

std::vector<action::Action> Player::onCastOrTick(const State& state, std::shared_ptr<spell::Spell> spell, int tick)
{
    std::vector<action::Action> actions;

    // Confirmed - on cast/tick
    if (hasTrinket(TRINKET_EMBRACE_SPIDER) && !hasCooldown(cooldown::EMBRACE_SPIDER) && random<int>(0, 9) == 0) {
        actions.push_back(buffCooldownAction<buff::EmbraceSpider, cooldown::EmbraceSpider>());
    }

    // Confirmed - on cast/tick
    // Exception for blizzard ticks (maybe all channeled aoe?)
    if (hasTrinket(TRINKET_ILLUSTRATION_DRAGON_SOUL) && (spell->id != spell::BLIZZARD || !tick)) {
        actions.push_back(buffAction<buff::IllustrationDragonSoul>());
    }

    // Unconfirmed - on cast/tick ?
    // Assumed to be the same as Illustration
    if (hasTrinket(TRINKET_EYE_BROODMOTHER) && (spell->id != spell::BLIZZARD || !tick)) {
        actions.push_back(buffAction<buff::EyeBroodmother>());
    }

    // Confirmed - on cast/tick
    if (hasTrinket(TRINKET_DYING_CURSE) && !hasCooldown(cooldown::DYING_CURSE) && random<int>(0, 19) < 3) {
        actions.push_back(buffCooldownAction<buff::DyingCurse, cooldown::DyingCurse>());
    }

    // Unconfirmed - on cast/tick ?
    if (hasTrinket(TRINKET_PANDORAS_PLEA) && !hasCooldown(cooldown::PANDORAS_PLEA) && random<int>(0, 9) == 0) {
        actions.push_back(buffCooldownAction<buff::PandorasPlea, cooldown::PandorasPlea>());
    }

    // Harmful spells
    if (spell->max_dmg > 0) {

        // Confirmed - on harmful
        if (hasTrinket(TRINKET_FORGE_EMBER) && !hasCooldown(cooldown::FORGE_EMBER) && random<int>(0, 9) == 0) {
            actions.push_back(buffCooldownAction<buff::ForgeEmber, cooldown::ForgeEmber>());
        }

        // Confirmed - on harmful
        if (hasTrinket(TRINKET_SUNDIAL_EXILED) && !hasCooldown(cooldown::NOW_IS_THE_TIME) && random<int>(0, 9) == 0) {
            actions.push_back(buffCooldownAction<buff::NowIsTheTime, cooldown::NowIsTheTime>());
        }

        // Unconfirmed - on harmful ?
        if (hasTrinket(TRINKET_PENDULUM_TELLURIC_CURRENTS) && !hasCooldown(cooldown::PENDULUM_TELLURIC_CURRENTS) && random<int>(0, 19) < 3) {
            actions.push_back(spellCooldownAction<spell::PendulumTelluricCurrents, cooldown::PendulumTelluricCurrents>());
        }

        // Unconfirmed - on harmful ?
        if (hasTrinket(TRINKET_FLARE_HEAVENS) && !hasCooldown(cooldown::FLARE_HEAVENS) && random<int>(0, 9) == 0) {
            actions.push_back(buffCooldownAction<buff::FlareHeavens, cooldown::FlareHeavens>());
        }

        // Unconfirmed - on harmful ?
        if (hasTrinket(TRINKET_ELEMENTAL_FOCUS_STONE) && !hasCooldown(cooldown::ALACRITY_ELEMENTS) && random<int>(0, 9) == 0) {
            actions.push_back(buffCooldownAction<buff::AlacrityElements, cooldown::AlacrityElements>());
        }

        // Unconfirmed - on harmful ?
        if (hasTrinket(TRINKET_DISLODGED_OBJECT_HC) && !hasCooldown(cooldown::DISLODGED_OBJECT_HC) && random<int>(0, 9) == 0) {
            actions.push_back(buffCooldownAction<buff::DislodgedObjectHc, cooldown::DislodgedObjectHc>());
        }
        if (hasTrinket(TRINKET_DISLODGED_OBJECT_NM) && !hasCooldown(cooldown::DISLODGED_OBJECT_NM) && random<int>(0, 9) == 0) {
            actions.push_back(buffCooldownAction<buff::DislodgedObjectNm, cooldown::DislodgedObjectNm>());
        }
    }

    return actions;
}

bool Player::hasManaGem() const
{
    return mana_sapphire > 0;
}

double Player::manaGemMax() const
{
    double max = 0;
    if (mana_sapphire > 0)
        max = 3500;
    else
        return 0;

    double imp = 1;
    if (config.t7_2set)
        imp += 0.25;
    if (hasTrinket(TRINKET_SERPENT_COIL))
        imp += 0.25;
    max *= imp;

    if (glyphs.mana_gem)
        max *= 1.4;

    return max;
}

bool Player::shouldUseManaGem(const State& state)
{
    if (hasCooldown(cooldown::MANA_GEM) || !hasManaGem())
        return false;

    // Check for planned mana gem timings
    auto timing = getNextTiming("mana_gem");
    if (timing && timing->t < state.t + 120)
        return false;

    double max = manaGemMax();

    // If tide is running, add a tick as buffer
    if (hasBuff(buff::MANA_TIDE))
        max += maxMana() * 0.06;

    return maxMana() - mana >= max;
}

bool Player::shouldUseManaPotion(const State& state)
{
    if (config.potion != POTION_MANA || hasCooldown(cooldown::POTION))
        return false;

    // Check for planned potions timings
    auto timing = getNextTiming("potion");
    if (timing)
        return false;

    if (hasBuff(buff::INNERVATE))
        return false;

    double max = 4400;

    if (hasTrinket(TRINKET_MERCURIAL_ALCHEMIST_STONE))
        max *= 1.4;
    if (config.prof_engineer)
        max *= 1.25;

    // If tide is running, add a tick as buffer
    if (hasBuff(buff::MANA_TIDE))
        max += maxMana() * 0.06;

    // If gem is configured to be used within 10 sec, count with the mana gain to avoid overcapping
    if (!hasCooldown(cooldown::MANA_GEM) && hasManaGem()) {
        bool gem_soon = false;
        auto timing = getNextTiming("mana_gem");
        if (timing != NULL && timing->t - state.t < 10)
            max += manaGemMax();
    }

    return maxMana() - mana >= max;
}

bool Player::shouldEvocate(const State& state)
{
    if (hasCooldown(cooldown::EVOCATION))
        return false;

    // Check for planned evocation timings
    auto timing = getNextTiming("evocation");
    if (timing) {
        if (isTimingReady(timing, state)) {
            useTiming(timing);
            return true;
        }
        if (timing->t < state.t + 480)
            return false;
    }

    if (hasBuff(buff::INNERVATE) || hasBuff(buff::MANA_TIDE))
        return false;

    if (manaPercent() > 20.0)
        return false;

    if (hasBuff(buff::BLOODLUST) && manaPercent() > 10.0)
        return false;

    if (state.duration - state.t < 12)
        return false;

    return true;
}

std::vector<action::Action> Player::useManaGem()
{
    std::vector<action::Action> actions;

    double mana = 0;

    if (mana_sapphire > 0) {
        mana_sapphire--;
        mana = random<int>(3330, 3500);
    }

    double imp = 1;
    if (config.t7_2set)
        imp += 0.25;
    if (hasTrinket(TRINKET_SERPENT_COIL))
        imp += 0.25;
    mana *= imp;

    if (glyphs.mana_gem)
        mana *= 1.4;

    actions.push_back(manaAction(mana, "Mana Gem"));

    if (config.t7_2set)
        actions.push_back(buffAction<buff::ManaSurges>());
    if (hasTrinket(TRINKET_SERPENT_COIL))
        actions.push_back(buffAction<buff::ManaSurge>());

    actions.push_back(cooldownAction<cooldown::ManaGem>());

    return actions;
}

std::vector<action::Action> Player::useConjured(const State& state, Conjured conjured)
{
    std::vector<action::Action> actions = Unit::useConjured(state, conjured);

    double cd = 120;

    if (conjured == CONJURED_DARK_RUNE) {
        if (used_dark_rune) {
            cd = 900;
        }
        else {
            cd = 120;
            used_dark_rune = true;
        }
        double dmg = random<double>(600, 1000);
        double mana_gain = random<double>(900, 1500);
        for (auto &i : onSelfDmg(state, dmg, SCHOOL_SHADOW))
            actions.push_back(std::move(i));
        actions.push_back(manaAction(mana_gain, "Dark Rune"));
    }
    else {
        return actions;
    }

    actions.push_back(cooldownAction<cooldown::Conjured>(cd));

    return actions;
}

std::vector<action::Action> Player::usePotion(Potion potion, bool in_combat)
{
    std::vector<action::Action> actions = Unit::usePotion(potion, in_combat);

    double duration = 60;
    if (potion == POTION_MANA) {
        double mana = round(random<double>(4200, 4400));

        if (hasTrinket(TRINKET_MERCURIAL_ALCHEMIST_STONE))
            mana *= 1.4;
        if (config.prof_engineer)
            mana *= 1.25;

        actions.push_back(manaAction(mana, "Mana Potion"));
    }
    else if (potion == POTION_SPEED) {
        actions.push_back(buffAction<buff::Speed>());
    }
    else if (potion == POTION_WILD_MAGIC) {
        actions.push_back(buffAction<buff::WildMagic>());
    }
    else if (potion == POTION_FLAME_CAP) {
        actions.push_back(buffAction<buff::FlameCap>());
        duration = 180;
    }
    else {
        return actions;
    }

    actions.push_back(cooldownAction<cooldown::Potion>(in_combat, duration));

    return actions;
}

bool Player::isTrinketOnSharedCD(Trinket trinket) const
{
    if (!trinketSharesCD(trinket))
        return false;
    return hasCooldown(cooldown::TRINKET_SHARED);
}

bool Player::trinketSharesCD(Trinket trinket) const
{
    return true;
}

bool Player::isUseTrinket(Trinket trinket) const
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

const Timing* Player::getNextTiming(const std::string& name) const
{
    for (int i = 0; i < config.timings.size(); ++i) {
        if (!used_timings[i] && config.timings[i].name == name)
            return &config.timings[i];
    }

    return nullptr;
}

void Player::useTiming(const Timing* timing)
{
    for (auto i = 0; i < config.timings.size(); ++i)
        if (&config.timings[i] == timing)
            used_timings[i] = true;
}

bool Player::isTimingReady(const Timing* timing, const State& state) const
{
    if (timing->t > state.t)
        return false;

    for (auto i = 0; i < config.timings.size(); ++i)
        if (&config.timings[i] == timing && used_timings[i])
            return false;

    if (timing->wait_for_buff != 0 && timing->wait_t + timing->t > state.t) {
        auto buff_id = static_cast<buff::ID>(timing->wait_for_buff);
        if (buff_id == buff::MOLTEN_FURY)
            return state.t / state.duration >= 0.65;
        if (!canReactTo(buff_id, state.t))
            return false;
    }

    return true;
}

bool Player::useTimingIfPossible(const std::string& name, const State& state, bool expl)
{
    auto timing = getNextTiming(name);

    if (timing == NULL && !expl)
        return true;

    if (timing != NULL && isTimingReady(timing, state)) {
        useTiming(timing);
        return true;
    }

    return false;
}

bool Player::isTimingReadySoon(const std::string& name, const State& state, double t)
{
    auto timing = getNextTiming(name);

    if (timing != NULL && timing->t - state.t < t)
        return true;

    return false;
}

std::vector<action::Action> Player::useTrinket(Trinket trinket, std::shared_ptr<cooldown::Cooldown> cooldown)
{
    std::vector<action::Action> actions = Unit::useTrinket(trinket, cooldown);
    std::shared_ptr<buff::Buff> buff;

    cooldown->duration = 120;

    if (trinket == TRINKET_TWILIGHT_SERPENT) {
        buff = std::make_shared<buff::TwilightSerpent>();
    }
    else if (trinket == TRINKET_RUNE_INFINITE_POWER) {
        buff = std::make_shared<buff::InfinitePower>();
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
        buff = std::make_shared<buff::VolatilePowerHc>();
    }
    else if (trinket == TRINKET_VOLATILE_POWER_NM) {
        buff = std::make_shared<buff::VolatilePowerNm>();
    }
    else if (trinket == TRINKET_SCALE_FATES) {
        buff = std::make_shared<buff::ScaleFates>();
    }
    else if (trinket == TRINKET_LIVING_FLAME) {
        buff = std::make_shared<buff::LivingFlame>();
    }
    else if (trinket == TRINKET_TOME_ARCANE_PHENOMENA) {
        buff = std::make_shared<buff::TomeArcanePhenomena>();
    }
    else if (trinket == TRINKET_CANNONEERS_FUSELIGHTER) {
        buff = std::make_shared<buff::ArgentValor>();
    }
    else if (trinket == TRINKET_MARK_WAR_PRISONER) {
        buff = std::make_shared<buff::MarkWarPrisoner>();
    }
    else if (trinket == TRINKET_NAARU_SLIVER) {
        buff = std::make_shared<buff::NaaruSliver>();
        cooldown->duration = 90;
    }
    else if (trinket == TRINKET_SKULL_GULDAN) {
        buff = std::make_shared<buff::SkullGuldan>();
    }
    else if (trinket == TRINKET_SHRUNKEN_HEAD) {
        buff = std::make_shared<buff::ShrunkenHead>();
    }
    else if (trinket == TRINKET_MQG) {
        buff = std::make_shared<buff::MindQuickening>();
        cooldown->duration = 300;
    }

    if (buff != nullptr)
        actions.push_back(buffAction(buff));

    if (actions.size() > 0) {
        actions.push_back(cooldownAction(cooldown));
        if (trinketSharesCD(trinket))
            actions.push_back(cooldownAction<cooldown::TrinketShared>(buff->duration));
    }

    return actions;
}

int Player::evocationTicks() const
{
    int ticks = 4;

    if (config.t6_2set)
        ++ticks;

    if (config.evo_ticks && config.evo_ticks <= ticks)
        ticks = config.evo_ticks;

    return ticks;
}

action::Action Player::useCooldown(const State& state)
{
    if (state.isMoving()) {
        if (talents.presence_of_mind && !hasCooldown(cooldown::PRESENCE_OF_MIND) && !hasBuff(buff::ARCANE_POWER) && useTimingIfPossible("presence_of_mind", state)) {
            return buffCooldownAction<buff::PresenceOfMind, cooldown::PresenceOfMind>(true);
        }
        else if (!hasCooldown(cooldown::MIRROR_IMAGE) && useTimingIfPossible("mirror_image", state, true)) {
            return spellCooldownAction<spell::MirrorImage, cooldown::MirrorImage>();
        }
        return { action::TYPE_NONE };
    }

    if (talents.arcane_power && !hasCooldown(cooldown::ARCANE_POWER) && !hasBuff(buff::PRESENCE_OF_MIND) && useTimingIfPossible("arcane_power", state)) {
        auto action = buffAction<buff::ArcanePower>(true, glyphs.arcane_power);
        action.cooldown = std::make_shared<cooldown::ArcanePower>();
        return action;
    }
    else if (talents.combustion && !hasCooldown(cooldown::COMBUSTION) && !hasBuff(buff::COMBUSTION) && useTimingIfPossible("combustion", state)) {
        combustion = 0;
        return buffAction<buff::Combustion>(true);
    }
    else if (talents.presence_of_mind && !hasCooldown(cooldown::PRESENCE_OF_MIND) && !hasBuff(buff::ARCANE_POWER) && useTimingIfPossible("presence_of_mind", state)) {
        return buffCooldownAction<buff::PresenceOfMind, cooldown::PresenceOfMind>(true);
    }
    else if (talents.icy_veins && !hasCooldown(cooldown::ICY_VEINS) && useTimingIfPossible("icy_veins", state)) {
        return buffCooldownAction<buff::IcyVeins, cooldown::IcyVeins>(true);
    }
    else if (talents.cold_snap && !hasCooldown(cooldown::COLD_SNAP) && useTimingIfPossible("cold_snap", state, true)) {
        return spellCooldownAction<spell::ColdSnap, cooldown::ColdSnap>();
    }
    else if (race == RACE_TROLL && !hasCooldown(cooldown::BERSERKING) && useTimingIfPossible("berserking", state)) {
        return buffCooldownAction<buff::Berserking, cooldown::Berserking>(true);
    }
    else if (config.hyperspeed_accelerators && !hasCooldown(cooldown::HYPERSPEED_ACCELERATION) && useTimingIfPossible("hyperspeed_accelerators", state)) {
        return buffCooldownAction<buff::HyperspeedAcceleration, cooldown::HyperspeedAcceleration>(true);
    }
    else if (config.potion != POTION_NONE && config.potion != POTION_MANA && !hasCooldown(cooldown::POTION) && useTimingIfPossible("potion", state)) {
        action::Action action { action::TYPE_POTION };
        action.potion = config.potion;
        action.primary_action = true;
        return action;
    }
    else if (config.potion == POTION_MANA && !hasCooldown(cooldown::POTION) && useTimingIfPossible("potion", state, true)) {
        action::Action action { action::TYPE_POTION };
        action.potion = config.potion;
        action.primary_action = true;
        return action;
    }
    else if (config.conjured == CONJURED_DARK_RUNE && !hasCooldown(cooldown::CONJURED) && (
        useTimingIfPossible("conjured", state, true) ||
        (talents.incanters_absorption && config.pre_incanters_absorption && config.pre_mana_incanters_absorption && hasBuff(buff::MANA_SHIELD) && hasBuff(buff::ARCANE_POWER) && getNextTiming("conjured") == NULL) ||
        ((!talents.incanters_absorption || !config.pre_incanters_absorption || !config.pre_mana_incanters_absorption) && maxMana() - mana > 1500 && useTimingIfPossible("conjured", state))))
    {
        action::Action action{ action::TYPE_CONJURED };
        action.conjured = config.conjured;
        action.primary_action = true;
        return action;
    }
    else if (!hasCooldown(cooldown::MANA_GEM) && useTimingIfPossible("mana_gem", state, true)) {
        return spellAction<spell::ManaGem>();
    }
    else if (!hasCooldown(cooldown::SAPPER_CHARGE) && (
        useTimingIfPossible("sapper_charge", state, true) ||
        (talents.incanters_absorption && config.pre_incanters_absorption && (hasBuff(buff::FIRE_WARD) || hasBuff(buff::MANA_SHIELD)) && hasBuff(buff::ARCANE_POWER) && getNextTiming("sapper_charge") == NULL)))
    {
        return spellCooldownAction<spell::SapperCharge, cooldown::SapperCharge>();
    }
    else if (isUseTrinket(config.trinket1) && !hasCooldown(cooldown::TRINKET1) && !isTrinketOnSharedCD(config.trinket1) && useTimingIfPossible("trinket1", state)) {
        action::Action action{ action::TYPE_TRINKET };
        action.cooldown = std::make_shared<cooldown::Cooldown>(cooldown::TRINKET1);
        action.trinket = config.trinket1;
        action.primary_action = true;
        return action;
    }
    else if (isUseTrinket(config.trinket2) && !hasCooldown(cooldown::TRINKET2) && !isTrinketOnSharedCD(config.trinket2) && useTimingIfPossible("trinket2", state)) {
        action::Action action{ action::TYPE_TRINKET };
        action.cooldown = std::make_shared<cooldown::Cooldown>(cooldown::TRINKET2);
        action.trinket = config.trinket2;
        action.primary_action = true;
        return action;
    }
    else if (!hasCooldown(cooldown::EVOCATION) && useTimingIfPossible("evocation", state, true)) {
        return spellAction<spell::Evocation>(evocationTicks());
    }
    else if (talents.water_elemental && !hasCooldown(cooldown::WATER_ELEMENTAL) && !state.hasUnit(unit::WATER_ELEMENTAL) && useTimingIfPossible("water_elemental", state)) {
        return spellCooldownAction<spell::WaterElemental, cooldown::WaterElemental>();
    }
    else if (!hasCooldown(cooldown::MIRROR_IMAGE) && useTimingIfPossible("mirror_image", state, true)) {
        return spellCooldownAction<spell::MirrorImage, cooldown::MirrorImage>();
    }

    return { action::TYPE_NONE };
}

bool Player::canBlast(const State& state) const
{
    // Lets assume we cant blast for 30+ seconds to save computing time
    if (state.timeRemain() > 30)
        return false;

    auto ab = std::make_shared<spell::ArcaneBlast>();
    auto const cast_time = 2.5 * castHaste();
    auto const base_cost = baseManaCost(ab);

    double cur_mana = mana;
    int stacks = buffStacks(buff::ARCANE_BLAST);

    for (auto t = state.t; t <= state.duration; t += cast_time) {
        if (stacks < 4)
            stacks++;
        cur_mana -= base_cost * 1.75 * stacks;
        if (cur_mana < 0)
            return false;
    }

    return true;
}

action::Action Player::preCombat(const State& state)
{
    if (talents.incanters_absorption && config.pre_incanters_absorption && config.pre_mana_incanters_absorption && !hasBuff(buff::MANA_SHIELD)) {
        return spellAction<spell::ManaShield>();
    }
    else if (talents.incanters_absorption && config.pre_incanters_absorption && !hasBuff(buff::FIRE_WARD)) {
        return spellAction<spell::FireWard>();
    }
    else if (config.pre_potion && !hasCooldown(cooldown::POTION) && state.t >= -2.0) {
        action::Action action{ action::TYPE_POTION };
        action.potion = config.pre_potion;
        return action;
    }
    else if (talents.water_elemental && config.pre_water_elemental && !state.hasUnit(unit::WATER_ELEMENTAL)) {
        return spellCooldownAction<spell::WaterElemental, cooldown::WaterElemental>();
    }
    else if (config.pre_mirror_image && !hasCooldown(cooldown::MIRROR_IMAGE)) {
        return spellCooldownAction<spell::MirrorImage, cooldown::MirrorImage>();
    }

    return { action::TYPE_NONE };
}

action::Action Player::nextAction(const State& state)
{
    if (!state.inCombat())
        return preCombat(state);

    auto gcd = gcdAction(state.t);

    if (gcd.type != action::TYPE_NONE)
        return gcd;

    auto cd = useCooldown(state);

    if (cd.type != action::TYPE_NONE)
        return cd;

    // Mana consumes
    if (shouldUseManaGem(state)) {
        return spellAction<spell::ManaGem>();
    }
    else if (shouldUseManaPotion(state)) {
        action::Action action{ action::TYPE_POTION };
        action.potion = config.potion;
        action.primary_action = true;
        return action;
    }
    else if (shouldEvocate(state)) {
        return spellAction<spell::Evocation>(evocationTicks());
    }

    if (config.maintain_imp_scorch && talents.imp_scorch && t_scorch + 27.0 <= state.t) {
        return spellAction<spell::Scorch>();
    }

    // FFB / Scorch
    if (config.rotation == ROTATION_ST_FROSTFIRE || config.rotation == ROTATION_ST_FIRE_SC) {
        bool no_bomb = talents.living_bomb && t_living_bomb + 12.0 <= state.t && state.t + 12.0 < state.duration;
        if (no_bomb && !heating_up) {
            return spellAction<spell::LivingBomb>();
        }
        else if (canReactTo(buff::HOT_STREAK, state.t)) {
            return spellAction<spell::Pyroblast>();
        }
        else if (no_bomb) {
            return spellAction<spell::LivingBomb>();
        }
        else if (state.isMoving() && !hasBuff(buff::PRESENCE_OF_MIND)) {
            if (!hasCooldown(cooldown::FIRE_BLAST)) {
                return spellAction<spell::FireBlast>();
            }
            else {
                action::Action action{ action::TYPE_WAIT };
                action.value = state.interruptedFor()
;                if (action.value > 0.1)
                    action.value = 0.1;
                return action;
            }
        }
        else {
            if (config.rotation == ROTATION_ST_FIRE_SC)
                return spellAction<spell::Scorch>();
            else
                return spellAction<spell::FrostfireBolt>();
        }
    }

    // Fire rotation
    else if (config.rotation == ROTATION_ST_FIRE) {
        bool no_bomb = talents.living_bomb && t_living_bomb + 12.0 <= state.t && state.t + 12.0 < state.duration;
        if (no_bomb && !heating_up) {
            auto action = spellAction<spell::LivingBomb>();
            should_wait = false;
            return action;
        }
        else if (canReactTo(buff::HOT_STREAK, state.t)) {
            if (waited || !should_wait || !config.hot_streak_cqs) {
                auto action = spellAction<spell::Pyroblast>();
                waited = false;
                should_wait = false;
                return action;
            }
            else {
                action::Action action;
                action.type = action::TYPE_WAIT;
                action.value = config.hot_streak_cqs_time / 1000.0;
                waited = true;
                return action;
            }
        }
        else if (no_bomb) {
            auto action = spellAction<spell::LivingBomb>();
            should_wait = false;
            return action;
        }
        else if (state.isMoving() && !hasBuff(buff::PRESENCE_OF_MIND)) {
            if (!hasCooldown(cooldown::FIRE_BLAST)) {
                return spellAction<spell::FireBlast>();
            }
            else {
                action::Action action{ action::TYPE_WAIT };
                action.value = state.interruptedFor();
                if (action.value > 0.1)
                    action.value = 0.1;
                return action;
            }
        }
        else {
            auto action = spellAction<spell::Fireball>();
            should_wait = true;
            return action;
        }
    }

    // Arcane rotations
    else if (config.rotation == ROTATION_ST_AB_AM || config.rotation == ROTATION_ST_AB_AM_BARRAGE) {
        int ab_stacks = 4;
        if (config.rot_ab3_mana > 0 && manaPercent() < config.rot_ab3_mana)
            ab_stacks = 3;
        bool has_mb = canReactTo(buff::MISSILE_BARRAGE, state.t);

        if (state.isMoving() && !hasBuff(buff::PRESENCE_OF_MIND)) {
            if (talents.arcane_barrage && !hasCooldown(cooldown::ARCANE_BARRAGE)) {
                return spellAction<spell::ArcaneBarrage>();
            }
            else if (!hasCooldown(cooldown::FIRE_BLAST)) {
                return spellAction<spell::FireBlast>();
            }
            else {
                action::Action action{ action::TYPE_WAIT };
                action.value = state.interruptedFor();
                if (action.value > 0.1)
                    action.value = 0.1;
                return action;
            }
        }
        // AB until the end
        else if (canBlast(state))
            return spellAction<spell::ArcaneBlast>();
        // Extra ABs before first AP
        else if (!hasBuff(buff::ARCANE_POWER) && isTimingReadySoon("arcane_power", state, 5) && state.t < 10)
            return spellAction<spell::ArcaneBlast>();
        // Extra ABs during AP
        else if (hasBuff(buff::ARCANE_POWER) && config.rot_abs_ap + 4 > ab_streak && state.t < 60)
            return spellAction<spell::ArcaneBlast>();
        // AM if we have MB and below n AB stacks
        else if (config.rot_mb_below_ab && has_mb && buffStacks(buff::ARCANE_BLAST) < config.rot_mb_below_ab)
            return spellAction<spell::ArcaneMissiles>();
        // AM if we have MB and below mana %
        else if (config.rot_mb_mana && has_mb && manaPercent() < config.rot_mb_mana)
            return spellAction<spell::ArcaneMissiles>();
        // AB if we don't have barrage and over mana %
        else if (!has_mb && config.rot_ab_no_mb_mana < manaPercent())
            return spellAction<spell::ArcaneBlast>();
        // AM / Abarr if we have AB stacks
        else if (buffStacks(buff::ARCANE_BLAST) >= ab_stacks) {
            // Arcane Barrage if we don't have Missile Barrage proc
            if (!has_mb && config.rotation == ROTATION_ST_AB_AM_BARRAGE)
                return spellAction<spell::ArcaneBarrage>();
            else
                return spellAction<spell::ArcaneMissiles>();
        }
        else
            return spellAction<spell::ArcaneBlast>();
    }

    // Frost
    else if (config.rotation == ROTATION_ST_FROST) {
        if (hasBuff(buff::GHOST_FINGERS)) {
            if (talents.deep_freeze && !hasCooldown(cooldown::DEEP_FREEZE))
                return spellAction<spell::DeepFreeze>();
            else if (canReactTo(buff::BRAIN_FREEZE, state.t))
                return spellAction<spell::FrostfireBolt>();
            else if (config.rot_ice_lance)
                return spellAction<spell::IceLance>();
        }
        else if (canReactTo(buff::BRAIN_FREEZE, state.t) && 15.0 - (state.t - t_brain_freeze) <= config.rot_brain_freeze_hold) {
            return spellAction<spell::FrostfireBolt>();
        }
        else if (state.isMoving() && !hasBuff(buff::PRESENCE_OF_MIND)) {
            if (!hasCooldown(cooldown::FIRE_BLAST)) {
                return spellAction<spell::FireBlast>();
            }
            else {
                action::Action action{ action::TYPE_WAIT };
                action.value = state.interruptedFor();
                if (action.value > 0.1)
                    action.value = 0.1;
                return action;
            }
        }

        return spellAction<spell::Frostbolt>();
    }
    // Arcane Explosion
    else if (config.rotation == ROTATION_AOE_AE) {
        return spellAction<spell::ArcaneExplosion>();
    }

    // Blizzard
    else if (config.rotation == ROTATION_AOE_BLIZZ) {
        if (state.isMoving())
            return spellAction<spell::ArcaneExplosion>();
        return spellAction<spell::Blizzard>();
    }

    // Blizzard + Flamestrike
    else if (config.rotation == ROTATION_AOE_BLIZZ_FS) {
        auto fs = std::make_shared<spell::Flamestrike>();
        if (state.isMoving())
            return spellAction<spell::ArcaneExplosion>();
        else if (t_flamestrike + 8.0 + castTime(fs) <= state.t)
            return spellAction(fs);
        else
            return spellAction<spell::Blizzard>();
    }

    // Undefined rotation
    else {
        return spellAction<spell::Fireball>();
    }

    throw std::runtime_error("Player::nextAction failed to decide");
}

}