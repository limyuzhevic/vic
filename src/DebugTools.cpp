// NLM Debug Tools implementation
// Advanced debugging and monitoring capabilities for NLM

#include "DebugTools.hpp"
#include "src/brain/Brain.hpp"
#include "src/core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <filesystem>
#include <cmath>
#include <ctime>

namespace nlm {

struct DebugTools::Impl {
    DebugLevel debugLevel;
    std::map<std::string, bool> enabledFeatures;
    std::shared_ptr<Logger> logger;
    
    std::queue<BrainStateSnapshot> realTimeData;
    size_t maxRealTimePoints;
    bool realTimeMonitoringEnabled;
    
    std::vector<std::string> debugLogs;
    std::map<std::string, std::chrono::steady_clock::time_point> profilers;
    std::map<std::string, PerformanceProfile> profilingResults;
    
    uint64_t timestampCounter;
    
    Impl() : debugLevel(DebugLevel::INFO),
             maxRealTimePoints(1000),
             realTimeMonitoringEnabled(false),
             timestampCounter(0) {
        logger = std::make_shared<Logger>();
        logger->setLevel(LogLevel::INFO);
        
        // Enable default features
        enabledFeatures["brain_state"] = true;
        enabledFeatures["memory_analysis"] = true;
        enabledFeatures["performance_profiling"] = true;
        enabledFeatures["anomaly_detection"] = true;
        enabledFeatures["real_time_monitoring"] = false;
        enabledFeatures["log_analysis"] = true;
    }
    
    uint64_t getTimestamp() {
        return ++timestampCounter;
    }
    
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    void addLog(const std::string& message, DebugLevel level = DebugLevel::INFO) {
        debugLogs.push_back("[" + getCurrentTimestamp() + "] [" + debugLevelToString(level) + "] " + message);
        
        if (debugLevel <= level) {
            logger->log(message, static_cast<LogLevel>(level));
        }
    }
    
    std::string debugLevelToString(DebugLevel level) {
        switch (level) {
            case DebugLevel::OFF: return "OFF";
            case DebugLevel::ERROR: return "ERROR";
            case DebugLevel::WARNING: return "WARNING";
            case DebugLevel::INFO: return "INFO";
            case DebugLevel::DEBUG: return "DEBUG";
            case DebugLevel::TRACE: return "TRACE";
            default: return "UNKNOWN";
        }
    }
};

// DebugTools implementation

DebugTools::DebugTools() : pImpl(std::make_unique<Impl>()) {
    initialize();
}

DebugTools::~DebugTools() {
    shutdown();
}

void DebugTools::initialize() {
    pImpl->logger->setLevel(LogLevel::INFO);
    pImpl->addLog("Debug system initialized", DebugLevel::INFO);
}

void DebugTools::setDebugLevel(DebugLevel level) {
    pImpl->debugLevel = level;
    pImpl->addLog("Debug level set to: " + pImpl->debugLevelToString(level), DebugLevel::INFO);
}

void DebugTools::enableFeature(const std::string& feature, bool enable) {
    pImpl->enabledFeatures[feature] = enable;
    pImpl->addLog((enable ? "Enabled" : "Disabled") + " debug feature: " + feature, DebugLevel::INFO);
}

DebugLevel DebugTools::getDebugLevel() const {
    return pImpl->debugLevel;
}

bool DebugTools::isFeatureEnabled(const std::string& feature) const {
    auto it = pImpl->enabledFeatures.find(feature);
    return (it != pImpl->enabledFeatures.end()) ? it->second : false;
}

BrainStateSnapshot DebugTools::takeBrainStateSnapshot(std::shared_ptr<Brain> brain) {
    BrainStateSnapshot snapshot;
    snapshot.timestamp = pImpl->getTimestamp();
    
    if (brain) {
        snapshot.neuronCount = brain->getTotalNeuronCount();
        snapshot.synapseCount = brain->getTotalSynapseCount();
        snapshot.activeNeuronCount = brain->getActiveNeuronCount();
        snapshot.firingNeuronCount = brain->getFiringNeuronCount();
        snapshot.averageFiringRate = brain->getAverageFiringRate();
        snapshot.excitationInhibitionRatio = brain->getExcitationInhibitionRatio();
        snapshot.totalSpikeCount = brain->getTotalSpikeCount();
        snapshot.pendingSpikeEventCount = brain->getPendingSpikeEventCount();
        
        auto devStage = brain->getDevelopmentalStage();
        snapshot.developmentalStage = (devStage == DevelopmentalStage::Initial ? "Initial" :
                                      devStage == DevelopmentalStage::CriticalPeriod ? "CriticalPeriod" :
                                      devStage == DevelopmentalStage::Maturation ? "Maturation" :
                                      devStage == DevelopmentalStage::Adult ? "Adult" :
                                      devStage == DevelopmentalStage::Aging ? "Aging" : "Unknown");
        
        auto* neuromod = brain->getDopamine();
        snapshot.neuromodulationLevel = neuromod ? neuromod->getLevel() : 0.0f;
        
        auto* curiosity = brain->getCuriosity();
        snapshot.curiosityLevel = curiosity ? curiosity->getLevel() : 0.0f;
        
        auto* novelty = brain->getNovelty();
        snapshot.noveltyLevel = novelty ? novelty->getLevel() : 0.0f;
        
        auto* predError = brain->getPredictionErrorSignal();
        snapshot.predictionError = predError ? predError->getError() : 0.0f;
        
        // Get energy level (placeholder)
        snapshot.energyLevel = 100.0; // Placeholder
        
        // Add custom metrics
        snapshot.customMetrics["spike_rate"] = snapshot.firingNeuronCount / std::max<size_t>(snapshot.neuronCount, 1);
        snapshot.customMetrics["connectivity"] = snapshot.synapseCount / std::max<size_t>(snapshot.neuronCount, 1);
    }
    
    // Add to real-time data if enabled
    if (pImpl->realTimeMonitoringEnabled) {
        pImpl->realTimeData.push(snapshot);
        if (pImpl->realTimeData.size() > pImpl->maxRealTimePoints) {
            pImpl->realTimeData.pop();
        }
    }
    
    return snapshot;
}

std::vector<std::string> DebugTools::compareBrainStates(const BrainStateSnapshot& snapshot1,
                                                        const BrainStateSnapshot& snapshot2) {
    std::vector<std::string> differences;
    
    if (snapshot1.neuronCount != snapshot2.neuronCount) {
        differences.push_back("Neuron count changed: " + std::to_string(snapshot1.neuronCount) + 
                            " -> " + std::to_string(snapshot2.neuronCount));
    }
    
    if (snapshot1.synapseCount != snapshot2.synapseCount) {
        differences.push_back("Synapse count changed: " + std::to_string(snapshot1.synapseCount) + 
                            " -> " + std::to_string(snapshot2.synapseCount));
    }
    
    if (snapshot1.averageFiringRate != snapshot2.averageFiringRate) {
        differences.push_back("Firing rate changed: " + std::to_string(snapshot1.averageFiringRate) + 
                            " -> " + std::to_string(snapshot2.averageFiringRate));
    }
    
    if (snapshot1.excitationInhibitionRatio != snapshot2.excitationInhibitionRatio) {
        differences.push_back("E/I ratio changed: " + std::to_string(snapshot1.excitationInhibitionRatio) + 
                            " -> " + std::to_string(snapshot2.excitationInhibitionRatio));
    }
    
    if (snapshot1.totalSpikeCount != snapshot2.totalSpikeCount) {
        differences.push_back("Total spike count changed: " + std::to_string(snapshot1.totalSpikeCount) + 
                            " -> " + std::to_string(snapshot2.totalSpikeCount));
    }
    
    if (snapshot1.energyLevel != snapshot2.energyLevel) {
        differences.push_back("Energy level changed: " + std::to_string(snapshot1.energyLevel) + 
                            " -> " + std::to_string(snapshot2.energyLevel));
    }
    
    return differences;
}

std::vector<AnomalyResult> DebugTools::detectAnomalies(const BrainStateSnapshot& snapshot) {
    std::vector<AnomalyResult> anomalies;
    
    // Check for neuron count anomalies
    if (snapshot.neuronCount > 100000) {
        AnomalyResult anomaly;
        anomaly.type = "NEURON_OVERLOAD";
        anomaly.description = "Neuron count exceeds typical range";
        anomaly.severity = 0.8;
        anomaly.timestamp = snapshot.timestamp;
        anomaly.location = "brain";
        anomaly.details = "Current count: " + std::to_string(snapshot.neuronCount);
        anomaly.isResolved = false;
        anomalies.push_back(anomaly);
    }
    
    // Check for synapse density anomalies
    float neuronDensity = static_cast<float>(snapshot.synapseCount) / std::max<size_t>(snapshot.neuronCount, 1);
    if (neuronDensity > 10.0) {
        AnomalyResult anomaly;
        anomaly.type = "SYNAPSE_DENSITY_HIGH";
        anomaly.description = "Synapse density exceeds normal range";
        anomaly.severity = 0.6;
        anomaly.timestamp = snapshot.timestamp;
        anomaly.location = "brain";
        anomaly.details = "Synapse density: " + std::to_string(neuronDensity);
        anomaly.isResolved = false;
        anomalies.push_back(anomaly);
    }
    
    // Check for firing rate anomalies
    if (snapshot.averageFiringRate > 50.0) {
        AnomalyResult anomaly;
        anomaly.type = "FIRING_RATE_HIGH";
        anomaly.description = "Average firing rate exceeds normal range";
        anomaly.severity = 0.5;
        anomaly.timestamp = snapshot.timestamp;
        anomaly.location = "brain";
        anomaly.details = "Average firing rate: " + std::to_string(snapshot.averageFiringRate) + " Hz";
        anomaly.isResolved = false;
        anomalies.push_back(anomaly);
    }
    
    // Check for energy anomalies
    if (snapshot.energyLevel < 10.0) {
        AnomalyResult anomaly;
        anomaly.type = "ENERGY_LOW";
        anomaly.description = "Energy level critically low";
        anomaly.severity = 0.9;
        anomaly.timestamp = snapshot.timestamp;
        anomaly.location = "agent";
        anomaly.details = "Energy level: " + std::to_string(snapshot.energyLevel);
        anomaly.isResolved = false;
        anomalies.push_back(anomaly);
    }
    
    // Check for neuromodulation anomalies
    if (std::abs(snapshot.neuromodulationLevel) > 1.0) {
        AnomalyResult anomaly;
        anomaly.type = "NEUROMODULATION_ANOMALY";
        anomaly.description = "Neuromodulation level outside normal range";
        anomaly.severity = 0.7;
        anomaly.timestamp = snapshot.timestamp;
        anomaly.location = "brain";
        anomaly.details = "Neuromodulation level: " + std::to_string(snapshot.neuromodulationLevel);
        anomaly.isResolved = false;
        anomalies.push_back(anomaly);
    }
    
    return anomalies;
}

MemoryInspection DebugTools::inspectMemory(const std::string& componentName) {
    MemoryInspection inspection;
    inspection.componentName = componentName.empty() ? "system" : componentName;
    
    // Placeholder memory values
    inspection.totalMemory = 8192; // MB
    inspection.usedMemory = 2048; // MB
    inspection.availableMemory = inspection.totalMemory - inspection.usedMemory;
    
    // Memory usage by component
    inspection.memoryUsage["brain"] = 2048;
    inspection.memoryUsage["config"] = 512;
    inspection.memoryUsage["logger"] = 256;
    inspection.memoryUsage["performance"] = 1024;
    inspection.memoryUsage["memory_systems"] = 1024;
    
    // Check for memory leaks
    if (inspection.usedMemory > inspection.totalMemory * 0.8) {
        inspection.memoryLeaks.push_back("High memory usage detected");
    }
    
    inspection.isOptimized = (inspection.usedMemory < inspection.totalMemory * 0.7);
    
    return inspection;
}

void DebugTools::startProfiler(const std::string& componentName) {
    pImpl->profilers[componentName] = std::chrono::steady_clock::now();
    pImpl->addLog("Started profiling: " + componentName, DebugLevel::DEBUG);
}

PerformanceProfile DebugTools::stopProfiler(const std::string& componentName) {
    PerformanceProfile profile;
    profile.componentName = componentName;
    
    auto it = pImpl->profilers.find(componentName);
    if (it != pImpl->profilers.end()) {
        auto startTime = it->second;
        auto endTime = std::chrono::steady_clock::now();
        
        profile.totalTime = std::chrono::duration<double>(endTime - startTime).count();
        
        // Get timing details
        profile.timings["cpu_time"] = profile.totalTime * 0.8;
        profile.timings["wall_clock_time"] = profile.totalTime;
        
        // Calculate efficiency (placeholder)
        profile.efficiency = 0.85;
        
        // Check for bottlenecks
        if (profile.totalTime > 1.0) {
            profile.bottlenecks.push_back("Slow component execution");
        }
        
        // Generate optimizations
        if (profile.efficiency < 0.9) {
            profile.optimizations.push_back("Consider parallel execution");
        }
        
        pImpl->profilers.erase(it);
        pImpl->profilingResults[componentName] = profile;
        
        pImpl->addLog("Completed profiling: " + componentName + " in " + std::to_string(profile.totalTime) + "s", DebugLevel::INFO);
    } else {
        pImpl->addLog("Warning: Profiler not started for component: " + componentName, DebugLevel::WARNING);
    }
    
    return profile;
}

std::map<std::string, PerformanceProfile> DebugTools::getProfilingData(const std::string& componentName) const {
    if (componentName.empty()) {
        return pImpl->profilingResults;
    } else {
        std::map<std::string, PerformanceProfile> result;
        auto it = pImpl->profilingResults.find(componentName);
        if (it != pImpl->profilingResults.end()) {
            result[componentName] = it->second;
        }
        return result;
    }
}

void DebugTools::enableRealTimeMonitoring(bool enable) {
    pImpl->realTimeMonitoringEnabled = enable;
    pImpl->addLog((enable ? "Enabled" : "Disabled") + " real-time monitoring", DebugLevel::INFO);
    
    if (enable) {
        pImpl->realTimeData = std::queue<BrainStateSnapshot>();
    }
}

std::vector<BrainStateSnapshot> DebugTools::getRealTimeData(size_t maxPoints) {
    std::vector<BrainStateSnapshot> result;
    
    // Copy current data
    std::queue<BrainStateSnapshot> temp = pImpl->realTimeData;
    size_t count = 0;
    
    while (!temp.empty() && count < maxPoints) {
        result.push_back(temp.front());
        temp.pop();
        count++;
    }
    
    // Return in chronological order
    std::reverse(result.begin(), result.end());
    
    return result;
}

bool DebugTools::exportDebugData(const std::string& filepath, const std::string& format) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    if (format == "json") {
        // Export as JSON (simplified)
        file << "{\n";
        file << "  \"debug_level\": \"" << pImpl->debugLevelToString(pImpl->debugLevel) << "\",\n";
        file << "  \"enabled_features\": [";
        bool first = true;
        for (const auto& pair : pImpl->enabledFeatures) {
            if (!first) file << ", ";
            file << "\"" << pair.first << "\"";
            if (pair.second) file << ": true";
            else file << ": false";
            first = false;
        }
        file << "],\n";
        file << "  \"log_count\": " << pImpl->debugLogs.size() << "\n";
        file << "}\n";
    } else {
        // Export as text
        file << "Debug Data Export" << std::endl;
        file << "=================" << std::endl;
        file << "Debug Level: " << pImpl->debugLevelToString(pImpl->debugLevel) << std::endl;
        file << "Enabled Features:" << std::endl;
        for (const auto& pair : pImpl->enabledFeatures) {
            file << "  " << pair.first << ": " << (pair.second ? "enabled" : "disabled") << std::endl;
        }
        file << "Log Entries: " << pImpl->debugLogs.size() << std::endl;
    }
    
    file.close();
    pImpl->addLog("Exported debug data to: " + filepath, DebugLevel::INFO);
    return true;
}

bool DebugTools::importDebugData(const std::string& filepath) {
    if (!std::filesystem::exists(filepath)) {
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Simple import (placeholder)
    std::string line;
    while (std::getline(file, line)) {
        // Parse line (simplified)
        if (line.find("Debug Level:") != std::string::npos) {
            // Parse debug level
        } else if (line.find("Enabled Features:") != std::string::npos) {
            // Parse features
        }
    }
    
    file.close();
    pImpl->addLog("Imported debug data from: " + filepath, DebugLevel::INFO);
    return true;
}

std::vector<std::string> DebugTools::getDebugLogs() const {
    return pImpl->debugLogs;
}

void DebugTools::clearDebugLogs() {
    pImpl->debugLogs.clear();
    pImpl->addLog("Debug logs cleared", DebugLevel::INFO);
}

std::string DebugTools::getDebugStatistics() const {
    std::stringstream ss;
    
    ss << "Debug System Statistics:" << std::endl;
    ss << "=======================" << std::endl;
    ss << "Debug Level: " << pImpl->debugLevelToString(pImpl->debugLevel) << std::endl;
    ss << "Log Entries: " << pImpl->debugLogs.size() << std::endl;
    ss << "Enabled Features: " << pImpl->enabledFeatures.size() << std::endl;
    ss << "Real-time Data Points: " << pImpl->realTimeData.size() << std::endl;
    ss << "Active Profilers: " << pImpl->profilers.size() << std::endl;
    ss << "Completed Profiles: " << pImpl->profilingResults.size() << std::endl;
    
    return ss.str();
}

void DebugTools::reset() {
    pImpl->debugLevel = DebugLevel::INFO;
    pImpl->debugLogs.clear();
    pImpl->realTimeData = std::queue<BrainStateSnapshot>();
    pImpl->profilers.clear();
    pImpl->profilingResults.clear();
    
    pImpl->addLog("Debug system reset", DebugLevel::INFO);
}

void DebugTools::shutdown() {
    reset();
    pImpl->addLog("Debug system shutdown", DebugLevel::INFO);
}

} // namespace nlm
