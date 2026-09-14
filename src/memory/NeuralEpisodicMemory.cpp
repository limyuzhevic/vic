#include "NeuralEpisodicMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <unordered_map>
#include <unordered_set>

namespace nlm {

struct NeuralEpisodicMemory::Impl {
    Brain* brain;
    
    // Efficient storage: timestamp -> episode pointer mapping
    std::unordered_map<size_t, const EpisodicMemoryItem*> episodeMap;
    // Episode index for O(1) access by age/timestamp
    std::unordered_map<size_t, const EpisodicMemoryItem*> ageIndex;
    
    // Legacy storage for backward compatibility
    std::deque<EpisodicMemoryItem> legacyEpisodes;
    
    // Pattern neurons for episodic recall (legacy)
    std::vector<NeuronId> episodeNeurons;
    
    Impl() : brain(nullptr) {}
};

NeuralEpisodicMemory::NeuralEpisodicMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
    , maxEpisodes_(1000)
    , replayEnabled_(true)
{
}

NeuralEpisodicMemory::~NeuralEpisodicMemory() = default;

void NeuralEpisodicMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("NeuralEpisodicMemory initialized");
}

void NeuralEpisodicMemory::storeEpisode(const EpisodicMemoryItem& episode) {
    // Create episode copy with age 0
    EpisodicMemoryItem stored = episode;
    stored.age = 0;
    
    // Store in efficient map using timestamp as key
    pImpl->episodeMap[stored.timestamp] = &episodes_.back();
    pImpl->ageIndex[stored.age] = &episodes_.back();
    
    // Add to deque (maintains order for iteration)
    episodes_.push_back(stored);
    
    // Create episode neuron for pattern completion
    if (brain_ && !episode.sensoryState.empty()) {
        NeuronId epNeuron(episodes_.size() + 20000);
        pImpl->episodeNeurons.push_back(epNeuron);
        
        // Store episode index in the neuron
        // (This is a simplified approach - real implementation would use more distributed encoding)
    }
    
    // Remove old episodes if over capacity
    while (episodes_.size() > maxEpisodes_) {
        // Remove from efficient maps first
        auto it = episodes_.begin();
        size_t oldTimestamp = it->timestamp;
        size_t oldAge = it->age;
        
        pImpl->episodeMap.erase(oldTimestamp);
        pImpl->ageIndex.erase(oldAge);
        
        // Then remove from deque
        episodes_.erase(it);
        
        // Also remove from legacy
        pImpl->legacyEpisodes.erase(pImpl->legacyEpisodes.begin());
        
        if (!pImpl->episodeNeurons.empty()) {
            pImpl->episodeNeurons.erase(pImpl->episodeNeurons.begin());
        }
    }
    
    // Age all episodes
    for (auto& ep : episodes_) {
        ep.age++;
    }
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::retrieveSimilar(
    const std::vector<float>& sensoryPattern,
    size_t maxResults) const {
    
    std::vector<std::pair<float, size_t>> scores;  // (similarity, timestamp)
    
    // Try efficient retrieval from map first
    if (!pImpl->episodeMap.empty()) {
        for (const auto& pair : pImpl->episodeMap) {
            size_t timestamp = pair.first;
            const EpisodicMemoryItem* episode = pair.second;
            
            float sim = computeSimilarity(sensoryPattern, *episode);
            scores.emplace_back(sim, timestamp);
        }
    } else {
        // Fallback to linear search through deque
        for (size_t i = 0; i < episodes_.size(); ++i) {
            float sim = computeSimilarity(sensoryPattern, episodes_[i]);
            scores.emplace_back(sim, i);
        }
    }
    
    // Sort by similarity (descending)
    std::sort(scores.begin(), scores.end(),
        [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // Return top results
    std::vector<const EpisodicMemoryItem*> results;
    for (size_t i = 0; i < std::min(maxResults, scores.size()); ++i) {
        if (scores[i].first > 0.3f) {  // Threshold
            // Try to get from efficient map first
            auto it = pImpl->episodeMap.find(scores[i].second);
            if (it != pImpl->episodeMap.end()) {
                results.push_back(it->second);
            } else {
                // Fallback to deque access
                size_t idx = scores[i].second;
                if (idx < episodes_.size()) {
                    results.push_back(&episodes_[idx]);
                }
            }
        }
    }
    
    return results;
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::retrieveTemporal(
    SimulationStep startTime,
    SimulationStep endTime,
    size_t maxResults) const {
    
    std::vector<const EpisodicMemoryItem*> results;
    
    // Use efficient access if available
    if (!pImpl->episodeMap.empty()) {
        for (const auto& pair : pImpl->episodeMap) {
            const EpisodicMemoryItem* episode = pair.second;
            if (episode->timestamp >= startTime && episode->timestamp <= endTime) {
                results.push_back(episode);
            }
        }
    } else {
        // Fallback to linear search through deque
        for (const auto& ep : episodes_) {
            if (ep.timestamp >= startTime && ep.timestamp <= endTime) {
                results.push_back(&ep);
            }
        }
    }
    
    // Return most recent first
    std::sort(results.begin(), results.end(),
        [](const EpisodicMemoryItem* a, const EpisodicMemoryItem* b) {
            return a->timestamp > b->timestamp;
        });
    
    if (results.size() > maxResults) {
        results.resize(maxResults);
    }
    
    return results;
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::retrieveByLocation(
    float x, float y, float radius, size_t maxResults) const {
    
    std::vector<std::pair<float, const EpisodicMemoryItem*>> distances;  // (distance, episode)
    
    // Use efficient access if available
    if (!pImpl->episodeMap.empty()) {
        for (const auto& pair : pImpl->episodeMap) {
            const EpisodicMemoryItem* episode = pair.second;
            float dx = episode->positionX - x;
            float dy = episode->positionY - y;
            float dist = std::sqrt(dx * dx + dy * dy);
            
            if (dist <= radius) {
                distances.emplace_back(dist, episode);
            }
        }
    } else {
        // Fallback to linear search
        for (const auto& ep : episodes_) {
            float dx = ep.positionX - x;
            float dy = ep.positionY - y;
            float dist = std::sqrt(dx * dx + dy * dy);
            
            if (dist <= radius) {
                distances.emplace_back(dist, &ep);
            }
        }
    }
    
    // Sort by distance
    std::sort(distances.begin(), distances.end(),
        [](const auto& a, const auto& b) { return a.first < b.first; });
    
    std::vector<const EpisodicMemoryItem*> results;
    for (size_t i = 0; i < std::min(maxResults, distances.size()); ++i) {
        results.push_back(distances[i].second);
    }
    
    return results;
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::retrieveAfterAction(
    ActionType action, size_t maxResults) const {
    
    std::vector<const EpisodicMemoryItem*> results;
    
    // Use efficient access if available
    if (!pImpl->episodeMap.empty()) {
        for (const auto& pair : pImpl->episodeMap) {
            const EpisodicMemoryItem* episode = pair.second;
            if (episode->action == action) {
                results.push_back(episode);
            }
        }
    } else {
        // Fallback to linear search
        for (const auto& ep : episodes_) {
            if (ep.action == action) {
                results.push_back(&ep);
            }
        }
    }
    
    // Most recent first
    std::sort(results.begin(), results.end(),
        [](const EpisodicMemoryItem* a, const EpisodicMemoryItem* b) {
            return a->timestamp > b->timestamp;
        });
    
    if (results.size() > maxResults) {
        results.resize(maxResults);
    }
    
    return results;
}

void NeuralEpisodicMemory::replayEpisode(const EpisodicMemoryItem* episode) {
    if (!episode || !brain_) return;
    
    // Reactivate neurons that were active during this episode
    for (size_t i = 0; i < episode->activeNeurons.size(); ++i) {
        NeuronId neuron = episode->activeNeurons[i];
        float activation = i < episode->neuronActivations.size() ? 
                          episode->neuronActivations[i] : 0.5f;
        
        // Inject current to reawaken this pattern
        brain_->injectCurrent(neuron, activation * 3.0f);
    }
    
    // Also inject sensory pattern if available
    if (!episode->sensoryState.empty()) {
        // This would go through sensory channels - simplified here
    }
}

void NeuralEpisodicMemory::updateRelevance(SimulationStep episodeId, float relevanceDelta) {
    // Try efficient access first
    auto it = pImpl->episodeMap.find(episodeId);
    if (it != pImpl->episodeMap.end()) {
        // In a full implementation, episodes would have a relevance field
        // For now, this would age the episode differently
    } else {
        // Fallback to legacy linear search
        if (episodeId < episodes_.size()) {
            // In a full implementation...
        }
    }
}

const EpisodicMemoryItem* NeuralEpisodicMemory::getEpisode(size_t index) const {
    // Try efficient access first
    auto it = pImpl->ageIndex.find(index);
    if (it != pImpl->ageIndex.end()) {
        return it->second;
    }
    
    // Fallback to linear search
    if (index < episodes_.size()) {
        return &episodes_[index];
    }
    
    return nullptr;
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::getRecentEpisodes(size_t count) const {
    std::vector<const EpisodicMemoryItem*> results;
    
    if (!pImpl->episodeMap.empty()) {
        // Get recent episodes from efficient map
        // Need to get last N entries (not trivial with unordered_map)
        // For now, fallback to deque
    }
    
    size_t start = count > episodes_.size() ? 0 : episodes_.size() - count;
    for (size_t i = start; i < episodes_.size(); ++i) {
        results.push_back(&episodes_[i]);
    }
    
    return results;
}

float NeuralEpisodicMemory::getAverageReward() const {
    if (episodes_.empty()) return 0.0f;
    
    float sum = 0.0f;
    for (const auto& ep : episodes_) {
        sum += ep.reward;
    }
    return sum / episodes_.size();
}

void NeuralEpisodicMemory::consolidate(float relevanceThreshold) {
    // Remove episodes below relevance threshold
    // In a full implementation, episodes would have relevance scores
    // For now, just enforce max capacity
    while (episodes_.size() > maxEpisodes_ * 0.8f) {
        // Remove from efficient maps
        auto it = episodes_.begin();
        size_t oldTimestamp = it->timestamp;
        size_t oldAge = it->age;
        
        pImpl->episodeMap.erase(oldTimestamp);
        pImpl->ageIndex.erase(oldAge);
        
        episodes_.erase(it);
    }
}

void NeuralEpisodicMemory::clear() {
    episodes_.clear();
    pImpl->episodeNeurons.clear();
    pImpl->episodeMap.clear();
    pImpl->ageIndex.clear();
    pImpl->legacyEpisodes.clear();
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::getEpisodesForReplay(
    size_t count) const {
    
    // Select episodes for replay based on:
    // 1. Recency (more recent episodes)
    // 2. Reward (higher reward episodes)
    // 3. Unpredictability (higher prediction error episodes)
    
    std::vector<std::tuple<float, float, const EpisodicMemoryItem*>> scored;  // (score, reward, episode)
    
    for (const auto& ep : episodes_) {
        float recencyScore = 1.0f / (ep.age + 1);
        float rewardScore = ep.reward;
        float score = recencyScore * 0.5f + rewardScore * 0.5f;
        scored.emplace_back(score, rewardScore, &ep);
    }
    
    // Sort by score
    std::sort(scored.begin(), scored.end(),
        [](const auto& a, const auto& b) {
            return std::get<0>(a) > std::get<0>(b);
        });
    
    std::vector<const EpisodicMemoryItem*> results;
    for (size_t i = 0; i < std::min(count, scored.size()); ++i) {
        results.push_back(std::get<2>(scored[i]));
    }
    
    return results;
}

void NeuralEpisodicMemory::replaySequence(const std::vector<size_t>& episodeIds) {
    for (size_t id : episodeIds) {
        // Try efficient access first
        auto it = pImpl->ageIndex.find(id);
        if (it != pImpl->ageIndex.end()) {
            replayEpisode(it->second);
        } else {
            // Fallback to linear search
            if (id < episodes_.size()) {
                replayEpisode(&episodes_[id]);
            }
        }
        
        // Small delay between replays
        // (In simulation, this would be steps)
    }
}

float NeuralEpisodicMemory::computeSimilarity(const std::vector<float>& query,
                                             const EpisodicMemoryItem& episode) const {
    if (query.empty() || episode.sensoryState.empty()) return 0.0f;
    if (query.size() != episode.sensoryState.size()) return 0.0f;
    
    // Cosine similarity
    float dot = 0.0f, normQ = 0.0f, normE = 0.0f;
    for (size_t i = 0; i < query.size(); ++i) {
        dot += query[i] * episode.sensoryState[i];
        normQ += query[i] * query[i];
        normE += episode.sensoryState[i] * episode.sensoryState[i];
    }
    
    if (normQ < 0.0001f || normE < 0.0001f) return 0.0f;
    
    return dot / (std::sqrt(normQ) * std::sqrt(normE));
}

void NeuralEpisodicMemory::migrateToEfficientStorage() {
    if (!pImpl->legacyEpisodes.empty() && pImpl->episodeMap.empty()) {
        for (const auto& episode : pImpl->legacyEpisodes) {
            pImpl->episodeMap[episode.timestamp] = &episodes_[episodes_.size() - 1];
            pImpl->ageIndex[episode.age] = &episodes_[episodes_.size() - 1];
            episodes_.push_back(episode);
        }
        
        // Clear legacy storage after migration
        pImpl->legacyEpisodes.clear();
    }
}

void NeuralEpisodicMemory::migrateToLegacyStorage() {
    if (!pImpl->episodeMap.empty() && pImpl->legacyEpisodes.empty()) {
        // Convert efficient map back to deque
        for (const auto& pair : pImpl->episodeMap) {
            const EpisodicMemoryItem* episode = pair.second;
            pImpl->legacyEpisodes.push_back(*episode);
        }
        
        // Clear efficient storage after migration
        pImpl->episodeMap.clear();
        pImpl->ageIndex.clear();
        episodes_.clear();
        pImpl->episodeNeurons.clear();
    }
}