#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <string>

namespace nlm {

// AssociativeMemory: Pattern completion and relationship inference
// NOT simple lookup tables - complex neural networks for pattern completion
//
// Key mechanisms:
// - Pattern completion through attractor dynamics
// - Hebbian association learning
// - Temporal contiguity associations
// - Spreading activation for related patterns
// - Competitive learning for feature extraction

class NeuralAssociativeMemory {
public:
    NeuralAssociativeMemory();
    ~NeuralAssociativeMemory();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Create association between two patterns
    void associate(const std::vector<float>& patternA,
                   const std::vector<float>& patternB,
                   float strength = 1.0f);
    
    // Retrieve associated patterns
    std::vector<float> retrieveAssociations(
        const std::vector<float>& pattern,
        size_t maxResults = 5) const;
    
    // Complete partial pattern based on associations
    std::vector<float> completePattern(
        const std::vector<float>& partialPattern) const;
    
    // Get association strength between patterns
    float getAssociationStrength(
        const std::vector<float>& patternA,
        const std::vector<float>& patternB) const;
    
    // Update associations based on experience
    void updateAssociations(const std::vector<float>& pattern,
                           const std::vector<float>& outcome,
                           float learningRate = 0.01f);
    
    // Get activation vectors for pattern
    std::vector<float> getActivationVector(
        const std::vector<float>& pattern) const;
    
    // Calculate pattern similarity through association network
    float calculatePatternSimilarity(
        const std::vector<float>& patternA,
        const std::vector<float>& patternB) const;
    
    // Get number of associations
    size_t getAssociationCount() const;
    
    // Get all activation vectors
    std::vector<std::vector<float>> getAllActivationVectors() const;
    
    // Perform pattern completion on activation vectors
    std::vector<float> patternCompletion(
        const std::vector<float>& activation) const;
    
    // Apply reinforcement learning to associations
    void reinforceAssociations(const std::vector<float>& pattern,
                              float reward,
                              float learningRate = 0.05f);
    
    // Decay weak associations
    void decayAssociations(float decayRate = 0.001f);
    
    // Get activation map for visualization
    std::vector<std::vector<float>> getActivationMap() const;
    
    // Clear all associations
    void clear();
    
    // Load associations from pattern database
    void loadPatterns(const std::vector<std::vector<float>>& patterns);
    
    // Save associations to pattern database
    void savePatterns(std::vector<std::vector<float>>& patterns) const;
    
    // Get concept associations for higher-level abstraction
    std::vector<size_t> getConceptAssociations(
        const std::vector<float>& pattern) const;
    
    // Create new concept from pattern
    size_t createConcept(const std::vector<float>& pattern,
                         const std::vector<float>& features);
    
    // Associate concepts with each other
    void associateConcepts(size_t conceptA, size_t conceptB, float strength);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    
    // Pattern database
    std::vector<std::vector<float>> patternDatabase_;
    
    // Activation vectors
    std::vector<std::vector<float>> activationVectors_;
    
    // Concept system
    std::vector<std::vector<size_t>> conceptAssociations_;
    std::vector<std::vector<float>> conceptFeatures_;
    
    // Parameters
    float learningRate_;
    float decayRate_;
    float completionThreshold_;
    size_t maxAssociationsPerPattern_;
};

// PatternCompletion: Auto-associative memory for pattern completion
// NOT content-addressable memory - dynamic pattern completion through neural dynamics
//
// Key mechanisms:
// - Hopfield-like attractor networks
// - Energy minimization for pattern completion
// - Noise tolerance for robust recall
// - Capacity limits based on network size
// - Multi-stable dynamics for pattern recognition

class PatternCompletion {
public:
    PatternCompletion();
    ~PatternCompletion();
    
    void initialize(Brain* brain);
    
    // Store pattern in attractor network
    void storePattern(const std::vector<float>& pattern);
    
    // Retrieve pattern from network state
    std::vector<float> retrievePattern(const std::vector<float>& cue) const;
    
    // Update network weights
    void updateWeights(const std::vector<float>& pattern, float learningRate = 0.01f);
    
    // Get network energy for pattern
    float getPatternEnergy(const std::vector<float>& pattern) const;
    
    // Check if pattern can be stored without interference
    bool canStorePattern(const std::vector<float>& pattern) const;
    
    // Get number of stored patterns
    size_t getPatternCount() const;
    
    // Remove pattern from network
    void removePattern(const std::vector<float>& pattern);
    
    // Get stable states for pattern
    std::vector<std::vector<float>> getStableStates(
        const std::vector<float>& pattern) const;
    
    // Calculate pattern separation (how different patterns are stored)
    float calculatePatternSeparation() const;
    
    // Get network capacity
    float getNetworkCapacity() const;
    
    // Clear all patterns
    void clear();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    
    // Network weights (symmetric)
    std::vector<std::vector<float>> weights_;
    
    // Pattern storage
    std::vector<std::vector<float>> storedPatterns_;
    
    // Network parameters
    float threshold_;
    float learningRate_;
    float symmetryPenalty_;
};

// SequenceMemory: Temporal pattern learning and prediction
// NOT simple list storage - neural sequence learning through temporal relations
//
// Key mechanisms:
// - Temporal difference learning
// - Sequence completion through predictive coding
// - Short-term vs long-term sequence memory
// - Hierarchical sequence organization
// - Forgetting curves based on temporal interference

class SequenceMemory {
public:
    SequenceMemory();
    ~SequenceMemory();
    
    void initialize(Brain* brain);
    
    // Learn sequence pattern
    void learnSequence(const std::vector<std::vector<float>>& sequence,
                       float reward = 1.0f);
    
    // Predict next element in sequence
    std::vector<float> predictNext(
        const std::vector<float>& currentSequence) const;
    
    // Complete partial sequence
    std::vector<std::vector<float>> completeSequence(
        const std::vector<std::vector<float>>& partialSequence) const;
    
    // Get sequence probability
    float getSequenceProbability(
        const std::vector<std::vector<float>>& sequence) const;
    
    // Get all learned sequences
    std::vector<std::vector<std::vector<float>>> getLearnedSequences() const;
    
    // Remove outdated sequences
    void cleanupSequences(float timeThreshold = 1000.0f);
    
    // Get sequence statistics
    float getSequenceStatistics() const;
    
    // Set sequence learning parameters
    void setLearningParameters(float alpha, float beta, float gamma);
    
    // Get temporal prediction accuracy
    float getPredictionAccuracy() const;
    
    // Clear all sequences
    void clear();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    
    // Learned sequences (temporal context)
    std::vector<std::vector<std::vector<float>>> sequences_;
    
    // Transition probabilities
    std::vector<std::vector<float>> transitionProbs_;
    
    // Short-term buffer
    std::vector<std::vector<float>> shortTermBuffer_;
    
    // Learning parameters
    float alpha_;  // learning rate
    float beta_;   // memory consolidation rate
    float gamma_;  // replay probability
    
    // Statistical measures
    float predictionAccuracy_;
    float sequenceEntropy_;
};

} // namespace nlm