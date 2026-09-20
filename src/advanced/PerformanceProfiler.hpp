#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <sstream>
#include <set>
#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../performance/Performance.hpp"
#include "EventTracking.hpp"

namespace nlm {

// Performance metrics types
struct PerformanceMetrics {
    double cpuUsage;              // Percentage (0-100)
    double memoryUsage;           // MB
    double memoryUsagePercent;    // Percentage (0-100)
    double diskUsage;             // MB
    size_t activeThreads;         // Number of active threads
    size_t pendingEvents;         // Number of queued events
    size_t totalEventsProcessed;   // Total events processed since start
    double eventProcessingRate;   // Events per second
    double simulationSpeed;       // Simulation steps per second
    double avgStepTime;           // Average time per simulation step (ms)
    double peakMemoryUsage;       // Peak memory usage (MB)
    double temperature;           // CPU temperature (if available)
    std::chrono::steady_clock::time_point timestamp;  // When metrics were collected
};

struct MemoryProfile {
    std::string allocationSite;   // File and line where allocation occurred
    size_t size;                  // Size of allocation in bytes
    size_t count;                 // Number of allocations
    double percentage;            // Percentage of total memory
    std::chrono::steady_clock::time_point lastAccess;
};

struct TimelineEvent {
    std::chrono::steady_clock::time_point timestamp;
    std::string category;         // CPU, Memory, I/O, Network, etc.
    std::string eventType;        // Allocation, Deallocation, Spike, etc.
    std::string details;          // Human-readable description
    double duration;              // Duration in seconds (if applicable)
    double value;                 // Numeric value (if applicable)
};

struct OptimizationRecommendation {
    std::string issue;            // Description of the performance issue
    std::string category;         // Memory, CPU, I/O, Network, etc.
    double impact;                // Expected performance improvement percentage
    double estimatedCost;         // Memory or CPU cost to implement
    std::vector<std::string> suggestedSolutions; // Possible fixes
    bool automaticallyFixable;    // Whether the system can auto-fix this
};

// PerformanceProfiler: Detailed performance monitoring and optimization
class PerformanceProfiler {
public:
    PerformanceProfiler(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config);
    ~PerformanceProfiler();
    
    // Core monitoring functionality
    void startMonitoring();
    void stopMonitoring();
    void pauseMonitoring(bool pause);
    bool isMonitoring() const;
    
    // Metrics collection
    void collectMetrics();
    PerformanceMetrics getCurrentMetrics() const;
    std::vector<PerformanceMetrics> getRecentMetrics(size_t count) const;
    double getSimulationTime() const;
    double getRealTimeElapsed() const;
    
    // Memory tracking
    void startMemoryTracking();
    void stopMemoryTracking();
    std::vector<MemoryProfile> getMemoryProfile() const;
    size_t getTotalMemoryUsage() const;
    size_t getPeakMemoryUsage() const;
    void setMemoryThreshold(size_t thresholdMB);
    
    // Performance analysis
    void analyzeSimulationSpeed();
    void analyzeMemoryUsage();
    void analyzeEventProcessing();
    void analyzeSystemResources();
    
    // Event tracking
    void startEventTracking(EventCategory category, EventType type,
                           const std::string& details = "");
    void stopEventTracking(EventCategory category, EventType type);
    void recordEvent(const TimelineEvent& event);
    std::vector<TimelineEvent> getEventTimeline(EventCategory category,
                                               EventType eventType = EventType::Unknown,
                                               const std::string& filter = "") const;
    size_t getEventCount(EventCategory category, EventType eventType) const;
    
    // Optimization
    std::vector<OptimizationRecommendation> getOptimizationRecommendations() const;
    bool analyzeAndOptimize();
    void enableAutoOptimization(bool enable);
    bool isAutoOptimizationEnabled() const;
    
    // Simulation control
    void setTargetPerformance(double targetFPS, double targetMemoryMB);
    void setOptimizationLevel(OptimizationLevel level);
    OptimizationLevel getOptimizationLevel() const;
    
    // Reporting and export
    std::string generatePerformanceReport() const;
    std::string generateMemoryReport() const;
    std::string generateTimelineReport() const;
    std::string generateOptimizationReport() const;
    bool exportMetrics(const std::string& filepath, ExportFormat format = ExportFormat::CSV);
    bool saveProfile(const std::string& filepath);
    bool loadProfile(const std::string& filepath);
    
    // Integration with brain simulation
    void attachToBrain(std::shared_ptr<Brain> brain);
    void detachFromBrain();
    bool isAttached() const;
    void onSimulationStep(size_t step, double simTime, double realTime);
    
    // Low-level controls
    void enableProfiling(bool enable);
    void setProfilingInterval(double intervalSeconds);
    void setMaxHistoricalDataSize(size_t maxPoints);
    void clearHistoricalData();
    
    // System information
    std::string getSystemInfo() const;
    std::vector<std::string> getAvailableOptimizationStrategies() const;
    std::string getPerformanceSummary() const;
    
    // Cleanup
    void cleanup() noexcept;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
