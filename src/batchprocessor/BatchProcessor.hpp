// ============================================================================
// BatchProcessor Implementation
// ============================================================================

#pragma once

#include "../core/Logger/Logger.hpp"
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <vector>
#include <map>
#include <string>
#include <memory>
#include <functional>
#include <chrono>

namespace nlm {

struct BatchTask {
    uint64_t id;
    uint64_t priority;
    std::function<void()> task;
    std::chrono::high_resolution_clock::time_point submissionTime;
    std::chrono::high_resolution_clock::time_point scheduledTime;
    bool completed;
    std::chrono::duration<double> executionTime;
    
    BatchTask(uint64_t id, uint64_t priority, std::function<void()> task)
        : id(id), priority(priority), task(std::move(task)), 
          completed(false), executionTime(0.0) {
        submissionTime = std::chrono::high_resolution_clock::now();
        scheduledTime = submissionTime;
    }
};

struct BatchResult {
    uint64_t taskId;
    uint64_t batchId;
    bool success;
    std::string errorMessage;
    std::chrono::duration<double> executionTime;
    std::map<std::string, std::string> metadata;
    
    BatchResult(uint64_t taskId, uint64_t batchId, bool success, 
                const std::string& errorMessage = "",
                std::chrono::duration<double> executionTime = std::chrono::duration<double>(0.0))
        : taskId(taskId), batchId(batchId), success(success), 
          errorMessage(errorMessage), executionTime(executionTime) {}
};

struct BatchStatistics {
    uint64_t totalTasks;
    uint64_t completedTasks;
    uint64_t pendingTasks;
    uint64_t failedTasks;
    double completionPercentage;
    std::chrono::duration<double> totalExecutionTime;
    std::chrono::duration<double> averageTaskTime;
    std::chrono::duration<double> queueTime;
    std::chrono::duration<double> processingTime;
    
    BatchStatistics() : totalTasks(0), completedTasks(0), pendingTasks(0), 
                      failedTasks(0), completionPercentage(0.0),
                      totalExecutionTime(0.0), averageTaskTime(0.0),
                      queueTime(0.0), processingTime(0.0) {}
};

class BatchProcessor {
public:
    BatchProcessor();
    ~BatchProcessor();
    
    // Task management
    uint64_t submitTask(std::function<void()> task, uint64_t priority = 1);
    uint64_t submitTask(const std::string& name, std::function<void()> task, 
                        uint64_t priority = 1, std::map<std::string, std::string> metadata = {});
    
    // Batch operations
    void startBatch(uint64_t batchId, uint64_t taskCount);
    void completeBatch(uint64_t batchId);
    bool isBatchActive(uint64_t batchId) const;
    
    // Task management
    bool completeTask(uint64_t taskId, bool success = true, 
                     const std::string& errorMessage = "",
                     std::chrono::duration<double> executionTime = std::chrono::duration<double>(0.0));
    BatchTask* getTask(uint64_t taskId);
    const BatchTask* getTask(uint64_t taskId) const;
    
    // Queue management
    void startWorkerThreads(uint64_t numThreads = 0);
    void stopWorkerThreads();
    bool isRunning() const;
    uint64_t getWorkerCount() const;
    
    // Monitoring and statistics
    BatchStatistics getStatistics() const;
    std::vector<BatchTask> getTasks(uint64_t batchId = 0) const;
    std::vector<BatchResult> getResults(uint64_t batchId = 0) const;
    double getProgress(uint64_t batchId = 0) const;
    
    // Time management
    void setTaskTimeout(uint64_t taskId, std::chrono::duration<double> timeout);
    void setBatchTimeout(uint64_t batchId, std::chrono::duration<double> timeout);
    
    // Control operations
    void pauseProcessing();
    void resumeProcessing();
    void clearCompletedTasks();
    void clearBatch(uint64_t batchId);
    void clearAllBatches();
    
    // Export
    std::string exportToJSON(uint64_t batchId = 0) const;
    std::string exportToCSV(uint64_t batchId = 0) const;
    std::string exportToText() const;
    
    // Callbacks
    void registerBatchCallback(std::function<void(BatchProcessor&, uint64_t batchId)> callback);
    void registerTaskCallback(std::function<void(BatchProcessor&, uint64_t taskId)> callback);
    void clearCallbacks();
    
    // Performance optimization
    void enablePrioritization(bool enable);
    void enableTaskBatching(bool enable);
    void enableParallelProcessing(bool enable);
    
    // Simulation and testing
    void simulateFailure(uint64_t taskId, bool fail = true);
    void simulateSlowTask(uint64_t taskId, std::chrono::duration<double> slowDown);
    
    // Integration with brain systems
    void integrateWithBrain(const std::shared_ptr<Brain>& brain);
    uint64_t submitBrainSimulationTask(const std::string& name, const std::shared_ptr<Brain>& brain,
                                      uint64_t steps, uint64_t seed = 42);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    static uint64_t nextTaskId_;
    static uint64_t nextBatchId_;
};

} // namespace nlm
