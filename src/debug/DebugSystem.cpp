#include "DebugSystem.hpp"
#include <fstream>
#include <iostream>
#include <thread>
#include <chrono>

namespace nlm {

// Initialize DebugSystem with brain and agent brain
DebugSystem::DebugSystem() {
    setupDefaultConfiguration();
}

// Initialize debug system with brain components
void DebugSystem::initialize(std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agentBrain) {
    brain_ = brain;
    agentBrain_ = agentBrain;
    
    // Setup monitoring thread
    startMonitoringThread();
    
    // Setup performance profiler
    setupPerformanceProfiler();
    
    // Setup memory tracker
    setupMemoryTracker();
    
    // Setup network monitor
    setupNetworkMonitor();
    
    debugActive_ = true;
}

// Shutdown debug system
DebugSystem::~DebugSystem() {
    shutdown();
}

// Update real-time metrics
void DebugSystem::updateMetrics() {
    if (!brain_) return;
    
    metrics_.timestamp = std::chrono::system_clock::now();
    metrics_.totalNeurons = brain_->getTotalNeuronCount();
    metrics_.totalSynapses = brain_->getTotalSynapseCount();
    metrics_.totalSpikes = brain_->getTotalSpikeCount();
    metrics_.activeNeurons = brain_->getActiveNeuronCount();
    metrics_.firingNeurons = brain_->getFiringNeuronCount();
    metrics_.averageFiringRate = brain_->getAverageFiringRate();
    
    if (agentBrain_) {
        metrics_.dopamineLevel = agentBrain_->getNeuromodulationLevel();
        metrics_.curiosityLevel = agentBrain_->getCuriosityLevel();
        metrics_.noveltyLevel = agentBrain_->getNoveltyLevel();
        metrics_.predictionError = agentBrain_->getPredictionError();
    }
    
    // Update performance metrics
    updatePerformanceMetrics();
    updateMemoryMetrics();
}

// Start monitoring thread
void DebugSystem::startMonitoringThread() {
    monitoringActive_ = true;
    monitoringThread_ = std::thread(&DebugSystem::monitoringLoop, this);
}

// Stop monitoring thread
void DebugSystem::stopMonitoringThread() {
    monitoringActive_ = false;
    if (monitoringThread_.joinable()) {
        monitoringThread_.join();
    }
}

// Monitoring loop
void DebugSystem::monitoringLoop() {
    while (monitoringActive_ && debugActive_) {
        updateMetrics();
        std::this_thread::sleep_for(updateInterval_);
    }
}

// Setup performance profiler
void DebugSystem::setupPerformanceProfiler() {
    performanceProfilerActive_ = true;
}

// Stop performance profiler
void DebugSystem::stopPerformanceProfiler() {
    performanceProfilerActive_ = false;
}

// Update performance metrics
void DebugSystem::updatePerformanceMetrics() {
    // Placeholder for performance metrics
    metrics_.simulationTime = 0.0;
    metrics_.cpuUsage = 0.0;
    metrics_.memoryUsage = 0.0;
}

// Setup memory tracker
void DebugSystem::setupMemoryTracker() {
    memoryTrackerActive_ = true;
}

// Stop memory tracker
void DebugSystem::stopMemoryTracker() {
    memoryTrackerActive_ = false;
}

// Update memory metrics
void DebugSystem::updateMemoryMetrics() {
    // Placeholder for memory metrics
}

// Setup network monitor
void DebugSystem::setupNetworkMonitor() {
    networkMonitorActive_ = false; // Disabled by default
}

// Stop network monitor
void DebugSystem::stopNetworkMonitor() {
    networkMonitorActive_ = false;
}

// Save debug session
void DebugSystem::saveDebugSession() {
    std::string filename = "debug_session_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + ".json";
    std::string data = generateJSONData();
    
    std::ofstream file(filename);
    if (file.is_open()) {
        file << data;
        file.close();
        logSuccess("Debug session saved to: " + filename);
    } else {
        logError("Failed to save debug session to: " + filename);
    }
}

// Shutdown debug system completely
void DebugSystem::shutdown() {
    debugActive_ = false;
    
    stopMonitoringThread();
    stopPerformanceProfiler();
    stopMemoryTracker();
    stopNetworkMonitor();
    
    saveDebugSession();
    
    brain_.reset();
    agentBrain_.reset();
}

} // namespace nlm
