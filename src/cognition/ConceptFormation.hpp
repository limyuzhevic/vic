#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../sensory/SensoryInput.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <random>

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
    
    // Pattern variance (for concept stability analysis)
    std::vector<float> variance;
    
    // Time since first observation (for decay)
    SimulationStep age;
    
    ConceptInstance()
        : observationCount(0), lastObserved(0), avgReward(0), stability(0), age(0) {}
};

// DiscoveredConcept: A concept that emerged from repeated experience
// NOT a predefined label - created through Hebbian clustering
struct DiscoveredConcept {
    // Unique identifier
    size_t id;
    
    // Central prototype pattern (Hebbian-averaged)
    std::vector<float> prototype;
    
    // Mean and covariance for Mahalanobis distance
    std::vector<float> meanPattern;
    std::vector<std::vector<float>> covarianceMatrix;
    
    // Instances that belong to this concept
    std::vector<ConceptInstance> instances;
    
    // Category label (derived from properties, not predefined)
    // e.g., "edible", "dangerous", "rewarding", "familiar"
    std::string categoryHint;
    
    // Category confidence (0-1)
    float categoryConfidence;
    
    // Associated concepts (through associative memory)
    std::vector<size_t> associatedConceptIds;
    
    // How many different instances this concept has absorbed
    size_t totalObservations;
    
    // Average stability of instances
    float avgStability;
    
    // Pattern variance across instances
    std::vector<float> patternVariance;
    
    // Creation time
    SimulationStep creationTime;
    
    // Last stability update
    SimulationStep lastStabilityUpdate;
    
    DiscoveredConcept() 
        : id(0), totalObservations(0), avgStability(0), categoryConfidence(0), 
          creationTime(0), lastStabilityUpdate(0) {}
};

// PatternCluster: Internal structure for clustering
struct PatternCluster {
    std::vector<size_t> instanceIndices;
    std::vector<float> center;
    float radius;
    float weight;
    SimulationStep lastUpdate;
    
    PatternCluster() : radius(0), weight(1.0f), lastUpdate(0) {}
};

// HebbianUpdate: Configuration for Hebbian learning mechanisms
struct HebbianUpdate {
    float learningRate;      // Alpha (0-1)
    float decayRate;         // Gamma (0-1)
    float eligibilityTrace;  // How long influences last
    float consolidationRate; // How fast to consolidate
    
    HebbianUpdate()
        : learningRate(0.1f), decayRate(0.01f), eligibilityTrace(10),
          consolidationRate(0.05f) {}
};

// ConceptStabilityMetrics: Track concept stability over time
struct ConceptStabilityMetrics {
    float patternStability;      // Similarity to prototype
    float instanceStability;     // Consistency of instances
    float rewardStability;       // Reward consistency
    float temporalStability;     // Persistence over time
    float overallStability;      // Combined stability score
    
    ConceptStabilityMetrics()
        : patternStability(0), instanceStability(0), rewardStability(0),
          temporalStability(0), overallStability(0) {}
};

// AutomaticCategory: Configuration for category discovery
struct AutomaticCategory {
    std::string name;
    std::function<bool(const DiscoveredConcept&)> detector;
    float confidenceThreshold;
    float weight;               // How important this category is
    
    AutomaticCategory(const std::string& n = "", 
                     std::function<bool(const DiscoveredConcept&)> d = nullptr,
                     float conf = 0.5f, float w = 1.0f)
        : name(n), detector(d), confidenceThreshold(conf), weight(w) {}
};

// ConceptFormation: Discovers recurring patterns from experience
// NOT a predefined taxonomy - concepts emerge through repeated experience
//
// Key mechanisms:
// - Pattern clustering through similarity
// - Prototype formation through Hebbian averaging
// - Instance tracking for stability measurement
// - Category discovery based on shared properties
// - Integration with brain's sensory processing pipeline
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

    // Present a new experience from sensory input
    size_t presentSensoryExperience(const SensoryInput& sensoryInput,
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

    // Compute similarity between two patterns using different metrics
    float computeSimilarity(const std::vector<float>& a,
                          const std::vector<float>& b,
                          int metric = 0) const; // 0: cosine, 1: euclidean, 2: mahalanobis

    // Check if pattern is novel (doesn't belong to any existing concept)
    bool isNovel(const std::vector<float>& pattern,
                 float similarityThreshold = 0.7f) const;

    // Get concept for a given pattern
    size_t findConceptForPattern(const std::vector<float>& pattern) const;

    // Clear all concepts
    void clear();

    // Get generalization ability (can concept handle variations?)
    float getGeneralizationAbility(size_t conceptId) const;

    // Get concept stability metrics
    ConceptStabilityMetrics getConceptStabilityMetrics(size_t conceptId) const;

    // Get automatic categories derived from patterns
    std::vector<std::string> getConceptCategories(size_t conceptId) const;

    // Force concept formation (even if not stable)
    size_t forceConceptFormation(const std::vector<float>& pattern,
                                const std::vector<float>& features,
                                float reward);

    // Set parameters
    void setFormationThreshold(float t) { formationThreshold_ = t; }
    void setStabilityThreshold(float t) { stabilityThreshold_ = t; }
    void setStabilityWindow(size_t w) { stabilityWindow_ = w; }
    void setHebbianLearningRate(float rate) { hebbianLearningRate_ = rate; }
    void setMaxClusters(size_t max) { maxClusters_ = max; }
    void setEnableAutomaticCategoryDiscovery(bool enable) { enableAutomaticCategoryDiscovery_ = enable; }
    void setSimilarityMetric(int metric) { defaultSimilarityMetric_ = metric; }

    // Add automatic category detector
    void addAutomaticCategory(const std::string& name,
                            std::function<bool(const DiscoveredConcept&)> detector,
                            float confidence = 0.5f,
                            float weight = 1.0f);

    // Cluster patterns using advanced algorithms
    void performClusterMaintenance(SimulationStep currentTime);
    void performHebbianConsolidation(SimulationStep currentTime);
    void updatePatternStatistics(size_t conceptId);
    void discoverCategories();

private:
    // Create new concept from pattern
    size_t createConcept(const std::vector<float>& pattern,
                        const std::vector<float>& features,
                        float reward,
                        SimulationStep currentTime);

    // Update concept prototype using Hebbian averaging
    void updatePrototype(size_t conceptId, const std::vector<float>& newInstance,
                        SimulationStep currentTime);

    // Check if concept is stable enough to be considered "discovered"
    bool isConceptStable(size_t conceptId, SimulationStep currentTime) const;

    // Real pattern clustering algorithms
    size_t performClustering(const std::vector<float>& pattern);
    void kmeansLikeClustering(SimulationStep currentTime);
    void updateClusterCenters(size_t conceptId);
    float computePatternDistance(const std::vector<float>& a,
                                const std::vector<float>& b,
                                size_t conceptId) const;

    // Instance tracking and stability
    void addInstanceToConcept(size_t conceptId, const std::vector<float>& pattern,
                            const std::vector<float>& features, float reward,
                            SimulationStep currentTime);
    void updateInstanceStability(size_t conceptId, size_t instanceIndex,
                                SimulationStep currentTime);
    void decayOldInstances(SimulationStep currentTime);
    void consolidateStableConcepts(SimulationStep currentTime);

    // Hebbian learning mechanisms
    void hebbianUpdate(size_t conceptId, const std::vector<float>& pattern,
                      float reward, SimulationStep currentTime);
    void applyEligibilityTraces(SimulationStep currentTime);
    void updateConnectionStrength(size_t conceptId, const std::vector<float>& pattern,
                                 float reward, SimulationStep currentTime);

    // Automatic category discovery
    void analyzePatternProperties(size_t conceptId);
    void deriveCategoryHint(size_t conceptId);
    bool isRewardingPattern(float reward, const DiscoveredConcept& concept) const;
    bool isHarmfulPattern(float reward, const DiscoveredConcept& concept) const;
    bool isFamiliarPattern(const DiscoveredConcept& concept) const;

    // Sensory pattern processing
    std::vector<float> extractSensoryFeatures(const SensoryInput& sensoryInput) const;
    std::vector<float> convertToPattern(const SensoryInput& sensoryInput) const;

    // Stability analysis
    void computePatternVariance(size_t conceptId);
    void updateConceptStability(size_t conceptId, SimulationStep currentTime);
    void measureTemporalStability(size_t conceptId, SimulationStep currentTime);

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    std::vector<DiscoveredConcept> concepts_;
    size_t nextConceptId_;
    
    // Configuration parameters
    float formationThreshold_;      // Similarity threshold for forming new concept
    float stabilityThreshold_;       // Stability needed to be considered stable
    size_t stabilityWindow_;        // How many observations to consider for stability
    float hebbianLearningRate_;      // Hebbian learning rate (alpha)
    size_t maxClusters_;            // Maximum number of clusters per concept
    bool enableAutomaticCategoryDiscovery_;  // Whether to auto-discover categories
    int defaultSimilarityMetric_;   // Default similarity metric (0:cosine, 1:euclidean, 2:mahalanobis)
    
    // Automatic category detectors
    std::vector<AutomaticCategory> automaticCategories_;
    
    // Pattern cache for recent experiences
    std::vector<std::vector<float>> patternHistory_;
    SimulationStep lastConsolidationStep_;
    
    // Statistical properties
    std::unordered_map<size_t, std::vector<float>> conceptMeans_;
    std::unordered_map<size_t, std::vector<std::vector<float>>> conceptCovariances_;
};

// Utility functions for concept formation
namespace ConceptFormationUtils {
    float euclideanDistance(const std::vector<float>& a, const std::vector<float>& b);
    float cosineSimilarity(const std::vector<float>& a, const std::vector<float>& b);
    float mahalanobisDistance(const std::vector<float>& a, const std::vector<float>& b,
                            const std::vector<float>& mean,
                            const std::vector<std::vector<float>>& cov);
    std::vector<float> computeMean(const std::vector<std::vector<float>>& patterns);
    std::vector<std::vector<float>> computeCovariance(const std::vector<std::vector<float>>& patterns,
                                                    const std::vector<float>& mean);
    float computePatternVariance(const std::vector<float>& pattern, const std::vector<float>& mean);
    bool isPatternSimilar(const std::vector<float>& a, const std::vector<float>& b,
                         float threshold, int metric = 0);
}}

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
        
        // Spatial relationships
        std::vector<float> spatialSignature;
        float orientation;
    };

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    std::vector<PlaceCell> placeCells_;
    float currentX_;
    float currentY_;
    float gridSpacing_;
    std::vector<float> lastSensoryCue_;
    
    // Path integration state
    float pathX_;
    float pathY_;
    float pathOrientation_;
    std::vector<float> integrationHistory_;
};

// TemporalRelation: Learns temporal relationships (before, after, sequence)
// NOT symbolic timestamps - encodes temporal order in neural activity
//
// Key mechanisms:
// - Sequence links form through repeated temporal patterns
// - Predict temporal order based on neural patterns
// - Confidence in predictions based on observation count
// - Causal inference through correlation

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
        SimulationStep lastObserved;
        float confidence;
        
        // Temporal properties
        float meanInterval;
        float intervalVariance;
    };

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    std::vector<SequenceLink> sequences_;
    
    // Temporal analysis
    SimulationStep currentTime_;
    float timeScale_;
};

} // namespace nlm

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