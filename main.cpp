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

    std::cout << "\033[1;35m";
    std::cout << "╔════════════════════════════════════════╗\n";
    std::cout << "║    HIVEFLIGHT DRONE SWARM SIMULATION   ║\n";
    std::cout << "║   Algorithm: Reynolds Boids + Target   ║\n";
    std::cout << "╚════════════════════════════════════════╝\n";
    std::cout << "\033[0m";
    std::cout << "Drones: " << cfg.droneCount << "  |  dt=" << cfg.dt 
              << "  |  steps=" << cfg.steps << "  |  seed=" << cfg.seed << "\n";
    std::cout << "World: " << cfg.worldWidth << "x" << cfg.worldHeight << "\n";
    std::cout << "Render PPM: " << (cfg.renderPpm ? "true" : "false") << "\n\n";
    std::this_thread::sleep_for(std::chrono::seconds(1));

    for (int step = 0; step < cfg.steps; ++step) {
        if (step % 2 == 0) {
            console.print(sim.drones(), sim.obstacles(), sim.target(), 
                         step, sim.time(), cfg);
        }
        
        if (cfg.renderPpm && (step % cfg.renderEvery == 0)) {
            ppm.render(sim.drones(), step, sim.config());
        }
        
        sim.step();
        std::this_thread::sleep_for(std::chrono::milliseconds(cfg.sleepMs));
    }

    console.printStats(sim.drones(), sim.tick(), sim.time(), sim.obstacles());
    std::cout << "\n\033[1;32mSimulation complete!\033[0m\n";
    return 0;
}

