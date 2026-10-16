#include "PerformanceProfiler.hpp"
#include "../core/Logger/Logger.hpp"
#include <thread>
#include <mutex>
#include <unordered_map>
#include <chrono>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <cmath>
#include <cstring>
#include <algorithm>
#include <functional>

namespace nlm {

struct PerformanceProfiler::Impl {
    // Function profiling
    struct FunctionProfileData {
        std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
        std::chrono::time_point<std::chrono::high_resolution_clock> lastStart;
        uint64_t callCount;
        double totalTime;
        double minTime;
        double maxTime;
        bool isActive;
        std::string signature;
        std::chrono::time_point<std::chrono::high_resolution_clock> lastCallTime;
        
        FunctionProfileData() : callCount(0), totalTime(0.0), minTime(1e9), maxTime(0.0), isActive(false) {}
    };
    
    std::unordered_map<std::string, FunctionProfileData> functionProfiles;
    mutable std::mutex functionProfilesMutex;
    
    // Memory tracking
    std::vector<MemoryAllocation> memoryAllocations;
    mutable std::mutex memoryAllocationsMutex;
    std::unordered_map<void*, size_t> allocatedAddresses;
    mutable std::mutex allocatedAddressesMutex;
    
    // Network I/O tracking
    struct NetworkIOData {
        std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
        std::string endpoint;
        std::string protocol;
        std::string operation;
        uint64_t bytesTransferred;
        bool isActive;
        
        NetworkIOData() : bytesTransferred(0), isActive(false) {}
    };
    
    NetworkIOData currentNetworkIO;
    mutable std::mutex networkIOMutex;
    std::vector<NetworkIO> networkIOStats;
    
    // Simulation metrics
    SimulationMetrics currentSimulationMetrics;
    mutable std::mutex simulationMetricsMutex;
    bool simulationTrackingActive;
    std::chrono::time_point<std::chrono::high_resolution_clock> simulationStartTime;
    
    // Performance alerts
    std::vector<PerformanceAlert> alerts;
    mutable std::mutex alertsMutex;
    std::function<void(const PerformanceAlert&)> alertCallback;
    
    // System monitoring thread
    std::thread monitoringThread;
    bool monitoringActive;
    std::chrono::milliseconds monitoringInterval;
    
    // Statistics
    PerformanceData lastPerformanceData;
    mutable std::mutex performanceDataMutex;
    std::chrono::time_point<std::chrono::high_resolution_clock> lastUpdateTime;
    
    Impl() : simulationTrackingActive(false), monitoringActive(false),
             monitoringInterval(1000), lastUpdateTime(std::chrono::high_resolution_clock::now()) {}
    
    ~Impl() {
        if (monitoringActive) {
            monitoringActive = false;
            if (monitoringThread.joinable()) {
                monitoringThread.join();
            }
        }
    }
    
    void startMonitoring() {
        monitoringActive = true;
        monitoringThread = std::thread([this]() {
            while (monitoringActive) {
                updateSystemPerformance();
                std::this_thread::sleep_for(monitoringInterval);
            }
        });
    }
    
    void stopMonitoring() {
        monitoringActive = false;
        if (monitoringThread.joinable()) {
            monitoringThread.join();
        }
    }
    
    void updateSystemPerformance() {
        std::lock_guard<std::mutex> lock(performanceDataMutex);
        lastPerformanceData = getSystemResourceUsage();
        lastUpdateTime = std::chrono::high_resolution_clock::now();
        
        // Check for performance alerts
        checkPerformanceAlerts(lastPerformanceData);
    }
    
    void checkPerformanceAlerts(const PerformanceData& data) {
        // Memory usage alert
        if (data.memoryUsage > 1024.0) {  // 1GB threshold
            addAlert(AlertLevel::WARNING, 
                    "High memory usage: " + std::to_string(data.memoryUsage) + " MB",
                    "SystemMonitor", 1024.0, data.memoryUsage);
        }
        
        // CPU usage alert
        if (data.cpuUsage > 90.0) {  // 90% threshold
            addAlert(AlertLevel::ERROR,
                    "High CPU usage: " + std::to_string(data.cpuUsage) + "%",
                    "SystemMonitor", 90.0, data.cpuUsage);
        }
        
        // Disk I/O alert
        if (data.diskReadSpeed < 10.0 || data.diskWriteSpeed < 10.0) {  // 10 MB/s threshold
            addAlert(AlertLevel::WARNING,
                    "Low disk I/O performance",
                    "SystemMonitor", 10.0, std::min(data.diskReadSpeed, data.diskWriteSpeed));
        }
    }
    
    static PerformanceData getSystemResourceUsage() {
        PerformanceData data;
        
        // Get thread count
        data.activeThreads = std::thread::hardware_concurrency();
        
        // Note: Actual implementation would use platform-specific APIs
        // to get CPU usage, memory usage, disk I/O, etc.
        // This is a simplified implementation
        
        return data;
    }
    
    FunctionProfile convertToFunctionProfile(const std::string& name, 
                                             const FunctionProfileData& data) const {
        FunctionProfile profile;
        profile.functionName = name;
        profile.functionSignature = data.signature;
        profile.callCount = data.callCount;
        profile.totalTime = data.totalTime;
        profile.averageTime = data.callCount > 0 ? data.totalTime / data.callCount : 0.0;
        profile.minTime = data.minTime;
        profile.maxTime = data.maxTime;
        profile.cpuPercentage = data.callCount > 0 ? (data.totalTime / 1000.0) * 100.0 : 0.0;
        
        return profile;
    }
    
    std::string exportToJSON() const {
        std::ostringstream oss;
        oss << "{\n";
        
        oss << "  \"timestamp\": \"" << std::chrono::system_clock::now().time_since_epoch().count() << "\",\n";
        oss << "  \"cpuUsage\": " << lastPerformanceData.cpuUsage << ",\n";
        oss << "  \"memoryUsage\": " << lastPerformanceData.memoryUsage << ",\n";
        oss << "  \"memoryPeak\": " << lastPerformanceData.memoryPeak << ",\n";
        oss << "  \"networkBytesRead\": " << lastPerformanceData.networkBytesRead << ",\n";
        oss << "  \"networkBytesWritten\": " << lastPerformanceData.networkBytesWritten << ",\n";
        
        // Export function profiles
        oss << "  \"functionProfiles\": [\n";
        bool firstFunc = true;
        for (const auto& pair : functionProfiles) {
            if (!firstFunc) oss << ",\n";
            firstFunc = false;
            
            const auto& funcData = pair.second;
            if (funcData.callCount > 0) {
                oss << "    {\n";
                oss << "      \"name\": \"" << pair.first << "\",\n";
                oss << "      \"signature\": \"" << funcData.signature << "\",\n";
                oss << "      \"callCount\": " << funcData.callCount << ",\n";
                oss << "      \"totalTime\": " << funcData.totalTime << ",\n";
                oss << "      \"averageTime\": " << funcData.averageTime << "\n";
                oss << "    }";
            }
        }
        oss << "\n  ],\n";
        
        // Export memory allocations
        oss << "  \"memoryAllocations\": [\n";
        bool firstMem = true;
        for (const auto& alloc : memoryAllocations) {
            if (!alloc.isFreed && !firstMem) oss << ",\n";
            firstMem = false;
            
            oss << "    {\n";
            oss << "      \"address\": \"" << alloc.address << "\",\n";
            oss << "      \"size\": " << alloc.size << ",\n";
            oss << "      \"context\": \"" << alloc.allocationContext << "\",\n";
            oss << "      \"isFreed\": " << (alloc.isFreed ? "true" : "false") << "\n";
            oss << "    }";
        }
        oss << "\n  ],\n";
        
        // Export network I/O
        oss << "  \"networkIO\": [\n";
        for (size_t i = 0; i < networkIOStats.size(); ++i) {
            if (i > 0) oss << ",\n";
            const auto& io = networkIOStats[i];
            oss << "    {\n";
            oss << "      \"timestamp\": \"" << io.timestamp.time_since_epoch().count() << "\",\n";
            oss << "      \"endpoint\": \"" << io.endpoint << "\",\n";
            oss << "      \"bytesTransferred\": " << io.bytesTransferred << ",\n";
            oss << "      \"transferSpeed\": " << io.transferSpeed << ",\n";
            oss << "      \"latency\": " << io.latency << ",\n";
            oss << "      \"protocol\": \"" << io.protocol << "\",\n";
            oss << "      \"operation\": \"" << io.operation << "\"\n";
            oss << "    }";
        }
        oss << "\n  ],\n";
        
        oss << "  \"simulationMetrics\": {\n";
        oss << "    \"stepsCompleted\": " << currentSimulationMetrics.stepsCompleted << ",\n";
        oss << "    \"totalSpikes\": " << currentSimulationMetrics.totalSpikes << ",\n";
        oss << "    \"averageFiringRate\": " << currentSimulationMetrics.averageFiringRate << ",\n";
        oss << "    \"computationTimePerStep\": " << currentSimulationMetrics.computationTimePerStep << "\n";
        oss << "  },\n";
        
        oss << "  \"alerts\": [\n";
        for (size_t i = 0; i < alerts.size(); ++i) {
            if (i > 0) oss << ",\n";
            const auto& alert = alerts[i];
            oss << "    {\n";
            oss << "      \"level\": \"" << static_cast<int>(alert.level) << "\",\n";
            oss << "      \"message\": \"" << alert.message << "\",\n";
            oss << "      \"source\": \"" << alert.source << "\",\n";
            oss << "      \"threshold\": " << alert.threshold << ",\n";
            oss << "      \"currentValue\": " << alert.currentValue << "\n";
            oss << "    }";
        }
        oss << "\n  ]\n";
        oss << "}\n";
        
        return oss.str();
    }
    
    std::string exportToCSV() const {
        std::ostringstream oss;
        oss << "Timestamp,CPUUsage,MemoryUsage,MemoryPeak,StepsCompleted,TotalSpikes,AverageFiringRate\n";
        
        oss << std::chrono::system_clock::now().time_since_epoch().count() << ","
            << lastPerformanceData.cpuUsage << ","
            << lastPerformanceData.memoryUsage << ","
            << lastPerformanceData.memoryPeak << ","
            << currentSimulationMetrics.stepsCompleted << ","
            << currentSimulationMetrics.totalSpikes << ","
            << currentSimulationMetrics.averageFiringRate << "\n";
        
        return oss.str();
    }
    
    std::string generateReport() const {
        std::ostringstream oss;
        oss << "=== NLM Performance Report ===\n\n";
        oss << "System Performance:\n";
        oss << "  CPU Usage: " << lastPerformanceData.cpuUsage << "%\n";
        oss << "  Memory Usage: " << lastPerformanceData.memoryUsage << " MB\n";
        oss << "  Memory Peak: " << lastPerformanceData.memoryPeak << " MB\n";
        oss << "  Active Threads: " << lastPerformanceData.activeThreads << "\n\n";
        
        oss << "Function Profiles:\n";
        bool firstFunc = true;
        for (const auto& pair : functionProfiles) {
            const auto& funcData = pair.second;
            if (funcData.callCount == 0) continue;
            
            if (!firstFunc) oss << "\n";
            firstFunc = false;
            
            oss << "  " << pair.first << "\n";
            oss << "    Calls: " << funcData.callCount << "\n";
            oss << "    Total Time: " << funcData.totalTime << "s\n";
            oss << "    Average Time: " << funcData.averageTime << "s\n";
            oss << "    Min Time: " << funcData.minTime << "s\n";
            oss << "    Max Time: " << funcData.maxTime << "s\n";
        }
        oss << "\n";
        
        oss << "Memory Allocations:\n";
        oss << "  Total Allocations: " << memoryAllocations.size() << "\n";
        oss << "  Active Allocations: " << std::count_if(memoryAllocations.begin(), memoryAllocations.end(),
                                                     [](const MemoryAllocation& alloc) { return !alloc.isFreed; }) << "\n";
        oss << "  Total Allocated Memory: " << std::accumulate(memoryAllocations.begin(), memoryAllocations.end(), 0,
                                                             [](size_t sum, const MemoryAllocation& alloc) {
                                                                 return sum + (alloc.isFreed ? 0 : alloc.size);
                                                             }) << " bytes\n\n";
        
        oss << "Simulation Metrics:\n";
        oss << "  Steps Completed: " << currentSimulationMetrics.stepsCompleted << "\n";
        oss << "  Total Spikes: " << currentSimulationMetrics.totalSpikes << "\n";
        oss << "  Average Firing Rate: " << currentSimulationMetrics.averageFiringRate << " spikes/step\n";
        oss << "  Computation Time per Step: " << currentSimulationMetrics.computationTimePerStep << "s\n\n";
        
        oss << "Performance Alerts:\n";
        for (const auto& alert : alerts) {
            std::string levelStr;
            switch (alert.level) {
                case AlertLevel::INFO: levelStr = "INFO"; break;
                case AlertLevel::WARNING: levelStr = "WARNING"; break;
                case AlertLevel::ERROR: levelStr = "ERROR"; break;
                case AlertLevel::CRITICAL: levelStr = "CRITICAL"; break;
            }
            oss << "  [" << levelStr << "] " << alert.message << " (" << alert.source << ")\n";
        }
        
        if (alerts.empty()) {
            oss << "  No alerts\n";
        }
        
        return oss.str();
    }
    
    void addAlert(AlertLevel level, const std::string& message, const std::string& source,
                  double threshold, double currentValue) {
        std::lock_guard<std::mutex> lock(alertsMutex);
        PerformanceAlert alert;
        alert.level = level;
        alert.message = message;
        alert.timestamp = std::chrono::high_resolution_clock::now();
        alert.source = source;
        alert.threshold = threshold;
        alert.currentValue = currentValue;
        
        // Set action based on alert level
        if (level == AlertLevel::ERROR || level == AlertLevel::CRITICAL) {
            alert.action = [this, alert]() {
                NLM_LOG_ERROR("PERFORMANCE ALERT: " + alert.message + " (Source: " + alert.source + ")");
            };
        } else if (level == AlertLevel::WARNING) {
            alert.action = [this, alert]() {
                NLM_LOG_WARNING("PERFORMANCE WARNING: " + alert.message + " (Source: " + alert.source + ")");
            };
        } else {
            alert.action = [this, alert]() {
                NLM_LOG_INFO("PERFORMANCE INFO: " + alert.message + " (Source: " + alert.source + ")");
            };
        }
        
        alerts.push_back(alert);
        
        // Trigger callback if set
        if (alertCallback) {
            alertCallback(alert);
        }
        
        // Execute action
        alert.action();
    }
    
    void removeOldAlerts() {
        std::lock_guard<std::mutex> lock(alertsMutex);
        auto now = std::chrono::high_resolution_clock::now();
        auto threshold = now - std::chrono::hours(24);  // Remove alerts older than 24 hours
        
        alerts.erase(std::remove_if(alerts.begin(), alerts.end(),
                                   [threshold](const PerformanceAlert& alert) {
                                       return alert.timestamp < threshold;
                                   }),
                     alerts.end());
    }
};

PerformanceProfiler::PerformanceProfiler() : pImpl(std::make_unique<Impl>()) {
    initialize();
}

PerformanceProfiler::~PerformanceProfiler() {
    shutdown();
}

bool PerformanceProfiler::initialize() {
    try {
        // Start system monitoring
        pImpl->startMonitoring();
        
        NLM_LOG_INFO("Performance profiler initialized successfully");
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to initialize performance profiler: ") + e.what());
        return false;
    }
}

void PerformanceProfiler::shutdown() {
    try {
        pImpl->stopMonitoring();
        NLM_LOG_INFO("Performance profiler shutdown complete");
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error during performance profiler shutdown: ") + e.what());
    }
}

void PerformanceProfiler::startFunctionProfile(const std::string& functionName, 
                                              const std::string& signature) {
    std::lock_guard<std::mutex> lock(pImpl->functionProfilesMutex);
    auto& funcData = pImpl->functionProfiles[functionName];
    funcData.startTime = std::chrono::high_resolution_clock::now();
    funcData.lastStart = funcData.startTime;
    funcData.isActive = true;
    funcData.signature = signature;
    funcData.lastCallTime = funcData.startTime;
}

void PerformanceProfiler::stopFunctionProfile(const std::string& functionName) {
    std::lock_guard<std::mutex> lock(pImpl->functionProfilesMutex);
    auto it = pImpl->functionProfiles.find(functionName);
    if (it != pImpl->functionProfiles.end() && it->second.isActive) {
        auto& funcData = it->second;
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(endTime - funcData.lastStart).count();
        
        funcData.callCount++;
        funcData.totalTime += duration;
        funcData.minTime = std::min(funcData.minTime, duration);
        funcData.maxTime = std::max(funcData.maxTime, duration);
        funcData.isActive = false;
    }
}

void PerformanceProfiler::recordMemoryAllocation(void* address, size_t size, 
                                                 const std::string& context) {
    std::lock_guard<std::mutex> lock(pImpl->memoryAllocationsMutex);
    MemoryAllocation alloc;
    alloc.address = address;
    alloc.size = size;
    alloc.timestamp = std::chrono::high_resolution_clock::now();
    alloc.allocationContext = context;
    alloc.isFreed = false;
    
    // Generate stack trace (simplified)
    std::ostringstream stackStream;
    stackStream << "Stack trace for " << context << ": [TODO: actual stack trace]";
    alloc.stackTrace = stackStream.str();
    
    pImpl->memoryAllocations.push_back(alloc);
    pImpl->allocatedAddresses[address] = size;
}

void PerformanceProfiler::recordMemoryDeallocation(void* address) {
    std::lock_guard<std::mutex> lock(pImpl->memoryAllocationsMutex);
    auto it = pImpl->allocatedAddresses.find(address);
    if (it != pImpl->allocatedAddresses.end()) {
        for (auto& alloc : pImpl->memoryAllocations) {
            if (alloc.address == address && !alloc.isFreed) {
                alloc.isFreed = true;
                break;
            }
        }
        pImpl->allocatedAddresses.erase(it);
    }
}

void PerformanceProfiler::startNetworkIO(const std::string& endpoint, 
                                         const std::string& protocol,
                                         const std::string& operation) {
    std::lock_guard<std::mutex> lock(pImpl->networkIOMutex);
    pImpl->currentNetworkIO.endpoint = endpoint;
    pImpl->currentNetworkIO.protocol = protocol;
    pImpl->currentNetworkIO.operation = operation;
    pImpl->currentNetworkIO.startTime = std::chrono::high_resolution_clock::now();
    pImpl->currentNetworkIO.bytesTransferred = 0;
    pImpl->currentNetworkIO.isActive = true;
}

void PerformanceProfiler::stopNetworkIO() {
    std::lock_guard<std::mutex> lock(pImpl->networkIOMutex);
    if (pImpl->currentNetworkIO.isActive) {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(endTime - pImpl->currentNetworkIO.startTime).count();
        
        NetworkIO io;
        io.timestamp = endTime;
        io.endpoint = pImpl->currentNetworkIO.endpoint;
        io.protocol = pImpl->currentNetworkIO.protocol;
        io.operation = pImpl->currentNetworkIO.operation;
        io.bytesTransferred = pImpl->currentNetworkIO.bytesTransferred;
        io.transferSpeed = duration > 0 ? (io.bytesTransferred / (1024.0 * 1024.0)) / duration : 0.0;
        io.latency = 0.0; // Would be measured per transaction
        
        pImpl->networkIOStats.push_back(io);
        pImpl->currentNetworkIO.isActive = false;
    }
}

void PerformanceProfiler::startSimulationMetrics() {
    std::lock_guard<std::mutex> lock(pImpl->simulationMetricsMutex);
    pImpl->simulationTrackingActive = true;
    pImpl->simulationStartTime = std::chrono::high_resolution_clock::now();
    pImpl->currentSimulationMetrics.startTime = pImpl->simulationStartTime;
}

void PerformanceProfiler::updateSimulationMetrics(uint64_t stepsCompleted, uint64_t neuronsActive,
                                                  uint64_t totalSpikes, uint64_t synapseActivations) {
    std::lock_guard<std::mutex> lock(pImpl->simulationMetricsMutex);
    if (!pImpl->simulationTrackingActive) return;
    
    pImpl->currentSimulationMetrics.stepsCompleted = stepsCompleted;
    pImpl->currentSimulationMetrics.neuronsActive = neuronsActive;
    pImpl->currentSimulationMetrics.totalSpikes = totalSpikes;
    pImpl->currentSimulationMetrics.synapseActivations = synapseActivations;
    
    // Calculate average firing rate
    if (stepsCompleted > 0) {
        pImpl->currentSimulationMetrics.averageFiringRate = 
            static_cast<double>(totalSpikes) / static_cast<double>(stepsCompleted);
    }
    
    // Calculate computation time per step (simplified)
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration<double>(currentTime - pImpl->simulationStartTime).count();
    if (stepsCompleted > 0 && elapsed > 0) {
        pImpl->currentSimulationMetrics.computationTimePerStep = elapsed / static_cast<double>(stepsCompleted);
    }
    
    // Store spike count and step time for analysis
    pImpl->currentSimulationMetrics.spikeCounts.push_back(static_cast<double>(totalSpikes));
    pImpl->currentSimulationMetrics.stepTimes.push_back(pImpl->currentSimulationMetrics.computationTimePerStep);
}

SimulationMetrics PerformanceProfiler::stopSimulationMetrics() {
    std::lock_guard<std::mutex> lock(pImpl->simulationMetricsMutex);
    pImpl->simulationTrackingActive = false;
    
    auto endTime = std::chrono::high_resolution_clock::now();
    pImpl->currentSimulationMetrics.endTime = endTime;
    
    // Calculate final statistics
    if (!pImpl->currentSimulationMetrics.stepTimes.empty()) {
        double sumSteps = std::accumulate(pImpl->currentSimulationMetrics.stepTimes.begin(),
                                         pImpl->currentSimulationMetrics.stepTimes.end(), 0.0);
        pImpl->currentSimulationMetrics.computationTimePerStep = sumSteps / pImpl->currentSimulationMetrics.stepTimes.size();
    }
    
    if (!pImpl->currentSimulationMetrics.spikeCounts.empty()) {
        double sumSpikes = std::accumulate(pImpl->currentSimulationMetrics.spikeCounts.begin(),
                                          pImpl->currentSimulationMetrics.spikeCounts.end(), 0.0);
        pImpl->currentSimulationMetrics.averageFiringRate = sumSpikes / pImpl->currentSimulationMetrics.spikeCounts.size();
    }
    
    return pImpl->currentSimulationMetrics;
}

void PerformanceProfiler::addAlert(AlertLevel level, const std::string& message,
                                   const std::string& source, double threshold,
                                   double currentValue) {
    pImpl->addAlert(level, message, source, threshold, currentValue);
}

void PerformanceProfiler::setAlertCallback(std::function<void(const PerformanceAlert&)> callback) {
    pImpl->alertCallback = callback;
}

PerformanceData PerformanceProfiler::getCurrentPerformance() const {
    return pImpl->lastPerformanceData;
}

std::unordered_map<std::string, FunctionProfile> PerformanceProfiler::getFunctionProfiles() const {
    std::unordered_map<std::string, FunctionProfile> profiles;
    
    std::lock_guard<std::mutex> lock(pImpl->functionProfilesMutex);
    for (const auto& pair : pImpl->functionProfiles) {
        profiles[pair.first] = pImpl->convertToFunctionProfile(pair.first, pair.second);
    }
    
    return profiles;
}

std::vector<MemoryAllocation> PerformanceProfiler::getMemoryAllocations() const {
    std::lock_guard<std::mutex> lock(pImpl->memoryAllocationsMutex);
    return pImpl->memoryAllocations;
}

std::vector<NetworkIO> PerformanceProfiler::getNetworkIOStats() const {
    return pImpl->networkIOStats;
}

SimulationMetrics PerformanceProfiler::getCurrentSimulationMetrics() const {
    return pImpl->currentSimulationMetrics;
}

std::vector<PerformanceAlert> PerformanceProfiler::getAlerts() const {
    return pImpl->alerts;
}

std::string PerformanceProfiler::exportToJSON() const {
    return pImpl->exportToJSON();
}

std::string PerformanceProfiler::exportToCSV() const {
    return pImpl->exportToCSV();
}

std::string PerformanceProfiler::generateReport() const {
    return pImpl->generateReport();
}

void PerformanceProfiler::reset() {
    {
        std::lock_guard<std::mutex> lock(pImpl->functionProfilesMutex);
        pImpl->functionProfiles.clear();
    }
    {
        std::lock_guard<std::mutex> lock(pImpl->memoryAllocationsMutex);
        pImpl->memoryAllocations.clear();
        pImpl->allocatedAddresses.clear();
    }
    {
        std::lock_guard<std::mutex> lock(pImpl->networkIOMutex);
        pImpl->currentNetworkIO = NetworkIOData();
        pImpl->networkIOStats.clear();
    }
    {
        std::lock_guard<std::mutex> lock(pImpl->simulationMetricsMutex);
        pImpl->currentSimulationMetrics = SimulationMetrics();
        pImpl->simulationTrackingActive = false;
    }
    {
        std::lock_guard<std::mutex> lock(pImpl->alertsMutex);
        pImpl->alerts.clear();
    }
}

std::vector<MemoryAllocation> PerformanceProfiler::detectMemoryLeaks() const {
    std::vector<MemoryAllocation> leaks;
    
    std::lock_guard<std::mutex> lock(pImpl->memoryAllocationsMutex);
    for (const auto& alloc : pImpl->memoryAllocations) {
        if (!alloc.isFreed) {
            leaks.push_back(alloc);
        }
    }
    
    return leaks;
}

PerformanceProfiler& PerformanceProfiler::getInstance() {
    static PerformanceProfiler instance;
    return instance;
}

} // namespace nlm