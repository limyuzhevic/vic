#include "ProgressIntegration.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <thread>

namespace nlm {

struct ProgressIntegrationManager::Impl {
    // Configuration
    IntegrationConfig config;
    
    // Integration components
    std::shared_ptr<ProgressReporter> progressReporter;
    std::shared_ptr<StatusManager> statusManager;
    
    // Individual integrations
    std::shared_ptr<BrainProgressIntegration> brainIntegration;
    std::shared_ptr<ExperimentProgressIntegration> experimentIntegration;
    std::shared_ptr<ClockProgressIntegration> clockIntegration;
    std::shared_ptr<CheckpointProgressIntegration> checkpointIntegration;
    
    // Integrated components
    std::shared_ptr<Brain> brain;
    std::shared_ptr<ExperimentRunner> experimentRunner;
    std::shared_ptr<SimulationClock> clock;
    std::shared_ptr<CheckpointManager> checkpointManager;
    
    // Monitoring state
    bool isMonitoring{false};
    std::chrono::system_clock::time_point startTime;
    double totalProgress{0.0};
    
    // Constructor
    Impl() : progressReporter(std::make_shared<ProgressReporter>()),
             statusManager(std::make_shared<StatusManager>()),
             isMonitoring(false) {}
};

ProgressIntegrationManager::ProgressIntegrationManager(const IntegrationConfig& config) : pImpl(std::make_unique<Impl>()) {
    configure(config);
    
    // Initialize progress reporter and status manager with their configurations
    pImpl->progressReporter->configure(config.progressConfig);
    pImpl->statusManager->configure(config.statusConfig);
}

ProgressIntegrationManager::~ProgressIntegrationManager() = default;

void ProgressIntegrationManager::configure(const IntegrationConfig& config) {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    pImpl->config = config;
    
    // Reconfigure integrations if they exist
    if (pImpl->brainIntegration) {
        // Reconfigure brain integration
        pImpl->brainIntegration->configure(config);
    }
    
    if (pImpl->experimentIntegration) {
        // Reconfigure experiment integration
        pImpl->experimentIntegration->configure(config);
    }
    
    if (pImpl->clockIntegration) {
        // Reconfigure clock integration
        pImpl->clockIntegration->configure(config);
    }
    
    if (pImpl->checkpointIntegration) {
        // Reconfigure checkpoint integration
        pImpl->checkpointIntegration->configure(config);
    }
}

void ProgressIntegrationManager::initialize(std::shared_ptr<Brain> brain,
                                           std::shared_ptr<ExperimentRunner> experimentRunner,
                                           std::shared_ptr<SimulationClock> clock,
                                           std::shared_ptr<CheckpointManager> checkpointManager) {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    // Store integrated components
    pImpl->brain = brain;
    pImpl->experimentRunner = experimentRunner;
    pImpl->clock = clock;
    pImpl->checkpointManager = checkpointManager;
    
    // Initialize individual integrations if configured
    if (pImpl->config.enableBrainIntegration && brain) {
        pImpl->brainIntegration = std::make_shared<BrainProgressIntegration>(
            pImpl->progressReporter, pImpl->statusManager, pImpl->config);
        pImpl->brainIntegration->initialize(brain);
    }
    
    if (pImpl->config.enableExperimentIntegration && experimentRunner) {
        pImpl->experimentIntegration = std::make_shared<ExperimentProgressIntegration>(
            pImpl->progressReporter, pImpl->statusManager, pImpl->config);
        pImpl->experimentIntegration->initialize(experimentRunner);
    }
    
    if (pImpl->config.enableClockIntegration && clock) {
        pImpl->clockIntegration = std::make_shared<ClockProgressIntegration>(
            pImpl->progressReporter, pImpl->statusManager, pImpl->config);
        pImpl->clockIntegration->initialize(clock);
    }
    
    if (pImpl->config.enableCheckpointIntegration && checkpointManager) {
        pImpl->checkpointIntegration = std::make_shared<CheckpointProgressIntegration>(
            pImpl->progressReporter, pImpl->statusManager, pImpl->config);
        pImpl->checkpointIntegration->initialize(checkpointManager);
    }
    
    // Start monitoring
    startMonitoring();
}

void ProgressIntegrationManager::updateProgress() {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    if (!pImpl->isMonitoring) {
        return;
    }
    
    // Update each integration
    if (pImpl->brainIntegration && pImpl->config.enableBrainIntegration) {
        pImpl->brainIntegration->updateProgress();
    }
    
    if (pImpl->experimentIntegration && pImpl->config.enableExperimentIntegration) {
        pImpl->experimentIntegration->updateProgress();
    }
    
    if (pImpl->clockIntegration && pImpl->config.enableClockIntegration) {
        pImpl->clockIntegration->updateProgress();
    }
    
    if (pImpl->checkpointIntegration && pImpl->config.enableCheckpointIntegration) {
        pImpl->checkpointIntegration->updateProgress();
    }
    
    // Calculate overall progress
    double overallProgress = 0.0;
    int activeIntegrations = 0;
    
    if (pImpl->brainIntegration && pImpl->config.enableBrainIntegration) {
        overallProgress += pImpl->brainIntegration->getProgressMetrics().getOverallProgress();
        activeIntegrations++;
    }
    
    if (pImpl->experimentIntegration && pImpl->config.enableExperimentIntegration) {
        overallProgress += pImpl->experimentIntegration->getProgressMetrics().getOverallProgress();
        activeIntegrations++;
    }
    
    if (pImpl->clockIntegration && pImpl->config.enableClockIntegration) {
        overallProgress += pImpl->clockIntegration->getProgressMetrics().getOverallProgress();
        activeIntegrations++;
    }
    
    if (pImpl->checkpointIntegration && pImpl->config.enableCheckpointIntegration) {
        overallProgress += pImpl->checkpointIntegration->getProgressMetrics().getOverallProgress();
        activeIntegrations++;
    }
    
    if (activeIntegrations > 0) {
        pImpl->totalProgress = overallProgress / activeIntegrations;
    }
    
    // Update progress reporter
    std::string operationName = "Integrated Operations";
    ProgressMetrics combinedMetrics;
    
    // Combine metrics from all integrations
    if (pImpl->brainIntegration && pImpl->config.enableBrainIntegration) {
        const auto& brainMetrics = pImpl->brainIntegration->getProgressMetrics();
        combinedMetrics.spikeCount += brainMetrics.spikeCount;
        combinedMetrics.neuronCount = std::max(combinedMetrics.neuronCount, brainMetrics.neuronCount);
        combinedMetrics.synapseCount = std::max(combinedMetrics.synapseCount, brainMetrics.synapseCount);
        combinedMetrics.firingNeuronCount = std::max(combinedMetrics.firingNeuronCount, brainMetrics.firingNeuronCount);
        combinedMetrics.firingRate += brainMetrics.firingRate;
        combinedMetrics.excitationInhibitionRatio += brainMetrics.excitationInhibitionRatio;
        combinedMetrics.memoryUsage += brainMetrics.memoryUsage;
        combinedMetrics.cpuUsage += brainMetrics.cpuUsage;
    }
    
    // Output integrated progress
    if (pImpl->config.enableRealTimeUpdates) {
        pImpl->progressReporter->updateProgress(pImpl->totalProgress, "Integrated Progress", combinedMetrics);
    }
    
    // Update status manager
    pImpl->statusManager->updateBrainStatus(pImpl->brainIntegration ? 
        pImpl->brainIntegration->getStatus() : BrainStatus{});
    
    pImpl->statusManager->updateSystemStatus(pImpl->clockIntegration ? 
        pImpl->clockIntegration->getStatus() : SystemStatus{});
    
    pImpl->statusManager->updateExperimentStatus(pImpl->experimentIntegration ? 
        pImpl->experimentIntegration->getStatus() : ExperimentStatus{});
}

void ProgressIntegrationManager::startMonitoring() {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    if (pImpl->isMonitoring) {
        return;
    }
    
    pImpl->isMonitoring = true;
    pImpl->startTime = std::chrono::system_clock::now();
    
    // Start individual integrations
    if (pImpl->brainIntegration) {
        pImpl->brainIntegration->startMonitoring();
    }
    
    if (pImpl->experimentIntegration) {
        pImpl->experimentIntegration->startMonitoring();
    }
    
    if (pImpl->clockIntegration) {
        pImpl->clockIntegration->startMonitoring();
    }
    
    if (pImpl->checkpointIntegration) {
        pImpl->checkpointIntegration->startMonitoring();
    }
    
    // Log start
    Logger::getGlobal().info("Progress integration monitoring started");
}

void ProgressIntegrationManager::stopMonitoring() {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    if (!pImpl->isMonitoring) {
        return;
    }
    
    pImpl->isMonitoring = false;
    
    // Stop individual integrations
    if (pImpl->brainIntegration) {
        pImpl->brainIntegration->stopMonitoring();
    }
    
    if (pImpl->experimentIntegration) {
        pImpl->experimentIntegration->stopMonitoring();
    }
    
    if (pImpl->clockIntegration) {
        pImpl->clockIntegration->stopMonitoring();
    }
    
    if (pImpl->checkpointIntegration) {
        pImpl->checkpointIntegration->stopMonitoring();
    }
    
    // Log stop
    Logger::getGlobal().info("Progress integration monitoring stopped");
}

bool ProgressIntegrationManager::isMonitoring() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    return pImpl->isMonitoring;
}

ProgressIntegrationManager& ProgressIntegrationManager::getGlobalIntegrationManager() {
    static std::shared_ptr<ProgressIntegrationManager> globalIntegrationManager;
    if (!globalIntegrationManager) {
        globalIntegrationManager = std::make_shared<ProgressIntegrationManager>();
    }
    return *globalIntegrationManager;
}

void ProgressIntegrationManager::setGlobalIntegrationManager(std::shared_ptr<ProgressIntegrationManager> manager) {
    static std::shared_ptr<ProgressIntegrationManager> globalIntegrationManager;
    globalIntegrationManager = manager;
}

} // namespace nlm