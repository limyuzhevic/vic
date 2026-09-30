#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <map>
#include <fmt/core.h>
#include <fmt/format.h>

namespace nlm {

// Enhanced log severity levels
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4
};

// Log entry structure with enhanced metadata
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    
    // Enhanced metadata for structured logging
    std::map<std::string, std::string> metadata;
    std::map<std::string, double> metrics;
    std::string threadId;
    
    LogEntry() : level(LogLevel::Info), line(0), timestamp(std::chrono::system_clock::now()) {}
};

// Log formatter interface
class ILogFormatter {
public:
    virtual ~ILogFormatter() = default;
    virtual std::string format(const LogEntry& entry) = 0;
    virtual std::string getName() const = 0;
};

// Simple text formatter (default)
class TextFormatter : public ILogFormatter {
public:
    std::string format(const LogEntry& entry) override {
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        std::stringstream ss;
        
        ss << "[" << levelToString(entry.level) << "] ";
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        ss << " " << entry.message;
        
        // Add metadata if present
        if (!entry.metadata.empty()) {
            ss << " | Metadata:";
            for (const auto& [key, value] : entry.metadata) {
                ss << " " << key << "=" << value;
            }
        }
        
        // Add metrics if present
        if (!entry.metrics.empty()) {
            ss << " | Metrics:";
            for (const auto& [key, value] : entry.metrics) {
                ss << " " << key << "=" << value;
            }
        }
        
        return ss.str();
    }
    
    std::string getName() const override {
        return "TextFormatter";
    }
private:
    std::string levelToString(LogLevel level) const {
        switch (level) {
            case LogLevel::Debug: return "DEBUG";
            case LogLevel::Info: return "INFO";
            case LogLevel::Warning: return "WARN";
            case LogLevel::Error: return "ERROR";
            case LogLevel::Critical: return "CRIT";
            default: return "UNKNOWN";
        }
    }
};

// JSON formatter for structured logging
class JSONFormatter : public ILogFormatter {
public:
    std::string format(const LogEntry& entry) override {
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        std::stringstream ss;
        
        ss << "{";
        ss << "\"timestamp\":\"" << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S") << "\",";
        ss << "\"level\":\"" << levelToString(entry.level) << "\",";
        ss << "\"message\":\"" << escapeJSONString(entry.message) << "\"";
        
        // Add metadata
        if (!entry.metadata.empty()) {
            ss << ",\"metadata\":{";
            bool first = true;
            for (const auto& [key, value] : entry.metadata) {
                if (!first) ss << ",";
                first = false;
                ss << "\"" << escapeJSONString(key) << "\":\"" << escapeJSONString(value) << "\"";
            }
            ss << "}";
        }
        
        // Add metrics
        if (!entry.metrics.empty()) {
            ss << ",\"metrics\":{";
            bool first = true;
            for (const auto& [key, value] : entry.metrics) {
                if (!first) ss << ",";
                first = false;
                ss << "\"" << escapeJSONString(key) << "\":" << value;
            }
            ss << "}";
        }
        
        // Add source location
        ss << ",\"source\":{";
        ss << "\"file\":\"" << escapeJSONString(entry.file) << "\",";
        ss << "\"line\":" << entry.line << ",";
        ss << "\"function\":\"" << escapeJSONString(entry.function) << "\"";
        ss << "}";
        
        ss << "}";
        return ss.str();
    }
    
    std::string getName() const override {
        return "JSONFormatter";
    }
private:
    std::string levelToString(LogLevel level) const {
        switch (level) {
            case LogLevel::Debug: return "DEBUG";
            case LogLevel::Info: return "INFO";
            case LogLevel::Warning: return "WARN";
            case LogLevel::Error: return "ERROR";
            case LogLevel::Critical: return "CRIT";
            default: return "UNKNOWN";
        }
    }
    
    std::string escapeJSONString(const std::string& str) const {
        std::string escaped;
        for (char c : str) {
            switch (c) {
                case '\\': escaped += "\\\\"; break;
                case '\"': escaped += "\\\""; break;
                case '\n': escaped += "\\n"; break;
                case '\r': escaped += "\\r"; break;
                case '\t': escaped += "\\t"; break;
                default: escaped += c; break;
            }
        }
        return escaped;
    }
};

// Logger interface
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    virtual void setFormatter(std::shared_ptr<ILogFormatter> formatter) {
        // Default implementation - override in derived classes if needed
    }
    virtual std::shared_ptr<ILogFormatter> getFormatter() const {
        return std::make_shared<TextFormatter>();
    }
    
    // Log management methods
    virtual void setMaxFileSize(size_t maxSize) {}
    virtual void setMaxFiles(int maxFiles) {}
    virtual void enableLogRotation(bool enable) {}
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
    
    // Formatting
    void setFormatter(std::shared_ptr<ILogFormatter> formatter) override;
    std::shared_ptr<ILogFormatter> getFormatter() const override;
    
    // Log rotation
    void setMaxFileSize(size_t maxSize) override;
    void setMaxFiles(int maxFiles) override;
    void enableLogRotation(bool enable) override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
    void rotateLogFileIfNeeded();
};

// Rotating file logger implementation
class RotatingFileLogger : public ILogger {
public:
    RotatingFileLogger(const std::string& filepath, size_t maxSize = 10 * 1024 * 1024, int maxFiles = 5);
    ~RotatingFileLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    void setFormatter(std::shared_ptr<ILogFormatter> formatter) override;
    std::shared_ptr<ILogFormatter> getFormatter() const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Main Logger class with enhanced capabilities
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
    
    // Global logging functions with enhanced parameters
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "");
    void logWithMetadata(LogLevel level, const std::string& message,
                         const std::map<std::string, std::string>& metadata,
                         const std::string& file = "", int line = 0,
                         const std::string& function = "");
    void logWithMetrics(LogLevel level, const std::string& message,
                        const std::map<std::string, double>& metrics,
                        const std::string& file = "", int line = 0,
                        const std::string& function = "");
    
    // Convenience methods
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Enhanced logging methods with parameters
    template<typename... Args>
    void debugFmt(const std::string& format, Args&&... args) {
        debug(fmt::format(format, std::forward<Args>(args)...));
    }
    
    template<typename... Args>
    void infoFmt(const std::string& format, Args&&... args) {
        info(fmt::format(format, std::forward<Args>(args)...));
    }
    
    template<typename... Args>
    void warningFmt(const std::string& format, Args&&... args) {
        warning(fmt::format(format, std::forward<Args>(args)...));
    }
    
    template<typename... Args>
    void errorFmt(const std::string& format, Args&&... args) {
        error(fmt::format(format, std::forward<Args>(args)...));
    }
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Global log configuration
    static void configureGlobalLogger(const std::string& configFile = "");
    
    // Log statistics
    struct LogStatistics {
        int totalLogs = 0;
        int debugLogs = 0;
        int infoLogs = 0;
        int warningLogs = 0;
        int errorLogs = 0;
        int criticalLogs = 0;
        double averageLogRate = 0.0;
    };
    
    LogStatistics getStatistics() const;
    void resetStatistics();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Convenience macros for different log levels
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

// Enhanced stream-style logging helper with metadata
class LogStream {
public:
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function);
    ~LogStream();
    
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    // Add metadata
    LogStream& withMetadata(const std::string& key, const std::string& value);
    LogStream& withMetric(const std::string& key, double value);
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
    std::map<std::string, std::string> metadata_;
    std::map<std::string, double> metrics_;
};

#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

} // namespace nlm
