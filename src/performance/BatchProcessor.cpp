#include "BatchProcessor.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <thread>
#include <iostream>

namespace nlm {

// ActionThreadPool implementation
ActionThreadPool::ActionThreadPool(size_t threadCount)
    : running_(true), activeThreads_(0) {
    
    threadCount = std::min(threadCount, std::thread::hardware_concurrency());
    threads_.reserve(threadCount);
    
    for (size_t i = 0; i < threadCount; ++i) {
        threads_.emplace_back([this]() {
            while (running_) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(queueMutex_);
                    condition_.wait(lock, [this]() { return !taskQueue_.empty() || !running_; });
                    
                    if (!running_ && taskQueue_.empty()) {
                        return;
                    }
                    
                    if (!taskQueue_.empty()) {
                        task = std::move(taskQueue_.front());
                        taskQueue_.pop();
                    }
                }
                
                if (task) {
                    activeThreads_++;
                    task();
                    activeThreads_--;
                }
            }
        });
    }
}

ActionThreadPool::~ActionThreadPool() {
    stopAllThreads();
    for (auto& thread : threads_) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

void ActionThreadPool::submit(std::function<void()> task) {
    if (task && running_) {
        std::lock_guard<std::mutex> lock(queueMutex_);
        taskQueue_.push(std::move(task));
        condition_.notify_one();
    }
}

std::vector<std::function<void()>> ActionThreadPool::getAvailableTasks(size_t count) {
    std::vector<std::function<void()>> tasks;
    tasks.reserve(count);
    
    std::lock_guard<std::mutex> lock(queueMutex_);
    size_t available = std::min(count, taskQueue_.size());
    
    for (size_t i = 0; i < available; ++i) {
        tasks.push_back(std::move(taskQueue_.front()));
        taskQueue_.pop();
    }
    
    return tasks;
}

void ActionThreadPool::stopAllThreads() {
    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        running_ = false;
    }
    condition_.notify_all();
}

bool ActionThreadPool::isRunning() const {
    return running_;
}

// BatchProcessor implementation
struct BatchProcessor::Impl {
    BatchConfig config;
    BatchActionQueue actionQueue;
    BatchValidator validator;
    BatchMemoryManager memoryManager;
    
    std::unique_ptr<ActionThreadPool> threadPool;
    std::vector<std::thread> workerThreads;
    
    std::mutex processingMutex;
    std::condition_variable processingCondition;
    bool processing_ = false;
    bool paused_ = false;
    bool stopped_ = false;
    
    std::vector<BatchAction> currentBatch;
    std::queue<BatchAction> pendingActions;
    std::queue<BatchResult> resultQueue;
    
    BatchStats stats;
    std::chrono::steady_clock::time_point lastStatsUpdate;
    
    std::vector<std::function<void(const BatchResult&)>> callbacks;
    
    StreamingBatchProcessor streamingProcessor;
};

BatchProcessor::BatchProcessor() : pImpl(std::make_unique<Impl>()) {
    // Initialize with default config
    pImpl->config.batchSize = 100;
    pImpl->config.maxProcessingTime = 1.0f;
    pImpl->config.enableParallelProcessing = false;
    pImpl->config.maxThreads = 4;
    
    // Start worker threads if parallel processing enabled
    if (pImpl->config.enableParallelProcessing) {
        pImpl->threadPool = std::make_unique<ActionThreadPool>(pImpl->config.maxThreads);
        for (size_t i = 0; i < pImpl->config.maxThreads; ++i) {
            pImpl->workerThreads.emplace_back([this]() {
                while (true) {
                    std::unique_lock<std::mutex> lock(pImpl->processingMutex);
                    pImpl->processingCondition.wait(lock, [this]() {
                        return !pImpl->currentBatch.empty() || pImpl->stopped_;
                    });
                    
                    if (pImpl->stopped_ && pImpl->currentBatch.empty()) {
                        return;
                    }
                    
                    if (!pImpl->currentBatch.empty()) {
                        BatchAction action = std::move(pImpl->currentBatch.back());
                        pImpl->currentBatch.pop_back();
                        
                        lock.unlock();
                        
                        // Process the action
                        ActionResult result(std::move(action.action));
                        
                        // Simulate processing
                        std::this_thread::sleep_for(std::chrono::milliseconds(1));
                        
                        result.setStatus(ActionResult::Status::Success);
                        result.setResults({1.0f, 0.5f});
                        
                        lock.lock();
                        
                        // Store result
                        BatchResult batchResult;
                        batchResult.results.push_back(result);
                        batchResult.completed = true;
                        batchResult.successfulActions = 1;
                        batchResult.processingTime = 0.001;
                        
                        pImpl->resultQueue.push(batchResult);
                        
                        // Notify callbacks
                        for (const auto& callback : pImpl->callbacks) {
                            callback(batchResult);
                        }
                    }
                }
            });
        }
    }
}

BatchProcessor::~BatchProcessor() {
    stopProcessing();
    
    for (auto& thread : pImpl->workerThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

void BatchProcessor::setConfig(const BatchConfig& config) {
    pImpl->config = config;
    
    // Reinitialize thread pool if needed
    if (config.enableParallelProcessing && !pImpl->threadPool) {
        pImpl->threadPool = std::make_unique<ActionThreadPool>(config.maxThreads);
    } else if (!config.enableParallelProcessing && pImpl->threadPool) {
        pImpl->threadPool.reset();
    }
}

BatchConfig BatchProcessor::getConfig() const {
    return pImpl->config;
}

void BatchProcessor::startProcessing() {
    std::lock_guard<std::mutex> lock(pImpl->processingMutex);
    pImpl->processing_ = true;
    pImpl->processingCondition.notify_all();
}

void BatchProcessor::stopProcessing() {
    {
        std::lock_guard<std::mutex> lock(pImpl->processingMutex);
        pImpl->stopped_ = true;
    }
    pImpl->processingCondition.notify_all();
}

void BatchProcessor::pauseProcessing() {
    std::lock_guard<std::mutex> lock(pImpl->processingMutex);
    pImpl->paused_ = true;
}

bool BatchProcessor::isProcessing() const {
    std::lock_guard<std::mutex> lock(pImpl->processingMutex);
    return pImpl->processing_ && !pImpl->paused_ && !pImpl->currentBatch.empty();
}

BatchResult BatchProcessor::processBatch(const std::vector<std::unique_ptr<Action>>& actions) {
    BatchResult result;
    
    // Convert to BatchAction format
    std::vector<BatchAction> batchActions;
    for (const auto& action : actions) {
        BatchAction batchAction;
        batchAction.action = action->clone();
        batchAction.priority = ActionPriority::Normal;
        batchActions.push_back(std::move(batchAction));
    }
    
    return processBatch(batchActions);
}

BatchResult BatchProcessor::processBatch(const std::vector<BatchAction>& actions) {
    BatchResult result;
    double startTime = std::chrono::duration<double>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    
    // Validate the batch
    auto validationErrors = pImpl->validator.validateBatch(actions);
    if (!validationErrors.empty()) {
        result.errorMessage = "Batch validation failed: " + validationErrors[0];
        return result;
    }
    
    // Process actions
    pImpl->currentBatch.clear();
    
    for (const auto& action : actions) {
        pImpl->currentBatch.push_back(action);
    }
    
    pImpl->processingCondition.notify_all();
    
    // Wait for processing to complete
    auto deadline = std::chrono::steady_clock::now() + 
                    std::chrono::milliseconds(static_cast<int>(pImpl->config.maxProcessingTime * 1000));
    
    while (!pImpl->currentBatch.empty() && 
           std::chrono::steady_clock::now() < deadline &&
           pImpl->processing_) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    
    // Collect results
    while (!pImpl->resultQueue.empty()) {
        result.results.push_back(pImpl->resultQueue.front().results[0]);
        pImpl->resultQueue.pop();
    }
    
    double endTime = std::chrono::duration<double>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    
    result.processingTime = endTime - startTime;
    result.completed = true;
    
    // Update statistics
    pImpl->stats.totalBatchesProcessed++;
    pImpl->stats.totalActionsProcessed += actions.size();
    pImpl->stats.totalActionsCompleted += result.results.size();
    pImpl->stats.averageProcessingTime = 
        (pImpl->stats.averageProcessingTime * (pImpl->stats.totalBatchesProcessed - 1) + 
         result.processingTime) / pImpl->stats.totalBatchesProcessed;
    
    pImpl->stats.lastProcessed = std::chrono::steady_clock::now();
    
    return result;
}

void BatchProcessor::enqueueAction(std::unique_ptr<Action> action, ActionPriority priority) {
    BatchAction batchAction;
    batchAction.action = std::move(action);
    batchAction.priority = priority;
    batchAction.enqueueTime = std::chrono::steady_clock::now();
    
    pImpl->pendingActions.push(std::move(batchAction));
}

void BatchProcessor::enqueueAction(BatchAction action) {
    pImpl->pendingActions.push(std::move(action));
}

BatchStats BatchProcessor::getStats() const {
    return pImpl->stats;
}

void BatchProcessor::resetStats() {
    pImpl->stats = BatchStats();
}

void BatchProcessor::addCallback(std::function<void(const BatchResult&)>& callback) {
    pImpl->callbacks.push_back(callback);
}

void BatchProcessor::removeCallback(std::function<void(const BatchResult&)>& callback) {
    pImpl->callbacks.erase(
        std::remove(pImpl->callbacks.begin(), pImpl->callbacks.end(), callback),
        pImpl->callbacks.end()
    );
}

void BatchProcessor::optimizeForLatency() {
    pImpl->config.maxThreads = pImpl->config.maxThreads * 2;
    pImpl->config.processingStrategy = "priority";
    pImpl->config.maxProcessingTime = 0.1f;
}

void BatchProcessor::optimizeForThroughput() {
    pImpl->config.maxThreads = pImpl->config.maxThreads * 2;
    pImpl->config.processingStrategy = "round_robin";
    pImpl->config.maxProcessingTime = 1.0f;
}

void BatchProcessor::optimizeForMemory() {
    pImpl->config.maxThreads = std::max(1, pImpl->config.maxThreads / 2);
    pImpl->config.batchSize = std::min(pImpl->config.batchSize, 50);
    pImpl->config.enableParallelProcessing = false;
}

void BatchProcessor::enableMonitoring(bool enable) {
    // Monitoring implementation
}

std::vector<BatchResult> BatchProcessor::getRecentResults(size_t count) const {
    std::vector<BatchResult> results;
    // Would need to store results for retrieval
    return results;
}

// StreamingBatchProcessor implementation
struct StreamingBatchProcessor::Impl {
    BatchProcessor batchProcessor;
    std::queue<BatchAction> actionQueue;
    std::mutex queueMutex;
    std::condition_variable queueCondition;
    bool streaming_ = false;
    
    BatchStats stats;
    size_t bufferSize = 100;
    float maxLatency = 0.1f;
    
    std::queue<BatchResult> resultQueue;
    std::vector<std::function<void(const BatchResult&)>> resultCallbacks;
};

StreamingBatchProcessor::StreamingBatchProcessor() : pImpl(std::make_unique<Impl>()) {}

StreamingBatchProcessor::~StreamingBatchProcessor() {
    stopStreaming();
}

void StreamingBatchProcessor::feedAction(std::unique_ptr<Action> action) {
    BatchAction batchAction;
    batchAction.action = std::move(action);
    batchAction.enqueueTime = std::chrono::steady_clock::now();
    
    std::lock_guard<std::mutex> lock(pImpl->queueMutex);
    pImpl->actionQueue.push(std::move(batchAction));
    pImpl->queueCondition.notify_one();
}

void StreamingBatchProcessor::feedAction(BatchAction action) {
    std::lock_guard<std::mutex> lock(pImpl->queueMutex);
    pImpl->actionQueue.push(std::move(action));
    pImpl->queueCondition.notify_one();
}

BatchResult StreamingBatchProcessor::getNextResult() {
    std::lock_guard<std::mutex> lock(pImpl->queueMutex);
    if (pImpl->resultQueue.empty()) {
        return BatchResult(); // Empty result
    }
    
    BatchResult result = pImpl->resultQueue.front();
    pImpl->resultQueue.pop();
    
    // Notify callbacks
    for (const auto& callback : pImpl->resultCallbacks) {
        callback(result);
    }
    
    return result;
}

bool StreamingBatchProcessor::hasResults() const {
    std::lock_guard<std::mutex> lock(pImpl->queueMutex);
    return !pImpl->resultQueue.empty();
}

void StreamingBatchProcessor::setStreamingBufferSize(size_t size) {
    pImpl->bufferSize = size;
}

void StreamingBatchProcessor::setMaxLatency(float maxLatency) {
    pImpl->maxLatency = maxLatency;
}

void StreamingBatchProcessor::startStreaming() {
    std::lock_guard<std::mutex> lock(pImpl->queueMutex);
    pImpl->streaming_ = true;
}

void StreamingBatchProcessor::stopStreaming() {
    {
        std::lock_guard<std::mutex> lock(pImpl->queueMutex);
        pImpl->streaming_ = false;
    }
    pImpl->queueCondition.notify_all();
}

void StreamingBatchProcessor::pauseStreaming() {
    std::lock_guard<std::mutex> lock(pImpl->queueMutex);
    pImpl->streaming_ = false;
}

bool StreamingBatchProcessor::isStreaming() const {
    std::lock_guard<std::mutex> lock(pImpl->queueMutex);
    return pImpl->streaming_;
}

BatchStats StreamingBatchProcessor::getStats() const {
    return pImpl->stats;
}

void StreamingBatchProcessor::addResultCallback(std::function<void(const BatchResult&)>& callback) {
    pImpl->resultCallbacks.push_back(callback);
}

void StreamingBatchProcessor::removeResultCallback(std::function<void(const BatchResult&)>& callback) {
    pImpl->resultCallbacks.erase(
        std::remove(pImpl->resultCallbacks.begin(), pImpl->resultCallbacks.end(), callback),
        pImpl->resultCallbacks.end()
    );
}

// BatchStatsExporter implementation
struct BatchStatsExporter::Impl {
    std::string format = "json";
    std::string filename = "batch_stats.json";
    bool includeTimestamps = false;
    bool includeDetailedStats = true;
    bool compressionEnabled = false;
    std::stringstream buffer;
};

BatchStatsExporter::BatchStatsExporter() : pImpl(std::make_unique<Impl>()) {}

BatchStatsExporter::~BatchStatsExporter() = default;

std::string BatchStatsExporter::exportToJSON(const BatchStats& stats) const {
    nlohmann::json jsonStats;
    
    jsonStats["totalBatchesProcessed"] = stats.totalBatchesProcessed;
    jsonStats["totalActionsProcessed"] = stats.totalActionsProcessed;
    jsonStats["totalActionsCompleted"] = stats.totalActionsCompleted;
    jsonStats["totalActionsFailed"] = stats.totalActionsFailed;
    jsonStats["averageBatchSize"] = stats.averageBatchSize;
    jsonStats["averageProcessingTime"] = stats.averageProcessingTime;
    jsonStats["throughput"] = stats.throughput;
    
    return jsonStats.dump(2);
}

std::string BatchStatsExporter::exportToCSV(const BatchStats& stats) const {
    std::stringstream ss;
    ss << "metric,value\n";
    ss << "totalBatchesProcessed," << stats.totalBatchesProcessed << "\n";
    ss << "totalActionsProcessed," << stats.totalActionsProcessed << "\n";
    ss << "totalActionsCompleted," << stats.totalActionsCompleted << "\n";
    ss << "totalActionsFailed," << stats.totalActionsFailed << "\n";
    ss << "averageBatchSize," << stats.averageBatchSize << "\n";
    ss << "averageProcessingTime," << stats.averageProcessingTime << "\n";
    ss << "throughput," << stats.throughput << "\n";
    
    return ss.str();
}

std::string BatchStatsExporter::exportToText(const BatchStats& stats) const {
    std::stringstream ss;
    ss << "Batch Processing Statistics\n";
    ss << "=========================\n\n";
    ss << "Total Batches Processed: " << stats.totalBatchesProcessed << "\n";
    ss << "Total Actions Processed: " << stats.totalActionsProcessed << "\n";
    ss << "Total Actions Completed: " << stats.totalActionsCompleted << "\n";
    ss << "Total Actions Failed: " << stats.totalActionsFailed << "\n";
    ss << "Average Batch Size: " << stats.averageBatchSize << "\n";
    ss << "Average Processing Time: " << stats.averageProcessingTime << "s\n";
    ss << "Throughput: " << stats.throughput << " actions/s\n";
    
    return ss.str();
}

void BatchStatsExporter::setExportFormat(const std::string& format) {
    format = format;
}

void BatchStatsExporter::setExportFile(const std::string& filename) {
    filename = filename;
}

void BatchStatsExporter::setIncludeTimestamps(bool include) {
    includeTimestamps = include;
}

void BatchStatsExporter::setIncludeDetailedStats(bool include) {
    includeDetailedStats = include;
}

void BatchStatsExporter::exportStats(const BatchStats& stats, const std::string& filename) {
    std::string file = filename.empty() ? pImpl->filename : filename;
    std::string format = pImpl->format;
    
    std::string data;
    if (format == "json") {
        data = exportToJSON(stats);
    } else if (format == "csv") {
        data = exportToCSV(stats);
    } else {
        data = exportToText(stats);
    }
    
    if (pImpl->compressionEnabled) {
        // Simple compression - in real implementation would use proper compression
        data = "compressed:" + std::to_string(std::hash<std::string>{}(data));
    }
    
    pImpl->buffer.str(std::string());
    pImpl->buffer << data;
}

std::string BatchStatsExporter::getExportedData() const {
    return pImpl->buffer.str();
}

void BatchStatsExporter::enableCompression(bool enable) {
    compressionEnabled = enable;
}

bool BatchStatsExporter::isCompressionEnabled() const {
    return compressionEnabled;
}

// BatchUtils namespace implementations
namespace BatchUtils {
    
    double measureProcessingTime(std::function<void()> task) {
        auto start = std::chrono::high_resolution_clock::now();
        task();
        auto end = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        return duration.count() / 1000000.0; // Convert to seconds
    }
    
    double calculateThroughput(size_t count, double time) {
        return time > 0.0 ? count / time : 0.0;
    }
    
    double calculateEfficiency(double idealTime, double actualTime) {
        return idealTime > 0.0 ? (idealTime / actualTime) * 100.0 : 0.0;
    }
    
    std::vector<float> normalizeResults(const std::vector<ActionResult>& results) {
        std::vector<float> normalized;
        
        if (results.empty()) {
            return normalized;
        }
        
        float minVal = std::numeric_limits<float>::max();
        float maxVal = std::numeric_limits<float>::lowest();
        
        for (const auto& result : results) {
            const auto& resultVals = result.getResults();
            for (float val : resultVals) {
                minVal = std::min(minVal, val);
                maxVal = std::max(maxVal, val);
            }
        }
        
        float range = maxVal - minVal;
        if (range > 0.0f) {
            for (const auto& result : results) {
                const auto& resultVals = result.getResults();
                for (float val : resultVals) {
                    normalized.push_back((val - minVal) / range);
                }
            }
        } else {
            for (const auto& result : results) {
                const auto& resultVals = result.getResults();
                for (float val : resultVals) {
                    normalized.push_back(val);
                }
            }
        }
        
        return normalized;
    }
    
    std::vector<ActionResult> filterResults(const std::vector<ActionResult>& results,
                                           std::function<bool(const ActionResult&)> predicate) {
        std::vector<ActionResult> filtered;
        for (const auto& result : results) {
            if (predicate(result)) {
                filtered.push_back(result);
            }
        }
        return filtered;
    }
    
    std::vector<std::string> validateBatchConfig(const BatchConfig& config) {
        std::vector<std::string> errors;
        
        if (config.batchSize == 0) {
            errors.push_back("Batch size must be greater than 0");
        }
        
        if (config.maxProcessingTime <= 0.0f) {
            errors.push_back("Max processing time must be greater than 0");
        }
        
        if (config.maxThreads <= 0) {
            errors.push_back("Max threads must be greater than 0");
        }
        
        if (config.timeoutPerAction <= 0.0f) {
            errors.push_back("Timeout per action must be greater than 0");
        }
        
        return errors;
    }
    
    bool validateActionPriority(ActionPriority priority) {
        return priority != ActionPriority::Discardable; // Basic validation
    }
    
    void logBatchStats(const BatchStats& stats) {
        // Logging implementation
        std::cout << "BatchStats: " << stats.totalBatchesProcessed << " batches, "
                  << stats.totalActionsProcessed << " actions, "
                  << stats.throughput << " throughput\n";
    }
    
    void logBatchResult(const BatchResult& result) {
        // Logging implementation
        std::cout << "BatchResult: " << result.completed << " - "
                  << result.successfulActions << " successful, "
                  << result.failedActions << " failed in "
                  << result.processingTime << "s\n";
    }
}

} // namespace nlm
