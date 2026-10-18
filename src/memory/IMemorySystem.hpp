#pragma once

#include "../core/Types/Types.hpp"
#include <memory>
#include <string>
#include <vector>
#include <functional>

namespace nlm {

// Memory system interface - unified base for all memory types
class IMemorySystem {
public:
    virtual ~IMemorySystem() = default;
    
    // Initialize memory system with brain reference
    virtual void initialize(Brain* brain) = 0;
    
    // Clear all memory
    virtual void clear() = 0;
    
    // Get system name
    virtual std::string getName() const = 0;
    
    // Check if memory system is enabled
    virtual bool isEnabled() const = 0;
    
    // Enable/disable memory system
    virtual void setEnabled(bool enabled) = 0;
};

// Unified working memory interface
class IWorkingMemory : public IMemorySystem {
public:
    virtual ~IWorkingMemory() = default;
    
    // Store information in working memory
    virtual void store(const std::vector<float>& pattern, float strength = 1.0f) = 0;
    
    // Store specific neuron activation
    virtual void storeToNeuron(NeuronId neuron, float activation) = 0;
    
    // Retrieve current working memory content as activity levels
    virtual std::vector<float> retrieve() const = 0;
    
    // Check if specific neuron is part of working memory
    virtual bool contains(NeuronId neuron) const = 0;
    
    // Get activation level of a specific neuron in working memory
    virtual float getNeuronActivation(NeuronId neuron) const = 0;
    
    // Get number of active memory traces
    virtual size_t getActiveTraces() const = 0;
    
    // Get capacity
    virtual size_t getCapacity() const = 0;
    virtual void setCapacity(size_t cap) = 0;
    
    // Decay rate for memory traces
    virtual float getDecayRate() const = 0;
    virtual void setDecayRate(float rate) = 0;
    
    // Get neurons currently in working memory
    virtual const std::vector<NeuronId>& getMemoryNeurons() const = 0;
};

// Unified episodic memory interface
class IEpisodicMemory : public IMemorySystem {
public:
    virtual ~IEpisodicMemory() = default;
    
    // Store a new experience
    virtual void storeEpisode(const std::vector<float>& sensoryState,
                             float reward,
                             float energy,
                             float novelty,
                             const std::vector<NeuronId>& activeNeurons,
                             const std::vector<float>& neuronActivations,
                             SimulationStep timestamp = 0) = 0;
    
    // Retrieve episodes similar to a query pattern
    virtual std::vector<const void*> retrieveSimilar(const std::vector<float>& sensoryPattern,
                                                    size_t maxResults = 5) const = 0;
    
    // Retrieve episodes from a specific time window
    virtual std::vector<const void*> retrieveTemporal(SimulationStep startTime,
                                                      SimulationStep endTime,
                                                      size_t maxResults = 10) const = 0;
    
    // Get total number of episodes
    virtual size_t getEpisodeCount() const = 0;
    
    // Get episodes for replay
    virtual std::vector<const void*> getEpisodesForReplay(size_t count) const = 0;
    
    // Enable/disable replay
    virtual void enableReplay(bool enable) = 0;
    virtual bool isReplayEnabled() const = 0;
};

// Unified associative memory interface
class IAssociativeMemory : public IMemorySystem {
public:
    virtual ~IAssociativeMemory() = default;
    
    // Create association between two patterns
    virtual void associate(const std::vector<float>& patternA,
                          const std::vector<float>& patternB,
                          float strength = 1.0f) = 0;
    
    // Retrieve patterns associated with a query
    virtual std::vector<std::vector<float>> retrieve(const std::vector<float>& queryPattern,
                                                    size_t maxResults = 5) const = 0;
    
    // Get association strength
    virtual float getAssociationStrength(const std::vector<float>& patternA,
                                        const std::vector<float>& patternB) const = 0;
    
    // Get number of associations
    virtual size_t getAssociationCount() const = 0;
};

// Unified memory manager - single entry point for all memory systems
class MemoryManager {
public:
    MemoryManager();
    ~MemoryManager();
    
    // Initialize all memory systems
    void initialize(Brain* brain);
    
    // Update all memory systems
    void update(TimestepDuration dt);
    
    // Clear all memory
    void clear();
    
    // Get working memory interface
    IWorkingMemory* getWorkingMemory() const;
    
    // Get episodic memory interface
    IEpisodicMemory* getEpisodicMemory() const;
    
    // Get associative memory interface
    IAssociativeMemory* getAssociativeMemory() const;
    
    // Enable/disable specific memory system
    void enableWorkingMemory(bool enable);
    void enableEpisodicMemory(bool enable);
    void enableAssociativeMemory(bool enable);
    
    // Get working memory statistics
    size_t getWorkingMemoryCapacity() const;
    size_t getWorkingMemoryActiveTraces() const;
    float getWorkingMemoryActivity() const;
    
    // Get episodic memory statistics
    size_t getEpisodicMemoryCount() const;
    size_t getEpisodicMemoryMaxEpisodes() const;
    float getEpisodicMemoryAverageReward() const;
    
    // Get associative memory statistics
    size_t getAssociativeMemoryCount() const;
    
    // Store current brain state as working memory entry
    void storeWorkingMemoryFromBrain();
    
    // Store current brain experience as episodic memory entry
    void storeEpisodicMemoryFromBrain(float reward = 0.0f);
    
    // Retrieve similar experiences
    std::vector<const void*> findSimilarExperiences(const std::vector<float>& sensoryPattern,
                                                   size_t maxResults = 5) const;
    
    // Get memory quality score
    double getMemoryQualityScore() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Default implementations for the interfaces
class DefaultWorkingMemory : public IWorkingMemory {
public:
    DefaultWorkingMemory();
    ~DefaultWorkingMemory() override;
    
    void initialize(Brain* brain) override;
    void store(const std::vector<float>& pattern, float strength) override;
    void storeToNeuron(NeuronId neuron, float activation) override;
    std::vector<float> retrieve() const override;
    bool contains(NeuronId neuron) const override;
    float getNeuronActivation(NeuronId neuron) const override;
    void update(TimestepDuration dt) override;
    void clear() override;
    std::string getName() const override { return "WorkingMemory"; }
    bool isEnabled() const override { return enabled_; }
    void setEnabled(bool enabled) override { enabled_ = enabled; }
    
    size_t getActiveTraces() const override;
    size_t getCapacity() const override;
    void setCapacity(size_t cap) override;
    float getDecayRate() const override;
    void setDecayRate(float rate) override;
    const std::vector<NeuronId>& getMemoryNeurons() const override;
    
    void strengthenMemory(float factor) override;
    void runCompetition() override;
    bool isWinning(NeuronId neuron) const override;
    float getMemoryActivity() const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    bool enabled_;
};

class DefaultEpisodicMemory : public IEpisodicMemory {
public:
    DefaultEpisodicMemory();
    ~DefaultEpisodicMemory() override;
    
    void initialize(Brain* brain) override;
    void storeEpisode(const std::vector<float>& sensoryState,
                     float reward,
                     float energy,
                     float novelty,
                     const std::vector<NeuronId>& activeNeurons,
                     const std::vector<float>& neuronActivations,
                     SimulationStep timestamp) override;
    std::vector<const void*> retrieveSimilar(const std::vector<float>& sensoryPattern,
                                            size_t maxResults) const override;
    std::vector<const void*> retrieveTemporal(SimulationStep startTime,
                                             SimulationStep endTime,
                                             size_t maxResults) const override;
    size_t getEpisodeCount() const override;
    std::vector<const void*> getEpisodesForReplay(size_t count) const override;
    void enableReplay(bool enable) override;
    bool isReplayEnabled() const override;
    void update(TimestepDuration dt) override;
    void clear() override;
    std::string getName() const override { return "EpisodicMemory"; }
    bool isEnabled() const override { return enabled_; }
    void setEnabled(bool enabled) override { enabled_ = enabled; }
    
    void consolidate(float relevanceThreshold) override;
    float getAverageReward() const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    bool enabled_;
    bool replayEnabled_;
};

class DefaultAssociativeMemory : public IAssociativeMemory {
public:
    DefaultAssociativeMemory();
    ~DefaultAssociativeMemory() override;
    
    void initialize(Brain* brain) override;
    void associate(const std::vector<float>& patternA,
                  const std::vector<float>& patternB,
                  float strength) override;
    std::vector<std::vector<float>> retrieve(const std::vector<float>& queryPattern,
                                            size_t maxResults) const override;
    float getAssociationStrength(const std::vector<float>& patternA,
                                const std::vector<float>& patternB) const override;
    void clear() override;
    std::string getName() const override { return "AssociativeMemory"; }
    bool isEnabled() const override { return enabled_; }
    void setEnabled(bool enabled) override { enabled_ = enabled; }
    
    size_t getAssociationCount() const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    bool enabled_;
};

} // namespace nlm