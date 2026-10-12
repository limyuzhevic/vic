#pragma once

#include <vector>

namespace nlm {

class Neuron;

class MotorDecoder {
public:
    struct Config {
        float activationThreshold = 0.5f;       // Minimum activity to consider action
        bool normalizeActivities = true;        // Normalize neuron activities
        float visionScaleFactor = 5.0f;         // Scale factor for vision input
        float touchScaleFactor = 8.0f;          // Scale factor for touch input
        float internalScaleFactor = 5.0f;       // Scale factor for internal signals
        float proprioceptionScaleFactor = 3.0f; // Scale factor for proprioception
    };

    MotorDecoder(const Config& config = Config()) : config_(config) {}

    MotorCommand decodeFromMotorNeurons(const std::vector<Neuron*>& motorForward,
                                        const std::vector<Neuron*>& motorBackward,
                                        const std::vector<Neuron*>& motorTurnLeft,
                                        const std::vector<Neuron*>& motorTurnRight,
                                        const std::vector<Neuron*>& motorInteract,
                                        const std::vector<Neuron*>& motorWait) {
        auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
            if (neurons.empty()) return 0.0f;
            float sum = 0.0f;
            for (Neuron* n : neurons) {
                sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
            }
            return sum / neurons.size();
        };

        float forwardAct = calcActivity(motorForward);
        float backwardAct = calcActivity(motorBackward);
        float leftAct = calcActivity(motorTurnLeft);
        float rightAct = calcActivity(motorTurnRight);
        float interactAct = calcActivity(motorInteract);
        float waitAct = calcActivity(motorWait);

        // Find maximum activity
        struct { MotorCommand cmd; float activity; } commands[] = {
            {MotorCommand::MoveForward, forwardAct},
            {MotorCommand::MoveBackward, backwardAct},
            {MotorCommand::TurnLeft, leftAct},
            {MotorCommand::TurnRight, rightAct},
            {MotorCommand::Interact, interactAct},
            {MotorCommand::Wait, waitAct}
        };

        MotorCommand best = MotorCommand::Wait;
        float bestActivity = waitAct; // Default to wait if nothing stronger

        for (const auto& c : commands) {
            if (c.activity > bestActivity) {
                bestActivity = c.activity;
                best = c.cmd;
            }
        }

        // Only act if there's meaningful activity
        if (bestActivity < config_.activationThreshold) {
            return MotorCommand::Wait;
        }

        return best;
    }

    std::vector<float> scaleSensoryInput(const std::vector<float>& input, float scaleFactor) {
        std::vector<float> scaled;
        scaled.reserve(input.size());
        for (float value : input) {
            scaled.push_back(value * scaleFactor);
        }
        return scaled;
    }

    void setConfig(const Config& config) { config_ = config; }

private:
    Config config_;
};

} // namespace nlm
