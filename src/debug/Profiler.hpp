// Performance Profiler for NLM v2.0+
// High-performance profiling system for CPU, memory, and GPU monitoring

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <functional>
#include <optional>
#include <map>

namespace nlm {

// Profiling targets
enum class ProfileTarget {
    CPU,
    Memory,
    GPU,
    Network,
    Simulation,
    MemorySystem,
    Neuromodulation,
    All
};

// Profile sample data
struct ProfileSample {
    std::chrono::system_clock::time_point timestamp;
    uint64_t step;
    double cpuUsage;           // CPU usage percentage
    double memoryUsage;        // Memory usage in MB
    double gpuUsage;           // GPU usage percentage (if available)
    uint64_t memoryAllocated;  // Allocated memory in bytes
    uint64_t memoryPeak;       // Peak memory in bytes
    uint64_t threadCount;      // Number of active threads
    double networkBandwidth;   // Network bandwidth in Mbps
    std::string component;     // Which component was profiled
    std::map<std::string, double> customMetrics; // Custom metrics
    
    ProfileSample(uint64_t s, double cpu, double mem, double gpu = 0.0, 
                  uint64_t alloc = 0, uint64_t peak = 0, uint64_t threads = 0,
                  double net = 0.0, const std::string& comp = "")
        : step(s), cpuUsage(cpu), memoryUsage(mem), gpuUsage(gpu),
          memoryAllocated(alloc), memoryPeak(peak), threadCount(threads),
          networkBandwidth(net), component(comp), timestamp(std::chrono::system_clock::now()) {}
};

// Function profile information
struct FunctionProfile {
    std::string functionName;
    std::string component;
    std::chrono::duration<double> totalTime;
    std::chrono::duration<double> minTime;
    std::chrono::duration<double> maxTime;
    uint64_t callCount;
    double averageTime() const { return totalTime.count() / callCount; }
    double minTimePercent() const { return (minTime.count() / totalTime.count()) * 100.0; }
    double maxTimePercent() const { return (maxTime.count() / totalTime.count()) * 100.0; }
    
    FunctionProfile(const std::string& name, const std::string& comp)
        : functionName(name), component(comp), totalTime(0), minTime(0), maxTime(0), callCount(0) {}
};

// Profile statistics
struct ProfileStats {
    uint64_t totalSamples;
    double averageCpuUsage;
    double averageMemoryUsage;
    double peakMemoryUsage;
    uint64_t totalMemoryAllocated;
    double cpuTimePercentage;
    double memoryTimePercentage;
    std::chrono::duration<double> totalProfileDuration;
    uint64_t stepRangeStart;
    uint64_t stepRangeEnd;
    std::map<std::string, FunctionProfile> functionProfiles;
    
    ProfileStats() : totalSamples(0), averageCpuUsage(0.0), averageMemoryUsage(0.0),
                    peakMemoryUsage(0.0), totalMemoryAllocated(0), cpuTimePercentage(0.0),
                    memoryTimePercentage(0.0), stepRangeStart(0), stepRangeEnd(0) {}
};

// Profile listener function type
using ProfileListener = std::function<void(const ProfileSample&)>;

// Function profile listener function type
using FunctionProfileListener = std::function<void(const FunctionProfile&)>;

// Profile statistics listener function type
using ProfileStatsListener = std::function<void(const ProfileStats&)>;

// Profiler implementation
class Profiler {
public:
    Profiler();
    ~Profiler() = default;
    
    // Start/stop profiling
    void startProfiling(const std::vector<ProfileTarget>& targets);
    void stopProfiling();
    bool isProfiling() const;
    
    // Sample collection
    void takeSample(uint64_t step, const std::string& component = "");
    
    // Function profiling
    void startFunctionProfile(const std::string& functionName, const std::string& component = "");
    void stopFunctionProfile(const std::string& functionName, const std::string& component = "");
    
    // Profile statistics
    ProfileStats getProfileStats(uint64_t startStep = 0, uint64_t endStep = UINT64_MAX) const;
    
    // Sample history
    std::vector<ProfileSample> getRecentSamples(size_t count = 100, 
                                                 uint64_t startStep = 0, 
                                                 uint64_t endStep = UINT64_MAX) const;
    std::vector<FunctionProfile> getFunctionProfiles(const std::string& component = "") const;
    
    // Configuration
    void setSampleInterval(double intervalSeconds);
    double getSampleInterval() const;
    
    void setMaxSamplesPerStep(size_t maxSamples);
    size_t getMaxSamplesPerStep() const;
    
    // Filtering
    void enableTarget(ProfileTarget target);
    void disableTarget(ProfileTarget target);
    bool isTargetEnabled(ProfileTarget target) const;
    
    void enableComponent(const std::string& component);
    void disableComponent(const std::string& component);
    bool isComponentEnabled(const std::string& component) const;
    
    // Listeners
    void addProfileListener(const ProfileListener& listener);
    void removeProfileListener(const ProfileListener& listener);
    
    void addFunctionProfileListener(const FunctionProfileListener& listener);
    void removeFunctionProfileListener(const FunctionProfileListener& listener);
    
    void addProfileStatsListener(const ProfileStatsListener& listener);
    void removeProfileStatsListener(const ProfileStatsListener& listener);
    
    // Export functionality
    std::string exportSamples(const std::string& format = "json", 
                             uint64_t startStep = 0, 
                             uint64_t endStep = UINT64_MAX) const;
    std::string exportFunctionProfiles(const std::string& format = "json") const;
    std::string exportProfileStats(const std::string& format = "json") const;
    
    // Benchmark mode
    void startBenchmark(const std::string& mode);
    void stopBenchmark();
    bool isBenchmarking() const;
    std::string getCurrentBenchmarkMode() const;
    
    // Performance metrics
    double getCurrentCpuUsage() const;
    double getCurrentMemoryUsage() const;
    uint64_t getCurrentMemoryAllocated() const;
    uint64_t getCurrentMemoryPeak() const;
    
    // Cleanup
    void clearSamples();
    void clearFunctionProfiles();
    
    // Configuration file
    bool saveConfiguration(const std::string& filepath) const;
    bool loadConfiguration(const std::string& filepath);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Thread safety
    mutable std::mutex mutex;
    
    // Sampling thread
    void samplingThread();
    std::thread sampleThread;
    std::atomic<bool> samplingThreadRunning;
    std::chrono::duration<double> sampleInterval;
    
    // Function profiling
    struct FunctionProfileData {
        std::chrono::steady_clock::time_point startTime;
        std::chrono::duration<double> accumulatedTime;
        uint64_t callCount;
        std::chrono::duration<double> minTime;
        std::chrono::duration<double> maxTime;
        
        FunctionProfileData() : accumulatedTime(0), callCount(0), minTime(0), maxTime(0) {}
    };
    
    std::map<std::string, FunctionProfileData> activeFunctionProfiles;
    
    // Sample collection
    std::vector<ProfileSample> samples;
    size_t maxSamplesPerStep;
    
    // Configuration
    std::vector<ProfileTarget> enabledTargets;
    std::vector<std::string> enabledComponents;
    
    // Listeners
    std::vector<ProfileListener> profileListeners;
    std::vector<FunctionProfileListener> functionProfileListeners;
    std::vector<ProfileStatsListener> profileStatsListeners;
    
    // Benchmark mode
    bool benchmarking;
    std::string currentBenchmarkMode;
    
    // Performance monitoring
    double currentCpuUsage;
    double currentMemoryUsage;
    uint64_t currentMemoryAllocated;
    uint64_t currentMemoryPeak;
    
    // Cleanup
    void cleanupOldSamples();
    void updateFunctionProfiles();
    
    // Performance monitoring helpers
    void updatePerformanceMetrics();
    
    // Export helpers
    std::string exportToJSON(const std::vector<ProfileSample>& samples) const;
    std::string exportToJSON(const std::vector<FunctionProfile>& profiles) const;
    std::string exportToJSON(const ProfileStats& stats) const;
    
    // Configuration management
    void loadDefaultConfiguration();
    void saveDefaultConfiguration() const;
};

} // namespace nlm
