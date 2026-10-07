#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include <vector>
#include <memory>
#include <algorithm>
#include <cmath>
#include <unordered_map>

namespace nlm {

// Emotional Memory Prioritizer
// Implements memory prioritization based on emotional valence
// Integrates neuromodulation signals (Dopamine, Novelty, Curiosity) to influence memory relevance
//
// Key mechanisms:
// - Multi-factor memory scoring (recency, reward, novelty, emotional valence)
// - Dynamic relevance adjustment based on current brain state
// - Integration with working memory for active maintenance
// - Hierarchical memory selection with emotional weighting

class EmotionalMemoryPrioritizer {
public:
    EmotionalMemoryPrioritizer();
    ~EmotionalMemoryPrioritizer();

    // Initialize with brain and neuromodulation references
    void initialize(Brain* brain, Dopamine* dopamine, Novelty* novelty, Curiosity* curiosity);

    // Prioritize episodes for replay/storage based on emotional valence
    // Returns episodes ordered by importance/relevance
    std::vector<const class EpisodicMemoryItem*> prioritizeEpisodes(
        const std::vector<const class EpisodicMemoryItem*>& episodes,
        size_t maxResults = 10,
        float timeWindowFactor = 1.0f) const;

    // Calculate emotional valence score for an episode
    // Integrates reward prediction error, novelty detection, and curiosity signals
    float calculateEmotionalValence(const class EpisodicMemoryItem& episode) const;

    // Update memory priorities based on current neuromodulation state
    void updatePriorities(TimestepDuration dt);

    // Get current emotional state influence
    float getCurrentEmotionalInfluence() const { return currentEmotionalInfluence_; }
    void setEmotionalWeight(float weight) { emotionalWeight_ = weight; }
    void setRecencyWeight(float weight) { recencyWeight_ = weight; }
    void setRewardWeight(float weight) { rewardWeight_ = weight; }
    void setNoveltyWeight(float weight) { noveltyWeight_ = weight; }

    // Configure priority thresholds
    void setPriorityThreshold(float threshold) { priorityThreshold_ = threshold; }
    void setConsolidationThreshold(float threshold) { consolidationThreshold_ = threshold; }

    // Get priority statistics
    struct PriorityStats {
        size_t totalPrioritized;
        float averageValenceScore;
        size_t highPriorityCount;
        size_t mediumPriorityCount;
        size_t lowPriorityCount;
        float emotionalInfluenceAverage;
    };

    PriorityStats getStats() const;

    // Reset statistics
    void resetStats();

private:
    // Enhanced priority scoring with emotional integration
    struct PrioritizedEpisode {
        const class EpisodicMemoryItem* episode;
        float valenceScore;
        float recencyScore;
        float rewardScore;
        float noveltyScore;
        float finalScore;
        
        PrioritizedEpisode(const class EpisodicMemoryItem* ep)
            : episode(ep), valenceScore(0.0f), recencyScore(0.0f), 
              rewardScore(0.0f), noveltyScore(0.0f), finalScore(0.0f) {}
    };

    // Calculate individual priority components
    float calculateRecencyScore(const class EpisodicMemoryItem& episode) const;
    float calculateRewardScore(const class EpisodicMemoryItem& episode) const;
    float calculateNoveltyScore(const class EpisodicMemoryItem& episode) const;
    float calculateEmotionalValenceComponent(const class EpisodicMemoryItem& episode) const;

    // Neuromodulation integration
    void updateNeuromodulationState();
    float getDopamineModulation() const;
    float getNoveltyModulation() const;
    float getCuriosityModulation() const;

    // Dynamic threshold adjustment
    float adjustThresholds(TimestepDuration dt) const;
    float getDynamicPriorityThreshold() const;
    float getDynamicConsolidationThreshold() const;

    // Priority-based selection algorithms
    void applyEmotionalModulation(std::vector<PrioritizedEpisode>& episodes) const;
    void applyAttentionBias(std::vector<PrioritizedEpisode>& episodes) const;
    void applyMemorySaturation(std::vector<PrioritizedEpisode>& episodes) const;

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    Dopamine* dopamine_;
    Novelty* novelty_;
    Curiosity* curiosity_;
    
    // Priority weights (configurable)
    float emotionalWeight_;
    float recencyWeight_;
    float rewardWeight_;
    float noveltyWeight_;
    
    // Dynamic thresholds
    float priorityThreshold_;
    float consolidationThreshold_;
    
    // Current brain state
    float currentEmotionalInfluence_;
    float averageNeuromodulation_;
    SimulationStep lastConsolidationStep_;
    
    // Statistics
    mutable size_t totalPrioritized_;
    mutable float accumulatedValenceScore_;
    mutable size_t highPriorityCount_;
    mutable size_t mediumPriorityCount_;
    mutable size_t lowPriorityCount_;
    mutable float accumulatedEmotionalInfluence_;
};

} // namespace nlm
