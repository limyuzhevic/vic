#pragma once

#include "MemoryMonitor.hpp"
#include "Profiler.hpp"
#include <sys/resource.h>
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <sstream>
#include <unordered_set>
#include <regex>

namespace nlm {

MemoryMonitor::MemoryMonitor()
    : enabled_(false)
    , samplingInterval_(0.1)  // 100ms default
    , maxMemoryUsage_{0}
    , peakMemoryUsage_{0}
    , totalAllocated_{0}
    , totalDeallocated_{0}
    , allocationCount_{0}
    , deallocationCount_{0}
    , leakCount_{0}
    , activeSampling_(false)
    , memoryPoolsEnabled_(false)
    , fragmentationLevel_(0.0)
    , poolStats_()
    , componentStats_() {
}

MemoryMonitor::~MemoryMonitor() {
    stopSampling();
}

void MemoryMonitor::start() {
    if (!enabled_) return;

    activeSampling_ = true;
    samplingThread_ = std::thread(&MemoryMonitor::samplingLoop, this);
}

void MemoryMonitor::stop() {
    activeSampling_ = false;
    if (samplingThread_.joinable()) {
        samplingThread_.join();
    }
}

void MemoryMonitor::setEnabled(bool enabled) {
    enabled_ = enabled;
    if (!enabled_) {
        stop();
    }
}

bool MemoryMonitor::isEnabled() const {
    return enabled_;
}

void MemoryMonitor::setSamplingInterval(double interval) {
    samplingInterval_ = interval;
}

void MemoryMonitor::setMaxMemoryUsage(size_t maxBytes) {
    maxMemoryUsage_ = maxBytes;
}

void MemoryMonitor::setMemoryPoolsEnabled(bool enabled) {
    memoryPoolsEnabled_ = enabled;
}

void MemoryMonitor::recordAllocation(size_t size, const std::string& component, const std::string& stackTrace) {
    std::lock_guard<std::mutex> lock(allocationsMutex_);

    AllocationRecord record;
    record.address = nullptr;  // Actual implementation would track real pointers
    record.size = size;
    record.allocationTime = std::chrono::high_resolution_clock::now();
    record.component = component;
    record.stackTrace = stackTrace;
    record.isFreed = false;

    allocations_.push_back(record);
    allocationCount_++;
    totalAllocated_ += size;

    // Update component statistics
    auto& compStats = componentStats_[component];
    compStats.allocations++;
    compStats.totalAllocated += size;

    // Check for potential leaks
    if (checkForLeaks()) {
        leakCount_++;
    }
}

void MemoryMonitor::recordDeallocation(void* address, const std::string& component) {
    std::lock_guard<std::mutex> lock(allocationsMutex_);

    // Find allocation and mark as freed
    for (auto& alloc : allocations_) {
        if (alloc.address == address && !alloc.isFreed) {
            alloc.isFreed = true;
            deallocationCount_++;
            totalDeallocated_ += alloc.size;

            // Update component statistics
            if (componentStats_.find(component) != componentStats_.end()) {
                auto& compStats = componentStats_[component];
                compStats.deallocations++;
                compStats.totalDeallocated += alloc.size;
            }
            break;
        }
    }
}

void MemoryMonitor::updatePoolStats(const std::string& poolName, size_t allocated, size_t capacity, double usage) {
    std::lock_guard<std::mutex> lock(poolsMutex_);

    PoolStats stats;
    stats.name = poolName;
    stats.currentlyAllocated = allocated;
    stats.capacity = capacity;
    stats.usagePercentage = capacity > 0 ? (usage * 100.0) : 0.0;
    stats.fragments = 0;  // Simplified

    poolStats_[poolName] = stats;

    // Check for fragmentation (simplified calculation)
    double totalUsage = 0.0;
    size_t totalCapacity = 0;
    for (const auto& [name, stats] : poolStats_) {
        totalUsage += stats.currentlyAllocated;
        totalCapacity += stats.capacity;
    }
    if (totalCapacity > 0) {
        double idealUsage = 1.0;  // Perfect usage
        double actualUsage = static_cast<double>(totalUsage) / totalCapacity;
        fragmentationLevel_ = std::abs(idealUsage - actualUsage);
    }
}

void MemoryMonitor::recordMemoryUsage(size_t memoryBytes, const std::string& component) {
    std::lock_guard<std::mutex> lock(usageMutex_);

    MemoryUsageRecord record;
    record.timestamp = std::chrono::high_resolution_clock::now();
    record.memoryBytes = memoryBytes;
    record.component = component;

    usageHistory_.push_back(record);
    peakMemoryUsage_ = std::max(peakMemoryUsage_, memoryBytes);

    // Clean up old records (keep last hour)
    auto cutoff = std::chrono::high_resolution_clock::now() -
                  std::chrono::hours(1);
    usageHistory_.erase(
        std::remove_if(usageHistory_.begin(), usageHistory_.end(),
            [cutoff](const MemoryUsageRecord& r) { return r.timestamp < cutoff; }),
        usageHistory_.end()
    );
}

void MemoryMonitor::setProfiler(PerformanceProfiler* profiler) {
    profiler_ = profiler;
}

size_t MemoryMonitor::getCurrentMemoryUsage() const {
    // Get current memory usage using system calls
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    return usage.ru_maxrss * 1024;  // Convert from KB to bytes (Linux)
}

bool MemoryMonitor::checkForLeaks() {
    // Simple leak detection: allocations without deallocations
    // This is a simplified implementation
    size_t totalAllocated = 0;
    size_t totalDeallocated = 0;

    for (const auto& alloc : allocations_) {
        if (alloc.isFreed) {
            totalDeallocated += alloc.size;
        } else {
            totalAllocated += alloc.size;
        }
    }

    return totalAllocated > totalDeallocated * 0.5;  // Heuristic: if allocated > 50% of deallocated, potential leak
}

double MemoryMonitor::getMemoryLeakRate() const {
    if (allocationCount_ == 0) return 0.0;
    return static_cast<double>(leakCount_) / allocationCount_;
}

double MemoryMonitor::getFragmentationLevel() const {
    return fragmentationLevel_;
}

size_t MemoryMonitor::getPeakMemoryUsage() const {
    return peakMemoryUsage_;
}

size_t MemoryMonitor::getTotalAllocated() const {
    return totalAllocated_;
}

size_t MemoryMonitor::getTotalDeallocated() const {
    return totalDeallocated_;
}

const std::map<std::string, MemoryMonitor::ComponentStats>& MemoryMonitor::getComponentStats() const {
    return componentStats_;
}

const std::map<std::string, MemoryMonitor::PoolStats>& MemoryMonitor::getPoolStats() const {
    return poolStats_;
}

std::string MemoryMonitor::generateReport() const {
    std::stringstream ss;
    generateReportInternal(ss);
    return ss.str();
}

void MemoryMonitor::exportToJSON(const std::string& filename) const {
    nlohmann::json jsonData;
    exportToJSONInternal(jsonData);

    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for writing: " + filename);
    }
    file << jsonData.dump(2);
}

void MemoryMonitor::exportToCSV(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for writing: " + filename);
    }
    exportToCSVInternal(file);
}

void MemoryMonitor::samplingLoop() {
    while (activeSampling_) {
        // Record current memory usage
        size_t currentUsage = getCurrentMemoryUsage();
        recordMemoryUsage(currentUsage, "system");

        std::this_thread::sleep_for(std::chrono::duration<double>(samplingInterval_));
    }
}

void MemoryMonitor::generateReportInternal(std::ostream& output) const {
    output << "=== Memory Monitoring Report ===\n";
    output << "Enabled: " << (enabled_ ? "Yes" : "No") << "\n";
    output << "Sampling Interval: " << samplingInterval_ << " seconds\n";
    output << "Peak Memory Usage: " << peakMemoryUsage_ << " bytes\n";
    output << "Total Allocated: " << totalAllocated_ << " bytes\n";
    output << "Total Deallocated: " << totalDeallocated_ << " bytes\n";
    output << "Allocation Count: " << allocationCount_ << "\n";
    output << "Deallocation Count: " << deallocationCount_ << "\n";
    output << "Leak Count: " << leakCount_ << "\n";
    output << "Memory Leak Rate: " << getMemoryLeakRate() * 100 << "%\n";
    output << "Fragmentation Level: " << getFragmentationLevel() * 100 << "%\n\n";

    output << "=== Component Statistics ===\n";
    for (const auto& [name, stats] : componentStats_) {
        output << "Component: " << name << "\n";
        output << "  Allocations: " << stats.allocations << "\n";
        output << "  Deallocations: " << stats.deallocations << "\n";
        output << "  Total Allocated: " << stats.totalAllocated << " bytes\n";
        output << "  Total Deallocated: " << stats.totalDeallocated << " bytes\n\n";
    }

    output << "=== Pool Statistics ===\n";
    for (const auto& [name, stats] : poolStats_) {
        output << "Pool: " << stats.name << "\n";
        output << "  Currently Allocated: " << stats.currentlyAllocated << "\n";
        output << "  Capacity: " << stats.capacity << "\n";
        output << "  Usage Percentage: " << stats.usagePercentage << "%\n";
        output << "  Fragments: " << stats.fragments << "\n\n";
    }
}

void MemoryMonitor::exportToJSONInternal(nlohmann::json& json) const {
    json["enabled"] = enabled_;
    json["sampling_interval"] = samplingInterval_;
    json["max_memory_usage"] = maxMemoryUsage_;
    json["peak_memory_usage"] = peakMemoryUsage_;
    json["total_allocated"] = totalAllocated_;
    json["total_deallocated"] = totalDeallocated_;
    json["allocation_count"] = allocationCount_;
    json["deallocation_count"] = deallocationCount_;
    json["leak_count"] = leakCount_;
    json["memory_leak_rate"] = getMemoryLeakRate();
    json["fragmentation_level"] = getFragmentationLevel();

    // Component stats
    json["component_stats"] = nlohmann::json::array();
    for (const auto& [name, stats] : componentStats_) {
        nlohmann::json compJson;
        compJson["name"] = name;
        compJson["allocations"] = stats.allocations;
        compJson["deallocations"] = stats.deallocations;
        compJson["total_allocated"] = stats.totalAllocated;
        compJson["total_deallocated"] = stats.totalDeallocated;
        json["component_stats"].push_back(compJson);
    }

    // Pool stats
    json["pool_stats"] = nlohmann::json::array();
    for (const auto& [name, stats] : poolStats_) {
        nlohmann::json poolJson;
        poolJson["name"] = stats.name;
        poolJson["currently_allocated"] = stats.currentlyAllocated;
        poolJson["capacity"] = stats.capacity;
        poolJson["usage_percentage"] = stats.usagePercentage;
        poolJson["fragments"] = stats.fragments;
        json["pool_stats"].push_back(poolJson);
    }
}

void MemoryMonitor::exportToCSVInternal(std::ostream& output) const {
    output << "Component,Allocations,Deallocations,Total Allocated,Total Deallocated\n";
    for (const auto& [name, stats] : componentStats_) {
        output << name << "," << stats.allocations << "," << stats.deallocations << "," << stats.totalAllocated << "," << stats.totalDeallocated << "\n";
    }

    output << "\nPool Name,Currently Allocated,Capacity,Usage Percentage,Fragments\n";
    for (const auto& [name, stats] : poolStats_) {
        output << stats.name << "," << stats.currentlyAllocated << "," << stats.capacity << "," << stats.usagePercentage << "," << stats.fragments << "\n";
    }
}
