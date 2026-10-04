#include "ProgressIntegration.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <cmath>

namespace nlm {

struct CheckpointProgressIntegration::Impl {
    // Integration configuration
    IntegrationConfig config;
    
    // Integrated components
    std::shared_ptr<ProgressReporter> progressReporter;
    std::shared_ptr<StatusManager> statusManager;
    std::shared_ptr<CheckpointManager> checkpointManager;
    
    // Progress tracking
    std::string operationName;
    size_t totalCheckpoints{0};
    bool isMonitoring{false};
    std::chrono::system_clock::time_point startTime;
    size_t savedCheckpoints{0};
    size_t loadedCheckpoints{0};
    
    // Constructor
    Impl() {}
};

CheckpointProgressIntegration::CheckpointProgressIntegration(std::shared_ptr<ProgressReporter> progressReporter,
                                                               std::shared_ptr<StatusManager> statusManager,
                                                               const IntegrationConfig& config)
    : pImpl(std::make_unique<Impl>()) {
    configure(config);
    pImpl->progressReporter = progressReporter;
    pImpl->statusManager = statusManager;
}

void CheckpointProgressIntegration::configure(const IntegrationConfig& config) {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    pImpl->config = config;
}

void CheckpointProgressIntegration::initialize(std::shared_ptr<CheckpointManager> checkpointManager) {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    pImpl->checkpointManager = checkpointManager;
    
    if (checkpointManager && pImpl->config.enableCheckpointIntegration) {
        // Start monitoring
        startMonitoring();
        
        // Set operation name
        setOperation("Checkpoint Management");
        
        // Set total checkpoints based on configuration
        setTotalCheckpoints(10); // Default, can be updated based on actual configuration
        
        // Log initialization
        Logger::getGlobal().info("Checkpoint progress integration initialized");
    }
}

void CheckpointProgressIntegration::updateProgress() {
    if (!isMonitoring() || !pImpl->checkpointManager) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    // Get checkpoint manager status
    std::string checkpointDir = pImpl->checkpointManager->getCheckpointDir();
    std::vector<CheckpointManager::CheckpointInfo> checkpoints = 
        pImpl->checkpointManager->listCheckpoints();
    
    // Calculate progress
    double progress = 0.0;
    if (pImpl->totalCheckpoints > 0) {
        progress = (static_cast<double>(checkpoints.size()) / pImpl->totalCheckpoints) * 100.0;
    }
    
    // Build metrics
    ProgressMetrics metrics;
    metrics.checkpointProgress = progress;
    metrics.customMetrics["checkpoint_dir"] = checkpointDir;
    metrics.customMetrics["checkpoint_count"] = static_cast<double>(checkpoints.size());
    metrics.customMetrics["total_checkpoints"] = static_cast<double>(pImpl->totalCheckpoints);
    
    // Calculate checkpoint statistics
    size_t totalSize = 0;
    uint64_t oldestStep = UINT64_MAX;
    uint64_t newestStep = 0;
    time_t oldestTime = time(nullptr);
    time_t newestTime = 0;
    
    for (const auto& cp : checkpoints) {
        totalSize += cp.fileSize;
        if (cp.step < oldestStep) {
            oldestStep = cp.step;
            oldestTime = cp.timestamp;
        }
        if (cp.step > newestStep) {
            newestStep = cp.step;
            newestTime = cp.timestamp;
        }
    }
    
    metrics.customMetrics["total_checkpoint_size"] = static_cast<double>(totalSize);
    metrics.customMetrics["oldest_checkpoint_step"] = static_cast<double>(oldestStep);
    metrics.customMetrics["newest_checkpoint_step"] = static_cast<double>(newestStep);
    metrics.customMetrics["average_checkpoint_size"] = checkpoints.size() > 0 ? 
        static_cast<double>(totalSize) / checkpoints.size() : 0.0;
    
    // Build message
    std::stringstream message;
    message << "Checkpoints:" << checkpoints.size() << "/" << pImpl->totalCheckpoints
            << " Progress:" << std::fixed << std::setprecision(1) << progress << "%"
            << " Oldest:" << oldestStep
            << " Newest:" << newestStep
            << " TotalSize:" << totalSize / 1024 / 1024 << "MB";
    
    // Update progress reporter
    pImpl->progressReporter->updateProgress(progress, message.str(), metrics);
    
    // Update status
    SystemStatus status;
    status.uptime = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now() - pImpl->startTime).count();
    status.cpuUsage = 5.0; // Estimate 5% CPU for checkpoint management
    status.memoryTotal = 1024 * 1024 * 1024;
    status.memoryUsed = 128 * 1024 * 1024; // Estimate 128MB for checkpoint manager
    status.memoryAvailable = status.memoryTotal - status.memoryUsed;
    status.runningProcesses = {"NeuralSimulation", "CheckpointManager", "CheckpointWriter", "CheckpointReader"};
    status.diskUsage = 15.0; // Estimate 15% disk usage for checkpoints
    status.lastCheckpoint = checkpoints.empty() ? time(nullptr) : checkpoints.back().timestamp;
    status.checkpointCount = checkpoints.size();
    status.performanceScore = std::min(100.0, (checkpoints.size() / pImpl->totalCheckpoints) * 100.0);
    
    pImpl->statusManager->updateSystemStatus(status);
}

ProgressMetrics CheckpointProgressIntegration::getProgressMetrics() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    ProgressMetrics metrics;
    
    if (pImpl->checkpointManager) {
        std::vector<CheckpointManager::CheckpointInfo> checkpoints = 
            pImpl->checkpointManager->listCheckpoints();
        
        double progress = 0.0;
        if (pImpl->totalCheckpoints > 0) {
            progress = (static_cast<double>(checkpoints.size()) / pImpl->totalCheckpoints) * 100.0;
        }
        
        metrics.checkpointProgress = progress;
        metrics.customMetrics["checkpoint_count"] = static_cast<double>(checkpoints.size());
        metrics.customMetrics["total_checkpoints"] = static_cast<double>(pImpl->totalCheckpoints);
        
        // Calculate checkpoint statistics
        size_t totalSize = 0;
        for (const auto& cp : checkpoints) {
            totalSize += cp.fileSize;
        }
        
        metrics.customMetrics["total_checkpoint_size"] = static_cast<double>(totalSize);
        metrics.customMetrics["average_checkpoint_size"] = checkpoints.size() > 0 ? 
            static_cast<double>(totalSize) / checkpoints.size() : 0.0;
    }
    
    return metrics;
}

SystemStatus CheckpointProgressIntegration::getStatus() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    SystemStatus status;
    if (pImpl->checkpointManager) {
        std::vector<CheckpointManager::CheckpointInfo> checkpoints = 
            pImpl->checkpointManager->listCheckpoints();
        
        status.uptime = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now() - pImpl->startTime).count();
        status.cpuUsage = 5.0;
        status.memoryTotal = 1024 * 1024 * 1024;
        status.memoryUsed = 128 * 1024 * 1024;
        status.memoryAvailable = status.memoryTotal - status.memoryUsed;
        status.runningProcesses = {"NeuralSimulation", "CheckpointManager", "CheckpointWriter", "CheckpointReader"};
        status.diskUsage = 15.0;
        status.lastCheckpoint = checkpoints.empty() ? time(nullptr) : checkpoints.back().timestamp;
        status.checkpointCount = checkpoints.size();
        status.performanceScore = std::min(100.0, (checkpoints.size() / pImpl->totalCheckpoints) * 100.0);
    }
    
    return status;
}

void CheckpointProgressIntegration::startMonitoring() {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    if (pImpl->isMonitoring) {
        return;
    }
    
    pImpl->isMonitoring = true;
    pImpl->startTime = std::chrono::system_clock::now();
    
    Logger::getGlobal().info("Checkpoint progress monitoring started");
}

void CheckpointProgressIntegration::stopMonitoring() {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    if (!pImpl->isMonitoring) {
        return;
    }
    
    pImpl->isMonitoring = false;
    
    Logger::getGlobal().info("Checkpoint progress monitoring stopped");
}

void CheckpointProgressIntegration::setOperation(const std::string& operationName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    pImpl->operationName = operationName;
    
    if (pImpl->progressReporter) {
        pImpl->progressReporter->setOperation(operationName);
    }
}

void CheckpointProgressIntegration::setTotalCheckpoints(size_t totalCheckpoints) {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    pImpl->totalCheckpoints = totalCheckpoints;
}

void CheckpointProgressIntegration::recordCheckpointSave(const std::string& checkpointName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    pImpl->savedCheckpoints++;
    
    if (pImpl->progressReporter) {
        double progress = std::min(100.0, (static_cast<double>(pImpl->savedCheckpoints) / pImpl->totalCheckpoints) * 100.0);
        pImpl->progressReporter->updateProgress(progress, "Checkpoint saved: " + checkpointName);
    }
}

void CheckpointProgressIntegration::recordCheckpointLoad(const std::string& checkpointName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    pImpl->loadedCheckpoints++;
    
    if (pImpl->progressReporter) {
        double progress = std::min(100.0, (static_cast<double>(pImpl->loadedCheckpoints) / pImpl->totalCheckpoints) * 100.0);
        pImpl->progressReporter->updateProgress(progress, "Checkpoint loaded: " + checkpointName);
    }
}

} // namespace nlm