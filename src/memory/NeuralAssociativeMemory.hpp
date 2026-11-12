#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <unordered_map>
#include <algorithm>
#include <functional>

namespace nlm {

// NeuralAssociativeMemory: Stores and retrieves relationships between neural patterns
// Implements Hebbian and STDP-style associative connections between experiences
// Used for pattern completion, recall, and concept formation

class NeuralAssociativeMemory {
public:
    NeuralAssociativeMemory();
    ~NeuralAssociativeMemory();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Create a new association between patterns
    void store(const std::vector<float>& patternA, const std::vector<float>& patternB, float strength);
    
    // Retrieve associated pattern given one pattern (pattern completion)
    std::vector<float> retrieve(const std::vector<float>& pattern) const;
    
    // Get all associations for a pattern
    std::vector<std::pair<std::vector<float>, float>> getAssociations(const std::vector<float>& pattern) const;
    
    // Update association strength
    void updateAssociation(const std::vector<float>& patternA, const std::vector<float>& patternB, float delta);
    
    // Get association strength between patterns
    float getAssociationStrength(const std::vector<float>& patternA, const std::vector<float>& patternB) const;
    
    // Clear all associations
    void clear();
    
    // Get all patterns stored
    std::vector<std::vector<float>> getAllPatterns() const;
    
    // Prune weak associations
    void prune(float threshold);
    
    // Get current capacity
    size_t getCapacity() const;
    
    // Set capacity
    void setCapacity(size_t capacity);
    
    // Get memory usage statistics
    size_t getAssociationCount() const;
    
    // Hebbian learning: strengthen connections that are co-active
    void hebbianLearning(const std::vector<float>& patternA, const std::vector<float>& patternB);
    
    // Temporal learning: strengthen associations with timing
    void temporalLearning(const std::vector<float>& patternA, const std::vector<float>& patternB, 
                         SimulationStep timeDifference);

private:
    struct Impl;
    Impl* pImpl;
};
