#pragma once

#include "../core/Types/Types.hpp"
#include "Neuron.hpp"
#include "../performance/MemoryPool.hpp"
#include "../performance/SIMDVectorization.hpp"
#include "../performance/SparseConnectivity.hpp"
#include "../performance/ParallelProcessing.hpp"
#include "../performance/EventQueue.hpp"

namespace nlm {

// Performance infrastructure configuration
namespace BrainPerformanceConstants {
    // Memory pool configuration
    static constexpr size_t NEURON_POOL_BLOCK_SIZE = 1024;
    static constexpr size_t SYNAPSE_POOL_BLOCK_SIZE = 2048;
    
    // SIMD configuration
    static constexpr bool SIMD_ENABLED = true;
    static constexpr size_t SIMD_BATCH_SIZE = 256;  // Minimum neurons per SIMD batch
    
    // Parallel processing configuration
    static constexpr bool PARALLEL_ENABLED = true;
    static constexpr size_t MAX_THREADS = 0;  // 0 = use hardware concurrency
    static constexpr size_t PARALLEL_BATCH_SIZE = 256;
    
    // Sparse connectivity configuration
    static constexpr bool SPARSE_ENABLED = true;
    static constexpr float CONNECTION_DENSITY = 0.1f;  // ~10% connectivity
    
    // Event queue configuration
    static constexpr bool EVENT_QUEUE_ENABLED = true;
    static constexpr size_t SPIKE_QUEUE_CAPACITY = 16384;
    static constexpr size_t DELAYED_SPIKE_BUCKETS = 1024;
    
    // Performance tuning
    static constexpr float SIMD_THRESHOLD = 64.0f;  // Neurons per batch for SIMD
    static constexpr float PARALLEL_THRESHOLD = 1024.0f;  // Neurons per batch for parallel
};

class Brain {
public:
    // Constructor
    explicit Brain(std::shared_ptr<Config> config);
    
    // Destructor
    ~Brain();
    
    // Disable copying
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    
    // Enable moving
    Brain(Brain&& other) noexcept;
    Brain& operator=(Brain&& other) noexcept;
    
    // Initialization
    bool initialize();
    
    // Step the simulation
    void step(SimulationStep currentStep);
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Region management
    RegionId addRegion(const std::string& name);
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    size_t getRegionCount() const;
    std::vector<RegionId> getRegionIds() const;
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    // Inter-region connections
    void addInterRegionConnection(RegionId source, RegionId target, float weight, Delay delay);
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // Statistics
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    float getExcitationInhibitionRatio() const;
    
    // Working memory and episodic memory access
    NeuralWorkingMemory* getWorkingMemory();
    NeuralEpisodicMemory* getEpisodicMemory();
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // Prediction system access
    PredictionSystem* getPredictionSystem();
    
    // Cognition systems access
    NeuralPlanner* getPlanner();
    ConceptFormation* getConceptFormation();
    AttentionalSelection* getAttention();
    
    // Development system access
    DevelopmentSystem* getDevelopmentSystem();
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // Neuromodulation systems access
    Dopamine* getDopamine();
    Curiosity* getCuriosity();
    Novelty* getNovelty();
    PredictionError* getPredictionErrorSignal();
    
    // Random generator access
    RandomGenerator* getRandomGenerator();
    
    // Sensory and motor access
    void receiveSensoryInput(const class SensoryInput& input);
    void injectCurrent(NeuronId neuron, MembranePotential current);
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Spike system access
    SpikeSystem* getSpikeSystem();
    const SpikeSystem* getSpikeSystem() const;
    
    // Plasticity system access
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    
    // Statistics and status
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    std::unique_ptr<class Action> produceAction();
    
    // System control
    void applyNeuromodulation(const class Neuromodulator& signal);
    void updatePlasticity();
    void develop();
    void reset();
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    
    // Status logging
    void logStatus() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// ============================================================================
// BRAIN INTERNAL CONSTANTS (Brain.hpp)
// ============================================================================
// These constants are used internally by the Brain class and related components
// for synaptic operations, spike processing, and system dynamics.

namespace BrainConstants {

// ============================================================================
// SYNAPTIC WEIGHT CONSTANTS
// ============================================================================

// Default synaptic weight bounds for stability
static constexpr float SYNAPSE_MIN_WEIGHT = -1.0f;
static constexpr float SYNAPSE_MAX_WEIGHT = 1.0f;

// Minimum absolute weight to maintain synaptic efficacy
static constexpr float SYNAPSE_MIN_ABS_WEIGHT = 0.01f;

// ============================================================================
// SPIKE PROCESSING CONSTANTS
// ============================================================================

// Synaptic current scale factor: event.weight * scale = injected current
static constexpr float SPIKE_SYNAPTIC_CURRENT_SCALE = 10.0f; // default from Brain.cpp:277

// ============================================================================
// PLACEMENT AND LIFETIMES
// ============================================================================

// Default region ID start (regions are 1-indexed)
static constexpr RegionId DEFAULT_FIRST_REGION_ID = RegionId(1);

// ============================================================================
// DEVELOPMENT AND PLASTICITY CONSTANTS
// ============================================================================

// Synaptogenesis rate multiplier during different developmental stages
static constexpr float PLASTICITY_MOD_INITIAL = 1.0f;      // Initial/High plasticity
static constexpr float PLASTICITY_MOD_CRITICAL = 0.8f;     // Critical period
static constexpr float PLASTICITY_MOD_MATURE = 0.5f;       // Maturation
static constexpr float PLASTICITY_MOD_ADULT = 0.2f;         // Stable adult

// ============================================================================
// EPISODIC MEMORY CONSTANTS
// ============================================================================

// Steps between episode storage (Brain.cpp:482)
static constexpr size_t EPISODE_STORAGE_INTERVAL = 10;

// ============================================================================
// REPLAY SYSTEM CONSTANTS
// ============================================================================

// Number of episodes to replay (Brain.cpp:543)
static constexpr size_t REPLAY_EPISODE_COUNT = 3;

// ============================================================================
// DEVELOPMENT TIMING CONSTANTS
// ============================================================================

// Development system update interval (steps)
static constexpr size_t DEVELOPMENT_UPDATE_INTERVAL = 1000; // Brain.cpp:550

// ============================================================================
// MEMORY CONSOLIDATION CONSTANTS
// ============================================================================

// Memory consolidation threshold for importance
static constexpr float MEMORY_CONSOLIDATION_THRESHOLD = 0.3f; // Brain.cpp:582

// ============================================================================
// SPATIAL TEMPORAL DYNAMICS CONSTANTS
// ============================================================================

// Neuromodulation effect on STDP learning rates
static constexpr float NEUROMODULATION_STDP_SCALE = 1.0f;

// Neuromodulation excitability scaling factor (Brain.cpp:437)
static constexpr float NEUROMODULATION_EXCITABILITY_SCALE = 0.5f;

// Plasticity weight adjustment factor (Brain.cpp:464)
static constexpr float PLASTICITY_WEIGHT_ADJUSTMENT_FACTOR = 0.001f;

// ============================================================================
// SHORT-TERM PLASTICITY CONSTANTS
// ============================================================================

// STP facilitation time constant (ms) (from Synapse.cpp:42)
static constexpr float STP_FACILITATION_TAU = 100.0f;

// STP depression time constant (ms) (from Synapse.cpp:43)
static constexpr float STP_DEPRESSION_TAU = 200.0f;

// STP maximum utilization factor
static constexpr float STP_U_MAX = 1.0f;

// ============================================================================
// SPIKE HISTORY AND RECORDING CONSTANTS
// ============================================================================

// Maximum number of spike timestamps to record per synapse/neuron
static constexpr size_t MAX_SPIKE_HISTORY_SIZE = 100;

// ============================================================================
// ADAPTATION AND REGULATORY CONSTANTS
// ============================================================================

// Spike-frequency adaptation increment per spike
static constexpr float ADAPTATION_INCREMENT = 1.0f; // Neuron.cpp:251

// Spike-frequency adaptation decay rate
static constexpr float ADAPTATION_DECAY_RATE = 0.01f; // Neuron.cpp:227

// Spike-frequency adaptation decay factor  
static constexpr float ADAPTATION_DECAY_FACTOR = 0.95f; // Neuron.cpp:228

// ============================================================================
// ATTENTION SYSTEM CONSTANTS
// ============================================================================

// Attention inhibition strength (Brain.cpp:259)
static constexpr float ATTENTION_INHIBITION_STRENGTH = 0.5f;

// Attention excitation strength (Brain.cpp:260)
static constexpr float ATTENTION_EXCITATION_STRENGTH = 1.5f;

// ============================================================================
// PLANNING SYSTEM CONSTANTS
// ============================================================================

// Minimum firing count to trigger movement action (main.cpp:710)
static constexpr size_t MIN_MOVEMENT_FIRING_COUNT = 0;

// ============================================================================
// SYNAPTIC DELAY CONSTANTS
// ============================================================================

// Default synaptic delay in steps
static constexpr Delay DEFAULT_SYNAPTIC_DELAY = 1; // Synapse.cpp:55, NeuralRegion.cpp:287

// ============================================================================
// EFFICACY CONSTANTS
// ============================================================================

// Initial synaptic efficacy for new synapses
static constexpr float SYNAPSE_INITIAL_EFFICACY = 1.0f; // Synapse.cpp:58, 230

// ============================================================================
// RUNNING SUM AND AGGREGATION CONSTANTS
// ============================================================================

// ============================================================================
// Checkpoint and Persistence
// ============================================================================

// Checkpoint compression level
static constexpr float DEFAULT_COMPRESSION_LEVEL = 0.5f;

// ============================================================================
// TEMPORAL CONSTANTS
// ============================================================================

// ============================================================================
// PHYSIOLOGICAL CONSTANTS
// ============================================================================

// ============================================================================
// METRICS AND LOGGING
// ============================================================================

// ============================================================================
// EXPERIMENT AND TESTING CONSTANTS
// ============================================================================

// ============================================================================
// SYSTEM CONSTANTS
// ============================================================================

// ============================================================================
// DEPENDENCY CONSTANTS
// ============================================================================

} // namespace BrainConstants

} // namespace nlm