#pragma once

#include <string>
#include <vector>

#include "SwarmSimulation.hpp"

// Minimal visualization output: writes one PPM image per simulation step.
// These images can be converted to a GIF/video externally.
class PpmRenderer {
public:
    // imageW/H are pixel dimensions for the output.
    // framesDir is where frame_00000.ppm etc are written.
    PpmRenderer(int imageW = 800, int imageH = 600, std::string framesDir = "frames");

    void render(const std::vector<DroneState>& drones, int step, const SimConfig& cfg) const;

private:
    int m_imageW;
    int m_imageH;
    std::string m_framesDir;

    static bool writeTextFile(const std::string& path, const std::string& contents);
};

