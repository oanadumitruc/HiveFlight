#pragma once

#include "SwarmSimulation3D.hpp"

class OpenGLSwarmViewer {
public:
    explicit OpenGLSwarmViewer(const SwarmConfig3D& cfg);

    int run(int argc, char** argv);

private:
    void display();
    void reshape(int width, int height);
    void keyboard(unsigned char key, int x, int y);
    void special(int key, int x, int y);
    void mouse(int button, int state, int x, int y);
    void motion(int x, int y);
    void timer(int value);

    void drawScene();
    void drawWorldBox() const;
    void drawGrid() const;
    void drawObstacles() const;
    void drawTarget() const;
    void drawDrones() const;
    void drawOverlay() const;

    void setCamera() const;
    void drawText(float x, float y, const char* text) const;
    void drawDroneModel(const Drone3D& drone, float r, float g, float b) const;
    void drawSphere(const Vec3& p, double radius, float r, float g, float b) const;
    void drawLine(const Vec3& a, const Vec3& b, float r, float g, float bcol) const;

    static OpenGLSwarmViewer* instance();
    static void displayCallback();
    static void reshapeCallback(int width, int height);
    static void keyboardCallback(unsigned char key, int x, int y);
    static void specialCallback(int key, int x, int y);
    static void mouseCallback(int button, int state, int x, int y);
    static void motionCallback(int x, int y);
    static void timerCallback(int value);

    SwarmConfig3D m_cfg;
    SwarmSimulation3D m_sim;

    int m_width = 1280;
    int m_height = 720;
    bool m_paused = false;
    bool m_showVelocity = true;
    int m_stepsPerFrame = 3;

    double m_yaw = 42.0;
    double m_pitch = 26.0;
    double m_distance = 330.0;

    bool m_dragging = false;
    int m_lastMouseX = 0;
    int m_lastMouseY = 0;
};
