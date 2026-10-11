#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <deque>
#include <functional>

namespace nlm {

// NeuralAssociativeMemory: Creates and retrieves relationships between neural patterns
// NOT a lookup table - associations are encoded in synaptic connectivity and neural activity
//
// Key mechanisms:
// - Distributed representations through sparse coding
// - Hebbian associative learning for pattern associations
// - Predictive coding through forward connections
// - Memory efficiency through pattern compression
// - Neural replay for association strengthening
//
// Implementation uses:
// - Sparse distributed representations (SDRs)
// - Correlation-based learning
// - Pattern completion via attractor dynamics
// - Homeostatic regulation

class NeuralAssociativeMemory {
public:
    NeuralAssociativeMemory();
    ~NeuralAssociativeMemory();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Create association between two neural patterns
    // Both patterns should be from actual neural activity
    void associate(const std::vector<NeuronId>& keyPattern,
                   const std::vector<NeuronId>& valuePattern,
                   float strength = 1.0f);
    
    // Retrieve patterns associated with a query pattern
    std::vector<NeuronId> retrieveAssociation(const std::vector<NeuronId>& keyPattern,
                                               float threshold = 0.5f) const;
    
    // Get association strength between two patterns
    float getAssociationStrength(NeuronId a, NeuronId b) const;
    
    // Clear all associations
    void clear();
    
    // Update memory with time step
    void update(float timeStep);
    
    // Get statistics
    size_t getTotalAssociations() const;
    size_t getTotalPatternNeurons() const;
    float getLearningRate() const;
    float getConsolidationThreshold() const;
    float getSparsityTarget() const;
    float getAverageAssociationStrength() const;
    
    // Configuration
    void setLearningRate(float rate);
    void setConsolidationThreshold(float threshold);
    void setSparsityTarget(float sparsity);
    
    // Get all associations (for debugging/monitoring)
    const std::vector<std::tuple<NeuronId, NeuronId, float>>& getAllAssociations() const;
    const std::vector<NeuronId>& getAllPatternNeurons() const;
    
    // Log memory statistics
    void logStats() const;
    
private:
    // Pattern management
    NeuronId findOrCreatePatternNeuron(const std::vector<float>& pattern);
    
    // Learning mechanisms
    void implementHebbianAssociativeLearning(NeuronId keyNeuron, NeuronId valueNeuron, float strength);
    void implementSparseCoding(NeuronId keyNeuron, NeuronId valueNeuron, size_t patternSize);
    void implementPredictiveCoding(NeuronId keyNeuron, NeuronId valueNeuron);
    void implementMemoryEfficiencyOptimizations();
    
    // Consolidation and homeostasis
    void consolidateAssociations(float timeStep);
    void applyHomeostaticPlasticity();
    
    // Pattern similarity computation
    float computePatternSimilarity(const std::vector<float>& a,
                                   const std::vector<float>& b) const;
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    std::vector<NeuronId> patternNeurons_;
};

} // namespace nlm
