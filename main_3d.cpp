#include <chrono>
#include <thread>
#include <iostream>
#include <cstring>

#include "SwarmSimulation3D.hpp"
#include "Renderer3D.hpp"

int main(int argc, char** argv) {
    SwarmConfig3D cfg;
    std::string exportFormat = "";
    std::string exportFile = "";

    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--drones" && i + 1 < argc) {
            cfg.droneCount = std::atoi(argv[++i]);
        } else if (arg == "--steps" && i + 1 < argc) {
            cfg.dt = 0.016;  // Fixed 60 FPS timestep
            // steps can be controlled via fps and duration
        } else if (arg == "--seed" && i + 1 < argc) {
            cfg.seed = std::atoi(argv[++i]);
        } else if (arg == "--export" && i + 1 < argc) {
            exportFormat = argv[++i];
            if (i + 1 < argc) exportFile = argv[++i];
        } else if (arg == "--help") {
            std::cout << "3D Drone Swarm Simulation\n";
            std::cout << "Usage: drone_swarm_3d [options]\n";
            std::cout << "Options:\n";
            std::cout << "  --drones N          Number of drones (default: 30)\n";
            std::cout << "  --seed S            Random seed (default: 42)\n";
            std::cout << "  --export fmt file   Export to format (obj/csv)\n";
            std::cout << "  --help              Show this help\n";
            return 0;
        }
    }

    SwarmSimulation3D sim(cfg);
    Renderer3D renderer;

    std::cout << "\033[1;35m";
    std::cout << "╔════════════════════════════════════════╗\n";
    std::cout << "║  3D HIVEFLIGHT DRONE SWARM SIMULATION  ║\n";
    std::cout << "║  Reynolds Boids + Obstacles + Target   ║\n";
    std::cout << "╚════════════════════════════════════════╝\n";
    std::cout << "\033[0m";
    std::cout << "Drones: " << cfg.droneCount 
              << "  |  World: " << cfg.worldWidth << "x" << cfg.worldHeight << "x" << cfg.worldDepth
              << "  |  Seed: " << cfg.seed << "\n";
    std::cout << "Framerate: 60 FPS  |  Duration: ~30 seconds\n\n";

    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Run simulation
    const int TOTAL_FRAMES = 1800;  // 30 seconds at 60 FPS
    const int RENDER_EVERY = 2;     // Render every 2 frames
    
    auto startTime = std::chrono::high_resolution_clock::now();

    for (int frame = 0; frame < TOTAL_FRAMES; ++frame) {
        if (frame % RENDER_EVERY == 0) {
            renderer.printConsole(sim.drones(), sim.obstacles(), sim.target(), cfg, frame);
        }

        sim.step();

        // Maintain 60 FPS (16.67 ms per frame)
        auto frameEnd = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(frameEnd - startTime);
        auto targetTime = std::chrono::milliseconds((frame + 1) * 16);
        
        if (elapsed < targetTime) {
            std::this_thread::sleep_for(targetTime - elapsed);
        }
    }

    // Print final statistics
    renderer.printStats(sim.drones(), sim.tick(), sim.time(), sim.obstacles(), cfg);

    // Export if requested
    if (!exportFormat.empty() && !exportFile.empty()) {
        if (exportFormat == "obj") {
            renderer.exportOBJ(sim.drones(), sim.obstacles(), sim.target(), exportFile);
            std::cout << "\n✓ Exported to OBJ: " << exportFile << "\n";
        } else if (exportFormat == "csv") {
            renderer.exportCSV(sim.drones(), exportFile);
            std::cout << "\n✓ Exported to CSV: " << exportFile << "\n";
        }
    }

    std::cout << "\n\033[1;32mSimulation complete!\033[0m\n";
    return 0;
}
