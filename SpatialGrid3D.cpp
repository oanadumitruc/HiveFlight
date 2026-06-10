#include "SpatialGrid3D.hpp"

#include "SwarmSimulation3D.hpp" // Drone3D

void SpatialGrid3D::init(double w, double h, double d, double cs) {

    worldWidth = w;
    worldHeight = h;
    worldDepth = d;
    cellSize = cs;

    cols = (cellSize > 0.0) ? static_cast<std::size_t>(worldWidth / cellSize) + 1 : 0;
    rows = (cellSize > 0.0) ? static_cast<std::size_t>(worldHeight / cellSize) + 1 : 0;
    layers = (cellSize > 0.0) ? static_cast<std::size_t>(worldDepth / cellSize) + 1 : 0;

    buckets.clear();
    buckets.resize(cols * rows * layers);
}

void SpatialGrid3D::clear() {
    for (auto& b : buckets) b.clear();
}

std::size_t SpatialGrid3D::bucketIndex(int cx, int cy, int cz) const {
    return static_cast<std::size_t>(cz) * cols * rows
        + static_cast<std::size_t>(cy) * cols
        + static_cast<std::size_t>(cx);
}

void SpatialGrid3D::insert(std::size_t droneIndex, const Drone3D& d, bool wrapAround) {
    if (cols == 0 || rows == 0 || layers == 0) return;

    int cx = static_cast<int>(d.position.x / cellSize);
    int cy = static_cast<int>(d.position.y / cellSize);
    int cz = static_cast<int>(d.position.z / cellSize);

    if (wrapAround) {
        cx = (cx % static_cast<int>(cols) + static_cast<int>(cols)) % static_cast<int>(cols);
        cy = (cy % static_cast<int>(rows) + static_cast<int>(rows)) % static_cast<int>(rows);
        cz = (cz % static_cast<int>(layers) + static_cast<int>(layers)) % static_cast<int>(layers);
    } else {
        if (cx < 0 || cy < 0 || cz < 0) return;
        if (cx >= static_cast<int>(cols) || cy >= static_cast<int>(rows) || cz >= static_cast<int>(layers)) return;
    }

    buckets[bucketIndex(cx, cy, cz)].push_back(droneIndex);
}

