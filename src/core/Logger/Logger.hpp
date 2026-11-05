#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>

namespace nlm {

// Log severity levels
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4
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
};

// Console logger implementation
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
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
};

// Main Logger class
class Logger {
public:
    Logger();
    ~Logger();
    
    // Disable copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // Enhanced configuration and management
    // Add logger with automatic cleanup
    template<typename LoggerType>
    void addAutoCleanupLogger(std::shared_ptr<LoggerType> logger, std::chrono::milliseconds timeout = std::chrono::milliseconds(30000)) {
        addLogger(logger);
        // Note: Actual cleanup logic would be implemented based on specific needs
    }
    
    // Get number of active loggers
    size_t getLoggerCount() const { return pImpl ? pImpl->loggers.size() : 0; }
    
    // Check if specific logger type is active
    template<typename LoggerType>
    bool hasLoggerType() const {
        if (!pImpl) return false;
        for (const auto& logger : pImpl->loggers) {
            if (dynamic_cast<const LoggerType*>(logger.get()) != nullptr) {
                return true;
            }
        }
        return false;
    }
    
    // Get active log level summary
    std::map<LogLevel, size_t> getLogLevelSummary() const {
        std::map<LogLevel, size_t> summary;
        if (!pImpl) return summary;
        
        for (LogLevel level : {LogLevel::Debug, LogLevel::Info, LogLevel::Warning, 
                               LogLevel::Error, LogLevel::Critical}) {
            size_t count = 0;
            for (const auto& logger : pImpl->loggers) {
                if (logger->getLevel() <= level) {
                    count++;
                }
            }
            summary[level] = count;
        }
        return summary;
    }
    
    // Performance monitoring
    struct PerformanceStats {
        size_t logCount = 0;
        double avgLogTime = 0.0;
        std::chrono::system_clock::time_point lastLogTime;
    };
    
    PerformanceStats getPerformanceStats() const {
        PerformanceStats stats;
        if (!pImpl) return stats;
        stats.logCount = pImpl->loggers.size();
        // Note: Actual timing stats would require more sophisticated tracking
        stats.lastLogTime = std::chrono::system_clock::now();
        return stats;
    }
    
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

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Logging macros that include file and function info
    #define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
    #define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
    #define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
    #define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
    #define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)
    
    // Stream-style logging helper
    class LogStream {
    public:
        LogStream(LogLevel level, const std::string& file, int line, const std::string& function);
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
    };
    
    #define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)
    
    // Global logging convenience functions
    inline void logDebug(const std::string& msg) { Logger::getGlobal().debug(msg); }
    inline void logInfo(const std::string& msg) { Logger::getGlobal().info(msg); }
    inline void logWarning(const std::string& msg) { Logger::getGlobal().warning(msg); }
    inline void logError(const std::string& msg) { Logger::getGlobal().error(msg); }
    inline void logCritical(const std::string& msg) { Logger::getGlobal().critical(msg); }
    
    // Multi-level logging
    inline void logMulti(const std::vector<std::pair<LogLevel, std::string>>& entries) {
        for (const auto& entry : entries) {
            Logger::getGlobal().log(entry.first, entry.second);
        }
    }

} // namespace nlm
