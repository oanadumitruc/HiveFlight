#pragma once

#include <cstddef>
#include <vector>

#include "Vec3.hpp"

struct Drone3D;

// 3D uniform grid for neighbor queries in (x,y,z).
struct SpatialGrid3D {
    double cellSize = 50.0;

    std::size_t cols = 0;
    std::size_t rows = 0;
    std::size_t layers = 0;

    double worldWidth = 0.0;
    double worldHeight = 0.0;
    double worldDepth = 0.0;

    // buckets[idx] contains drone indices
    std::vector<std::vector<std::size_t>> buckets;

    void init(double w, double h, double d, double cs);
    void clear();

    std::size_t bucketIndex(int cx, int cy, int cz) const;

    void insert(std::size_t droneIndex, const Drone3D& d, bool wrapAround);

    // Collect possible neighbor indices around a point (including wrap-around cells if enabled)
    template <typename F>
    void forEachCandidate(const Vec3& p, bool wrapAround, F&& fn) const {
        if (cols == 0 || rows == 0 || layers == 0) return;

        int cx = static_cast<int>(p.x / cellSize);
        int cy = static_cast<int>(p.y / cellSize);
        int cz = static_cast<int>(p.z / cellSize);

        for (int oz = -1; oz <= 1; ++oz) {
            for (int oy = -1; oy <= 1; ++oy) {
                for (int ox = -1; ox <= 1; ++ox) {
                    int ncx = cx + ox;
                    int ncy = cy + oy;
                    int ncz = cz + oz;

                    if (wrapAround) {
                        ncx = (ncx % static_cast<int>(cols) + static_cast<int>(cols)) % static_cast<int>(cols);
                        ncy = (ncy % static_cast<int>(rows) + static_cast<int>(rows)) % static_cast<int>(rows);
                        ncz = (ncz % static_cast<int>(layers) + static_cast<int>(layers)) % static_cast<int>(layers);
                    } else {
                        if (ncx < 0 || ncy < 0 || ncz < 0) continue;
                        if (ncx >= static_cast<int>(cols) || ncy >= static_cast<int>(rows) || ncz >= static_cast<int>(layers)) continue;
                    }

                    std::size_t idx = bucketIndex(ncx, ncy, ncz);
                    for (std::size_t di : buckets[idx]) {
                        fn(di);
                    }
                }
            }
        }
    }
};

