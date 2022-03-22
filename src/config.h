struct Config
{

    int duration = 200;
    double duration_variance = 1.5;
    int rng_seed = 0;
    bool avg_spell_dmg = false;
    bool additional_data = false;
    int targets = 1;
    int spell_travel_time = 500;

    bool debuff_spell_crit = false;
    bool debuff_spell_dmg = false;
    bool buff_kings = false;
    bool buff_dmg = false;
    bool mage_armor = true;
    bool molten_armor = false;
    bool drums_friend = false;

    Drums drums;
    Potion potion;
    Conjured conjured;

    Trinket trinket1 = TRINKET_NONE;
    Trinket trinket2 = TRINKET_NONE;
    MetaGem meta_gem = META_NONE;

    int innervate = 0;
    int evo_ticks = 0;
    bool mana_tide = true;
    bool bloodlust = true;
    bool power_infusion = false;

    Rotation rotation = ROTATION_ST_ARCANE;
    bool fire_blast_weave = false;

    std::vector<double> trinket1_t;
    std::vector<double> trinket2_t;
    std::vector<double> presence_of_mind_t;
    std::vector<double> arcane_power_t;
    std::vector<double> icy_veins_t;
    std::vector<double> cold_snap_t;
    std::vector<double> combustion_t;
    std::vector<double> berserking_t;
    std::vector<double> mana_tide_t;
    std::vector<double> bloodlust_t;
    std::vector<double> power_infusion_t;
    std::vector<double> drums_t;
    std::vector<double> innervate_t;
    std::vector<double> potion_t;
    std::vector<double> conjured_t;

    double evocation_at = 0;

};
