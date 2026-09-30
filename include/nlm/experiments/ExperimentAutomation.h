#pragma once

#include <vector>
#include <string>
#include <memory>
#include <thread>
#include <future>
#include <map>
#include <chrono>
#include <functional>
#include <unordered_map>
#include "../experiments/AdvancedExperiment.h"

namespace nlm {

/**
 * Experiment automation and batch processing system
 * for running multiple experiments in parallel and managing experiment
 * workflows with advanced scheduling and monitoring capabilities.
 */
class ExperimentAutomation {
public:
    // Experiment execution result
    struct ExecutionResult {
        std::string experimentName;
        bool success;
        std::chrono::duration<double> executionTime;
        double progress;
        std::string status;
        std::string errorMessage;
        std::shared_ptr<ExperimentResults> results;
        
        ExecutionResult() : success(false), progress(0.0) {}
    };
    
    // Batch execution configuration
    struct BatchConfig {
        size_t maxConcurrentExperiments;
        double timeoutSeconds;
        std::string outputDirectory;
        bool saveIntermediateResults;
        bool continueOnError;
        std::vector<std::string> experimentOrder;
        std::function<bool(const std::string&)> experimentFilter;
        std::function<void(double)> progressCallback;
        
        BatchConfig() : maxConcurrentExperiments(4), timeoutSeconds(3600.0),
                       saveIntermediateResults(true), continueOnError(false),
                       progress(0.0) {}
    };
    
    // Experiment workflow definition
    struct ExperimentWorkflow {
        std::string name;
        std::vector<std::string> experimentIds;
        std::map<std::string, std::string> dependencies;
        std::string outputPrefix;
        bool runSequential;
        
        ExperimentWorkflow() : runSequential(false) {}
    };
    
    // Automation statistics
    struct AutomationStats {
        size_t totalExperiments;
        size_t successfulExperiments;
        size_t failedExperiments;
        std::chrono::duration<double> totalExecutionTime;
        double averageExecutionTime;
        double successRate;
        
        AutomationStats() : totalExperiments(0), successfulExperiments(0),
                          failedExperiments(0), totalExecutionTime(0.0),
                          averageExecutionTime(0.0), successRate(0.0) {}
    };
    
    ExperimentAutomation();
    ~ExperimentAutomation() = default;
    
    // Experiment execution
    ExecutionResult runExperiment(std::shared_ptr<AdvancedExperiment> experiment);
    std::vector<ExecutionResult> runExperiments(
        const std::vector<std::shared_ptr<AdvancedExperiment>>& experiments);
    
    // Batch execution
    std::vector<ExecutionResult> runBatch(
        const std::vector<std::shared_ptr<AdvancedExperiment>>& experiments,
        const BatchConfig& config = BatchConfig()
    );
    
    // Workflow management
    bool addWorkflow(const ExperimentWorkflow& workflow);
    bool executeWorkflow(const std::string& workflowName);
    std::vector<ExecutionResult> executeAllWorkflows();
    
    // Experiment comparison and analysis
    void compareExperiments(const std::vector<std::shared_ptr<AdvancedExperiment>>& experiments);
    std::string generateComparativeReport(
        const std::vector<std::shared_ptr<AdvancedExperiment>>& experiments
    ) const;
    
    // Results management
    void saveResults(const std::string& filepath,
                    const std::vector<std::shared_ptr<AdvancedExperiment>>& experiments);
    bool loadResults(const std::string& filepath,
                    std::vector<std::shared_ptr<AdvancedExperiment>>& experiments);
    
    // Progress monitoring
    void setProgressCallback(std::function<void(double)> callback);
    double getProgress() const;
    
    // Configuration management
    void setBatchConfig(const BatchConfig& config);
    const BatchConfig& getBatchConfig() const;
    
    // Experiment management
    void addExperiment(std::shared_ptr<AdvancedExperiment> experiment);
    void removeExperiment(const std::string& name);
    bool hasExperiment(const std::string& name) const;
    std::shared_ptr<AdvancedExperiment> getExperiment(const std::string& name) const;
    const std::vector<std::shared_ptr<AdvancedExperiment>>& getExperiments() const;
    
    // Statistics and reporting
    AutomationStats getStats() const;
    std::string generateReport() const;
    
    // Control functions
    void pauseExecution();
    void resumeExecution();
    void stopExecution();
    bool isPaused() const;
    bool isRunning() const;
    
    // Experiment validation
    bool validateExperiment(std::shared_ptr<AdvancedExperiment> experiment) const;
    std::vector<std::string> getExperimentValidationErrors(
        std::shared_ptr<AdvancedExperiment> experiment
    ) const;
    
private:
    // Internal execution helpers
    ExecutionResult executeSingleExperiment(std::shared_ptr<AdvancedExperiment> experiment);
    void executeBatchParallel(const std::vector<std::shared_ptr<AdvancedExperiment>>& experiments,
                             const BatchConfig& config);
    
    // Workflow execution
    std::vector<ExecutionResult> executeWorkflowSequential(const ExperimentWorkflow& workflow);
    std::vector<ExecutionResult> executeWorkflowParallel(const ExperimentWorkflow& workflow);
    
    // Progress tracking
    void updateProgress(double progress);
    
    // Result comparison
    void compareResults(const std::vector<ExecutionResult>& results);
    
    // Configuration storage
    BatchConfig batchConfig;
    
    // State management
    bool paused;
    bool running;
    std::atomic<double> progress;
    
    // Experiment storage
    std::vector<std::shared_ptr<AdvancedExperiment>> experiments;
    std::map<std::string, size_t> experimentIndex;
    
    // Workflow storage
    std::map<std::string, ExperimentWorkflow> workflows;
    
    // Results and statistics
    std::vector<ExecutionResult> executionHistory;
    AutomationStats stats;
    
    // Callbacks and threads
    std::function<void(double)> progressCallback;
    mutable std::mutex executionMutex;
    
    // Thread pool for parallel execution
    std::vector<std::future<ExecutionResult>> futures;
};

} // namespace nlm