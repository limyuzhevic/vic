#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <memory>
#include <vector>
#include <set>
#include <queue>
#include <chrono>

namespace nlm {

/**
 * MemoryReplay: System for replaying consolidated memories during sleep/rest phases
 * 
 * Key mechanisms:
 * - Selects episodes from episodic memory for replay
 * - Operates during sleep/rest phases to strengthen synaptic connections
 * - Replay timing follows optimal theories (offline consolidation)
 * - Repeats important memories in reverse order for strengthening
 * - Integrates with sleep/wake cycle management
 * 
 * Integration:
 * - Receives input from memory consolidation system
 * - Operates during sleep/rest phases
 * - Replays memories to strengthen neural connections
 * - Works independently of active memory systems
 * - Can be interrupted during wake periods
 */

class MemoryReplay {
public:
    MemoryReplay();
    ~MemoryReplay();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Update replay system (called during sleep/rest phases)
    // Returns true if replay is active this step
    bool update(SimulationStep currentStep);
    
    // Check if we are in a sleep/rest phase where replay should occur
    bool isSleepPhase(SimulationStep currentStep) const;
    
    // Start replay sequence with selected episodes
    void startReplay(const std::vector<const EpisodicMemoryItem*>& episodes);
    
    // Stop replay (waking up or interrupted)
    void stopReplay();
    
    // Is replay currently active?
    bool isReplayActive() const { return replayActive_; }
    
    // Set sleep phase parameters
    void setSleepPhaseDuration(size_t duration) { sleepPhaseDuration_ = duration; }
    void setReplayInterval(size_t interval) { replayInterval_ = interval; }
    void setMaxReplayEpisodes(size_t max) { maxReplayEpisodes_ = max; }
    
    // Enable/disable replay
    void enable(bool enable) { enabled_ = enable; }
    bool isEnabled() const { return enabled_; }
    
    // Get replay statistics
    size_t getReplayCount() const { return replayCount_; }
    size_t getTotalEpisodesReplayed() const { return totalEpisodesReplayed_; }
    size_t getCurrentReplayIndex() const { return currentReplayIndex_; }
    
    // Get episodes selected for replay
    std::vector<const EpisodicMemoryItem*> getEpisodesForReplay(SimulationStep currentStep) const;
    
    // Force replay regardless of sleep phase
    void forceReplay(SimulationStep currentStep);
    
    // Check if system is ready (has brain reference)
    bool isReady() const { return brain_ != nullptr; }
    
    // Reset replay state
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    
    // Replay parameters
    size_t sleepPhaseDuration_;
    size_t replayInterval_;
    size_t maxReplayEpisodes_;
    bool enabled_;
    
    // Replay state
    bool replayActive_;
    SimulationStep replayStartStep_;
    SimulationStep replayEndStep_;
    size_t currentReplayIndex_;
    
    // Statistics
    size_t replayCount_;
    size_t totalEpisodesReplayed_;
    
    // Queue of episodes to replay
    std::vector<const EpisodicMemoryItem*> replayQueue_;
    
    // Replay timing
    SimulationStep stepCounter_;
    
    // Calculate sleep phase boundaries
    SimulationStep calculateSleepStart(SimulationStep currentStep) const;
    SimulationStep calculateSleepEnd(SimulationStep currentStep) const;
    
    // Select episodes for replay based on relevance
    std::vector<const EpisodicMemoryItem*> selectEpisodesForReplay(SimulationStep currentStep) const;
    
    // Queue episodes for replay (reverse order for strengthening)
    void queueEpisodesForReplay(const std::vector<const EpisodicMemoryItem*>& episodes);
    
    // Process next episode in replay queue
    void processNextReplayEpisode();
    
    // Calculate replay order (reverse temporal for strengthening)
    std::vector<const EpisodicMemoryItem*> calculateReverseTemporalOrder(
        const std::vector<const EpisodicMemoryItem*>& episodes) const;
};

} // namespace nlm