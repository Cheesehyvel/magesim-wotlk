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

#ifndef __EMSCRIPTEN__

using namespace std;

int main(int argc, char **argv)
{
    int runs = 1;
    bool log_mana = false;

    auto config = std::make_shared<Config>();
    auto player = std::make_shared<unit::Player>(config);

    if (argc > 1)
        runs = atoi(argv[1]);
    if (argc > 2)
        config->duration = atoi(argv[2]);

    Simulation sim(config, player);

    auto start = chrono::high_resolution_clock::now();

    if (runs < 2) {
        sim.logging = true;
        SimulationResult result = sim.run();
        
        std::cout << sim.getLog() << "Damage: " << result.dmg << "\nDPS: " << result.dps << std::endl;
    }
    else {
        sim.logging = false;
        SimulationsResult result = sim.runMultiple(runs);

        std::cout << "Sims: " << runs << "\nDPS: " << result.avg_dps << " ("
            << result.min_dps << " - " << result.max_dps << ")" << std::endl;
    }

    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);
    std::cout << "Exec time: " << duration.count() << std::endl;

    return 0;
}

#endif