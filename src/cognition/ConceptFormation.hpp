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
    
    // Advanced learning metrics
    float predictionError;           // Prediction error for learning
    float noveltyScore;              // How novel this instance is
    float confidence;                // Confidence in this instance
    float weight;                    // Instance importance weight
    float entropy;                   // Pattern entropy (variability)
    float coherence;                 // Pattern coherence with concept
    
    // Multi-scale feature vectors
    std::vector<std::vector<float>> multiScaleFeatures;
    
    // Temporal context
    SimulationStep timeStep;
    float temporalContext;
    
    ConceptInstance()
        : observationCount(0), lastObserved(0), avgReward(0), stability(0)
        , predictionError(0.0f), noveltyScore(0.0f), confidence(0.0f)
        , weight(1.0f), entropy(0.0f), coherence(0.0f), temporalContext(0.0f) {}
};

// DiscoveredConcept: Enhanced with advanced features
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
    
    // Hierarchy information
    size_t parentConceptId;           // Parent in specialization hierarchy
    std::vector<size_t> childConceptIds;  // Children in specialization hierarchy
    std::string conceptType;          // "abstract", "concrete", "intermediate"
    
    // Clustering information
    size_t clusterId;                 // Which cluster this concept belongs to
    std::vector<size_t> siblingConceptIds; // Concepts in same cluster
    
    // Advanced metrics
    float abstractionLevel;           // How abstract (0.0 to 1.0)
    float generalizationAbility;      // Can handle variations
    float specializationDepth;        // How specific (0.0 to 1.0)
    float consolidationLevel;         // How consolidated (0.0 to 1.0)
    float driftRate;                  // How fast concept changes
    float decayTimer;                 // Time until concept expiration
    float predictionError;           // Concept-level prediction error
    float confidence;                 // Overall confidence in concept
    float uncertainty;                // Uncertainty in concept
    float entropy;                    // Pattern entropy
    float noveltySum;                 // Sum of novelty scores
    float rewardHistory;              // Historical reward
    float stabilitySum;               // Sum of stabilities
    
    // Feature vectors
    std::vector<float> compressedFeatureVector;
    std::vector<std::vector<float>> multiScaleFeatureVectors;
    
    // Neural patterns and bindings
    std::vector<float> neuralPattern;   // Neural representation
    std::vector<size_t> boundFeatures;  // Features bound to this concept
    
    // Learning statistics
    size_t observationCount;         // How many different instances
    size_t totalObservations;         // Total observations (including repeats)
    size_t formationTime;             // When concept was formed
    SimulationStep lastUpdate;        // When last updated
    
    // Average stability of instances
    float avgStability;
    
    DiscoveredConcept() 
        : id(0), totalObservations(0), avgStability(0)
        , parentConceptId(0), clusterId(0), abstractionLevel(0.0f)
        , generalizationAbility(0.0f), specializationDepth(0.0f)
        , consolidationLevel(0.0f), driftRate(0.0f), decayTimer(0.0f)
        , predictionError(0.0f), confidence(0.0f), uncertainty(0.0f)
        , entropy(0.0f), noveltySum(0.0f), rewardHistory(0.0f), stabilitySum(0.0f)
        , observationCount(0), formationTime(0), lastUpdate(0) {}
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

    // Get concept hierarchy information
    size_t getParentConcept(size_t conceptId) const;
    const std::vector<size_t>& getChildConcepts(size_t conceptId) const;
    
    // Get concept clustering and grouping
    size_t getConceptCluster(size_t conceptId) const;
    const std::vector<size_t>& getClusterConcepts(size_t clusterId) const;
    
    // Get abstraction and generalization
    float getAbstractionLevel(size_t conceptId) const;
    float getGeneralizationAbility(size_t conceptId) const;
    float getSpecializationDepth(size_t conceptId) const;
    
    // Get meta-cognitive metrics
    float getConceptConfidence(size_t conceptId) const;
    float getConceptUncertainty(size_t conceptId) const;
    float getConceptNovelty(size_t conceptId) const;
    float getConceptPredictionError(size_t conceptId) const;
    
    // Get consolidation and decay
    float getConsolidationLevel(size_t conceptId) const;
    float getConceptDecayTimer(size_t conceptId) const;
    bool isConceptExpired(size_t conceptId) const;
    
    // Get advanced feature representations
    const std::vector<float>& getCompressedFeatureVector(size_t conceptId) const;
    const std::vector<std::vector<float>>& getMultiScaleFeatures(size_t conceptId) const;
    const std::vector<float>& getNeuralPattern(size_t conceptId) const;
    
    // Get concept stability over time
    float getConceptDriftRate(size_t conceptId) const;
    bool isConceptStable(size_t conceptId) const;
    
    // Get clustering and grouping statistics
    size_t getTotalClusters() const;
    size_t getActiveConceptsCount() const;
    
    // Set advanced parameters
    void setAnomalyThreshold(float t) { pImpl->anomalyThreshold_ = t; }
    void setDriftDetectionThreshold(float t) { pImpl->driftDetectionThreshold_ = t; }
    void setConceptDecayRate(float r) { pImpl->conceptDecayRate_ = r; }
    void setPredictionErrorThreshold(float t) { pImpl->predictionErrorThreshold_ = t; }
    void setNoveltyThreshold(float t) { pImpl->noveltyThreshold_ = t; }
    void setFeatureNormalizationFactor(float f) { pImpl->featureNormalizationFactor_ = f; }
    void setMaxClusterSize(size_t s) { pImpl->maxClusterSize_ = s; }
    void setFeatureHistorySize(size_t s) { pImpl->featureHistorySize_ = s; }
    
    // Get current active concepts
    size_t getActiveConceptsCount() const { return static_cast<size_t>(pImpl->activeConcepts_.size()); }
    
    // Get feature importance for a concept
    const std::vector<float>& getFeatureImportance(size_t conceptId) const {
        auto it = pImpl->featureImportanceWeights_.find(conceptId);
        static const std::vector<float> empty;
        return (it != pImpl->featureImportanceWeights_.end()) ? it->second : empty;
    }
    
    // Get concept associations
    const std::unordered_map<size_t, float>& getConceptAssociations(size_t conceptId) const {
        auto it = pImpl->conceptAssociations.find(conceptId);
        static const std::unordered_map<size_t, float> empty;
        return (it != pImpl->conceptAssociations.end()) ? it->second : empty;
    }
    
    // Get concept drift history
    const std::unordered_map<size_t, float>& getConceptDriftHistory() const {
        return pImpl->conceptDriftHistory;
    }
    
    // Get concept feature vectors
    const std::unordered_map<size_t, std::vector<float>>& getConceptFeatureVectors() const {
        return pImpl->featureVectors;
    }
    
    // Advanced learning methods
    void discoverConceptsThroughAnomaly(const std::vector<float>& pattern);
    void detectAndAdaptToDrift(size_t conceptId, float newSimilarity);
    void learnConceptCoOccurrences(const std::vector<float>& pattern1, const std::vector<float>& pattern2);
    void reinforceConcepts(const std::vector<float>& pattern, float reward);
    void updateFeatureImportance(const std::vector<float>& features);
    void bindFeaturesToConcepts(const std::vector<float>& pattern, const std::vector<size_t>& featureIndices);
    
    // Meta-cognitive methods
    void updateConceptConfidence(size_t conceptId, float error);
    void updateConceptUncertainty(size_t conceptId, float novelty);
    void decayConcepts();
    void consolidateConcepts();
    
    // Integration with other systems
    void integrateWithEpisodicMemory(size_t conceptId, const std::vector<float>& experience);
    void integrateWithWorkingMemory(size_t conceptId, float attention);
    void integrateWithNeuromodulation(size_t conceptId, float reward);
    void integrateWithAttention(size_t conceptId, float attentionWeight);
    
    // Concept composition and generalization
    size_t composeConcepts(size_t conceptA, size_t conceptB);
    size_t generalizeConcept(size_t conceptId, const std::vector<float>& novelPattern);
    std::vector<size_t> findSimilarConcepts(const std::vector<float>& pattern, float similarityThreshold = 0.7f) const;
    
private:
    // Advanced concept management
    void organizeConceptHierarchy();
    void clusterConcepts();
    void consolidateConceptsOverTime();
    
    // Advanced feature extraction and representation
    std::vector<std::vector<float>> extractMultiScaleFeatures(const std::vector<float>& pattern) const;
    std::vector<float> normalizeAndCompressFeatures(const std::vector<float>& features) const;
    void updateFeatureImportanceWeights(const std::vector<float>& features);
    std::vector<size_t> bindFeaturesWithNeuralPattern(const std::vector<float>& pattern, const std::vector<float>& prototype) const;
    
    // Concept retrieval and matching
    size_t retrieveTemplateBased(const std::vector<float>& pattern) const;
    size_t retrieveSimilarityBased(const std::vector<float>& pattern, float& bestSimilarity) const;
    size_t retrieveGeneralized(const std::vector<float>& pattern) const;
    
    // Meta-cognitive updates
    void updateMetaCognitiveMetrics(size_t conceptId, const std::vector<float>& pattern);
    void updateConceptPredictionError(size_t conceptId, const std::vector<float>& pattern);
    
    // Integration updates
    void updateEpisodicIntegration(size_t conceptId, const std::vector<float>& experience);
    void updateWorkingMemoryIntegration(size_t conceptId, float attention);
    void updateNeuromodulationIntegration(size_t conceptId, float reward);
    
    // Advanced pattern analysis
    float computeAnomalousSimilarity(const std::vector<float>& pattern, size_t conceptId) const;
    std::vector<float> predictConceptPattern(size_t conceptId) const;
    std::vector<size_t> getConceptAssociations(size_t conceptId) const;
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    std::vector<DiscoveredConcept> concepts_;
    size_t nextConceptId_;
    
    // Parameters
    float formationThreshold_;  // Similarity threshold for forming new concept
    float stabilityThreshold_;   // Stability needed to be considered stable
    size_t stabilityWindow_;    // How many observations to consider for stability
    
    // Advanced tracking
    size_t conceptCount_;
    size_t totalObservationCount_;
    std::vector<float> lastFeatureVector_;
    std::vector<std::vector<float>> featureVectorHistory_;
    
    // Active concept management
    std::unordered_set<size_t> activeConcepts_;
    std::unordered_map<size_t, float> attentionWeights_;
    std::unordered_map<size_t, std::vector<float>> featureImportanceWeights_;
    
    // Clustering and grouping
    std::unordered_map<size_t, size_t> conceptToCluster_;
    std::unordered_map<size_t, std::vector<size_t>> clusterToConcepts_;
    
    // Hierarchy and ancestry
    std::unordered_map<size_t, size_t> conceptToParent_;
    std::unordered_map<size_t, std::vector<size_t>> conceptToChildren_;
    std::unordered_map<size_t, std::vector<size_t>> ancestryMap_;
}

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