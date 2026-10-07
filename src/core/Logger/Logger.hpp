// Improved NLM Logging System - Enhanced Implementation
// This is the new enhanced Logger.hpp implementation that maintains backward compatibility
// while adding all requested features

#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <mutex>
#include <unordered_map>
#include <thread>
#include <map>
#include <fstream>
#include <iomanip>
#include <atomic>
#include <ctime>
#include <sys/types.h>
#include <unistd.h>

namespace nlm {

// Enhanced log severity levels with backward compatibility
enum class LogLevel {
    Trace = 0,           // 0 - Most detailed, lowest priority (NEW)
    Debug = 1,           // 1 - Original
    Info = 2,            // 2 - Original
    Notice = 3,          // 3 - NEW (less common but important events)
    Warning = 4,         // 4 - Original
    Error = 5,           // 5 - Original
    Critical = 6,        // 6 - Original
    Alert = 7,           // 7 - NEW (immediate action required)
    Emergency = 8,       // 8 - NEW (highest priority, system-wide failure)
    Custom = 100         // Custom levels for user-defined priorities
};

// Performance timing
struct PerformanceMetrics {
    std::chrono::nanoseconds duration;
    std::string operation;
    uint64_t memoryUsage;
};

// Structured data container for JSON/key-value pairs
struct StructuredData {
    std::string json;
    std::unordered_map<std::string, std::string> keyValue;
    bool isJson;
};

// Enhanced log entry structure with all new fields
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    
    // NEW: Additional fields for enhanced logging
    std::string category;
    std::thread::id threadId;
    pid_t processId;
    std::string operationContext;
    std::vector<std::string> tags;
    PerformanceMetrics performanceMetrics;
    StructuredData structuredData;
    std::chrono::system_clock::time_point logTime;
    std::unordered_map<std::string, std::string> metadata;
};

// Base formatter interface
class IFormatter {
public:
    virtual ~IFormatter() = default;
    virtual std::string format(const LogEntry& entry) = 0;
    virtual std::string getName() const = 0;
};

// JSON formatter implementation
class JSONFormatter : public IFormatter {
public:
    std::string format(const LogEntry& entry) override {
        std::ostringstream oss;
        oss << "{";
        oss << "\"timestamp\": \"" << formatTime(entry.timestamp) << "\",";
        oss << "\"level\": \"" << levelToString(entry.level) << "\",";
        oss << "\"message\": \"" << escapeJSON(entry.message) << "\"";
        
        if (!entry.category.empty()) {
            oss << ",\"category\": \"" << escapeJSON(entry.category) << "\"";
        }
        if (!entry.operationContext.empty()) {
            oss << ",\"operation\": \"" << escapeJSON(entry.operationContext) << "\"";
        }
        oss << "}";
        return oss.str();
    }
    
    std::string getName() const override { return "json"; }
    
private:
    std::string formatTime(const std::chrono::system_clock::time_point& tp) {
        auto time = std::chrono::system_clock::to_time_t(tp);
        std::tm tm_buf;
        gmtime_r(&time, &tm_buf);
        std::ostringstream oss;
        oss << std::put_time(&tm_buf, "%Y-%m-%dT%H:%M:%SZ");
        return oss.str();
    }
    
    std::string escapeJSON(const std::string& str) {
        std::ostringstream oss;
        for (char c : str) {
            switch (c) {
                case '\\': oss << "\\\\"; break;
                case '\"': oss << "\\\""; break;
                case '\b': oss << "\\b"; break;
                case '\f': oss << "\\f"; break;
                case '\n': oss << "\\n"; break;
                case '\r': oss << "\\r"; break;
                case '\t': oss << "\\t"; break;
                default:
                    if (c < 32 || c > 126) {
                        oss << "\\u" << std::hex << std::setw(4) << std::setfill('0') << (int)c;
                    } else {
                        oss << c;
                    }
            }
        }
        return oss.str();
    }
};

// Key-value formatter implementation
class KeyValueFormatter : public IFormatter {
public:
    std::string format(const LogEntry& entry) override {
        std::ostringstream oss;
        oss << levelToString(entry.level) << " " << entry.message;
        
        if (!entry.category.empty()) {
            oss << " [" << entry.category << "]";
        }
        if (!entry.operationContext.empty()) {
            oss << " {" << entry.operationContext << "}";
        }
        
        if (!entry.structuredData.keyValue.empty()) {
            oss << " ";
            bool first = true;
            for (const auto& pair : entry.structuredData.keyValue) {
                if (!first) oss << ", ";
                oss << pair.first << "=" << pair.second;
                first = false;
            }
        }
        return oss.str();
    }
    
    std::string getName() const override { return "keyvalue"; }
};

// Template-based custom formatter
class TemplateFormatter : public IFormatter {
public:
    TemplateFormatter(const std::string& template) : template_(template) {}
    
    std::string format(const LogEntry& entry) override {
        std::ostringstream oss;
        std::string result = template_;
        
        // Replace placeholders
        replacePlaceholder(result, "%timestamp%", formatTime(entry.timestamp));
        replacePlaceholder(result, "%level%", levelToString(entry.level));
        replacePlaceholder(result, "%message%", entry.message);
        replacePlaceholder(result, "%file%", entry.file);
        replacePlaceholder(result, "%line%", std::to_string(entry.line));
        replacePlaceholder(result, "%function%", entry.function);
        replacePlaceholder(result, "%category%", entry.category);
        replacePlaceholder(result, "%thread_id%", threadIdToString(entry.threadId));
        replacePlaceholder(result, "%process_id%", pidToString(entry.processId));
        
        return result;
    }
    
    std::string getName() const override { return "template"; }
    
    void setTemplate(const std::string& template) { template_ = template; }
    
private:
    std::string template_;
    std::string formatTime(const std::chrono::system_clock::time_point& tp) {
        auto time = std::chrono::system_clock::to_time_t(tp);
        std::tm tm_buf;
        gmtime_r(&time, &tm_buf);
        std::ostringstream oss;
        oss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }
    
    void replacePlaceholder(std::string& str, const std::string& placeholder, const std::string& value) {
        size_t pos = 0;
        while ((pos = str.find(placeholder, pos)) != std::string::npos) {
            str.replace(pos, placeholder.length(), value);
            pos += value.length();
        }
    }
    
    std::string threadIdToString(std::thread::id id) {
        std::stringstream ss;
        ss << id;
        return ss.str();
    }
    
    std::string pidToString(pid_t pid) {
        return std::to_string(pid);
    }
};

// Performance formatter
class PerformanceFormatter : public IFormatter {
public:
    std::string format(const LogEntry& entry) override {
        if (entry.performanceMetrics.duration.count() <= 0) {
            return levelToString(entry.level) + " " + entry.message;
        }
        
        std::ostringstream oss;
        oss << levelToString(entry.level) << " " << entry.message;
        oss << " [duration=" << entry.performanceMetrics.duration.count() / 1000000.0 
            << "ms, operation=\"" << entry.performanceMetrics.operation << "\"]";
        
        return oss.str();
    }
    
    std::string getName() const override { return "performance"; }
};

// Logger interface
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    virtual void setFormatter(std::shared_ptr<IFormatter> formatter) = 0;
    virtual std::shared_ptr<IFormatter> getFormatter() const = 0;
};

// Async logger implementation
class AsyncLogger : public ILogger {
public:
    AsyncLogger(size_t queueSize = 10000, size_t workerThreads = 2);
    ~AsyncLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    void setFormatter(std::shared_ptr<IFormatter> formatter) override;
    std::shared_ptr<IFormatter> getFormatter() const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Category logger implementation
class CategoryLogger : public ILogger {
public:
    CategoryLogger(const std::string& category, std::shared_ptr<ILogger> baseLogger);
    ~CategoryLogger() override = default;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    void setFormatter(std::shared_ptr<IFormatter> formatter) override;
    std::shared_ptr<IFormatter> getFormatter() const override;
    
    void addCategoryFilter(const std::string& category, LogLevel level);
    void removeCategoryFilter(const std::string& category);
    void setCategoryFilter(const std::unordered_map<std::string, LogLevel>& filters);
    
private:
    std::string category_;
    std::shared_ptr<ILogger> baseLogger_;
    std::unordered_map<std::string, LogLevel> categoryFilters_;
    std::mutex mutex_;
};

// Log rotation utility
class LogRotation {
public:
    LogRotation(const std::string& baseFilename, size_t maxFiles = 10, uint64_t maxSize = 10 * 1024 * 1024); // 10MB
    ~LogRotation();
    
    void rotateIfNeeded();
    std::string getCurrentFile() const { return currentFile_; }
    void write(const std::string& data);
    
private:
    std::string baseFilename_;
    size_t maxFiles_;
    uint64_t maxSize_;
    std::atomic<uint64_t> currentSize_{0};
    std::string currentFile_;
    std::mutex mutex_;
    std::ofstream fileOut_;
    
    void cleanupOldFiles();
    std::string getRotatedFilename(size_t index) const;
};

// Log statistics
class LogStatistics {
public:
    LogStatistics();
    ~LogStatistics() = default;
    
    void recordLog(const LogEntry& entry);
    void reset();
    
    // Get statistics
    size_t getLogCount(LogLevel level = LogLevel::Custom) const;
    double getLogRate() const; // logs per second
    std::unordered_map<LogLevel, size_t> getLevelCounts() const;
    std::unordered_map<std::string, size_t> getCategoryCounts() const;
    std::vector<std::pair<std::string, uint64_t>> getTopMessages(size_t limit = 10) const;
    
    // Get formatted statistics
    std::string getFormattedStats() const;
    
private:
    struct Stats {
        std::atomic<size_t> count{0};
        std::chrono::system_clock::time_point firstLog;
        std::chrono::system_clock::time_point lastLog;
        std::unordered_map<LogLevel, size_t> levelCounts;
        std::unordered_map<std::string, size_t> categoryCounts;
        std::unordered_map<std::string, size_t> messageCounts;
    };
    
    mutable std::mutex mutex_;
    Stats stats_;
};

// Log context for operation tracking
class LogContext {
public:
    LogContext(const std::string& operation, const std::string& category = "");
    ~LogContext();
    
    void setProperty(const std::string& key, const std::string& value);
    std::string getProperty(const std::string& key) const;
    std::string getOperation() const { return operation_; }
    std::string getCategory() const { return category_; }
    
private:
    std::string operation_;
    std::string category_;
    std::unordered_map<std::string, std::string> properties_;
    std::mutex mutex_;
    static thread_local LogContext* currentContext_;
    
    friend LogContext* getCurrentContext();
    friend void setCurrentContext(LogContext* context);
};

// Performance timer utility
class LogPerformanceTimer {
public:
    LogPerformanceTimer(const std::string& operation, const std::string& category = "performance");
    ~LogPerformanceTimer();
    
    void stop();
    bool isRunning() const { return running_; }
    
    template<typename T>
    LogPerformanceTimer& operator<<(const T& value) {
        tempData_ << value;
        return *this;
    }
    
private:
    std::string operation_;
    std::string category_;
    std::chrono::high_resolution_clock::time_point startTime_;
    bool running_ = true;
    std::ostringstream tempData_;
    std::mutex mutex_;
};

// Original ConsoleLogger class (backward compatibility)
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
    
    void setFormatter(std::shared_ptr<IFormatter> formatter) override;
    std::shared_ptr<IFormatter> getFormatter() const override;
    
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
    
    // Enhanced logging with new parameters
    void log(LogLevel level, const std::string& message,
             const std::string& file, int line,
             const std::string& function,
             const std::string& category,
             const std::vector<std::string>& tags);
    
    // Structured logging functions
    void logStructured(LogLevel level, const std::string& message,
                       const StructuredData& structured,
                       const std::string& file = "", int line = 0,
                       const std::string& function = "",
                       const std::string& category = "");
    
    // Performance timing logging
    void logPerformance(const PerformanceMetrics& metrics,
                        const std::string& file = "", int line = 0,
                        const std::string& function = "");
    
    // Convenience methods (original API)
    void trace(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void notice(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void alert(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void emergency(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Statistics access
    static LogStatistics& getStatistics();
    
    // Context management
    static LogContext& getCurrentContext();
    static void setCurrentContext(LogContext& context);
    
    // Enhanced convenience methods
    void logWithContext(LogLevel level, const std::string& message,
                        const std::string& operation,
                        const std::string& category = "",
                        const std::vector<std::string>& tags = {});
    
    void logWithProperties(LogLevel level, const std::string& message,
                           const std::unordered_map<std::string, std::string>& properties,
                           const std::string& file = "", int line = 0, const std::string& function = "");

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
};

// Convenience macros for backward compatibility
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

// Convenience macros for enhanced logging
#define NLM_LOG_TRACE(msg) nlm::Logger::getGlobal().trace(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_NOTICE(msg) nlm::Logger::getGlobal().notice(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ALERT(msg) nlm::Logger::getGlobal().alert(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_EMERGENCY(msg) nlm::Logger::getGlobal().emergency(msg, __FILE__, __LINE__, __func__)

// Enhanced macros with context
#define NLM_LOG_CONTEXT(level, msg, operation, category) nlm::Logger::getGlobal().logWithContext(level, msg, operation, category)

// Performance timing macros
#define NLM_PERF_TIMER(category, operation) nlm::LogPerformanceTimer timer(category, operation)

// Structured logging macros
#define NLM_LOG_JSON(level, msg, json) \
    do { \
        nlm::StructuredData sd; \
        sd.json = json; \
        sd.isJson = true; \
        nlm::Logger::getGlobal().logStructured(level, msg, sd, __FILE__, __LINE__, __func__); \
    } while(0)

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
    
    // Log with category
    LogStream& category(const std::string& category) {
        category_ = category;
        return *this;
    }
    
    // Log with operation context
    LogStream& operation(const std::string& operation) {
        operation_ = operation;
        return *this;
    }
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
    std::string category_;
    std::string operation_;
    std::chrono::system_clock::time_point timestamp_;
};

#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

// Global convenience functions
void initializeLogging();
std::shared_ptr<Logger> createDefaultLogger();
void configureDefaultLogger(const std::string& logfile = "", LogLevel level = LogLevel::Info);

// Global helper functions outside namespace for convenience
inline std::shared_ptr<nlm::Logger> getGlobalLogger() {
    return std::make_shared<nlm::Logger>(nlm::Logger::getGlobal());
}

inline void setGlobalLogLevel(nlm::LogLevel level) {
    nlm::Logger::getGlobal().setLevel(level);
}

inline void addLogFile(const std::string& filename, nlm::LogLevel level = nlm::LogLevel::Info) {
    // Implementation would add file logging
}

inline std::string formatLogEntry(const nlm::LogEntry& entry) {
    // Format log entry as string
    std::ostringstream oss;
    oss << "[" << entry.timestamp.time_since_epoch().count() << "] "
        << static_cast<int>(entry.level) << " " << entry.message;
    if (!entry.category.empty()) oss << " [" << entry.category << "]";
    if (!entry.operationContext.empty()) oss << " {" << entry.operationContext << "}";
    return oss.str();
}

// Forward declarations
LogContext* getCurrentContext();
void setCurrentContext(LogContext* context);

} // namespace nlm