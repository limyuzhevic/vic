#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <atomic>
#include <shared_mutex>
#include <unordered_map>
#include <unordered_set>
#include <deque>
#include <map>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <optional>
#include <variant>
#include <span>
#include <numeric>
#include <cassert>
#include <cstring>
#include <cmath>

namespace nlm
{

// Forward declarations
class ILogger;
class LogAppender;
class LogFormatter;
class LogBuffer;

// Log severity levels with hierarchy support
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4
};

// Custom log level for extended severity hierarchy
enum class CustomLogLevel {
    Trace = -2,
    Debug = 0,
    Info = 1,
    Notice = 2,
    Warning = 3,
    Error = 4,
    Critical = 5,
    Alert = 6,
    Emergency = 7
};

// Log level hierarchy comparison
inline bool operator<(const LogLevel& lhs, const LogLevel& rhs) {
    return static_cast<int>(lhs) < static_cast<int>(rhs);
}

inline bool operator<=(const LogLevel& lhs, const LogLevel& rhs) {
    return static_cast<int>(lhs) <= static_cast<int>(rhs);
}

inline bool operator>(const LogLevel& lhs, const LogLevel& rhs) {
    return static_cast<int>(lhs) > static_cast<int>(rhs);
}

inline bool operator>=(const LogLevel& lhs, const LogLevel& rhs) {
    return static_cast<int>(lhs) >= static_cast<int>(rhs);
}

// Log entry structure with pooling support
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    
    // Structured logging fields
    std::unordered_map<std::string, std::string> fields;
    std::unordered_set<std::string> tags;
    std::string module;
    std::thread::id threadId;
    uint64_t sequenceNumber = 0;
    
    LogEntry() : level(LogLevel::Info), line(0), threadId(std::this_thread::get_id()) {}
    LogEntry(LogLevel lvl, const std::string& msg, const std::string& file = "", 
             int ln = 0, const std::string& func = "")
        : level(lvl), message(msg), file(file), line(ln), function(func),
          timestamp(std::chrono::system_clock::now()), threadId(std::this_thread::get_id()) {}
};

// Logger filter interface for filtering log entries
class ILogFilter {
public:
    virtual ~ILogFilter() = default;
    virtual bool shouldLog(const LogEntry& entry) const = 0;
    virtual void setEnabled(bool enabled) = 0;
    virtual bool isEnabled() const = 0;
};

// Logger interface
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    
    // Configuration and control
    virtual void setPattern(const std::string& pattern) = 0;
    virtual std::string getPattern() const = 0;
    virtual void setFilter(std::shared_ptr<ILogFilter> filter) = 0;
    virtual std::shared_ptr<ILogFilter> getFilter() const = 0;
    virtual void setFlushOnEachLog(bool flush) = 0;
    virtual bool getFlushOnEachLog() const = 0;
    
    // Batch logging support
    virtual void logBatch(const std::vector<LogEntry>& entries) = 0;
    
    // Performance monitoring
    virtual uint64_t getLogCount() const = 0;
    virtual double getAverageLatency() const = 0;
    virtual std::chrono::steady_clock::time_point getLastLogTime() const = 0;
};

// Log formatter interface
class LogFormatter {
public:
    virtual ~LogFormatter() = default;
    virtual std::string format(const LogEntry& entry) = 0;
    virtual void setPattern(const std::string& pattern) = 0;
    virtual std::string getPattern() const = 0;
};

// Log appender interface
class LogAppender {
public:
    virtual ~LogAppender() = default;
    virtual void append(const std::string& formattedMessage) = 0;
    virtual void flush() = 0;
    virtual void close() = 0;
    virtual bool isOpen() const = 0;
    virtual void setFormat(const std::string& format) = 0;
};

// Memory pool for LogEntry objects
class LogEntryPool {
private:
    std::deque<LogEntry> pool_;
    std::mutex mutex_;
    size_t maxSize_ = 1024;
    size_t currentSize_ = 0;
    
public:
    LogEntryPool(size_t maxSize = 1024) : maxSize_(maxSize) {}
    
    LogEntry* acquire() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!pool_.empty()) {
            LogEntry* entry = &pool_.back();
            pool_.pop_back();
            return entry;
        }
        return nullptr;
    }
    
    void release(LogEntry* entry) {
        if (!entry) return;
        std::lock_guard<std::mutex> lock(mutex_);
        if (currentSize_ < maxSize_) {
            pool_.push_back(*entry);
            currentSize_++;
        }
    }
    
    size_t size() const { return currentSize_; }
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        pool_.clear();
        currentSize_ = 0;
    }
};

// String pool for reducing allocations
class StringPool {
private:
    std::deque<std::string> pool_;
    std::mutex mutex_;
    size_t maxSize_ = 2048;
    size_t currentSize_ = 0;
    
public:
    StringPool(size_t maxSize = 2048) : maxSize_(maxSize) {}
    
    std::string* acquire() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!pool_.empty()) {
            std::string* str = &pool_.back();
            pool_.pop_back();
            str->clear();
            return str;
        }
        return nullptr;
    }
    
    void release(std::string* str) {
        if (!str) return;
        std::lock_guard<std::mutex> lock(mutex_);
        if (currentSize_ < maxSize_) {
            pool_.push_back(std::move(*str));
            currentSize_++;
        }
    }
    
    size_t size() const { return currentSize_; }
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        pool_.clear();
        currentSize_ = 0;
    }
};

// Thread-local storage for performance
class ThreadLocalStorage {
private:
    thread_local static inline std::unique_ptr<LogEntryPool> entryPool_;
    thread_local static inline std::unique_ptr<StringPool> stringPool_;
    
public:
    static LogEntryPool* getEntryPool(size_t maxSize = 1024) {
        if (!entryPool_) {
            entryPool_ = std::make_unique<LogEntryPool>(maxSize);
        }
        return entryPool_.get();
    }
    
    static StringPool* getStringPool(size_t maxSize = 2048) {
        if (!stringPool_) {
            stringPool_ = std::make_unique<StringPool>(maxSize);
        }
        return stringPool_.get();
    }
    
    static void clear() {
        if (entryPool_) entryPool_->clear();
        if (stringPool_) stringPool_->clear();
    }
};

// Pattern formatter implementation
class PatternFormatter : public LogFormatter {
private:
    std::string pattern_;
    
public:
    PatternFormatter(const std::string& pattern = "%timestamp %level %module [%file:%line] %message") 
        : pattern_(pattern) {}
    
    std::string format(const LogEntry& entry) override {
        std::string result = pattern_;
        
        // Replace placeholders
        size_t pos = 0;
        while ((pos = result.find("%timestamp", pos)) != std::string::npos) {
            std::time_t timestamp = std::chrono::system_clock::to_time_t(entry.timestamp);
            char timeBuf[100];
            std::strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M:%S", std::localtime(&timestamp));
            result.replace(pos, 10, timeBuf);
            pos += strlen(timeBuf);
        }
        
        pos = 0;
        while ((pos = result.find("%level", pos)) != std::string::npos) {
            std::string levelStr;
            switch (entry.level) {
                case LogLevel::Debug: levelStr = "DEBUG"; break;
                case LogLevel::Info: levelStr = "INFO"; break;
                case LogLevel::Warning: levelStr = "WARN"; break;
                case LogLevel::Error: levelStr = "ERROR"; break;
                case LogLevel::Critical: levelStr = "CRIT"; break;
            }
            result.replace(pos, 5, levelStr);
            pos += levelStr.size();
        }
        
        pos = 0;
        while ((pos = result.find("%module", pos)) != std::string::npos) {
            result.replace(pos, 7, entry.module.empty() ? "" : entry.module);
            pos += entry.module.size();
        }
        
        pos = 0;
        while ((pos = result.find("%file", pos)) != std::string::npos) {
            size_t lastSlash = entry.file.find_last_of("/\\");
            std::string filename = (lastSlash != std::string::npos) ? entry.file.substr(lastSlash + 1) : entry.file;
            result.replace(pos, 5, filename);
            pos += filename.size();
        }
        
        pos = 0;
        while ((pos = result.find("%line", pos)) != std::string::npos) {
            result.replace(pos, 5, std::to_string(entry.line));
            pos += std::to_string(entry.line).size();
        }
        
        pos = 0;
        while ((pos = result.find("%message", pos)) != std::string::npos) {
            result.replace(pos, 7, entry.message);
            pos += entry.message.size();
        }
        
        pos = 0;
        while ((pos = result.find("%tags", pos)) != std::string::npos) {
            if (!entry.tags.empty()) {
                std::string tags;
                bool first = true;
                for (const auto& tag : entry.tags) {
                    if (!first) tags += ",";
                    tags += tag;
                    first = false;
                }
                result.replace(pos, 5, tags);
                pos += tags.size();
            }
        }
        
        // Add fields
        for (const auto& field : entry.fields) {
            pos = 0;
            while ((pos = result.find("%" + field.first, pos)) != std::string::npos) {
                result.replace(pos, field.first.size() + 1, field.second);
                pos += field.second.size();
            }
        }
        
        return result;
    }
    
    void setPattern(const std::string& pattern) override { pattern_ = pattern; }
    std::string getPattern() const override { return pattern_; }
};

// Console appender implementation
class ConsoleAppender : public LogAppender {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
public:
    ConsoleAppender();
    ~ConsoleAppender() override;
    
    void append(const std::string& formattedMessage) override;
    void flush() override;
    void close() override;
    bool isOpen() const override;
    void setFormat(const std::string& format) override;
    
    // Control methods
    void setLevel(LogLevel level);
    LogLevel getLevel() const;
    void setUseColors(bool useColors);
    bool getUseColors() const;
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
};

// Log filter implementations
class LevelFilter : public ILogFilter {
private:
    LogLevel minLevel_;
    bool enabled_ = true;
    
public:
    LevelFilter(LogLevel level) : minLevel_(level) {}
    
    bool shouldLog(const LogEntry& entry) const override {
        return enabled_ && entry.level >= minLevel_;
    }
    
    void setEnabled(bool enabled) override { enabled_ = enabled; }
    bool isEnabled() const override { return enabled_; }
    
    void setLevel(LogLevel level) { minLevel_ = level; }
};

class ModuleFilter : public ILogFilter {
private:
    std::string module_;
    bool enabled_ = true;
    
public:
    ModuleFilter(const std::string& module) : module_(module) {}
    
    bool shouldLog(const LogEntry& entry) const override {
        return enabled_ && (entry.module == module_ || module_.empty());
    }
    
    void setEnabled(bool enabled) override { enabled_ = enabled; }
    bool isEnabled() const override { return enabled_; }
    
    void setModule(const std::string& module) { module_ = module; }
};

class TagFilter : public ILogFilter {
private:
    std::unordered_set<std::string> requiredTags_;
    std::unordered_set<std::string> excludedTags_;
    bool enabled_ = true;
    
public:
    TagFilter() = default;
    
    TagFilter(const std::vector<std::string>& required, const std::vector<std::string>& excluded = {}) {
        requiredTags_ = std::unordered_set<std::string>(required.begin(), required.end());
        excludedTags_ = std::unordered_set<std::string>(excluded.begin(), excluded.end());
    }
    
    bool shouldLog(const LogEntry& entry) const override {
        if (!enabled_) return false;
        
        // Check required tags
        for (const auto& tag : requiredTags_) {
            if (entry.tags.find(tag) == entry.tags.end()) {
                return false;
            }
        }
        
        // Check excluded tags
        for (const auto& tag : excludedTags_) {
            if (entry.tags.find(tag) != entry.tags.end()) {
                return false;
            }
        }
        
        return true;
    }
    
    void setEnabled(bool enabled) override { enabled_ = enabled; }
    bool isEnabled() const override { return enabled_; }
    
    void addRequiredTag(const std::string& tag) { requiredTags_.insert(tag); }
    void addExcludedTag(const std::string& tag) { excludedTags_.insert(tag); }
};

// Performance metrics
struct LogMetrics {
    std::atomic<uint64_t> logCount{0};
    std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point lastLogTime;
    std::atomic<uint64_t> totalLatencyNanos{0};
    
    void recordLog(std::chrono::nanoseconds latency) {
        logCount++;
        lastLogTime = std::chrono::steady_clock::now();
        totalLatencyNanos += latency.count();
    }
    
    double getAverageLatency() const {
        return logCount > 0 ? (totalLatencyNanos / logCount) / 1e6 : 0.0;
    }
    
    uint64_t getLogCount() const { return logCount; }
    std::chrono::steady_clock::time_point getLastLogTime() const { return lastLogTime; }
};

// Asynchronous logger
class AsyncLogger {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
public:
    AsyncLogger(size_t queueSize = 10000, size_t workerThreads = 2);
    ~AsyncLogger();
    
    void log(const LogEntry& entry);
    void logBatch(const std::vector<LogEntry>& entries);
    void shutdown();
    bool isShutdown() const;
    
    void setLevel(LogLevel level);
    LogLevel getLevel() const;
    void setPattern(const std::string& pattern);
    std::string getPattern() const;
    
    uint64_t getQueueSize() const;
    uint64_t getProcessedCount() const;
    double getAverageLatency() const;
};

// Main Logger class with enhanced features
class Logger {
public:
    Logger(const std::string& name = "default");
    ~Logger();
    
    // Disable copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // Logger configuration
    void setName(const std::string& name);
    std::string getName() const;
    
    // Core logging methods with optimized locking
    void log(LogLevel level, const std::string& message,
             const std::string& file = "", int line = 0,
             const std::string& function = "", const std::string& module = "");
    
    void log(const LogEntry& entry);
    
    // Structured logging with key-value pairs
    template<typename... Args>
    void logStructured(LogLevel level, const std::string& message,
                       const std::string& file = "", int line = 0,
                       const std::string& function = "", const std::string& module = "",
                       const std::unordered_map<std::string, std::string>& fields = {});
    
    // Batch logging for performance
    void logBatch(const std::vector<LogEntry>& entries);
    
    // Convenience methods
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& module = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& module = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& module = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& module = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& module = "");
    
    // Enhanced formatting
    void setPattern(const std::string& pattern);
    std::string getPattern() const;
    void setFormatter(std::shared_ptr<LogFormatter> formatter);
    std::shared_ptr<LogFormatter> getFormatter() const;
    
    // Filtering
    void setFilter(std::shared_ptr<ILogFilter> filter);
    std::shared_ptr<ILogFilter> getFilter() const;
    
    // Multi-appender support
    void addAppender(std::shared_ptr<LogAppender> appender);
    void removeAppender(const std::string& name);
    void clearAppenders();
    void setAppenders(const std::vector<std::shared_ptr<LogAppender>>& appenders);
    
    // Performance and control
    void setLevel(LogLevel level);
    LogLevel getLevel() const;
    void setFlushOnEachLog(bool flush);
    bool getFlushOnEachLog() const;
    
    // Log level filtering
    bool isEnabled(LogLevel level) const;
    
    // Performance monitoring
    LogMetrics getMetrics() const;
    void resetMetrics();
    
    // Global logger management
    static Logger& getGlobal(const std::string& name = "global");
    static void setGlobal(const std::string& name, std::shared_ptr<Logger> logger);
    static void clearGlobal(const std::string& name = "global");
    
    // Stream-style logging
    class LogStream;
    LogStream logStream(LogLevel level, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& module = "");
    
    // Configuration loading
    static std::shared_ptr<Logger> fromConfig(const std::string& configFile);
    
    // Memory management
    static void cleanupPools();
    static size_t getEntryPoolSize();
    static size_t getStringPoolSize();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Convenience macro for file/line/function info
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
    
    // Structured logging
    LogStream& addField(const std::string& key, const std::string& value);
    LogStream& addTag(const std::string& tag);
    LogStream& setModule(const std::string& module);
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
    std::string module_;
    std::unordered_map<std::string, std::string> fields_;
    std::unordered_set<std::string> tags_;
};

#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

} // namespace nlm
