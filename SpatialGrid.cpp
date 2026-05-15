#include "SpatialGrid.hpp"

#include "SwarmSimulation.hpp" // Vec2, DroneState

// Template implementation lives in the header by default.
// This grid is only used for Vec2/DroneState in this project, so we provide
// an explicit non-templated helper and call sites are updated accordingly.

void SpatialGrid::init(double w, double h, double cs) {
    worldWidth = w;
    worldHeight = h;
    cellSize = cs;

    cols = (cellSize > 0.0) ? static_cast<std::size_t>(worldWidth / cellSize) + 1 : 0;
    rows = (cellSize > 0.0) ? static_cast<std::size_t>(worldHeight / cellSize) + 1 : 0;

    buckets.clear();
    buckets.resize(cols * rows);
}

void SpatialGrid::clear() {
    for (auto& b : buckets) b.clear();
}

std::size_t SpatialGrid::bucketIndex(int cx, int cy) const {
    return static_cast<std::size_t>(cy) * cols + static_cast<std::size_t>(cx);
}

void SpatialGrid::insert(std::size_t droneIndex, const DroneState& d, bool wrapAround) {
    if (cols == 0 || rows == 0) return;

    int cx = static_cast<int>(d.position.x / cellSize);
    int cy = static_cast<int>(d.position.y / cellSize);

    if (wrapAround) {
        cx = (cx % static_cast<int>(cols) + static_cast<int>(cols)) % static_cast<int>(cols);
        cy = (cy % static_cast<int>(rows) + static_cast<int>(rows)) % static_cast<int>(rows);
    } else {
        if (cx < 0 || cy < 0) return;
        if (cx >= static_cast<int>(cols) || cy >= static_cast<int>(rows)) return;
    }

    buckets[bucketIndex(cx, cy)].push_back(droneIndex);
}

