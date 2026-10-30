#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <unordered_map>
#include <string>

namespace nlm {

// Working Memory: Neural working memory with persistent activity and plasticity
// Implements working memory using neural dynamics, not just storage
//
// Key mechanisms:
// - Persistent neural activity (reverberating activity)
// - Activity-dependent synaptic facilitation/inhibition
// - Trace aging and decay
// - Consolidation to episodic memory
// - Competition between traces
//
// This is NOT a simple buffer - it's an active neural system that maintains information

class WorkingMemory {
public:
    WorkingMemory();
    ~WorkingMemory();
    
    // Initialize working memory with configuration
    void initialize(std::shared_ptr<Config> config);
    
    // Store neural pattern with automatic encoding and plasticity
    void storePattern(const std::vector<float>& pattern, float strength = 1.0f);
    
    // Store specific neuron activation (for direct neural manipulation)
    void storeActivation(NeuronId neuron, float activation, float importance = 1.0f);
    
    // Retrieve current working memory as neural activity pattern
    std::vector<float> retrievePattern() const;
    
    // Get activation level for a specific neuron
    float getNeuronActivation(NeuronId neuron) const;
    
    // Check if neuron is currently active in working memory
    bool containsNeuron(NeuronId neuron) const;
    
    // Update working memory (maintain persistent activity, apply plasticity)
    void update(TimestepDuration dt);
    
    // Clear all memory traces
    void clear();
    
    // Get working memory statistics
    size_t getTraceCount() const;
    float getMemoryLoad() const;
    float getAverageActivation() const;
    float getMemoryStability() const;
    
    // Competition and selection
    std::vector<NeuronId> getActiveNeurons() const;
    std::vector<NeuronId> getWinningNeurons(float threshold = 0.5f) const;
    
    // Consolidation to long-term memory
    void consolidate(float threshold);
    bool shouldConsolidate() const;
    
    // External integration
    void integrateWith(NeuralWorkingMemory& other);
    void decay(float rate);
    
    // Save/load functionality
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Neural Episodic Memory: Storage of experienced events and episodes
// Implements episodic memory using neural assemblies and pattern completion
//
// Key mechanisms:
// - Episode encoding with temporal context
// - Pattern completion and recall
// - Memory replay during sleep/rest
// - Emotional tagging (via neuromodulators)
// - Spatial and temporal indexing
//
// Each episode is a distributed neural pattern that can be reactivated

class NeuralEpisodicMemory {
public:
    NeuralEpisodicMemory();
    ~NeuralEpisodicMemory();
    
    void initialize(std::shared_ptr<Config> config);
    
    // Store an experienced episode with neural pattern
    void storeEpisode(const std::vector<float>& sensoryPattern,
                     const std::vector<float>& resultingPattern,
                     ActionType action = ActionType::None,
                     float reward = 0.0f,
                     float emotionalValue = 0.0f);
    
    // Retrieve similar episodes based on current sensory input
    std::vector<EpisodicMemoryItem> retrieveSimilar(
        const std::vector<float>& pattern,
        size_t maxResults = 5) const;
    
    // Retrieve episodes by time range
    std::vector<EpisodicMemoryItem> retrieveTemporal(
        SimulationStep startTime,
        SimulationStep endTime,
        size_t maxResults = 10) const;
    
    // Retrieve episodes by spatial location
    std::vector<EpisodicMemoryItem> retrieveByLocation(
        float x, float y, float radius = 10.0f,
        size_t maxResults = 5) const;
    
    // Retrieve episodes by action taken
    std::vector<EpisodicMemoryItem> retrieveByAction(
        ActionType action,
        size_t maxResults = 5) const;
    
    // Activate memory replay for training/learning
    void replayEpisode(const EpisodicMemoryItem& episode);
    void replaySequence(const std::vector<size_t>& episodeIds);
    void replayAll(float probability = 0.1f);
    
    // Update memory relevance and consolidation
    void updateRelevance(size_t episodeId, float delta);
    void consolidate(float relevanceThreshold);
    
    // Get stored episodes
    const EpisodicMemoryItem* getEpisode(size_t index) const;
    std::vector<const EpisodicMemoryItem*> getAllEpisodes() const;
    std::vector<const EpisodicMemoryItem*> getRecentEpisodes(size_t count) const;
    
    // Memory statistics
    size_t getEpisodeCount() const;
    float getAverageReward() const;
    float getMemoryStrength() const;
    
    // Memory management
    void clear();
    bool isEmpty() const;
    void pruneOldEpisodes(size_t maxCount);
    
    // Save/load functionality
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Neural Associative Memory: Network of pattern associations
// Implements associative memory using neural assemblies and spreading activation
//
// Key mechanisms:
// - Pattern completion from partial cues
// - Spreading activation through association network
// - Bidirectional associations (A->B and B->A)
// - Strength-based learning (Hebbian-like)
// - Interference management
//
// Each association is a bidirectional link between neural patterns

class NeuralAssociativeMemory {
public:
    NeuralAssociativeMemory();
    ~NeuralAssociativeMemory();
    
    void initialize(std::shared_ptr<Config> config);
    
    // Create association between two patterns
    void associate(const std::vector<float>& patternA,
                   const std::vector<float>& patternB,
                   float strength = 1.0f);
    
    // Build associations from experiences
    void buildFromExperience(const EpisodicMemoryItem& episode);
    
    // Retrieve pattern given partial cue
    std::vector<std::vector<float>> retrieve(
        const std::vector<float>& cue,
        size_t maxResults = 5) const;
    
    // Get association strength between patterns
    float getAssociationStrength(
        const std::vector<float>& patternA,
        const std::vector<float>& patternB) const;
    
    // Update association strength
    void updateAssociation(const std::vector<float>& patternA,
                          const std::vector<float>& patternB,
                          float delta);
    
    // Spread activation through network
    std::vector<std::vector<float>> spreadActivation(
        const std::vector<float>& cue,
        size_t steps = 5) const;
    
    // Get all associations for a pattern
    std::vector<std::vector<float>> getAssociates(
        const std::vector<float>& pattern) const;
    
    // Neural network operations
    void reinforceAssociations(float factor);
    void weakenAssociations(float factor);
    
    // Memory management
    void clear();
    bool isEmpty() const;
    size_t getAssociationCount() const;
    float getNetworkActivity() const;
    
    // Save/load functionality
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
