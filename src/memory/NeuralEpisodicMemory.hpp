#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../motor/Action.hpp"
#include <vector>
#include <memory>
#include <deque>
#include <functional>

namespace nlm {

// EpisodicMemoryItem: Represents a stored experience
// NOT a textual record - stores neural activity patterns and associations
struct EpisodicMemoryItem {
    SimulationStep timestamp;
    
    // Sensory state at time of experience (encoded as activity pattern)
    std::vector<float> sensoryState;
    
    // Location/position information
    float positionX;
    float positionY;
    float orientation;
    
    // Action taken (if any)
    ActionType action;
    
    // Internal state (energy, reward, etc.)
    float reward;
    float energy;
    float novelty;
    
    // Consequence of action
    std::vector<float> resultingSensoryState;
    float resultingReward;
    
    // Associated neural activity pattern (who was firing)
    std::vector<NeuronId> activeNeurons;
    std::vector<float> neuronActivations;
    
    // Time since this episode
    SimulationStep age;
    
    EpisodicMemoryItem()
        : timestamp(0), positionX(0), positionY(0), orientation(0)
        , action(ActionType::Wait)
        , reward(0), energy(0), novelty(0)
        , resultingReward(0), age(0) {}
};

// NeuralEpisodicMemory: Stores experiences in a way that interacts with neural substrate
// NOT a database - episodes are encoded as synaptic patterns and can influence neural activity
//
// Key mechanisms:
// - Episodes stored as neural activity patterns
// - Retrieval via pattern completion (Hopfield-like)
// - Temporal ordering through timestamps
// - Relevance-weighted recall
// - Replay during offline periods

class NeuralEpisodicMemory {
public:
    NeuralEpisodicMemory();
    ~NeuralEpisodicMemory();

    // Initialize with brain reference
    void initialize(Brain* brain) {
        pImpl->brain = brain;
        brain_ = brain;
        NLM_LOG_INFO("NeuralEpisodicMemory initialized");
    }

    // Store a new experience
    // The experience is encoded as neural activity and can influence future processing
    void storeEpisode(const EpisodicMemoryItem& episode) {
        // Create episode copy with age 0
        EpisodicMemoryItem stored = episode;
        stored.age = 0;
        
        episodes_.push_back(stored);
        
        // Limit episode count
        if (episodes_.size() > maxEpisodes_) {
            episodes_.pop_front();
        }
        
        // Create episode neuron for pattern completion
        if (brain_ && !episode.sensoryState.empty()) {
            NeuronId epNeuron(20000 + episodes_.size() - 1);
            pImpl->episodeNeurons.push_back(epNeuron);
            
            // Inject current to establish the episode pattern
            brain_->injectCurrent(epNeuron, 1.0f);
        }
        
        // Enhance similarity retrieval
        enhanceSimilarityRetrieval(stored);
        
        // Update episode relevance
        updateRelevance(stored.timestamp, 1.0f);
    }

    // Retrieve episodes similar to a query pattern
    // Returns episodes ordered by relevance
    std::vector<const EpisodicMemoryItem*> retrieveSimilar(const std::vector<float>& sensoryPattern,
                                                           size_t maxResults = 5) const {
        std::vector<std::pair<float, size_t>> relevanceScores;
        
        for (size_t i = 0; i < episodes_.size(); ++i) {
            float similarity = computeSimilarity(sensoryPattern, episodes_[i]);
            relevanceScores.push_back({similarity, i});
        }
        
        // Sort by relevance (highest similarity first)
        std::sort(relevanceScores.begin(), relevanceScores.end(),
                  [](const std::pair<float, size_t>& a, const std::pair<float, size_t>& b) {
                      return a.first > b.first;
                  });
        
        std::vector<const EpisodicMemoryItem*> results;
        for (size_t i = 0; i < std::min(maxResults, relevanceScores.size()); ++i) {
            results.push_back(&episodes_[relevanceScores[i].second]);
        }
        
        return results;
    }

    // Retrieve episodes from a specific time window
    std::vector<const EpisodicMemoryItem*> retrieveTemporal(SimulationStep startTime,
                                                             SimulationStep endTime,
                                                             size_t maxResults = 10) const {
        std::vector<const EpisodicMemoryItem*> results;
        
        for (const auto& episode : episodes_) {
            if (episode.timestamp >= startTime && episode.timestamp <= endTime) {
                results.push_back(&episode);
                if (results.size() >= maxResults) break;
            }
        }
        
        return results;
    }

    // Retrieve episodes by location
    std::vector<const EpisodicMemoryItem*> retrieveByLocation(float x, float y,
                                                             float radius,
                                                             size_t maxResults = 5) const {
        std::vector<const EpisodicMemoryItem*> results;
        
        for (const auto& episode : episodes_) {
            float dx = episode.positionX - x;
            float dy = episode.positionY - y;
            float dist = std::sqrt(dx * dx + dy * dy);
            
            if (dist <= radius) {
                results.push_back(&episode);
                if (results.size() >= maxResults) break;
            }
        }
        
        return results;
    }

    // Retrieve episodes following a specific action
    std::vector<const EpisodicMemoryItem*> retrieveAfterAction(ActionType action,
                                                               size_t maxResults = 5) const {
        std::vector<const EpisodicMemoryItem*> results;
        
        for (const auto& episode : episodes_) {
            if (episode.action == action) {
                results.push_back(&episode);
                if (results.size() >= maxResults) break;
            }
        }
        
        return results;
    }

    // Replay an episode - reactivates the neural patterns associated with it
    // This allows the brain to "relive" the experience
    void replayEpisode(const EpisodicMemoryItem* episode) {
        if (!episode || !brain_) return;
        
        // Find the corresponding episode neuron
        for (NeuronId neuron : pImpl->episodeNeurons) {
            brain_->injectCurrent(neuron, 1.0f);
        }
        
        // Also reactivate associated neurons
        for (NeuronId neuronId : episode->activeNeurons) {
            brain_->injectCurrent(neuronId, 0.5f);
        }
    }

    // Update episode relevance based on recent use
    void updateRelevance(SimulationStep episodeId, float relevanceDelta) {
        // Find the episode by timestamp (id)
        for (auto& episode : episodes_) {
            if (episode.timestamp == episodeId) {
                // In a real implementation, this would update a relevance score
                // For now, just update the age to show it's been used
                episode.age = 0;
                break;
            }
        }
    }

    // Get episode by index
    const EpisodicMemoryItem* getEpisode(size_t index) const {
        if (index >= episodes_.size()) {
            return nullptr;
        }
        auto it = episodes_.begin();
        std::advance(it, index);
        return &(*it);
    }

    // Get total number of episodes
    size_t getEpisodeCount() const { return episodes_.size(); }

    // Phase 5 checkpoint support: full episode list for snapshot, and
    // restore that clears then re-stores (storeEpisode caps at
    // maxEpisodes_, so a snapshot larger than the cap is truncated
    // the same way live storage would be).
    const std::deque<EpisodicMemoryItem>& getEpisodes() const { return episodes_; }
    void restoreEpisodes(const std::vector<EpisodicMemoryItem>& episodes) {
        clear();
        for (const auto& ep : episodes) storeEpisode(ep);
    }
    void setMaxEpisodes(size_t n) { maxEpisodes_ = n; }

    // Get recent episodes
    std::vector<const EpisodicMemoryItem*> getRecentEpisodes(size_t count) const {
        std::vector<const EpisodicMemoryItem*> results;
        size_t startIdx = std::max(size_t(0), episodes_.size() - count);
        
        for (size_t i = startIdx; i < episodes_.size(); ++i) {
            results.push_back(&episodes_[i]);
        }
        
        return results;
    }

    // Get average reward from episodes
    float getAverageReward() const {
        if (episodes_.empty()) {
            return 0.0f;
        }
        
        float totalReward = 0.0f;
        for (const auto& episode : episodes_) {
            totalReward += episode.reward;
        }
        
        return totalReward / static_cast<float>(episodes_.size());
    }

    // Clear old episodes
    void consolidate(float relevanceThreshold) {
        auto it = episodes_.begin();
        while (it != episodes_.end()) {
            // In a real implementation, this would check episode relevance
            // For now, just remove episodes with very low reward
            if (it->reward < relevanceThreshold) {
                it = episodes_.erase(it);
            } else {
                ++it;
            }
        }
    }

    // Clear all episodes
    void clear() {
        for (const auto& episode : episodes_) {
            // Remove associated neurons from brain
            for (NeuronId neuronId : episode.activeNeurons) {
                brain_->injectCurrent(neuronId, 0.0f);
            }
        }
        
        episodes_.clear();
        pImpl->episodeNeurons.clear();
    }

    // Enable/disable replay
    void enableReplay(bool enable) { replayEnabled_ = enable; }
    bool isReplayEnabled() const { return replayEnabled_; }

    // Get episodes for replay (selection based on relevance and recency)
    std::vector<const EpisodicMemoryItem*> getEpisodesForReplay(size_t count) const {
        // Simple implementation: return most recent episodes
        return getRecentEpisodes(count);
    }

    // Replay multiple episodes (for sleep-like consolidation)
    void replaySequence(const std::vector<size_t>& episodeIds) {
        for (size_t id : episodeIds) {
            if (id >= episodes_.size()) continue;
            replayEpisode(&episodes_[id]);
        }
    }

private:
    // Compute similarity between query and stored episode
    float computeSimilarity(const std::vector<float>& query,
                          const EpisodicMemoryItem& episode) const {
        if (query.empty() || episode.sensoryState.empty()) {
            return 0.0f;
        }
        
        // Simple similarity based on pattern overlap
        size_t minSize = std::min(query.size(), episode.sensoryState.size());
        float similarity = 0.0f;
        
        for (size_t i = 0; i < minSize; ++i) {
            similarity += std::abs(query[i] - episode.sensoryState[i]);
        }
        
        // Normalize to [0, 1] where lower is more similar
        similarity = 1.0f - similarity / minSize;
        
        return similarity;
    }

    // Check if episode matches criteria
    bool matchesCriteria(const EpisodicMemoryItem& episode,
                        const std::vector<float>* sensoryQuery,
                        SimulationStep* startTime,
                        SimulationStep* endTime,
                        ActionType* actionQuery) const {
        // Implementation would check all provided criteria
        if (sensoryQuery) {
            // Compare sensory patterns
            float similarity = computeSimilarity(*sensoryQuery, episode);
            if (similarity < 0.5f) return false;  // Threshold for similarity
        }
        
        if (startTime && episode.timestamp < *startTime) return false;
        if (endTime && episode.timestamp > *endTime) return false;
        
        if (actionQuery && episode.action != *actionQuery) return false;
        
        return true;
    }

    void enhanceSimilarityRetrieval(const EpisodicMemoryItem& episode) {
        // This would enhance the retrieval system by adding the episode
        // to a faster lookup structure (like a KD-tree or hash table)
        // For now, just a placeholder for future enhancement
    }

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    std::deque<EpisodicMemoryItem> episodes_;
    size_t maxEpisodes_;
    bool replayEnabled_;
};

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    std::deque<EpisodicMemoryItem> episodes_;
    size_t maxEpisodes_;
    bool replayEnabled_;
};

// AssociativeMemory: Creates and retrieves relationships between experiences
// NOT a lookup table - associations are encoded in synaptic connectivity
//
// Key mechanisms:
// - Co-activating neurons become associated via Hebbian plasticity
// - Stronger associations form with repeated co-activation
// - Can retrieve related items given a partial cue
// - Spreading activation for associative recall

class NeuralAssociativeMemory {
public:
    NeuralAssociativeMemory();
    ~NeuralAssociativeMemory();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Create association between two neural patterns
    // Both patterns should be from actual neural activity
    void associate(const std::vector<float>& patternA,
                   const std::vector<float>& patternB,
                   float strength = 1.0f);

    // Create association based on experience
    void associateFromExperience(const EpisodicMemoryItem& episode);

    // Retrieve patterns associated with a query pattern
    std::vector<std::vector<float>> retrieve(const std::vector<float>& queryPattern,
                                            size_t maxResults = 5) const;

    // Get association strength between two patterns
    float getAssociationStrength(const std::vector<float>& patternA,
                                const std::vector<float>& patternB) const;

    // Update association based on outcome
    void updateAssociation(const std::vector<float>& patternA,
                          const std::vector<float>& patternB,
                          float delta);

    // Spread activation from cue pattern
    // Returns patterns that become activated through association chains
    std::vector<std::vector<float>> spreadActivation(const std::vector<float>& cuePattern,
                                                     size_t steps = 2) const;

    // Clear associations
    void clear();

    // Get number of associations
    size_t getAssociationCount() const { return associations_.size(); }

private:
    // Find pattern neuron or create one
    NeuronId findPatternNeuron(const std::vector<float>& pattern);

    // Compute pattern similarity
    float computeSimilarity(const std::vector<float>& a,
                            const std::vector<float>& b) const;

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    std::vector<std::pair<NeuronId, std::vector<float>>> patternNeurons_;
    std::vector<std::tuple<NeuronId, NeuronId, float>> associations_;
};

} // namespace nlm