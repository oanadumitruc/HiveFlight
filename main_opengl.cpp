#include <cstdlib>
#include <iostream>
#include <string>

#include "OpenGLSwarmViewer.hpp"

namespace {
void printHelp() {
    std::cout << "HiveFlight OpenGL Swarm Viewer\n"
              << "Usage: hiveflight_gl_viewer [options]\n"
              << "Options:\n"
              << "  --drones N       Number of drones (default: 30)\n"
              << "  --targets N      Number of moving targets (default: 3)\n"
              << "  --seed S         Random seed, use 0 for random_device (default: 42)\n"
              << "  --world W H D    World size (default: 200 200 150)\n"
              << "  --vectors        Start with velocity vectors enabled\n"
              << "  --help           Show this help\n";
}
}

int main(int argc, char** argv) {
    SwarmConfig3D cfg;
    cfg.showVelocityVectors = true;

    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];

        if (arg == "--drones" && i + 1 < argc) {
            cfg.droneCount = static_cast<std::size_t>(std::strtoull(argv[++i], nullptr, 10));
        } else if (arg == "--targets" && i + 1 < argc) {
            cfg.targetCount = static_cast<std::size_t>(std::strtoull(argv[++i], nullptr, 10));
        } else if (arg == "--seed" && i + 1 < argc) {
            cfg.seed = static_cast<unsigned int>(std::strtoul(argv[++i], nullptr, 10));
        } else if (arg == "--world" && i + 3 < argc) {
            cfg.worldWidth = std::atof(argv[++i]);
            cfg.worldHeight = std::atof(argv[++i]);
            cfg.worldDepth = std::atof(argv[++i]);
        } else if (arg == "--vectors") {
            cfg.showVelocityVectors = true;
        } else if (arg == "--help") {
            printHelp();
            return 0;
        } else {
            std::cerr << "Unknown or incomplete option: " << arg << "\n\n";
            printHelp();
            return 1;
        }
    }

    OpenGLSwarmViewer viewer(cfg);
    return viewer.run(argc, argv);
}
