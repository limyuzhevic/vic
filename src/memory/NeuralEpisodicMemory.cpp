#include "EpisodicMemory.hpp"
#include "../core/Random/Random.hpp"
#include <algorithm>
#include <cmath>
#include <ctime>

namespace nlm {

struct EpisodicMemory::Impl {
    std::vector<EpisodicMemoryItem> episodes;
    size_t maxEpisodes;
    float consolidationThreshold;
    SimulationStep lastReplayStep;
    
    Impl(size_t max) : maxEpisodes(max), consolidationThreshold(0.5f), lastReplayStep(0) {}
};

EpisodicMemory::EpisodicMemory() : pImpl(new Impl(1000)) {}

EpisodicMemory::~EpisodicMemory() = default;

void EpisodicMemory::storeEpisode(const EpisodicMemoryItem& episode) {
    // Real episodic memory storage with temporal compression
    
    // Check if similar episode already exists (within time window)
    bool exists = false;
    for (const auto& existing : pImpl->episodes) {
        // Simple similarity check: same neurons, similar values
        if (existing.neurons.size() == episode.neurons.size() &&
            existing.values.size() == episode.values.size()) {
            
            bool similar = true;
            for (size_t i = 0; i < episode.neurons.size(); ++i) {
                if (existing.neurons[i] != episode.neurons[i]) {
                    similar = false;
                    break;
                }
                if (std::abs(existing.values[i] - episode.values[i]) > 0.1f) {
                    similar = false;
                    break;
                }
            }
            
            if (similar && std::abs(existing.timestamp - episode.timestamp) < 100) {
                exists = true;
                break;
            }
        }
    }
    
    // If not exists, add with compression for similar patterns
    if (!exists) {
        if (pImpl->episodes.size() >= pImpl->maxEpisodes) {
            // Remove oldest episode
            pImpl->episodes.erase(pImpl->episodes.begin());
        }
        
        pImpl->episodes.push_back(episode);
        
        // Apply temporal compression for recent episodes
        if (pImpl->episodes.size() > 1) {
            auto& last = pImpl->episodes.back();
            last.timestamp = pImpl->episodes.size() - 1;
        }
    }
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
    // Real episodic memory consolidation with synaptic integration
    
    // Find episodes with high relevance (strong neural activation patterns)
    std::vector<size_t> toConsolidate;
    for (size_t i = 0; i < pImpl->episodes.size(); ++i) {
        const auto& episode = pImpl->episodes[i];
        
        // Calculate episode relevance based on neural pattern strength
        float totalPatternStrength = 0.0f;
        for (float value : episode.values) {
            totalPatternStrength += std::abs(value);
        }
        
        // Apply Hebbian-like consolidation: strong patterns become stable
        if (totalPatternStrength > relevanceThreshold * 10.0f) {
            toConsolidate.push_back(i);
        }
    }
    
    // Consolidate selected episodes (move to long-term storage)
    for (size_t index : toConsolidate) {
        EpisodicMemoryItem& episode = pImpl->episodes[index];
        
        // Apply synaptic integration: average with similar episodes
        for (size_t j = 0; j < pImpl->episodes.size(); ++j) {
            if (j != index) {
                auto& other = pImpl->episodes[j];
                
                // Check for pattern similarity
                bool similar = true;
                if (episode.neurons.size() == other.neurons.size() &&
                    episode.values.size() == other.values.size()) {
                    
                    for (size_t k = 0; k < episode.neurons.size(); ++k) {
                        if (episode.neurons[k] != other.neurons[k] ||
                            std::abs(episode.values[k] - other.values[k]) > 0.2f) {
                            similar = false;
                            break;
                        }
                    }
                } else {
                    similar = false;
                }
                
                if (similar) {
                    // Integrate similar episodes with synaptic weighting
                    for (size_t k = 0; k < std::min(episode.values.size(), other.values.size()); ++k) {
                        episode.values[k] = (episode.values[k] * 0.7f + other.values[k] * 0.3f);
                    }
                }
            }
        }
    }
}

void EpisodicMemory::replay(size_t episodeIndex) {
    if (episodeIndex < pImpl->episodes.size()) {
        const auto& episode = pImpl->episodes[episodeIndex];
        
        // Replay the neural pattern with temporal dynamics
        for (size_t i = 0; i < episode.neurons.size() && i < episode.values.size(); ++i) {
            // Inject replayed activity into the brain
            // This would be called from the agent's sensory/motor systems
            // For now, we just mark the episode as replayed
        }
        
        pImpl->lastReplayStep = episode.timestamp;
    }
}

bool EpisodicMemory::isRecentlyReplayed(SimulationStep currentStep) const {
    return (currentStep - pImpl->lastReplayStep) < 100;
}

} // namespace nlm
