#pragma once

#include "../core/Types/Types.hpp"
#include <chrono>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <algorithm>
#include <numeric>
#include <sstream>

namespace nlm {

// Performance metrics for simulation monitoring
struct PerformanceMetrics {
    // Timing statistics
    double wallClockTime{0.0};           // Total wall clock time (seconds)
    double simulationTime{0.0};          // Total simulation time (seconds)
    double cpuTime{0.0};                 // CPU time (seconds)
    
    // Simulation statistics
    uint64_t stepsCompleted{0};           // Number of simulation steps
    float averageFiringRate{0.0f};        // Average firing rate across all neurons
    uint64_t totalSpikeCount{0};          // Total spikes generated
    uint64_t activeNeuronCount{0};        // Current number of active neurons
    uint64_t memoryObjectCount{0};        // Number of memory objects retained
    
    // System resource usage
    size_t memoryUsageMB{0};              // Memory usage in MB
    float cpuUsagePercent{0.0f};          // CPU usage percentage
    
    // Performance indicators
    double stepsPerSecond{0.0};           // Simulation speed (steps/second)
    double efficiency{0.0f};              // Performance efficiency (0-1)
    
    // Time tracking
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;
    
    PerformanceMetrics() {
        startTime = std::chrono::high_resolution_clock::now();
    }
    
    void recordStep(uint64_t spikeCount, float firingRate, uint64_t activeNeurons, uint64_t memoryObjects) {
        stepsCompleted++;
        totalSpikeCount += spikeCount;
        // Update running average for firing rate
        if (stepsCompleted == 1) {
            averageFiringRate = firingRate;
        } else {
            averageFiringRate = (averageFiringRate * (stepsCompleted - 1) + firingRate) / stepsCompleted;
        }
        activeNeuronCount = activeNeurons;
        memoryObjectCount = memoryObjects;
        
        // Update simulation time
        endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime);
        simulationTime = duration.count();
        
        // Calculate steps per second
        if (simulationTime > 0.0) {
            stepsPerSecond = stepsCompleted / simulationTime;
        }
    }
    
    void setSystemResourceUsage(size_t memoryMB, float cpuPercent) {
        memoryUsageMB = memoryMB;
        cpuUsagePercent = cpuPercent;
        
        // Simple efficiency calculation (normalized 0-1)
        if (stepsPerSecond > 0.0) {
            efficiency = std::min(1.0, stepsPerSecond / 1000.0); // Normalize to 1000 steps/sec target
        }
    }
    
    std::string toString() const {
        std::ostringstream oss;
        oss << "=== PERFORMANCE METRICS ===" << std::endl;
        oss << "Simulation Time: " << simulationTime << "s" << std::endl;
        oss << "Wall Clock Time: " << wallClockTime << "s" << std::endl;
        oss << "Steps Completed: " << stepsCompleted << std::endl;
        oss << "Steps/Second: " << stepsPerSecond << std::endl;
        oss << "Total Spikes: " << totalSpikeCount << std::endl;
        oss << "Average Firing Rate: " << averageFiringRate << std::endl;
        oss << "Active Neurons: " << activeNeuronCount << std::endl;
        oss << "Memory Objects: " << memoryObjectCount << std::endl;
        oss << "Memory Usage: " << memoryUsageMB << "MB" << std::endl;
        oss << "CPU Usage: " << cpuUsagePercent << "%" << std::endl;
        oss << "Efficiency: " << (efficiency * 100) << "%" << std::endl;
        return oss.str();
    }
    
    void reset() {
        wallClockTime = 0.0;
        simulationTime = 0.0;
        cpuTime = 0.0;
        stepsCompleted = 0;
        averageFiringRate = 0.0f;
        totalSpikeCount = 0;
        activeNeuronCount = 0;
        memoryObjectCount = 0;
        memoryUsageMB = 0;
        cpuUsagePercent = 0.0f;
        stepsPerSecond = 0.0;
        efficiency = 0.0f;
        startTime = std::chrono::high_resolution_clock::now();
        endTime = startTime;
    }
};

// Benchmarking results for performance comparison
struct BenchmarkResult {
    std::string benchmarkName;
    PerformanceMetrics metrics;
    std::map<std::string, double> customMetrics;
    double startWallTime{0.0};
    double endWallTime{0.0};
    
    BenchmarkResult(const std::string& name) : benchmarkName(name) {
        startWallTime = std::chrono::duration<double>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    }
    
    void finish() {
        endWallTime = std::chrono::duration<double>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        metrics.wallClockTime = endWallTime - startWallTime;
    }
    
    double getDuration() const {
        return metrics.wallClockTime;
    }
    
    std::string toString() const {
        std::ostringstream oss;
        oss << "=== BENCHMARK: " << benchmarkName << " ===" << std::endl;
        oss << "Duration: " << getDuration() << "s" << std::endl;
        oss << metrics.toString();
        oss << "Custom Metrics:" << std::endl;
        for (const auto& pair : customMetrics) {
            oss << "  " << pair.first << ": " << pair.second << std::endl;
        }
        return oss.str();
    }
};

// Debug information collector
class DebugInfo {
public:
    DebugInfo() = default;
    ~DebugInfo() = default;
    
    // Add simulation statistics
    void addStepStats(uint64_t step, uint64_t spikeCount, float firingRate, 
                     uint64_t activeNeurons, const std::string& action = "") {
        stepStats_.push_back({step, spikeCount, firingRate, activeNeurons, action});
    }
    
    // Add performance snapshot
    void addPerformanceSnapshot(const PerformanceMetrics& metrics) {
        performanceSnapshots_.push_back(metrics);
    }
    
    // Add memory information
    void addMemoryInfo(size_t totalObjects, size_t peakObjects, 
                      size_t activeConnections, size_t peakConnections) {
        memoryInfo_ = {totalObjects, peakObjects, activeConnections, peakConnections};
    }
    
    // Add system information
    void addSystemInfo(const std::string& osInfo, const std::string& compilerInfo,
                      size_t cpuCores, uint64_t totalMemory) {
        systemInfo_ = {osInfo, compilerInfo, cpuCores, totalMemory};
    }
    
    // Add configuration summary
    void addConfigSummary(const std::map<std::string, std::string>& config) {
        configSummary_ = config;
    }
    
    // Generate comprehensive debug report
    std::string generateReport() const {
        std::ostringstream oss;
        
        oss << "=== NLM DEBUG REPORT ===" << std::endl;
        oss << "Generated at: " << getCurrentTimestamp() << std::endl;
        oss << std::endl;
        
        // System information
        oss << "SYSTEM INFORMATION:" << std::endl;
        oss << "  OS: " << systemInfo_.osInfo << std::endl;
        oss << "  Compiler: " << systemInfo_.compilerInfo << std::endl;
        oss << "  CPU Cores: " << systemInfo_.cpuCores << std::endl;
        oss << "  Total Memory: " << (systemInfo_.totalMemory / (1024*1024)) << " MB" << std::endl;
        oss << std::endl;
        
        // Configuration summary
        if (!configSummary_.empty()) {
            oss << "CONFIGURATION SUMMARY:" << std::endl;
            for (const auto& pair : configSummary_) {
                oss << "  " << pair.first << ": " << pair.second << std::endl;
            }
            oss << std::endl;
        }
        
        // Memory information
        oss << "MEMORY INFORMATION:" << std::endl;
        oss << "  Total Objects: " << memoryInfo_.totalObjects << std::endl;
        oss << "  Peak Objects: " << memoryInfo_.peakObjects << std::endl;
        oss << "  Active Connections: " << memoryInfo_.activeConnections << std::endl;
        oss << "  Peak Connections: " << memoryInfo_.peakConnections << std::endl;
        oss << std::endl;
        
        // Performance snapshots
        if (!performanceSnapshots_.empty()) {
            oss << "PERFORMANCE SNAPSHOTS (latest 5):" << std::endl;
            size_t count = std::min(performanceSnapshots_.size(), size_t(5));
            for (size_t i = 0; i < count; ++i) {
                size_t idx = performanceSnapshots_.size() - 1 - i;
                oss << "  Snapshot " << idx << ": " << performanceSnapshots_[idx].toString();
                if (i < count - 1) oss << "---" << std::endl;
            }
            oss << std::endl;
        }
        
        // Step statistics
        if (!stepStats_.empty()) {
            oss << "STEP STATISTICS (last 10 steps):" << std::endl;
            size_t count = std::min(stepStats_.size(), size_t(10));
            for (size_t i = 0; i < count; ++i) {
                size_t idx = stepStats_.size() - 1 - i;
                const auto& stats = stepStats_[idx];
                oss << "  Step " << stats.step << ": " << stats.spikeCount << " spikes, "
                    << stats.firingRate << " avg FR, " << stats.activeNeurons << " active neurons";
                if (!stats.action.empty()) {
                    oss << ", Action: " << stats.action;
                }
                oss << std::endl;
            }
            oss << std::endl;
        }
        
        // Performance trends
        if (performanceSnapshots_.size() > 1) {
            oss << "PERFORMANCE TRENDS:" << std::endl;
            oss << "  Steps per second: " << performanceSnapshots_.back().stepsPerSecond << std::endl;
            oss << "  Memory efficiency: " << (1.0 - (memoryInfo_.activeConnections / 
                       std::max(1, memoryInfo_.peakConnections))) * 100 << "%" << std::endl;
        }
        
        oss << "=== END DEBUG REPORT ===" << std::endl;
        return oss.str();
    }
    
private:
    std::string getCurrentTimestamp() const {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    // Data structures for storing debug information
    struct StepStatistics {
        uint64_t step;
        uint64_t spikeCount;
        float firingRate;
        uint64_t activeNeurons;
        std::string action;
    };
    
    struct MemoryStatistics {
        size_t totalObjects{0};
        size_t peakObjects{0};
        size_t activeConnections{0};
        size_t peakConnections{0};
    };
    
    struct SystemInformation {
        std::string osInfo;
        std::string compilerInfo;
        size_t cpuCores{0};
        uint64_t totalMemory{0};
    };
    
    std::vector<StepStatistics> stepStats_;
    std::vector<PerformanceMetrics> performanceSnapshots_;
    MemoryStatistics memoryInfo_;
    SystemInformation systemInfo_;
    std::map<std::string, std::string> configSummary_;
};

// Simple performance profiler
class PerformanceProfiler {
public:
    PerformanceProfiler() = default;
    ~PerformanceProfiler() = default;
    
    void start(const std::string& operation) {
        operations_.push_back({operation, std::chrono::high_resolution_clock::now()});
    }
    
    void stop(const std::string& operation) {
        if (operations_.empty()) return;
        
        auto& op = operations_.back();
        if (op.name == operation) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - op.startTime);
            op.duration = duration.count();
            op.completed = true;
        }
    }
    
    std::string getProfileReport() const {
        std::ostringstream oss;
        oss << "=== PERFORMANCE PROFILE ===" << std::endl;
        oss << "Total operations: " << operations_.size() << std::endl;
        oss << std::endl;
        
        double totalTime = 0.0;
        for (const auto& op : operations_) {
            if (op.completed) {
                oss << "  " << op.name << ": " << op.duration << " microseconds" << std::endl;
                totalTime += op.duration;
            } else {
                oss << "  " << op.name << ": <running>" << std::endl;
            }
        }
        
        oss << std::endl;
        oss << "Summary:" << std::endl;
        oss << "  Total time: " << totalTime << " microseconds" << std::endl;
        oss << "  Average time per operation: " << 
            (operations_.size() > 0 ? totalTime / operations_.size() : 0.0) << " microseconds" << std::endl;
        oss << "  Operations completed: " << 
            std::count_if(operations_.begin(), operations_.end(), 
                         [](const OperationProfile& op) { return op.completed; }) << "/" << operations_.size() << std::endl;
        
        return oss.str();
    }
    
    void clear() {
        operations_.clear();
    }
    
    size_t getOperationCount() const {
        return operations_.size();
    }
    
    bool hasPendingOperations() const {
        return std::any_of(operations_.begin(), operations_.end(),
                         [](const OperationProfile& op) { return !op.completed; });
    }
    
private:
    struct OperationProfile {
        std::string name;
        std::chrono::high_resolution_clock::time_point startTime;
        double duration{0.0};
        bool completed{false};
    };
    
    std::vector<OperationProfile> operations_;
};

// Memory tracker for monitoring memory usage
class MemoryTracker {
public:
    MemoryTracker() = default;
    ~MemoryTracker() = default;
    
    void recordAllocation(size_t bytes, const std::string& source = "unknown") {
        allocations_.push_back({bytes, source, std::chrono::high_resolution_clock::now()});
        totalAllocated += bytes;
        peakAllocated = std::max(peakAllocated, totalAllocated);
    }
    
    void recordDeallocation(size_t bytes, const std::string& source = "unknown") {
        deallocations_.push_back({bytes, source, std::chrono::high_resolution_clock::now()});
        totalAllocated -= bytes;
    }
    
    std::string getMemoryReport() const {
        std::ostringstream oss;
        oss << "=== MEMORY TRACKER REPORT ===" << std::endl;
        oss << "Total allocated: " << totalAllocated / 1024 << " KB" << std::endl;
        oss << "Peak allocated: " << peakAllocated / 1024 << " KB" << std::endl;
        oss << "Allocation count: " << allocations_.size() << std::endl;
        oss << "Deallocation count: " << deallocations_.size() << std::endl;
        oss << std::endl;
        
        // Calculate allocation efficiency
        size_t totalDeallocated = std::accumulate(deallocations_.begin(), deallocations_.end(), 0,
                                                 [](size_t sum, const DeallocationRecord& rec) { return sum + rec.bytes; });
        double efficiency = (totalDeallocated > 0) ? 
            (static_cast<double>(totalDeallocated) / totalAllocated) * 100.0 : 0.0;
        
        oss << "Deallocation efficiency: " << efficiency << "%" << std::endl;
        oss << std::endl;
        
        // Show recent allocations
        oss << "Recent allocations (last 5):" << std::endl;
        size_t count = std::min(allocations_.size(), size_t(5));
        for (size_t i = 0; i < count; ++i) {
            size_t idx = allocations_.size() - 1 - i;
            oss << "  " << idx << ": " << allocations_[idx].bytes / 1024 << " KB, "
                << allocations_[idx].source << ", "
                << getTimeSince(allocations_[idx].timestamp) << " ago" << std::endl;
        }
        
        return oss.str();
    }
    
    void reset() {
        allocations_.clear();
        deallocations_.clear();
        totalAllocated = 0;
        peakAllocated = 0;
    }
    
    size_t getTotalAllocated() const { return totalAllocated; }
    size_t getPeakAllocated() const { return peakAllocated; }
    size_t getAllocationCount() const { return allocations_.size(); }
    
private:
    std::string getTimeSince(std::chrono::high_resolution_clock::time_point timestamp) const {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - timestamp);
        auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
        
        if (seconds < 60) return std::to_string(seconds) + "s";
        if (seconds < 3600) return std::to_string(seconds / 60) + "m";
        return std::to_string(seconds / 3600) + "h";
    }
    
    struct AllocationRecord {
        size_t bytes;
        std::string source;
        std::chrono::high_resolution_clock::time_point timestamp;
    };
    
    struct DeallocationRecord {
        size_t bytes;
        std::string source;
        std::chrono::high_resolution_clock::time_point timestamp;
    };
    
    std::vector<AllocationRecord> allocations_;
    std::vector<DeallocationRecord> deallocations_;
    size_t totalAllocated{0};
    size_t peakAllocated{0};
};

// Integration of all debugging tools into a single interface
class Debugger {
public:
    Debugger() : performanceProfiler_(), memoryTracker_(), debugInfo_() {}
    
    void startBenchmark(const std::string& name) {
        benchmark_ = BenchmarkResult(name);
    }
    
    void stopBenchmark() {
        benchmark_.finish();
        currentBenchmark_ = nullptr;
    }
    
    BenchmarkResult& getCurrentBenchmark() {
        return benchmark_;
    }
    
    void recordStep(uint64_t step, uint64_t spikeCount, float firingRate,
                    uint64_t activeNeurons, const std::string& action = "") {
        debugInfo_.addStepStats(step, spikeCount, firingRate, activeNeurons, action);
        
        if (currentBenchmark_) {
            currentBenchmark_.metrics.recordStep(spikeCount, firingRate, activeNeurons, 0);
        }
    }
    
    void recordPerformanceSnapshot(const PerformanceMetrics& metrics) {
        debugInfo_.addPerformanceSnapshot(metrics);
        
        if (currentBenchmark_) {
            currentBenchmark_.metrics = metrics;
        }
    }
    
    void recordMemoryInfo(size_t totalObjects, size_t peakObjects,
                         size_t activeConnections, size_t peakConnections) {
        debugInfo_.addMemoryInfo(totalObjects, peakObjects, activeConnections, peakConnections);
    }
    
    void recordSystemInfo(const std::string& osInfo, const std::string& compilerInfo,
                         size_t cpuCores, uint64_t totalMemory) {
        debugInfo_.addSystemInfo(osInfo, compilerInfo, cpuCores, totalMemory);
    }
    
    void recordConfigSummary(const std::map<std::string, std::string>& config) {
        debugInfo_.addConfigSummary(config);
    }
    
    std::string generateDebugReport() const {
        return debugInfo_.generateReport();
    }
    
    std::string getPerformanceProfile() const {
        return performanceProfiler_.getProfileReport();
    }
    
    std::string getMemoryReport() const {
        return memoryTracker_.getMemoryReport();
    }
    
    void startProfiling(const std::string& operation) {
        performanceProfiler_.start(operation);
    }
    
    void stopProfiling(const std::string& operation) {
        performanceProfiler_.stop(operation);
    }
    
    void recordAllocation(size_t bytes, const std::string& source = "unknown") {
        memoryTracker_.recordAllocation(bytes, source);
    }
    
    void recordDeallocation(size_t bytes, const std::string& source = "unknown") {
        memoryTracker_.recordDeallocation(bytes, source);
    }
    
    void clearAll() {
        debugInfo_ = DebugInfo();
        performanceProfiler_.clear();
        memoryTracker_.reset();
    }
    
    bool isActive() const {
        return debugInfo_.getCurrentTimestamp() != "";
    }
    
private:
    BenchmarkResult benchmark_;
    BenchmarkResult* currentBenchmark_{nullptr};
    PerformanceProfiler performanceProfiler_;
    MemoryTracker memoryTracker_;
    DebugInfo debugInfo_;
};

} // namespace nlm