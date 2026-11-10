#include "DebugTools.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <numeric>

namespace nlm {

void DebugTools::endStepTimer(const std::string& stepName) {
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(endTime - stepStartTime).count();
    
    metrics.stepTimes[stepName] += duration;
    metrics.stepCount++;
    
    NLM_LOG_DEBUG("Step " + std::to_string(metrics.stepCount) + " " + stepName + 
                  ": " + std::to_string(duration) + "s");
}

void DebugTools::logEvent(const std::string& eventType, const std::string& description,
                         const std::map<std::string, std::string>& metadata) {
    DebugEvent event;
    event.timestamp = std::chrono::high_resolution_clock::now();
    event.eventType = eventType;
    event.description = description;
    event.metadata = metadata;
    
    // Add thread ID for debugging
    std::ostringstream threadId;
    threadId << std::this_thread::get_id();
    event.threadId = threadId.str();
    
    events.push_back(std::move(event));
    
    // Limit event history
    if (events.size() > 10000) {
        events.erase(events.begin(), events.begin() + 1000);
    }
    
    // Log to NLM logger if level is appropriate
    if (eventType == "ERROR" && currentLogLevel >= DebugLevelError) {
        NLM_LOG_ERROR("Debug Error: " + description);
    } else if (eventType == "WARNING" && currentLogLevel >= DebugLevelWarning) {
        NLM_LOG_WARNING("Debug Warning: " + description);
    } else if (eventType == "INFO" && currentLogLevel >= DebugLevelInfo) {
        NLM_LOG_INFO("Debug Info: " + description);
    } else if (eventType == "DEBUG" && currentLogLevel >= DebugLevelDebug) {
        NLM_LOG_DEBUG("Debug: " + description);
    }
}

void DebugTools::takeMemorySnapshot() {
    MemoryAnalysis::MemorySnapshot snapshot;
    
    // In a real implementation, this would use system calls or memory profilers
    // For now, we'll use a placeholder implementation
    static size_t counter = 0;
    memoryAnalysis.memorySnapshots.push_back(++counter * 1024);  // Placeholder
    
    if (memoryAnalysis.memorySnapshots.size() > 1000) {
        memoryAnalysis.memorySnapshots.erase(
            memoryAnalysis.memorySnapshots.begin(),
            memoryAnalysis.memorySnapshots.begin() + 100);
    }
    
    // Update peak memory
    for (size_t mem : memoryAnalysis.memorySnapshots) {
        if (mem > memoryAnalysis.peakMemoryUsage) {
            memoryAnalysis.peakMemoryUsage = mem;
        }
    }
}

void DebugTools::recordBrainSnapshot(size_t step, const std::string& description) {
    BrainStateSnapshot snapshot;
    snapshot.step = step;
    snapshot.description = description;
    
    // In a real implementation, this would query the actual brain state
    // For now, placeholder implementation
    snapshot.neuronCountsByType = {100, 400, 100};  // Placeholder
    snapshot.averageWeights = {0.5f, -0.2f, 0.1f};  // Placeholder
    snapshot.totalSpikes = metrics.totalSpikeCount;
    snapshot.eIrRatio = 2.5;  // Placeholder
    
    brainSnapshots.push_back(std::move(snapshot));
    
    if (brainSnapshots.size() > 1000) {
        brainSnapshots.erase(brainSnapshots.begin(), brainSnapshots.begin() + 100);
    }
}

void DebugTools::updateSystemMetrics(const std::string& component,
                                   const std::map<std::string, double>& metricsMap) {
    systemMetrics[component] = metricsMap;
}

bool DebugTools::validateConfig(const std::string& configFilepath) {
    // Use the existing Config class for validation
    Config config;
    if (!config.loadFromFile(configFilepath)) {
        return false;
    }
    
    // Perform basic validation
    if (config.has("neuron_count")) {
        auto neuronCount = config.get<int64_t>("neuron_count");
        if (neuronCount && (*neuronCount < 1 || *neuronCount > 10000)) {
            NLM_LOG_ERROR("Invalid neuron_count in config: " + std::to_string(*neuronCount));
            return false;
        }
    }
    
    if (config.has("connection_probability")) {
        auto connProb = config.get<double>("connection_probability");
        if (connProb && (*connProb < 0.0 || *connProb > 1.0)) {
            NLM_LOG_ERROR("Invalid connection_probability in config: " + std::to_string(*connProb));
            return false;
        }
    }
    
    return true;
}

void DebugTools::printConfigSummary(const std::string& configFilepath) {
    Config config;
    if (config.loadFromFile(configFilepath)) {
        NLM_LOG_INFO("Configuration Summary for " + configFilepath + ":");
        NLM_LOG_INFO(config.summary());
    }
}

void DebugTools::generatePerformanceReport(const std::string& outputFile) const {
    std::ostringstream report;
    report << "NLM Performance Report\n";
    report << "======================\n\n";
    report << "Total Steps: " << metrics.stepCount << "\n";
    report << "Total Wall Time: " << metrics.totalWallTime << "s\n";
    report << "Total Spike Count: " << metrics.totalSpikeCount << "\n";
    report << "Average Firing Rate: " << metrics.avgFiringRate << " Hz\n\n";
    
    if (!metrics.stepTimes.empty()) {
        report << "Step Times:\n";
        for (const auto& [stepName, time] : metrics.stepTimes) {
            report << "  " << stepName << ": " << time << "s (" 
                   << (time / metrics.stepCount * 100) << "% per step)\n";
        }
    }
    
    report << "\nMemory Analysis:\n";
    report << "  Peak Memory Usage: " << memoryAnalysis.peakMemoryUsage << " KB\n";
    report << "  Total Allocations: " << memoryAnalysis.totalAllocated << "\n";
    report << "  Current Allocations: " << memoryAnalysis.currentAllocated << "\n";
    
    if (!outputFile.empty()) {
        std::ofstream file(outputFile);
        if (file.is_open()) {
            file << report.str();
            NLM_LOG_INFO("Performance report saved to: " + outputFile);
        }
    } else {
        NLM_LOG_INFO(report.str());
    }
}

double DebugTools::getAverageStepTime() const {
    if (metrics.stepTimes.empty()) return 0.0;
    double totalTime = 0.0;
    for (const auto& time : metrics.stepTimes) {
        totalTime += time.second;
    }
    return totalTime / metrics.stepTimes.size();
}

double DebugTools::getSpikeRate(size_t steps) const {
    if (steps == 0 || metrics.spikeRates.size() < steps) return 0.0;
    double sum = std::accumulate(metrics.spikeRates.end() - steps, metrics.spikeRates.end(), 0.0);
    return sum / steps;
}

template<typename T>
std::string DebugTools::toString(const T& value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

void DebugTools::createDebugProfile(const std::string& outputFile) {
    // Generate a comprehensive debug profile
    std::ostringstream profile;
    profile << "NLM Debug Profile\n";
    profile << "==================\n\n";
    
    // Add configuration information
    profile << "Configuration:\n";
    profile << "  - Neuron Count: " << metrics.neuronCounts["neurons"] << "\n";
    profile << "  - Region Count: " << metrics.neuronCounts["regions"] << "\n";
    profile << "  - Synapse Count: " << metrics.synapseCounts["synapses"] << "\n\n";
    
    // Add performance information
    profile << "Performance:\n";
    profile << "  - Average Step Time: " << getAverageStepTime() << "s\n";
    profile << "  - Average Firing Rate: " << metrics.avgFiringRate << " Hz\n";
    profile << "  - E/I Ratio: " << (metrics.avgFiringRate > 0 ? 1.0 : 0.0) << "\n\n";
    
    // Add plasticity information
    profile << "Plasticity:\n";
    profile << "  - STDP LTP Weight: " << 0.01f << "\n";
    profile << "  - STDP LTD Weight: " << 0.012f << "\n";
    profile << "  - Synaptogenesis Rate: " << 0.0001f << "\n";
    profile << "  - Structure Pruning Rate: " << 0.00001f << "\n\n";
    
    // Add memory information
    profile << "Memory:\n";
    profile << "  - Peak Memory Usage: " << memoryAnalysis.peakMemoryUsage << " KB\n";
    profile << "  - Total Allocations: " << memoryAnalysis.totalAllocated << "\n";
    profileは一貫しています。 profile << "\nPerformance report generated:" << report.str();
    
    NLM_LOG_INFO(profile.str());
    
    if (!outputFile.empty()) {
        std::ofstream file(outputFile);
        if (file.is_open()) {
            file << profile.str();
            NLM_LOG_INFO("Debug profile saved to: " + outputFile);
        }
    }
}

void DebugTools::analyzePlasticityPatterns(const std::string& outputFile) {
    // Generate plasticity analysis report
    std::ostringstream report;
    report << "NLM Plasticity Pattern Analysis\n";
    report << "=============================\n\n";
    
    // Add plasticity metrics
    report << "Plasticity Metrics:\n";
    report << "  - STDP Weight Changes: (placeholder data)\n";
    report << "  - Hebbian Weight Changes: (placeholder data)\n";
    report << "  - Structural Plasticity Events: (placeholder data)\n";
    report << "  - Synaptogenesis Events: (placeholder data)\n";
    report << "  - Pruning Events: (placeholder data)\n\n";
    
    // Add learning statistics
    report << "Learning Statistics:\n";
    report << "  - Total Weight Changes: (placeholder data)\n";
    report << "  - Average Weight Change: (placeholder data)\n";
    report << "  - Stabilization Time: (placeholder data)\n\n";
    
    NLM_LOG_INFO(report.str());
    
    if (!outputFile.empty()) {
        std::ofstream file(outputFile);
        if (file.is_open()) {
            file << report.str();
            NLM_LOG_INFO("Plasticity analysis saved to: " + outputFile);
        }
    }
}

void DebugTools::generateVisualizationData(const std::string& outputFile) {
    // Generate data for visualization tools
    std::ostringstream data;
    data << "neuron_id,neuron_type,membrane_potential,firing_rate,weight,plasticity_flag\n";
    
    // Generate placeholder neuron data
    for (size_t i = 0; i < 100; ++i) {
        data << i << ",\"Internal\",-70.0," << (rand() % 100) / 100.0 
             << "," << (rand() % 100) / 100.0 << "," << (rand() % 2) << "\n";
    }
    
    NLM_LOG_INFO("Visualization data generated:");
    
    if (!outputFile.empty()) {
        std::ofstream file(outputFile);
        if (file.is_open()) {
            file << data.str();
            NLM_LOG_INFO("Visualization data saved to: " + outputFile);
        }
    }
}

} // namespace nlm
