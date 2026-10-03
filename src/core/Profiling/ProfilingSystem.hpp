// Advanced logging and profiling system for NLM
// Provides performance profiling, metrics collection, and debugging capabilities

#ifndef NLM_PROFILING_HPP
#define NLM_PROFILING_HPP

#include "core/Logger/Logger.hpp"
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <functional>
#include <thread>
#include <mutex>
#include <atomic>
#include <filesystem>
#include <cstdint>

namespace nlm {

// Performance timing utilities
class HighResTimer {
public:
    using Clock = std::chrono::high_resolution_clock;
    using Duration = std::chrono::duration<double>;
    
    HighResTimer() : startTime_(Clock::now()) {}
    
    double elapsed() const {
        auto endTime = Clock::now();
        return std::chrono::duration<double>(endTime - startTime_).count();
    }
    
    double elapsedMilliseconds() const {
        return elapsed() * 1000.0;
    }
    
    double elapsedMicroseconds() const {
        return elapsed() * 1000000.0;
    }
    
    void reset() {
        startTime_ = Clock::now();
    }
    
private:
    Clock::time_point startTime_;
};

// Thread-local storage for profiling data
class ThreadLocalProfiler {
private:
    struct ProfileData {
        std::string name;
        HighResTimer timer;
        uint64_t callCount = 0;
        double totalTime = 0.0;
        double minTime = std::numeric_limits<double>::max();
        double maxTime = 0.0;
        double lastTime = 0.0;
        
        ProfileData(const std::string& name) : name(name) {}
    };
    
    std::map<std::string, ProfileData> profiles;
    std::mutex mutex;
    
public:
    void startProfile(const std::string& name) {
        std::lock_guard<std::mutex> lock(mutex);
        profiles[name] = ProfileData(name);
    }
    
    void stopProfile(const std::string& name) {
        std::lock_guard<std::mutex> lock(mutex);
        auto it = profiles.find(name);
        if (it != profiles.end()) {
            auto& data = it->second;
            double elapsed = data.timer.elapsed();
            
            data.callCount++;
            data.totalTime += elapsed;
            data.minTime = std::min(data.minTime, elapsed);
            data.maxTime = std::max(data.maxTime, elapsed);
            data.lastTime = elapsed;
        }
    }
    
    ProfileData* getProfile(const std::string& name) {
        std::lock_guard<std::mutex> lock(mutex);
        auto it = profiles.find(name);
        return it != profiles.end() ? &it->second : nullptr;
    }
    
    void resetProfile(const std::string& name) {
        std::lock_guard<std::mutex> lock(mutex);
        profiles.erase(name);
    }
    
    std::vector<ProfileData> getAllProfiles() const {
        std::lock_guard<std::mutex> lock(mutex);
        std::vector<ProfileData> result;
        for (const auto& pair : profiles) {
            result.push_back(pair.second);
        }
        return result;
    }
    
    void clear() {
        std::lock_guard<std::mutex> lock(mutex);
        profiles.clear();
    }
};

// Global profiler manager
class Profiler {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
public:
    Profiler();
    ~Profiler();
    
    // Start a profile section
    void start(const std::string& name);
    
    // Stop current profile section
    void stop(const std::string& name);
    
    // Start scoped profile (RAII)
    class ScopedTimer {
    public:
        ScopedTimer(Profiler& profiler, const std::string& name)
            : profiler_(profiler), name_(name) {
            profiler_.start(name_);
        }
        
        ~ScopedTimer() {
            profiler_.stop(name_);
        }
        
    private:
        Profiler& profiler_;
        std::string name_;
    };
    
    // Convenience function for creating scoped timers
    static ScopedTimer scoped(const std::string& name);
    
    // Get profile data
    ThreadLocalProfiler::ProfileData* getProfile(const std::string& name);
    
    // Get all profile data
    std::vector<ThreadLocalProfiler::ProfileData> getAllProfiles() const;
    
    // Clear all profiles
    void clear();
    
    // Generate report
    std::string generateReport() const;
    
    // Save report to file
    bool saveReport(const std::string& filepath) const;
    
    // Merge with another profiler
    void merge(const Profiler& other);
    
    // Get total time for all profiles
    double getTotalTime() const;
    
    // Get call count for a profile
    uint64_t getCallCount(const std::string& name) const;
    
    // Check if profiling is active
    bool isActive() const;
    
    // Enable/disable profiling
    void setActive(bool active);
};n

// Metrics collection system
class MetricsCollector {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
public:
    MetricsCollector();
    ~MetricsCollector();
    
    // Record a metric
    void record(const std::string& name, double value,
                const std::map<std::string, std::string>& labels = {});
    
    // Record a metric with timestamp
    void record(const std::string& name, double value,
                std::chrono::system_clock::time_point timestamp,
                const std::map<std::string, std::string>& labels = {});
    
    // Get latest value of a metric
    std::optional<double> getLatest(const std::string& name) const;
    
    // Get all values of a metric
    std::vector<std::pair<std::chrono::system_clock::time_point, double>> 
    getAll(const std::string& name) const;
    
    // Get all metric names
    std::vector<std::string> getMetricNames() const;
    
    // Get metrics by label
    std::vector<std::pair<std::string, double>> 
    getByLabel(const std::string& labelName, const std::string& labelValue) const;
    
    // Calculate statistic for a metric
    struct MetricStats {
        double min = 0.0;
        double max = 0.0;
        double mean = 0.0;
        double stddev = 0.0;
        size_t count = 0;
        double sum = 0.0;
        double sumSquared = 0.0;
    };
    
    MetricStats calculateStats(const std::string& name) const;
    
    // Clean old metrics
    void cleanupOldMetrics(std::chrono::system_clock::time_point cutoff);
    
    // Export metrics
    std::string exportToCSV() const;
    std::string exportToJSON() const;
    
    // Clear all metrics
    void clear();
    
    // Get total number of metrics recorded
    size_t getTotalCount() const;
};n

// Memory profiler
class MemoryProfiler {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
public:
    MemoryProfiler();
    ~MemoryProfiler();
    
    // Track memory allocation
    void trackAllocation(void* ptr, size_t size, const std::string& label = "");
    
    // Track memory deallocation
    void trackDeallocation(void* ptr);
    
    // Get current memory usage
    size_t getCurrentMemoryUsage() const;
    
    // Get peak memory usage
    size_t getPeakMemoryUsage() const;
    
    // Get memory usage history
    std::vector<std::pair<std::chrono::system_clock::time_point, size_t>> 
    getMemoryHistory() const;
    
    // Start memory tracking with periodic sampling
    void startTracking(size_t sampleIntervalMs = 1000);
    
    // Stop memory tracking
    void stopTracking();
    
    // Generate memory report
    std::string generateMemoryReport() const;
    
    // Get allocation statistics
    struct AllocationStats {
        size_t totalAllocated = 0;
        size_t totalDeallocated = 0;
        size_t current = 0;
        size_t peak = 0;
        std::map<std::string, size_t> allocationsByLabel;
    };
    
    AllocationStats getAllocationStats() const;
    
    // Check for memory leaks
    std::vector<void*> getPotentialLeaks() const;
    
    // Clean up tracking
    void cleanup();
};n

// CPU profiler
class CPUProfiler {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
public:
    CPUProfiler();
    ~CPUProfiler();
    
    // Start CPU profiling for a thread
    void startThreadProfiling(const std::string& threadName = "");
    
    // Stop CPU profiling for current thread
    void stopThreadProfiling();
    
    // Get CPU usage for a thread
    double getThreadCPUUsage(const std::string& threadName = "") const;
    
    // Get system CPU usage
    double getSystemCPUUsage() const;
    
    // Get process CPU usage
    double getProcessCPUUsage() const;
    
    // Get detailed CPU usage information
    struct CPUUsageInfo {
        double user = 0.0;
        double system = 0.0;
        double nice = 0.0;
        double idle = 0.0;
        double iowait = 0.0;
        double irq = 0.0;
        double softirq = 0.0;
        double steal = 0.0;
        double guest = 0.0;
        double guest_nice = 0.0;
    };
    
    CPUUsageInfo getDetailedCPUUsage() const;
    
    // Start comprehensive profiling
    void startProfiling();
    
    // Stop comprehensive profiling
    void stopProfiling();
    
    // Get profiling results
    std::string getProfilingResults() const;
    
    // Generate CPU performance report
    std::string generateCPUReport() const;
    
    // Get thread CPU usage history
    std::vector<std::pair<std::chrono::system_clock::time_point, double>> 
    getCPUUsageHistory(const std::string& threadName = "") const;
};n

// Debugging utilities
class Debugger {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
public:
    Debugger();
    ~Debugger();
    
    // Set breakpoints (placeholder for future implementation)
    void setBreakpoint(const std::string& location);
    
    // Clear breakpoints
    void clearBreakpoints();
    
    // Add watch variable
    void addWatchVariable(const std::string& expression);
    
    // Print current state
    void printState(const std::string& context = "") const;
    
    // Dump memory
    void dumpMemory(const std::string& description = "") const;
    
    // Dump stack trace
    void dumpStackTrace() const;
    
    // Enable/disable debugging
    void setDebugMode(bool enabled);
    
    bool isDebugMode() const;
    
    // Log with debug level
    void debug(const std::string& message) const;
    
    // Assert with message
    bool assert(bool condition, const std::string& message = "Assertion failed") const;
    
    // Log with different levels based on debug mode
    void log(const std::string& message) const;
    
    // Get debug info
    std::string getDebugInfo() const;
    
    // Set debug output destination
    void setDebugOutput(std::ostream& output);
    
    // Enable stack trace collection
    void enableStackTrace(bool enabled);
    
    // Get recent stack traces
    std::vector<std::string> getRecentStackTraces(size_t maxCount = 10) const;
};n

// Integrated profiling and debugging system
class ProfilingSystem {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
public:
    ProfilingSystem();
    ~ProfilingSystem();
    
    // Initialize system
    void initialize(const std::shared_ptr<Logger>& logger = nullptr);
    
    // Get profiler instance
    Profiler& getProfiler();
    
    // Get metrics collector instance
    MetricsCollector& getMetricsCollector();
    
    // Get memory profiler instance
    MemoryProfiler& getMemoryProfiler();
    
    // Get CPU profiler instance
    CPUProfiler& getCPUProfiler();
    
    // Get debugger instance
    Debugger& getDebugger();
    
    // Start all profiling
    void startAllProfiling();
    
    // Stop all profiling
    void stopAllProfiling();
    
    // Generate comprehensive report
    std::string generateReport() const;
    
    // Save report to files
    bool saveReports(const std::string& directory) const;
    
    // Export data for analysis
    std::string exportData() const;
    
    // Clean up resources
    void cleanup();
    
    // Check if system is initialized
    bool isInitialized() const;
    
    // Get system version
    static std::string getVersion();
    
    // Global access
    static ProfilingSystem& getInstance();
    
    // Configuration
    struct ProfilingConfig {
        bool enableProfiler = true;
        bool enableMetrics = true;
        bool enableMemoryProfiler = true;
        bool enableCPUProfiler = true;
        bool enableDebugger = true;
        bool enableStackTrace = false;
        size_t memorySampleIntervalMs = 1000;
        double cpuSampleIntervalS = 0.1;
        std::string outputDirectory = "profiling_output";
        bool exportOnExit = true;
        std::chrono::seconds autoSaveInterval = std::chrono::seconds(60);
    };
    
    // Configure profiling system
    void configure(const ProfilingConfig& config);
};n

// Convenience functions for common profiling tasks
namespace Profiling {
    // Start/stop scoped timers
    class ScopedTimer {
    public:
        ScopedTimer(const std::string& name) {
            ProfilingSystem::getInstance().getProfiler().start(name);
            startTime_ = std::chrono::high_resolution_clock::now();
            name_ = name;
        }
        
        ~ScopedTimer() {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime_).count();
            ProfilingSystem::getInstance().getProfiler().stop(name_);
            NLM_LOG_DEBUG("Scoped timer '" + name_ + "': " + std::to_string(duration) + "μs");
        }
        
    private:
        std::chrono::high_resolution_clock::time_point startTime_;
        std::string name_;
    };
    
    // Convenience macros
    #define PROFILE_SCOPE(name) nlm::Profiling::ScopedTimer timer_##__LINE__(name)
    
    // Simple function timing
    void timeFunction(const std::string& name, std::function<void()> func);
    
    // Memory tracking
    void trackMemory(const std::string& label, size_t size);
    
    // Performance assertion
    bool assertPerformance(const std::string& testName, double maxTimeSeconds,
                          std::function<void()> testFunc);
    
    // Benchmark function
    struct BenchmarkResult {
        std::string name;
        double timeSeconds;
        size_t iterations;
        double averageTime;
        double minTime;
        double maxTime;
        double stddev;
    };
    
    BenchmarkResult benchmark(const std::string& name, std::function<void()> func,
                             size_t iterations = 100);
    
    // Report generation
    std::string generateReport();
    
    // Export results
    bool exportResults(const std::string& filepath);
    
    // Check system status
    bool isProfilingActive();
    bool isMetricsEnabled();
    bool isDebuggingEnabled();
}

} // namespace nlm

#endif // NLM_PROFILING_HPP