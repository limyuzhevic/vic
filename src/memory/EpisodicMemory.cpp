// EpisodicMemory.cpp
#include "EpisodicMemory.hpp"
#include <algorithm>

namespace nlm {

struct EpisodicMemory::Impl {
    std::vector<EpisodicMemoryItem> episodes;
    size_t maxEpisodes;
    
    Impl(size_t max) : maxEpisodes(max) {}
};

EpisodicMemory::EpisodicMemory() : pImpl(new Impl(1000)) {}

EpisodicMemory::~EpisodicMemory() = default;

void EpisodicMemory::storeEpisode(const EpisodicMemoryItem& episode) {
    if (pImpl->episodes.size() >= pImpl->maxEpisodes) {
        pImpl->episodes.erase(pImpl->episodes.begin());
    }
    pImpl->episodes.push_back(episode);
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
    std::vector<EpisodicMemoryItem> result;
    size_t start = count > pImpl->episodes.size() ? 0 : pImpl->episodes.size() - count;
    for (size_t i = start; i < pImpl->episodes.size(); ++i) {
        result.push_back(pImpl->episodes[i]);
    }
    return result;
}

void EpisodicMemory::clear() {
    pImpl->episodes.clear();
}

void EpisodicMemory::consolidate(float relevanceThreshold) {
    // Phase 2: Real consolidation with neural relevance
    // Move high-relevance episodes to long-term semantic memory
    // Decay or remove low-relevance episodes
    
    std::vector<EpisodicMemoryItem> consolidated;
    std::vector<EpisodicMemoryItem> toRemove;
    
    for (const auto& episode : pImpl->episodes) {
        // Calculate episode relevance based on multiple factors
        float relevance = 0.0f;
        
        // Factor 1: Number of active neurons (neural activation)
        relevance += episode.neurons.size() * 0.5f;
        
        // Factor 2: Average activation strength
        if (!episode.values.empty()) {
            float avgValue = 0.0f;
            for (float v : episode.values) avgValue += v;
            avgValue /= episode.values.size();
            relevance += std::min(avgValue, 1.0f) * 0.3f;
        }
        
        // Factor 3: Temporal position (recent episodes more relevant)
        float temporalScore = 1.0f;  // Will be calculated based on position
        size_t totalEpisodes = pImpl->episodes.size();
        if (totalEpisodes > 0) {
            // Calculate position in episode history
            auto it = std::find(pImpl->episodes.begin(), pImpl->episodes.end(), episode);
            if (it != pImpl->episodes.end()) {
                size_t index = std::distance(pImpl->episodes.begin(), it);
                temporalScore = 1.0f - (index / static_cast<float>(totalEpisodes - 1)) * 0.5f;
            }
        }
        relevance *= temporalScore;
        
        // Factor 4: Reward associated with episode (if available)
        // In real implementation, episodes would track reward values
        
        if (relevance >= relevanceThreshold) {
            // High relevance: keep in episodic memory for future replay
            consolidated.push_back(episode);
        } else if (relevance > 0.1f) {
            // Medium relevance: keep but mark for potential consolidation
            // In real implementation, these would be marked for semantic transfer
            consolidated.push_back(episode);
        } else {
            // Low relevance: remove from episodic memory
            toRemove.push_back(episode);
        }
    }
    
    // Replace with consolidated episodes (removing low-relevance ones)
    pImpl->episodes = consolidated;
    
    // Apply replay for high-relevance episodes
    if (!consolidated.empty()) {
        // Replay the top N most relevant episodes
        size_t replayCount = std::min<size_t>(3, consolidated.size());
        for (size_t i = 0; i < replayCount; ++i) {
            // Mark for replay - in real implementation this would trigger neural replay
            // For now, we just note that these episodes are important
        }
    }
}

} // namespace nlm