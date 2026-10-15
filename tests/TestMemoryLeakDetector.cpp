// Test Memory Leak Detector Implementation
// Memory leak detection during test execution

#include "TestMemoryLeakDetector.hpp"
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <unordered_map>

namespace nlm {
namespace tests {

struct TestMemoryLeakDetector::Impl {
    // Memory tracking
    std::unordered_map<void*, MemoryAllocation> allocations;
    std::mutex allocationsMutex;
    
    // Leak detection
    std::vector<MemoryLeak> detectedLeaks;
    std::mutex leaksMutex;
    
    // Memory statistics
    size_t totalAllocated;
    size_t totalFreed;
    size_t peakMemoryUsage;
    std::chrono::milliseconds peakMemoryTime;
    
    // Tracking state
    bool trackingActive;
    bool stackTraceEnabled;
    bool allocationSiteEnabled;
    double leakThreshold;
    
    // Performance
    std::chrono::system_clock::time_point sessionStartTime;
    
    Impl() {
        totalAllocated = 0;
        totalFreed = 0;
        peakMemoryUsage = 0;
        trackingActive = false;
        stackTraceEnabled = false;
        allocationSiteEnabled = false;
        leakThreshold = 0.1; // 10% threshold
        sessionStartTime = std::chrono::system_clock::now();
    }
};

TestMemoryLeakDetector::TestMemoryLeakDetector() : pImpl(std::make_unique<Impl>()) {}
TestMemoryLeakDetector::~TestMemoryLeakDetector() {
    resetTracking();
}

void* TestMemoryLeakDetector::trackAllocation(size_t size, const std::string& site) {
    if (!pImpl->trackingActive) {
        return nullptr;
    }
    
    // Simulate memory allocation (in real implementation, would allocate actual memory)
    void* address = std::malloc(size);
    
    if (address) {
        std::lock_guard<std::mutex> lock(pImpl->allocationsMutex);
        
        MemoryAllocation alloc(address, size, site);
        if (pImpl->stackTraceEnabled) {
            alloc.callStack = getCurrentCallStack();
        }
        
        pImpl->allocations[address] = alloc;
        pImpl->totalAllocated += size;
        
        if (size > pImpl->peakMemoryUsage) {
            pImpl->peakMemoryUsage = size;
            pImpl->peakMemoryTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now() - pImpl->sessionStartTime);
        }
        
        // Log allocation
        logAllocation(address, size, site);
    }
    
    return address;
}

bool TestMemoryLeakDetector::trackDeallocation(void* address) {
    if (!address || !pImpl->trackingActive) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->allocationsMutex);
    
    auto it = pImpl->allocations.find(address);
    if (it == pImpl->allocations.end()) {
        return false; // Not tracked
    }
    
    // Mark as freed
    it->second.isFreed = true;
    pImpl->totalFreed += it->second.size;
    
    // Log deallocation
    logDeallocation(address);
    
    return true;
}

bool TestMemoryLeakDetector::isTracked(void* address) const {
    std::lock_guard<std::mutex> lock(pImpl->allocationsMutex);
    return pImpl->allocations.find(address) != pImpl->allocations.end();
}

std::vector<MemoryLeak> TestMemoryLeakDetector::detectMemoryLeaks() const {
    return detectMemoryLeaks("all");
}

std::vector<MemoryLeak> TestMemoryLeakDetector::detectMemoryLeaks(const std::string& leakType) const {
    std::lock_guard<std::mutex> lock(pImpl->allocationsMutex);
    
    std::vector<MemoryLeak> leaks;
    
    for (const auto& pair : pImpl->allocations) {
        const auto& alloc = pair.second;
        
        // Check if allocation is not freed
        if (!alloc.isFreed) {
            // Determine leak type based on allocation size or site
            std::string type = leakType == "all" ? determineLeakType(alloc) : leakType;
            
            // Check if size exceeds threshold
            double leakRatio = static_cast<double>(alloc.size) / pImpl->totalAllocated;
            if (leakRatio >= pImpl->leakThreshold) {
                MemoryLeak leak(pair.first, alloc.size, type, 
                               alloc.allocationSite, alloc.callStack, 
                               "Memory leak detected");
                leaks.push_back(leak);
            }
        }
    }
    
    return leaks;
}

size_t TestMemoryLeakDetector::getMemoryLeakCount() const {
    return getMemoryLeakCount("all");
}

size_t TestMemoryLeakDetector::getMemoryLeakCount(const std::string& leakType) const {
    auto leaks = detectMemoryLeaks(leakType);
    return leaks.size();
}

size_t TestMemoryLeakDetector::getTotalAllocatedMemory() const {
    return pImpl->totalAllocated;
}

size_t TestMemoryLeakDetector::getTotalFreedMemory() const {
    return pImpl->totalFreed;
}

size_t TestMemoryLeakDetector::getCurrentMemoryUsage() const {
    std::lock_guard<std::mutex> lock(pImpl->allocationsMutex);
    
    size_t current = 0;
    for (const auto& pair : pImpl->allocations) {
        if (!pair.second.isFreed) {
            current += pair.second.size;
        }
    }
    
    return current;
}

std::chrono::milliseconds TestMemoryLeakDetector::getPeakMemoryUsage() const {
    return std::chrono::milliseconds(pImpl->peakMemoryUsage);
}

std::string TestMemoryLeakDetector::analyzeLeakPattern(const MemoryLeak& leak) const {
    std::stringstream ss;
    ss << "Leak Pattern Analysis:\n";
    ss << "  Leak Address: " << leak.address << "\n";
    ss << "  Leak Size: " << leak.size << " bytes\n";
    ss << "  Leak Type: " << leak.leakType << "\n";
    ss << "  Allocation Site: " << leak.allocationSite << "\n";
    ss << "  Detected Time: " << formatTime(leak.detectedTime) << "\n";
    
    return ss.str();
}

std::map<std::string, size_t> TestMemoryLeakDetector::getLeakTypeDistribution() const {
    std::map<std::string, size_t> distribution;
    
    auto allLeaks = detectMemoryLeaks();
    for (const auto& leak : allLeaks) {
        distribution[leak.leakType]++;
    }
    
    return distribution;
}

std::vector<std::string> TestMemoryLeakDetector::getTopLeakSources(size_t limit) const {
    std::map<std::string, size_t> sourceCounts;
    
    auto allLeaks = detectMemoryLeaks();
    for (const auto& leak : allLeaks) {
        // Extract source from allocation site (simplified)
        std::string source = leak.allocationSite;
        if (source.empty()) source = "unknown";
        sourceCounts[source]++;
    }
    
    // Sort by count (descending)
    std::vector<std::pair<std::string, size_t>> sortedSources(
        sourceCounts.begin(), sourceCounts.end());
    std::sort(sortedSources.begin(), sortedSources.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    std::vector<std::string> topSources;
    for (size_t i = 0; i < std::min(limit, sortedSources.size()); ++i) {
        topSources.push_back(sortedSources[i].first + " (" + 
                           std::to_string(sortedSources[i].second) + " leaks)");
    }
    
    return topSources;
}

std::string TestMemoryLeakDetector::generateMemoryLeakReport() const {
    std::stringstream ss;
    ss << "NLM Memory Leak Detection Report\n";
    ss << "===============================\n\n";
    
    auto allLeaks = detectMemoryLeaks();
    
    ss << "Memory Leak Summary:\n";
    ss << "  Total Leaks Detected: " << allLeaks.size() << "\n";
    ss << "  Total Allocated Memory: " << getTotalAllocatedMemory() << " bytes\n";
    ss << "  Total Freed Memory: " << getTotalFreedMemory() << " bytes\n";
    ss << "  Current Memory Usage: " << getCurrentMemoryUsage() << " bytes\n";
    ss << "  Peak Memory Usage: " << getPeakMemoryUsage().count() << " bytes\n\n";
    
    // Leak type distribution
    auto distribution = getLeakTypeDistribution();
    if (!distribution.empty()) {
        ss << "Leak Type Distribution:\n";
        for (const auto& pair : distribution) {
            ss << "  " << pair.first << ": " << pair.second << " leaks\n";
        }
        ss << "\n";
    }
    
    // Top leak sources
    auto topSources = getTopLeakSources();
    if (!topSources.empty()) {
        ss << "Top Leak Sources:\n";
        for (const auto& source : topSources) {
            ss << "  - " << source << "\n";
        }
        ss << "\n";
    }
    
    // Detailed leaks
    if (!allLeaks.empty()) {
        ss << "Detailed Leaks:\n";
        for (size_t i = 0; i < std::min<size_t>(5, allLeaks.size()); ++i) {
            const auto& leak = allLeaks[i];
            ss << "  Leak " << (i + 1) << ":\n";
            ss << "    Address: " << leak.address << "\n";
            ss << "    Size: " << leak.size << " bytes\n";
            ss << "    Type: " << leak.leakType << "\n";
            ss << "    Site: " << leak.allocationSite << "\n";
            ss << "    Description: " << leak.description << "\n";
        }
        if (allLeaks.size() > 5) {
            ss << "    ... and " << (allLeaks.size() - 5) << " more leaks\n";
        }
        ss << "\n";
    }
    
    return ss.str();
}

std::string TestMemoryLeakDetector::generateDetailedLeakReport() const {
    std::stringstream ss;
    ss << "NLM Detailed Memory Leak Report\n";
    ss << "==============================\n\n";
    
    auto allLeaks = detectMemoryLeaks();
    
    ss << "Memory Allocation History:\n";
    std::lock_guard<std::mutex> lock(pImpl->allocationsMutex);
    
    for (const auto& pair : pImpl->allocations) {
        const auto& alloc = pair.second;
        ss << "  Address: " << alloc.address << "\n";
        ss << "    Size: " << alloc.size << " bytes\n";
        ss << "    Site: " << alloc.allocationSite << "\n";
        ss << "    Status: " << (alloc.isFreed ? "Freed" : "LEAK") << "\n";
        if (!alloc.callStack.empty()) {
            ss << "    Stack Trace:\n";
            ss << "      " << alloc.callStack << "\n";
        }
        ss << "\n";
    }
    
    return ss.str();
}

std::string TestMemoryLeakDetector::generateMemoryUsageReport() const {
    std::stringstream ss;
    ss << "NLM Memory Usage Report\n";
    ss << "========================\n\n";
    
    ss << "Memory Usage Statistics:\n";
    ss << "  Session Start: " << formatTime(pImpl->sessionStartTime) << "\n";
    ss << "  Total Allocated: " << getTotalAllocatedMemory() << " bytes\n";
    ss << "  Total Freed: " << getTotalFreedMemory() << " bytes\n";
    ss << "  Current Usage: " << getCurrentMemoryUsage() << " bytes\n";
    ss << "  Peak Usage: " << getPeakMemoryUsage().count() << " bytes\n";
    ss << "  Peak Usage Time: " << pImpl->peakMemoryTime.count() << " ms\n";
    
    // Memory leak analysis
    auto leaks = detectMemoryLeaks();
    ss << "  Memory Leaks: " << leaks.size() << "\n";
    
    if (!leaks.empty()) {
        ss << "  Memory Leak Percentage: " 
           << (static_cast<double>(getCurrentMemoryUsage()) / getTotalAllocatedMemory() * 100.0) << "%\n";
    }
    
    return ss.str();
}

bool TestMemoryLeakDetector::exportLeakReport(const std::string& filename, const std::string& format) {
    try {
        if (format == "txt") {
            std::ofstream file(filename);
            if (!file.is_open()) {
                return false;
            }
            
            file << generateMemoryLeakReport();
            file.close();
            
        } else if (format == "json") {
            // JSON export implementation
            std::ofstream file(filename);
            if (!file.is_open()) {
                return false;
            }
            
            file << generateMemoryLeakReport();
            file.close();
            
        } else {
            return false;
        }
        
        return true;
        
    } catch (const std::exception&) {
        return false;
    }
}

bool TestMemoryLeakDetector::exportAllocationHistory(const std::string& filename) {
    try {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        std::lock_guard<std::mutex> lock(pImpl->allocationsMutex);
        
        file << "Memory Allocation History\n";
        file << "========================\n\n";
        
        for (const auto& pair : pImpl->allocations) {
            const auto& alloc = pair.second;
            file << "Address: " << alloc.address << "\n";
            file << "Size: " << alloc.size << " bytes\n";
            file << "Site: " << alloc.allocationSite << "\n";
            file << "Status: " << (alloc.isFreed ? "Freed" : "Active") << "\n";
            file << "Time: " << formatTime(alloc.allocationTime) << "\n";
            if (!alloc.callStack.empty()) {
                file << "Stack: " << alloc.callStack << "\n";
            }
            file << "\n";
        }
        
        file.close();
        return true;
        
    } catch (const std::exception&) {
        return false;
    }
}

bool TestMemoryLeakDetector::importAllocationHistory(const std::string& filename) {
    // Implementation for importing allocation history
    return false;
}

void TestMemoryLeakDetector::startMemoryTracking() {
    pImpl->trackingActive = true;
    pImpl->sessionStartTime = std::chrono::system_clock::now();
}

void TestMemoryLeakDetector::stopMemoryTracking() {
    pImpl->trackingActive = false;
}

bool TestMemoryLeakDetector::isMemoryTrackingActive() const {
    return pImpl->trackingActive;
}

void TestMemoryLeakDetector::resetTracking() {
    std::lock_guard<std::mutex> lock(pImpl->allocationsMutex);
    pImpl->allocations.clear();
    pImpl->detectedLeaks.clear();
    pImpl->totalAllocated = 0;
    pImpl->totalFreed = 0;
    pImpl->peakMemoryUsage = 0;
    pImpl->trackingActive = false;
}

void TestMemoryLeakDetector::pauseTracking() {
    pImpl->trackingActive = false;
}

void TestMemoryLeakDetector::resumeTracking() {
    pImpl->trackingActive = true;
    pImpl->sessionStartTime = std::chrono::system_clock::now();
}

void TestMemoryLeakDetector::enableStackTraceRecording(bool enable) {
    pImpl->stackTraceEnabled = enable;
}

void TestMemoryLeakDetector::enableAllocationSiteTracking(bool enable) {
    pImpl->allocationSiteEnabled = enable;
}

void TestMemoryLeakDetector::setLeakDetectionThreshold(double threshold) {
    pImpl->leakThreshold = threshold;
}

std::string TestMemoryLeakDetector::identifyMemoryLeakSources() const {
    std::stringstream ss;
    ss << "Memory Leak Source Analysis:\n";
    
    auto topSources = getTopLeakSources(10);
    for (size_t i = 0; i < topSources.size(); ++i) {
        ss << "  " << (i + 1) << ". " << topSources[i] << "\n";
    }
    
    return ss.str();
}

std::vector<std::string> TestMemoryLeakDetector::getMemoryLeakRecommendations() const {
    std::vector<std::string> recommendations;
    
    auto leaks = detectMemoryLeaks();
    if (leaks.empty()) {
        recommendations.push_back("No memory leaks detected.");
        return recommendations;
    }
    
    // General recommendations
    recommendations.push_back("Review memory allocation/deallocation patterns.");
    recommendations.push_back("Check for missing free() calls or delete operations.");
    recommendations.push_back("Consider using smart pointers to manage memory automatically.");
    recommendations.push_back("Implement memory leak detection in your build system.");
    
    // Specific recommendations based on leak types
    auto distribution = getLeakTypeDistribution();
    for (const auto& pair : distribution) {
        if (pair.first == "allocation_error") {
            recommendations.push_back("Fix allocation errors in " + pair.first + " leaks.");
        } else if (pair.first == "deallocation_error") {
            recommendations.push_back("Fix deallocation errors in " + pair.first + " leaks.");
        }
    }
    
    return recommendations;
}

std::string TestMemoryLeakDetector::getCurrentCallStack() const {
    // Simplified implementation - in real implementation would use platform-specific APIs
    return "Call stack tracking not implemented in this simplified version";
}

std::string TestMemoryLeakDetector::formatAllocationSite(const std::string& site) const {
    if (site.empty()) {
        return "unknown";
    }
    return site;
}

void TestMemoryLeakDetector::logAllocation(void* address, size_t size, const std::string& site) {
    // Logging implementation
    // In real implementation, would log to a file or debug output
}

void TestMemoryLeakDetector::logDeallocation(void* address) {
    // Logging implementation
    // In real implementation, would log to a file or debug output
}

std::string TestMemoryLeakDetector::formatTime(std::chrono::system_clock::time_point time) const {
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - time);
    
    std::stringstream ss;
    ss << duration.count() << " seconds ago";
    return ss.str();
}

std::string TestMemoryLeakDetector::determineLeakType(const MemoryAllocation& alloc) const {
    // Simplified leak type determination
    if (alloc.size > 1024 * 1024) { // > 1MB
        return "large_allocation_leak";
    } else if (alloc.size > 0) {
        return "small_allocation_leak";
    } else {
        return "allocation_error";
    }
}

TestMemoryLeakDetector& TestMemoryLeakDetector::getInstance() {
    static TestMemoryLeakDetector instance;
    return instance;
}

void TestMemoryLeakDetector::initialize() {
    getInstance();
}

void TestMemoryLeakDetector::shutdown() {
    getInstance().resetTracking();
}

} // namespace tests
} // namespace nlm
