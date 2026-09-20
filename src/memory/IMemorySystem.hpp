#pragma once

#include "../core/Types/Types.hpp"
#include "BrainMemoryAdapter.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <string>
#include <unordered_map>

namespace nlm {

// Forward declarations
class Brain;

// Memory System Interface - Base abstraction for all memory systems
class IMemorySystem {
public:
    virtual ~IMemorySystem() = default;
    
    // Initialize the memory system with a reference to its owner
    virtual void initialize(Brain* owner) = 0;
    
    // Update system state
    virtual void update(TimestepDuration dt) = 0;
    
    // Clear all memory
    virtual void clear() = 0;
    
    // Get the owner brain reference
    virtual Brain* getOwner() const = 0;
    
    // Check if system is initialized
    virtual bool isInitialized() const = 0;
};

// Working Memory Interface - Abstraction for neural working memory
class IWorkingMemory : public virtual IMemorySystem {
public:
    virtual ~IWorkingMemory() = default;
    
    // Store neural pattern in working memory
    virtual void store(const std::vector<float>& pattern, float strength) = 0;
    
    // Store specific neuron activation
    virtual void storeToNeuron(NeuronId neuron, float activation) = 0;
    
    // Retrieve working memory content
    virtual std::vector<float> retrieve() const = 0;
    
    // Check if neuron is in working memory
    virtual bool contains(NeuronId neuron) const = 0;
    
    // Get activation level
    virtual float getNeuronActivation(NeuronId neuron) const = 0;
    
    // Get capacity
    virtual size_t getCapacity() const = 0;
    virtual void setCapacity(size_t cap) = 0;
    
    // Get decay rate
    virtual float getDecayRate() const = 0;
    virtual void setDecayRate(float rate) = 0;
    
    // Get active traces
    virtual size_t getActiveTraces() const = 0;
    
    // Competition between traces
    virtual void runCompetition() = 0;
    
    // Check if neuron is a winner
    virtual bool isWinning(NeuronId neuron) const = 0;
    
    // Get overall activity
    virtual float getMemoryActivity() const = 0;
    
    // Strengthen representations
    virtual void strengthenMemory(float factor) = 0;
    
    // Get neurons in memory
    virtual const std::vector<NeuronId>& getMemoryNeurons() const = 0;
};

// Episodic Memory Interface - Abstraction for episodic memory
class IEpisodicMemory : public virtual IMemorySystem {
public:
    virtual ~IEpisodicMemory() = default;
    
    // Store an experience
    virtual void storeEpisode(const EpisodicMemoryItem& episode) = 0;
    
    // Retrieve similar episodes
    virtual std::vector<const EpisodicMemoryItem*> retrieveSimilar(
        const std::vector<float>& sensoryPattern,
        size_t maxResults = 5) const = 0;
    
    // Retrieve temporal episodes
    virtual std::vector<const EpisodicMemoryItem*> retrieveTemporal(
        SimulationStep startTime,
        SimulationStep endTime,
        size_t maxResults = 10) const = 0;
    
    // Retrieve by location
    virtual std::vector<const EpisodicMemoryItem*> retrieveByLocation(
        float x, float y,
        float radius,
        size_t maxResults = 5) const = 0;
    
    // Retrieve by action
    virtual std::vector<const EpisodicMemoryItem*> retrieveAfterAction(
        ActionType action,
        size_t maxResults = 5) const = 0;
    
    // Replay episode
    virtual void replayEpisode(const EpisodicMemoryItem* episode) = 0;
    
    // Update relevance
    virtual void updateRelevance(SimulationStep episodeId, float relevanceDelta) = 0;
    
    // Get episode
    virtual const EpisodicMemoryItem* getEpisode(size_t index) const = 0;
    
    // Get episode count
    virtual size_t getEpisodeCount() const = 0;
    
    // Get recent episodes
    virtual std::vector<const EpisodicMemoryItem*> getRecentEpisodes(size_t count) const = 0;
    
    // Get average reward
    virtual float getAverageReward() const = 0;
    
    // Consolidate memories
    virtual void consolidate(float relevanceThreshold) = 0;
    
    // Enable/disable replay
    virtual void enableReplay(bool enable) = 0;
    virtual bool isReplayEnabled() const = 0;
    
    // Get episodes for replay
    virtual std::vector<const EpisodicMemoryItem*> getEpisodesForReplay(size_t count) const = 0;
    
    // Replay sequence
    virtual void replaySequence(const std::vector<size_t>& episodeIds) = 0;
    
    // Get max episodes
    virtual size_t getMaxEpisodes() const = 0;
    virtual void setMaxEpisodes(size_t max) = 0;
};

// Associative Memory Interface - Abstraction for associative memory
class IAssociativeMemory : public virtual IMemorySystem {
public:
    virtual ~IAssociativeMemory() = default;
    
    // Create association between patterns
    virtual void associate(const std::vector<float>& patternA,
                         const std::vector<float>& patternB,
                         float strength) = 0;
    
    // Create association from experience
    virtual void associateFromExperience(const EpisodicMemoryItem& episode) = 0;
    
    // Retrieve patterns
    virtual std::vector<std::vector<float>> retrieve(
        const std::vector<float>& queryPattern,
        size_t maxResults) const = 0;
    
    // Get association strength
    virtual float getAssociationStrength(
        const std::vector<float>& patternA,
        const std::vector<float>& patternB) const = 0;
    
    // Update association
    virtual void updateAssociation(const std::vector<float>& patternA,
                                  const std::vector<float>& patternB,
                                  float delta) = 0;
    
    // Spread activation
    virtual std::vector<std::vector<float>> spreadActivation(
        const std::vector<float>& cuePattern,
        size_t steps) const = 0;
    
    // Clear associations
    virtual void clear() = 0;
    
    // Get association count
    virtual size_t getAssociationCount() const = 0;
};

// Memory System Factory Interface - Creates and manages memory systems
class IMemorySystemFactory {
public:
    virtual ~IMemorySystemFactory() = default;
    
    // Create working memory system
    virtual std::unique_ptr<IWorkingMemory> createWorkingMemory() = 0;
    
    // Create episodic memory system
    virtual std::unique_ptr<IEpisodicMemory> createEpisodicMemory() = 0;
    
    // Create associative memory system
    virtual std::unique_ptr<IAssociativeMemory> createAssociativeMemory() = 0;
    
    // Get default configuration
    virtual const std::unordered_map<std::string, std::string>& getDefaultConfig() const = 0;
};

// Default implementation of IMemorySystemFactory
class DefaultMemorySystemFactory : public IMemorySystemFactory {
public:
    DefaultMemorySystemFactory() : defaultAdapter_(std::make_shared<DefaultBrainMemoryAdapter>()) {}
    
    std::unique_ptr<IWorkingMemory> createWorkingMemory() override {
        auto memory = std::make_unique<WorkingMemoryAdapter>();
        memory->initialize(nullptr, defaultAdapter_);
        return memory;
    }
    
    std::unique_ptr<IEpisodicMemory> createEpisodicMemory() override {
        auto memory = std::make_unique<EpisodicMemoryAdapter>();
        memory->initialize(nullptr, defaultAdapter_);
        return memory;
    }
    
    std::unique_ptr<IAssociativeMemory> createAssociativeMemory() override {
        auto memory = std::make_unique<AssociativeMemoryAdapter>();
        memory->initialize(nullptr, defaultAdapter_);
        return memory;
    }
    
    const std::unordered_map<std::string, std::string>& getDefaultConfig() const override {
        // Return default configuration
        static std::unordered_map<std::string, std::string> defaultConfig;
        return defaultConfig;
    }
    
    // Set default adapter
    void setDefaultAdapter(std::shared_ptr<BrainMemoryAdapter> adapter) {
        defaultAdapter_ = adapter;
    }
    
    // Get default adapter
    std::shared_ptr<BrainMemoryAdapter> getDefaultAdapter() const {
        return defaultAdapter_;
    }

private:
    std::shared_ptr<BrainMemoryAdapter> defaultAdapter_;
};

// Forward declarations
class WorkingMemoryAdapter;
class EpisodicMemoryAdapter;
class AssociativeMemoryAdapter;
class DefaultMemorySystemFactory;

} // namespace nlm
