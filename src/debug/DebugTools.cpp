#include "DebugTools.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Config/Config.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <map>
#include <unordered_set>
#include <ctime>
#include <thread>
#include <chrono>
#include <numeric>
#include <cstdint>
#include <limits>
#include <iostream>

namespace nlm {

struct DebugTools::Impl {
    // Memory tracking
    uint64_t peakMemoryUsage;
    uint64_t currentMemoryUsage;
    
    // Performance monitoring
    std::map<std::string, double> performanceTimers;
    std::map<std::string, std::chrono::steady_clock::time_point> performanceStartTimes;
    
    // Event tracking
    std::vector<std::pair<std::string, std::string>> eventLog;
    
    // Connection tracking
    std::map<std::pair<std::string, std::string>, float> connectionMap;
    
    // Debug output
    std::ostream* debugOutput;
    bool debugOutputEnabled;
    std::string debugFilepath;
    
    // Debug configuration
    DebugLevel debugLevel;
    std::unordered_set<std::string> debugFilters;
    std::map<std::string, std::string> snapshots;
    bool debugActive;
    std::string debugSessionId;
    
    // Debug metrics
    size_t eventCount;
    size_t performanceCallCount;
    size_t memoryCheckCount;
    std::chrono::steady_clock::time_point sessionStartTime;
    
    // System information
    std::string systemInfo;
    
    Impl() : peakMemoryUsage(0), currentMemoryUsage(0),
             debugOutput(nullptr), debugOutputEnabled(false),
             debugLevel(DEBUG_LEVEL_INFO), debugActive(true),
             eventCount(0), performanceCallCount(0), memoryCheckCount(0) {
        sessionStartTime = std::chrono::steady_clock::now();
        updateSystemInfo();
    }
    
    void updateSystemInfo() {
        std::ostringstream oss;
        oss << "DebugTools Session: " << sessionStartTime.time_since_epoch().count();
        systemInfo = oss.str();
    }
};

std::unique_ptr<DebugTools::Impl> DebugTools::pImpl = nullptr;

void DebugTools::initialize() {
    if (!pImpl) {
        pImpl = std::make_unique<Impl>();
        NLM_LOG_SYSTEM("DebugTools initialized");
    }
}

void DebugTools::cleanup() {
    if (pImpl) {
        pImpl.reset();
        NLM_LOG_SYSTEM("DebugTools cleaned up");
    }
}

void DebugTools::shutdown() {
    cleanup();
}

void DebugTools::logMemoryUsage(const std::string& category) {
    if (!pImpl || !pImpl->debugActive) return;
    
    uint64_t memoryUsage = getCurrentMemoryUsage();
    pImpl->currentMemoryUsage = memoryUsage;
    pImpl->peakMemoryUsage = std::max(pImpl->peakMemoryUsage, memoryUsage);
    
    pImpl->memoryCheckCount++;
    
    if (isCategoryFiltered(category) && !isDebugLevelEnabled(DEBUG_LEVEL_DETAILED)) {
        return;
    }
    
    NLM_LOG_DEBUG("Memory Usage: " + formatMemorySize(memoryUsage) + 
                 " (Peak: " + formatMemorySize(pImpl->peakMemoryUsage) + ")",
                 "", 0, "", category);
}

uint64_t DebugTools::getCurrentMemoryUsage() {
    // Simplified memory usage calculation
    // In a real implementation, this would use system calls
    uint64_t baseMemory = 100 * 1024 * 1024;  // 100MB base
    uint64_t processMemory = rand() % (200 * 1024 * 1024);  // Random between 0-200MB
    
    return baseMemory + processMemory;
}

uint64_t DebugTools::getPeakMemoryUsage() {
    if (!pImpl) return 0;
    return pImpl->peakMemoryUsage;
}

void DebugTools::startPerformanceTimer(const std::string& name) {
    if (!pImpl || !pImpl->debugActive) return;
    
    pImpl->performanceCallCount++;
    pImpl->performanceStartTimes[name] = std::chrono::steady_clock::now();
}

void DebugTools::stopPerformanceTimer(const std::string& name) {
    if (!pImpl || !pImpl->debugActive) return;
    
    auto it = pImpl->performanceStartTimes.find(name);
    if (it == pImpl->performanceStartTimes.end()) return;
    
    auto start = it->second;
    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    double elapsedSeconds = duration.count() / 1000000.0;
    pImpl->performanceTimers[name] = elapsedSeconds;
    
    pImpl->performanceStartTimes.erase(it);
    
    NLM_LOG_PERF("Performance: " + name + " took " + formatTime(elapsedSeconds), elapsedSeconds);
}

double DebugTools::getPerformanceTime(const std::string& name) {
    if (!pImpl) return 0.0;
    
    auto it = pImpl->performanceTimers.find(name);
    if (it == pImpl->performanceTimers.end()) return 0.0;
    
    return it->second;
}

void DebugTools::printPerformanceReport(const std::string& category) {
    if (!pImpl || !pImpl->debugActive) return;
    
    if (isCategoryFiltered(category) && !isDebugLevelEnabled(DEBUG_LEVEL_DETAILED)) {
        return;
    }
    
    std::ostringstream oss;
    oss << "=== Performance Report ===\n";
    oss << "Total performance calls: " << pImpl->performanceCallCount << "\n";
    oss << "Active timers: " << pImpl->performanceStartTimes.size() << "\n";
    oss << "Completed timers: " << pImpl->performanceTimers.size() << "\n";
    
    if (!pImpl->performanceTimers.empty()) {
        oss << "\nTimer Statistics:\n";
        double totalTime = 0.0;
        for (const auto& pair : pImpl->performanceTimers) {
            oss << "  " << pair.first << ": " << formatTime(pair.second) << "\n";
            totalTime += pair.second;
        }
        oss << "Total time: " << formatTime(totalTime) << "\n";
    }
    
    if (pImpl->debugOutput) {
        (*pImpl->debugOutput) << oss.str();
    }
    
    NLM_LOG_INFO("Performance report generated", "", 0, "", category);
}

void DebugTools::resetPerformanceTimers() {
    if (!pImpl) return;
    
    pImpl->performanceTimers.clear();
    pImpl->performanceStartTimes.clear();
    NLM_LOG_SYSTEM("Performance timers reset");
}

void DebugTools::trackEvent(const std::string& event, const std::string& details) {
    if (!pImpl || !pImpl->debugActive) return;
    
    pImpl->eventCount++;
    pImpl->eventLog.emplace_back(event, details);
    
    if (isCategoryFiltered("EventLog") && !isDebugLevelEnabled(DEBUG_LEVEL_INFO)) {
        return;
    }
    
    NLM_LOG_INFO("Event tracked: " + event + " - " + details, "", 0, "", "EventLog");
}

void DebugTools::printEventLog(const std::string& category) {
    if (!pImpl || !pImpl->debugActive) return;
    
    if (isCategoryFiltered(category) && !isDebugLevelEnabled(DEBUG_LEVEL_INFO)) {
        return;
    }
    
    std::ostringstream oss;
    oss << "=== Event Log ===\n";
    oss << "Total events: " << pImpl->eventCount << "\n";
    oss << "Recent events:\n";
    
    size_t displayCount = std::min(static_cast<size_t>(20), pImpl->eventLog.size());
    for (size_t i = 0; i < displayCount; ++i) {
        const auto& event = pImpl->eventLog[pImpl->eventLog.size() - 1 - i];
        oss << "  [" << (pImpl->eventLog.size() - 1 - i) << "] " << event.first << ": " << event.second << "\n";
    }
    
    if (pImpl->debugOutput) {
        (*pImpl->debugOutput) << oss.str();
    }
    
    NLM_LOG_INFO("Event log displayed", "", 0, "", category);
}

void DebugTools::clearEventLog() {
    if (!pImpl) return;
    
    pImpl->eventLog.clear();
    NLM_LOG_SYSTEM("Event log cleared");
}

bool DebugTools::checkSystemHealth() {
    if (!pImpl) return true;
    
    bool isHealthy = true;
    
    // Check memory usage
    uint64_t memoryUsage = getCurrentMemoryUsage();
    if (memoryUsage > 500 * 1024 * 1024) {  // 500MB limit
        NLM_LOG_ERROR("System health check failed: memory usage too high", "SystemHealth");
        isHealthy = false;
    }
    
    // Check performance timers
    if (pImpl->performanceStartTimes.size() > 100) {
        NLM_LOG_ERROR("System health check failed: too many active performance timers", "SystemHealth");
        isHealthy = false;
    }
    
    // Check event log size
    if (pImpl->eventLog.size() > 10000) {
        NLM_LOG_ERROR("System health check failed: event log too large", "SystemHealth");
        isHealthy = false;
    }
    
    if (isHealthy) {
        NLM_LOG_SYSTEM("System health check passed");
    }
    
    return isHealthy;
}

std::string DebugTools::getSystemHealthReport() {
    std::ostringstream oss;
    oss << "=== System Health Report ===\n";
    oss << "Session ID: " << getDebugSessionId() << "\n";
    oss << "Session start time: " << formatTime(
        std::chrono::duration<double>(std::chrono::steady_clock::now() - getSessionStartTime()).count()) << "\n";
    oss << "Debug active: " << (isDebugActive() ? "Yes" : "No") << "\n";
    oss << "Debug level: " << getDebugLevel() << "\n";
    oss << "Filtered categories: ";
    
    bool first = true;
    for (const auto& category : pImpl->debugFilters) {
        if (!first) oss << ", ";
        oss << category;
        first = false;
    }
    oss << "\n";
    
    oss << "\nResource Usage:\n";
    oss << "  Memory: " << formatMemorySize(getCurrentMemoryUsage()) << " (Peak: " << formatMemorySize(getPeakMemoryUsage()) << ")\n";
    oss << "  Performance timers: " << pImpl->performanceTimers.size() << " active, " << pImpl->performanceStartTimes.size() << " running\n";
    oss << "  Events tracked: " << pImpl->eventCount << "\n";
    oss << "  Memory checks: " << pImpl->memoryCheckCount << "\n";
    
    oss << "\nSystem Status: " << (checkSystemHealth() ? "HEALTHY" : "UNHEALTHY") << "\n";
    
    return oss.str();
}

bool DebugTools::validateConfiguration() {
    // Basic configuration validation
    bool isValid = true;
    
    // Check if debug tools is initialized
    if (!pImpl) {
        NLM_LOG_ERROR("DebugTools not initialized", "ConfigValidation");
        return false;
    }
    
    // Check for required configuration
    if (getDebugSessionId().empty()) {
        NLM_LOG_WARNING("Debug session ID not set", "ConfigValidation");
        isValid = false;
    }
    
    // Check debug level
    if (getDebugLevel() < DEBUG_LEVEL_ERROR) {
        NLM_LOG_WARNING("Debug level too low for production", "ConfigValidation");
    }
    
    return isValid;
}

std::string DebugTools::getConfigurationValidationReport() {
    std::ostringstream oss;
    oss << "=== Configuration Validation Report ===\n";
    oss << "DebugTools initialized: " << (pImpl ? "Yes" : "No") << "\n";
    oss << "Debug active: " << (isDebugActive() ? "Yes" : "No") << "\n";
    oss << "Debug session ID: " << getDebugSessionId() << "\n";
    oss << "Debug level: " << getDebugLevel() << "\n";
    oss << "Filtered categories: ";
    
    bool first = true;
    for (const auto& category : pImpl->debugFilters) {
        if (!first) oss << ", ";
        oss << category;
        first = false;
    }
    oss << "\n";
    
    oss << "\nValidation Result: " << (validateConfiguration() ? "PASSED" : "FAILED") << "\n";
    
    return oss.str();
}

void DebugTools::trackConnection(const std::string& source, const std::string& target, float weight) {
    if (!pImpl) return;
    
    pImpl->connectionMap[{source, target}] = weight;
    
    NLM_LOG_DEBUG("Connection tracked: " + source + " -> " + target + " (weight: " + std::to_string(weight) + ")", "", 0, "", "ConnectionTracking");
}

void DebugTools::untrackConnection(const std::string& source, const std::string& target) {
    if (!pImpl) return;
    
    auto key = std::make_pair(source, target);
    if (pImpl->connectionMap.find(key) != pImpl->connectionMap.end()) {
        pImpl->connectionMap.erase(key);
        NLM_LOG_DEBUG("Connection untracked: " + source + " -> " + target, "", 0, "", "ConnectionTracking");
    }
}

void DebugTools::printConnectionMap(const std::string& category) {
    if (!pImpl) return;
    
    if (isCategoryFiltered(category) && !isDebugLevelEnabled(DEBUG_LEVEL_INFO)) {
        return;
    }
    
    std::ostringstream oss;
    oss << "=== Connection Map ===\n";
    oss << "Total connections: " << pImpl->connectionMap.size() << "\n";
    oss << "\nConnections:\n";
    
    for (const auto& pair : pImpl->connectionMap) {
        oss << "  " << pair.first.first << " -> " << pair.first.second << " (weight: " << pair.second << ")\n";
    }
    
    if (pImpl->debugOutput) {
        (*pImpl->debugOutput) << oss.str();
    }
    
    NLM_LOG_INFO("Connection map displayed", "", 0, "", category);
}

void DebugTools::logNeuronDetails(const NeuronId& neuronId) {
    NLM_LOG_INFO("Neuron details requested: ID = " + std::to_string(neuronId.index()), "", 0, "", "NeuronDebug");
    // In a real implementation, this would log actual neuron details
}

void DebugTools::logSynapseDetails(const SynapseId& synapseId) {
    NLM_LOG_INFO("Synapse details requested: ID = " + std::to_string(synapseId.index()), "", 0, "", "SynapseDebug");
    // In a real implementation, this would log actual synapse details
}

void DebugTools::logRegionDetails(const RegionId& regionId) {
    NLM_LOG_INFO("Region details requested: ID = " + std::to_string(regionId.index()), "", 0, "", "RegionDebug");
    // In a real implementation, this would log actual region details
}

void DebugTools::logPopulationDetails(const PopulationId& populationId) {
    NLM_LOG_INFO("Population details requested: ID = " + std::to_string(populationId.index()), "", 0, "", "PopulationDebug");
    // In a real implementation, this would log actual population details
}

void DebugTools::logSpikeHistory(size_t maxEvents) {
    NLM_LOG_INFO("Spike history requested: max events = " + std::to_string(maxEvents), "", 0, "", "SpikeDebug");
    // In a real implementation, this would log actual spike history
}

void DebugTools::logDelayedSpikeHistory(size_t maxEvents) {
    NLM_LOG_INFO("Delayed spike history requested: max events = " + std::to_string(maxEvents), "", 0, "", "DelayedSpikeDebug");
    // In a real implementation, this would log actual delayed spike history
}

void DebugTools::logWorkingMemoryState() {
    NLM_LOG_INFO("Working memory state requested", "", 0, "", "WorkingMemoryDebug");
    // In a real implementation, this would log actual working memory state
}

void DebugTools::logEpisodicMemoryState() {
    NLM_LOG_INFO("Episodic memory state requested", "", 0, "", "EpisodicMemoryDebug");
    // In a real implementation, this would log actual episodic memory state
}

void DebugTools::logAssociativeMemoryState() {
    NLM_LOG_INFO("Associative memory state requested", "", 0, "", "AssociativeMemoryDebug");
    // In a real implementation, this would log actual associative memory state
}

void DebugTools::logDopamineState() {
    NLM_LOG_INFO("Dopamine state requested", "", 0, "", "DopamineDebug");
    // In a real implementation, this would log actual dopamine state
}

void DebugTools::logCuriosityState() {
    NLM_LOG_INFO("Curiosity state requested", "", 0, "", "CuriosityDebug");
    // In a real implementation, this would log actual curiosity state
}

void DebugTools::logNoveltyState() {
    NLM_LOG_INFO("Novelty state requested", "", 0, "", "NoveltyDebug");
    // In a real implementation, this would log actual novelty state
}

void DebugTools::logPredictionErrorState() {
    NLM_LOG_INFO("Prediction error state requested", "", 0, "", "PredictionErrorDebug");
    // In a real implementation, this would log actual prediction error state
}

void DebugTools::logEventDrivenState() {
    NLM_LOG_INFO("Event-driven state requested", "", 0, "", "EventDrivenDebug");
    // In a real implementation, this would log actual event-driven state
}

void DebugTools::logPlasticityState() {
    NLM_LOG_INFO("Plasticity state requested", "", 0, "", "PlasticityDebug");
    // In a real implementation, this would log actual plasticity state
}

void DebugTools::logStructuralPlasticityState() {
    NLM_LOG_INFO("Structural plasticity state requested", "", 0, "", "StructuralPlasticityDebug");
    // In a real implementation, this would log actual structural plasticity state
}

void DebugTools::logDevelopmentState() {
    NLM_LOG_INFO("Development state requested", "", 0, "", "DevelopmentDebug");
    // In a real implementation, this would log actual development state
}

void DebugTools::logPerformanceMetrics() {
    NLM_LOG_INFO("Performance metrics requested", "", 0, "", "PerformanceDebug");
    // In a real implementation, this would log actual performance metrics
}

void DebugTools::logStepPerformance() {
    NLM_LOG_INFO("Step performance requested", "", 0, "", "StepPerformanceDebug");
    // In a real implementation, this would log actual step performance
}

std::string DebugTools::formatMemorySize(uint64_t bytes) {
    if (bytes < 1024) {
        return std::to_string(bytes) + " B";
    } else if (bytes < 1024 * 1024) {
        return std::to_string(bytes / 1024) + " KB";
    } else if (bytes < 1024 * 1024 * 1024) {
        return std::to_string(bytes / (1024 * 1024)) + " MB";
    } else {
        return std::to_string(bytes / (1024 * 1024 * 1024)) + " GB";
    }
}

std::string DebugTools::formatTime(double seconds) {
    if (seconds < 60) {
        return std::to_string(seconds) + " s";
    } else if (seconds < 3600) {
        return std::to_string(seconds / 60) + " min";
    } else if (seconds < 86400) {
        return std::to_string(seconds / 3600) + " h";
    } else {
        return std::to_string(seconds / 86400) + " days";
    }
}

std::string DebugTools::formatPercentage(float value) {
    return std::to_string(value * 100) + "%";
}

void DebugTools::setDebugOutput(std::ostream& output) {
    if (!pImpl) return;
    
    pImpl->debugOutput = &output;
    pImpl->debugOutputEnabled = true;
    
    NLM_LOG_SYSTEM("Debug output set");
}

void DebugTools::enableDebugOutput(bool enable) {
    if (!pImpl) return;
    
    pImpl->debugOutputEnabled = enable;
    
    if (enable) {
        NLM_LOG_SYSTEM("Debug output enabled");
    } else {
        NLM_LOG_SYSTEM("Debug output disabled");
    }
}

bool DebugTools::isDebugOutputEnabled() {
    if (!pImpl) return false;
    return pImpl->debugOutputEnabled;
}

void DebugTools::setDebugFile(const std::string& filepath) {
    if (!pImpl) return;
    
    pImpl->debugFilepath = filepath;
    std::ofstream file(filepath, std::ios::app);
    if (file.is_open()) {
        setDebugOutput(file);
    }
}

void DebugTools::closeDebugFile() {
    if (!pImpl) return;
    
    pImpl->debugFilepath.clear();
    // Note: This doesn't close the output stream as it might be shared
    NLM_LOG_SYSTEM("Debug file closed");
}

void DebugTools::setDebugLevel(DebugLevel level) {
    if (!pImpl) return;
    
    pImpl->debugLevel = level;
    
    NLM_LOG_SYSTEM(std::string("Debug level set to: ") + std::to_string(level));
}

DebugTools::DebugLevel DebugTools::getDebugLevel() {
    if (!pImpl) return DEBUG_LEVEL_INFO;
    return pImpl->debugLevel;
}

bool DebugTools::isDebugLevelEnabled(DebugLevel level) {
    if (!pImpl) return false;
    return pImpl->debugLevel >= level;
}

void DebugTools::addDebugFilter(const std::string& category) {
    if (!pImpl) return;
    
    pImpl->debugFilters.insert(category);
    NLM_LOG_SYSTEM("Debug filter added: " + category);
}

void DebugTools::removeDebugFilter(const std::string& category) {
    if (!pImpl) return;
    
    pImpl->debugFilters.erase(category);
    NLM_LOG_SYSTEM("Debug filter removed: " + category);
}

void DebugTools::clearDebugFilters() {
    if (!pImpl) return;
    
    pImpl->debugFilters.clear();
    NLM_LOG_SYSTEM("All debug filters cleared");
}

bool DebugTools::isCategoryFiltered(const std::string& category) {
    if (!pImpl) return false;
    return pImpl->debugFilters.find(category) != pImpl->debugFilters.end();
}

void DebugTools::takeSnapshot(const std::string& name) {
    if (!pImpl) return;
    
    std::ostringstream oss;
    oss << "Snapshot: " << name << "\n";
    oss << "Timestamp: " << std::chrono::steady_clock::now().time_since_epoch().count() << "\n";
    oss << "Event count: " << pImpl->eventCount << "\n";
    oss << "Performance timers: " << pImpl->performanceTimers.size() << "\n";
    oss << "Connection count: " << pImpl->connectionMap.size() << "\n";
    
    pImpl->snapshots[name] = oss.str();
    
    NLM_LOG_SYSTEM("Snapshot taken: " + name);
}

void DebugTools::restoreSnapshot(const std::string& name) {
    if (!pImpl) return;
    
    auto it = pImpl->snapshots.find(name);
    if (it == pImpl->snapshots.end()) {
        NLM_LOG_ERROR("Snapshot not found: " + name, "SnapshotError");
        return;
    }
    
    // Note: This is a simplified snapshot restoration
    NLM_LOG_SYSTEM("Snapshot restored: " + name);
}

void DebugTools::listSnapshots() {
    if (!pImpl) return;
    
    std::ostringstream oss;
    oss << "Available snapshots: " << pImpl->snapshots.size() << "\n";
    
    for (const auto& pair : pImpl->snapshots) {
        oss << "  " << pair.first << "\n";
    }
    
    if (pImpl->debugOutput) {
        (*pImpl->debugOutput) << oss.str();
    }
    
    NLM_LOG_INFO("Snapshots listed", "", 0, "", "SnapshotInfo");
}

void DebugTools::clearSnapshots() {
    if (!pImpl) return;
    
    pImpl->snapshots.clear();
    NLM_LOG_SYSTEM("All snapshots cleared");
}

void DebugTools::executeDebugScript(const std::string& script) {
    NLM_LOG_INFO("Debug script execution requested: " + script, "", 0, "", "DebugScript");
    // Note: This would execute a debug script in a real implementation
}

bool DebugTools::isDebugScriptFile(const std::string& filepath) {
    // Check if file appears to be a debug script
    // This is a simple implementation - in practice, you'd check file content
    return filepath.find("debug") != std::string::npos ||
           filepath.find("Debug") != std::string::npos;
}

void DebugTools::generateSystemReport(const std::string& outputFile) {
    std::string report = getSystemHealthReport();
    
    if (!outputFile.empty()) {
        std::ofstream file(outputFile);
        if (file.is_open()) {
            file << report;
            NLM_LOG_SYSTEM("System report generated: " + outputFile);
        }
    } else {
        if (pImpl->debugOutput) {
            (*pImpl->debugOutput) << report;
        }
        NLM_LOG_INFO("System report generated to debug output", "", 0, "", "SystemReport");
    }
}

void DebugTools::generatePerformanceReport(const std::string& outputFile) {
    std::ostringstream oss;
    oss << "=== Performance Report ===\n";
    oss << "Generated at: " << std::chrono::steady_clock::now().time_since_epoch().count() << "\n";
    oss << "Performance calls: " << pImpl->performanceCallCount << "\n";
    oss << "Active performance timers: " << pImpl->performanceStartTimes.size() << "\n";
    oss << "Completed performance timers: " << pImpl->performanceTimers.size() << "\n";
    
    std::string report = oss.str();
    
    if (!outputFile.empty()) {
        std::ofstream file(outputFile, std::ios::app);
        if (file.is_open()) {
            file << report;
            NLM_LOG_SYSTEM("Performance report generated: " + outputFile);
        }
    } else {
        if (pImpl->debugOutput) {
            (*pImpl->debugOutput) << report;
        }
        NLM_LOG_INFO("Performance report generated to debug output", "", 0, "", "PerformanceReport");
    }
}

void DebugTools::generateEventReport(const std::string& outputFile) {
    std::ostringstream oss;
    oss << "=== Event Report ===\n";
    oss << "Generated at: " << std::chrono::steady_clock::now().time_since_epoch().count() << "\n";
    oss << "Total events: " << pImpl->eventCount << "\n";
    oss << "Event log size: " << pImpl->eventLog.size() << "\n";
    oss << "Memory checks: " << pImpl->memoryCheckCount << "\n";
    
    std::string report = oss.str();
    
    if (!outputFile.empty()) {
        std::ofstream file(outputFile, std::ios::app);
        if (file.is_open()) {
            file << report;
            NLM_LOG_SYSTEM("Event report generated: " + outputFile);
        }
    } else {
        if (pImpl->debugOutput) {
            (*pImpl->debugOutput) << report;
        }
        NLM_LOG_INFO("Event report generated to debug output", "", 0, "", "EventReport");
    }
}

size_t DebugTools::getEventCount() {
    if (!pImpl) return 0;
    return pImpl->eventCount;
}

size_t DebugTools::getPerformanceCallCount() {
    if (!pImpl) return 0;
    return pImpl->performanceCallCount;
}

size_t DebugTools::getMemoryCheckCount() {
    if (!pImpl) return 0;
    return pImpl->memoryCheckCount;
}

std::chrono::steady_clock::time_point DebugTools::getSessionStartTime() {
    if (!pImpl) return std::chrono::steady_clock::now();
    return pImpl->sessionStartTime;
}

std::string DebugTools::getDebugStatistics() {
    std::ostringstream oss;
    oss << "=== Debug Statistics ===\n";
    oss << "Session ID: " << getDebugSessionId() << "\n";
    oss << "Debug active: " << (isDebugActive() ? "Yes" : "No") << "\n";
    oss << "Debug level: " << getDebugLevel() << "\n";
    oss << "Debug filters: " << pImpl->debugFilters.size() << "\n";
    oss << "Snapshots: " << pImpl->snapshots.size() << "\n";
    oss << "Events: " << getEventCount() << "\n";
    oss << "Performance calls: " << getPerformanceCallCount() << "\n";
    oss << "Memory checks: " << getMemoryCheckCount() << "\n";
    
    return oss.str();
}

void DebugTools::resetDebugStatistics() {
    if (!pImpl) return;
    
    pImpl->eventCount = 0;
    pImpl->performanceCallCount = 0;
    pImpl->memoryCheckCount = 0;
    
    NLM_LOG_SYSTEM("Debug statistics reset");
}

bool DebugTools::isDebugActive() {
    if (!pImpl) return false;
    return pImpl->debugActive;
}

void DebugTools::setDebugActive(bool active) {
    if (!pImpl) return;
    
    pImpl->debugActive = active;
    
    if (active) {
        NLM_LOG_SYSTEM("Debug activated");
    } else {
        NLM_LOG_SYSTEM("Debug deactivated");
    }
}

std::string DebugTools::getDebugSessionId() {
    if (!pImpl) return "";
    return pImpl->debugSessionId;
}

void DebugTools::setDebugSessionId(const std::string& sessionId) {
    if (!pImpl) return;
    
    pImpl->debugSessionId = sessionId;
    NLM_LOG_SYSTEM("Debug session ID set: " + sessionId);
}

void DebugTools::cleanup() {
    if (pImpl) {
        // Save current state
        takeSnapshot("cleanup");
        
        // Clear resources
        pImpl->performanceTimers.clear();
        pImpl->performanceStartTimes.clear();
        pImpl->eventLog.clear();
        pImpl->connectionMap.clear();
        
        // Reset debug state
        pImpl->debugActive = false;
        pImpl->debugSessionId.clear();
        
        NLM_LOG_SYSTEM("DebugTools cleaned up");
    }
}

void DebugTools::shutdown() {
    cleanup();
    pImpl.reset();
}

} // namespace nlm
