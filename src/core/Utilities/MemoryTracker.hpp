// Memory tracking and leak detection implementation

#pragma once

#include "MemoryTracker.hpp"
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

namespace nlm {

class MemoryTracker {
public:
    MemoryTracker();
    ~MemoryTracker();
    
    // Memory tracking
    void trackAllocation(void* pointer, size_t bytes, const std::string& context = "unknown");
    void trackDeallocation(void* pointer);
    void trackReallocation(void* oldPointer, void* newPointer, size_t newSize, const std::string& context = "unknown");
    
    // Memory leak detection
    bool detectMemoryLeaks() const;
    std::vector<std::pair<void*, size_t>> getMemoryLeaks() const;
    std::vector<std::pair<std::string, size_t>> getMemoryLeaksByContext() const;
    
    // Memory usage monitoring
    void updateMemoryUsage();
    uint64_t getCurrentMemoryUsage() const;
    uint64_t getPeakMemoryUsage() const;
    
    // Integration points
    void integrateWithLogger(std::shared_ptr<LoggerManager> loggerManager);
    void integrateWithDebugConsole(std::shared_ptr<IDebugConsole> console);
    void integrateWithSystemMonitor(std::shared_ptr<ISystemMonitor> monitor);
    
    // Configuration
    void setLeakDetectionEnabled(bool enabled);
    void setAllocationTrackingEnabled(bool enabled);
    void setThresholdForLeakDetection(size_t minBytes);
    void addIgnorePattern(const std::string& pattern);
    void removeIgnorePattern(const std::string& pattern);
    
    // Export and analysis
    void exportMemoryReport(const std::string& filepath) const;
    void exportAllocationTimeline(const std::string& filepath) const;
    void clearTracking();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    struct AllocationRecord {
        void* pointer;
        size_t bytes;
        std::string context;
        std::chrono::system_clock::time_point timestamp;
        bool tracked = true;
    };
};