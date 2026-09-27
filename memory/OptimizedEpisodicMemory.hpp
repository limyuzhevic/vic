#pragma once

#include "OptimizedEpisodicMemory.hpp"
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <memory>
#include <mutex>
#include <chrono>
#include <cmath>

namespace nlm {

class OptimizedEpisodicMemory {
public:
    struct EpisodicMemoryItem {
        size_t episodeId;
        SimulationStep timestamp;
        float reward;
        std::vector<NeuronId> activeNeurons;
        std::vector<float> neuronActivations;
        bool compressed;
        size_t compressedSize;
        
        EpisodicMemoryItem() : episodeId(0), timestamp(0), reward(0.0f), 
                              compressed(false), compressedSize(0) {}
        EpisodicMemoryItem(size_t id, SimulationStep ts, float r)
            : episodeId(id), timestamp(ts), reward(r), compressed(false), compressedSize(0) {}
    };

    OptimizedEpisodicMemory();
    ~OptimizedEpisodicMemory();

    // Disable copying, enable moving
    OptimizedEpisodicMemory(const OptimizedEpisodicMemory&) = delete;
    OptimizedEpisodicMemory& operator=(const OptimizedEpisodicMemory&) = delete;
    OptimizedEpisodicMemory(OptimizedEpisodicMemory&&) noexcept;
    OptimizedEpisodicMemory& operator=(OptimizedEpisodicMemory&&) noexcept;

    // Memory management
    void storeEpisode(const EpisodicMemoryItem& episode);
    const EpisodicMemoryItem* getEpisode(size_t episodeId) const;
    std::vector<const EpisodicMemoryItem*> getEpisodes(size_t count) const;
    std::vector<const EpisodicMemoryItem*> getEpisodesForReplay(size_t count) const;
    void replayEpisode(const EpisodicMemoryItem* episode);
    void consolidate(float threshold);

    // Management
    void clear();
    void setMaxEpisodes(size_t maxEpisodes);
    size_t getMaxEpisodes() const;
    size_t getEpisodeCount() const;

    // Optimization
    void enableCompression(bool enable);
    void setCompressionRatio(float ratio);
    void enableEfficientReplay(bool enable);
    void optimizeStoragePattern();
    void cleanupOldEpisodes(float ageThreshold);

    // Statistics
    float getAverageEpisodeReward() const;
    size_t getCompressedMemorySize() const;
    std::string getStatistics() const;

private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
