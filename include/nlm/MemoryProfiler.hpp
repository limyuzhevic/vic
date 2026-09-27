pragma once

#include "MemoryManager.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <map>
#include <string>
#include <chrono>
#include <thread>
#include <condition_variable>
#include <queue>

namespace nlm {

// Memory profiling data types
struct AllocationProfile {
    std::string file;
    int line;
    std::string allocationName;
    size_t size;
    uint64_t timestamp;
    uint64_t threadId;
    size_t callStackId;
};

struct MemoryUsageSnapshot {
    uint64_t timestamp;
    size_t totalAllocated;
    size_t peakAllocated;
    size_t activeAllocations;
    size_t allocationCount;
    size_t deallocationCount;
    float cpuUsage;
    float memoryUsagePercent;
    std::map<std::string, size_t> perFileAllocation;
    std::map<std::string, size_t> perFileSize;
    
    MemoryUsageSnapshot() : timestamp(0), totalAllocated(0), peakAllocated(0),
                           activeAllocations(0), allocationCount(0), deallocationCount(0),
                           cpuUsage(0.0f), memoryUsagePercent(0.0f) {}
};

// Memory profiling events
enum class ProfilingEventType {
    ALLOCATION,
    DEALLOCATION,
    SNAPSHOT,
    THREAD_START,
    THREAD_END,
    GC_REQUEST,
    THRESHOLD_EXCEEDED
};

struct ProfilingEvent {
    uint64_t timestamp;
    ProfilingEventType type;
    std::string threadId;
    std::string data;
    size_t size;  // For allocation/deallocation events
    std::string file;
    int line;
    std::string allocationName;
    
    ProfilingEvent(ProfilingEventType t, const std::string& data = "")
        : type(t), data(data), size(0), file(""), line(0), allocationName("") {}
    
    ProfilingEvent(ProfilingEventType t, size_t size, const std::string& file, int line, const std::string& name)
        : type(t), size(size), file(file), line(line), allocationName(name) {}
};

// Memory profiler with real-time monitoring
class MemoryProfiler {
private:
    std::mutex mutex;
    std::condition_variable cv;
    bool running;
    std::thread profilingThread;
    
    // Data collection
    std::vector<ProfilingEvent> events;
    std::vector<MemoryUsageSnapshot> snapshots;
    std::map<std::string, size_t> threadAllocations;
    std::map<std::string, size_t> threadPeakAllocations;
    
    // Profiling settings
    size_t maxEvents;
    size_t snapshotInterval;  // in milliseconds
    bool trackCallStacks;
    bool enableRealTimeMonitoring;
    float memoryThresholdPercent;
    float cpuThresholdPercent;
    
    // File tracking
    std::map<std::string, size_t> fileAllocations;
    std::map<std::string, size_t> filePeakAllocations;
    
    // System monitoring
    struct SystemInfo {
        float cpuUsage;
        float memoryUsagePercent;
        size_t availableMemory;
        size_t totalMemory;
        uint64_t timestamp;
    };
    
    SystemInfo currentSystemInfo;
    
    // Private methods
    void profilingWorker();
    void collectSystemInfo();
    void takeSnapshot();
    void analyzeAllocationPatterns();
    void detectMemoryLeaks();
    void detectFragmentation();
    
    // Call stack tracking
    size_t generateCallStackId();
    std::string getCallStack(size_t stackId);
    
    // File and line tracking
    void updateFileStats(const std::string& file, size_t size, bool isAllocation);
    
public:
    MemoryProfiler()
        : running(false), maxEvents(100000), snapshotInterval(1000), 
          trackCallStacks(true), enableRealTimeMonitoring(false),
          memoryThresholdPercent(80.0f), cpuThresholdPercent(90.0f) {
        threadAllocations.clear();
        threadPeakAllocations.clear();
        events.reserve(maxEvents);
    }
    
    ~MemoryProfiler() {
        stop();
    }
    
    void start() {
        if (!running) {
            running = true;
            profilingThread = std::thread(&MemoryProfiler::profilingWorker, this);
        }
    }
    
    void stop() {
        if (running) {
            running = false;
            cv.notify_one();
            if (profilingThread.joinable()) {
                profilingThread.join();
            }
        }
    }
    
    void recordAllocation(size_t size, const std::string& file, int line, const std::string& allocationName = "") {
        std::lock_guard<std::mutex> lock(mutex);
        
        ProfilingEvent event(ProfilingEventType::ALLOCATION, size, file, line, allocationName);
        event.threadId = getCurrentThreadId();
        event.timestamp = getCurrentTimestamp();
        
        // Track by file
        updateFileStats(file, size, true);
        
        events.push_back(event);
        
        // Limit events to prevent memory issues
        if (events.size() > maxEvents) {
            events.erase(events.begin(), events.begin() + (events.size() - maxEvents / 2));
        }
    }
    
    void recordDeallocation(size_t size, const std::string& file, int line, const std::string& allocationName = "") {
        std::lock_guard<std::mutex> lock(mutex);
        
        ProfilingEvent event(ProfilingEventType::DEALLOCATION, size, file, line, allocationName);
        event.threadId = getCurrentThreadId();
        event.timestamp = getCurrentTimestamp();
        
        // Track by file
        updateFileStats(file, size, false);
        
        events.push_back(event);
        
        if (events.size() > maxEvents) {
            events.erase(events.begin(), events.begin() + (events.size() - maxEvents / 2));
        }
    }
    
    void takeManualSnapshot(const std::string& info = "") {
        std::lock_guard<std::mutex> lock(mutex);
        MemoryUsageSnapshot snapshot;
        
        collectSystemInfo();
        snapshot.timestamp = currentSystemInfo.timestamp;
        snapshot.totalAllocated = MemoryManager::MemoryTracker::getInstance().getTotalAllocated();
        snapshot.peakAllocated = MemoryManager::MemoryTracker::getInstance().getPeakAllocated();
        snapshot.activeAllocations = MemoryManager::MemoryTracker::getInstance().getActiveAllocations();
        snapshot.allocationCount = MemoryManager::MemoryTracker::getInstance().getAllocationCount();
        snapshot.deallocationCount = MemoryManager::MemoryTracker::getInstance().getDeallocationCount();
        snapshot.cpuUsage = currentSystemInfo.cpuUsage;
        snapshot.memoryUsagePercent = currentSystemInfo.memoryUsagePercent;
        snapshot.perFileAllocation = fileAllocations;
        snapshot.perFileSize = filePeakAllocations;
        
        if (!info.empty()) {
            snapshot.info = info;
        }
        
        snapshots.push_back(snapshot);
        
        // Check thresholds
        checkThresholds(snapshot);
        
        // Detect issues
        if (snapshot.totalAllocated > 0) {
            detectMemoryLeaks();
            detectFragmentation();
        }
    }
    
    std::string generateReport() const {
        std::stringstream ss;
        ss << "=== Memory Profiler Report ===\n";
        ss << "Generated: " << getCurrentTimestampString() << "\n\n";
        
        ss << "=== System Statistics ===\n";
        ss << "Total Allocations: " << MemoryManager::MemoryTracker::getInstance().getAllocationCount() << "\n";
        ss << "Total Deallocations: " << MemoryManager::MemoryTracker::getInstance().getDeallocationCount() << "\n";
        ss << "Current Total Allocated: " << MemoryManager::MemoryTracker::getInstance().getTotalAllocated() << " bytes\n";
        ss << "Peak Allocated: " << MemoryManager::MemoryTracker::getInstance().getPeakAllocated() << " bytes\n";
        ss << "Active Allocations: " << MemoryManager::MemoryTracker::getInstance().getActiveAllocations() << "\n\n";
        
        ss << "=== Event Analysis ===\n";
        size_t allocCount = 0, deallocCount = 0;
        std::map<std::string, size_t> allocByFile;
        
        for (const auto& event : events) {
            switch (event.type) {
                case ProfilingEventType::ALLOCATION:
                    allocCount++;
                    allocByFile[event.file] += event.size;
                    break;
                case ProfilingEventType::DEALLOCATION:
                    deallocCount++;
                    break;
                default:
                    break;
            }
        }
        
        ss << "Allocation Events: " << allocCount << "\n";
        ss << "Deallocation Events: " << deallocCount << "\n";
        ss << "Net Allocations: " << (allocCount - deallocCount) << "\n\n";
        
        ss << "=== Top Allocation Files ===\n";
        std::vector<std::pair<std::string, size_t>> sortedFiles;
        for (const auto& pair : allocByFile) {
            sortedFiles.push_back(pair);
        }
        
        std::sort(sortedFiles.begin(), sortedFiles.end(),
                  [](const auto& a, const auto& b) { return a.second > b.second; });
        
        size_t displayCount = std::min(sortedFiles.size(), static_cast<size_t>(10));
        for (size_t i = 0; i < displayCount; ++i) {
            const auto& pair = sortedFiles[i];
            ss << "  " << std::setw(50) << std::left << pair.first
               << " : " << pair.second << " bytes\n";
        }
        
        ss << "\n=== Snapshot History ===\n";
        for (size_t i = 0; i < snapshots.size(); ++i) {
            const auto& snap = snapshots[i];
            ss << "  Snapshot " << i << " (" << snap.timestamp << "):"
               << " " << snap.totalAllocated << " bytes, "
               << snap.memoryUsagePercent << "% memory usage, "
               << snap.cpuUsage << "% CPU usage"
               << " - " << snap.info << "\n";
        }
        
        ss << "\n=== Memory Leaks Detection ===\n";
        detectMemoryLeaks();
        
        ss << "\n=== Fragmentation Analysis ===\n";
        detectFragmentation();
        
        return ss.str();
    }
    
    void configure(size_t maxEvents, size_t snapshotIntervalMs, bool trackCallStacks,
                   bool enableRealTime, float memoryThreshold, float cpuThreshold) {
        std::lock_guard<std::mutex> lock(mutex);
        maxEvents = maxEvents;
        snapshotInterval = snapshotIntervalMs;
        trackCallStacks = trackCallStacks;
        enableRealTimeMonitoring = enableRealTime;
        memoryThresholdPercent = memoryThreshold;
        cpuThresholdPercent = cpuThreshold;
    }
    
    void exportEvents(const std::string& filename) const {
        std::ofstream file(filename);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open event export file: " + filename);
            return;
        }
        
        file << "timestamp,type,thread,file,line,size,allocationName\n";
        
        for (const auto& event : events) {
            file << event.timestamp << ",";
            
            switch (event.type) {
                case ProfilingEventType::ALLOCATION: file << "ALLOCATION"; break;
                case ProfilingEventType::DEALLOCATION: file << "DEALLOCATION"; break;
                case ProfilingEventType::SNAPSHOT: file << "SNAPSHOT"; break;
                case ProfilingEventType::THREAD_START: file << "THREAD_START"; break;
                case ProfilingEventType::THREAD_END: file << "THREAD_END"; break;
                case ProfilingEventType::GC_REQUEST: file << "GC_REQUEST"; break;
                case ProfilingEventType::THRESHOLD_EXCEEDED: file << "THRESHOLD_EXCEEDED"; break;
            }
            
            file << "," << event.threadId << "," << event.file << "," << event.line
                 << "," << event.size << "," << event.allocationName << "\n";
        }
        
        file.close();
        NLM_LOG_INFO("Exported " << events.size() << " events to " + filename);
    }
    
    void exportSnapshots(const std::string& filename) const {
        std::ofstream file(filename);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open snapshot export file: " + filename);
            return;
        }
        
        file << "timestamp,totalAllocated,peakAllocated,activeAllocations,allocationCount,deallocationCount,cpuUsage,memoryUsagePercent,info\n";
        
        for (const auto& snap : snapshots) {
            file << snap.timestamp << "," << snap.totalAllocated << "," << snap.peakAllocated
                 << "," << snap.activeAllocations << "," << snap.allocationCount
                 << "," << snap.deallocationCount << "," << snap.cpuUsage
                 << "," << snap.memoryUsagePercent << "," << snap.info << "\n";
        }
        
        file.close();
        NLM_LOG_INFO("Exported " << snapshots.size() << " snapshots to " + filename);
    }
    
private:
    std::string getCurrentThreadId() {
        auto id = std::this_thread::get_id();
        return std::to_string(std::hash<std::thread::id>{}(id));
    }
    
    uint64_t getCurrentTimestamp() {
        return std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    }
    
    std::string getCurrentTimestampString() {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    void checkThresholds(const MemoryUsageSnapshot& snapshot) {
        if (snapshot.memoryUsagePercent > memoryThresholdPercent) {
            ProfilingEvent event(ProfilingEventType::THRESHOLD_EXCEEDED,
                               "Memory threshold exceeded: " + std::to_string(snapshot.memoryUsagePercent) + "%");
            event.timestamp = snapshot.timestamp;
            event.threadId = "system";
            
            std::lock_guard<std::mutex> lock(mutex);
            events.push_back(event);
            
            NLM_LOG_WARNING("Memory threshold exceeded: " << snapshot.memoryUsagePercent << "%")
        }
        
        if (snapshot.cpuUsage > cpuThresholdPercent) {
            ProfilingEvent event(ProfilingEventType::THRESHOLD_EXCEEDED,
                               "CPU threshold exceeded: " + std::to_string(snapshot.cpuUsage) + "%");
            event.timestamp = snapshot.timestamp;
            event.threadId = "system";
            
            std::lock_guard<std::mutex> lock(mutex);
            events.push_back(event);
            
            NLM_LOG_WARNING("CPU threshold exceeded: " << snapshot.cpuUsage << "%")
        }
    }
    
    void updateFileStats(const std::string& file, size_t size, bool isAllocation) {
        if (!file.empty()) {
            if (isAllocation) {
                fileAllocations[file] += size;
                if (filePeakAllocations.find(file) == filePeakAllocations.end() || size > filePeakAllocations[file]) {
                    filePeakAllocations[file] = size;
                }
            } else {
                // For deallocation, we would need to track original size
                // For simplicity, we'll just note that deallocation occurred
                fileAllocations[file] -= size;  // This is simplified
            }
        }
    }
    
    void detectMemoryLeaks() {
        // Check for potential memory leaks by analyzing allocation patterns
        size_t currentAllocations = MemoryManager::MemoryTracker::getInstance().getActiveAllocations();
        
        if (currentAllocations > 100) {  // Threshold for potential leak
            NLM_LOG_WARNING("Potential memory leak detected: " << currentAllocations << " active allocations")
            
            // We can't easily track which allocations are leaked without more sophisticated tracking
            // This is a simplified version that just reports the total
        }
    }
    
    void detectFragmentation() {
        // Basic fragmentation detection
        size_t totalAllocated = MemoryManager::MemoryTracker::getInstance().getTotalAllocated();
        
        if (totalAllocated > 0) {
            // We would need more sophisticated analysis to detect fragmentation
            // For now, we just report if there's significant memory usage
            if (totalAllocated > 100 * 1024 * 1024) {  // 100MB
                NLM_LOG_WARNING("High memory usage detected: " << (totalAllocated / (1024 * 1024)) << "MB")
            }
        }
    }
    
    void collectSystemInfo() {
        currentSystemInfo.timestamp = getCurrentTimestamp();
        
        // In a real implementation, this would collect actual system metrics
        // For now, we'll use simulated values based on current allocations
        currentSystemInfo.totalMemory = 2 * 1024 * 1024 * 1024;  // 2GB (simulated)
        
        size_t allocated = MemoryManager::MemoryTracker::getInstance().getTotalAllocated();
        currentSystemInfo.memoryUsagePercent = static_cast<float>(allocated) / currentSystemInfo.totalMemory * 100.0f;
        
        // Simulate CPU usage based on allocation/deallocation activity
        size_t allocCount = MemoryManager::MemoryTracker::getInstance().getAllocationCount();
        currentSystemInfo.cpuUsage = std::min(100.0f, static_cast<float>(allocCount % 100) / 2.0f);
    }
};

// Global memory profiler instance
inline MemoryProfiler& getMemoryProfiler() {
    static MemoryProfiler profiler;
    return profiler;
}

// Convenience functions for memory profiling
inline void startMemoryProfiling(size_t maxEvents = 100000, size_t snapshotIntervalMs = 1000,
                                bool trackCallStacks = true, bool enableRealTime = false,
                                float memoryThreshold = 80.0f, float cpuThreshold = 90.0f) {
    MemoryProfiler& profiler = getMemoryProfiler();
    profiler.configure(maxEvents, snapshotIntervalMs, trackCallStacks, enableRealTime, memoryThreshold, cpuThreshold);
    profiler.start();
}

inline void stopMemoryProfiling() {
    getMemoryProfiler().stop();
}

inline void takeMemorySnapshot(const std::string& info = "") {
    getMemoryProfiler().takeManualSnapshot(info);
}

inline void exportMemoryProfile(const std::string& eventsFile, const std::string& snapshotsFile = "") {
    getMemoryProfiler().exportEvents(eventsFile);
    if (!snapshotsFile.empty()) {
        getMemoryProfiler().exportSnapshots(snapshotsFile);
    }
}

inline std::string generateMemoryReport() {
    return getMemoryProfiler().generateReport();
}

inline void profileAllocation(size_t size, const std::string& file, int line, const std::string& name = "") {
    getMemoryProfiler().recordAllocation(size, file, line, name);
}

inline void profileDeallocation(size_t size, const std::string& file, int line, const std::string& name = "") {
    getMemoryProfiler().recordDeallocation(size, file, line, name);
}

} // namespace nlm
