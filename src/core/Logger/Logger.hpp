#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <deque>
#include <thread>
#include <atomic>
#include <mutex>
#include <shared_mutex>
#include <iomanip>
#include <fstream>
#include <ctime>
#include <unordered_set>
#include <map>
#include <variant>

namespace nlm {

// Log severity levels
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4
};

// Log category for organization
class LogCategory {
public:
    static constexpr const char* Default = "default";
    static constexpr const char* Core = "core";
    static constexpr const char* Memory = "memory";
    static constexpr const char* Brain = "brain";
    static constexpr const char* Motor = "motor";
    static constexpr const char* Sensory = "sensory";
    static constexpr const char* Cognition = "cognition";
    static constexpr const char* Development = "development";
    static constexpr const char* Plasticity = "plasticity";
    static constexpr const char* Neuromodulation = "neuromodulation";
    static constexpr const char* Prediction = "prediction";
    static constexpr const char* ErrorHandling = "error_handling";
    static constexpr const char* Config = "config";
    static constexpr const char* Plugin = "plugin";
    static constexpr const char* Performance = "performance";
    static constexpr const char* MemoryAllocation = "memory_allocation";
    static constexpr const char* Threading = "threading";
    static constexpr const char* Backtrace = "backtrace";
};

// Log entry structure (enhanced)
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::string category;
    std::string threadId;
    std::unordered_map<std::string, std::string> metadata;
    
    LogEntry() : level(LogLevel::Info), line(0), category(LogCategory::Default) {}
    LogEntry(LogLevel lvl, const std::string& msg) 
        : level(lvl), message(msg), line(0), category(LogCategory::Default) {
        timestamp = std::chrono::system_clock::now();
    }
    
    std::string getFormattedTimestamp() const {
        auto time = std::chrono::system_clock::to_time_t(timestamp);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
};

// Log formatter interface
class LogFormatter {
public:
    virtual ~LogFormatter() = default;
    virtual std::string format(const LogEntry& entry) = 0;
    virtual std::string format(const std::string& message) = 0;
};

// Basic log formatter implementation
class SimpleFormatter : public LogFormatter {
public:
    std::string format(const LogEntry& entry) override;
    std::string format(const std::string& message) override;
};

// JSON formatter implementation
class JSONFormatter : public LogFormatter {
public:
    std::string format(const LogEntry& entry) override;
    std::string format(const std::string& message) override;
};

// XML formatter implementation
class XMLFormatter : public LogFormatter {
public:
    std::string format(const LogEntry& entry) override;
    std::string format(const std::string& message) override;
};

// Log file rotation strategy
enum class RotationStrategy {
    None,
    SizeBased,
    TimeBasedHourly,
    TimeBasedDaily,
    TimeBasedWeekly
};

// Log file rotation configuration
struct RotationConfig {
    RotationStrategy strategy = RotationStrategy::None;
    uint64_t maxFileSize = 10 * 1024 * 1024; // 10MB
    int maxFiles = 5;
    std::string pattern = "log_%Y%m%d_%H%M%S"; // for time-based
};

// ILogger interface (enhanced)
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    virtual void setCategoryFilter(const std::string& category) = 0;
    virtual void addCategory(const std::string& category) = 0;
    virtual void removeCategory(const std::string& category) = 0;
    virtual bool isCategoryEnabled(const std::string& category) const = 0;
    virtual void setFormatter(std::shared_ptr<LogFormatter> formatter) = 0;
    virtual std::shared_ptr<LogFormatter> getFormatter() const = 0;
};

// Forward declarations for logger implementations
class AsyncLogger;
class FileLogger;
class MemoryLogger;
class StructLogger;
class NetworkLogger;
class LoggerFactory;
class PerformanceMetrics;
class GlobalLogManager;

// Enhanced Console logger
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(LogLevel level);
    ~ConsoleLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    void setCategoryFilter(const std::string& category) override;
    void addCategory(const std::string& category) override;
    void removeCategory(const std::string& category) override;
    bool isCategoryEnabled(const std::string& category) const override;
    void setFormatter(std::shared_ptr<LogFormatter> formatter) override;
    std::shared_ptr<LogFormatter> getFormatter() const override;
    
    // Original API preserved for backward compatibility
    void setUseColors(bool useColors);
    bool getUseColors() const;
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    
    // Enhanced features
    void setRotation(const RotationConfig& config);
    void enableAsynchronousOutput(bool enabled);
    
private:
    struct Impl {
        LogLevel level = LogLevel::Info;
        bool useColors = true;
        std::ofstream fileOut;
        std::mutex mutex;
    };
    std::unique_ptr<Impl> pImpl;
    
    // Helper methods
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
};

// Async logger for non-blocking logging
class AsyncLogger : public ILogger {
public:
    AsyncLogger(size_t threadCount = 2, size_t queueSize = 10000);
    ~AsyncLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    void setCategoryFilter(const std::string& category) override;
    void addCategory(const std::string& category) override;
    void removeCategory(const std::string& category) override;
    bool isCategoryEnabled(const std::string& category) const override;
    void setFormatter(std::shared_ptr<LogFormatter> formatter) override;
    std::shared_ptr<LogFormatter> getFormatter() const override;
    
    void stop(); // Gracefully shutdown the async logger
    
private:
    void workerThread(); // Worker thread method
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced file logger with rotation support
class FileLogger : public ILogger {
public:
    FileLogger();
    explicit FileLogger(LogLevel level);
    ~FileLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    void setCategoryFilter(const std::string& category) override;
    void addCategory(const std::string& category) override;
    void removeCategory(const std::string& category) override;
    bool isCategoryEnabled(const std::string& category) const override;
    void setFormatter(std::shared_ptr<LogFormatter> formatter) override;
    std::shared_ptr<LogFormatter> getFormatter() const override;
    
    // File-specific operations
    bool open(const std::string& filepath, const RotationConfig& rotation = {});
    void close();
    std::string getCurrentFile() const;
    
    // Enhanced features
    void enableBuffering(bool enabled, size_t bufferSize = 8192);
    void setFlushOnEachLog(bool flush);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Memory tracking and debugging logger
class MemoryLogger : public ILogger {
public:
    MemoryLogger();
    ~MemoryLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    void setCategoryFilter(const std::string& category) override;
    void addCategory(const std::string& category) override;
    void removeCategory(const std::string& category) override;
    bool isCategoryEnabled(const std::string& category) const override;
    void setFormatter(std::shared_ptr<LogFormatter> formatter) override;
    std::shared_ptr<LogFormatter> getFormatter() const override;
    
    // Memory tracking features
    void enableMemoryTracking(bool enabled);
    void trackAllocation(void* ptr, size_t size, const std::string& context = "");
    void trackDeallocation(void* ptr);
    void logMemoryStats();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// OutputFormat enum moved to global scope for LoggerFactory usage
enum class OutputFormat { JSON, XML, YAML };

// Structured logging (JSON/XML) logger
class StructLogger : public ILogger {
public:
    StructLogger();
    explicit StructLogger(OutputFormat format);
    ~StructLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    void setCategoryFilter(const std::string& category) override;
    void addCategory(const std::string& category) override;
    void removeCategory(const std::string& category) override;
    bool isCategoryEnabled(const std::string& category) const override;
    void setFormatter(std::shared_ptr<LogFormatter> formatter) override;
    std::shared_ptr<LogFormatter> getFormatter() const override;
    
    // Structured logging features
    void setStructuredFormat(OutputFormat format);
    void enableCompression(bool enabled);
    void setIndentation(int spaces);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper methods
    std::string formatAsJSON(const LogEntry& entry);
    std::string formatAsXML(const LogEntry& entry);
    std::string formatAsYAML(const LogEntry& entry);
};

// Logger factory for creating different logger types
class LoggerFactory {
public:
    static std::shared_ptr<ILogger> createConsoleLogger(LogLevel level = LogLevel::Info);
    static std::shared_ptr<ILogger> createAsyncLogger(size_t threadCount = 2, size_t queueSize = 10000);
    static std::shared_ptr<ILogger> createFileLogger(const std::string& filepath, const RotationConfig& rotation = {});
    static std::shared_ptr<ILogger> createMemoryLogger();
    static std::shared_ptr<ILogger> createStructLogger(OutputFormat format = OutputFormat::JSON);
    static std::shared_ptr<ILogger> createNetworkLogger(const std::string& host, uint16_t port);
    static std::shared_ptr<ILogger> createCustomLogger(const std::string& type, const std::unordered_map<std::string, std::string>& config);
};

// Performance metrics collector
class PerformanceMetrics {
public:
    struct Metrics {
        uint64_t logCount = 0;
        uint64_t droppedLogs = 0;
        double averageLogTime = 0.0;
        std::unordered_map<std::string, uint64_t> logByCategory;
        std::unordered_map<LogLevel, uint64_t> logByLevel;
    };
    
    static void recordLog(const LogEntry& entry, bool dropped = false, double logTime = 0.0);
    static Metrics getMetrics();
    static void resetMetrics();
    static std::string getMetricsReport();
private:
    static Metrics metrics_;
    static std::mutex metricsMutex_;
};

// Global log level management
class GlobalLogManager {
public:
    static void setDefaultLevel(LogLevel level);
    static LogLevel getDefaultLevel();
    static void setCategoryEnabled(const std::string& category, bool enabled);
    static bool isCategoryEnabled(const std::string& category);
    static void enableSynchronizedLogging(bool enabled);
    static void setLogFile(const std::string& filepath);
    static void initializeFromConfig();
private:
    static std::mutex mutex_;
    static LogLevel defaultLevel_;
    static std::unordered_map<std::string, bool> categoryEnabled_;
    static bool synchronizedLogging_;
    static std::string logFile_;
};

// Main Logger class (enhanced for backward compatibility)
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
             const std::string& function = "", const std::string& category = "");
    
    // Convenience methods
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Category-based logging
    void logCategory(LogLevel level, const std::string& category, const std::string& message,
                    const std::string& file = "", int line = 0, const std::string& function = "");
    void debugCategory(const std::string& category, const std::string& message,
                      const std::string& file = "", int line = 0, const std::string& function = "");
    void infoCategory(const std::string& category, const std::string& message,
                     const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    bool isCategoryEnabled(const std::string& category) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Performance and memory debugging
    static void logMemoryStats();
    static void logPerformanceMetrics();
    static void generateBacktrace(const std::string& context = "");
    
    // Configuration integration
    static void loadConfigFromFile(const std::string& configFile);
    static void initializeFromConfig(const std::string& configKey = "logging");
    
    // Sanitization
    static std::string sanitizeLogMessage(const std::string& message);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Global convenience functions
void initializeLogging(const std::string& configFile = "");
void shutdownLogging();
void setGlobalLogLevel(LogLevel level);
LogLevel getGlobalLogLevel();
void logEvent(const std::string& category, LogLevel level, const std::string& message);

// Convenience macro for file/line/function info (backward compatibility)
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

// Enhanced macro with category support
#define NLM_LOG_CAT(level, category, msg) nlm::Logger::getGlobal().logCategory(level, category, msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CAT_DEBUG(category, msg) nlm::Logger::getGlobal().debugCategory(category, msg, __FILE__, __LINE__, __func__)

// Stream-style logging helper (enhanced)
class LogStream {
public:
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function,
              const std::string& category = "");
    ~LogStream();
    
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
    std::string category_;
};

#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)
#define NLM_LOG_STREAM_CAT(level, category) nlm::LogStream(level, __FILE__, __LINE__, __func__, category)

// Backtrace and error integration
namespace Backtrace {
    void generate(const std::string& context = "");
    std::string getCurrentBacktrace();
    void enableAutomaticGeneration(bool enabled);
    bool isEnabled();
}

} // namespace nlm