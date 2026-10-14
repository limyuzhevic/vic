#pragma once

#include <string>
#include <algorithm>

namespace nlm {

// Exception class for AgentBrain errors
class AgentBrainException : public std::runtime_error {
public:
    explicit AgentBrainException(const std::string& message)
        : std::runtime_error("AgentBrain: " + message) {}
    
    static AgentBrainException nullPointerError(const std::string& component) {
        return AgentBrainException("Null pointer error: " + component);
    }
    
    static AgentBrainException invalidValueError(const std::string& operation, float value, float expectedMin, float expectedMax) {
        return AgentBrainException(
            "Invalid value for " + operation + ": " + std::to_string(value) +
            " (expected range: [" + std::to_string(expectedMin) + ", " + std::to_string(expectedMax) + "])");
    }
};

// Configuration namespace containing all magic numbers and constants
namespace AgentBrainConfig {
    // Neural network architecture constants
    constexpr size_t VISION_WIDTH = 16;           // Vision grid dimensions
    constexpr size_t VISION_HEIGHT = 16;
    constexpr size_t VISION_NEURONS = VISION_WIDTH * VISION_HEIGHT;  // 256
    
    constexpr size_t TOUCH_NEUTRONS = 8;          // Touch sensor neurons
    constexpr size_t INTERNAL_NEUTRONS = 4;       // Internal state neurons
    constexpr size_t PROPRIOCEPTIVE_NEUTRONS = 6;  // Body position neurons
    constexpr size_t MOTOR_GROUP_COUNT = 6;        // Motor actions: forward, backward, left, right, interact, wait
    
    // Sensory scaling factors for different input modalities
    constexpr float VISION_SCALE = 5.0f;          // Vision intensity scaling
    constexpr float TOUCH_SCALE = 8.0f;           // Touch/collision signal strength
    constexpr float INTERNAL_SCALE = 5.0f;        // Internal signals scaling
    constexpr float PROPRIOCEPTIVE_SCALE = 3.0f;   // Proprioception scaling
    
    // Sensory signal processing
    constexpr float SENSORY_NOVELTY_DECAY = 0.99f; // Exponential decay for novelty
    
    // Neuromodulation constants
    constexpr float DOPAMINE_CLAMP_MIN = -1.0f;    // Dopamine signal bounds
    constexpr float DOPAMINE_CLAMP_MAX = 1.0f;
    constexpr float PREDICTION_ERROR_WEIGHT = 0.5f;  // Weight of prediction error in curiosity
    constexpr float EXPECTED_REWARD_ALPHA = 0.05f;   // Learning rate for expected reward
    constexpr float EXPECTED_REWARD_BETA = 0.95f;   // Decay factor for expected reward
    constexpr float ELIGIBILITY_TRACE_DECAY = 0.1f;  // Decay rate for eligibility traces
    constexpr float ELIGIBILITY_THRESHOLD = 0.001f;  // Minimum eligibility to apply updates
    constexpr float PLASTICITY_FACTOR_BASE = 0.5f;   // Base plasticity factor
    constexpr float PLASTICITY_RANGE = 1.0f;         // Plasticity range adjustment
    
    // Development system constants
    constexpr double INITIAL_DEVELOPMENT_TIME = 0.0;  // Starting development age
    constexpr double CRITICAL_PERIOD_THRESHOLD = 60.0;  // ~1 minute in simulation
    constexpr double MATURATION_THRESHOLD = 300.0;     // ~5 minutes
    constexpr double ADULT_THRESHOLD = 900.0;         // ~15 minutes
    constexpr float HIGH_PLASTICITY = 1.0f;          // Plasticity in initial stage
    constexpr float MODERATE_PLASTICITY = 0.8f;      // Plasticity in critical period
    constexpr float LOW_PLASTICITY = 0.5f;           // Plasticity in maturation
    constexpr float ADULT_PLASTICITY = 0.2f;         // Plasticity in adult stage
    constexpr float SYNAPTOGENESIS_RATE_BASE = 0.0001f;  // Base synaptogenesis rate
    constexpr float PRUNING_RATE_BASE = 0.00001f;       // Base pruning rate
    
    // Curiosity and exploration constants
    constexpr float CURIOSITY_NOVELTY_WEIGHT = 2.0f;    // Novelty contribution to curiosity
    constexpr float CURIOSITY_PREDICTION_ERROR_WEIGHT = 0.5f;  // Prediction error contribution
    constexpr float CURIOSITY_MAX = 1.0f;                // Maximum curiosity level
    constexpr float EXPLORATION_THRESHOLD = 0.5f;       // Curiosity level for exploration
    constexpr float EXPLORATION_CHANCE_MAX = 0.3f;      // Maximum exploration probability
    constexpr float MOTOR_THRESHOLD = 0.5f;              // Minimum activity for motor actions
    
    // Motor command constants
    constexpr size_t RANDOM_ACTION_RANGE_MIN = 0;       // Range for random actions
    constexpr size_t RANDOM_ACTION_RANGE_MAX = 7;       // Range for random actions (extended)
    
    // Error handling constants
    constexpr float VALIDATION_EPSILON = 1e-6f;        // Small value for floating point comparisons
    
    // Configuration validation
    constexpr float MIN_SCALE_FACTOR = 0.0f;           // Minimum valid scaling factor
    constexpr float MAX_SCALE_FACTOR = 20.0f;          // Maximum valid scaling factor
    
} // namespace AgentBrainConfig

} // namespace nlm
