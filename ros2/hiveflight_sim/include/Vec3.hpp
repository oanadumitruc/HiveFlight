#pragma once

#include <cmath>

struct Vec3 {
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;

    Vec3() = default;
    Vec3(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {}

    // Operators
    Vec3 operator+(const Vec3& o) const { return Vec3(x + o.x, y + o.y, z + o.z); }
    Vec3 operator-(const Vec3& o) const { return Vec3(x - o.x, y - o.y, z - o.z); }
    Vec3 operator*(double s) const { return Vec3(x * s, y * s, z * s); }
    Vec3 operator/(double s) const { return Vec3(x / s, y / s, z / s); }
    
    Vec3& operator+=(const Vec3& o) { x += o.x; y += o.y; z += o.z; return *this; }
    Vec3& operator-=(const Vec3& o) { x -= o.x; y -= o.y; z -= o.z; return *this; }
    Vec3& operator*=(double s) { x *= s; y *= s; z *= s; return *this; }

    // Vector operations
    double magnitude() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    Vec3 normalized() const {
        double mag = magnitude();
        if (mag > 1e-9) return *this / mag;
        return Vec3(0.0, 0.0, 0.0);
    }

    Vec3 limit(double maxLen) const {
        double mag = magnitude();
        if (mag > maxLen) {
            return normalized() * maxLen;
        }
        return *this;
    }

    double dot(const Vec3& o) const {
        return x * o.x + y * o.y + z * o.z;
    }

    Vec3 cross(const Vec3& o) const {
        return Vec3(
            y * o.z - z * o.y,
            z * o.x - x * o.z,
            x * o.y - y * o.x
        );
    }

    double distance(const Vec3& o) const {
        return (*this - o).magnitude();
    }
};

inline Vec3 operator*(double s, const Vec3& v) { return v * s; }
