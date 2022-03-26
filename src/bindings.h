#ifdef __EMSCRIPTEN__

#include <emscripten.h>
#include <emscripten/bind.h>


std::shared_ptr<Config> allocConfig()
{
    return std::make_shared<Config>();
}

std::shared_ptr<unit::Player> allocPlayer(std::shared_ptr<Config> config)
{
    return std::make_shared<unit::Player>(config);
}

SimulationResult runSimulation(std::shared_ptr<Config> config, std::shared_ptr<unit::Player> player)
{
    std::shared_ptr<Simulation> sim(new Simulation(config, player));
    sim->logging = true;

    return sim->run();
}

SimulationsResult runSimulations(std::shared_ptr<Config> config, std::shared_ptr<unit::Player> player, int iterations)
{
    std::shared_ptr<Simulation> sim(new Simulation(config, player));

    return sim->runMultiple(iterations);
}

EMSCRIPTEN_BINDINGS(my_module) {

    emscripten::register_vector<double>("VectorDouble");

    emscripten::enum_<Rotation>("Rotation")
        .value("ROTATION_ST_ARCANE", ROTATION_ST_ARCANE)
        .value("ROTATION_AOE_AE", ROTATION_AOE_AE)
        ;

    emscripten::enum_<Race>("Race")
        .value("RACE_BLOOD_ELF", RACE_BLOOD_ELF)
        .value("RACE_DRAENEI", RACE_DRAENEI)
        .value("RACE_GNOME", RACE_GNOME)
        .value("RACE_HUMAN", RACE_HUMAN)
        .value("RACE_TROLL", RACE_TROLL)
        .value("RACE_UNDEAD", RACE_UNDEAD);

    emscripten::enum_<Trinket>("Trinket")
        .value("TRINKET_NONE", TRINKET_NONE)
        .value("TRINKET_SERPENT_COIL", TRINKET_SERPENT_COIL)
        .value("TRINKET_MQG", TRINKET_MQG)
        .value("TRINKET_SKULL_GULDAN", TRINKET_SKULL_GULDAN)
        .value("TRINKET_SHRUNKEN_HEAD", TRINKET_SHRUNKEN_HEAD)
        .value("TRINKET_NAARU_SLIVER", TRINKET_NAARU_SLIVER)
        .value("TRINKET_ASHTONGUE_TALISMAN", TRINKET_ASHTONGUE_TALISMAN)
        .value("TRINKET_MERCURIAL_ALCHEMIST_STONE", TRINKET_MERCURIAL_ALCHEMIST_STONE)
        ;

    emscripten::enum_<MetaGem>("MetaGem")
        .value("META_NONE", META_NONE)
        .value("META_CHAOTIC_SKYFLARE", META_CHAOTIC_SKYFLARE)
        .value("META_EMBER_SKYFLARE", META_EMBER_SKYFLARE)
        .value("META_BEAMING_EARTHSIEGE", META_BEAMING_EARTHSIEGE)
        .value("META_INSIGHTFUL_EARTHSIEGE", META_INSIGHTFUL_EARTHSIEGE)
        ;

    emscripten::enum_<Drums>("Drums")
        .value("DRUMS_NONE", DRUMS_NONE)
        .value("DRUMS_OF_WAR", DRUMS_OF_WAR)
        .value("DRUMS_OF_RESTORATION", DRUMS_OF_RESTORATION)
        .value("DRUMS_OF_BATTLE", DRUMS_OF_BATTLE)
    ;

    emscripten::enum_<Potion>("Potion")
        .value("POTION_NONE", POTION_NONE)
        .value("POTION_MANA", POTION_MANA)
        .value("POTION_SPEED", POTION_SPEED)
        .value("POTION_WILD_MAGIC", POTION_WILD_MAGIC);

    emscripten::enum_<Conjured>("Conjured")
        .value("CONJURED_NONE", CONJURED_NONE)
        .value("CONJURED_MANA_GEM", CONJURED_MANA_GEM)
        .value("CONJURED_FLAME_CAP", CONJURED_FLAME_CAP);

    emscripten::enum_<LogType>("LogType")
        .value("LOG_NONE", LOG_NONE)
        .value("LOG_CAST_START", LOG_CAST_START)
        .value("LOG_CAST_SUCCESS", LOG_CAST_SUCCESS)
        .value("LOG_SPELL_IMPACT", LOG_SPELL_IMPACT)
        .value("LOG_MANA", LOG_MANA)
        .value("LOG_BUFF", LOG_BUFF)
        .value("LOG_DOT", LOG_DOT)
        .value("LOG_DEBUG", LOG_DEBUG)
        .value("LOG_WAIT", LOG_WAIT)
        .value("LOG_GCD_CAP", LOG_GCD_CAP)
    ;

    emscripten::class_<Config>("Config")
        .smart_ptr<std::shared_ptr<Config>>("Config")
        .property("duration", &Config::duration)
        .property("duration_variance", &Config::duration_variance)
        .property("rng_seed", &Config::rng_seed)
        .property("avg_spell_dmg", &Config::avg_spell_dmg)
        .property("additional_data", &Config::additional_data)
        .property("targets", &Config::targets)

        .property("debuff_spell_crit", &Config::debuff_spell_crit)
        .property("debuff_spell_dmg", &Config::debuff_spell_dmg)
        .property("buff_dmg", &Config::buff_dmg)
        .property("mana_replenishment", &Config::mana_replenishment)
        .property("mage_armor", &Config::mage_armor)
        .property("molten_armor", &Config::molten_armor)
        .property("drums_friend", &Config::drums_friend)

        .property("drums", &Config::drums)
        .property("potion", &Config::potion)
        .property("conjured", &Config::conjured)

        .property("trinket1", &Config::trinket1)
        .property("trinket2", &Config::trinket2)
        .property("meta_gem", &Config::meta_gem)

        .property("innervate", &Config::innervate)
        .property("evo_ticks", &Config::evo_ticks)
        .property("mana_tide", &Config::mana_tide)
        .property("bloodlust", &Config::bloodlust)
        .property("power_infusion", &Config::power_infusion)

        .property("rotation", &Config::rotation)

        .property("trinket1_t", &Config::trinket1_t)
        .property("trinket2_t", &Config::trinket2_t)
        .property("presence_of_mind_t", &Config::presence_of_mind_t)
        .property("arcane_power_t", &Config::arcane_power_t)
        .property("icy_veins_t", &Config::icy_veins_t)
        .property("cold_snap_t", &Config::cold_snap_t)
        .property("combustion_t", &Config::combustion_t)
        .property("berserking_t", &Config::berserking_t)
        .property("mana_tide_t", &Config::mana_tide_t)
        .property("bloodlust_t", &Config::bloodlust_t)
        .property("power_infusion_t", &Config::power_infusion_t)
        .property("drums_t", &Config::drums_t)
        .property("innervate_t", &Config::innervate_t)
        .property("potion_t", &Config::potion_t)
        .property("conjured_t", &Config::conjured_t)

        .property("evocation_at", &Config::evocation_at)
        ;

    emscripten::function("allocConfig", &allocConfig);

    emscripten::value_object<Stats>("Stats")
        .field("intellect", &Stats::intellect)
        .field("spirit", &Stats::spirit)
        .field("mp5", &Stats::mp5)
        .field("crit", &Stats::crit)
        .field("hit", &Stats::hit)
        .field("haste", &Stats::haste)
        .field("haste_rating", &Stats::haste_rating)
        .field("spell_power", &Stats::spell_power)
        ;

    emscripten::value_object<Talents>("Talents")
        .field("arcane_focus", &Talents::arcane_focus)
        .field("clearcast", &Talents::clearcast)
        .field("spell_impact", &Talents::spell_impact)
        .field("student_of_the_mind", &Talents::student_of_the_mind)
        .field("focus_magic", &Talents::focus_magic)
        .field("arcane_meditation", &Talents::arcane_meditation)
        .field("torment_of_the_weak", &Talents::torment_of_the_weak)
        .field("presence_of_mind", &Talents::presence_of_mind)
        .field("arcane_mind", &Talents::arcane_mind)
        .field("arcane_instability", &Talents::arcane_instability)
        .field("arcane_potency", &Talents::arcane_potency)
        .field("arcane_empowerment", &Talents::arcane_empowerment)
        .field("arcane_power", &Talents::arcane_power)
        .field("arcane_floes", &Talents::arcane_floes)
        .field("mind_mastery", &Talents::mind_mastery)
        .field("missile_barrage", &Talents::missile_barrage)
        .field("netherwind_presence", &Talents::netherwind_presence)
        .field("spell_power", &Talents::spell_power)
        .field("arcane_barrage", &Talents::arcane_barrage)
        .field("imp_fire_blast", &Talents::imp_fire_blast)
        .field("incineration", &Talents::incineration)
        .field("imp_fireball", &Talents::imp_fireball)
        .field("ignite", &Talents::ignite)
        .field("world_in_flames", &Talents::world_in_flames)
        .field("pyroblast", &Talents::pyroblast)
        .field("imp_scorch", &Talents::imp_scorch)
        .field("master_of_elements", &Talents::master_of_elements)
        .field("playing_with_fire", &Talents::playing_with_fire)
        .field("critical_mass", &Talents::critical_mass)
        .field("blast_weave", &Talents::blast_weave)
        .field("fire_power", &Talents::fire_power)
        .field("pyromaniac", &Talents::pyromaniac)
        .field("combustion", &Talents::combustion)
        .field("molten_fury", &Talents::molten_fury)
        .field("empowered_fire", &Talents::empowered_fire)
        .field("firestarter", &Talents::firestarter)
        .field("dragons_breath", &Talents::dragons_breath)
        .field("hot_streak", &Talents::hot_streak)
        .field("burnout", &Talents::burnout)
        .field("living_bomb", &Talents::living_bomb)
        .field("imp_frostbolt", &Talents::imp_frostbolt)
        .field("ice_floes", &Talents::ice_floes)
        .field("ice_shards", &Talents::ice_shards)
        .field("precision", &Talents::precision)
        .field("piercing_ice", &Talents::piercing_ice)
        .field("icy_veins", &Talents::icy_veins)
        .field("frost_channeling", &Talents::frost_channeling)
        .field("shatter", &Talents::shatter)
        .field("cold_snap", &Talents::cold_snap)
        .field("imp_cone_of_cold", &Talents::imp_cone_of_cold)
        .field("cold_as_ice", &Talents::cold_as_ice)
        .field("winters_chill", &Talents::winters_chill)
        .field("arctic_winds", &Talents::arctic_winds)
        .field("empowered_frostbolt", &Talents::empowered_frostbolt)
        .field("fingers_of_frost", &Talents::fingers_of_frost)
        .field("brain_freeze", &Talents::brain_freeze)
        .field("water_elemental", &Talents::water_elemental)
        .field("enduring_winter", &Talents::enduring_winter)
        .field("chilled_to_the_bone", &Talents::chilled_to_the_bone)
        .field("deep_freeze", &Talents::deep_freeze)
        ;

    emscripten::value_object<Glyphs>("Glyphs")
        .field("arcane_barrage", &Glyphs::arcane_barrage)
        .field("arcane_blast", &Glyphs::arcane_blast)
        .field("arcane_explosion", &Glyphs::arcane_explosion)
        .field("arcane_missiles", &Glyphs::arcane_missiles)
        .field("arcane_power", &Glyphs::arcane_power)
        .field("eternal_water", &Glyphs::eternal_water)
        .field("fireball", &Glyphs::fireball)
        .field("frostbolt", &Glyphs::frostbolt)
        .field("frostfire", &Glyphs::frostfire)
        .field("ice_lance", &Glyphs::ice_lance)
        .field("living_bomb", &Glyphs::living_bomb)
        .field("mage_armor", &Glyphs::mage_armor)
        .field("mana_gem", &Glyphs::mana_gem)
        .field("mirror_image", &Glyphs::mirror_image)
        .field("molten_armor", &Glyphs::molten_armor)
        .field("scorch", &Glyphs::scorch)
        .field("water_elemental", &Glyphs::water_elemental)
        .field("blast_wave", &Glyphs::blast_wave)
        ;

    emscripten::class_<unit::Player>("Player")
        .smart_ptr<std::shared_ptr<unit::Player>>("Player")
        .property("race", &unit::Player::race)
        .function("getStats", &unit::Player::getStats)
        .function("setStats", &unit::Player::setStats)
        .function("getTalents", &unit::Player::getTalents)
        .function("setTalents", &unit::Player::setTalents)
        .function("getGlyphs", &unit::Player::getGlyphs)
        .function("setGlyphs", &unit::Player::setGlyphs)
        ;

    emscripten::function("allocPlayer", &allocPlayer);

    emscripten::value_object<SimulationResult>("SimulationResult")
        .field("dmg", &SimulationResult::dmg)
        .field("t", &SimulationResult::t)
        .field("dps", &SimulationResult::dps)
        .field("evocated_at", &SimulationResult::evocated_at)
        .field("t_gcd_capped", &SimulationResult::t_gcd_capped)
        .field("log", &SimulationResult::log)
        .field("spells", &SimulationResult::spells)
        ;

    emscripten::value_object<SimulationsResult>("SimulationsResult")
        .field("min_dps", &SimulationsResult::min_dps)
        .field("avg_dps", &SimulationsResult::avg_dps)
        .field("max_dps", &SimulationsResult::max_dps)
        .field("iterations", &SimulationsResult::iterations)
        .field("histogram", &SimulationsResult::histogram)
        .field("all_results", &SimulationsResult::all_results)
        .field("stats", &SimulationsResult::stats)
        ;

    emscripten::function("runSimulation", &runSimulation);
    emscripten::function("runSimulations", &runSimulations);
}

#endif
