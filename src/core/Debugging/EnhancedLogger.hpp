#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <map>
#include <set>

namespace nlm {

// Enhanced log levels for advanced debugging
enum class DebugLogLevel {
    DEBUG = 0,
    INFO = 1,
    WARNING = 2,
    ERROR = 3,
    CRITICAL = 4
};

// Log entry with enhanced metadata for debugging
struct DebugLogEntry {
    DebugLogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::string component;  // Module/component name
    uint64_t threadId;       // Thread identifier for debugging
    bool filtered = false;   // If filtered by component
    std::vector<std::string> tags;  // Debug tags for categorization
    uint64_t sequenceNumber;  // Sequence number for ordering
};

// Component filter configuration for selective logging
struct ComponentFilter {
    std::string name;
    bool enabled = true;
    DebugLogLevel minLevel = DebugLogLevel::DEBUG;
    std::vector<std::string> includePatterns;
    std::vector<std::string> excludePatterns;
    std::vector<std::string> tags;
};

// Performance metrics structure for timing and profiling
struct PerformanceMetrics {
    std::string operationName;
    std::chrono::duration<double> duration;
    size_t memoryUsage;  // bytes
    uint64_t callCount;
    double avgDuration;
    double minDuration;
    double maxDuration;
    std::string threadId;
    std::chrono::system_clock::time_point timestamp;
    std::vector<std::string> callStack;
};

// Phase 6 integration debug information
struct Phase6DebugInfo {
    std::string phase;
    std::string component;
    std::string operation;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    bool success;
    std::string errorMessage;
    std::string systemState;  // JSON string of system state
    size_t memoryUsage;       // bytes
    float spikeRate;          // Hz
    std::vector<std::string> debugMessages;
    std::vector<float> traceData;  // For memory/episodic debugging
    std::map<std::string, std::string> metadata;
};

// Configuration validation result for debugging
struct ConfigValidationResult {
    std::string parameter;
    ConfigValue value;
    bool isValid;
    std::string errorMessage;
    std::string warningMessage;
    std::chrono::system_clock::time_point timestamp;
    std::string source;  // Where the config was set
    std::vector<std::string> validationSteps;
};

// Error context for detailed error recovery debugging
struct ErrorContext {
    std::string errorId;
    std::string errorType;
    std::string errorMessage;
    std::string sourceFile;
    int sourceLine;
    std::string sourceFunction;
    std::chrono::system_clock::time_point timestamp;
    std::string stackTrace;
    std::unordered_map<std::string, std::string> contextData;
    std::vector<std::string> recoveryAttempts;
    bool recovered;
    std::chrono::system_clock::time_point firstOccurrence;
    int occurrenceCount;
};

// Recovery attempt information
struct RecoveryAttempt {
    std::string attemptId;
    std::chrono::system_clock::time_point timestamp;
    std::string attemptType;
    std::string description;
    bool success;
    std::string errorMessage;
    std::string systemState;  // JSON string of state before recovery
    std::chrono::duration<double> recoveryTime;
};

// Advanced console logger with component filtering and colors
class AdvancedConsoleLogger {
public:
    AdvancedConsoleLogger();
    explicit AdvancedConsoleLogger(DebugLogLevel level);
    ~AdvancedConsoleLogger();
    
    void log(const DebugLogEntry& entry);
    void setLevel(DebugLogLevel level);
    DebugLogLevel getLevel() const;
    
    // Color and formatting
    void setUseColors(bool useColors);
    bool getUseColors() const;
    void setColorMap(const std::map<std::string, std::string>& colors);
    
    // Component filtering
    void addComponentFilter(const ComponentFilter& filter);
    void removeComponentFilter(const std::string& name);
    void clearComponentFilters();
    
    // Output management
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    void setMaxFileSize(size_t maxSize);
    void setBackupCount(size_t count);
    
    // Log formatting
    void setTimestampFormat(const std::string& format);
    void setIncludeThreadId(bool include);
    void setIncludeSequenceNumbers(bool include);
    
    // Performance tracking
    void enablePerformanceTracking(bool enable);
    bool isPerformanceTrackingEnabled() const;
    std::vector<PerformanceMetrics> getRecentPerformanceMetrics(size_t limit = 100) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(DebugLogLevel level) const;
    std::string componentToColor(const std::string& component) const;
    std::string formatTimestamp(const std::chrono::system_clock::time_point& timestamp) const;
    bool shouldLogComponent(const DebugLogEntry& entry) const;
    void rotateLogFileIfNeeded();
    void writeToFile(const std::string& message);
    void recordPerformanceMetric(const std::string& operation, double duration, size_t memoryUsage);
};

// Enhanced Logger class with comprehensive debugging features
class EnhancedLogger {
public:
    EnhancedLogger();
    ~EnhancedLogger();
    
    // Disable copying
    EnhancedLogger(const EnhancedLogger&) = delete;
    EnhancedLogger& operator=(const EnhancedLogger&) = delete;
    
    // Singleton access
    static EnhancedLogger& getGlobal();
    
    // Global configuration
    void initialize(bool enableDebugLogging = false, DebugLogLevel level = DebugLogLevel::INFO);
    void enableDebugLogging(bool enable);
    bool isDebugLoggingEnabled() const;
    void setLogLevel(DebugLogLevel level);
    DebugLogLevel getLogLevel() const;
    
    // Component filtering
    void addComponentFilter(const ComponentFilter& filter);
    void removeComponentFilter(const std::string& name);
    void clearComponentFilters();
    
    // Performance monitoring
    void startPerformanceMeasurement(const std::string& operation);
    void stopPerformanceMeasurement(const std::string& operation);
    void enableMemoryTracking(bool enable);
    bool isMemoryTrackingEnabled() const;
    
    // Basic logging methods
    void log(DebugLogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "", 
             const std::string& component = "");
    
    void debug(const std::string& message, const std::string& component = "",
               const std::string& file = "", int line = 0,
               const std::string& function = "");
    void info(const std::string& message, const std::string& component = "",
              const std::string& file = "", int line = 0,
              const std::string& function = "");
    void warning(const std::string& message, const std::string& component = "",
                 const std::string& file = "", int line = 0,
                 const std::string& function = "");
    void error(const std::string& message, const std::string& component = "",
               const std::string& file = "", int line = 0,
               const std::string& function = "");
    void critical(const std::string& message, const std::string& component = "",
                  const std::string& file = "", int line = 0,
                  const std::string& function = "");
    
    // Convenience stream-style logging
    class LogStream {
    public:
        LogStream(DebugLogLevel level, const std::string& component = "",
                  const std::string& file = "", int line = 0,
                  const std::string& function = "");
        ~LogStream();
        
        template<typename T>
        LogStream& operator<<(const T& value) {
            oss_ << value;
            return *this;
        }
        
        LogStream& operator<<(const std::string& value) {
            oss_ << value;
            return *this;
        }
        
    private:
        DebugLogLevel level_;
        std::string component_;
        std::ostringstream oss_;
        std::string file_;
        int line_;
        std::string function_;
    };
    
    // Convenience methods for stream logging
    LogStream debugStream(const std::string& component = "",
                         const std::string& file = "", int line = 0,
                         const std::string& function = "");
    LogStream infoStream(const std::string& component = "",
                        const std::string& file = "", int line = 0,
                        const std::string& function = "");
    LogStream warningStream(const std::string& component = "",
                           const std::string& file = "", int line = 0,
                           const std::string& function = "");
    LogStream errorStream(const std::string& component = "",
                         const std::string& file = "", int line = 0,
                         const std::string& function = "");
    LogStream criticalStream(const std::string& component = "",
                            const std::string& file = "", int line = 0,
                            const std::string& function = "");
    
    // Phase 6 integration debugging
    void logPhase6Step(const std::string& phase, const std::string& component,
                      const std::string& operation, bool success,
                      const std::string& error = "");
    void logSystemConnection(const std::string& source, const std::string& target,
                            bool connected, const std::string& details = "");
    void logMemoryTrace(const std::string& type, size_t size, const std::vector<float>& data);
    void logNeuromodulation(const std::string& system, float value, float rate);
    void logDevelopmentStage(const std::string& stage, float progress,
                           const std::string& details = "");
    
    // Configuration debugging
    void validateConfigParameter(const std::string& key, const ConfigValue& value,
                                bool isValid, const std::string& error = "",
                                const std::string& source = "");
    void enableConfigValidation(bool enable);
    bool isConfigValidationEnabled() const;
    std::vector<ConfigValidationResult> getValidationHistory() const;
    
    // Error recovery debugging
    void logError(const ErrorContext& error);
    void logRecoveryAttempt(const RecoveryAttempt& attempt);
    void setMaxRecoveryAttempts(size_t max);
    std::vector<ErrorContext> getRecentErrors(size_t limit = 100) const;
    std::vector<RecoveryAttempt> getRecoveryAttempts(const std::string& errorId) const;
    
    // Statistics and reporting
    std::string getStatisticsReport() const;
    void generateDebugReport(const std::string& filepath) const;
    void clearDebugData();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Global enhanced logger instance
EnhancedLogger& getEnhancedLogger();

// Convenience macros for enhanced debugging
#define DEBUG_LOG_COMP(msg, component) nlm::getEnhancedLogger().debug(msg, component, __FILE__, __LINE__, __func__)
#define INFO_LOG_COMP(msg, component) nlm::getEnhancedLogger().info(msg, component, __FILE__, __LINE__, __func__)
#define WARNING_LOG_COMP(msg, component) nlm::getEnhancedLogger().warning(msg, component, __FILE__, __LINE__, __func__)
#define ERROR_LOG_COMP(msg, component) nlm::getEnhancedLogger().error(msg, component, __FILE__, __LINE__, __func__)
#define CRITICAL_LOG_COMP(msg, component) nlm::getEnhancedLogger().critical(msg, component, __FILE__, __LINE__, __func__)

// Performance monitoring macros
#define PERF_START(op) nlm::getEnhancedLogger().startPerformanceMeasurement(op)
#define PERF_STOP(op) nlm::getEnhancedLogger().stopPerformanceMeasurement(op)

// Phase 6 integration debugging macros
#define PHASE6_LOG_STEP(phase, component, operation, success) \
    nlm::getEnhancedLogger().logPhase6Step(phase, component, operation, success)
#define PHASE6_LOG_CONNECTION(source, target, connected, details) \
    nlm::getEnhancedLogger().logSystemConnection(source, target, connected, details)

// Configuration validation macros
#define CONFIG_VALIDATE(key, value, isValid, error, source) \
    nlm::getEnhancedLogger().validateConfigParameter(key, value, isValid, error, source)

// Error recovery macros
#define ERROR_RECOVERY_LOG(error) nlm::getEnhancedLogger().logError(error)
#define RECOVERY_ATTEMPT_LOG(attempt) nlm::getEnhancedLogger().logRecoveryAttempt(attempt)

// Performance timing utility
class PerformanceTimer {
public:
    PerformanceTimer(const std::string& operationName, bool log = true);
    ~PerformanceTimer();
    
    void stop();
    double getDuration() const;
    
private:
    std::string operationName_;
    bool log_;
    std::chrono::system_clock::time_point startTime_;
    bool stopped_;
    double duration_;
};

// Enhanced performance timer with system and component tracking
class PerformanceTimerAdvanced {
public:
    PerformanceTimerAdvanced(const std::string& operationName, const std::string& system = "",
                           const std::string& component = "", bool log = true);
    ~PerformanceTimerAdvanced();
    
    void stop();
    double getDuration() const;
    
    // Start/stop without constructing
    void start();
    void stopWithMetadata(size_t memoryUsage = 0);
    
private:
    std::string operationName_;
    std::string system_;
    std::string component_;
    bool log_;
    std::chrono::system_clock::time_point startTime_;
    bool stopped_;
    double duration_;
    size_t memoryUsage_;
};

#define PERF_TIMER(op) nlm::PerformanceTimer timer_##op(#op, true)
#define PERF_TIMER_NC(op) nlm::PerformanceTimer timer_##op(#op, false)

#define PERF_TIMER_ADV(op, sys, comp) nlm::PerformanceTimerAdvanced timer_adv_##op(#op, sys, comp, true)
#define PERF_TIMER_ADV_NC(op, sys, comp) nlm::PerformanceTimerAdvanced timer_adv_##op(#op, sys, comp, false)

// Memory tracking utility
class MemoryTracker {
public:
    MemoryTracker(const std::string& operationName);
    ~MemoryTracker();
    
    void stop();
    size_t getMemoryUsage() const;
    
private:
    std::string operationName_;
    bool stopped_;
    size_t memoryUsage_;
};

// Enhanced memory tracker with system and component tracking
class MemoryTrackerAdvanced {
public:
    MemoryTrackerAdvanced(const std::string& operationName, const std::string& system = "",
                         const std::string& component = "");
    ~MemoryTrackerAdvanced();
    
    void stop();
    size_t getMemoryUsage() const;
    
    // Start/stop without constructing
    void start();
    void stopWithMetadata();
    
private:
    std::string operationName_;
    std::string system_;
    std::string component_;
    bool stopped_;
    size_t memoryUsage_;
    std::chrono::system_clock::time_point startTime_;
};

#define MEMORY_TRACK(op) nlm::MemoryTracker tracker_##op(#op)
#define MEMORY_TRACK_ADV(op, sys, comp) nlm::MemoryTrackerAdvanced tracker_adv_##op(#op, sys, comp)

// Integration with global performance profiler
#define PERF_START_GLOBAL(op, sys, comp) nlm::getPerformanceProfiler().startOperation(op, sys, comp)
#define PERF_STOP_GLOBAL(op) nlm::getPerformanceProfiler().stopOperation(op)

#define MEMORY_TRACK_GLOBAL(op, sys, comp) \
    nlm::MemoryTrackerAdvanced mem_tracker_##op(#op, sys, comp); \
    mem_tracker_##op.start();

#define MEMORY_TRACK_GLOBAL_NC(op, sys, comp) \
    nlm::MemoryTrackerAdvanced mem_tracker_##op(#op, sys, comp); \
    mem_tracker_##op.start(); \
    mem_tracker_##op.stopWithMetadata()

// Phase 6 integration macros
#define PHASE6_PERF_START(phase, component, operation) \
    nlm::getPerformanceProfiler().enablePhase6Integration(true); \
    PERF_TIMER_ADV(phase##component##operation, "phase6", #component)

#define PHASE6_PERF_STOP(phase, component, operation) \
    PERF_TIMER_ADV_NC(phase##component##operation, "phase6", #component).stop(); \
    nlm::getPerformanceProfiler().logPhase6Performance(#phase, #component, \
        PERF_TIMER_ADV_NC(phase##component##operation, "phase6", #component).getDuration(), true)

} // namespace nlm
