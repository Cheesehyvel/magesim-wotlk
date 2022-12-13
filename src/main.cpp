#include <chrono>
#include <memory>
#include <map>
#include <vector>
#include <string>
#include <list>
#include <math.h>
#include <random>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <thread>
#include <mutex>

#ifdef __EMSCRIPTEN__
#include "bindings.h"
#else

#include "config.h"
#include "player.h"
#include "simulation.h"
#include "stats.h"
#include "talents.h"

static constexpr size_t iterations_per_gear_set = 1000;

namespace
{
struct
{
    double max;
    GearSet gear;
    std::mutex mutex;
    std::atomic_uint64_t sims;
    std::atomic_int finished_threads;
} BulkResults;

void BulkSimWork(const Config& config, const Talents& talents, const Glyphs &glyphs, BulkSimPlan* plan, int thread_id)
{
    do
    {
        auto const work = plan->Consumer(thread_id);

        // if there is nothing for us to do, wait for a bit and try again
        if (work.empty())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        for (auto const& i : work)
        {
            auto player = std::make_shared<unit::Player>(
                config, i.BuildStats(config, talents, glyphs, *plan),
                talents, glyphs);

            Simulation sim(config, player);

            auto const results = sim.runMultiple(iterations_per_gear_set);
            BulkResults.sims += iterations_per_gear_set;

            {
                std::lock_guard<std::mutex> lock(BulkResults.mutex);
                if (results.avg_dps > BulkResults.max)
                {
                    BulkResults.max = results.avg_dps;
                    BulkResults.gear = i;
                    std::cout << "New max DPS: " << BulkResults.max << std::endl;
                }
            }
        }

        // done producing data? end this thread
        if (plan->Done())
            break;
    } while (true);

    ++BulkResults.finished_threads;
}

bool TestGearSetStats()
{
    Glyphs glyphs;

    glyphs.fireball = true;
    glyphs.molten_armor = true;
    glyphs.living_bomb = true;
    glyphs.blast_wave = true;

    auto const talents = talents_from_url(
        "https://www.wowhead.com/wotlk/talent-calc/mage/23000503110003-0055030012302331053120311351_001q1g11q1y21xkk31wne41rj451rj5"
    );

    // frost... nothing

    Config config;

    config.race = RACE_TROLL;
    config.duration = 120;
    config.duration_variance = 10;
    config.rng_seed = 46;

    config.debuff_spell_crit = true;
    config.debuff_spell_dmg = true;
    config.debuff_spell_hit = true;
    config.debuff_crit = true;

    config.buff_spell_crit = true;
    config.buff_spell_haste = true;
    config.buff_haste = true;
    config.buff_dmg = true;

    config.demonic_pact = true;
    config.demonic_pact_bonus = 280;

    config.judgement_of_wisdom = true;
    config.mage_armor = false;
    config.molten_armor = true;
    config.prof_engineer = true;

    config.pre_mirror_image = true;
    config.pre_potion = POTION_SPEED;

    config.potion = POTION_SPEED;
    config.conjured = CONJURED_FLAME_CAP;

    config.rotation = ROTATION_ST_FIRE;
    config.ignite_munching = true;
    config.hot_streak_cqs = true;
    config.hot_streak_cqs_time = 100;
    config.evo_ticks = 3;

    config.timings.emplace_back("berserking", 2, 0, 0);
    config.timings.emplace_back("bloodlust", 2, 0, 0);
    config.timings.emplace_back("combustion", 2, 0, 0);
    config.timings.emplace_back("conjured", 2, 0, 0);
    config.timings.emplace_back("evocation", 40, 0, 0);
    config.timings.emplace_back("hyperspeed_accelerators", 2, 0, 0);
    config.timings.emplace_back("hyperspeed_accelerators", 62, 0, 0);
    config.timings.emplace_back("mana_gem", 15, 0, 0);
    config.timings.emplace_back("potion", 62, 0, 0);
    config.timings.emplace_back("trinket1", 2, 0, 0);

    const std::array<std::vector<int>, BulkSimPlan::slots> items{
        std::vector<int>{45085},
        std::vector<int>{40698},
        std::vector<int>{37177},
        std::vector<int>{42553},
        std::vector<int>{39472},
        std::vector<int>{34210},
        std::vector<int>{41610},
        std::vector<int>{39492},
        std::vector<int>{37361},
        std::vector<int>{39495},
        std::vector<int>{40696},
        std::vector<int>{37854},
        std::vector<int>{44202},
        std::vector<int>{40585, 42644},
        std::vector<int>{37873, 40682}
    };

    const std::vector<int> gems{ 41285, 40014, 40049, 40026, 40099 };

    BulkSimPlan plan{ 1u, items, {}, gems};

    GearSet gear{ {45085,40698, 37177, 42553, 39472, 34210, 41610, 39492, 37361, 39495, 40696, 37854, 44202, 40585, 42644, 37873, 40682 } };

    gear.gems[3] = { 41285, 40014 };
    gear.gems[5] = { 40049, 40014 };
    gear.gems[7] = { 40049, 40014 };
    gear.gems[9] = { 40049 };
    gear.gems[10] = { 40014, 40026 };
    gear.gems[12] = { 40099 };
    gear.gems[14] = { 40049 };

    auto const stats = gear.BuildStats(config, talents, glyphs, plan);

    auto player = std::make_shared<unit::Player>(config, stats, talents, glyphs);

    Simulation sim(config, player);

    auto const result = sim.run(true);

    return false;
}
}

int main(int argc, char** argv)
{
    size_t thread_count = 16;


    auto start = std::chrono::high_resolution_clock::now();

    if (argc > 1)
        thread_count = static_cast<size_t>(atoi(argv[1]));

    if (thread_count <= 0)
    {
        std::cerr << "Bad threads: " << thread_count << std::endl;
        return 1;
    }

    if (!TestGearSetStats())
    {
        std::cerr << "WARNING: Sanity test is not correct" << std::endl;
        //return 1;
    }

    BulkResults.max = 0;

    try
    {
        Config config;
        Talents talents;
        Glyphs glyphs;

        auto plan = initialize_from_json("c:\\users\\bill\\source\\repos\\magesim-wotlk\\config_batch.json",
            config, talents, glyphs, thread_count);

        std::cout << "Maximum permutations: " << plan.Permutations(config.regem) << std::endl;

        std::vector<std::thread> threads;
        threads.reserve(thread_count + 1);

        for (auto i = 0; i < thread_count; ++i)
            threads.emplace_back(BulkSimWork, config, talents, glyphs, &plan, i);

        threads.emplace_back(&BulkSimPlan::Producer, &plan);

        auto last_status = std::chrono::high_resolution_clock::now();

        bool first_iteration_seen = false;
        std::chrono::steady_clock::time_point first_iteration;

        do
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            if (BulkResults.finished_threads.load() == thread_count)
                break;

            if (!first_iteration_seen && !BulkResults.sims)
                continue;
            else if (BulkResults.sims > 0)
            {
                first_iteration_seen = true;
                first_iteration = std::chrono::high_resolution_clock::now();
            }

            auto const now = std::chrono::high_resolution_clock::now();
            if (std::chrono::duration_cast<std::chrono::milliseconds>(now - last_status).count() > 5000)
            {
                auto const runtime = std::chrono::duration_cast<std::chrono::seconds>(now - start).count();
                auto const iterations_per_sec = BulkResults.sims / runtime;
                auto const total_planned = iterations_per_gear_set * plan.Permutations(config.regem);
                auto const percent_complete = (100 * BulkResults.sims) / total_planned;
                auto const time_remaining = (total_planned - BulkResults.sims) / iterations_per_sec;

                std::cout
                    << "Iterations: " << BulkResults.sims
                    << " in " << runtime
                    << " seconds.  Iterations per second: " << iterations_per_sec
                    << " Total planned: " << total_planned
                    << " Percent complete: " << percent_complete
                    << " Seconds remaining: " << time_remaining
                    << std::endl;
                last_status = now;
            }
        } while (true);

        for (auto& t : threads)
            t.join();

        auto stop = std::chrono::high_resolution_clock::now();
        auto exec_time = std::chrono::duration_cast<std::chrono::seconds>(stop - start);

        std::cout << "Finished in " << exec_time.count() << " seconds" << std::endl;

        for (auto const& i : BulkResults.gear.items)
            std::cout << i << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

#endif