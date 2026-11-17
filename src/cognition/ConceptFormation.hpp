#pragma once

#include "../core/Types/Types.hpp"
#include "../motor/Action.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <string>

namespace nlm {

struct ConceptMetaData {
    // Self-awareness metrics
    float selfConfidence = 0.0f;      // How reliable is this concept?
    float stabilityBoost = 0.0f;      // Extra stability from meta-cognition
    
    // Action selection metrics
    float usefulnessScore = 0.0f;     // How useful is this concept for behavior?
    float priority = 0.0f;            // Priority in action selection
    float relevanceWeight = 1.0f;     // Weight in decision making
    
    // Learning metrics
    float noveltyThreshold = 0.0f;    // Sensitivity to novelty
    SimulationStep lastUpdated = 0;   // When was this updated?
    
    ConceptMetaData() : selfConfidence(0), stabilityBoost(0), usefulnessScore(0),
                       priority(0), relevanceWeight(1), noveltyThreshold(0), lastUpdated(0) {}
};

// ConceptMetaData: Stores meta-cognitive information about concepts
// NOT a higher-level theory of mind - just utility for action selection
//
// Key functions:
// - Tracks concept reliability and confidence
// - Determines concept usefulness for behavior
// - Enables selective attention to important concepts
// - Supports adaptive concept abstraction

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
    
    // Meta-cognitive data
    ConceptMetaData metaData;
    
    // Abstraction level (0 = concrete, 1 = abstract)
    float abstractionLevel;
    
    DiscoveredConcept() 
        : id(0), totalObservations(0), avgStability(0), abstractionLevel(0) {}
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
//
// Enhanced with meta-cognitive capabilities:
// - Concept reliability tracking
// - Active concept selection
// - Meta-awareness and stability assessment
// - Abstraction level adjustment
// - Action selection strategy optimization

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

    // Get concept for a given pattern
    size_t findConceptForPattern(const std::vector<float>& pattern) const;

    // Clear all concepts
    void clear();

    // Get generalization ability (can concept handle variations?)
    float getGeneralizationAbility(size_t conceptId) const;

    // Set parameters
    void setFormationThreshold(float t) { formationThreshold_ = t; }
    void setStabilityWindow(size_t w) { stabilityWindow_ = w; }

    // Meta-cognitive enhancement methods
    void enhanceWithMetaCognition();
    void detectNovelty(const std::vector<float>& pattern);
    void enhanceConceptAbstraction(size_t conceptId);
    void updateMetaData(size_t conceptId);
    void prepareForLearning();
    void updateActionSelectionStrategy();

private:
    // Create new concept from pattern
    size_t createConcept(const std::vector<float>& pattern,
                        const std::vector<float>& features,
                        float reward);

    // Update concept prototype using Hebbian averaging
    void updatePrototype(size_t conceptId, const std::vector<float>& newInstance);

    // Check if concept is stable enough to be considered "discovered"
    bool isConceptStable(size_t conceptId) const;

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    std::vector<DiscoveredConcept> concepts_;
    size_t nextConceptId_;
    
    // Meta-cognitive state
    std::vector<size_t> activeConcepts_;        // Currently active concepts for decision making
    std::vector<std::vector<float>> novelPatterns_;    // Buffer of novel patterns
    std::unordered_set<std::vector<float>> recentNovelties_;  // Recently observed novel patterns
    
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