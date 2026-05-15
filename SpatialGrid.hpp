#pragma once

#include <cstddef>
#include <vector>

// Forward declarations to avoid include cycles.
struct Vec2;
struct DroneState;


struct SpatialGrid {
    // uniform grid for neighbor queries
    double cellSize = 50.0;
    std::size_t cols = 0;
    std::size_t rows = 0;
    double worldWidth = 0.0;
    double worldHeight = 0.0;

    std::vector<std::vector<std::size_t>> buckets;

    void init(double w, double h, double cs);
    void clear();

    std::size_t bucketIndex(int cx, int cy) const;
    void insert(std::size_t droneIndex, const DroneState& d, bool wrapAround);

    // Collect possible neighbor indices around a point (including wrap-around cells if enabled)
    template <typename F>
    void forEachCandidate(const Vec2& p, bool wrapAround, F&& fn) const {
        int cx = static_cast<int>(p.x / cellSize);
        int cy = static_cast<int>(p.y / cellSize);


        for (int oy = -1; oy <= 1; ++oy) {
            for (int ox = -1; ox <= 1; ++ox) {
                int ncx = cx + ox;
                int ncy = cy + oy;

                if (wrapAround) {
                    if (cols > 0) {
                        ncx = (ncx % static_cast<int>(cols) + static_cast<int>(cols)) % static_cast<int>(cols);
                        ncy = (ncy % static_cast<int>(rows) + static_cast<int>(rows)) % static_cast<int>(rows);
                    }
                } else {
                    if (ncx < 0 || ncy < 0) continue;
                    if (ncx >= static_cast<int>(cols) || ncy >= static_cast<int>(rows)) continue;
                }

                if (cols == 0 || rows == 0) continue;

                std::size_t idx = bucketIndex(ncx, ncy);
                for (std::size_t di : buckets[idx]) {
                    fn(di);
                }
            }
        }
    }
};

