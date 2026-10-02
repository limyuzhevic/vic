#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>

namespace nlm {

// NeuralAssociativeMemory: Forms associations between patterns through Hebbian learning
// Integrates with episodic and working memory systems
//
// Key mechanisms:
// - Hebbian pattern associations (neurons that fire together wire together)
// - Pattern similarity matching for association retrieval
// - Strength-based association consolidation
// - Bidirectional associations (A->B and B->A)

class NeuralAssociativeMemory {
public:
    NeuralAssociativeMemory();
    ~NeuralAssociativeMemory();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Create an association between two patterns
    // If association already exists, its strength is increased
    void createAssociation(const std::vector<float>& patternA,
                          const std::vector<float>& patternB,
                          float initialStrength = 0.5f);

    // Retrieve associated pattern
    // Returns the pattern most strongly associated with input, or empty vector if none
    std::vector<float> retrieveAssociation(const std::vector<float>& pattern,
                                          float threshold = 0.3f) const;

    // Check if pattern has associations
    bool hasAssociations(const std::vector<float>& pattern) const;

    // Get association strength between patterns
    float getAssociationStrength(const std::vector<float>& patternA,
                                 const std::vector<float>& patternB) const;

    // Update association strength (Hebbian learning)
    void updateAssociation(const std::vector<float>& patternA,
                          const std::vector<float>& patternB,
                          float learningRate = 0.01f,
                          bool shouldStrengthen = true);

    // Get all patterns in this memory system
    std::vector<std::vector<float>> getAllPatterns() const;

    // Clear all associations
    void clear();

    // Get association statistics
    size_t getAssociationCount() const;
    float getAverageAssociationStrength() const;

    // Consolidation: strengthen strong associations, weaken weak ones
    void consolidate(float strengthenThreshold = 0.7f,
                     float weakenThreshold = 0.2f);

    // Get associations for pattern (returns vector of strength-weighted patterns)
    std::vector<std::pair<std::vector<float>, float>> getAssociations(
        const std::vector<float>& pattern,
        size_t maxResults = 5) const;

    // Set Hebbian learning rate
    void setHebbianRate(float rate) { hebbianRate_ = rate; }

    // Set association strength bounds
    void setStrengthBounds(float min, float max) {
        strengthMin_ = min;
        strengthMax_ = max;
    }

private:
    // Compute pattern similarity (cosine similarity)
    float computePatternSimilarity(const std::vector<float>& a,
                                   const std::vector<float>& b) const;

    // Find best matching pattern for given input
    size_t findBestMatch(const std::vector<float>& pattern,
                        const std::vector<std::vector<float>>& candidates,
                        float threshold = 0.3f) const;

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;

    // Association storage: pattern -> (targetPattern -> strength)
    std::unordered_map<std::vector<float>, std::unordered_map<std::vector<float>, float>> associations_;

    // Patterns list for fast lookup
    std::vector<std::vector<float>> patterns_;

    // Parameters
    float hebbianRate_;
    float strengthMin_;
    float strengthMax_;
};
