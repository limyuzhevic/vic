#pragma once

#include <memory>
#include <vector>
#include <string>

namespace nlm {

class Brain;
class Neuron;

class MotorCommandSelector {
public:
    struct Config {
        float curiosityThreshold = 0.3f;      // Threshold for curiosity-based exploration
        float randomActionThreshold = 0.5f;   // Threshold for high curiosity exploration
        float maxExploreChance = 0.3f;       // Maximum probability of random action
        bool enableCuriosity = true;          // Enable curiosity system
        std::vector<MotorCommand> actionSpace; // Available actions for random selection
    };

    MotorCommandSelector(std::shared_ptr<Brain> brain, const Config& config = Config())
        : brain_(brain), config_(config) {
        if (brain_) {
            initializeActionSpace();
        }
    }

    MotorCommand selectWithCuriosity(MotorCommand defaultCmd) {
        if (!config_.enableCuriosity || curiosityLevel_ <= config_.randomActionThreshold) {
            return defaultCmd;
        }

        float exploreChance = std::min(curiosityLevel_ * config_.maxExploreChance, config_.maxExploreChance);
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        
        if (r < exploreChance) {
            return randomAction();
        }

        return defaultCmd;
    }

    void setCuriosityLevel(float level) { curiosityLevel_ = std::clamp(level, 0.0f, 1.0f); }
    void enableCuriosity(bool enable) { config_.enableCuriosity = enable; }
    void setActionSpace(const std::vector<MotorCommand>& actions) {
        config_.actionSpace = actions;
    }

private:
    void initializeActionSpace() {
        config_.actionSpace.clear();
        config_.actionSpace.push_back(MotorCommand::MoveForward);
        config_.actionSpace.push_back(MotorCommand::MoveBackward);
        config_.actionSpace.push_back(MotorCommand::TurnLeft);
        config_.actionSpace.push_back(MotorCommand::TurnRight);
        config_.actionSpace.push_back(MotorCommand::LookLeft);
        config_.actionSpace.push_back(MotorCommand::LookRight);
        config_.actionSpace.push_back(MotorCommand::Interact);
        config_.actionSpace.push_back(MotorCommand::Wait);
    }

    MotorCommand randomAction() {
        if (config_.actionSpace.empty()) return MotorCommand::Wait;

        size_t index = brain_->getRandomGenerator()->uniformInt(0, static_cast<int>(config_.actionSpace.size()) - 1);
        return config_.actionSpace[index];
    }

    std::shared_ptr<Brain> brain_;
    Config config_;
    float curiosityLevel_ = 0.0f;
};

} // namespace nlm
