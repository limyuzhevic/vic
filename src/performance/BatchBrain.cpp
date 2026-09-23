#include "BatchBrain.hpp"
#include <thread>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <filesystem>

namespace nlm {

BatchBrain::BatchBrain(BatchBrain&& other) noexcept
    : config_(std::move(other.config_))
    , brain_(std::move(other.brain_))
    , state_(std::move(other.state_))
    , batchStepProcessor_(std::move(other.batchStepProcessor_)) {
    other.state_.shouldTerminate.store(true, std::memory_order_relaxed);
}

BatchBrain& BatchBrain::operator=(BatchBrain&& other) noexcept {
    if (this != &other) {
        cancel();
        config_ = std::move(other.config_);
        brain_ = std::move(other.brain_);
        state_ = std::move(other.state_);
        batchStepProcessor_ = std::move(other.batchStepProcessor_);
    }
    return *this;
}

BatchBrain::~BatchBrain() {
    cancel();
}

bool BatchBrain::processBatch(SimulationStep startStep, SimulationStep endStep) {
    return processBatchInternal(startStep, endStep);
}

bool BatchBrain::processBatchWithTime(SimulationStep startStep, SimulationStep endStep,
                                      Timestamp startTime, Timestamp endTime) {
    // Time-based batch processing
    // This would implement time-based step processing
    NLM_LOG_INFO("Time-based batch processing from " + std::to_string(startTime) +
                 " to " + std::to_string(endTime) + " seconds");
    
    // For now, delegate to standard batch processing
    return processBatch(startStep, endStep);
}

bool BatchBrain::processBatchWithEarlyTermination(SimulationStep startStep, SimulationStep endStep) {
    // Configure early termination
    BatchStepProcessor::Config batchConfig = batchStepProcessor_->getConfig();
    batchConfig.shouldTerminate = [this]() -> bool {
        return shouldTerminate();
    };
    batchConfig.earlyTerminationThreshold = config_.earlyTerminationThreshold;
    batchStepProcessor_->setConfig(batchConfig);
    
    return processBatch(startStep, endStep);
}

bool BatchBrain::processBatchWithCheckpointing(SimulationStep startStep, SimulationStep endStep,
                                               const std::string& checkpointDir) {
    std::string dir = checkpointDir.empty() ? config_.checkpointDir : checkpointDir;
    
    // Create checkpoint directory
    std::filesystem::create_directories(dir);
    
    NLM_LOG_INFO("Processing batch with checkpointing to directory: " + dir);
    
    // Configure checkpointing
    BatchStepProcessor::Config batchConfig = batchStepProcessor_->getConfig();
    batchConfig.shouldTerminate = [this]() -> bool {
        return shouldTerminate();
    };
    batchStepProcessor_->setConfig(batchConfig);
    
    return processBatch(startStep, endStep);
}

std::future<bool> BatchBrain::processBatchAsync(SimulationStep startStep, SimulationStep endStep) {
    auto task = [this, startStep, endStep]() -> bool {
        return processBatch(startStep, endStep);
    };
    
    // Submit to thread pool
    return batchStepProcessor_->getThreadPool().submit(task);
}

bool BatchBrain::processMultipleBatches(
    const std::vector<std::pair<SimulationStep, SimulationStep>>& batches) {
    bool allSuccessful = true;
    
    for (const auto& batch : batches) {
        if (!processBatch(batch.first, batch.second)) {
            allSuccessful = false;
            break;
        }
    }
    
    return allSuccessful;
}

std::vector<std::future<bool>> BatchBrain::processMultipleBatchesParallel(
    const std::vector<std::pair<SimulationStep, SimulationStep>>& batches) {
    std::vector<std::future<bool>> futures;
    
    for (const auto& batch : batches) {
        futures.push_back(processBatchAsync(batch.first, batch.second));
    }
    
    return futures;
}

bool BatchBrain::processBatchInternal(SimulationStep startStep, SimulationStep endStep) {
    std::lock_guard<std::mutex> lock(stateMutex_);
    
    if (state_.isProcessing) {
        NLM_LOG_WARN("Batch processing already in progress");
        return false;
    }
    
    // Initialize batch state
    state_.isProcessing = true;
    state_.processingStartTime = std::chrono::high_resolution_clock::now();
    state_.shouldTerminate.store(false, std::memory_order_relaxed);
    
    // Calculate batch metrics
    SimulationStep totalSteps = endStep - startStep + 1;
    state_.currentMetrics.totalSteps = totalSteps;
    state_.currentMetrics.startTime = std::chrono::duration<double>(
        state_.processingStartTime.time_since_epoch()).count();
    state_.currentMetrics.progress = 0.0f;
    
    NLM_LOG_INFO("Starting batch processing: steps " + std::to_string(startStep) +
                 " to " + std::to_string(endStep) +
                 " (" + std::to_string(totalSteps) + " steps)");
    
    // Process steps
    bool success = true;
    SimulationStep stepsProcessed = 0;
    
    for (SimulationStep step = startStep; step <= endStep && !shouldTerminate(); ++step) {
        // Process single step
        if (brain_ && !processSingleStep(step)) {
            success = false;
            break;
        }
        
        // Update progress
        stepsProcessed++;
        state_.currentMetrics.stepsProcessed = stepsProcessed;
        state_.currentMetrics.progress = 
            static_cast<float>(stepsProcessed) / static_cast<float>(totalSteps);
        
        // Update metrics
        updateMetrics();
        
        // Report progress if configured
        if (config_.reportProgress && config_.progressUpdateInterval > 0.0f) {
            handleProgressUpdate();
        }
        
        // Checkpoint if configured
        if (config_.enableCheckpointing && 
            stepsProcessed % config_.checkpointInterval == 0) {
            std::string checkpointName = "batch_step_" + std::to_string(step);
            saveCheckpoint(step, checkpointName);
            state_.currentMetrics.checkpointCount++;
        }
    }
    
    // Handle completion
    state_.isProcessing = false;
    state_.currentMetrics.endTime = std::chrono::duration<double>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    state_.currentMetrics.completedSuccessfully = success;
    state_.currentMetrics.terminatedEarly = shouldTerminate();
    
    handleCompletion(success);
    
    NLM_LOG_INFO("Batch processing " + std::string(success ? "completed" : "failed") +
                 " successfully: " + std::to_string(stepsProcessed) + " steps processed");
    
    return success;
}

bool BatchBrain::processSingleStep(SimulationStep step) {
    if (!brain_) {
        return false;
    }
    
    // Process single step
    try {
        brain_->step(step);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error processing step " + std::to_string(step) + ": ") + e.what());
        return false;
    }
}

void BatchBrain::updateMetrics() {
    if (!brain_) return;
    
    BatchMetrics& metrics = state_.currentMetrics;
    
    // Update spike count
    metrics.spikesGenerated = brain_->getTotalSpikeCount();
    
    // Update neurons processed
    metrics.neuronsProcessed = brain_->getTotalNeuronCount() * metrics.stepsProcessed;
    
    // Calculate throughput
    if (metrics.startTime > 0.0) {
        metrics.wallTime = metrics.endTime - metrics.startTime;
        if (metrics.wallTime > 0.0) {
            metrics.throughput = metrics.stepsProcessed / metrics.wallTime;
        }
    }
    
    // Estimate time remaining
    metrics.estimatedTimeRemaining = calculateEstimatedTimeRemaining();
    
    // Update progress
    if (metrics.totalSteps > 0) {
        metrics.progress = static_cast<float>(metrics.stepsProcessed) / 
                         static_cast<float>(metrics.totalSteps);
    }
}

double BatchBrain::calculateEstimatedTimeRemaining() const {
    const BatchMetrics& metrics = state_.currentMetrics;
    
    if (metrics.progress <= 0.0f || metrics.stepsProcessed <= 0 || metrics.throughput <= 0.0) {
        return 0.0;
    }
    
    double elapsedTime = metrics.wallTime;
    double totalEstimatedTime = elapsedTime / metrics.progress;
    return totalEstimatedTime - elapsedTime;
}

void BatchBrain::handleProgressUpdate() {
    if (progressCallback_) {
        progressCallback_(state_.currentMetrics.progress, state_.currentMetrics);
    }
}

void BatchBrain::handleCompletion(bool success) {
    if (completionCallback_) {
        completionCallback_(success, state_.currentMetrics);
    }
}

bool BatchBrain::saveCheckpoint(SimulationStep step, const std::string& name) {
    if (!brain_) return false;
    
    std::string filepath = config_.checkpointDir;
    if (!filepath.empty() && filepath.back() != '/') {
        filepath += '/';
    }
    filepath += (name.empty() ? "checkpoint_step_" + std::to_string(step) : name);
    
    try {
        if (brain_->save(filepath)) {
            NLM_LOG_INFO("Checkpoint saved: " + filepath);
            return true;
        } else {
            NLM_LOG_ERROR("Failed to save checkpoint: " + filepath);
            return false;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving checkpoint: ") + e.what());
        return false;
    }
}

bool BatchBrain::loadCheckpoint(const std::string& name) {
    if (!brain_) return false;
    
    std::string filepath = config_.checkpointDir;
    if (!filepath.empty() && filepath.back() != '/') {
        filepath += '/';
    }
    filepath += name;
    
    try {
        if (brain_->load(filepath)) {
            NLM_LOG_INFO("Checkpoint loaded: " + filepath);
            return true;
        } else {
            NLM_LOG_ERROR("Failed to load checkpoint: " + filepath);
            return false;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading checkpoint: ") + e.what());
        return false;
    }
}

std::vector<std::string> BatchBrain::listCheckpoints() const {
    std::vector<std::string> checkpoints;
    
    if (std::filesystem::exists(config_.checkpointDir)) {
        for (const auto& entry : std::filesystem::directory_iterator(config_.checkpointDir)) {
            if (entry.is_regular_file()) {
                checkpoints.push_back(entry.path().filename().string());
            }
        }
    }
    
    return checkpoints;
}

void BatchBrain::pruneCheckpoints(size_t keepCount) {
    std::vector<std::string> checkpoints = listCheckpoints();
    
    if (checkpoints.size() <= keepCount) {
        return;
    }
    
    // Sort by timestamp (newest first)
    std::sort(checkpoints.begin(), checkpoints.end(), [this](const std::string& a, const std::string& b) {
        std::filesystem::path pathA = config_.checkpointDir + '/' + a;
        std::filesystem::path pathB = config_.checkpointDir + '/' + b;
        
        auto timeA = std::filesystem::last_write_time(pathA);
        auto timeB = std::filesystem::last_write_time(pathB);
        
        return timeA > timeB;
    });
    
    // Delete old checkpoints
    for (size_t i = keepCount; i < checkpoints.size(); ++i) {
        std::string filepath = config_.checkpointDir + '/' + checkpoints[i];
        try {
            std::filesystem::remove(filepath);
            NLM_LOG_INFO("Deleted old checkpoint: " + filepath);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception deleting checkpoint: ") + e.what());
        }
    }
}

void BatchBrain::reset() {
    std::lock_guard<std::mutex> lock(stateMutex_);
    cancel();
    state_.currentMetrics = BatchMetrics{};
    if (batchStepProcessor_) {
        batchStepProcessor_->resetProcessingState();
    }
    NLM_LOG_INFO("BatchBrain reset");
}

std::string BatchBrain::getPerformanceReport() const {
    std::stringstream ss;
    
    ss << "=== BatchBrain Performance Report ===\n";
    ss << "Total Steps: " << state_.currentMetrics.stepsProcessed << "/" 
       << state_.currentMetrics.totalSteps << "\n";
    ss << "Progress: " << (state_.currentMetrics.progress * 100.0f) << "%\n";
    ss << "Throughput: " << state_.currentMetrics.throughput << " steps/sec\n";
    ss << "CPU Time: " << state_.currentMetrics.cpuTime << "s\n";
    ss << "Wall Time: " << state_.currentMetrics.wallTime << "s\n";
    ss << "Estimated Time Remaining: " << state_.currentMetrics.estimatedTimeRemaining << "s\n";
    ss << "Memory Usage: " << state_.currentMetrics.memoryUsage << " bytes\n";
    ss << "Neurons Processed: " << state_.currentMetrics.neuronsProcessed << "\n";
    ss << "Spikes Generated: " << state_.currentMetrics.spikesGenerated << "\n";
    ss << "Checkpoints Created: " << state_.currentMetrics.checkpointCount << "\n";
    ss << "Successfully Completed: " << (state_.currentMetrics.completedSuccessfully ? "Yes" : "No") << "\n";
    ss << "Terminated Early: " << (state_.currentMetrics.terminatedEarly ? "Yes" : "No") << "\n";
    
    return ss.str();
}

} // namespace nlm
