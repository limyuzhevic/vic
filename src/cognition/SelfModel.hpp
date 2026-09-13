#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <string>

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
    float computeSelfGeneratedLikeness(const std::vector<float>& beforeState,
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

private:
    // Find best matching previous experience
    std::vector<float> findMatchingEffect(ActionType action,
                                          const std::vector<float>& beforeState) const;

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    float capabilityLevel_;
    
    // Action -> (beforeState -> afterState) mappings
    struct ActionEffect {
        std::vector<float> beforeState;
        std::vector<float> afterState;
        float confidence;
        size_t observationCount;
    };
    
    std::vector<std::vector<ActionEffect>> actionEffects_;  // Indexed by ActionType
};

} // namespace nlm
