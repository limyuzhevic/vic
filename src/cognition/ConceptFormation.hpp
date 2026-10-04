#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <string>
#include <unordered_map>

namespace nlm {

// AttentionalState: Current attentional context for concept processing
struct AttentionalState {
    // Currently attended neural populations
    std::vector<NeuronId> attendedNeurons;
    
    // Regions currently in attentional focus
    std::vector<RegionId> attendedRegions;
    
    // Attentional strength (0-1)
    float attentionStrength;
    
    // Inhibitory focus level
    float inhibitionLevel;
    
    // Excitatory drive
    float excitationLevel;
    
    // Attentional predictions based on current focus
    std::vector<float> attentionalPredictions;
    
    // Time since attentional shift
    SimulationStep attentionalAge;
    
    // Concept formation modulation from attention
    float conceptLearningBoost;
    float patternSeparationBias;
    
    AttentionalState()
        : attentionStrength(0.0f), inhibitionLevel(0.0f), excitationLevel(0.0f),
          attentionalAge(0), conceptLearningBoost(1.0f), patternSeparationBias(1.0f) {}
};

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
    
    // Attentional relevance (how well attended this instance is)
    float attentionalRelevance;
    
    // Concept update prediction error
    float predictionError;
    
    ConceptInstance()
        : observationCount(0), lastObserved(0), avgReward(0), stability(0),
          attentionalRelevance(0.0f), predictionError(0.0f) {}
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
    
    // Attentional modulation factors
    float attentionalGain;
    float attentionalFocus;
    
    // Prediction error for concept refinement
    float accumulatedPredictionError;
    
    DiscoveredConcept() 
        : id(0), totalObservations(0), avgStability(0), attentionalGain(1.0f), 
          attentionalFocus(1.0f), accumulatedPredictionError(0.0f) {}
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
// ATTRIBUTES:
// - Bidirectional integration with attentional selection system
// - Top-down attentional modulation of concept learning
// - Bottom-up attentional capture by salient patterns
// - Neuromodulatory control of attentional strength and focus
// - Integration with prediction error for attention learning

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

    // ATENTION INTEGRATION METHODS

    // Update concept formation with attentional context
    void updateWithAttentionalContext(const AttentionalState& attentionalState);

    // Process sensory input with attentional modulation
    void processSensoryWithAttention(const std::vector<float>& sensoryState,
                                    const AttentionalState& attentionalState);

    // Get attentional predictions based on current concept prototypes
    std::vector<float> getAttentionalPredictions() const;

    // Integrate attentional predictions into concept updates
    void integrateAttentionalPredictions(const std::vector<float>& predictions);

    // Enhance learning based on attentional focus
    void enhanceLearningBasedOnAttention(float attentionStrength);

    // Apply attentional feedback to concept refinement
    void applyAttentionalFeedback(const AttentionalState& attentionalState);

    // Get attentional modulation factors for a specific concept
    float getAttentionalModulation(size_t conceptId) const;

    // Set attentional parameters
    void setAttentionalParameters(float gain, float focus);

private:
    // Create new concept from pattern
    size_t createConcept(const std::vector<float>& pattern,
                        const std::vector<float>& features,
                        float reward);

    // Update concept prototype using Hebbian averaging with attentional modulation
    void updatePrototype(size_t conceptId, const std::vector<float>& newInstance,
                        float attentionalWeight = 1.0f);

    // Check if concept is stable enough to be considered "discovered"
    bool isConceptStable(size_t conceptId) const;

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    std::vector<DiscoveredConcept> concepts_;
    size_t nextConceptId_;
    
    // Parameters
    float formationThreshold_;  // Similarity threshold for forming new concept
    float stabilityThreshold_;   // Stability needed to be considered stable
    size_t stabilityWindow_;    // How many observations to consider for stability
    
    // Attentional parameters
    float attentionalGain_;     // Overall attentional influence on learning
    float attentionalFocus_;    // How focused attention is (vs distributed)
};