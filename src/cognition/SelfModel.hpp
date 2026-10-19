#pragma once

/**
 * SelfModel - Represents the agent's internal model of itself
 * NOT consciousness - sensorimotor self-awareness through experience
 *
 * Key mechanisms:
 * - Learns body schema (how actions affect sensory state)
 * - Predicts consequences of own actions
 * - Distinguishes self from external events
 * - Represents own capabilities and limitations
 *
 * This system was developed in Phase 4 and is intended to be integrated
 * into the main brain loop for self-awareness and action prediction.
 */

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <unordered_map>
#include <string>

namespace nlm {

// BodyEffect: Records the relationship between an action and its sensory consequences
struct BodyEffect {
    // The action that causes this effect
    ActionType action;
    
    // The sensory state BEFORE the action
    std::vector<float> beforeState;
    
    // The sensory state AFTER the action
    std::vector<float> afterState;
    
    // Confidence in this effect (0-1)
    float confidence;
    
    // How many times this effect has been observed
    size_t observationCount;
    
    // Average reward associated with this effect
    float avgReward;
    
    BodyEffect()
        : action(ActionType::Wait), confidence(0.0f), observationCount(0), avgReward(0.0f) {}
    
    BodyEffect(ActionType act, const std::vector<float>& before,
               const std::vector<float>& after, float conf)
        : action(act), beforeState(before), afterState(after), confidence(conf),
          observationCount(1), avgReward(0.0f) {}
};

// CapabilityLevel: Represents the agent's learned abilities
struct CapabilityLevel {
    // Action type
    ActionType action;
    
    // Current capability level (0.0 = cannot perform, 1.0 = can perform perfectly)
    float level;
    
    // How stable this capability is (0-1)
    float stability;
    
    // How many attempts have been made
    size_t attempts;
    
    CapabilityLevel()
        : action(ActionType::Wait), level(0.0f), stability(0.0f), attempts(0) {}
    
    CapabilityLevel(ActionType act, float lvl, float stability)
        : action(act), level(lvl), stability(stability), attempts(1) {}
};

// SelfModel: Represents the agent's internal model of itself
class SelfModel {
public:
    SelfModel();
    ~SelfModel();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Record that taking an action caused a specific sensory change
    // This is the core learning mechanism for the self-model
    void recordSelfAction(ActionType action,
                         const std::vector<float>& beforeState,
                         const std::vector<float>& afterState,
                         float reward = 0.0f);

    // Predict sensory consequence of an action
    // This is the "forward model" of the self
    std::vector<float> predictActionConsequence(ActionType action,
                                                const std::vector<float>& currentState) const;

    // Get confidence in self-model for a given action
    float getSelfModelConfidence(ActionType action) const;

    // Is this change likely caused by self (action) vs external?
    // Returns 1.0 if definitely self-caused, 0.0 if definitely external
    float computeSelfGeneratedLikelihood(const std::vector<float>& beforeState,
                                        const std::vector<float>& afterState,
                                        ActionType action) const;

    // Get the body schema (preferred actions in different states)
    // Returns the most likely action for a given sensory state
    ActionType getPreferredAction(const std::vector<float>& state) const;

    // Update self-model based on prediction error
    void updateSelfModel(const std::vector<float>& predicted,
                        const std::vector<float>& actual,
                        ActionType action);

    // Get current capability level (0-1)
    float getCapabilityLevel(ActionType action) const;

    // Get body awareness (how accurately can predict consequences)
    float getBodyAwareness() const;

    // Learn new capability from successful action execution
    void learnCapability(ActionType action, float successRate);

    // Update body schema based on prediction accuracy
    void updateBodySchema(ActionType action,
                         const std::vector<float>& predictedEffect,
                         const std::vector<float>& actualEffect);

    // Clear self-model (for reset)
    void clear();

    // Has self-model formed?
    bool hasSelfModel() const { return !bodyEffects_.empty(); }

    // Get all recorded body effects
    const std::vector<std::vector<BodyEffect>>& getBodyEffects() const { return bodyEffects_; }

    // Get all capability levels
    const std::vector<CapabilityLevel>& getCapabilityLevels() const { return capabilityLevels_; }

    // Get number of body effects learned
    size_t getBodyEffectCount() const { return bodyEffects_.size(); }

    // Get average self-model accuracy
    float getSelfModelAccuracy() const;

    // Set parameters
    void setLearningRate(float rate) { learningRate_ = rate; }
    void setConfidenceThreshold(float threshold) { confidenceThreshold_ = threshold; }
    void setMaxEffectsPerAction(size_t max) { maxEffectsPerAction_ = max; }

private:
    // Find best matching previous experience for prediction
    std::vector<float> findMatchingEffect(ActionType action,
                                          const std::vector<float>& beforeState) const;

    // Calculate similarity between two sensory states
    float calculateStateSimilarity(const std::vector<float>& state1,
                                   const std::vector<float>& state2) const;

    // Update confidence based on prediction accuracy
    void updateConfidence(ActionType action, float predictionError);

    // Compute average reward for an action effect
    float computeAverageReward(ActionType action) const;

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    float learningRate_;
    float confidenceThreshold_;
    size_t maxEffectsPerAction_;
    
    // Body effects indexed by action type
    std::vector<std::vector<BodyEffect>> bodyEffects_;  // Indexed by ActionType
    
    // Capability levels indexed by action type
    std::vector<CapabilityLevel> capabilityLevels_;  // Indexed by ActionType
    
    // Body awareness score (0-1)
    float bodyAwareness_;
    
    // Overall self-model accuracy
    float selfModelAccuracy_;
};

} // namespace nlm