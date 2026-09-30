#include "VisualizationInterface.hpp"

namespace nlm {

#include "VisualizationInterface.hpp"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <algorithm>
#include <cmath>

namespace nlm {

struct VisualizationInterface::Impl {
    bool active;
    double updateRate;
    std::chrono::high_resolution_clock::time_point lastUpdate;
    
    // Performance monitoring
    std::chrono::high_resolution_clock::time_point profilingStart;
    std::unordered_map<std::string, std::vector<double>> performanceData;
    
    // Network monitoring
    size_t totalConnections;
    float averageWeight;
    double lastUpdateTime;
    
    // Activity monitoring
    size_t totalSpikes;
    size_t activeNeurons;
    double lastActivityTime;
    
    // Memory monitoring
    size_t memoryUsage;
    
    // Development tracking
    int developmentStage;
    
    Impl() : active(false), updateRate(30.0), lastUpdate(std::chrono::high_resolution_clock::now()),
             totalConnections(0), averageWeight(0.0), lastUpdateTime(0.0),
             totalSpikes(0), activeNeurons(0), lastActivityTime(0.0),
             memoryUsage(0), developmentStage(0) {}
};

VisualizationInterface::VisualizationInterface() : pImpl(new Impl()) {}

VisualizationInterface::~VisualizationInterface() = default;

bool VisualizationInterface::initialize() {
    // Enhanced initialization with comprehensive monitoring
    pImpl->active = true;
    pImpl->lastUpdate = std::chrono::high_resolution_clock::now();
    pImpl->lastUpdateTime = 0.0;
    pImpl->totalConnections = 0;
    pImpl->averageWeight = 0.0;
    pImpl->totalSpikes = 0;
    pImpl->activeNeurons = 0;
    pImpl->lastActivityTime = 0.0;
    pImpl->memoryUsage = 0;
    pImpl->developmentStage = 0;
    
    std::cout << "Visualization Interface initialized with comprehensive monitoring" << std::endl;
    return true;
}

void VisualizationInterface::update() {
    if (!pImpl->active) return;
    
    auto now = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration<double>(now - pImpl->lastUpdate).count();
    
    if (elapsed >= 1.0 / pImpl->updateRate) {
        pImpl->lastUpdate = now;
        pImpl->lastUpdateTime = elapsed;
    }
}

void VisualizationInterface::render() {
    if (!pImpl->active) return;
    
    std::cout << "\n=== NLM Visualization Dashboard ===" << std::endl;
    std::cout << "Update Rate: " << pImpl->updateRate << " Hz" << std::endl;
    std::cout << "Last Update: " << pImpl->lastUpdateTime << " seconds ago" << std::endl;
    std::cout << "Total Connections: " << pImpl->totalConnections << std::endl;
    std::cout << "Average Weight: " << pImpl->averageWeight << std::endl;
    std::cout << "Total Spikes: " << pImpl->totalSpikes << std::endl;
    std::cout << "Active Neurons: " << pImpl->activeNeurons << std::endl;
    std::cout << "Memory Usage: " << pImpl->memoryUsage << " bytes" << std::endl;
    std::cout << "Development Stage: " << pImpl->developmentStage << std::endl;
    std::cout << "===============================" << std::endl;
}

void VisualizationInterface::close() {
    pImpl->active = false;
    std::cout << "Visualization Interface closed" << std::endl;
}

bool VisualizationInterface::isActive() const {
    return pImpl->active;
}

void VisualizationInterface::setUpdateRate(double hz) {
    pImpl->updateRate = hz;
    std::cout << "Update rate set to: " << hz << " Hz" << std::endl;
}

double VisualizationInterface::getUpdateRate() const {
    return pImpl->updateRate;
}

void VisualizationInterface::visualizeNetwork(const Brain& brain) {
    std::cout << "\n--- Network Visualization ---" << std::endl;
    
    // Compute connectivity metrics
    ConnectivityMetrics metrics = computeConnectivityMetrics(brain);
    
    std::cout << "Network Topology:" << std::endl;
    std::cout << "  Total Neurons: " << brain.getTotalNeuronCount() << std::endl;
    std::cout << "  Total Regions: " << brain.getRegionCount() << std::endl;
    std::cout << "  Total Connections: " << metrics.totalConnections << std::endl;
    std::cout << "  Connectivity Density: " << 
        (metrics.totalConnections * 1.0 / (brain.getTotalNeuronCount() * brain.getTotalNeuronCount())) << std::endl;
    
    std::cout << "\nConnection Types:" << std::endl;
    std::cout << "  Excitatory Connections: " << metrics.excitatoryConnections << std::endl;
    std::cout << "  Inhibitory Connections: " << metrics.inhibitoryConnections << std::endl;
    std::cout << "  E/I Ratio: " << metrics.excitationInhibitionRatio << std::endl;
    
    std::cout << "\nWeight Distribution:" << std::endl;
    std::cout << "  Mean Weight: " << metrics.averageWeight << std::endl;
    std::cout << "  Max Weight: " << metrics.maxWeight << std::endl;
    std::cout << "  Min Weight: " << metrics.minWeight << std::endl;
    std::cout << "  Weight Standard Deviation: " << metrics.weightStdDev << std::endl;
    
    // Update internal metrics
    pImpl->totalConnections = metrics.totalConnections;
    pImpl->averageWeight = metrics.averageWeight;
    
    std::cout << "--- End Network Visualization ---" << std::endl;
}

void VisualizationInterface::visualizeActivity(const Brain& brain) {
    std::cout << "\n--- Activity Visualization ---" << std::endl;
    
    // Compute activity statistics
    ActivityStatistics stats = computeActivityStatistics(brain);
    
    std::cout << "Activity Overview:" << std::endl;
    std::cout << "  Total Spikes (Lifetime): " << stats.totalSpikes << std::endl;
    std::cout << "  Average Firing Rate: " << stats.averageFiringRate << " Hz" << std::endl;
    std::cout << "  Max Firing Rate: " << stats.maxFiringRate << " Hz" << std::endl;
    std::cout << "  Min Firing Rate: " << stats.minFiringRate << " Hz" << std::endl;
    std::cout << "  Current Time Since Last Spike: " << 
        (std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - 
                                      std::chrono::duration<double>(stats.lastActivityTime)).count()) << " seconds" << std::endl;
    
    std::cout << "\nNetwork Activity:" << std::endl;
    std::cout << "  Currently Active Neurons: " << stats.activeNeurons << std::endl;
    std::cout << "  Maximum Active Neurons: " << stats.maxActiveNeurons << std::endl;
    std::cout << "  Activity Percentage: " << 
        (stats.activeNeurons * 100.0 / brain.getTotalNeuronCount()) << "%" << std::endl;
    
    // Update internal metrics
    pImpl->totalSpikes = stats.totalSpikes;
    pImpl->activeNeurons = stats.activeNeurons;
    pImpl->lastActivityTime = stats.lastActivityTime;
    
    std::cout << "--- End Activity Visualization ---" << std::endl;
}

void VisualizationInterface::visualizeSpikes(const std::vector<SpikeEvent>& spikes) {
    std::cout << "\n--- Spike Raster Visualization ---" << std::endl;
    
    if (spikes.empty()) {
        std::cout << "No spikes recorded" << std::endl;
        return;
    }
    
    std::cout << "Spike Analysis:" << std::endl;
    std::cout << "  Total Spikes: " << spikes.size() << std::endl;
    
    // Calculate firing statistics
    std::vector<double> spikeTimes;
    for (const auto& spike : spikes) {
        spikeTimes.push_back(spike.time);
    }
    
    double minTime = *std::min_element(spikeTimes.begin(), spikeTimes.end());
    double maxTime = *std::max_element(spikeTimes.begin(), spikeTimes.end());
    double avgTime = std::accumulate(spikeTimes.begin(), spikeTimes.end(), 0.0) / spikeTimes.size();
    
    std::cout << "  Time Range: " << minTime << " - " << maxTime << " seconds" << std::endl;
    std::cout << "  Average Inter-Spike Interval: " << 
        (spikeTimes.size() > 1 ? (maxTime - minTime) / (spikeTimes.size() - 1) : 0.0) << " seconds" << std::endl;
    
    // Simple raster plot (text-based)
    std::cout << "\nRaster Plot (Time in seconds):" << std::endl;
    for (size_t i = 0; i < std::min(spikes.size(), size_t(20)); ++i) {
        std::cout << "  " << i << ": " << spikes[i].time << " s (Neuron " << spikes[i].neuronId << ")" << std::endl;
    }
    
    if (spikes.size() > 20) {
        std::cout << "  ... and " << (spikes.size() - 20) << " more spikes" << std::endl;
    }
    
    std::cout << "--- End Spike Visualization ---" << std::endl;
}

void VisualizationInterface::visualizeWeights(const Brain& brain) {
    std::cout << "\n--- Weight Matrix Visualization ---" << std::endl;
    
    ConnectivityMetrics metrics = computeConnectivityMetrics(brain);
    
    if (metrics.totalConnections == 0) {
        std::cout << "No connections in the network" << std::endl;
        return;
    }
    
    std::cout << "Weight Analysis:" << std::endl;
    std::cout << "  Weight Range: " << metrics.minWeight << " to " << metrics.maxWeight << std::endl;
    std::cout << "  Weight Mean: " << metrics.averageWeight << std::endl;
    std::cout << "  Weight Standard Deviation: " << metrics.weightStdDev << std::endl;
    std::cout << "  Weight Coefficient of Variation: " << 
        (metrics.averageWeight > 0 ? metrics.weightStdDev / metrics.averageWeight : 0.0) << std::endl;
    
    // Weight distribution histogram (text-based)
    std::cout << "\nWeight Distribution Histogram:" << std::endl;
    // Simplified histogram with bins
    std::vector<size_t> bins(10, 0);
    // Note: In a real implementation, we would sample weights from the brain
    
    std::cout << "  [";
    for (size_t i = 0; i < 10; ++i) {
        if (i > 0) std::cout << "|";
        // Simplified visualization
        std::cout << "=";
    }
    std::cout << "]" << std::endl;
    
    std::cout << "--- End Weight Visualization ---" << std::endl;
}

void VisualizationInterface::updateMonitoringDashboard(const Brain& brain, double timestep) {
    std::cout << "\n=== Real-Time Monitoring Dashboard ===" << std::endl;
    std::cout << "Simulation Time: " << timestep << " seconds" << std::endl;
    std::cout << "Update Rate: " << pImpl->updateRate << " Hz" << std::endl;
    
    // Update metrics
    ConnectivityMetrics connectivity = computeConnectivityMetrics(brain);
    ActivityStatistics activity = computeActivityStatistics(brain);
    
    std::cout << "\n--- Real-Time Metrics ---" << std::endl;
    std::cout << "Network State:" << std::endl;
    std::cout << "  Neurons: " << brain.getTotalNeuronCount() << std::endl;
    std::cout << "  Connections: " << connectivity.totalConnections << std::endl;
    std::cout << "  E/I Ratio: " << connectivity.excitationInhibitionRatio << std::endl;
    
    std::cout << "\nActivity State:" << std::endl;
    std::cout << "  Total Spikes: " << activity.totalSpikes << std::endl;
    std::cout << "  Active Neurons: " << activity.activeNeurons << std::endl;
    std::cout << "  Firing Rate: " << activity.averageFiringRate << " Hz" << std::endl;
    
    std::cout << "\nPerformance:" << std::endl;
    std::cout << "  System Load: " << (pImpl->updateRate / 60.0 * 100.0) << "%" << std::endl;
    
    std::cout << "===============================" << std::endl;
}

void VisualizationInterface::startPerformanceProfiling() {
    pImpl->profilingStart = std::chrono::high_resolution_clock::now();
    std::cout << "Performance profiling started" << std::endl;
}

void VisualizationInterface::endPerformanceProfiling() {
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(end - pImpl->profilingStart).count();
    
    std::cout << "Performance profiling ended. Total time: " << duration << " seconds" << std::endl;
    
    logPerformanceMetrics("total_profiling", duration);
}

void VisualizationInterface::logPerformanceMetrics(const std::string& operation, double duration, size_t count) {
    pImpl->performanceData[operation].push_back(duration);
    
    // Keep only last 100 measurements for each operation
    if (pImpl->performanceData[operation].size() > 100) {
        pImpl->performanceData[operation].erase(pImpl->performanceData[operation].begin());
    }
    
    std::cout << "Performance metric logged: " << operation << " = " << duration << " seconds (count: " << count << ")" << std::endl;
}

void VisualizationInterface::logMemoryUsage() {
    // In a real implementation, this would read actual memory usage
    pImpl->memoryUsage = 1024 * 1024;  // Placeholder: 1MB
    
    std::cout << "Memory Usage: " << pImpl->memoryUsage << " bytes" << std::endl;
}

void VisualizationInterface::analyzeNetworkTopology(const Brain& brain) {
    std::cout << "\n--- Network Topology Analysis ---" << std::endl;
    
    ConnectivityMetrics metrics = computeConnectivityMetrics(brain);
    
    std::cout << "Topology Metrics:" << std::endl;
    std::cout << "  Total Connections: " << metrics.totalConnections << std::endl;
    std::cout << "  Average Weight: " << metrics.averageWeight << std::endl;
    std::cout << "  Average Degree: " << 
        (brain.getTotalNeuronCount() > 0 ? 
         static_cast<double>(metrics.totalConnections) / brain.getTotalNeuronCount() : 0.0) << std::endl;
    
    // Calculate clustering coefficient (placeholder)
    double clusteringCoefficient = 0.0;
    std::cout << "  Clustering Coefficient: " << clusteringCoefficient << std::endl;
    
    // Calculate small-world coefficient (placeholder)
    double smallWorldCoefficient = 0.0;
    std::cout << "  Small-World Coefficient: " << smallWorldCoefficient << std::endl;
    
    std::cout << "--- End Topology Analysis ---" << std::endl;
}

VisualizationInterface::ConnectivityMetrics VisualizationInterface::computeConnectivityMetrics(const Brain& brain) const {
    ConnectivityMetrics metrics;
    metrics.totalConnections = brain.getTotalSynapseCount();
    metrics.excitatoryConnections = 0;
    metrics.inhibitoryConnections = 0;
    metrics.averageWeight = 0.0;
    metrics.maxWeight = -1000.0f;
    metrics.minWeight = 1000.0f;
    metrics.weightStdDev = 0.0f;
    
    if (metrics.totalConnections == 0) {
        return metrics;
    }
    
    // In a real implementation, we would iterate through all synapses to compute these metrics
    // For now, we'll provide placeholder values
    
    metrics.excitatoryConnections = metrics.totalConnections / 2;
    metrics.inhibitoryConnections = metrics.totalConnections / 2;
    metrics.averageWeight = 0.1f;
    metrics.maxWeight = 1.0f;
    metrics.minWeight = -1.0f;
    metrics.weightStdDev = 0.3f;
    metrics.excitationInhibitionRatio = metrics.excitatoryConnections > 0 ? 
        static_cast<float>(metrics.excitatoryConnections) / metrics.inhibitoryConnections : 1.0f;
    
    return metrics;
}

VisualizationInterface::ActivityStatistics VisualizationInterface::computeActivityStatistics(const Brain& brain) const {
    ActivityStatistics stats;
    
    // Get spike statistics from brain
    stats.totalSpikes = brain.getTotalSpikeCount();
    stats.averageFiringRate = brain.getAverageFiringRate();
    stats.maxFiringRate = stats.averageFiringRate * 2.0f;  // Placeholder
    stats.minFiringRate = 0.0f;
    
    stats.lastActivityTime = 0.0;  // In real implementation, this would be stored
    stats.activeNeurons = brain.getActiveNeuronCount();
    stats.maxActiveNeurons = brain.getFiringNeuronCount();
    
    return stats;
}

void VisualizationInterface::updateDevelopmentMetrics(const Brain& brain) {
    // Update development stage based on brain activity
    pImpl->developmentStage = brain.getDevelopmentalStage();
    
    std::cout << "Development Stage Updated: " << pImpl->developmentStage << std::endl;
}

void VisualizationInterface::updateMemoryStatistics() {
    // Update memory usage statistics
    logMemoryUsage();
    
    // In real implementation, we would track checkpoint memory usage
    std::cout << "Memory statistics updated" << std::endl;
}

void VisualizationInterface::printDebugInfo(const Brain& brain, double currentTime) const {
    std::cout << "\n=== NLM Debug Information ===" << std::endl;
    std::cout << "Current Simulation Time: " << currentTime << " seconds" << std::endl;
    std::cout << "Brain State:" << std::endl;
    std::cout << "  Total Neurons: " << brain.getTotalNeuronCount() << std::endl;
    std::cout << "  Total Synapses: " << brain.getTotalSynapseCount() << std::endl;
    std::cout << "  Active Neurons: " << brain.getActiveNeuronCount() << std::endl;
    std::cout << "  Firing Neurons: " << brain.getFiringNeuronCount() << std::endl;
    std::cout << "  Average Firing Rate: " << brain.getAverageFiringRate() << " Hz" << std::endl;
    std::cout << "  Pending Spike Events: " << brain.getPendingSpikeEventCount() << std::endl;
    std::cout << "  E/I Ratio: " << brain.getExcitationInhibitionRatio() << std::endl;
    
    std::cout << "\n=== End Debug Information ===" << std::endl;
}

void VisualizationInterface::runPerformanceBenchmark(const Brain& brain) {
    std::cout << "\n=== Performance Benchmark ===" << std::endl;
    
    startPerformanceProfiling();
    
    // Run several simulation steps for benchmarking
    size_t steps = 100;
    double timestep = 0.001;
    
    for (size_t i = 0; i < steps; ++i) {
        brain.step(i, i * timestep);
    }
    
    endPerformanceProfiling();
    
    std::cout << "Benchmark completed: " << steps << " steps" << std::endl;
    std::cout << "Average step time: " << 
        (pImpl->performanceData["total_profiling"].front() / steps * 1000) << " ms" << std::endl;
    
    std::cout << "=== End Performance Benchmark ===" << std::endl;
}

} // namespace nlm
