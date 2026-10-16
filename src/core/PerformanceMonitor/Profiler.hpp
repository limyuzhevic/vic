#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <chrono>
#include <atomic>
#include <mutex>
#include <functional>
#include <unordered_map>
#include <deque>
#include <optional>
#include <thread>
#include <condition_variable>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <set>
#include "../core/Logger/Logger.hpp"

namespace nlm {

// Profiling categories
enum class ProfileCategory {
    CPU,
    Memory,
    GPU,
    I_O,
    Network,
    Neural,
    Plasticity,
    Simulation,
    Visualization,
    Custom
};

// Profiler configuration
struct ProfilerConfig {
    bool enableCPUProfiling = true;
    bool enableMemoryProfiling = true;
    bool enableGPUProfiling = false;
    bool enableTimelineProfiling = true;
    bool enableRegressionDetection = true;
    size_t maxProfileDepth = 10;
    double profileSamplingInterval = 0.01;
    size_t maxTimelineEvents = 10000;
    std::string outputDirectory = "profiles";
    std::string format = "json"; // json, csv, text
    bool saveProfiles = true;
    bool autoAnalyze = true;
    
    ProfilerConfig() {
        // Create output directory if it doesn't exist
        std::error_code ec;
        std::filesystem::create_directories(outputDirectory, ec);
    }
};

// Timeline event for bottleneck analysis
struct TimelineEvent {
    std::string operation;
    ProfileCategory category;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    double duration;
    uint64_t memoryUsage;
    size_t threadId;
    std::string threadName;
    std::map<std::string, std::string> metadata;
    
    TimelineEvent() : duration(0.0), memoryUsage(0), threadId(0) {}
    
    TimelineEvent(const std::string& op, ProfileCategory cat,
                  const std::chrono::system_clock::time_point& start,
                  const std::chrono::system_clock::time_point& end,
                  uint64_t memory = 0)
        : operation(op), category(cat), startTime(start), endTime(end),
          duration(std::chrono::duration<double>(end - start).count()),
          memoryUsage(memory), threadId(std::hash<std::thread::id>{}(std::this_thread::get_id())) {
        threadName = "thread_" + std::to_string(threadId);
    }
};

// Memory allocation event
struct MemoryAllocation {
    enum class Type { Malloc, Free, New, Delete, Realloc, Custom };
    
    Type type;
    std::chrono::system_clock::time_point timestamp;
    uint64_t address;
    size_t size;
    std::string operation;
    size_t threadId;
    std::string threadName;
    std::map<std::string, std::string> metadata;
    
    MemoryAllocation() : type(Type::Malloc), address(0), size(0), threadId(0) {}
    
    MemoryAllocation(Type t, uint64_t addr, size_t sz, const std::string& op)
        : type(t), address(addr), size(sz), operation(op),
          threadId(std::hash<std::thread::id>{}(std::this_thread::get_id())) {
        timestamp = std::chrono::system_clock::now();
        threadName = "thread_" + std::to_string(threadId);
    }
};

// Performance profile result
struct ProfileResult {
    std::string profileName;
    ProfileConfig config;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    double totalDuration;
    size_t totalOperations;
    std::vector<TimelineEvent> timelineEvents;
    std::vector<MemoryAllocation> memoryAllocations;
    std::map<std::string, double> aggregatedMetrics;
    std::vector<std::pair<std::string, double>> bottleneckAnalysis;
    std::string analysisReport;
    std::string performanceSummary;
    
    std::string toJSON() const;
    std::string toCSV() const;
    std::string toText() const;
};

// CPU profile sample
struct CPUSample {
    std::chrono::system_clock::time_point timestamp;
    std::string threadName;
    uint64_t threadId;
    std::string operation;
    uint64_t cpuCycles;
    uint64_t instructionsRetired;
    double cpuUsage;
    double memoryBandwidth;
    std::map<std::string, double> registers;
    
    CPUSample() : cpuCycles(0), instructionsRetired(0), cpuUsage(0.0), memoryBandwidth(0.0) {}
};

// GPU profile sample
struct GPUSample {
    std::chrono::system_clock::time_point timestamp;
    std::string deviceName;
    uint32_t deviceId;
    uint64_t memoryUsed;
    uint64_t memoryTotal;
    uint64_t memoryBandwidth;
    double gpuUsage;
    uint64_t computeUnitsActive;
    uint64_t warpsActive;
    uint64_t instructionsIssued;
    std::vector<std::pair<std::string, double>> kernelTimes;
    
    GPUSample() : deviceId(0), memoryUsed(0), memoryTotal(0), memoryBandwidth(0),
                  gpuUsage(0.0), computeUnitsActive(0), warpsActive(0), instructionsIssued(0) {}
};

// Profiler internal implementation
class Profiler {
public:
    using ProfileCallback = std::function<void(const TimelineEvent&)>
    using MemoryCallback = std::function<void(const MemoryAllocation&)>
    using AnalysisCallback = std::function<void(const ProfileResult&)>
    
    Profiler(const std::string& name = "default", const ProfilerConfig& config = ProfilerConfig());
    ~Profiler();
    
    // Disable copying
    Profiler(const Profiler&) = delete;
    Profiler& operator=(const Profiler&) = delete;
    
    // Profiling control
    void start();
    void stop();
    bool isRunning() const { return running_; }
    
    // Profile-specific operations
    void startOperation(const std::string& name, ProfileCategory category = ProfileCategory::General);
    void endOperation(const std::string& name, uint64_t memoryUsage = 0);
    
    // Memory profiling
    void recordMemoryAllocation(MemoryAllocation::Type type, uint64_t address, 
                               size_t size, const std::string& operation = "");
    void recordMemoryDeallocation(uint64_t address);
    
    // CPU profiling
    void recordCPUSample(const CPUSample& sample);
    
    // GPU profiling
    void recordGPUSample(const GPUSample& sample);
    
    // Timeline profiling
    void recordTimelineEvent(const TimelineEvent& event);
    
    // Configuration
    void setConfig(const ProfilerConfig& config);
    const ProfilerConfig& getConfig() const { return config_; }
    
    // Callbacks
    void setTimelineCallback(TimelineCallback callback) { timelineCallback_ = callback; }
    void setMemoryCallback(MemoryCallback callback) { memoryCallback_ = callback; }
    void setAnalysisCallback(AnalysisCallback callback) { analysisCallback_ = callback; }
    
    // Profile result management
    void saveProfile(const std::string& filepath = "");
    bool loadProfile(const std::string& filepath);
    
    // Profile analysis
    ProfileResult generateProfileResult() const;
    std::string analyzeBottlenecks(const ProfileResult& result) const;
    std::string analyzeMemoryUsage(const ProfileResult& result) const;
    std::string analyzeCPUPerformance(const ProfileResult& result) const;
    
    // Performance regression detection
    bool detectRegression(const std::string& profileName, 
                         const ProfileResult& currentResult,
                         double threshold = 0.1) const;
    std::vector<std::string> getRegressionReports(const std::string& profileName,
                                                const ProfileResult& currentResult) const;
    
    // Performance comparison
    std::string compareProfiles(const ProfileResult& result1,
                               const ProfileResult& result2) const;
    
    // Profile statistics
    size_t getOperationCount(const std::string& name) const;
    double getTotalOperationTime(const std::string& name) const;
    double getAverageOperationTime(const std::string& name) const;
    
    // Thread-local profiling
    void setThreadName(const std::string& name);
    std::string getCurrentThreadName() const;
    
    // Integration with existing systems
    void integrateWithLogger(std::shared_ptr<Logger> logger);
    void integrateWithStatisticsCollector(std::shared_ptr<StatisticsCollector> collector);
    void integrateWithRealTimeMonitor(std::shared_ptr<RealTimeMonitor> monitor);
    
    // Neural network profiling
    void profileNeuralOperation(const std::string& operation, size_t neuronCount,
                               double time, size_t spikeCount);
    void profilePlasticityOperation(const std::string& operation, 
                                   size_t synapseCount, double time);
    
    // Simulation profiling
    void profileSimulationStep(size_t step, double time, double duration,
                              size_t spikeCount, double cpuUsage);
    
    // Performance hooks for main systems
    void setupMainSystemHooks();
    void setupRuntimeConfigHooks();
    void setupVisualizationHooks();
    void setupPythonInterfaceHooks();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    bool running_ = false;
    std::string name_;
    ProfilerConfig config_;
    
    mutable std::mutex dataMutex_;
    
    // Profiling data
    std::deque<TimelineEvent> timelineEvents_;
    std::deque<MemoryAllocation> memoryAllocations_;
    std::deque<CPUSample> cpuSamples_;
    std::deque<GPUSample> gpuSamples_;
    std::map<std::string, size_t> operationCounts_;
    std::map<std::string, double> operationTimes_;
    
    // Callbacks
    TimelineCallback timelineCallback_;
    MemoryCallback memoryCallback_;
    AnalysisCallback analysisCallback_;
    
    // Thread information
    thread_local static std::string threadName_;
    thread_local static uint64_t threadId_;
    
    // Integration objects
    std::shared_ptr<Logger> logger_;
    std::shared_ptr<StatisticsCollector> statisticsCollector_;
    std::shared_ptr<RealTimeMonitor> realTimeMonitor_;
    
    // Profiling implementation
    void startOperationInternal(const std::string& name, ProfileCategory category,
                               uint64_t threadId, const std::string& threadName);
    void endOperationInternal(const std::string& name, uint64_t memoryUsage,
                             uint64_t threadId);
    void recordMemoryAllocationInternal(const MemoryAllocation& allocation);
    void recordCPUSampleInternal(const CPUSample& sample);
    void recordGPUSampleInternal(const GPUSample& sample);
    void recordTimelineEventInternal(const TimelineEvent& event);
    
    // Profile analysis implementation
    void analyzeProfile(ProfileResult& result) const;
    void detectPerformanceRegression(ProfileResult& result) const;
    void generatePerformanceAnalysis(ProfileResult& result) const;
    
    // File I/O
    void saveProfileInternal(const std::string& filepath, const ProfileResult& result) const;
    bool loadProfileInternal(const std::string& filepath, ProfileResult& result);
    
    // Thread-local helpers
    void initializeThreadLocalState();
    std::string getCurrentThreadNameInternal() const;
    uint64_t getCurrentThreadIdInternal() const;
};

// Global profiler manager
class ProfilerManager {
public:
    ProfilerManager();
    ~ProfilerManager();
    
    // Disable copying
    ProfilerManager(const ProfilerManager&) = delete;
    ProfilerManager& operator=(const ProfilerManager&) = delete;
    
    // Singleton pattern
    static ProfilerManager& getInstance() {
        static ProfilerManager instance;
        return instance;
    }
    
    // Profiler management
    std::shared_ptr<Profiler> createProfiler(const std::string& name = "default",
                                              const ProfilerConfig& config = ProfilerConfig());
    bool removeProfiler(const std::string& name);
    std::shared_ptr<Profiler> getProfiler(const std::string& name);
    std::shared_ptr<Profiler> getDefaultProfiler();
    void setDefaultProfiler(const std::string& name);
    
    // Global profiling control
    void startAllProfilers();
    void stopAllProfilers();
    bool areAllProfilersRunning();
    
    // Global configuration
    void setGlobalConfig(const ProfilerConfig& config);
    const ProfilerConfig& getGlobalConfig() const { return globalConfig_; }
    
    // Global integration
    void integrateWithLogger(std::shared_ptr<Logger> logger);
    void integrateWithStatisticsCollector(std::shared_ptr<StatisticsCollector> collector);
    void integrateWithRealTimeMonitor(std::shared_ptr<RealTimeMonitor> monitor);
    
    // Profile management
    void saveAllProfiles(const std::string& directory = "profiles");
    void loadAllProfiles(const std::string& directory = "profiles");
    
    // Analysis
    std::string generateGlobalAnalysis() const;
    std::string identifyGlobalBottlenecks() const;
    
    // Performance hooks for main.cpp
    void setupMainCppHooks();
    void setupPerformanceMonitoring();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    ProfilerConfig globalConfig_;
    std::map<std::string, std::shared_ptr<Profiler>> profilers_;
    std::shared_ptr<Profiler> defaultProfiler_;
    
    std::shared_ptr<Logger> logger_;
    std::shared_ptr<StatisticsCollector> statisticsCollector_;
    std::shared_ptr<RealTimeMonitor> realTimeMonitor_;
};

// Convenience functions
inline std::shared_ptr<Profiler> getProfiler(const std::string& name = "default") {
    return ProfilerManager::getInstance().getProfiler(name);
}

inline std::shared_ptr<Profiler> getDefaultProfiler() {
    return ProfilerManager::getInstance().getDefaultProfiler();
}

inline void startProfiling(const std::string& name = "default") {
    auto profiler = ProfilerManager::getInstance().getProfiler(name);
    if (profiler) {
        profiler->start();
    }
}

inline void stopProfiling(const std::string& name = "default") {
    auto profiler = ProfilerManager::getInstance().getProfiler(name);
    if (profiler) {
        profiler->stop();
    }
}

inline void startOperation(const std::string& name, ProfileCategory category = ProfileCategory::General) {
    auto profiler = ProfilerManager::getInstance().getDefaultProfiler();
    if (profiler) {
        profiler->startOperation(name, category);
    }
}

inline void endOperation(const std::string& name, uint64_t memoryUsage = 0) {
    auto profiler = ProfilerManager::getInstance().getDefaultProfiler();
    if (profiler) {
        profiler->endOperation(name, memoryUsage);
    }
}

} // namespace nlm
