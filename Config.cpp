#include "Config.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

static bool parseBool(const std::string& v, bool& out) {
    if (v == "1" || v == "true" || v == "TRUE" || v == "yes" || v == "YES") {
        out = true;
        return true;
    }
    if (v == "0" || v == "false" || v == "FALSE" || v == "no" || v == "NO") {
        out = false;
        return true;
    }
    return false;
}

static std::string trimCopy(std::string s) {
    while (!s.empty() && (s.back() == ' ' || s.back() == '\t' || s.back() == '\r' || s.back() == '\n')) s.pop_back();
    std::size_t i = 0;
    while (i < s.size() && (s[i] == ' ' || s[i] == '\t' || s[i] == '\r' || s[i] == '\n')) i++;
    if (i > 0) s = s.substr(i);
    return s;
}

static bool loadKeyValuePairs(const std::string& path, SimConfig& cfg) {
    std::ifstream in(path);
    if (!in.is_open()) {
        return false;
    }

    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        if (line[0] == '#') continue;

        // allow inline comments: key=value # comment
        auto hashPos = line.find('#');
        if (hashPos != std::string::npos) {
            line = line.substr(0, hashPos);
        }

        auto eq = line.find('=');
        if (eq == std::string::npos) continue;

        std::string key = trimCopy(line.substr(0, eq));
        std::string value = trimCopy(line.substr(eq + 1));
        if (key.empty()) continue;

        auto parseDouble = [&](double& dst) {
            std::istringstream iss(value);
            return (iss >> dst) ? true : false;
        };
        auto parseInt = [&](int& dst) {
            std::istringstream iss(value);
            return (iss >> dst) ? true : false;
        };
        auto parseUInt = [&](unsigned int& dst) {
            std::istringstream iss(value);
            return (iss >> dst) ? true : false;
        };
        auto parseSizeT = [&](std::size_t& dst) {
            std::istringstream iss(value);
            long long tmp = 0;
            if (!(iss >> tmp)) return false;
            if (tmp < 0) return false;
            dst = static_cast<std::size_t>(tmp);
            return true;
        };

        bool b;
        if (key == "worldWidth" && parseDouble(cfg.worldWidth)) continue;
        if (key == "worldHeight" && parseDouble(cfg.worldHeight)) continue;
        if (key == "wrapAround" && parseBool(value, b)) {
            cfg.wrapAround = b;
            continue;
        }

        if (key == "droneCount" && parseSizeT(cfg.droneCount)) continue;
        if (key == "seed" && parseUInt(cfg.seed)) continue;

        if (key == "posMin" && parseDouble(cfg.posMin)) continue;
        if (key == "posMax" && parseDouble(cfg.posMax)) continue;
        if (key == "velMin" && parseDouble(cfg.velMin)) continue;
        if (key == "velMax" && parseDouble(cfg.velMax)) continue;

        if (key == "dt" && parseDouble(cfg.dt)) continue;
        if (key == "steps" && parseInt(cfg.steps)) continue;
        if (key == "sleepMs" && parseInt(cfg.sleepMs)) continue;

        if (key == "separationRadius" && parseDouble(cfg.separationRadius)) continue;
        if (key == "alignmentRadius" && parseDouble(cfg.alignmentRadius)) continue;
        if (key == "cohesionRadius" && parseDouble(cfg.cohesionRadius)) continue;

        if (key == "desiredSeparationSpeed" && parseDouble(cfg.desiredSeparationSpeed)) continue;

        if (key == "weightSeparation" && parseDouble(cfg.weightSeparation)) continue;
        if (key == "weightAlignment" && parseDouble(cfg.weightAlignment)) continue;
        if (key == "weightCohesion" && parseDouble(cfg.weightCohesion)) continue;

        if (key == "maxSpeed" && parseDouble(cfg.maxSpeed)) continue;

        // Visualization
        if (key == "renderPpm" && parseBool(value, b)) {
            cfg.renderPpm = b;
            continue;
        }
        if (key == "renderImageW" && parseInt(cfg.renderImageW)) continue;
        if (key == "renderImageH" && parseInt(cfg.renderImageH)) continue;
        if (key == "renderEvery" && parseInt(cfg.renderEvery)) continue;
        if (key == "renderFramesDir") {
            cfg.renderFramesDir = value;
            continue;
        }

        // unknown key => ignore
    }

    return true;
}

SimConfig loadConfigFromFile(const std::string& path) {
    SimConfig cfg;
    loadKeyValuePairs(path, cfg);
    return cfg;
}

SimConfig loadConfigFromArgs(int argc, char** argv) {
    SimConfig cfg;

    for (int i = 1; i < argc; i++) {
        std::string a = argv[i];

        auto consumeValue = [&]() -> std::string {
            if (i + 1 >= argc) return "";
            return argv[++i];
        };

        if (a == "--config") {
            std::string p = consumeValue();
            if (!p.empty()) {
                cfg = loadConfigFromFile(p);
            }
            continue;
        }

        auto parseOverrideDouble = [&](const std::string& key, double& dst) {
            if (a == key) {
                std::string v = consumeValue();
                if (!v.empty()) {
                    std::istringstream iss(v);
                    iss >> dst;
                }
                return true;
            }
            return false;
        };

        auto parseOverrideInt = [&](const std::string& key, int& dst) {
            if (a == key) {
                std::string v = consumeValue();
                if (!v.empty()) {
                    std::istringstream iss(v);
                    iss >> dst;
                }
                return true;
            }
            return false;
        };

        auto parseOverrideUInt = [&](const std::string& key, unsigned int& dst) {
            if (a == key) {
                std::string v = consumeValue();
                if (!v.empty()) {
                    std::istringstream iss(v);
                    iss >> dst;
                }
                return true;
            }
            return false;
        };

        auto parseOverrideSizeT = [&](const std::string& key, std::size_t& dst) {
            if (a == key) {
                std::string v = consumeValue();
                if (!v.empty()) {
                    std::istringstream iss(v);
                    long long tmp = 0;
                    if (iss >> tmp && tmp >= 0) dst = static_cast<std::size_t>(tmp);
                }
                return true;
            }
            return false;
        };

        auto parseOverrideBool = [&](const std::string& key, bool& dst) {
            if (a == key) {
                std::string v = consumeValue();
                if (!v.empty()) {
                    bool b;
                    if (parseBool(v, b)) dst = b;
                }
                return true;
            }
            return false;
        };

        // Doubles
        if (parseOverrideDouble("--worldWidth", cfg.worldWidth)) continue;
        if (parseOverrideDouble("--worldHeight", cfg.worldHeight)) continue;
        if (parseOverrideDouble("--posMin", cfg.posMin)) continue;
        if (parseOverrideDouble("--posMax", cfg.posMax)) continue;
        if (parseOverrideDouble("--velMin", cfg.velMin)) continue;
        if (parseOverrideDouble("--velMax", cfg.velMax)) continue;
        if (parseOverrideDouble("--dt", cfg.dt)) continue;
        if (parseOverrideDouble("--separationRadius", cfg.separationRadius)) continue;
        if (parseOverrideDouble("--alignmentRadius", cfg.alignmentRadius)) continue;
        if (parseOverrideDouble("--cohesionRadius", cfg.cohesionRadius)) continue;
        if (parseOverrideDouble("--desiredSeparationSpeed", cfg.desiredSeparationSpeed)) continue;
        if (parseOverrideDouble("--weightSeparation", cfg.weightSeparation)) continue;
        if (parseOverrideDouble("--weightAlignment", cfg.weightAlignment)) continue;
        if (parseOverrideDouble("--weightCohesion", cfg.weightCohesion)) continue;
        if (parseOverrideDouble("--maxSpeed", cfg.maxSpeed)) continue;

        // Ints
        if (parseOverrideInt("--steps", cfg.steps)) continue;
        if (parseOverrideInt("--sleepMs", cfg.sleepMs)) continue;

        if (parseOverrideInt("--renderImageW", cfg.renderImageW)) continue;
        if (parseOverrideInt("--renderImageH", cfg.renderImageH)) continue;
        if (parseOverrideInt("--renderEvery", cfg.renderEvery)) continue;

        // UInt/size
        if (parseOverrideUInt("--seed", cfg.seed)) continue;
        if (parseOverrideSizeT("--droneCount", cfg.droneCount)) continue;

        // Bool
        if (parseOverrideBool("--wrapAround", cfg.wrapAround)) continue;
        if (parseOverrideBool("--renderPpm", cfg.renderPpm)) continue;

        // Strings
        if (a == "--renderFramesDir") {
            std::string v = consumeValue();
            if (!v.empty()) cfg.renderFramesDir = v;
            continue;
        }

        // unknown => ignore
    }

    return cfg;
}

