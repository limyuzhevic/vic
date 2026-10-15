// Test Memory Leak Detector
// Memory leak detection during test execution

#ifndef TEST_MEMORY_LEAK_DETECTOR_HPP
#define TEST_MEMORY_LEAK_DETECTOR_HPP

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <unordered_map>
#include <memory>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <algorithm>

namespace nlm {
namespace tests {

// Memory allocation tracking
struct MemoryAllocation {
    void* address;
    size_t size;
    std::chrono::system_clock::time_point allocationTime;
    std::string allocationSite;
    std::string callStack;
    bool isFreed;
    
    MemoryAllocation(void* addr, size_t sz, const std::string& site)
        : address(addr), size(sz), allocationSite(site), isFreed(false) {
        allocationTime = std::chrono::system_clock::now();
    }
};

// Memory leak information
struct MemoryLeak {
    std::string leakType;
    void* address;
    size_t size;
    std::chrono::system_clock::time_point detectedTime;
    std::string allocationSite;
    std::string callStack;
    std::string description;
    
    MemoryLeak(void* addr, size_t sz, const std::string& type, const std::string& site,
               const std::string& stack, const std::string& desc)
        : leakType(type), address(addr), size(sz), allocationSite(site), 
          callStack(stack), description(desc) {
        detectedTime = std::chrono::system_clock::now();
    }
};

// Memory leak detector
class TestMemoryLeakDetector {
public:
    TestMemoryLeakDetector();
    ~TestMemoryLeakDetector();
    
    // Memory tracking
    void* trackAllocation(size_t size, const std::string& site = "unknown");
    bool trackDeallocation(void* address);
    bool isTracked(void* address) const;
    
    // Leak detection
    std::vector<MemoryLeak> detectMemoryLeaks() const;
    std::vector<MemoryLeak> detectMemoryLeaks(const std::string& leakType) const;
    size_t getMemoryLeakCount() const;
    size_t getMemoryLeakCount(const std::string& leakType) const;
    
    // Memory statistics
    size_t getTotalAllocatedMemory() const;
    size_t getTotalFreedMemory() const;
    size_t getCurrentMemoryUsage() const;
    std::chrono::milliseconds getPeakMemoryUsage() const;
    
    // Leak analysis
    std::string analyzeLeakPattern(const MemoryLeak& leak) const;
    std::map<std::string, size_t> getLeakTypeDistribution() const;
    std::vector<std::string> getTopLeakSources(size_t limit = 10) const;
    
    // Reporting
    std::string generateMemoryLeakReport() const;
    std::string generateDetailedLeakReport() const;
    std::string generateMemoryUsageReport() const;
    
    // File operations
    bool exportLeakReport(const std::string& filename, const std::string& format = "txt");
    bool exportAllocationHistory(const std::string& filename);
    bool importAllocationHistory(const std::string& filename);
    
    // Test integration
    void startMemoryTracking();
    void stopMemoryTracking();
    bool isMemoryTrackingActive() const;
    void resetTracking();
    void pauseTracking();
    void resumeTracking();
    
    // Advanced features
    void enableStackTraceRecording(bool enable);
    void enableAllocationSiteTracking(bool enable);
    void setLeakDetectionThreshold(double threshold);
    
    // Analysis
    std::string identifyMemoryLeakSources() const;
    std::vector<std::string> getMemoryLeakRecommendations() const;
    
    // Singleton pattern
    static TestMemoryLeakDetector& getInstance();
    static void initialize();
    static void shutdown();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Private constructor and destructor
    TestMemoryLeakDetector(const TestMemoryLeakDetector&) = delete;
    TestMemoryLeakDetector& operator=(const TestMemoryLeakDetector&) = delete;
    
    // Helper functions
    std::string getCurrentCallStack() const;
    std::string formatAllocationSite(const std::string& site) const;
    void logAllocation(void* address, size_t size, const std::string& site);
    void logDeallocation(void* address);
};

} // namespace tests
} // namespace nlm

#endif // TEST_MEMORY_LEAK_DETECTOR_HPP
