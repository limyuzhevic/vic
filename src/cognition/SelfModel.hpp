#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <algorithm>
#include <cmath>
#include "MathUtilities.hpp"

namespace nlm {

// SelfModel: Internal cognitive model of self and body
// Tracks how one's actions affect the environment
// Used for predicting consequences of planned actions
//
// Key mechanisms:
// - Stores learned action-effect associations
// - Can predict outcomes of actions
// - Updates based on prediction errors
// - Supports goal-directed planning

class SelfModel {
public:
    SelfModel();
    ~SelfModel();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Record an action and its effect on state
    // This learns a new cause-effect relationship
    void recordSelfAction(ActionType action,
                         const std::vector<float>& beforeState,
                         const std::vector<float>& afterState);
    
    // Predict the state that will result from an action
    // Uses similar past experiences to make predictions
    std::vector<float> predictActionConsequence(ActionType action,
                                              const std::vector<float>& currentState) const;
    
    // Get confidence in a prediction for this action
    // Higher confidence means more reliable prediction
    float getSelfModelConfidence(ActionType action) const;
    
    // Get how well one understands one's own capabilities
    // Measures consistency between predictions and outcomes
    float getBodyAwareness() const { return bodyAwarenessLevel_; }
    
    // Update model based on prediction error
    // Helps the system learn from mistakes
    void updateSelfModel(const std::vector<float>& predicted,
                       const std::vector<float>& actual,
                       ActionType action);
    
    // Get preferred action for current state
    // Based on past experience and self-knowledge
    ActionType getPreferredAction(const std::vector<float>& state) const;
    
    // Clear all learned experiences
    void clear();
    
    // Get most confident prediction for an action
    // Returns similarity score for matching experiences
    float getPredictionConfidence(ActionType action) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    float bodyAwarenessLevel_;
    
    // Action effects database
    // Maps actions to past experiences and their outcomes
    struct ActionEffect {
        std::vector<float> beforeState;
        std::vector<float> afterState;
        float confidence;
        size_t observationCount;
        
        ActionEffect() : confidence(0.0f), observationCount(0) {}
    };
    
    std::vector<std::vector<ActionEffect>> actionEffects_;
};

} // namespace nlm
