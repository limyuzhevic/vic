#pragma once

#include "ReplaySystem.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct ReplaySystem::Impl {
    Brain* brain;
    
    // Replay event queue
    std::vector<size_t> replayQueue;
    
    // Last replay times for timing optimization
    std::unordered_map<size_t, size_t> lastReplayTimes;
    
    // Replay success tracking
    std::unordered_map<size_t, size_t> replaySuccessCount;
    
    Impl() : brain(nullptr), replayStrengthMultiplier(1.0f), 
             importanceWeight(0.5f), recencyWeight(0.3f), 
             rewardWeight(0.2f), replayDuringRest(true), 
             totalReplays(0), successfulReplays(0) {}
    
    float replayStrengthMultiplier;
    float importanceWeight;
    float recencyWeight;
    float rewardWeight;
};

ReplaySystem::ReplaySystem()
    : pImpl(new Impl)
    , brain_(nullptr)
    , replayStrengthMultiplier_(1.0f)
    , importanceWeight_(0.5f)
    , recencyWeight_(0.3f)
    , rewardWeight_(0.2f)
    , replayDuringRest_(true)
    , totalReplays_(0)
    , successfulReplays_(0)
{
}

ReplaySystem::~ReplaySystem() = default;

void ReplaySystem::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("ReplaySystem initialized");
}

void ReplaySystem::update(float dt) {
    if (!brain_ || !brain_->getEpisodicMemory()) return;
    
    auto* em = brain_->getEpisodicMemory();
    
    // Process learning events for timing optimization
    while (!learningEvents_.empty()) {
        auto event = learningEvents_.front();
        learningEvents_.pop_front();
        
        // Adjust replay timing based on learning importance
        adjustReplayTiming(event.episodeId);
    }
    
    // Perform scheduled replays
    std::vector<size_t> episodesToReplay;
    
    if (brain_->isResting() && replayDuringRest_) {
        // During rest: replay more episodes
        episodesToReplay = getEpisodesForReplay(std::min(size_t(10), em->getEpisodeCount()));
    } else {
        // During active: replay fewer episodes
        episodesToReplay = getEpisodesForReplay(3);
    }
    
    // Replay selected episodes
    for (size_t episodeId : episodesToReplay) {
        replayEpisode(episodeId, replayStrengthMultiplier_);
    }
}

void ReplaySystem::replayEpisode(size_t episodeId, float strength) {
    if (!brain_ || !brain_->getEpisodicMemory()) return;
    
    auto* em = brain_->getEpisodicMemory();
    const auto* episode = em->getEpisode(episodeId);
    
    if (!episode) return;
    
    // Validate replay opportunity
    if (!validateReplay(*episode)) {
        NLM_LOG_INFO("Replay validation failed for episode " + std::to_string(episodeId));
        return;
    }
    
    // Perform replay by reactivating neural patterns
    brain_->getEpisodicMemory()->replayEpisode(episode);
    
    // Track replay statistics
    ++pImpl->totalReplays_;
    ++pImpl->successfulReplays_;
    
    // Record success
    pImpl->replaySuccessCount[episodeId]++;
    
    // Update episode relevance (replay strengthens memory)
    em->updateRelevance(episodeId, strength * 0.1f);
    
    // Trigger replay-dependent learning
    triggerReplayDependentLearning(episodeId, strength);
    
    // Update last replay time for timing optimization
    pImpl->lastReplayTimes[episodeId] = pImpl->brain ? pImpl->brain->getCurrentStep() : 0;
    
    NLM_LOG_INFO("Replayed episode " + std::to_string(episodeId) + 
                " with strength " + std::to_string(strength));
}

void ReplaySystem::recordLearningEvent(size_t episodeId, float importance) {
    LearningEvent event;
    event.episodeId = episodeId;
    event.importance = importance;
    event.timestamp = pImpl->brain ? pImpl->brain->getCurrentStep() : 0;
    
    learningEvents_.push_back(event);
}

void ReplaySystem::adjustReplayTiming(size_t episodeId) {
    // If episode was learned recently with high importance,
    // prioritize its replay
    auto currentStep = pImpl->brain ? pImpl->brain->getCurrentStep() : 0;
    
    auto it = pImpl->lastReplayTimes.find(episodeId);
    if (it != pImpl->lastReplayTimes.end()) {
        size_t timeSinceLastReplay = currentStep - it->second;
        
        // If last replay was long ago, prioritize this episode
        if (timeSinceLastReplay > 100) {
            // Move to front of replay queue
            auto findIt = std::find(replayQueue_.begin(), replayQueue_.end(), episodeId);
            if (findIt != replayQueue_.end()) {
                replayQueue_.erase(findIt);
                replayQueue_.insert(replayQueue_.begin(), episodeId);
            }
        }
    }
}

bool ReplaySystem::validateReplay(const EpisodicMemoryItem& episode) const {
    // Validate replay based on episode state and brain condition
    bool valid = true;
    
    // Check if episode is still relevant
    if (episode.age > 1000) {  // Too old
        valid = false;
    }
    
    // Check if episode has sufficient reward
    if (episode.reward < 0.1f && !valid) {  // Negative or zero reward
        valid = false;
    }
    
    return valid;
}

void ReplaySystem::triggerReplayDependentLearning(size_t episodeId, float strength) {
    if (!brain_ || !brain_->getSTDP()) return;
    
    // Replay-dependent learning: reactivate synaptic connections
    // This is done through the episodic memory replay mechanism
    auto* em = brain_->getEpisodicMemory();
    const auto* episode = em->getEpisode(episodeId);
    
    if (!episode) return;
    
    // The replay already triggers plasticity through neural reactivation
    // Additional learning can be triggered here if needed
}

std::vector<size_t> ReplaySystem::getEpisodesForReplay(size_t count) const {
    if (!brain_ || !brain_->getEpisodicMemory()) return {};
    
    auto* em = brain_->getEpisodicMemory();
    
    std::vector<std::tuple<float, size_t>> scoredEpisodes;  // (score, episodeId)
    
    for (size_t i = 0; i < em->getEpisodeCount(); ++i) {
        const auto* episode = em->getEpisode(i);
        
        float importanceScore = 0.0f;
        
        // Importance factor
        float recencyScore = 1.0f / (episode->age + 1);
        importanceScore += recencyScore * pImpl->recencyWeight;
        
        // Reward factor
        float rewardScore = std::min(1.0f, episode->reward / 10.0f);
        importanceScore += rewardScore * pImpl->rewardWeight;
        
        // Novelty factor (higher novelty = more important to replay)
        float noveltyScore = std::min(1.0f, episode->novelty / 5.0f);
        importanceScore += noveltyScore * 0.1f;
        
        scoredEpisodes.emplace_back(importanceScore, i);
    }
    
    // Sort by importance score (descending)
    std::sort(scoredEpisodes.begin(), scoredEpisodes.end(),
              [](const auto& a, const auto& b) {
                  return std::get<0>(a) > std::get<0>(b);
              });
    
    std::vector<size_t> result;
    for (size_t i = 0; i < std::min(count, scoredEpisodes.size()); ++i) {
        result.push_back(std::get<1>(scoredEpisodes[i]));
    }
    
    return result;
}

} // namespace nlm