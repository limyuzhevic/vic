#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <string>
#include <memory>

namespace nlm {

// Simple associative memory for pattern learning
class NeuralAssociativeMemory {
public:
    NeuralAssociativeMemory();
    ~NeuralAssociativeMemory();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Learn associations
    void learnAssociation(const std::vector<float>& keyPattern,
                         const std::vector<float>& valuePattern,
                         float strength = 1.0f);
    
    // Retrieve value for key
    std::vector<float> retrieveValue(const std::vector<float>& keyPattern,
                                     float threshold = 0.5f) const;
    
    // Clear memory
    void clear();
    
    // Query state
    size_t getAssociationCount() const;
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm