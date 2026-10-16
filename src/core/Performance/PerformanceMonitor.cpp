#include "PerformanceMonitor.hpp"
#include <chrono>
#include <thread>
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <mutex>
#include <atomic>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace nlm {

// System monitoring interface
class SystemMonitor {
public:
    virtual ~SystemMonitor() = default;
    
    virtual double getCpuUsage() = 0;
    virtual uint64_t getTotalMemory() = 0;
    virtual uint64_t getUsedMemory() = 0;
    virtual double getDiskUsage() = 0;
    virtual double getNetworkUsage() = 0;
    virtual double getCpuTemperature() = 0;
    virtual std::chrono::system_clock::time_point getCurrentTime() = 0;
};

// Default system monitor implementation
class DefaultSystemMonitor : public SystemMonitor {
public:
    DefaultSystemMonitor() : currentCpuUsage_(0.0), totalMemory_(0), usedMemory_(0),
        diskUsage_(0.0), networkUsage_(0.0), cpuTemperature_(0.0) {
        // Initialize with default values
        totalMemory_ = 8 * 1024 * 1024 * 1024;  // 8GB
        usedMemory_ = 2 * 1024 * 1024 * 1024;   // 2GB
        currentCpuUsage_ = 25.0;
        diskUsage_ = 65.0;
        networkUsage_ = 5.0;
        cpuTemperature_ = 45.0;
    }
    
    double getCpuUsage() override {
        // Simulate CPU usage variation
        currentCpuUsage_ = 20.0 + (rand() % 60); // 20-80%
        return std::min(100.0, currentCpuUsage_);
    }
    
    uint64_t getTotalMemory() override {
        return totalMemory_;
    }
    
    uint64_t getUsedMemory() override {
        return usedMemory_;
    }
    
    double getDiskUsage() override {
        // Simulate disk usage variation
        diskUsage_ = 60.0 + (rand() % 35); // 60-95%
        return std::min(100.0, diskUsage_);
    }
    
    double getNetworkUsage() override {
        // Simulate network usage
        networkUsage_ = (rand() % 1000) / 10.0; // 0-100 Mbps
        return networkUsage_;
    }
    
    double getCpuTemperature() override {
        // Simulate CPU temperature
        cpuTemperature_ = 45.0 + (rand() % 30); // 45-75 Celsius
        return cpuTemperature_;
    }
    
    std::chrono::system_clock::time_point getCurrentTime() override {
        return std::chrono::system_clock::now();
    }
    
private:
    double currentCpuUsage_;
    uint64_t totalMemory_;
    uint64_t usedMemory_;
    double diskUsage_;
    double networkUsage_;
    double cpuTemperature_;
};

// Global system monitor instance
static std::unique_ptr<SystemMonitor> gSystemMonitor;

void initializeSystemMonitor(std::unique_ptr<SystemMonitor> monitor) {
    gSystemMonitor = std::move(monitor);
}

SystemMonitor& getSystemMonitor() {
    if (!gSystemMonitor) {
        gSystemMonitor = std::make_unique<DefaultSystemMonitor>();
    }
    return *gSystemMonitor;
}

PerformanceMonitor::PerformanceMonitor()
    : isRunning_(false), monitoringSystem_(false), cpuThresholdWarning_(80.0),
      cpuThresholdCritical_(90.0), memoryThresholdWarning_(85.0),
      memoryThresholdCritical_(95.0), memoryAllocationLimit_(1024 * 1024 * 1024), // 1GB
      maxComponentProfiles_(1000), maxCheckpoints_(100), samplingIntervalMs_(1000) {
    
    systemMonitor_ = std::make_unique<SystemMonitor>();
    lastMetricsUpdate_ = std::chrono::system_clock::now();
}

PerformanceMonitor::~PerformanceMonitor() {
    stop();
}

void PerformanceMonitor::start() {
    if (!isRunning_) {
        isRunning_ = true;
        monitoringSystem_ = true;
        
        // Start system monitoring thread
        systemMonitoringThread_ = std::thread(&PerformanceMonitor::monitorSystem, this);
        
        NLM_LOG_INFO("Performance monitor started");
    }
}

void PerformanceMonitor::stop() {
    if (isRunning_) {
        isRunning_ = false;
        monitoringSystem_ = false;
        
        if (systemMonitoringThread_.joinable()) {
            systemMonitoringThread_.join();
        }
        
        // Create final checkpoint
        createCheckpoint("Final", "Performance monitor shutdown");
        
        NLM_LOG_INFO("Performance monitor stopped");
    }
}

bool PerformanceMonitor::isRunning() const {
    return isRunning_.load();
}

void PerformanceMonitor::recordComponentStart(const std::string& componentName) {
    std::lock_guard<std::mutex> lock(timingMutex_);
    
    componentStartTimes_[componentName] = std::chrono::system_clock::now();
    
    // Initialize component timing if needed
    if (componentTimings_.find(componentName) == componentTimings_.end()) {
        componentTimings_[componentName] = ComponentTiming(componentName);
    }
}

void PerformanceMonitor::recordComponentEnd(const std::string& componentName) {
    std::lock_guard<std::mutex> lock(timingMutex_);
    
    auto it = componentStartTimes_.find(componentName);
    if (it != componentStartTimes_.end()) {
        auto endTime = std::chrono::system_clock::now();
        auto startTime = it->second;
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime);
        
        // Update component timing
        auto& timing = componentTimings_[componentName];
        timing.totalDuration += duration;
        timing.minDuration = std::min(timing.minDuration, duration);
        timing.maxDuration = std::max(timing.maxDuration, duration);
        timing.lastDuration = duration;
        timing.callCount++;
        
        // Calculate average
        if (timing.callCount > 0) {
            timing.averageDuration = (timing.totalDuration.count() / timing.callCount) / 1000000.0; // Convert to ms
        }
        
        // Remove from start times
        componentStartTimes_.erase(it);
    }
}

void PerformanceMonitor::recordMemoryAllocation(const std::string& component, size_t size) {
    std::lock_guard<std::mutex> lock(memoryMutex_);
    
    if (memoryAllocations_.find(component) == memoryAllocations_.end()) {
        memoryAllocations_[component] = MemoryAllocation();
    }
    
    auto& allocation = memoryAllocations_[component];
    allocation.allocationSize += size;
    allocation.maxAllocated = std::max(allocation.maxAllocated, allocation.maxAllocated + size);
    allocation.allocationCount++;
    
    // Check if allocation exceeds limit
    if (allocation.maxAllocated > memoryAllocationLimit_) {
        NLM_LOG_WARNING("Memory allocation exceeded limit for component " + component + ": " + 
                       std::to_string(allocation.maxAllocated) + " > " + 
                       std::to_string(memoryAllocationLimit_));
    }
}

void PerformanceMonitor::recordMemoryDeallocation(const std::string& component, size_t size) {
    std::lock_guard<std::mutex> lock(memoryMutex_);
    
    if (memoryAllocations_.find(component) == memoryAllocations_.end()) {
        memoryAllocations_[component] = MemoryAllocation();
    }
    
    auto& allocation = memoryAllocations_[component];
    allocation.deallocationSize += size;
    allocation.deallocationCount++;
}

PerformanceMetrics PerformanceMonitor::getSystemMetrics() {
    std::lock_guard<std::mutex> lock(metricsMutex_);
    updateSystemMetrics();
    return currentMetrics_;
}

void PerformanceMonitor::addAlertThreshold(const std::string& component, const std::string& metric,
                                          double threshold, PerformanceLevel severity) {
    std::lock_guard<std::mutex> lock(alertMutex_);
    alertThresholds_.push_back({component, metric, threshold, severity});
    
    NLM_LOG_INFO("Added performance alert threshold: " + component + "." + metric + " > " + 
                std::to_string(threshold) + " (severity: " + std::to_string(static_cast<int>(severity)) + ")");
}

std::vector<PerformanceAlert> PerformanceMonitor::getAlerts() const {
    std::lock_guard<std::mutex> lock(alertMutex_);
    return alerts_;
}

void PerformanceMonitor::createCheckpoint(const std::string& name, const std::string& description) {
    PerformanceCheckpoint checkpoint(name, description);
    checkpoint.metrics = getSystemMetrics();
    checkpoint.timings = getComponentTimings();
    checkpoint.memoryAllocations = getMemoryAllocations();
    
    std::lock_guard<std::mutex> lock(checkpointMutex_);
    checkpoints_.push_back(checkpoint);
    
    NLM_LOG_INFO("Created performance checkpoint: " + name);
}

std::vector<PerformanceCheckpoint> PerformanceMonitor::getCheckpoints() const {
    std::lock_guard<std::mutex> lock(checkpointMutex_);
    return checkpoints_;
}

PerformanceCheckpoint PerformanceMonitor::getLatestCheckpoint() const {
    std::lock_guard<std::mutex> lock(checkpointMutex_);
    if (checkpoints_.empty()) {
        return PerformanceCheckpoint("Empty", "No checkpoints recorded");
    }
    return checkpoints_.back();
}

std::vector<std::string> PerformanceMonitor::getOptimizationSuggestions() const {
    std::vector<std::string> suggestions;
    
    // Check for slow components
    std::lock_guard<std::mutex> lock(timingMutex_);
    for (const auto& [name, timing] : componentTimings_) {
        if (timing.callCount > 0 && timing.averageDuration > 100.0) { // > 100ms average
            suggestions.push_back("Component " + name + " has high average execution time: " + 
                                std::to_string(timing.averageDuration) + "ms");
        }
    }
    
    // Check for high memory usage
    std::lock_guard<std::mutex> memoryLock(memoryMutex_);
    for (const auto& [component, allocation] : memoryAllocations_) {
        if (allocation.netAllocation > memoryAllocationLimit_ * 0.5) { // > 50% of limit
            suggestions.push_back("Component " + component + " has high memory usage: " + 
                                std::to_string(allocation.netAllocation) + " bytes");
        }
    }
    
    return suggestions;
}

std::string PerformanceMonitor::generateReport() const {
    return generateTextReport();
}

std::string PerformanceMonitor::generateJsonReport() const {
    return generateTextReport(); // Simplified for this implementation
}

std::string PerformanceMonitor::generateHtmlReport() const {
    return generateTextReport(); // Simplified for this implementation
}

std::string PerformanceMonitor::generateTextReport() const {
    std::ostringstream oss;
    
    oss << "NLM Performance Report" << std::endl;
    oss << std::string(50, '=') << std::endl;
    oss << "Generated on: " << getCurrentTimestamp() << std::endl;
    oss << "Profiler Status: " << (isRunning_ ? "Running" : "Stopped") << std::endl;
    oss << std::endl;
    
    oss << "System Metrics:" << std::endl;
    oss << "  CPU Usage: " << currentMetrics_.cpuUsage << "%" << std::endl;
    oss << "  Memory Usage: " << currentMetrics_.memoryUsage << "%" << std::endl;
    oss << "  Process Memory: " << currentMetrics_.processMemory << " bytes" << std::endl;
    oss << "  Disk Usage: " << currentMetrics_.diskUsage << "%" << std::endl;
    oss << "  Network Usage: " << currentMetrics_.networkUsage << " Mbps" << std::endl;
    oss << "  Temperature: " << currentMetrics_.temperature << " C" << std::endl;
    oss << std::endl;
    
    oss << "Component Performance:" << std::endl;
    std::lock_guard<std::mutex> lock(timingMutex_);
    for (const auto& [name, timing] : componentTimings_) {
        oss << "  " << name << ": " << timing.callCount << " calls, " 
            << timing.averageDuration << " ms avg, " << timing.p95Duration << " ms p95" << std::endl;
    }
    
    oss << std::endl << "Memory Allocations:" << std::endl;
    std::lock_guard<std::mutex> memoryLock(memoryMutex_);
    for (const auto& [component, allocation] : memoryAllocations_) {
        oss << "  " << component << ": " << allocation.allocationCount << " allocations, " 
            << allocation.netAllocation << " bytes net" << std::endl;
    }
    
    oss << std::endl << "Performance Alerts:" << std::endl;
    std::lock_guard<std::mutex> alertLock(alertMutex_);
    for (const auto& alert : alerts_) {
        std::string severityStr;
        switch (alert.severity) {
            case PerformanceLevel::Critical: severityStr = "CRITICAL"; break;
            case PerformanceLevel::Poor: severityStr = "WARNING"; break;
            case PerformanceLevel::Fair: severityStr = "INFO"; break;
            case PerformanceLevel::Good: severityStr = "INFO"; break;
            case PerformanceLevel::Excellent: severityStr = "INFO"; break;
        }
        oss << "  [" << severityStr << "] " << alert.message << std::endl;
    }
    
    oss << std::endl << "Performance Checkpoints:" << std::endl;
    std::lock_guard<std::mutex> checkpointLock(checkpointMutex_);
    for (const auto& checkpoint : checkpoints_) {
        oss << "  - " << checkpoint.checkpointName << ": " << checkpoint.description 
            << " (" << getProfileTimestamp(checkpoint.timestamp) << ")" << std::endl;
    }
    
    return oss.str();
}

void PerformanceMonitor::updateSystemMetrics() {
    // Update system metrics using system monitor
    currentMetrics_.cpuUsage = gSystemMonitor().getCpuUsage();
    currentMetrics_.totalMemory = gSystemMonitor().getTotalMemory();
    currentMetrics_.usedMemory = gSystemMonitor().getUsedMemory();
    currentMetrics_.processMemory = currentMetrics_.usedMemory * 0.1; // Estimate: 10% of used memory
    currentMetrics_.diskUsage = gSystemMonitor().getDiskUsage();
    currentMetrics_.networkUsage = gSystemMonitor().getNetworkUsage();
    currentMetrics_.temperature = gSystemMonitor().getCpuTemperature();
    
    currentMetrics_.timestamp = gSystemMonitor().getCurrentTime();
    
    // Calculate memory usage percentage
    if (currentMetrics_.totalMemory > 0) {
        currentMetrics_.memoryUsage = (currentMetrics_.usedMemory * 100.0) / currentMetrics_.totalMemory;
    } else {
        currentMetrics_.memoryUsage = 0.0;
    }
    
    currentMetrics_.availableMemory = currentMetrics_.totalMemory - currentMetrics_.usedMemory;
    
    // Check performance alerts
    checkPerformanceAlerts();
    
    // Update metrics history
    metricsHistory_.push_back(currentMetrics_);
    if (metricsHistory_.size() > 1000) {
        metricsHistory_.erase(metricsHistory_.begin());
    }
}

void PerformanceMonitor::checkPerformanceAlerts() {
    // Check CPU alert
    if (currentMetrics_.cpuUsage > cpuThresholdCritical_) {
        addAlert("System", "CPU", currentMetrics_.cpuUsage, cpuThresholdCritical_,
                PerformanceLevel::Critical, "CPU usage critical");
    } else if (currentMetrics_.cpuUsage > cpuThresholdWarning_) {
        addAlert("System", "CPU", currentMetrics_.cpuUsage, cpuThresholdWarning_,
                PerformanceLevel::Warning, "CPU usage high");
    }
    
    // Check memory alert
    if (currentMetrics_.memoryUsage > memoryThresholdCritical_) {
        addAlert("System", "Memory", currentMetrics_.memoryUsage, memoryThresholdCritical_,
                PerformanceLevel::Critical, "Memory usage critical");
    } else if (currentMetrics_.memoryUsage > memoryThresholdWarning_) {
        addAlert("System", "Memory", currentMetrics_.memoryUsage, memoryThresholdWarning_,
                PerformanceLevel::Warning, "Memory usage high");
    }
    
    // Check component time alerts
    std::lock_guard<std::mutex> lock(timingMutex_);
    for (const auto& [name, timing] : componentTimings_) {
        if (timing.callCount > 0 && timing.averageDuration > 1000.0) { // > 1 second average
            addAlert(name, "AverageTime", timing.averageDuration, 1000.0,
                    PerformanceLevel::Warning, "Component average time too high");
        }
    }
}

void PerformanceMonitor::addAlert(const std::string& component, const std::string& metric,
                                 double value, double threshold, PerformanceLevel severity,
                                 const std::string& message) {
    PerformanceAlert alert(component, metric, value, threshold, severity);
    alert.message = message;
    
    std::lock_guard<std::mutex> lock(alertMutex_);
    alerts_.push_back(alert);
    
    // Clean old alerts
    cleanAlerts();
}

void PerformanceMonitor::cleanAlerts() {
    std::lock_guard<std::mutex> lock(alertMutex_);
    auto it = alerts_.begin();
    while (it != alerts_.end()) {
        auto now = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::minutes>(now - it->timestamp);
        if (duration.count() > 60) { // Remove alerts older than 1 hour
            it = alerts_.erase(it);
        } else {
            ++it;
        }
    }
}

std::string PerformanceMonitor::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    
    return ss.str();
}

std::string PerformanceMonitor::getProfileTimestamp(std::chrono::system_clock::time_point timestamp) const {
    auto in_time_t = std::chrono::system_clock::to_time_t(timestamp);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    
    return ss.str();
}

void PerformanceMonitor::setConfig(const PerformanceConfig& config) {
    config_ = config;
    cpuThresholdWarning_ = config.cpuThresholdWarning;
    cpuThresholdCritical_ = config.cpuThresholdCritical;
    memoryThresholdWarning_ = config.memoryThresholdWarning;
    memoryThresholdCritical_ = config.memoryThresholdCritical;
    memoryAllocationLimit_ = config.memoryAllocationLimit;
    maxComponentProfiles_ = config.maxComponentProfiles;
    maxCheckpoints_ = config.maxCheckpoints;
    samplingIntervalMs_ = config.samplingIntervalMs;
}

PerformanceConfig PerformanceMonitor::getConfig() const {
    return config_;
}

std::unordered_map<std::string, ComponentTiming> PerformanceMonitor::getComponentTimings() const {
    std::lock_guard<std::mutex> lock(timingMutex_);
    return componentTimings_;
}

std::unordered_map<std::string, MemoryAllocation> PerformanceMonitor::getMemoryAllocations() const {
    std::lock_guard<std::mutex> lock(memoryMutex_);
    return memoryAllocations_;
}

void PerformanceMonitor::monitorSystem() {
    while (isRunning_) {
        updateSystemMetrics();
        std::this_thread::sleep_for(std::chrono::milliseconds(samplingIntervalMs_));
    }
}

void PerformanceMonitor::cleanupExpiredData() {
    std::lock_guard<std::mutex> lock(timingMutex_);
    
    // Remove component timings with zero calls
    auto it = componentTimings_.begin();
    while (it != componentTimings_.end()) {
        if (it->second.callCount == 0) {
            it = componentTimings_.erase(it);
        } else {
            ++it;
        }
    }
    
    // Clean checkpoints if needed
    if (checkpoints_.size() > maxCheckpoints_) {
        checkpoints_.erase(checkpoints_.begin(), checkpoints_.end() - maxCheckpoints_);
    }
}

// Initialize global performance monitor
std::unique_ptr<IPerformanceMonitor> gPerformanceMonitor;

void initializePerformanceMonitor(const PerformanceConfig& config) {
    gPerformanceMonitor = std::make_unique<PerformanceMonitor>();
    gPerformanceMonitor->setConfig(config);
    gPerformanceMonitor->start();
}

IPerformanceMonitor& getPerformanceMonitor() {
    if (!gPerformanceMonitor) {
        initializePerformanceMonitor(PerformanceConfig());
    }
    return *gPerformanceMonitor;
}

void shutdownPerformanceMonitor() {
    if (gPerformanceMonitor) {
        gPerformanceMonitor->stop();
        gPerformanceMonitor.reset();
    }
}

// Performance monitoring macros
#define NLM_PERF_START(component) nlm::getPerformanceMonitor().recordComponentStart(component)
#define NLM_PERF_END(component) nlm::getPerformanceMonitor().recordComponentEnd(component)
#define NLM_PERF_ALLOC(component, size) nlm::getPerformanceMonitor().recordMemoryAllocation(component, size)
#define NLM_PERF_DEALLOC(component, size) nlm::getPerformanceMonitor().recordMemoryDeallocation(component, size)
#define NLM_PERF_ALERT(component, metric, value, threshold, severity) \
    nlm::getPerformanceMonitor().addAlertThreshold(component, metric, threshold, severity)

// Convenience functions for common components
#define NLM_PERF_NEURON_START neuronId string("Neuron(") + std::to_string(neuronId) + ")"
#define NLM_PERF_NEURON_END neuronId string("Neuron(") + std::to_string(neuronId) + ")"

// Function to get current performance metrics
PerformanceMetrics getCurrentMetrics() {
    return nlm::getPerformanceMonitor().getSystemMetrics();
}

// Function to create a performance checkpoint
void createPerfCheckpoint(const std::string& name, const std::string& description) {
    nlm::getPerformanceMonitor().createCheckpoint(name, description);
}

// Function to get optimization suggestions
std::vector<std::string> getPerfOptimizationSuggestions() {
    return nlm::getPerformanceMonitor().getOptimizationSuggestions();
}

// Function to generate performance report
std::string generatePerfReport() {
    return nlm::getPerformanceMonitor().generateReport();
}

} // namespace nlm