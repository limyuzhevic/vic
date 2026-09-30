#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <atomic>
#include <thread>
#include <mutex>

namespace nlm {

// Enhanced log severity levels for research
enum class LogLevel {
    Trace = -1,     // Very detailed, for debugging
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4,
    Fatal = 5
};

// Log entry with structured data support
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    
    // Structured data for research use
    std::unordered_map<std::string, std::string> fields;
    std::unordered_map<std::string, double> numericFields;
    std::unordered_map<std::string, bool> boolFields;
    
    // Performance metrics
    double executionTime = 0.0;
    size_t memoryUsage = 0;
    
    // Source tracking
    std::string sourceModule;
    std::string experimentId;
    
    // Filtering support
    std::vector<std::string> tags;
};

// Logger statistics
struct LogStatistics {
    std::atomic<uint64_t> totalLogs{0};
    std::atomic<uint64_t> debugLogs{0};
    std::atomic<uint64_t> infoLogs{0};
    std::atomic<uint64_t> warningLogs{0};
    std::atomic<uint64_t> errorLogs{0};
    std::atomic<uint64_t> criticalLogs{0};
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    
    void reset() {
        totalLogs = 0;
        debugLogs = 0;
        infoLogs = 0;
        warningLogs = 0;
        errorLogs = 0;
        criticalLogs = 0;
        startTime = std::chrono::system_clock::now();
    }
    
    std::string toJson() const {
        std::ostringstream oss;
        oss << "{\n";
        oss << "  \"total_logs\": " << totalLogs << ",\n";
        oss << "  \"debug_logs\": " << debugLogs << ",\n";
        oss << "  \"info_logs\": " << infoLogs << ",\n";
        oss << "  \"warning_logs\": " << warningLogs << ",\n";
        oss << "  \"error_logs\": " << errorLogs << ",\n";
        oss << "  \"critical_logs\": " << criticalLogs << ",\n";
        if (startTime.time_since_epoch().count() > 0) {
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now() - startTime).count();
            oss << "  \"duration_seconds\": " << duration << "\n";
        }
        oss << "}\n";
        return oss.str();
    }
};

// Enhanced logger interface
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    
    // Filter support
    virtual void addFilter(const std::function<bool(const LogEntry&)>& filter) = 0;
    virtual void clearFilters() = 0;
    
    // Statistics support
    virtual LogStatistics getStatistics() const = 0;
    virtual void resetStatistics() = 0;
    
    // Source filtering
    virtual void setSourceFilter(const std::string& pattern) = 0;
    virtual void setModuleFilter(const std::string& module) = 0;
    
    // Structured logging
    virtual void logStructured(LogLevel level, const std::string& message,
                              const std::unordered_map<std::string, std::string>& fields) = 0;
    
    // Performance logging
    virtual void logWithMetrics(LogLevel level, const std::string& message,
                               double executionTime, size_t memoryUsage = 0) = 0;
};

// Enhanced console logger implementation
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
    
    // File rotation
    void setFileRotation(size_t maxFileSize, size_t maxFiles = 5);
    void enableFileRotation(bool enable);
    
    // Filter support
    void addFilter(const std::function<bool(const LogEntry&)>& filter) override;
    void clearFilters() override;
    
    // Statistics support
    LogStatistics getStatistics() const override;
    void resetStatistics() override;
    
    // Source filtering
    void setSourceFilter(const std::string& pattern) override;
    void setModuleFilter(const std::string& module) override;
    
    // Structured logging
    void logStructured(LogLevel level, const std::string& message,
                      const std::unordered_map<std::string, std::string>& fields) override;
    
    // Performance logging
    void logWithMetrics(LogLevel level, const std::string& message,
                       double executionTime, size_t memoryUsage = 0) override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
    bool shouldLog(const LogEntry& entry) const;
    std::string formatStructured(const LogEntry& entry) const;
};

// Main Logger class with advanced features
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
             const std::string& function = "", const std::string& sourceModule = "");
    
    // Convenience methods
    void trace(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& sourceModule = "");
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& sourceModule = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& sourceModule = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& sourceModule = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& sourceModule = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& sourceModule = "");
    void fatal(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& sourceModule = "");
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Global statistics
    static LogStatistics getGlobalStatistics();
    static void resetGlobalStatistics();
    
    // Performance monitoring
    static double measureExecutionTime(std::function<void()> func);
    
    // Structured logging
    static void logStructured(LogLevel level, const std::string& message,
                              const std::unordered_map<std::string, std::string>& fields,
                              const std::string& sourceModule = "");
    
    // Research mode
    static void enableResearchMode(bool enable);
    static bool isResearchModeEnabled();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced convenience macros for research use
#define NLM_LOG_TRACE(msg) nlm::Logger::getGlobal().trace(msg, __FILE__, __LINE__, __func__, "RESEARCH")
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__, "RESEARCH")
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__, "RESEARCH")
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__, "RESEARCH")
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__, "RESEARCH")
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__, "RESEARCH")
#define NLM_LOG_FATAL(msg) nlm::Logger::getGlobal().fatal(msg, __FILE__, __LINE__, __func__, "RESEARCH")

// Performance measurement macro
#define NLM_MEASURE_TIME(expr) nlm::Logger::measureExecutionTime([&]() { expr; })

// Structured logging helper
#define NLM_LOG_STRUCTURED(level, msg, ...) \
    do { \
        std::unordered_map<std::string, std::string> fields = {__VA_ARGS__}; \
        nlm::Logger::logStructured(level, msg, fields); \
    } while(0)

// Stream-style logging helper with metrics
class LogStream {
public:
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function, const std::string& sourceModule = "");
    ~LogStream();
    
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    // Add structured field
    LogStream& field(const std::string& key, const std::string& value);
    LogStream& field(const std::string& key, double value);
    LogStream& field(const std::string& key, int value);
    LogStream& field(const std::string& key, bool value);
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
    std::string sourceModule_;
    std::unordered_map<std::string, std::string> fields_;
    std::chrono::high_resolution_clock::time_point startTime_;
};

#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

} // namespace nlm
