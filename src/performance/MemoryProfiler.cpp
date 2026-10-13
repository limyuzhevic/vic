// MemoryProfiler.cpp
// Implementation of the memory profiler for NLM brain system

#include "MemoryProfiler.h"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <chrono>
#include <numeric>
#include <cmath>
#include <algorithm>
#include <sstream>

namespace {
    // Simple stack trace capture (placeholder)
    std::string captureStackTrace() {
        // In a real implementation, this would use platform-specific stack unwinding
        return "Stack trace not available in this build";
    }
}

MemoryProfiler::MemoryProfiler(const MemoryProfilerConfig& config)
    : config(config), sessionStartTime(std::chrono::high_resolution_clock::now()) {
    // Initialize tracking
    allocations.reserve(config.maxTrackedAllocations);
    deallocations.reserve(config.maxTrackedAllocations);
    memoryStats.reserve(config.maxMemorySamples);
}

MemoryProfiler::~MemoryProfiler() {
    // Ensure all allocations are properly tracked
    detectLeaks();
}

void MemoryProfiler::trackAllocation(size_t size, void* address, const std::string& typeName,
                                    const std::string& sourceFile, int sourceLine) {
    if (!config.enableAllocationTracking) return;
    
    std::lock_guard<std::mutex> lock(dataMutex);
    
    AllocationRecord record(address, size, typeName, sourceFile, sourceLine);
    
    if (config.enableStackTraces) {
        captureStackTrace(record);
    }
    
    allocations.push_back(record);
    
    // Update allocation map for leak detection
    if (config.enableLeakDetection) {
        allocationMap[address] = record;
    }
    
    // Update statistics
    updateMemoryStatistics();
}

void MemoryProfiler::trackDeallocation(void* address) {
    if (!config.enableDeallocationTracking) return;
    
    std::lock_guard<std::mutex> lock(dataMutex);
    
    // Find the corresponding allocation
    auto it = allocationMap.find(address);
    if (it != allocationMap.end()) {
        it->second.deallocationTime = std::chrono::high_resolution_clock::now();
        it->second.isLeaked = false;
        deallocations.push_back(it->second);
        
        // Remove from allocation map
        allocationMap.erase(it);
    } else {
        // Deallocation without corresponding allocation (potential double free)
        AllocationRecord record(address, 0, "Unknown", "", 0);
        record.deallocationTime = std::chrono::high_resolution_clock::now();
        deallocations.push_back(record);
    }
    
    updateMemoryStatistics();
}

MemoryStatistics MemoryProfiler::getCurrentStatistics() const {
    std::lock_guard<std::mutex> lock(dataMutex);
    return memoryStats.empty() ? MemoryStatistics() : memoryStats.back();
}

MemoryStatistics MemoryProfiler::getStatisticsAt(size_t index) const {
    std::lock_guard<std::mutex> lock(dataMutex);
    
    if (index < memoryStats.size()) {
        return memoryStats[index];
    }
    return MemoryStatistics();
}

std::vector<AllocationRecord> MemoryProfiler::getLeaks() const {
    std::lock_guard<std::mutex> lock(dataMutex);
    
    std::vector<AllocationRecord> leaks;
    for (const auto& record : allocations) {
        if (record.isLeaked) {
            leaks.push_back(record);
        }
    }
    
    return leaks;
}

std::vector<AllocationRecord> MemoryProfiler::getLeaksByType(const std::string& typeName) const {
    std::lock_guard<std::mutex> lock(dataMutex);
    
    std::vector<AllocationRecord> leaks;
    for (const auto& record : allocations) {
        if (record.isLeaked && record.typeName == typeName) {
            leaks.push_back(record);
        }
    }
    
    return leaks;
}

size_t MemoryProfiler::getLeakCount() const {
    std::lock_guard<std::mutex> lock(dataMutex);
    
    size_t count = 0;
    for (const auto& record : allocations) {
        if (record.isLeaked) {
            count++;
        }
    }
    
    return count;
}

double MemoryProfiler::getLeakPercentage() const {
    std::lock_guard<std::mutex> lock(dataMutex);
    
    if (allocations.empty()) return 0.0;
    
    size_t leaks = getLeakCount();
    return static_cast<double>(leaks) / allocations.size() * 100.0;
}

void MemoryProfiler::updateMemoryUsage(size_t currentUsage) {
    if (!config.enableRealTimeMonitoring) return;
    
    std::lock_guard<std::mutex> lock(dataMutex);
    
    memoryUsageHistory.push_back(currentUsage);
    if (memoryUsageHistory.size() > config.maxMemorySamples) {
        memoryUsageHistory.erase(memoryUsageHistory.begin());
    }
    
    updateMemoryStatistics();
}

size_t MemoryProfiler::getPeakMemoryUsage() const {
    std::lock_guard<std::mutex> lock(dataMutex);
    
    if (memoryUsageHistory.empty()) return 0;
    
    return *std::max_element(memoryUsageHistory.begin(), memoryUsageHistory.end());
}

double MemoryProfiler::getAverageMemoryUsage() const {
    std::lock_guard<std::mutex> lock(dataMutex);
    
    if (memoryUsageHistory.empty()) return 0.0;
    
    double sum = std::accumulate(memoryUsageHistory.begin(), memoryUsageHistory.end(), 0.0);
    return sum / memoryUsageHistory.size();
}

void MemoryProfiler::profileBrainMemoryUsage(nlm::Brain* brain) {
    if (!brain) return;
    
    // Profile brain memory usage
    size_t brainMemory = brain->getTotalNeuronCount() * sizeof(nlm::Neuron) +
                        brain->getTotalSynapseCount() * sizeof(nlm::Synapse);
    
    updateMemoryUsage(brainMemory);
    
    MemoryStatistics stats = getCurrentStatistics();
    stats.totalAllocations++;
    stats.currentMemoryUsage = brainMemory;
    memoryStats.push_back(stats);
}

void MemoryProfiler::profileNeuronMemory(nlm::Neuron* neuron) {
    if (!neuron) return;
    
    // Profile neuron memory usage
    size_t neuronMemory = sizeof(nlm::Neuron);
    updateMemoryUsage(neuronMemory);
}

void MemoryProfiler::profileSynapseMemory(nlm::Synapse* synapse) {
    if (!synapse) return;
    
    // Profile synapse memory usage
    size_t synapseMemory = sizeof(nlm::Synapse);
    updateMemoryUsage(synapseMemory);
}

void MemoryProfiler::updateMemoryStatistics() {
    MemoryStatistics stats;
    
    // Calculate current statistics
    stats.totalAllocations = allocations.size();
    stats.totalDeallocations = deallocations.size();
    stats.netAllocations = stats.totalAllocations - stats.totalDeallocations;
    
    if (config.enableLeakDetection) {
        detectLeaks();
    }
    
    // Calculate memory usage
    stats.currentMemoryUsage = calculateCurrentMemoryUsage();
    stats.peakMemoryUsage = getPeakMemoryUsage();
    
    // Calculate allocation rates
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(now - sessionStartTime).count();
    
    if (duration > 0.0) {
        stats.allocationRate = static_cast<double>(stats.totalAllocations) / duration;
        stats.deallocationRate = static_cast<double>(stats.totalDeallocations) / duration;
    }
    
    // Calculate allocation size statistics
    if (!allocations.empty()) {
        size_t totalSize = 0;
        stats.largestAllocation = 0;
        stats.smallestAllocation = std::numeric_limits<size_t>::max();
        
        for (const auto& alloc : allocations) {
            totalSize += alloc.size;
            if (alloc.size > stats.largestAllocation) {
                stats.largestAllocation = alloc.size;
            }
            if (alloc.size < stats.smallestAllocation) {
                stats.smallestAllocation = alloc.size;
            }
        }
        
        // stats.currentMemoryUsage += totalSize; // Include allocated size
    }
    
    memoryStats.push_back(stats);
}

void MemoryProfiler::detectLeaks() {
    // Mark allocations without deallocations as leaks
    for (auto& record : allocations) {
        if (!record.isLeaked && record.deallocationTime.time_since_epoch().count() == 0) {
            // Check if this address exists in deallocations
            bool found = false;
            for (const auto& dealloc : deallocations) {
                if (dealloc.address == record.address) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                record.isLeaked = true;
            }
        }
    }
}

void MemoryProfiler::cleanupDeallocatedRecords() {
    // Remove deallocated records that are older than threshold
    auto now = std::chrono::high_resolution_clock::now();
    auto threshold = now - std::chrono::hours(1); // Keep records for 1 hour
    
    allocations.erase(std::remove_if(allocations.begin(), allocations.end(),
                                    [threshold](const AllocationRecord& record) {
                                        return record.deallocationTime.time_since_epoch().count() > 0 &&
                                               record.deallocationTime < threshold;
                                    }),
                      allocations.end());
    
    deallocations.erase(std::remove_if(deallocations.begin(), deallocations.end(),
                                      [threshold](const AllocationRecord& record) {
                                          return record.deallocationTime < threshold;
                                      }),
                      deallocations.end());
}

void MemoryProfiler::captureStackTrace(AllocationRecord& record) {
    record.stackTrace = captureStackTrace();
}

size_t MemoryProfiler::calculateCurrentMemoryUsage() const {
    // Simulate current memory usage calculation
    size_t total = 0;
    for (const auto& record : allocations) {
        if (!record.isLeaked) {
            total += record.size;
        }
    }
    return total;
}

void MemoryProfiler::setConfig(const MemoryProfilerConfig& newConfig) {
    std::lock_guard<std::mutex> lock(dataMutex);
    config = newConfig;
}

void MemoryProfiler::generateMemoryReport(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file for writing: " << filename << std::endl;
        return;
    }
    
    file << "NLM Memory Profiler Report" << std::endl;
    file << "===========================" << std::endl;
    file << std::endl;
    
    file << "Total Allocations: " << allocations.size() << std::endl;
    file << "Total Deallocations: " << deallocations.size() << std::endl;
    file << "Net Allocations: " << (allocations.size() - deallocations.size()) << std::endl;
    file << "Current Memory Usage: " << calculateCurrentMemoryUsage() << " bytes" << std::endl;
    file << "Peak Memory Usage: " << getPeakMemoryUsage() << " bytes" << std::endl;
    file << std::endl;
    
    // Allocation breakdown by type
    std::unordered_map<std::string, size_t> typeCounts;
    for (const auto& alloc : allocations) {
        typeCounts[alloc.typeName]++;
    }
    
    file << "Allocations by Type:" << std::endl;
    for (const auto& pair : typeCounts) {
        file << "  " << pair.first << ": " << pair.second << std::endl;
    }
    file << std::endl;
    
    // Leak report
    file << "Memory Leaks: " << getLeakCount() << std::endl;
    file << "Leak Percentage: " << std::fixed << std::setprecision(2) << getLeakPercentage() << "%" << std::endl;
    
    file << std::endl;
    file << "Top 10 Leaked Types:" << std::endl;
    std::vector<std::pair<std::string, size_t>> typeLeaks;
    for (const auto& alloc : allocations) {
        if (alloc.isLeaked) {
            typeLeaks.emplace_back(alloc.typeName, 0);
        }
    }
    
    file.close();
}

void MemoryProfiler::printMemorySummary() const {
    std::cout << std::endl;
    std::cout << "╔══════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║ " << std::left << std::setw(70) << "Memory Profiler Summary" << " ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Total Allocations: " << allocations.size() << std::endl;
    std::cout << "Total Deallocations: " << deallocations.size() << std::endl;
    std::cout << "Net Allocations: " << (allocations.size() - deallocations.size()) << std::endl;
    std::cout << "Current Memory Usage: " << calculateCurrentMemoryUsage() << " bytes" << std::endl;
    std::cout << "Peak Memory Usage: " << getPeakMemoryUsage() << " bytes" << std::endl;
    std::cout << std::endl;
}

void MemoryProfiler::printAllocationHistogram() const {
    std::cout << std::endl;
    std::cout << "╔══════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║ " << std::left << std::setw(70) << "Allocation Size Histogram" << " ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
    
    // Create histogram of allocation sizes
    std::vector<size_t> buckets = {0, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, SIZE_MAX};
    std::vector<size_t> counts(buckets.size(), 0);
    
    for (const auto& alloc : allocations) {
        for (size_t i = 0; i < buckets.size() - 1; ++i) {
            if (alloc.size >= buckets[i] && alloc.size < buckets[i + 1]) {
                counts[i]++;
                break;
            }
        }
    }
    
    // Print histogram
    for (size_t i = 0; i < buckets.size() - 1; ++i) {
        std::cout << std::left << std::setw(20) << (buckets[i] == 0 ? "< 16B" : 
                        buckets[i] >= 1024 ? std::to_string(buckets[i] / 1024) + "KB" :
                        std::to_string(buckets[i]) + "B")
                  << " " << std::string(counts[i] / 10, '*') << std::endl;
    }
}

void MemoryProfiler::printLeakReport() const {
    std::cout << std::endl;
    std::cout << "╔══════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║ " << std::left << std::setw(70) << "Memory Leak Report" << " ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Total Leaks: " << getLeakCount() << std::endl;
    std::cout << "Leak Percentage: " << std::fixed << std::setprecision(2) << getLeakPercentage() << "%" << std::endl;
    std::cout << std::endl;
    
    // Print leaks by type
    std::unordered_map<std::string, size_t> typeLeaks;
    for (const auto& alloc : allocations) {
        if (alloc.isLeaked) {
            typeLeaks[alloc.typeName]++;
        }
    }
    
    for (const auto& pair : typeLeaks) {
        std::cout << pair.first << ": " << pair.second << std::endl;
    }
}

void MemoryProfiler::clear() {
    std::lock_guard<std::mutex> lock(dataMutex);
    allocations.clear();
    deallocations.clear();
    memoryStats.clear();
    memoryUsageHistory.clear();
    allocationMap.clear();
}

bool MemoryProfiler::isLeak(void* address) const {
    std::lock_guard<std::mutex> lock(dataMutex);
    
    auto it = allocationMap.find(address);
    return (it != allocationMap.end()) && it->second.isLeaked;
}

namespace {
    MemoryProfiler* globalMemoryProfiler = nullptr;
    std::mutex memoryProfilerMutex;
}

MemoryProfiler& MemoryProfiler::getInstance(const MemoryProfilerConfig& config) {
    std::lock_guard<std::mutex> lock(memoryProfilerMutex);
    if (!globalMemoryProfiler) {
        globalMemoryProfiler = new MemoryProfiler(config);
    }
    return *globalMemoryProfiler;
}

void MemoryProfiler::cleanupInstance() {
    std::lock_guard<std::mutex> lock(memoryProfilerMutex);
    delete globalMemoryProfiler;
    globalMemoryProfiler = nullptr;
}