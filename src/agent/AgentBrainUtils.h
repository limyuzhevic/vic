// AgentBrain.h - Header for AgentBrain implementation with improved structure
// Extracted and refactored from AgentBrain.cpp

#pragma once

#include "AgentBrain.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

// Constants for AgentBrain functionality
namespace AgentBrainConstants {
    // Sensory processing constants
    constexpr float VISION_SCALE_FACTOR = 5.0f;
    constexpr float TOUCH_SCALE_FACTOR = 8.0f;
    constexpr float INTERNAL_SIGNAL_CENTER = -1.0f;
    constexpr float INTERNAL_SIGNAL_SCALE = 5.0f;
    constexpr float PROPRIOCEPTION_CENTER = -1.0f;
    constexpr float PROPRIOCEPTION_SCALE = 3.0f;
    
    // Novelty detection
    constexpr float DEFAULT_NOVELTY_DECAY = 0.99f;
    constexpr float NOVELTY_DAMPING_FACTOR = 2.0f;
    
    // Curiosity parameters
    constexpr float CURIOSITY_NOVELTY_WEIGHT = 2.0f;
    constexpr float CURIOSITY_PREDICTION_ERROR_WEIGHT = 0.5f;
    constexpr float MAX_CURIOUSITY_LEVEL = 1.0f;
    constexpr float EXPLORATION_THRESHOLD_LOW = 0.3f;
    constexpr float EXPLORATION_THRESHOLD_HIGH = 0.5f;
    constexpr float MAX_EXPLORATION_CHANCE = 0.3f;
    
    // Motor decoding
    constexpr float MOTOR_ACTIVITY_THRESHOLD = 0.5f;
    constexpr int32_t MOTOR_GROUP_COUNT = 6;
    constexpr int32_t SENSORY_GROUP_COUNT = 4;
    
    // Neuromodulation
    constexpr float DOPAMINE_CLAMP_MIN = -1.0f;
    constexpr float DOPAMINE_CLAMP_MAX = 1.0f;
    constexpr float EXPECTATION_DECAY_RATE = 0.95f;
    constexpr float REWARD_IMPACT_ON_EXPECTATION = 0.05f;
    constexpr float ELIGIBILITY_TRACE_DECAY = 0.1f;
    constexpr float ELIGIBILITY_THRESHOLD = 0.001f;
    constexpr float PLASTICITY_FACTOR_MIN = 0.1f;
    constexpr float PLASTICITY_FACTOR_MAX = 2.0f;
    constexpr float DOPAMINE_TO_PLASTICITY_FACTOR = 0.5f;
    
    // Development
    constexpr double DEVELOPMENT_STAGE_INITIAL = 60.0;     // ~1 minute
    constexpr double DEVELOPMENT_STAGE_CRITICAL = 300.0;  // ~5 minutes
    constexpr double DEVELOPMENT_STAGE_MATURATION = 900.0; // ~15 minutes
    constexpr float PLASTICITY_INITIAL = 1.0f;
    constexpr float PLASTICITY_CRITICAL = 0.8f;
    constexpr float PLASTICITY_MATURE = 0.5f;
    constexpr float PLASTICITY_ADULT = 0.2f;
    
    // Structural plasticity
    constexpr float SYNAPTOGENESIS_RATE_INITIAL = 0.0001f;
    constexpr float PRUNING_RATE_INITIAL = 0.00001f;
    constexpr float PLASTICITY_TO_SYNAPTOGENESIS_FACTOR = 0.0001f;
    constexpr float PLASTICITY_TO_PRUNING_FACTOR = 0.00001f;
    constexpr float SYNAPTOGENESIS_BASE_FACTOR = 1.0f;
    constexpr float PRUNING_BASE_FACTOR = 2.0f;
    
    // Input processing
    constexpr size_t SIZEOF_VISION = 256;      // 16x16
    constexpr size_t SIZEOF_TOUCH = 8;
    constexpr size_t SIZEOF_INTERNAL = 4;
    constexpr size_t SIZEOF_PROPRIOCEPTION = 6;
    
    // Motor output groups
    constexpr size_t MOTOR_GROUP_FORWARD = 0;
    constexpr size_t MOTOR_GROUP_BACKWARD = 1;
    constexpr size_t MOTOR_GROUP_TURN_LEFT = 2;
    constexpr size_t MOTOR_GROUP_TURN_RIGHT = 3;
    constexpr size_t MOTOR_GROUP_INTERACT = 4;
    constexpr size_t MOTOR_GROUP_WAIT = 5;
    
    // Motor command mapping
    constexpr size_t MOTOR_COMMAND_MOVE_FORWARD = 0;
    constexpr size_t MOTOR_COMMAND_MOVE_BACKWARD = 1;
    constexpr size_t MOTOR_COMMAND_TURN_LEFT = 2;
    constexpr size_t MOTOR_COMMAND_TURN_RIGHT = 3;
    constexpr size_t MOTOR_COMMAND_LOOK_LEFT = 4;
    constexpr size_t MOTOR_COMMAND_LOOK_RIGHT = 5;
    constexpr size_t MOTOR_COMMAND_INTERACT = 6;
    constexpr size_t MOTOR_COMMAND_WAIT = 7;
}

// Helper functions for AgentBrain
namespace AgentBrainHelpers {
    // Calculate average activity in motor neuron groups
    inline float calculateMotorGroupActivity(const std::vector<Neuron*>& neurons) {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            // Use membrane potential deviation from rest as activity measure
            sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
        }
        return sum / neurons.size();
    }
    
    // Normalize novelty value
    inline float normalizeNovelty(float totalDiff, size_t visionSize) {
        return totalDiff / std::max<size_t>(visionSize, 1);
    }
    
    // Clamp value between min and max
    template<typename T>
    inline T clamp(T value, T min, T max) {
        return std::max(min, std::min(value, max));
    }
    
    // Calculate prediction error
    inline float calculatePredictionError(float reward, float predictedReward) {
        return reward - predictedReward;
    }
    
    // Update expected reward with exponential moving average
    inline float updateExpectedReward(float currentExpected, float reward) {
        return 0.95f * currentExpected + 0.05f * reward;
    }
    
    // Get motor group index from neuron index
    inline size_t getMotorGroupIndex(size_t neuronIndex) {
        return neuronIndex % AgentBrainConstants::MOTOR_GROUP_COUNT;
    }
    
    // Get sensory group index from neuron index
    inline size_t getSensoryGroupIndex(size_t neuronIndex) {
        return neuronIndex % AgentBrainConstants::SENSORY_GROUP_COUNT;
    }
    
    // Check if motor command is valid
    inline bool isValidMotorCommand(MotorCommand cmd) {
        return cmd >= MotorCommand::MoveForward && cmd <= MotorCommand::Wait;
    }
    
    // Get random motor command for exploration
    inline MotorCommand getRandomMotorCommand(std::shared_ptr<Brain> brain) {
        if (!brain) return MotorCommand::Wait;
        
        int choice = brain->getRandomGenerator()->uniformInt(0, 7);
        switch (choice) {
            case AgentBrainConstants::MOTOR_COMMAND_MOVE_FORWARD: return MotorCommand::MoveForward;
            case AgentBrainConstants::MOTOR_COMMAND_MOVE_BACKWARD: return MotorCommand::MoveBackward;
            case AgentBrainConstants::MOTOR_COMMAND_TURN_LEFT: return MotorCommand::TurnLeft;
            case AgentBrainConstants::MOTOR_COMMAND_TURN_RIGHT: return MotorCommand::TurnRight;
            case AgentBrainConstants::MOTOR_COMMAND_LOOK_LEFT: return MotorCommand::LookLeft;
            case AgentBrainConstants::MOTOR_COMMAND_LOOK_RIGHT: return MotorCommand::LookRight;
            case AgentBrainConstants::MOTOR_COMMAND_INTERACT: return MotorCommand::Interact;
            default: return MotorCommand::Wait;
        }
    }
}

// Struct for vision comparison data
struct VisionDifference {
    const std::vector<float>& vision;
    const std::vector<float>& previousVision;
    float& noveltyLevel;
    float decayFactor;
    
    void compute() {
        if (vision.empty()) return;
        
        float totalDiff = 0.0f;
        for (size_t i = 0; i < vision.size() && i < previousVision.size(); ++i) {
            float diff = std::abs(vision[i] - previousVision[i]);
            totalDiff += diff;
        }
        
        // Normalize
        noveltyLevel = AgentBrainHelpers::normalizeNovelty(totalDiff, vision.size());
        
        // Decay
        noveltyLevel *= decayFactor;
    }
};

// Struct for motor command ranking
struct MotorCommandWithActivity {
    MotorCommand cmd;
    float activity;
    
    MotorCommandWithActivity(MotorCommand c, float a) : cmd(c), activity(a) {}
    
    bool operator<(const MotorCommandWithActivity& other) const {
        return activity > other.activity; // For sorting in descending order
    }
};

// Function to initialize motor neuron groups
inline void initializeMotorGroups(AgentBrain& agentBrain, std::shared_ptr<Brain> brain) {
    if (!brain) return;
    
    for (const auto& region : brain->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            
            if (type == NeuronType::Motor) {
                for (Neuron* n : pop->getNeurons()) {
                    // Use helper function to get motor group index
                    size_t groupIndex = AgentBrainHelpers::getMotorGroupIndex(
                        agentBrain.motorForward_.size() + agentBrain.motorBackward_.size() + 
                        agentBrain.motorTurnLeft_.size() + agentBrain.motorTurnRight_.size() +
                        agentBrain.motorInteract_.size() + agentBrain.motorWait_.size()
                    );
                    
                    switch (groupIndex) {
                        case AgentBrainConstants::MOTOR_GROUP_FORWARD:
                            agentBrain.motorForward_.push_back(n);
                            break;
                        case AgentBrainConstants::MOTOR_GROUP_BACKWARD:
                            agentBrain.motorBackward_.push_back(n);
                            break;
                        case AgentBrainConstants::MOTOR_GROUP_TURN_LEFT:
                            agentBrain.motorTurnLeft_.push_back(n);
                            break;
                        case AgentBrainConstants::MOTOR_GROUP_TURN_RIGHT:
                            agentBrain.motorTurnRight_.push_back(n);
                            break;
                        case AgentBrainConstants::MOTOR_GROUP_INTERACT:
                            agentBrain.motorInteract_.push_back(n);
                            break;
                        case AgentBrainConstants::MOTOR_GROUP_WAIT:
                            agentBrain.motorWait_.push_back(n);
                            break;
                    }
                }
            } else if (type == NeuronType::Sensory) {
                for (Neuron* n : pop->getNeurons()) {
                    size_t groupIndex = AgentBrainHelpers::getSensoryGroupIndex(
                        agentBrain.sensoryVision_.size() + agentBrain.sensoryTouch_.size() +
                        agentBrain.sensoryInternal_.size() + agentBrain.sensoryProprioception_.size()
                    );
                    
                    switch (groupIndex) {
                        case 0: agentBrain.sensoryVision_.push_back(n); break;
                        case 1: agentBrain.sensoryTouch_.push_back(n); break;
                        case 2: agentBrain.sensoryInternal_.push_back(n); break;
                        case 3: agentBrain.sensoryProprioception_.push_back(n); break;
                    }
                }
            }
        }
    }
}

// Function to process vision input
inline void processVisionInput(const std::vector<float>& vision, 
                              std::vector<Neuron*>& sensoryVision,
                              float scaleFactor) {
    for (size_t i = 0; i < sensoryVision.size() && i < vision.size(); ++i) {
        if (sensoryVision[i]) {
            float current = vision[i] * scaleFactor;
            sensoryVision[i]->injectCurrent(current);
        }
    }
}

// Function to process touch input
inline void processTouchInput(const std::vector<float>& touch,
                             std::vector<Neuron*>& sensoryTouch,
                             float scaleFactor) {
    for (size_t i = 0; i < sensoryTouch.size() && i < touch.size(); ++i) {
        if (sensoryTouch[i]) {
            float current = touch[i] * scaleFactor;
            sensoryTouch[i]->injectCurrent(current);
        }
    }
}

// Function to process internal signals
inline void processInternalSignals(const std::vector<float>& internal,
                                  std::vector<Neuron*>& sensoryInternal,
                                  float center, float scale) {
    for (size_t i = 0; i < sensoryInternal.size() && i < internal.size(); ++i) {
        if (sensoryInternal[i]) {
            float current = (internal[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
            sensoryInternal[i]->injectCurrent(current);
        }
    }
}

// Function to process proprioception
inline void processProprioception(const std::vector<float>& proprio,
                                 std::vector<Neuron*>& sensoryProprioception,
                                 float center, float scale) {
    for (size_t i = 0; i < sensoryProprioception.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
            sensoryProprioception[i]->injectCurrent(current);
        }
    }
}

} // namespace nlm
