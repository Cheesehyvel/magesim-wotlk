#include "config.h"
#include "player.h"
#include "simulation.h"

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

#ifdef __EMSCRIPTEN__
#include "bindings.h"
#else

namespace
{
    std::vector<SimulationsResult> results;

    void Work(int runs, int duration, int thread_id)
    {
        auto config = std::make_shared<Config>();
        if (duration > 0)
            config->duration = duration;
        auto player = std::make_shared<unit::Player>(config);

        auto start = std::chrono::high_resolution_clock::now();

        Simulation sim(config, player);

        sim.logging = false;
        results[thread_id] = sim.runMultiple(runs);
    }
}

int main(int argc, char **argv)
{
    int runs = 1, duration = 0, thread_count = 1;
    bool log_mana = false;

    auto start = std::chrono::high_resolution_clock::now();

    if (argc > 1)
        runs = atoi(argv[1]);
    if (argc > 2)
        duration = atoi(argv[2]);
    if (argc > 3)
        thread_count = atoi(argv[3]);

    if (thread_count <= 0)
    {
        std::cerr << "Bad threads: " << thread_count << std::endl;
        return 1;
    }

    results.resize(thread_count);

    if (thread_count == 1 || runs < 20)
        Work(runs, duration, 0);
    else
    {
        std::vector<std::thread> threads;

        for (auto i = 0; i < thread_count; ++i)
        {
            // for 10 runs over three threads
            // thread 0: 3 runs
            // thread 1: 3 runs
            // thread 2: 4 runs

            auto thread_runs = runs / thread_count;

            // if this is the last thread, take whatever are left
            if (i == thread_count - 1)
            {
                auto const total = thread_runs * i;
                thread_runs = runs - total;
            }

            threads.emplace_back(Work, thread_runs, duration, i);
        }

        do
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            bool finished = true;
            for (auto const &r : results)
                if (r.iterations == 0)
                {
                    finished = false;
                    break;
                }

            if (finished)
                break;
        } while (true);

        for (auto &t : threads)
            t.join();
    }

    double avg_dps = 0, min_dps = 0, max_dps = 0;

    for (auto const &r : results)
    {
        avg_dps += r.avg_dps;
        min_dps += r.min_dps;
        max_dps += r.max_dps;
    }

    auto stop = std::chrono::high_resolution_clock::now();
    auto exec_time = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

    std::cout << "Threads: " << thread_count << "\nSims: " << runs << "\nDPS: " << avg_dps << " ("
              << min_dps << " - " << max_dps << ")"
              << "\nExec time: " << exec_time.count() << std::endl;

    return 0;
}

#endif