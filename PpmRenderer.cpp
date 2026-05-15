#include "PpmRenderer.hpp"

#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>

#include <cstdio>

#ifdef _WIN32
#include <direct.h>
#endif

static void ensureDir(const std::string& dir) {
#ifdef _WIN32
    _mkdir(dir.c_str());
#else
    // best-effort; if it fails it's fine (caller will see missing output)
    std::string cmd = "mkdir -p " + dir;
    std::system(cmd.c_str());
#endif
}

PpmRenderer::PpmRenderer(int imageW, int imageH, std::string framesDir)
    : m_imageW(imageW), m_imageH(imageH), m_framesDir(std::move(framesDir)) {
    ensureDir(m_framesDir);
}

bool PpmRenderer::writeTextFile(const std::string& path, const std::string& contents) {
    std::ofstream out(path, std::ios::binary);
    if (!out.is_open()) return false;
    out << contents;
    return true;
}

void PpmRenderer::render(const std::vector<DroneState>& drones, int step, const SimConfig& cfg) const {
    // PPM P3 (ASCII) for maximum portability.
    // For speed/size you could switch to P6 (binary).

    const int W = m_imageW;
    const int H = m_imageH;

    // white background
    std::vector<unsigned char> img(3 * static_cast<std::size_t>(W) * static_cast<std::size_t>(H), 255);

    auto setPixel = [&](int x, int y, unsigned char r, unsigned char g, unsigned char b) {
        if (x < 0 || y < 0 || x >= W || y >= H) return;
        std::size_t idx = (static_cast<std::size_t>(y) * static_cast<std::size_t>(W) + static_cast<std::size_t>(x)) * 3;
        img[idx + 0] = r;
        img[idx + 1] = g;
        img[idx + 2] = b;
    };

    // Simple drawing: each drone as a small filled circle.
    const int radius = 3;

    for (const auto& d : drones) {
        double nx = (cfg.worldWidth > 0.0) ? (d.position.x / cfg.worldWidth) : 0.0;
        double ny = (cfg.worldHeight > 0.0) ? (d.position.y / cfg.worldHeight) : 0.0;

        int px = static_cast<int>(nx * (W - 1));
        int py = static_cast<int>((1.0 - ny) * (H - 1)); // flip y for image coordinates

        // color based on id
        unsigned char r = static_cast<unsigned char>(40 + (d.id * 37) % 215);
        unsigned char g = static_cast<unsigned char>(40 + (d.id * 67) % 215);
        unsigned char b = static_cast<unsigned char>(40 + (d.id * 97) % 215);

        for (int oy = -radius; oy <= radius; ++oy) {
            for (int ox = -radius; ox <= radius; ++ox) {
                if (ox * ox + oy * oy <= radius * radius) {
                    setPixel(px + ox, py + oy, r, g, b);
                }
            }
        }
    }

    // Write PPM header + pixels (P3 ASCII)
    std::ostringstream ss;
    ss << "P3\n" << W << " " << H << "\n" << 255 << "\n";

    // Convert to ASCII RGB triplets
    // Note: This is not the fastest method; OK for moderate drone counts/frames.
    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            std::size_t idx = (static_cast<std::size_t>(y) * static_cast<std::size_t>(W) + static_cast<std::size_t>(x)) * 3;
            ss << static_cast<int>(img[idx + 0]) << ' '
               << static_cast<int>(img[idx + 1]) << ' '
               << static_cast<int>(img[idx + 2]) << ' ';
        }
        ss << '\n';
    }

    std::ostringstream fname;
    fname << m_framesDir << "/frame_" << std::setfill('0') << std::setw(5) << step << ".ppm";

    writeTextFile(fname.str(), ss.str());
}

