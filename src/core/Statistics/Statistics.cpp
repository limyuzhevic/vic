// NLM Statistics Implementation
// Real-time performance and learning metrics for neural simulation

#include "core/Statistics/Statistics.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>

namespace nlm {

Statistics::Statistics(std::shared_ptr<Brain> brain) : brain(brain) {
    if (brain) {
        // Capture initial weights for comparison
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                initialWeights.push_back(syn->getWeight());
            }
        }
    }
}

PerformanceMetrics Statistics::calculatePerformanceMetrics() const {
    PerformanceMetrics metrics;
    
    if (!brain) {
        return metrics;
    }
    
    // Basic brain statistics
    metrics.totalStepsExecuted = brain->getTotalSpikeCount();
    metrics.totalSpikeCount = brain->getTotalSpikeCount();
    metrics.activeNeuronCount = brain->getActiveNeuronCount();
    metrics.averageFiringRate = brain->getAverageFiringRate();
    metrics.efficiencyScore = calculateEfficiencyScore(
        metrics.totalSpikeCount, 
        brain->getTotalNeuronCount(), 
        1.0  // Placeholder duration
    );
    
    // Calculate steps per second (placeholder - needs real timing)
    metrics.stepsPerSecond = metrics.totalStepsExecuted / 1.0;  // Assuming 1 second
    metrics.spikesPerSecond = metrics.totalSpikeCount / 1.0;    // Assuming 1 second
    
    return metrics;
}

LearningMetrics Statistics::calculateLearningMetrics() const {
    LearningMetrics metrics;
    
    if (brain) {
        // Get current final weights
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                finalWeights.push_back(syn->getWeight());
            }
        }
        
        // Calculate learning metrics
        metrics.initialSynapseWeightMean = calculateMean(initialWeights);
        metrics.finalSynapseWeightMean = calculateMean(finalWeights);
        metrics.weightChangeMagnitude = metrics.finalSynapseWeightMean - metrics.initialSynapseWeightMean;
        
        // Count synaptic changes
        size_t minSize = std::min(initialWeights.size(), finalWeights.size());
        for (size_t i = 0; i < minSize; ++i) {
            float delta = finalWeights[i] - initialWeights[i];
            if (delta > 0.01f) {
                metrics.strengthenedConnections++;
            } else if (delta < -0.01f) {
                metrics.weakenedConnections++;
            } else {
                metrics.stableConnections++;
            }
        }
        
        // Calculate synaptic plasticity level
        float totalChange = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f) - 
                           std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
        metrics.synapticPlasticityLevel = std::abs(totalChange) / std::max(static_cast<float>(initialWeights.size()), 1.0f);
        
        // Determine if learning occurred
        metrics.learningDetected = (std::abs(metrics.weightChangeMagnitude) > 0.001f) ||
                                  (metrics.strengthenedConnections > 0 || metrics.weakenedConnections > 0);
    }
    
    return metrics;
}

SystemMetrics Statistics::calculateSystemMetrics() const {
    SystemMetrics metrics;
    
    if (!brain) {
        return metrics;
    }
    
    // Brain system statistics
    metrics.neuronCount = brain->getTotalNeuronCount();
    metrics.synapseCount = brain->getTotalSynapseCount();
    metrics.regionCount = brain->getRegionCount();
    
    // Estimate memory usage (rough approximation)
    // Each neuron/synapse uses approximately 100 bytes
    metrics.memoryUsage = (metrics.neuronCount * 100 + metrics.synapseCount * 50) / (1024.0 * 1024.0);  // MB
    
    // Get current timing
    auto now = std::chrono::system_clock::now();
    metrics.simulationStart = now;  // This should be captured from simulation start
    metrics.simulationEnd = now;    // This should be captured from simulation end
    
    return metrics;
}

std::string Statistics::generateReport(const std::string& reportType) const {
    std::ostringstream report;
    report << std::fixed << std::setprecision(3);
    
    if (reportType == "performance" || reportType == "full") {
        auto perf = calculatePerformanceMetrics();
        report << "=== PERFORMANCE REPORT ===" << std::endl;
        report << "Steps per Second: " << perf.stepsPerSecond << std::endl;
        report << "Spikes per Second: " << perf.spikesPerSecond << std::endl;
        report << "Average Firing Rate: " << perf.averageFiringRate << " Hz" << std::endl;
        report << "Efficiency Score: " << perf.efficiencyScore << std::endl;
        report << "Active Neurons: " << perf.activeNeuronCount << std::endl;
        report << "Total Steps: " << perf.totalStepsExecuted << std::endl;
        report << "Total Spikes: " << perf.totalSpikeCount << std::endl;
    }
    
    if (reportType == "learning" || reportType == "full") {
        auto learning = calculateLearningMetrics();
        report << "\n=== LEARNING REPORT ===" << std::endl;
        report << "Learning Detected: " << (learning.learningDetected ? "YES" : "NO") << std::endl;
        report << "Initial Weight Mean: " << learning.initialSynapseWeightMean << std::endl;
        report << "Final Weight Mean: " << learning.finalSynapseWeightMean << std::endl;
        report << "Weight Change: " << learning.weightChangeMagnitude << std::endl;
        report << "Strengthened Connections: " << learning.strengthenedConnections << std::endl;
        report << "Weakened Connections: " << learning.weakenedConnections << std::endl;
        report << "Stable Connections: " << learning.stableConnections << std::endl;
        report << "Synaptic Plasticity Level: " << learning.synapticPlasticityLevel << std::endl;
    }
    
    if (reportType == "system" || reportType == "full") {
        auto system = calculateSystemMetrics();
        report << "\n=== SYSTEM REPORT ===" << std::endl;
        report << "Total Neurons: " << system.neuronCount << std::endl;
        report << "Total Synapses: " << system.synapseCount << std::endl;
        report << "Brain Regions: " << system.regionCount << std::endl;
        report << "Estimated Memory Usage: " << system.memoryUsage << " MB" << std::endl;
    }
    
    if (reportType == "version") {
        report << "NLM Statistics Version 1.0" << std::endl;
        report << "C++20 Implementation" << std::endl;
        report << "Python Bindings: v0.1.0" << std::endl;
    }
    
    return report.str();
}

bool Statistics::saveReport(const std::string& filepath, const std::string& reportType) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << generateReport(reportType);
    file.close();
    return true;
}

std::string Statistics::getVersionInfo() {
    return "NLM Statistics v1.0 - Built for C++20 neural simulation framework";
}

std::string Statistics::getSystemInfo() {
    std::ostringstream info;
    info << "NLM System Information:" << std::endl;
    info << "Architecture: x86_64" << std::endl;
    info << "Compiler: C++20" << std::endl;
    info << "Build Type: Release" << std::endl;
    info << "Python Bindings: Available" << std::endl;
    info << "Memory Model: High-performance C++" << std::endl;
    return info.str();
}

float Statistics::calculateStandardDeviation(const std::vector<float>& values) const {
    if (values.empty()) return 0.0f;
    
    float mean = calculateMean(values);
    float variance = 0.0f;
    
    for (float value : values) {
        float diff = value - mean;
        variance += diff * diff;
    }
    
    return std::sqrt(variance / values.size());
}

float Statistics::calculateMean(const std::vector<float>& values) const {
    if (values.empty()) return 0.0f;
    
    float sum = std::accumulate(values.begin(), values.end(), 0.0f);
    return sum / values.size();
}

double Statistics::calculateEfficiencyScore(size_t spikeCount, size_t neuronCount, double duration) const {
    if (neuronCount == 0 || duration <= 0.0) return 0.0;
    
    // Efficiency = spikes per neuron per second, normalized
    double spikesPerNeuronPerSecond = static_cast<double>(spikeCount) / neuronCount / duration;
    
    // Normalize to range [0, 1] based on expected maximum
    double maxExpected = 100.0;  // Arbitrary maximum for normalization
    return std::min(spikesPerNeuronPerSecond / maxExpected, 1.0);
}

std::string Statistics::formatTime(double seconds) const {
    if (seconds < 60) {
        return std::to_string(seconds) + "s";
    } else if (seconds < 3600) {
        int minutes = static_cast<int>(seconds / 60);
        int secs = static_cast<int>(seconds) % 60;
        return std::to_string(minutes) + "m " + std::to_string(secs) + "s";
    } else {
        int hours = static_cast<int>(seconds / 3600);
        int minutes = static_cast<int>((seconds - hours * 3600) / 60);
        return std::to_string(hours) + "h " + std::to_string(minutes) + "m";
    }
}

std::string Statistics::formatNumber(size_t number) const {
    std::ostringstream oss;
    oss << number;
    return oss.str();
}

} // namespace nlm