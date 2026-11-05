#if !defined(NLM_PERFORMANCE_HPP)
#define NLM_PERFORMANCE_HPP

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <mutex>
#include <shared_mutex>
#include <numeric>
#include <algorithm>
#include <iomanip>
#include <fstream>
#include <stacktrace>
#include <filesystem>

// Forward declarations
namespace nlm {
    class PerformanceProfiler;
    class MemoryTracker;
    class Timer;
    class LogStream;
}

// Performance measurement types
struct PerformanceMetrics {
    std::string name;
    std::chrono::nanoseconds duration;
    size_t memoryUsage; // bytes
    double cpuUsage;    // percentage (0.0 - 100.0)
    uint64_t timestamp;
    std::string threadId;
    std::string context;
    
    PerformanceMetrics(const std::string& name, std::chrono::nanoseconds duration,
                      size_t memoryUsage = 0, double cpuUsage = 0.0,
                      uint64_t timestamp = 0, const std::string& threadId = "",
                      const std::string& context = "")
        : name(name), duration(duration), memoryUsage(memoryUsage), cpuUsage(cpuUsage),
          timestamp(timestamp), threadId(threadId), context(context) {}
};

struct MemoryInfo {
    size_t totalMemory;
    size_t usedMemory;
    size_t freeMemory;
    size_t allocatedMemory;
    size_t residentSetSize;
    size_t virtualMemory;
    
    MemoryInfo() : totalMemory(0), usedMemory(0), freeMemory(0),
                   allocatedMemory(0), residentSetSize(0), virtualMemory(0) {}
};

struct SystemInfo {
    std::string hostname;
    std::string operatingSystem;
    std::string architecture;
    std::string processorModel;
    int cpuCores;
    double cpuFrequencyMHz;
    size_t totalRAM;
    std::string gpuModel;
    double gpuMemoryGB;
};

// Timer class for performance measurement
class Timer {
private:
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;
    bool running;
    std::string name;
    std::string context;
    
public:
    Timer(const std::string& name = "timer", const std::string& context = "")
        : running(false), name(name), context(context) {}
    
    void start() {
        startTime = std::chrono::high_resolution_clock::now();
        running = true;
    }
    
    void stop() {
        if (running) {
            endTime = std::chrono::high_resolution_clock::now();
            running = false;
        }
    }
    
    void reset() {
        running = false;
    }
    
    std::chrono::nanoseconds elapsed() const {
        if (running) {
            return std::chrono::duration_cast<std::chrono::nanoseconds>(
                std::chrono::high_resolution_clock::now() - startTime);
        } else {
            return std::chrono::duration_cast<std::chrono::nanoseconds>(
                endTime - startTime);
        }
    }
    
    std::chrono::milliseconds elapsedMs() const {
        return std::chrono::duration_cast<std::chrono::milliseconds>(elapsed());
    }
    
    std::chrono::microseconds elapsedUs() const {
        return std::chrono::duration_cast<std::chrono::microseconds>(elapsed());
    }
    
    std::chrono::nanoseconds elapsedNs() const {
        return elapsed();
    }
    
    bool isRunning() const { return running; }
    const std::string& getName() const { return name; }
    const std::string& getContext() const { return context; }
};

// Enhanced logger with performance monitoring
class PerformanceLogger {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
public:
    PerformanceLogger();
    ~PerformanceLogger();
    
    // Disable copying
    PerformanceLogger(const PerformanceLogger&) = delete;
    PerformanceLogger& operator=(const PerformanceLogger&) = delete;
    
    // Start/stop performance tracking
    void startProfiling(const std::string& operation, const std::string& context = "");
    PerformanceMetrics stopProfiling(const std::string& operation);
    
    // Log performance metrics
    void logMetrics(const PerformanceMetrics& metrics);
    void logMetrics(const std::string& operation, const Timer& timer,
                   size_t memoryUsage = 0, double cpuUsage = 0.0);
    
    // Memory tracking
    void startMemoryTracking(const std::string& context = "");
    MemoryInfo getCurrentMemoryInfo() const;
    void logMemoryUsage(const std::string& context = "");
    
    // System information
    SystemInfo getSystemInfo() const;
    void logSystemInfo();
    
    // Profiling session management
    void startSession(const std::string& sessionName);
    void endSession();
    std::string getCurrentSession() const;
    std::vector<PerformanceMetrics> getSessionMetrics() const;
    
    // Performance analysis
    void analyzePerformance(const std::string& operation, 
                           std::vector<PerformanceMetrics>& metrics);
    std::vector<std::string> getPerformanceRecommendations() const;
    
    // Configuration
    void setLogLevel(int minLevel);
    void setMaxMetricsSize(size_t maxSize);
    void setProfileInterval(double interval); // seconds
    void setMemoryTrackingEnabled(bool enabled);
    
    // Output
    void setOutputFile(const std::string& filepath);
    void enableRealTimeOutput(bool enabled);
    void setOutputFormat(const std::string& format); // JSON, CSV, PLAIN
    
    // Get statistics
    size_t getMetricsCount() const;
    size_t getActiveTimersCount() const;
    double getAverageOperationTime(const std::string& operation) const;
    
    // Clear
    void clearMetrics();
    void clearSession();
    
    // Global instance
    static PerformanceLogger& getGlobal();
    static void setGlobal(std::unique_ptr<PerformanceLogger> logger);
};

// Enhanced LogStream with performance context
class PerformanceLogStream {
private:
    std::chrono::high_resolution_clock::time_point startTime;
    std::string operationName;
    std::string context;
    bool profilingEnabled;
    
public:
    PerformanceLogStream(const std::string& operationName = "log_operation",
                        const std::string& context = "", bool profilingEnabled = false);
    
    template<typename T>
    PerformanceLogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    ~PerformanceLogStream();
    
    std::string getMessage() const { return oss_.str(); }
    std::chrono::nanoseconds getDuration() const {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::high_resolution_clock::now() - startTime);
    }
    
private:
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
};

// Performance analysis utilities
class PerformanceAnalyzer {
public:
    static std::vector<std::string> analyzeBottlenecks(
        const std::vector<PerformanceMetrics>& metrics);
    
    static double calculateThroughput(const std::vector<PerformanceMetrics>& metrics);
    
    static std::vector<std::string> getOptimizationSuggestions(
        const std::vector<PerformanceMetrics>& metrics);
    
    static std::string generatePerformanceReport(
        const std::vector<PerformanceMetrics>& metrics);
    
    static std::unordered_map<std::string, double> calculateMetrics(
        const std::vector<PerformanceMetrics>& metrics);
};

// Memory tracking utilities
class MemoryTracker {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
public:
    MemoryTracker();
    ~MemoryTracker();
    
    void startTracking(const std::string& context = "");
    MemoryInfo getMemoryInfo() const;
    void logMemoryUsage(const std::string& context = "");
    std::vector<MemoryInfo> getMemoryHistory() const;
    void clearHistory();
    
    static size_t getCurrentRSS();
    static size_t getCurrentVMS();
    static size_t getPeakRSS();
};

// System performance utilities
class SystemPerformance {
public:
    static SystemInfo getSystemInfo();
    static double getCPUUsage();
    static size_t getProcessMemoryUsage();
    static std::vector<double> getCPUUsagePerCore();
    static double getGPUMemoryUsage();
    static std::string getGPUInfo();
    
    static void setCPUMonitoringEnabled(bool enabled);
    static void setMemoryMonitoringEnabled(bool enabled);
    static void setGPUMonitoringEnabled(bool enabled);
};

// Convenience macros for performance logging
#define NLM_PERF_START(operation) nlm::PerformanceLogger::getGlobal().startProfiling(operation)
#define NLM_PERF_STOP(operation) nlm::PerformanceLogger::getGlobal().stopProfiling(operation)
#define NLM_PERF_LOG(operation, timer) nlm::PerformanceLogger::getGlobal().logMetrics(operation, timer)
#define NLM_PERF_TIMER(name) nlm::Timer timer##__LINE__(name, "code: " #__LINE__)
#define NLM_PERF_SCOPE(operation) nlm::PerformanceLogStream perfStream##__LINE__(operation)

// Stream-style performance logging
class PerfStream {
public:
    PerfStream(const std::string& operation) {
        PerformanceLogger::getGlobal().startProfiling(operation);
    }
    
    template<typename T>
    PerfStream& operator<<(const T& value) {
        // Store for later output
        return *this;
    }
    
    ~PerfStream() {
        PerformanceLogger::getGlobal().stopProfiling("stream_operation");
    }
};

#define NLM_PERF_SCOPE_LOG(operation) nlm::PerfStream perfScope##__LINE__(operation)

#endif // NLM_PERFORMANCE_HPP
