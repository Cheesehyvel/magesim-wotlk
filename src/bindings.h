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
        .value("DRUMS_OF_FORGOTTEN_KINGS", DRUMS_OF_FORGOTTEN_KINGS)
        .value("DRUMS_OF_THE_WILD", DRUMS_OF_THE_WILD)
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
        .property("buff_kings", &Config::buff_kings)
        .property("buff_dmg", &Config::buff_dmg)
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
        .property("fire_blast_weave", &Config::fire_blast_weave)

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

    emscripten::class_<unit::Player>("Player")
        .smart_ptr<std::shared_ptr<unit::Player>>("Player")
        .property("race", &unit::Player::race)
        .function("getStats", &unit::Player::getStats)
        .function("setStats", &unit::Player::setStats)
        .function("loadTalentsFromString", &unit::Player::loadTalentsFromString);

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
