#include "NeuralEpisodicMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <unordered_set>

namespace nlm {

struct NeuralEpisodicMemory::Impl {
    Brain* brain;
    
    // Neural pattern neurons for episodic encoding (actual brain neurons)
    std::vector<NeuronId> episodicNeurons;
    
    // Patterns stored as distributed representations
    std::vector<std::vector<float>> patternMemories;
    
    // Temporal compression for efficient storage
    std::vector<float> temporalWeights;
    
    // Replay queues for consolidation
    std::queue<size_t> replayQueue;
    
    // Synaptic traces for pattern completion
    std::vector<std::vector<NeuronId>> patternConnections;
    
    Impl() : brain(nullptr) {
        patternConnections.reserve(1000);
    }
};

NeuralEpisodicMemory::NeuralEpisodicMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
    , maxEpisodes_(1000)
    , replayEnabled_(true)
{
    // Initialize with better defaults
    NLM_LOG_INFO("NeuralEpisodicMemory created with enhanced neural integration");
}

NeuralEpisodicMemory::~NeuralEpisodicMemory() = default;

void NeuralEpisodicMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    
    // Create dedicated episodic memory regions
    if (brain_) {
        pImpl->episodicNeurons.clear();
        pImpl->patternMemories.clear();
        
        // Create 3 dedicated episodic memory regions
        for (int i = 0; i < 3; ++i) {
            RegionId regionId = brain_->addRegion("EpisodicMemory_" + std::to_string(i));
            pImpl->episodicNeurons.push_back(regionId);
        }
        
        // Initialize pattern storage
        pImpl->patternMemories.reserve(maxEpisodes_);
        pImpl->temporalWeights.reserve(maxEpisodes_);
        
        NLM_LOG_INFO("NeuralEpisodicMemory initialized with " + 
                     std::to_string(pImpl->episodicNeurons.size()) + " dedicated regions");
    }
}

void NeuralEpisodicMemory::storeEpisode(const EpisodicMemoryItem& episode) {
    if (!brain_) return;
    
    // Create enhanced episode with neural encoding
    EpisodicMemoryItem enhancedEpisode = episode;
    enhancedEpisode.age = 0;
    
    // Encode episode into neural patterns using brain's actual neurons
    if (!enhancedEpisode.sensoryState.empty() && brain_) {
        // Get working memory reference for pattern encoding
        auto* workingMem = brain_->getWorkingMemory();
        if (workingMem) {
            // Encode sensory pattern into working memory
            workingMem->store(enhancedEpisode.sensoryState, 0.8f);
            
            // Encode action-outcome relationship
            std::vector<float> actionPattern(enhancedEpisode.sensoryState.size(), 0.0f);
            actionPattern[0] = static_cast<float>(enhancedEpisode.action) / 10.0f;
            workingMem->store(actionPattern, 0.6f);
        }
        
        // Create neural pattern representation
        std::vector<float> neuralPattern;
        
        // Use episodic memory regions to encode pattern
        for (size_t i = 0; i < enhancedEpisode.activeNeurons.size() && 
                           i < enhancedEpisode.neuronActivations.size(); ++i) {
            NeuronId neuron = enhancedEpisode.activeNeurons[i];
            float activation = enhancedEpisode.neuronActivations[i];
            
            // Store pattern in episodic memory regions
            neuralPattern.push_back(activation);
            
            // Update neuron state to encode this episode
            if (brain_) {
                // This would normally go through brain's neural plasticity
                // For now, we'll simulate by injecting current
                brain_->injectCurrent(neuron, activation * 2.0f);
            }
        }
        
        // Store the neural pattern
        pImpl->patternMemories.push_back(neuralPattern);
        pImpl->temporalWeights.push_back(1.0f); // Start with full weight
        
        // Store temporal context (episode index)
        enhancedEpisode.timestamp = episodes_.size();
        
        // Add to episodes deque
        episodes_.push_back(enhancedEpisode);
        
        // Remove old episodes if over capacity
        while (episodes_.size() > maxEpisodes_) {
            episodes_.pop_front();
            if (!pImpl->patternMemories.empty()) {
                pImpl->patternMemories.erase(pImpl->patternMemories.begin());
            }
            if (!pImpl->temporalWeights.empty()) {
                pImpl->temporalWeights.erase(pImpl->temporalWeights.begin());
            }
        }
    }
    
    // Age all episodes
    for (auto& ep : episodes_) {
        ep.age++;
        
        // Decay temporal weight based on age
        if (ep.age < pImpl->temporalWeights.size()) {
            pImpl->temporalWeights[ep.age] *= 0.99f;
        }
    }
    
    // Trigger consolidation for older episodes
    if (episodes_.size() > 10) {
        consolidate(0.5f); // More aggressive consolidation
    }
    
    NLM_LOG_INFO("Stored episodic memory: " + std::to_string(episodes_.size()) + " episodes, " +
                 std::to_string(enhancedEpisode.activeNeurons.size()) + " active neurons");
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::retrieveSimilar(
    const std::vector<float>& sensoryPattern,
    size_t maxResults) const {
    
    std::vector<std::pair<float, size_t>> scores;  // (similarity, index)
    
    // Use neural pattern similarity for retrieval
    for (size_t i = 0; i < episodes_.size(); ++i) {
        float sim = computeNeuralSimilarity(sensoryPattern, episodes_[i]);
        scores.emplace_back(sim, i);
    }
    
    // Sort by similarity (descending)
    std::sort(scores.begin(), scores.end(),
        [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // Return top results above threshold
    std::vector<const EpisodicMemoryItem*> results;
    for (size_t i = 0; i < std::min(maxResults, scores.size()); ++i) {
        if (scores[i].first > 0.2f) {  // Lower threshold for neural patterns
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
    
    std::vector<std::pair<float, size_t>> distances;  // (distance, index)
    
    for (size_t i = 0; i < episodes_.size(); ++i) {
        float dx = episodes_[i].positionX - x;
        float dy = episodes_[i].positionY - y;
        float dist = std::sqrt(dx * dx + dy * dy);
        
        if (dist <= radius) {
            distances.emplace_back(dist, i);
        }
    }
    
    // Sort by distance
    std::sort(distances.begin(), distances.end(),
        [](const auto& a, const auto& b) { return a.first < b.first; });
    
    std::vector<const EpisodicMemoryItem*> results;
    for (size_t i = 0; i < std::min(maxResults, distances.size()); ++i) {
        results.push_back(&episodes_[distances[i].second]);
    }
    
    return results;
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::retrieveAfterAction(
    ActionType action, size_t maxResults) const {
    
    std::vector<const EpisodicMemoryItem*> results;
    
    for (const auto& ep : episodes_) {
        if (ep.action == action) {
            results.push_back(&ep);
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
    
    // Enhanced replay: reactivate the complete neural pattern
    for (size_t i = 0; i < episode->activeNeurons.size() && 
                       i < episode->neuronActivations.size(); ++i) {
        NeuronId neuron = episode->activeNeurons[i];
        float activation = episode->neuronActivations[i];
        
        // Strong reactivation through brain's neural plasticity
        brain_->injectCurrent(neuron, activation * 5.0f);  // Stronger injection
        
        // Also update episodic memory patterns
        if (i < pImpl->patternMemories.size()) {
            for (auto& pattern : pImpl->patternMemories) {
                if (pattern.size() > i) {
                    pattern[i] = std::max(pattern[i], activation * 0.8f);
                }
            }
        }
    }
    
    // Update prediction error based on replay
    auto* predSys = brain_->getPredictionSystem();
    if (predSys) {
        // Replay provides prediction error signal
        float error = computeReplayPredictionError(*episode);
        // In a full implementation, this would update prediction system
    }
    
    NLM_LOG_INFO("Replayed episodic memory: " + std::to_string(episode->activeNeurons.size()) + " neurons");
}

void NeuralEpisodicMemory::updateRelevance(SimulationStep episodeId, float relevanceDelta) {
    if (episodeId < episodes_.size()) {
        // Update temporal weight for this episode
        if (episodeId < pImpl->temporalWeights.size()) {
            pImpl->temporalWeights[episodeId] = std::clamp(
                pImpl->temporalWeights[episodeId] + relevanceDelta, 0.0f, 1.0f);
        }
    }
}

const EpisodicMemoryItem* NeuralEpisodicMemory::getEpisode(size_t index) const {
    if (index < episodes_.size()) {
        return &episodes_[index];
    }
    return nullptr;
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::getRecentEpisodes(size_t count) const {
    std::vector<const EpisodicMemoryItem*> results;
    
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
    // Enhanced consolidation based on neural relevance
    
    // Use temporal weights and neural pattern similarity for consolidation decisions
    std::vector<size_t> toRemove;
    
    for (size_t i = 0; i < episodes_.size(); ++i) {
        // Calculate relevance based on multiple factors
        float relevance = 0.0f;
        
        // 1. Recency factor
        relevance += 1.0f / (episodes_[i].age + 1);
        
        // 2. Temporal weight
        if (i < pImpl->temporalWeights.size()) {
            relevance *= pImpl->temporalWeights[i];
        }
        
        // 3. Reward value
        relevance *= (episodes_[i].reward + 1.0f) * 0.3f;
        
        // 4. Pattern distinctiveness (compare to other episodes)
        float distinctiveness = 1.0f;
        if (!episodes_[i].activeNeurons.empty()) {
            // Check similarity to other episodes
            for (size_t j = 0; j < episodes_.size(); ++j) {
                if (i != j && !episodes_[j].activeNeurons.empty()) {
                    float sim = computeNeuralSimilarity(
                        episodes_[i].sensoryState, episodes_[j]);
                    if (sim > 0.7f) {
                        distinctiveness *= 0.9f;
                        break;
                    }
                }
            }
        }
        
        relevance *= distinctiveness;
        
        // Remove episodes below relevance threshold
        if (relevance < relevanceThreshold) {
            toRemove.push_back(i);
        }
    }
    
    // Remove low-relevance episodes (in reverse order)
    for (auto it = toRemove.rbegin(); it != toRemove.rend(); ++it) {
        size_t idx = *it;
        
        // Remove from deque (need to erase from front or use custom logic)
        auto epIt = episodes_.begin();
        std::advance(epIt, idx);
        episodes_.erase(epIt);
        
        // Remove corresponding pattern memory
        if (idx < pImpl->patternMemories.size()) {
            pImpl->patternMemories.erase(pImpl->patternMemories.begin() + idx);
        }
        
        // Remove temporal weight
        if (idx < pImpl->temporalWeights.size()) {
            pImpl->temporalWeights.erase(pImpl->temporalWeights.begin() + idx);
        }
    }
    
    NLM_LOG_INFO("Consolidated episodic memory: " + std::to_string(toRemove.size()) + " episodes removed");
}

void NeuralEpisodicMemory::clear() {
    episodes_.clear();
    pImpl->patternMemories.clear();
    pImpl->temporalWeights.clear();
    pImpl->episodicNeurons.clear();
    pImpl->patternConnections.clear();
    
    while (!pImpl->replayQueue.empty()) {
        pImpl->replayQueue.pop();
    }
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::getEpisodesForReplay(
    size_t count) const {
    
    // Enhanced selection for replay based on multiple criteria
    
    std::vector<std::tuple<float, float, float, size_t>> scored;  // (score, reward, age, index)
    
    for (size_t i = 0; i < episodes_.size(); ++i) {
        float recencyScore = 1.0f / (episodes_[i].age + 1);  // More recent = higher score
        float rewardScore = episodes_[i].reward;              // Higher reward = higher score
        
        // Neural pattern importance
        float patternScore = 0.0f;
        if (i < pImpl->patternMemories.size() && !pImpl->patternMemories[i].empty()) {
            float maxPattern = *std::max_element(pImpl->patternMemories[i].begin(),
                                                pImpl->patternMemories[i].end());
            patternScore = maxPattern;
        }
        
        // Combined score with neural weighting
        float score = recencyScore * 0.3f + rewardScore * 0.4f + patternScore * 0.3f;
        
        scored.emplace_back(score, rewardScore, episodes_[i].age, i);
    }
    
    // Sort by score (descending)
    std::sort(scored.begin(), scored.end(),
        [](const auto& a, const auto& b) {
            return std::get<0>(a) > std::get<0>(b);
        });
    
    std::vector<const EpisodicMemoryItem*> results;
    for (size_t i = 0; i < std::min(count, scored.size()); ++i) {
        results.push_back(&episodes_[std::get<3>(scored[i])]);
    }
    
    return results;
}

void NeuralEpisodicMemory::replaySequence(const std::vector<size_t>& episodeIds) {
    for (size_t id : episodeIds) {
        if (id < episodes_.size()) {
            replayEpisode(&episodes_[id]);
            
            // Add to replay queue for consolidation
            pImpl->replayQueue.push(id);
            
            // Small delay between replays for proper consolidation
            // (In simulation, this would be steps)
        }
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

float NeuralEpisodicMemory::computeNeuralSimilarity(
    const std::vector<float>& query,
    const EpisodicMemoryItem& episode) const {
    
    // Enhanced similarity that considers neural patterns
    
    if (query.empty() || episode.sensoryState.empty()) return 0.0f;
    
    float similarity = computeSimilarity(query, episode);
    
    // Boost similarity if neural patterns match
    for (size_t i = 0; i < query.size() && i < episode.activeNeurons.size(); ++i) {
        if (brain_) {
            // Get current activation of the neuron that was active during episode
            NeuronId neuron = episode.activeNeurons[i];
            float currentAct = brain_->getWorkingMemory()->getNeuronActivation(neuron);
            float episodeAct = episode.neuronActivations[i];
            
            // Similarity based on current neural state vs episode state
            float neuralSim = 1.0f - std::abs(currentAct - episodeAct);
            similarity = std::max(similarity, neuralSim * 0.5f);
        }
    }
    
    return similarity;
}

float NeuralEpisodicMemory::computeReplayPredictionError(const EpisodicMemoryItem& episode) const {
    // Compute prediction error during replay
    
    float error = 0.0f;
    
    if (!episode.sensoryState.empty()) {
        // Compare sensory pattern with predicted pattern
        // This would use brain's prediction system in full implementation
        
        // Simple calculation: difference between expected reward and actual reward
        float rewardError = std::abs(episode.reward - 0.5f);  // Assume target reward 0.5
        error = rewardError;
    }
    
    return error;
}

bool NeuralEpisodicMemory::matchesCriteria(
    const EpisodicMemoryItem& episode,
    const std::vector<float>* sensoryQuery,
    SimulationStep* startTime,
    SimulationStep* endTime,
    ActionType* actionQuery) const {
    
    if (sensoryQuery) {
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
