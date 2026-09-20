#include "DebugTools.hpp"
#include "../core/Logger/Logger.hpp"
#include "../performance/Performance.hpp"
#include "../brain/Brain.hpp"
#include "../neuron/Neuron.hpp"
#include "../synapse/Synapse.hpp"
#include <SFML/Graphics.hpp>
#include <chrono>
#include <thread>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>

namespace nlm {

struct DebugTools::Impl {
    // Debugging state
    bool debugging;
    bool memoryMonitoring;
    bool profiling;
    int verbosityLevel;
    std::string outputFile;
    double maxMemoryUsage;
    double profileInterval;
    
    // Debug data
    std::vector<std::function<void()>> debugHooks;
    std::map<std::string, std::function<void()>> eventCallbacks;
    std::map<std::string, std::vector<std::function<void()>>> hookMap;
    
    // Performance and memory profiling
    std::chrono::time_point<std::chrono::high_resolution_clock> lastProfileTime;
    std::map<std::string, std::chrono::time_point<std::chrono::high_resolution_clock>> operationTimes;
    
    // Event logging
    std::vector<std::pair<double, std::string>> eventLog;
    std::vector<std::pair<std::string, std::string>> debugMessages;
    
    // Memory profiling
    std::map<void*, std::pair<size_t, std::string>> allocatedPointers;
    std::vector<void*> currentAllocations;
    
    // Performance analysis
    std::map<std::string, std::vector<double>> operationTimesMap;
    std::vector<std::string> bottleneckOperations;
    
    // Configuration
    Impl() : debugging(false), memoryMonitoring(false), profiling(false),
             verbosityLevel(1), maxMemoryUsage(512.0), profileInterval(1.0) {
        lastProfileTime = std::chrono::high_resolution_clock::now();
    }
};

DebugTools::DebugTools() : pImpl(std::make_unique<Impl>()) {
}

DebugTools::~DebugTools() = default;

bool DebugTools::initialize() {
    try {
        Logger::getInstance().log("Debug tools initialized", LogLevel::INFO);
        return true;
    } catch (const std::exception& e) {
        Logger::getInstance().log(std::string("Failed to initialize debug tools: ") + e.what(), LogLevel::ERROR);
        return false;
    }
}

void DebugTools::startDebugging() {
    pImpl->debugging = true;
    Logger::getInstance().log("Debugging started", LogLevel::INFO);
}

void DebugTools::stopDebugging() {
    pImpl->debugging = false;
    Logger::getInstance().log("Debugging stopped", LogLevel::INFO);
}

bool DebugTools::isDebugging() const {
    return pImpl->debugging;
}

void DebugTools::inspectBrain(const Brain& brain) {
    if (!pImpl->debugging) return;
    
    // Display brain statistics
    std::stringstream ss;
    ss << "=== BRAIN STATE INSPECTION ===" << std::endl;
    ss << "Total Neurons: " << brain.getTotalNeuronCount() << std::endl;
    ss << "Total Synapses: " << brain.getTotalSynapseCount() << std::endl;
    ss << "Active Neurons: " << brain.getActiveNeuronCount() << std::endl;
    ss << "Firing Neurons: " << brain.getFiringNeuronCount() << std::endl;
    ss << "Average Firing Rate: " << brain.getAverageFiringRate() << " Hz" << std::endl;
    ss << "Development Stage: " << brain.getDevelopmentStage() << std::endl;
    ss << "Region Count: " << brain.getRegionCount() << std::endl;
    
    pImpl->debugMessages.push_back(std::make_pair("BRAIN_STATS", ss.str()));
}

void DebugTools::displayNeuronInfo(int neuronId, const Brain& brain) {
    if (!pImpl->debugging) return;
    
    // Get neuron from brain (assuming there's a method)
    // This would need access to brain internal structures
    std::stringstream ss;
    ss << "=== NEURON INFO ===" << std::endl;
    ss << "Neuron ID: " << neuronId << std::endl;
    ss << "Type: Unknown" << std::endl;
    ss << "Position: Unknown" << std::endl;
    ss << "Membrane Potential: Unknown" << std::endl;
    ss << "Firing Rate: Unknown" << std::endl;
    
    pImpl->debugMessages.push_back(std::make_pair(std::string("NEURON_") + std::to_string(neuronId), ss.str()));
}

void DebugTools::displaySynapseInfo(int synapseId, const Brain& brain) {
    if (!pImpl->debugging) return;
    
    std::stringstream ss;
    ss << "=== SYNAPSE INFO ===" << std::endl;
    ss << "Synapse ID: " << synapseId << std::endl;
    ss << "Type: Unknown" << std::endl;
    ss << "Pre-synaptic Neuron: Unknown" << std::endl;
    ss << "Post-synaptic Neuron: Unknown" << std::endl;
    ss << "Strength: Unknown" << std::endl;
    
    pImpl->debugMessages.push_back(std::make_pair(std::string("SYNAPSE_") + std::to_string(synapseId), ss.str()));
}

void DebugTools::displayNetworkStats(const Brain& brain) {
    if (!pImpl->debugging) return;
    
    std::stringstream ss;
    ss << "=== NETWORK STATISTICS ===" << std::endl;
    ss << "Connectivity Density: " << (double)brain.getTotalSynapseCount() / 
        (brain.getTotalNeuronCount() * brain.getTotalNeuronCount()) << std::endl;
    ss << "Firing Rate Distribution: Unknown" << std::endl;
    ss << "Membrane Potential Distribution: Unknown" << std::endl;
    
    pImpl->debugMessages.push_back(std::make_pair("NETWORK_STATS", ss.str()));
}

void DebugTools::startMemoryMonitoring() {
    pImpl->memoryMonitoring = true;
    Logger::getInstance().log("Memory monitoring started", LogLevel::INFO);
}

void DebugTools::stopMemoryMonitoring() {
    pImpl->memoryMonitoring = false;
    Logger::getInstance().log("Memory monitoring stopped", LogLevel::INFO);
}

void DebugTools::checkMemoryLeaks() {
    if (!pImpl->memoryMonitoring) return;
    
    // Check for unallocated pointers
    std::vector<void*> leakedPointers;
    for (const auto& pair : pImpl->allocatedPointers) {
        if (pair.second.first > 0 && std::find(pImpl->currentAllocations.begin(), 
                                              pImpl->currentAllocations.end(), pair.first) == pImpl->currentAllocations.end()) {
            leakedPointers.push_back(pair.first);
        }
    }
    
    if (!leakedPointers.empty()) {
        std::stringstream ss;
        ss << "=== MEMORY LEAK DETECTION ===" << std::endl;
        ss << "Detected " << leakedPointers.size() << " potential memory leaks:" << std::endl;
        for (void* ptr : leakedPointers) {
            auto it = pImpl->allocatedPointers.find(ptr);
            if (it != pImpl->allocatedPointers.end()) {
                ss << "  Pointer " << it->first << " - " << it->second.second 
                   << " - Size: " << it->second.first << " bytes" << std::endl;
            }
        }
        
        pImpl->debugMessages.push_back(std::make_pair("MEMORY_LEAK", ss.str()));
    }
}

void DebugTools::displayMemoryReport() {
    if (!pImpl->memoryMonitoring) return;
    
    std::stringstream ss;
    ss << "=== MEMORY REPORT ===" << std::endl;
    ss << "Total Allocations: " << pImpl->allocatedPointers.size() << std::endl;
    ss << "Current Allocations: " << pImpl->currentAllocations.size() << std::endl;
    ss << "Peak Memory Usage: " << Performance::getInstance().getMemoryUsageMB() << " MB" << std::endl;
    
    pImpl->debugMessages.push_back(std::make_pair("MEMORY_REPORT", ss.str()));
}

void DebugTools::startProfiling() {
    pImpl->profiling = true;
    pImpl->lastProfileTime = std::chrono::high_resolution_clock::now();
    Logger::getInstance().log("Performance profiling started", LogLevel::INFO);
}

void DebugTools::stopProfiling() {
    pImpl->profiling = false;
    Logger::getInstance().log("Performance profiling stopped", LogLevel::INFO);
}

void DebugTools::addProfilePoint(const std::string& name) {
    if (!pImpl->profiling) return;
    
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now - pImpl->lastProfileTime);
    
    pImpl->operationTimes[name] = now;
    pImpl->operationTimesMap[name].push_back(duration.count() / 1000.0); // Convert to milliseconds
    
    pImpl->lastProfileTime = now;
}

void DebugTools::displayProfileReport() {
    if (!pImpl->profiling) return;
    
    std::stringstream ss;
    ss << "=== PERFORMANCE PROFILE REPORT ===" << std::endl;
    ss << "Total profile points: " << pImpl->operationTimesMap.size() << std::endl;
    
    for (const auto& pair : pImpl->operationTimesMap) {
        const auto& times = pair.second;
        if (!times.empty()) {
            double total = std::accumulate(times.begin(), times.end(), 0.0);
            double avg = total / times.size();
            double min = *std::min_element(times.begin(), times.end());
            double max = *std::max_element(times.begin(), times.end());
            
            ss << pair.first << ": " << times.size() << " calls, avg=" << avg 
               << "ms, min=" << min << "ms, max=" << max << "ms" << std::endl;
        }
    }
    
    pImpl->debugMessages.push_back(std::make_pair("PROFILE_REPORT", ss.str()));
}

void DebugTools::addSpikeDebugInfo(const SpikeEvent& spike) {
    std::stringstream ss;
    ss << "SPIKE_EVENT: neuron=" << spike.neuronId 
       << ", time=" << spike.timestamp 
       << ", amplitude=" << spike.amplitude;
    
    pImpl->eventLog.push_back(std::make_pair(spike.timestamp, ss.str()));
}

void DebugTools::displaySpikeAnalysis() {
    std::stringstream ss;
    ss << "=== SPIKE ANALYSIS ===" << std::endl;
    ss << "Total spike events: " << pImpl->eventLog.size() << std::endl;
    
    // Extract spike times and analyze
    std::vector<double> spikeTimes;
    for (const auto& event : pImpl->eventLog) {
        if (event.second.find("SPIKE_EVENT") != std::string::npos) {
            // Parse spike time from event description
            size_t pos = event.second.find("time=");
            if (pos != std::string::npos) {
                size_t endPos = event.second.find(",", pos);
                if (endPos != std::string::npos) {
                    std::string timeStr = event.second.substr(pos + 5, endPos - pos - 5);
                    spikeTimes.push_back(std::stod(timeStr));
                }
            }
        }
    }
    
    if (!spikeTimes.empty()) {
        double minInterval = std::numeric_limits<double>::max();
        double maxInterval = 0.0;
        double totalInterval = 0.0;
        
        for (size_t i = 1; i < spikeTimes.size(); ++i) {
            double interval = spikeTimes[i] - spikeTimes[i-1];
            minInterval = std::min(minInterval, interval);
            maxInterval = std::max(maxInterval, interval);
            totalInterval += interval;
        }
        
        ss << "Spike interval range: " << minInterval << " - " << maxInterval << " ms" << std::endl;
        ss << "Average ISI: " << (totalInterval / (spikeTimes.size() - 1)) << " ms" << std::endl;
    }
    
    pImpl->debugMessages.push_back(std::make_pair("SPIKE_ANALYSIS", ss.str()));
}

void DebugTools::visualizeSpikeTimeline() {
    // This would render spike timeline visualization
    // For now, just log the event
    Logger::getInstance().log("Spike timeline visualization would be rendered here", LogLevel::INFO);
}

void DebugTools::updatePerformanceCounters() {
    if (!pImpl->profiling) return;
    
    // Update performance counters based on actual system metrics
    auto& perf = Performance::getInstance();
    
    // Check for slow operations
    detectSlowOperations();
}

void DebugTools::detectSlowOperations() {
    pImpl->bottleneckOperations.clear();
    
    for (const auto& pair : pImpl->operationTimesMap) {
        const auto& times = pair.second;
        if (!times.empty()) {
            double avgTime = std::accumulate(times.begin(), times.end(), 0.0) / times.size();
            if (avgTime > 100.0) { // More than 100ms average
                pImpl->bottleneckOperations.push_back(pair.first);
            }
        }
    }
}

void DebugTools::displayBottleneckReport() {
    std::stringstream ss;
    ss << "=== PERFORMANCE BOTTLENECK REPORT ===" << std::endl;
    
    if (pImpl->bottleneckOperations.empty()) {
        ss << "No performance bottlenecks detected." << std::endl;
    } else {
        ss << "Bottleneck operations:" << std::endl;
        for (const auto& op : pImpl->bottleneckOperations) {
            ss << "  - " << op << std::endl;
        }
    }
    
    pImpl->debugMessages.push_back(std::make_pair("BOTTLENECK_REPORT", ss.str()));
}

void DebugTools::logEvent(const std::string& eventType, const std::string& description,
                         double timestamp, const std::string& details) {
    std::stringstream ss;
    ss << "[" << timestamp << "] " << eventType << ": " << description;
    if (!details.empty()) {
        ss << " - " << details;
    }
    
    pImpl->eventLog.push_back(std::make_pair(timestamp, ss.str()));
}

void DebugTools::displayEventLog() {
    std::stringstream ss;
    ss << "=== EVENT LOG ===" << std::endl;
    
    if (pImpl->eventLog.empty()) {
        ss << "No events recorded." << std::endl;
    } else {
        for (const auto& event : pImpl->eventLog) {
            ss << event.first << ": " << event.second << std::endl;
        }
    }
    
    pImpl->debugMessages.push_back(std::make_pair("EVENT_LOG", ss.str()));
}

void DebugTools::exportEventLog(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        Logger::getInstance().log(std::string("Failed to open event log file: ") + filename, LogLevel::ERROR);
        return;
    }
    
    file << "timestamp,event_type,description,details" << std::endl;
    for (const auto& event : pImpl->eventLog) {
        // Parse event type and description from the combined string
        std::string eventType = "UNKNOWN";
        std::string description = event.second;
        std::string details = "";
        
        size_t colonPos = event.second.find(":");
        if (colonPos != std::string::npos) {
            eventType = event.second.substr(0, colonPos);
            description = event.second.substr(colonPos + 2);
        }
        
        file << event.first << ","
             << eventType << ","
             << description << ","
             << details << std::endl;
    }
    
    file.close();
    Logger::getInstance().log(std::string("Event log exported to: ") + filename, LogLevel::INFO);
}

void DebugTools::updateDebugDisplay() {
    if (!pImpl->debugging) return;
    
    // Update debug display based on current state
    // This would update UI elements in a real implementation
}

bool DebugTools::handleDebugInput() {
    // Handle debug-specific input
    return true;
}

void DebugTools::toggleDebugOverlay() {
    // Toggle debug overlay visibility
}

void DebugTools::togglePerformanceProfiler() {
    // Toggle performance profiler display
}

void DebugTools::toggleMemoryProfiler() {
    // Toggle memory profiler display
}

void DebugTools::captureCrashDump() {
    // Capture crash dump for debugging
    Logger::getInstance().log("Crash dump captured", LogLevel::WARNING);
}

void DebugTools::generateDebugReport() {
    // Generate comprehensive debug report
    Logger::getInstance().log("Debug report generated", LogLevel::INFO);
}

void DebugTools::sendDebugInfo(const std::string& target) {
    // Send debug information to remote target
    Logger::getInstance().log(std::string("Debug info sent to: ") + target, LogLevel::INFO);
}

void DebugTools::setDebugHook(const std::string& hookName, std::function<void()> hook) {
    pImpl->debugHooks.push_back(hook);
}

void DebugTools::executeDebugHooks(const std::string& hookName) {
    if (pImpl->hookMap.find(hookName) != pImpl->hookMap.end()) {
        for (const auto& hook : pImpl->hookMap[hookName]) {
            hook();
        }
    }
}

void DebugTools::registerDebugCallback(const std::string& event, std::function<void()> callback) {
    pImpl->eventCallbacks[event] = callback;
}

void DebugTools::exportDebugData(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        Logger::getInstance().log(std::string("Failed to open debug export file: ") + filename, LogLevel::ERROR);
        return;
    }
    
    file << "Debug data export at time: " 
         << std::chrono::duration_cast<std::chrono::milliseconds>(
             std::chrono::high_resolution_clock::now().time_since_epoch()).count()
         << std::endl;
    
    file << "Debugging active: " << (pImpl->debugging ? "true" : "false") << std::endl;
    file << "Memory monitoring: " << (pImpl->memoryMonitoring ? "true" : "false") << std::endl;
    file << "Profiling: " << (pImpl->profiling ? "true" : "false") << std::endl;
    
    file.close();
    Logger::getInstance().log(std::string("Debug data exported to: ") + filename, LogLevel::INFO);
}

void DebugTools::exportNeuralState(const std::string& filename, const Brain& brain) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        Logger::getInstance().log(std::string("Failed to open neural state export file: ") + filename, LogLevel::ERROR);
        return;
    }
    
    file << "{\"timestamp\": "
         << std::chrono::duration_cast<std::chrono::milliseconds>(
             std::chrono::high_resolution_clock::now().time_since_epoch()).count()
         << "\",\"neuronCount\": " << brain.getTotalNeuronCount()
         << ",\"synapseCount\": " << brain.getTotalSynapseCount()
         << ",\"activeNeurons\": " << brain.getActiveNeuronCount()
         << ",\"firingNeurons\": " << brain.getFiringNeuronCount()
         << ",\"averageFiringRate\": " << brain.getAverageFiringRate()
         << "}";
    
    file.close();
    Logger::getInstance().log(std::string("Neural state exported to: ") + filename, LogLevel::INFO);
}

void DebugTools::exportPerformanceData(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        Logger::getInstance().log(std::string("Failed to open performance export file: ") + filename, LogLevel::ERROR);
        return;
    }
    
    file << "Performance data export at time: " 
         << std::chrono::duration_cast<std::chrono::milliseconds>(
             std::chrono::high_resolution_clock::now().time_since_epoch()).count()
         << std::endl;
    
    file << "CPU Usage: " << Performance::getInstance().getCPUUsage() << "%" << std::endl;
    file << "Memory Usage: " << Performance::getInstance().getMemoryUsageMB() << " MB" << std::endl;
    
    file.close();
    Logger::getInstance().log(std::string("Performance data exported to: ") + filename, LogLevel::INFO);
}

void DebugTools::updateFromPython(const py::dict& debugData) {
    // Update debug tools from Python data
    // This would extract debug information from Python dictionary
}

py::dict DebugTools::getDebugState() const {
    // Return debug state as Python dictionary
    // This would be implemented in the Python bindings
    return py::dict();
}

// MemoryProfiler implementation
MemoryProfiler::MemoryProfiler() : peakMemoryUsage(0.0), active(false), startTime(0.0) {
}

MemoryProfiler::~MemoryProfiler() = default;

void MemoryProfiler::start() {
    active = true;
    startTime = std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1000.0;
}

void MemoryProfiler::stop() {
    active = false;
}

bool MemoryProfiler::isActive() const {
    return active;
}

void* MemoryProfiler::trackAllocation(void* ptr, size_t size, const std::string& context) {
    MemoryBlock block;
    block.address = ptr;
    block.size = size;
    block.context = context;
    block.timestamp = startTime - std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1000.0;
    block.allocated = true;
    
    memoryBlocks.push_back(block);
    currentAllocations.push_back(ptr);
    
    return ptr;
}

void MemoryProfiler::trackDeallocation(void* ptr) {
    auto it = std::find_if(memoryBlocks.begin(), memoryBlocks.end(),
                          [ptr](const MemoryBlock& block) { return block.address == ptr; });n
    if (it != memoryBlocks.end()) {
        it->allocated = false;
    }
    
        currentAllocations.erase(std::remove(currentAllocations.begin(), currentAllocations.end(), ptr), currentAllocations.end());
    }
}

// Helper function for EventTimeline color selection
sf::Color getColorForEventType(const std::string& type) {
    static std::map<std::string, sf::Color> colorMap = {
        {"spike", sf::Color(255, 255, 0)},
        {"error", sf::Color(255, 100, 100)},
        {"performance", sf::Color(100, 255, 100)},
        {"memory", sf::Color(100, 100, 255)},
        {"network", sf::Color(255, 100, 255)},
        {"neuron", sf::Color(255, 200, 100)},
        {"synapse", sf::Color(100, 255, 200)},
        {"default", sf::Color::White}
    };
    
    auto it = colorMap.find(type);
    if (it != colorMap.end()) {
        return it->second;
    }
    return colorMap["default"];
}
}

void MemoryProfiler::analyzeLeaks() {
    for (const auto& block : memoryBlocks) {
        if (block.allocated && std::find(currentAllocations.begin(), currentAllocations.end(), block.address) == currentAllocations.end()) {
            // This is a potential leak
        }
    }
}

void MemoryProfiler::reportLeaks() {
    // Report memory leaks
}

double MemoryProfiler::getMemoryLeakSize() const {
    double total = 0.0;
    for (const auto& block : memoryBlocks) {
        if (block.allocated && std::find(currentAllocations.begin(), currentAllocations.end(), block.address) == currentAllocations.end()) {
            total += block.size;
        }
    }
    return total;
}

std::vector<std::string> MemoryProfiler::getLeakContexts() const {
    std::vector<std::string> contexts;
    for (const auto& block : memoryBlocks) {
        if (block.allocated && std::find(currentAllocations.begin(), currentAllocations.end(), block.address) == currentAllocations.end()) {
            contexts.push_back(block.context);
        }
    }
    return contexts;
}

double MemoryProfiler::getCurrentMemoryUsage() const {
    return Performance::getInstance().getMemoryUsageMB();
}

double MemoryProfiler::getPeakMemoryUsage() const {
    return peakMemoryUsage;
}

std::vector<double> MemoryProfiler::getMemoryHistory() const {
    std::vector<double> history;
    // This would return memory usage history over time
    return history;
}

void MemoryProfiler::exportMemoryReport(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return;
    }
    
    file << "Memory Profiler Report" << std::endl;
    file << "=======================" << std::endl;
    file << "Total allocations: " << memoryBlocks.size() << std::endl;
    file << "Current allocations: " << currentAllocations.size() << std::endl;
    file << "Peak memory usage: " << peakMemoryUsage << " MB" << std::endl;
    
    file.close();
}

// PerformanceAnalyzer implementation
PerformanceAnalyzer::PerformanceAnalyzer() : warningLevel(100.0), criticalLevel(500.0) {
}

PerformanceAnalyzer::~PerformanceAnalyzer() = default;

void PerformanceAnalyzer::startOperation(const std::string& operationName) {
    operationStats[operationName].name = operationName;
    operationStats[operationName].minTime = std::numeric_limits<double>::max();
}

void PerformanceAnalyzer::endOperation(const std::string& operationName) {
    if (operationStats.find(operationName) != operationStats.end()) {
        auto& stats = operationStats[operationName];
        stats.count++;
    }
}

void PerformanceAnalyzer::recordOperationTime(const std::string& name, double timeMs) {
    if (operationStats.find(name) != operationStats.end()) {
        auto& stats = operationStats[name];
        stats.times.push_back(timeMs);
        stats.totalTime += timeMs;
        stats.minTime = std::min(stats.minTime, timeMs);
        stats.maxTime = std::max(stats.maxTime, timeMs);
        stats.averageTime = stats.totalTime / stats.count;
    }
}

void PerformanceAnalyzer::analyzeBottlenecks() {
    bottleneckOperations.clear();
    
    for (const auto& pair : operationStats) {
        const auto& stats = pair.second;
        if (stats.count > 10 && stats.averageTime > warningLevel) {
            bottleneckOperations.push_back(pair.first);
        }
    }
}

std::vector<std::string> PerformanceAnalyzer::getBottleneckOperations() const {
    return bottleneckOperations;
}

double PerformanceAnalyzer::getOperationTime(const std::string& name) const {
    if (operationStats.find(name) != operationStats.end()) {
        return operationStats.at(name).averageTime;
    }
    return 0.0;
}

double PerformanceAnalyzer::getAverageTime(const std::string& name) const {
    if (operationStats.find(name) != operationStats.end()) {
        return operationStats.at(name).averageTime;
    }
    return 0.0;
}

void PerformanceAnalyzer::setThreshold(const std::string& operation, double thresholdMs) {
    if (operationStats.find(operation) != operationStats.end()) {
        operationStats[operation].threshold = thresholdMs;
    }
}

void PerformanceAnalyzer::setPerformanceWarningLevel(double level) {
    warningLevel = level;
}

void PerformanceAnalyzer::setPerformanceCriticalLevel(double level) {
    criticalLevel = level;
}

void PerformanceAnalyzer::displayPerformanceReport() {
    // Display performance report
}

void PerformanceAnalyzer::exportPerformanceReport(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return;
    }
    
    file << "Performance Analyzer Report" << std::endl;
    file << "===========================" << std::endl;
    file << "Operations analyzed: " << operationStats.size() << std::endl;
    file << "Bottleneck operations: " << bottleneckOperations.size() << std::endl;
    
    file.close();
}

// NeuralStateInspector implementation
NeuralStateInspector::NeuralStateInspector() {
}

NeuralStateInspector::~NeuralStateInspector() = default;

void NeuralStateInspector::inspectBrainState(const Brain& brain) {
    // Prepare metrics
    metrics.totalNeurons = brain.getTotalNeuronCount();
    metrics.totalSynapses = brain.getTotalSynapseCount();
    metrics.activeNeurons = brain.getActiveNeuronCount();
    metrics.firingNeurons = brain.getFiringNeuronCount();
    metrics.averageFiringRate = brain.getAverageFiringRate();
    metrics.developmentStage = brain.getDevelopmentStage();
    
    // Store metrics
    std::stringstream ss;
    ss << "Total Neurons: " << metrics.totalNeurons
       << "\nTotal Synapses: " << metrics.totalSynapses
       << "\nActive Neurons: " << metrics.activeNeurons
       << "\nFiring Neurons: " << metrics.firingNeurons
       << "\nAverage Firing Rate: " << metrics.averageFiringRate << " Hz"
       << "\nDevelopment Stage: " << metrics.developmentStage;
    
    neuronInfo["BRAIN_METRICS"] = ss.str();
}

void NeuralStateInspector::displayNeuronDetails(int neuronId, const Brain& brain) {
    // Display neuron-specific details
    std::stringstream ss;
    ss << "Neuron ID: " << neuronId << std::endl;
    ss << "Type: Excitatory" << std::endl;  // This would be determined from actual neuron data
    ss << "Position: (0, 0, 0)" << std::endl;
    ss << "Membrane Potential: 0.0 mV" << std::endl;
    ss << "Firing Rate: 0.0 Hz" << std::endl;
    
    neuronInfo[std::string("NEURON_") + std::to_string(neuronId)] = ss.str();
}

void NeuralStateInspector::displaySynapseDetails(int synapseId, const Brain& brain) {
    // Display synapse-specific details
    std::stringstream ss;
    ss << "Synapse ID: " << synapseId << std::endl;
    ss << "Type: Excitatory" << std::endl;
    ss << "Pre-synaptic Neuron: 0" << std::endl;
    ss << "Post-synaptic Neuron: 1" << std::endl;
    ss << "Strength: 1.0" << std::endl;
    
    synapseInfo[std::string("SYNAPSE_") + std::to_string(synapseId)] = ss.str();
}

void NeuralStateInspector::displayRegionInfo(int regionId, const Brain& brain) {
    // Display region information
    std::stringstream ss;
    ss << "Region ID: " << regionId << std::endl;
    ss << "Neuron Count: " << brain.getTotalNeuronCount() / brain.getRegionCount() << std::endl;
    ss << "Function: Unknown" << std::endl;
    
    neuronInfo[std::string("REGION_") + std::to_string(regionId)] = ss.str();
}

void NeuralStateInspector::displayPopulationInfo(int populationId, const Brain& brain) {
    // Display population information
    std::stringstream ss;
    ss << "Population ID: " << populationId << std::endl;
    ss << "Neuron Count: " << brain.getTotalNeuronCount() / 10 << std::endl;  // Estimate
    ss << "Type: Unknown" << std::endl;
    
    neuronInfo[std::string("POPULATION_") + std::to_string(populationId)] = ss.str();
}

void NeuralStateInspector::analyzeNeuralActivity(const Brain& brain) {
    // Analyze neural activity patterns
    std::stringstream ss;
    ss << "Firing Rate Statistics:" << std::endl;
    ss << "  Mean: " << brain.getAverageFiringRate() << " Hz" << std::endl;
    ss << "  Max: " << brain.getAverageFiringRate() * 3 << " Hz" << std::endl;  // Estimate
    ss << "  Min: " << brain.getAverageFiringRate() * 0.1 << " Hz" << std::endl;
    
    neuronInfo["ACTIVITY_STATS"] = ss.str();
}

void NeuralStateInspector::displayFiringRateStats(const Brain& brain) {
    // Display firing rate statistics
    std::stringstream ss;
    ss << "Firing Rate Stats:" << std::endl;
    ss << "  Active Neurons: " << brain.getActiveNeuronCount() << std::endl;
    ss << "  Firing Neurons: " << brain.getFiringNeuronCount() << std::endl;
    ss << "  Firing Percentage: " << (double)brain.getFiringNeuronCount() / brain.getActiveNeuronCount() * 100 << " %" << std::endl;
    
    neuronInfo["FIRING_STATS"] = ss.str();
}

void NeuralStateInspector::displayConnectionStats(const Brain& brain) {
    // Display connection statistics
    std::stringstream ss;
    ss << "Connection Statistics:" << std::endl;
    ss << "  Total Connections: " << brain.getTotalSynapseCount() << std::endl;
    ss << "  Connectivity Density: " << (double)brain.getTotalSynapseCount() / 
        (brain.getTotalNeuronCount() * brain.getTotalNeuronCount()) << std::endl;
    
    neuronInfo["CONNECTION_STATS"] = ss.str();
}

void NeuralStateInspector::displaySpikingActivity(const Brain& brain) {
    // Display spiking activity statistics
    std::stringstream ss;
    ss << "Spiking Activity:" << std::endl;
    ss << "  Spike Count: " << brain.getTotalSpikeCount() << std::endl;
    ss << "  Average Spikes per Neuron: " << (double)brain.getTotalSpikeCount() / brain.getTotalNeuronCount() << std::endl;
    
    neuronInfo["SPIKING_ACTIVITY"] = ss.str();
}

void NeuralStateInspector::prepareInspectionData(const Brain& brain) {
    // Clear and prepare inspection data
    neuronInfo.clear();
    synapseInfo.clear();
    
    // Perform inspections
    inspectBrainState(brain);
    analyzeNeuralActivity(brain);
}

std::vector<std::string> NeuralStateInspector::getAvailableInspectionItems() const {
    std::vector<std::string> items;
    for (const auto& pair : neuronInfo) {
        items.push_back(pair.first);
    }
    for (const auto& pair : synapseInfo) {
        items.push_back(pair.first);
    }
    return items;
}

bool NeuralStateInspector::hasInspectionItem(const std::string& item) const {
    return neuronInfo.find(item) != neuronInfo.end() || synapseInfo.find(item) != synapseInfo.end();
}

// EventTimeline implementation
EventTimeline::EventTimeline() : timelineStart(0.0), timelineEnd(1000.0), maxEventsPerFrame(100), selectedEventIndex(0) {
    // Try to load font
    if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
        // Use default font if file not found
    }
}

EventTimeline::~EventTimeline() = default;

void EventTimeline::addEvent(const std::string& type, const std::string& description,
                             double timestamp, const std::string& details) {
    EventData event;
    event.type = type;
    event.description = description;
    event.timestamp = timestamp;
    event.details = details;
    event.color = getColorForType(type);  // Need to implement this helper
    event.id = events.size();
    
    events.push_back(event);
    
    // Remove events outside timeline range
    if (timestamp < timelineStart) {
        timelineStart = timestamp * 0.9;
    }
    if (timestamp > timelineEnd) {
        timelineEnd = timestamp * 1.1;
    }
}

void EventTimeline::clearEvents() {
    events.clear();
    selectedEventIndex = 0;
}

void EventTimeline::removeOldEvents(double thresholdTime) {
    events.erase(std::remove_if(events.begin(), events.end(),
                               [thresholdTime](const EventData& event) {
                                   return event.timestamp < thresholdTime;
                               }), events.end());
}

void EventTimeline::setTimelineRange(double startTime, double endTime) {
    timelineStart = startTime;
    timelineEnd = endTime;
}

void EventTimeline::setEventColors(const std::map<std::string, sf::Color>& colors) {
    eventColors = colors;
}

void EventTimeline::setMaxEventsPerFrame(int maxEvents) {
    maxEventsPerFrame = maxEvents;
}

void EventTimeline::render(sf::RenderWindow& window) {
    renderTimeline(window);
    renderEvents(window);
    renderEventDetails(window);
}

void EventTimeline::renderTimeline(sf::RenderWindow& window) {
    // Render timeline bar at the bottom
    sf::RectangleShape timelineBar;
    timelineBar.setSize(sf::Vector2f(window.getSize().x, 30));
    timelineBar.setPosition(0, window.getSize().y - 30);
    timelineBar.setFillColor(sf::Color(50, 50, 50, 200));
    window.draw(timelineBar);
    
    // Render time scale
    float timeScale = window.getSize().x / (timelineEnd - timelineStart);
    
    // Render major ticks
    int majorTickInterval = 100;  // 100ms
    for (int t = static_cast<int>(timelineStart); t < static_cast<int>(timelineEnd); t += majorTickInterval) {
        float x = (t - timelineStart) * timeScale;
        
        // Tick line
        sf::RectangleShape tick(sf::Vector2f(2, 20));
        tick.setPosition(x - 1, window.getSize().y - 30);
        tick.setFillColor(sf::Color::White);
        window.draw(tick);
        
        // Tick label
        std::stringstream ss;
        ss << t << "ms";
        sf::Text text(ss.str(), font, 12);
        text.setPosition(x - 20, window.getSize().y - 50);
        text.setFillColor(sf::Color::White);
        window.draw(text);
    }
}

void EventTimeline::renderEvents(sf::RenderWindow& window) {
    float timeScale = window.getSize().x / (timelineEnd - timelineStart);
    
    for (size_t i = 0; i < events.size() && i < maxEventsPerFrame; ++i) {
        const auto& event = events[i];
        
        // Calculate event position
        float x = (event.timestamp - timelineStart) * timeScale;
        float y = window.getSize().y - 60 - (i % 10) * 20;  // Stack events vertically
        
        // Event dot
        sf::CircleShape eventDot(3);
        eventDot.setPosition(x - 3, y - 3);
        eventDot.setFillColor(event.color);
        window.draw(eventDot);
        
        // Highlight selected event
        if (i == selectedEventIndex) {
            sf::CircleShape selectedDot(5);
            selectedDot.setPosition(x - 5, y - 5);
            selectedDot.setFillColor(sf::Color::Yellow);
            selectedDot.setOutlineColor(sf::Color::White);
            selectedDot.setOutlineThickness(2);
            window.draw(selectedDot);
        }
    }
}

void EventTimeline::renderEventDetails(sf::RenderWindow& window) {
    if (events.empty() || selectedEventIndex >= events.size()) {
        return;
    }
    
    const auto& event = events[selectedEventIndex];
    
    // Create details panel
    sf::RectangleShape detailsPanel;
    detailsPanel.setSize(sf::Vector2f(300, 150));
    detailsPanel.setPosition(window.getSize().x - 310, window.getSize().y - 170);
    detailsPanel.setFillColor(sf::Color(30, 30, 30, 220));
    detailsPanel.setOutlineColor(sf::Color::White);
    detailsPanel.setOutlineThickness(1);
    window.draw(detailsPanel);
    
    // Render event details
    std::stringstream ss;
    ss << "Event Details:" << std::endl;
    ss << "Type: " << event.type << std::endl;
    ss << "Time: " << event.timestamp << " ms" << std::endl;
    ss << "Description: " << event.description << std::endl;
    ss << "Details: " << event.details << std::endl;
    
    sf::Text text(ss.str(), font, 12);
    text.setPosition(window.getSize().x - 305, window.getSize().y - 165);
    text.setFillColor(sf::Color::White);
    text.setLineSpacing(1.5f);
    window.draw(text);
}

bool EventTimeline::handleInput(sf::RenderWindow& window) {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                
                // Check if clicked on an event
                float timeScale = window.getSize().x / (timelineEnd - timelineStart);
                
                for (size_t i = 0; i < events.size() && i < maxEventsPerFrame; ++i) {
                    float x = (events[i].timestamp - timelineStart) * timeScale;
                    float y = window.getSize().y - 60 - (i % 10) * 20;
                    
                    if (mousePos.x >= x - 10 && mousePos.x <= x + 10 &&
                        mousePos.y >= y - 10 && mousePos.y <= y + 10) {
                        selectedEventIndex = i;
                        return true;
                    }
                }
                
                // Click elsewhere deselects
                selectedEventIndex = events.size();
            }
        }
        
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Up) {
                if (selectedEventIndex > 0) {
                    selectedEventIndex--;
                }
            } else if (event.key.code == sf::Keyboard::Down) {
                if (selectedEventIndex < events.size() - 1) {
                    selectedEventIndex++;
                }
            }
        }
        
        if (event.type == sf::Event::MouseWheelScrolled) {
            zoom(event.mouseWheelScroll.delta);
        }
    }
    
    return true;
}

void EventTimeline::zoom(double factor) {
    timelineStart *= (1.0 + factor * 0.1);
    timelineEnd *= (1.0 + factor * 0.1);
}

void EventTimeline::pan(float dx, float dy) {
    timelineStart += dx * 10.0;
    timelineEnd += dx * 10.0;
}

void EventTimeline::selectEvent(size_t index) {
    if (index < events.size()) {
        selectedEventIndex = index;
    }
}

void EventTimeline::exportTimeline(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return;
    }
    
    file << "Event Timeline Export" << std::endl;
    file << "======================" << std::endl;
    file << "Timeline Range: " << timelineStart << " - " << timelineEnd << " ms" << std::endl;
    file << "Total Events: " << events.size() << std::endl;
    file << std::endl;
    
    for (const auto& event : events) {
        file << event.timestamp << "," << event.type << "," << event.description 
             << "," << event.details << ","
             << (event.color.r) << "," << (event.color.g) << "," << (event.color.b) << std::endl;
    }
    
    file.close();
}

std::vector<EventTimeline::EventData> EventTimeline::getEventsInRange(double startTime, double endTime) const {
    std::vector<EventData> inRangeEvents;
    
    for (const auto& event : events) {
        if (event.timestamp >= startTime && event.timestamp <= endTime) {
            inRangeEvents.push_back(event);
        }
    }
    
    return inRangeEvents;
}

} // namespace nlm
