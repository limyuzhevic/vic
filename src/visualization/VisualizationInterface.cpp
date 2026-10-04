#include "VisualizationInterface.hpp"

#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <filesystem>
#include <iostream>

namespace nlm {

struct VisualizationInterface::Impl {
    bool active;
    double updateRate;
    VisualizationConfig config;
    bool monitoring;
    std::vector<PerformanceMetrics> metricsHistory;
    std::vector<SystemAlert> alerts;
    std::function<void(const PerformanceMetrics&)> metricsCallback;
    std::chrono::steady_clock::time_point lastUpdate;
    int stepCount;
    
    Impl() : active(false), updateRate(30.0), monitoring(false), stepCount(0) {}
};

VisualizationInterface::VisualizationInterface() : pImpl(std::make_unique<Impl>()) {}

VisualizationInterface::~VisualizationInterface() = default;

bool VisualizationInterface::initialize() {
    active = true;
    pImpl->lastUpdate = std::chrono::steady_clock::now();
    std::cout << "VisualizationInterface: Initialized successfully" << std::endl;
    return true;
}

void VisualizationInterface::update() {
    if (!active) return;
    
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - pImpl->lastUpdate).count();
    
    if (elapsed >= pImpl->config.updateInterval) {
        PerformanceMetrics metrics = getCurrentMetrics();
        pImpl->metricsHistory.push_back(metrics);
        
        if (pImpl->monitoring && pImpl->metricsCallback) {
            pImpl->metricsCallback(metrics);
        }
        
        if (pImpl->config.saveSnapshots && pImpl->stepCount % pImpl->config.snapshotInterval == 0) {
            saveSnapshot();
        }
        
        pImpl->lastUpdate = now;
        pImpl->stepCount++;
    }
}

void VisualizationInterface::render() {
    if (!active) return;
    
    std::cout << "VisualizationInterface: Rendering visualization" << std::endl;
    
    if (pImpl->config.outputFormat == "terminal") {
        // Terminal output for basic visualization
        std::cout << "=== NLM Visualization ===" << std::endl;
        std::cout << "Active: " << (isActive() ? "Yes" : "No") << std::endl;
        std::cout << "Step Count: " << pImpl->stepCount << std::endl;
        std::cout << "Update Rate: " << pImpl->updateRate << " Hz" << std::endl;
        
        for (const auto& alert : pImpl->alerts) {
            std::string levelStr;
            switch (alert.level) {
                case SystemAlert::Level::INFO: levelStr = "INFO"; break;
                case SystemAlert::Level::WARNING: levelStr = "WARN"; break;
                case SystemAlert::Level::ERROR: levelStr = "ERROR"; break;
                case SystemAlert::Level::CRITICAL: levelStr = "CRITICAL"; break;
            }
            std::cout << "  [" << levelStr << "] " << alert.message << std::endl;
        }
    }
}

void VisualizationInterface::close() {
    active = false;
    pImpl->monitoring = false;
    std::cout << "VisualizationInterface: Closed" << std::endl;
}

bool VisualizationInterface::isActive() const {
    return pImpl->active;
}

void VisualizationInterface::setUpdateRate(double hz) {
    pImpl->updateRate = hz;
    pImpl->config.updateInterval = 1000.0 / hz;
}

double VisualizationInterface::getUpdateRate() const {
    return pImpl->updateRate;
}

void VisualizationInterface::setConfig(const VisualizationConfig& config) {
    pImpl->config = config;
}

const VisualizationConfig& VisualizationInterface::getConfig() const {
    return pImpl->config;
}

void VisualizationInterface::visualizeNetwork(const Brain& brain) {
    std::cout << "VisualizationInterface: Network visualization (" << brain.getTotalNeuronCount() 
              << " neurons, " << brain.getTotalSynapseCount() << " synapses)" << std::endl;
}

void VisualizationInterface::visualizeActivity(const Brain& brain) {
    double firingRate = brain.getAverageFiringRate();
    int activeNeurons = brain.getActiveNeuronCount();
    int firingNeurons = brain.getFiringNeuronCount();
    
    std::cout << "VisualizationInterface: Activity visualization - "
              << "Firing Rate: " << firingRate << " Hz, "
              << "Active Neurons: " << activeNeurons << ", "
              << "Firing Neurons: " << firingNeurons << std::endl;
}

void VisualizationInterface::visualizeSpikes(const std::vector<SpikeEvent>& spikes) {
    std::cout << "VisualizationInterface: Spike raster plot with " << spikes.size() << " spike events" << std::endl;
}

void VisualizationInterface::visualizeWeights(const Brain& brain) {
    std::cout << "VisualizationInterface: Weight matrix visualization - "
              << "Total synapses: " << brain.getTotalSynapseCount() << std::endl;
}

void VisualizationInterface::visualizeRegions(const Brain& brain) {
    auto regionIds = brain.getRegionIds();
    std::cout << "VisualizationInterface: Brain regions visualization - "
              << regionIds.size() << " regions" << std::endl;
}

void VisualizationInterface::visualizeDevelopment(const Brain& brain) {
    std::cout << "VisualizationInterface: Development visualization - Stage: " 
              << static_cast<int>(brain.getDevelopmentalStage()) << std::endl;
}

void VisualizationInterface::visualizePlasticity(const Brain& brain) {
    std::cout << "VisualizationInterface: Plasticity visualization" << std::endl;
}

void VisualizationInterface::visualizeMemory(const Brain& brain) {
    auto workingMemory = brain.getWorkingMemory();
    auto episodicMemory = brain.getEpisodicMemory();
    
    std::cout << "VisualizationInterface: Memory system visualization - "
              << "Working Memory Size: " << workingMemory->getCurrentSize() << ", "
              << "Episodic Episodes: " << episodicMemory->getEpisodeCount() << std::endl;
}

void VisualizationInterface::visualizeRegionHeatmaps(const Brain& brain) {
    auto regionIds = brain.getRegionIds();
    std::cout << "VisualizationInterface: Brain region activity heatmaps for " << regionIds.size() << " regions" << std::endl;
}

void VisualizationInterface::startMonitoring(std::function<void(const PerformanceMetrics&)> callback) {
    pImpl->monitoring = true;
    pImpl->metricsCallback = callback;
    std::cout << "VisualizationInterface: Monitoring started" << std::endl;
}

void VisualizationInterface::stopMonitoring() {
    pImpl->monitoring = false;
    pImpl->metricsCallback = nullptr;
    std::cout << "VisualizationInterface: Monitoring stopped" << std::endl;
}

bool VisualizationInterface::isMonitoring() const {
    return pImpl->monitoring;
}

void VisualizationInterface::addAlert(const SystemAlert& alert) {
    pImpl->alerts.push_back(alert);
    
    std::string levelStr;
    switch (alert.level) {
        case SystemAlert::Level::INFO: levelStr = "INFO"; break;
        case SystemAlert::Level::WARNING: levelStr = "WARN"; break;
        case SystemAlert::Level::ERROR: levelStr = "ERROR"; break;
        case SystemAlert::Level::CRITICAL: levelStr = "CRITICAL"; break;
    }
    
    std::cout << "VisualizationInterface: Alert [" << levelStr << "] " << alert.message << std::endl;
}

std::vector<SystemAlert> VisualizationInterface::getAlerts() const {
    return pImpl->alerts;
}

void VisualizationInterface::clearAlerts() {
    pImpl->alerts.clear();
    std::cout << "VisualizationInterface: Alerts cleared" << std::endl;
}

bool VisualizationInterface::saveSnapshot(const std::string& filename) {
    std::string filepath = filename.empty() ? pImpl->config.outputDirectory + "/snapshot_" + std::to_string(pImpl->stepCount) + ".txt" : filename;
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "VisualizationInterface: Failed to open file for snapshot: " << filepath << std::endl;
        return false;
    }
    
    file << "NLM Visualization Snapshot" << std::endl;
    file << "=====================" << std::endl;
    file << "Step Count: " << pImpl->stepCount << std::endl;
    file << "Active: " << (isActive() ? "Yes" : "No") << std::endl;
    file << "Timestamp: " << std::chrono::system_clock::now().time_since_epoch().count() << std::endl;
    
    file.close();
    std::cout << "VisualizationInterface: Snapshot saved to " << filepath << std::endl;
    return true;
}

bool VisualizationInterface::loadSnapshot(const std::string& filename) {
    // Placeholder implementation
    std::cout << "VisualizationInterface: Load snapshot (placeholder)" << std::endl;
    return true;
}

void VisualizationInterface::integrateWithExperiment(const std::string& experimentName) {
    std::cout << "VisualizationInterface: Integrating with experiment: " << experimentName << std::endl;
}

void VisualizationInterface::integrateWithPhase2(const std::string& phase2Output) {
    std::cout << "VisualizationInterface: Integrating with Phase 2 output: " << phase2Output << std::endl;
}

void VisualizationInterface::integrateWithPhase6(const std::string& phase6Output) {
    std::cout << "VisualizationInterface: Integrating with Phase 6 output: " << phase6Output << std::endl;
}

PerformanceMetrics VisualizationInterface::getCurrentMetrics() const {
    PerformanceMetrics metrics;
    metrics.cpuUsage = 0.0; // Placeholder
    metrics.memoryUsage = 0.0; // Placeholder
    metrics.simulationTime = 0.0; // Placeholder
    metrics.renderingTime = 0.0; // Placeholder
    metrics.spikeCount = pImpl->stepCount; // Placeholder
    metrics.activeNeuronCount = 100; // Placeholder
    metrics.averageFiringRate = 10.0; // Placeholder
    return metrics;
}

std::vector<PerformanceMetrics> VisualizationInterface::getMetricsHistory() const {
    return pImpl->metricsHistory;
}

void VisualizationInterface::resetMetrics() {
    pImpl->metricsHistory.clear();
    std::cout << "VisualizationInterface: Metrics reset" << std::endl;
}

} // namespace nlm
