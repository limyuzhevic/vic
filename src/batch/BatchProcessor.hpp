// Batch Processor
// This class provides batch processing capabilities for NLM

#pragma once

#include <string>
#include <vector>
#include <map>
#include <thread>
#include <future>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <unordered_map>

namespace nlm {
namespace batch {

class BatchProcessor {
public:
    BatchProcessor();
    ~BatchProcessor();
    
    // Batch execution control
    bool startBatch(const std::string& batchName);
    void stopBatch();
    void pauseBatch();
    void resumeBatch();
    
    // Neural network simulation batch
    bool runNeuralNetworkBatch(const std::vector<std::string>& configurations, int iterations);
    bool runNeuralNetworkBatchWithParameters(const std::vector<std::map<std::string, std::string>>& paramSets, int iterations);
    
    // Configuration testing batch
    bool runConfigurationTestBatch(const std::vector<std::string>& configFiles, const std::string& testType = "validation");
    bool runConfigurationOptimizationBatch(const std::string& parameterSpace, int iterations);
    
    // Experiment running batch
    bool runExperimentBatch(const std::vector<std::string>& experimentFiles, const std::string& environment = "default");
    bool runExperimentBatchWithVariations(const std::string& baseExperiment, const std::vector<std::map<std::string, std::string>>& variations);
    
    // Performance testing batch
    bool runPerformanceTestBatch(const std::string& workload, int iterations, const std::string& benchmarkType = "standard");
    bool runLoadTestBatch(const std::string& testType, int concurrentUsers, int duration);
    
    // Parallel execution support
    bool enableParallelExecution(int threadCount);
    bool disableParallelExecution();
    bool runParallel(const std::vector<std::function<bool()>>& tasks);
    int getParallelThreadCount() const { return parallelThreadCount_; }
    
    // Batch statistics and analysis
    struct BatchStats {
        std::string batchName;
        std::chrono::nanoseconds startTime;
        std::chrono::nanoseconds endTime;
        std::chrono::nanoseconds totalDuration;
        int taskCount;
        int completedTasks;
        int failedTasks;
        double successRate;
        
        BatchStats() : startTime(0), endTime(0), totalDuration(0), taskCount(0), 
                      completedTasks(0), failedTasks(0), successRate(0) {}
    };
    
    BatchStats getBatchStats() const;
    std::string getBatchReport() const;
    
    // Batch results management
    bool saveBatchResults(const std::string& filepath);
    bool loadBatchResults(const std::string& filepath);
    bool exportBatchResults(const std::string& format, const std::string& outputFile);
    
    // Batch monitoring
    bool enableBatchMonitoring();
    bool disableBatchMonitoring();
    bool isMonitoringEnabled() const { return batchMonitoringEnabled_; }
    
    // Batch configuration
    bool setBatchConfiguration(const std::map<std::string, std::string>& config);
    bool setBatchParameter(const std::string& name, const std::string& value);
    std::string getBatchParameter(const std::string& name) const;
    
    // Batch status
    bool isBatchActive() const { return batchActive_; }
    std::string getBatchStatus() const;
    
private:
    bool batchActive_;
    bool batchMonitoringEnabled_;
    int parallelThreadCount_;
    
    std::chrono::nanoseconds batchStartTime_;
    std::chrono::nanoseconds batchEndTime_;
    
    mutable std::mutex mutex_;
    
    std::vector<std::future<bool>> parallelTasks_;
    
    // Batch execution
    bool executeTask(const std::function<bool()>& task);
    void executeParallelTask(const std::function<bool()>& task, std::vector<bool>& results);
    
    // Task management
    void addTask(const std::function<bool()>& task);
    void clearTasks();
    
    // Results management
    std::vector<bool> executeBatch(const std::vector<std::function<bool()>>& tasks);
    std::map<std::string, std::string> collectBatchParameters() const;
    
    // Monitoring
    void updateBatchStats();
    void logBatchProgress();
    
    // Report generation
    std::string generateBatchStatsReport() const;
    std::string generateTaskReport(const std::vector<bool>& taskResults) const;
};

} // namespace batch
} // namespace nlm
