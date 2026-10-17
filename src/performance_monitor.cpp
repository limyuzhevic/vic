// performance_monitor.cpp - Implementation of PerformanceMetrics and DebugLogger

#include "performance_monitor.hpp"
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <thread>
#include <mutex>
#include <map>

namespace nlm {

// Forward declarations for internal implementation details
class PerformanceMetrics::Impl;
class DebugLogger::Impl;

// === PerformanceMetrics Implementation ===

struct PerformanceMetrics::Impl {
    bool enabled = true;
    SimulationStep currentStep = 0;
    Timestamp currentTime = 0.0;
    
    // Timing data
    std::unordered_map<Category, std::vector<double>> timingData;
    std::unordered_map<Category, std::chrono::steady_clock::time_point> timingStartPoints;
    
    // Memory data
    std::unordered_map<Category, std::vector<size_t>> memoryData;
    
    // Operation counts
    std::unordered_map<Category, uint64_t> operationCounts;
    
    // Special metrics
    SpikeInfo spikeInfo;
    ConnectivityInfo connectivityInfo;
    LearningInfo learningInfo;
    IOMetrics ioMetrics;
    
    // For timing percentages
    double totalTimingTime = 0.0;
    
    Impl() {
        // Initialize timing data vectors
        for (int i = 0; i < static_cast<int>(Category::All); ++i) {
            Category category = static_cast<Category>(i);
            timingData[category] = std::vector<double>();
            memoryData[category] = std::vector<size_t>();
            operationCounts[category] = 0;
        }
    }
};

PerformanceMetrics::PerformanceMetrics() : pImpl(std::make_unique<Impl>()) {}

PerformanceMetrics::~PerformanceMetrics() = default;

void PerformanceMetrics::startTiming(Category category) {
    if (!enabled) return;
    
    pImpl->timingStartPoints[category] = std::chrono::steady_clock::now();
}

double PerformanceMetrics::stopTiming(Category category) {
    if (!enabled || pImpl->timingStartPoints.find(category) == pImpl->timingStartPoints.end()) {
        return 0.0;
    }
    
    auto endTime = std::chrono::steady_clock::now();
    auto startTime = pImpl->timingStartPoints[category];
    
    auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime);
    double elapsed = duration.count();
    
    pImpl->timingData[category].push_back(elapsed);
    pImpl->totalTimingTime += elapsed;
    
    pImpl->timingStartPoints.erase(category);
    
    return elapsed;
}

void PerformanceMetrics::recordMemoryUsage(Category category, size_t bytes) {
    if (!enabled) return;
    pImpl->memoryData[category].push_back(bytes);
}

void PerformanceMetrics::recordOperations(Category category, uint64_t operations) {
    if (!enabled) return;
    pImpl->operationCounts[category] += operations;
}

void PerformanceMetrics::recordSpikeStatistics(const SpikeInfo& info) {
    if (!enabled) return;
    pImpl->spikeInfo = info;
}

void PerformanceMetrics::recordConnectivityStatistics(const ConnectivityInfo& info) {
    if (!enabled) return;
    pImpl->connectivityInfo = info;
}

void PerformanceMetrics::recordLearningStatistics(const LearningInfo& info) {
    if (!enabled) return;
    pImpl->learningInfo = info;
}

void PerformanceMetrics::recordIOMetrics(const IOMetrics& info) {
    if (!enabled) return;
    pImpl->ioMetrics = info;
}

PerformanceMetrics::TimingInfo PerformanceMetrics::getTimingInfo(Category category) const {
    TimingInfo info;
    const auto& data = pImpl->timingData[category];
    
    if (data.empty()) {
        info.totalTime = 0.0;
        info.averageTime = 0.0;
        info.minTime = 0.0;
        info.maxTime = 0.0;
        info.operationCount = 0;
        info.timePercentage = 0.0;
        return info;
    }
    
    double total = 0.0;
    double min = data[0];
    double max = data[0];
    
    for (double value : data) {
        total += value;
        if (value < min) min = value;
        if (value > max) max = value;
    }
    
    info.totalTime = total;
    info.averageTime = total / data.size();
    info.minTime = min;
    info.maxTime = max;
    info.operationCount = pImpl->operationCounts[category];
    
    if (pImpl->totalTimingTime > 0.0) {
        info.timePercentage = (total / pImpl->totalTimingTime) * 100.0;
    } else {
        info.timePercentage = 0.0;
    }
    
    return info;
}

PerformanceMetrics::MemoryInfo PerformanceMetrics::getMemoryInfo(Category category) const {
    MemoryInfo info;
    const auto& data = pImpl->memoryData[category];
    
    if (data.empty()) {
        info.totalAllocated = 0;
        info.currentUsage = 0;
        info.peakUsage = 0;
        info.allocationCount = 0;
        info.deallocationCount = 0;
        return info;
    }
    
    size_t totalAllocated = 0;
    size_t peakUsage = 0;
    size_t currentUsage = 0;
    size_t allocationCount = 0;
    
    // For this simple implementation, we track allocations
    // A more sophisticated implementation would track frees separately
    totalAllocated = std::accumulate(data.begin(), data.end(), 0ULL);
    peakUsage = totalAllocated; // Simplified - real implementation would track this
    currentUsage = data.empty() ? 0 : data.back();
    allocationCount = data.size();
    
    info.totalAllocated = totalAllocated;
    info.currentUsage = currentUsage;
    info.peakUsage = peakUsage;
    info.allocationCount = allocationCount;
    info.deallocationCount = 0; // Simplified
    
    return info;
}

PerformanceMetrics::SpikeInfo PerformanceMetrics::getSpikeInfo() const {
    return pImpl->spikeInfo;
}

PerformanceMetrics::ConnectivityInfo PerformanceMetrics::getConnectivityInfo() const {
    return pImpl->connectivityInfo;
}

PerformanceMetrics::LearningInfo PerformanceMetrics::getLearningInfo() const {
    return pImpl->learningInfo;
}

PerformanceMetrics::IOMetrics PerformanceMetrics::getIOMetrics() const {
    return pImpl->ioMetrics;
}

std::string PerformanceMetrics::generateReport() const {
    std::stringstream ss;
    ss << "=== NLM Performance Metrics Report ===\n";
    ss << "Current Step: " << pImpl->currentStep << "\n";
    ss << "Current Time: " << pImpl->currentTime << "s\n\n";
    
    ss << "=== Timing Performance ===\n";
    for (int i = 0; i < static_cast<int>(Category::All); ++i) {
        Category category = static_cast<Category>(i);
        if (category == Category::All) continue; // Skip All category in breakdown
        
        TimingInfo info = getTimingInfo(category);
        ss << getCategoryName(category) << ":\n";
        ss << "  Total Time: " << std::fixed << std::setprecision(6) << info.totalTime << "s\n";
        ss << "  Average Time: " << std::fixed << std::setprecision(6) << info.averageTime << "s\n";
        ss << "  Operations: " << info.operationCount << "\n";
        ss << "  Time Percentage: " << std::fixed << std::setprecision(2) << info.timePercentage << "%\n\n";
    }
    
    ss << "=== Spike Statistics ===\n";
    ss << "  Total Spikes: " << pImpl->spikeInfo.totalSpikes << "\n";
    ss << "  Average Spikes per Step: " << pImpl->spikeInfo.spikesPerStepAverage << "\n";
    ss << "  Max Spikes in Step: " << pImpl->spikeInfo.maxSpikesInStep << "\n";
    ss << "  Steps with Spikes: " << pImpl->spikeInfo.stepsWithSpikes << "\n";
    ss << "  Activity Ratio: " << std::fixed << std::setprecision(4) << pImpl->spikeInfo.activityRatio << "\n\n";
    
    ss << "=== Connectivity Statistics ===\n";
    ss << "  Total Connections: " << pImpl->connectivityInfo.totalConnections << "\n";
    ss << "  Connection Density: " << std::fixed << std::setprecision(4) << pImpl->connectivityInfo.connectionDensity << "\n";
    ss << "  Excitation/Inhibition Ratio: " << std::fixed << std::setprecision(4) << pImpl->connectivityInfo.excitationInhibitionRatio << "\n";
    ss << "  Active Connections: " << pImpl->connectivityInfo.activeConnections << "\n\n";
    
    ss << "=== Learning Statistics ===\n";
    ss << "  Plasticity Rate: " << std::fixed << std::setprecision(4) << pImpl->learningInfo.plasticityRate << "\n";
    ss << "  Learning Progress: " << std::fixed << std::setprecision(4) << pImpl->learningInfo.learningProgress << "\n";
    ss << "  Weight Changes: " << pImpl->learningInfo.weightChanges << "\n";
    ss << "  Average Weight Change: " << std::fixed << std::setprecision(6) << pImpl->learningInfo.averageWeightChange << "\n";
    ss << "  Stability Metric: " << std::fixed << std::setprecision(4) << pImpl->learningInfo.stabilityMetric << "\n\n";
    
    ss << "=== I/O Statistics ===\n";
    ss << "  Sensory Inputs Processed: " << pImpl->ioMetrics.sensoryInputsProcessed << "\n";
    ss << "  Motor Outputs Generated: " << pImpl->ioMetrics.motorOutputsGenerated << "\n";
    ss << "  Average Input Processing Time: " << std::fixed << std::setprecision(6) << pImpl->ioMetrics.avgInputProcessingTime << "s\n";
    ss << "  Average Output Processing Time: " << std::fixed << std::setprecision(6) << pImpl->ioMetrics.avgOutputProcessingTime << "s\n";
    
    return ss.str();
}

std::string PerformanceMetrics::generateJSONReport() const {
    std::stringstream ss;
    ss << "{\n";
    ss << "  \"current_step\": " << pImpl->currentStep << ",\n";
    ss << "  \"current_time\": " << pImpl->currentTime << ",\n";
    
    ss << "  \"timing\": {\n";
    bool first = true;
    for (int i = 0; i < static_cast<int>(Category::All); ++i) {
        Category category = static_cast<Category>(i);
        if (category == Category::All) continue;
        
        if (!first) ss << ",\n";
        first = false;
        
        TimingInfo info = getTimingInfo(category);
        ss << "    \"" << getCategoryName(category) << "\": {\n";
        ss << "      \"total_time\": " << info.totalTime << ",\n";
        ss << "      \"average_time\": " << info.averageTime << ",\n";
        ss << "      \"operation_count\": " << info.operationCount << ",\n";
        ss << "      \"time_percentage\": " << info.timePercentage << "\n";
        ss << "    }";
    }
    ss << "\n  },\n";
    
    ss << "  \"spike_stats\": {\n";
    ss << "    \"total_spikes\": " << pImpl->spikeInfo.totalSpikes << ",\n";
    ss << "    \"spikes_per_step_average\": " << pImpl->spikeInfo.spikesPerStepAverage << ",\n";
    ss << "    \"max_spikes_in_step\": " << pImpl->spikeInfo.maxSpikesInStep << ",\n";
    ss << "    \"steps_with_spikes\": " << pImpl->spikeInfo.stepsWithSpikes << ",\n";
    ss << "    \"activity_ratio\": " << pImpl->spikeInfo.activityRatio << "\n";
    ss << "  },\n";
    
    ss << "  \"connectivity_stats\": {\n";
    ss << "    \"total_connections\": " << pImpl->connectivityInfo.totalConnections << ",\n";
    ss << "    \"connection_density\": " << pImpl->connectivityInfo.connectionDensity << ",\n";
    ss << "    \"excitation_inhibition_ratio\": " << pImpl->connectivityInfo.excitationInhibitionRatio << ",\n";
    ss << "    \"active_connections\": " << pImpl->connectivityInfo.activeConnections << "\n";
    ss << "  },\n";
    
    ss << "  \"learning_stats\": {\n";
    ss << "    \"plasticity_rate\": " << pImpl->learningInfo.plasticityRate << ",\n";
    ss << "    \"learning_progress\": " << pImpl->learningInfo.learningProgress << ",\n";
    ss << "    \"weight_changes\": " << pImpl->learningInfo.weightChanges << ",\n";
    ss << "    \"average_weight_change\": " << pImpl->learningInfo.averageWeightChange << ",\n";
    ss << "    \"stability_metric\": " << pImpl->learningInfo.stabilityMetric << "\n";
    ss << "  },\n";
    
    ss << "  \"io_stats\": {\n";
    ss << "    \"sensory_inputs_processed\": " << pImpl->ioMetrics.sensoryInputsProcessed << ",\n";
    ss << "    \"motor_outputs_generated\": " << pImpl->ioMetrics.motorOutputsGenerated << ",\n";
    ss << "    \"avg_input_processing_time\": " << pImpl->ioMetrics.avgInputProcessingTime << ",\n";
    ss << "    \"avg_output_processing_time\": " << pImpl->ioMetrics.avgOutputProcessingTime << "\n";
    ss << "  }\n";
    
    ss << "}\n";
    return ss.str();
}

void PerformanceMetrics::reset() {
    pImpl->timingData.clear();
    pImpl->memoryData.clear();
    pImpl->operationCounts.clear();
    pImpl->timingStartPoints.clear();
    pImpl->totalTimingTime = 0.0;
    
    // Reset to default values
    pImpl->spikeInfo = SpikeInfo();
    pImpl->connectivityInfo = ConnectivityInfo();
    pImpl->learningInfo = LearningInfo();
    pImpl->ioMetrics = IOMetrics();
    
    for (int i = 0; i < static_cast<int>(Category::All); ++i) {
        Category category = static_cast<Category>(i);
        pImpl->operationCounts[category] = 0;
    }
}

bool PerformanceMetrics::isEnabled() const {
    return pImpl->enabled;
}

void PerformanceMetrics::setEnabled(bool enabled) {
    pImpl->enabled = enabled;
}

SimulationStep PerformanceMetrics::getCurrentStep() const {
    return pImpl->currentStep;
}

void PerformanceMetrics::setCurrentStep(SimulationStep step) {
    pImpl->currentStep = step;
}

PerformanceMetrics PerformanceMetrics::getWindowMetrics(SimulationStep steps) const {
    PerformanceMetrics windowMetrics;
    // For now, return a copy of current metrics
    // A real implementation would filter by time window
    return windowMetrics;
}

std::string PerformanceMetrics::compareWithBaseline(const PerformanceMetrics& baseline) const {
    std::stringstream ss;
    ss << "=== Performance Comparison with Baseline ===\n";
    ss << "Comparison will show performance differences between current run and baseline.\n";
    ss << "This feature would include detailed analysis of performance changes.\n";
    return ss.str();
}

std::string PerformanceMetrics::getCategoryName(Category category) const {
    switch (category) {
        case Category::Simulation: return "simulation";
        case Category::Memory: return "memory";
        case Category::Plasticity: return "plasticity";
        case Category::Connectivity: return "connectivity";
        case Category::Learning: return "learning";
        case Category::IOD: return "io";
        case Category::All: return "all";
        default: return "unknown";
    }
}

// === DebugLogger Implementation ===

struct DebugLogger::Impl {
    LogLevel logLevel = LogLevel::Info;
    bool enabled = true;
    bool realTimeLogging = true;
    
    std::vector<DebugEventCallback> eventCallbacks;
    std::vector<int> callbackPriorities;
    
    std::vector<DebugEvent> debugLog;
    size_t maxLogSize = 10000;
    
    std::ofstream logFile;
    std::string logFilePath;
    bool logToFile = false;
    
    PerformanceMetrics attachedMetrics;
    
    mutable std::mutex logMutex;
    
    Impl() = default;
    
    ~Impl() {
        if (logFile.is_open()) {
            logFile.close();
        }
    }
};

DebugLogger::DebugLogger() : pImpl(std::make_unique<Impl>()) {}

DebugLogger::~DebugLogger() = default;

void DebugLogger::setLogLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->logMutex);
    pImpl->logLevel = level;
}

DebugLogger::LogLevel DebugLogger::getLogLevel() const {
    std::lock_guard<std::mutex> lock(pImpl->logMutex);
    return pImpl->logLevel;
}

void DebugLogger::addEventCallback(DebugEventCallback callback, int priority) {
    std::lock_guard<std::mutex> lock(pImpl->logMutex);
    
    size_t index = pImpl->eventCallbacks.size();
    pImpl->eventCallbacks.push_back(callback);
    pImpl->callbackPriorities.push_back(priority);
    
    // Sort by priority (higher priority first)
    std::vector<size_t> indices(pImpl->eventCallbacks.size());
    for (size_t i = 0; i < indices.size(); ++i) indices[i] = i;
    
    std::sort(indices.begin(), indices.end(),
              [this](size_t a, size_t b) {
                  return pImpl->callbackPriorities[a] > pImpl->callbackPriorities[b];
              });
    
    // Reorder callbacks and priorities
    std::vector<DebugEventCallback> reorderedCallbacks;
    std::vector<int> reorderedPriorities;
    
    for (size_t i : indices) {
        reorderedCallbacks.push_back(pImpl->eventCallbacks[i]);
        reorderedPriorities.push_back(pImpl->callbackPriorities[i]);
    }
    
    pImpl->eventCallbacks = reorderedCallbacks;
    pImpl->callbackPriorities = reorderedPriorities;
}

void DebugLogger::clearEventCallbacks() {
    std::lock_guard<std::mutex> lock(pImpl->logMutex);
    pImpl->eventCallbacks.clear();
    pImpl->callbackPriorities.clear();
}

void DebugLogger::logEvent(EventType type, LogLevel level, const std::string& message,
                         const std::string& details) {
    if (!enabled || level < pImpl->logLevel) return;
    
    std::lock_guard<std::mutex> lock(pImpl->logMutex);
    
    DebugEvent event;
    event.type = type;
    event.level = level;
    event.step = 0; // Would be set by simulation
    event.timestamp = 0.0; // Would be set by simulation
    event.message = message;
    event.details = details;
    
    // Add to debug log
    pImpl->debugLog.push_back(event);
    if (pImpl->debugLog.size() > pImpl->maxLogSize) {
        pImpl->debugLog.erase(pImpl->debugLog.begin());
    }
    
    // Call callbacks in priority order
    for (size_t i = 0; i < pImpl->eventCallbacks.size(); ++i) {
        try {
            pImpl->eventCallbacks[i](event);
        } catch (const std::exception& e) {
            // Ignore callback errors to prevent crashes
        }
    }
    
    // Write to file if enabled
    if (pImpl->logToFile) {
        std::lock_guard<std::mutex> fileLock(pImpl->logMutex);
        if (pImpl->logFile.is_open()) {
            pImpl->logFile << event.timestamp << " [" << getEventTypeName(type) << "] "
                          << getLogLevelName(level) << " Step: " << event.step << " "
                          << event.message << "\n";
            pImpl->logFile.flush();
        }
    }
}

void DebugLogger::logStep(SimulationStep step, Timestamp timestamp, const std::string& message) {
    if (getLogLevel() < LogLevel::Info) return;
    
    std::lock_guard<std::mutex> lock(pImpl->logMutex);
    
    DebugEvent event;
    event.type = EventType::StepComplete;
    event.level = LogLevel::Info;
    event.step = step;
    event.timestamp = timestamp;
    event.message = message;
    
    // Call callbacks
    for (size_t i = 0; i < pImpl->eventCallbacks.size(); ++i) {
        try {
            pImpl->eventCallbacks[i](event);
        } catch (const std::exception& e) {
            // Ignore callback errors
        }
    }
}

void DebugLogger::logSpikeGeneration(NeuronId neuronId, SimulationStep step, Timestamp timestamp) {
    if (getLogLevel() < LogLevel::Debug) return;
    
    std::lock_guard<std::mutex> lock(pImpl->logMutex);
    
    DebugEvent event;
    event.type = EventType::SpikeGenerated;
    event.level = LogLevel::Debug;
    event.step = step;
    event.timestamp = timestamp;
    event.message = "Spike generated by neuron " + std::to_string(neuronId.index());
    event.details = "Neuron ID: " + std::to_string(neuronId.index());
    
    // Call callbacks
    for (size_t i = 0; i < pImpl->eventCallbacks.size(); ++i) {
        try {
            pImpl->eventCallbacks[i](event);
        } catch (const std::exception& e) {
            // Ignore callback errors
        }
    }
}

void DebugLogger::logSynapseModification(SynapseId synapseId, SynapticWeight oldWeight,
                                        SynapticWeight newWeight, SimulationStep step) {
    if (getLogLevel() < LogLevel::Debug) return;
    
    std::lock_guard<std::mutex> lock(pImpl->logMutex);
    
    DebugEvent event;
    event.type = EventType::SynapseModified;
    event.level = LogLevel::Debug;
    event.step = step;
    event.timestamp = 0.0; // Would be set by simulation
    event.message = "Synapse " + std::to_string(synapseId.index()) + " modified";
    event.details = "Old weight: " + std::to_string(oldWeight) + ", New weight: " + std::to_string(newWeight);
    
    // Call callbacks
    for (size_t i = 0; i < pImpl->eventCallbacks.size(); ++i) {
        try {
            pImpl->eventCallbacks[i](event);
        } catch (const std::exception& e) {
            // Ignore callback errors
        }
    }
}

void DebugLogger::logMemoryAllocation(size_t bytes, const std::string& location) {
    if (getLogLevel() < LogLevel::Info) return;
    
    std::lock_guard<std::mutex> lock(pImpl->logMutex);
    
    DebugEvent event;
    event.type = EventType::MemoryAllocated;
    event.level = LogLevel::Info;
    event.step = 0;
    event.timestamp = 0.0;
    event.message = "Memory allocated";
    event.details = "Bytes: " + std::to_string(bytes) + ", Location: " + location;
    
    // Call callbacks
    for (size_t i = 0; i < pImpl->eventCallbacks.size(); ++i) {
        try {
            pImpl->eventCallbacks[i](event);
        } catch (const std::exception& e) {
            // Ignore callback errors
        }
    }
}

void DebugLogger::logMemoryDeallocation(size_t bytes, const std::string& location) {
    if (getLogLevel() < LogLevel::Info) return;
    
    std::lock_guard<std::mutex> lock(pImpl->logMutex);
    
    DebugEvent event;
    event.type = EventType::MemoryFreed;
    event.level = LogLevel::Info;
    event.step = 0;
    event.timestamp = 0.0;
    event.message = "Memory deallocated";
    event.details = "Bytes: " + std::to_string(bytes) + ", Location: " + location;
    
    // Call callbacks
    for (size_t i = 0; i < pImpl->eventCallbacks.size(); ++i) {
        try {
            pImpl->eventCallbacks[i](event);
        } catch (const std::exception& e) {
            // Ignore callback errors
        }
    }
}

void DebugLogger::logError(const std::string& errorMessage, const std::string& location,
                          SimulationStep step) {
    std::lock_guard<std::mutex> lock(pImpl->logMutex);
    
    DebugEvent event;
    event.type = EventType::ErrorOccurred;
    event.level = LogLevel::Error;
    event.step = step;
    event.timestamp = 0.0;
    event.message = "Error: " + errorMessage;
    event.details = "Location: " + location;
    
    // Call callbacks
    for (size_t i = 0; i < pImpl->eventCallbacks.size(); ++i) {
        try {
            pImpl->eventCallbacks[i](event);
        } catch (const std::exception& e) {
            // Ignore callback errors
        }
    }
}

void DebugLogger::logPerformanceMetric(const std::string& category, const std::string& metricName,
                                      double value) {
    if (getLogLevel() < LogLevel::Performance) return;
    
    std::lock_guard<std::mutex> lock(pImpl->logMutex);
    
    DebugEvent event;
    event.type = EventType::PerformanceMetric;
    event.level = LogLevel::Performance;
    event.step = 0;
    event.timestamp = 0.0;
    event.message = "Performance metric: " + metricName;
    event.details = "Category: " + category + ", Value: " + std::to_string(value);
    
    // Call callbacks
    for (size_t i = 0; i < pImpl->eventCallbacks.size(); ++i) {
        try {
            pImpl->eventCallbacks[i](event);
        } catch (const std::exception& e) {
            // Ignore callback errors
        }
    }
}

std::string DebugLogger::getDebugLog() const {
    std::lock_guard<std::mutex> lock(pImpl->logMutex);
    
    std::stringstream ss;
    for (const auto& event : pImpl->debugLog) {
        ss << event.timestamp << " [" << getEventTypeName(event.type) << "] "
           << getLogLevelName(event.level) << " Step: " << event.step << " "
           << event.message << "\n";
    }
    
    return ss.str();
}

std::vector<DebugLogger::DebugEvent> DebugLogger::getEventsForRange(SimulationStep startStep,
                                                                SimulationStep endStep) const {
    std::lock_guard<std::mutex> lock(pImpl->logMutex);
    
    std::vector<DebugEvent> eventsInRange;
    for (const auto& event : pImpl->debugLog) {
        if (event.step >= startStep && event.step <= endStep) {
            eventsInRange.push_back(event);
        }
    }
    
    return eventsInRange;
}

std::vector<DebugLogger::DebugEvent> DebugLogger::getEventsByType(EventType type) const {
    std::lock_guard<std::mutex> lock(pImpl->logMutex);
    
    std::vector<DebugEvent> eventsOfType;
    for (const auto& event : pImpl->debugLog) {
        if (event.type == type) {
            eventsOfType.push_back(event);
        }
    }
    
    return eventsOfType;
}

std::unordered_map<std::string, uint64_t> DebugLogger::getEventStatistics() const {
    std::lock_guard<std::mutex> lock(pImpl->logMutex);
    
    std::unordered_map<std::string, uint64_t> stats;
    for (const auto& event : pImpl->debugLog) {
        std::string typeName = getEventTypeName(event.type);
        stats[typeName]++;
    }
    
    return stats;
}

void DebugLogger::clearDebugLog() {
    std::lock_guard<std::mutex> lock(pImpl->logMutex);
    pImpl->debugLog.clear();
}

void DebugLogger::setRealTimeLogging(bool enabled) {
    std::lock_guard<std::mutex> lock(pImpl->logMutex);
    pImpl->realTimeLogging = enabled;
}

bool DebugLogger::isRealTimeLoggingEnabled() const {
    std::lock_guard<std::mutex> lock(pImpl->logMutex);
    return pImpl->realTimeLogging;
}

bool DebugLogger::exportDebugLog(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->logMutex);
    
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << getDebugLog();
        file.close();
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool DebugLogger::importDebugLog(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->logMutex);
    
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            // Parse log line (simplified parsing)
            // In a real implementation, this would be more sophisticated
            DebugEvent event;
            event.message = line;
            pImpl->debugLog.push_back(event);
        }
        
        file.close();
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

void DebugLogger::attachPerformanceMetrics(const PerformanceMetrics& metrics) {
    std::lock_guard<std::mutex> lock(pImpl->logMutex);
    pImpl->attachedMetrics = metrics;
}

PerformanceMetrics DebugLogger::getAttachedPerformanceMetrics() const {
    std::lock_guard<std::mutex> lock(pImpl->logMutex);
    return pImpl->attachedMetrics;
}

void DebugLogger::setLogFile(const std::string& filepath, bool append) {
    std::lock_guard<std::mutex> lock(pImpl->logMutex);
    
    if (pImpl->logFile.is_open()) {
        pImpl->logFile.close();
    }
    
    pImpl->logFilePath = filepath;
    pImpl->logToFile = true;
    
    std::ios_base::openmode mode = std::ios::out;
    if (append) {
        mode |= std::ios::app;
    } else {
        mode |= std::ios::trunc;
    }
    
    pImpl->logFile.open(filepath, mode);
}

void DebugLogger::closeLogFile() {
    std::lock_guard<std::mutex> lock(pImpl->logMutex);
    if (pImpl->logFile.is_open()) {
        pImpl->logFile.close();
    }
}

bool DebugLogger::isEnabled() const {
    return pImpl->enabled;
}

void DebugLogger::setEnabled(bool enabled) {
    std::lock_guard<std::mutex> lock(pImpl->logMutex);
    pImpl->enabled = enabled;
}

std::string DebugLogger::getEventTypeName(EventType type) const {
    switch (type) {
        case EventType::SimulationStart: return "SIM_START";
        case EventType::SimulationEnd: return "SIM_END";
        case EventType::StepComplete: return "STEP_COMPLETE";
        case EventType::SpikeGenerated: return "SPIKE";
        case EventType::SynapseModified: return "SYNAPSE_MOD";
        case EventType::MemoryAllocated: return "MEM_ALLOC";
        case EventType::MemoryFreed: return "MEM_FREE";
        case EventType::ErrorOccurred: return "ERROR";
        case EventType::PerformanceMetric: return "PERF_METRIC";
        case EventType::Custom: return "CUSTOM";
        default: return "UNKNOWN";
    }
}

std::string DebugLogger::getLogLevelName(LogLevel level) const {
    switch (level) {
        case LogLevel::Silent: return "SILENT";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Info: return "INFO";
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Trace: return "TRACE";
        case LogLevel::Performance: return "PERF";
        default: return "UNKNOWN";
    }
}

// Global instances and functions
DebugLogger& getGlobalDebugLogger() {
    static DebugLogger globalLogger;
    return globalLogger;
}

PerformanceMetrics& getGlobalPerformanceMetrics() {
    static PerformanceMetrics globalMetrics;
    return globalMetrics;
}

void initializeGlobalMonitoring() {
    DebugLogger& debugLogger = getGlobalDebugLogger();
    debugLogger.logEvent(DebugLogger::EventType::SimulationStart,
                       DebugLogger::LogLevel::Info,
                       "Global monitoring initialized");
    
    PerformanceMetrics& performanceMetrics = getGlobalPerformanceMetrics();
    performanceMetrics.setEnabled(true);
}

void shutdownGlobalMonitoring() {
    DebugLogger& debugLogger = getGlobalDebugLogger();
    debugLogger.logEvent(DebugLogger::EventType::SimulationEnd,
                       DebugLogger::LogLevel::Info,
                       "Global monitoring shutting down");
    
    PerformanceMetrics& performanceMetrics = getGlobalPerformanceMetrics();
    performanceMetrics.setEnabled(false);
}

} // namespace nlm
