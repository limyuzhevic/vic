#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

// Enhanced AssociativeMemory with Fuzzy Pattern Completion
// Extends NeuralAssociativeMemory with continuous similarity matching
//
// Key mechanisms:
// - Continuous similarity scoring (0.0 to 1.0) instead of binary thresholds
// - Weighted recall based on similarity strength
// - Gradual memory activation based on pattern proximity
// - Multi-step spreading activation with similarity decay

class EnhancedAssociativeMemory {
public:
    EnhancedAssociativeMemory();
    ~EnhancedAssociativeMemory();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Create association between neural patterns with continuous strength
    // Patterns are automatically normalized for similarity comparison
    void associate(const std::vector<float>& patternA,
                   const std::vector<float>& patternB,
                   float strength = 1.0f);

    // Create association from episodic memory with enhanced learning
    void associateFromExperience(const class EpisodicMemoryItem& episode,
                                float noveltyBonus = 0.0f);

    // Retrieve patterns with fuzzy matching - returns weighted results
    // Scores results by similarity strength, not just binary matching
    std::vector<std::pair<std::vector<float>, float>> retrieveFuzzy(
        const std::vector<float>& queryPattern,
        size_t maxResults = 5) const;

    // Retrieve patterns with thresholding for backward compatibility
    std::vector<std::vector<float>> retrieve(const std::vector<float>& queryPattern,
                                            size_t maxResults = 5) const;

    // Get association strength with fuzzy matching
    float getAssociationStrength(const std::vector<float>& patternA,
                                const std::vector<float>& patternB) const;

    // Enhanced update with multiple factors
    void updateAssociation(const std::vector<float>& patternA,
                          const std::vector<float>& patternB,
                          float delta,
                          float noveltyWeight = 1.0f);

    // Enhanced spreading activation with similarity gradients
    std::vector<std::pair<std::vector<float>, float>> spreadActivationEnhanced(
        const std::vector<float>& cuePattern,
        size_t steps = 3,
        float decayFactor = 0.8f) const;

    // Get all associations for analysis
    const std::vector<std::tuple<NeuronId, NeuronId, float>>& getAllAssociations() const;

    // Get pattern neurons for pattern matching
    const std::vector<std::pair<NeuronId, std::vector<float>>>& getPatternNeurons() const;

    // Clear all associations
    void clear();

    // Get statistics about fuzzy matching performance
    struct FuzzyMatchingStats {
        size_t totalRetrievals;
        float averageSimilarityScore;
        size_t fuzzyMatches;
        size_t hardMatches;
        float thresholdUsed;
    };

    FuzzyMatchingStats getFuzzyStats() const;

    // Configure fuzzy matching parameters
    void setSimilarityThreshold(float threshold) { similarityThreshold_ = threshold; }
    void setMaxSimilaritySteps(size_t steps) { maxSimilaritySteps_ = steps; }
    void setSimilarityDecay(float decay) { similarityDecay_ = decay; }

    // Get current parameters
    float getSimilarityThreshold() const { return similarityThreshold_; }
    size_t getMaxSimilaritySteps() const { return maxSimilaritySteps_; }
    float getSimilarityDecay() const { return similarityDecay_; }

private:
    // Enhanced similarity computation with multiple similarity measures
    float computeEnhancedSimilarity(const std::vector<float>& a,
                                   const std::vector<float>& b) const;

    // Normalize pattern to unit sphere for cosine similarity
    std::vector<float> normalizePattern(const std::vector<float>& pattern) const;

    // Compute multiple similarity metrics
    struct SimilarityMetrics {
        float cosineSimilarity;
        float euclideanDistance;
        float manhattanDistance;
        float correlationSimilarity;
    };

    SimilarityMetrics computeSimilarityMetrics(const std::vector<float>& a,
                                              const std::vector<float>& b) const;

    // Find or create pattern neuron with enhanced pattern storage
    NeuronId findPatternNeuronEnhanced(const std::vector<float>& pattern);

    // Compute weighted similarity based on association strength
    float computeWeightedSimilarity(const std::vector<float>& query,
                                   const std::pair<NeuronId, std::vector<float>>& pattern,
                                   const std::vector<std::tuple<NeuronId, NeuronId, float>>& associations) const;

    // Enhanced pattern matching with multiple similarity measures
    struct PatternMatch {
        NeuronId neuronId;
        float cosineSimilarity;
        float euclideanSimilarity;
        float correlationSimilarity;
        std::vector<float> pattern;
    };

    std::vector<PatternMatch> findPatternMatches(const std::vector<float>& query) const;

    // Update association with novelty weighting
    void updateAssociationWithNovelty(const std::vector<std::tuple<NeuronId, NeuronId, float>>& associations,
                                     NeuronId neuronA, NeuronId neuronB,
                                     float delta, float noveltyWeight);

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    
    // Fuzzy matching parameters
    float similarityThreshold_;
    size_t maxSimilaritySteps_;
    float similarityDecay_;
    
    // Enhanced pattern storage
    std::vector<std::pair<NeuronId, std::vector<float>>> patternNeurons_;
    std::vector<std::tuple<NeuronId, NeuronId, float>> associations_;
    
    // Fuzzy matching statistics
    mutable size_t totalRetrievals_;
    mutable float accumulatedSimilarity_;
    mutable size_t fuzzyMatches_;
    mutable size_t hardMatches_;
};

} // namespace nlm
