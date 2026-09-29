#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <mutex>
#include <iomanip>
#include <fstream>

namespace nlm {

// Enhanced log severity levels
enum class LogLevel {
    Silent = -1,      // No logging
    Fatal = 0,        // Fatal errors
    Critical = 1,     // Critical errors
    Error = 2,        // Error conditions
    Warning = 3,      // Warning conditions
    Info = 4,         // Informational messages
    Debug = 5,        // Debug information
    Trace = 6,        // Trace level
    Perf = 7          // Performance metrics
};

// Enhanced log entry structure with additional metadata
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::string threadId;
    std::string sourceId;
    std::map<std::string, std::string> metadata;
    std::vector<std::string> tags;
    size_t threadHash;
    
    LogEntry() : level(LogLevel::Info), line(0), timestamp(std::chrono::system_clock::now()), 
                 threadHash(0) {}
    
    LogEntry(LogLevel lvl, const std::string& msg, const std::string& file = "", int line = 0, 
             const std::string& function = "")
        : level(lvl), message(msg), file(file), line(line), function(function), 
          timestamp(std::chrono::system_clock::now()), threadHash(0) {}
};

// Enhanced logger interface with additional features
class ILogger {
public:
    virtual ~ILogger() = default;
    
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    virtual void setMinimumLevel(LogLevel level) = 0;
    virtual LogLevel getMinimumLevel() const = 0;
    
    // Enhanced filtering
    virtual void setSourceFilter(const std::string& source, bool enabled) = 0;
    virtual bool isSourceEnabled(const std::string& source) const = 0;
    virtual void setLevelForSource(const std::string& source, LogLevel level) = 0;
    virtual LogLevel getLevelForSource(const std::string& source) const = 0;
    
    // Buffering
    virtual void setBufferSize(size_t size) = 0;
    virtual size_t getBufferSize() const = 0;
    virtual void flush() = 0;
    virtual bool isBufferFull() const = 0;
    
    // Output management
    virtual void setOutputStream(std::ostream& stream) = 0;
    virtual void addOutputFile(const std::string& filepath, bool append = true) = 0;
    virtual void removeOutputFile(const std::string& filepath) = 0;
    
    // Synchronization
    virtual void setSynchronous(bool synchronous) = 0;
    virtual bool isSynchronous() const = 0;
};

// Enhanced console logger with advanced features
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(LogLevel level, LogLevel minimumLevel = LogLevel::Silent);
    ~ConsoleLogger() override;
    
    // Core interface
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    void setMinimumLevel(LogLevel level) override;
    LogLevel getMinimumLevel() const override;
    
    // Enhanced features
    void setSourceFilter(const std::string& source, bool enabled) override;
    bool isSourceEnabled(const std::string& source) const override;
    void setLevelForSource(const std::string& source, LogLevel level) override;
    LogLevel getLevelForSource(const std::string& source) const override;
    
    // Color and styling
    void setUseColors(bool useColors);
    bool getUseColors() const;
    void setColorForLevel(LogLevel level, const std::string& colorCode);
    
    // Performance
    void setBufferSize(size_t size) override;
    size_t getBufferSize() const override;
    void flush() override;
    bool isBufferFull() const override;
    
    // Synchronization
    void setSynchronous(bool synchronous) override;
    bool isSynchronous() const override;
    
    // Source management
    void registerSource(const std::string& sourceId, const std::string& description = "");
    void unregisterSource(const std::string& sourceId);
    std::vector<std::string> getRegisteredSources() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string generateThreadId() const;
};

// Main Logger class
class Logger {
public:
    Logger();
    ~Logger();
    
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    void addLogger(std::shared_ptr<ILogger> logger);
    void removeLogger(std::shared_ptr<ILogger> logger);
    void clearLoggers();
    
    void setGlobalLevel(LogLevel level);
    void setGlobalMinimumLevel(LogLevel level);
    LogLevel getGlobalLevel() const;
    LogLevel getGlobalMinimumLevel() const;
    
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "",
             const std::string& sourceId = "",
             const std::map<std::string, std::string>& metadata = {});
    
    void logWithTags(LogLevel level, const std::vector<std::string>& tags, const std::string& message,
                     const std::string& file = "", int line = 0, const std::string& function = "");
    
    void debug(const std::string& message, const std::string& file = "", int line = 0, 
               const std::string& function = "",
               const std::string& sourceId = "",
               const std::map<std::string, std::string>& metadata = {});
    void info(const std::string& message, const std::string& file = "", int line = 0,
              const std::string& function = "",
              const std::string& sourceId = "",
              const std::map<std::string, std::string>& metadata = {});
    void warning(const std::string& message, const std::string& file = "", int line = 0,
                 const std::string& function = "",
                 const std::string& sourceId = "",
                 const std::map<std::string, std::string>& metadata = {});
    void error(const std::string& message, const std::string& file = "", int line = 0,
               const std::string& function = "",
               const std::string& sourceId = "",
               const std::map<std::string, std::string>& metadata = {});
    void critical(const std::string& message, const std::string& file = "", int line = 0,
                  const std::string& function = "",
                  const std::string& sourceId = "",
                  const std::map<std::string, std::string>& metadata = {});
    
    bool isEnabled(LogLevel level) const;
    bool isSourceEnabled(const std::string& sourceId, LogLevel level) const;
    
    static Logger& getGlobal();
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Performance monitoring
    void enablePerformanceMonitoring(bool enable);
    bool isPerformanceMonitoringEnabled() const;
    void logPerformanceSample(const std::string& name, double duration, LogLevel level = LogLevel::Perf);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced LogStream with metadata support
class LogStream {
public:
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function,
              const std::string& sourceId = "");
    ~LogStream();
    
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    LogStream& metadata(const std::string& key, const std::string& value);
    LogStream& tag(const std::string& tag);
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
    std::string sourceId_;
    std::map<std::string, std::string> metadata_;
    std::vector<std::string> tags_;
};

// Enhanced macros
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__, "CORE")
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__, "CORE")
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__, "CORE")
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__, "CORE")
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__, "CORE")

#define NLM_LOG_DEBUG_WITH_METADATA(msg, metadata) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__, "CORE", metadata)
#define NLM_LOG_INFO_WITH_TAGS(level, tags, msg) nlm::Logger::getGlobal().logWithTags(level, tags, msg, __FILE__, __LINE__, __func__)

// Thread-safe logger
class ThreadSafeLogger {
public:
    static ThreadSafeLogger& getInstance() {
        static ThreadSafeLogger instance;
        return instance;
    }
    
    void log(LogLevel level, const std::string& source, const std::string& message,
             const std::string& file = "", int line = 0,
             const std::map<std::string, std::string>& metadata = {});
    
    void setGlobalLogger(std::shared_ptr<Logger> logger) {
        globalLogger_ = logger;
    }
    
private:
    ThreadSafeLogger() = default;
    std::mutex mutex_;
    static std::shared_ptr<Logger> globalLogger_;
};

} // namespace nlm
