#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../performance/MemoryPool.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <mutex>

namespace nlm {

// Forward declarations
class Config;
class RandomGenerator;
class SimulationClock;
class Logger;
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class NeuralAssociativeMemory;
class PredictionSystem;
class NeuralPlanner;
class ConceptFormation;
class AttentionalSelection;
class DevelopmentSystem;
class Dopamine;
class Curiosity;
class Novelty;
class PredictionError;

// Inter-regional connection (long-range connectivity)
struct InterRegionConnection {
    RegionId sourceRegion;
    RegionId targetRegion;
    float weight;
    Delay delay;
    PlasticityFlags plasticityFlags;
    
    InterRegionConnection()
        : sourceRegion(), targetRegion(), weight(0.0f), delay(1), plasticityFlags() {}
    
    InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
        : sourceRegion(src), targetRegion(tgt), weight(w), delay(d), plasticityFlags() {}
};

// Object pool for Neuron optimization
using NeuronPool = MemoryPool<Neuron>;

// Object pool for Synapse optimization  
using SynapsePool = MemoryPool<Synapse>;

// Enhanced ConfigEntry with reference counting
struct ConfigEntryWithRefCount {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::atomic<size_t> refCount{0};
    
    ConfigEntryWithRefCount() : key(), value(), source(ConfigSource::Default), description(), refCount(0) {}
    ConfigEntryWithRefCount(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), refCount(0) {}
    
    void addRef() { refCount.fetch_add(1, std::memory_order_relaxed); }
    bool release() { return refCount.fetch_sub(1, std::memory_order_relaxed) == 1; }
};

// Cache entry for lazy loading
struct CacheEntry {
    std::chrono::time_point<std::chrono::steady_clock> timestamp;
    std::vector<uint8_t> data;
    bool valid;
    size_t accessCount;
    
    CacheEntry() : timestamp(), valid(false), accessCount(0) {}
};

// Performance optimization structures
struct BrainPerformanceOptimizations {
    // Object pools for memory efficiency
    std::unique_ptr<NeuronPool> neuronPool;
    std::unique_ptr<SynapsePool> synapsePool;
    std::unique_ptr<MemoryPool<ConfigEntryWithRefCount>> configEntryPool;
    
    // SoA (Structure of Arrays) for better cache locality
    struct alignas(64) NeuralStateSoA {
        std::vector<float> membranePotential;
        std::vector<float> restingPotential;
        std::vector<float> threshold;
        std::vector<float> resetPotential;
        std::vector<float> leakConductance;
        std::vector<float> synapseInput;
        std::vector<uint8_t> firingState;
        std::vector<uint32_t> refractoryRemaining;
        std::vector<uint32_t> refractoryPeriod;
        std::vector<float> lastSpikeTime;
        std::vector<uint64_t> neuronType;
        
        void initialize(size_t size);
        void clear();
        size_t memoryUsage() const;
    };
    
    // Cache for frequently accessed data
    std::unordered_map<std::string, CacheEntry> dataCache;
    std::mutex cacheMutex;
    static constexpr size_t MAX_CACHE_SIZE = 1024;
    
    // Performance metrics
    struct PerformanceMetrics {
        size_t totalMemoryUsage;
        size_t objectsAllocated;
        size_t objectsDeallocated;
        double averageAllocationTime;
        double cacheHitRate;
        uint64_t lastCheckpointSize;
    } metrics;
    
    // Lazy loading flags
    bool lazyLoadingEnabled;
    bool cachingEnabled;
    bool objectPoolingEnabled;
    
    // Resource management
    std::atomic<size_t> activeAllocations;
    std::mutex allocationMutex;
    
    // Statistics
    struct Statistics {
        uint64_t totalAllocations;
        uint64_t totalDeallocations;
        uint64_t cacheHits;
        uint64_t cacheMisses;
        uint64_t checkpointSaves;
        uint64_t checkpointLoads;
        std::chrono::steady_clock::time_point lastUpdate;
    } stats;
};

// Brain: The central coordinator of the neural system
// Implements real spiking neural computation with event-driven dynamics
// and integrated memory, prediction, cognition, and neuromodulation systems

class Brain {
public:
    // Create brain with configuration
    explicit Brain(std::shared_ptr<Config> config);
    
    ~Brain();
    
    // Disable copying, enable moving
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    // Initialize brain with configuration
    bool initialize();
    
    // Main simulation step
    void step(SimulationStep currentStep);
    
    // Simulation time step
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Receive sensory input from environment
    // Injects current into sensory neurons based on input pattern
    void receiveSensoryInput(const class SensoryInput& input);
    
    // Inject current directly into a specific neuron
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    // Inject current into all neurons of a specific type
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Spike system access
    SpikeSystem* getSpikeSystem();
    const SpikeSystem* getSpikeSystem() const;
    
    // Plasticity system access
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    
    // Statistics
    float getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    
    // Produce motor/action output based on motor neuron activity
    std::unique_ptr<class Action> produceAction();
    
    // Apply neuromodulatory signals
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    // Update plasticity rules (called automatically in step)
    void updatePlasticity();
    
    // Apply developmental changes (called automatically in step)
    void develop();
    
    // Reset brain state
    void reset();
    
    // Save brain state to file (checkpointing)
    bool save(const std::string& filepath) const;
    
    // Load brain state from file
    bool load(const std::string& filepath);
    
    // Region management
    RegionId addRegion(const std::string& name = "");
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    size_t getRegionCount() const;
    std::vector<RegionId> getRegionIds() const;
    
    // Get all regions
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    // Inter-region connection management
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // Global statistics
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    
    // ========== MEMORY SYSTEMS ==========
    
    // Working memory - transient active information
    NeuralWorkingMemory* getWorkingMemory();
    
    // Episodic memory - experience storage
    NeuralEpisodicMemory* getEpisodicMemory();
    
    // Associative memory - pattern associations
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // ========== PREDICTION SYSTEM ==========
    
    // Prediction system for sensory prediction and error computation
    PredictionSystem* getPredictionSystem();
    
    // ========== COGNITION SYSTEMS ==========
    
    // Neural planner for action planning
    NeuralPlanner* getPlanner();
    
    // Concept formation for pattern discovery
    ConceptFormation* getConceptFormation();
    
    // Attentional selection for focus
    AttentionalSelection* getAttention();
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    DevelopmentSystem* getDevelopmentSystem();
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    // Dopamine - reward and reinforcement
    Dopamine* getDopamine();
    
    // Curiosity - exploration motivation
    Curiosity* getCuriosity();
    
    // Novelty - novelty detection
    Novelty* getNovelty();
    
    // Prediction error signal
    PredictionError* getPredictionErrorSignal();
    
// Get current configuration
    std::shared_ptr<const Config> getConfig() const;

    // Get random generator
    RandomGenerator* getRandomGenerator();

    // Performance optimization interfaces
    void enableObjectPooling(bool enable);
    void enableCaching(bool enable);
    void enableLazyLoading(bool enable);
    void enableSoA(bool enable);
    
    // Performance monitoring
    BrainPerformanceOptimizations::PerformanceMetrics getPerformanceMetrics() const;
    size_t getObjectPoolMemoryUsage() const;
    size_t getCacheMemoryUsage() const;
    
    // Batch operations for performance
    void batchInitializeNeurons(const std::vector<NeuronId>& neuronIds);
    void batchCreateSynapses(const std::vector<std::pair<NeuronId, NeuronId>>& connections);
    void batchUpdateWeights(const std::vector<std::pair<SynapseId, float>>& weightUpdates);

    // Logging
    void logStatus() const;
    void logPerformanceStats() const;

private:
    struct Impl;
    Impl* pImpl;
    
    // Performance optimization instance
    std::unique_ptr<BrainPerformanceOptimizations> performanceOpts;
};

} // namespace nlm
