#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>
#include <string>
#include <algorithm>
#include <cmath>
#include <chrono>

namespace nlm {

// Episode: A unit of experience consisting of neural patterns over time
struct Episode {
    size_t id;
    Timestamp startTime;
    Timestamp endTime;
    std::vector<std::vector<float>> neuralPatterns;  // Neural activity patterns
    std::vector<Timestamp> spikeTimestamps;           // When spikes occurred
    std::vector<float> rewardSignals;                 // Reward received
    std::vector<float> predictionErrors;             // Prediction errors
    
    Episode(size_t episodeId) 
        : id(episodeId), startTime(0.0f), endTime(0.0f) {}
    
    float getDuration() const {
        return endTime - startTime;
    }
    
    size_t getPatternCount() const {
        return neuralPatterns.size();
    }
};

// NeuralEpisodicMemory: Stores experiences for later replay and consolidation
// Implements pattern completion and temporal sequencing
class NeuralEpisodicMemory {
public:
    NeuralEpisodicMemory();
    ~NeuralEpisodicMemory();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Set memory capacity
    void setMaxEpisodes(size_t maxEpisodes);
    
    // Record an episode
    void recordEpisode(const std::vector<std::vector<float>>& patterns,
                       const std::vector<Timestamp>& spikes,
                       float reward = 0.0f, float predictionError = 0.0f);
    
    // Get episodes for replay
    std::vector<size_t> getEpisodesForReplay(size_t count = 3) const;
    
    // Retrieve specific episode
    Episode getEpisode(size_t episodeId) const;
    
    // Pattern completion from partial cue
    std::vector<float> completePattern(const std::vector<float>& partialPattern) const;
    
    // Query state
    size_t getEpisodeCount() const;
    size_t getMaxEpisodes() const;
    float getConsolidationProgress() const;
    
    // Replay mechanism
    void replayEpisode(size_t episodeId, float speed = 1.0f);
    void replaySequence(const std::vector<size_t>& episodeIds);
    
    // Consolidation (transfer to long-term storage)
    void consolidateEpisodes(const std::vector<size_t>& episodeIds);
    void consolidateImportantEpisodes();
    
    // Integration with brain systems
    void update(float timeStep);
    void integrateWithWorkingMemory(WorkingMemory* workingMem);
    
    // Reset
    void reset();
    
    // Aging and decay
    void applyAging(float decayRate);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm