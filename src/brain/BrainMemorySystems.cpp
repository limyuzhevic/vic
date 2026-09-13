#include "BrainMemorySystems.hpp"
#include "BrainCore.hpp"
#include "../core/Types/Types.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

void BrainMemorySystems::storeToNeuron(NeuronId neuronId, float activation) {
    if (!brainRef) return;
    
    MemoryTrace trace;
    trace.neuronId = neuronId;
    trace.activation = activation;
    trace.timestamp = brainRef->pImpl->currentTime;
    trace.decayRate = 0.95f; // Decay factor
    
    workingMemory.push_back(trace);
    
    // Trim if exceeds capacity
    if (workingMemory.size() > workingMemoryCapacity) {
        workingMemory.erase(workingMemory.begin());
    }
}

void BrainMemorySystems::update(float timestep) {
    // Decay activations over time
    for (auto& trace : workingMemory) {
        trace.activation *= trace.decayRate;
        trace.timestamp += timestep;
    }
    
    // Remove traces with very low activation
    workingMemory.erase(
        std::remove_if(workingMemory.begin(), workingMemory.end(),
                      [](const MemoryTrace& trace) { return trace.activation < 0.001f; }),
        workingMemory.end()
    );
}

void BrainMemorySystems::setCapacity(size_t capacity) {
    workingMemoryCapacity = capacity;
    
    // Trim if currently exceeds capacity
    if (workingMemory.size() > workingMemoryCapacity) {
        size_t excess = workingMemory.size() - workingMemoryCapacity;
        workingMemory.erase(workingMemory.begin(), workingMemory.begin() + excess);
    }
}

size_t BrainMemorySystems::getActiveTraces() const {
    return workingMemory.size();
}

void BrainMemorySystems::clear() {
    workingMemory.clear();
    episodicMemory.clear();
}

void BrainMemorySystems::storeEpisode(const EpisodicMemoryItem& episode) {
    episodicMemory.push_back(episode);
    
    // Trim if exceeds max episodes
    if (episodicMemory.size() > maxEpisodes) {
        // Remove oldest episodes (FIFO)
        size_t excess = episodicMemory.size() - maxEpisodes;
        episodicMemory.erase(episodicMemory.begin(), episodicMemory.begin() + excess);
    }
}

void BrainMemorySystems::getEpisodesForReplay(size_t count, std::vector<const EpisodicMemoryItem*>& episodes) const {
    // Sort episodes by importance (activation-based)
    std::vector<std::pair<float, const EpisodicMemoryItem*>> sortedEpisodes;
    sortedEpisodes.reserve(episodicMemory.size());
    
    for (const auto& episode : episodicMemory) {
        float importance = 0.0f;
        for (float activation : episode.neuronActivations) {
            importance += activation;
        }
        importance /= std::max(1.0f, static_cast<float>(episode.neuronActivations.size()));
        sortedEpisodes.push_back({importance, &episode});
    }
    
    // Sort by importance (highest first)
    std::sort(sortedEpisodes.begin(), sortedEpisodes.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // Add top episodes to output list
    episodes.clear();
    size_t toAdd = std::min(count, sortedEpisodes.size());
    for (size_t i = 0; i < toAdd; ++i) {
        episodes.push_back(sortedEpisodes[i].second);
    }
}

void BrainMemorySystems::replayEpisode(const EpisodicMemoryItem* episode) {
    if (!episode) return;
    
    // Replay episode activations
    for (size_t i = 0; i < std::min(episode->activeNeurons.size(), episodicMemory.back().activeNeurons.size()); ++i) {
        NeuronId neuronId = episode->activeNeurons[i];
        float activation = episode->neuronActivations[i];
        storeToNeuron(neuronId, activation * 0.5f); // Replay with reduced strength
    }
}

void BrainMemorySystems::consolidate(float threshold) {
    // Remove episodes with low overall activation
    auto newEnd = std::remove_if(episodicMemory.begin(), episodicMemory.end(),
                                 [threshold](const EpisodicMemoryItem& episode) {
                                     float totalActivation = 0.0f;
                                     for (float activation : episode.neuronActivations) {
                                         totalActivation += activation;
                                     }
                                     return totalActivation < threshold;
                                 });
    episodicMemory.erase(newEnd, episodicMemory.end());
}

size_t BrainMemorySystems::getEpisodeCount() const {
    return episodicMemory.size();
}

void BrainMemorySystems::setMaxEpisodes(size_t max) {
    maxEpisodes = max;
    
    // Trim if currently exceeds max
    if (episodicMemory.size() > maxEpisodes) {
        size_t excess = episodicMemory.size() - maxEpisodes;
        episodicMemory.erase(episodicMemory.begin(), episodicMemory.begin() + excess);
    }
}

void BrainMemorySystems::storeAssociation(const std::string& key, const std::vector<float>& pattern) {
    associativeMemory[key] = pattern;
}

bool BrainMemorySystems::retrieveAssociation(const std::string& key, std::vector<float>& pattern) const {
    auto it = associativeMemory.find(key);
    if (it == associativeMemory.end()) {
        return false;
    }
    pattern = it->second;
    return true;
}

float BrainMemorySystems::calculateAssociationStrength(const std::vector<float>& pattern1, const std::vector<float>& pattern2) const {
    if (pattern1.empty() || pattern2.empty()) return 0.0f;
    
    float dotProduct = 0.0f;
    for (size_t i = 0; i < std::min(pattern1.size(), pattern2.size()); ++i) {
        dotProduct += pattern1[i] * pattern2[i];
    }
    
    float magnitude1 = 0.0f;
    float magnitude2 = 0.0f;
    for (float val : pattern1) magnitude1 += val * val;
    for (float val : pattern2) magnitude2 += val * val;
    
    magnitude1 = std::sqrt(magnitude1);
    magnitude2 = std::sqrt(magnitude2);
    
    if (magnitude1 == 0.0f || magnitude2 == 0.0f) return 0.0f;
    
    return dotProduct / (magnitude1 * magnitude2);
}

size_t BrainMemorySystems::getWorkingMemoryCapacity() const {
    return workingMemoryCapacity;
}

float BrainMemorySystems::getMemoryUtilization() const {
    if (workingMemoryCapacity == 0) return 0.0f;
    return static_cast<float>(workingMemory.size()) / static_cast<float>(workingMemoryCapacity);
}

void BrainMemorySystems::initialize(class Brain* brain) {
    brainRef = brain;
    workingMemoryCapacity = 100; // Default capacity
    maxEpisodes = 1000; // Default max episodes
}

} // namespace nlm