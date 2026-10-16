#include "PerformanceMonitor/MemoryProfiler.hpp"
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

// Helper functions for MemoryProfiler
namespace {
    std::string formatBytes(size_t bytes) {
        if (bytes < 1024) return std::to_string(bytes) + " B";
        if (bytes < 1024 * 1024) return std::to_string(bytes / 1024) + " KB";
        if (bytes < 1024 * 1024 * 1024) return std::to_string(bytes / (1024 * 1024)) + " MB";
        return std::to_string(bytes / (1024 * 1024 * 1024)) + " GB";
    }
    
    std::string formatPercentage(double value) {
        return std::to_string(static_cast<int>(value)) + "%";
    }
}

MemoryProfiler::MemoryProfiler() : pImpl(std::make_unique<Impl>()) {
    // Start memory tracking if configured
    if (trackMemoryAllocations_) {
        startTracking();
    }
}

MemoryProfiler::~MemoryProfiler() {
    shutdown();
}

void MemoryProfiler::initialize() {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    if (initialized_) {
        return;
    }
    
    // Initialize memory tracking
    initialized_ = true;
    
    // Start tracking if configured
    if (trackMemoryAllocations_) {
        startTracking();
    }
    
    // Log initialization
    if (logger_) {
        logger_->info("Memory profiler initialized");
    }
}

void MemoryProfiler::shutdown() {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    if (!initialized_) {
        return;
    }
    
    // Stop tracking
    stopTracking();
    
    // Clear memory tracking data
    currentAllocations_.clear();
    allocationHistory_.clear();
    
    // Clear integration objects
    logger_.reset();
    statisticsCollector_.reset();
    realTimeMonitor_.reset();
    
    // Reset flags
    initialized_ = false;
    tracking_ = false;
    
    // Log shutdown
    if (logger_) {
        logger_->info("Memory profiler shutdown");
    }
}

void MemoryProfiler::trackAllocation(uint64_t address, size_t size, const std::string& type,
                                      const std::string& stackTrace) {
    trackAllocationWithCallback(address, size, type, stackTrace);
}

void MemoryProfiler::trackDeallocation(uint64_t address) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    if (!tracking_) {
        return;
    }
    
    auto it = currentAllocations_.find(address);
    if (it != currentAllocations_.end()) {
        // Call deallocation callback
        if (deallocationCallback_) {
            deallocationCallback_(address);
        }
        
        // Remove from current allocations
        currentAllocations_.erase(it);
    }
    
    // Log deallocation
    if (logger_) {
        logger_->debug("Memory deallocated: " + std::to_string(address));
    }
}

void MemoryProfiler::trackAllocationWithCallback(uint64_t address, size_t size,
                                                  const std::string& type,
                                                  const std::string& stackTrace) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    if (!tracking_) {
        return;
    }
    
    // Create allocation info
    MemoryAllocationInfo allocation;
    allocation.address = address;
    allocation.size = size;
    allocation.allocationType = type;
    allocation.allocatedTime = std::chrono::system_clock::now();
    allocation.isAllocated = true;
    allocation.threadId = std::hash<std::thread::id>{}(std::this_thread::get_id());
    allocation.allocationStack = stackTrace;
    
    // Record allocation
    recordAllocationInternal(allocation);
    
    // Call allocation callback
    if (allocationCallback_) {
        allocationCallback_(allocation);
    }
    
    // Log allocation
    if (logger_) {
        logger_->debug("Memory allocated: " + std::to_string(address) +
                      " size: " + formatBytes(size) +
                      " type: " + type);
    }
}

void MemoryProfiler::startTracking() {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    if (tracking_) {
        return;
    }
    
    tracking_ = true;
    
    // Start memory tracking thread
    memoryTrackingThread_ = std::thread(&MemoryProfiler::trackMemoryAllocations, this);
    
    // Log start
    if (logger_) {
        logger_->info("Memory profiler tracking started");
    }
}

void MemoryProfiler::stopTracking() {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    if (!tracking_) {
        return;
    }
    
    tracking_ = false;
    
    // Wait for memory tracking thread to finish
    if (memoryTrackingThread_.joinable()) {
        memoryTrackingThread_.join();
    }
    
    // Analyze memory usage before stopping
    if (detectLeaks_) {
        analyzeMemoryUsage();
    }
    
    // Generate memory report
    if (logger_) {
        logger_->debug(getMemoryUsageSummary());
    }
    
    // Log stop
    if (logger_) {
        logger_->info("Memory profiler tracking stopped");
    }
}

void MemoryProfiler::trackMemoryAllocations() {
    while (tracking_) {
        std::this_thread::sleep_for(std::chrono::duration<double>(1.0));
        
        // Check for memory leaks
        if (detectLeaks_) {
            checkForMemoryLeaks();
        }
        
        // Generate memory analysis periodically
        if (std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now() - lastAnalysisTime_).count() > 60) {
            generateMemoryAnalysis();
            lastAnalysisTime_ = std::chrono::system_clock::now();
        }
    }
}

void MemoryProfiler::cleanupMemoryHistory() {
    // Remove old allocations from history
    auto it = allocationHistory_.begin();
    while (it != allocationHistory_.end()) {
        if (!it->isAllocated && std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now() - it->freedTime).count() > 3600) { // 1 hour
            it = allocationHistory_.erase(it);
        } else {
            ++it;
        }
    }
    
    // Limit history size
    while (allocationHistory_.size() > allocationHistoryLimit_) {
        allocationHistory_.pop_front();
    }
}

void MemoryProfiler::checkForMemoryLeaks() {
    std::vector<MemoryAllocationInfo> leaks;
    
    // Find current allocations that might be leaks
    for (const auto& pair : currentAllocations_) {
        const auto& allocation = pair.second;
        
        // Check if allocation has been alive for a long time
        auto age = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now() - allocation.allocatedTime).count();
        
        if (age > 3600) { // 1 hour
            leaks.push_back(allocation);
        }
    }
    
    // If leaks detected, call leak callback
    if (!leaks.empty() && leakCallback_) {
        leakCallback_(leaks);
    }
}

void MemoryProfiler::generateMemoryAnalysis() {
    std::ostringstream oss;
    oss << "Memory Analysis Report\n";
    oss << "======================\n\n";
    
    // Analyze allocation patterns
    oss << "Allocation Patterns:\n";
    auto countsByType = getAllocationCountsByType();
    for (const auto& pair : countsByType) {
        oss << "  " << pair.first << ": " << pair.second << " allocations\n";
    }
    
    // Analyze deallocation patterns
    oss << "\nDeallocation Patterns:\n";
    auto deallocCountsByType = getDeallocationCountsByType();
    for (const auto& pair : deallocCountsByType) {
        oss << "  " << pair.first << ": " << pair.second << " deallocations\n";
    }
    
    // Analyze memory leaks
    auto leaks = detectMemoryLeaks();
    oss << "\nMemory Leaks:\n";
    oss << "  Detected: " << leaks.size() << " potential leaks\n";
    oss << "  Total leaked memory: " << calculateMemoryLeakSize() << " bytes\n";
    
    // Generate optimization suggestions
    oss << "\nOptimization Suggestions:\n";
    auto suggestions = generateMemoryOptimizationSuggestions();
    for (const auto& suggestion : suggestions) {
        oss << "  - " << suggestion << "\n";
    }
    
    // Write to file if configured
    if (logger_) {
        logger_->info(oss.str());
    }
}

void MemoryProfiler::recordAllocationInternal(const MemoryAllocationInfo& allocation) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    // Record allocation
    currentAllocations_[allocation.address] = allocation;
    
    // Add to history
    allocationHistory_.push_back(allocation);
    
    // Keep history size manageable
    while (allocationHistory_.size() > allocationHistoryLimit_) {
        allocationHistory_.pop_front();
    }
}

void MemoryProfiler::recordDeallocationInternal(uint64_t address) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    auto it = currentAllocations_.find(address);
    if (it != currentAllocations_.end()) {
        it->second.isAllocated = false;
        it->second.freedTime = std::chrono::system_clock::now();
        
        // Add to history
        allocationHistory_.push_back(it->second);
        
        // Keep history size manageable
        while (allocationHistory_.size() > allocationHistoryLimit_) {
            allocationHistory_.pop_front();
        }
    }
}

void MemoryProfiler::integrateWithLogger(std::shared_ptr<Logger> logger) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    logger_ = logger;
    
    if (logger_) {
        logger_->info("Memory profiler integrated with Logger");
    }
}

void MemoryProfiler::integrateWithStatisticsCollector(std::shared_ptr<StatisticsCollector> collector) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    statisticsCollector_ = collector;
    
    if (statisticsCollector_) {
        if (logger_) {
            logger_->info("Memory profiler integrated with StatisticsCollector");
        }
    }
}

void MemoryProfiler::integrateWithRealTimeMonitor(std::shared_ptr<RealTimeMonitor> monitor) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    realTimeMonitor_ = monitor;
    
    if (realTimeMonitor_) {
        if (logger_) {
            logger_->info("Memory profiler integrated with RealTimeMonitor");
        }
    }
}

void MemoryProfiler::setupMemoryProfilingForMode(const std::string& mode) {
    setupMemoryProfilingForMode(mode, 0);
}

void MemoryProfiler::setupMemoryProfilingForMode(const std::string& mode, size_t simulationSize) {
    // Setup memory profiling for specific simulation mode
    startTracking();
    
    // Configure based on mode
    if (mode == "benchmark" || mode == "scaling_benchmark") {
        trackStackTraces_ = true;
    }
    
    // Record mode-specific metrics
    if (logger_) {
        logger_->info("Memory profiler setup for mode: " + mode);
    }
}

void MemoryProfiler::profileMemoryAllocation(const std::string& operation, size_t size) {
    uint64_t address = 0x1000 + currentAllocations_.size(); // Mock address
    trackAllocation(address, size, operation);
}

void MemoryProfiler::profileMemoryDeallocation(const std::string& operation, size_t size) {
    // Mock deallocation (would track actual address)
    if (!currentAllocations_.empty()) {
        trackDeallocation(0x1000 + currentAllocations_.size() - 1);
    }
}

void MemoryProfiler::profileMemoryUsage(const std::string& operation, uint64_t memoryUsage) {
    if (logger_) {
        logger_->debug("Memory usage in " + operation + ": " + formatBytes(memoryUsage));
    }
}

void MemoryProfiler::analyzeMemoryUsage() {
    std::ostringstream oss;
    oss << "Memory Usage Analysis\n";
    oss << "=====================\n\n";
    
    oss << "Total Allocations: " << currentAllocations_.size() << "\n";
    oss << "Total Deallocations: " << allocationHistory_.size() - currentAllocations_.size() << "\n";
    oss << "Current Memory Usage: " << getCurrentMemoryUsage() << " bytes\n";
    oss << "Peak Memory Usage: " << getPeakMemoryUsage() << " bytes\n";
    
    if (logger_) {
        logger_->info(oss.str());
    }
}

void MemoryProfiler::generateMemoryReport() {
    std::string report = getMemoryUsageSummary() + "\n" + getMemoryLeakReport();
    
    if (logger_) {
        logger_->info(report);
    }
}

void MemoryProfiler::exportMemoryData(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return;
    }
    
    file << "Memory Allocation Report\n";
    file << "======================\n\n";
    
    file << "Address,Size,Type,AllocatedTime,FreedTime,IsAllocated\n";
    for (const auto& allocation : allocationHistory_) {
        file << allocation.address << "," << allocation.size << "," << allocation.allocationType << ",";
        auto allocatedTime = std::chrono::system_clock::to_time_t(allocation.allocatedTime);
        file << std::put_time(std::localtime(&allocatedTime), "%Y-%m-%d %H:%M:%S") << ",";
        
        if (!allocation.isAllocated && allocation.freedTime.time_since_epoch().count() > 0) {
            auto freedTime = std::chrono::system_clock::to_time_t(allocation.freedTime);
            file << std::put_time(std::localtime(&freedTime), "%Y-%m-%d %H:%M:%S");
        }
        file << "," << (allocation.isAllocated ? "true" : "false") << "\n";
    }
    
    file.close();
}

std::vector<MemoryAllocationInfo> MemoryProfiler::detectMemoryLeaks() const {
    std::vector<MemoryAllocationInfo> leaks;
    
    for (const auto& pair : currentAllocations_) {
        const auto& allocation = pair.second;
        
        // Check if allocation has been alive for a long time
        auto age = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now() - allocation.allocatedTime).count();
        
        if (age > 3600) { // 1 hour
            leaks.push_back(allocation);
        }
    }
    
    return leaks;
}

void MemoryProfiler::analyzeAllocationPatterns(std::map<std::string, size_t>& countsByType) const {
    countsByType.clear();
    
    for (const auto& allocation : currentAllocations_) {
        countsByType[allocation.allocationType]++;
    }
}

void MemoryProfiler::analyzeDeallocationPatterns(std::map<std::string, size_t>& countsByType) const {
    countsByType.clear();
    
    for (const auto& allocation : allocationHistory_) {
        if (!allocation.isAllocated) {
            countsByType[allocation.allocationType]++;
        }
    }
}

void MemoryProfiler::analyzeSizeDistribution(std::map<std::string, size_t>& sizeByType) const {
    sizeByType.clear();
    
    for (const auto& allocation : currentAllocations_) {
        // Group by size ranges
        size_t sizeRange;
        if (allocation.size < 1024) sizeRange = 0;
        else if (allocation.size < 1024 * 1024) sizeRange = 1;
        else if (allocation.size < 1024 * 1024 * 1024) sizeRange = 2;
        else sizeRange = 3;
        
        std::string rangeKey = (sizeRange == 0) ? "< 1KB" :
                              (sizeRange == 1) ? "< 1MB" :
                              (sizeRange == 2) ? "< 1GB" : "≥ 1GB";
        sizeByType[rangeKey] += allocation.size;
    }
}

void MemoryProfiler::analyzeStackTraces(std::map<std::string, size_t>& functionsByCount) const {
    functionsByCount.clear();
    
    // This would extract function names from stack traces
    // For now, return empty map
}

void MemoryProfiler::suggestPoolAllocators(std::vector<std::string>& suggestions) const {
    suggestions.push_back("Consider using memory pools for frequently allocated objects");
    suggestions.push_back("Implement object pooling for temporary allocations");
    suggestions.push_back("Use arena allocation for simulation objects");
}

void MemoryProfiler::suggestMemoryReuse(std::vector<std::string>& suggestions) const {
    suggestions.push_back("Reuse objects instead of creating/destroying");
    suggestions.push_back("Implement object pooling for reusable resources");
    suggestions.push_back("Use smart pointers for automatic memory management");
}

void MemoryProfiler::suggestAllocationSizeOptimization(std::vector<std::string>& suggestions) const {
    suggestions.push_back("Use smaller data types where possible");
    suggestions.push_back("Align memory allocations to cache lines");
    suggestions.push_back("Use structure-of-arrays (SoA) for better cache locality");
}

void MemoryProfiler::suggestLifetimeOptimization(std::vector<std::string>& suggestions) const {
    suggestions.push_back("Extend object lifetime to reduce allocations");
    suggestions.push_back("Reuse buffers instead of reallocating");
    suggestions.push_back("Implement object pooling for temporary data");
}

size_t MemoryProfiler::getTotalAllocatedMemory() const {
    size_t total = 0;
    for (const auto& allocation : currentAllocations_) {
        total += allocation.size;
    }
    return total;
}

size_t MemoryProfiler::getTotalDeallocatedMemory() const {
    size_t total = 0;
    for (const auto& allocation : allocationHistory_) {
        if (!allocation.isAllocated && allocation.freedTime.time_since_epoch().count() > 0) {
            total += allocation.size;
        }
    }
    return total;
}

size_t MemoryProfiler::getCurrentMemoryUsage() const {
    return getTotalAllocatedMemory();
}

size_t MemoryProfiler::getPeakMemoryUsage() const {
    // Calculate peak memory usage from history
    size_t peak = 0;
    size_t current = 0;
    
    for (const auto& allocation : allocationHistory_) {
        if (allocation.isAllocated) {
            current += allocation.size;
        } else {
            current -= allocation.size;
        }
        
        peak = std::max(peak, current);
    }
    
    return peak;
}

std::string MemoryProfiler::getMemoryUsageSummary() const {
    std::ostringstream oss;
    oss << "Memory Usage Summary\n";
    oss << "====================\n\n";
    oss << "Total Allocations: " << currentAllocations_.size() << "\n";
    oss << "Total Deallocations: " << allocationHistory_.size() - currentAllocations_.size() << "\n";
    oss << "Current Memory Usage: " << formatBytes(getCurrentMemoryUsage()) << "\n";
    oss << "Peak Memory Usage: " << formatBytes(getPeakMemoryUsage()) << "\n";
    
    return oss.str();
}

std::string MemoryProfiler::getMemoryLeakReport() const {
    std::ostringstream oss;
    oss << "Memory Leak Report\n";
    oss << "==================\n\n";
    
    auto leaks = detectMemoryLeaks();
    oss << "Memory Leaks Detected: " << leaks.size() << "\n";
    oss << "Total Leaked Memory: " << formatBytes(calculateMemoryLeakSize()) << "\n\n";
    
    if (!leaks.empty()) {
        oss << "Top Leaked Allocations:\n";
        for (size_t i = 0; i < std::min<size_t>(5, leaks.size()); ++i) {
            const auto& leak = leaks[i];
            oss << "  " << (i + 1) << ". Address: " << leak.address << "\n";
            oss << "     Size: " << formatBytes(leak.size) << "\n";
            oss << "     Type: " << leak.allocationType << "\n";
            oss << "     Age: " << std::chrono::duration_cast<std::chrono::hours>(
                std::chrono::system_clock::now() - leak.allocatedTime).count() << " hours\n";
        }
    }
    
    return oss.str();
}

size_t MemoryProfiler::calculateMemoryLeakSize() const {
    size_t total = 0;
    auto leaks = detectMemoryLeaks();
    
    for (const auto& leak : leaks) {
        total += leak.size;
    }
    
    return total;
}

bool MemoryProfiler::hasMemoryLeaks() const {
    return detectMemoryLeaks().size() > 0;
}

std::vector<std::string> MemoryProfiler::generateMemoryOptimizationSuggestions() const {
    std::vector<std::string> suggestions;
    
    suggestPoolAllocators(suggestions);
    suggestMemoryReuse(suggestions);
    suggestAllocationSizeOptimization(suggestions);
    suggestLifetimeOptimization(suggestions);
    
    // Remove duplicates
    std::sort(suggestions.begin(), suggestions.end());
    suggestions.erase(std::unique(suggestions.begin(), suggestions.end()), suggestions.end());
    
    return suggestions;
}

} // namespace nlm
