struct Config
{

    int duration = 200;
    double duration_variance = 1.5;
    int rng_seed = 0;
    bool avg_spell_dmg = false;
    bool additional_data = false;
    int targets = 1;
    int target_resistance = 0;
    int target_level = 83;
    int spell_travel_time = 500;
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
    bool mage_armor = true;
    bool molten_armor = false;

    // Enchants / procs
    bool black_magic = false;
    bool lightweave_embroidery = false;
    bool darkglow_embroidery = false;
    bool hyperspeed_accelerators = false;
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

    bool drums_friend = false;
    bool pre_mirror_image = false;
    bool pre_water_elemental = false;

    Drums drums;
    Potion potion;
    Potion pre_potion;
    Conjured conjured;

    Trinket trinket1 = TRINKET_NONE;
    Trinket trinket2 = TRINKET_NONE;
    MetaGem meta_gem = META_NONE;

    // Rotation
    Rotation rotation = ROTATION_ST_AB_AM;
    bool rot_ab_stacks_three = false;
    bool rot_ice_lance = false;

    std::vector<std::shared_ptr<Timing>> timings;
    std::vector<std::shared_ptr<Interruption>> interruptions;

};
