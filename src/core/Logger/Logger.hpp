#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <mutex>

namespace nlm {

// Log severity levels
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4
};

// Log category for structured logging
enum class LogCategory {
    System = 0,
    Neural = 1,
    Sensory = 2,
    Motor = 3,
    Learning = 4,
    Memory = 5,
    World = 6,
    Performance = 7,
    Debug = 8,
    User = 9
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
    uint64_t threadId;
    uint64_t simulationStep;
    
    LogEntry() : level(LogLevel::Info), category(LogCategory::System), line(0), 
                 threadId(0), simulationStep(0) {}
};

// Log filter interface
class ILogFilter {
public:
    virtual ~ILogFilter() = default;
    virtual bool shouldLog(const LogEntry& entry) const = 0;
};

// Simple log filter based on level and category
class SimpleLogFilter : public ILogFilter {
public:
    SimpleLogFilter(LogLevel minLevel = LogLevel::Debug, 
                   const std::vector<LogCategory>& categories = {})
        : minLevel_(minLevel), categories_(categories) {}
    
    bool shouldLog(const LogEntry& entry) const override {
        if (entry.level < minLevel_) return false;
        if (!categories_.empty() && 
            std::find(categories_.begin(), categories_.end(), entry.category) == categories_.end()) {
            return false;
        }
        return true;
    }
    
    void setMinLevel(LogLevel level) { minLevel_ = level; }
    void addCategory(LogCategory category) { categories_.push_back(category); }
    void clearCategories() { categories_.clear(); }
    
private:
    LogLevel minLevel_;
    std::vector<LogCategory> categories_;
};

// Log sink interface
class ILogSink {
public:
    virtual ~ILogSink() = default;
    virtual void write(const LogEntry& entry) = 0;
    virtual void flush() = 0;
    virtual void setFilter(std::shared_ptr<ILogFilter> filter) {
        filter_ = filter;
    }
    virtual std::shared_ptr<ILogFilter> getFilter() const { return filter_; }
    
protected:
    std::shared_ptr<ILogFilter> filter_;
};

// Console logger implementation with colors
class ConsoleLogger : public ILogSink {
public:
    ConsoleLogger();
    explicit ConsoleLogger(LogLevel level);
    ~ConsoleLogger() override;
    
    void write(const LogEntry& entry) override;
    void flush() override;
    
    // Enable/disable colors
    void setUseColors(bool useColors);
    bool getUseColors() const;
    
    // Set output file (in addition to console)
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    
    // Control formatting
    void setFormat(const std::string& format);
    std::string getFormat() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string categoryToString(LogCategory category) const;
    std::string levelToColor(LogLevel level) const;
};

// File logger implementation with rotation
class FileLogger : public ILogSink {
public:
    FileLogger();
    explicit FileLogger(LogLevel level);
    ~FileLogger() override;
    
    void write(const LogEntry& entry) override;
    void flush() override;
    
    // File rotation settings
    void setFilePath(const std::string& filepath);
    void setMaxFileSize(size_t maxSize); // in bytes
    void setMaxFiles(int maxFiles);
    void enableRotation(bool enable);
    
    // Format settings
    void setFormat(const std::string& format);
    void setTimestampFormat(const std::string& format);
    
    // Buffer management
    void setBufferSize(size_t bufferSize);
    size_t getBufferSize() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Memory logger for log capture and analysis
class MemoryLogger : public ILogSink {
public:
    MemoryLogger();
    explicit MemoryLogger(LogLevel level);
    ~MemoryLogger() override;
    
    void write(const LogEntry& entry) override;
    void flush() override;
    
    // Get recent logs
    std::vector<LogEntry> getRecentLogs(size_t count = 100) const;
    std::vector<LogEntry> getLogsByLevel(LogLevel level) const;
    std::vector<LogEntry> getLogsByCategory(LogCategory category) const;
    
    // Log search
    std::vector<LogEntry> search(const std::string& pattern) const;
    
    // Clear logs
    void clear();
    size_t size() const;
    
    // Export
    std::string exportAsJSON() const;
    std::string exportAsCSV() const;
    
    // Performance stats
    struct Stats {
        size_t totalWritten = 0;
        size_t memoryUsage = 0;
        double averageWriteTime = 0.0;
        std::chrono::system_clock::time_point firstEntry;
        std::chrono::system_clock::time_point lastEntry;
    };
    
    Stats getStats() const;
    
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
    
    // Add log sink
    void addSink(std::shared_ptr<ILogSink> sink);
    
    // Remove all sinks
    void clearSinks();
    
    // Get sinks
    std::vector<std::shared_ptr<ILogSink>> getSinks() const;
    
    // Set minimum level for all sinks
    void setLevel(LogLevel level);
    
    // Set global filter
    void setFilter(std::shared_ptr<ILogFilter> filter);
    
    // Global logging functions
    void log(LogLevel level, LogCategory category, const std::string& message,
             const std::string& file = "", int line = 0,
             const std::string& function = "", uint64_t simulationStep = 0);
    
    // Convenience methods
    void debug(LogCategory category, const std::string& message,
              const std::string& file = "", int line = 0,
              const std::string& function = "", uint64_t simulationStep = 0);
    void info(LogCategory category, const std::string& message,
             const std::string& file = "", int line = 0,
             const std::string& function = "", uint64_t simulationStep = 0);
    void warning(LogCategory category, const std::string& message,
                const std::string& file = "", int line = 0,
                const std::string& function = "", uint64_t simulationStep = 0);
    void error(LogCategory category, const std::string& message,
              const std::string& file = "", int line = 0,
              const std::string& function = "", uint64_t simulationStep = 0);
    void critical(LogCategory category, const std::string& message,
                 const std::string& file = "", int line = 0,
                 const std::string& function = "", uint64_t simulationStep = 0);
    
    // Check if level is enabled
    bool isEnabled(LogLevel level, LogCategory category = LogCategory::System) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Performance monitoring
    struct PerformanceStats {
        size_t logCount = 0;
        double processingTime = 0.0;
        size_t memoryUsage = 0;
        uint64_t peakThreads = 0;
    };
    
    PerformanceStats getPerformanceStats() const;
    
    void resetPerformanceStats();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Convenience macro for structured logging
#define NLM_LOG_DEBUG_CAT(category, msg) nlm::Logger::getGlobal().debug(category, msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO_CAT(category, msg) nlm::Logger::getGlobal().info(category, msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING_CAT(category, msg) nlm::Logger::getGlobal().warning(category, msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR_CAT(category, msg) nlm::Logger::getGlobal().error(category, msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL_CAT(category, msg) nlm::Logger::getGlobal().critical(category, msg, __FILE__, __LINE__, __func__)

// System category convenience macros
#define NLM_LOG_SYSTEM_DEBUG(msg) NLM_LOG_DEBUG_CAT(nlm::LogCategory::System, msg)
#define NLM_LOG_SYSTEM_INFO(msg) NLM_LOG_INFO_CAT(nlm::LogCategory::System, msg)
#define NLM_LOG_SYSTEM_WARNING(msg) NLM_LOG_WARNING_CAT(nlm::LogCategory::System, msg)
#define NLM_LOG_SYSTEM_ERROR(msg) NLM_LOG_ERROR_CAT(nlm::LogCategory::System, msg)
#define NLM_LOG_SYSTEM_CRITICAL(msg) NLM_LOG_CRITICAL_CAT(nlm::LogCategory::System, msg)

// Neural category convenience macros
#define NLM_LOG_NEURAL_DEBUG(msg) NLM_LOG_DEBUG_CAT(nlm::LogCategory::Neural, msg)
#define NLM_LOG_NEURAL_INFO(msg) NLM_LOG_INFO_CAT(nlm::LogCategory::Neural, msg)
#define NLM_LOG_NEURAL_WARNING(msg) NLM_LOG_WARNING_CAT(nlm::LogCategory::Neural, msg)

// Performance logging
#define NLM_PERF_START(name) auto name##_start = std::chrono::high_resolution_clock::now()
#define NLM_PERF_END(name) do { \
    auto name##_end = std::chrono::high_resolution_clock::now(); \
    auto name##_duration = std::chrono::duration_cast<std::chrono::microseconds>(name##_end - name##_start); \
    NLM_LOG_INFO_CAT(nlm::LogCategory::Performance, "Performance: " #name " took " + std::to_string(name##_duration.count()) + " μs");
} while(0)

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
    
private:
    LogLevel level_;
    LogCategory category_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
};

#define NLM_LOG_STREAM_CAT(level, category) nlm::LogStream(level, category, __FILE__, __LINE__, __func__)
#define NLM_LOG_STREAM(level) nlm::LogStream(level, nlm::LogCategory::System, __FILE__, __LINE__, __func__)

} // namespace nlm
