#include "ResourceMonitor.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <random>

namespace nlm {

ResourceMonitor::ResourceMonitor()
    : monitoring_(false), samplingInterval_(0.5), cpuThreshold_(80.0), memoryThreshold_(90.0),
      enableGPUTracking_(false), enableNetworkTracking_(false), enableDiskTracking_(false),
      currentCpuUsage_(0.0), currentMemoryUsage_(0.0), currentMemoryAllocations_(0),
      currentThreadCount_(0), currentGpuUsage_(0.0), currentNetworkUsage_(0.0), currentDiskUsage_(0.0),
      lastUpdateTime_(std::chrono::system_clock::now()) {
}

ResourceMonitor::~ResourceMonitor() {
    stop();
}

void ResourceMonitor::start() {
    if (monitoring_) return;
    
    monitoring_ = true;
    monitoringThread_ = std::thread(&ResourceMonitor::monitoringLoop, this);
    
    Logger::getGlobal().info("Resource monitoring started");
}

void ResourceMonitor::stop() {
    if (!monitoring_) return;
    
    monitoring_ = false;
    
    if (monitoringThread_.joinable()) {
        monitoringThread_.join();
    }
    
    Logger::getGlobal().info("Resource monitoring stopped");
}

void ResourceMonitor::monitoringLoop() {
    while (monitoring_) {
        updateResourceUsage();
        
        std::this_thread::sleep_for(std::chrono::duration<double>(samplingInterval_));
    }
}

void ResourceMonitor::updateResourceUsage() {
    simulateResourceUsage();
    
    if (debuggingSystem_) {
        debuggingSystem_->recordResourceUsage(
            currentCpuUsage_, currentMemoryUsage_, 
            currentMemoryAllocations_, currentThreadCount_);
    }
    
    // Add to history
    ResourceUsage usage;
    usage.cpuUsage = currentCpuUsage_;
    usage.memoryUsage = currentMemoryUsage_;
    usage.memoryAllocations = currentMemoryAllocations_;
    usage.threadCount = currentThreadCount_;
    usage.gpuUsage = currentGpuUsage_;
    usage.networkUsage = currentNetworkUsage_;
    usage.diskUsage = currentDiskUsage_;
    usage.timestamp = std::chrono::system_clock::now();
    
    std::lock_guard<std::mutex> lock(historyMutex_);
    resourceHistory_.push_back(usage);
    
    lastUpdateTime_ = usage.timestamp;
    
    // Keep only last 1000 entries
    if (resourceHistory_.size() > 1000) {
        resourceHistory_.erase(resourceHistory_.begin());
    }
}

void ResourceMonitor::simulateResourceUsage() {
    // Simulate resource usage - in real implementation, this would query actual system resources
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> cpuDist(10.0, 90.0);
    std::uniform_real_distribution<> memDist(20.0, 80.0);
    std::uniform_int_distribution<> allocDist(100, 500);
    std::uniform_int_distribution<> threadDist(4, 16);
    
    currentCpuUsage_ = cpuDist(gen);
    currentMemoryUsage_ = memDist(gen);
    currentMemoryAllocations_ = static_cast<size_t>(allocDist(gen));
    currentThreadCount_ = static_cast<size_t>(threadDist(gen));
    
    // Optional metrics
    if (enableGPUTracking_) {
        std::uniform_real_distribution<> gpuDist(0.0, 100.0);
        currentGpuUsage_ = gpuDist(gen);
    }
    
    if (enableNetworkTracking_) {
        std::uniform_real_distribution<> netDist(0.0, 50.0);
        currentNetworkUsage_ = netDist(gen);
    }
    
    if (enableDiskTracking_) {
        std::uniform_real_distribution<> diskDist(0.0, 100.0);
        currentDiskUsage_ = diskDist(gen);
    }
}

std::vector<ResourceMonitor::ResourceUsage> ResourceMonitor::getResourceHistory() const {
    std::lock_guard<std::mutex> lock(historyMutex_);
    return resourceHistory_;
}

std::string ResourceMonitor::getResourceReport() const {
    std::ostringstream oss;
    oss << "Resource Monitor Report\n";
    oss << "======================\n\n";
    
    oss << "Current Usage:\n";
    oss << "  CPU: " << currentCpuUsage_ << "%\n";
    oss << "  Memory: " << currentMemoryUsage_ << " MB\n";
    oss << "  Allocations: " << currentMemoryAllocations_ << "\n";
    oss << "  Threads: " << currentThreadCount_ << "\n";
    
    if (enableGPUTracking_) {
        oss << "  GPU: " << currentGpuUsage_ << "%\n";
    }
    if (enableNetworkTracking_) {
        oss << "  Network: " << currentNetworkUsage_ << " MB/s\n";
    }
    if (enableDiskTracking_) {
        oss << "  Disk: " << currentDiskUsage_ << "%\n";
    }
    
    oss << "\nThresholds:\n";
    oss << "  CPU: " << cpuThreshold_ << "%\n";
    oss << "  Memory: " << memoryThreshold_ << "%\n\n";
    
    oss << "History:\n";
    oss << "  Samples: " << resourceHistory_.size() << "\n";
    if (!resourceHistory_.empty()) {
        double avgCpu = 0.0, avgMem = 0.0;
        size_t overCpu = 0, overMem = 0;
        for (const auto& usage : resourceHistory_) {
            avgCpu += usage.cpuUsage;
            avgMem += usage.memoryUsage;
            if (usage.cpuUsage > cpuThreshold_) overCpu++;
            if (usage.memoryUsage > memoryThreshold_) overMem++;
        }
        avgCpu /= resourceHistory_.size();
        avgMem /= resourceHistory_.size();
        
        oss << "  Average CPU: " << avgCpu << "%\n";
        oss << "  Average Memory: " << avgMem << " MB\n";
        oss << "  Time over CPU threshold: " << overCpu << " samples\n";
        oss << "  Time over Memory threshold: " << overMem << " samples\n";
    }
    
    return oss.str();
}

std::string ResourceMonitor::getDetailedResourceReport() const {
    std::ostringstream oss;
    oss << "=== DETAILED RESOURCE ANALYSIS ===\n\n";
    
    oss << "Real-time Resource Statistics:\n";
    oss << getRealTimeResourceStats() << "\n";
    
    oss << "Resource Usage Trends:\n";
    oss << analyzeResourceTrends() << "\n";
    
    oss << "Resource Bottlenecks:\n";
    oss << identifyResourceBottlenecks() << "\n";
    
    oss << "Resource History Summary:\n";
    {
        std::lock_guard<std::mutex> lock(historyMutex_);
        oss << "  Total Samples: " << resourceHistory_.size() << "\n";
        if (!resourceHistory_.empty()) {
            oss << "  Time Span: " << 
                std::chrono::duration_cast<std::chrono::seconds>(
                    resourceHistory_.back().timestamp - resourceHistory_.front().timestamp).count() << " seconds\n";
            
            double avgCpu = calculateAverageCpuUsage();
            double avgMem = calculateAverageMemoryUsage();
            
            oss << "  Average CPU: " << avgCpu << "%\n";
            oss << "  Average Memory: " << avgMem << " MB\n";
            oss << "  Max CPU: " << getMaxCpuUsage() << "%\n";
            oss << "  Max Memory: " << getMaxMemoryUsage() << " MB\n";
        }
    }
    
    return oss.str();
}

std::string ResourceMonitor::analyzeResourceTrends() const {
    std::ostringstream oss;
    oss << "Resource Trend Analysis:\n";
    oss << "===================\n\n";
    
    std::lock_guard<std::mutex> lock(historyMutex_);
    
    if (resourceHistory_.size() < 2) {
        oss << "Insufficient data for trend analysis.\n";
        return oss.str();
    }
    
    // Analyze recent trend (last 10 samples)
    size_t recentSamples = std::min(resourceHistory_.size(), size_t(10));
    std::vector<double> cpuUsages, memUsages;
    
    for (size_t i = 0; i < recentSamples; ++i) {
        cpuUsages.push_back(resourceHistory_[resourceHistory_.size() - 1 - i].cpuUsage);
        memUsages.push_back(resourceHistory_[resourceHistory_.size() - 1 - i].memoryUsage);
    }
    
    oss << "CPU Usage Trend:\n";
    for (size_t i = 0; i < cpuUsages.size(); ++i) {
        oss << "  Sample " << i + 1 << ": " << cpuUsages[i] << "%\n";
    }
    
    oss << "\nMemory Usage Trend:\n";
    for (size_t i = 0; i < memUsages.size(); ++i) {
        oss << "  Sample " << i + 1 << ": " << memUsages[i] << " MB\n";
    }
    
    // Calculate trend direction
    if (cpuUsages.size() >= 2) {
        double cpuTrend = calculateTrend(resourceHistory_, "cpu");
        oss << "\nCPU Trend: " << (cpuTrend > 0 ? "Increasing" : (cpuTrend < 0 ? "Decreasing" : "Stable")) << " (" << cpuTrend << ")\n";
    }
    
    if (memUsages.size() >= 2) {
        double memTrend = calculateTrend(resourceHistory_, "memory");
        oss << "Memory Trend: " << (memTrend > 0 ? "Increasing" : (memTrend < 0 ? "Decreasing" : "Stable")) << " (" << memTrend << ")\n";
    }
    
    return oss.str();
}

std::string ResourceMonitor::identifyResourceBottlenecks() const {
    std::ostringstream oss;
    oss << "Resource Bottleneck Identification:\n";
    oss << "===============================\n\n";
    
    std::lock_guard<std::mutex> lock(historyMutex_);
    
    oss << "Bottleneck Analysis:\n";
    
    if (!resourceHistory_.empty()) {
        // Find time periods with high resource usage
        size_t highCpuTime = 0, highMemTime = 0;
        double maxCpu = 0.0, maxMem = 0.0;
        
        for (const auto& usage : resourceHistory_) {
            if (usage.cpuUsage > cpuThreshold_) highCpuTime++;
            if (usage.memoryUsage > memoryThreshold_) highMemTime++;
            maxCpu = std::max(maxCpu, usage.cpuUsage);
            maxMem = std::max(maxMem, usage.memoryUsage);
        }
        
        oss << "  Time spent over CPU threshold: " << highCpuTime << " samples\n";
        oss << "  Time spent over memory threshold: " << highMemTime << " samples\n";
        oss << "  Maximum CPU usage: " << maxCpu << "%\n";
        oss << "  Maximum memory usage: " << maxMem << " MB\n";
        
        if (maxCpu > 90.0) {
            oss << "  *** CRITICAL: CPU near saturation ***\n";
        } else if (maxCpu > 80.0) {
            oss << "  *** WARNING: High CPU usage ***\n";
        }
        
        if (maxMem > 90.0) {
            oss << "  *** CRITICAL: Memory near saturation ***\n";
        } else if (maxMem > 80.0) {
            oss << "  *** WARNING: High memory usage ***\n";
        }
        
        if (highCpuTime > resourceHistory_.size() * 0.5) {
            oss << "  *** WARNING: Sustained high CPU usage ***\n";
        }
        
        if (highMemTime > resourceHistory_.size() * 0.5) {
            oss << "  *** WARNING: Sustained high memory usage ***\n";
        }
    }
    
    oss << "\nRecommended Actions:\n";
    oss << "  - Monitor CPU-intensive operations\n";
    oss << "  - Optimize memory allocation patterns\n";
    oss << "  - Implement load balancing strategies\n";
    oss << "  - Consider system resource scaling\n";
    
    return oss.str();
}

double ResourceMonitor::calculateAverageCpuUsage() const {
    std::lock_guard<std::mutex> lock(historyMutex_);
    if (resourceHistory_.empty()) return 0.0;
    
    double sum = std::accumulate(resourceHistory_.begin(), resourceHistory_.end(), 0.0,
                                 [](double acc, const ResourceUsage& usage) {
                                     return acc + usage.cpuUsage;
                                 });
    return sum / resourceHistory_.size();
}

double ResourceMonitor::calculateAverageMemoryUsage() const {
    std::lock_guard<std::mutex> lock(historyMutex_);
    if (resourceHistory_.empty()) return 0.0;
    
    double sum = std::accumulate(resourceHistory_.begin(), resourceHistory_.end(), 0.0,
                                 [](double acc, const ResourceUsage& usage) {
                                     return acc + usage.memoryUsage;
                                 });
    return sum / resourceHistory_.size();
}

double ResourceMonitor::getMaxCpuUsage() const {
    std::lock_guard<std::mutex> lock(historyMutex_);
    if (resourceHistory_.empty()) return 0.0;
    
    return std::max_element(resourceHistory_.begin(), resourceHistory_.end(),
                           [](const ResourceUsage& a, const ResourceUsage& b) {
                               return a.cpuUsage < b.cpuUsage;
                           })->cpuUsage;
}

double ResourceMonitor::getMaxMemoryUsage() const {
    std::lock_guard<std::mutex> lock(historyMutex_);
    if (resourceHistory_.empty()) return 0.0;
    
    return std::max_element(resourceHistory_.begin(), resourceHistory_.end(),
                           [](const ResourceUsage& a, const ResourceUsage& b) {
                               return a.memoryUsage < b.memoryUsage;
                           })->memoryUsage;
}

bool ResourceMonitor::isResourceUsageHigh() const {
    return currentCpuUsage_ > cpuThreshold_ || currentMemoryUsage_ > memoryThreshold_;
}

std::string ResourceMonitor::getRealTimeResourceStats() const {
    std::ostringstream oss;
    oss << "Real-Time Resource Statistics:\n";
    oss << "==========================\n\n";
    
    oss << "Current Resource Usage:\n";
    oss << "  CPU: " << currentCpuUsage_ << "%\n";
    oss << "  Memory: " << currentMemoryUsage_ << " MB\n";
    oss << "  Allocations: " << currentMemoryAllocations_ << "\n";
    oss << "  Threads: " << currentThreadCount_ << "\n";
    
    if (enableGPUTracking_) {
        oss << "  GPU: " << currentGpuUsage_ << "%\n";
    }
    if (enableNetworkTracking_) {
        oss << "  Network: " << currentNetworkUsage_ << " MB/s\n";
    }
    if (enableDiskTracking_) {
        oss << "  Disk: " << currentDiskUsage_ << "%\n";
    }
    
    oss << "\nThreshold Status:\n";
    oss << "  CPU Threshold: " << (currentCpuUsage_ > cpuThreshold_ ? "OVER" : "OK") << " (" << cpuThreshold_ << "%)\n";
    oss << "  Memory Threshold: " << (currentMemoryUsage_ > memoryThreshold_ ? "OVER" : "OK") << " (" << memoryThreshold_ << "%)\n";
    
    if (isResourceUsageHigh()) {
        oss << "\nALERT: High resource usage detected!\n";
    }
    
    oss << "\nLast Update: " << 
        std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now() - lastUpdateTime_).count() << " seconds ago\n";
    
    return oss.str();
}

bool ResourceMonitor::isResourceThresholdExceeded() const {
    return currentCpuUsage_ > cpuThreshold_ || currentMemoryUsage_ > memoryThreshold_;
}

std::string ResourceMonitor::getSystemInfo() const {
    std::ostringstream oss;
    oss << "System Information:\n";
    oss << "=================\n\n";
    
    // Simulated system info
    oss << "CPU Cores: 8\n";
    oss << "Memory: 16 GB\n";
    oss << "OS: Linux\n";
    oss << "Architecture: x86_64\n";
    oss << "Current Working Directory: /workspace\n";
    
    return oss.str();
}

std::string ResourceMonitor::formatResourceUsage(const ResourceUsage& usage) const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    oss << "CPU: " << usage.cpuUsage << "%, Memory: " << usage.memoryUsage << " MB, "
        << "Threads: " << usage.threadCount << ", GPU: " << usage.gpuUsage << "%";
    return oss.str();
}

double ResourceMonitor::calculateTrend(const std::vector<ResourceUsage>& history, 
                                      const std::string& metric) const {
    if (history.size() < 2) return 0.0;
    
    double sumX = 0.0, sumY = 0.0, sumXY = 0.0, sumX2 = 0.0, sumY2 = 0.0;
    size_t n = history.size();
    
    for (size_t i = 0; i < n; ++i) {
        double x = static_cast<double>(i);
        double y = (metric == "cpu") ? history[i].cpuUsage : history[i].memoryUsage;
        sumX += x;
        sumY += y;
        sumXY += x * y;
        sumX2 += x * x;
        sumY2 += y * y;
    }
    
    double denominator = std::sqrt((n * sumX2 - sumX * sumX) * (n * sumY2 - sumY * sumY));
    if (denominator < 1e-10) return 0.0;
    
    return (n * sumXY - sumX * sumY) / denominator;
}

bool ResourceMonitor::exportResourceProfile(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << exportResourceProfileAsString();
        file.close();
        return true;
    } catch (...) {
        return false;
    }
}

std::string ResourceMonitor::exportResourceProfileAsString() const {
    std::ostringstream oss;
    oss << "Resource Profile\n";
    oss << "================\n\n";
    
    oss << "Monitoring Session:\n";
    oss << "  Started: " << (monitoring_ ? "Yes" : "No") << "\n";
    oss << "  Sampling Interval: " << samplingInterval_ << " seconds\n";
    oss << "  Current Samples: " << resourceHistory_.size() << "\n";
    
    oss << "\nCurrent Resource State:\n";
    oss << "  CPU: " << currentCpuUsage_ << "%\n";
    oss << "  Memory: " << currentMemoryUsage_ << " MB\n";
    oss << "  Allocations: " << currentMemoryAllocations_ << "\n";
    oss << "  Threads: " << currentThreadCount_ << "\n";
    
    oss << "\nThresholds:\n";
    oss << "  CPU: " << cpuThreshold_ << "%\n";
    oss << "  Memory: " << memoryThreshold_ << "%\n";
    
    oss << "\nStatistics:\n";
    oss << "  Average CPU: " << calculateAverageCpuUsage() << "%\n";
    oss << "  Average Memory: " << calculateAverageMemoryUsage() << " MB\n";
    oss << "  Max CPU: " << getMaxCpuUsage() << "%\n";
    oss << "  Max Memory: " << getMaxMemoryUsage() << " MB\n";
    
    oss << "\nMonitoring Status:\n";
    oss << "  Active: " << (monitoring_ ? "Yes" : "No") << "\n";
    if (monitoring_) {
        oss << "  Last Update: " << 
            std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now() - lastUpdateTime_).count() << " seconds ago\n";
    }
    
    return oss.str();
}

} // namespace nlm
