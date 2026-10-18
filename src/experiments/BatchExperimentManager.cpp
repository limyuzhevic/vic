#include "BatchExperimentManager.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>

namespace nlm {

BatchExperimentManager::BatchExperimentManager() : status_(BatchStatus::IDLE),
                                                   cancellationFlag_(false),
                                                   totalPausedTimeMs_(0.0) {}

BatchExperimentManager::~BatchExperimentManager() {
    cancel();
}

void BatchExperimentManager::configure(const BatchConfig& config) {
    std::lock_guard<std::mutex> lock(statusMutex_);
    config_ = config;
    
    if (status_ == BatchStatus::RUNNING) {
        status_ = BatchStatus::PAUSED;
    }
}

std::vector<ExperimentResult> BatchExperimentManager::executeBatch(
    const std::vector<std::shared_ptr<Experiment>>& experiments,
    std::shared_ptr<Brain> brainTemplate,
    std::shared_ptr<Environment> environmentTemplate
) {
    return executeBatch(experiments, brainTemplate, environmentTemplate, progressCallback_);
}

std::vector<ExperimentResult> BatchExperimentManager::executeBatch(
    const std::vector<std::shared_ptr<Experiment>>& experiments,
    std::shared_ptr<Brain> brainTemplate,
    std::shared_ptr<Environment> environmentTemplate,
    std::function<void(size_t, size_t, const std::string&)> progressCallback
) {
    std::lock_guard<std::mutex> lock(statusMutex_);
    
    if (status_ == BatchStatus::RUNNING) {
        throw std::runtime_error("Batch execution already in progress");
    }
    
    // Validate input
    if (experiments.empty()) {
        return {};
    }
    
    if (!brainTemplate || !environmentTemplate) {
        throw std::invalid_argument("Brain or environment template is null");
    }
    
    // Clear previous results
    {
        std::lock_guard<std::mutex> resultsLock(resultsMutex_);
        results_.clear();
    }
    
    status_ = BatchStatus::RUNNING;
    cancellationFlag_ = false;
    startTime_ = std::chrono::system_clock::now();
    pauseTime_ = std::chrono::system_clock::now();
    totalPausedTimeMs_ = 0.0;
    
    // Choose execution mode
    std::vector<ExperimentResult> allResults;
    switch (config_.executionMode) {
        case ExecutionMode::SEQUENTIAL:
            allResults = executeSequential(experiments, brainTemplate, environmentTemplate);
            break;
        case ExecutionMode::PARALLEL:
            allResults = executeParallel(experiments, brainTemplate, environmentTemplate);
            break;
        case ExecutionMode::PIPELINED:
            allResults = executePipelined(experiments, brainTemplate, environmentTemplate);
            break;
        case ExecutionMode::INTERLEAVED:
            allResults = executeInterleaved(experiments, brainTemplate, environmentTemplate);
            break;
    }
    
    status_ = BatchStatus::COMPLETED;
    return allResults;
}

void BatchExperimentManager::pause() {
    std::lock_guard<std::mutex> lock(statusMutex_);
    
    if (status_ == BatchStatus::RUNNING) {
        status_ = BatchStatus::PAUSED;
        pauseTime_ = std::chrono::system_clock::now();
    }
}

void BatchExperimentManager::resume() {
    std::lock_guard<std::mutex> lock(statusMutex_);
    
    if (status_ == BatchStatus::PAUSED) {
        status_ = BatchStatus::RUNNING;
        auto now = std::chrono::system_clock::now();
        totalPausedTimeMs_ += std::chrono::duration<double, std::milli>(now - pauseTime_).count();
    }
}

void BatchExperimentManager::cancel() {
    std::lock_guard<std::mutex> lock(statusMutex_);
    
    status_ = BatchStatus::CANCELLED;
    cancellationFlag_ = true;
    
    // Wait for threads to finish
    for (auto& thread : workerThreads_) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    
    workerThreads_.clear();
}

BatchExperimentManager::BatchResultSummary BatchExperimentManager::getResultSummary() const {
    BatchResultSummary summary;
    
    std::lock_guard<std::mutex> lock(resultsMutex_);
    
    summary.totalExperiments = results_.size();
    
    for (const auto& result : results_) {
        if (result.getStatus() == ExperimentResult::Status::SUCCESS) {
            summary.completedExperiments++;
            summary.successfulExperimentNames.push_back(result.getExperimentName());
        } else if (result.getStatus() == ExperimentResult::Status::ERROR) {
            summary.failedExperiments++;
            summary.failedExperimentNames.push_back(result.getExperimentName());
        } else if (result.getStatus() == ExperimentResult::Status::PAUSED) {
            summary.pausedExperiments++;
            summary.pausedExperimentNames.push_back(result.getExperimentName());
        }
    }
    
    if (!results_.empty()) {
        summary.totalDurationMs = 0.0;
        for (const auto& result : results_) {
            summary.totalDurationMs += result.getDuration();
        }
        summary.avgExperimentDurationMs = summary.totalDurationMs / results_.size();
    }
    
    return summary;
}

bool BatchExperimentManager::saveResults(const std::string& filepath) const {
    std::lock_guard<std::mutex> lock(resultsMutex_);
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "BatchExperimentManager Results:" << std::endl;
    file << "Version: " << getVersion() << std::endl;
    file << "Timestamp: " << std::chrono::system_clock::to_time_t(startTime_) << std::endl;
    file << "Total Experiments: " << results_.size() << std::endl;
    
    for (const auto& result : results_) {
        file << "--- Experiment: " << result.getExperimentName() << " ---" << std::endl;
        file << result.getSummary() << std::endl;
    }
    
    return true;
}

bool BatchExperimentManager::loadResults(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // In real implementation, would parse the file
    // For now, clear results and add placeholder
    {
        std::lock_guard<std::mutex> lock(resultsMutex_);
        results_.clear();
        
        // Create placeholder result
        ExperimentResult result;
        result.setExperimentName("Loaded from " + filepath);
        result.setStatus(ExperimentResult::Status::SUCCESS);
        results_.push_back(result);
    }
    
    return true;
}

std::vector<ExperimentResult> BatchExperimentManager::getExperimentResults(const std::string& experimentName) const {
    std::lock_guard<std::mutex> lock(resultsMutex_);
    
    std::vector<ExperimentResult> filtered;
    for (const auto& result : results_) {
        if (result.getExperimentName() == experimentName) {
            filtered.push_back(result);
        }
    }
    
    return filtered;
}

std::vector<bool> BatchExperimentManager::compareWithReference(
    const std::vector<ExperimentResult>& referenceResults,
    double tolerance
) const {
    std::vector<bool> matches;
    matches.reserve(results_.size());
    
    for (const auto& result : results_) {
        bool foundMatch = false;
        for (const auto& ref : referenceResults) {
            if (result.approximatelyEqual(ref, tolerance)) {
                foundMatch = true;
                break;
            }
        }
        matches.push_back(foundMatch);
    }
    
    return matches;
}

ExperimentResult BatchExperimentManager::aggregateResults(const std::string& metricName, const std::string& aggregationType) const {
    ExperimentResult aggregated;
    aggregated.setExperimentName("AGGREGATED");
    aggregated.setStatus(ExperimentResult::Status::SUCCESS);
    
    if (results_.empty()) return aggregated;
    
    std::vector<double> values;
    for (const auto& result : results_) {
        values.push_back(result.getMetric(metricName));
    }
    
    double aggregatedValue = 0.0;
    if (aggregationType == "mean" || aggregationType.empty()) {
        aggregatedValue = std::accumulate(values.begin(), values.end(), 0.0) / values.size();
    } else if (aggregationType == "sum") {
        aggregatedValue = std::accumulate(values.begin(), values.end(), 0.0);
    } else if (aggregationType == "max") {
        aggregatedValue = *std::max_element(values.begin(), values.end());
    } else if (aggregationType == "min") {
        aggregatedValue = *std::min_element(values.begin(), values.end());
    } else if (aggregationType == "median") {
        std::vector<double> sorted(values);
        std::sort(sorted.begin(), sorted.end());
        size_t mid = sorted.size() / 2;
        aggregatedValue = sorted[mid];
    }
    
    aggregated.setMetric(metricName, aggregatedValue);
    return aggregated;
}

BatchExperimentManager::PerformanceMetrics BatchExperimentManager::getPerformanceMetrics() const {
    PerformanceMetrics metrics;
    
    // Calculate throughput
    auto now = std::chrono::system_clock::now();
    double elapsedSeconds = std::chrono::duration<double>(now - startTime_).count() - (totalPausedTimeMs_ / 1000.0);
    
    if (elapsedSeconds > 0.0) {
        metrics.throughputExperimentsPerSecond = results_.size() / elapsedSeconds;
    } else {
        metrics.throughputExperimentsPerSecond = 0.0;
    }
    
    // Calculate average experiment time
    double totalDuration = 0.0;
    for (const auto& result : results_) {
        metrics.experimentTimes.push_back(result.getDuration());
        totalDuration += result.getDuration();
    }
    
    if (!results_.empty()) {
        metrics.avgExperimentTimeMs = totalDuration / results_.size();
    }
    
    return metrics;
}

bool BatchExperimentManager::checkpointExists() const {
    if (config_.checkpointPath.empty()) return false;
    
    std::ifstream file(config_.checkpointPath);
    return file.good();
}

bool BatchExperimentManager::saveCheckpoint() const {
    if (!config_.checkpointPath.empty()) {
        // In real implementation, would save detailed checkpoint
        std::ofstream file(config_.checkpointPath);
        if (file.is_open()) {
            file << "Checkpoint saved at " << std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) << std::endl;
            return true;
        }
    }
    return false;
}

bool BatchExperimentManager::loadCheckpoint() {
    if (checkpointExists()) {
        // In real implementation, would load checkpoint data
        return true;
    }
    return false;
}

BatchExperimentManager::BatchConfig BatchExperimentManager::createConfigFromFile(const std::string& filepath) {
    BatchConfig config;
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open config file: " + filepath);
    }
    
    // In real implementation, would parse YAML or JSON config file
    // For now, return default config
    return config;
}

bool BatchExperimentManager::saveConfigToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "BatchConfig:" << std::endl;
    file << "  maxConcurrentExperiments: " << config_.maxConcurrentExperiments << std::endl;
    file << "  maxStepsPerExperiment: " << config_.maxStepsPerExperiment << std::endl;
    file << "  useBrainCloning: " << (config_.useBrainCloning ? "true" : "false") << std::endl;
    file << "  useEnvironmentCloning: " << (config_.useEnvironmentCloning ? "true" : "false") << std::endl;
    file << "  brainCloneMode: " << static_cast<int>(config_.brainCloneMode) << std::endl;
    file << "  envCloneMode: " << static_cast<int>(config_.envCloneMode) << std::endl;
    file << "  timeoutSeconds: " << config_.timeoutSeconds << std::endl;
    file << "  checkpointEnabled: " << (config_.checkpointEnabled ? "true" : "false") << std::endl;
    if (!config_.checkpointPath.empty()) {
        file << "  checkpointPath: " << config_.checkpointPath << std::endl;
    }
    
    return true;
}

void BatchExperimentManager::reset() {
    std::lock_guard<std::mutex> lock(statusMutex_);
    
    status_ = BatchStatus::IDLE;
    cancellationFlag_ = false;
    
    {
        std::lock_guard<std::mutex> resultsLock(resultsMutex_);
        results_.clear();
    }
    
    workerThreads_.clear();
    experimentQueue_ = std::queue<std::shared_ptr<Experiment>>();
    brainQueue_ = std::queue<std::shared_ptr<Brain>>();
    environmentQueue_ = std::queue<std::shared_ptr<Environment>>();
    
    totalPausedTimeMs_ = 0.0;
}

bool BatchExperimentManager::validateConfig() const {
    if (config_.maxConcurrentExperiments == 0) return false;
    if (config_.maxStepsPerExperiment == 0) return false;
    if (config_.timeoutSeconds <= 0.0) return false;
    if (config_.useBrainCloning && !brainTemplate_) return false;
    if (config_.useEnvironmentCloning && !environmentTemplate_) return false;
    
    return true;
}

double BatchExperimentManager::estimateExecutionTime(const std::vector<std::shared_ptr<Experiment>>& experiments) const {
    if (experiments.empty()) return 0.0;
    
    double baseTime = config_.maxStepsPerExperiment * 10.0; // Estimate per experiment
    double totalTime = baseTime * experiments.size();
    
    // Adjust based on concurrent execution
    double concurrencyFactor = static_cast<double>(config_.maxConcurrentExperiments);
    double adjustedTime = totalTime / concurrencyFactor;
    
    return adjustedTime;
}

std::vector<ExperimentResult> BatchExperimentManager::executeSequential(
    const std::vector<std::shared_ptr<Experiment>>& experiments,
    std::shared_ptr<Brain> brainTemplate,
    std::shared_ptr<Environment> environmentTemplate
) {
    std::vector<ExperimentResult> results;
    results.reserve(experiments.size());
    
    for (size_t i = 0; i < experiments.size() && !cancellationFlag_; ++i) {
        updateProgress(i, experiments.size(), experiments[i]->getName());
        
        if (isExperimentExcluded(experiments[i]->getName())) {
            continue;
        }
        
        // Create experiment
        auto experiment = std::make_shared<Experiment>(experiments[i]->getName());
        experiment->setSeed(experiments[i]->getSeed());
        
        // Create brain
        std::shared_ptr<Brain> brain;
        if (config_.useBrainCloning) {
            brain = BrainClone::cloneFromTemplate(brainTemplate, *brainTemplate->getConfig(),
                                                  config_.brainCloneMode);
        } else {
            brain = std::make_shared<Brain>(brainTemplate->getConfig());
            brain->initialize();
        }
        
        // Create environment
        std::shared_ptr<Environment> env;
        if (config_.useEnvironmentCloning) {
            env = EnvironmentClone::cloneFromTemplate(environmentTemplate, config_.envCloneMode);
        } else {
            env = environmentTemplate;
        }
        
        // Run experiment
        if (runExperiment(experiment, brain, env, config_.maxStepsPerExperiment)) {
            results.push_back(ExperimentResult::fromExperiment(experiment, brain, env));
        }
    }
    
    return results;
}

std::vector<ExperimentResult> BatchExperimentManager::executeParallel(
    std::vector<std::shared_ptr<Experiment>>& experiments,
    std::shared_ptr<Brain> brainTemplate,
    std::shared_ptr<Environment> environmentTemplate
) {
    std::vector<ExperimentResult> results;
    results.reserve(experiments.size());
    
    std::mutex resultsMutex;
    std::condition_variable resultsCondition;
    std::atomic<size_t> completedCount(0);
    
    auto worker = [&](size_t startIdx, size_t endIdx) {
        std::vector<ExperimentResult> localResults;
        
        for (size_t i = startIdx; i < endIdx && !cancellationFlag_; ++i) {
            updateProgress(completedCount.load(), experiments.size(), experiments[i]->getName());
            
            if (isExperimentExcluded(experiments[i]->getName())) {
                completedCount++;
                continue;
            }
            
            auto experiment = std::make_shared<Experiment>(experiments[i]->getName());
            experiment->setSeed(experiments[i]->getSeed());
            
            std::shared_ptr<Brain> brain;
            if (config_.useBrainCloning) {
                brain = BrainClone::cloneFromTemplate(brainTemplate, *brainTemplate->getConfig(),
                                                      config_.brainCloneMode);
            } else {
                brain = std::make_shared<Brain>(brainTemplate->getConfig());
                brain->initialize();
            }
            
            std::shared_ptr<Environment> env;
            if (config_.useEnvironmentCloning) {
                env = EnvironmentClone::cloneFromTemplate(environmentTemplate, config_.envCloneMode);
            } else {
                env = environmentTemplate;
            }
            
            if (runExperiment(experiment, brain, env, config_.maxStepsPerExperiment)) {
                localResults.push_back(ExperimentResult::fromExperiment(experiment, brain, env));
            }
            
            completedCount++;
        }
        
        std::lock_guard<std::mutex> lock(resultsMutex);
        results.insert(results.end(), localResults.begin(), localResults.end());
        resultsCondition.notify_one();
    };
    
    size_t threadsPerExperiment = config_.maxConcurrentExperiments;
    std::vector<std::thread> threads;
    
    size_t experimentsPerThread = experiments.size() / threadsPerExperiment;
    size_t remainder = experiments.size() % threadsPerExperiment;
    
    for (size_t i = 0; i < threadsPerExperiment; ++i) {
        size_t start = i * experimentsPerThread;
        size_t count = experimentsPerThread + (i == threadsPerExperiment - 1 ? remainder : 0);
        
        threads.emplace_back(worker, start, start + count);
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    return results;
}

std::vector<ExperimentResult> BatchExperimentManager::executePipelined(
    const std::vector<std::shared_ptr<Experiment>>& experiments,
    std::shared_ptr<Brain> brainTemplate,
    std::shared_ptr<Environment> environmentTemplate
) {
    std::vector<ExperimentResult> results;
    results.reserve(experiments.size());
    
    std::mutex queueMutex;
    std::condition_variable queueCond;
    std::atomic<size_t> completedCount(0);
    
    // Start workers
    auto worker = [&]() {
        while (!cancellationFlag_) {
            std::shared_ptr<Experiment> experiment;
            {
                std::unique_lock<std::mutex> lock(queueMutex);
                if (experimentQueue_.empty()) {
                    queueCond.wait_for(lock, std::chrono::milliseconds(100));
                    continue;
                }
                experiment = experimentQueue_.front();
                experimentQueue_.pop();
            }
            
            updateProgress(completedCount.load(), experiments.size(), experiment->getName());
            
            if (isExperimentExcluded(experiment->getName())) {
                completedCount++;
                continue;
            }
            
            std::shared_ptr<Brain> brain;
            if (config_.useBrainCloning) {
                brain = BrainClone::cloneFromTemplate(brainTemplate, *brainTemplate->getConfig(),
                                                      config_.brainCloneMode);
            } else {
                brain = std::make_shared<Brain>(brainTemplate->getConfig());
                brain->initialize();
            }
            
            std::shared_ptr<Environment> env;
            if (config_.useEnvironmentCloning) {
                env = EnvironmentClone::cloneFromTemplate(environmentTemplate, config_.envCloneMode);
            } else {
                env = environmentTemplate;
            }
            
            if (runExperiment(experiment, brain, env, config_.maxStepsPerExperiment)) {
                std::lock_guard<std::mutex> lock(resultsMutex_);
                results.push_back(ExperimentResult::fromExperiment(experiment, brain, env));
            }
            
            completedCount++;
        }
    };
    
    // Fill experiment queue
    for (const auto& exp : experiments) {
        experimentQueue_.push(exp);
    }
    
    std::vector<std::thread> threads;
    for (size_t i = 0; i < config_.maxConcurrentExperiments; ++i) {
        threads.emplace_back(worker);
    }
    
    // Wait for all experiments to complete
    while (completedCount < experiments.size() && !cancellationFlag_) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    return results;
}

std::vector<ExperimentResult> BatchExperimentManager::executeInterleaved(
    const std::vector<std::shared_ptr<Experiment>>& experiments,
    std::shared_ptr<Brain> brainTemplate,
    std::shared_ptr<Environment> environmentTemplate
) {
    std::vector<ExperimentResult> results;
    results.reserve(experiments.size());
    
    // For now, use sequential execution as simplified implementation
    return executeSequential(experiments, brainTemplate, environmentTemplate);
}

std::shared_ptr<Brain> BatchExperimentManager::createBrainForExperiment(
    std::shared_ptr<Brain> brainTemplate,
    const std::string& experimentName
) {
    if (!brainTemplate) return nullptr;
    
    if (config_.useBrainCloning) {
        return BrainClone::cloneFromTemplate(brainTemplate, *brainTemplate->getConfig(),
                                             config_.brainCloneMode);
    } else {
        return std::make_shared<Brain>(brainTemplate->getConfig());
    }
}

std::shared_ptr<Environment> BatchExperimentManager::createEnvironmentForExperiment(
    std::shared_ptr<Environment> envTemplate,
    const std::string& experimentName
) {
    if (!envTemplate) return nullptr;
    
    if (config_.useEnvironmentCloning) {
        return EnvironmentClone::cloneFromTemplate(envTemplate, config_.envCloneMode);
    } else {
        return envTemplate;
    }
}

void BatchExperimentManager::updateProgress(size_t completed, size_t total, const std::string& currentExperiment) const {
    if (progressCallback_) {
        progressCallback_(completed, total, currentExperiment);
    }
}

bool BatchExperimentManager::isExperimentExcluded(const std::string& experimentName) const {
    return std::find(config_.excludedExperiments.begin(), config_.excludedExperiments.end(),
                     experimentName) != config_.excludedExperiments.end();
}

void BatchExperimentManager::applyExperimentParams(std::shared_ptr<Experiment> experiment) const {
    auto it = config_.experimentParams.find(experiment->getName());
    if (it != config_.experimentParams.end()) {
        // Apply parameters to experiment
    }
}

} // namespace nlm