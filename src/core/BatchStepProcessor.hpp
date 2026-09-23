#pragma once

/**
 * BatchStepProcessor - Core batch processing engine for neural simulation
 * 
 * Provides efficient processing of multiple simulation steps with memory optimization,
 * progress tracking, and early termination capabilities. Built on existing memory pools
 * and SIMD vectorization for maximum performance.
 * 
 * Features:
 * - Process multiple steps in a single batch
 * - Memory pooling for efficient allocation/deallocation
 * - Progress reporting and early termination detection
 * - Thread-safe operations
 * - Vectorized batch operations
 * - Checkpoint support for large batches
 * 
 * Mathematical models:
 * - Batch LIF updates: V_new = V + dt * ((V_rest - V)/tau + I/C)
 * - Batch synaptic integration with vectorized accumulation
 * - Parallel processing with work-stealing
 * 
 * Performance targets:
 * - 10x faster than single-step processing for large batches
 * - Memory usage reduced by 50% through pooling
 * - Cache-friendly memory access patterns
 */

#include "../core/Types/Types.hpp"
#include "../performance/MemoryPool.hpp"
#include "../performance/SIMDVectorization.hpp"
#include "../performance/ParallelProcessing.hpp"
#include <vector>
#include <memory>
#include <atomic>
#include <thread>
#include <mutex>
#include <functional>
#include <algorithm>
#include <chrono>
#include <optional>
#include <numeric>

namespace nlm {

/**
 * BatchStepProcessor - Core batch processing engine
 * 
 * Processes multiple simulation steps with optimized memory management
 * and vectorized operations for maximum performance.
 */
class BatchStepProcessor {
public:
    /**
     * Batch processing configuration
     */
    struct Config {
        size_t batchSize = 1000;           // Default batch size
        size_t maxBatchSize = 10000;       // Maximum allowed batch size
        bool enableSIMD = true;            // Enable SIMD vectorization
        bool enableMultithreading = true;  // Enable parallel processing
        bool enableMemoryPool = true;      // Enable memory pooling
        float earlyTerminationThreshold = 0.95f;  // Early termination threshold (95% complete)
        std::function<bool()> shouldTerminate = nullptr;  // User-provided termination callback
        std::function<void(float)> progressCallback = nullptr;  // Progress reporting callback
    };
    
    /**
     * Batch step result
     */
    struct StepResult {
        SimulationStep startStep;           // First step in batch
        SimulationStep endStep;             // Last step in batch
        SimulationStep stepsProcessed;      // Number of steps actually processed
        size_t totalNeurons;               // Total neurons processed
        size_t totalSpikes;                // Total spikes generated
        size_t totalSynapticEvents;        // Total synaptic events
        double startTime;                   // Batch start time
        double endTime;                     // Batch end time
        double cpuTime;                     // CPU time used
        double wallTime;                    // Wall clock time
        bool terminatedEarly;               // Whether batch was terminated early
        size_t checkpointCount;             // Number of checkpoints saved
    };
    
    /**
     * Neuron processing state for batch operations
     */
    struct NeuronProcessingState {
        // Neuron state (SoA format for cache efficiency)
        std::vector<float> membranePotential;
        std::vector<float> restingPotential;
        std::vector<float> threshold;
        std::vector<float> resetPotential;
        std::vector<float> leakConductance;
        std::vector<float> synapticInput;      // Accumulated synaptic input for batch
        std::vector<uint8_t> firingState;
        std::vector<uint32_t> refractoryRemaining;
        std::vector<uint32_t> refractoryPeriod;
        std::vector<float> lastSpikeTime;
        std::vector<uint64_t> neuronType;
        
        // Active neuron tracking
        std::vector<uint32_t> activeNeurons;   // Indices of neurons with non-zero input
        std::vector<uint32_t> firingNeurons;   // Indices of neurons that fired this batch
        
        void initialize(size_t size);
        void clear();
        size_t memoryUsage() const;
    };
    
    /**
     * Synapse processing state for batch operations
     */
    struct SynapseProcessingState {
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
        
        // Batch-specific state
        std::vector<float> batchSynapticInput;   // Accumulated input per synapse
        std::vector<float> batchWeightChange;   // Weight change accumulated over batch
        
        void initialize(size_t size);
        void clear();
        size_t memoryUsage() const;
    };
    
    /**
     * Batch processing statistics
     */
    struct ProcessingStats {
        std::atomic<size_t> stepsCompleted{0};
        std::atomic<size_t> neuronsProcessed{0};
        std::atomic<size_t> spikesGenerated{0};
        std::atomic<size_t> synapticEvents{0};
        std::atomic<double> cpuTime{0.0};
        std::atomic<double> memoryUsage{0.0};
        std::chrono::high_resolution_clock::time_point startTime;
        std::chrono::high_resolution_clock::time_point endTime;
    };
    
    explicit BatchStepProcessor(const Config& config = Config());
    ~BatchStepProcessor();
    
    // Disable copying
    BatchStepProcessor(const BatchStepProcessor&) = delete;
    BatchStepProcessor& operator=(const BatchStepProcessor&) = delete;
    
    /**
     * Process a batch of simulation steps
     */
    StepResult processBatch(
        Brain* brain,
        SimulationStep startStep,
        SimulationStep endStep,
        double currentTime = 0.0
    );
    
    /**
     * Process multiple steps with continuous time
     */
    StepResult processBatchWithTime(
        Brain* brain,
        SimulationStep startStep,
        SimulationStep endStep,
        Timestamp startTime,
        Timestamp endTime
    );
    
    /**
     * Process steps with early termination detection
     */
    StepResult processBatchWithEarlyTermination(
        Brain* brain,
        SimulationStep startStep,
        SimulationStep endStep,
        double currentTime = 0.0
    );
    
    /**
     * Process batch with checkpointing for large batches
     */
    StepResult processBatchWithCheckpointing(
        Brain* brain,
        SimulationStep startStep,
        SimulationStep endStep,
        double currentTime = 0.0,
        const std::string& checkpointDir = "./batch_checkpoints",
        uint64_t checkpointInterval = 1000
    );
    
    /**
     * Process batch asynchronously
     */
    std::future<StepResult> processBatchAsync(
        Brain* brain,
        SimulationStep startStep,
        SimulationStep endStep,
        double currentTime = 0.0
    );
    
    /**
     * Get current processing progress (0.0 - 1.0)
     */
    float getProgress() const;
    
    /**
     * Check if batch processing is in progress
     */
    bool isProcessing() const;
    
    /**
     * Get current processing statistics
     */
    ProcessingStats getStats() const;
    
    /**
     * Enable/disable processing optimizations
     */
    void enableSIMD(bool enable) { simdEnabled_ = enable; }
    void enableMultithreading(bool enable) { multithreadingEnabled_ = enable; }
    void enableMemoryPool(bool enable) { memoryPoolEnabled_ = enable; }
    
    /**
     * Set configuration
     */
    void setConfig(const Config& config) { config_ = config; }
    
    /**
     * Get configuration
     */
    const Config& getConfig() const { return config_; }
    
private:
    /**
     * Internal batch processing implementation
     */
    StepResult processBatchInternal(
        Brain* brain,
        SimulationStep startStep,
        SimulationStep endStep,
        double currentTime,
        bool enableEarlyTermination
    );
    
    /**
     * Initialize neuron state for batch processing
     */
    void initializeNeuronState(size_t neuronCount);
    
    /**
     * Initialize synapse state for batch processing
     */
    void initializeSynapseState(size_t synapseCount);
    
    /**
     * Reset processing state
     */
    void resetProcessingState();
    
    /**
     * Update neuron membrane potentials for batch
     */
    size_t updateNeuronsBatch(
        const NeuronProcessingState& neuronState,
        const std::vector<NeuronId>& activeNeurons,
        size_t begin,
        size_t end,
        double currentTime,
        TimestepDuration timestep
    );
    
    /**
     * Process synaptic inputs for batch
     */
    void processSynapticInputsBatch(
        const SynapseProcessingState& synapseState,
        NeuronProcessingState& neuronState,
        const std::vector<NeuronId>& activeNeurons,
        size_t begin,
        size_t end
    );
    
    /**
     * Apply plasticity for batch
     */
    void applyPlasticityBatch(
        const SynapseProcessingState& synapseState,
        size_t begin,
        size_t end,
        double currentTime
    );
    
    /**
     * Save checkpoint for batch processing
     */
    bool saveBatchCheckpoint(
        const Brain* brain,
        SimulationStep step,
        const std::string& checkpointDir
    );
    
    /**
     * Load checkpoint for batch processing
     */
    bool loadBatchCheckpoint(
        Brain* brain,
        const std::string& checkpointPath,
        SimulationStep& targetStep
    );
    
    // Configuration
    Config config_;
    
    // Processing state
    bool processing_{false};
    std::atomic<float> progress_{0.0f};
    ProcessingStats stats_;
    
    // Memory pools for batch operations
    std::unique_ptr<MemoryPool<float>> neuronStatePool_;
    std::unique_ptr<MemoryPool<uint8_t>> synapseStatePool_;
    
    // Batch processing buffers
    NeuronProcessingState neuronState_;
    SynapseProcessingState synapseState_;
    
    // Thread pool for parallel processing
    std::unique_ptr<ParallelNeuralProcessor> parallelProcessor_;
    
    // SIMD vectorizer
    VectorizedLIF lifVectorizer_;
    
    // Optimization flags
    bool simdEnabled_;
    bool multithreadingEnabled_;
    bool memoryPoolEnabled_;
    
    // Thread safety
    mutable std::mutex processingMutex_;
};

// Inline implementations

inline void BatchStepProcessor::NeuronProcessingState::initialize(size_t size) {
    membranePotential.resize(size);
    restingPotential.resize(size);
    threshold.resize(size);
    resetPotential.resize(size);
    leakConductance.resize(size);
    synapticInput.resize(size, 0.0f);
    firingState.resize(size, 0);
    refractoryRemaining.resize(size, 0);
    refractoryPeriod.resize(size, 0);
    lastSpikeTime.resize(size, -1.0f);
    neuronType.resize(size);
    
    activeNeurons.reserve(size);
    firingNeurons.reserve(size);
}

inline void BatchStepProcessor::NeuronProcessingState::clear() {
    std::fill(synapticInput.begin(), synapticInput.end(), 0.0f);
    activeNeurons.clear();
    firingNeurons.clear();
}

inline size_t BatchStepProcessor::NeuronProcessingState::memoryUsage() const {
    size_t total = 0;
    total += membranePotential.capacity() * sizeof(float);
    total += restingPotential.capacity() * sizeof(float);
    total += threshold.capacity() * sizeof(float);
    total += resetPotential.capacity() * sizeof(float);
    total += leakConductance.capacity() * sizeof(float);
    total += synapticInput.capacity() * sizeof(float);
    total += firingState.capacity() * sizeof(uint8_t);
    total += refractoryRemaining.capacity() * sizeof(uint32_t);
    total += refractoryPeriod.capacity() * sizeof(uint32_t);
    total += lastSpikeTime.capacity() * sizeof(float);
    total += neuronType.capacity() * sizeof(uint64_t);
    return total;
}

inline void BatchStepProcessor::SynapseProcessingState::initialize(size_t size) {
    sourceNeuron.resize(size);
    destinationNeuron.resize(size);
    weight.resize(size);
    delay.resize(size);
    synapseType.resize(size);
    plasticityFlags.resize(size);
    eligibilityTrace.resize(size, 0.0f);
    efficacy.resize(size, 1.0f);
    shortTermDepression.resize(size, 1.0f);
    shortTermFacilitation.resize(size, 0.0f);
    
    batchSynapticInput.resize(size, 0.0f);
    batchWeightChange.resize(size, 0.0f);
}

inline void BatchStepProcessor::SynapseProcessingState::clear() {
    std::fill(batchSynapticInput.begin(), batchSynapticInput.end(), 0.0f);
    std::fill(batchWeightChange.begin(), batchWeightChange.end(), 0.0f);
}

inline size_t BatchStepProcessor::SynapseProcessingState::memoryUsage() const {
    size_t total = 0;
    total += sourceNeuron.capacity() * sizeof(uint64_t);
    total += destinationNeuron.capacity() * sizeof(uint64_t);
    total += weight.capacity() * sizeof(float);
    total += delay.capacity() * sizeof(uint32_t);
    total += synapseType.capacity() * sizeof(uint8_t);
    total += plasticityFlags.capacity() * sizeof(uint8_t);
    total += eligibilityTrace.capacity() * sizeof(float);
    total += efficacy.capacity() * sizeof(float);
    total += shortTermDepression.capacity() * sizeof(float);
    total += shortTermFacilitation.capacity() * sizeof(float);
    total += batchSynapticInput.capacity() * sizeof(float);
    total += batchWeightChange.capacity() * sizeof(float);
    return total;
}

} // namespace nlm
