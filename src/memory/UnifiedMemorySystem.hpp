// ============================================================================
// Unified Memory Management System
// ============================================================================

#pragma once

#include "core/Types/Types.hpp"
#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include <vector>
#include <deque>
#include <memory>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <chrono>

namespace nlm {

// ============================================================================
// MEMORY BASE CLASSES
// ============================================================================

class MemoryBase {
public:
    virtual ~MemoryBase() = default;
    
    virtual void initialize(Brain* brain) = 0;
    virtual void clear() = 0;
    virtual bool isInitialized() const = 0;
    virtual void logStatus() const = 0;
    
protected:
    MemoryBase() = default;
};

// ============================================================================
// NEURAL WORKING MEMORY - Transposed for Unified Integration
// ============================================================================

class NeuralWorkingMemory : public MemoryBase {
public:
    NeuralWorkingMemory();
    ~NeuralWorkingMemory() override;
    
    void initialize(Brain* brain) override;
    void clear() override;
    bool isInitialized() const override { return brain_ != nullptr; }
    void logStatus() const override;
    
    // Core working memory functionality
    void store(const std::vector<float>& pattern, float strength);
    std::vector<float> retrieve() const;
    bool contains(NeuronId neuron) const;
    float getNeuronActivation(NeuronId neuron) const;
    void update(TimestepDuration dt);
    
    // Management
    void storeToNeuron(NeuronId neuron, float activation);
    void strengthenMemory(float factor);
    void createRecurrentConnection(NeuronId from, NeuronId to, float strength);
    void updateRecurrentConnections();
    void decayWeakTraces();
    void runCompetition();
    bool isWinning(NeuronId neuron) const;
    float getMemoryActivity() const;
    size_t getActiveTraceCount() const;
    
    // Integration with brain systems
    void integrateWithPlasticity(float plasticityInfluence);
    void integrateWithNeuromodulation(float dopamineLevel);
    
    // Memory consolidation
    void consolidateWithEpisodicMemory(NeuralEpisodicMemory* episodicMem);
    void replayForConsolidation();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    bool initialized_;
};

// ============================================================================
// NEURAL EPISODIC MEMORY - Enhanced Integration
// ============================================================================

struct EpisodicMemoryItem {
    SimulationStep timestamp;
    std::chrono::system_clock::time_point timePoint;
    
    // Sensory state at time of experience (encoded as activity pattern)
    std::vector<float> sensoryState;
    std::vector<float> internalState;
    
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
    float predictionError;
    
    // Consequence of action
    std::vector<float> resultingSensoryState;
    float resultingReward;
    
    // Associated neural activity pattern (who was firing)
    std::vector<NeuronId> activeNeurons;
    std::vector<float> neuronActivations;
    
    // Time since this episode
    SimulationStep age;
    
    // Consolidation state
    bool isConsolidated;
    SimulationStep consolidationTime;
    
    // Memory strength (after consolidation)
    float strength;
    
    EpisodicMemoryItem()
        : timestamp(0), positionX(0), positionY(0), orientation(0)
        , action(ActionType::Wait)
        , reward(0), energy(0), novelty(0), predictionError(0)
        , resultingReward(0), age(0), isConsolidated(false)
        , consolidationTime(0), strength(1.0f) {}
};

class NeuralEpisodicMemory : public MemoryBase {
public:
    NeuralEpisodicMemory();
    ~NeuralEpisodicMemory() override;
    
    void initialize(Brain* brain) override;
    void clear() override;
    bool isInitialized() const override { return brain_ != nullptr; }
    void logStatus() const override;
    
    // Core episodic memory functionality
    void storeEpisode(const EpisodicMemoryItem& episode);
    std::vector<const EpisodicMemoryItem*> retrieveSimilar(const std::vector<float>& sensoryPattern,
                                                           size_t maxResults = 5) const;
    std::vector<const EpisodicMemoryItem*> retrieveTemporal(SimulationStep startTime,
                                                           SimulationStep endTime,
                                                           size_t maxResults = 10) const;
    std::vector<const EpisodicMemoryItem*> retrieveByLocation(float x, float y,
                                                             float radius,
                                                             size_t maxResults = 5) const;
    std::vector<const EpisodicMemoryItem*> retrieveAfterAction(ActionType action,
                                                               size_t maxResults = 5) const;
    void replayEpisode(const EpisodicMemoryItem* episode);
    void updateRelevance(SimulationStep episodeIndex, float relevanceDelta);
    const EpisodicMemoryItem* getEpisode(size_t index) const;
    size_t getEpisodeCount() const { return episodes_.size(); }
    std::vector<const EpisodicMemoryItem*> getRecentEpisodes(size_t count) const;
    float getAverageReward() const;
    void consolidate(float relevanceThreshold);
    void enableReplay(bool enable) { replayEnabled_ = enable; }
    bool isReplayEnabled() const { return replayEnabled_; }
    std::vector<const EpisodicMemoryItem*> getEpisodesForReplay(size_t count) const;
    void replaySequence(const std::vector<size_t>& episodeIds);
    
    // Integration with working memory
    void integrateWithWorkingMemory(NeuralWorkingMemory* workingMem);
    void processIncomingReplay(const std::vector<NeuronId>& replayPattern);
    
    // Memory management
    void cleanupExpiredEpisodes(SimulationStep maxAge);
    void compressMemory(size_t targetSize);
    
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
    std::deque<EpisodicMemoryItem> episodes_;
    std::vector<EpisodicMemoryItem> longTermStorage_;
    size_t maxEpisodicEpisodes_;
    bool replayEnabled_;
    SimulationStep timeStep_;
    float consolidationThreshold_;
};

// ============================================================================
// NEURAL ASSOCIATIVE MEMORY - Enhanced Integration
// ============================================================================

class NeuralAssociativeMemory : public MemoryBase {
public:
    NeuralAssociativeMemory();
    ~NeuralAssociativeMemory() override;
    
    void initialize(Brain* brain) override;
    void clear() override;
    bool isInitialized() const override { return brain_ != nullptr; }
    void logStatus() const override;
    
    // Core associative memory functionality
    void associate(const std::vector<float>& patternA,
                   const std::vector<float>& patternB,
                   float strength = 1.0f);
    void associateFromExperience(const EpisodicMemoryItem& episode);
    std::vector<std::vector<float>> retrieve(const std::vector<float>& queryPattern,
                                             size_t maxResults = 5) const;
    float getAssociationStrength(const std::vector<float>& patternA,
                                const std::vector<float>& patternB) const;
    void updateAssociation(const std::vector<float>& patternA,
                          const std::vector<float>& patternB,
                          float delta);
    std::vector<std::vector<float>> spreadActivation(const std::vector<float>& cuePattern,
                                                     size_t steps = 2) const;
    size_t getAssociationCount() const { return associations_.size(); }
    
    // Integration with neural plasticity
    void integrateWithSTDP(STDP* stdp);
    void integrateWithHebbian(Hebbian* hebbian);
    void strengthenAssociations(float factor);
    void decayAssociations(float factor);
    
    // Memory management
    void cleanupWeakAssociations(float minStrength);
    void compressAssociations();
    
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
    size_t maxAssociations_;
    float minAssociationStrength_;
};

// ============================================================================
// MEMORY SYSTEM MANAGER - Unified Interface
// ============================================================================

class MemorySystemManager {
public:
    MemorySystemManager();
    ~MemorySystemManager();
    
    // Initialize all memory systems
    bool initialize(Brain* brain);
    
    // Core memory operations
    void updateAll(TimestepDuration dt);
    void consolidateAll();
    void clearAll();
    
    // Memory system access
    NeuralWorkingMemory* getWorkingMemory() { return workingMemory_.get(); }
    NeuralEpisodicMemory* getEpisodicMemory() { return episodicMemory_.get(); }
    NeuralAssociativeMemory* getAssociativeMemory() { return associativeMemory_.get(); }
    
    // Integration utilities
    void enableReplay(bool enable);
    void setReplayInterval(size_t steps);
    void enableConsolidation(bool enable);
    void setConsolidationInterval(size_t steps);
    
    // Statistics and monitoring
    void logStatus() const;
    float getSystemIntegrity() const;
    size_t getTotalMemoryUsage() const;
    float getMemoryEfficiency() const;
    
    // Configuration
    void configureMemorySystem(const std::string& configFile);
    void setMemoryCapacity(size_t workingMemSize, size_t episodicMemSize);
    void setAssociationParameters(float minStrength, size_t maxAssociations);
    
    // Memory inspection
    SimulationStep getCurrentTimeStep() const { return currentTimeStep_; }
    bool isReplayEnabled() const { return replayEnabled_; }
    bool isConsolidationEnabled() const { return consolidationEnabled_; }
    
private:
    // Memory system lifecycle
    void createMemorySystems();
    void destroyMemorySystems();
    void initializeMemorySystems();
    
    // Integration between systems
    void integrateWorkingEpisodicMemory();
    void integrateEpisodicAssociativeMemory();
    void integrateAllSystems();
    
    // System management
    void updateReplaySchedule();
    void updateConsolidationSchedule();
    
    // Cleanup and maintenance
    void cleanupExpiredMemory();
    void optimizeMemoryUsage();
    
    // Memory system instances
    std::unique_ptr<NeuralWorkingMemory> workingMemory_;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory_;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory_;
    
    // Integration systems
    Brain* brain_;
    
    // System state
    SimulationStep currentTimeStep_;
    size_t stepsSinceLastReplay_;
    size_t stepsSinceLastConsolidation_;
    size_t replayInterval_;
    size_t consolidationInterval_;
    bool replayEnabled_;
    bool consolidationEnabled_;
    bool systemsInitialized_;
    
    // Configuration
    size_t workingMemoryCapacity_;
    size_t episodicMemoryCapacity_;
    float associationMinStrength_;
    size_t maxAssociations_;
    float consolidationThreshold_;
};

// Global memory system instance
extern std::unique_ptr<MemorySystemManager> gMemorySystem;

// Convenience functions
void initializeMemorySystem(Brain* brain);
void shutdownMemorySystem();
NeuralWorkingMemory* getWorkingMemory();
NeuralEpisodicMemory* getEpisodicMemory();
NeuralAssociativeMemory* getAssociativeMemory();

} // namespace nlm

// ============================================================================
// End of Unified Memory Management System
// ============================================================================
