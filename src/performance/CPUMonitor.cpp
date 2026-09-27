// Implementation of CPUMonitor

#include "nlm/performance/CPUMonitor.hpp"
#include <algorithm>
#include <numeric>
#include <sstream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <sys/sysinfo.h>
#include <unistd.h>

namespace nlm {

CPUMonitor::CPUMonitor(
    std::chrono::milliseconds updateInterval,
    bool enableDetailedMetrics)
    : updateInterval_(updateInterval)
    , enableDetailedMetrics_(enableDetailedMetrics) {
    // Initialize core statistics
    size_t coreCount = getCoreCount();
    coreStats_.reserve(coreCount);
    
    for (size_t i = 0; i < coreCount; ++i) {
        CoreUsageStats stats;
        stats.coreId = i;
        stats.timestamp = std::chrono::high_resolution_clock::now();
        coreStats_.push_back(stats);
    }
}

void CPUMonitor::start() {
    if (running_.load(std::memory_order_relaxed)) {
        return;
    }
    
    running_.store(true, std::memory_order_relaxed);
    monitoringThread_ = std::thread(&CPUMonitor::cpuMonitoringThreadFunc, this);
}

void CPUMonitor::stop() {
    if (!running_.load(std::memory_order_relaxed)) {
        return;
    }
    
    {
        std::unique_lock<std::mutex> lock(stopMutex_);
        running_.store(false, std::memory_order_relaxed);
        stopCondition_.notify_one();
    }
    
    if (monitoringThread_.joinable()) {
        monitoringThread_.join();
    }
}

void CPUMonitor::cpuMonitoringThreadFunc() {
    while (running_.load(std::memory_order_relaxed)) {
        std::unique_lock<std::mutex> lock(stopMutex_);
        if (stopCondition_.wait_for(lock, updateInterval_, 
                                   [this] { return !running_.load(std::memory_order_relaxed); })) {
            break;
        }
        lock.unlock();
        
        updateCoreStats();
        
        if (enableDetailedMetrics_) {
            updateInstructionStats();
            updateCacheStats();
            updateBranchStats();
        }
        
        checkCPUThresholds();
    }
}

void CPUMonitor::updateCoreStats() {
    std::lock_guard<std::mutex> lock(coreStatsMutex_);
    
    // Get system load averages
    double loadAvg[3];
    getloadavg(loadAvg, 3);
    
    // Get CPU times
    std::vector<std::pair<pid_t, std::string>> processes;
    
    // Get CPU frequency
    double cpuFrequency = getCurrentCPUFrequency();
    
    // Update core statistics based on load averages
    size_t coreCount = getCoreCount();
    
    // Simplified core usage calculation based on load averages
    double loadPerCore = loadAvg[0] / coreCount;
    
    for (size_t i = 0; i < coreCount && i < coreStats_.size(); ++i) {
        CoreUsageStats& stats = coreStats_[i];
        
        // Calculate usage based on load average and core index
        double baseUsage = loadPerCore * 100.0;
        double variation = std::sin(static_cast<double>(i) * 0.5) * 20.0; // Simulate variation
        double usage = std::max(0.0, std::min(100.0, baseUsage + variation));
        
        stats.coreId = i;
        stats.usagePercentage = usage;
        
        // Simulate active/idle time based on usage
        auto totalTime = std::chrono::seconds(1);
        stats.activeTime = std::chrono::duration_cast<std::chrono::high_resolution_clock::duration>(
            std::chrono::duration<double>(usage / 100.0 * totalTime.count())
        );
        stats.totalTime = totalTime;
        
        stats.threadCount = 1 + static_cast<size_t>(usage / 20.0); // More threads for higher usage
        
        // Calculate efficiency (usage / threads)
        if (stats.threadCount > 0) {
            stats.efficiency = usage / static_cast<double>(stats.threadCount);
        }
        
        stats.timestamp = std::chrono::high_resolution_clock::now();
        
        // Call callbacks
        for (const auto& callback : coreUsageCallbacks_) {
            callback(stats);
        }
    }
}

void CPUMonitor::updateInstructionStats() {
    std::lock_guard<std::mutex> lock(instructionMutex_);
    
    // Simulate instruction statistics
    InstructionStats stats;
    stats.instructionSet = "AVX-512";
    stats.instructionsExecuted = 1000000000 + std::rand() % 100000000;
    stats.cycles = stats.instructionsExecuted / 4; // Assume 4 instructions per cycle
    stats.instructionsPerCycle = stats.instructionsExecuted / static_cast<double>(stats.cycles);
    stats.utilization = 85.0 + std::rand() % 15; // 85-100%
    stats.timestamp = std::chrono::high_resolution_clock::now();
    
    instructionStats_.push_back(stats);
    
    // Maintain history
    while (instructionStats_.size() > 1000) {
        instructionStats_.erase(instructionStats_.begin());
    }
    
    // Call callbacks
    for (const auto& callback : instructionCallbacks_) {
        callback(stats);
    }
}

void CPUMonitor::updateCacheStats() {
    std::lock_guard<std::mutex> lock(cacheMutex_);
    
    // Simulate cache statistics
    std::vector<std::string> cacheLevels = {"L1", "L2", "L3"};
    
    for (const auto& level : cacheLevels) {
        CacheStats stats;
        stats.cacheLevel = level;
        stats.size = (level == "L1") ? 32 * 1024 : ((level == "L2") ? 256 * 1024 : 32 * 1024 * 1024);
        stats.lines = stats.size / 64; // Assuming 64-byte cache lines
        stats.lineSize = 64;
        stats.hitRate = 0.7 + std::rand() % 301 / 1000.0; // 70-100%
        stats.accesses = 1000000 + std::rand() % 500000;
        stats.misses = stats.accesses * (1.0 - stats.hitRate);
        stats.timestamp = std::chrono::high_resolution_clock::now();
        
        cacheStats_.push_back(stats);
    }
    
    // Maintain history
    while (cacheStats_.size() > 1000) {
        cacheStats_.erase(cacheStats_.begin());
    }
}

void CPUMonitor::updateBranchStats() {
    std::lock_guard<std::mutex> lock(branchMutex_);
    
    BranchStats& stats = branchStats_;
    stats.totalBranches = 1000000 + std::rand() % 500000;
    stats.mispredictedBranches = stats.totalBranches * (0.05 + std::rand() % 101 / 1000.0); // 5-15% misprediction
    stats.mispredictionRate = stats.mispredictedBranches / static_cast<double>(stats.totalBranches);
    stats.timestamp = std::chrono::high_resolution_clock::now();
}

void CPUMonitor::checkCPUThresholds() {
    std::lock_guard<std::mutex> lock(coreStatsMutex_);
    
    // Check for thermal warnings
    for (const auto& stats : coreStats_) {
        if (stats.usagePercentage > 90.0) {
            CPUAlert alert;
            alert.type = CPUAlert::AlertType::HIGH_USAGE;
            alert.description = "Core " + std::to_string(stats.coreId) + " usage is high: " + 
                               std::to_string(stats.usagePercentage) + "%";
            alert.severity = 0.8;
            alert.timestamp = std::chrono::high_resolution_clock::now();
            alert.details["core_id"] = std::to_string(stats.coreId);
            alert.details["usage_percentage"] = std::to_string(stats.usagePercentage);
            
            cpuAlerts_.push_back(alert);
            
            for (const auto& callback : cpuAlertCallbacks_) {
                callback(alert);
            }
        }
    }
}

std::vector<CoreUsageStats> CPUMonitor::getCoreUsageStats() const {
    std::lock_guard<std::mutex> lock(coreStatsMutex_);
    return coreStats_;
}

std::optional<CoreUsageStats> CPUMonitor::getCoreUsage(size_t coreId) const {
    std::lock_guard<std::mutex> lock(coreStatsMutex_);
    
    for (const auto& stats : coreStats_) {
        if (stats.coreId == coreId) {
            return stats;
        }
    }
    
    return std::nullopt;
}

double CPUMonitor::getOverallCPUUsage() const {
    std::lock_guard<std::mutex> lock(coreStatsMutex_);
    
    if (coreStats_.empty()) {
        return 0.0;
    }
    
    double totalUsage = 0.0;
    for (const auto& stats : coreStats_) {
        totalUsage += stats.usagePercentage;
    }
    
    return totalUsage / coreStats_.size();
}

size_t CPUMonitor::getCoreCount() const {
    // Get number of CPU cores from system
    long numCores = sysconf(_SC_NPROCESSORS_ONLN);
    return static_cast<size_t>(numCores > 0 ? numCores : 1);
}

double CPUMonitor::getCurrentCPUFrequency() const {
    // Simplified frequency reading - in reality this would read from /proc/cpuinfo or similar
    return 2500.0; // MHz
}

std::vector<InstructionStats> CPUMonitor::getInstructionStats(const std::string& instructionSet) const {
    std::lock_guard<std::mutex> lock(instructionMutex_);
    
    std::vector<InstructionStats> stats;
    
    if (instructionSet.empty()) {
        return instructionStats_;
    }
    
    for (const auto& stat : instructionStats_) {
        if (stat.instructionSet == instructionSet) {
            stats.push_back(stat);
        }
    }
    
    return stats;
}

std::vector<CacheStats> CPUMonitor::getCacheStats(const std::string& level) const {
    std::lock_guard<std::mutex> lock(cacheMutex_);
    
    std::vector<CacheStats> stats;
    
    if (level.empty()) {
        return cacheStats_;
    }
    
    for (const auto& stat : cacheStats_) {
        if (stat.cacheLevel == level) {
            stats.push_back(stat);
        }
    }
    
    return stats;
}

BranchStats CPUMonitor::getBranchStats() const {
    std::lock_guard<std::mutex> lock(branchMutex_);
    return branchStats_;
}

std::vector<CPUAlert> CPUMonitor::getRecentAlerts(size_t count) const {
    std::lock_guard<std::mutex> lock(cpuAlertMutex_);
    
    std::vector<CPUAlert> alerts;
    size_t actualCount = std::min(count, cpuAlerts_.size());
    
    if (actualCount == 0) {
        return alerts;
    }
    
    size_t startIndex = cpuAlerts_.size() - actualCount;
    alerts.reserve(actualCount);
    
    for (size_t i = startIndex; i < cpuAlerts_.size(); ++i) {
        alerts.push_back(cpuAlerts_[i]);
    }
    
    return alerts;
}

void CPUMonitor::addCoreUsageCallback(const CoreUsageCallback& callback) {
    std::lock_guard<std::mutex> lock(coreStatsMutex_);
    coreUsageCallbacks_.push_back(callback);
}

void CPUMonitor::addCPUAlertCallback(const CPUAlertCallback& callback) {
    std::lock_guard<std::mutex> lock(cpuAlertMutex_);
    cpuAlertCallbacks_.push_back(callback);
}

void CPUMonitor::addInstructionCallback(const InstructionCallback& callback) {
    std::lock_guard<std::mutex> lock(instructionMutex_);
    instructionCallbacks_.push_back(callback);
}

std::vector<std::string> CPUMonitor::getCPUTuningSuggestions() const {
    std::vector<std::string> suggestions;
    
    // Check for load imbalance
    std::lock_guard<std::mutex> lock(coreStatsMutex_);
    
    if (coreStats_.size() > 1) {
        double minUsage = coreStats_[0].usagePercentage;
        double maxUsage = coreStats_[0].usagePercentage;
        
        for (const auto& stats : coreStats_) {
            minUsage = std::min(minUsage, stats.usagePercentage);
            maxUsage = std::max(maxUsage, stats.usagePercentage);
        }
        
        double loadImbalance = (maxUsage - minUsage) / 100.0;
        
        if (loadImbalance > 0.3) {
            suggestions.push_back("High CPU load imbalance detected. Consider workload redistribution.");
        }
    }
    
    // Check for thermal issues
    bool thermalWarning = false;
    for (const auto& stats : coreStats_) {
        if (stats.usagePercentage > 85.0) {
            thermalWarning = true;
            break;
        }
    }
    
    if (thermalWarning) {
        suggestions.push_back("High CPU usage detected. Consider reducing workload or increasing cooling.");
    }
    
    // Check for inefficient thread usage
    for (const auto& stats : coreStats_) {
        if (stats.threadCount > 4 && stats.efficiency < 0.5) {
            suggestions.push_back("Core " + std::to_string(stats.coreId) + " has too many threads for its workload. Consider reducing threads.");
        }
    }
    
    return suggestions;
}

CPUMonitor::CPUPerformanceStats CPUMonitor::getCPUPerformanceStats(
    std::chrono::high_resolution_clock::duration timeWindow) const {
    CPUPerformanceStats stats;
    
    std::lock_guard<std::mutex> lock(coreStatsMutex_);
    
    if (coreStats_.empty()) {
        return stats;
    }
    
    // Calculate average usage
    double totalUsage = 0.0;
    double maxUsage = 0.0;
    size_t count = 0;
    
    auto now = std::chrono::high_resolution_clock::now();
    for (const auto& coreStats : coreStats_) {
        if (now - coreStats.timestamp < timeWindow) {
            totalUsage += coreStats.usagePercentage;
            maxUsage = std::max(maxUsage, coreStats.usagePercentage);
            count++;
        }
    }
    
    if (count > 0) {
        stats.averageUsage = totalUsage / count;
        stats.peakUsage = maxUsage;
        
        // Calculate stddev
        double sum = 0.0;
        for (const auto& coreStats : coreStats_) {
            if (now - coreStats.timestamp < timeWindow) {
                double diff = coreStats.usagePercentage - stats.averageUsage;
                sum += diff * diff;
            }
        }
        
        stats.stddevUsage = std::sqrt(sum / count);
        
        // Calculate imbalance score
        size_t imbalanceScore = 0;
        if (count > 1) {
            double variance = stats.stddevUsage;
            imbalanceScore = static_cast<size_t>(variance * 100); // Scale to 0-100
        }
        stats.imbalanceScore = imbalanceScore;
    }
    
    // These would be calculated from actual system data
    stats.totalActiveTime = std::chrono::seconds(3600); // 1 hour placeholder
    stats.totalIdleTime = std::chrono::seconds(86400) - stats.totalActiveTime; // 24 hours placeholder
    
    return stats;
}

void CPUMonitor::analyzePerformanceBottlenecks() {
    std::lock_guard<std::mutex> lock(coreStatsMutex_);
    
    // Check for single-core bottlenecks
    size_t singleCoreCount = 0;
    for (const auto& stats : coreStats_) {
        if (stats.usagePercentage > 80.0) {
            singleCoreCount++;
        }
    }
    
    if (singleCoreCount > coreStats_.size() / 2) {
        CPUAlert alert;
        alert.type = CPUAlert::AlertType::HIGH_USAGE;
        alert.description = "Many cores are heavily loaded. Consider parallelization.";
        alert.severity = 0.7;
        alert.timestamp = std::chrono::high_resolution_clock::now();
        
        cpuAlerts_.push_back(alert);
        
        for (const auto& callback : cpuAlertCallbacks_) {
            callback(alert);
        }
    }
}

void CPUMonitor::updateInstructionStats() {
    std::lock_guard<std::mutex> lock(instructionMutex_);
    
    // Simulate instruction statistics
    InstructionStats stats;
    stats.instructionSet = "AVX-512";
    stats.instructionsExecuted = 1000000000 + std::rand() % 100000000;
    stats.cycles = stats.instructionsExecuted / 4; // Assume 4 instructions per cycle
    stats.instructionsPerCycle = stats.instructionsExecuted / static_cast<double>(stats.cycles);
    stats.utilization = 85.0 + std::rand() % 15; // 85-100%
    stats.timestamp = std::chrono::high_resolution_clock::now();
    
    instructionStats_.push_back(stats);
    
    // Maintain history
    while (instructionStats_.size() > 1000) {
        instructionStats_.erase(instructionStats_.begin());
    }
    
    // Call callbacks
    for (const auto& callback : instructionCallbacks_) {
        callback(stats);
    }
}

void CPUMonitor::updateCacheStats() {
    std::lock_guard<std::mutex> lock(cacheMutex_);
    
    // Simulate cache statistics
    std::vector<std::string> cacheLevels = {"L1", "L2", "L3"};
    
    for (const auto& level : cacheLevels) {
        CacheStats stats;
        stats.cacheLevel = level;
        stats.size = (level == "L1") ? 32 * 1024 : ((level == "L2") ? 256 * 1024 : 32 * 1024 * 1024);
        stats.lines = stats.size / 64; // Assuming 64-byte cache lines
        stats.lineSize = 64;
        stats.hitRate = 0.7 + std::rand() % 301 / 1000.0; // 70-100%
        stats.accesses = 1000000 + std::rand() % 500000;
        stats.misses = stats.accesses * (1.0 - stats.hitRate);
        stats.timestamp = std::chrono::high_resolution_clock::now();
        
        cacheStats_.push_back(stats);
    }
    
    // Maintain history
    while (cacheStats_.size() > 1000) {
        cacheStats_.erase(cacheStats_.begin());
    }
}

void CPUMonitor::updateBranchStats() {
    std::lock_guard<std::mutex> lock(branchMutex_);
    
    BranchStats& stats = branchStats_;
    stats.totalBranches = 1000000 + std::rand() % 500000;
    stats.mispredictedBranches = stats.totalBranches * (0.05 + std::rand() % 101 / 1000.0); // 5-15% misprediction
    stats.mispredictionRate = stats.mispredictedBranches / static_cast<double>(stats.totalBranches);
    stats.timestamp = std::chrono::high_resolution_clock::now();
}

void CPUMonitor::checkCPUThresholds() {
    std::lock_guard<std::mutex> lock(coreStatsMutex_);
    
    // Check for thermal warnings
    for (const auto& stats : coreStats_) {
        if (stats.usagePercentage > 90.0) {
            CPUAlert alert;
            alert.type = CPUAlert::AlertType::HIGH_USAGE;
            alert.description = "Core " + std::to_string(stats.coreId) + " usage is high: " + 
                               std::to_string(stats.usagePercentage) + "%";
            alert.severity = 0.8;
            alert.timestamp = std::chrono::high_resolution_clock::now();
            alert.details["core_id"] = std::to_string(stats.coreId);
            alert.details["usage_percentage"] = std::to_string(stats.usagePercentage);
            
            cpuAlerts_.push_back(alert);
            
            for (const auto& callback : cpuAlertCallbacks_) {
                callback(alert);
            }
        }
    }
}

std::vector<CoreUsageStats> CPUMonitor::getCoreUsageStats() const {
    std::lock_guard<std::mutex> lock(coreStatsMutex_);
    return coreStats_;
}

std::optional<CoreUsageStats> CPUMonitor::getCoreUsage(size_t coreId) const {
    std::lock_guard<std::mutex> lock(coreStatsMutex_);
    
    for (const auto& stats : coreStats_) {
        if (stats.coreId == coreId) {
            return stats;
        }
    }
    
    return std::nullopt;
}

double CPUMonitor::getOverallCPUUsage() const {
    std::lock_guard<std::mutex> lock(coreStatsMutex_);
    
    if (coreStats_.empty()) {
        return 0.0;
    }
    
    double totalUsage = 0.0;
    for (const auto& stats : coreStats_) {
        totalUsage += stats.usagePercentage;
    }
    
    return totalUsage / coreStats_.size();
}

size_t CPUMonitor::getCoreCount() const {
    // Get number of CPU cores from system
    long numCores = sysconf(_SC_NPROCESSORS_ONLN);
    return static_cast<size_t>(numCores > 0 ? numCores : 1);
}

double CPUMonitor::getCurrentCPUFrequency() const {
    // Simplified frequency reading - in reality this would read from /proc/cpuinfo or similar
    return 2500.0; // MHz
}

std::vector<InstructionStats> CPUMonitor::getInstructionStats(const std::string& instructionSet) const {
    std::lock_guard<std::mutex> lock(instructionMutex_);
    
    std::vector<InstructionStats> stats;
    
    if (instructionSet.empty()) {
        return instructionStats_;
    }
    
    for (const auto& stat : instructionStats_) {
        if (stat.instructionSet == instructionSet) {
            stats.push_back(stat);
        }
    }
    
    return stats;
}

std::vector<CacheStats> CPUMonitor::getCacheStats(const std::string& level) const {
    std::lock_guard<std::mutex> lock(cacheMutex_);
    
    std::vector<CacheStats> stats;
    
    if (level.empty()) {
        return cacheStats_;
    }
    
    for (const auto& stat : cacheStats_) {
        if (stat.cacheLevel == level) {
            stats.push_back(stat);
        }
    }
    
    return stats;
}

BranchStats CPUMonitor::getBranchStats() const {
    std::lock_guard<std::mutex> lock(branchMutex_);
    return branchStats_;
}

std::vector<CPUAlert> CPUMonitor::getRecentAlerts(size_t count) const {
    std::lock_guard<std::mutex> lock(cpuAlertMutex_);
    
    std::vector<CPUAlert> alerts;
    size_t actualCount = std::min(count, cpuAlerts_.size());
    
    if (actualCount == 0) {
        return alerts;
    }
    
    size_t startIndex = cpuAlerts_.size() - actualCount;
    alerts.reserve(actualCount);
    
    for (size_t i = startIndex; i < cpuAlerts_.size(); ++i) {
        alerts.push_back(cpuAlerts_[i]);
    }
    
    return alerts;
}

void CPUMonitor::addCoreUsageCallback(const CoreUsageCallback& callback) {
    std::lock_guard<std::mutex> lock(coreStatsMutex_);
    coreUsageCallbacks_.push_back(callback);
}

void CPUMonitor::addCPUAlertCallback(const CPUAlertCallback& callback) {
    std::lock_guard<std::mutex> lock(cpuAlertMutex_);
    cpuAlertCallbacks_.push_back(callback);
}

void CPUMonitor::addInstructionCallback(const InstructionCallback& callback) {
    std::lock_guard<std::mutex> lock(instructionMutex_);
    instructionCallbacks_.push_back(callback);
}

std::vector<std::string> CPUMonitor::getCPUTuningSuggestions() const {
    std::vector<std::string> suggestions;
    
    // Check for load imbalance
    std::lock_guard<std::mutex> lock(coreStatsMutex_);
    
    if (coreStats_.size() > 1) {
        double minUsage = coreStats_[0].usagePercentage;
        double maxUsage = coreStats_[0].usagePercentage;
        
        for (const auto& stats : coreStats_) {
            minUsage = std::min(minUsage, stats.usagePercentage);
            maxUsage = std::max(maxUsage, stats.usagePercentage);
        }
        
        double loadImbalance = (maxUsage - minUsage) / 100.0;
        
        if (loadImbalance > 0.3) {
            suggestions.push_back("High CPU load imbalance detected. Consider workload redistribution.");
        }
    }
    
    // Check for thermal issues
    bool thermalWarning = false;
    for (const auto& stats : coreStats_) {
        if (stats.usagePercentage > 85.0) {
            thermalWarning = true;
            break;
        }
    }
    
    if (thermalWarning) {
        suggestions.push_back("High CPU usage detected. Consider reducing workload or increasing cooling.");
    }
    
    // Check for inefficient thread usage
    for (const auto& stats : coreStats_) {
        if (stats.threadCount > 4 && stats.efficiency < 0.5) {
            suggestions.push_back("Core " + std::to_string(stats.coreId) + " has too many threads for its workload. Consider reducing threads.");
        }
    }
    
    return suggestions;
}

CPUMonitor::CPUPerformanceStats CPUMonitor::getCPUPerformanceStats(
    std::chrono::high_resolution_clock::duration timeWindow) const {
    CPUPerformanceStats stats;
    
    std::lock_guard<std::mutex> lock(coreStatsMutex_);
    
    if (coreStats_.empty()) {
        return stats;
    }
    
    // Calculate average usage
    double totalUsage = 0.0;
    double maxUsage = 0.0;
    size_t count = 0;
    
    auto now = std::chrono::high_resolution_clock::now();
    for (const auto& coreStats : coreStats_) {
        if (now - coreStats.timestamp < timeWindow) {
            totalUsage += coreStats.usagePercentage;
            maxUsage = std::max(maxUsage, coreStats.usagePercentage);
            count++;
        }
    }
    
    if (count > 0) {
        stats.averageUsage = totalUsage / count;
        stats.peakUsage = maxUsage;
        
        // Calculate stddev
        double sum = 0.0;
        for (const auto& coreStats : coreStats_) {
            if (now - coreStats.timestamp < timeWindow) {
                double diff = coreStats.usagePercentage - stats.averageUsage;
                sum += diff * diff;
            }
        }
        
        stats.stddevUsage = std::sqrt(sum / count);
        
        // Calculate imbalance score
        size_t imbalanceScore = 0;
        if (count > 1) {
            double variance = stats.stddevUsage;
            imbalanceScore = static_cast<size_t>(variance * 100); // Scale to 0-100
        }
        stats.imbalanceScore = imbalanceScore;
    }
    
    // These would be calculated from actual system data
    stats.totalActiveTime = std::chrono::seconds(3600); // 1 hour placeholder
    stats.totalIdleTime = std::chrono::seconds(86400) - stats.totalActiveTime; // 24 hours placeholder
    
    return stats;
}

void CPUMonitor::analyzePerformanceBottlenecks() {
    std::lock_guard<std::mutex> lock(coreStatsMutex_);
    
    // Check for single-core bottlenecks
    size_t singleCoreCount = 0;
    for (const auto& stats : coreStats_) {
        if (stats.usagePercentage > 80.0) {
            singleCoreCount++;
        }
    }
    
    if (singleCoreCount > coreStats_.size() / 2) {
        CPUAlert alert;
        alert.type = CPUAlert::AlertType::HIGH_USAGE;
        alert.description = "Many cores are heavily loaded. Consider parallelization.";
        alert.severity = 0.7;
        alert.timestamp = std::chrono::high_resolution_clock::now();
        
        cpuAlerts_.push_back(alert);
        
        for (const auto& callback : cpuAlertCallbacks_) {
            callback(alert);
        }
    }
}