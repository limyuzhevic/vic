#pragma once

#include <chrono>
#include <map>
#include <string>
#include <vector>
#include <atomic>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <filesystem>

namespace nlm {

class EventProfiler;
class MemoryMonitor;

class PerformanceProfiler {
public:
    // Profiling modes
    enum class Mode {
        OFF,           // No profiling
        BASIC,         // Basic timing only
        DETAILED,      // Include memory tracking
        COMPREHENSIVE  // Full profiling with all metrics
    };

    // Time measurement point
    struct TimePoint {
        std::string operation;
        std::chrono::high_resolution_clock::time_point timestamp;
        std::chrono::duration<double> duration;
        int threadId;
        std::map<std::string, std::string> metadata;
    };

    // Performance statistics for an operation
    struct OperationStats {
        std::string name;
        uint64_t callCount{0};
        std::chrono::duration<double> totalDuration{0.0};
        std::chrono::duration<double> minDuration{std::chrono::duration<double>::max()};
        std::chrono::duration<double> maxDuration{0.0};
        std::chrono::duration<double> avgDuration{0.0};
        double stdDev{0.0};
        uint64_t memoryAllocated{0};
        uint64_t memoryDeallocated{0};
        std::vector<double> samples;  // For detailed histograms
    };

    // Memory allocation record
    struct AllocationRecord {
        void* address;
        size_t size;
        std::chrono::high_resolution_clock::time_point allocationTime;
        std::string stackTrace;
        std::string component;
        std::atomic<bool> isFreed{false};
    };

    // Thread statistics
    struct ThreadStats {
        int threadId;
        uint64_t operationsProcessed{0};
        std::chrono::duration<double> activeTime{0.0};
        double utilization{0.0};  // Percentage
    };

    PerformanceProfiler();
    ~PerformanceProfiler();

    // Configuration
    void setMode(Mode mode);
    void enableOperationProfiling(const std::string& operationName, bool enable = true);
    void setMaxHistorySize(size_t maxPoints);
    void setProfilingInterval(double intervalSeconds);

    // Core profiling functions
    void recordTimePoint(const std::string& operation, const std::map<std::string, std::string>& metadata = {});
    void recordAllocation(size_t size, const std::string& component, const std::string& stackTrace = "");
    void recordDeallocation(void* address, const std::string& component);

    // Integration with Brain operations
    void profileStepStart(SimulationStep step, Timestamp time);
    void profileStepEnd(SimulationStep step, Timestamp time);
    void profileNeuronProcessing(size_t neuronCount);
    void profileSynapseProcessing(size_t synapseCount);
    void profilePlasticityApplication(const std::string& ruleName, size_t synapseCount);
    void profileMemorySystem(const std::string& systemName, size_t memoryBytes);
    void profileEventQueue(const std::string& eventType, size_t queueSize, double processingTime);
    void profileSpikeProcessing(size_t spikeCount, double processingTime);
    void profileConcurrentOperation(const std::string& operation, int threadId, double duration);

    // Memory monitoring
    void setMemoryMonitor(MemoryMonitor* monitor);
    void setEventProfiler(EventProfiler* eventProfiler);

    // Statistics queries
    const OperationStats* getOperationStats(const std::string& operationName) const;
    double getTotalSimulationTime() const;
    size_t getTotalTimePoints() const;
    double getMemoryLeakRate() const;
    std::vector<ThreadStats> getThreadStats() const;

    // Reporting and exporting
    void exportToJSON(const std::string& filename) const;
    void exportToCSV(const std::string& filename) const;
    std::string generateReport() const;

    // Control
    void start();
    void stop();
    void reset();
    bool isRunning() const;

private:
    Mode mode_;
    std::atomic<bool> running_{false};
    std::chrono::high_resolution_clock::time_point startTime_;
    std::chrono::duration<double> interval_;

    // Time series data
    std::vector<TimePoint> timePoints_;
    mutable std::mutex timePointsMutex_;

    // Memory tracking
    std::vector<AllocationRecord> allocations_;
    mutable std::mutex allocationsMutex_;

    // Statistics
    std::map<std::string, OperationStats> operationStats_;
    mutable std::mutex statsMutex_;

    // Thread tracking
    std::vector<ThreadStats> threadStats_;
    mutable std::mutex threadStatsMutex_;

    // Integration references
    MemoryMonitor* memoryMonitor_;
    EventProfiler* eventProfiler_;

    // Helper functions
    void updateOperationStats(const std::string& operation, std::chrono::duration<double> duration);
    void cleanupFreedAllocations();
    void updateThreadStats(int threadId, std::chrono::duration<double> duration);
    void exportToJSONInternal(std::ostream& output) const;
    void exportToCSVInternal(std::ostream& output) const;
    void generateReportInternal(std::ostream& output) const;
};
