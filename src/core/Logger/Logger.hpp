#pragma once

#include <string>
#include <unordered_map>
#include <chrono>
#include <vector>
#include <memory>
#include <mutex>
#include <fstream>
#include <functional>
#include <map>
#include <iomanip>
#include <sstream>
#include <thread>

namespace nlm {

// Structured log entry with correlation ID and metadata
struct StructuredLogEntry {
    std::string correlationId;
    std::chrono::system_clock::time_point timestamp;
    std::string category;
    std::string level;
    std::string message;
    std::unordered_map<std::string, std::string> metadata;
    std::unordered_map<std::string, double> metrics;
    std::string file;
    int line;
    std::string function;
};

// Log category enumeration
enum class LogCategory {
    System = 0,
    Simulation = 1,
    Neural = 2,
    Memory = 3,
    Performance = 4,
    Network = 5,
    GUI = 6,
    Error = 7,
    Audit = 8
};

// Enhanced LogLevel with Trace level
enum class LogLevel {
    Trace = 0,
    Debug = 1,
    Info = 2,
    Warning = 3,
    Error = 4,
    Critical = 5
};

// Performance metrics structure
struct PerformanceMetric {
    std::string name;
    double value;
    std::string unit;
    std::chrono::system_clock::time_point timestamp;
    std::unordered_map<std::string, std::string> tags;
};

// Advanced logging system with structured logging, correlation IDs, and performance monitoring
class StructuredLogger {
public:
    StructuredLogger();
    ~StructuredLogger();
    
    // Core logging methods with structured data
    void trace(const std::string& message, const std::string& category = "System",
              const std::string& correlationId = "",
              const std::unordered_map<std::string, std::string>& metadata = {});
    void debug(const std::string& message, const std::string& category = "System",
              const std::string& correlationId = "",
              const std::unordered_map<std::string, std::string>& metadata = {});
    void info(const std::string& message, const std::string& category = "System",
             const std::string& correlationId = "",
             const std::unordered_map<std::string, std::string>& metadata = {});
    void warning(const std::string& message, const std::string& category = "System",
                 const std::string& correlationId = "",
                 const std::unordered_map<std::string, std::string>& metadata = {});
    void error(const std::string& message, const std::string& category = "Error",
              const std::string& correlationId = "",
              const std::unordered_map<std::string, std::string>& metadata = {});
    void critical(const std::string& message, const std::string& category = "Error",
                  const std::string& correlationId = "",
                  const std::unordered_map<std::string, std::string>& metadata = {});
    
    // Structured logging with performance metrics
    void logPerformance(const std::string& metricName, double value,
                       const std::string& unit = "ms",
                       const std::string& category = "Performance",
                       const std::unordered_map<std::string, std::string>& tags = {});
    
    // Context-aware logging with correlation ID
    void logContext(const std::string& category, const std::string& correlationId,
                   LogLevel level, const std::string& message,
                   const std::unordered_map<std::string, std::string>& metadata = {});
    
    // Memory allocation/deallocation logging
    void logMemoryAllocation(const std::string& pointerId, size_t size,
                            const std::string& category = "Memory",
                            const std::string& correlationId = "");
    void logMemoryDeallocation(const std::string& pointerId,
                              const std::string& category = "Memory",
                              const std::string& correlationId = "");
    
    // CPU profiling and bottleneck identification
    void logCPUProfile(const std::string& functionName, double cpuTime,
                       int threadId = -1,
                       const std::string& category = "Performance",
                       const std::unordered_map<std::string, std::string>& metadata = {});
    
    // Network communication logging
    void logNetworkActivity(const std::string& operation, const std::string& endpoint,
                           double latency, size_t bytesSent, size_t bytesReceived,
                           const std::string& category = "Network",
                           const std::string& correlationId = "");
    
    // GUI debugging
    void logGUIClick(const std::string& elementId, const std::string& elementType,
                    const std::string& correlationId = "");
    void logGUIUpdate(const std::string& elementId, const std::string& updateType,
                     const std::string& correlationId = "");
    
    // Error recovery mechanisms
    void logErrorRecovery(const std::string& errorType, const std::string& recoveryAction,
                         bool success, const std::string& correlationId = "",
                         const std::unordered_map<std::string, std::string>& context = {});
    
    // Audit logging
    void logAuditEvent(const std::string& eventType, const std::string& userId,
                      const std::string& action, const std::string& correlationId = "",
                      const std::unordered_map<std::string, std::string>& details = {});
    
    // Simulation-specific logging
    void logSimulationStep(int step, double simulationTime, int activeNeurons,
                           int totalNeurons, const std::string& correlationId = "");
    
    // Performance benchmarking
    void logBenchmark(const std::string& benchmarkName, double executionTime,
                      int iterations, const std::string& category = "Benchmark");
    
    // System health monitoring
    void logSystemHealth(double cpuUsage, double memoryUsage,
                        double diskUsage, const std::string& correlationId = "");
    
    // Debug breakpoints
    void setDebugBreakpoint(bool condition, const std::string& message,
                           const std::string& correlationId = "");
    
    // Real-time monitoring integration
    void registerPerformanceMetric(const PerformanceMetric& metric);
    std::vector<PerformanceMetric> getRecentMetrics(size_t limit = 100) const;
    
    // Log file management
    void rotateLogFiles(size_t maxFileSizeMB = 10, int maxBackupFiles = 5);
    void flushLogs();
    
    // Correlation ID management
    std::string generateCorrelationId() const;
    void startCorrelationTrace(const std::string& correlationId,
                              const std::string& operation,
                              const std::unordered_map<std::string, std::string>& metadata = {});
    void endCorrelationTrace(const std::string& correlationId,
                           bool success, const std::string& result = "");
    
    // Singleton pattern
    static StructuredLogger& getInstance();
    static void setGlobalStructuredLogger(std::shared_ptr<StructuredLogger> logger);
    
    // Configuration
    void setMinimumLevel(LogLevel level);
    void setMinimumCategory(LogCategory category, bool enabled = true);
    void setLogFile(const std::string& filepath);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    void logInternal(const StructuredLogEntry& entry);
    std::string levelToString(LogLevel level) const;
    std::string categoryToString(LogCategory category) const;
    std::string formatStructuredLog(const StructuredLogEntry& entry) const;
};

// Convenience macros for structured logging
#define LOG_TRACE(msg, ...) nlm::StructuredLogger::getInstance().trace(msg, "System", __VA_ARGS__)
#define LOG_DEBUG(msg, ...) nlm::StructuredLogger::getInstance().debug(msg, "System", __VA_ARGS__)
#define LOG_INFO(msg, ...) nlm::StructuredLogger().info(msg, "System", __VA_ARGS__)
#define LOG_WARNING(msg, ...) nlm::StructuredLogger::getInstance().warning(msg, "System", __VA_ARGS__)
#define LOG_ERROR(msg, ...) nlm::StructuredLogger::getInstance().error(msg, "Error", __VA_ARGS__)
#define LOG_CRITICAL(msg, ...) nlm::StructuredLogger::getInstance().critical(msg, "Error", __VA_ARGS__)

// Performance monitoring macros
#define GET_PERFORMANCE_CLOCK() std::chrono::high_resolution_clock::now()
#define LOG_PERFORMANCE_SCOPE(name, ...) \
    do { \
        auto start = GET_PERFORMANCE_CLOCK(); \
        auto scope_logger = nlm::PerformanceScope(__func__, __FILE__, __LINE__, name); \
        scope_logger.start(); \
        \
        // User code here (to be implemented with proper RAII) \
        \
        scope_logger.end(__VA_ARGS__); \
    } while(0)

// Performance scope for timing measurements
class PerformanceScope {
public:
    PerformanceScope(const std::string& function, const std::string& file, int line, const std::string& name);
    ~PerformanceScope();
    
    void start();
    void end(const std::unordered_map<std::string, std::string>& metadata = {});
    
private:
    std::string function_;
    std::string file_;
    int line_;
    std::string name_;
    std::chrono::system_clock::time_point startTime_;
    bool started_ = false;
};

// Debugging breakpoint macro
#define DEBUG_BREAKPOINT(condition, msg, ...) \
    do { \
        if (condition) { \
            nlm::StructuredLogger::getInstance().setDebugBreakpoint(condition, msg, ##__VA_ARGS__); \
        } \
    } while(0)

// Memory debugging macros
#define DEBUG_MEMORY_ALLOC(ptr, size, ...) \
    nlm::StructuredLogger::getInstance().logMemoryAllocation(#ptr, size, "Memory", ##__VA_ARGS__)

#define DEBUG_MEMORY_DEALLOC(ptr, ...) \
    nlm::StructuredLogger::getInstance().logMemoryDeallocation(#ptr, "Memory", ##__VA_ARGS__)

} // namespace nlm