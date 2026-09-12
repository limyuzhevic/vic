#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>
#include <unordered_map>
#include <algorithm>
#include "../brain/Brain.hpp"
#include "../environment/Environment.hpp"
#include "../experiments/Experiment.hpp"
#include "../performance/Performance.hpp"
#include "../debug/VisualizationInterface.hpp"
#include "../debug/BrainInspector.hpp"
#include "../debug/MemoryProfiler.hpp"

namespace nlm {

class BatchProcessor {
public:
    BatchProcessor();
    ~BatchProcessor();
    
    // Batch processing modes
    enum class ProcessingMode {
        Sequential,
        Parallel,
        Mixed,
        Pipeline
    };
    
    // Initialize batch processor
    bool initialize(ProcessingMode mode = ProcessingMode::Parallel, 
                   size_t maxConcurrency = std::thread::hardware_concurrency());
    
    // Batch experiment processing
    struct BatchResult {
        size_t experimentId;
        std::string experimentName;
        bool success;
        double executionTime;
        size_t totalSteps;
        size_t completedSteps;
        std::string status;
        std::vector<std::string> metrics;
    };
    
    // Run batch of experiments
    std::vector<BatchResult> runBatch(
        const std::vector<std::shared_ptr<Experiment>>& experiments,
        std::shared_ptr<Brain> brainTemplate,
        std::shared_ptr<Environment> environmentTemplate,
        SimulationStep maxSteps,
        double timeout = -1.0); // -1 = no timeout
    
    // Parallel execution
    std::vector<BatchResult> runParallel(
        const std::vector<std::shared_ptr<Experiment>>& experiments,
        const std::vector<std::pair<std::shared_ptr<Brain>, std::shared_ptr<Environment>>>& brainEnvironmentPairs,
        SimulationStep maxSteps,
        size_t workerCount = std::thread::hardware_concurrency());
    
    // Pipeline processing
    std::vector<BatchResult> runPipeline(
        const std::vector<std::shared_ptr<Experiment>>& experiments,
        std::shared_ptr<Brain> brainTemplate,
        std::shared_ptr<Environment> environmentTemplate,
        SimulationStep maxSteps);
    
    // Get batch results
    const std::vector<BatchResult>& getBatchResults() const;
    BatchResult getBatchResult(size_t index) const;
    
    // Batch management
    void clearBatchResults();
    bool saveBatchResults(const std::string& filename) const;
    
    // Progress tracking
    double getOverallProgress() const;
    std::string getCurrentBatchStatus() const;
    
    // Configuration
    void setMaxConcurrency(size_t count);
    void setMemoryLimit(size_t memoryBytes);
    void setTimeout(double timeoutSeconds);
    void enableProgressReporting(bool enable);
    void setProgressReportingInterval(double interval); // seconds
    
    // Progress callbacks
    using BatchProgressCallback = std::function<void(size_t completed, size_t total, double progress, const std::string& status)>;
    void setProgressCallback(BatchProgressCallback callback);
    
    // Performance monitoring
    void enablePerformanceMonitoring(bool enable);
    PerformanceStats getPerformanceStats() const;
    
    // Real-time visualization integration
    void enableVisualization(bool enable);
    void setVisualizationInterface(VisualizationInterface* viz);
    
    // Experiment optimization
    struct OptimizationSuggestion {
        std::string experimentName;
        std::string suggestion;
        double potentialSpeedup;
        std::string reason;
    };
    
    std::vector<OptimizationSuggestion> analyzeBatchForOptimization(
        const std::vector<std::shared_ptr<Experiment>>& experiments,
        std::shared_ptr<Brain> brainTemplate,
        std::shared_ptr<Environment> environmentTemplate,
        SimulationStep maxSteps) const;
    
    // Advanced features
    void enableDynamicLoadBalancing(bool enable);
    void enableCheckpointing(bool enable, const std::string& checkpointDir = ".");
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
