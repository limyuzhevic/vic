#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace nlm {

// AssociativeMemory: Pattern association and memory linkage
// Creates and manages associations between different patterns and experiences
//
// Key mechanisms:
// - Hebbian learning for pattern associations
// - Bidirectional associative memory (BAM)
// - Pattern completion
// - Interference management
// - Memory consolidation

class NeuralAssociativeMemory {
public:
    NeuralAssociativeMemory();
    ~NeuralAssociativeMemory();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Create association between two patterns
    // Patterns are represented as vectors of floats
    bool createAssociation(const std::vector<float>& pattern1, 
                           const std::vector<float>& pattern2,
                           float strength = 0.5f);

    // Retrieve associated pattern
    std::vector<float> retrieveAssociation(const std::vector<float>& pattern,
                                           float threshold = 0.5f) const;

    // Check if pattern has associations
    bool hasAssociation(const std::vector<float>& pattern) const;

    // Update associative memory (consolidation, interference management)
    void update(TimestepDuration dt);

    // Get association strength between patterns
    float getAssociationStrength(const std::vector<float>& pattern1,
                                 const std::vector<float>& pattern2) const;

    // Remove specific association
    bool removeAssociation(const std::vector<float>& pattern1,
                          const std::vector<float>& pattern2);

    // Clear all associations
    void clear();

    // Get all patterns in associative memory
    std::vector<std::vector<float>> getAllPatterns() const;

    // Get capacity
    size_t getCapacity() const { return capacity_; }
    void setCapacity(size_t cap) { capacity_ = cap; }

    // Get current number of associations
    size_t getAssociationCount() const { return associations_.size(); }

    // Set learning rate for associations
    void setLearningRate(float rate) { learningRate_ = rate; }

    // Set interference threshold
    void setInterferenceThreshold(float threshold) { interferenceThreshold_ = threshold; }

    // Check if adding new association would exceed interference threshold
    bool wouldExceedInterference(const std::vector<float>& pattern1,
                                 const std::vector<float>& pattern2) const;

    // Consolidate weak associations
    void consolidateWeakAssociations(float threshold);

    // Update pattern recency for LRU management
    void updatePatternRecency(const std::vector<float>& pattern);

    // Get last access time for pattern
    float getPatternLastAccess(const std::vector<float>& pattern) const;

private:
    // Create hash for pattern vector
    size_t patternHash(const std::vector<float>& pattern) const;

    // Get similar patterns for interference check
    std::vector<std::vector<float>> getSimilarPatterns(const std::vector<float>& pattern) const;

    // Calculate pattern similarity
    float calculatePatternSimilarity(const std::vector<float>& pattern1,
                                     const std::vector<float>& pattern2) const;

    // Remove oldest associations if capacity exceeded
    void manageCapacity();

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    size_t capacity_;
    float learningRate_;
    float interferenceThreshold_;

    // Association storage
    std::unordered_map<size_t, std::vector<float>> patterns_;
    std::unordered_map<size_t, std::unordered_map<size_t, float>> associations_;
    std::unordered_map<size_t, float> patternAccessTimes_;
    std::unordered_map<size_t, std::vector<size_t>> patternSimilarities_;
};

} // namespace nlm