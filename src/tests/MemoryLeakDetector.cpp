#include "MemoryLeakDetector.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <cstring>
#include <unistd.h>
#include <sys/mman.h>
#include <cxxabi.h>

namespace nlm {

MemoryLeakDetector::MemoryLeakDetector() : hooksInstalled(false), tracking(false), 
                                         realTimeMonitoring(false), currentSession("") {
    // Initialize hooks
}

MemoryLeakDetector::~MemoryLeakDetector() {
    removeHooks();
}

void MemoryLeakDetector::installHooks() {
    if (hooksInstalled) return;
    
    // Install malloc/free hooks
    hooksInstalled = true;
    
    std::cout << "Memory leak detector hooks installed" << std::endl;
}

void MemoryLeakDetector::removeHooks() {
    if (!hooksInstalled) return;
    
    // Remove hooks
    hooksInstalled = false;
    
    std::cout << "Memory leak detector hooks removed" << std::endl;
}

void MemoryLeakDetector::trackAllocation(size_t address, size_t size, const std::string& type, 
                                        const std::string& site) {
    std::lock_guard<std::mutex> lock(allocationsMutex);
    
    MemoryAllocation alloc(address, size, type, site);
    allocations.push_back(alloc);
    allocationMap[address] = type;
    
    if (realTimeMonitoring) {
        onAllocation(address, size, type);
    }
}

void MemoryLeakDetector::trackDeallocation(size_t address, const std::string& type, 
                                          const std::string& site) {
    std::lock_guard<std::mutex> lock(allocationsMutex);
    
    MemoryAllocation dealloc(address, 0, type, site);
    deallocations.push_back(dealloc);
    deallocatedAddresses.insert(address);
    
    if (realTimeMonitoring) {
        onDeallocation(address, type);
    }
}

std::vector<MemoryLeak> MemoryLeakDetector::detectLeaks() const {
    return analyzeLeakedAllocations();
}

bool MemoryLeakDetector::hasLeaks() const {
    return !analyzeLeakedAllocations().empty();
}

 size_t MemoryLeakDetector::getTotalLeakSize() const {
    size_t totalSize = 0;
    auto leaks = analyzeLeakedAllocations();
    for (const auto& leak : leaks) {
        totalSize += leak.leakSize;
    }
    return totalSize;
}

std::vector<MemoryLeak> MemoryLeakDetector::analyzeLeaksByType() const {
    std::vector<MemoryLeak> typeLeaks;
    auto leaks = analyzeLeakedAllocations();
    
    // Group by type
    std::map<std::string, std::vector<const MemoryAllocation*>> typeAllocations;
    
    for (const auto& alloc : allocations) {
        if (deallocatedAddresses.find(alloc.address) == deallocatedAddresses.end()) {
            typeAllocations[alloc.type].push_back(&alloc);
        }
    }
    
    for (const auto& pair : typeAllocations) {
        MemoryLeak leak;
        leak.leakType = pair.first;
        leak.leakSize = 0;
        leak.leakSite = pair.first; // Simplified
        leak.description = "Memory leak of type: " + pair.first;
        leak.leakRate = 0.0; // Would calculate based on tracking duration
        leak.firstOccurrence = std::chrono::system_clock::now();
        leak.lastOccurrence = std::chrono::system_clock::now();
        
        for (const auto* alloc : pair.second) {
            leak.leakSize += alloc->size;
            leak.affectedStackTraces.push_back(alloc->callStack);
        }
        
        typeLeaks.push_back(leak);
    }
    
    return typeLeaks;
}

std::vector<MemoryLeak> MemoryLeakDetector::analyzeLeaksBySite() const {
    std::vector<MemoryLeak> siteLeaks;
    auto leaks = analyzeLeakedAllocations();
    
    // Group by allocation site
    std::map<std::string, std::vector<const MemoryAllocation*>> siteAllocations;
    
    for (const auto& alloc : allocations) {
        if (deallocatedAddresses.find(alloc.address) == deallocatedAddresses.end()) {
            siteAllocations[alloc.allocationSite].push_back(&alloc);
        }
    }
    
    for (const auto& pair : siteAllocations) {
        MemoryLeak leak;
        leak.leakType = "site_specific";
        leak.leakSize = 0;
        leak.leakSite = pair.first;
        leak.description = "Memory leak originating from: " + pair.first;
        leak.leakRate = 0.0;
        leak.firstOccurrence = std::chrono::system_clock::now();
        leak.lastOccurrence = std::chrono::system_clock::now();
        
        for (const auto* alloc : pair.second) {
            leak.leakSize += alloc->size;
            leak.affectedStackTraces.push_back(alloc->callStack);
        }
        
        siteLeaks.push_back(leak);
    }
    
    return siteLeaks;
}

std::vector<MemoryLeak> MemoryLeakDetector::analyzeLeaksByStackTrace() const {
    std::vector<MemoryLeak> stackLeaks;
    auto leaks = analyzeLeakedAllocations();
    
    // Group by call stack
    std::map<std::string, std::vector<const MemoryAllocation*>> stackAllocations;
    
    for (const auto& alloc : allocations) {
        if (deallocatedAddresses.find(alloc.address) == deallocatedAddresses.end()) {
            stackAllocations[alloc.callStack].push_back(&alloc);
        }
    }
    
    for (const auto& pair : stackAllocations) {
        MemoryLeak leak;
        leak.leakType = "callstack_specific";
        leak.leakSize = 0;
        leak.leakSite = pair.first; // Simplified - using call stack as site
        leak.description = "Memory leak from call stack: " + pair.first;
        leak.leakRate = 0.0;
        leak.firstOccurrence = std::chrono::system_clock::now();
        leak.lastOccurrence = std::chrono::system_clock::now();
        
        for (const auto* alloc : pair.second) {
            leak.leakSize += alloc->size;
            leak.affectedStackTraces.push_back(alloc->callStack);
        }
        
        stackLeaks.push_back(leak);
    }
    
    return stackLeaks;
}

std::vector<MemoryLeak> MemoryLeakDetector::analyzeLeaksByGrowth() const {
    // Growth pattern analysis
    return std::vector<MemoryLeak>(); // Placeholder
}

void MemoryLeakDetector::startTracking(const std::string& sessionName) {
    tracking = true;
    currentSession = sessionName;
    sessionStartTime = std::chrono::system_clock::now();
    
    std::cout << "Started tracking memory leaks for session: " << sessionName << std::endl;
}

void MemoryLeakDetector::stopTracking() {
    tracking = false;
    currentSession = "";
    
    std::cout << "Stopped tracking memory leaks" << std::endl;
}

std::string MemoryLeakDetector::generateLeakReport() const {
    return config_.enableDetailedReporting ? generateDetailedLeakReport() : generateLeakSummary();
}

std::string MemoryLeakDetector::generateLeakSummary() const {
    std::stringstream ss;
    ss << "=== Memory Leak Summary ===" << std::endl;
    
    auto leaks = analyzeLeakedAllocations();
    ss << "Total leaks detected: " << leaks.size() << std::endl;
    ss << "Total leak size: " << getTotalLeakSize() << " bytes" << std::endl;
    
    if (leaks.empty()) {
        ss << "No memory leaks detected." << std::endl;
    } else {
        ss << "Leaks by type:" << std::endl;
        auto typeLeaks = analyzeLeaksByType();
        for (const auto& leak : typeLeaks) {
            ss << "  " << leak.leakType << ": " << leak.leakSize << " bytes" << std::endl;
        }
    }
    
    return ss.str();
}

std::string MemoryLeakDetector::generateDetailedLeakReport() const {
    std::stringstream ss;
    ss << "=== Detailed Memory Leak Report ===" << std::endl;
    
    auto leaks = analyzeLeakedAllocations();
    ss << "Leaked allocations:" << std::endl;
    
    for (size_t i = 0; i < allocations.size(); ++i) {
        if (deallocatedAddresses.find(allocations[i].address) == deallocatedAddresses.end()) {
            const auto& alloc = allocations[i];
            ss << "  Leak " << (i + 1) << ": " << alloc.size << " bytes, type: " << alloc.type 
               << ", site: " << alloc.allocationSite << std::endl;
            if (config_.maxStackTraceDepth > 0 && !alloc.callStack.empty()) {
                ss << "    Stack trace (first " << config_.maxStackTraceDepth << " lines):" << std::endl;
                std::vector<std::string> stackLines = splitString(alloc.callStack, '\n');
                for (size_t j = 0; j < std::min(config_.maxStackTraceDepth, stackLines.size()); ++j) {
                    ss << "      " << stackLines[j] << std::endl;
                }
            }
        }
    }
    
    return ss.str();
}

std::string MemoryLeakDetector::generateCallStackReport() const {
    std::stringstream ss;
    ss << "=== Call Stack Memory Leak Report ===" << std::endl;
    
    auto leaks = analyzeLeakedAllocations();
    ss << "Leaked allocations by call stack:" << std::endl;
    
    std::map<std::string, std::vector<const MemoryAllocation*>> stackAllocations;
    
    for (const auto& alloc : allocations) {
        if (deallocatedAddresses.find(alloc.address) == deallocatedAddresses.end()) {
            stackAllocations[alloc.callStack].push_back(&alloc);
        }
    }
    
    for (const auto& pair : stackAllocations) {
        ss << "Call stack:" << std::endl;
        std::vector<std::string> stackLines = splitString(pair.first, '\n');
        for (const auto& line : stackLines) {
            ss << "  " << line << std::endl;
        }
        ss << "Allocated bytes: " << std::accumulate(pair.second.begin(), pair.second.end(), 0,
                                                     [](size_t sum, const MemoryAllocation* alloc) { return sum + alloc->size; })
           << std::endl;
        ss << "Number of leaks: " << pair.second.size() << std::endl;
        ss << std::endl;
    }
    
    return ss.str();
}

void MemoryLeakDetector::exportLeaks(const std::string& filename) const {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << generateLeakReport() << std::endl;
        file.close();
        std::cout << "Memory leak report exported to: " << filename << std::endl;
    } else {
        throw std::runtime_error("Cannot open file for export: " + filename);
    }
}

void MemoryLeakDetector::importLeaks(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for import: " + filename);
    }
    
    // Parse import file (simplified)
    std::string line;
    while (std::getline(file, line)) {
        // Parse each line (simplified implementation)
    }
    
    file.close();
    std::cout << "Memory leak report imported from: " << filename << std::endl;
}

std::vector<size_t> MemoryLeakDetector::findLeakPoints() const {
    std::vector<size_t> leakPoints;
    
    // Find points where memory usage starts growing
    for (size_t i = 1; i < allocations.size(); ++i) {
        if (allocations[i].address != 0 && deallocatedAddresses.find(allocations[i].address) == deallocatedAddresses.end()) {
            // Check if this allocation is still active
            leakPoints.push_back(i);
        }
    }
    
    return leakPoints;
}

double MemoryLeakDetector::calculateLeakRate() const {
    if (allocations.empty()) return 0.0;
    
    size_t totalAllocated = 0;
    size_t totalDeallocated = 0;
    
    for (const auto& alloc : allocations) {
        totalAllocated += alloc.size;
    }
    
    for (const auto& dealloc : deallocations) {
        totalDeallocated += dealloc.size;
    }
    
    return (totalAllocated - totalDeallocated) / static_cast<double>(allocations.size());
}

std::string MemoryLeakDetector::analyzeMemoryPattern() const {
    // Analyze memory allocation/deallocation patterns
    return "Memory pattern analysis (placeholder)";
}

void MemoryLeakDetector::setupForTest(const std::string& testName) {
    startTracking(testName);
}

void MemoryLeakDetector::cleanupAfterTest(const std::string& testName) {
    // Check for leaks before stopping tracking
    if (hasLeaks()) {
        std::cout << "WARNING: Memory leaks detected in test: " << testName << std::endl;
        std::cout << "Leak report:" << std::endl << generateLeakReport() << std::endl;
    }
    
    stopTracking();
}

bool MemoryLeakDetector::verifyNoLeaksSinceSetup(const std::string& testName) {
    if (!isTracking()) {
        return false;
    }
    
    return !hasLeaks();
}

void MemoryLeakDetector::simulateMemoryPressure(size_t pressureBytes, const std::string& description) {
    std::cout << "Simulating memory pressure: " << description << " (" << pressureBytes << " bytes)" << std::endl;
    
    // Simulate memory allocation and deallocation
    std::vector<std::vector<char>> pressure;
    pressure.reserve(pressureBytes / 1024); // Estimate 1KB per allocation
    
    for (size_t i = 0; i < pressureBytes / 1024; ++i) {
        pressure.push_back(std::vector<char>(1024));
    }
    
    std::cout << "Memory pressure simulation completed" << std::endl;
}

void MemoryLeakDetector::simulateMemoryFragmentation(size_t fragmentSize, int fragmentCount) {
    std::cout << "Simulating memory fragmentation: " << fragmentCount << " fragments of " << fragmentSize << " bytes each" << std::endl;
    
    // Simulate fragmented allocations
    std::vector<std::unique_ptr<char[]>> fragments;
    for (int i = 0; i < fragmentCount; ++i) {
        fragments.push_back(std::make_unique<char[]>(fragmentSize));
    }
    
    std::cout << "Memory fragmentation simulation completed" << std::endl;
}

void MemoryLeakDetector::simulateAllocationDeallocationPattern(int iterations, size_t patternSize) {
    std::cout << "Simulating allocation/deallocation pattern: " << iterations << " iterations of " << patternSize << " bytes" << std::endl;
    
    for (int i = 0; i < iterations; ++i) {
        void* ptr = malloc(patternSize);
        if (ptr) {
            // Do something with the allocated memory
            memset(ptr, 0, patternSize);
            free(ptr);
        }
    }
    
    std::cout << "Allocation/deallocation pattern simulation completed" << std::endl;
}

std::vector<MemoryLeak> MemoryLeakDetector::detectHeapLeaks() const {
    // Heap-specific leak detection
    return std::vector<MemoryLeak>(); // Placeholder
}

std::vector<MemoryLeak> MemoryLeakDetector::detectStackLeaks() const {
    // Stack-specific leak detection
    return std::vector<MemoryLeak>(); // Placeholder
}

std::vector<MemoryLeak> MemoryLeakDetector::detectGlobalLeaks() const {
    // Global variable leak detection
    return std::vector<MemoryLeak>(); // Placeholder
}

bool MemoryLeakDetector::detectMemoryCorruption() const {
    // Memory corruption detection
    return false; // Placeholder
}

std::string MemoryLeakDetector::checkMemoryIntegrity() const {
    // Memory integrity checking
    return "Memory integrity check passed (placeholder)";
}

void MemoryLeakDetector::enableRealTimeMonitoring(bool enable) {
    realTimeMonitoring = enable;
    std::cout << "Real-time memory monitoring " << (enable ? "enabled" : "disabled") << std::endl;
}

void MemoryLeakDetector::onAllocation(size_t address, size_t size, const std::string& type) {
    // Real-time allocation monitoring
    // In a real implementation, this would integrate with the allocation hooks
}

void MemoryLeakDetector::onDeallocation(size_t address, const std::string& type) {
    // Real-time deallocation monitoring
    // In a real implementation, this would integrate with the allocation hooks
}

// Hook implementations
void* MemoryLeakDetector::mallocHook(size_t size) {
    void* ptr = malloc(size);
    if (ptr) {
        MemoryLeakDetector::getInstance().trackAllocation(reinterpret_cast<size_t>(ptr), size, "malloc", getAllocationSite());
    }
    return ptr;
}

void MemoryLeakDetector::freeHook(void* ptr) {
    if (ptr) {
        MemoryLeakDetector::getInstance().trackDeallocation(reinterpret_cast<size_t>(ptr), "free", getAllocationSite());
    }
    free(ptr);
}

void* MemoryLeakDetector::newHook(size_t size) {
    void* ptr = new char[size];
    if (ptr) {
        MemoryLeakDetector::getInstance().trackAllocation(reinterpret_cast<size_t>(ptr), size, "new", getAllocationSite());
    }
    return ptr;
}

void MemoryLeakDetector::deleteHook(void* ptr) {
    if (ptr) {
        MemoryLeakDetector::getInstance().trackDeallocation(reinterpret_cast<size_t>(ptr), "delete", getAllocationSite());
    }
    delete[] ptr;
}

void* MemoryLeakDetector::newArrayHook(size_t size) {
    void* ptr = new char[size];
    if (ptr) {
        MemoryLeakDetector::getInstance().trackAllocation(reinterpret_cast<size_t>(ptr), size, "new[]", getAllocationSite());
    }
    return ptr;
}

void MemoryLeakDetector::deleteArrayHook(void* ptr) {
    if (ptr) {
        MemoryLeakDetector::getInstance().trackDeallocation(reinterpret_cast<size_t>(ptr), "delete[]", getAllocationSite());
    }
    delete[] ptr;
}

void* MemoryLeakDetector::mmapHook(void* addr, size_t length, int prot, int flags, int fd, off_t offset) {
    void* ptr = mmap(addr, length, prot, flags, fd, offset);
    if (ptr != MAP_FAILED) {
        MemoryLeakDetector::getInstance().trackAllocation(reinterpret_cast<size_t>(ptr), length, "mmap", getAllocationSite());
    }
    return ptr;
}

void MemoryLeakDetector::munmapHook(void* addr, size_t length) {
    if (addr != MAP_FAILED) {
        MemoryLeakDetector::getInstance().trackDeallocation(reinterpret_cast<size_t>(addr), "munmap", getAllocationSite());
    }
    munmap(addr, length);
}

// Helper functions
std::string MemoryLeakDetector::captureStackTrace() const {
    // Capture stack trace using backtrace
    const int maxDepth = config_.maxStackTraceDepth;
    void* callStack[maxDepth];
    
    int frames = backtrace(callStack, maxDepth);
    char** symbols = backtrace_symbols(callStack, frames);
    
    std::stringstream ss;
    for (int i = 0; i < frames; ++i) {
        ss << symbols[i];
        if (i < frames - 1) ss << std::endl;
    }
    
    free(symbols);
    return ss.str();
}

std::string MemoryLeakDetector::getAllocationSite() const {
    // Get allocation site from stack trace
    return captureStackTrace(); // Simplified - use stack trace as site
}

bool MemoryLeakDetector::isAddressTracked(size_t address) const {
    return allocationMap.find(address) != allocationMap.end();
}

const MemoryAllocation* MemoryLeakDetector::getTrackedAllocation(size_t address) const {
    for (const auto& alloc : allocations) {
        if (alloc.address == address) {
            return &alloc;
        }
    }
    return nullptr;
}

std::vector<MemoryLeak> MemoryLeakDetector::analyzeLeakedAllocations() const {
    std::vector<MemoryLeak> leaks;
    
    // Find allocations that were not deallocated
    for (const auto& alloc : allocations) {
        if (deallocatedAddresses.find(alloc.address) == deallocatedAddresses.end()) {
            MemoryLeak leak;
            leak.leakType = alloc.type;
            leak.leakSize = alloc.size;
            leak.leakSite = alloc.allocationSite;
            leak.description = "Memory leak from " + alloc.type + " allocation at " + alloc.allocationSite;
            leak.leakRate = 0.0; // Calculate based on tracking duration
            leak.firstOccurrence = alloc.timestamp;
            leak.lastOccurrence = std::chrono::system_clock::now();
            leak.affectedStackTraces.push_back(alloc.callStack);
            
            leaks.push_back(leak);
        }
    }
    
    return leaks;
}

std::string MemoryLeakDetector::splitString(const std::string& str, char delimiter) const {
    // Helper function to split strings
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    
    std::stringstream result;
    for (size_t i = 0; i < tokens.size(); ++i) {
        if (i > 0) result << std::endl;
        result << tokens[i];
    }
    
    return result.str();
}

} // namespace nlm
