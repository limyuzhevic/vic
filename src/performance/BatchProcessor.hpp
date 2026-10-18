#pragma once

#include "Action.hpp"
#include <vector>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <atomic>
#include <functional>
#include <chrono>

namespace nlm {

// Batch processing configuration
struct BatchConfig {
    size_t batchSize = 100;
    float maxProcessingTime = 1.0f; // seconds
    bool enableParallelProcessing = false;
    int maxThreads = 4;
    std::string processingStrategy = "fifo"; // fifo, priority, round_robin
    bool enablePriorityScheduling = false;
    float timeoutPerAction = 0.1f;
    bool enableEarlyTermination = true;
};

// Batch statistics
struct BatchStats {
    size_t totalBatchesProcessed = 0;
    size_t totalActionsProcessed = 0;
    size_t totalActionsCompleted = 0;
    size_t totalActionsFailed = 0;
    double averageBatchSize = 0.0;
    double averageProcessingTime = 0.0;
    double throughput = 0.0; // actions per second
    std::chrono::steady_clock::time_point lastProcessed;
};

// Batch execution result
struct BatchResult {
    std::vector<ActionResult> results;
    double processingTime;
    bool completed = false;
    std::string errorMessage;
    size_t successfulActions = 0;
    size_t failedActions = 0;
};

// Action priority system
enum class ActionPriority {
    Critical,      // Must complete
    High,          // High importance
    Normal,        // Default
    Low,           // Background
    Discardable    // Can be skipped
};

// Action with metadata for batch processing
struct BatchAction {
    std::unique_ptr<Action> action;
    ActionPriority priority;
    std::chrono::steady_clock::time_point enqueueTime;
    std::chrono::steady_clock::time_point deadline;
    std::unordered_map<std::string, float> metadata;
    std::function<bool(const ActionResult&)> successCondition;
    std::function<void(const BatchAction&)> callback;
    bool requiresValidation = false;
    std::vector<std::string> validationRules;
};

// Thread pool for parallel processing
class ActionThreadPool {
public:
    ActionThreadPool(size_t threadCount = std::thread::hardware_concurrency());
    ~ActionThreadPool();
    
    void submit(std::function<void()> task);
    std::vector<std::function<void()>> getAvailableTasks(size_t count = 1);
    size_t getActiveThreadCount() const;
    void stopAllThreads();
    bool isRunning() const;

private:
    std::vector<std::thread> threads_;
    std::queue<std::function<void()>> taskQueue_;
    std::mutex queueMutex_;
    std::condition_variable condition_;
    std::atomic<bool> running_;
    std::atomic<size_t> activeThreads_;
};

// Batch processor interface
class BatchProcessor {
public:
    BatchProcessor();
    ~BatchProcessor();
    
    // Configuration
    void setConfig(const BatchConfig& config);
    BatchConfig getConfig() const;
    
    // Batch operations
    BatchResult processBatch(const std::vector<std::unique_ptr<Action>>& actions);
    BatchResult processBatch(const std::vector<BatchAction>& actions);
    
    // Queue management
    void enqueueAction(std::unique_ptr<Action> action, ActionPriority priority = ActionPriority::Normal);
    void enqueueAction(BatchAction action);
    
    // Control operations
    void startProcessing();
    void stopProcessing();
    void pauseProcessing();
    bool isProcessing() const;
    
    // Statistics
    BatchStats getStats() const;
    void resetStats();
    
    // Callback system
    void addCallback(std::function<void(const BatchResult&)>& callback);
    void removeCallback(std::function<void(const BatchResult&)>& callback);
    
    // Performance optimization
    void optimizeForLatency();
    void optimizeForThroughput();
    void optimizeForMemory();
    
    // Monitoring
    void enableMonitoring(bool enable);
    std::vector<BatchResult> getRecentResults(size_t count = 10) const;
    
    // Streaming interface
    class BatchStream {
    public:
        BatchStream(BatchProcessor& processor);
        BatchResult processNext();
        bool hasNext() const;
        void reset();
    
    private:
        BatchProcessor& processor_;
        size_t currentIndex_;
    };

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Priority queue for batch actions
class BatchActionQueue {
public:
    BatchActionQueue();
    ~BatchActionQueue();
    
    // Action insertion
    void enqueue(std::unique_ptr<BatchAction> action);
    void enqueue(BatchAction action);
    
    // Action retrieval
    std::unique_ptr<BatchAction> dequeue();
    std::vector<std::unique_ptr<BatchAction>> dequeueBatch(size_t count);
    std::unique_ptr<BatchAction> peekNext() const;
    
    // Queue statistics
    size_t size() const;
    bool isEmpty() const;
    
    // Priority management
    void setPriorityThreshold(ActionPriority threshold);
    bool isBelowPriorityThreshold(const BatchAction& action) const;
    
    // Control operations
    void clear();
    void pause();
    void resume();
    bool isPaused() const;
    
    // Monitoring
    double getAverageWaitTime() const;
    std::vector<std::chrono::steady_clock::time_point> getWaitTimes() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Batch validation system
class BatchValidator {
public:
    BatchValidator();
    ~BatchValidator();
    
    // Validation rules
    void addPreValidationRule(const std::string& name,
                             std::function<bool(const BatchAction&)> rule,
                             const std::string& errorMessage);
    
    void addPostValidationRule(const std::string& name,
                              std::function<bool(const ActionResult&)> rule,
                              const std::string& errorMessage);
    
    // Batch validation
    std::vector<std::string> validateBatch(const std::vector<BatchAction>& actions) const;
    std::vector<std::string> validateBatch(const std::vector<std::unique_ptr<Action>>& actions) const;
    
    // Action validation
    std::vector<std::string> validateAction(const BatchAction& action) const;
    std::vector<std::string> validateAction(const Action& action) const;
    
    // Configuration
    void enableStrictMode(bool enable);
    void setValidationTimeout(float timeout);
    
    // Statistics
    struct ValidationStats {
        size_t totalValidActions = 0;
        size_t totalInvalidActions = 0;
        double averageValidationTime = 0.0;
        std::unordered_map<std::string, size_t> validationErrorsByRule;
    };
    
    ValidationStats getStats() const;
    void resetStats();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Batch memory manager
class BatchMemoryManager {
public:
    BatchMemoryManager();
    ~BatchMemoryManager();
    
    // Memory allocation
    std::vector<float> allocateBatchMemory(size_t size);
    void deallocateBatchMemory(std::vector<float>& memory);
    
    // Memory pooling
    void enableMemoryPooling(bool enable, size_t poolSize = 1024);
    bool isMemoryPoolingEnabled() const;
    
    // Memory statistics
    struct MemoryStats {
        size_t totalAllocated = 0;
        size_t totalDeallocated = 0;
        size_t currentUsage = 0;
        size_t peakUsage = 0;
        double averageAllocationSize = 0.0;
    };
    
    MemoryStats getStats() const;
    
    // Optimization
    void optimizeMemoryLayout(const std::vector<BatchAction>& actions);
    void compactMemory();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Streaming batch processor for real-time processing
class StreamingBatchProcessor {
public:
    StreamingBatchProcessor();
    ~StreamingBatchProcessor();
    
    // Streaming interface
    void feedAction(std::unique_ptr<Action> action);
    void feedAction(BatchAction action);
    
    BatchResult getNextResult();
    bool hasResults() const;
    
    // Configuration
    void setStreamingBufferSize(size_t size);
    void setMaxLatency(float maxLatency);
    
    // Control
    void startStreaming();
    void stopStreaming();
    void pauseStreaming();
    bool isStreaming() const;
    
    // Statistics
    BatchStats getStats() const;
    
    // Callbacks
    void addResultCallback(std::function<void(const BatchResult&)>& callback);
    void removeResultCallback(std::function<void(const BatchResult&)>& callback);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Batch processing statistics exporter
class BatchStatsExporter {
public:
    BatchStatsExporter();
    ~BatchStatsExporter();
    
    // Export formats
    std::string exportToJSON(const BatchStats& stats) const;
    std::string exportToCSV(const BatchStats& stats) const;
    std::string exportToText(const BatchStats& stats) const;
    
    // Export configurations
    void setExportFormat(const std::string& format);
    void setExportFile(const std::string& filename);
    void setIncludeTimestamps(bool include);
    void setIncludeDetailedStats(bool include);
    
    // Export operations
    void exportStats(const BatchStats& stats, const std::string& filename = "");
    std::string getExportedData() const;
    
    // Configuration
    void enableCompression(bool enable);
    bool isCompressionEnabled() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Batch processing utilities
namespace BatchUtils {
    // Performance measurement
    double measureProcessingTime(std::function<void()> task);
    
    // Statistics calculation
    double calculateThroughput(size_t count, double time);
    double calculateEfficiency(double idealTime, double actualTime);
    
    // Data processing
    std::vector<float> normalizeResults(const std::vector<ActionResult>& results);
    std::vector<ActionResult> filterResults(const std::vector<ActionResult>& results,
                                           std::function<bool(const ActionResult&)> predicate);
    
    // Validation
    std::vector<std::string> validateBatchConfig(const BatchConfig& config);
    bool validateActionPriority(ActionPriority priority);
    
    // Logging
    void logBatchStats(const BatchStats& stats);
    void logBatchResult(const BatchResult& result);
}

} // namespace nlm
