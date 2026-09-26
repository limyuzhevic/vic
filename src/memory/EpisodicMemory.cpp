#include "EpisodicMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <ctime>

namespace nlm {

struct EpisodicMemory::Impl {
    std::vector<EpisodicMemoryItem> episodes;
    size_t maxEpisodes;
    
    Impl(size_t max) : maxEpisodes(max) {}
};

EpisodicMemory::EpisodicMemory() : pImpl(new Impl(1000)) {}

EpisodicMemory::~EpisodicMemory() = default;

void EpisodicMemory::storeEpisode(const EpisodicMemoryItem& episode) {
    // TODO PHASE 2: Implement episode storage with time stamps and metadata
    
    // Check capacity
    if (pImpl->episodes.size() >= pImpl->maxEpisodes) {
        // Remove oldest episodes
        pImpl->episodes.erase(pImpl->episodes.begin(), 
                            pImpl->episodes.begin() + pImpl->episodes.size() - pImpl->maxEpisodes + 1);
    }
    
    pImpl->episodes.push_back(episode);
}

EpisodicMemoryItem EpisodicMemory::retrieveEpisode(size_t index) const {
    if (index < pImpl->episodes.size()) {
        return pImpl->episodes[index];
    }
    
    EpisodicMemoryItem empty;
    return empty;
}

size_t EpisodicMemory::getEpisodeCount() const {
    return pImpl->episodes.size();
}

std::vector<EpisodicMemoryItem> EpisodicMemory::getRecentEpisodes(size_t count) const {
    std::vector<EpisodicMemoryItem> recent;
    
    if (count > pImpl->episodes.size()) {
        count = pImpl->episodes.size();
    }
    
    size_t start = pImpl->episodes.size() - count;
    recent.insert(recent.begin(), pImpl->episodes.begin() + start, pImpl->episodes.end());
    
    return recent;
}

void EpisodicMemory::clear() {
    pImpl->episodes.clear();
}

void EpisodicMemory::consolidate(float relevanceThreshold) {
    // TODO PHASE 2: Implement consolidation to long-term memory
    // Move high-relevance episodes to long-term storage
}

} // namespace nlm