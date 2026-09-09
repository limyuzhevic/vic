#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <functional>

namespace nlm {

// NeuralWorkingMemory: Temporary neural activity patterns for active maintenance
// Phase 3+: Implements persistent activity using recurrent connections
// Compatible with Brain::Impl workingMemory member

class NeuralWorkingMemory {
public:
    NeuralWorkingMemory();
    ~NeuralWorkingMemory();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Store information in working memory
    void store(const std::vector<float>& pattern, float strength = 1.0f);
    
    // Store specific neuron activation
    void storeToNeuron(NeuronId neuron, float activation);
    
    // Retrieve current working memory content
    std::vector<float> retrieve() const;
    
    // Check if neuron is in working memory
    bool contains(NeuronId neuron) const;
    
    // Get neuron activation
    float getNeuronActivation(NeuronId neuron) const;
    
    // Update working memory
    void update(TimestepDuration dt);
    
    // Clear working memory
    void clear();
    
    // Get statistics
    size_t getActiveTraces() const { return activeTraces_.size(); }
    size_t getCapacity() const { return capacity_; }
    float getDecayRate() const { return decayRate_; }
    
    // Get memory neurons
    const std::vector<NeuronId>& getMemoryNeurons() const { return memoryNeurons_; }
    
    // Competition
    void runCompetition();
    bool isWinning(NeuronId neuron) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    size_t capacity_;
    float decayRate_;
    std::vector<NeuronId> memoryNeurons_;
    std::vector<float> memoryActivations_;
    std::vector<SimulationStep> memoryTimestamps_;
    std::vector<size_t> activeTraces_;
    std::vector<std::pair<NeuronId, NeuronId>> recurrentConnections_;
    std::vector<NeuronId> winners_;
};

// NeuralEpisodicMemory: Persistent experience storage and replay
// Phase 3+: Stores neural patterns of experiences with temporal indexing
// Compatible with Brain::Impl episodicMemory member

struct EpisodicMemoryItem {
    SimulationStep timestamp;
    std::vector<NeuronId> neurons;
    std::vector<float> activations;
    std::string metadata;
    
    EpisodicMemoryItem() : timestamp(0) {}
};

class NeuralEpisodicMemory {
public:
    NeuralEpisodicMemory();
    ~NeuralEpisodicMemory();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Store experience
    void storeEpisode(const EpisodicMemoryItem& episode);
    
    // Retrieve episode
    EpisodicMemoryItem retrieveEpisode(size_t index) const;
    
    // Get episode count
    size_t getEpisodeCount() const;
    
    // Get recent episodes
    std::vector<EpisodicMemoryItem> getRecentEpisodes(size_t count) const;
    
    // Get episodes for replay
    std::vector<const EpisodicMemoryItem*> getEpisodesForReplay(size_t count) const;
    
    // Replay an episode
    void replayEpisode(const EpisodicMemoryItem* episode);
    
    // Clear memory
    void clear();
    
    // Consolidation
    void consolidate(float relevanceThreshold);
    
    // Set capacity
    void setMaxEpisodes(size_t max) { maxEpisodes_ = max; }
    
    size_t getMaxEpisodes() const { return maxEpisodes_; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    size_t maxEpisodes_;
};

// NeuralAssociativeMemory: Pattern relationship storage
// Phase 4+: Stores Hebbian associations between neural patterns
// Compatible with Brain::Impl associativeMemory member (implemented separately)

class NeuralAssociativeMemory {
public:
    NeuralAssociativeMemory();
    ~NeuralAssociativeMemory();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Create association
    void associate(NeuronId a, NeuronId b, float strength);
    
    // Get associations
    std::vector<NeuronId> getAssociations(NeuronId neuron) const;
    
    // Get strength
    float getAssociationStrength(NeuronId a, NeuronId b) const;
    
    // Update association
    void updateAssociation(NeuronId a, NeuronId b, float delta);
    
    // Clear all
    void clear();
    
    // Update dynamics
    void update(TimestepDuration dt);
    
    // Pattern-based operations (Phase 6)
    void associatePattern(const std::vector<NeuronId>& neurons,
                         const std::vector<float>& activations,
                         float strength);
    void retrievePattern(const std::vector<NeuronId>& pattern,
                        std::vector<float>& activations,
                        float& strength) const;
    
    size_t getTotalAssociations() const { return totalAssociations_; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    size_t totalAssociations_;
};

} // namespace nlm
