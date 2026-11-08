#pragma once

#include <chrono>
#include <string>
#include <unordered_map>
#include <vector>
#include <mutex>
#include "../core/Logger/Logger.hpp"
#include "MemoryPool.hpp"
#include "EventQueue.hpp"
#include "SparseConnectivity.hpp"
#include "ParallelProcessing.hpp"

namespace nlm {

class PerformanceMonitor {
public:
    struct MemoryStats {
        size_t allocatedNeurons;
        size_t freeNeurons;
        size_t allocatedSynapses;
        size_t freeSynapses;
        size_t memoryPoolHits;
        size_t memoryPoolMisses;
        double memoryAllocationTime;
    };

    struct EventStats {
        size_t queueSize;
        size_t overflowCount;
        size_t undercountCount;
        double spikeProcessingLatency;
        size_t totalSpikeEvents;
        double averageProcessingTime;
    };

    struct CpuStats {
        double cpuUsage;
        double memoryUsage;
        double peakMemoryUsage;
        size_t activeThreads;
    };

    struct SimulationStats {
        uint64_t currentStep;
        Timestamp currentTime;
        size_t activeNeurons;
        size_t totalSpikes;
        double averageFiringRate;
        double maxFiringRate;
    };

    struct PerformanceMetrics {
        MemoryStats memory;
        EventStats event;
        CpuStats cpu;
        SimulationStats simulation;
        double systemLoad;
        double networkLatency;
    };

    PerformanceMonitor();
    ~PerformanceMonitor();

    // Core monitoring methods
    void update(float timestep);
    void recordMemoryStats();
    void recordEventStats();
    void recordCpuStats();
    void recordSimulationStats(uint64_t step, Timestamp time, size_t activeNeurons, size_t totalSpikes);

    // Memory monitoring integration
    void onMemoryAllocation(size_t size, bool hit);
    void onMemoryFree(size_t size);

    // Event monitoring integration
    void onEventProcessed(double latency);
    void onEventOverflow();
    void onEventUndercount();

    // Thread safety
    PerformanceMetrics getCurrentMetrics() const;
    std::string getPerformanceReport() const;
    void reset();

    // Alert system
    void setMemoryThreshold(size_t threshold);
    void setEventThreshold(size_t threshold);
    void setCpuThreshold(double threshold);

    // Alert system
    void setMemoryThreshold(size_t threshold);
    void setEventThreshold(size_t threshold);
    void setCpuThreshold(double threshold);

    // Statistics
    size_t getAlertCount() const;
    std::chrono::milliseconds getMonitoringInterval() const;

    // Report generation
    std::string getReport() const;
    std::string getDetailedReport(const std::string& format = "text") const;

private:
    // Performance metrics aggregation
    mutable std::mutex metricsMutex;
    PerformanceMetrics currentMetrics;
    PerformanceMetrics previousMetrics;

    // Time tracking
    std::chrono::steady_clock::time_point lastUpdateTime;
    std::chrono::milliseconds monitoringInterval;

    // Thresholds and alerts
    size_t memoryThreshold;
    size_t eventThreshold;
    double cpuThreshold;
    size_t alertCount;

    // Time series data for trend analysis
    struct TimePoint {
        std::chrono::steady_clock::time_point timestamp;
        PerformanceMetrics metrics;
    };

    std::vector<TimePoint> metricHistory;
    size_t maxHistorySize;

    // Helper methods
    double calculatePercentageChange(double previous, double current) const;
    bool checkThresholds() const;
    void generateAlerts(const std::string& category, double value) const;
    std::string generateTextReport() const;
    std::string generateJsonReport() const;
    std::string generateCsvReport() const;
};
};

} // namespace nlm