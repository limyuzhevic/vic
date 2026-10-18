#pragma once

#include "IMemorySystem.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct DefaultEpisodicMemory::Impl {
    Brain* brain_;
    std::vector<EpisodicMemoryItem> episodes_;
    size_t maxEpisodes_;
    bool replayEnabled_;
    
    DefaultEpisodicMemory::Impl() : brain_(nullptr), maxEpisodes_(1000), replayEnabled_(false) {}
};

DefaultEpisodicMemory::DefaultEpisodicMemory() : pImpl(std::make_unique<Impl>()), enabled_(true), replayEnabled_(false) {}

DefaultEpisodicMemory::~DefaultEpisodicMemory() = default;

void DefaultEpisodicMemory::initialize(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot initialize DefaultEpisodicMemory with null brain");
        return;
    }
    
    pImpl->brain_ = brain;
    NLM_LOG_INFO("DefaultEpisodicMemory initialized");
}

void DefaultEpisodicMemory::storeEpisode(const std::vector<float>& sensoryState,
                                        float reward,
                                        float energy,
                                        float novelty,
                                        const std::vector<NeuronId>& activeNeurons,
                                        const std::vector<float>& neuronActivations,
                                        SimulationStep timestamp) {
    if (!enabled_) return;
    
    EpisodicMemoryItem episode;
    episode.timestamp = timestamp;
    episode.sensoryState = sensoryState;
    episode.reward = reward;
    episode.energy = energy;
    episode.novelty = novelty;
    episode.activeNeurons = activeNeurons;
    episode.neuronActivations = neuronActivations;
    episode.age = 0;
    
    pImpl->episodes_.push_back(episode);
    
    // Trim episodes if exceeds maximum
    if (pImpl->episodes_.size() > pImpl->maxEpisodes_) {
        pImpl->episodes_.erase(pImpl->episodes_.begin());
    }
    
    NLM_LOG_INFO("Stored episodic memory at step " + std::to_string(timestamp) + ", reward = " + std::to_string(reward));
}

std::vector<const void*> DefaultEpisodicMemory::retrieveSimilar(const std::vector<float>& sensoryPattern,
                                                                size_t maxResults) const {
    if (!enabled_) return std::vector<const void*>();
    
    std::vector<const void*> results;
    results.reserve(std::min(maxResults, pImpl->episodes_.size()));
    
    for (const auto& episode : pImpl->episodes_) {
        float similarity = computeSimilarity(sensoryPattern, episode);
        if (similarity > 0.1f) { // Threshold for relevance
            results.push_back(&episode);
        }
        if (results.size() >= maxResults) break;
    }
    
    return results;
}

std::vector<const void*> DefaultEpisodicMemory::retrieveTemporal(SimulationStep startTime,
                                                                SimulationStep endTime,
                                                                size_t maxResults) const {
    if (!enabled_) return std::vector<const void*>();
    
    std::vector<const void*> results;
    results.reserve(std::min(maxResults, pImpl->episodes_.size()));
    
    for (const auto& episode : pImpl->episodes_) {
        if (episode.timestamp >= startTime && episode.timestamp <= endTime) {
            results.push_back(&episode);
            if (results.size() >= maxResults) break;
        }
    }
    
    return results;
}

size_t DefaultEpisodicMemory::getEpisodeCount() const {
    if (!enabled_) return 0;
    return pImpl->episodes_.size();
}

std::vector<const void*> DefaultEpisodicMemory::getEpisodesForReplay(size_t count) const {
    if (!enabled_ || !replayEnabled_) return std::vector<const void*>();
    
    std::vector<const void*> results;
    size_t episodeCount = pImpl->episodes_.size();
    size_t replayCount = std::min(count, episodeCount);
    
    // Select episodes for replay (simplified: use most recent)
    for (size_t i = 0; i < replayCount; ++i) {
        size_t index = episodeCount - 1 - i;
        results.push_back(&pImpl->episodes_[index]);
    }
    
    return results;
}

void DefaultEpisodicMemory::enableReplay(bool enable) {
    replayEnabled_ = enable;
}

bool DefaultEpisodicMemory::isReplayEnabled() const {
    return replayEnabled_;
}

void DefaultEpisodicMemory::update(TimestepDuration dt) {
    if (!enabled_) return;
    
    // Update episode ages
    for (auto& episode : pImpl->episodes_) {
        episode.age += static_cast<SimulationStep>(dt);
    }
}

void DefaultEpisodicMemory::clear() {
    pImpl->episodes_.clear();
}

void DefaultEpisodicMemory::consolidate(float relevanceThreshold) {
    // Remove low-relevance episodes
    auto it = std::remove_if(pImpl->episodes_.begin(), pImpl->episodes_.end(),
        [this](const EpisodicMemoryItem& episode) {
            return computeRelevance(episode) < relevanceThreshold;
        });
    
    pImpl->episodes_.erase(it, pImpl->episodes_.end());
}

float DefaultEpisodicMemory::getAverageReward() const {
    if (pImpl->episodes_.empty()) return 0.0f;
    
    float totalReward = 0.0f;
    for (const auto& episode : pImpl->episodes_) {
        totalReward += episode.reward;
    }
    
    return totalReward / pImpl->episodes_.size();
}

float DefaultEpisodicMemory::computeRelevance(const EpisodicMemoryItem& episode) const {
    // Simple relevance calculation: weighted combination of reward and novelty
    return (episode.reward * 0.7f) + (episode.novelty * 0.3f);
}

float DefaultEpisodicMemory::computeSimilarity(const std::vector<float>& query,
                                              const EpisodicMemoryItem& episode) const {
    if (query.empty() || episode.sensoryState.empty()) return 0.0f;
    
    // Simple cosine similarity
    float dotProduct = 0.0f;
    float queryNorm = 0.0f;
    float episodeNorm = 0.0f;
    
    for (size_t i = 0; i < std::min(query.size(), episode.sensoryState.size()); ++i) {
        dotProduct += query[i] * episode.sensoryState[i];
        queryNorm += query[i] * query[i];
        episodeNorm += episode.sensoryState[i] * episode.sensoryState[i];
    }
    
    float denominator = std::sqrt(queryNorm) * std::sqrt(episodeNorm);
    if (denominator < 1e-10f) return 0.0f;
    
    return dotProduct / denominator;
}

} // namespace nlm