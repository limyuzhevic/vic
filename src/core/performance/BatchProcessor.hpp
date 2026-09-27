#pragma once

/**
 * BatchProcessor - Batch processing for performance optimization
 * 
 * Groups individual operations into batches for more efficient execution.
 * Particularly effective for STDP calculations, plasticity updates, and
 * other operations that benefit from vectorization and reduced overhead.
 * 
 * Features:
 * - Batch operation grouping
 * - Vectorized batch processing
 * - Automatic batching based on workload
 * - Batch statistics and monitoring
 * - Dynamic batch size adjustment
 * 
 * Optimization techniques:
 * 1. Batching STDP calculations
 * 2. Batch plasticity updates
 * 3. Vectorized neuron updates
 * 4. Reduced synchronization overhead
 * 5. Cache-friendly batch execution
 */

#include <cstddef>
#include <cstdint>
#include <vector>
#include <memory>
#include <functional>
#include <atomic>
#include <chrono>
#include <algorithm>
#include <thread>

namespace nlm {

/**
 * Batch operation type
 */
enum class BatchOperationType {
    STDP_CALCULATION,
    PLASTICITY_UPDATE,
    NEURON_UPDATE,
    SYNAPSE_UPDATE,
    MEMORY_UPDATE,
    CUSTOM
};

/**
 * Batch operation interface
 */
class BatchOperation {
public:
    virtual ~BatchOperation() = default;
    
    virtual BatchOperationType getType() const = 0;
    virtual size_t getOperationCount() const = 0;
    virtual double getEstimatedCost() const = 0;
    virtual void execute() = 0;
    virtual void reset() = 0;
    virtual bool isReady() const = 0;
};

/**
 * Neuron batch operation
 */
class NeuronBatchOperation : public BatchOperation {
public:
    NeuronBatchOperation() : operationCount_(0), cost_(0.0) {}
    
    BatchOperationType getType() const override { return BatchOperationType::NEURON_UPDATE; }
    size_t getOperationCount() const override { return operationCount_; }
    double getEstimatedCost() const override { return cost_; }
    
    void addNeuronUpdate(NeuronId id, const NeuronState& oldState, const NeuronState& newState);
    void execute() override;
    void reset() override { operationCount_ = 0; cost_ = 0.0; neurons_.clear(); }
    bool isReady() const override { return !neurons_.empty(); }
    
private:
    struct NeuronUpdate {
        NeuronId id;
        NeuronState oldState;
        NeuronState newState;
    };
    
    std::vector<NeuronUpdate> neurons_;
    size_t operationCount_;
    double cost_;
};

/**
 * Synapse batch operation
 */
class SynapseBatchOperation : public BatchOperation {
public:
    SynapseBatchOperation() : operationCount_(0), cost_(0.0) {}
    
    BatchOperationType getType() const override { return BatchOperationType::SYNAPSE_UPDATE; }
    size_t getOperationCount() const override { return operationCount_; }
    double getEstimatedCost() const override { return cost_; }
    
    void addSynapseUpdate(SynapseId id, const SynapseState& oldState, const SynapseState& newState);
    void execute() override;
    void reset() override { operationCount_ = 0; cost_ = 0.0; synapses_.clear(); }
    bool isReady() const override { return !synapses_.empty(); }
    
private:
    struct SynapseUpdate {
        SynapseId id;
        SynapseState oldState;
        SynapseState newState;
    };
    
    std::vector<SynapseUpdate> synapses_;
    size_t operationCount_;
    double cost_;
};

/**
 * STDP batch operation
 */
class STDPBatchOperation : public BatchOperation {
public:
    STDPBatchOperation() : operationCount_(0), cost_(0.0) {}
    
    BatchOperationType getType() const override { return BatchOperationType::STDP_CALCULATION; }
    size_t getOperationCount() const override { return operationCount_; }
    double getEstimatedCost() const override { return cost_; }
    
    void addSTDPUpdate(SynapseId synapseId, NeuronId preId, NeuronId postId, 
                      float dt, float weightChange);
    void execute() override;
    void reset() override { operationCount_ = 0; cost_ = 0.0; stdpUpdates_.clear(); }
    bool isReady() const override { return !stdpUpdates_.empty(); }
    
private:
    struct STDPUpdate {
        SynapseId synapseId;
        NeuronId preId;
        NeuronId postId;
        float dt;
        float weightChange;
    };
    
    std::vector<STDPUpdate> stdpUpdates_;
    size_t operationCount_;
    double cost_;
};

/**
 * Batch statistics
 */
struct BatchStats {
    size_t totalBatches;
    size_t completedBatches;
    size_t pendingBatches;
    double totalBatchTime;
    double avgBatchTime;
    size_t operationsProcessed;
    size_t operationsCompleted;
    
    BatchStats()
        : totalBatches(0), completedBatches(0), pendingBatches(0)
        , totalBatchTime(0.0), avgBatchTime(0.0)
        , operationsProcessed(0), operationsCompleted(0) {}
    
    double calculateThroughput() const {
        if (totalBatchTime == 0.0) return 0.0;
        return static_cast<double>(operationsCompleted) / totalBatchTime;
    }
};

/**
 * Batch processor for performance optimization
 */
class BatchProcessor {
public:
    BatchProcessor(size_t defaultBatchSize = 100);
    ~BatchProcessor();
    
    // Disable copying
    BatchProcessor(const BatchProcessor&) = delete;
    BatchProcessor& operator=(const BatchProcessor&) = delete;
    
    /**
     * Add operation to batch
     */
    template<typename Operation>
    void addOperation(std::unique_ptr<Operation> operation);
    
    /**
     * Add neuron batch operation
     */
    void addNeuronBatchOperation(const NeuronBatchOperation& operation);
    
    /**
     * Add synapse batch operation
     */
    void addSynapseBatchOperation(const SynapseBatchOperation& operation);
    
    /**
     * Add STDP batch operation
     */
    void addSTDPBatchOperation(const STDPBatchOperation& operation);
    
    /**
     * Process all ready operations
     */
    void processReadyOperations();
    
    /**
     * Check if there are pending operations
     */
    bool hasPendingOperations() const;
    
    /**
     * Get number of pending operations
     */
    size_t getPendingOperationCount() const;
    
    /**
     * Get batch statistics
     */
    BatchStats getStats() const;
    
    /**
     * Set batch size
     */
    void setBatchSize(size_t batchSize);
    
    /**
     * Get current batch size
     */
    size_t getBatchSize() const { return defaultBatchSize_; }
    
    /**
     * Enable/disable batch processing
     */
    void enableBatchProcessing(bool enable) { batchProcessingEnabled_ = enable; }
    
    /**
     * Check if batch processing is enabled
     */
    bool isBatchProcessingEnabled() const { return batchProcessingEnabled_; }
    
    /**
     * Get next ready batch
     */
    std::unique_ptr<BatchOperation> getNextReadyBatch();
    
    /**
     * Reset batch processor
     */
    void reset();
    
    /**
     * Cleanup completed batches
     */
    void cleanup();
    
    /**
     * Get optimal batch size based on workload
     */
    size_t calculateOptimalBatchSize() const;
    
private:
    /**
     * Group operations into batches
     */
    void groupOperationsIntoBatches();n    
    /**
     * Execute batch operation
     */
    void executeBatchOperation(BatchOperation& operation);
    
    /**
     * Check if operation should be batched
     */
    bool shouldBatchOperation(const BatchOperation& operation) const;
    
    /**
     * Estimate operation cost
     */
    double estimateOperationCost(const BatchOperation& operation) const;
    
private:
    size_t defaultBatchSize_;
    bool batchProcessingEnabled_;
    
    // Pending operations
    std::vector<std::unique_ptr<BatchOperation>> pendingOperations_;
    
    // Ready batches
    std::vector<std::unique_ptr<BatchOperation>> readyBatches_;
    
    // Statistics
    BatchStats stats_;
    mutable std::mutex statsMutex_;
    
    // Timing
    std::chrono::steady_clock::time_point lastBatchTime_;
};

} // namespace nlm
