#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
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

// Log entry structure
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::unordered_map<std::string, std::string> metadata;
};

// Logger interface
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    
    // Flush any buffered logs
    virtual void flush() {}
    
    // Enable/disable logging
    virtual void setEnabled(bool enabled) {}
    virtual bool isEnabled() const { return true; }
};

// File logger implementation
class FileLogger : public ILogger {
public:
    FileLogger(const std::string& filepath);
    ~FileLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    void setFilepath(const std::string& filepath);
    void setMaxFileSize(size_t maxSize);
    void setMaxFiles(size_t maxFiles);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
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
    
    // Enable/disable colors
    void setUseColors(bool useColors);
    bool getUseColors() const;
    
    // Set output file (in addition to console)
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    
    // Control output formatting
    void setFormat(const std::string& format);
    std::string getFormat() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
};

// Buffered logger for performance
class BufferedLogger : public ILogger {
public:
    BufferedLogger(size_t maxBufferSize = 1000);
    ~BufferedLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    void flush() override;
    void setFlushInterval(std::chrono::milliseconds interval);
    
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
    
    // Set global logging flags
    void setEnabled(bool enabled);
    bool isEnabled() const;
    
    // Global logging functions with enhanced features
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "", 
             const std::unordered_map<std::string, std::string>& metadata = {});
    
    // Convenience methods with context
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", 
               const std::unordered_map<std::string, std::string>& metadata = {});
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", 
              const std::unordered_map<std::string, std::string>& metadata = {});
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", 
                 const std::unordered_map<std::string, std::string>& metadata = {});
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", 
               const std::unordered_map<std::string, std::string>& metadata = {});
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", 
                  const std::unordered_map<std::string, std::string>& metadata = {});
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Configure logging from environment variables
    static void configureFromEnv();
    
    // Get log statistics
    struct LogStats {
        size_t totalLogs = 0;
        std::unordered_map<LogLevel, size_t> logsByLevel;
        std::chrono::system_clock::time_point startTime;
        std::chrono::system_clock::time_point endTime;
        
        double getUptimeSeconds() const;
    };
    
    LogStats getLogStats() const;
    
    // Reset statistics
    void resetStats();
    
    // Log system performance metrics
    void logPerformanceMetrics() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced stream-style logging helper with metadata
class LogStream {
public:
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function,
              const std::unordered_map<std::string, std::string>& metadata = {});
    ~LogStream();
    
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    // Add metadata to the log entry
    LogStream& withMetadata(const std::string& key, const std::string& value);
    
    // Set context information
    LogStream& context(const std::string& operation, const std::string& component);
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
    std::unordered_map<std::string, std::string> metadata_;
};

// Enhanced macros for different logging scenarios
#define NLM_LOG_TRACE(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_PERF(operation, duration_ms) nlm::Logger::getGlobal().info(
    "Performance: " + std::string(operation) + " took " + std::to_string(duration_ms) + "ms",
    __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR_WITH_CONTEXT(msg, operation, component) { \
    auto contextStream = nlm::LogStream(nlm::LogLevel::Error, __FILE__, __LINE__, __func__); \
    contextStream.context(operation, component) << (msg);
}

// Smart logging utility
namespace nlm {
    // RAII class for timing operations
    class Timer {
    public:
        Timer(const std::string& operation, const std::string& component = "");
        ~Timer();
        
        void stop();
        double getDurationMs() const;
        
    private:
        std::string operation_;
        std::string component_;
        std::chrono::steady_clock::time_point startTime_;
        bool stopped_;
        double durationMs_;
    };
    
    // Memory tracking utility
    class MemoryTracker {
    public:
        void track(const std::string& label, size_t bytes);
        void report() const;
        void reset();
        
    private:
        struct Allocation {
            std::string label;
            size_t bytes;
            std::chrono::system_clock::time_point timestamp;
        };
        std::vector<Allocation> allocations_;
    };
}

// Macro for timed operations
#define NLM_TIME_OPERATION(operation) \
    nlm::Timer timer(operation, __func__); \
    try { operation; } catch (...) { timer.stop(); throw; }

// Macro for memory tracking
#define NLM_TRACK_MEMORY(label, bytes) nlm::MemoryTracker::getInstance().track(label, bytes)

} // namespace nlm
