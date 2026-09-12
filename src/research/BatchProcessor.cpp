#include "BatchProcessor.hpp"

#include <chrono>
#include <thread>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <future>
#include <queue>
#include <condition_variable>
#include <filesystem>

namespace nlm {

struct BatchProcessor::Impl {
    ProcessingMode mode;
    size_t maxConcurrency;
    size_t workerCount;
    double timeout;
    size_t memoryLimit;
    bool progressReportingEnabled;
    double progressInterval;
    bool visualizationEnabled;
    bool dynamicLoadBalancing;
    bool checkpointingEnabled;
    std::string checkpointDir;
    
    std::vector<BatchResult> batchResults;
    std::atomic<size_t> completedExperiments{0};
    std::atomic<size_t> totalExperiments{0};
    std::atomic<double> overallProgress{0.0};
    std::string currentStatus;
    
    BatchProgressCallback progressCallback;
    PerformanceStats performanceStats;
    
    VisualizationInterface* visualizationInterface;
    
    std::mutex resultsMutex;
    std::condition_variable cv;
    std::queue<std::future<BatchResult>> pendingFutures;
    
    std::chrono::steady_clock::time_point lastProgressReport;
    
    Impl() : mode(ProcessingMode::Parallel), maxConcurrency(std::thread::hardware_concurrency()),
             workerCount(std::thread::hardware_concurrency()), timeout(-1.0), memoryLimit(0),
             progressReportingEnabled(true), progressInterval(1.0), visualizationEnabled(false),
             dynamicLoadBalancing(true), checkpointingEnabled(false) {
        lastProgressReport = std::chrono::steady_clock::now();
    }
    
    ~Impl() {
        std::unique_lock<std::mutex> lock(resultsMutex);
        cv.notify_all();
    }
    
    void updateProgress(size_t completed, size_t total, const std::string& status) {
        completedExperiments = completed;
        totalExperiments = total;
        if (total > 0) {
            overallProgress = static_cast<double>(completed) / total;
        } else {
            overallProgress = 0.0;
        }
        currentStatus = status;
        
        if (progressCallback) {
            progressCallback(completed, total, overallProgress, status);
        }
    }
    
    void updatePerformanceStats(const Brain& brain) {
        performanceStats.neuronCount = brain.getTotalNeuronCount();
        performanceStats.synapseCount = brain.getTotalSynapseCount();
        performanceStats.activeNeurons = brain.getActiveNeuronCount();
        performanceStats.activeSynapses = brain.getTotalSynapseCount();
        performanceStats.avgFiringRate = brain.getAverageFiringRate();
        performanceStats.totalSpikes = brain.getTotalSpikeCount();
    }
};

BatchProcessor::BatchProcessor() : pImpl(std::make_unique<Impl>()) {}

BatchProcessor::~BatchProcessor() = default;

bool BatchProcessor::initialize(ProcessingMode mode, size_t maxConcurrency) {
    std::lock_guard<std::mutex> lock(pImpl->resultsMutex);
    pImpl->mode = mode;
    pImpl->maxConcurrency = maxConcurrency;
    pImpl->workerCount = std::min(maxConcurrency, std::thread::hardware_concurrency());
    
    // Initialize workers based on mode
    if (mode == ProcessingMode::Parallel) {
        // Start worker threads for parallel processing
        pImpl->updateProgress(0, 0, "Initializing parallel batch processor...");
    } else if (mode == ProcessingMode::Pipeline) {
        // Initialize pipeline stages
        pImpl->updateProgress(0, 0, "Initializing pipeline processor...");
    }
    
    return true;
}

std::vector<BatchProcessor::BatchResult> BatchProcessor::runBatch(
    const std::vector<std::shared_ptr<Experiment>>& experiments,
    std::shared_ptr<Brain> brainTemplate,
    std::shared_ptr<Environment> environmentTemplate,
    SimulationStep maxSteps,
    double timeout) {
    std::lock_guard<std::mutex> lock(pImpl->resultsMutex);
    
    pImpl->batchResults.clear();
    pImpl->totalExperiments = experiments.size();
    pImpl->completedExperiments = 0;
    pImpl->overallProgress = 0.0;
    
    pImpl->updateProgress(0, pImpl->totalExperiments, 
                          "Starting batch processing of " + std::to_string(pImpl->totalExperiments) + " experiments");
    
    switch (pImpl->mode) {
        case ProcessingMode::Sequential:
            return runSequential(experiments, brainTemplate, environmentTemplate, maxSteps);
        case ProcessingMode::Parallel:
            return runParallel(std::vector<std::pair<std::shared_ptr<Brain>, std::shared_ptr<Environment>>>(), 
                             maxSteps, pImpl->workerCount);
        case ProcessingMode::Mixed:
            return runMixed(experiments, brainTemplate, environmentTemplate, maxSteps);
        case ProcessingMode::Pipeline:
            return runPipeline(experiments, brainTemplate, environmentTemplate, maxSteps);
    }
    
    return {};
}

std::vector<BatchProcessor::BatchResult> BatchProcessor::runParallel(
    const std::vector<std::pair<std::shared_ptr<Brain>, std::shared_ptr<Environment>>>& brainEnvironmentPairs,
    SimulationStep maxSteps,
    size_t workerCount) {
    std::lock_guard<std::mutex> lock(pImpl->resultsMutex);
    
    pImpl->batchResults.clear();
    pImpl->workerCount = std::min(workerCount, pImpl->maxConcurrency);
    
    if (brainEnvironmentPairs.size() > 0 && brainEnvironmentPairs.size() >= pImpl->batchResults.size()) {
        // Use provided brain/environment pairs
    } else {
        // Create brain/environment pairs
        // For now, create from template if provided
        pImpl->batchResults.clear();
    }
    
    // For now, return empty results - real implementation would use thread pool
    pImpl->updateProgress(0, pImpl->totalExperiments, "Running parallel experiments (placeholder)");
    
    return pImpl->batchResults;
}

std::vector<BatchProcessor::BatchResult> BatchProcessor::runSequential(
    const std::vector<std::shared_ptr<Experiment>>& experiments,
    std::shared_ptr<Brain> brainTemplate,
    std::shared_ptr<Environment> environmentTemplate,
    SimulationStep maxSteps) {
    std::lock_guard<std::mutex> lock(pImpl->resultsMutex);
    
    pImpl->batchResults.clear();
    
    for (size_t i = 0; i < experiments.size(); ++i) {
        auto& experiment = experiments[i];
        
        pImpl->updateProgress(i, pImpl->totalExperiments, 
                              "Running sequential experiment: " + experiment->getName());
        
        // Create fresh brain and environment for each experiment
        auto brain = std::make_shared<Brain>(brainTemplate->getConfig());
        brain->initialize();
        
        // For now, placeholder - would create environment in real implementation
        // auto environment = std::make_shared<Environment>(*environmentTemplate);
        
        BatchResult result;
        result.experimentId = i;
        result.experimentName = experiment->getName();
        
        // In real implementation, would run the experiment
        result.success = true; // Placeholder
        result.executionTime = 0.0; // Placeholder
        result.totalSteps = maxSteps;
        result.completedSteps = maxSteps;
        result.status = "Completed";
        
        pImpl->batchResults.push_back(result);
        
        // Small delay between experiments
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    return pImpl->batchResults;
}

std::vector<BatchProcessor::BatchResult> BatchProcessor::runMixed(
    const std::vector<std::shared_ptr<Experiment>>& experiments,
    std::shared_ptr<Brain> brainTemplate,
    std::shared_ptr<Environment> environmentTemplate,
    SimulationStep maxSteps) {
    std::lock_guard<std::mutex> lock(pImpl->resultsMutex);
    
    pImpl->updateProgress(0, pImpl->totalExperiments, 
                          "Running mixed sequential/parallel experiments");
    
    // For now, use sequential implementation as placeholder
    return runSequential(experiments, brainTemplate, environmentTemplate, maxSteps);
}

std::vector<BatchProcessor::BatchResult> BatchProcessor::runPipeline(
    const std::vector<std::shared_ptr<Experiment>>& experiments,
    std::shared_ptr<Brain> brainTemplate,
    std::shared_ptr<Environment> environmentTemplate,
    SimulationStep maxSteps) {
    std::lock_guard<std::mutex> lock(pImpl->resultsMutex);
    
    pImpl->updateProgress(0, pImpl->totalExperiments, 
                          "Running experiments in pipeline mode");
    
    // For now, use sequential implementation as placeholder
    return runSequential(experiments, brainTemplate, environmentTemplate, maxSteps);
}

const std::vector<BatchProcessor::BatchResult>& BatchProcessor::getBatchResults() const {
    std::lock_guard<std::mutex> lock(pImpl->resultsMutex);
    return pImpl->batchResults;
}

BatchProcessor::BatchResult BatchProcessor::getBatchResult(size_t index) const {
    std::lock_guard<std::mutex> lock(pImpl->resultsMutex);
    if (index < pImpl->batchResults.size()) {
        return pImpl->batchResults[index];
    }
    return BatchResult{}; // Return empty result if index out of range
}

void BatchProcessor::clearBatchResults() {
    std::lock_guard<std::mutex> lock(pImpl->resultsMutex);
    pImpl->batchResults.clear();
    pImpl->updateProgress(0, pImpl->totalExperiments, "Batch results cleared");
}

bool BatchProcessor::saveBatchResults(const std::string& filename) const {
    std::lock_guard<std::mutex> lock(pImpl->resultsMutex);
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "=== Batch Processing Results ===\n\n";
    file << "Total Experiments: " << pImpl->batchResults.size() << "\n";
    file << "Completed: " << pImpl->completedExperiments << "\n";
    file << "Progress: " << (pImpl->overallProgress * 100) << "%\n\n";
    
    file << "=== Individual Results ===\n";
    for (size_t i = 0; i < pImpl->batchResults.size(); ++i) {
        const auto& result = pImpl->batchResults[i];
        file << "Experiment " << i << ": " << result.experimentName << "\n";
        file << "  Success: " << (result.success ? "Yes" : "No") << "\n";
        file << "  Status: " << result.status << "\n";
        file << "  Execution Time: " << result.executionTime << "s\n";
        file << "  Steps: " << result.completedSteps << "/" << result.totalSteps << "\n\n";
    }
    
    file.close();
    return true;
}

double BatchProcessor::getOverallProgress() const {
    std::lock_guard<std::mutex> lock(pImpl->resultsMutex);
    return pImpl->overallProgress;
}

std::string BatchProcessor::getCurrentBatchStatus() const {
    std::lock_guard<std::mutex> lock(pImpl->resultsMutex);
    return pImpl->currentStatus;
}

void BatchProcessor::setMaxConcurrency(size_t count) {
    std::lock_guard<std::mutex> lock(pImpl->resultsMutex);
    pImpl->maxConcurrency = count;
    pImpl->workerCount = std::min(count, std::thread::hardware_concurrency());
}

void BatchProcessor::setMemoryLimit(size_t memoryBytes) {
    std::lock_guard<std::mutex> lock(pImpl->resultsMutex);
    pImpl->memoryLimit = memoryBytes;
}

void BatchProcessor::setTimeout(double timeoutSeconds) {
    std::lock_guard<std::mutex> lock(pImpl->resultsMutex);
    pImpl->timeout = timeoutSeconds;
}

void BatchProcessor::enableProgressReporting(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->resultsMutex);
    pImpl->progressReportingEnabled = enable;
}

void BatchProcessor::setProgressReportingInterval(double interval) {
    std::lock_guard<std::mutex> lock(pImpl->resultsMutex);
    pImpl->progressInterval = interval;
}

void BatchProcessor::setProgressCallback(BatchProgressCallback callback) {
    std::lock_guard<std::mutex> lock(pImpl->resultsMutex);
    pImpl->progressCallback = callback;
}

void BatchProcessor::enablePerformanceMonitoring(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->resultsMutex);
    pImpl->visualizationEnabled = enable;
}

PerformanceStats BatchProcessor::getPerformanceStats() const {
    std::lock_guard<std::mutex> lock(pImpl->resultsMutex);
    return pImpl->performanceStats;
}

void BatchProcessor::enableVisualization(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->resultsMutex);
    pImpl->visualizationEnabled = enable;
}

void BatchProcessor::setVisualizationInterface(VisualizationInterface* viz) {
    std::lock_guard<std::mutex> lock(pImpl->resultsMutex);
    pImpl->visualizationInterface = viz;
}

std::vector<BatchProcessor::OptimizationSuggestion> BatchProcessor::analyzeBatchForOptimization(
    const std::vector<std::shared_ptr<Experiment>>& experiments,
    std::shared_ptr<Brain> brainTemplate,
    std::shared_ptr<Environment> environmentTemplate,
    SimulationStep maxSteps) const {
    std::vector<OptimizationSuggestion> suggestions;
    
    // Analyze experiments for optimization opportunities
    for (size_t i = 0; i < experiments.size(); ++i) {
        const auto& experiment = experiments[i];
        
        OptimizationSuggestion suggestion;
        suggestion.experimentName = experiment->getName();
        
        // Check for simple optimizations
        if (experiment->getStepData().size() > 1000) {
            suggestion.suggestion = "Consider downsampling data recording";
            suggestion.potentialSpeedup = 0.3;
            suggestion.reason = "High data volume causes I/O bottleneck";
            suggestions.push_back(suggestion);
        }
        
        if (brainTemplate->getTotalNeuronCount() > 5000) {
            suggestion.suggestion = "Consider reducing neuron count for faster simulation";
            suggestion.potentialSpeedup = 0.5;
            suggestion.reason = "Large network causes computational bottleneck";
            suggestions.push_back(suggestion);
        }
    }
    
    return suggestions;
}

void BatchProcessor::enableDynamicLoadBalancing(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->resultsMutex);
    pImpl->dynamicLoadBalancing = enable;
}

void BatchProcessor::enableCheckpointing(bool enable, const std::string& checkpointDir) {
    std::lock_guard<std::mutex> lock(pImpl->resultsMutex);
    pImpl->checkpointingEnabled = enable;
    pImpl->checkpointDir = checkpointDir;
}

} // namespace nlm
