// ConceptFormation checkpoint data structures

#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <memory>
#include "../core/Types/Types.hpp"

namespace nlm {

// ConceptInstance for checkpointing
struct ConceptInstanceCheckpoint {
    // The neural activity pattern
    std::vector<float> pattern;
    
    // Associated features
    std::vector<float> features;
    
    // Lifecycle information
    size_t observationCount;
    SimulationStep lastObserved;
    float avgReward;
    float stability;
    
    // Creation context
    float temporalNovelty;
    float predictionError;
    
    ConceptInstanceCheckpoint()
        : observationCount(0), lastObserved(0), avgReward(0), stability(0)
        , temporalNovelty(0), predictionError(0) {}
};

// DiscoveredConcept for checkpointing
struct DiscoveredConceptCheckpoint {
    // Concept identity
    size_t id;
    
    // Core prototype pattern
    std::vector<float> prototype;
    
    // All instances
    std::vector<ConceptInstanceCheckpoint> instances;
    
    // Category information
    std::string categoryHint;
    std::vector<size_t> associatedConceptIds;
    
    // Statistics
    size_t totalObservations;
    float avgStability;
    std::vector<float> categoryVector;
    float categoryEntropy;
    
    // Formation context
    SimulationStep formationTime;
    float formationNovelty;
    bool isMerged;
    
    DiscoveredConceptCheckpoint()
        : id(0), totalObservations(0), avgStability(0), categoryEntropy(0)
        , formationTime(0), formationNovelty(0), isMerged(false) {}
};

// ConceptFormation checkpoint state
struct ConceptFormationCheckpoint {
    // All discovered concepts
    std::vector<DiscoveredConceptCheckpoint> concepts;
    
    // Concept registry (id -> concept)
    std::unordered_map<size_t, DiscoveredConceptCheckpoint> conceptRegistry;
    
    // System statistics
    size_t nextConceptId;
    size_t conceptCount;
    float formationThreshold;
    float stabilityThreshold;
    size_t stabilityWindow;
    size_t memoryCapacity;
    
    // Novelty tracking
    std::vector<float> noveltyHistory;
    size_t novelConceptsCount;
    
    // Integration state
    std::vector<float> integratedAttentionState;
    std::vector<float> integratedPredictionState;
    std::vector<float> integratedWorkingMemoryState;
    
    // Temporal dynamics
    float conceptAgingRate;
    size_t conceptPruningThreshold;
    SimulationStep lastConceptUpdate;
    
    // Learning statistics
    float conceptLearningRate;
    float prototypeLearningRate;
    float categoryEvolutionRate;
    
    ConceptFormationCheckpoint()
        : nextConceptId(0), conceptCount(0), formationThreshold(0.7f), stabilityThreshold(0.6f)
        , stabilityWindow(50), memoryCapacity(100), novelConceptsCount(0)
        , conceptAgingRate(0.01f), conceptPruningThreshold(20), lastConceptUpdate(0)
        , conceptLearningRate(0.1f), prototypeLearningRate(0.05f), categoryEvolutionRate(0.02f) {}
};

// ConceptFormation save/load interface
class ConceptFormationSaverLoader {
public:
    virtual ~ConceptFormationSaverLoader() = default;
    
    // Save concept formation state
    virtual bool save(const ConceptFormationCheckpoint& checkpoint) const = 0;
    
    // Load concept formation state
    virtual bool load(ConceptFormationCheckpoint& checkpoint) = 0;
    
    // Create empty checkpoint
    virtual ConceptFormationCheckpoint createEmptyCheckpoint() const = 0;
    
    // Get concept stability for checkpoint validation
    virtual float getConceptStability(const DiscoveredConceptCheckpoint& concept) const = 0;
};

} // namespace nlm
