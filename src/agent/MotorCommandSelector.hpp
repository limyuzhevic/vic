// MotorCommandSelector.hpp - Extracts motor command selection logic from AgentBrain
#pragma once

#include "../src/brain/Brain.hpp"
#include "../src/agent/AgentBrain.hpp"
#include <string>
#include <vector>

namespace nlm {

// Motor command selection logic extracted from AgentBrain
class MotorCommandSelector {
public:
    // Configuration for motor command selection
    struct Config {
        float forwardBonus = 10.0f;          // Bonus for moving forward toward objects
        float backwardPenalty = 2.0f;       // Penalty for moving backward
        float turnBonus = 15.0f;             // Bonus for turning toward objects
        float interactBonus = 20.0f;         // Bonus for interacting with resources
        float hazardPenalty = 12.0f;        // Penalty for moving toward hazards
        float plannerOverrideThreshold = 0.15f; // Minimum quality gap for planner override
        float plannerBias = 15.0f;           // Bias for planner suggestions
        float conceptBias = 8.0f;            // Bias for concept preferences
        float selfModelBias = 3.0f;          // Bias for self-model predictions
        float socialBias = 5.0f;             // Bias for social learning
        float turnDiversityBonus = 6.0f;    // Bonus for turning diversity
        float repetitionPenalty = 8.0f;     // Penalty for repeating actions
        float momentumBias = 8.0f;           // Bias against direction reversal
    };
    
    MotorCommandSelector() = default;
    MotorCommandSelector(const Config& config) : config_(config) {}
    
    // Select motor command based on neural activities
    MotorCommand selectCommand(const std::vector<AgentBrain::MotorGroupActivity>& activities,
                             const AgentBrain& agent,
                             float noveltyLevel,
                             float forwardTouch,
                             float forwardObjectVision,
                             float maxTouch,
                             float learnedPreference);
    
    // Apply movement-based vision reflex
    void applyVisionGating(float& forwardRate, float& backwardRate, float& turnLeftRate, 
                          float& turnRightRate, float& interactRate,
                          float maxTouch, float forwardObjectVision);
    
    // Apply movement penalties
    void applyMovementPenalties(float& forwardRate, float& backwardRate,
                                const AgentBrain& agent);
    
    // Apply curiosity-based exploration
    MotorCommand applyCuriosity(const MotorCommand& defaultCmd, float curiosityLevel,
                               std::mt19937& rng);
    
    // Calculate activity margin and winner determination
    void calculateWinner(const std::vector<AgentBrain::MotorGroupActivity>& activities,
                        int& bestIdx, float& bestMargin);
    
    // Get current configuration
    const Config& getConfig() const { return config_; }
    
    // Set configuration
    void setConfig(const Config& config) { config_ = config; }
    
private:
    Config config_;
    
    // Helper methods
    MotorCommand getMotorCommandFromGroup(int groupIdx) const;
    bool isMovementCommand(MotorCommand cmd) const;
    float calculateDistance(float forwardVision, float forwardTouch) const;
};

} // namespace nlm
