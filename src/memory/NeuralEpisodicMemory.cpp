#include "NeuralEpisodicMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct NeuralEpisodicMemory::Impl {
    Brain* brain;
    
    // Pattern neurons for episodic recall
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
        episodes_.erase(episodes_.begin());
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
    
    std::vector<std::pair<float, size_t>> scores;  // (similarity, index)
    
    for (size_t i = 0; i < episodes_.size(); ++i) {
        float sim = computeSimilarity(sensoryPattern, episodes_[i]);
        scores.emplace_back(sim, i);
    }
    
    // Sort by similarity (descending)
    std::sort(scores.begin(), scores.end(),
        [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // Return top results
    std::vector<const EpisodicMemoryItem*> results;
    for (size_t i = 0; i < std::min(maxResults, scores.size()); ++i) {
        if (scores[i].first > 0.3f) {  // Threshold
            results.push_back(&episodes_[scores[i].second]);
        }
    }
    
    return results;
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::retrieveTemporal(
    SimulationStep startTime,
    SimulationStep endTime,
    size_t maxResults) const {
    
    std::vector<const EpisodicMemoryItem*> results;
    
    for (const auto& ep : episodes_) {
        if (ep.timestamp >= startTime && ep.timestamp <= endTime) {
            results.push_back(&ep);
        }
    }
    
    return results;
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::retrieveByLocation(float x, float y,
                                                              float radius,
                                                              size_t maxResults) const {
    
    std::vector<const EpisodicMemoryItem*> results;
    
    for (const auto& ep : episodes_) {
        float dist = std::sqrt(std::pow(ep.positionX - x, 2) + std::pow(ep.positionY - y, 2));
        if (dist <= radius) {
            results.push_back(&ep);
        }
    }
    
    return results;
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::retrieveAfterAction(ActionType action,
                                                              size_t maxResults) const {
    
    std::vector<const EpisodicMemoryItem*> results;
    
    for (const auto& ep : episodes_) {
        if (ep.action == action) {
            results.push_back(&ep);
        }
    }
    
    return results;
}

void NeuralEpisodicMemory::replayEpisode(const EpisodicMemoryItem* episode) {
    if (!brain_ || !episode) return;
    
    // Reactivate neural patterns associated with this episode
    // In a full implementation, this would use the brain's synaptic weights
    // to reconstruct the neural activity patterns
    
    // Store in replay buffer for later consolidation
    replayBuffer_.push_back(episode);
    
    // Increase weight of synapses involved in this episode
    // This is a placeholder - real implementation would be more sophisticated
    NLM_LOG_INFO("Replaying episode with " + std::to_string(episode->activeNeurons.size()) + " neurons");
}

void NeuralEpisodicMemory::updateRelevance(SimulationStep episodeId, float relevanceDelta) {
    if (episodeId < episodes_.size()) {
        episodes_[episodeId].novelty += relevanceDelta;
    }
}

const EpisodicMemoryItem* NeuralEpisodicMemory::getEpisode(size_t index) const {
    if (index < episodes_.size()) {
        return &episodes_[index];
    }
    return nullptr;
}

float NeuralEpisodicMemory::getAverageReward() const {
    if (episodes_.empty()) return 0.0f;
    
    float totalReward = 0.0f;
    for (const auto& ep : episodes_) {
        totalReward += ep.reward;
    }
    
    return totalReward / static_cast<float>(episodes_.size());
}

void NeuralEpisodicMemory::consolidate(float relevanceThreshold) {
    // Move highly relevant episodes to long-term storage
    size_t startIndex = 0;
    size_t endIndex = episodes_.size();
    
    // Remove low-relevance episodes
    auto newEnd = std::remove_if(episodes_.begin(), episodes_.end(),
        [relevanceThreshold](const EpisodicMemoryItem& ep) {
            return ep.novelty < relevanceThreshold;
        });
    
    episodes_.erase(newEnd, episodes_.end());
}

void NeuralEpisodicMemory::clear() {
    episodes_.clear();
    if (pImpl) {
        pImpl->episodeNeurons.clear();
    }
}

void NeuralEpisodicMemory::enableReplay(bool enable) { replayEnabled_ = enable; }
bool NeuralEpisodicMemory::isReplayEnabled() const { return replayEnabled_; }

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::getEpisodesForReplay(size_t count) const {
    std::vector<const EpisodicMemoryItem*> results;
    
    // Select episodes based on novelty, recency, and reward
    std::vector<std::pair<float, size_t>> scores;  // (score, index)
    
    for (size_t i = 0; i < episodes_.size(); ++i) {
        float score = episodes_[i].novelty + episodes_[i].reward * 2.0f - episodes_[i].age * 0.1f;
        scores.emplace_back(score, i);
    }
    
    std::sort(scores.begin(), scores.end(),
        [](const auto& a, const auto& b) { return a.first > b.first; });
    
    for (size_t i = 0; i < std::min(count, scores.size()); ++i) {
        results.push_back(&episodes_[scores[i].second]);
    }
    
    return results;
}

void NeuralEpisodicMemory::replaySequence(const std::vector<size_t>& episodeIds) {
    if (!brain_) return;
    
    // Replay episodes in sequence for consolidation
    for (size_t id : episodeIds) {
        if (id < episodes_.size()) {
            replayEpisode(&episodes_[id]);
        }
    }
}

float NeuralEpisodicMemory::computeSimilarity(const std::vector<float>& query,
                                          const EpisodicMemoryItem& episode) const {
    if (query.empty()) return 0.0f;
    
    float dot = 0.0f, normQuery = 0.0f, normEpisode = 0.0f;
    
    for (size_t i = 0; i < std::min(query.size(), episode.sensoryState.size()); ++i) {
        dot += query[i] * episode.sensoryState[i];
        normQuery += query[i] * query[i];
        normEpisode += episode.sensoryState[i] * episode.sensoryState[i];
    }
    
    if (normQuery < 0.0001f || normEpisode < 0.0001f) return 0.0f;
    
    return dot / (std::sqrt(normQuery) * std::sqrt(normEpisode));
}

bool NeuralEpisodicMemory::matchesCriteria(const EpisodicMemoryItem& episode,
                                     const std::vector<float>* sensoryQuery,
                                     SimulationStep* startTime,
                                     SimulationStep* endTime,
                                     ActionType* actionQuery) const {
    bool matches = true;
    
    if (sensoryQuery && !sensoryQuery->empty()) {
        if (computeSimilarity(*sensoryQuery, episode) < 0.3f) {
            return false;
        }
    }
    
    if (startTime && endTime) {
        if (episode.timestamp < *startTime || episode.timestamp > *endTime) {
            return false;
        }
    }
    
    if (actionQuery && episode.action != *actionQuery) {
        return false;
    }
    
    return true;
}

} // namespace nlm
