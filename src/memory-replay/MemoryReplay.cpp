#include "MemoryReplay.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct MemoryReplay::Impl {
    Brain* brain;
    
    Impl() : brain(nullptr) {}
};

MemoryReplay::MemoryReplay()
    : pImpl(new Impl)
    , brain_(nullptr)
    , sleepPhaseDuration_(2000)
    , replayInterval_(100)
    , maxReplayEpisodes_(3)
    , enabled_(true)
    , replayActive_(false)
    , replayStartStep_(0)
    , replayEndStep_(0)
    , currentReplayIndex_(0)
    , replayCount_(0)
    , totalEpisodesReplayed_(0)
    , stepCounter_(0)
{
}

MemoryReplay::~MemoryReplay() = default;

void MemoryReplay::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("MemoryReplay initialized");
}

bool MemoryReplay::update(SimulationStep currentStep) {
    if (!enabled_ || !brain_) return false;
    
    stepCounter_++;
    
    // Check if we're in sleep phase
    if (isSleepPhase(currentStep)) {
        if (!replayActive_) {
            // Start replay during sleep phase
            auto episodes = getEpisodesForReplay(currentStep);
            if (!episodes.empty()) {
                startReplay(episodes);
                NLM_LOG_INFO("MemoryReplay: Starting replay during sleep phase at step " + std::to_string(currentStep));
            }
        }
    } else {
        // We're awake, stop replay if active
        if (replayActive_) {
            stopReplay();
            NLM_LOG_INFO("MemoryReplay: Stopping replay at step " + std::to_string(currentStep));
        }
    }
    
    // Process replay if active
    if (replayActive_) {
        processNextReplayEpisode();
        return true;
    }
    
    return false;
}

bool MemoryReplay::isSleepPhase(SimulationStep currentStep) const {
    if (!brain_) return false;
    
    // Sleep phases occur every N steps
    // In this simplified implementation, sleep happens at step intervals
    // More complex implementations would have duty cycles
    return (currentStep % sleepPhaseDuration_ == 0);
}

void MemoryReplay::startReplay(const std::vector<const EpisodicMemoryItem*>& episodes) {
    if (episodes.empty() || !brain_) return;
    
    // Calculate replay timing
    replayStartStep_ = stepCounter_;
    replayEndStep_ = replayStartStep_ + replayInterval_;
    replayActive_ = true;
    currentReplayIndex_ = 0;
    
    // Queue episodes for replay (in reverse temporal order for strengthening)
    replayQueue_ = calculateReverseTemporalOrder(episodes);
    
    NLM_LOG_INFO("MemoryReplay: Queued " + std::to_string(replayQueue_.size()) + " episodes for replay");
}

void MemoryReplay::stopReplay() {
    replayActive_ = false;
    replayQueue_.clear();
    currentReplayIndex_ = 0;
}

void MemoryReplay::forceReplay(SimulationStep currentStep) {
    NLM_LOG_INFO("MemoryReplay: Force replay at step " + std::to_string(currentStep));
    
    // Get episodes for replay
    auto episodes = getEpisodesForReplay(currentStep);
    if (!episodes.empty()) {
        startReplay(episodes);
    }
}

void MemoryReplay::reset() {
    stopReplay();
    replayCount_ = 0;
    totalEpisodesReplayed_ = 0;
    stepCounter_ = 0;
    NLM_LOG_INFO("MemoryReplay: Reset complete");
}

std::vector<const EpisodicMemoryItem*> MemoryReplay::getEpisodesForReplay(SimulationStep currentStep) const {
    if (!brain_ || !brain_->getEpisodicMemory()) return std::vector<const EpisodicMemoryItem*>();
    
    // Select episodes based on relevance and recency
    auto allEpisodes = brain_->getEpisodicMemory()->getRecentEpisodes(100);
    
    // Filter to high-reward episodes
    std::vector<const EpisodicMemoryItem*> candidates;
    for (const auto* episode : allEpisodes) {
        if (episode && episode->reward > 0.0f) {
            candidates.push_back(episode);
        }
    }
    
    // Sort by reward and recency (most recent/rewarding first)
    std::sort(candidates.begin(), candidates.end(),
        [](const EpisodicMemoryItem* a, const EpisodicMemoryItem* b) {
            if (a->reward != b->reward) {
                return a->reward > b->reward;
            }
            return a->timestamp > b->timestamp;
        });
    
    // Take top N episodes
    size_t count = std::min(maxReplayEpisodes_, candidates.size());
    return std::vector<const EpisodicMemoryItem*>(candidates.begin(), candidates.begin() + count);
}

void MemoryReplay::processNextReplayEpisode() {
    if (currentReplayIndex_ >= replayQueue_.size()) {
        // Finished all episodes
        replayCount_++;
        totalEpisodesReplayed_ += currentReplayIndex_;
        stopReplay();
        return;
    }
    
    // Get current episode
    const EpisodicMemoryItem* episode = replayQueue_[currentReplayIndex_];
    
    if (brain_ && episode) {
        // Replay the episode
        brain_->getEpisodicMemory()->replayEpisode(episode);
        
        currentReplayIndex_++;
        
        // Small delay between episode replays
        if (currentReplayIndex_ < replayQueue_.size()) {
            // In a real implementation, this would respect timing
            // For simulation, we process quickly
        }
    }
}

std::vector<const EpisodicMemoryItem*> MemoryReplay::calculateReverseTemporalOrder(
    const std::vector<const EpisodicMemoryItem*>& episodes) const {
    
    // Create a copy to sort
    std::vector<const EpisodicMemoryItem*> sorted = episodes;
    
    // Sort by timestamp (newest first) for reverse replay
    std::sort(sorted.begin(), sorted.end(),
        [](const EpisodicMemoryItem* a, const EpisodicMemoryItem* b) {
            return a->timestamp > b->timestamp;
        });
    
    // For strengthening, replay in reverse temporal order
    // (oldest first) to promote consolidation
    std::reverse(sorted.begin(), sorted.end());
    
    return sorted;
}

} // namespace nlm