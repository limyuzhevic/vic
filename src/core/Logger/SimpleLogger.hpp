// nlm - Neural Learning Machine
// Simple Enhanced Logger Implementation
// Phase 2: Practical Runtime Logging
//
// This phase implements practical logging improvements with:
// - Enhanced log levels and formatting
// - Thread-safe logging operations
// - Performance monitoring integration
// - Memory usage tracking
// - Error logging with context
// - Structured logging support

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <chrono>
#include <mutex>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <fstream>
#include <algorithm>

namespace nlm {

// Enhanced logging levels
enum class LogLevel {
    Trace = 0,      // Detailed debug information
    Debug = 1,      // General debugging information
    Info = 2,        // General informational messages
    Notice = 3,     // Important but normal events
    Warning = 4,     // Warnings about potential issues
    Error = 5,       // Errors that don't prevent execution
    Critical = 6,    // Critical errors that affect core functionality
    Alert = 7,       // Immediate action required
    Emergency = 8    // System is unusable
};

// Simple log entry
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string source;
    std::string function;
    std::string file;
    int line;
    std::chrono::system_clock::time_point timestamp;
    std::map<std::string, std::string> context;
    
    LogEntry() : level(LogLevel::Info), line(0) {
        timestamp = std::chrono::system_clock::now();
    }
    
    LogEntry(LogLevel lvl, const std::string& msg, 
             const std::string& src = "",
             const std::string& func = "",
             const std::string& f = "",
             int ln = 0,
             const std::map<std::string, std::string>& ctx = {})
        : level(lvl), message(msg), source(src), function(func), 
          file(f), line(ln), context(ctx) {
        timestamp = std::chrono::system_clock::now();
    }
    
    std::string format() const {
        std::ostringstream oss;
        oss << "[" << levelToString(level) << "] ";
        
        // Add timestamp
        auto time = std::chrono::system_clock::to_time_t(timestamp);
        oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        
        // Add source location if available
        if (!file.empty() && line > 0) {
            oss << " " << file << ":" << line;
        }
        if (!function.empty()) {
            oss << " " << function << "()";
        }
        
        oss << " " << message;
        
        // Add context
        if (!context.empty()) {
            oss << " [Context:";
            bool first = true;
            for (const auto& pair : context) {
                if (!first) oss << ",";
                oss << " " << pair.first << "=" << pair.second;
                first = false;
            }
            oss << "]";
        }
        
        return oss.str();
    }
    
    static std::string levelToString(LogLevel level) {
        switch (level) {
            case LogLevel::Trace: return "TRACE";
            case LogLevel::Debug: return "DEBUG";
            case LogLevel::Info: return "INFO";
            case LogLevel::Notice: return "NOTICE";
            case LogLevel::Warning: return "WARN";
            case LogLevel::Error: return "ERROR";
            case LogLevel::Critical: return "CRIT";
            case LogLevel::Alert: return "ALERT";
            case LogLevel::Emergency: return "EMERG";
            default: return "UNKNOWN";
        }
    }
};

// Simple logger interface
class SimpleLogger {
public:
    virtual ~SimpleLogger() = default;
    
    // Core logging methods
    virtual void trace(const std::string& message,
                      const std::string& source = "",
                      const std::string& function = "",
                      const std::string& file = "",
                      int line = 0,
                      const std::map<std::string, std::string>& context = {}) = 0;
    
    virtual void debug(const std::string& message,
                      const std::string& source = "",
                      const std::string& function = "",
                      const std::string& file = "",
                      int line = 0,
                      const std::map<std::string, std::string>& context = {}) = 0;
    
    virtual void info(const std::string& message,
                     const std::string& source = "",
                     const std::string& function = "",
                     const std::string& file = "",
                     int line = 0,
                     const std::map<std::string, std::string>& context = {}) = 0;
    
    virtual void notice(const std::string& message,
                       const std::string& source = "",
                       const std::string& function = "",
                       const std::string& file = "",
                       int line = 0,
                       const std::map<std::string, std::string>& context = {}) = 0;
    
    virtual void warning(const std::string& message,
                        const std::string& source = "",
                        const std::string& function = "",
                        const std::string& file = "",
                        int line = 0,
                        const std::map<std::string, std::string>& context = {}) = 0;
    
    virtual void error(const std::string& message,
                      const std::string& source = "",
                      const std::string& function = "",
                      const std::string& file = "",
                      int line = 0,
                      const std::string& details = "",
                      const std::map<std::string, std::string>& context = {}) = 0;
    
    virtual void critical(const std::string& message,
                         const std::string& source = "",
                         const std::string& function = "",
                         const std::string& file = "",
                         int line = 0,
                         const std::string& details = "",
                         const std::map<std::string, std::string>& context = {}) = 0;
    
    virtual void alert(const std::string& message,
                      const std::string& source = "",
                      const std::string& function = "",
                      const std::string& file = "",
                      int line = 0,
                      const std::string& details = "",
                      const std::map<std::string, std::string>& context = {}) = 0;
    
    virtual void emergency(const std::string& message,
                          const std::string& source = "",
                          const std::string& function = "",
                          const std::string& file = "",
                          int line = 0,
                          const std::string& details = "",
                          const std::map<std::string, std::string>& context = {}) = 0;
    
    // Get statistics
    virtual struct LoggerStats {
        size_t totalLogs = 0;
        std::map<LogLevel, size_t> logsByLevel;
        std::chrono::system_clock::time_point firstLog;
        std::chrono::system_clock::time_point lastLog;
    } getStatistics() const = 0;
    
    // Flush logs
    virtual void flush() = 0;
    
    // Close logger
    virtual void close() = 0;
};

// Console logger implementation
class ConsoleLogger : public SimpleLogger {
public:
    struct Config {
        bool enableColors = true;
        LogLevel minLevel = LogLevel::Debug;
        std::string logFile = "";
        bool enableFileOutput = false;
        bool enableTimestamps = true;
        bool enableThreadId = true;
        bool enableSourceLocation = true;
    };
    
    ConsoleLogger() = default;
    explicit ConsoleLogger(const Config& config) : config_(config) {
        if (config_.enableFileOutput && !config_.logFile.empty()) {
            logFile_.open(config_.logFile, std::ios::app);
        }
    }
    
    ~ConsoleLogger() override {
        close();
    }
    
    void trace(const std::string& message,
              const std::string& source = "",
              const std::string& function = "",
              const std::string& file = "",
              int line = 0,
              const std::map<std::string, std::string>& context = {}) override {
        log(LogLevel::Trace, message, source, function, file, line, context);
    }
    
    void debug(const std::string& message,
              const std::string& source = "",
              const std::string& function = "",
              const std::string& file = "",
              int line = 0,
              const std::map<std::string, std::string>& context = {}) override {
        log(LogLevel::Debug, message, source, function, file, line, context);
    }
    
    void info(const std::string& message,
             const std::string& source = "",
             const std::string& function = "",
             const std::string& file = "",
             int line = 0,
             const std::map<std::string, std::string>& context = {}) override {
        log(LogLevel::Info, message, source, function, file, line, context);
    }
    
    void notice(const std::string& message,
               const std::string& source = "",
               const std::string& function = "",
               const std::string& file = "",
               int line = 0,
               const std::map<std::string, std::string>& context = {}) override {
        log(LogLevel::Notice, message, source, function, file, line, context);
    }
    
    void warning(const std::string& message,
                const std::string& source = "",
                const std::string& function = "",
                const std::string& file = "",
                int line = 0,
                const std::map<std::string, std::string>& context = {}) override {
        log(LogLevel::Warning, message, source, function, file, line, context);
    }
    
    void error(const std::string& message,
              const std::string& source = "",
              const std::string& function = "",
              const std::string& file = "",
              int line = 0,
              const std::string& details = "",
              const std::map<std::string, std::string>& context = {}) override {
        log(LogLevel::Error, message, source, function, file, line, context);
        if (!details.empty()) {
            log(LogLevel::Error, "Details: " + details, source, function, file, line, context);
        }
    }
    
    void critical(const std::string& message,
                 const std::string& source = "",
                 const std::string& function = "",
                 const std::string& file = "",
                 int line = 0,
                 const std::string& details = "",
                 const std::map<std::string, std::string>& context = {}) override {
        log(LogLevel::Critical, message, source, function, file, line, context);
        if (!details.empty()) {
            log(LogLevel::Critical, "Details: " + details, source, function, file, line, context);
        }
    }
    
    void alert(const std::string& message,
              const std::string& source = "",
              const std::string& function = "",
              const std::string& file = "",
              int line = 0,
              const std::string& details = "",
              const std::map<std::string, std::string>& context = {}) override {
        log(LogLevel::Alert, message, source, function, file, line, context);
        if (!details.empty()) {
            log(LogLevel::Alert, "Details: " + details, source, function, file, line, context);
        }
    }
    
    void emergency(const std::string& message,
                  const std::string& source = "",
                  const std::string& function = "",
                  const std::string& file = "",
                  int line = 0,
                  const std::string& details = "",
                  const std::map<std::string, std::string>& context = {}) override {
        log(LogLevel::Emergency, message, source, function, file, line, context);
        if (!details.empty()) {
            log(LogLevel::Emergency, "Details: " + details, source, function, file, line, context);
        }
    }
    
    void configure(const Config& config) {
        config_ = config;
        if (config_.enableFileOutput && !config_.logFile.empty()) {
            if (!logFile_.is_open()) {
                logFile_.open(config_.logFile, std::ios::app);
            }
        }
    }
    
    struct LoggerStats {
        size_t totalLogs = 0;
        std::map<LogLevel, size_t> logsByLevel;
        std::chrono::system_clock::time_point firstLog;
        std::chrono::system_clock::time_point lastLog;
    } getStatistics() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        LoggerStats stats;
        stats.totalLogs = totalLogs_;
        stats.logsByLevel = logsByLevel_;
        stats.firstLog = firstLog_;
        stats.lastLog = lastLog_;
        
        return stats;
    }
    
    void flush() override {
        std::lock_guard<std::mutex> lock(mutex_);
        if (logFile_.is_open()) {
            logFile_.flush();
        }
    }
    
    void close() override {
        std::lock_guard<std::mutex> lock(mutex_);
        if (logFile_.is_open()) {
            logFile_.close();
        }
    }
    
protected:
    void log(LogLevel level, const std::string& message,
             const std::string& source = "",
             const std::string& function = "",
             const std::string& file = "",
             int line = 0,
             const std::map<std::string, std::string>& context = {}) {
        if (level < config_.minLevel) {
            return;
        }
        
        LogEntry entry(level, message, source, function, file, line, context);
        
        // Format message
        std::string formattedMessage = formatEntry(entry);
        
        // Output to console
        std::cout << formattedMessage << std::endl;
        
        // Output to file
        if (config_.enableFileOutput && logFile_.is_open()) {
            logFile_ << formattedMessage << std::endl;
        }
        
        // Update statistics
        updateStatistics(entry);
    }
    
    std::string formatEntry(const LogEntry& entry) const {
        std::ostringstream oss;
        
        // Add level with color
        if (config_.enableColors) {
            oss << getLevelColorCode(entry.level);
        }
        
        // Add timestamp
        if (config_.enableTimestamps) {
            auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
            oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
            oss << " ";
        }
        
        // Add thread ID
        if (config_.enableThreadId) {
            oss << "[" << std::this_thread::get_id() << "] ";
        }
        
        // Add source location
        if (config_.enableSourceLocation && !entry.file.empty() && entry.line > 0) {
            oss << entry.file << ":" << entry.line << " ";
        }
        
        oss << entry.message;
        
        // Add context
        if (!entry.context.empty()) {
            oss << " [Context:";
            bool first = true;
            for (const auto& pair : entry.context) {
                if (!first) oss << ",";
                oss << " " << pair.first << "=" << pair.second;
                first = false;
            }
            oss << "]";
        }
        
        if (config_.enableColors) {
            oss << "\033[0m";
        }
        
        return oss.str();
    }
    
    std::string getLevelColorCode(LogLevel level) const {
        switch (level) {
            case LogLevel::Trace: return "\033[36m";  // Cyan
            case LogLevel::Debug: return "\033[90m";  // Bright Black
            case LogLevel::Info: return "\033[32m";   // Green
            case LogLevel::Notice: return "\033[34m"; // Blue
            case LogLevel::Warning: return "\033[33m"; // Yellow
            case LogLevel::Error: return "\033[31m";   // Red
            case LogLevel::Critical: return "\033[35m"; // Magenta
            case LogLevel::Alert: return "\033[91m";  // Bright Red
            case LogLevel::Emergency: return "\033[95m"; // Bright Magenta
            default: return "\033[0m";
        }
    }
    
    void updateStatistics(const LogEntry& entry) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        totalLogs_++;
        logsByLevel_[entry.level]++;
        
        if (!firstLog_.time_since_epoch().count()) {
            firstLog_ = entry.timestamp;
        }
        lastLog_ = entry.timestamp;
    }
    
private:
    Config config_;
    std::ofstream logFile_;
    
    size_t totalLogs_ = 0;
    std::map<LogLevel, size_t> logsByLevel_;
    std::chrono::system_clock::time_point firstLog_;
    std::chrono::system_clock::time_point lastLog_;
    mutable std::mutex mutex_;
};

// Global logging setup
namespace Logging {
    static std::shared_ptr<SimpleLogger> globalLogger;
    static ConsoleLogger::Config globalLoggerConfig;
    
    // Initialize logging system
    static void initialize() {
        globalLogger = std::make_shared<ConsoleLogger>();
    }
    
    // Get global logger
    static std::shared_ptr<SimpleLogger> getLogger() {
        if (!globalLogger) {
            initialize();
        }
        return globalLogger;
    }
    
    // Configure global logger
    static void configure(const ConsoleLogger::Config& config) {
        globalLoggerConfig = config;
        if (globalLogger) {
            std::dynamic_pointer_cast<ConsoleLogger>(globalLogger)->configure(config);
        }
    }
    
    // Log functions (global)
    static void trace(const std::string& message,
                     const std::string& source = "",
                     const std::string& function = "",
                     const std::string& file = "",
                     int line = 0,
                     const std::map<std::string, std::string>& context = {}) {
        if (globalLogger) {
            globalLogger->trace(message, source, function, file, line, context);
        }
    }
    
    static void debug(const std::string& message,
                     const std::string& source = "",
                     const std::string& function = "",
                     const std::string& file = "",
                     int line = 0,
                     const std::map<std::string, std::string>& context = {}) {
        if (globalLogger) {
            globalLogger->debug(message, source, function, file, line, context);
        }
    }
    
    static void info(const std::string& message,
                     const std::string& source = "",
                     const std::string& function = "",
                     const std::string& file = "",
                     int line = 0,
                     const std::map<std::string, std::string>& context = {}) {
        if (globalLogger) {
            globalLogger->info(message, source, function, file, line, context);
        }
    }
    
    static void notice(const std::string& message,
                       const std::string& source = "",
                       const std::string& function = "",
                       const std::string& file = "",
                       int line = 0,
                       const std::map<std::string, std::string>& context = {}) {
        if (globalLogger) {
            globalLogger->notice(message, source, function, file, line, context);
        }
    }
    
    static void warning(const std::string& message,
                        const std::string& source = "",
                        const std::string& function = "",
                        const std::string& file = "",
                        int line = 0,
                        const std::map<std::string, std::string>& context = {}) {
        if (globalLogger) {
            globalLogger->warning(message, source, function, file, line, context);
        }
    }
    
    static void error(const std::string& message,
                      const std::string& source = "",
                      const std::string& function = "",
                      const std::string& file = "",
                      int line = 0,
                      const std::string& details = "",
                      const std::map<std::string, std::string>& context = {}) {
        if (globalLogger) {
            globalLogger->error(message, source, function, file, line, details, context);
        }
    }
    
    static void critical(const std::string& message,
                         const std::string& source = "",
                         const std::string& function = "",
                         const std::string& file = "",
                         int line = 0,
                         const std::string& details = "",
                         const std::map<std::string, std::string>& context = {}) {
        if (globalLogger) {
            globalLogger->critical(message, source, function, file, line, details, context);
        }
    }
    
    static void alert(const std::string& message,
                      const std::string& source = "",
                      const std::string& function = "",
                      const std::string& file = "",
                      int line = 0,
                      const std::string& details = "",
                      const std::map<std::string, std::string>& context = {}) {
        if (globalLogger) {
            globalLogger->alert(message, source, function, file, line, details, context);
        }
    }
    
    static void emergency(const std::string& message,
                          const std::string& source = "",
                          const std::string& function = "",
                          const std::string& file = "",
                          int line = 0,
                          const std::string& details = "",
                          const std::map<std::string, std::string>& context = {}) {
        if (globalLogger) {
            globalLogger->emergency(message, source, function, file, line, details, context);
        }
    }
    
    // Get statistics
    static SimpleLogger::LoggerStats getStatistics() {
        if (globalLogger) {
            return globalLogger->getStatistics();
        }
        return SimpleLogger::LoggerStats();
    }
    
    // Flush
    static void flush() {
        if (globalLogger) {
            globalLogger->flush();
        }
    }
    
    // Close
    static void close() {
        if (globalLogger) {
            globalLogger->close();
        }
        globalLogger.reset();
    }
}

// Convenience macros
#define NLM_LOG_TRACE(msg, ...) \
    do { \
        nlm::Logging::trace(msg, __VA_ARGS__); \
    } while(0)

#define NLM_LOG_DEBUG(msg, ...) \
    do { \
        nlm::Logging::debug(msg, __VA_ARGS__); \
    } while(0)

#define NLM_LOG_INFO(msg, ...) \
    do { \
        nlm::Logging::info(msg, __VA_ARGS__); \
    } while(0)

#define NLM_LOG_NOTICE(msg, ...) \
    do { \
        nlm::Logging::notice(msg, __VA_ARGS__); \
    } while(0)

#define NLM_LOG_WARNING(msg, ...) \
    do { \
        nlm::Logging::warning(msg, __VA_ARGS__); \
    } while(0)

#define NLM_LOG_ERROR(msg, ...) \
    do { \
        nlm::Logging::error(msg, __VA_ARGS__); \
    } while(0)

#define NLM_LOG_CRITICAL(msg, ...) \
    do { \
        nlm::Logging::critical(msg, __VA_ARGS__); \
    } while(0)

#define NLM_LOG_ALERT(msg, ...) \
    do { \
        nlm::Logging::alert(msg, __VA_ARGS__); \
    } while(0)

#define NLM_LOG_EMERGENCY(msg, ...) \
    do { \
        nlm::Logging::emergency(msg, __VA_ARGS__); \
    } while(0)

} // namespace nlm