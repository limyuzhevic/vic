#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>

namespace nlm {

// ReconstructionError: Auto-encoder style reconstruction learning
// Implements error-based learning where the network learns to reconstruct
// its own sensory input or internal representations
// 
// This mechanism enables:
// - Pattern completion and recall
// - Robust representation learning
// - Error detection and correction
// - Memory consolidation through reconstruction
//
// The reconstruction error drives synaptic plasticity, allowing the network
// to learn compressed representations that can reproduce the original input

class ReconstructionError {
public:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    ReconstructionError();
    ~ReconstructionError();
    
    // Initialize with brain reference and learning parameters
    void initialize(class Brain* brain, float learningRate = 0.01f, 
                    size_t bufferSize = 1000);
    
    // Set target input for reconstruction
    void setTarget(const std::vector<float>& target);
    
    // Get current reconstruction error
    float getError() const;
    
    // Get predicted reconstruction
    std::vector<float> getReconstruction() const;
    
    // Compute reconstruction error based on current network state
    void computeError(const std::vector<float>& currentState);
    
    // Update weights based on reconstruction error
    void applyLearning(class RandomGenerator& rng);
    
    // Check if reconstruction has converged
    bool hasConverged(float threshold = 0.001f) const;
    
    // Get reconstruction statistics
    float getMeanSquaredError() const;
    float getMaxError() const;
    size_t getBufferSize() const;
    
    // Get reconstruction history
    const std::vector<float>& getErrorHistory() const;
    const std::vector<std::vector<float>>& getReconstructionHistory() const;
    
    // Clear history and reset
    void clearHistory();
    
    // Enable/disable reconstruction learning
    void enable(bool enable);
    bool isEnabled() const;
    
    // Configure learning parameters
    void setLearningRate(float rate);
    void setErrorThreshold(float threshold);
    void setBufferSize(size_t size);
    
    // Get reconstruction error statistics
    float getLearningProgress() const;
    size_t getConvergedSteps() const;
    
private:
    ReconstructionError(const ReconstructionError&) = delete;
    ReconstructionError& operator=(const ReconstructionError&) = delete;
};

} // namespace nlm