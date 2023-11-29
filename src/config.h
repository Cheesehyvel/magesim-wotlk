#pragma once

#include "common.h"
#include "timing.h"
#include "interruption.h"

struct Config
{
    int duration = 200;
    double duration_variance = 1.5;
    int rng_seed = 0;
    bool avg_spell_dmg = false;
    bool additional_data = false;
    bool only_main_dmg = false;
    int targets = 1;
    int dot_targets = 1;
    int target_resistance = 0;
    int target_level = 83;
    double target_hp = 100;
    int distance = 20;
    int reaction_time = 300;

    // Buffs / debuffs
    bool debuff_spell_crit = false;
    bool debuff_spell_dmg = false;
    bool debuff_spell_hit = false;
    bool debuff_crit = false;
    bool buff_spell_crit = false;
    bool buff_spell_haste = false;
    bool buff_haste = false;
    bool buff_dmg = false;
    bool totem_of_wrath = false;
    bool flametongue = false;
    bool demonic_pact = false;
    double demonic_pact_bonus = 0;
    bool mana_replenishment = false;
    bool judgement_of_wisdom = false;
    bool gift_of_arthas = false;
    bool mage_armor = true;
    bool molten_armor = false;
    bool amplify_magic = false;
    bool prof_engineer = false;

    // Enchants / procs
    int icc_buff = 0;
    bool sulfuron_slammer = false;
    bool black_magic = false;
    bool lightweave_embroidery = false;
    bool darkglow_embroidery = false;
    bool hyperspeed_accelerators = false;
    bool t3_2set = false;
    bool t5_2set = false;
    bool t5_4set = false;
    bool t6_2set = false;
    bool t6_4set = false;
    bool t7_2set = false;
    bool t7_4set = false;
    bool t8_2set = false;
    bool t8_4set = false;
    bool t9_2set = false;
    bool t9_4set = false;
    bool t10_2set = false;
    bool t10_4set = false;
    bool udc_2set = false;
    bool udc_3set = false;
    bool udc_4set = false;
    bool cudc_3set = false;
    bool ashen_band = false;
    bool nibelung_hc = false;
    bool nibelung_nm = false;

    bool pre_mirror_image = false;
    bool pre_water_elemental = false;
    bool pre_incanters_absorption = false;
    bool pre_mana_incanters_absorption = false;
    bool pre_cast = false;

    Potion potion = POTION_NONE;
    Potion pre_potion = POTION_NONE;
    Conjured conjured = CONJURED_NONE;

    Trinket trinket1 = TRINKET_NONE;
    Trinket trinket2 = TRINKET_NONE;
    MetaGem meta_gem = META_NONE;

    // Rotation
    Rotation rotation = ROTATION_ST_AB_AM;
    double rot_ab3_mana = 0;
    double rot_ab_no_mb_mana = 100;
    int rot_abs_ap = 0;
    int rot_mb_below_ab = 0;
    double rot_mb_mana = 0;
    bool rot_ice_lance = false;
    bool rot_brain_freeze_fireball = false;
    double rot_brain_freeze_hold = 15;
    bool rot_black_magic = false;
    Enchant rot_black_magic_ench = ENCHANT_NONE;
    bool maintain_imp_scorch = false;
    int evo_ticks = 0;

    std::vector<Timing> timings;
    std::vector<Interruption> interruptions;
};
