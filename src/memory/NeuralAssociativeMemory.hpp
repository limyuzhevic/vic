#pragma once

#include "NeuralWorkingMemory.hpp"
#include "../brain/Brain.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../prediction/PredictionSystem.hpp"
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <cmath>

namespace nlm {

// NeuralAssociativeMemory: Long-term pattern associations and memory consolidation
// NOT a simple database - uses neural mechanisms for pattern completion and retrieval
//
// Key mechanisms:
// - Hebbian-style synaptic strengthening for pattern associations
// - Pattern completion through recurrent connections
// - Competitive learning for memory consolidation
// - Integration with prediction system for temporal associations
// - Neuromodulation effects on memory formation and retrieval

class NeuralAssociativeMemory {
public:
    NeuralAssociativeMemory();
    ~NeuralAssociativeMemory();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Create association between two patterns
    // Patterns are represented as neural activity vectors
    void associate(const std::vector<float>& patternA, 
                  const std::vector<float>& patternB,
                  float strength = 1.0f,
                  bool bidirectional = true);
    
    // Retrieve associated pattern
    // Given one pattern, retrieve its associated partner
    std::vector<float> retrieveAssociation(const std::vector<float>& pattern,
                                          float similarityThreshold = 0.7f) const;
    
    // Get all associations for a pattern
    std::vector<std::vector<float>> getAssociations(const std::vector<float>& pattern) const;
    
    // Update association strength
    void updateAssociation(const std::vector<float>& patternA,
                          const std::vector<float>& patternB,
                          float delta);
    
    // Check if pattern has associations
    bool hasAssociations(const std::vector<float>& pattern) const;
    
    // Get association strength between patterns
    float getAssociationStrength(const std::vector<float>& patternA,
                                const std::vector<float>& patternB) const;
    
    // Pattern completion: reconstruct full pattern from partial input
    std::vector<float> completePattern(const std::vector<float>& partialPattern,
                                      float threshold = 0.5f) const;
    
    // Consolidate associations (move to long-term memory)
    void consolidateAssociations(float threshold);
    
    // Learn from prediction errors
    void learnFromPredictionError(const std::vector<float>& predicted,
                                 const std::vector<float>& actual,
                                 float error);
    
    // Update with neuromodulation signals
    void applyNeuromodulation(const Neuromodulator& modulator);
    
    // Clear all associations
    void clear();
    
    // Get number of associations
    size_t getAssociationCount() const { return associations_.size(); }
    
    // Get memory capacity
    size_t getCapacity() const { return capacity_; }
    void setCapacity(size_t cap) { capacity_ = cap; }
    
    // Get learning rate
    float getLearningRate() const { return learningRate_; }
    void setLearningRate(float rate) { learningRate_ = rate; }
    
    // Get consolidation threshold
    float getConsolidationThreshold() const { return consolidationThreshold_; }
    void setConsolidationThreshold(float threshold) { consolidationThreshold_ = threshold; }
    
    // Get memory activity level
    float getMemoryActivity() const;
    
    // Update associations based on neural activity
    void updateFromNeuralActivity(const Brain* brain);
    
    // Check if memory needs consolidation
    bool needsConsolidation() const;
    
private:
    // Association entry
    struct Association {
        std::vector<float> patternA;
        std::vector<float> patternB;
        float strength;
        size_t lastAccessed;
        float decayFactor;
        bool consolidated;
        
        Association() : strength(0.0f), lastAccessed(0), decayFactor(1.0f), consolidated(false) {}
        Association(const std::vector<float>& a, const std::vector<float>& b, float s)
            : patternA(a), patternB(b), strength(s), lastAccessed(0), decayFactor(1.0f), consolidated(false) {}
    };
    
    // Pattern completion result
    struct CompletionResult {
        std::vector<float> completedPattern;
        float confidence;
        size_t matchedAssociations;
    };
    
    // Create Hebbian-style association
    void createHebbianAssociation(const std::vector<float>& patternA,
                                 const std::vector<float>& patternB,
                                 float strength);
    
    // Pattern similarity computation
    float computePatternSimilarity(const std::vector<float>& patternA,
                                  const std::vector<float>& patternB) const;
    
    // Find best matching associations
    std::vector<size_t> findMatchingAssociations(const std::vector<float>& pattern,
                                                float threshold) const;
    
    // Pattern completion from matches
    CompletionResult completePatternFromMatches(const std::vector<float>& partialPattern,
                                               const std::vector<size_t>& matchingIndices) const;
    
    // Decay weak associations
    void decayWeakAssociations();
    
    // Consolidate associations to long-term storage
    void consolidateToLongTerm(const std::vector<size_t>& indices);
    
    // Convert neural activity to pattern
    std::vector<float> neuralActivityToPattern(const Brain* brain) const;
    
    // Boost association strength with neuromodulation
    void boostAssociationStrength(float factor);
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Temporary data
    std::vector<Association> associations_;
    std::vector<Association> longTermAssociations_;
    size_t capacity_;
    float learningRate_;
    float consolidationThreshold_;
    size_t nextId_;
    Brain* brain_;
    float neuromodulationLevel_;
    float predictionErrorWeight_;
};

} // namespace nlm
