#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <algorithm>
#include <unordered_set>
#include "DebuggingSystem.hpp"

namespace nlm {

// Memory allocation tracking
struct MemoryAllocation {
    std::chrono::system_clock::time_point timestamp;
    size_t size;
    std::string context;
    std::string allocationSite;
    uint64_t threadId;
    
    MemoryAllocation(size_t size, const std::string& context, const std::string& site)
        : size(size), context(context), allocationSite(site),
          threadId(std::hash<std::thread::id>{}(std::this_thread::get_id())) {
        timestamp = std::chrono::system_clock::now();
    }
};

// Memory deallocation tracking
struct MemoryDeallocation {
    std::chrono::system_clock::time_point timestamp;
    size_t size;
    std::string context;
    std::string deallocationSite;
    uint64_t threadId;
    
    MemoryDeallocation(size_t size, const std::string& context, const std::string& site)
        : size(size), context(context), deallocationSite(site),
          threadId(std::hash<std::thread::id>{}(std::this_thread::get_id())) {
        timestamp = std::chrono::system_clock::now();
    }
};

// Memory snapshot for leak detection
struct MemorySnapshot {
    std::chrono::system_clock::time_point timestamp;
    size_t totalAllocated;
    size_t totalFreed;
    size_t currentUsage;
    std::unordered_map<std::string, size_t> allocationsByContext;
    std::unordered_map<std::string, size_t> deallocationsByContext;
    
    MemorySnapshot() : totalAllocated(0), totalFreed(0), currentUsage(0) {
        timestamp = std::chrono::system_clock::now();
    }
};

// MemoryLeakInfo: Information about a detected memory leak
struct MemoryLeakInfo {
    std::string context;
    size_t leakedSize;
    size_t allocationCount;
    std::vector<MemoryAllocation> allocations;
    std::string suspectedSite;
    std::chrono::system_clock::time_point firstAllocation;
    
    MemoryLeakInfo(const std::string& ctx, size_t size, size_t count,
                   const std::vector<MemoryAllocation>& allocs,
                   const std::string& site)
        : context(ctx), leakedSize(size), allocationCount(count),
          allocations(allocs), suspectedSite(site) {
        if (!allocs.empty()) {
            firstAllocation = allocs.front().timestamp;
        }
    }
};

// MemoryProfiler: Memory usage and leak detection
class MemoryProfiler {
public:
    MemoryProfiler();
    explicit MemoryProfiler(std::shared_ptr<DebuggingSystem> debuggingSystem);
    ~MemoryProfiler();
    
    // Configuration
    void setMemoryCheckInterval(size_t interval) { memoryCheckInterval_ = interval; }
    void setDetectMemoryLeaks(bool enable) { detectMemoryLeaks_ = enable; }
    void setLeakThreshold(size_t threshold) { leakThreshold_ = threshold; }
    void setMaxSnapshots(size_t max) { maxSnapshots_ = max; }
    
    // Memory tracking
    void recordAllocation(size_t size, const std::string& context = "",
                         const std::string& site = "");
    void recordDeallocation(size_t size, const std::string& context = "",
                           const std::string& site = "");
    
    // Snapshot management
    void takeSnapshot();
    std::vector<MemorySnapshot> getSnapshots() const { return snapshots_; }
    size_t getSnapshotCount() const { return snapshots_.size(); }
    
    // Leak detection
    std::vector<MemoryLeakInfo> detectMemoryLeaks() const;
    std::string getLeakReport() const;
    
    // Memory analysis
    size_t getTotalAllocated() const { return totalAllocated_; }
    size_t getTotalFreed() const { return totalFreed_; }
    size_t getCurrentUsage() const { return currentUsage_; }
    
    // Context analysis
    std::unordered_map<std::string, size_t> getAllocationStats() const;
    std::unordered_map<std::string, size_t> getDeallocationStats() const;
    
    // Integration
    void setDebuggingSystem(std::shared_ptr<DebuggingSystem> debuggingSystem) { debuggingSystem_ = debuggingSystem; }
    
    // Export
    bool exportMemoryProfile(const std::string& filepath) const;
    std::string exportMemoryProfileAsString() const;
    
    // Leak tracking
    void addLeakedAllocation(const MemoryAllocation& allocation);
    void removeLeakedAllocation(const MemoryAllocation& allocation);
    
private:
    std::vector<MemoryAllocation> allocations_;
    std::vector<MemoryDeallocation> deallocations_;
    std::vector<MemorySnapshot> snapshots_;
    
    mutable std::mutex allocationsMutex_;
    mutable std::mutex deallocationsMutex_;
    mutable std::mutex snapshotsMutex_;
    mutable std::mutex leaksMutex_;
    
    std::shared_ptr<DebuggingSystem> debuggingSystem_;
    
    size_t totalAllocated_;
    size_t totalFreed_;
    size_t currentUsage_;
    
    size_t memoryCheckInterval_;
    bool detectMemoryLeaks_;
    size_t leakThreshold_;
    size_t maxSnapshots_;
    
    std::vector<MemoryLeakInfo> currentLeaks_;
    
    // Helper methods
    void updateStats();
    bool isMemoryLeak(const MemoryAllocation& alloc) const;
    std::string getAllocationSite(const std::string& context) const;
    double calculateLeakScore(const MemoryLeakInfo& leak) const;
};

// DebugVisualizer: Interactive visualization of debug data
class DebugVisualizer {
public:
    DebugVisualizer();
    ~DebugVisualizer();
    
    // Configuration
    void setPort(uint16_t port) { port_ = port; }
    void setHost(const std::string& host) { host_ = host; }
    void setEnableWebInterface(bool enable) { enableWebInterface_ = enable; }
    void setUpdateInterval(double interval) { updateInterval_ = interval; }
    
    // Server control
    void startServer();
    void stopServer();
    bool isServerRunning() const { return serverRunning_; }
    
    // Visualization data
    void setDebuggingSystem(std::shared_ptr<DebuggingSystem> debuggingSystem) { debuggingSystem_ = debuggingSystem; }
    void setPerformanceProfiler(std::shared_ptr<PerformanceProfiler> profiler) { performanceProfiler_ = profiler; }
    void setMemoryProfiler(std::shared_ptr<MemoryProfiler> profiler) { memoryProfiler_ = profiler; }
    void setEventLogger(std::shared_ptr<EventLogger> logger) { eventLogger_ = logger; }
    
    // Web interface
    std::string generateDashboardHTML() const;
    std::string generateTraceView() const;
    std::string generatePerformanceView() const;
    std::string generateMemoryView() const;
    std::string generateEventView() const;
    
    // API endpoints
    std::string getAPIDebugInfo() const;
    std::string getAPIPerformanceData() const;
    std::string getAPIMemoryData() const;
    std::string getAPIEventsData() const;
    
    // Export
    bool exportVisualization(const std::string& directory) const;
    
    // State
    bool isWebInterfaceEnabled() const { return enableWebInterface_; }
    
private:
    std::shared_ptr<DebuggingSystem> debuggingSystem_;
    std::shared_ptr<PerformanceProfiler> performanceProfiler_;
    std::shared_ptr<MemoryProfiler> memoryProfiler_;
    std::shared_ptr<EventLogger> eventLogger_;
    
    uint16_t port_;
    std::string host_;
    bool enableWebInterface_;
    double updateInterval_;
    
    bool serverRunning_;
    std::thread serverThread_;
    mutable std::mutex serverMutex_;
    
    // Helper methods
    std::string generateHTMLHeader() const;
    std::string generateHTMLFooter() const;
    std::string generateEventChart(const std::vector<SimulationEvent>& events) const;
    std::string generateMemoryChart(const std::vector<MemorySnapshot>& snapshots) const;
    std::string generatePerformanceChart(const std::vector<std::string>& operations) const;
    std::string escapeHTML(const std::string& text) const;
};

} // namespace nlm
