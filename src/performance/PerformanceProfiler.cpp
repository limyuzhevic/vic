#pragma once

#include "PerformanceProfiler.hpp"
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <thread>
#include <csignal>
#include <cstring>
#include <cmath>

namespace nlm {

// Platform-specific memory usage detection
#ifdef _WIN32
    #include <windows.h>
    #include <psapi.h>
    size_t getCurrentMemoryUsage() {
        PROCESS_MEMORY_COUNTERS pmc;
        if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
            return pmc.WorkingSetSize;
        }
        return 0;
    }
#else
    #include <unistd.h>
    #include <sys/resource.h>
    size_t getCurrentMemoryUsage() {
        struct rusage usage;
        getrusage(RUSAGE_SELF, &usage);
        return usage.ru_maxrss * 1024; // Convert KB to bytes
    }
#endif

// PerformanceProfiler implementation
struct PerformanceProfiler::Impl {
    ProfilerConfig config;
    std::unordered_map<std::string, std::chrono::system_clock::time_point> activeOperations;
    std::unordered_map<std::string, std::chrono::duration<double>> operationDurations;
    std::vector<PerformanceMetric> metrics;
    std::vector<PerformanceAlert> alerts;
    std::vector<SimulationStepStats> simulationStats;
    std::unordered_map<std::string, size_t> memoryTrackers;
    std::mutex mutex;
    bool phase6IntegrationEnabled = false;
    std::chrono::system_clock::time_point lastSystemCheck;
    double lastMemoryUsage = 0.0;
    std::atomic<bool> running{true};
    std::thread monitoringThread;
    
    Impl() {
        lastSystemCheck = std::chrono::system_clock::now();
        startMonitoringThread();
    }
    
    ~Impl() {
        running = false;
        if (monitoringThread.joinable()) {
            monitoringThread.join();
        }
    }
    
    void startMonitoringThread() {
        monitoringThread = std::thread([this]() {
            while (running) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
                if (config.enableRealTimeMonitoring) {
                    monitorSystemResources();
                }
            }
        });
    }
    
    void monitorSystemResources() {
        auto now = std::chrono::system_clock::now();
        double elapsed = std::chrono::duration<double>(now - lastSystemCheck).count();
        
        if (elapsed >= 1.0) {
            double currentMem = getCurrentMemoryUsage() / (1024.0 * 1024.0); // MB
            
            if (std::abs(currentMem - lastMemoryUsage) > 10.0) { // 10MB threshold
                PerformanceAlert alert;
                alert.operationName = "memory_monitor";
                alert.system = "system";
                alert.severity = "warning";
                alert.duration = currentMem - lastMemoryUsage;
                alert.threshold = 10.0;
                alert.timestamp = now;
                alert.message = "Memory usage changed significantly: " + std::to_string(currentMem - lastMemoryUsage) + " MB";
                addAlert(alert);
            }
            
            lastMemoryUsage = currentMem;
            lastSystemCheck = now;
        }
    }
    
    void addAlert(const PerformanceAlert& alert) {
        std::lock_guard<std::mutex> lock(mutex);
        alerts.push_back(alert);
        if (alerts.size() > config.performanceMetricsHistory) {
            alerts.erase(alerts.begin());
        }
        
        if (config.enablePerformanceAlerts) {
            std::cout << "[PERFORMANCE ALERT] " << alert.severity << ": " 
                      << alert.message << " (at " 
                      << std::chrono::duration<double>(alert.timestamp - std::chrono::system_clock::now()).count()
                      << "s ago)" << std::endl;
        }
    }
    
    void recordMetricInternal(const PerformanceMetric& metric) {
        std::lock_guard<std::mutex> lock(mutex);
        metrics.push_back(metric);
        if (metrics.size() > config.performanceMetricsHistory) {
            metrics.erase(metrics.begin());
        }
    }
    
    void checkAlerts(const PerformanceMetric& metric) {
        for (const auto& alert : config.performanceMetricsHistory) {
            if (metric.duration.count() > alert) {
                PerformanceAlert a;
                a.operationName = metric.operationName;
                a.system = metric.system;
                a.severity = metric.duration.count() > alert * 2.0 ? "critical" : "warning";
                a.duration = metric.duration.count();
                a.threshold = alert;
                a.timestamp = metric.timestamp;
                a.message = "Operation '" + metric.operationName + "' exceeded threshold: " + 
                           std::to_string(metric.duration.count()) + "s > " + std::to_string(alert) + "s";
                addAlert(a);
            }
        }
    }
};

PerformanceProfiler::PerformanceProfiler() : pImpl(std::make_unique<Impl>()) {}

PerformanceProfiler::PerformanceProfiler(const ProfilerConfig& config) : pImpl(std::make_unique<Impl>()) {
    pImpl->config = config;
}

PerformanceProfiler::~PerformanceProfiler() = default;

void PerformanceProfiler::setConfig(const ProfilerConfig& config) {
    pImpl->config = config;
}

const PerformanceProfiler::ProfilerConfig& PerformanceProfiler::getConfig() const {
    return pImpl->config;
}

void PerformanceProfiler::startOperation(const std::string& operation, const std::string& system,
                                        const std::string& component) {
    if (!isProfilingEnabled()) return;
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::string key = operation + "_" + component;
    pImpl->activeOperations[key] = std::chrono::system_clock::now();
}

void PerformanceProfiler::stopOperation(const std::string& operation) {
    if (!isProfilingEnabled()) return;
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (auto it = pImpl->activeOperations.begin(); it != pImpl->activeOperations.end(); ++it) {
        if (it->first.find(operation) != std::string::npos) {
            auto duration = std::chrono::duration<double>(
                std::chrono::system_clock::now() - it->second).count();
            
            // Extract system and component from key
            std::string key = it->first;
            size_t underscore1 = key.find('_');
            size_t underscore2 = key.find('_', underscore1 + 1);
            
            std::string sys = (underscore1 != std::string::npos) ? key.substr(0, underscore1) : "unknown";
            std::string comp = (underscore2 != std::string::npos) ? key.substr(underscore2 + 1) : operation;
            
            PerformanceMetric metric(operation, sys, comp);
            metric.duration = std::chrono::duration<double>(duration);
            metric.memoryUsage = getCurrentMemoryUsage();
            metric.callCount = 1;
            metric.avgDuration = duration;
            metric.minDuration = duration;
            metric.maxDuration = duration;
            metric.threadId = std::hash<std::thread::id>{}(std::this_thread::get_id());
            metric.timestamp = std::chrono::system_clock::now();
            
            pImpl->recordMetricInternal(metric);
            pImpl->checkAlerts(metric);
            
            pImpl->activeOperations.erase(it);
            return;
        }
    }
}

void PerformanceProfiler::startMemoryTracking(const std::string& operation) {
    if (!isProfilingEnabled()) return;
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->memoryTrackers[operation] = getCurrentMemoryUsage();
}

void PerformanceProfiler::stopMemoryTracking(const std::string& operation) {
    if (!isProfilingEnabled()) return;
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->memoryTrackers.find(operation);
    if (it != pImpl->memoryTrackers.end()) {
        size_t startMem = it->second;
        size_t endMem = getCurrentMemoryUsage();
        size_t memDelta = endMem - startMem;
        
        PerformanceMetric metric(operation + "_memory", "system", "memory_tracker");
        metric.memoryUsage = memDelta;
        metric.duration = std::chrono::duration<double>(0.0); // No timing for memory tracking
        metric.callCount = 1;
        metric.avgDuration = 0.0;
        metric.minDuration = 0.0;
        metric.maxDuration = 0.0;
        metric.threadId = std::hash<std::thread::id>{}(std::this_thread::get_id());
        metric.timestamp = std::chrono::system_clock::now();
        metric.metadata["memory_delta_bytes"] = static_cast<double>(memDelta);
        
        pImpl->recordMetricInternal(metric);
        
        pImpl->memoryTrackers.erase(it);
    }
}

size_t PerformanceProfiler::getCurrentMemoryUsage() const {
    return nlm::getCurrentMemoryUsage();
}

void PerformanceProfiler::recordMetric(const PerformanceMetric& metric) {
    pImpl->recordMetricInternal(metric);
    pImpl->checkAlerts(metric);
}

void PerformanceProfiler::recordSpikeProcessing(size_t spikeCount, double duration) {
    if (!isProfilingEnabled()) return;
    
    PerformanceMetric metric("process_spikes", "neural", "spike_system");
    metric.duration = std::chrono::duration<double>(duration);
    metric.callCount = 1;
    metric.avgDuration = duration;
    metric.minDuration = duration;
    metric.maxDuration = duration;
    metric.memoryUsage = static_cast<size_t>(spikeCount * 128); // Estimate 128 bytes per spike
    metric.timestamp = std::chrono::system_clock::now();
    metric.metadata["spike_count"] = static_cast<double>(spikeCount);
    
    recordMetric(metric);
}

void PerformanceProfiler::recordNeuronOperations(size_t operations, double duration) {
    if (!isProfilingEnabled()) return;
    
    PerformanceMetric metric("neuron_operations", "neural", "neuron_system");
    metric.duration = std::chrono::duration<double>(duration);
    metric.callCount = 1;
    metric.avgDuration = duration;
    metric.minDuration = duration;
    metric.maxDuration = duration;
    metric.memoryUsage = 0;
    metric.timestamp = std::chrono::system_clock::now();
    metric.metadata["operation_count"] = static_cast<double>(operations);
    
    recordMetric(metric);
}

void PerformanceProfiler::recordSynapticOperations(size_t operations, double duration) {
    if (!isProfilingEnabled()) return;
    
    PerformanceMetric metric("synaptic_operations", "neural", "synapse_system");
    metric.duration = std::chrono::duration<double>(duration);
    metric.callCount = 1;
    metric.avgDuration = duration;
    metric.minDuration = duration;
    metric.maxDuration = duration;
    metric.memoryUsage = 0;
    metric.timestamp = std::chrono::system_clock::now();
    metric.metadata["operation_count"] = static_cast<double>(operations);
    
    recordMetric(metric);
}

void PerformanceProfiler::recordMemoryOperations(size_t operations, double duration) {
    if (!isProfilingEnabled()) return;
    
    PerformanceMetric metric("memory_operations", "memory", "memory_system");
    metric.duration = std::chrono::duration<double>(duration);
    metric.callCount = 1;
    metric.avgDuration = duration;
    metric.minDuration = duration;
    metric.maxDuration = duration;
    metric.memoryUsage = 0;
    metric.timestamp = std::chrono::system_clock::now();
    metric.metadata["operation_count"] = static_cast<double>(operations);
    
    recordMetric(metric);
}

void PerformanceProfiler::recordSimulationStep(const SimulationStepStats& stats) {
    if (!isProfilingEnabled()) return;
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->simulationStats.push_back(stats);
    if (pImpl->simulationStats.size() > pImpl->config.performanceMetricsHistory) {
        pImpl->simulationStats.erase(pImpl->simulationStats.begin());
    }
    
    // Record as performance metrics
    PerformanceMetric stepMetric("simulation_step", "simulation", "brain");
    stepMetric.duration = stats.stepTime;
    stepMetric.callCount = 1;
    stepMetric.avgDuration = stats.stepTime.count();
    stepMetric.minDuration = stats.stepTime.count();
    stepMetric.maxDuration = stats.stepTime.count();
    stepMetric.memoryUsage = static_cast<size_t>(stats.memoryUsageMB * 1024 * 1024);
    stepMetric.timestamp = std::chrono::system_clock::now();
    stepMetric.metadata["step_number"] = static_cast<double>(stats.stepNumber);
    stepMetric.metadata["spike_count"] = static_cast<double>(stats.spikeCount);
    stepMetric.metadata["neuron_operations"] = static_cast<double>(stats.neuronOperations);
    stepMetric.metadata["synaptic_operations"] = static_cast<double>(stats.synapticOperations);
    stepMetric.metadata["avg_firing_rate"] = stats.avgFiringRate;
    stepMetric.metadata["active_neurons"] = static_cast<double>(stats.activeNeurons);
    
    recordMetric(stepMetric);
}

std::vector<PerformanceProfiler::PerformanceMetric> PerformanceProfiler::getMetrics(
    const std::string& operation, const std::string& system, size_t limit) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<PerformanceMetric> result;
    for (const auto& metric : pImpl->metrics) {
        if ((!operation.empty() && metric.operationName.find(operation) == std::string::npos) ||
            (!system.empty() && metric.system != system)) {
            continue;
        }
        result.push_back(metric);
    }
    
    if (result.size() > limit) {
        result.resize(limit);
    }
    
    return result;
}

std::vector<PerformanceProfiler::PerformanceAlert> PerformanceProfiler::getAlerts() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->alerts;
}

std::vector<PerformanceProfiler::SimulationStepStats> PerformanceProfiler::getSimulationStats(
    size_t limit) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<SimulationStepStats> result(pImpl->simulationStats.begin(), pImpl->simulationStats.end());
    if (result.size() > limit) {
        result.resize(limit);
    }
    
    return result;
}

std::string PerformanceProfiler::analyzeBottlenecks() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::stringstream ss;
    ss << "=== Performance Bottleneck Analysis ===\n";
    
    if (pImpl->metrics.empty()) {
        ss << "No performance metrics collected.\n";
        return ss.str();
    }
    
    // Group metrics by operation
    std::unordered_map<std::string, std::vector<const PerformanceMetric*>> operationMetrics;
    for (const auto& metric : pImpl->metrics) {
        operationMetrics[metric.operationName].push_back(&metric);
    }
    
    // Find slow operations
    ss << "\nTop Slow Operations (by average duration):\n";
    std::vector<std::pair<std::string, double>> opDurations;
    for (const auto& pair : operationMetrics) {
        double totalDuration = 0.0;
        size_t count = 0;
        for (const auto* metric : pair.second) {
            totalDuration += metric->duration.count();
            count++;
        }
        double avgDuration = (count > 0) ? totalDuration / count : 0.0;
        opDurations.push_back({pair.first, avgDuration});
    }
    
    std::sort(opDurations.begin(), opDurations.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    size_t limit = std::min(size_t(10), opDurations.size());
    for (size_t i = 0; i < limit; ++i) {
        ss << "  " << (i + 1) << ". " << opDurations[i].first
           << ": " << formatDuration(opDurations[i].second) << " avg\n";
    }
    
    // Find memory bottlenecks
    ss << "\nMemory Usage Analysis:\n";
    size_t totalMemory = 0;
    for (const auto& metric : pImpl->metrics) {
        totalMemory += metric.memoryUsage;
    }
    ss << "  Total memory tracked: " << formatMemory(totalMemory) << "\n";
    
    // Check for alerts
    if (!pImpl->alerts.empty()) {
        ss << "\nActive Performance Alerts: " << pImpl->alerts.size() << "\n";
        for (const auto& alert : pImpl->alerts) {
            ss << "  - " << alert.severity << ": " << alert.message << "\n";
        }
    }
    
    return ss.str();
}

std::string PerformanceProfiler::generatePerformanceReport() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::stringstream ss;
    ss << "=== NLM Performance Report ===\n";
    ss << "Generated at: " << getCurrentTime() << "\n";
    ss << "Total metrics collected: " << pImpl->metrics.size() << "\n";
    ss << "Total alerts: " << pImpl->alerts.size() << "\n";
    ss << "Total simulation steps: " << pImpl->simulationStats.size() << "\n\n";
    
    // Summary by system
    ss << "Performance by System:\n";
    std::unordered_map<std::string, size_t> systemCounts;
    for (const auto& metric : pImpl->metrics) {
        systemCounts[metric.system]++;
    }
    
    for (const auto& pair : systemCounts) {
        ss << "  " << pair.first << ": " << pair.second << " metrics\n";
    }
    
    ss << "\n" << analyzeBottlenecks() << "\n";
    
    return ss.str();
}

double PerformanceProfiler::calculatePerformanceTrend(const std::string& operation) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto metrics = getMetrics(operation, "", 100);
    if (metrics.size() < 2) return 0.0;
    
    // Calculate trend based on duration changes
    std::vector<double> durations;
    for (const auto& metric : metrics) {
        durations.push_back(metric.duration.count());
    }
    
    // Simple linear regression
    double sumX = 0.0, sumY = 0.0, sumXY = 0.0, sumX2 = 0.0;
    size_t n = durations.size();
    
    for (size_t i = 0; i < n; ++i) {
        double x = static_cast<double>(i);
        double y = durations[i];
        sumX += x;
        sumY += y;
        sumXY += x * y;
        sumX2 += x * x;
    }
    
    double slope = (n * sumXY - sumX * sumY) / (n * sumX2 - sumX * sumX);
    
    return slope; // Positive means getting slower, negative means getting faster
}

void PerformanceProfiler::enableAllProfiling() {
    ProfilerConfig config;
    config.enablePerformanceTracking = true;
    config.enableMemoryTracking = true;
    config.enableRealTimeMonitoring = true;
    config.enablePerformanceAlerts = true;
    config.enableBottleneckDetection = true;
    setConfig(config);
}

void PerformanceProfiler::disableAllProfiling() {
    ProfilerConfig config;
    setConfig(config);
}

bool PerformanceProfiler::isProfilingEnabled() const {
    return pImpl->config.enablePerformanceTracking;
}

std::string PerformanceProfiler::compareWithPreviousRun(const std::string& runId) const {
    // In a real implementation, this would load previous run data
    std::stringstream ss;
    ss << "=== Performance Comparison (Run: " << runId << ") ===\n";
    ss << "This feature requires storing previous run performance data.\n";
    ss << "Use savePerformanceData() to store runs for comparison.\n";
    return ss.str();
}

void PerformanceProfiler::savePerformanceData(const std::string& filepath) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ofstream file(filepath, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open performance data file: " << filepath << std::endl;
        return;
    }
    
    // Simple binary serialization
    size_t metricsCount = pImpl->metrics.size();
    file.write(reinterpret_cast<const char*>(&metricsCount), sizeof(metricsCount));
    
    for (const auto& metric : pImpl->metrics) {
        // Write metric data (simplified)
        size_t opNameLen = metric.operationName.size();
        file.write(reinterpret_cast<const char*>(&opNameLen), sizeof(opNameLen));
        file.write(metric.operationName.c_str(), opNameLen);
        
        size_t sysLen = metric.system.size();
        file.write(reinterpret_cast<const char*>(&sysLen), sizeof(sysLen));
        file.write(metric.system.c_str(), sysLen);
        
        double duration = metric.duration.count();
        file.write(reinterpret_cast<const char*>(&duration), sizeof(duration));
        
        file.write(reinterpret_cast<const char*>(&metric.memoryUsage), sizeof(metric.memoryUsage));
        file.write(reinterpret_cast<const char*>(&metric.callCount), sizeof(metric.callCount));
    }
    
    file.close();
}

void PerformanceProfiler::loadPerformanceData(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open performance data file: " << filepath << std::endl;
        return;
    }
    
    // Clear current data
    {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        pImpl->metrics.clear();
        pImpl->alerts.clear();
        pImpl->simulationStats.clear();
    }
    
    size_t metricsCount;
    file.read(reinterpret_cast<char*>(&metricsCount), sizeof(metricsCount));
    
    for (size_t i = 0; i < metricsCount; ++i) {
        // Read metric data (simplified)
        size_t opNameLen;
        file.read(reinterpret_cast<char*>(&opNameLen), sizeof(opNameLen));
        std::string operationName(opNameLen, '\0');
        file.read(&operationName[0], opNameLen);
        
        size_t sysLen;
        file.read(reinterpret_cast<char*>(&sysLen), sizeof(sysLen));
        std::string system(sysLen, '\0');
        file.read(&system[0], sysLen);
        
        double duration;
        file.read(reinterpret_cast<char*>(&duration), sizeof(duration));
        
        size_t memoryUsage;
        file.read(reinterpret_cast<char*>(&memoryUsage), sizeof(memoryUsage));
        
        uint64_t callCount;
        file.read(reinterpret_cast<char*>(&callCount), sizeof(callCount));
        
        PerformanceMetric metric(operationName, system, "loaded");
        metric.duration = std::chrono::duration<double>(duration);
        metric.memoryUsage = memoryUsage;
        metric.callCount = callCount;
        metric.avgDuration = duration;
        metric.minDuration = duration;
        metric.maxDuration = duration;
        metric.timestamp = std::chrono::system_clock::now();
        
        recordMetric(metric);
    }
    
    file.close();
}

std::string PerformanceProfiler::formatDuration(double seconds) {
    std::stringstream ss;
    if (seconds >= 1.0) {
        ss << std::fixed << std::setprecision(3) << seconds << "s";
    } else if (seconds >= 0.001) {
        ss << std::fixed << std::setprecision(3) << seconds * 1000 << "ms";
    } else {
        ss << std::fixed << std::setprecision(3) << seconds * 1000000 << "µs";
    }
    return ss.str();
}

std::string PerformanceProfiler::formatMemory(size_t bytes) {
    std::stringstream ss;
    if (bytes >= 1024 * 1024) {
        ss << std::fixed << std::setprecision(2) << (bytes / (1024.0 * 1024.0)) << " MB";
    } else if (bytes >= 1024) {
        ss << std::fixed << std::setprecision(2) << (bytes / 1024.0) << " KB";
    } else {
        ss << bytes << " bytes";
    }
    return ss.str();
}

std::string PerformanceProfiler::getSystemInfo() {
    std::stringstream ss;
    ss << "System: " << std::this_thread::get_id() << std::endl;
    ss << "Platform: " << (sizeof(void*) == 8 ? "x64" : "x86") << std::endl;
    ss << "Performance enabled: " << (getPerformanceProfiler().isProfilingEnabled() ? "yes" : "no") << std::endl;
    return ss.str();
}

void PerformanceProfiler::enablePhase6Integration(bool enable) {
    pImpl->phase6IntegrationEnabled = enable;
}

void PerformanceProfiler::logPhase6Performance(const std::string& phase, const std::string& component,
                                               double duration, bool success) {
    if (!pImpl->phase6IntegrationEnabled) return;
    
    PerformanceMetric metric("phase6_" + phase + "_" + component, "phase6", component);
    metric.duration = std::chrono::duration<double>(duration);
    metric.callCount = 1;
    metric.avgDuration = duration;
    metric.minDuration = duration;
    metric.maxDuration = duration;
    metric.memoryUsage = 0;
    metric.timestamp = std::chrono::system_clock::now();
    metric.metadata["success"] = success ? 1.0 : 0.0;
    
    recordMetric(metric);
}

double PerformanceProfiler::getCurrentTime() const {
    auto now = std::chrono::system_clock::now();
    auto epoch = now.time_since_epoch();
    return std::chrono::duration<double>(epoch).count();
}

} // namespace nlm