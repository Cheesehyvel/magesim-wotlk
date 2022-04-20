#include <cstdio>
#include <cstdlib>
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

#include "common.h"
#include "timing.h"
#include "interruption.h"
#include "config.h"
#include "stats.h"
#include "talents.h"
#include "buff.h"
#include "debuff.h"
#include "cooldown.h"
#include "spell.h"
#include "action.h"
#include "unit.h"
#include "state.h"
#include "mirror_image.h"
#include "water_elemental.h"
#include "player.h"
#include "log.h"
#include "simulation.h"
#include "bindings.h"

#ifndef __EMSCRIPTEN__

using namespace std;

int main(int argc, char **argv)
{
    int runs = 1;
    bool log_mana = false;

    shared_ptr<Config> config(new Config());
    shared_ptr<Player> player(new Player(config));
    player->quickReady();
    shared_ptr<Simulation> sim(new Simulation(config, player));

    if (argc > 1)
        runs = atoi(argv[1]);
    if (argc > 2)
        sim->config->duration = atoi(argv[2]);

    auto start = chrono::high_resolution_clock::now();

    if (runs < 2) {
        sim->logging = true;
        SimulationResult result = sim->run();
        sim->printLog();

        printf("Damage: %d\n", result.dmg);
        printf("DPS: %.2f\n", result.dps);
    }
    else {
        sim->logging = false;
        SimulationsResult result = sim->runMultiple(runs);

        printf("Sims: %d\n", runs);
        printf("DPS: %.2f (%.2f - %.2f)\n", result.avg_dps, result.min_dps, result.max_dps);
    }

    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);
    printf("Exec time: %ldms\n", duration.count());

    return 0;
}

#endif