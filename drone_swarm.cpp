#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <thread>
#include <chrono>

// Simple 2D vector class
struct Vector2D {
    double x, y;
    Vector2D(double x = 0, double y = 0) : x(x), y(y) {}
    Vector2D operator+(const Vector2D& other) const { return Vector2D(x + other.x, y + other.y); }
    Vector2D operator-(const Vector2D& other) const { return Vector2D(x - other.x, y - other.y); }
    Vector2D operator*(double scalar) const { return Vector2D(x * scalar, y * scalar); }
    Vector2D operator/(double scalar) const { return Vector2D(x / scalar, y / scalar); }
    double magnitude() const { return std::sqrt(x*x + y*y); }
    Vector2D normalized() const { double mag = magnitude(); return mag > 0 ? *this / mag : Vector2D(0, 0); }
};

class Drone {
public:
    Vector2D position;
    Vector2D velocity;
    int id;

    Drone(int id, Vector2D pos, Vector2D vel) : id(id), position(pos), velocity(vel) {}

    void update(const std::vector<Drone>& drones, double dt) {
        Vector2D separation = calculateSeparation(drones);
        Vector2D alignment = calculateAlignment(drones);
        Vector2D cohesion = calculateCohesion(drones);

        // Combine forces with weights
        Vector2D force = separation * 1.5 + alignment * 1.0 + cohesion * 1.0;

        // Update velocity
        velocity = velocity + force * dt;

        // Limit speed
        double maxSpeed = 50.0;
        if (velocity.magnitude() > maxSpeed) {
            velocity = velocity.normalized() * maxSpeed;
        }

        // Update position
        position = position + velocity * dt;

        // Wrap around boundaries (assuming 800x600 world)
        if (position.x < 0) position.x = 800;
        if (position.x > 800) position.x = 0;
        if (position.y < 0) position.y = 600;
        if (position.y > 600) position.y = 0;
    }

private:
    Vector2D calculateSeparation(const std::vector<Drone>& drones) {
        Vector2D steer(0, 0);
        int count = 0;
        double desiredSeparation = 30.0;

        for (const auto& other : drones) {
            double distance = (position - other.position).magnitude();
            if (distance > 0 && distance < desiredSeparation) {
                Vector2D diff = (position - other.position).normalized() / distance;
                steer = steer + diff;
                count++;
            }
        }

        if (count > 0) {
            steer = steer / count;
            steer = steer.normalized() * 50.0 - velocity;
        }

        return steer;
    }

    Vector2D calculateAlignment(const std::vector<Drone>& drones) {
        Vector2D sum(0, 0);
        int count = 0;
        double neighborDist = 50.0;

        for (const auto& other : drones) {
            double distance = (position - other.position).magnitude();
            if (distance > 0 && distance < neighborDist) {
                sum = sum + other.velocity;
                count++;
            }
        }

        if (count > 0) {
            sum = sum / count;
            sum = sum.normalized() * 50.0;
            return sum - velocity;
        }

        return Vector2D(0, 0);
    }

    Vector2D calculateCohesion(const std::vector<Drone>& drones) {
        Vector2D sum(0, 0);
        int count = 0;
        double neighborDist = 50.0;

        for (const auto& other : drones) {
            double distance = (position - other.position).magnitude();
            if (distance > 0 && distance < neighborDist) {
                sum = sum + other.position;
                count++;
                
            }
        }

        if (count > 0) {
            sum = sum / count;
            return seek(sum) - velocity;
        }

        return Vector2D(0, 0);
    }

    Vector2D seek(Vector2D target) {
        Vector2D desired = target - position;
        desired = desired.normalized() * 50.0;
        return desired - velocity;
    }
};

void printSwarm(const std::vector<Drone>& drones, int step) {
    std::cout << "Step " << step << ":" << std::endl;
    for (const auto& drone : drones) {
        std::cout << "Drone " << drone.id << ": (" << drone.position.x << ", " << drone.position.y << ") Vel: (" << drone.velocity.x << ", " << drone.velocity.y << ")" << std::endl;
    }
    std::cout << std::endl;
}

int main() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> posDist(100, 700);
    std::uniform_real_distribution<> velDist(-20, 20);

    std::vector<Drone> drones;
    for (int i = 0; i < 5; ++i) {
        Vector2D pos(posDist(gen), posDist(gen));
        Vector2D vel(velDist(gen), velDist(gen));
        drones.emplace_back(i + 1, pos, vel);
    }

    double dt = 0.1;
    int steps = 100;

    for (int step = 0; step < steps; ++step) {
        printSwarm(drones, step);

        for (auto& drone : drones) {
            drone.update(drones, dt);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    return 0;
}
