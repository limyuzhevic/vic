#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

// SelfModel: Represents the agent's internal model of itself
// NOT consciousness - sensorimotor self-awareness through experience
//
// Key mechanisms:
// - Learns body schema (how actions affect sensory state)
// - Predicts consequences of own actions
// - Distinguishes self from external events
// - Represents own capabilities and limitations

class SelfModel {
public:
    SelfModel();
    ~SelfModel();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Record that taking an action caused a specific sensory change
    void recordSelfAction(ActionType action,
                         const std::vector<float>& beforeState,
                         const std::vector<float>& afterState);

    // Predict sensory consequence of an action
    // This is the "forward model" of the self
    std::vector<float> predictActionConsequence(ActionType action,
                                                const std::vector<float>& currentState);

    // Get confidence in self-model for a given action
    float getSelfModelConfidence(ActionType action) const;

    // Is this change likely caused by self (action) vs external?
    float computeSelfGeneratedLikelihood(const std::vector<float>& beforeState,
                                       const std::vector<float>& afterState,
                                       ActionType action) const;

    // Get the body schema (preferred actions in different states)
    // Returns map of state -> preferred action
    ActionType getPreferredAction(const std::vector<float>& state);

    // Update self-model based on prediction error
    void updateSelfModel(const std::vector<float>& predicted,
                        const std::vector<float>& actual,
                        ActionType action);

    // Get current capability level (0-1)
    float getCapabilityLevel() const { return capabilityLevel_; }

    // Get body awareness (how accurately can predict consequences)
    float getBodyAwareness() const;

    // Clear self-model
    void clear();

    // Has self-model formed?
    bool hasSelfModel() const { return !actionEffects_.empty(); }

    // Enable/disable self-model learning
    void setEnabled(bool enabled) { enabled_ = enabled; }
    bool isEnabled() const { return enabled_; }

    // Set learning rate for self-model updates
    void setLearningRate(float rate) { learningRate_ = rate; }
    float getLearningRate() const { return learningRate_; }

    // Get current state representation size
    size_t getStateSize() const { return stateSize_; }

    // Get all recorded action effects for analysis
    const std::vector<std::vector<ActionEffect>>& getActionEffects() const { return actionEffects_; }

    // Find the best matching experience for prediction
    std::vector<float> findBestMatchingEffect(ActionType action,
                                            const std::vector<float>& currentState) const;

    // Calculate prediction error for self-model updates
    float calculatePredictionError(const std::vector<float>& predicted,
                                 const std::vector<float>& actual) const;

    // Update capability level based on prediction performance
    void updateCapabilityLevel(float error, float confidence);

    // Get the most recent action effects
    std::vector<ActionEffect> getRecentEffects(ActionType action, size_t limit = 10) const;

    // Check if self-model has sufficient experience for reliable predictions
    bool isReadyForPrediction() const;

    // Get statistics about self-model performance
    float getAveragePredictionAccuracy() const;
    float getTypicalPredictionError() const;
    size_t getTotalObservations() const;

    // Export self-model for analysis/debugging
    std::string exportSelfModel() const;

    // Import self-model from saved data
    bool importSelfModel(const std::string& data);

private:
    // Internal implementation details
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    float capabilityLevel_;
    bool enabled_;
    float learningRate_;
    size_t stateSize_;

    // Action -> (beforeState -> afterState) mappings
    std::vector<std::vector<ActionEffect>> actionEffects_;  // Indexed by ActionType
    
    // Track prediction performance
    struct PredictionRecord {
        float accuracy;
        float error;
        SimulationStep lastUpdated;
    };
    std::vector<PredictionRecord> predictionRecords_;
    
    // Body schema: state -> preferred action mapping
    std::unordered_map<std::string, ActionType> bodySchema_;
    
    // Temporal decay for effects
    SimulationStep lastCleanupStep_;
};

} // namespace nlm
