#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <mutex>
#include <atomic>
#include <type_traits>

namespace nlm {

// Log severity levels
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4
};

// Performance metrics
struct PerformanceMetrics {
    std::chrono::nanoseconds cpuTime;
    size_t memoryUsage;
    size_t activeNeurons;
    size_t firingNeurons;
    size_t totalSpikes;
    float averageFiringRate;
    float synapticActivity;
    float learningRate;
    std::chrono::system_clock::time_point timestamp;
};

// Simulation metrics
struct SimulationMetrics {
    std::chrono::nanoseconds simulationTime;
    size_t totalSteps;
    size_t stepsPerSecond;
    size_t neuronsCreated;
    size_t neuronsDestroyed;
    size_t synapsesCreated;
    size_t synapsesDestroyed;
    float convergenceRate;
    float stabilityIndex;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
};

// Debug snapshot for analysis
struct DebugSnapshot {
    std::chrono::system_clock::time_point timestamp;
    std::string simulationStage;
    PerformanceMetrics perfMetrics;
    SimulationMetrics simMetrics;
    std::unordered_map<std::string, double> systemMetrics;
    std::vector<std::string> activeEvents;
};

// Log entry structure
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
};

// Logger interface
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    
    // Enhanced logging with metrics
    virtual void logWithMetrics(LogLevel level, const std::string& message, 
                               const PerformanceMetrics& metrics, 
                               const std::string& file = "", int line = 0, 
                               const std::string& function = "");
    
    // Performance monitoring
    virtual void recordPerformanceMetrics(const PerformanceMetrics& metrics);
    virtual PerformanceMetrics getCurrentPerformanceMetrics() const;
};

// Enhanced Console logger implementation
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(LogLevel level);
    ~ConsoleLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Enable/disable colors
    void setUseColors(bool useColors);
    bool getUseColors() const;
    
    // Set output file (in addition to console)
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    
    // Performance monitoring
    void setPerformanceMonitoring(bool enabled);
    bool getPerformanceMonitoring() const;
    void setMetricsCollectionInterval(std::chrono::milliseconds interval);
    
    // Performance logging
    void logPerformanceMetrics(const PerformanceMetrics& metrics);
    void logSimulationMetrics(const SimulationMetrics& metrics);
    
    // Debug snapshot management
    void takeDebugSnapshot(const std::string& stage);
    std::vector<DebugSnapshot> getDebugSnapshots() const;
    void clearDebugSnapshots();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
    void logFormatted(const std::string& levelStr, const std::string& color, const std::string& message);
};

// Memory usage monitor
class MemoryMonitor {
public:
    MemoryMonitor();
    ~MemoryMonitor();
    
    void startMonitoring();
    void stopMonitoring();
    bool isMonitoring() const;
    
    size_t getCurrentMemoryUsage() const;
    size_t getPeakMemoryUsage() const;
    size_t getTotalMemoryAllocated() const;
    
    // Memory tracking
    void trackMemoryAllocation(size_t size);
    void trackMemoryDeallocation(size_t size);
    void trackMemoryReallocation(size_t oldSize, size_t newSize);
    
    // Memory statistics
    std::vector<size_t> getMemoryUsageHistory() const;
    std::chrono::system_clock::time_point getLastAllocationTime() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Performance profiler
class PerformanceProfiler {
public:
    PerformanceProfiler();
    ~PerformanceProfiler();
    
    void startProfiling(const std::string& operation);
    void stopProfiling(const std::string& operation);
    void pauseProfiling(const std::string& operation);
    void resumeProfiling(const std::string& operation);
    
    // Profile data
    struct ProfileData {
        std::string operation;
        std::chrono::nanoseconds totalTime;
        std::chrono::nanoseconds exclusiveTime;
        std::chrono::nanoseconds cpuTime;
        size_t callCount;
        std::chrono::system_clock::time_point startTime;
        std::chrono::system_clock::time_point endTime;
        bool isActive;
    };
    
    std::vector<ProfileData> getProfileData() const;
    ProfileData getProfileData(const std::string& operation) const;
    
    // Profiling statistics
    std::chrono::nanoseconds getTotalProfileTime() const;
    std::chrono::nanoseconds getAverageProfileTime(const std::string& operation) const;
    size_t getProfileCount(const std::string& operation) const;
    
    // Clear profiles
    void clearProfiles();
    void clearProfile(const std::string& operation);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Main Logger class with enhanced debugging capabilities
class Logger {
public:
    Logger();
    ~Logger();
    
    // Disable copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // Add logger sink
    void addLogger(std::shared_ptr<ILogger> logger);
    
    // Remove all loggers
    void clearLoggers();
    
    // Set minimum level for all loggers
    void setLevel(LogLevel level);
    
    // Global logging functions
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "");
    
    // Convenience methods
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Enhanced logging with metrics
    void logWithMetrics(LogLevel level, const std::string& message, 
                        const PerformanceMetrics& metrics,
                        const std::string& file = "", int line = 0, 
                        const std::string& function = "");
    
    // Performance monitoring
    void recordPerformanceMetrics(const PerformanceMetrics& metrics);
    void recordSimulationMetrics(const SimulationMetrics& metrics);
    PerformanceMetrics getCurrentPerformanceMetrics() const;
    SimulationMetrics getCurrentSimulationMetrics() const;
    
    // Debug snapshot management
    void takeDebugSnapshot(const std::string& stage);
    std::vector<DebugSnapshot> getDebugSnapshots() const;
    void clearDebugSnapshots();
    
    // Memory monitoring
    void startMemoryMonitoring();
    void stopMemoryMonitoring();
    size_t getCurrentMemoryUsage() const;
    
    // Performance profiling
    void startProfiling(const std::string& operation);
    void stopProfiling(const std::string& operation);
    void pauseProfiling(const std::string& operation);
    void resumeProfiling(const std::string& operation);
    std::vector<PerformanceProfiler::ProfileData> getProfileData() const;
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Global monitoring control
    static void enableAllMonitoring(bool enable);
    static bool isAllMonitoringEnabled();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced logging macros with performance metrics
#define NLM_LOG_DEBUG_WITH_METRICS(msg, metrics) \
    nlm::Logger::getGlobal().logWithMetrics(nlm::LogLevel::Debug, msg, metrics, __FILE__, __LINE__, __func__)

#define NLM_LOG_INFO_WITH_METRICS(msg, metrics) \
    nlm::Logger::getGlobal().logWithMetrics(nlm::LogLevel::Info, msg, metrics, __FILE__, __LINE__, __func__)

#define NLM_LOG_PERFORMANCE(msg, metrics) \
    nlm::Logger::getGlobal().logWithMetrics(nlm::LogLevel::Info, msg, metrics, __FILE__, __LINE__, __func__)

// Stream-style logging helper with metrics
class LogStream {
public:
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function);
    ~LogStream();
    
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    // Performance metrics for the log entry
    LogStream& operator<<(const PerformanceMetrics& metrics);
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
};

#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

// Convenience functions for monitoring
void startPerformanceMonitoring(bool enable = true);
void recordSimulationStep(size_t neuronsFiring, size_t totalSpikes, float averageFiringRate);
void recordMemoryUsage(size_t memoryBytes);
void recordLearningEvent(float learningRate, float plasticityChange);
void takeBenchmarkSnapshot(const std::string& benchmarkName);

// Monitoring statistics
struct MonitoringStats {
    size_t totalSimulations;
    std::chrono::nanoseconds totalSimulationTime;
    size_t peakMemoryUsage;
    float averageFiringRate;
    float convergenceScore;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
};

MonitoringStats getMonitoringStats();
void resetMonitoringStats();

} // namespace nlm