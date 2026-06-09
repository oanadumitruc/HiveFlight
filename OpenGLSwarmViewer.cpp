#include "OpenGLSwarmViewer.hpp"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cmath>

using namespace std;

#if defined(_WIN32) || defined(_WIN64)
#define NOMINMAX
#define _HAS_STD_BYTE 0
//#include <windows.h>
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

namespace {
OpenGLSwarmViewer* g_viewer = nullptr;

constexpr double kPi = 3.14159265358979323846;
constexpr double kRadToDeg = 180.0 / kPi;

static const float kTargetColors[6][3] = {
    {0.96f, 0.57f, 0.21f},  // amber
    {0.16f, 0.77f, 0.68f},  // teal
    {0.60f, 0.40f, 0.92f},  // violet
    {0.95f, 0.24f, 0.36f},  // coral
    {0.30f, 0.72f, 0.94f},  // sky
    {0.88f, 0.88f, 0.26f}   // lemon
};

double toRadians(double degrees) {
    return degrees * kPi / 180.0;
}

void setMaterial(float r, float g, float b, float alpha = 1.0f) {
    const GLfloat diffuse[] = {r, g, b, alpha};
    const GLfloat specular[] = {0.55f, 0.55f, 0.55f, alpha};
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 40.0f);
    glColor4f(r, g, b, alpha);
}
}

OpenGLSwarmViewer::OpenGLSwarmViewer(const SwarmConfig3D& cfg)
    : m_cfg(cfg), m_sim(cfg) {
    m_showVelocity = cfg.showVelocityVectors;
}

int OpenGLSwarmViewer::run(int argc, char** argv) {
    g_viewer = this;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowSize(m_width, m_height);
    glutCreateWindow("HiveFlight OpenGL Swarm Viewer");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);

    const GLfloat lightPosition[] = {120.0f, 220.0f, 180.0f, 1.0f};
    const GLfloat lightAmbient[] = {0.18f, 0.18f, 0.20f, 1.0f};
    const GLfloat lightDiffuse[] = {0.92f, 0.92f, 0.88f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);

    glClearColor(0.04f, 0.055f, 0.07f, 1.0f);

    glutDisplayFunc(displayCallback);
    glutReshapeFunc(reshapeCallback);
    glutKeyboardFunc(keyboardCallback);
    glutSpecialFunc(specialCallback);
    glutMouseFunc(mouseCallback);
    glutMotionFunc(motionCallback);
    glutTimerFunc(16, timerCallback, 0);

    glutMainLoop();
    return 0;
}

void OpenGLSwarmViewer::display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Basic fixed-function sanity: ensure we're drawing with an active model-view/projection.
    // Some GLUT/driver combinations can leave matrices in an unexpected state between frames.
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);

    setCamera();
    drawScene();
    drawOverlay();
    glutSwapBuffers();
}


void OpenGLSwarmViewer::reshape(int width, int height) {
    m_width = max(1, width);
    m_height = max(1, height);
    glViewport(0, 0, m_width, m_height);
}

void OpenGLSwarmViewer::keyboard(unsigned char key, int, int) {
    switch (key) {
    case 27:
    case 'q':
    case 'Q':
        std::exit(0);
        break;
    case ' ':
        m_paused = !m_paused;
        break;
    case 'r':
    case 'R':
        m_sim.reset();
        break;
    case 'v':
    case 'V':
        m_showVelocity = !m_showVelocity;
        break;
    case '+':
    case '=':
        m_stepsPerFrame = min(16, m_stepsPerFrame + 1);
        break;
    case '-':
    case '_':
        m_stepsPerFrame = max(1, m_stepsPerFrame - 1);
        break;
    case '0':
        m_yaw = 42.0;
        m_pitch = 26.0;
        m_distance = 330.0;
        break;
    default:
        break;
    }
    glutPostRedisplay();
}

void OpenGLSwarmViewer::special(int key, int, int) {
    switch (key) {
    case GLUT_KEY_LEFT:
        m_yaw -= 4.0;
        break;
    case GLUT_KEY_RIGHT:
        m_yaw += 4.0;
        break;
    case GLUT_KEY_UP:
        m_pitch = min(85.0, m_pitch + 3.0);
        break;
    case GLUT_KEY_DOWN:
        m_pitch = max(-20.0, m_pitch - 3.0);
        break;
    default:
        break;
    }
    glutPostRedisplay();
}

void OpenGLSwarmViewer::mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        m_dragging = state == GLUT_DOWN;
        m_lastMouseX = x;
        m_lastMouseY = y;
    } else if (button == 3 && state == GLUT_DOWN) {
        m_distance = max(80.0, m_distance - 15.0);
    } else if (button == 4 && state == GLUT_DOWN) {
        m_distance = min(700.0, m_distance + 15.0);
    }
    glutPostRedisplay();
}

void OpenGLSwarmViewer::motion(int x, int y) {
    if (!m_dragging) return;

    const int dx = x - m_lastMouseX;
    const int dy = y - m_lastMouseY;
    m_yaw += dx * 0.35;
    m_pitch = std::clamp(m_pitch + dy * 0.25, -20.0, 85.0);
    m_lastMouseX = x;
    m_lastMouseY = y;
    glutPostRedisplay();
}

void OpenGLSwarmViewer::timer(int) {
    if (!m_paused) {
        for (int i = 0; i < m_stepsPerFrame; ++i) {
            m_sim.step();
        }
    }

    // Force a redisplay and also detect common “rendering nothing” cases.
    // If all drones lose health immediately, nothing will be drawn.
    static int s_printEvery = 60;
    if ((m_sim.tick() % s_printEvery) == 0) {
        const auto& drones = m_sim.drones();
        std::size_t alive = 0;
        for (const auto& d : drones) if (d.health > 0.0) ++alive;
        std::fprintf(stderr,
            "[HiveFlight OpenGL] tick=%d time=%.2fs drones=%zu alive=%zu targets=%zu\n",
            m_sim.tick(), m_sim.time(), drones.size(), alive, m_sim.targets().size());
        std::fflush(stderr);

        // After a first few prints, reduce spam.
        if (s_printEvery > 240) s_printEvery = 240;
    }

    glutPostRedisplay();
    glutTimerFunc(16, timerCallback, 0);
}


void OpenGLSwarmViewer::drawScene() {
    drawGrid();
    drawWorldBox();
    drawObstacles();
    drawTarget();
    drawDrones();
}

void OpenGLSwarmViewer::setCamera() const {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, static_cast<double>(m_width) / static_cast<double>(m_height), 1.0, 1600.0);

    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();

    const Vec3 center(m_cfg.worldWidth * 0.5, m_cfg.worldHeight * 0.5, m_cfg.worldDepth * 0.5);
    const double yaw = toRadians(m_yaw);
    const double pitch = toRadians(m_pitch);
    const double cp = std::cos(pitch);
    const Vec3 eye(
        center.x + m_distance * cp * std::sin(yaw),
        center.y + m_distance * std::sin(pitch),
        center.z + m_distance * cp * std::cos(yaw));

    gluLookAt(eye.x, eye.y, eye.z,
              center.x, center.y, center.z,
              0.0, 1.0, 0.0);
}

void OpenGLSwarmViewer::drawWorldBox() const {
    glDisable(GL_LIGHTING);
    glColor3f(0.45f, 0.56f, 0.66f);
    glLineWidth(1.3f);

    const double w = m_cfg.worldWidth;
    const double h = m_cfg.worldHeight;
    const double d = m_cfg.worldDepth;

    glBegin(GL_LINES);
    const double xs[] = {0.0, w};
    const double ys[] = {0.0, h};
    const double zs[] = {0.0, d};

    for (double y : ys) {
        for (double z : zs) {
            glVertex3d(0.0, y, z);
            glVertex3d(w, y, z);
        }
    }
    for (double x : xs) {
        for (double z : zs) {
            glVertex3d(x, 0.0, z);
            glVertex3d(x, h, z);
        }
    }
    for (double x : xs) {
        for (double y : ys) {
            glVertex3d(x, y, 0.0);
            glVertex3d(x, y, d);
        }
    }
    glEnd();

    glLineWidth(1.0f);
    glEnable(GL_LIGHTING);
}

void OpenGLSwarmViewer::drawGrid() const {
    glDisable(GL_LIGHTING);
    glColor3f(0.16f, 0.21f, 0.24f);
    glLineWidth(1.0f);

    const double step = 20.0;
    glBegin(GL_LINES);
    for (double x = 0.0; x <= m_cfg.worldWidth + 0.01; x += step) {
        glVertex3d(x, 0.0, 0.0);
        glVertex3d(x, 0.0, m_cfg.worldDepth);
    }
    for (double z = 0.0; z <= m_cfg.worldDepth + 0.01; z += step) {
        glVertex3d(0.0, 0.0, z);
        glVertex3d(m_cfg.worldWidth, 0.0, z);
    }
    glEnd();

    glEnable(GL_LIGHTING);
}

void OpenGLSwarmViewer::drawObstacles() const {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    setMaterial(0.85f, 0.30f, 0.24f, 0.55f);

    for (const auto& obs : m_sim.obstacles()) {
        glPushMatrix();
        glTranslated(obs.position.x, obs.position.y, obs.position.z);
        glutSolidSphere(obs.radius, 32, 16);
        glPopMatrix();
    }

    glDisable(GL_BLEND);
}

void OpenGLSwarmViewer::drawTarget() const {
    const auto& targets = m_sim.targets();
    if (targets.empty()) {
        return;
    }
    for (std::size_t idx = 0; idx < targets.size(); ++idx) {
        const auto& target = targets[idx];
        const auto& color = kTargetColors[idx % 6];
        drawSphere(target, 3.4, color[0], color[1], color[2]);
        drawTargetLabel(target, static_cast<int>(idx + 1));
    }

    glDisable(GL_LIGHTING);
    glLineWidth(2.2f);
    glColor3f(0.95f, 0.82f, 0.20f);
    glBegin(GL_LINES);
    for (const auto& target : targets) {
        glVertex3d(target.x - 7.0, target.y, target.z);
        glVertex3d(target.x + 7.0, target.y, target.z);
        glVertex3d(target.x, target.y - 7.0, target.z);
        glVertex3d(target.x, target.y + 7.0, target.z);
        glVertex3d(target.x, target.y, target.z - 7.0);
        glVertex3d(target.x, target.y, target.z + 7.0);
    }
    glEnd();
    glLineWidth(1.0f);
    glEnable(GL_LIGHTING);
}

void OpenGLSwarmViewer::drawDrones() const {
    for (std::size_t i = 0; i < m_sim.drones().size(); ++i) {
        const auto& drone = m_sim.drones()[i];
        if (drone.health <= 0.0) continue;

        const std::size_t tIdx = m_sim.assignedTarget(i);
        const auto& base = kTargetColors[tIdx % 6];

        const double speed = drone.velocity.magnitude();
        const float heat = static_cast<float>(std::clamp(speed / m_cfg.maxSpeed, 0.0, 1.0));

        const float r = std::clamp(base[0] + 0.25f * heat, 0.0f, 1.0f);
        const float g = std::clamp(base[1] + 0.25f * heat, 0.0f, 1.0f);
        const float b = std::clamp(base[2] + 0.25f * heat, 0.0f, 1.0f);

        drawDroneModel(drone, r, g, b);

        if (m_showVelocity && speed > 1e-6) {
            const Vec3 tip = drone.position + drone.velocity.normalized() * 13.0;
            drawLine(drone.position, tip, r, g, b);
        }
    }
}


void OpenGLSwarmViewer::drawOverlay() const {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0.0, m_width, 0.0, m_height);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glColor3f(0.88f, 0.92f, 0.95f);

    char line[256];
    std::snprintf(line, sizeof(line),
                  "HiveFlight OpenGL | tick %d | time %.2fs | drones %zu | targets %zu | speed x%d%s",
                  m_sim.tick(), m_sim.time(), m_sim.drones().size(), m_sim.targets().size(), m_stepsPerFrame,
                  m_paused ? " | paused" : "");
    drawText(16.0f, static_cast<float>(m_height - 26), line);
    drawText(16.0f, 20.0f,
             "Mouse drag/orbit, wheel/zoom, arrows/camera, Space/pause, R/reset, V/vectors, +/- speed, 0/view, Q/quit");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void OpenGLSwarmViewer::drawText(float x, float y, const char* text) const {
    glRasterPos2f(x, y);
    for (const char* p = text; *p != '\0'; ++p) {
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *p);
    }
}

void OpenGLSwarmViewer::drawDroneModel(const Drone3D& drone, float r, float g, float b) const {
    glPushMatrix();
    glTranslated(drone.position.x, drone.position.y, drone.position.z);

    const double speed = drone.velocity.magnitude();
    if (speed > 1e-6) {
        const Vec3 dir = drone.velocity.normalized();
        const double yaw = std::atan2(dir.x, dir.z) * kRadToDeg;
        const double horizontal = std::sqrt(dir.x * dir.x + dir.z * dir.z);
        const double pitch = -std::atan2(dir.y, horizontal) * kRadToDeg;
        glRotated(yaw, 0.0, 1.0, 0.0);
        glRotated(pitch, 1.0, 0.0, 0.0);
    }

    setMaterial(r, g, b);
    glPushMatrix();
    glScaled(1.45, 0.34, 2.15);
    glutSolidCube(2.0);
    glPopMatrix();

    setMaterial(min(1.0f, r + 0.18f), min(1.0f, g + 0.18f), min(1.0f, b + 0.18f));
    glPushMatrix();
    glTranslated(0.0, 0.0, 2.45);
    glutSolidCone(0.85, 1.35, 18, 8);
    glPopMatrix();

    setMaterial(0.18f, 0.22f, 0.24f);
    glPushMatrix();
    glScaled(3.8, 0.12, 0.18);
    glutSolidCube(2.0);
    glPopMatrix();

    glPushMatrix();
    glScaled(0.18, 0.12, 3.8);
    glutSolidCube(2.0);
    glPopMatrix();

    const double rotorOffset = 3.55;
    const double rotors[4][2] = {
        {-rotorOffset, -rotorOffset},
        { rotorOffset, -rotorOffset},
        {-rotorOffset,  rotorOffset},
        { rotorOffset,  rotorOffset}
    };

    for (const auto& rotor : rotors) {
        glPushMatrix();
        glTranslated(rotor[0], 0.18, rotor[1]);
        setMaterial(0.08f, 0.10f, 0.11f);
        glPushMatrix();
        glScaled(0.42, 0.16, 0.42);
        glutSolidSphere(1.0, 14, 8);
        glPopMatrix();

        setMaterial(0.82f, 0.88f, 0.92f, 0.72f);
        glPushMatrix();
        glScaled(1.35, 0.045, 0.18);
        glutSolidCube(2.0);
        glPopMatrix();

        glPushMatrix();
        glRotated(90.0, 0.0, 1.0, 0.0);
        glScaled(1.35, 0.045, 0.18);
        glutSolidCube(2.0);
        glPopMatrix();
        glPopMatrix();
    }

    setMaterial(0.10f, 0.12f, 0.13f);
    for (double x : {-1.15, 1.15}) {
        glPushMatrix();
        glTranslated(x, -0.92, 0.0);
        glScaled(0.14, 0.14, 1.8);
        glutSolidCube(2.0);
        glPopMatrix();
    }

    glPopMatrix();
}

void OpenGLSwarmViewer::drawSphere(const Vec3& p, double radius, float r, float g, float b) const {
    setMaterial(r, g, b);
    glPushMatrix();
    glTranslated(p.x, p.y, p.z);
    glutSolidSphere(radius, 18, 10);
    glPopMatrix();
}

void OpenGLSwarmViewer::drawLine(const Vec3& a, const Vec3& b, float r, float g, float bcol) const {
    glDisable(GL_LIGHTING);
    glColor3f(r, g, bcol);
    glLineWidth(1.6f);
    glBegin(GL_LINES);
    glVertex3d(a.x, a.y, a.z);
    glVertex3d(b.x, b.y, b.z);
    glEnd();
    glLineWidth(1.0f);
    glEnable(GL_LIGHTING);
}

void OpenGLSwarmViewer::drawTargetLabel(const Vec3& position, int index) const {
    char label[16];
    std::snprintf(label, sizeof(label), "T%d", index);
    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos3d(position.x + 4.0, position.y + 4.0, position.z);
    for (const char* p = label; *p != '\0'; ++p) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *p);
    }
    glEnable(GL_LIGHTING);
}

OpenGLSwarmViewer* OpenGLSwarmViewer::instance() {
    return g_viewer;
}

void OpenGLSwarmViewer::displayCallback() {
    instance()->display();
}

void OpenGLSwarmViewer::reshapeCallback(int width, int height) {
    instance()->reshape(width, height);
}

void OpenGLSwarmViewer::keyboardCallback(unsigned char key, int x, int y) {
    instance()->keyboard(key, x, y);
}

void OpenGLSwarmViewer::specialCallback(int key, int x, int y) {
    instance()->special(key, x, y);
}

void OpenGLSwarmViewer::mouseCallback(int button, int state, int x, int y) {
    instance()->mouse(button, state, x, y);
}

void OpenGLSwarmViewer::motionCallback(int x, int y) {
    instance()->motion(x, y);
}

void OpenGLSwarmViewer::timerCallback(int value) {
    instance()->timer(value);
}
