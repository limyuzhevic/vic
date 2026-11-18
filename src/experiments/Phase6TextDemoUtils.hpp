// Helper utilities for Phase 6 text/thought demonstration
// This file contains utility functions extracted from the monolithic Phase6TextDemo.cpp

#pragma once

#include <string>
#include <vector>
#include <memory>

namespace nlm {

// Configuration factory for text/thought demo
class ConfigFactory {
public:
    static std::shared_ptr<Config> makeConfig() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 500);
        config->set("region_count", 2);
        config->set("connection_probability", 0.1f);
        config->set("random_seed", static_cast<int64_t>(42));
        config->set("simulation_timestep", 0.001);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("stdp_tau", 20.0f);
        config->set("synaptogenesis_rate", 0.0001f);
        config->set("pruning_rate", 0.00001f);
        return config;
    }
};

// Perception factory for text/thought demo
class PerceptionFactory {
public:
    static SensoryPercept makePercept(const std::vector<float>& vision,
                                     const std::vector<float>& touch,
                                     const std::string& text = "") {
        SensoryPercept p;
        p.setVision(vision);
        p.setTouch(touch);
        p.setInternal({1.0f, 1.0f, 0.0f, 0.0f});
        p.setProprioception({0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f});
        p.setAudio({});
        p.setText(text);
        return p;
    }
};

// Mathematical utilities
class MathUtils {
public:
    static std::vector<float> subtract(const std::vector<float>& a, const std::vector<float>& b) {
        std::vector<float> out;
        size_t n = std::min(a.size(), b.size());
        out.reserve(n);
        for (size_t i = 0; i < n; ++i) out.push_back(a[i] - b[i]);
        return out;
    }

    static float cosine(const std::vector<float>& a, const std::vector<float>& b) {
        size_t n = std::min(a.size(), b.size());
        float dot = 0.0f, na = 0.0f, nb = 0.0f;
        for (size_t i = 0; i < n; ++i) {
            dot += a[i] * b[i];
            na += a[i] * a[i];
            nb += b[i] * b[i];
        }
        if (na < 1e-9f || nb < 1e-9f) return 0.0f;
        return dot / (std::sqrt(na) * std::sqrt(nb));
    }
};

// Motor command name utilities
class MotorCommandUtils {
public:
    static std::string cmdName(MotorCommand cmd) {
        switch (cmd) {
            case MotorCommand::Idle: return "idle";
            case MotorCommand::MoveForward: return "move-forward";
            case MotorCommand::MoveBackward: return "move-backward";
            case MotorCommand::MoveLeft: return "move-left";
            case MotorCommand::MoveRight: return "move-right";
            case MotorCommand::RotateLeft: return "rotate-left";
            case MotorCommand::RotateRight: return "rotate-right";
            case MotorCommand::Interact: return "interact";
            default: return "unknown";
        }
    }
};

// Demo step utilities
class DemoStepUtils {
public:
    static std::vector<float> presentAndRead(AgentBrain& agent, 
                                             const std::shared_ptr<Brain>& brain,
                                             const SensoryPercept& percept, 
                                             int actions,
                                             uint64_t& step) {
        for (int a = 0; a < actions; ++a) {
            agent.processSensoryInput(percept);
            for (int i = 0; i < 10; ++i) brain->step(step++);
        }
        return agent.getInternalFiringRates();
    }
};

} // namespace nlm
