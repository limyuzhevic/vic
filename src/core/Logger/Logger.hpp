#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <mutex>
#include <fstream>
#include <unordered_map>
#include <thread>
#include <queue>
#include <condition_variable>
#include <map>
#include <iomanip>

namespace nlm {

// Log severity levels
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4
};

// Structured log formats
enum class LogFormat {
    TEXT,
    JSON,
    CSV,
    XML,
    CUSTOM
};

// Output destinations
enum class LogOutput {
    CONSOLE,
    FILE,
    BOTH
};

// Log entry structure
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::string category;
    std::string threadId;
    std::unordered_map<std::string, std::string> metadata;
};

// Log filter for category/tag filtering
struct LogFilter {
    LogLevel minLevel;
    std::vector<std::string> categories;
    std::unordered_map<std::string, std::string> metadataFilters;
    bool excludeCategories;
    bool excludeMetadata;

    LogFilter() : minLevel(LogLevel::Debug), excludeCategories(false), excludeMetadata(false) {}
};

// Log rotation configuration
struct LogRotation {
    size_t maxFileSize;
    size_t maxFiles;
    bool rotateDaily;
    std::string rotationTime;
    bool compressOldFiles;

    LogRotation() : maxFileSize(10 * 1024 * 1024), maxFiles(5), rotateDaily(false), 
                   compressOldFiles(false) {}
};

// Color configuration
struct ColorConfig {
    std::string debugColor;
    std::string infoColor;
    std::string warningColor;
    std::string errorColor;
    std::string criticalColor;
    bool useColor;

    ColorConfig() : useColor(true) {
        debugColor = "\033[36m";    // Cyan
        infoColor = "\033[32m";      // Green
        warningColor = "\033[33m";   // Yellow
        errorColor = "\033[31m";     // Red
        criticalColor = "\033[35m"; // Magenta
    }
};

// Logger interface
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    virtual void setFilter(const LogFilter& filter) = 0;
    virtual LogFilter getFilter() const = 0;
    virtual void setFormat(LogFormat format) = 0;
    virtual LogFormat getFormat() const = 0;
    virtual void setOutput(LogOutput output) = 0;
    virtual LogOutput getOutput() const = 0;
    virtual void setFilepath(const std::string& filepath) = 0;
    virtual std::string getFilepath() const = 0;
};

// Advanced console logger implementation
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(LogLevel level);
    ~ConsoleLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    void setFilter(const LogFilter& filter) override;
    LogFilter getFilter() const override;
    void setFormat(LogFormat format) override;
    LogFormat getFormat() const override;
    void setOutput(LogOutput output) override;
    LogOutput getOutput() const override;
    void setFilepath(const std::string& filepath) override;
    std::string getFilepath() const override;

    // Color configuration
    void setColorConfig(const ColorConfig& config);
    ColorConfig getColorConfig() const;
    void setUseColors(bool useColors);
    bool getUseColors() const;

    // Formatting options
    void setTimestampFormat(const std::string& format);
    std::string getTimestampFormat() const;
    void setIncludeThreadId(bool include);
    bool getIncludeThreadId() const;
    void setIncludeMetadata(bool include);
    bool getIncludeMetadata() const;

    // Set output file (in addition to console)
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    std::string getCurrentLogFile() const;

    // Log rotation
    void setRotation(const LogRotation& rotation);
    LogRotation getRotation() const;
    void enableRotation(bool enable);
    bool isRotationEnabled() const;
    std::vector<std::string> getRotatedFiles() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
    std::string formatTimestamp(const std::chrono::system_clock::time_point& timestamp) const;
    std::string formatMessage(const LogEntry& entry) const;
    std::string formatJSON(const LogEntry& entry) const;
    std::string formatCSV(const LogEntry& entry) const;
    std::string formatXML(const LogEntry& entry) const;
    bool shouldLog(const LogEntry& entry) const;
    void rotateLogFile();
    std::string getRotatedFilename() const;
};

// Async logger with thread pool
class AsyncLogger : public ILogger {
public:
    AsyncLogger(size_t poolSize = std::thread::hardware_concurrency());
    ~AsyncLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    void setFilter(const LogFilter& filter) override;
    LogFilter getFilter() const override;
    void setFormat(LogFormat format) override;
    LogFormat getFormat() const override;
    void setOutput(LogOutput output) override;
    LogOutput getOutput() const override;
    void setFilepath(const std::string& filepath) override;
    std::string getFilepath() const override;
    void flush();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Batch logger for performance
class BatchLogger : public ILogger {
public:
    BatchLogger(size_t batchSize = 100, std::chrono::milliseconds flushInterval = std::chrono::milliseconds(100));
    ~BatchLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    void setFilter(const LogFilter& filter) override;
    LogFilter getFilter() const override;
    void setFormat(LogFormat format) override;
    LogFormat getFormat() const override;
    void setOutput(LogOutput output) override;
    LogOutput getOutput() const override;
    void setFilepath(const std::string& filepath) override;
    std::string getFilepath() const override;
    void flush();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
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
             const std::string& function = "", const std::string& category = "");
    void logWithMetadata(LogLevel level, const std::string& message, 
                         const std::string& file = "", int line = 0, 
                         const std::string& function = "", const std::string& category = "",
                         const std::unordered_map<std::string, std::string>& metadata = {});
    
    // Convenience methods
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& category = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& category = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& category = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& category = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& category = "");
    
    // Stream-style logging helper
    class LogStream {
    public:
        LogStream(LogLevel level, const std::string& file, int line, const std::string& function, const std::string& category = "");
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
        std::string category_;
    };
    
    // Convenience macro for file/line/function info
    #define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
    #define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
    #define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
    #define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
    #define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

    // Advanced logging macros
    #define NLM_LOG_DEBUG_CAT(msg, cat) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__, cat)
    #define NLM_LOG_INFO_CAT(msg, cat) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__, cat)
    #define NLM_LOG_STREAM_LOG(level) nlm::Logger::LogStream(level, __FILE__, __LINE__, __func__)

    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Configuration methods for global logger
    void setGlobalLevel(LogLevel level);
    void setGlobalFormat(LogFormat format);
    void setGlobalFilter(const LogFilter& filter);
    void setGlobalOutput(LogOutput output);
    void setGlobalFilepath(const std::string& filepath);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Type-safe logging macros for common types
#define NLM_LOG_DEBUG_VAL(msg, val) NLM_LOG_STREAM_LOG(nlm::LogLevel::Debug) << msg << " = " << val
#define NLM_LOG_INFO_VAL(msg, val) NLM_LOG_STREAM_LOG(nlm::LogLevel::Info) << msg << " = " << val
#define NLM_LOG_WARNING_VAL(msg, val) NLM_LOG_STREAM_LOG(nlm::LogLevel::Warning) << msg << " = " << val

// Performance optimization macros
#define NLM_PERF_LOG(level, category, msg, duration) \
    do { \
        auto start = std::chrono::high_resolution_clock::now(); \
        /* Log the message here */ \
        auto end = std::chrono::high_resolution_clock::now(); \
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count(); \
        NLM_LOG_STREAM_LOG(level) << category << ": " << msg << " (took " << elapsed << "μs)"; \
    } while(0)

} // namespace nlm
