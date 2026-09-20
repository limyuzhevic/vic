#pragma once

#include "../core/Types/Types.hpp"
#include "IMemorySystem.hpp"
#include <vector>
#include <memory>
#include <deque>
#include <functional>

namespace nlm {

// Forward declarations
class Brain;
class BrainMemoryAdapter;

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
//
// Now implements IEpisodicMemory interface for better integration

class NeuralEpisodicMemory : public IEpisodicMemory {
public:
    NeuralEpisodicMemory();
    ~NeuralEpisodicMemory() override;

    // IEpisodicMemory interface implementation
    void initialize(Brain* owner) override;
    void update(TimestepDuration dt) override;
    void clear() override;
    Brain* getOwner() const override { return brain_; }
    bool isInitialized() const override { return brain_ != nullptr; }

    // IEpisodicMemory interface implementation
    void storeEpisode(const EpisodicMemoryItem& episode) override;
    std::vector<const EpisodicMemoryItem*> retrieveSimilar(
        const std::vector<float>& sensoryPattern,
        size_t maxResults = 5) const override;
    std::vector<const EpisodicMemoryItem*> retrieveTemporal(
        SimulationStep startTime,
        SimulationStep endTime,
        size_t maxResults = 10) const override;
    std::vector<const EpisodicMemoryItem*> retrieveByLocation(
        float x, float y,
        float radius,
        size_t maxResults = 5) const override;
    std::vector<const EpisodicMemoryItem*> retrieveAfterAction(
        ActionType action,
        size_t maxResults = 5) const override;
    void replayEpisode(const EpisodicMemoryItem* episode) override;
    void updateRelevance(SimulationStep episodeId, float relevanceDelta) override;
    const EpisodicMemoryItem* getEpisode(size_t index) const override;
    size_t getEpisodeCount() const override { return episodes_.size(); }
    std::vector<const EpisodicMemoryItem*> getRecentEpisodes(size_t count) const override;
    float getAverageReward() const override;
    void consolidate(float relevanceThreshold) override;
    void enableReplay(bool enable) override { replayEnabled_ = enable; }
    bool isReplayEnabled() const override { return replayEnabled_; }
    std::vector<const EpisodicMemoryItem*> getEpisodesForReplay(size_t count) const override;
    void replaySequence(const std::vector<size_t>& episodeIds) override;
    size_t getMaxEpisodes() const override { return maxEpisodes_; }
    void setMaxEpisodes(size_t max) override { maxEpisodes_ = max; }

    // Legacy methods (for compatibility)
    void initialize(Brain* brain) { initialize(brain); }
    void update(TimestepDuration dt) { update(dt); }
    void clear() { clear(); }

    // Set adapter for clean brain interaction
    void setAdapter(std::shared_ptr<BrainMemoryAdapter> adapter) { adapter_ = adapter; }

private:
    // Compute similarity between query and stored episode
    float computeSimilarity(const std::vector<float>& query,
                          const EpisodicMemoryItem& episode) const;

    // Check if episode matches criteria
    bool matchesCriteria(const EpisodicMemoryItem& episode,
                        const std::vector<float>* sensoryQuery,
                        SimulationStep* startTime,
                        SimulationStep* endTime,
                        ActionType* actionQuery) const;

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    std::shared_ptr<BrainMemoryAdapter> adapter_;
    std::deque<EpisodicMemoryItem> episodes_;
    size_t maxEpisodes_;
    bool replayEnabled_;
};

// NeuralAssociativeMemory: Creates and retrieves relationships between experiences
// NOT a lookup table - associations are encoded in synaptic connectivity
//
// Key mechanisms:
// - Co-activating neurons become associated via Hebbian plasticity
// - Stronger associations form with repeated co-activation
// - Can retrieve related items given a partial cue
// - Spreading activation for associative recall
//
// Now implements IAssociativeMemory interface for better integration

class NeuralAssociativeMemory : public IAssociativeMemory {
public:
    NeuralAssociativeMemory();
    ~NeuralAssociativeMemory() override;

    // IAssociativeMemory interface implementation
    void initialize(Brain* owner) override;
    void update(TimestepDuration dt) override;
    void clear() override;
    Brain* getOwner() const override { return brain_; }
    bool isInitialized() const override { return brain_ != nullptr; }

    // IAssociativeMemory interface implementation
    void associate(const std::vector<float>& patternA,
                   const std::vector<float>& patternB,
                   float strength) override;
    void associateFromExperience(const EpisodicMemoryItem& episode) override;
    std::vector<std::vector<float>> retrieve(
        const std::vector<float>& queryPattern,
        size_t maxResults) const override;
    float getAssociationStrength(
        const std::vector<float>& patternA,
        const std::vector<float>& patternB) const override;
    void updateAssociation(const std::vector<float>& patternA,
                          const std::vector<float>& patternB,
                          float delta) override;
    std::vector<std::vector<float>> spreadActivation(
        const std::vector<float>& cuePattern,
        size_t steps) const override;
    void clear() override;
    size_t getAssociationCount() const override { return associations_.size(); }

    // Legacy methods (for compatibility)
    void initialize(Brain* brain) { initialize(brain); }
    void update(TimestepDuration dt) { update(dt); }
    void clear() { clear(); }

    // Set adapter for clean brain interaction
    void setAdapter(std::shared_ptr<BrainMemoryAdapter> adapter) { adapter_ = adapter; }

private:
    // Find pattern neuron or create one
    NeuronId findPatternNeuron(const std::vector<float>& pattern);

    // Compute pattern similarity
    float computeSimilarity(const std::vector<float>& a,
                          const std::vector<float>& b) const;

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    std::shared_ptr<BrainMemoryAdapter> adapter_;
    std::vector<std::pair<NeuronId, std::vector<float>>> patternNeurons_;
    std::vector<std::tuple<NeuronId, NeuronId, float>> associations_;
};

} // namespace nlm