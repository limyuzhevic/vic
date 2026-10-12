#pragma once

/**
 * OptimizedBrain - High-performance neural simulation brain
 * 
 * Phase 5 introduces significant performance optimizations:
 * 
 * 1. Event-Driven Processing
 *    - Only active neurons consume computation
 *    - Efficient spike event queues
 *    - Time-bucketed delayed spikes
 * 
 * 2. Structure-of-Arrays (SoA)
 *    - Contiguous neuron state arrays
 *    - Better cache utilization
 *    - Improved SIMD vectorization
 * 
 * 3. Sparse Connectivity
 *    - Adjacency lists instead of dense matrices
 *    - Efficient synapse lookup
 *    - Reduced memory for large networks
 * 
 * 4. Multithreading
 *    - Parallel population updates
 *    - Work-stealing load balancing
 *    - Thread-local state accumulation
 * 
 * 5. Memory Efficiency
 *    - Pre-allocated neuron/synapse pools
 *    - Reduced heap fragmentation
 *    - Compact data structures
 */

#include "Brain.hpp"
#include "MemoryPoolManager.hpp"
#include "EventSystem.hpp"
#include "ConnectivityManager.hpp"
#include "ParallelProcessor.hpp"
#include "SIMDVectorization.hpp"
#include <vector>
#include <array>
#include <optional>
#include <future>
#include <shared_mutex>

namespace nlm {

class OptimizedBrain : public Brain {
public:
    explicit OptimizedBrain(std::shared_ptr<Config> config);
    ~OptimizedBrain() override;
    
    // Initialize with configuration
    bool initialize() override;
    
    // Optimized simulation step
    void step(SimulationStep currentStep) override;
    void step(SimulationStep currentStep, Timestamp currentTime) override;
    
    // Statistics
    struct PerformanceStats {
        double neuronsPerSecond;
        double spikesPerSecond;
        double synapticEventsPerSecond;
        double realTimeFactor;
        size_t activeNeurons;
        size_t firingNeurons;
        size_t pendingSpikes;
        size_t pendingDelayedSpikes;
        float avgFiringRate;
        float memoryUsageMB;
        double stepTimeMs;
    };
    
    PerformanceStats getPerformanceStats() const;
    
    // Enable/disable optimizations
    void enableEventDriven(bool enable) { eventDrivenEnabled_ = enable; }
    void enableMultithreading(bool enable) { multithreadingEnabled_ = enable; }
    void enableSIMD(bool enable) { simdEnabled_ = enable; }
    void enableSparseConnectivity(bool enable) { sparseEnabled_ = enable; }
    
    bool isEventDrivenEnabled() const { return eventDrivenEnabled_; }
    bool isMultithreadingEnabled() const { return multithreadingEnabled_; }
    bool isSIMDEnabled() const { return simdEnabled_; }
    bool isSparseEnabled() const { return sparseEnabled_; }
    
    // Thread count
    void setThreadCount(size_t count) { threadCount_ = count; }
    size_t getThreadCount() const { return threadCount_; }
    
    // Memory pool integration
    void setMemoryPool(MemoryPoolManager& poolManager);
    
    // Event system integration
    void setEventSystem(EventSystem& eventSystem);
    
    // Performance monitoring
    void enablePerformanceMonitoring(bool enable);
    void reportPerformance();
    
protected:
    // Optimized implementations
    void processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime) override;
    void updateNeurons(SimulationStep currentStep, Timestamp currentTime) override;
    void applyPlasticity(SimulationStep currentStep, Timestamp currentTime) override;
    void applyStructuralPlasticity(SimulationStep currentStep, Timestamp currentTime) override;
    
private:
    /**
     * Neural state in SoA format for efficient processing
     */
    struct NeuralStateSoA {
        std::vector<float> membranePotential;
        std::vector<float> restingPotential;
        std::vector<float> threshold;
        std::vector<float> resetPotential;
        std::vector<float> leakConductance;
        std::vector<float> synapseInput;  // Accumulated synaptic input for this step
        std::vector<uint8_t> firingState;  // FiringState enum
        std::vector<uint32_t> refractoryRemaining;
        std::vector<uint32_t> refractoryPeriod;
        std::vector<float> lastSpikeTime;
        std::vector<uint64_t> neuronType;  // NeuronType enum
        std::vector<float> adaptationVariable;  // For spike-frequency adaptation
        
        // Active neuron tracking
        std::vector<uint32_t> activeNeuronIndices;  // Indices of neurons with non-zero input
        std::vector<uint32_t> firingNeuronIndices;  // Indices of neurons that fired this step
        
        void initialize(size_t size);
        void clear();
        size_t memoryUsage() const;
    };
    
    /**
     * Synaptic state in SoA format
     */
    struct SynapseStateSoA {
        std::vector<uint64_t> sourceNeuron;
        std::vector<uint64_t> destinationNeuron;
        std::vector<float> weight;
        std::vector<uint32_t> delay;
        std::vector<uint8_t> synapseType;
        std::vector<uint8_t> plasticityFlags;
        std::vector<float> eligibilityTrace;
        std::vector<float> efficacy;
        std::vector<float> shortTermDepression;
        std::vector<float> shortTermFacilitation;
        
        // Spike history for STDP (circular buffers)
        static constexpr size_t MAX_HISTORY = 32;
        std::vector<std::array<float, MAX_HISTORY>> preSpikeHistory;
        std::vector<std::array<float, MAX_HISTORY>> postSpikeHistory;
        std::vector<uint8_t> preHistoryHead;
        std::vector<uint8_t> postHistoryHead;
        
        void initialize(size_t size);
        void clear();
        size_t memoryUsage() const;
    };
    
    /**
     * Neuron index for O(1) lookup
     */
    struct NeuronIndex {
        uint64_t globalId;
        uint32_t localIndex;
        uint32_t regionIndex;
        uint32_t populationIndex;
    };
    
    // Initialize connectivity
    void initializeSparseConnectivity();
    
    // Event-driven processing
    void processActiveNeurons(uint64_t currentStep, double currentTime);
    void processFiringNeurons(uint64_t currentStep, double currentTime);
    
    // Batch processing
    void updateNeuronsBatch(size_t begin, size_t end, uint64_t currentStep, double currentTime);
    void updateSynapsesBatch(size_t begin, size_t end, double currentTime);
    
    // Apply plasticity
    void applyPlasticity();
    
    // Statistics
    void updateStatistics();
    
    // Configuration
    bool eventDrivenEnabled_;
    bool multithreadingEnabled_;
    bool simdEnabled_;
    bool sparseEnabled_;
    size_t threadCount_;
    
    // Neural state
    NeuralStateSoA neuralState_;
    std::vector<NeuronIndex> neuronIndices_;
    std::vector<uint64_t> activeNeurons_;  // Set of active neuron local indices
    std::atomic<size_t> activeNeuronCount_;
    
    // Synapse state
    SynapseStateSoA synapseState_;
    std::unique_ptr<SparseConnectivity> connectivity_;
    
    // Event queues
    std::unique_ptr<EventSystem> eventSystem_;
    
    // Parallel processor
    std::unique_ptr<ParallelProcessor> parallelProcessor_;
    
    // Statistics
    PerformanceStats perfStats_;
    std::chrono::high_resolution_clock::time_point lastStepTime_;
    size_t totalSteps_;
    
    // LIF vectorizer
    VectorizedLIF lifVectorizer_;
    
    // SIMD features
    SIMDFeatures simdFeatures_;
    bool simdInitialized_;
    
    // Memory pool manager
    MemoryPoolManager* memoryPoolManager_;
    
    // Performance monitoring
    bool performanceMonitoringEnabled_;
    
    // Configuration constants
    static constexpr size_t BATCH_SIZE_THRESHOLD = 1000;
    static constexpr size_t PLASTICITY_BATCH_THRESHOLD = 5000;
    
    // Helper functions
    void processNeuronsSequential(uint64_t currentStep, double currentTime);
    void updateNeuronOptimized(size_t neuronIdx, uint64_t currentStep, double currentTime);
    void updateNeuronScalar(size_t neuronIdx, uint64_t currentStep, double currentTime);
    void recordSpikeForPlasticity(size_t neuronIdx, double currentTime);
    void applyPlasticityAtIndex(size_t synapseIdx);
    void updateNeuronsSequential(size_t begin, size_t end, uint64_t currentStep, double currentTime);
    void updateSynapsesSequential(size_t begin, size_t end, double currentTime);
    void processNeuronsBatchParallel(size_t activeCount, uint64_t currentStep, double currentTime);
    void processNeuronsBatchSequential(size_t activeCount, uint64_t currentStep, double currentTime);
    
    // Friendship declarations
    friend class Brain;  // For Brain integration
    friend class MemoryPoolManager;  // For memory pool integration
};

} // namespace nlm