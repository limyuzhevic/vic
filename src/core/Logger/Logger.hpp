#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <map>
#include <optional>

namespace nlm {

// Log severity levels
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4
};

// Log format configuration
struct LogFormatConfig {
    bool includeTimestamp = true;
    bool includeLevel = true;
    bool includeFile = false;
    bool includeLineNumber = false;
    bool includeFunction = false;
    std::string timestampFormat = "%Y-%m-%d %H:%M:%S";
    std::string levelFormat = "[%s]";
    std::string filePrefix = "file:";
    std::string linePrefix = "line:";
    std::string functionPrefix = "func:";
    bool useColors = true;
    bool useAnsiColors = true;
};

// Log file rotation configuration
struct LogRotationConfig {
    bool enabled = false;
    size_t maxFileSize = 10 * 1024 * 1024;  // 10MB
    size_t maxFiles = 5;
    std::string rotationFormat = ".%Y%m%d%H%M%S";
    bool compressOldFiles = false;
};

// Logging configuration
struct LoggingConfig {
    LogLevel level = LogLevel::Info;
    LogFormatConfig format;
    LogRotationConfig rotation;
    std::string outputFile;
    bool consoleOutput = true;
    bool fileOutput = false;
    bool async = true;  // Asynchronous logging
    size_t asyncBufferSize = 1024;
    std::string asyncThreadName = "NLM-Logger";
    
    // Configuration file integration
    bool loadFromConfig(const std::string& configFile);
    void applyToLogger(std::shared_ptr<Logger> logger);
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
    virtual void setFormatConfig(const LogFormatConfig& config) = 0;
    virtual LogFormatConfig getFormatConfig() const = 0;
    virtual void setRotationConfig(const LogRotationConfig& config) = 0;
    virtual LogRotationConfig getRotationConfig() const = 0;
    virtual void setOutputFile(const std::string& filepath) = 0;
    virtual void closeOutputFile() = 0;
    virtual bool isEnabled(LogLevel level) const = 0;
    virtual std::string formatEntry(const LogEntry& entry) const = 0;
};

// Enhanced Console logger implementation
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(LogLevel level);
    explicit ConsoleLogger(const LoggingConfig& config);
    ~ConsoleLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Format configuration
    void setFormatConfig(const LogFormatConfig& config) override;
    LogFormatConfig getFormatConfig() const override;
    
    // Rotation configuration
    void setRotationConfig(const LogRotationConfig& config) override;
    LogRotationConfig getRotationConfig() const override;
    
    // Output configuration
    void setOutputFile(const std::string& filepath) override;
    void closeOutputFile() override;
    
    // Color configuration
    void setUseColors(bool useColors);
    bool getUseColors() const;
    
    // File output
    bool hasFileOutput() const;
    void rotateLogFiles();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
    std::string formatTimestamp(const std::chrono::system_clock::time_point& timestamp) const;
    std::string formatEntry(const LogEntry& entry) const;
};

// Main Logger class
class Logger {
public:
    Logger();
    explicit Logger(const LoggingConfig& config);
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
    
    // Apply configuration
    void applyConfig(const LoggingConfig& config);
    LoggingConfig getConfig() const;
    
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
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Configuration file loading
    static bool loadGlobalConfigFromFile(const std::string& configFile);
    static bool saveGlobalConfigToFile(const std::string& configFile) const;
    
    // Configuration validation
    static bool validateConfig(const LoggingConfig& config, std::vector<std::string>& errors);
    
    // Configuration schema
    static std::map<std::string, std::string> getConfigurationSchema();
    
    // Performance statistics
    struct PerformanceStats {
        size_t messagesLogged = 0;
        size_t messagesDropped = 0;
        double avgLogTimeUs = 0.0;
        size_t maxQueueSize = 0;
        std::chrono::system_clock::time_point lastReset;
    };
    
    PerformanceStats getPerformanceStats() const;
    void resetPerformanceStats();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced convenience macros
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

// Enhanced stream-style logging helper
class LogStream {
public:
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function);
    ~LogStream();
    
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    // Chain with log level
    LogStream& operator<<(LogLevel level) {
        level_ = level;
        return *this;
    }
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
};

#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

// Configuration file helpers
namespace LogConfig {
    // Default configuration
    LoggingConfig getDefaultConfig();
    
    // Parse configuration from string
    bool parseConfigString(const std::string& configStr, LoggingConfig& config);
    
    // Generate configuration string
    std::string generateConfigString(const LoggingConfig& config);
    
    // Configuration validation
    std::vector<std::string> validateConfig(const LoggingConfig& config);
    
    // Color configuration
    std::string getLevelColor(LogLevel level);
    std::string getLevelName(LogLevel level);
}

} // namespace nlm
