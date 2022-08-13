#ifdef __EMSCRIPTEN__

#include <emscripten.h>
#include <emscripten/bind.h>


void addTiming(std::shared_ptr<Config> config, std::string name, double t, int wait_for_buff = 0, int wait_t = 0)
{
    std::shared_ptr<Timing> timing = std::make_shared<Timing>();
    timing->name = name;
    timing->t = t;
    timing->wait_for_buff = wait_for_buff;
    timing->wait_t = wait_t;
    config->timings.push_back(timing);
}

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
        .value("ROTATION_ST_FROSTFIRE", ROTATION_ST_FROSTFIRE)
        .value("ROTATION_ST_AB_AM", ROTATION_ST_AB_AM)
        .value("ROTATION_ST_AB_AM_BARRAGE", ROTATION_ST_AB_AM_BARRAGE)
        .value("ROTATION_ST_FIRE", ROTATION_ST_FIRE)
        .value("ROTATION_ST_FROST", ROTATION_ST_FROST)
        .value("ROTATION_AOE_AE", ROTATION_AOE_AE)
        .value("ROTATION_AOE_BLIZZ", ROTATION_AOE_BLIZZ)
        .value("ROTATION_AOE_BLIZZ_FS", ROTATION_AOE_BLIZZ_FS)
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
        .value("TRINKET_MQG", TRINKET_MQG)
        .value("TRINKET_SERPENT_COIL", TRINKET_SERPENT_COIL)
        .value("TRINKET_SKULL_GULDAN", TRINKET_SKULL_GULDAN)
        .value("TRINKET_SHRUNKEN_HEAD", TRINKET_SHRUNKEN_HEAD)
        .value("TRINKET_NAARU_SLIVER", TRINKET_NAARU_SLIVER)
        .value("TRINKET_ASHTONGUE_TALISMAN", TRINKET_ASHTONGUE_TALISMAN)
        .value("TRINKET_MERCURIAL_ALCHEMIST_STONE", TRINKET_MERCURIAL_ALCHEMIST_STONE)
        .value("TRINKET_TWILIGHT_SERPENT", TRINKET_TWILIGHT_SERPENT)
        .value("TRINKET_TOME_ARCANE_PHENOMENA", TRINKET_TOME_ARCANE_PHENOMENA)
        .value("TRINKET_CANNONEERS_FUSELIGHTER", TRINKET_CANNONEERS_FUSELIGHTER)
        .value("TRINKET_MARK_WAR_PRISONER", TRINKET_MARK_WAR_PRISONER)
        .value("TRINKET_FORGE_EMBER", TRINKET_FORGE_EMBER)
        .value("TRINKET_PENDULUM_TELLURIC_CURRENTS", TRINKET_PENDULUM_TELLURIC_CURRENTS)
        .value("TRINKET_DARKMOON_DEATH", TRINKET_DARKMOON_DEATH)
        .value("TRINKET_DARKMOON_ILLUSION", TRINKET_DARKMOON_ILLUSION)
        .value("TRINKET_SUNDIAL_EXILED", TRINKET_SUNDIAL_EXILED)
        .value("TRINKET_EMBRACE_SPIDER", TRINKET_EMBRACE_SPIDER)
        .value("TRINKET_DYING_CURSE", TRINKET_DYING_CURSE)
        .value("TRINKET_EXTRACT_NECROMANTIC_POWER", TRINKET_EXTRACT_NECROMANTIC_POWER)
        .value("TRINKET_SOUL_DEAD", TRINKET_SOUL_DEAD)
        .value("TRINKET_ILLUSTRATION_DRAGON_SOUL", TRINKET_ILLUSTRATION_DRAGON_SOUL)
        .value("TRINKET_ELEMENTAL_FOCUS_STONE", TRINKET_ELEMENTAL_FOCUS_STONE)
        .value("TRINKET_EYE_BROODMOTHER", TRINKET_EYE_BROODMOTHER)
        .value("TRINKET_SIFS_REMEMBERANCE", TRINKET_SIFS_REMEMBERANCE)
        .value("TRINKET_SPARK_HOPE", TRINKET_SPARK_HOPE)
        .value("TRINKET_SCALE_FATES", TRINKET_SCALE_FATES)
        .value("TRINKET_FLARE_HEAVENS", TRINKET_FLARE_HEAVENS)
        .value("TRINKET_SHOW_FAITH", TRINKET_SHOW_FAITH)
        .value("TRINKET_VOLATILE_POWER_HC", TRINKET_VOLATILE_POWER_HC)
        .value("TRINKET_VOLATILE_POWER_NM", TRINKET_VOLATILE_POWER_NM)
        .value("TRINKET_SOLACE_DEFEATED_HC", TRINKET_SOLACE_DEFEATED_HC)
        .value("TRINKET_SOLACE_DEFEATED_NM", TRINKET_SOLACE_DEFEATED_NM)
        .value("TRINKET_REIGN_UNLIVING_HC", TRINKET_REIGN_UNLIVING_HC)
        .value("TRINKET_REIGN_UNLIVING_NM", TRINKET_REIGN_UNLIVING_NM)
        .value("TRINKET_MURADINS_SPYGLASS_HC", TRINKET_MURADINS_SPYGLASS_HC)
        .value("TRINKET_MURADINS_SPYGLASS_NM", TRINKET_MURADINS_SPYGLASS_NM)
        .value("TRINKET_SLIVER_PURE_ICE_HC", TRINKET_SLIVER_PURE_ICE_HC)
        .value("TRINKET_SLIVER_PURE_ICE_NM", TRINKET_SLIVER_PURE_ICE_NM)
        .value("TRINKET_DISLODGED_OBJECT_HC", TRINKET_DISLODGED_OBJECT_HC)
        .value("TRINKET_DISLODGED_OBJECT_NM", TRINKET_DISLODGED_OBJECT_NM)
        .value("TRINKET_NAMELESS_LICH_HC", TRINKET_NAMELESS_LICH_HC)
        .value("TRINKET_NAMELESS_LICH_NM", TRINKET_NAMELESS_LICH_NM)
        ;

    emscripten::enum_<MetaGem>("MetaGem")
        .value("META_NONE", META_NONE)
        .value("META_CHAOTIC_SKYFLARE", META_CHAOTIC_SKYFLARE)
        .value("META_EMBER_SKYFLARE", META_EMBER_SKYFLARE)
        .value("META_BEAMING_EARTHSIEGE", META_BEAMING_EARTHSIEGE)
        .value("META_INSIGHTFUL_EARTHSIEGE", META_INSIGHTFUL_EARTHSIEGE)
        ;

    emscripten::enum_<Potion>("Potion")
        .value("POTION_NONE", POTION_NONE)
        .value("POTION_MANA", POTION_MANA)
        .value("POTION_SPEED", POTION_SPEED)
        .value("POTION_WILD_MAGIC", POTION_WILD_MAGIC);

    emscripten::enum_<Conjured>("Conjured")
        .value("CONJURED_NONE", CONJURED_NONE)
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
        .value("LOG_UNIT", LOG_UNIT)
        ;

    emscripten::class_<Config>("Config")
        .smart_ptr<std::shared_ptr<Config>>("Config")
        .property("duration", &Config::duration)
        .property("duration_variance", &Config::duration_variance)
        .property("rng_seed", &Config::rng_seed)
        .property("avg_spell_dmg", &Config::avg_spell_dmg)
        .property("additional_data", &Config::additional_data)
        .property("targets", &Config::targets)
        .property("target_resistance", &Config::target_resistance)
        .property("target_level", &Config::target_level)
        .property("spell_travel_time", &Config::spell_travel_time)

        .property("debuff_spell_crit", &Config::debuff_spell_crit)
        .property("debuff_spell_dmg", &Config::debuff_spell_dmg)
        .property("debuff_spell_hit", &Config::debuff_spell_hit)
        .property("debuff_crit", &Config::debuff_crit)
        .property("buff_spell_crit", &Config::buff_spell_crit)
        .property("buff_spell_haste", &Config::buff_spell_haste)
        .property("buff_haste", &Config::buff_haste)
        .property("buff_dmg", &Config::buff_dmg)
        .property("totem_of_wrath", &Config::totem_of_wrath)
        .property("flametongue", &Config::flametongue)
        .property("demonic_pact", &Config::demonic_pact)
        .property("demonic_pact_bonus", &Config::demonic_pact_bonus)
        .property("mana_replenishment", &Config::mana_replenishment)
        .property("judgement_of_wisdom", &Config::judgement_of_wisdom)
        .property("mage_armor", &Config::mage_armor)
        .property("molten_armor", &Config::molten_armor)
        .property("prof_engineer", &Config::prof_engineer)

        .property("black_magic", &Config::black_magic)
        .property("lightweave_embroidery", &Config::lightweave_embroidery)
        .property("darkglow_embroidery", &Config::darkglow_embroidery)
        .property("hyperspeed_accelerators", &Config::hyperspeed_accelerators)
        .property("t3_2set", &Config::t3_2set)
        .property("t6_2set", &Config::t6_2set)
        .property("t6_4set", &Config::t6_4set)
        .property("t7_2set", &Config::t7_2set)
        .property("t7_4set", &Config::t7_4set)
        .property("t8_2set", &Config::t8_2set)
        .property("t8_4set", &Config::t8_4set)
        .property("t9_2set", &Config::t9_2set)
        .property("t9_4set", &Config::t9_4set)
        .property("t10_2set", &Config::t10_2set)
        .property("t10_4set", &Config::t10_4set)
        .property("udc_2set", &Config::udc_2set)
        .property("udc_3set", &Config::udc_3set)
        .property("udc_4set", &Config::udc_4set)
        .property("cudc_3set", &Config::cudc_3set)

        .property("pre_mirror_image", &Config::pre_mirror_image)
        .property("pre_water_elemental", &Config::pre_water_elemental)
        .property("pre_incanters_absorption", &Config::pre_incanters_absorption)

        .property("potion", &Config::potion)
        .property("pre_potion", &Config::pre_potion)
        .property("conjured", &Config::conjured)

        .property("trinket1", &Config::trinket1)
        .property("trinket2", &Config::trinket2)
        .property("meta_gem", &Config::meta_gem)

        .property("rotation", &Config::rotation)
        .property("rot_ab3_mana", &Config::rot_ab3_mana)
        .property("rot_ab_no_mb_mana", &Config::rot_ab_no_mb_mana)
        .property("rot_abs_ap", &Config::rot_abs_ap)
        .property("rot_ice_lance", &Config::rot_ice_lance)
        .property("ignite_munching", &Config::ignite_munching)
        .property("hot_streak_cqs", &Config::hot_streak_cqs)
        ;

    emscripten::function("allocConfig", &allocConfig);

    emscripten::function("addTiming", &addTiming);

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
        .field("incanters_absorption", &Talents::incanters_absorption)
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
        ;

    emscripten::function("runSimulation", &runSimulation);
    emscripten::function("runSimulations", &runSimulations);
}

#endif
