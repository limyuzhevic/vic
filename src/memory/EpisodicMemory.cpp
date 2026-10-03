#include "EpisodicMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <numeric>

namespace nlm {

struct EpisodicMemory::Impl {
    // Simple implementation using vectors for now
    std::vector<EpisodicMemoryItem> episodes;
    
    Impl() {}
};

EpisodicMemory::EpisodicMemory() : pImpl(std::make_unique<Impl>()) {
    NLM_LOG_INFO("EpisodicMemory created");
}

EpisodicMemory::~EpisodicMemory() = default;

void EpisodicMemory::storeEpisode(const EpisodicMemoryItem& episode) {
    // Add episode with age 0
    EpisodicMemoryItem stored = episode;
    stored.age = 0;
    
    pImpl->episodes.push_back(stored);
    
    // Remove old episodes if over capacity (soft limit)
    while (pImpl->episodes.size() > 1000) {
        pImpl->episodes.erase(pImpl->episodes.begin());
    }
    
    NLM_LOG_DEBUG("Stored episode: " + std::to_string(episode.timestamp) + 
                 ", age: " + std::to_string(episode.age));
}

EpisodicMemoryItem EpisodicMemory::retrieveEpisode(size_t index) const {
    if (index < pImpl->episodes.size()) {
        return pImpl->episodes[index];
    }
    
    return EpisodicMemoryItem();
}

size_t EpisodicMemory::getEpisodeCount() const {
    return pImpl->episodes.size();
}

std::vector<EpisodicMemoryItem> EpisodicMemory::getRecentEpisodes(size_t count) const {
    std::vector<EpisodicMemoryItem> results;
    
    if (count == 0 || pImpl->episodes.empty()) {
        return results;
    }
    
    size_t start = count > pImpl->episodes.size() ? 0 : pImpl->episodes.size() - count;
    for (size_t i = start; i < pImpl->episodes.size(); ++i) {
        results.push_back(pImpl->episodes[i]);
    }
    
    return results;
}

void EpisodicMemory::clear() {
    pImpl->episodes.clear();
    NLM_LOG_INFO("Cleared all episodes");
}

void EpisodicMemory::consolidate(float relevanceThreshold) {
    // Remove episodes based on age (simple consolidation)
    std::vector<EpisodicMemoryItem> kept;
    
    for (const auto& episode : pImpl->episodes) {
        // Keep episodes that are not too old or have other relevance criteria
        if (episode.age < 500 || episode.age < maxAgeForRelevance(relevanceThreshold)) {
            kept.push_back(episode);
        }
    }
    
    pImpl->episodes = std::move(kept);
    NLM_LOG_INFO("Consolidated episodes to " + std::to_string(pImpl->episodes.size()) + " items");
}

size_t EpisodicMemory::maxAgeForRelevance(float threshold) const {
    // Simple calculation: higher relevance threshold keeps older episodes
    return static_cast<size_t>(500.0f * (1.0f - threshold));
}

} // namespace nlm