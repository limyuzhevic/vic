#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <string>

namespace nlm {

// ConceptInstance: Represents a single instance of a concept
struct ConceptInstance {
    // The neural activity pattern representing this instance
    std::vector<float> pattern;
    
    // Associated sensory features
    std::vector<float> features;
    
    // How many times this instance has been observed
    size_t observationCount;
    
    // Last time this was observed
    SimulationStep lastObserved;
    
    // Average reward when this was observed
    float avgReward;
    
    // Stability: how consistent is this pattern
    float stability;
    
    ConceptInstance()
        : observationCount(0), lastObserved(0), avgReward(0), stability(0) {}
};

// DiscoveredConcept: A concept that emerged from repeated experience
// NOT a predefined label - created through Hebbian clustering
struct DiscoveredConcept {
    // Unique identifier
    size_t id;
    
    // Central prototype pattern
    std::vector<float> prototype;
    
    // Instances that belong to this concept
    std::vector<ConceptInstance> instances;
    
    // Category label (derived from properties, not predefined)
    // e.g., "edible", "dangerous", "rewarding"
    std::string categoryHint;
    
    // Associated concepts (through associative memory)
    std::vector<size_t> associatedConceptIds;
    
    // How many different instances this concept has absorbed
    size_t totalObservations;
    
    // Average stability of instances
    float avgStability;
    
    DiscoveredConcept() 
        : id(0), totalObservations(0), avgStability(0) {}
};

// ConceptFormation: Discovers recurring patterns from experience
// NOT a predefined taxonomy - concepts emerge through repeated experience
//
// Key mechanisms:
// - Pattern clustering through similarity
// - Prototype formation through averaging
// - Instance tracking for stability measurement
// - Category discovery based on shared properties
// - No predefined labels or semantic categories

class ConceptFormation {
public:
    ConceptFormation();
    ~ConceptFormation();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Present a new experience to the concept formation system
    // Returns concept ID if this experience belongs to an existing concept,
    // or 0 if it's too early to tell, or new concept ID if novel
    size_t presentExperience(const std::vector<float>& pattern,
                            const std::vector<float>& features,
                            float reward,
                            SimulationStep currentTime);

    // Get the concept that best matches a pattern
    size_t getMatchingConcept(const std::vector<float>& pattern,
                              float similarityThreshold = 0.7f) const;

    // Get all concepts
    const std::vector<DiscoveredConcept>& getConcepts() const { return concepts_; }

    // Get a specific concept
    const DiscoveredConcept* getConcept(size_t conceptId) const;

    // Get concept prototype
    std::vector<float> getConceptPrototype(size_t conceptId) const;

    // Get concept instances
    const std::vector<ConceptInstance>& getConceptInstances(size_t conceptId) const;

    // Measure concept stability (0-1)
    float getConceptStability(size_t conceptId) const;

    // Get number of concepts discovered
    size_t getConceptCount() const { return concepts_.size(); }

    // Merge two concepts (when discovered they are actually the same)
    void mergeConcepts(size_t conceptA, size_t conceptB);

    // Update concept with new instance
    void updateConcept(size_t conceptId, const std::vector<float>& newPattern,
                      const std::vector<float>& features, float reward);

    // Compute similarity between two patterns
    float computeSimilarity(const std::vector<float>& a,
                          const std::vector<float>& b) const;

    // Check if pattern is novel (doesn't belong to any existing concept)
    bool isNovel(const std::vector<float>& pattern,
                 float similarityThreshold = 0.7f) const;

    // Enhanced ConceptFormation integration with sensory processing
    // Process comprehensive sensory state information with working memory integration
    void processSensoryState(const std::vector<float>& sensoryState,
                           const std::vector<float>& features,
                           float reward,
                           SimulationStep currentTime);
    
    // Concept abstraction and learning integration
    // Abstract concepts from episodic memory for higher-level understanding
    void abstractFromEpisodicMemory();
    
    // Enhanced concept storage and retrieval
    // Store concept in episodic memory for consolidation
    void storeConceptInEpisodicMemory(size_t conceptId);
    
    // Integration with neural planner for abstract goal setting
    void integrateWithNeuralPlanner();
    
    // Integration with neuromodulation for concept novelty detection
    void integrateWithNeuromodulation();
    
    // Integration with attention system for concept salience
    void integrateWithAttentionSystem();
    
    // Configuration methods
    void setAbstractionEnabled(bool enabled) { abstractionEnabled_ = enabled; }
    void setNoveltyDetectionEnabled(bool enabled) { noveltyDetectionEnabled_ = enabled; }
    void setWorkingMemoryIntegrationEnabled(bool enabled) { workingMemoryIntegrationEnabled_ = enabled; }
    void setAttentionIntegrationEnabled(bool enabled) { attentionIntegrationEnabled_ = enabled; }
    void setPredictionIntegrationEnabled(bool enabled) { predictionIntegrationEnabled_ = enabled; }
    void setPlannerIntegrationEnabled(bool enabled) { plannerIntegrationEnabled_ = enabled; }
    void setEpisodicMemoryStorageEnabled(bool enabled) { episodicMemoryStorageEnabled_ = enabled; }

private:
    // Additional integration flags
    bool abstractionEnabled_;
    bool noveltyDetectionEnabled_;
    bool workingMemoryIntegrationEnabled_;
    bool attentionIntegrationEnabled_;
    bool predictionIntegrationEnabled_;
    bool plannerIntegrationEnabled_;
    bool episodicMemoryStorageEnabled_;

    // Helper methods for enhanced integration
    void updateConceptForPredictionError(float predictionError);
    void applyAttentionToConceptFormation();
    void storeConceptFeaturesInEpisodicMemory();
    void refineConceptsBasedOnNeuromodulation(float neuromodulationSignal);
    std::vector<float> computePatternAbstraction(const std::vector<std::vector<float>>& patterns);
    std::vector<float> extractFeaturesFromEpisodes(const std::vector<const EpisodicMemoryItem*>& episodes);
    void enhanceConceptWithEpisodicContext(size_t conceptId, const std::vector<const EpisodicMemoryItem*>& episodes);
    float calculateConceptNovelty(size_t conceptId);
    size_t createConceptFromPattern(const std::vector<float>& pattern,
                                  const std::vector<float>& features,
                                  float reward);


    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    std::vector<DiscoveredConcept> concepts_;
    size_t nextConceptId_;
    
    // Parameters
    float formationThreshold_;  // Similarity threshold for forming new concept
    float stabilityThreshold_;   // Stability needed to be considered stable
    size_t stabilityWindow_;    // How many observations to consider for stability
};

// SpatialRepresentation: Learns spatial relationships from experience
// NOT a map with coordinates - encodes spatial relationships in neural patterns
//
// Key mechanisms:
// - Place neurons form through spatial exploration
// - Spatial relationships encoded in synaptic connectivity
// - Path integration through dead reckoning
// - Landmark associations

class SpatialRepresentation {
public:
    SpatialRepresentation();
    ~SpatialRepresentation();

    void initialize(Brain* brain);

    // Record current position experience
    void recordPosition(float x, float y, const std::vector<float>& sensoryCues);

    // Get predicted position based on path integration
    std::pair<float, float> getPredictedPosition() const;

    // Update position estimate based on movement
    void integrateMovement(float dx, float dy);

    // Get position neuron activation for a given position
    float getPositionActivation(float x, float y) const;

    // Find place neurons near a position
    std::vector<NeuronId> getPlaceNeuronsNear(float x, float y, float radius) const;

    // Get grid spacing (if grid cells have formed)
    float getGridSpacing() const { return gridSpacing_; }

    // Clear spatial representations
    void clear();

    // Have place cells formed?
    bool hasPlaceCells() const { return !placeCells_.empty(); }

private:
    struct PlaceCell {
        NeuronId neuron;
        float x;
        float y;
        float fieldRadius;
        float activation;
    };

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    std::vector<PlaceCell> placeCells_;
    float currentX_;
    float currentY_;
    float gridSpacing_;
    std::vector<float> lastSensoryCue_;
};

// TemporalRelation: Learns temporal relationships (before, after, sequence)
// NOT symbolic timestamps - encodes temporal order in neural activity

class TemporalRelation {
public:
    TemporalRelation();
    ~TemporalRelation();

    void initialize(Brain* brain);

    // Record that event A was followed by event B
    void recordSequence(const std::vector<float>& eventA,
                      const std::vector<float>& eventB,
                      float reward);

    // Given event A, predict what comes next
    std::vector<float> predictNext(const std::vector<float>& eventA) const;

    // Given event B, predict what came before
    std::vector<float> predictPrevious(const std::vector<float>& eventB) const;

    // Get confidence in prediction
    float getPredictionConfidence(const std::vector<float>& event) const;

    // Get sequence length
    size_t getSequenceLength() const { return sequences_.size(); }

    // Clear sequences
    void clear();

private:
    struct SequenceLink {
        std::vector<float> fromEvent;
        std::vector<float> toEvent;
        float strength;
        size_t observationCount;
    };

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    std::vector<SequenceLink> sequences_;
};

} // namespace nlm