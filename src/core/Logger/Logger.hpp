#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <mutex>
#include <shared_mutex>
#include <array>
#include <atomic>
#include <thread>
#include <filesystem>

namespace nlm {

// Custom exceptions for logging errors
class LogException : public std::runtime_error {
public:
    explicit LogException(const std::string& message) : std::runtime_error(message) {}
};

class LogStreamException : public LogException {
public:
    explicit LogStreamException(const std::string& message) : LogException(message) {}
};

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
    std::atomic<uint64_t> sequenceNumber{0};
    
    LogEntry() : level(LogLevel::Info), line(0), sequenceNumber(0) {
        timestamp = std::chrono::system_clock::now();
    }
};

// Logger interface
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    virtual void flush() = 0;
    virtual bool isAsync() const { return false; }
    virtual void enableAsyncLogging() {}
    virtual void closeOutputFile() {}
};

// Async log queue for performance
class AsyncLogQueue {
public:
    AsyncLogQueue(size_t maxSize = 10000, size_t workerThreads = 1);
    ~AsyncLogQueue();
    
    void start();
    void shutdown();
    void push(std::function<void()> callback);
    bool pop(std::function<void()>& callback);
    size_t size() const;
    bool isRunning() const;
    
private:
    void workerThread();
    
    size_t maxSize_;
    bool running_;
    std::mutex mutex_;
    std::condition_variable condition_;
    std::queue<std::pair<std::function<void()>, uint64_t>> queue_;
    std::vector<std::thread> workers_;
    std::atomic<uint64_t> nextSequence_{0};
};

// Enhanced ConsoleLogger implementation
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(LogLevel level);
    ~ConsoleLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    void flush() override;
    bool isAsync() const override;
    void setUseColors(bool useColors);
    bool getUseColors() const;
    void setOutputFile(const std::string& filepath, size_t maxFileSize = 10 * 1024 * 1024,
                       int maxFiles = 5);
    void closeOutputFile() override;
    void enableAsyncLogging(size_t queueSize = 10000, size_t workerThreads = 2) override;
    
    // Performance monitoring
    struct PerformanceStats {
        uint64_t totalLogs{0};
        uint64_t droppedLogs{0};
        std::chrono::steady_clock::time_point startTime;
        PerformanceStats() : startTime(std::chrono::steady_clock::now()) {}
    };
    
    PerformanceStats getPerformanceStats() const;
    
private:
    void setupConsoleColors();
    std::string formatEntry(const LogEntry& entry) const;
    void outputToConsole(const std::string& message);
    void rotateFilesIfNeeded();
    void rotateFiles();
    LogLevel getLevelForMessage(const std::string& message) const;
    void shutdownAsyncLogger();
    
    LogLevel level_;
    bool useColors_;
    bool async_;
    mutable std::mutex levelMutex_;
    mutable std::mutex colorMutex_;
    mutable std::mutex consoleMutex_;
    
    std::array<std::string, 5> colorCodes_;
    std::string resetCode_;
    
    struct FileConfig {
        std::string filepath;
        std::ofstream fileOut;
        size_t maxFileSize;
        int maxFiles;
        size_t currentSize;
        int rotationCount;
        FileConfig() : maxFileSize(10 * 1024 * 1024), maxFiles(5), 
                       currentSize(0), rotationCount(0) {}
    } fileConfig_;
    mutable std::mutex fileMutex_;
    
    AsyncLogQueue asyncQueue_;
    mutable std::mutex asyncMutex_;
};

// Main Logger class with enhanced features
class Logger {
public:
    Logger();
    ~Logger();
    
    // Disable copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // Add logger sink with thread-safe registration
    void addLogger(std::shared_ptr<ILogger> logger);
    bool removeLogger(std::shared_ptr<ILogger> logger);
    void clearLoggers();
    
    // Set minimum level for all loggers
    void setLevel(LogLevel level);
    
    // Global logging functions with enhanced error handling
    void log(LogLevel level, const std::string& message,
             const std::string& file = "", int line = 0,
             const std::string& function = "");
    
    // Enhanced convenience methods with context
    void debug(const std::string& message, const std::string& file = "",
               int line = 0, const std::string& function = "");
    void info(const std::string& message, const std::string& file = "",
              int line = 0, const std::string& function = "");
    void warning(const std::string& message, const std::string& file = "",
                 int line = 0, const std::string& function = "");
    void error(const std::string& message, const std::string& file = "",
               int line = 0, const std::string& function = "");
    void critical(const std::string& message, const std::string& file = "",
                  int line = 0, const std::string& function = "");
    
    // Stream-style logging with more context
    class LogStream {
    public:
        LogStream(LogLevel level, const std::string& file, int line,
                  const std::string& function, Logger& logger);
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
        Logger& logger_;
    };
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Async logging control
    void enableAsyncLogging(size_t queueSize = 10000, size_t workerThreads = 2);
    void disableAsyncLogging();
    bool isAsyncEnabled() const;
    
    // Performance monitoring
    struct LoggerPerformanceStats {
        uint64_t totalLogs{0};
        uint64_t droppedLogs{0};
        size_t currentQueueSize{0};
        std::chrono::steady_clock::time_point startTime;
        LoggerPerformanceStats() : startTime(std::chrono::steady_clock::now()) {}
    };
    
    LoggerPerformanceStats getPerformanceStats() const;
    
    // Configuration presets
    static void setupDevelopmentMode();
    static void setupProductionMode();
    static void setupFileOnlyMode(const std::string& logFile);
    
    // Cleanup
    void shutdown();
    
    // Log stream interface
    LogStream logStream(LogLevel level, const std::string& file, 
                        int line, const std::string& function);
    
private:
    void setupDefaultLoggers();
    void shutdownAsyncLogger();
    
    std::vector<std::shared_ptr<ILogger>> loggers_;
    LogLevel currentLevel_;
    
    // Thread safety
    mutable std::shared_mutex loggersMutex_;
    std::mutex levelMutex_;
    std::mutex globalMutex_;
    std::mutex asyncMutex_;
    
    // Async logging
    bool async_;
    AsyncLogQueue asyncQueue_;
    
    // Global instance
    static std::shared_ptr<Logger> globalInstance_;
    static std::mutex globalMutex_;
    
    // Helper functions
    std::string levelToString(LogLevel level) const;
};

// Convenience macro support
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().log(
    nlm::LogLevel::Debug, msg, __FILE__, __LINE__, __func__)

#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().log(
    nlm::LogLevel::Info, msg, __FILE__, __LINE__, __func__)

#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().log(
    nlm::LogLevel::Warning, msg, __FILE__, __LINE__, __func__)

#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().log(
    nlm::LogLevel::Error, msg, __FILE__, __LINE__, __func__)

#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().log(
    nlm::LogLevel::Critical, msg, __FILE__, __LINE__, __func__)

#define NLM_LOG_STREAM(level) nlm::Logger::getGlobal().logStream(level, __FILE__, __LINE__, __func__)