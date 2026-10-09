#pragma once

#include "../core/Types/Types.hpp"
#include <memory>
#include <vector>
#include <unordered_map>
#include <mutex>

namespace nlm {

// Neural associative memory for pattern associations in neural space
// More sophisticated than basic associative memory with neural pattern matching
class NeuralAssociativeMemory {
public:
    NeuralAssociativeMemory();
    ~NeuralAssociativeMemory();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Create neural association between patterns
    void associatePattern(const std::vector<NeuronId>& patternA, 
                         const std::vector<NeuronId>& patternB,
                         float initialStrength = 1.0f);
    
    // Retrieve associated pattern
    std::vector<NeuronId> getAssociatedPattern(const std::vector<NeuronId>& pattern) const;
    
    // Calculate pattern similarity
    float calculatePatternSimilarity(const std::vector<NeuronId>& patternA,
                                    const std::vector<NeuronId>& patternB) const;
    
    // Update association strength based on usage
    void updateAssociation(const std::vector<NeuronId>& patternA,
                          const std::vector<NeuronId>& patternB,
                          float delta);
    
    // Get all stored patterns
    std::vector<std::pair<std::vector<NeuronId>, std::vector<NeuronId>>> getAllAssociations() const;
    
    // Get association count
    size_t getAssociationCount() const;
    
    // Get associations for a specific pattern
    std::vector<std::pair<std::vector<NeuronId>, float>> getPatternAssociations(
        const std::vector<NeuronId>& pattern) const;
    
    // Clear all associations
    void clear();
    
    // Get average association strength
    float getAverageAssociationStrength() const;
    
    // Get strongest associations
    std::vector<std::pair<std::pair<std::vector<NeuronId>, std::vector<NeuronId>>, float>> 
        getStrongestAssociations(size_t count) const;
    
    // Decay all associations
    void decayAssociations(float decayRate);
    
    // Get memory statistics
    struct MemoryStats {
        size_t totalAssociations;
        float averageStrength;
        float minStrength;
        float maxStrength;
        float patternSimilarity;
    };
    
    MemoryStats getStatistics() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Neural pattern hashing for efficient lookup
    static size_t hashPattern(const std::vector<NeuronId>& pattern);
    
    // Pattern similarity calculation
    static float calculateCosineSimilarity(const std::vector<NeuronId>& patternA,
                                           const std::vector<NeuronId>& patternB);
};

} // namespace nlm