#pragma once

#include "NeuralEpisodicMemory.hpp"
#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"
#include <vector>
#include <memory>
#include <unordered_set>
#include <chrono>

namespace nlm {

class MemoryReplaySystem {
public:
    MemoryReplaySystem();
    ~MemoryReplaySystem();
    
    // Initialize the replay system
    void initialize(NeuralEpisodicMemory* episodicMemory, Config* config);
    
    // Start replay of selected episodes
    void startReplay(const std::vector<size_t>& episodeIds, ReplayMode mode = ReplayMode::SEQUENTIAL);
    
    // Stop replay
    void stopReplay();
    
    // Check if replay is active
    bool isReplaying() const;
    
    // Get next episode to replay
    const EpisodicMemoryItem* getNextEpisode();
    
    // Advance to next episode in sequence
    void advanceToNextEpisode();
    
    // Set replay rate (episodes per second)
    void setReplayRate(float rate) { replayRate_ = rate; }
    float getReplayRate() const { return replayRate_; }
    
    // Configure replay strategy
    void setReplayStrategy(ReplayStrategy strategy) { replayStrategy_ = strategy; }
    ReplayStrategy getReplayStrategy() const { return replayStrategy_; }
    
    // Set replay bounds (time window)
    void setReplayTimeWindow(SimulationStep startTime, SimulationStep endTime);
    void setReplayRelevanceThreshold(float threshold) { relevanceThreshold_ = threshold; }
    
    // Select episodes for replay based on strategy
    std::vector<size_t> selectEpisodesForReplay(size_t maxEpisodes = 10) const;
    
    // Get current replay statistics
    ReplayStats getReplayStats() const;
    
    // Enable/disable replay
    void enableReplay(bool enable) { replayEnabled_ = enable; }
    bool isReplayEnabled() const { return replayEnabled_; }
    
    // Update replay system (called each simulation step)
    void update(SimulationStep currentTime);
    
    // Get replay progress percentage
    float getReplayProgress() const;
    
    // Get current episode index in replay sequence
    size_t getCurrentEpisodeIndex() const { return currentEpisodeIndex_; }
    
    // Get total episodes in current replay
    size_t getTotalEpisodesInReplay() const { return replayEpisodeIds_.size(); }
    
    // Clear replay state
    void clear();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    NeuralEpisodicMemory* episodicMemory_;
    Config* config_;
    
    std::vector<size_t> replayEpisodeIds_;
    size_t currentEpisodeIndex_;
    ReplayMode replayMode_;
    ReplayStrategy replayStrategy_;
    float replayRate_;
    float relevanceThreshold_;
    
    SimulationStep replayStartTime_;
    SimulationStep replayEndTime_;
    bool replayEnabled_;
    bool replayActive_;
    
    // Timing for rate control
    std::chrono::steady_clock::time_point lastReplayStep_;
    
    // Statistics
    size_t totalReplayCount_;
    size_t successfulReplays_;
    float averageReplayQuality_;
};

} // namespace nlm
