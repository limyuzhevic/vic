#pragma once

#include <string>
#include <memory>
#include <chrono>
#include <map>
#include <vector>
#include <atomic>
#include <mutex>
#include <unordered_map>
#include <functional>
#include <thread>
#include <iomanip>
#include <sstream>
#include <array>
#include <algorithm>
#include <numeric>

namespace nlm {

// Brain state metrics
struct BrainStateMetrics {
    std::chrono::system_clock::time_point timestamp;
    uint64_t totalNeuronCount;
    uint64_t activeNeuronCount;
    uint64_t firingNeuronCount;
    float averageFiringRate;
    uint64_t totalSpikeCount;
    float excitationInhibitionRatio;
    std::map<std::string, double> regionStats;
    std::map<std::string, uint64_t> regionNeuronCounts;
    
    BrainStateMetrics() : totalNeuronCount(0), activeNeuronCount(0), firingNeuronCount(0), 
                         averageFiringRate(0.0f), totalSpikeCount(0), excitationInhibitionRatio(0.0f) {
        timestamp = std::chrono::system_clock::now();
    }
};

// Simulation metrics
struct SimulationMetrics {
    uint64_t step;
    std::chrono::system_clock::time_point timestamp;
    std::chrono::milliseconds stepDuration;
    uint64_t spikeCount;
    float firingRate;
    uint64_t activeNeuronCount;
    std::map<std::string, double> inputStats;
    std::map<std::string, double> outputStats;
    std::map<std::string, double> neuromodulatorStats;
    std::map<std::string, uint64_t> queueDepths;
    
    SimulationMetrics() : step(0), spikeCount(0), firingRate(0.0f), activeNeuronCount(0) {
        timestamp = std::chrono::system_clock::now();
        stepDuration = std::chrono::milliseconds(0);
    }
};

// Memory pool metrics
struct MemoryPoolMetrics {
    std::string poolName;
    size_t currentSize;
    size_t maxSize;
    size_t totalAllocated;
    size_t totalFreed;
    size_t allocationCount;
    size_t deallocationCount;
    double utilizationPercent;
    uint64_t totalMemoryBytes;
    uint64_t peakMemoryBytes;
    std::chrono::system_clock::time_point timestamp;
    
    MemoryPoolMetrics() : currentSize(0), maxSize(0), totalAllocated(0), totalFreed(0),
                         allocationCount(0), deallocationCount(0), utilizationPercent(0.0),
                         totalMemoryBytes(0), peakMemoryBytes(0) {
        timestamp = std::chrono::system_clock::now();
    }
};

// Event system metrics
struct EventMetrics {
    std::chrono::system_clock::time_point timestamp;
    size_t queueDepth;
    size_t processingLatency; // in microseconds
    size_t eventCount;
    double throughput; // events per second
    std::map<std::string, size_t> eventTypeCounts;
    std::map<std::string, size_t> queueLengths;
    std::map<std::string, double> processingTimes;
    
    EventMetrics() : queueDepth(0), processingLatency(0), eventCount(0), throughput(0.0) {
        timestamp = std::chrono::system_clock::now();
    }
};

// Neuromodulator metrics
struct NeuromodulatorMetrics {
    std::string type;
    double currentLevel;
    double previousLevel;
    double targetLevel;
    double changeRate;
    double effectMagnitude;
    double effectDuration;
    std::chrono::system_clock::time_point timestamp;
    
    NeuromodulatorMetrics() : currentLevel(0.0), previousLevel(0.0), targetLevel(0.0),
                              changeRate(0.0), effectMagnitude(0.0), effectDuration(0.0) {
        timestamp = std::chrono::system_clock::now();
    }
};

// Developmental stage metrics
struct DevelopmentalStageMetrics {
    DevelopmentalStage stage;
    std::chrono::system_clock::time_point stageStartTime;
    std::chrono::system_clock::time_point stageEndTime;
    std::chrono::milliseconds stageDuration;
    double plasticityRate;
    double excitationChangeRate;
    double developmentProgress;
    std::map<std::string, double> stageSpecificMetrics;
    
    DevelopmentalStageMetrics() : stage(DevelopmentalStage::Embryonic), developmentProgress(0.0),
                                 plasticityRate(0.0), excitationChangeRate(0.0) {
        stageStartTime = std::chrono::system_clock::now();
        stageEndTime = stageStartTime;
        stageDuration = std::chrono::milliseconds(0);
    }
};

// Forward declarations
class AdvancedLogger;

// Runtime metrics collector interface
class IRuntimeMetricsCollector {
public:
    virtual ~IRuntimeMetricsCollector() = default;
    virtual void recordBrainMetrics(const BrainStateMetrics& metrics) = 0;
    virtual void recordSimulationMetrics(const SimulationMetrics& metrics) = 0;
    virtual void recordMemoryPoolMetrics(const MemoryPoolMetrics& metrics) = 0;
    virtual void recordEventMetrics(const EventMetrics& metrics) = 0;
    virtual void recordNeuromodulatorMetrics(const NeuromodulatorMetrics& metrics) = 0;
    virtual void recordDevelopmentalStageMetrics(const DevelopmentalStageMetrics& metrics) = 0;
    virtual BrainStateMetrics getCurrentBrainState() const = 0;
    virtual SimulationMetrics getCurrentSimulationMetrics() const = 0;
    virtual std::vector<MemoryPoolMetrics> getMemoryPoolMetrics() const = 0;
    virtual std::vector<EventMetrics> getEventMetrics() const = 0;
    virtual std::vector<NeuromodulatorMetrics> getNeuromodulatorMetrics() const = 0;
    virtual std::vector<DevelopmentalStageMetrics> getDevelopmentalStageMetrics() const = 0;
    virtual void reset() = 0;
};

// Default runtime metrics collector implementation
class RuntimeMetricsCollector : public IRuntimeMetricsCollector {
public:
    RuntimeMetricsCollector();
    ~RuntimeMetricsCollector() override;
    
    // IRuntimeMetricsCollector interface
    void recordBrainMetrics(const BrainStateMetrics& metrics) override;
    void recordSimulationMetrics(const SimulationMetrics& metrics) override;
    void recordMemoryPoolMetrics(const MemoryPoolMetrics& metrics) override;
    void recordEventMetrics(const EventMetrics& metrics) override;
    void recordNeuromodulatorMetrics(const NeuromodulatorMetrics& metrics) override;
    void recordDevelopmentalStageMetrics(const DevelopmentalStageMetrics& metrics) override;
    
    BrainStateMetrics getCurrentBrainState() const override;
    SimulationMetrics getCurrentSimulationMetrics() const override;
    std::vector<MemoryPoolMetrics> getMemoryPoolMetrics() const override;
    std::vector<EventMetrics> getEventMetrics() const override;
    std::vector<NeuromodulatorMetrics> getNeuromodulatorMetrics() const override;
    std::vector<DevelopmentalStageMetrics> getDevelopmentalStageMetrics() const override;
    
    void reset() override;
    
    // Configuration and control
    void setCollectionInterval(std::chrono::milliseconds interval);
    void setMaxHistorySize(size_t maxSize);
    void enableRealTimeUpdates(bool enable);
    void setLogger(std::shared_ptr<AdvancedLogger> logger);
    
    // Data retrieval with filtering
    std::vector<BrainStateMetrics> getBrainStateHistory(const std::chrono::system_clock::time_point& since) const;
    std::vector<SimulationMetrics> getSimulationHistory(const std::chrono::system_clock::time_point& since) const;
    std::vector<MemoryPoolMetrics> getMemoryPoolHistory(const std::string& poolName, 
                                                         const std::chrono::system_clock::time_point& since) const;
    std::vector<EventMetrics> getEventHistory(const std::chrono::system_clock::time_point& since) const;
    std::vector<NeuromodulatorMetrics> getNeuromodulatorHistory(const std::string& type,
                                                               const std::chrono::system_clock::time_point& since) const;
    
    // Integration helpers
    void updateFromBrain(const class Brain& brain);
    void updateFromSimulation(uint64_t step, const std::chrono::milliseconds& stepDuration);
    void updateMemoryPool(const std::string& poolName, size_t currentSize, 
                         size_t maxSize, size_t totalAllocated);
    void updateEventQueue(size_t depth, double avgProcessingTime);
    void updateNeuromodulator(const std::string& type, double level, double effect);
    void updateDevelopment(DevelopmentalStage stage, double progress);
    
    // Analysis and statistics
    double getAverageFiringRate() const;
    uint64_t getTotalSpikeCount() const;
    size_t getCurrentActiveNeuronCount() const;
    std::chrono::system_clock::time_point getLastUpdateTime() const;
    
    // Static instance management
    static RuntimeMetricsCollector& getInstance();
    static void setGlobal(std::shared_ptr<RuntimeMetricsCollector> collector);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    void cleanupOldData();
    double calculateAverageFiringRate() const;
};

// Brain state tracking helper
class BrainStateTracker {
public:
    BrainStateTracker();
    ~BrainStateTracker();
    
    void updateFromBrain(const class Brain& brain);
    BrainStateMetrics getMetrics() const;
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Simulation metrics tracker
class SimulationMetricsTracker {
public:
    SimulationMetricsTracker();
    ~SimulationMetricsTracker();
    
    void recordStep(uint64_t step, const std::chrono::milliseconds& stepDuration,
                   uint64_t spikeCount, float firingRate, uint64_t activeNeuronCount);
    SimulationMetrics getMetrics() const;
    void reset();
    
    void setInputStats(const std::map<std::string, double>& stats);
    void setOutputStats(const std::map<std::string, double>& stats);
    void setNeuromodulatorStats(const std::map<std::string, double>& stats);
    void setQueueDepths(const std::map<std::string, uint64_t>& depths);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Memory pool tracker
class MemoryPoolTracker {
public:
    MemoryPoolTracker();
    ~MemoryPoolTracker();
    
    void recordAllocation(const std::string& poolName, size_t size, 
                         size_t totalAllocated = 0, size_t peakAllocated = 0);
    void recordDeallocation(const std::string& poolName, size_t size);
    MemoryPoolMetrics getMetrics(const std::string& poolName) const;
    void reset();
    
    void setPoolStats(const std::string& poolName, size_t currentSize,
                     size_t maxSize, size_t totalAllocated,
                     size_t totalFreed, size_t allocationCount,
                     size_t deallocationCount, uint64_t totalMemoryBytes,
                     uint64_t peakMemoryBytes);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Event metrics tracker
class EventMetricsTracker {
public:
    EventMetricsTracker();
    ~EventMetricsTracker();
    
    void recordEvent(const std::string& type, size_t processingLatency = 0);
    EventMetrics getMetrics() const;
    void reset();
    
    void setQueueDepth(size_t depth);
    void setEventCounts(const std::map<std::string, size_t>& counts);
    void setQueueLengths(const std::map<std::string, size_t>& lengths);
    void setProcessingTimes(const std::map<std::string, double>& times);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Neuromodulator metrics tracker
class NeuromodulatorMetricsTracker {
public:
    NeuromodulatorMetricsTracker();
    ~NeuromodulatorMetricsTracker();
    
    void update(const std::string& type, double level, double effect, double changeRate);
    NeuromodulatorMetrics getMetrics(const std::string& type) const;
    void reset();
    
    void setLevel(const std::string& type, double level);
    void setTargetLevel(const std::string& type, double targetLevel);
    void setEffect(const std::string& type, double effect, double duration);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Developmental stage tracker
class DevelopmentalStageTracker {
public:
    DevelopmentalStageTracker();
    ~DevelopmentalStageTracker();
    
    void updateStage(DevelopmentalStage stage, double progress, double plasticityRate,
                    double excitationChangeRate);
    DevelopmentalStageMetrics getMetrics() const;
    void reset();
    
    void setStageStartTime(const std::chrono::system_clock::time_point& startTime);
    void setStageEndTime(const std::chrono::system_clock::time_point& endTime);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Metric aggregation and analysis helper
class RuntimeMetricsAnalyzer {
public:
    RuntimeMetricsAnalyzer();
    ~RuntimeMetricsAnalyzer();
    
    // Statistical analysis
    double calculateMean(const std::vector<double>& values) const;
    double calculateStandardDeviation(const std::vector<double>& values, double mean) const;
    double calculatePercentile(const std::vector<double>& values, double percentile) const;
    
    // Trend analysis
    double calculateTrend(const std::vector<std::pair<std::chrono::system_clock::time_point, double>>& data) const;
    std::string detectPattern(const std::vector<double>& values) const;
    
    // Pattern detection
    std::string detectActivityPattern(const std::vector<SimulationMetrics>& simulations) const;
    std::string detectMemoryPattern(const std::vector<MemoryPoolMetrics>& pools) const;
    std::string detectNeuromodulatorPattern(const std::vector<NeuromodulatorMetrics>& neuromodulators) const;
    
    // Health monitoring
    std::string assessBrainHealth(const BrainStateMetrics& current, 
                                 const BrainStateMetrics& baseline) const;
    std::string assessSimulationHealth(const SimulationMetrics& current,
                                      const SimulationMetrics& baseline) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm