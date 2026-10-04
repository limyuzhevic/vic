#include "ProgressIntegration.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <cmath>

namespace nlm {

struct ExperimentProgressIntegration::Impl {
    // Integration configuration
    IntegrationConfig config;
    
    // Integrated components
    std::shared_ptr<ProgressReporter> progressReporter;
    std::shared_ptr<StatusManager> statusManager;
    std::shared_ptr<ExperimentRunner> experimentRunner;
    
    // Progress tracking
    std::string operationName;
    size_t totalExperiments{0};
    bool isMonitoring{false};
    std::chrono::system_clock::time_point startTime;
    
    // Constructor
    Impl() {}
};

ExperimentProgressIntegration::ExperimentProgressIntegration(std::shared_ptr<ProgressReporter> progressReporter,
                                                             std::shared_ptr<StatusManager> statusManager,
                                                             const IntegrationConfig& config)
    : pImpl(std::make_unique<Impl>()) {
    configure(config);
    pImpl->progressReporter = progressReporter;
    pImpl->statusManager = statusManager;
}

void ExperimentProgressIntegration::configure(const IntegrationConfig& config) {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    pImpl->config = config;
}

void ExperimentProgressIntegration::initialize(std::shared_ptr<ExperimentRunner> experimentRunner) {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    pImpl->experimentRunner = experimentRunner;
    
    if (experimentRunner && pImpl->config.enableExperimentIntegration) {
        // Start monitoring
        startMonitoring();
        
        // Set operation name
        setOperation("Experiment Batch");
        
        // Set total experiments based on actual experiments
        setTotalExperiments(experimentRunner->getExperiments().size());
        
        // Log initialization
        Logger::getGlobal().info("Experiment progress integration initialized with " +
                               std::to_string(experimentRunner->getExperiments().size()) + " experiments");
    }
}

void ExperimentProgressIntegration::updateProgress() {
    if (!isMonitoring() || !pImpl->experimentRunner) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    // Get experiments
    const auto& experiments = pImpl->experimentRunner->getExperiments();
    size_t totalExperiments = experiments.size();
    
    // Calculate progress
    double progress = 0.0;
    if (totalExperiments > 0) {
        size_t completedExperiments = 0;
        double totalFiringRate = 0.0;
        double totalReward = 0.0;
        
        for (const auto& exp : experiments) {
            // Check if experiment is complete (simplified - would need actual experiment state)
            if (exp->getEndStep() > 0) {
                completedExperiments++;
            }
            
            // Collect metrics
            totalFiringRate += exp->getAverageFiringRate();
            totalReward += exp->getMetric("total_reward");
        }
        
        progress = (static_cast<double>(completedExperiments) / totalExperiments) * 100.0;
        
        // Build metrics
        ProgressMetrics metrics;
        metrics.experimentProgress = progress;
        metrics.firingRate = totalExperiments > 0 ? totalFiringRate / totalExperiments : 0.0;
        metrics.customMetrics["total_reward"] = totalReward;
        metrics.customMetrics["completed_experiments"] = static_cast<double>(completedExperiments);
        metrics.customMetrics["total_experiments"] = static_cast<double>(totalExperiments);
        
        // Build message
        std::stringstream message;
        message << "Experiments:" << completedExperiments << "/" << totalExperiments
                << " Progress:" << std::fixed << std::setprecision(1) << progress << "%"
                << " AvgFiring:" << std::fixed << std::setprecision(1) << metrics.firingRate << "%"
                << " TotalReward:" << std::fixed << std::setprecision(1) << totalReward;
        
        // Update progress reporter
        pImpl->progressReporter->updateProgress(progress, message.str(), metrics);
    }
    
    // Update status
    ExperimentStatus status;
    status.name = "Batch Experiment";
    status.isRunning = true;
    status.isPaused = false;
    status.currentStep = 0; // Would need to track actual step
    status.maxSteps = 100; // Default, can be updated
    status.totalExperiments = totalExperiments;
    status.completedExperiments = static_cast<size_t>(progress / 100.0 * totalExperiments);
    status.completionPercentage = progress;
    status.averageFiringRate = 0.0; // Would need to calculate from actual experiments
    status.totalReward = 0.0; // Would need to calculate from actual experiments
    status.startTime = std::chrono::system_clock::to_time_t(startTime);
    status.lastUpdateTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    
    pImpl->statusManager->updateExperimentStatus(status);
}

ProgressMetrics ExperimentProgressIntegration::getProgressMetrics() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    ProgressMetrics metrics;
    
    if (pImpl->experimentRunner) {
        const auto& experiments = pImpl->experimentRunner->getExperiments();
        size_t totalExperiments = experiments.size();
        size_t completedExperiments = 0;
        double totalFiringRate = 0.0;
        
        for (const auto& exp : experiments) {
            if (exp->getEndStep() > 0) {
                completedExperiments++;
            }
            totalFiringRate += exp->getAverageFiringRate();
        }
        
        metrics.experimentProgress = totalExperiments > 0 ? 
            (static_cast<double>(completedExperiments) / totalExperiments) * 100.0 : 0.0;
        metrics.firingRate = totalExperiments > 0 ? totalFiringRate / totalExperiments : 0.0;
        metrics.customMetrics["completed_experiments"] = static_cast<double>(completedExperiments);
        metrics.customMetrics["total_experiments"] = static_cast<double>(totalExperiments);
    }
    
    return metrics;
}

ExperimentStatus ExperimentProgressIntegration::getStatus() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    ExperimentStatus status;
    if (pImpl->experimentRunner) {
        const auto& experiments = pImpl->experimentRunner->getExperiments();
        size_t totalExperiments = experiments.size();
        size_t completedExperiments = 0;
        
        for (const auto& exp : experiments) {
            if (exp->getEndStep() > 0) {
                completedExperiments++;
            }
        }
        
        status.name = "Batch Experiment";
        status.isRunning = !experiments.empty();
        status.isPaused = false;
        status.currentStep = 0; // Would need to track actual step
        status.maxSteps = 100; // Default
        status.totalExperiments = totalExperiments;
        status.completedExperiments = completedExperiments;
        status.completionPercentage = totalExperiments > 0 ? 
            (static_cast<double>(completedExperiments) / totalExperiments) * 100.0 : 0.0;
    }
    
    return status;
}

void ExperimentProgressIntegration::startMonitoring() {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    if (pImpl->isMonitoring) {
        return;
    }
    
    pImpl->isMonitoring = true;
    pImpl->startTime = std::chrono::system_clock::now();
    
    Logger::getGlobal().info("Experiment progress monitoring started");
}

void ExperimentProgressIntegration::stopMonitoring() {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    if (!pImpl->isMonitoring) {
        return;
    }
    
    pImpl->isMonitoring = false;
    
    Logger::getGlobal().info("Experiment progress monitoring stopped");
}

void ExperimentProgressIntegration::setOperation(const std::string& operationName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    pImpl->operationName = operationName;
    
    if (pImpl->progressReporter) {
        pImpl->progressReporter->setOperation(operationName);
    }
}

void ExperimentProgressIntegration::setTotalExperiments(size_t totalExperiments) {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    pImpl->totalExperiments = totalExperiments;
}

void ExperimentProgressIntegration::recordExperimentCompletion(const std::string& experimentName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    if (pImpl->progressReporter) {
        double progress = std::min(100.0, (static_cast<double>(pImpl->totalExperiments) / 100.0) * 100.0);
        pImpl->progressReporter->updateProgress(progress, "Experiment completed: " + experimentName);
    }
}

void ExperimentProgressIntegration::recordExperimentProgress(double progress) {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    if (pImpl->progressReporter) {
        pImpl->progressReporter->updateProgress(progress, "Experiment progress: " + 
                                                     std::to_string(static_cast<size_t>(progress)) + "%");
    }
}

} // namespace nlm