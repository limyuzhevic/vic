#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <mutex>
#include <thread>

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
    virtual void flush() {}  // Optional flush operation
};

// Enhanced Console logger implementation with modern features
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(LogLevel level);
    ~ConsoleLogger() override;
    
    // Core interface
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    void flush() override;
    
    // Formatting and display options
    void setUseColors(bool useColors);
    bool getUseColors() const;
    void setUseTimestamps(bool useTimestamps);
    bool getUseTimestamps() const;
    void setTimestampFormat(const std::string& format);
    std::string getTimestampFormat() const;
    void setUseThreadIds(bool useThreadIds);
    bool getUseThreadIds() const;
    void setUseFileLocations(bool useFileLocations);
    bool getUseFileLocations() const;
    void setUseFunctionNames(bool useFunctionNames);
    bool getUseFunctionNames() const;
    
    // Output management
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    bool hasOutputFile() const;
    
    // Filtering and routing
    void setTagFilters(const std::vector<std::string>& tags);
    std::vector<std::string> getTagFilters() const;
    void setLevelFilters(const std::vector<LogLevel>& levels);
    std::vector<LogLevel> getLevelFilters() const;
    void setCategoryLevel(const std::string& category, LogLevel level);
    LogLevel getCategoryLevel(const std::string& category) const;
    void addCategory(const std::string& category);
    void removeCategory(const std::string& category);
    std::vector<std::string> getCategories() const;
    
    // Performance and async features
    void setAsyncEnabled(bool enabled, size_t bufferSize = 100);
    bool isAsyncEnabled() const;
    size_t getBufferSize() const;
    bool isBufferFull() const;
    void setBatchSize(size_t size);
    
    // Advanced formatting
    void enableStructuredLogging(bool enable);
    bool isStructuredLoggingEnabled() const;
    void setLogPattern(const std::string& pattern);
    std::string getLogPattern() const;
    
    // Log level specific formatting
    void setLevelFormat(LogLevel level, const std::string& format);
    std::string getLevelFormat(LogLevel level) const;
    
    // Performance monitoring
    void setPerformanceMonitoring(bool enable);
    bool isPerformanceMonitoringEnabled() const;
    std::string getPerformanceStats() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Core logging logic
    bool shouldLog(const LogEntry& entry) const;
    std::string formatLogEntry(const LogEntry& entry) const;
    void outputLogMessage(const std::string& message);
    void flushBuffer();
    
    // Helper methods
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
    std::string extractCategory(const std::string& message) const;
    std::vector<std::string> extractTags(const std::string& message) const;
    std::string extractFileName(const std::string& filepath) const;
    std::string formatTimestamp(const std::chrono::system_clock::time_point& timestamp) const;
};

// Main Logger class with enhanced functionality
class Logger {
public:
    Logger();
    ~Logger();
    
    // Disable copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // Core logging operations
    void addLogger(std::shared_ptr<ILogger> logger);
    void removeLogger(std::shared_ptr<ILogger> logger);
    void clearLoggers();
    
    // Global logging functions with automatic source location
    void log(LogLevel level, const std::string& message,
             const std::string& file = "", int line = 0,
             const std::string& function = "");
    
    // Convenience methods with automatic source location
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Enhanced convenience methods
    void logStructured(LogLevel level, const std::string& category, const std::string& message);
    void logEvent(const std::string& eventType, const std::string& message);
    void logSystemStatus();
    
    // Level control
    void setLevel(LogLevel level);
    void setLevelForCategory(const std::string& category, LogLevel level);
    LogLevel getLevel() const;
    LogLevel getLevelForCategory(const std::string& category) const;
    
    // Logger control
    void enable(bool enable = true);
    bool isEnabled() const;
    std::string getName() const;
    void setName(const std::string& name);
    
    // Output configuration
    void setDefaultLogFile(const std::string& filepath);
    void clearLogFiles();
    
    // Performance and statistics
    void logStatistics();
    std::string getStatistics() const;
    void resetStatistics();
    
    // Global logger management
    static Logger& getGlobal();
    static void setGlobal(std::shared_ptr<Logger> logger);
    static void resetToDefault();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced stream-style logging classes
class StructuredLogStream {
public:
    StructuredLogStream(LogLevel level, const std::string& category,
                       const std::string& file = "", int line = 0,
                       const std::string& function = "");
    ~StructuredLogStream();
    
    template<typename T>
    StructuredLogStream& operator<<(const T& value) {
        if (!oss_.str().empty()) {
            oss_ << " ";
        }
        oss_ << value;
        return *this;
    }
    
    StructuredLogStream& operator<<(const std::string& value) {
        if (!oss_.str().empty()) {
            oss_ << " ";
        }
        oss_ << value;
        return *this;
    }
    
    StructuredLogStream& operator<<(const char* value) {
        if (!oss_.str().empty()) {
            oss_ << " ";
        }
        oss_ << value;
        return *this;
    }
    
    // Field-based structured logging
    template<typename T>
    StructuredLogStream& field(const std::string& key, const T& value) {
        if (!fields_.empty()) {
            oss_ << ", ";
        }
        oss_ << "\"" << key << "\": " << value;
        fields_.push_back(key);
        return *this;
    }
    
    // JSON-style output
    std::string toJson() const;
    std::string toPrettyJson() const;
    
    // Context management
    StructuredLogStream& setContext(const std::string& key, const std::string& value);
    std::string getContext(const std::string& key) const;
    
private:
    LogLevel level_;
    std::string category_;
    std::ostringstream oss_;
    std::unordered_map<std::string, std::string> fields_;
    std::unordered_map<std::string, std::string> context_;
    std::string file_;
    int line_;
    std::string function_;
    bool finalized_ = false;
};

class LogStream {
public:
    LogStream(LogLevel level, const std::string& file = "", int line = 0, const std::string& function = "");
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

// Enhanced macro definitions for different logging styles
#define NLM_LOG_SIMPLE(msg) nlm::Logger::getGlobal().info(msg)
#define NLM_LOG_STRUCTURED(category, msg) nlm::Logger::getGlobal().logStructured(nlm::LogLevel::Info, category, msg)
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

// Stream macros
#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)
#define NLM_LOG_STRUCT_STREAM(category, level) nlm::StructuredLogStream(level, category, __FILE__, __LINE__, __func__)

// Performance monitoring macros
class LogPerformanceMonitor {
public:
    LogPerformanceMonitor(const std::string& name);
    ~LogPerformanceMonitor();
    
    void log(const std::string& message);
    double getAverageTime() const;
    size_t getMessageCount() const;
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

#define NLM_PERF_MONITOR(name) nlm::LogPerformanceMonitor perfMonitor(name)

// Global helper functions
void enableStructuredLogging(bool enable = true);
void disableStructuredLogging();
bool isStructuredLoggingEnabled();

void configureGlobalLogger(const std::string& name = "global");
void resetGlobalLogger();

void setGlobalLogLevel(nlm::LogLevel level);
nlm::LogLevel getGlobalLogLevel();

void setGlobalTagFilters(const std::vector<std::string>& tags);
std::vector<std::string> getGlobalTagFilters();

void setGlobalCategoryLevel(const std::string& category, nlm::LogLevel level);
nlm::LogLevel getGlobalCategoryLevel(const std::string& category);

void addGlobalLogFile(const std::string& filepath, nlm::LogLevel level = nlm::LogLevel::Info);
void removeGlobalLogFile(const std::string& filepath);
void clearGlobalLogFiles();

void setLogPattern(const std::string& pattern);
std::string getLogPattern();

void enableLogPerformanceMonitoring(bool enable = true);
bool isLogPerformanceMonitoringEnabled();
std::string getLogPerformanceStatistics();

void flushAllLoggers();
void waitForLogFlush();

// Event system
class LogEvent {
public:
    enum class Type {
        DEBUG, INFO, WARNING, ERROR, CRITICAL,
        SYSTEM_STATUS, PERFORMANCE, CUSTOM
    };
    
    LogEvent(Type type, const std::string& message,
             const std::string& source = "",
             const std::unordered_map<std::string, std::string>& metadata = {});
    
    Type getType() const { return type_; }
    const std::string& getMessage() const { return message_; }
    const std::string& getSource() const { return source_; }
    const std::unordered_map<std::string, std::string>& getMetadata() const { return metadata_; }
    std::chrono::system_clock::time_point getTimestamp() const { return timestamp_; }
    
private:
    Type type_;
    std::string message_;
    std::string source_;
    std::unordered_map<std::string, std::string> metadata_;
    std::chrono::system_clock::time_point timestamp_;
};

class LogEventListener {
public:
    virtual ~LogEventListener() = default;
    virtual void onLogEvent(const LogEvent& event) = 0;
};

void addLogEventListener(std::shared_ptr<LogEventListener> listener);
void removeLogEventListener(std::shared_ptr<LogEventListener> listener);
void logEvent(const LogEvent& event);

// Utility functions
std::string getLogLevelName(nlm::LogLevel level);
nlm::LogLevel getLogLevelFromString(const std::string& name);

// Validation and utilities
bool validateLogMessage(const std::string& message);
void sanitizeLogMessage(std::string& message);

// Configuration management
class LogConfiguration {
public:
    static LogConfiguration& getInstance();
    
    void loadFromFile(const std::string& filepath);
    void saveToFile(const std::string& filepath) const;
    void clear();
    
    // Getters and setters for configuration options
    std::string getLogPattern() const;
    void setLogPattern(const std::string& pattern);
    
    nlm::LogLevel getDefaultLevel() const;
    void setDefaultLevel(nlm::LogLevel level);
    
    bool isStructuredLoggingEnabled() const;
    void setStructuredLoggingEnabled(bool enable);
    
    bool isPerformanceMonitoringEnabled() const;
    void setPerformanceMonitoringEnabled(bool enable);
    
    std::vector<std::string> getGlobalTags() const;
    void setGlobalTags(const std::vector<std::string>& tags);
    
private:
    LogConfiguration() = default;
    ~LogConfiguration() = default;
    LogConfiguration(const LogConfiguration&) = delete;
    LogConfiguration& operator=(const LogConfiguration&) = delete;
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm