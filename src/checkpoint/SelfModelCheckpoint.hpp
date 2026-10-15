// SelfModel checkpoint data structures

#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "../core/Types/Types.hpp"

namespace nlm {

// ActionEffectMapping for forward model
struct ActionEffectMappingCheckpoint {
    // Action taken
    ActionType action;
    
    // Resulting sensory state
    std::vector<float> resultingSensoryState;
    
    // Effect properties
    float reward;
    float energy;
    float novelty;
    
    // Reliability indicators
    float reliability;
    size_t observationCount;
    float predictionError;
    
    // Context information
    std::vector<float> actionContext;
    float temporalContext;
    
    // Social context
    bool isSociallyObserved;
    float socialReinforcement;
    
    ActionEffectMappingCheckpoint()
        : action(ActionType::Wait), reward(0), energy(0), novelty(0)
        , reliability(0), observationCount(0), predictionError(0)
        , temporalContext(0), isSociallyObserved(false), socialReinforcement(0) {}
};

// SelfModel checkpoint state
struct SelfModelCheckpoint {
    // Action-effect mappings (forward model)
    std::vector<ActionEffectMappingCheckpoint> actionEffectMappings;
    
    // Body schema (sensory-motor integration)
    std::vector<float> bodySchema;
    std::vector<float> motorCapabilities;
    
    // Self-other distinction
    std::unordered_map<ActionType, float> selfEffectStrengths;
    std::unordered_map<ActionType, float> otherEffectStrengths;
    
    // Predictive model parameters
    std::vector<float> forwardModelWeights;
    std::vector<float> sensoryMotorMappings;
    
    // Integration state
    bool isActive;
    float predictionAccuracy;
    float selfOtherDistinction;
    
    // Social learning integration
    std::unordered_set<size_t> learnedFromSocialObservations;
    std::vector<float> socialBehaviorModel;
    
    // State tracking
    SimulationStep lastSelfAction;
    float currentActionEffectPrediction;
    float bodyStateConfidence;
    
    SelfModelCheckpoint()
        : isActive(false), predictionAccuracy(0), selfOtherDistinction(0)
        , currentActionEffectPrediction(0), bodyStateConfidence(0) {}
};

// SelfModel save/load interface
class SelfModelSaverLoader {
public:
    virtual ~SelfModelSaverLoader() = default;
    
    // Save self-model state
    virtual bool save(const SelfModelCheckpoint& checkpoint) const = 0;
    
    // Load self-model state
    virtual bool load(SelfModelCheckpoint& checkpoint) = 0;
    
    // Create empty checkpoint
    virtual SelfModelCheckpoint createEmptyCheckpoint() const = 0;
    
    // Get action-effect prediction for checkpoint validation
    virtual float getActionEffectPrediction(const ActionEffectMappingCheckpoint& mapping) const = 0;
};

} // namespace nlm
