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

    // Initialize with brain reference and prediction system
    void initialize(Brain* brain, PredictionSystem* predictionSystem);

    // Present a new experience to the concept formation system
    // Returns concept ID if this experience belongs to an existing concept,
    // 0 if it's too early to tell, or new concept ID if novel
    size_t presentExperience(const std::vector<float>& pattern,
                            const std::vector<float>& features,
                            float reward,
                            SimulationStep currentTime);

    // Get the concept that best matches a pattern
    // Uses neural similarity computation
    size_t getMatchingConcept(const std::vector<float>& pattern,
                              float similarityThreshold = 0.7f) const;

    // Get all concepts discovered so far
    const std::vector<DiscoveredConcept>& getConcepts() const { return concepts_; }

    // Get a specific concept by ID
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

    // Update concept with new instance using Hebbian learning
    void updateConcept(size_t conceptId, const std::vector<float>& newPattern,
                      const std::vector<float>& features, float reward);

    // Check if pattern is novel (doesn't belong to any existing concept)
    bool isNovel(const std::vector<float>& pattern,
                 float similarityThreshold = 0.7f) const;

    // Get concept for a given pattern
    size_t findConceptForPattern(const std::vector<float>& pattern) const;

    // Clear all concepts and reset learning
    void clear();

    // Get generalization ability (can concept handle variations?)
    float getGeneralizationAbility(size_t conceptId) const;

    // Set concept formation parameters
    void setFormationThreshold(float t) { formationThreshold_ = t; }
    void setStabilityWindow(size_t w) { stabilityWindow_ = w; }
    void setConceptStabilityThreshold(float t) { conceptStabilityThreshold_ = t; }

    // Enable/disable hierarchical concept formation
    void enableHierarchicalConceptFormation(bool enable) { hierarchicalConceptEnabled_ = enable; }

    // Get current concept statistics
    struct ConceptStats {
        size_t totalConcepts;
        size_t stableConcepts;  // Concepts above stability threshold
        float meanConceptStability;
        float meanConceptConfidence;
        float novelConcepts;  // New concepts formed recently
    };

    ConceptStats getStats() const;

    // Update concepts based on neural activity patterns
    void updateFromNeuralActivity(const std::vector<float>& neuralPattern,
                                 SimulationStep currentStep);

    // Integrate prediction errors into concept formation
    void integratePredictionError(const std::vector<float>& predicted,
                                 const std::vector<float>& actual,
                                 SimulationStep currentStep);

    // Concept competition and cooperation
    void updateConceptCompetition();
    void updateConceptCooperation(size_t conceptId);

    // Get concepts relevant for current goal
    std::vector<size_t> getRelevantConcepts(const std::vector<float>& goalState) const;

private:
    // Create new concept from pattern using neural dynamics
    size_t createConcept(const std::vector<float>& pattern,
                        const std::vector<float>& features,
                        float reward,
                        SimulationStep currentTime);

    // Update concept prototype using neural averaging (Hebbian-like)
    void updatePrototype(size_t conceptId, const std::vector<float>& newInstance);

    // Check if concept is stable enough to be considered "discovered"
    bool isConceptStable(size_t conceptId) const;

    // Compute similarity between two neural patterns
    float computeSimilarity(const std::vector<float>& a,
                          const std::vector<float>& b) const;

    // Learn concept clustering from neural activity
    void learnConceptClustering(const std::vector<std::vector<float>>& patterns);

    // Compute concept stability based on instance consistency
    float computeConceptStability(size_t conceptId) const;

    // Update concept confidence based on neuromodulation
    void updateConceptConfidence(size_t conceptId, float neuromodulationSignal);

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    PredictionSystem* predictionSystem_;
    std::vector<DiscoveredConcept> concepts_;
    size_t nextConceptId_;

    // Parameters
    float formationThreshold_;      // Similarity threshold for forming new concept
    float stabilityThreshold_;       // Stability needed to be considered stable
    size_t stabilityWindow_;        // How many observations to consider for stability
    float conceptStabilityThreshold_; // Threshold for concept consolidation
    bool hierarchicalConceptEnabled_;

    // Concept competition tracking
    std::vector<float> conceptCompetitions_;

    // Learning state
    std::deque<std::vector<float>> recentPatterns_;
    SimulationStep lastConceptUpdate_;
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