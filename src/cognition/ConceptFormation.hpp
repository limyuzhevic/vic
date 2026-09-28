#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <string>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <unordered_map>

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
    
    // Feature prototype
    std::vector<float> featurePrototype;
    
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

    // Get concept for a given pattern
    size_t findConceptForPattern(const std::vector<float>& pattern) const;

    // Clear all concepts
    void clear();

    // Get generalization ability (can concept handle variations?)
    float getGeneralizationAbility(size_t conceptId) const;

    // Process neural activity patterns from working memory to form concepts
    // Returns true if concepts were updated/formed, false otherwise
    bool processExperience(const std::vector<float>& sensoryPattern);

    // Get concepts that are relevant for goal-directed behavior
    void getRelevantConceptsForGoals(std::vector<size_t>& conceptIds) const;

    // Enhanced feature extraction and concept formation

    // Extract statistical features from a sensory pattern
    std::vector<float> extractFeatures(const std::vector<float>& pattern) const;

    // Compute feature distance between two patterns
    float computeFeatureDistance(const std::vector<float>& featuresA,
                                const std::vector<float>& featuresB) const;

    // Find similar concepts based on feature space
    size_t findSimilarConceptByFeatures(
        const std::vector<float>& features,
        float similarityThreshold = 0.7f) const;

    // Update concept stability based on feature consistency
    void updateConceptFeatureStability(size_t conceptId);

    // Get concept relevance to prediction system goals
    float getConceptRelevanceToGoals(size_t conceptId) const;

    // Set parameters
    void setFormationThreshold(float t) { formationThreshold_ = t; }
    void setStabilityWindow(size_t w) { stabilityWindow_ = w; }

    // Feature extraction parameters
    void setFeatureVectorSize(size_t s) { featureVectorSize_ = s; }
    void setTemporalPatternHistory(size_t h) { temporalPatternHistory_ = h; }
    void setFeatureSimilarityThreshold(float t) { featureSimilarityThreshold_ = t; }
    void setTemporalPatternThreshold(float t) { temporalPatternThreshold_ = t; }
    void setConceptFeatureStabilityWeight(float w) { conceptFeatureStabilityWeight_ = w; }

private:
    // Feature extraction and temporal analysis
    std::vector<float> extractStatisticalFeatures(const std::vector<float>& pattern) const;
    std::vector<float> extractPeakFeatures(const std::vector<float>& pattern) const;
    std::vector<float> extractSpatialFeatures(const std::vector<float>& pattern) const;
    std::vector<float> extractTemporalFeatures(
        const std::vector<std::vector<float>>& patternHistory,
        size_t lookbackWindow = 10) const;

    // Feature matching and concept formation
    float computeFeatureSimilarity(
        const std::vector<float>& featuresA,
        const std::vector<float>& featuresB) const;
    
    // Feature-based concept updates
    void updateConceptWithFeatures(size_t conceptId,
                                  const std::vector<float>& newPattern,
                                  const std::vector<float>& newFeatures,
                                  float reward,
                                  float predictionError);
    void updateConceptFeaturePrototype(size_t conceptId,
                                     const std::vector<float>& newFeatures);
    void updateConceptFeatureStability(size_t conceptId);

    // Enhanced relevance filtering
    float getConceptRelevanceToGoals(size_t conceptId) const;
    bool isConceptFresh(size_t conceptId, SimulationStep currentTime) const;
    bool meetsGoalRelevanceCriteria(size_t conceptId) const;

    // Temporal pattern storage and analysis
    struct TemporalPattern {
        std::vector<std::vector<float>> patternHistory;
        std::vector<float> featureHistory;
        SimulationStep firstObserved;
        SimulationStep lastObserved;
        float avgPredictionError;
        float stability;
    };
    
    std::unordered_map<size_t, TemporalPattern> temporalPatterns_;

    // Feature-based concept matching
    struct FeatureVector {
        std::vector<float> features;
        size_t conceptId;
        SimulationStep lastObserved;
        float predictionError;
        float relevanceScore;
    };
    
    std::vector<FeatureVector> featureSpace_;

    // Parameters
    size_t featureVectorSize_;           // Number of features to extract
    size_t temporalPatternHistory_;      // How many past patterns to analyze
    float featureSimilarityThreshold_;   // Feature similarity threshold
    float temporalPatternThreshold_;     // Threshold for temporal pattern detection
    float conceptFeatureStabilityWeight_; // Weight for feature stability in concept stability

    Brain* brain_;
    std::vector<DiscoveredConcept> concepts_;
    size_t nextConceptId_;
    
    // Parameters
    float formationThreshold_;  // Similarity threshold for forming new concept
    float stabilityThreshold_;   // Stability needed to be considered stable
    size_t stabilityWindow_;    // How many observations to consider for stability
};

} // namespace nlm
