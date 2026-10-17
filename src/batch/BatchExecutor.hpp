// NLM Batch Executor
// Executes batch experiments and manages experiment workflows

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <thread>
#include <future>
#include <chrono>
#include <mutex>
#include "experiments/Experiment.hpp"
#include "core/Config/Config.hpp"
#include "script/ScriptProcessor.hpp"

namespace nlm {

// Experiment execution result
struct ExperimentExecutionResult {
    std::string experimentName;
    bool success = false;
    std::string errorMessage;
    double executionTime = 0.0;
    std::string timestamp;
    std::shared_ptr<Experiment> experiment;
    std::map<std::string, std::string> metrics;
    
    ExperimentExecutionResult() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        timestamp = std::ctime(&time);
        timestamp.erase(timestamp.find_last_not_of("\n\r") + 1);
    }
};

// Batch execution configuration
struct BatchExecutionConfig {
    std::string mode;  // sequential, parallel, mixed
    size_t maxConcurrentExperiments = 1;
    double timeoutSeconds = 0.0;  // 0 = no timeout
    bool continueOnError = false;
    bool saveCheckpoints = false;
    std::string checkpointFile;
    std::string outputFormat;  // json, csv, text
    std::string outputFile;
    bool collectResults = true;
    std::string resultAggregationMode;  // average, max, min, sum
    
    BatchExecutionConfig() {
        mode = "sequential";
        maxConcurrentExperiments = 4;
        outputFormat = "json";
        collectResults = true;
        resultAggregationMode = "average";
    }
};

// Experiment chaining configuration
struct ExperimentChain {
    std::vector<std::string> experimentSequence;
    std::map<std::string, std::string> dependencies;
    std::map<std::string, std::string> conditions;
    bool executeSequentially = true;
    std::string chainName;
    
    ExperimentChain() {}
    ExperimentChain(const std::vector<std::string>& sequence) : experimentSequence(sequence) {}
};

// Main batch executor class
class BatchExecutor {
public:
    BatchExecutor();
    ~BatchExecutor();
    
    // Execute batch of experiments
    std::vector<ExperimentExecutionResult> executeBatch(
        const std::vector<std::shared_ptr<Experiment>>& experiments,
        const BatchExecutionConfig& config);
    
    // Execute experiments with parameter sweep
    std::vector<ExperimentExecutionResult> executeWithParameterSweep(
        const std::shared_ptr<Experiment>& experimentTemplate,
        const ScriptOptions& sweepOptions,
        const BatchExecutionConfig& config);
    
    // Execute grid search
    std::vector<ExperimentExecutionResult> executeGridSearch(
        const std::shared_ptr<Experiment>& experimentTemplate,
        const ScriptOptions& gridOptions,
        const BatchExecutionConfig& config);
    
    // Execute experiment chain
    std::vector<ExperimentExecutionResult> executeChain(
        const ExperimentChain& chain,
        const std::map<std::string, std::shared_ptr<Experiment>>& experimentMap,
        const BatchExecutionConfig& config);
    
    // Execute single experiment
    ExperimentExecutionResult executeExperiment(
        const std::shared_ptr<Experiment>& experiment,
        const BatchExecutionConfig& config);
    
    // Save execution checkpoint
    bool saveBatchCheckpoint(const std::vector<ExperimentExecutionResult>& results,
                            const std::string& checkpointFile);
    
    // Load execution checkpoint
    bool loadBatchCheckpoint(std::vector<ExperimentExecutionResult>& results,
                            const std::string& checkpointFile);
    
    // Get execution statistics
    struct ExecutionStats {
        uint64_t totalExperiments = 0;
        uint64_t successfulExperiments = 0;
        uint64_t failedExperiments = 0;
        double totalExecutionTime = 0.0;
        double averageExecutionTime = 0.0;
        std::string startTime;
        std::string endTime;
    };
    
    ExecutionStats getExecutionStats() const { return stats; }
    
    // Reset statistics
    void resetStats() { stats = ExecutionStats(); }
    
    // Static batch executor instance
    static BatchExecutor& getInstance() {
        static BatchExecutor instance;
        return instance;
    }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    ExecutionStats stats;
    mutable std::mutex statsMutex;
    
    // Internal execution methods
    ExperimentExecutionResult executeExperimentInternal(
        const std::shared_ptr<Experiment>& experiment,
        const BatchExecutionConfig& config,
        const std::map<std::string, std::string>& parameters);
    
    // Parallel execution helpers
    ExperimentExecutionResult executeExperimentInThread(
        const std::shared_ptr<Experiment>& experiment,
        const std::map<std::string, std::string>& parameters);
    
    // Generate experiment from template with parameters
    std::shared_ptr<Experiment> createExperimentFromTemplate(
        const std::shared_ptr<Experiment>& templateExperiment,
        const std::map<std::string, std::string>& parameters);
    
    // Check if experiment meets conditions
    bool checkConditions(const std::map<std::string, std::string>& conditions,
                        const std::map<std::string, std::string>& experimentData);
};

} // namespace nlm