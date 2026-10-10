#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <mutex>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>

namespace nlm {

// System monitoring base classes
class SystemMonitor {
public:
    virtual ~SystemMonitor() = default;
    virtual void startMonitoring() = 0;
    virtual void stopMonitoring() = 0;
    virtual double getCurrentValue() const = 0;
    virtual std::string getUnit() const = 0;
    virtual std::string getName() const = 0;
};

// CPU usage monitor
class CPUMonitor : public SystemMonitor {
public:
    CPUMonitor();
    ~CPUMonitor() override = default;
    
    void startMonitoring() override;
    void stopMonitoring() override;
    double getCurrentValue() const override;
    std::string getUnit() const override { return "percent"; }
    std::string getName() const override { return "CPU Usage"; }
    
    // Get CPU usage over a specific period
    std::vector<double> getCPUUsageHistory(size_t samples) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Memory usage monitor
class MemoryMonitor : public SystemMonitor {
public:
    MemoryMonitor();
    ~MemoryMonitor() override = default;
    
    void startMonitoring() override;
    void stopMonitoring() override;
    double getCurrentValue() const override;
    std::string getUnit() const override { return "bytes"; }
    std::string getName() const override { return "Memory Usage"; }
    
    // Get memory breakdown
    struct MemoryBreakdown {
        size_t total;
        size_t used;
        size_t available;
        size_t shared;
        size_t buffers;
        size_t cached;
    };
    
    MemoryBreakdown getMemoryBreakdown() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Network I/O monitor
class NetworkIOMonitor : public SystemMonitor {
public:
    NetworkIOMonitor();
    ~NetworkIOMonitor() override = default;
    
    void startMonitoring() override;
    void stopMonitoring() override;
    double getCurrentValue() const override;
    std::string getUnit() const override { return "bytes/second"; }
    std::string getName() const override { return "Network I/O"; }
    
    // Get network statistics
    struct NetworkStats {
        uint64_t bytesSent;
        uint64_t bytesReceived;
        uint64_t packetsSent;
        uint64_t packetsReceived;
        uint64_t errorsSent;
        uint64_t errorsReceived;
    };
    
    NetworkStats getNetworkStats() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Disk I/O monitor
class DiskIOMonitor : public SystemMonitor {
public:
    DiskIOMonitor(const std::string& path = "/");
    ~DiskIOMonitor() override = default;
    
    void startMonitoring() override;
    void stopMonitoring() override;
    double getCurrentValue() const override;
    std::string getUnit() const override { return "bytes/second"; }
    std::string getName() const override { return "Disk I/O"; }
    
    // Get disk statistics
    struct DiskStats {
        uint64_t readBytes;
        uint64_t writeBytes;
        uint64_t readOperations;
        uint64_t writeOperations;
        uint64_t readTime;
        uint64_t writeTime;
    };
    
    DiskStats getDiskStats() const;
    
private:
    std::string path_;
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Process monitor
class ProcessMonitor : public SystemMonitor {
public:
    ProcessMonitor();
    ~ProcessMonitor() override = default;
    
    void startMonitoring() override;
    void stopMonitoring() override;
    double getCurrentValue() const override;
    std::string getUnit() const override { return "percent"; }
    std::string getName() const override { return "Process Usage"; }
    
    // Get process statistics
    struct ProcessStats {
        pid_t pid;
        std::string command;
        double cpuUsage;
        size_t memoryUsage;
        int threadCount;
        time_t startTime;
    };
    
    ProcessStats getProcessStats() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// System monitor registry
class SystemMonitorRegistry {
public:
    static SystemMonitorRegistry& getInstance();
    
    // Register a monitor
    void registerMonitor(std::shared_ptr<SystemMonitor> monitor);
    
    // Unregister a monitor
    void unregisterMonitor(const std::string& name);
    
    // Get monitor by name
    std::shared_ptr<SystemMonitor> getMonitor(const std::string& name);
    
    // Get all registered monitors
    const std::vector<std::shared_ptr<SystemMonitor>>& getAllMonitors() const { return monitors_; }
    
    // Get system health status
    struct SystemHealth {
        double cpuUsage;
        size_t memoryUsage;
        uint64_t networkIO;
        uint64_t diskIO;
        bool isHealthy;
    };
    
    SystemHealth getSystemHealth() const;
    
    // Start all monitors
    void startAllMonitoring();
    
    // Stop all monitors
    void stopAllMonitoring();
    
    // Export system status
    std::string exportSystemStatus() const;
    
private:
    SystemMonitorRegistry() = default;
    std::vector<std::shared_ptr<SystemMonitor>> monitors_;
    mutable std::mutex mutex_;
};

// Memory leak detector
class MemoryLeakDetector {
public:
    MemoryLeakDetector();
    ~MemoryLeakDetector();
    
    // Track memory allocation
    void trackAllocation(void* pointer, size_t size, const std::string& context);
    
    // Track memory deallocation
    void trackDeallocation(void* pointer, const std::string& context);
    
    // Get current memory leaks
    std::vector<std::pair<void*, size_t>> getCurrentLeaks() const;
    
    // Get leak statistics
    struct LeakStats {
        size_t totalLeaks;
        size_t totalLeakedMemory;
        std::string mostCommonContext;
    };
    
    LeakStats getLeakStats() const;
    
    // Check for memory leaks
    bool checkForLeaks() const;
    
    // Export leak report
    std::string exportLeakReport() const;
    
    // Clear all tracked allocations
    void clear();
    
private:
    struct AllocationInfo {
        void* pointer;
        size_t size;
        std::string context;
        std::chrono::system_clock::time_point timestamp;
        bool isFreed;
        
        AllocationInfo(void* ptr, size_t sz, const std::string& ctx)
            : pointer(ptr), size(sz), context(ctx), 
              timestamp(std::chrono::system_clock::now()), isFreed(false) {}
    };
    
    std::vector<AllocationInfo> allocations_;
    mutable std::mutex mutex_;
};

// Resource allocation tracker
class ResourceAllocationTracker {
public:
    ResourceAllocationTracker();
    
    // Track memory allocation
    void trackMemoryAllocation(size_t size, const std::string& context);
    
    // Track memory deallocation
    void trackMemoryDeallocation(size_t size, const std::string& context);
    
    // Track CPU allocation
    void trackCPUAllocation(double time, const std::string& context);
    
    // Track file operations
    void trackFileOperation(const std::string& operation, const std::string& file, const std::string& context);
    
    // Get resource usage statistics
    struct ResourceStats {
        size_t totalMemoryAllocated;
        size_t currentMemoryUsage;
        double totalCPUTime;
        size_t fileOperationCount;
        std::chrono::system_clock::time_point startTime;
    };
    
    ResourceStats getResourceStats() const;
    
    // Export resource usage report
    std::string exportResourceReport() const;
    
private:
    size_t totalMemoryAllocated_;
    size_t currentMemoryUsage_;
    double totalCPUTime_;
    size_t fileOperationCount_;
    std::chrono::system_clock::time_point startTime_;
    mutable std::mutex mutex_;
};

// Reference counting debugger
class ReferenceCountingDebugger {
public:
    ReferenceCountingDebugger();
    
    // Track reference counting operations
    void trackReferenceIncrement(const std::string& objectId, const std::string& context);
    
    // Track reference counting operations
    void trackReferenceDecrement(const std::string& objectId, const std::string& context);
    
    // Get reference counting statistics
    struct ReferenceStats {
        size_t totalIncrements;
        size_t totalDecrements;
        size_t currentReferences;
        std::unordered_map<std::string, size_t> objectCounts;
    };
    
    ReferenceStats getReferenceStats() const;
    
    // Check for potential reference leaks
    bool checkForReferenceLeaks() const;
    
    // Export reference counting report
    std::string exportReferenceReport() const;
    
private:
    size_t totalIncrements_;
    size_t totalDecrements_;
    mutable std::mutex mutex_;
    std::unordered_map<std::string, size_t> objectReferences_;
};

// Thread synchronization debugger
class ThreadSynchronizationDebugger {
public:
    ThreadSynchronizationDebugger();
    
    // Track lock acquisition
    void trackLockAcquisition(const std::string& lockId, const std::string& threadId, const std::string& context);
    
    // Track lock release
    void trackLockRelease(const std::string& lockId, const std::string& threadId, const std::string& context);
    
    // Get synchronization statistics
    struct SyncStats {
        size_t totalLockAcquisitions;
        size_t totalLockReleases;
        std::unordered_map<std::string, size_t> lockCounts;
        std::unordered_map<std::string, size_t> threadLockCounts;
    };
    
    SyncStats getSyncStats() const;
    
    // Check for potential deadlocks
    bool checkForDeadlocks() const;
    
    // Export synchronization report
    std::string exportSyncReport() const;
    
private:
    size_t totalLockAcquisitions_;
    size_t totalLockReleases_;
    mutable std::mutex mutex_;
    std::unordered_map<std::string, size_t> lockCounts_;
    std::unordered_map<std::string, size_t> threadLockCounts_;
};

// Plugin/system integration debugger
class IntegrationDebugger {
public:
    IntegrationDebugger();
    
    // Track plugin loading
    void trackPluginLoad(const std::string& pluginName, bool success, const std::string& context);
    
    // Track plugin unloading
    void trackPluginUnload(const std::string& pluginName, bool success, const std::string& context);
    
    // Track system service access
    void trackSystemAccess(const std::string& serviceName, bool success, const std::string& context);
    
    // Get integration statistics
    struct IntegrationStats {
        size_t totalPluginLoads;
        size_t totalPluginUnloads;
        size_t successfulPluginLoads;
        size_t successfulPluginUnloads;
        size_t totalSystemAccesses;
        size_t successfulSystemAccesses;
        std::unordered_map<std::string, size_t> pluginLoadCounts;
        std::unordered_map<std::string, size_t> systemAccessCounts;
    };
    
    IntegrationStats getIntegrationStats() const;
    
    // Check for integration issues
    bool checkForIntegrationIssues() const;
    
    // Export integration report
    std::string exportIntegrationReport() const;
    
private:
    size_t totalPluginLoads_;
    size_t totalPluginUnloads_;
    size_t successfulPluginLoads_;
    size_t successfulPluginUnloads_;
    size_t totalSystemAccesses_;
    size_t successfulSystemAccesses_;
    mutable std::mutex mutex_;
    std::unordered_map<std::string, size_t> pluginLoadCounts_;
    std::unordered_map<std::string, size_t> systemAccessCounts_;
};

} // namespace nlm