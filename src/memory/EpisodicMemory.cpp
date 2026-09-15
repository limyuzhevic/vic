// Episodic Memory - Simple implementation
// This delegates to NeuralEpisodicMemory for neural encoding

#include "Memory.hpp"
#include "NeuralEpisodicMemory.hpp"
#include <algorithm>

namespace nlm {

struct EpisodicMemory::Impl {
    NeuralEpisodicMemory neuralEpisodicMemory;
    
    Impl() {}
};

EpisodicMemory::EpisodicMemory() : pImpl(new Impl) {}

EpisodicMemory::~EpisodicMemory() = default;

void EpisodicMemory::storeEpisode(const EpisodicMemoryItem& episode) {
    pImpl->neuralEpisodicMemory.storeEpisode(episode);
}

EpisodicMemoryItem EpisodicMemory::retrieveEpisode(size_t index) const {
    auto episode = pImpl->neuralEpisodicMemory.getEpisode(index);
    if (!episode) {
        return EpisodicMemoryItem();
    }
    return *episode;
}

size_t EpisodicMemory::getEpisodeCount() const {
    return pImpl->neuralEpisodicMemory.getEpisodeCount();
}

std::vector<EpisodicMemoryItem> EpisodicMemory::getRecentEpisodes(size_t count) const {
    std::vector<EpisodicMemoryItem> result;
    auto recent = pImpl->neuralEpisodicMemory.getRecentEpisodes(count);
    
    for (auto ep : recent) {
        if (ep) {
            result.push_back(*ep);
        }
    }
    
    return result;
}

void EpisodicMemory::clear() {
    pImpl->neuralEpisodicMemory.clear();
}

void EpisodicMemory::consolidate(float relevanceThreshold) {
    pImpl->neuralEpisodicMemory.consolidate(relevanceThreshold);
}

} // namespace nlm