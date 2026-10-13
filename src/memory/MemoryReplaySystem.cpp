#include "MemoryReplaySystem.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"
#include <algorithm>
#include <cmath>
#include <chrono>
#include <limits>

namespace nlm {

struct MemoryReplaySystem::Impl {
    NeuralEpisodicMemory* episodicMemory;
    Config* config;
    
    // Pattern neurons for replay
    std::vector<NeuronId> replayNeurons;
    
    // Replay state
    bool replaying;
    ReplayMode mode;
    ReplayStrategy strategy;
    
    // Timing
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point lastStepTime;
    
    // Statistics
    size_t totalReplayed;
    size_t successful;
    float qualitySum;
    
    Impl() : episodicMemory(nullptr), config(nullptr), replaying(false), mode(ReplayMode::SEQUENTIAL),
             strategy(ReplayStrategy::RELEVANCE_BASED), totalReplayed(0), successful(0), qualitySum(0.0f) {}
};

MemoryReplaySystem::MemoryReplaySystem()
    : pImpl(new Impl)
    , episodicMemory_(nullptr)
    , config_(nullptr)
    , currentEpisodeIndex_(0)
    , replayMode_(ReplayMode::SEQUENTIAL)
    , replayStrategy_(ReplayStrategy::RELEVANCE_BASED)
    , replayRate_(1.0f)
    , relevanceThreshold_(0.5f)
    , replayStartTime_(0)
    , replayEndTime_(std::numeric_limits<SimulationStep>::max())
    , replayEnabled_(true)
    , replayActive_(false)
    , totalReplayCount_(0)
    , successfulReplays_(0)
    , averageReplayQuality_(0.0f)
{
}

MemoryReplaySystem::~MemoryReplaySystem() = default;

void MemoryReplaySystem::initialize(NeuralEpisodicMemory* episodicMemory, Config* config) {
    pImpl->episodicMemory = episodicMemory;
    pImpl->config = config;
    episodicMemory_ = episodicMemory;
    config_ = config;
    
    // Configure based on config if available
    if (config_) {
        replayRate_ = config_->getFloat("replay.rate", 1.0f);
        relevanceThreshold_ = config_->getFloat("replay.relevanceThreshold", 0.5f);
        replayEnabled_ = config_->getBool("replay.enabled", true);
    }
    
    NLM_LOG_INFO("MemoryReplaySystem initialized");
}

void MemoryReplaySystem::startReplay(const std::vector<size_t>& episodeIds, ReplayMode mode) {
    if (!episodicMemory_ || episodeIds.empty()) return;
    
    replayEpisodeIds_ = episodeIds;
    currentEpisodeIndex_ = 0;
    replayMode_ = mode;
    replayActive_ = true;
    pImpl->replaying = true;
    pImpl->mode = mode;
    
    // Record start time for rate control
    pImpl->startTime = std::chrono::steady_clock::now();
    pImpl->lastStepTime = pImpl->startTime;
    
    replayStartTime_ = episodicMemory_->getEpisode(currentEpisodeIndex_) ? 
                      episodicMemory_->getEpisode(currentEpisodeIndex_)->timestamp : 0;
    
    NLM_LOG_INFO("MemoryReplaySystem: Started replay of {} episodes", episodeIds.size());
}

void MemoryReplaySystem::stopReplay() {
    replayActive_ = false;
    pImpl->replaying = false;
    
    NLM_LOG_INFO("MemoryReplaySystem: Stopped replay");
}

bool MemoryReplaySystem::isReplaying() const {
    return replayActive_;
}

const EpisodicMemoryItem* MemoryReplaySystem::getNextEpisode() {
    if (currentEpisodeIndex_ >= replayEpisodeIds_.size()) return nullptr;
    
    return episodicMemory_->getEpisode(replayEpisodeIds_[currentEpisodeIndex_]);
}

void MemoryReplaySystem::advanceToNextEpisode() {
    currentEpisodeIndex_++;
    
    // Check if replay is complete
    if (currentEpisodeIndex_ >= replayEpisodeIds_.size()) {
        stopReplay();
    }
}

void MemoryReplaySystem::setReplayTimeWindow(SimulationStep startTime, SimulationStep endTime) {
    replayStartTime_ = startTime;
    replayEndTime_ = endTime;
}

std::vector<size_t> MemoryReplaySystem::selectEpisodesForReplay(size_t maxEpisodes) const {
    std::vector<size_t> selected;
    
    if (!episodicMemory_) return selected;
    
    // Get all episodes
    std::vector<size_t> allEpisodeIds;
    for (size_t i = 0; i < episodicMemory_->getEpisodeCount(); ++i) {
        if (auto* episode = episodicMemory_->getEpisode(i)) {
            allEpisodeIds.push_back(i);
        }
    }
    
    if (allEpisodeIds.empty()) return selected;
    
    // Score episodes based on strategy
    std::vector<std::pair<float, size_t>> scored;
    
    for (size_t id : allEpisodeIds) {
        if (auto* episode = episodicMemory_->getEpisode(id)) {
            float score = 0.0f;
            
            switch (replayStrategy_) {
                case ReplayStrategy::RELEVANCE_BASED:
                    // Score based on reward and recency
                    score = episode->reward * 0.7f;
                    score += 1.0f / (episode->age + 1) * 0.3f;
                    break;
                    
                case ReplayStrategy::TIME_BASED:
                    // Score based on timestamp
                    if (episode->timestamp >= replayStartTime_ && 
                        episode->timestamp <= replayEndTime_) {
                        score = 1.0f;
                    }
                    break;
                    
                case ReplayStrategy::RANDOM:
                    // Random selection
                    score = static_cast<float>(rand()) / RAND_MAX;
                    break;
                    
                case ReplayStrategy::QUALITY_BASED:
                    // Score based on neural pattern consistency
                    // This would require more complex analysis in full implementation
                    score = 0.5f;  // Placeholder
                    break;
            }
            
            if (score >= relevanceThreshold_) {
                scored.emplace_back(score, id);
            }
        }
    }
    
    // Sort by score (descending)
    std::sort(scored.begin(), scored.end(),
        [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // Select top episodes
    size_t count = std::min(maxEpisodes, scored.size());
    for (size_t i = 0; i < count; ++i) {
        selected.push_back(scored[i].second);
    }
    
    return selected;
}

ReplayStats MemoryReplaySystem::getReplayStats() const {
    ReplayStats stats;
    stats.totalReplayed = totalReplayCount_;
    stats.successfulReplays = successfulReplays_;
    stats.averageQuality = averageReplayQuality_;
    stats.currentIndex = currentEpisodeIndex_;
    stats.totalEpisodes = replayEpisodeIds_.size();
    return stats;
}

void MemoryReplaySystem::update(SimulationStep currentTime) {
    if (!replayActive_ || !episodicMemory_) return;
    
    // Control replay rate
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - pImpl->lastStepTime).count();
    
    // Calculate expected elapsed time based on replay rate
    float expectedStepTime = 1000.0f / replayRate_;  // milliseconds per episode
    
    // Check if it's time for the next replay step
    if (elapsed >= expectedStepTime) {
        // Perform replay of current episode
        if (currentEpisodeIndex_ < replayEpisodeIds_.size()) {
            size_t episodeId = replayEpisodeIds_[currentEpisodeIndex_];
            if (episodeId < episodicMemory_->getEpisodeCount()) {
                // Replay the episode
                if (auto* episode = episodicMemory_->getEpisode(episodeId)) {
                    // In a full implementation, this would actually replay the neural patterns
                    // For now, we just increment the index
                    advanceToNextEpisode();
                    
                    // Update statistics
                    pImpl->totalReplayed++;
                    pImpl->successful++;
                    
                    // Update running average quality
                    float quality = 1.0f;  // Placeholder - would compute actual quality
                    pImpl->qualitySum += quality;
                    averageReplayQuality_ = pImpl->qualitySum / pImpl->totalReplayed;
                    
                    // Log replay
                    NLM_LOG_DEBUG("MemoryReplaySystem: Replayed episode {}", episodeId);
                }
            }
        }
        
        pImpl->lastStepTime = now;
    }
}

float MemoryReplaySystem::getReplayProgress() const {
    if (replayEpisodeIds_.empty()) return 0.0f;
    return static_cast<float>(currentEpisodeIndex_) / replayEpisodeIds_.size();
}

void MemoryReplaySystem::clear() {
    replayEpisodeIds_.clear();
    currentEpisodeIndex_ = 0;
    replayActive_ = false;
    pImpl->replaying = false;
    
    totalReplayCount_ = 0;
    successfulReplays_ = 0;
    averageReplayQuality_ = 0.0f;
}

} // namespace nlm
