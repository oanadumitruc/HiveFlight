#include <chrono>
#include <thread>
#include <iostream>

#include "Config.hpp"
#include "SwarmSimulation.hpp"
#include "ConsoleRenderer.hpp"
#include "PpmRenderer.hpp"

int main(int argc, char** argv) {
    SimConfig cfg = loadConfigFromArgs(argc, argv);
    SwarmSimulation sim(cfg);
    ConsoleRenderer console;
    PpmRenderer ppm(cfg.renderImageW, cfg.renderImageH, cfg.renderFramesDir);

    std::cout << "Starting drone swarm simulation...\n";
    std::cout << "Drones: " << cfg.droneCount << "  dt=" << cfg.dt << "  steps=" << cfg.steps << "\n";
    std::cout << "Render PPM: " << (cfg.renderPpm ? "true" : "false") << "\n";

    for (int step = 0; step < cfg.steps; ++step) {
        console.print(sim.drones(), step);
        if (cfg.renderPpm && (step % cfg.renderEvery == 0)) {
            ppm.render(sim.drones(), step, sim.config());
        }
        sim.step();
        std::this_thread::sleep_for(std::chrono::milliseconds(cfg.sleepMs));
    }

    std::cout << "Simulation complete.\n";
    return 0;
}

