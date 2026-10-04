#include "ProgressIntegration.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <cmath>

namespace nlm {

struct BrainProgressIntegration::Impl {
    // Integration configuration
    IntegrationConfig config;
    
    // Integrated components
    std::shared_ptr<ProgressReporter> progressReporter;
    std::shared_ptr<StatusManager> statusManager;
    std::shared_ptr<Brain> brain;
    
    // Progress tracking
    std::string operationName;
    SimulationStep totalSteps{0};
    bool isMonitoring{false};
    std::chrono::system_clock::time_point startTime;
    
    // Constructor
    Impl() {}
};

BrainProgressIntegration::BrainProgressIntegration(std::shared_ptr<ProgressReporter> progressReporter,
                                                 std::shared_ptr<StatusManager> statusManager,
                                                 const IntegrationConfig& config)
    : pImpl(std::make_unique<Impl>()) {
    configure(config);
    pImpl->progressReporter = progressReporter;
    pImpl->statusManager = statusManager;
}

void BrainProgressIntegration::configure(const IntegrationConfig& config) {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    pImpl->config = config;
}

void BrainProgressIntegration::initialize(std::shared_ptr<Brain> brain) {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    pImpl->brain = brain;
    
    if (brain && pImpl->config.enableBrainIntegration) {
        // Start monitoring
        startMonitoring();
        
        // Set operation name
        setOperation("Neural Simulation");
        
        // Set total steps based on simulation configuration
        setTotalSteps(10000); // Default, can be updated based on actual simulation
        
        // Log initialization
        Logger::getGlobal().info("Brain progress integration initialized with " + 
                               std::to_string(pImpl->brain->getTotalNeuronCount()) + " neurons");
    }
}

void BrainProgressIntegration::updateProgress() {
    if (!isMonitoring() || !pImpl->brain) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    // Get current Brain statistics
    size_t neuronCount = pImpl->brain->getTotalNeuronCount();
    size_t synapseCount = pImpl->brain->getTotalSynapseCount();
    size_t firingNeuronCount = pImpl->brain->getFiringNeuronCount();
    double firingRate = pImpl->brain->getAverageFiringRate();
    double eir = pImpl->brain->getExcitationInhibitionRatio();
    size_t totalSpikeCount = pImpl->brain->getTotalSpikeCount();
    
    // Calculate progress based on spikes or simulation steps
    double progress = 0.0;
    if (pImpl->totalSteps > 0) {
        // Use current step if available, otherwise estimate based on spike count
        progress = std::min(100.0, (static_cast<double>(totalSpikeCount) / pImpl->totalSteps) * 100.0);
    } else if (neuronCount > 0 && synapseCount > 0) {
        // Estimate based on firing rate
        progress = firingRate / 100.0 * 100.0;  // firingRate is already percentage
    }
    
    // Build metrics
    ProgressMetrics metrics;
    metrics.spikeCount = totalSpikeCount;
    metrics.neuronCount = neuronCount;
    metrics.synapseCount = synapseCount;
    metrics.firingNeuronCount = firingNeuronCount;
    metrics.firingRate = firingRate;
    metrics.excitationInhibitionRatio = eir;
    
    // Add custom metrics based on config
    if (pImpl->config.brainIntegration.reportMemoryUsage) {
        // Get memory usage (simplified - would need to query actual memory usage)
        metrics.memoryUsage = neuronCount * 1000; // estimate 1KB per neuron
    }
    
    if (pImpl->config.brainIntegration.reportPerformanceMetrics) {
        // Calculate performance score
        double performance = 0.0;
        if (neuronCount > 0) {
            performance += (firingRate / 100.0) * 0.4;
        }
        if (synapseCount > 0) {
            performance += std::min(1.0, (static_cast<double>(totalSpikeCount) / synapseCount)) * 0.3;
        }
        performance += (eir / 2.0) * 0.3;  // eir is typically around 1.0 for balanced networks
        metrics.performanceScore = performance * 100.0;
    }
    
    // Build message
    std::stringstream message;
    message << "Step:" << (pImpl->totalSteps > 0 ? totalSpikeCount / 100 : 0) 
            << "/" << (pImpl->totalSteps > 0 ? pImpl->totalSteps / 100 : 0)
            << " Spikes:" << totalSpikeCount
            << " Firing:" << std::fixed << std::setprecision(1) << firingRate << "%"
            << " Neurons:" << neuronCount
            << " Synapses:" << synapseCount;
    
    // Update progress reporter
    pImpl->progressReporter->updateProgress(progress, message.str(), metrics);
    
    // Update status
    BrainStatus status;
    status.currentStep = pImpl->totalSteps > 0 ? totalSpikeCount / 100 : 0;
    status.currentTime = progress / 100.0 * 1000.0; // estimate time
    status.neuronCount = neuronCount;
    status.synapseCount = synapseCount;
    status.activeNeuronCount = firingNeuronCount;
    status.firingNeuronCount = firingNeuronCount;
    status.averageFiringRate = firingRate;
    status.totalSpikeCount = totalSpikeCount;
    status.pendingSpikeCount = 0; // Would need to query actual pending spikes
    status.excitationInhibitionRatio = eir;
    status.memoryUsage = metrics.memoryUsage;
    status.cpuUsage = metrics.cpuUsage;
    status.activeProcesses = {"NeuralSimulation", "SpikePropagation", "SynapticPlasticity"};
    
    pImpl->statusManager->updateBrainStatus(status);
}

ProgressMetrics BrainProgressIntegration::getProgressMetrics() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    if (!pImpl->brain) {
        return ProgressMetrics();
    }
    
    ProgressMetrics metrics;
    metrics.spikeCount = pImpl->brain->getTotalSpikeCount();
    metrics.neuronCount = pImpl->brain->getTotalNeuronCount();
    metrics.synapseCount = pImpl->brain->getTotalSynapseCount();
    metrics.firingNeuronCount = pImpl->brain->getFiringNeuronCount();
    metrics.firingRate = pImpl->brain->getAverageFiringRate();
    metrics.excitationInhibitionRatio = pImpl->brain->getExcitationInhibitionRatio();
    
    return metrics;
}

BrainStatus BrainProgressIntegration::getStatus() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    BrainStatus status;
    if (pImpl->brain) {
        status.currentStep = 0; // Would need to query actual step count
        status.currentTime = 0.0; // Would need to query actual time
        status.neuronCount = pImpl->brain->getTotalNeuronCount();
        status.synapseCount = pImpl->brain->getTotalSynapseCount();
        status.activeNeuronCount = pImpl->brain->getFiringNeuronCount();
        status.firingNeuronCount = pImpl->brain->getFiringNeuronCount();
        status.averageFiringRate = pImpl->brain->getAverageFiringRate();
        status.totalSpikeCount = pImpl->brain->getTotalSpikeCount();
        status.pendingSpikeCount = 0; // Would need to query actual pending spikes
        status.excitationInhibitionRatio = pImpl->brain->getExcitationInhibitionRatio();
        status.memoryUsage = 0; // Would need to query actual memory usage
        status.cpuUsage = 0.0; // Would need to query actual CPU usage
        status.activeProcesses = {"NeuralSimulation", "SpikePropagation", "SynapticPlasticity"};
    }
    
    return status;
}

void BrainProgressIntegration::startMonitoring() {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    if (pImpl->isMonitoring) {
        return;
    }
    
    pImpl->isMonitoring = true;
    pImpl->startTime = std::chrono::system_clock::now();
    
    Logger::getGlobal().info("Brain progress monitoring started");
}

void BrainProgressIntegration::stopMonitoring() {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    if (!pImpl->isMonitoring) {
        return;
    }
    
    pImpl->isMonitoring = false;
    
    Logger::getGlobal().info("Brain progress monitoring stopped");
}

void BrainProgressIntegration::setOperation(const std::string& operationName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    pImpl->operationName = operationName;
    
    if (pImpl->progressReporter) {
        pImpl->progressReporter->setOperation(operationName);
    }
}

void BrainProgressIntegration::setTotalSteps(SimulationStep totalSteps) {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    pImpl->totalSteps = totalSteps;
    
    if (pImpl->progressReporter) {
        pImpl->progressReporter->setTotalWork(totalSteps, 100.0);
    }
}

void BrainProgressIntegration::recordStep(SimulationStep currentStep) {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    if (pImpl->progressReporter) {
        double progress = (static_cast<double>(currentStep) / pImpl->totalSteps) * 100.0;
        pImpl->progressReporter->updateProgress(progress, "Simulation Step: " + std::to_string(currentStep));
    }
}

} // namespace nlm