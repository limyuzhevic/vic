// Unified Logging System
// Core logging infrastructure for NLM

#ifndef NLM_UNIFIED_LOGGING_H
#define NLM_UNIFIED_LOGGING_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

namespace nlm {
namespace logging {

// Log level enumeration
enum class LogLevel {
    OFF = 0,
    FATAL = 1,
    ERROR = 2,
    WARN = 3,
    INFO = 4,
    DEBUG = 5,
    TRACE = 6
};

// Log entry structure
struct LogEntry {
    std::chrono::system_clock::time_point timestamp;
    LogLevel level;
    std::string module;
    std::string message;
    std::string file;
    int line;
    std::map<std::string, std::string> metadata;
    std::vector<std::string> tags;
    bool colored;
};

// Log sink interface
class LogSink {
public:
    virtual ~LogSink() = default;
    virtual void write(const LogEntry& entry) = 0;
    virtual void flush() = 0;
    virtual bool isOpen() const = 0;
};

// Console log sink
class ConsoleLogSink : public LogSink {
public:
    explicit ConsoleLogSink(bool colored = true);
    void write(const LogEntry& entry) override;
    void flush() override;
    bool isOpen() const override { return true; }
    
    void setLevel(LogLevel level);
    LogLevel getLevel() const { return currentLevel; }
    void setColored(bool colored) { this->colored = colored; }
    
private:
    LogLevel currentLevel;
    bool colored;
    std::mutex mutex;
    
    std::string formatLevel(LogLevel level) const;
    std::string getLevelColor(LogLevel level) const;
};

// File log sink
class FileLogSink : public LogSink {
public:
    explicit FileLogSink(const std::string& filename);
    ~FileLogSink() override;
    
    void write(const LogEntry& entry) override;
    void flush() override;
    bool isOpen() const override { return file != nullptr && !file->fail(); }
    
    void setLevel(LogLevel level);
    void setFileSizeLimit(size_t maxSize);
    void setFileCountLimit(size_t maxFiles);
    void setLogFormat(const std::string& format);
    
private:
    std::unique_ptr<std::ofstream> file;
    LogLevel currentLevel;
    size_t maxFileSize;
    size_t maxFileCount;
    std::string logFormat;
    std::string currentFile;
    size_t currentFileSize;
    std::mutex mutex;
    
    void rotateFileIfNeeded();
    std::string getRotatedFilename(const std::string& baseFilename, int index) const;
};

// JSON log sink
class JsonLogSink : public LogSink {
public:
    explicit JsonLogSink(const std::string& filename);
    ~JsonLogSink() override;
    
    void write(const LogEntry& entry) override;
    void flush() override;
    bool isOpen() const override { return file != nullptr && !file->fail(); }
    
private:
    std::unique_ptr<std::ofstream> file;
    std::mutex mutex;
};

// Logger class - unified logging interface
class Logger {
public:
    static Logger& getInstance();
    
    void setLevel(LogLevel level);
    LogLevel getLevel() const { return defaultLevel; }
    
    void setDefaultModule(const std::string& module) { defaultModule = module; }
    const std::string& getDefaultModule() const { return defaultModule; }
    
    // Log methods
    void log(LogLevel level, const std::string& message,
             const std::string& file = "", int line = 0,
             const std::map<std::string, std::string>& metadata = {});
    
    void log(LogLevel level, const std::string& module, const std::string& message,
             const std::string& file = "", int line = 0,
             const std::map<std::string, std::string>& metadata = {});
    
    // Convenience methods
    void fatal(const std::string& message);
    void fatal(const std::string& module, const std::string& message);
    
    void error(const std::string& message);
    void error(const std::string& module, const std::string& message);
    
    void warn(const std::string& message);
    void warn(const std::string& module, const std::string& message);
    
    void info(const std::string& message);
    void info(const std::string& module, const std::string& message);
    
    void debug(const std::string& message);
    void debug(const std::string& module, const std::string& message);
    
    void trace(const std::string& message);
    void trace(const std::string& module, const std::string& message);
    
    // Log management
    void addSink(std::unique_ptr<LogSink> sink);
    void removeSink(LogSink* sink);
    void clearSinks();
    
    // Log queries
    std::vector<LogEntry> getRecentLogs(size_t limit = 100) const;
    std::vector<LogEntry> searchLogs(const std::string& pattern, LogLevel level = LogLevel::OFF) const;
    
    // Performance logging
    void logPerformance(const std::string& operation, std::chrono::duration<double> duration,
                       const std::map<std::string, std::string>& metadata = {});
    
    // Log statistics
    size_t getLogCount() const { return logBuffer.size(); }
    size_t getFatalCount() const { return fatalCount; }
    size_t getErrorCount() const { return errorCount; }
    size_t getWarnCount() const { return warnCount; }
    size_t getInfoCount() const { return infoCount; }
    size_t getDebugCount() const { return debugCount; }
    size_t getTraceCount() const { return traceCount; }
    
private:
    Logger();
    ~Logger();
    
    void outputToSinks(const LogEntry& entry);
    bool shouldLog(LogLevel level) const;
    
    LogLevel defaultLevel;
    std::string defaultModule;
    std::vector<std::unique_ptr<LogSink>> sinks;
    std::vector<LogEntry> logBuffer;
    size_t maxLogBufferSize;
    
    // Statistics
    size_t fatalCount;
    size_t errorCount;
    size_t warnCount;
    size_t infoCount;
    size_t debugCount;
    size_t traceCount;
    
    std::mutex mutex;
};

// Macro definitions for easy logging
#define NLM_LOG_FATAL(msg) nlm::logging::Logger::getInstance().fatal(msg)
#define NLM_LOG_FATAL_MODULE(mod, msg) nlm::logging::Logger::getInstance().fatal(mod, msg)

#define NLM_LOG_ERROR(msg) nlm::logging::Logger::getInstance().error(msg)
#define NLM_LOG_ERROR_MODULE(mod, msg) nlm::logging::Logger::getInstance().error(mod, msg)

#define NLM_LOG_WARN(msg) nlm::logging::Logger::getInstance().warn(msg)
#define NLM_LOG_WARN_MODULE(mod, msg) nlm::logging::Logger::getInstance().warn(mod, msg)

#define NLM_LOG_INFO(msg) nlm::logging::Logger::getInstance().info(msg)
#define NLM_LOG_INFO_MODULE(mod, msg) nlm::logging::Logger::getInstance().info(mod, msg)

#define NLM_LOG_DEBUG(msg) nlm::logging::Logger::getInstance().debug(msg)
#define NLM_LOG_DEBUG_MODULE(mod, msg) nlm::logging::Logger::getInstance().debug(mod, msg)

#define NLM_LOG_TRACE(msg) nlm::logging::Logger::getInstance().trace(msg)
#define NLM_LOG_TRACE_MODULE(mod, msg) nlm::logging::Logger::getInstance().trace(mod, msg)

// Helper function to get current file and line
#define NLM_LOG_CURRENT_FILE __FILE__
#define NLM_LOG_CURRENT_LINE __LINE__

// Context-aware logging macros
#define NLM_LOG_ERROR_WITH_CONTEXT(module, msg, ...) \
    nlm::logging::Logger::getInstance().error(module, \
        nlm::formatString(msg, ##__VA_ARGS__))

#define NLM_LOG_INFO_WITH_CONTEXT(module, msg, ...) \
    nlm::logging::Logger::getInstance().info(module, \
        nlm::formatString(msg, ##__VA_ARGS__))

// Performance logging macro
#define NLM_PERFORMANCE_LOG(operation, duration) \
    nlm::logging::Logger::getInstance().logPerformance(operation, duration)

// Initialize logging system
void initializeLogging(int argc, char* argv[]);

} // namespace logging
} // namespace nlm

#endif // NLM_UNIFIED_LOGGING_H
