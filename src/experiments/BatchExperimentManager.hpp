#pragma once

#include "ExperimentRunner.hpp"
#include "ExperimentResult.hpp"
#include "BrainClone.hpp"
#include "EnvironmentClone.hpp"
#include <memory>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <queue>
#include <future>
#include <map>

namespace nlm {

class BatchExperimentManager {
public:
    // Batch execution mode
    enum class ExecutionMode {
        SEQUENTIAL,
        PARALLEL,
        PIPELINED,
        INTERLEAVED
    };
    
    // Batch status
    enum class BatchStatus {
        IDLE,
        RUNNING,
        PAUSED,
        COMPLETED,
        FAILED,
        CANCELLED
    };
    
    // Batch configuration
    struct BatchConfig {
        size_t maxConcurrentExperiments;
        SimulationStep maxStepsPerExperiment;
        bool useBrainCloning;
        bool useEnvironmentCloning;
        BrainClone::CloneMode brainCloneMode;
        EnvironmentClone::CloneMode envCloneMode;
        double timeoutSeconds;
        bool checkpointEnabled;
        std::string checkpointPath;
        bool pauseOnError;
        std::vector<std::string> excludedExperiments;
        std::map<std::string, std::string> experimentParams;
        
        BatchConfig() : maxConcurrentExperiments(4),
                       maxStepsPerExperiment(1000),
                       useBrainCloning(true),
                       useEnvironmentCloning(true),
                       brainCloneMode(BrainClone::CloneMode::DEEP),
                       envCloneMode(EnvironmentClone::CloneMode::DEEP),
                       timeoutSeconds(300.0),
                       checkpointEnabled(false),
                       pauseOnError(false) {}
    };
    
    // Batch result summary
    struct BatchResultSummary {
        size_t totalExperiments;
        size_t completedExperiments;
        size_t failedExperiments;
        size_t pausedExperiments;
        double totalDurationMs;
        double avgExperimentDurationMs;
        std::vector<std::string> successfulExperimentNames;
        std::vector<std::string> failedExperimentNames;
        std::vector<std::string> pausedExperimentNames;
        
        BatchResultSummary() : totalExperiments(0),
                              completedExperiments(0),
                              failedExperiments(0),
                              pausedExperiments(0),
                              totalDurationMs(0.0),
                              avgExperimentDurationMs(0.0) {}
    };

public:
    BatchExperimentManager();
    ~BatchExperimentManager();
    
    // Disable copying
    BatchExperimentManager(const BatchExperimentManager&) = delete;
    BatchExperimentManager& operator=(const BatchExperimentManager&) = delete;
    
    // Configure batch execution
    void configure(const BatchConfig& config);
    
    // Get current configuration
    const BatchConfig& getConfig() const { return config_; }
    
    // Execute batch of experiments
    std::vector<ExperimentResult> executeBatch(
        const std::vector<std::shared_ptr<Experiment>>& experiments,
        std::shared_ptr<Brain> brainTemplate,
        std::shared_ptr<Environment> environmentTemplate
    );
    
    // Execute batch with callback for progress
    std::vector<ExperimentResult> executeBatch(
        const std::vector<std::shared_ptr<Experiment>>& experiments,
        std::shared_ptr<Brain> brainTemplate,
        std::shared_ptr<Environment> environmentTemplate,
        std::function<void(size_t, size_t, const std::string&)> progressCallback
    );
    
    // Get batch status
    BatchStatus getStatus() const { return status_; }
    
    // Check if batch is running
    bool isRunning() const { return status_ == BatchStatus::RUNNING; }
    
    // Check if batch is paused
    bool isPaused() const { return status_ == BatchStatus::PAUSED; }
    
    // Pause batch execution
    void pause();
    
    // Resume batch execution
    void resume();
    
    // Cancel batch execution
    void cancel();
    
    // Get current batch results
    const std::vector<ExperimentResult>& getResults() const { return results_; }
    
    // Get current batch result summary
    BatchResultSummary getResultSummary() const;
    
    // Save batch results to file
    bool saveResults(const std::string& filepath) const;
    
    // Load batch results from file
    bool loadResults(const std::string& filepath);
    
    // Clear batch results
    void clearResults() { results_.clear(); }
    
    // Get experiment results by name
    std::vector<ExperimentResult> getExperimentResults(const std::string& experimentName) const;
    
    // Compare batch results with reference
    std::vector<bool> compareWithReference(
        const std::vector<ExperimentResult>& referenceResults,
        double tolerance = 1e-6
    ) const;
    
    // Aggregate batch results
    ExperimentResult aggregateResults(const std::string& metricName, const std::string& aggregationType = "mean") const;
    
    // Get batch performance metrics
    struct PerformanceMetrics {
        double throughputExperimentsPerSecond;
        double avgExperimentTimeMs;
        double cpuUtilization;
        double memoryUsageMB;
        std::vector<double> experimentTimes;
    };
    
    PerformanceMetrics getPerformanceMetrics() const;
    
    // Set progress callback
    void setProgressCallback(std::function<void(size_t, size_t, const std::string&)> callback) {
        progressCallback_ = callback;
    }
    
    // Check if checkpoint file exists
    bool checkpointExists() const;
    
    // Save checkpoint
    bool saveCheckpoint() const;
    
    // Load checkpoint
    bool loadCheckpoint();
    
    // Create batch configuration from file
    static BatchConfig createConfigFromFile(const std::string& filepath);
    
    // Save batch configuration to file
    bool saveConfigToFile(const std::string& filepath) const;
    
    // Get batch version
    static std::string getVersion() { return "1.0.0"; }
    
    // Reset batch manager to initial state
    void reset();
    
    // Validate batch configuration
    bool validateConfig() const;
    
    // Estimate batch execution time
    double estimateExecutionTime(const std::vector<std::shared_ptr<Experiment>>& experiments) const;

private:
    BatchConfig config_;
    BatchStatus status_;
    std::vector<ExperimentResult> results_;
    std::function<void(size_t, size_t, const std::string&)> progressCallback_;
    
    // Threading and synchronization
    std::vector<std::thread> workerThreads_;
    std::mutex resultsMutex_;
    std::condition_variable resultsCondition_;
    std::atomic<bool> cancellationFlag_;
    mutable std::mutex statusMutex_;
    
    // Pipelining support
    std::queue<std::shared_ptr<Experiment>> experimentQueue_;
    std::queue<std::shared_ptr<Brain>> brainQueue_;
    std::queue<std::shared_ptr<Environment>> environmentQueue_;
    std::mutex queueMutex_;
    std::condition_variable queueCondition_;
    
    // Timing and progress tracking
    std::chrono::system_clock::time_point startTime_;
    std::chrono::system_clock::time_point pauseTime_;
    double totalPausedTimeMs_;
    
    // Execution mode handlers
    std::vector<ExperimentResult> executeSequential(
        const std::vector<std::shared_ptr<Experiment>>& experiments,
        std::shared_ptr<Brain> brainTemplate,
        std::shared_ptr<Environment> environmentTemplate
    );
    
    std::vector<ExperimentResult> executeParallel(
        const std::vector<std::shared_ptr<Experiment>>& experiments,
        std::shared_ptr<Brain> brainTemplate,
        std::shared_ptr<Environment> environmentTemplate
    );
    
    std::vector<ExperimentResult> executePipelined(
        const std::vector<std::shared_ptr<Experiment>>& experiments,
        std::shared_ptr<Brain> brainTemplate,
        std::shared_ptr<Environment> environmentTemplate
    );
    
    std::vector<ExperimentResult> executeInterleaved(
        const std::vector<std::shared_ptr<Experiment>>& experiments,
        std::shared_ptr<Brain> brainTemplate,
        std::shared_ptr<Environment> environmentTemplate
    );
    
    // Helper methods
    std::shared_ptr<Brain> createBrainForExperiment(
        std::shared_ptr<Brain> brainTemplate,
        const std::string& experimentName
    );
    
    std::shared_ptr<Environment> createEnvironmentForExperiment(
        std::shared_ptr<Environment> envTemplate,
        const std::string& experimentName
    );
    
    void updateProgress(size_t completed, size_t total, const std::string& currentExperiment) const;
    
    double calculateExperimentTimeout(const std::chrono::system_clock::time_point& startTime) const;
    
    bool isExperimentExcluded(const std::string& experimentName) const;
    
    void applyExperimentParams(std::shared_ptr<Experiment> experiment) const;
};

} // namespace nlm