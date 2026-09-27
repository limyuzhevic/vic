#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>
#include <algorithm>
#include <chrono>
#include <unordered_map>

namespace nlm {

// EnhancedWorkingMemory: Advanced working memory with improved capacity and content management
// Implements enhanced working memory that supports:
// - Dynamic capacity adjustment based on task demands
// - Content competition and prioritization
// - Interference reduction mechanisms
// - Consolidation and maintenance strategies
// - Task-dependent encoding and retrieval

class EnhancedWorkingMemory {
public:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    EnhancedWorkingMemory();
    ~EnhancedWorkingMemory();
    
    // Initialize with brain reference and enhanced parameters
    void initialize(class Brain* brain, float baseCapacity = 100.0f,
                    float capacityMultiplier = 2.0f, float decayRate = 0.05f);
    
    // Store information with competition
    void storeWithCompetition(NeuronId neuron, float activation, 
                             const std::vector<float>& context = {});
    
    // Get top N items based on activation
    std::vector<std::pair<NeuronId, float>> getTopItems(size_t count) const;
    
    // Get working memory capacity (dynamic)
    float getDynamicCapacity() const;
    
    // Get current memory load (percentage)
    float getMemoryLoad() const;
    
    // Get memory efficiency metrics
    float getEncodingEfficiency() const;
    float getRetrievalEfficiency() const;
    
    // Get memory statistics
    struct MemoryStats {
        size_t totalItems;
        float currentLoad;
        float averageActivation;
        float competitionIntensity;
        float consolidationProgress;
        std::vector<NeuronId> dominantItems;
    };
    
    MemoryStats getMemoryStats() const;
    
    // Clear memory or specific items
    void clear(bool clearAll = true);
    void clearLowActivation(float threshold);
    
    // Enable/disable enhanced features
    void enableDynamicCapacity(bool enable);
    void enableContentCompetition(bool enable);
    void enableConsolidation(bool enable);
    
    // Configure enhanced parameters
    void setBaseCapacity(float capacity);
    void setCapacityMultiplier(float multiplier);
    void setCompetitionThreshold(float threshold);
    void setConsolidationInterval(size_t interval);
    
    // Get learning progress
    float getLearningProgress() const;
    size_t getConsolidationCycles() const;
    
private:
    EnhancedWorkingMemory(const EnhancedWorkingMemory&) = delete;
    EnhancedWorkingMemory& operator=(const EnhancedWorkingMemory&) = delete;
};

} // namespace nlm