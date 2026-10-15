// Performance optimizations for NeuralEpisodicMemory
#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <deque>
#include <functional>
#include <unordered_map>
#include <chrono>
#include "../performance/Performance.hpp"

namespace nlm {

// Enhanced EpisodicMemoryItem with performance metadata
struct EpisodicMemoryItem {
    SimulationStep timestamp;
    
    // Sensory state at time of experience (encoded as activity pattern)
    std::vector<float> sensoryState;
    
    // Location/position information
    float positionX;
    float positionY;
    float orientation;
    
    // Action taken (if any)
    ActionType action;
    
    // Internal state (energy, reward, etc.)
    float reward;
    float energy;
    float novelty;
    
    // Consequence of action
    std::vector<float> resultingSensoryState;
    float resultingReward;
    
    // Associated neural activity pattern (who was firing)
    std::vector<NeuronId> activeNeurons;
    std::vector<float> neuronActivations;
    
    // Time since this episode
    SimulationStep age;
    
    // Performance metadata
    mutable size_t accessCount;
    mutable std::chrono::steady_clock::time_point lastAccessTime;
    mutable float cacheScore;
    
    EpisodicMemoryItem()
        : timestamp(0), positionX(0), positionY(0), orientation(0)
        , action(ActionType::Wait)
        , reward(0), energy(0), novelty(0)
        , resultingReward(0), age(0)
        , accessCount(0), cacheScore(0.0f) {}
};

// Performance-optimized NeuralEpisodicMemory with caching and SIMD
class NeuralEpisodicMemory {
public:
    NeuralEpisodicMemory();
    ~NeuralEpisodicMemory();
    
    // Disable copying
    NeuralEpisodicMemory(const NeuralEpisodicMemory&) = delete;
    NeuralEpisodicMemory& operator=(const NeuralEpisodicMemory&) = delete;
    
    // Initialize memory with brain reference
    void initialize(Brain* brain);
    
    // Add a new episode
    void storeEpisode(const EpisodicMemoryItem& episode);
    
    // Get episodes for replay with performance optimizations
    std::vector<EpisodicMemoryItem> getEpisodesForReplay(size_t maxEpisodes = 10) const;
    
    // Replay specific episode with performance optimizations
    bool replayEpisode(size_t episodeIndex, Brain* brain) const;
    
    // Get episode count
    size_t getEpisodeCount() const;
    
    // Get recent episodes
    std::vector<EpisodicMemoryItem> getRecentEpisodes(size_t count) const;
    
    // Clear all episodes
    void clear();
    
    // Get memory usage statistics
    size_t getMemoryUsage() const;
    
    // Performance optimization methods
    void setCachingEnabled(bool enabled);
    bool isCachingEnabled() const;
    
    // Get cached frequently accessed patterns
    std::vector<float> getCachedPattern(size_t episodeIndex) const;
    void cachePattern(size_t episodeIndex, const std::vector<float>& pattern);
    
    // Force cache cleanup
    void cleanupCache();
    
    // Set cache size limit
    void setCacheSizeLimit(size_t limit);
    
    // SIMD-accelerated similarity computation
    float computeSimilaritySIMD(const std::vector<float>& query,
                               const EpisodicMemoryItem& episode) const;
    
    // Batch processing for performance
    void batchUpdateRelevance(std::chrono::milliseconds timeWindow);
    
    // Performance monitoring
    float getCacheHitRatio() const;
    size_t getCacheSize() const;
    double getAverageAccessTime() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// SIMD-optimized pattern similarity computation
namespace simd {
    // Compute cosine similarity between two vectors using SIMD
    float cosineSimilaritySIMD(const std::vector<float>& a, const std::vector<float>& b);
    
    // Compute dot product using SIMD
    float dotProductSIMD(const std::vector<float>& a, const std::vector<float>& b);
    
    // Normalize vector using SIMD
    std::vector<float> normalizeSIMD(const std::vector<float>& vector);
    
    // Compute magnitude using SIMD
    float magnitudeSIMD(const std::vector<float>& vector);
}

} // namespace nlm
