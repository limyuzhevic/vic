#include "NeuralEpisodicMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <numeric>
#include <unordered_map>

namespace nlm {

struct NeuralEpisodicMemory::Impl {
    // Precompute similarity matrix for fast retrieval
    std::vector<std::vector<float>> similarityMatrix;
    std::vector<size_t> episodeIndices;
    size_t maxCapacity;
    
    // Episode relevance scoring
    std::unordered_map<size_t, float> relevanceScores;
    
    Impl(size_t maxCap) : maxCapacity(maxCap) {}
};

NeuralEpisodicMemory::NeuralEpisodicMemory()
    : pImpl(new Impl(1000))
    , brain_(nullptr)
    , maxEpisodes_(1000)
    , replayEnabled_(true)
{
}

NeuralEpisodicMemory::~NeuralEpisodicMemory() = default;

void NeuralEpisodicMemory::initialize(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot initialize NeuralEpisodicMemory with null brain");
        return;
    }
    brain_ = brain;
    NLM_LOG_INFO("NeuralEpisodicMemory initialized");
}

void NeuralEpisodicMemory::storeEpisode(const EpisodicMemoryItem& episode) {
    if (!brain_) return;
    
    // Add episode to storage
    episodes_.push_back(episode);
    
    // Trim if over capacity
    while (episodes_.size() > maxEpisodes_) {
        episodes_.pop_front();
    }
    
    // Update similarity matrix
    updateSimilarityMatrix();
    
    // Initialize relevance score
    pImpl->relevanceScores[episodes_.size() - 1] = 1.0f;
    
    NLM_LOG_DEBUG("Stored episodic memory episode at step " + std::to_string(episode.timestamp));
}

void NeuralEpisodicMemory::updateSimilarityMatrix() {
    // Precompute similarity between all episodes for fast retrieval
    // This is expensive but allows O(1) similarity queries
    size_t n = episodes_.size();
    similarityMatrix.clear();
    similarityMatrix.resize(n, std::vector<float>(n, 0.0f));
    
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            if (i != j) {
                similarityMatrix[i][j] = computeSimilarity(
                    episodes_[i].sensoryState, 
                    episodes_[j].sensoryState
                );
            }
        }
    }
}

float NeuralEpisodicMemory::computeSimilarity(const std::vector<float>& query,
                                              const EpisodicMemoryItem& episode) const {
    if (query.empty() || episode.sensoryState.empty()) return 0.0f;
    
    size_t minSize = std::min(query.size(), episode.sensoryState.size());
    float dotProduct = 0.0f;
    float queryNorm = 0.0f;
    float episodeNorm = 0.0f;
    
    for (size_t i = 0; i < minSize; ++i) {
        dotProduct += query[i] * episode.sensoryState[i];
        queryNorm += query[i] * query[i];
        episodeNorm += episode.sensoryState[i] * episode.sensoryState[i];
    }
    
    if (queryNorm == 0.0f || episodeNorm == 0.0f) return 0.0f;
    
    return dotProduct / std::sqrt(queryNorm * episodeNorm);
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::retrieveSimilar(
    const std::vector<float>& sensoryPattern,
    size_t maxResults) const {
    std::vector<std::pair<float, size_t>> similarities;
    
    for (size_t i = 0; i < episodes_.size(); ++i) {
        float similarity = computeSimilarity(sensoryPattern, episodes_[i]);
        similarities.emplace_back(similarity, i);
    }
    
    // Sort by similarity (descending)
    std::sort(similarities.begin(), similarities.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // Return top results
    std::vector<const EpisodicMemoryItem*> results;
    results.reserve(std::min(maxResults, similarities.size()));
    
    for (size_t i = 0; i < std::min(maxResults, similarities.size()); ++i) {
        results.push_back(&episodes_[similarities[i].second]);
    }
    
    return results;
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::retrieveTemporal(
    SimulationStep startTime,
    SimulationStep endTime,
    size_t maxResults) const {
    std::vector<const EpisodicMemoryItem*> results;
    
    for (const auto& episode : episodes_) {
        if (episode.timestamp >= startTime && episode.timestamp <= endTime) {
            results.push_back(&episode);
            if (results.size() >= maxResults) break;
        }
    }
    
    return results;
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::retrieveByLocation(
    float x, float y,
    float radius,
    size_t maxResults) const {
    std::vector<const EpisodicMemoryItem*> results;
    
    for (const auto& episode : episodes_) {
        float dx = episode.positionX - x;
        float dy = episode.positionY - y;
        float distance = std::sqrt(dx * dx + dy * dy);
        
        if (distance <= radius) {
            results.push_back(&episode);
            if (results.size() >= maxResults) break;
        }
    }
    
    return results;
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::retrieveAfterAction(
    ActionType action,
    size_t maxResults) const {
    std::vector<const EpisodicMemoryItem*> results;
    
    for (const auto& episode : episodes_) {
        if (episode.action == action) {
            results.push_back(&episode);
            if (results.size() >= maxResults) break;
        }
    }
    
    return results;
}

void NeuralEpisodicMemory::replayEpisode(const EpisodicMemoryItem* episode) {
    if (!episode || !brain_ || !replayEnabled_) return;
    
    // Reactivate the neural patterns from this episode
    NLM_LOG_INFO("Replaying episodic memory episode at step " + std::to_string(episode->timestamp));
    
    for (size_t i = 0; i < episode->activeNeurons.size(); ++i) {
        if (i < episode->neuronActivations.size()) {
            float activation = episode->neuronActivations[i] * 2.0f;  // Amplify during replay
            brain_->injectCurrent(episode->activeNeurons[i], activation);
        }
    }
    
    // Store replay in working memory for consolidation
    if (brain_->getWorkingMemory()) {
        std::vector<float> replayPattern;
        replayPattern.reserve(episode->neuronActivations.size());
        
        for (float activation : episode->neuronActivations) {
            replayPattern.push_back(activation * 0.5f);  // Scale down for working memory
        }
        
        brain_->getWorkingMemory()->store(replayPattern, 1.0f);
    }
}

void NeuralEpisodicMemory::updateRelevance(SimulationStep episodeId, float relevanceDelta) {
    // Note: We can't update relevance of stored episodes directly since they're in a deque
    // This would require more sophisticated storage that supports random access
    // For now, we'll just log that relevance update was requested
    if (relevanceDelta != 0.0f) {
        NLM_LOG_DEBUG("Relevance update requested for episode " + std::to_string(episodeId) + 
                     " by delta: " + std::to_string(relevanceDelta));
    }
}

const EpisodicMemoryItem* NeuralEpisodicMemory::getEpisode(size_t index) const {
    if (index < episodes_.size()) {
        // Return pointer to element in deque - careful with lifetime
        auto it = episodes_.begin();
        std::advance(it, index);
        return &*it;
    }
    return nullptr;
}

void NeuralEpisodicMemory::consolidate(float relevanceThreshold) {
    if (relevanceThreshold <= 0.0f || relevanceThreshold > 1.0f) return;
    
    // Remove low-relevance episodes to make room for new ones
    size_t originalSize = episodes_.size();
    episodes_.erase(std::remove_if(episodes_.begin(), episodes_.end(),
                                   [this, relevanceThreshold](const EpisodicMemoryItem& episode) {
                                       // Simple heuristic: remove episodes with low reward and old age
                                       float relevance = episode.reward / 10.0f + 
                                                        (10000.0f - static_cast<float>(episode.age)) / 10000.0f;
                                       return relevance < relevanceThreshold;
                                   }),
                    episodes_.end());
    
    if (episodes_.size() < originalSize) {
        NLM_LOG_INFO("Consolidated episodic memory: removed " + 
                     std::to_string(originalSize - episodes_.size()) + " low-relevance episodes");
    }
    
    // Update similarity matrix after consolidation
    updateSimilarityMatrix();
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::getRecentEpisodes(size_t count) const {
    std::vector<const EpisodicMemoryItem*> results;
    
    // Get last 'count' episodes (they're in temporal order)
    size_t startPos = std::max<size_t>(0, episodes_.size() - count);
    size_t endPos = episodes_.size();
    
    for (size_t i = startPos; i < endPos; ++i) {
        auto it = episodes_.begin();
        std::advance(it, i);
        results.push_back(&*it);
    }
    
    return results;
}

float NeuralEpisodicMemory::getAverageReward() const {
    if (episodes_.empty()) return 0.0f;
    
    float totalReward = 0.0f;
    for (const auto& episode : episodes_) {
        totalReward += episode.reward;
    }
    
    return totalReward / episodes_.size();
}

void NeuralEpisodicMemory::clear() {
    episodes_.clear();
    updateSimilarityMatrix();
    NLM_LOG_INFO("Cleared all episodic memory episodes");
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::getEpisodesForReplay(size_t count) const {
    // Select episodes for replay based on relevance (reward) and recency
    std::vector<std::pair<float, const EpisodicMemoryItem*>> episodesWithRelevance;
    
    for (const auto& episode : episodes_) {
        // Compute relevance score: combines reward and recency
        float recency = 1.0f - std::min(1.0f, static_cast<float>(episode.age) / 1000.0f);
        float rewardComponent = std::min(1.0f, episode.reward / 10.0f);
        float relevance = 0.7f * rewardComponent + 0.3f * recency;
        
        episodesWithRelevance.emplace_back(relevance, &episode);
    }
    
    // Sort by relevance (descending)
    std::sort(episodesWithRelevance.begin(), episodesWithRelevance.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // Return top results
    std::vector<const EpisodicMemoryItem*> results;
    results.reserve(std::min(count, episodesWithRelevance.size()));
    
    for (size_t i = 0; i < std::min(count, episodesWithRelevance.size()); ++i) {
        results.push_back(episodesWithRelevance[i].second);
    }
    
    return results;
}

void NeuralEpisodicMemory::replaySequence(const std::vector<size_t>& episodeIds) {
    for (size_t id : episodeIds) {
        if (id < episodes_.size()) {
            auto it = episodes_.begin();
            std::advance(it, id);
            replayEpisode(&*it);
        }
    }
}