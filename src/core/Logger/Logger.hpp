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

// Log severity levels with hierarchy
enum class LogLevel {
    Trace = 0,           // Detailed debugging information
    Debug = 1,           // General debugging information
    Info = 2,            // General information
    Notice = 3,          // Important system events
    Warning = 4,         // Warnings about potential issues
    Error = 5,           // Error conditions
    Critical = 6,        // Critical system errors
    Alert = 7,           // Immediate action required
    Emergency = 8        // System unusable
};

// Log category for organizing log messages
enum class LogCategory {
    None = 0,
    Core = 1,
    Brain = 2,
    Dynamics = 3,
    Plasticity = 4,
    Development = 5,
    Neuromodulation = 6,
    Memory = 7,
    Cognition = 8,
    Sensory = 9,
    Motor = 10,
    Prediction = 11,
    Environment = 12,
    Agent = 13,
    Experiment = 14,
    Performance = 15,
    Debug = 16
};

// Output format options
enum class LogFormat {
    Simple,      // [LEVEL] [TIMESTAMP] message
    Detailed,    // [CATEGORY] [LEVEL] [TIMESTAMP] [FILE:LINE] [FUNCTION] message
    Structured,  // JSON-like format with metadata
    Colored      // Colored output with categories
};

// Log entry structure with additional context
struct LogEntry {
    LogLevel level;
    LogCategory category;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    
    // Additional context
    uint64_t threadId;
    std::unordered_map<std::string, std::string> metadata;
    
    LogEntry()
        : level(LogLevel::Info), category(LogCategory::None), line(0), 
          threadId(std::hash<std::thread::id>{}(std::this_thread::get_id())) {}
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
    virtual void setFormat(LogFormat format) = 0;
};

// Console logger with advanced features
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(LogLevel level, LogFormat format = LogFormat::Colored);
    ~ConsoleLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    void setCategoryFilter(LogCategory category, bool enabled) override;
    bool isCategoryEnabled(LogCategory category) const override;
    void setFormat(LogFormat format) override;
    
    // Enable/disable colors
    void setUseColors(bool useColors);
    bool getUseColors() const;
    
    // Set output file (in addition to console)
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    
    // Buffer management
    void flush();
    void setFlushOnNewline(bool flushOnNewline);
    
    // Performance monitoring
    double getLastFlushTime() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string categoryToString(LogCategory category) const;
    std::string levelToColor(LogLevel level) const;
    std::string formatEntry(const LogEntry& entry) const;
};

// File logger implementation
class FileLogger : public ILogger {
public:
    FileLogger(const std::string& filepath, bool append = true);
    ~FileLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    void setCategoryFilter(LogCategory category, bool enabled) override;
    bool isCategoryEnabled(LogCategory category) const override;
    void setFormat(LogFormat format) override;
    
    // File rotation
    void setMaxFileSize(size_t maxBytes);
    void setMaxFiles(int maxFiles);
    void rotate();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Main Logger class with advanced features
class Logger {
public:
    Logger();
    ~Logger();
    
    // Disable copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // Add logger sink
    void addLogger(std::shared_ptr<ILogger> logger);
    
    // Remove loggers
    void removeLogger(std::shared_ptr<ILogger> logger);
    void clearLoggers();
    
    // Set minimum level for all loggers
    void setLevel(LogLevel level);
    
    // Category filtering
    void enableCategory(LogCategory category);
    void disableCategory(LogCategory category);
    void toggleCategory(LogCategory category);
    bool isCategoryEnabled(LogCategory category) const;
    
    // Log methods
    void log(LogLevel level, LogCategory category, const std::string& message,
             const std::string& file = "", int line = 0, 
             const std::string& function = "");
    
    // Convenience methods
    void trace(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void notice(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void alert(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void emergency(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Category-specific logging
    void logBrain(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void logMemory(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void logNeuromodulation(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void logCognition(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Check if level/category is enabled
    bool isEnabled(LogLevel level, LogCategory category = LogCategory::None) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Configuration
    static void initializeGlobalLogger(bool useAsync = false, bool useColors = true);
    static void configureFromEnvironment();
    
    // Performance monitoring
    double getTotalLogTime() const;
    uint64_t getTotalLogCount() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Convenience macros with thread safety
#define NLM_LOG_TRACE(msg) nlm::Logger::getGlobal().trace(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_NOTICE(msg) nlm::Logger::getGlobal().notice(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ALERT(msg) nlm::Logger::getGlobal().alert(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_EMERGENCY(msg) nlm::Logger::getGlobal().emergency(msg, __FILE__, __LINE__, __func__)

// Category-specific macros
#define NLM_LOG_BRAIN(msg) nlm::Logger::getGlobal().logBrain(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_MEMORY(msg) nlm::Logger::getGlobal().logMemory(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_NEUROMODULATION(msg) nlm::Logger::getGlobal().logNeuromodulation(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_COGNITION(msg) nlm::Logger::getGlobal().logCognition(msg, __FILE__, __LINE__, __func__)

// Stream-style logging helper
class LogStream {
public:
    LogStream(LogLevel level, LogCategory category, const std::string& file, int line, const std::string& function);
    ~LogStream();
    
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    LogStream& operator<<(const std::string& value);
    LogStream& operator<<(const char* value);
    
private:
    LogLevel level_;
    LogCategory category_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
};

#define NLM_LOG_STREAM(level, category) nlm::LogStream(level, category, __FILE__, __LINE__, __func__)

// Utility functions
namespace LogUtils {
    std::string getCurrentTimestamp();
    std::string formatBytes(size_t bytes);
    double formatTime(double seconds);
}

} // namespace nlm
