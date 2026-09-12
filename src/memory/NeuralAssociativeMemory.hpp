#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <string>
#include <algorithm>

namespace nlm {

class NeuralAssociativeMemory {
public:
    NeuralAssociativeMemory();
    ~NeuralAssociativeMemory();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Store an association between patterns
    void storeAssociation(const std::vector<float>& patternA,
                         const std::vector<float>& patternB,
                         float strength = 1.0f);
    
    // Retrieve pattern B given pattern A
    std::vector<float> retrieve(const std::vector<float>& patternA,
                               float threshold = 0.5f) const;
    
    // Get association strength
    float getAssociationStrength(const std::vector<float>& patternA,
                                const std::vector<float>& patternB) const;
    
    // Update association strength
    void updateAssociation(const std::vector<float>& patternA,
                          const std::vector<float>& patternB,
                          float newStrength);
    
    // Get all stored associations
    const std::vector<std::pair<std::vector<float>, std::vector<float>>>&
    getAllAssociations() const { return associations_; }
    
    // Get association count
    size_t getAssociationCount() const { return associations_.size(); }
    
    // Get Hebbian similarity between two patterns
    float computeHebbianSimilarity(const std::vector<float>& patternA,
                                  const std::vector<float>& patternB) const;
    
    // Apply STDP-like learning to associations
    void applySTDP(float predictionError, TimestepDuration dt);
    
    // Clear all associations
    void clear();
    
    // Reset associative memory
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    std::vector<std::pair<std::vector<float>, std::vector<float>>> associations_;
    std::vector<float> associationStrengths_;
    std::vector<float> eligibilityTraces_;
    SimulationStep lastUpdateTime_;
    float learningRate_;
    float decayRate_;
};

} // namespace nlm
