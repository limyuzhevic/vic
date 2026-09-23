#pragma once

/**
 * BatchBrain - High-level interface for batch processing in NLM
 * 
 * Provides a simplified API for batch processing of neural simulation steps.
 * Integrates with Brain and BatchStepProcessor to provide efficient batch operations.
 * 
 * Features:
 * - Simple batch processing API
 * - Progress tracking and monitoring
 * - Checkpoint support for large batches
 * - Performance metrics and statistics
 * - Thread-safe operations
 * - Graceful degradation for smaller batches
 * 
 * API design:
 * - High-level interface for easy adoption
 * - Performance-oriented with monitoring
 * - Backward compatible with single-step processing
 * - Thread-safe for concurrent access
 */

#include "Brain.hpp"
#include "BatchStepProcessor.hpp"
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <atomic>
#include <chrono>

namespace nlm {

/**
 * BatchBrain - Enhanced brain interface for batch processing
 * 
 * Provides optimized batch processing capabilities for neural simulation.
 * Designed for high-throughput simulation of multiple steps with reduced
 * overhead and improved memory usage patterns.
 * 
 * Key features:
 * - Batch step processing with memory pooling
 * - Progress reporting and early termination
 * - Checkpoint/restart support
 * - Performance monitoring
 * - Thread-safe operations
 * - SIMD and multithreading optimizations
 */
class BatchBrain {
public:
    /**
     * Batch processing configuration
     */
    struct BatchConfig {
        // Batch size parameters
        size_t defaultBatchSize = 1000;      // Default batch size
        size_t maxBatchSize = 10000;         // Maximum batch size
        size_t minBatchSize = 10;            // Minimum batch size
        
        // Processing optimization flags
        bool enableSIMD = true;              // Enable SIMD vectorization
        bool enableMultithreading = true;    // Enable parallel processing
        bool enableMemoryPool = true;        // Enable memory pooling
        bool enableCheckpointing = false;    // Enable checkpointing
        
        // Performance targets
        float targetThroughput = 1000.0f;    // Steps per second
        float targetMemoryUsage = 512.0f;    // MB
        
        // Progress and monitoring
        float progressUpdateInterval = 0.1f; // Update progress every 10%
        bool reportProgress = true;          // Report progress to console
        bool enableEarlyTermination = true;   // Enable early termination
        float earlyTerminationThreshold = 0.95f; // Early termination threshold
        
        // Checkpoint settings
        std::string checkpointDir = "./batch_checkpoints";
        uint64_t checkpointInterval = 1000;  // Steps between checkpoints
        size_t maxCheckpoints = 10;          // Maximum number of checkpoints to keep
        
        // Timeout and retry
        double stepTimeout = 1.0;            // Timeout per step (seconds)
        size_t maxRetries = 3;               // Maximum retry attempts
    };
    
    /**
     * Batch processing metrics
     */
    struct BatchMetrics {
        // Basic metrics
        SimulationStep stepsProcessed = 0;   // Steps processed
        SimulationStep totalSteps = 0;        // Total steps in batch
        double startTime = 0.0;               // Batch start time
        double endTime = 0.0;                 // Batch end time
        double cpuTime = 0.0;                 // CPU time used
        double wallTime = 0.0;                // Wall clock time
        
        // Performance metrics
        size_t neuronsProcessed = 0;          // Total neurons processed
        size_t spikesGenerated = 0;           // Total spikes generated
        size_t synapticEvents = 0;             // Total synaptic events
        double throughput = 0.0;              // Steps per second
        double memoryUsage = 0.0;             // Memory usage (bytes)
        
        // Progress metrics
        float progress = 0.0f;                 // Progress (0.0 - 1.0)
        double estimatedTimeRemaining = 0.0;   // Estimated time remaining
        
        // Quality metrics
        bool completedSuccessfully = false;   // Whether batch completed successfully
        bool terminatedEarly = false;          // Whether terminated early
        size_t checkpointCount = 0;            // Number of checkpoints created
    };
    
    /**
     * Batch processing state
     */
    struct BatchState {
        bool isProcessing = false;             // Whether batch is currently processing
        std::atomic<bool> shouldTerminate;    // Termination flag
        std::chrono::high_resolution_clock::time_point processingStartTime;
        BatchMetrics currentMetrics;
        std::vector<SimulationStep> checkpoints; // List of checkpoint steps
    };
    
    /**
     * Progress callback function type
     */
    using ProgressCallback = std::function<void(float, const BatchMetrics&)>;
    
    /**
     * Completion callback function type
     */
    using CompletionCallback = std::function<void(bool, const BatchMetrics&)>;
    
    /**
     * Create batch brain with configuration
     */
    explicit BatchBrain(const BatchConfig& config = BatchConfig());
    
    /**
     * Create batch brain with brain reference
     */
    explicit BatchBrain(std::shared_ptr<Brain> brain, const BatchConfig& config = BatchConfig());
    
    ~BatchBrain();
    
    // Disable copying
    BatchBrain(const BatchBrain&) = delete;
    BatchBrain& operator=(const BatchBrain&) = delete;
    
    // Enable moving
    BatchBrain(BatchBrain&&) noexcept;
    BatchBrain& operator=(BatchBrain&&) noexcept;
    
    /**
     * Set brain instance
     */
    void setBrain(std::shared_ptr<Brain> brain);
    
    /**
     * Get brain instance
     */
    std::shared_ptr<Brain> getBrain() const;
    
    /**
     * Process a batch of steps
     */
    bool processBatch(SimulationStep startStep, SimulationStep endStep);
    
    /**
     * Process a batch with continuous time
     */
    bool processBatchWithTime(SimulationStep startStep, SimulationStep endStep,
                              Timestamp startTime, Timestamp endTime);
    
    /**
     * Process a batch with early termination support
     */
    bool processBatchWithEarlyTermination(SimulationStep startStep, SimulationStep endStep);
    
    /**
     * Process a batch with checkpointing
     */
    bool processBatchWithCheckpointing(SimulationStep startStep, SimulationStep endStep,
                                       const std::string& checkpointDir = "");
    
    /**
     * Process a batch asynchronously
     */
    std::future<bool> processBatchAsync(SimulationStep startStep, SimulationStep endStep);
    
    /**
     * Process multiple batches in sequence
     */
    bool processMultipleBatches(const std::vector<std::pair<SimulationStep, SimulationStep>>& batches);
    
    /**
     * Process multiple batches in parallel
     */
    std::vector<std::future<bool>> processMultipleBatchesParallel(
        const std::vector<std::pair<SimulationStep, SimulationStep>>& batches);
    
    /**
     * Set progress callback
     */
    void setProgressCallback(ProgressCallback callback);
    
    /**
     * Set completion callback
     */
    void setCompletionCallback(CompletionCallback callback);
    
    /**
     * Set termination callback
     */
    void setTerminationCallback(std::function<bool()> callback);
    
    /**
     * Get current batch metrics
     */
    BatchMetrics getMetrics() const;
    
    /**
     * Get current batch state
     */
    BatchState getState() const;
    
    /**
     * Check if batch is currently processing
     */
    bool isProcessing() const;
    
    /**
     * Get current progress (0.0 - 1.0)
     */
    float getProgress() const;
    
    /**
     * Check if should terminate (called periodically during batch processing)
     */
    bool shouldTerminate() const;
    
    /**
     * Set configuration
     */
    void setConfig(const BatchConfig& config);
    
    /**
     * Get configuration
     */
    const BatchConfig& getConfig() const;
    
    /**
     * Enable/disable optimizations
     */
    void enableOptimization(bool enable);
    bool isOptimizationEnabled() const;
    
    /**
     * Get batch step processor
     */
    BatchStepProcessor* getBatchStepProcessor();
    
    /**
     * Get underlying brain
     */
    Brain* getUnderlyingBrain() const;
    
    /**
     * Save checkpoint
     */
    bool saveCheckpoint(SimulationStep step, const std::string& name = "");
    
    /**
     * Load checkpoint
     */
    bool loadCheckpoint(const std::string& name);
    
    /**
     * List available checkpoints
     */
    std::vector<std::string> listCheckpoints() const;
    
    /**
     * Delete old checkpoints
     */
    void pruneCheckpoints(size_t keepCount);
    
    /**
     * Reset batch processor
     */
    void reset();
    
    /**
     * Cancel current batch processing
     */
    void cancel();
    
    /**
     * Get performance report
     */
    std::string getPerformanceReport() const;
    
private:
    /**
     * Internal batch processing implementation
     */
    bool processBatchInternal(SimulationStep startStep, SimulationStep endStep);
    
    /**
     * Update metrics
     */
    void updateMetrics();
    
    /**
     * Calculate estimated time remaining
     */
    double calculateEstimatedTimeRemaining() const;
    
    /**
     * Process single step
     */
    bool processSingleStep(SimulationStep step);
    
    /**
     * Handle progress update
     */
    void handleProgressUpdate();
    
    /**
     * Handle completion
     */
    void handleCompletion(bool success);
    
    // Configuration
    BatchConfig config_;
    
    // Brain instance
    std::shared_ptr<Brain> brain_;
    
    // Batch processing state
    BatchState state_;
    
    // Batch processor
    std::unique_ptr<BatchStepProcessor> batchStepProcessor_;
    
    // Callbacks
    ProgressCallback progressCallback_;
    CompletionCallback completionCallback_;
    std::function<bool()> terminationCallback_;
    
    // Thread safety
    mutable std::mutex stateMutex_;
};

// Inline implementations

inline BatchBrain::BatchBrain(const BatchConfig& config)
    : config_(config)
    , batchStepProcessor_(std::make_unique<BatchStepProcessor>()) {
    state_.shouldTerminate.store(false, std::memory_order_relaxed);
}

inline BatchBrain::BatchBrain(std::shared_ptr<Brain> brain, const BatchConfig& config)
    : config_(config)
    , brain_(std::move(brain))
    , batchStepProcessor_(std::make_unique<BatchStepProcessor>()) {
    state_.shouldTerminate.store(false, std::memory_order_relaxed);
}

inline BatchBrain::BatchBrain(BatchBrain&& other) noexcept
    : config_(std::move(other.config_))
    , brain_(std::move(other.brain_))
    , state_(std::move(other.state_))
    , batchStepProcessor_(std::move(other.batchStepProcessor_)) {
    other.state_.shouldTerminate.store(true, std::memory_order_relaxed);
}

inline BatchBrain& BatchBrain::operator=(BatchBrain&& other) noexcept {
    if (this != &other) {
        cancel();
        config_ = std::move(other.config_);
        brain_ = std::move(other.brain_);
        state_ = std::move(other.state_);
        batchStepProcessor_ = std::move(other.batchStepProcessor_);
    }
    return *this;
}

inline void BatchBrain::setBrain(std::shared_ptr<Brain> brain) {
    std::lock_guard<std::mutex> lock(stateMutex_);
    brain_ = std::move(brain);
}

inline std::shared_ptr<Brain> BatchBrain::getBrain() const {
    return brain_;
}

inline BatchStepProcessor* BatchBrain::getBatchStepProcessor() {
    return batchStepProcessor_.get();
}

inline Brain* BatchBrain::getUnderlyingBrain() const {
    return brain_.get();
}

inline void BatchBrain::setConfig(const BatchConfig& config) {
    std::lock_guard<std::mutex> lock(stateMutex_);
    config_ = config;
}

inline const BatchConfig& BatchBrain::getConfig() const {
    return config_;
}

inline bool BatchBrain::isProcessing() const {
    return state_.isProcessing;
}

inline float BatchBrain::getProgress() const {
    return state_.currentMetrics.progress;
}

inline bool BatchBrain::shouldTerminate() const {
    return state_.shouldTerminate.load(std::memory_order_relaxed) ||
           (state_.isProcessing && config_.enableEarlyTermination &&
            state_.currentMetrics.progress >= config_.earlyTerminationThreshold);
}

inline void BatchBrain::setProgressCallback(ProgressCallback callback) {
    std::lock_guard<std::mutex> lock(stateMutex_);
    progressCallback_ = std::move(callback);
}

inline void BatchBrain::setCompletionCallback(CompletionCallback callback) {
    std::lock_guard<std::mutex> lock(stateMutex_);
    completionCallback_ = std::move(callback);
}

inline void BatchBrain::setTerminationCallback(std::function<bool()> callback) {
    std::lock_guard<std::mutex> lock(stateMutex_);
    terminationCallback_ = std::move(callback);
}

inline void BatchBrain::enableOptimization(bool enable) {
    if (batchStepProcessor_) {
        batchStepProcessor_->enableSIMD(enable);
        batchStepProcessor_->enableMultithreading(enable);
        batchStepProcessor_->enableMemoryPool(enable);
    }
}

inline bool BatchBrain::isOptimizationEnabled() const {
    return batchStepProcessor_ &&
           batchStepProcessor_->isSIMDEnabled() &&
           batchStepProcessor_->isMultithreadingEnabled();
}

inline BatchMetrics BatchBrain::getMetrics() const {
    return state_.currentMetrics;
}

inline BatchState BatchBrain::getState() const {
    return state_;
}

inline void BatchBrain::cancel() {
    std::lock_guard<std::mutex> lock(stateMutex_);
    state_.shouldTerminate.store(true, std::memory_order_relaxed);
    state_.isProcessing = false;
}

} // namespace nlm
