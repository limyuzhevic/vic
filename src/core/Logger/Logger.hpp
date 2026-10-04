#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <ctime>
#include <unordered_map>

namespace nlm {

// Log severity levels
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4
};

// Log categories for better organization
enum class LogCategory {
    System = 0,
    Neural = 1,
    Memory = 2,
    Plasticity = 3,
    Development = 4,
    Prediction = 5,
    Cognition = 6,
    Neuromodulation = 7,
    Performance = 8,
    Experiment = 9,
    User = 10,
    All = 100
};

// Enhanced log entry structure
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::string threadName;
    LogCategory category;
    
    // Structured logging fields
    std::unordered_map<std::string, std::string> metadata;
    std::unordered_map<std::string, double> numericMetadata;
    
    LogEntry()
        : level(LogLevel::Info), line(0), category(LogCategory::System) {}
    
    LogEntry(LogLevel lvl, const std::string& msg, const std::string& file = "", 
             int lineNum = 0, const std::string& func = "", 
             LogCategory cat = LogCategory::System)
        : level(lvl), message(msg), file(file), line(lineNum), function(func),
          timestamp(std::chrono::system_clock::now()), threadName(""),
          category(cat) {}
};

// Log filter interface
class LogFilter {
public:
    virtual ~LogFilter() = default;
    virtual bool shouldLog(const LogEntry& entry) const = 0;
    virtual void setLevel(LogLevel level) {}
    virtual void setCategory(LogCategory category) {}
};

// Category-based filter
class CategoryFilter : public LogFilter {
public:
    void setCategory(LogCategory category) override { category_ = category; }
    bool shouldLog(const LogEntry& entry) const override {
        return (category_ == LogCategory::All) || (entry.category == category_);
    }
private:
    LogCategory category_ = LogCategory::All;
};

// Level-based filter
class LevelFilter : public LogFilter {
public:
    void setLevel(LogLevel level) override { level_ = level; }
    bool shouldLog(const LogEntry& entry) const override {
        return entry.level >= level_;
    }
private:
    LogLevel level_ = LogLevel::Debug;
};

// Logger interface
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    virtual void setCategory(LogCategory category) {}
    virtual LogCategory getCategory() const { return LogCategory::All; }
    virtual void addFilter(std::shared_ptr<LogFilter> filter) {}
};

// Structured console logger implementation
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(LogLevel level, LogCategory category = LogCategory::All);
    ~ConsoleLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    void setCategory(LogCategory category) override;
    LogCategory getCategory() const override;
    void addFilter(std::shared_ptr<LogFilter> filter) override;
    
    // Enable/disable colors and formatting
    void setUseColors(bool useColors);
    bool getUseColors() const;
    void setUseTimestamps(bool useTimestamps);
    bool getUseTimestamps() const;
    void setUseThreadNames(bool useThreadNames);
    bool getUseThreadNames() const;
    void setUseCategory(bool useCategory);
    bool getUseCategory() const;
    
    // Set output file (in addition to console)
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
    std::string categoryToString(LogCategory category) const;
    std::string getThreadName() const;
    std::string formatTimestamp(const std::chrono::system_clock::time_point& timestamp) const;
    std::string formatEntry(const LogEntry& entry) const;
};

// Main Logger class with enhanced features
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
    
    // Set category filter for all loggers
    void setCategory(LogCategory category);
    
    // Global logging functions with enhanced features
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "", 
             LogCategory category = LogCategory::System,
             const std::string& threadName = "");
    
    // Convenience methods with category support
    void debug(const std::string& message, const std::string& file = "", 
               int line = 0, const std::string& function = "",
               LogCategory category = LogCategory::System,
               const std::string& threadName = "");
    void info(const std::string& message, const std::string& file = "", 
              int line = 0, const std::string& function = "",
              LogCategory category = LogCategory::System,
              const std::string& threadName = "");
    void warning(const std::string& message, const std::string& file = "", 
                 int line = 0, const std::string& function = "",
                 LogCategory category = LogCategory::System,
                 const std::string& threadName = "");
    void error(const std::string& message, const std::string& file = "", 
               int line = 0, const std::string& function = "",
               LogCategory category = LogCategory::System,
               const std::string& threadName = "");
    void critical(const std::string& message, const std::string& file = "", 
                  int line = 0, const std::string& function = "",
                  LogCategory category = LogCategory::System,
                  const std::string& threadName = "");
    
    // Structured logging methods
    void logStructured(LogLevel level, const std::string& message,
                       const std::unordered_map<std::string, std::string>& metadata,
                       LogCategory category = LogCategory::System);
    void logWithMetric(LogLevel level, const std::string& message,
                       const std::string& metricName, double value,
                       LogCategory category = LogCategory::System);
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Configuration methods
    void enableColors(bool enable);
    void enableTimestamps(bool enable);
    void enableThreadNames(bool enable);
    void enableCategories(bool enable);
    
    // Performance monitoring
    void startPerformanceTimer(const std::string& operation);
    void endPerformanceTimer(const std::string& operation);
    void logPerformance(const std::string& operation, double durationMs);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced convenience macros
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__, nlm::LogCategory::System)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__, nlm::LogCategory::System)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__, nlm::LogCategory::System)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__, nlm::LogCategory::System)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__, nlm::LogCategory::System)

// Category-specific macros
#define NLM_LOG_NEURAL(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__, nlm::LogCategory::Neural)
#define NLM_LOG_MEMORY(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__, nlm::LogCategory::Memory)
#define NLM_LOG_PLASTICITY(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__, nlm::LogCategory::Plasticity)
#define NLM_LOG_PERFORMANCE(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__, nlm::LogCategory::Performance)

// Structured logging macros
#define NLM_LOG_STRUCTURED(level, msg, ...) nlm::Logger::getGlobal().logStructured(level, msg, std::unordered_map<std::string, std::string>{__VA_ARGS__})

// Performance timing macros
#define NLM_PERFORMANCE_TIMER(name) nlm::PerformanceTimer timer(name)

// Stream-style logging helper (enhanced)
class LogStream {
public:
    LogStream(LogLevel level, const std::string& file, int line, 
              const std::string& function, LogCategory category = LogCategory::System);
    ~LogStream();
    
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    LogStream& operator<<(const std::string& value) {
        oss_ << value;
        return *this;
    }
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
    LogCategory category_;
};

#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__, nlm::LogCategory::System)

// Performance timer helper class
class PerformanceTimer {
public:
    PerformanceTimer(const std::string& name);
    ~PerformanceTimer();
    double getDurationMs() const;
private:
    std::string name_;
    std::chrono::steady_clock::time_point startTime_;
};

} // namespace nlm
