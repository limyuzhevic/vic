#pragma once

/**
 * BrainCore - Optimized Neural Computation Core
 * 
 * Phase 5 implementation of high-performance neural simulation core with:
 * - Vectorized Leaky Integrate-and-Fire (LIF) dynamics using SIMD
 * - Event-driven processing for efficiency
 * - Memory pool allocation for reduced overhead
 * - Structure-of-Arrays (SoA) data layout for cache efficiency
 * - Active neuron tracking for sparse processing
 * 
 * Features:
 * - SIMD-accelerated neuron state updates
 * - Event-driven spike scheduling
 * - Memory pool management for neurons and synapses
 * - Active neuron bitmask for event-driven processing
 * - Performance monitoring and statistics
 * 
 * Biological motivation:
 * - Event-driven processing mimics sparse neural activity
 * - Vectorized operations leverage CPU parallel processing
 * - Memory pools reduce allocation overhead like biological systems
 */

#include "../core/Types/Types.hpp"
#include "../performance/MemoryPool.hpp"
#include "../performance/SIMDVectorization.hpp"
#include "../performance/EventQueue.hpp"
#include <bitset>
#include <atomic>
#include <unordered_set>
#include <vector>
#include <chrono>

namespace nlm {

/**
 * NeuronState SoA (Structure-of-Arrays) layout for cache efficiency
 * 
 * Groups all neurons' state variables contiguously rather than having
 * each neuron's state scattered in memory (AoS - Array-of-Structs).
 * This improves cache locality when processing many neurons.
 */
class NeuronStateSOA {
public:
    // LIF neuron parameters (fixed per neuron)
    std::vector<float> membranePotential;     // Current membrane potential (mV)
    std::vector<float> restingPotential;     // Resting potential (mV)
    std::vector<float> threshold;            // Firing threshold (mV)
    std::vector<float> resetPotential;       // Post-spike reset value (mV)
    std::vector<float> leakConductance;       // Leak conductance (nS)
    std::vector<float> synapseConductance;    // Synaptic conductance (nS)
    std::vector<float> firingRate;           // Current firing rate (Hz)
    std::vector<uint8_t> firingState;        // Firing state (FiringState enum)
    std::vector<uint32_t> refractoryRemaining; // Steps remaining in refractory period
    std::vector<uint32_t> refractoryPeriod;   // Total refractory period (steps)
    std::vector<float> adaptationVariable;   // For spike-frequency adaptation
    std::vector<float> lastSpikeTime;        // Timestamp of last spike (-1 if none)
    
    // Per-neuron current injection (hot path, needs to be quickly accessible)
    std::vector<float> synapticInput;
    
    // Metadata
    std::vector<NeuronId> neuronIds;          // Neuron identifiers
    std::vector<NeuronType> neuronTypes;      // Neuron type
    std::vector<RegionId> regionIds;          // Region assignment
    std::vector<PopulationId> populationIds;  // Population assignment
    
    // Performance: batch processing iterators
    size_t begin() const { return 0; }
    size_t end() const { return neuronIds.size(); }
    size_t size() const { return neuronIds.size(); }
    
    void reserve(size_t count) {
        membranePotential.reserve(count);
        restingPotential.reserve(count);
        threshold.reserve(count);
        resetPotential.reserve(count);
        leakConductance.reserve(count);
        synapseConductance.reserve(count);
        firingRate.reserve(count);
        firingState.reserve(count);
        refractoryRemaining.reserve(count);
        refractoryPeriod.reserve(count);
        adaptationVariable.reserve(count);
        lastSpikeTime.reserve(count);
        synapticInput.reserve(count);
        neuronIds.reserve(count);
        neuronTypes.reserve(count);
        regionIds.reserve(count);
        populationIds.reserve(count);
    }
    
    void resize(size_t count) {
        membranePotential.resize(count);
        restingPotential.resize(count);
        threshold.resize(count);
        resetPotential.resize(count);
        leakConductance.resize(count);
        synapseConductance.resize(count);
        firingRate.resize(count);
        firingState.resize(count);
        refractoryRemaining.resize(count);
        refractoryPeriod.resize(count);
        adaptationVariable.resize(count);
        lastSpikeTime.resize(count);
        synapticInput.resize(count);
        neuronIds.resize(count);
        neuronTypes.resize(count);
        regionIds.resize(count);
        populationIds.resize(count);
    }
};

/**
 * SynapseState SoA layout for efficient synapse processing
 */
class SynapseStateSOA {
public:
    std::vector<uint64_t> sourceNeurons;
    std::vector<uint64_t> destinationNeurons;
    std::vector<float> weights;
    std::vector<uint32_t> delays;
    std::vector<uint8_t> synapseTypes;         // SynapseType enum
    std::vector<uint8_t> plasticityFlags;     // Packed plasticity flags
    
    // Short-term plasticity
    std::vector<float> shortTermDepression;
    std::vector<float> shortTermFacilitation;
    std::vector<float> lastPreSpikeTime;
    std::vector<float> lastPostSpikeTime;
    
    // Plasticity traces
    std::vector<float> eligibilityTrace;
    std::vector<float> synapticEfficacy;
    
    size_t size() const { return sourceNeurons.size(); }
    void reserve(size_t count) {
        sourceNeurons.reserve(count);
        destinationNeurons.reserve(count);
        weights.reserve(count);
        delays.reserve(count);
        synapseTypes.reserve(count);
        plasticityFlags.reserve(count);
        shortTermDepression.reserve(count);
        shortTermFacilitation.reserve(count);
        lastPreSpikeTime.reserve(count);
        lastPostSpikeTime.reserve(count);
        eligibilityTrace.reserve(count);
        synapticEfficacy.reserve(count);
    }
    
    void resize(size_t count) {
        sourceNeurons.resize(count);
        destinationNeurons.resize(count);
        weights.resize(count);
        delays.resize(count);
        synapseTypes.resize(count);
        plasticityFlags.resize(count);
        shortTermDepression.resize(count);
        shortTermFacilitation.resize(count);
        lastPreSpikeTime.resize(count);
        lastPostSpikeTime.resize(count);
        eligibilityTrace.resize(count);
        synapticEfficacy.resize(count);
    }
};

/**
 * Event-driven spike delivery system for efficient synaptic transmission
 */
class EventDrivenSpikeDelivery {
public:
    EventDrivenSpikeDelivery() = default;
    ~EventDrivenSpikeDelivery() = default;
    
    /**
     * Deliver spikes for a batch of neurons
     * Uses event-driven approach: only process neurons that actually fired
     */
    void deliverSpikes(const std::vector<NeuronId>& firedNeuronIds,
                      const std::vector<float>& synapticInputs,
                      const SynapseStateSOA& synapses,
                      float timestep,
                      NeuronStateSOA& neuronStates,
                      std::vector<size_t>& neuronIndices);
    
    /**
     * Get active neurons for processing (event-driven approach)
     */
    std::vector<size_t> getActiveNeuronIndices(const NeuronStateSOA& states,
                                              float minActivationThreshold = 1.0f) const;
    
    /**
     * Process delayed spikes from event queue
     */
    void processDelayedSpikes(const DelayedSpikeQueue& spikeQueue,
                             const SynapseStateSOA& synapses,
                             float timestep,
                             NeuronStateSOA& neuronStates,
                             std::vector<size_t>& neuronIndices);
    
    /**
     * Track neuron activity for event-driven processing
     */
    void updateActiveNeuronSet(const std::vector<NeuronId>& activeNeurons,
                              std::unordered_set<NeuronId>& activeSet) const;
};

/**
 * ActiveNeuronTracker - Tracks which neurons are active for event-driven processing
 * 
 * Uses multiple strategies for optimal performance:
 * - Bitmask for small populations (<64 neurons)
 * - Hash set for larger populations
 * - Priority queue for neurons needing immediate processing
 */
class ActiveNeuronTracker {
public:
    ActiveNeuronTracker(size_t maxNeurons);
    ~ActiveNeuronTracker() = default;
    
    /**
     * Mark neuron as active
     */
    void activate(NeuronId neuronId);
    
    /**
     * Remove neuron from active set
     */
    void deactivate(NeuronId neuronId);
    
    /**
     * Check if neuron is active
     */
    bool isActive(NeuronId neuronId) const;
    
    /**
     * Get all active neurons
     */
    std::vector<NeuronId> getActiveNeurons() const;
    
    /**
     * Get neuron indices for batch processing (optimized for SIMD)
     */
    std::vector<size_t> getActiveIndices(const std::vector<NeuronId>& neuronIds) const;
    
    /**
     * Clear all active neurons
     */
    void clear();
    
    /**
     * Get number of active neurons
     */
    size_t activeCount() const;
    
    /**
     * Transition to event-driven mode based on activity level
     */
    void updateProcessingMode();
    
    enum class ProcessingMode {
        DENSE,      // Process all neurons (traditional approach)
        EVENT_DRIVEN, // Process only active neurons (optimized)
        HYBRID     // Adaptive based on activity
    };
    
    ProcessingMode getProcessingMode() const { return mode_; }
    
private:
    ProcessingMode mode_;
    size_t maxNeurons_;
    
    // Dense storage for small populations (bitmask)
    std::bitset<64> denseActiveSet_;
    
    // Hash set for larger populations
    std::unordered_set<NeuronId> activeSet_;
    
    // Priority queue for neurons requiring immediate attention
    std::vector<std::pair<NeuronId, float>> priorityNeurons_;
    
    // Threshold for switching between dense and event-driven modes
    static constexpr size_t DENSE_MODE_THRESHOLD = 32;
};

/**
 * MemoryPoolManager - Manages memory pools for neurons and synapses
 * 
 * Provides pre-allocated pools to reduce allocation overhead and improve cache locality.
 */
class MemoryPoolManager {
public:
    MemoryPoolManager(size_t initialNeurons, size_t initialSynapses);
    ~MemoryPoolManager() = default;
    
    /**
     * Allocate neuron from pool
     */
    NeuronId allocateNeuron(NeuronType type, RegionId region, PopulationId population);
    
    /**
     * Deallocate neuron to pool
     */
    void deallocateNeuron(NeuronId neuronId);
    
    /**
     * Allocate synapse from pool
     */
    SynapseId allocateSynapse(NeuronId source, NeuronId destination);
    
    /**
     * Deallocate synapse to pool
     */
    void deallocateSynapse(SynapseId synId);
    
    /**
     * Get neuron state from SoA storage by index
     */
    NeuronState getNeuronState(NeuronIndex index) const;
    
    /**
     * Set neuron state in SoA storage by index
     */
    void setNeuronState(NeuronIndex index, const NeuronState& state);
    
    /**
     * Get synapse state from SoA storage by index
     */
    SynapseState getSynapseState(SynapseIndex index) const;
    
    /**
     * Set synapse state in SoA storage by index
     */
    void setSynapseState(SynapseIndex index, const SynapseState& state);
    
    /**
     * Get SoA neuron storage
     */
    const NeuronStateSOA& getNeuronStates() const { return neuronStates_; }
    
    /**
     * Get SoA synapse storage
     */
    const SynapseStateSOA& getSynapseStates() const { return synapseStates_; }
    
    /**
     * Get neuron SoA for writing
     */
    NeuronStateSOA& getNeuronStates() { return neuronStates_; }
    
    /**
     * Get synapse SoA for writing
     */
    SynapseStateSOA& getSynapseStates() { return synapseStates_; }
    
    /**
     * Map neuron ID to index for fast lookup
     */
    NeuronIndex getNeuronIndex(NeuronId neuronId) const;
    
    /**
     * Map synapse ID to index for fast lookup
     */
    SynapseIndex getSynapseIndex(SynapseId synId) const;
    
    /**
     * Map index to neuron ID
     */
    NeuronId getNeuronId(NeuronIndex index) const;
    
    /**
     * Map index to synapse ID
     */
    SynapseId getSynapseId(SynapseIndex index) const;
    
    /**
     * Get total neuron count
     */
    size_t getNeuronCount() const { return neuronStates_.size(); }
    
    /**
     * Get total synapse count
     */
    size_t getSynapseCount() const { return synapseStates_.size(); }
    
    /**
     * Reserve space for neurons and synapses
     */
    void reserve(size_t neurons, size_t synapses);
    
    /**
     * Get statistics about memory usage
     */
    struct MemoryStats {
        size_t totalMemory;          // Total memory usage (bytes)
        size_t neuronMemory;         // Memory for neuron states
        size_t synapseMemory;        // Memory for synapse states
        size_t allocatedNeurons;     // Number of allocated neurons
        size_t allocatedSynapses;    // Number of allocated synapses
        size_t freeNeurons;          // Number of free neuron slots
        size_t freeSynapses;         // Number of free synapse slots
    };
    
    MemoryStats getMemoryStats() const;
    
private:
    NeuronStateSOA neuronStates_;
    SynapseStateSOA synapseStates_;
    
    // Maps between IDs and indices for fast lookup
    std::unordered_map<NeuronId, NeuronIndex> neuronIdToIndex_;
    std::unordered_map<SynapseId, SynapseIndex> synapseIdToIndex_;
    std::vector<NeuronId> neuronIds_;
    std::vector<SynapseId> synapseIds_;
    
    // Memory pools for allocation
    NeuronPool neuronPool_;
    SynapsePool synapsePool_;
};

/**
 * PerformanceMonitor - Tracks performance metrics for BrainCore
 * 
 * Collects statistics about processing time, cache misses, SIMD utilization,
 * and other performance metrics to identify bottlenecks.
 */
class PerformanceMonitor {
public:
    PerformanceMonitor();
    ~PerformanceMonitor() = default;
    
    /**
     * Start timing a processing block
     */
    void startTimer(const std::string& name);
    
    /**
     * End timing and record the result
     */
    void endTimer(const std::string& name);
    
    /**
     * Record cache miss count
     */
    void recordCacheMisses(size_t misses) { cacheMisses_ += misses; }
    
    /**
     * Record SIMD operations count
     */
    void recordSIMDOperations(size_t ops) { simdOps_ += ops; }
    
    /**
     * Record spike processing statistics
     */
    void recordSpikes(size_t total, size_t active, size_t firing);
    
    /**
     * Get performance statistics
     */
    struct PerformanceStats {
        size_t totalNeurons;
        size_t activeNeurons;
        size_t firingNeurons;
        size_t totalSpikes;
        float averageFiringRate;
        double processingTime;  // nanoseconds
        size_t simdOperations;
        size_t cacheMisses;
        
        // Per-operation timing
        std::unordered_map<std::string, double> operationTimes;  // nanoseconds
    };
    
    PerformanceStats getStats() const;
    
    /**
     * Reset all statistics
     */
    void reset();
    
    /**
     * Check if performance monitoring is enabled
     */
    bool isEnabled() const { return enabled_; }
    
    /**
     * Enable/disable performance monitoring
     */
    void setEnabled(bool enabled) { enabled_ = enabled; }
    
private:
    bool enabled_;
    
    // Performance counters
    size_t totalNeurons_;
    size_t activeNeurons_;
    size_t firingNeurons_;
    size_t totalSpikes_;
    float averageFiringRate_;
    size_t simdOps_;
    size_t cacheMisses_;
    
    // Timing statistics
    std::chrono::high_resolution_clock::time_point startTime_;
    std::chrono::nanoseconds totalProcessingTime_;
    std::unordered_map<std::string, std::chrono::nanoseconds> operationTimes_;
};

/**
 * BrainCore - Optimized neural computation core for LIF dynamics
 * 
 * Replaces the traditional Brain class's per-step neural processing with
 * an event-driven, SIMD-optimized implementation that drastically reduces
 * computational complexity from O(n²) to near O(m) where m << n is the
 * number of active neurons.
 */
class BrainCore {
public:
    BrainCore();
    ~BrainCore();
    
    // Disable copying, enable moving
    BrainCore(const BrainCore&) = delete;
    BrainCore& operator=(const BrainCore&) = delete;
    BrainCore(BrainCore&&) noexcept;
    BrainCore& operator=(BrainCore&&) noexcept;
    
    /**
     * Initialize brain core with configuration
     */
    bool initialize(const std::shared_ptr<Config>& config);
    
    /**
     * Process a single simulation step with event-driven optimization
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * Update neuron states using vectorized SIMD operations
     */
    void updateNeuronStates(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * Deliver synaptic inputs from fired neurons (event-driven)
     */
    void deliverSynapticInputs(const std::vector<NeuronId>& firedNeurons,
                              Timestamp currentTime);
    
    /**
     * Apply plasticity rules to synapses
     */
    void applyPlasticityRules(SimulationStep currentStep, Timestamp currentTime,
                             float neuromodulationFactor = 1.0f);
    
    /**
     * Process pending delayed spike events
     */
    void processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * Get neuron state by index (SoA access)
     */
    NeuronState getNeuronState(NeuronIndex index) const;
    
    /**
     * Set neuron state by index (SoA access)
     */
    void setNeuronState(NeuronIndex index, const NeuronState& state);
    
    /**
     * Inject current into a neuron
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * Get list of currently active neurons
     */
    std::vector<NeuronId> getActiveNeurons() const;
    
    /**
     * Get neuron that fired in current step
     */
    std::vector<NeuronId> getFiringNeurons() const;
    
    /**
     * Get statistics about processing performance
     */
    PerformanceMonitor::PerformanceStats getPerformanceStats() const { return monitor_.getStats(); }
    
    /**
     * Reset brain core to initial state
     */
    void reset();
    
    /**
     * Add a neuron to the core
     */
    NeuronId addNeuron(NeuronType type, RegionId region, PopulationId population);
    
    /**
     * Remove a neuron from the core
     */
    void removeNeuron(NeuronId neuronId);
    
    /**
     * Add a synapse between neurons
     */
    SynapseId addSynapse(NeuronId source, NeuronId destination, 
                       float weight = 0.0f, Delay delay = 1,
                       bool isExcitatory = true);
    
    /**
     * Remove a synapse
     */
    void removeSynapse(SynapseId synId);
    
    /**
     * Get all neurons
     */
    const std::vector<NeuronId>& getNeuronIds() const { return memoryPool_.getNeuronIds(); }
    
    /**
     * Get all synapses
     */
    const std::vector<SynapseId>& getSynapseIds() const { return memoryPool_.getSynapseIds(); }
    
    /**
     * Get neuron index from neuron ID
     */
    NeuronIndex getNeuronIndex(NeuronId neuronId) const { return memoryPool_.getNeuronIndex(neuronId); }
    
    /**
     * Get synapse index from synapse ID
     */
    SynapseIndex getSynapseIndex(SynapseId synId) const { return memoryPool_.getSynapseIndex(synId); }
    
    /**
     * Get neuron ID from index
     */
    NeuronId getNeuronId(NeuronIndex index) const { return memoryPool_.getNeuronId(index); }
    
    /**
     * Get synapse ID from index
     */
    SynapseId getSynapseId(SynapseIndex index) const { return memoryPool_.getSynapseId(index); }
    
    /**
     * Get timestep
     */
    TimestepDuration getTimestep() const { return timestep_; }
    
    /**
     * Set timestep
     */
    void setTimestep(TimestepDuration dt) { timestep_ = dt; }

private:
    struct Impl;
    Impl* pImpl;
};

// Global function to convert between old Brain and new BrainCore interface
// This ensures backward compatibility during migration
class BrainCoreAdapter {
public:
    BrainCoreAdapter(std::shared_ptr<BrainCore> core) : core_(core) {}
    
    // Delegate to BrainCore for all operations
    void step(SimulationStep step) {
        core_->updateNeuronStates(step, static_cast<Timestamp>(step) * core_->getTimestep());
    }
    
    void step(SimulationStep step, Timestamp time) {
        core_->step(step, time);
    }
    
    void receiveSensoryInput(const SensoryInput& input) {
        // Delegate to appropriate neurons
        for (auto& neuronId : core_->getActiveNeurons()) {
            // Inject appropriate current based on input
            core_->injectCurrent(neuronId, input.getValue());
        }
    }
    
    // ... other adapter methods
    
private:
    std::shared_ptr<BrainCore> core_;
};

} // namespace nlm