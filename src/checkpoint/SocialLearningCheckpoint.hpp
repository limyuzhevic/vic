// SocialLearning checkpoint data structures

#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include "../core/Types/Types.hpp"

namespace nlm {

// SocialObservation for checkpointing
struct SocialObservationCheckpoint {
    // Observed agent action
    ActionType observedAction;
    
    // Action context
    std::vector<float> actionContext;
    
    // Observation metadata
    SimulationStep observationTime;
    float reward;
    float energy;
    
    // Observation quality
    float reliability;
    std::string observerId;
    
    // Resulting state
    std::vector<float> resultingState;
    float resultingReward;
    
    // Social relevance
    float socialValue;
    bool is Imitatable;
    
    // Integration fields
    std::vector<float> actionSignature;
    std::unordered_set<size_t> relatedSelfActions;
    
    SocialObservationCheckpoint()
        : observedAction(ActionType::Wait), reward(0), energy(0)
        , reliability(0), observationTime(0), isImitatable(false), socialValue(0) {}
};

// SocialKnowledge for checkpointing
struct SocialKnowledgeCheckpoint {
    // Observed actions and their outcomes
    std::vector<SocialObservationCheckpoint> observations;
    
    // Social rules/laws learned from observation
    std::vector<std::vector<float>> socialRules;
    
    // Imitation patterns
    std::unordered_map<ActionType, ActionType> imitationMap;
    std::unordered_map<ActionType, float> imitationStrengths;
    
    // Social statistics
    size_t totalObservations;
    size_t imitatedActions;
    float socialLearningRate;
    float imitationAccuracy;
    
    // Integration with other systems
    std::unordered_set<size_t> episodesObserved;
    std::unordered_set<size_t> conceptsSociallyLearned;
    std::unordered_set<size_t> actionsSociallyModified;
    
    // State tracking
    SimulationStep lastSocialObservation;
    float currentSocialConfidence;
    std::string socialContext;
    
    // Neural integration
    std::vector<float> integratedNeuromodulationState;
    std::vector<float> integratedAttentionState;
    
    SocialKnowledgeCheckpoint()
        : totalObservations(0), imitatedActions(0), socialLearningRate(0.1f)
        , imitationAccuracy(0), currentSocialConfidence(0)
        , lastSocialObservation(0) {}
};

// SocialLearning checkpoint state
struct SocialLearningCheckpoint {
    // Current social knowledge
    SocialKnowledgeCheckpoint currentKnowledge;
    
    // Social network representation
    std::unordered_map<std::string, float> socialNetwork;
    
    // Cultural transmission
    std::vector<std::string> culturalTransmission;
    
    // Social parameters
    float observationDecayRate;
    float imitationThreshold;
    float socialReinforcementStrength;
    
    // State tracking
    bool hasSocialKnowledge;
    size_t activeSocialContexts;
    float socialArousal;
    
    // Integration with other systems
    std::vector<float> integratedSelfModelState;
    std::vector<float> integratedPredictionState;
    std::vector<float> integratedEpisodicState;
    
    // Learning statistics
    size_t socialRulesDiscovered;
    size_t culturalElementsAcquired;
    
    SocialLearningCheckpoint()
        : observationDecayRate(0.05f), imitationThreshold(0.6f), socialReinforcementStrength(0.8f)
        , hasSocialKnowledge(false), activeSocialContexts(0), socialArousal(0)
        , socialRulesDiscovered(0), culturalElementsAcquired(0) {}
};

// SocialLearning save/load interface
class SocialLearningSaverLoader {
public:
    virtual ~SocialLearningSaverLoader() = default;
    
    // Save social learning state
    virtual bool save(const SocialLearningCheckpoint& checkpoint) const = 0;
    
    // Load social learning state
    virtual bool load(SocialLearningCheckpoint& checkpoint) = 0;
    
    // Create empty checkpoint
    virtual SocialLearningCheckpoint createEmptyCheckpoint() const = 0;
    
    // Get social confidence for validation
    virtual float getSocialConfidence(const SocialKnowledgeCheckpoint& knowledge) const = 0;
};

} // namespace nlm
