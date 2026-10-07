#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "EmotionalMemoryPrioritizer.hpp"
#include <vector>
#include <memory>
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <deque>

namespace nlm {

// Hierarchical Memory Architecture
// Multi-level memory organization with automatic consolidation and cross-level interaction
//
// Key mechanisms:
// - Level 1: Working Memory - transient active information
// - Level 2: Episodic Memory - event storage with temporal organization
// - Level 3: Semantic Memory - pattern abstraction and generalization
// - Level 4: Long-term Memory - consolidated knowledge structures
// - Cross-level consolidation with pattern completion
// - Memory pruning and resource management

class HierarchicalMemory {
public:
    enum class MemoryLevel {
        WORKING,      // Level 1: Transient active information
        EPISODIC,     // Level 2: Event storage and temporal organization
        SEMANTIC,     // Level 3: Pattern abstraction and generalization
        LONG_TERM     // Level 4: Consolidated knowledge structures
    };

    struct MemoryItem {
        MemoryLevel level;
        std::vector<float> pattern;
        SimulationStep timestamp;
        float strength;
        float relevance;
        std::vector<NeuronId> associatedNeurons;
        
        // Additional metadata for different levels
        union {
            struct { // Working memory metadata
                float decayRate;
                float maintenanceCurrent;
                bool isActive;
            } working;
            
            struct { // Episodic memory metadata
                ActionType action;
                float reward;
                float energy;
                float novelty;
                std::vector<float> sensoryState;
                std::vector<float> resultingSensoryState;
                float resultingReward;
            } episodic;
            
            struct { // Semantic memory metadata
                std::string conceptId;
                float stability;
                std::vector<std::string> relatedConcepts;
                float abstractionLevel;
            } semantic;
            
            struct { // Long-term memory metadata
                float consolidationScore;
                float emotionalValence;
                float importance;
                SimulationStep lastAccess;
                size_t accessCount;
            } longTerm;
        };
        
        MemoryItem() : level(MemoryLevel::WORKING), timestamp(0), strength(0.0f), relevance(0.0f) {
            // Initialize with default values
            working.isActive = true;
            working.decayRate = 0.01f;
            working.maintenanceCurrent = 0.0f;
            
            episodic.reward = 0.0f;
            episodic.energy = 0.0f;
            episodic.novelty = 0.0f;
            episodic.resultingReward = 0.0f;
            episodic.action = ActionType::Wait;
        }
    };

    HierarchicalMemory();
    ~HierarchicalMemory();

    // Initialize with brain reference
    void initialize(Brain* brain, EmotionalMemoryPrioritizer* prioritizer);

    // Store information at appropriate level
    void store(const std::vector<float>& pattern, MemoryLevel level = MemoryLevel::WORKING,
               float strength = 1.0f, float relevance = 0.0f);

    // Store episode with full metadata
    void storeEpisode(const std::vector<float>& sensoryState, ActionType action,
                     float reward, float energy, float novelty,
                     const std::vector<float>& resultingSensoryState,
                     float resultingReward, const std::vector<NeuronId>& activeNeurons);

    // Retrieve from specific level
    std::vector<MemoryItem> retrieveLevel(MemoryLevel level, size_t maxResults = 10) const;

    // Hierarchical retrieval - searches across levels
    std::vector<MemoryItem> retrieveHierarchical(const std::vector<float>& pattern,
                                                 float similarityThreshold = 0.3f,
                                                 size_t maxResults = 10) const;

    // Automatic consolidation between levels
    void consolidate(float consolidationThreshold = 0.7f);

    // Pattern completion across levels
    std::vector<float> completePattern(const std::vector<float>& partialPattern) const;

    // Memory pruning and resource management
    void prune(float retentionThreshold = 0.1f);

    // Get statistics about memory usage
    struct MemoryStats {
        size_t workingMemoryItems;
        size_t episodicMemoryItems;
        size_t semanticMemoryItems;
        size_t longTermMemoryItems;
        float averageStrength;
        float averageRelevance;
        float memoryEfficiency;
        size_t totalItems;
    };

    MemoryStats getStats() const;

    // Level management
    void setLevelCapacity(MemoryLevel level, size_t capacity);
    size_t getLevelCapacity(MemoryLevel level) const;
    void setLevelDecayRate(MemoryLevel level, float decayRate);
    float getLevelDecayRate(MemoryLevel level) const;

    // Cross-level influence
    void applyWorkingMemoryToEpisodic();
    void applyEpisodicToSemantic();
    void applySemanticToLongTerm();

    // Enhanced neuromodulatory update with stress and empathy
    void updateWithNeuromodulation(float emotionalInfluence, float attentionFocus,
                                 float stressLevel, float empathyLevel);

    // Enhanced neuromodulation configuration
    void setNeuromodulationParameters(float boost, float attention,
                                     float stress, float empathy, float dopamine);

    // Cross-level consolidation enhancement
    void enhanceCrossLevelIntegration(MemoryItem& item);
    void processConsolidationQueue();

    // Reset all levels
    void clear();

    // Get all items for analysis
    const std::vector<MemoryItem>& getAllItems() const { return allItems_; }

private:
    // Internal storage for each level
    struct Impl;
    std::unique_ptr<Impl> pImpl;

// Enhanced neuromodulation parameters
    float neuromodulationBoost_;
    float attentionAmplification_;
    float stressSensitivity_;
    float empathyInfluence_;
    float dopamineModulation_;
    
    // Central storage
    std::vector<MemoryItem> allItems_;
    
    // Level-specific storage for efficient access
    std::vector<MemoryItem> workingMemory_;
    std::vector<MemoryItem> episodicMemory_;
    std::vector<MemoryItem> semanticMemory_;
    std::vector<MemoryItem> longTermMemory_;
    
    // Configuration
    struct LevelConfig {
        size_t capacity;
        float decayRate;
        float consolidationThreshold;
        float relevanceThreshold;
    };
    
    std::vector<LevelConfig> levelConfigs_;

    // Internal helper methods
    void addToLevel(MemoryItem item);
    void removeFromLevel(MemoryItem item);
    void updateItemRelevance(MemoryItem& item);
    float computePatternSimilarity(const std::vector<float>& pattern1,
                                  const std::vector<float>& pattern2) const;
    std::vector<MemoryItem> findSimilarItems(const std::vector<float>& pattern,
                                            MemoryLevel level,
                                            float similarityThreshold) const;
    void consolidateLevel(MemoryLevel sourceLevel, MemoryLevel targetLevel,
                         float threshold);
    float calculateConsolidationScore(const MemoryItem& item) const;
    void updateMemoryEfficiency();
    // Enhanced consolidation calculation
    float calculateEnhancedConsolidationScore(const MemoryItem& item) const;
};

} // namespace nlm
