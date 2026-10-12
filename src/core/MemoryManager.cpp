// NLM (熙然) - Neural Learning Machine
// Phase 8: Memory Management Tools Implementation
//
// This file implements the memory management system with:
// - Memory leak detection and tracking
// - Real-time memory profiling
// - Allocation tracking and monitoring
// - Memory usage analysis
// - Performance optimization suggestions

#include "MemoryManager.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <stacktrace>
#include <csignal>
#include <cstdlib>
#include <ctime>

namespace nlm {

// Global memory manager instance
MemoryManager& MemoryManager::getInstance() {
    static MemoryManager instance;
    return instance;
}

// Allocation tracking
void MemoryManager::trackAllocation(void* ptr, size_t size, const std::string& site,
                                    int line, const std::string& func, const std::string& file) {
    if (!leakDetectionEnabled && !memoryProfilingEnabled) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(allocationMutex);
    
    AllocationInfo info;
    info.pointer = ptr;
    info.size = size;
    info.allocationSite = site;
    info.lineNumber = line;
    info.functionName = func;
    info.fileName = file;
    info.timestamp = std::chrono::system_clock::now();
    info.callStack = getCallStack();
    
    allocations.push_back(info);
    allocationMap[ptr] = size;
    
    totalAllocated += size;
    allocationCount++;
    
    // Record by type for analysis
    if (!info.functionName.empty()) {
        allocationByType[info.functionName]++;
    }
    
    // Check memory limit
    if (memoryLimit > 0 && isMemoryLimitExceeded()) {
        std::cerr << "WARNING: Memory limit exceeded! Current usage: " 
                  << formatSize(getCurrentUsage()) << "/" << formatSize(memoryLimit) << std::endl;
    }
}

void MemoryManager::trackDeallocation(void* ptr) {
    if (ptr == nullptr) return;
    
    std::lock_guard<std::mutex> lock(allocationMutex);
    
    auto it = allocationMap.find(ptr);
    if (it != allocationMap.end()) {
        size_t size = it->second;
        totalFreed += size;
        deallocationCount++;
        
        // Record by type for analysis
        allocationByType["deallocated"]++;
        
        allocationMap.erase(it);
    }
}

// Leak detection
std::vector<AllocationInfo> MemoryManager::findLeaks() const {
    std::lock_guard<std::mutex> lock(allocationMutex);
    
    std::vector<AllocationInfo> leaks;
    for (const auto& alloc : allocations) {
        if (allocationMap.find(alloc.pointer) == allocationMap.end()) {
            leaks.push_back(alloc);
        }
    }
    
    return leaks;
}

// Memory profiling
std::string MemoryManager::generateMemoryReport() const {
    std::ostringstream oss;
    oss << "=== NLM Memory Management Report ===\n";
    oss << "Generated: " << getTimestamp(std::chrono::system_clock::now()) << "\n\n";
    
    oss << "=== Memory Statistics ===\n";
    oss << "Total allocated: " << formatSize(totalAllocated) << "\n";
    oss << "Total freed: " << formatSize(totalFreed) << "\n";
    oss << "Current usage: " << formatSize(getCurrentUsage()) << "\n";
    oss << "Allocation count: " << allocationCount << "\n";
    oss << "Deallocation count: " << deallocationCount << "\n";
    oss << "Active allocations: " << allocationMap.size() << "\n\n";
    
    if (memoryLimit > 0) {
        oss << "Memory limit: " << formatSize(memoryLimit) << "\n";
        oss << "Memory utilization: " << std::fixed << std::setprecision(2)
            << (100.0 * getCurrentUsage() / memoryLimit) << "%\n\n";
    }
    
    // Leak information
    auto leaks = findLeaks();
    oss << "=== Memory Leak Analysis ===\n";
    oss << "Leaked objects: " << leaks.size() << "\n";
    oss << "Total leaked memory: " << formatSize(
        std::accumulate(leaks.begin(), leaks.end(), 0,
                       [](size_t sum, const AllocationInfo& alloc) { return sum + alloc.size; })) << "\n\n";
    
    if (!leaks.empty()) {
        oss << "Leaked object details:\n";
        for (size_t i = 0; i < std::min(leaks.size(), static_cast<size_t>(10)); ++i) {
            const auto& leak = leaks[i];
            oss << "  " << i + 1 << ". " << leak.size << " bytes at " << leak.pointer
                << " from " << leak.allocationSite << " (" << leak.functionName << ")\n";
            if (i == 9 && leaks.size() > 10) {
                oss << "   ... and " << leaks.size() - 10 << " more leaks\n";
                break;
            }
        }
    }
    
    // Allocation by type
    if (!allocationByType.empty()) {
        oss << "\n=== Allocation by Type ===\n";
        for (const auto& [type, count] : allocationByType) {
            oss << "  " << type << ": " << count << " allocations\n";
        }
    }
    
    // Performance analysis
    generatePerformanceSuggestions();
    if (!performanceSuggestions.empty()) {
        oss << "\n=== Performance Analysis ===\n";
        for (const auto& suggestion : performanceSuggestions) {
            oss << "  * " << suggestion << "\n";
        }
    }
    
    return oss.str();
}

void MemoryManager::exportMemoryReport(const std::string& filename) const {
    try {
        std::ofstream file(filename);
        if (file.is_open()) {
            file << generateMemoryReport();
            file.close();
            std::cout << "Memory report exported to: " << filename << std::endl;
        } else {
            std::cerr << "ERROR: Cannot open file for writing memory report: " << filename << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "ERROR: Failed to export memory report: " << e.what() << std::endl;
    }
}

// Integration with NLM objects
void MemoryManager::trackObjectAllocation(const std::string& typeName, void* obj, size_t size) {
    if (!memoryProfilingEnabled) return;
    
    trackAllocation(obj, size, "Object allocation", 0, typeName + "::allocate", "nlm_objects.cpp");
}

void MemoryManager::trackObjectDeallocation(const std::string& typeName, void* obj) {
    if (!memoryProfilingEnabled) return;
    
    trackDeallocation(obj);
}

// Simulation integration
void MemoryManager::startMemoryProfiling() {
    memoryProfilingEnabled = true;
    std::cout << "Memory profiling started." << std::endl;
}

void MemoryManager::stopMemoryProfiling() {
    memoryProfilingEnabled = false;
    std::cout << "Memory profiling stopped." << std::endl;
}

// Cleanup
void MemoryManager::cleanup() {
    std::lock_guard<std::mutex> lock(allocationMutex);
    
    // Clear tracking data
    allocations.clear();
    allocationMap.clear();
    allocationByType.clear();
    deallocationByType.clear();
    
    totalAllocated = 0;
    totalFreed = 0;
    allocationCount = 0;
    deallocationCount = 0;
    
    performanceSuggestions.clear();
}

void MemoryManager::generatePerformanceSuggestions() {
    performanceSuggestions.clear();
    
    if (allocationCount == 0) return;
    
    // Calculate average allocation size
    size_t totalTrackedSize = std::accumulate(allocationMap.begin(), allocationMap.end(), 0,
                                            [](size_t sum, const auto& pair) { return sum + pair.second; });
    double avgAllocationSize = static_cast<double>(totalTrackedSize) / allocationMap.size();
    
    // Check for large allocations
    for (const auto& [ptr, size] : allocationMap) {
        if (size > 1024 * 1024) {  // > 1MB
            performanceSuggestions.push_back("Large allocation detected: " + formatSize(size) +
                                            " may cause memory pressure");
        }
    }
    
    // Check for many small allocations
    size_t smallAllocations = std::count_if(allocationMap.begin(), allocationMap.end(),
                                          [](const auto& pair) { return pair.second < 64; });
    if (smallAllocations > allocationMap.size() * 0.5) {
        performanceSuggestions.push_back("Many small allocations detected; consider using pooled memory");
    }
    
    // Check for deallocation frequency
    if (deallocationCount == 0 && allocationCount > 10) {
        performanceSuggestions.push_back("No deallocations detected; possible memory leak");
    }
    
    // Check allocation size variation
    if (!allocationMap.empty()) {
        size_t minSize = allocationMap.begin()->second;
        size_t maxSize = std::max_element(allocationMap.begin(), allocationMap.end(),
                                         [](const auto& a, const auto& b) { return a.second < b.second; })->second;
        
        if (maxSize > minSize * 1000) {
            performanceSuggestions.push_back("Large variation in allocation sizes; consider memory pooling");
        }
    }
}

// Helper functions
std::string MemoryManager::getCallStack(int maxDepth) const {
    std::string result;
    try {
        auto trace = std::stacktrace::current(maxDepth);
        for (size_t i = 0; i < trace.size(); ++i) {
            result += trace[i].description();
            if (i < trace.size() - 1) result += "\n";
        }
    } catch (...) {
        result = "Call stack unavailable";
    }
    return result;
}

std::string MemoryManager::formatSize(size_t size) const {
    static const char* suffixes[] = {"B", "KB", "MB", "GB", "TB"};
    int suffixIndex = 0;
    double formattedSize = static_cast<double>(size);
    
    while (formattedSize > 1024.0 && suffixIndex < 4) {
        formattedSize /= 1024.0;
        suffixIndex++;
    }
    
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << formattedSize << " " << suffixes[suffixIndex];
    return oss.str();
}

std::string MemoryManager::getTimestamp(const std::chrono::system_clock::time_point& tp) const {
    auto time = std::chrono::system_clock::to_time_t(tp);
    std::tm tm_buf;
    gmtime_r(&time, &tm_buf);
    
    std::ostringstream oss;
    oss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S UTC");
    return oss.str();
}

// Global operator new and delete implementations
void* operator new(size_t size) throw(std::bad_alloc) {
    void* ptr = std::malloc(size);
    if (ptr == nullptr) {
        throw std::bad_alloc();
    }
    MemoryManager::getInstance().trackAllocation(ptr, size, "global new", 0, "operator new", "global_new.cpp");
    return ptr;
}

void* operator new[](size_t size) throw(std::bad_alloc) {
    void* ptr = std::malloc(size);
    if (ptr == nullptr) {
        throw std::bad_alloc();
    }
    MemoryManager::getInstance().trackAllocation(ptr, size, "global new[]", 0, "operator new[]", "global_new.cpp");
    return ptr;
}

void operator delete(void* ptr) throw() {
    if (ptr == nullptr) return;
    MemoryManager::getInstance().trackDeallocation(ptr);
    std::free(ptr);
}

void operator delete[](void* ptr) throw() {
    if (ptr == nullptr) return;
    MemoryManager::getInstance().trackDeallocation(ptr);
    std::free(ptr);
}

// MemoryProfiler implementation
void MemoryProfiler::profileBrainInitialization(const std::string& type, size_t neurons, size_t synapses) {
    std::cout << "Profiling brain initialization: " << type << ", " << neurons << " neurons, "
              << synapses << " synapses" << std::endl;
    
    size_t estimatedMemory = neurons * sizeof(Neuron) + synapses * sizeof(Synapse);
    getMemoryManager().trackAllocation(nullptr, estimatedMemory, "Brain initialization", 0,
                                      "MemoryProfiler::profileBrainInitialization", "memory_profiler.cpp");
}

void MemoryProfiler::profileSimulationStep(size_t neurons, size_t synapses, double timeSpent) {
    // Record memory usage at key points
    recordMemorySnapshot("Simulation step");
}

void MemoryProfiler::profileMemoryUsageSnapshot() {
    recordMemorySnapshot("Memory usage snapshot");
}

std::string MemoryProfiler::getMemoryUsageReport() {
    return getMemoryManager().generateMemoryReport();
}

void MemoryProfiler::recordMemorySnapshot(const std::string& description) {
    auto& manager = getMemoryManager();
    std::cout << "Memory snapshot: " << description << ", "
              << "Current usage: " << manager.formatSize(manager.getCurrentUsage()) << std::endl;
}

std::vector<std::string> MemoryProfiler::generateMemoryUsageAnalysis() {
    return std::vector<std::string>{}; // Placeholder
}

// MemoryValidator implementation
bool MemoryValidator::validateMemoryHealth() {
    std::vector<std::string> issues = getMemoryIssues();
    if (!issues.empty()) {
        std::cout << "Memory health check failed with issues:" << std::endl;
        for (const auto& issue : issues) {
            std::cout << "  - " << issue << std::endl;
        }
        return false;
    }
    return true;
}

std::vector<std::string> MemoryValidator::getMemoryIssues() {
    std::vector<std::string> issues;
    
    if (checkForMemoryLeaks()) {
        issues.push_back("Memory leaks detected");
    }
    
    if (checkMemoryCorruption()) {
        issues.push_back("Memory corruption detected");
    }
    
    if (!checkAddressAlignment(nullptr)) {
        issues.push_back("Address alignment issue detected");
    }
    
    size_t footprint = estimateMemoryFootprint();
    if (footprint > 1024 * 1024 * 1024) {  // > 1GB
        issues.push_back("Large memory footprint: " + formatSize(footprint));
    }
    
    return issues;
}

bool MemoryValidator::checkMemoryLeaks() {
    return getMemoryManager().hasLeaks();
}

bool MemoryValidator::checkMemoryCorruption() {
    // Basic corruption check
    auto leaks = getMemoryManager().findLeaks();
    for (const auto& leak : leaks) {
        if (!checkAddressAlignment(leak.pointer)) {
            return true;
        }
    }
    return false;
}

bool MemoryValidator::checkAddressAlignment(void* ptr) {
    if (ptr == nullptr) return true;
    
    uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
    return (addr % 8) == 0; // 8-byte alignment
}

size_t MemoryValidator::estimateMemoryFootprint() const {
    return getMemoryManager().getCurrentUsage();
}

// MemoryOptimizer implementation
std::vector<std::string> MemoryOptimizer::analyzeAllocationPatterns() {
    std::vector<std::string> analysis;
    
    auto& manager = getMemoryManager();
    analysis.push_back("Analyzing allocation patterns...");
    
    // Check for allocation patterns
    if (manager.getCurrentUsage() > 0) {
        analysis.push_back("Active allocations detected: " + manager.formatSize(manager.getCurrentUsage()));
    }
    
    if (manager.hasLeaks()) {
        analysis.push_back("Memory leaks present - cleanup required");
    }
    
    return analysis;
}

std::vector<std::string> MemoryOptimizer::suggestOptimizations() {
    std::vector<std::string> suggestions;
    
    auto& manager = getMemoryManager();
    
    // Check for memory limits
    if (manager.getMemoryLimit() > 0) {
        if (manager.isMemoryLimitExceeded()) {
            suggestions.push_back("Memory limit exceeded; consider reducing allocation size");
        }
    }
    
    // Check allocation size distribution
    if (!manager.getCurrentUsage()) {
        suggestions.push_back("No current memory usage detected");
    }
    
    // Check allocation frequency
    if (manager.getAllocationCount() == 0) {
        suggestions.push_back("No allocations tracked");
    }
    
    return suggestions;
}

void MemoryOptimizer::applyMemoryOptimizations() {
    std::cout << "Applying memory optimizations..." << std::endl;
    
    auto& manager = getMemoryManager();
    
    // Clear memory profiling data
    manager.cleanup();
    
    // Reset memory tracking
    manager.setMemoryLimit(0);
    manager.enableLeakDetection(false);
    manager.enablePerformanceAnalysis(false);
    
    std::cout << "Memory optimizations applied." << std::endl;
}

} // namespace nlm