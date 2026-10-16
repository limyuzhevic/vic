#include "PerformanceMonitor/ResourceMonitor.hpp"
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <fstream>
#include <thread>
#include <chrono>
#include <numeric>
#include <map>
#include <unordered_map>
#include <set>
#include <sstream>
#include <iostream>

namespace nlm {

// Helper functions for ResourceMonitor
namespace {
    ResourceType stringToResourceType(const std::string& type) {
        if (type == "cpu") return ResourceType::CPU;
        if (type == "memory") return ResourceType::Memory;
        if (type == "disk") return ResourceType::Disk;
        if (type == "network") return ResourceType::Network;
        if (type == "gpu") return ResourceType::GPU;
        if (type == "process") return ResourceType::Process;
        if (type == "system") return ResourceType::System;
        return ResourceType::System;
    }
    
    ResourceMetric stringToResourceMetric(const std::string& metric) {
        if (metric == "usage") return ResourceMetric::Usage;
        if (metric == "temperature") return ResourceMetric::Temperature;
        if (metric == "frequency") return ResourceMetric::Frequency;
        if (metric == "load") return ResourceMetric::Load;
        if (metric == "throughput") return ResourceMetric::Throughput;
        if (metric == "latency") return ResourceMetric::Latency;
        if (metric == "capacity") return ResourceMetric::Capacity;
        if (metric == "available") return ResourceMetric::Available;
        return ResourceMetric::Usage;
    }
    
    std::string resourceTypeToString(ResourceType type) {
        switch (type) {
            case ResourceType::CPU: return "CPU";
            case ResourceType::Memory: return "Memory";
            case ResourceType::Disk: return "Disk";
            case ResourceType::Network: return "Network";
            case ResourceType::GPU: return "GPU";
            case ResourceType::Process: return "Process";
            case ResourceType::System: return "System";
            default: return "Unknown";
        }
    }
    
    std::string resourceMetricToString(ResourceMetric metric) {
        switch (metric) {
            case ResourceMetric::Usage: return "Usage";
            case ResourceMetric::Temperature: return "Temperature";
            case ResourceMetric::Frequency: return "Frequency";
            case ResourceMetric::Load: return "Load";
            case ResourceMetric::Throughput: return "Throughput";
            case ResourceMetric::Latency: return "Latency";
            case ResourceMetric::Capacity: return "Capacity";
            case ResourceMetric::Available: return "Available";
            default: return "Usage";
        }
    }
}

ResourceMonitor::ResourceMonitor() : pImpl(std::make_unique<Impl>()) {
    // Initialize monitoring thread
    if (monitoringInterval_ > 0.0) {
        monitoringThread_ = std::thread(&ResourceMonitor::updateCPUMonitoring, this);
        if (memoryTracking_) {
            memoryTrackingThread_ = std::thread(&ResourceMonitor::trackMemoryAllocations, this);
        }
    }
}

ResourceMonitor::~ResourceMonitor() {
    shutdown();
}

bool ResourceMonitor::initialize() {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    if (initialized_) {
        return true;
    }
    
    // Initialize system state
    initialized_ = true;
    
    // Start monitoring if configured
    if (monitoringInterval_ > 0.0) {
        startMonitoring();
    }
    
    // Log initialization
    if (logger_) {
        logger_->info("Resource monitor initialized");
    }
    
    return true;
}

void ResourceMonitor::shutdown() {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    if (!initialized_) {
        return;
    }
    
    // Stop monitoring
    stopMonitoring();
    
    // Stop memory tracking
    if (memoryTrackingThread_.joinable()) {
        memoryTrackingThread_.join();
    }
    
    // Clear resource measurements
    cpuMeasurements_.clear();
    memoryMeasurements_.clear();
    diskMeasurements_.clear();
    networkMeasurements_.clear();
    gpuMeasurements_.clear();
    
    // Clear memory tracking
    std::lock_guard<std::mutex> memoryLock(memoryMutex_);
    memoryAllocations_.clear();
    
    // Clear thresholds
    resourceThresholds_.clear();
    
    // Clear integration objects
    logger_.reset();
    statisticsCollector_.reset();
    realTimeMonitor_.reset();
    
    // Reset flags
    initialized_ = false;
    monitoring_ = false;
    memoryTracking_ = false;
    
    // Log shutdown
    if (logger_) {
        logger_->info("Resource monitor shutdown");
    }
}

void ResourceMonitor::recordMeasurement(const ResourceMeasurement& measurement) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    // Record the measurement
    recordMeasurementInternal(measurement);
}

void ResourceMonitor::recordMeasurement(ResourceType type, ResourceMetric metric, double value,
                                       const std::string& unit,
                                       size_t processId, const std::string& processName) {
    ResourceMeasurement measurement(type, metric, value, unit, processId, processName);
    recordMeasurement(measurement);
}

void ResourceMonitor::recordCPUUsage(double usage) {
    recordMeasurement(ResourceType::CPU, ResourceMetric::Usage, usage, "percentage");
}

void ResourceMonitor::recordCPUFrequency(double frequency) {
    recordMeasurement(ResourceType::CPU, ResourceMetric::Frequency, frequency, "GHz");
}

void ResourceMonitor::recordCPULoad(double load) {
    recordMeasurement(ResourceType::CPU, ResourceMetric::Load, load, "load");
}

void ResourceMonitor::recordCPUThreads(size_t threadCount) {
    recordMeasurement(ResourceType::CPU, ResourceMetric::Usage, static_cast<double>(threadCount),
                    "threads", 0, "system");
}

void ResourceMonitor::recordMemoryUsage(uint64_t totalBytes, uint64_t usedBytes,
                                       uint64_t availableBytes, uint64_t swapUsed) {
    // Record total memory
    recordMeasurement(ResourceType::Memory, ResourceMetric::Capacity,
                    static_cast<double>(totalBytes), "bytes");
    
    // Record used memory
    double usedPercentage = (totalBytes > 0) ? (static_cast<double>(usedBytes) / totalBytes) * 100.0 : 0.0;
    recordMeasurement(ResourceType::Memory, ResourceMetric::Usage,
                    usedPercentage, "percentage");
    
    // Record available memory
    recordMeasurement(ResourceType::Memory, ResourceMetric::Available,
                    static_cast<double>(availableBytes), "bytes");
    
    // Record swap usage
    recordMeasurement(ResourceType::Memory, ResourceMetric::Usage,
                    static_cast<double>(swapUsed), "bytes", 0, "swap");
}

void ResourceMonitor::recordMemoryAllocation(uint64_t address, size_t size,
                                            const std::string& allocationType) {
    if (trackMemoryAllocations_) {
        allocateMemory(address, size, allocationType);
    }
}

void ResourceMonitor::recordMemoryDeallocation(uint64_t address) {
    if (trackMemoryAllocations_) {
        deallocateMemory(address);
    }
}

void ResourceMonitor::recordDiskRead(double throughput, double latency) {
    recordMeasurement(ResourceType::Disk, ResourceMetric::Throughput,
                    throughput, "MB/s");
    recordMeasurement(ResourceType::Disk, ResourceMetric::Latency,
                    latency, "ms");
}

void ResourceMonitor::recordDiskWrite(double throughput, double latency) {
    recordMeasurement(ResourceType::Disk, ResourceMetric::Throughput,
                    throughput, "MB/s");
    recordMeasurement(ResourceType::Disk, ResourceMetric::Latency,
                    latency, "ms");
}

void ResourceMonitor::recordDiskUsage(double usage) {
    recordMeasurement(ResourceType::Disk, ResourceMetric::Usage, usage, "percentage");
}

void ResourceMonitor::recordNetworkUpload(double throughput, size_t packets) {
    recordMeasurement(ResourceType::Network, ResourceMetric::Throughput,
                    throughput, "bytes/s");
    recordMeasurement(ResourceType::Network, ResourceMetric::Usage,
                    static_cast<double>(packets), "packets");
}

void ResourceMonitor::recordNetworkDownload(double throughput, size_t packets) {
    recordMeasurement(ResourceType::Network, ResourceMetric::Throughput,
                    throughput, "bytes/s");
    recordMeasurement(ResourceType::Network, ResourceMetric::Usage,
                    static_cast<double>(packets), "packets");
}

void ResourceMonitor::recordNetworkLatency(double latency) {
    recordMeasurement(ResourceType::Network, ResourceMetric::Latency,
                    latency, "ms");
}

void ResourceMonitor::recordProcessCPU(const std::string& processName, double usage, size_t pid) {
    recordMeasurement(ResourceType::Process, ResourceMetric::Usage, usage, "percentage",
                    pid, processName);
}

void ResourceMonitor::recordProcessMemory(const std::string& processName, uint64_t memoryBytes, size_t pid) {
    recordMeasurement(ResourceType::Process, ResourceMetric::Usage,
                    static_cast<double>(memoryBytes), "bytes",
                    pid, processName);
}

void ResourceMonitor::recordProcessIO(const std::string& processName, const IOMetrics& ioMetrics, size_t pid) {
    recordMeasurement(ResourceType::Process, ResourceMetric::Throughput,
                    ioMetrics.readThroughput, "MB/s",
                    pid, processName + "_read");
    recordMeasurement(ResourceType::Process, ResourceMetric::Throughput,
                    ioMetrics.writeThroughput, "MB/s",
                    pid, processName + "_write");
}

void ResourceMonitor::recordGPUUsage(double usage) {
    recordMeasurement(ResourceType::GPU, ResourceMetric::Usage, usage, "percentage");
}

void ResourceMonitor::recordGPUMemory(double usedBytes, double totalBytes) {
    recordMeasurement(ResourceType::GPU, ResourceMetric::Usage,
                    (totalBytes > 0) ? (usedBytes / totalBytes) * 100.0 : 0.0, "percentage");
    recordMeasurement(ResourceType::GPU, ResourceMetric::Capacity, totalBytes, "bytes");
}

void ResourceMonitor::recordGPUPower(double powerWatts) {
    recordMeasurement(ResourceType::GPU, ResourceMetric::Usage, powerWatts, "Watts");
}

void ResourceMonitor::addResourceThreshold(ResourceType type, ResourceMetric metric,
                                          double warningLevel, double criticalLevel) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    resourceThresholds_.push_back({type, metric, warningLevel, criticalLevel});
}

void ResourceMonitor::removeResourceThreshold(ResourceType type, ResourceMetric metric) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    auto it = std::remove_if(resourceThresholds_.begin(), resourceThresholds_.end(),
                           [&type, &metric](const auto& threshold) {
                               return std::get<0>(threshold) == type &&
                                      std::get<1>(threshold) == metric;
                           });
    resourceThresholds_.erase(it, resourceThresholds_.end());
}

void ResourceMonitor::startMonitoring() {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    if (monitoring_) {
        return;
    }
    
    monitoring_ = true;
    
    // Start monitoring thread if not already running
    if (!monitoringThread_.joinable()) {
        monitoringThread_ = std::thread([this]() {
            while (monitoring_) {
                updateCPUMonitoring();
                updateMemoryMonitoring();
                updateDiskMonitoring();
                updateNetworkMonitoring();
                updateGPUMonitoring();
                updateProcessMonitoring();
                
                std::this_thread::sleep_for(std::chrono::duration<double>(monitoringInterval_));
            }
        });
        
        // Set thread to lower priority to avoid interfering with simulation
        // (This is platform-specific and would require additional code)
    }
    
    // Log start
    if (logger_) {
        logger_->info("Resource monitoring started");
    }
}

void ResourceMonitor::stopMonitoring() {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    if (!monitoring_) {
        return;
    }
    
    monitoring_ = false;
    
    // Wait for monitoring thread to finish
    if (monitoringThread_.joinable()) {
        monitoringThread_.join();
    }
    
    // Log stop
    if (logger_) {
        logger_->info("Resource monitoring stopped");
    }
}

void ResourceMonitor::startMemoryTracking() {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    if (memoryTracking_) {
        return;
    }
    
    memoryTracking_ = true;
    
    // Start memory tracking thread if not already running
    if (!memoryTrackingThread_.joinable()) {
        memoryTrackingThread_ = std::thread(&ResourceMonitor::trackMemoryAllocations, this);
    }
}

void ResourceMonitor::stopMemoryTracking() {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    if (!memoryTracking_) {
        return;
    }
    
    memoryTracking_ = false;
    
    // Wait for memory tracking thread to finish
    if (memoryTrackingThread_.joinable()) {
        memoryTrackingThread_.join();
    }
}

void ResourceMonitor::recordMeasurementInternal(const ResourceMeasurement& measurement) {
    // Record the measurement
    std::deque<ResourceMeasurement>* targetDeque = nullptr;
    
    switch (measurement.resourceType) {
        case ResourceType::CPU:
            targetDeque = &cpuMeasurements_;
            break;
        case ResourceType::Memory:
            targetDeque = &memoryMeasurements_;
            break;
        case ResourceType::Disk:
            targetDeque = &diskMeasurements_;
            break;
        case ResourceType::Network:
            targetDeque = &networkMeasurements_;
            break;
        case ResourceType::GPU:
            targetDeque = &gpuMeasurements_;
            break;
        case ResourceType::Process:
            // Process measurements are not stored in history
            break;
        default:
            break;
    }
    
    if (targetDeque) {
        targetDeque->push_back(measurement);
        
        // Keep history size manageable
        if (targetDeque->size() > maxMeasurementHistory_) {
            targetDeque->pop_front();
        }
    }
    
    // Notify resource listeners
    notifyResourceListeners(measurement);
    
    // Check for resource thresholds
    checkResourceThresholds();
    
    // Generate resource alerts
    generateResourceAlerts();
}

void ResourceMonitor::notifyResourceListeners(const ResourceMeasurement& measurement) {
    if (resourceCallback_) {
        resourceCallback_(measurement);
    }
    
    // Also notify via logger if available
    if (logger_) {
        logger_->debug("Resource measurement: " + resourceTypeToString(measurement.resourceType) +
                      " " + resourceMetricToString(measurement.metric) +
                      " = " + std::to_string(measurement.value) +
                      " " + measurement.unit);
    }
}

void ResourceMonitor::notifyAlertListeners(const std::string& alert) {
    if (alertCallback_) {
        alertCallback_(alert);
    }
    
    // Also notify via logger if available
    if (logger_) {
        logger_->warning("Resource alert: " + alert);
    }
}

void ResourceMonitor::checkResourceThresholds() {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    if (!enableRealTimeAlerts_) {
        return;
    }
    
    // Check each threshold
    for (const auto& threshold : resourceThresholds_) {
        ResourceType type = std::get<0>(threshold);
        ResourceMetric metric = std::get<1>(threshold);
        double warningLevel = std::get<2>(threshold);
        double criticalLevel = std::get<3>(threshold);
        
        // Find the latest measurement for this type and metric
        double currentValue = 0.0;
        bool valueExists = false;
        
        std::deque<ResourceMeasurement>* sourceDeque = nullptr;
        switch (type) {
            case ResourceType::CPU:
                sourceDeque = &cpuMeasurements_;
                break;
            case ResourceType::Memory:
                sourceDeque = &memoryMeasurements_;
                break;
            case ResourceType::Disk:
                sourceDeque = &diskMeasurements_;
                break;
            case ResourceType::Network:
                sourceDeque = &networkMeasurements_;
                break;
            case ResourceType::GPU:
                sourceDeque = &gpuMeasurements_;
                break;
            default:
                break;
        }
        
        if (sourceDeque && !sourceDeque->empty()) {
            for (auto it = sourceDeque->rbegin(); it != sourceDeque->rend(); ++it) {
                if (it->resourceType == type && it->metric == metric) {
                    currentValue = it->value;
                    valueExists = true;
                    break;
                }
            }
        }
        
        if (valueExists) {
            // Check if threshold is exceeded
            std::string alert = "Resource threshold exceeded: " +
                               resourceTypeToString(type) + " " +
                               resourceMetricToString(metric) + " = " +
                               std::to_string(currentValue) + " " +
                               warningLevel + "/" + std::to_string(criticalLevel);
            
            if (currentValue >= criticalLevel || currentValue <= warningLevel) {
                notifyAlertListeners(alert);
            }
        }
    }
}

void ResourceMonitor::generateResourceAlerts() {
    // Generate resource alerts based on current measurements
    // This is a simplified implementation
}

void ResourceMonitor::updateCPUMonitoring() {
    // Measure CPU usage
    measureCPUUsage();
}

void ResourceMonitor::updateMemoryMonitoring() {
    // Measure memory usage
    measureMemoryUsage();
}

void ResourceMonitor::updateDiskMonitoring() {
    // Measure disk usage
    measureDiskUsage();
}

void ResourceMonitor::updateNetworkMonitoring() {
    // Measure network usage
    measureNetworkUsage();
}

void ResourceMonitor::updateGPUMonitoring() {
    // Measure GPU usage
    measureGPUUsage();
}

void ResourceMonitor::updateProcessMonitoring() {
    // Measure process usage
    measureProcessUsage();
}

void ResourceMonitor::measureCPUUsage() {
    // This would be implemented with platform-specific code
    // For now, just record a dummy value
    recordCPUUsage(50.0); // 50% CPU usage
}

void ResourceMonitor::measureMemoryUsage() {
    // This would be implemented with platform-specific code
    // For now, just record dummy values
    uint64_t totalBytes = 8 * 1024 * 1024 * 1024UL; // 8 GB
    uint64_t usedBytes = 4 * 1024 * 1024 * 1024UL; // 4 GB
    uint64_t availableBytes = 3 * 1024 * 1024 * 1024UL; // 3 GB
    uint64_t swapUsed = 512 * 1024 * 1024UL; // 512 MB
    recordMemoryUsage(totalBytes, usedBytes, availableBytes, swapUsed);
}

void ResourceMonitor::measureDiskUsage() {
    // This would be implemented with platform-specific code
    // For now, just record dummy values
    recordDiskUsage(25.0); // 25% disk usage
}

void ResourceMonitor::measureNetworkUsage() {
    // This would be implemented with platform-specific code
    // For now, just record dummy values
    recordNetworkUpload(10.0, 1000); // 10 MB/s, 1000 packets
    recordNetworkDownload(20.0, 2000); // 20 MB/s, 2000 packets
}

void ResourceMonitor::measureGPUUsage() {
    // This would be implemented with platform-specific code
    // For now, just record dummy values
    recordGPUUsage(0.0); // 0% GPU usage (assuming no GPU)
    recordGPUMemory(0, 1024 * 1024 * 1024UL); // 0 MB used of 1 GB
}

void ResourceMonitor::measureProcessUsage() {
    // This would be implemented with platform-specific code
    // For now, just record dummy values
}

void ResourceMonitor::trackMemoryAllocations() {
    while (memoryTracking_) {
        std::this_thread::sleep_for(std::chrono::duration<double>(1.0));
        
        cleanupMemoryAllocations();
        
        // Log memory statistics
        if (logger_) {
            size_t currentUsage = getCurrentMemoryUsage();
            size_t peakUsage = getPeakMemoryUsage();
            logger_->debug("Memory usage: " + std::to_string(currentUsage) +
                          " bytes, peak: " + std::to_string(peakUsage) + " bytes");
        }
    }
}

void ResourceMonitor::cleanupMemoryAllocations() {
    std::lock_guard<std::mutex> memoryLock(memoryMutex_);
    
    // Remove deallocated allocations from history
    auto it = allocationHistory_.begin();
    while (it != allocationHistory_.end()) {
        if (!it->isAllocated) {
            it = allocationHistory_.erase(it);
        } else {
            ++it;
        }
    }
    
    // Limit history size
    while (allocationHistory_.size() > maxMeasurementHistory_) {
        allocationHistory_.pop_front();
    }
}

void ResourceMonitor::recordMemoryAllocationInternal(const MemoryAllocationInfo& allocation) {
    std::lock_guard<std::mutex> memoryLock(memoryMutex_);
    
    // Record allocation
    memoryAllocations_[allocation.address] = allocation;
    
    // Add to history
    allocationHistory_.push_back(allocation);
    
    // Keep history size manageable
    while (allocationHistory_.size() > maxMeasurementHistory_) {
        allocationHistory_.pop_front();
    }
}

void ResourceMonitor::recordMemoryDeallocationInternal(uint64_t address) {
    std::lock_guard<std::mutex> memoryLock(memoryMutex_);
    
    auto it = memoryAllocations_.find(address);
    if (it != memoryAllocations_.end()) {
        it->second.isAllocated = false;
        it->second.freedTime = std::chrono::system_clock::now();
    }
}

double ResourceMonitor::getResourceUsage(ResourceType type, ResourceMetric metric,
                                         size_t processId) const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    std::deque<ResourceMeasurement>* sourceDeque = nullptr;
    switch (type) {
        case ResourceType::CPU:
            sourceDeque = &cpuMeasurements_;
            break;
        case ResourceType::Memory:
            sourceDeque = &memoryMeasurements_;
            break;
        case ResourceType::Disk:
            sourceDeque = &diskMeasurements_;
            break;
        case ResourceType::Network:
            sourceDeque = &networkMeasurements_;
            break;
        case ResourceType::GPU:
            sourceDeque = &gpuMeasurements_;
            break;
        default:
            break;
    }
    
    if (!sourceDeque || sourceDeque->empty()) {
        return 0.0;
    }
    
    // Find the latest measurement
    for (auto it = sourceDeque->rbegin(); it != sourceDeque->rend(); ++it) {
        if (it->resourceType == type && it->metric == metric &&
            (processId == 0 || it->processId == processId)) {
            return it->value;
        }
    }
    
    return 0.0;
}

ResourceMeasurement ResourceMonitor::getLatestMeasurement(ResourceType type, ResourceMetric metric,
                                                         size_t processId) const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    std::deque<ResourceMeasurement>* sourceDeque = nullptr;
    switch (type) {
        case ResourceType::CPU:
            sourceDeque = &cpuMeasurements_;
            break;
        case ResourceType::Memory:
            sourceDeque = &memoryMeasurements_;
            break;
        case ResourceType::Disk:
            sourceDeque = &diskMeasurements_;
            break;
        case ResourceType::Network:
            sourceDeque = &networkMeasurements_;
            break;
        case ResourceType::GPU:
            sourceDeque = &gpuMeasurements_;
            break;
        default:
            break;
    }
    
    if (!sourceDeque || sourceDeque->empty()) {
        return ResourceMeasurement();
    }
    
    // Find the latest measurement
    for (auto it = sourceDeque->rbegin(); it != sourceDeque->rend(); ++it) {
        if (it->resourceType == type && it->metric == metric &&
            (processId == 0 || it->processId == processId)) {
            return *it;
        }
    }
    
    return ResourceMeasurement();
}

std::vector<ResourceMeasurement> ResourceMonitor::getMeasurements(ResourceType type,
                                                                 ResourceMetric metric,
                                                                 size_t count) const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    std::vector<ResourceMeasurement> result;
    
    std::deque<ResourceMeasurement>* sourceDeque = nullptr;
    switch (type) {
        case ResourceType::CPU:
            sourceDeque = &cpuMeasurements_;
            break;
        case ResourceType::Memory:
            sourceDeque = &memoryMeasurements_;
            break;
        case ResourceType::Disk:
            sourceDeque = &diskMeasurements_;
            break;
        case ResourceType::Network:
            sourceDeque = &networkMeasurements_;
            break;
        case ResourceType::GPU:
            sourceDeque = &gpuMeasurements_;
            break;
        default:
            break;
    }
    
    if (!sourceDeque) {
        return result;
    }
    
    // Copy measurements to result
    auto start = sourceDeque->rbegin();
    auto end = sourceDeque->rend();
    auto copyStart = start;
    
    for (auto it = start; it != end && result.size() < count; ++it) {
        if (it->resourceType == type && it->metric == metric) {
            result.push_back(*it);
        }
    }
    
    return result;
}

ResourceStatistics ResourceMonitor::getResourceStatistics(ResourceType type, ResourceMetric metric,
                                                          size_t processId) const {
    std::vector<ResourceMeasurement> measurements = getMeasurements(type, metric, 1000);
    
    ResourceStatistics stats;
    
    if (measurements.empty()) {
        return stats;
    }
    
    // Calculate statistics
    double sum = 0.0;
    double sumSq = 0.0;
    double min = std::numeric_limits<double>::max();
    double max = std::numeric_limits<double>::lowest();
    
    for (const auto& measurement : measurements) {
        double value = measurement.value;
        sum += value;
        sumSq += value * value;
        min = std::min(min, value);
        max = std::max(max, value);
    }
    
    size_t count = measurements.size();
    stats.avgUsage = sum / count;
    stats.maxUsage = max;
    stats.minUsage = min;
    
    if (count > 1) {
        stats.stdDev = std::sqrt((sumSq / count) - (stats.avgUsage * stats.avgUsage));
    }
    
    stats.firstMeasurement = measurements.front().timestamp;
    stats.lastMeasurement = measurements.back().timestamp;
    stats.measurementCount = count;
    
    return stats;
}

std::vector<std::pair<size_t, std::string>> ResourceMonitor::getActiveProcesses() const {
    std::vector<std::pair<size_t, std::string>> processes;
    
    // This would be implemented with platform-specific code
    // For now, return empty list
    return processes;
}

std::optional<std::string> ResourceMonitor::getProcessName(size_t processId) const {
    // This would be implemented with platform-specific code
    // For now, return empty optional
    return std::nullopt;
}

void ResourceMonitor::allocateMemory(uint64_t address, size_t size, const std::string& type) {
    std::lock_guard<std::mutex> memoryLock(memoryMutex_);
    
    MemoryAllocationInfo allocation;
    allocation.address = address;
    allocation.size = size;
    allocation.allocationType = type;
    allocation.allocatedTime = std::chrono::system_clock::now();
    allocation.isAllocated = true;
    allocation.threadId = std::hash<std::thread::id>{}(std::this_thread::get_id());
    
    memoryAllocations_[address] = allocation;
    
    // Log allocation
    if (logger_) {
        logger_->debug("Memory allocated: " + std::to_string(address) +
                      " size: " + std::to_string(size) +
                      " type: " + type);
    }
}

void ResourceMonitor::deallocateMemory(uint64_t address) {
    std::lock_guard<std::mutex> memoryLock(memoryMutex_);
    
    auto it = memoryAllocations_.find(address);
    if (it != memoryAllocations_.end()) {
        it->second.isAllocated = false;
        it->second.freedTime = std::chrono::system_clock::now();
        
        // Log deallocation
        if (logger_) {
            logger_->debug("Memory deallocated: " + std::to_string(address));
        }
    }
}

std::optional<MemoryAllocationInfo> ResourceMonitor::getMemoryAllocation(uint64_t address) const {
    std::lock_guard<std::mutex> memoryLock(memoryMutex_);
    
    auto it = memoryAllocations_.find(address);
    if (it != memoryAllocations_.end()) {
        return it->second;
    }
    
    return std::nullopt;
}

std::vector<MemoryAllocationInfo> ResourceMonitor::getActiveAllocations() const {
    std::lock_guard<std::mutex> memoryLock(memoryMutex_);
    
    std::vector<MemoryAllocationInfo> activeAllocations;
    
    for (const auto& pair : memoryAllocations_) {
        if (pair.second.isAllocated) {
            activeAllocations.push_back(pair.second);
        }
    }
    
    return activeAllocations;
}

void ResourceMonitor::integrateWithLogger(std::shared_ptr<Logger> logger) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    logger_ = logger;
    
    if (logger_) {
        logger_->info("Resource monitor integrated with Logger");
    }
}

void ResourceMonitor::integrateWithStatisticsCollector(std::shared_ptr<StatisticsCollector> collector) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    statisticsCollector_ = collector;
    
    if (statisticsCollector_) {
        if (logger_) {
            logger_->info("Resource monitor integrated with StatisticsCollector");
        }
    }
}

void ResourceMonitor::integrateWithRealTimeMonitor(std::shared_ptr<RealTimeMonitor> monitor) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    realTimeMonitor_ = monitor;
    
    if (realTimeMonitor_) {
        if (logger_) {
            logger_->info("Resource monitor integrated with RealTimeMonitor");
        }
    }
}

void ResourceMonitor::setupBasicConnectivityModeMonitoring() {
    // Setup monitoring for basic connectivity mode
    startMonitoring();
    recordMeasurement(ResourceType::System, ResourceMetric::Usage, 1.0, "", 0, "basic_connectivity");
}

void ResourceMonitor::setupPlasticityExperimentMonitoring() {
    // Setup monitoring for plasticity experiment mode
    startMonitoring();
    recordMeasurement(ResourceType::System, ResourceMetric::Usage, 2.0, "", 0, "plasticity_experiment");
}

void ResourceMonitor::setupStdpVerificationMonitoring() {
    // Setup monitoring for STDP verification mode
    startMonitoring();
    recordMeasurement(ResourceType::System, ResourceMetric::Usage, 3.0, "", 0, "stdp_verification");
}

void ResourceMonitor::setupBenchmarkMonitoring() {
    // Setup monitoring for benchmark mode
    startMonitoring();
    recordMeasurement(ResourceType::System, ResourceMetric::Usage, 4.0, "", 0, "benchmark");
}

void ResourceMonitor::setupContinuousModeMonitoring() {
    // Setup monitoring for continuous mode
    startMonitoring();
    recordMeasurement(ResourceType::System, ResourceMetric::Usage, 5.0, "", 0, "continuous");
}

void ResourceMonitor::setupDemoModeMonitoring() {
    // Setup monitoring for demo mode
    startMonitoring();
    recordMeasurement(ResourceType::System, ResourceMetric::Usage, 6.0, "", 0, "demo");
}

void ResourceMonitor::setupPythonInterfaceMonitoring() {
    // Setup monitoring for Python interface
    startMonitoring();
    recordMeasurement(ResourceType::System, ResourceMetric::Usage, 7.0, "", 0, "python_interface");
}

void ResourceMonitor::setupScalingBenchmarkMonitoring() {
    // Setup monitoring for scaling benchmark
    startMonitoring();
    recordMeasurement(ResourceType::System, ResourceMetric::Usage, 8.0, "", 0, "scaling_benchmark");
}

void ResourceMonitor::setupResultsSaveMonitoring(const std::string& filepath) {
    // Setup monitoring for results save mode
    startMonitoring();
    recordMeasurement(ResourceType::System, ResourceMetric::Usage, 9.0, "", 0, "results_save");
}

} // namespace nlm
