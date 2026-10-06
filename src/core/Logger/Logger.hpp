#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <map>
#include <optional>
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

// Log categories for filtering
enum class LogCategory {
    All = 0,
    Core = 1,
    Brain = 2,
    Memory = 3,
    Plasticity = 4,
    Neuromodulation = 5,
    Cognitive = 6,
    Sensorimotor = 7,
    Environment = 8,
    Experiment = 9,
    Development = 10,
    Performance = 11,
    Debug = 12
};

// Log entry structure
struct LogEntry {
    LogLevel level;
    LogCategory category;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    // Performance timing
    std::optional<std::chrono::milliseconds> duration;
    // Structured data
    std::unordered_map<std::string, std::string> metadata;
};

// Statistics for logging
struct LogStatistics {
    int debugCount = 0;
    int infoCount = 0;
    int warningCount = 0;
    int errorCount = 0;
    int criticalCount = 0;
    int totalCount = 0;
    std::chrono::milliseconds totalDuration = std::chrono::milliseconds(0);
    std::chrono::system_clock::time_point firstLog;
    std::chrono::system_clock::time_point lastLog;
    
    void reset() {
        debugCount = infoCount = warningCount = errorCount = criticalCount = totalCount = 0;
        totalDuration = std::chrono::milliseconds(0);
        firstLog = lastLog = std::chrono::system_clock::time_point();
    }
    
    double getMessagesPerSecond() const {
        if (totalCount == 0) return 0.0;
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(lastLog - firstLog);
        return duration.count() > 0 ? (totalCount / duration.count()) : 0.0;
    }
};

// Logger interface
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    virtual void setCategoryFilter(LogCategory category, bool enabled) = 0;
    virtual bool isCategoryEnabled(LogCategory category) const = 0;
    virtual LogStatistics getStatistics() const = 0;
    virtual void resetStatistics() = 0;
};

// Performance timing helper
class PerfTimer {
public:
    PerfTimer(const std::string& operation, LogCategory category = LogCategory::Performance, LogLevel level = LogLevel::Info)
        : operation_(operation), category_(category), level_(level) {
        start_time_ = std::chrono::high_resolution_clock::now();
    }
    
    ~PerfTimer() {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time_);
        
        std::ostringstream oss;
        oss << operation_ << ": " << duration.count() << "ms";
        
        if (duration.count() > 10) { // Only log operations longer than 10ms
            Logger::getGlobal().log(level_, oss.str(), category_);
        }
    }
    
private:
    std::string operation_;
    LogCategory category_;
    LogLevel level_;
    std::chrono::high_resolution_clock::time_point start_time_;
};

// Structured logging helper
class StructuredLog {
public:
    StructuredLog(LogLevel level, LogCategory category, const std::string& message)
        : level_(level), category_(category), message_(message) {}
    
    template<typename T>
    StructuredLog& add(const std::string& key, const T& value) {
        metadata_[key] = std::to_string(value);
        return *this;
    }
    
    template<typename T>
    StructuredLog& add(const std::string& key, const std::string& value) {
        metadata_[key] = value;
        return *this;
    }
    
    void log() const {
        std::ostringstream oss;
        oss << message_;
        if (!metadata_.empty()) {
            oss << " {";
            bool first = true;
            for (const auto& pair : metadata_) {
                if (!first) oss << ", ";
                oss << pair.first << "=" << pair.second;
                first = false;
            }
            oss << "}";
        }
        Logger::getGlobal().log(level_, oss.str(), category_);
    }
    
private:
    LogLevel level_;
    LogCategory category_;
    std::string message_;
    std::unordered_map<std::string, std::string> metadata_;
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
    void setCategoryFilter(LogCategory category, bool enabled) override;
    bool isCategoryEnabled(LogCategory category) const override;
    LogStatistics getStatistics() const override;
    void resetStatistics() override;
    
    // Enable/disable colors
    void setUseColors(bool useColors);
    bool getUseColors() const;
    
    // Set output file (in addition to console)
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    
    // Get formatters
    void setMessageFormatter(std::function<std::string(const LogEntry&)> formatter);
    void setTimestampFormat(const std::string& format);
    
    // Get/set log buffer size
    void setMaxBufferSize(size_t maxSize);
    size_t getMaxBufferSize() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
    std::string formatTimestamp(const std::chrono::system_clock::time_point& timestamp) const;
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
    
    // Set category filters
    void setCategoryFilter(LogCategory category, bool enabled);
    bool isCategoryEnabled(LogCategory category) const;
    
    // Log with category and metadata
    void log(LogLevel level, const std::string& message, LogCategory category = LogCategory::All,
             const std::string& file = "", int line = 0, 
             const std::string& function = "");
    
    // Performance logging helper
    template<typename F>
    void timed(LogLevel level, LogCategory category, const std::string& operation, F&& func) {
        auto start = std::chrono::high_resolution_clock::now();
        auto result = func();
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        std::ostringstream oss;
        oss << operation << ": " << duration.count() << "ms";
        log(level, oss.str(), category);
        
        return result;
    }
    
    // Convenience methods
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Structured logging convenience
    template<typename T>
    void logStructured(LogLevel level, LogCategory category, const std::string& message) {
        StructuredLog(level, category, message).log();
    }
    
    template<typename T>
    void logStructured(LogLevel level, LogCategory category, const std::string& message, const std::string& key, const T& value) {
        StructuredLog(level, category, message).add(key, value).log();
    }
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Get log statistics
    LogStatistics getStatistics() const;
    
    // Reset statistics
    void resetStatistics();
    
    // Performance timing for current scope
    template<typename... Args>
    static PerfTimer measure(const std::string& operation, LogCategory category = LogCategory::Performance, LogLevel level = LogLevel::Info) {
        return PerfTimer(operation, category, level);
    }

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Convenience macro for file/line/function info with category
#define NLM_LOG_DEBUG_CATEGORY(msg, category) nlm::Logger::getGlobal().log(nlm::LogLevel::Debug, msg, category, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO_CATEGORY(msg, category) nlm::Logger::getGlobal().log(nlm::LogLevel::Info, msg, category, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING_CATEGORY(msg, category) nlm::Logger::getGlobal().log(nlm::LogLevel::Warning, msg, category, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR_CATEGORY(msg, category) nlm::Logger::getGlobal().log(nlm::LogLevel::Error, msg, category, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL_CATEGORY(msg, category) nlm::Logger::getGlobal().log(nlm::LogLevel::Critical, msg, category, __FILE__, __LINE__, __func__)

// Structured logging macros
#define NLM_LOG_PERF(operation) nlm::PerfTimer _perf_timer_##__LINE__(operation)
#define NLM_LOG_STRUCT(level, category, msg) nlm::StructuredLog(level, category, msg).log()
#define NLM_LOG_STRUCT_VAL(level, category, msg, key, value) nlm::StructuredLog(level, category, msg).add(key, value).log()

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

} // namespace nlm
