// NeuralAssociativeMemory checkpoint data structures

#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "../core/Types/Types.hpp"

namespace nlm {

// PatternAssociation for associative memory
struct PatternAssociationCheckpoint {
    // Pattern identifiers (represented as hash of pattern)
    uint64_t patternAHash;
    uint64_t patternBHash;
    
    // Actual patterns (stored for reconstruction)
    std::vector<float> patternA;
    std::vector<float> patternB;
    
    // Association strength
    float strength;
    
    // Additional metadata
    size_t observationCount;
    float lastUpdated;
    bool isBidirectional;
    
    PatternAssociationCheckpoint()
        : patternAHash(0), patternBHash(0), strength(0.0f)
        , observationCount(0), lastUpdated(0.0f), isBidirectional(false) {}
};

// NeuralAssociativeMemory checkpoint state
struct NeuralAssociativeMemoryCheckpoint {
    // All pattern associations
    std::vector<PatternAssociationCheckpoint> associations;
    
    // Pattern registry (hash -> pattern)
    std::unordered_map<uint64_t, std::vector<float>> patternRegistry;
    
    // Association statistics
    size_t totalAssociations;
    float averageStrength;
    float maxStrength;
    float minStrength;
    
    // Integration with other systems
    std::unordered_set<uint64_t> episodeAssociatedPatterns;
    std::unordered_set<uint64_t> conceptAssociatedPatterns;
    
    // Learning parameters
    float learningRate;
    float decayRate;
    float noveltyThreshold;
    
    NeuralAssociativeMemoryCheckpoint()
        : totalAssociations(0), averageStrength(0.0f), maxStrength(0.0f), minStrength(0.0f)
        , learningRate(0.01f), decayRate(0.001f), noveltyThreshold(0.5f) {}
};

// NeuralAssociativeMemory save/load interface
class NeuralAssociativeMemorySaverLoader {
public:
    virtual ~NeuralAssociativeMemorySaverLoader() = default;
    
    // Save associative memory state
    virtual bool save(const NeuralAssociativeMemoryCheckpoint& checkpoint) const = 0;
    
    // Load associative memory state
    virtual bool load(NeuralAssociativeMemoryCheckpoint& checkpoint) = 0;
    
    // Create empty checkpoint
    virtual NeuralAssociativeMemoryCheckpoint createEmptyCheckpoint() const = 0;
    
    // Convert pattern hash back to pattern
    virtual bool getPatternFromHash(uint64_t hash, std::vector<float>& pattern) const = 0;
};

} // namespace nlm
