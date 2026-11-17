#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <stacktrace>
#include <format>

namespace nlm {

// Enhanced Log severity levels
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4,
    Fatal = 5
};

// Enhanced Log entry structure with additional metadata
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::string threadId;
    uint64_t correlationId;
    
    // Error details
    std::string errorCode;
    std::string errorCategory;
    std::string stackTrace;
    
    // Performance tracking
    std::chrono::microseconds duration;
    
    // Context information
    std::unordered_map<std::string, std::string> context;
    std::unordered_map<std::string, std::string> userContext;
};

// Log format options
enum class LogFormat {
    Plain,
    JSON,
    CSV,
    Structured,
    Colored
};

// Log filtering options
struct LogFilter {
    LogLevel minLevel = LogLevel::Debug;
    std::vector<std::string> includeModules;
    std::vector<std::string> excludeModules;
    std::vector<std::string> includeKeywords;
    std::vector<std::string> excludeKeywords;
    std::unordered_map<std::string, LogLevel> moduleOverrides;
    bool matchAllKeywords = false;
};

// Log statistics
struct LogStatistics {
    uint64_t totalLogs = 0;
    uint64_t logsByLevel[6] = {0};
    uint64_t logsPerMinute = 0;
    double avgMessageLength = 0.0;
    std::chrono::system_clock::time_point firstLog;
    std::chrono::system_clock::time_point lastLog;
    std::string mostFrequentModule;
    std::string mostFrequentKeyword;
};

// Enhanced ILogger interface
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    
    // Enhanced configuration
    virtual void setFormat(LogFormat format) = 0;
    virtual void setFilter(const LogFilter& filter) = 0;
    virtual void setPattern(const std::string& pattern) = 0;
    
    // Statistics
    virtual LogStatistics getStatistics() const = 0;
    virtual void resetStatistics() = 0;
    
    // Async logging support
    virtual void logAsync(const LogEntry& entry) = 0;
    virtual bool isAsync() const = 0;
    
    // Output management
    virtual void setOutputFile(const std::string& filepath) = 0;
    virtual void setRotationSize(uint64_t bytes) = 0;
    virtual void rotateLog() = 0;
};

// Enhanced Console logger with advanced features
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(LogLevel level);
    ~ConsoleLogger() override;
    
    void log(const LogEntry& entry) override;
    void logAsync(const LogEntry& entry) override;
    
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Enhanced configuration
    void setFormat(LogFormat format) override;
    void setFilter(const LogFilter& filter) override;
    void setPattern(const std::string& pattern) override;
    
    // Color and formatting
    void setUseColors(bool useColors);
    bool getUseColors() const;
    void setColorScheme(const std::string& scheme);
    
    // Output management
    void setOutputFile(const std::string& filepath) override;
    void setRotationSize(uint64_t bytes) override;
    void rotateLog() override;
    void closeOutputFile();
    
    // Statistics
    LogStatistics getStatistics() const override;
    void resetStatistics() override;
    
    bool isAsync() const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string formatMessage(const LogEntry& entry) const;
    bool shouldLog(const LogEntry& entry) const;
    void writeToFile(const std::string& message);
};

// File logger implementation
class FileLogger : public ILogger {
public:
    FileLogger(const std::string& filepath);
    ~FileLogger() override;
    
    void log(const LogEntry& entry) override;
    void logAsync(const LogEntry& entry) override;
    
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    void setFormat(LogFormat format) override;
    void setFilter(const LogFilter& filter) override;
    void setPattern(const std::string& pattern) override;
    
    void setOutputFile(const std::string& filepath) override;
    void setRotationSize(uint64_t bytes) override;
    void rotateLog() override;
    void closeOutputFile();
    
    LogStatistics getStatistics() const override;
    void resetStatistics() override;
    
    bool isAsync() const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced main Logger class with error handling and recovery
class Logger {
public:
    Logger();
    ~Logger();
    
    // Disable copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // Logger management
    void addLogger(std::shared_ptr<ILogger> logger);
    void removeLogger(ILogger* logger);
    void clearLoggers();
    
    // Enhanced configuration
    void setLevel(LogLevel level);
    void setGlobalFilter(const LogFilter& filter);
    void setGlobalPattern(const std::string& pattern);
    
    // Enhanced logging with error handling
    void log(LogLevel level, const std::string& message,
             const std::string& file = "", int line = 0,
             const std::string& function = "", 
             const std::string& errorCode = "",
             const std::string& errorCategory = "");
    
    // Enhanced error handling methods
    void logError(const std::string& message, const std::string& file = "", 
                  int line = 0, const std::string& function = "",
                  const std::string& errorCode = "",
                  const std::string& errorCategory = "",
                  std::exception_ptr exception = nullptr);
    
    void logWithContext(LogLevel level, const std::string& message,
                        const std::unordered_map<std::string, std::string>& context,
                        const std::string& file = "", int line = 0,
                        const std::string& function = "");
    
    // Performance tracking
    void startTimedLog(const std::string& operation);
    void endTimedLog(const std::string& operation, LogLevel level = LogLevel::Info);
    
    // Convenience methods with enhanced signatures
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void fatal(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Check if level is enabled with enhanced filtering
    bool isEnabled(LogLevel level, const std::string& module = "") const;
    
    // Statistics and monitoring
    LogStatistics getGlobalStatistics() const;
    void resetGlobalStatistics();
    std::string getLogReport() const;
    
    // Global logger management
    static Logger& getGlobal();
    static void setGlobal(std::shared_ptr<Logger> logger);
    static void configureGlobalLogger(const std::string& configFile = "");
    
    // Error recovery and health monitoring
    bool isHealthy() const;
    std::vector<std::string> getHealthIssues() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced macro for logging with file/line/function info
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_FATAL(msg) nlm::Logger::getGlobal().fatal(msg, __FILE__, __LINE__, __func__)

// Enhanced stream-style logging with error context
class LogStream {
public:
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function,
              const std::string& operation = "");
    ~LogStream();
    
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    // Context management
    LogStream& withContext(const std::string& key, const std::string& value);
    LogStream& withCorrelationId(uint64_t id);
    
    // Error handling
    LogStream& withException(std::exception_ptr ex);
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
    std::string operation_;
    std::unordered_map<std::string, std::string> context_;
    uint64_t correlationId_;
};

// Macro for timed logging
#define NLM_LOG_TIMED(level, operation) nlm::LogStream(level, __FILE__, __LINE__, __func__, operation)

// Macro for error context logging
#define NLM_LOG_ERROR_CONTEXT(msg, errorCode) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__, errorCode, "user")

// Macro for structured logging
#define NLM_LOG_STRUCTURED(level, module, message) nlm::Logger::getGlobal().log(level, message, __FILE__, __LINE__, __func__)

} // namespace nlm