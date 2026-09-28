#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <mutex>
#include <shared_mutex>
#include <fstream>
#include <iomanip>
#include <queue>
#include <thread>
#include <condition_variable>
#include <atomic>

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
};

// Async log entry
struct AsyncLogEntry {
    std::function<void()> callback;
    std::chrono::system_clock::time_point timestamp;
    uint64_t sequenceNumber;
    
    AsyncLogEntry(std::function<void()> cb, uint64_t seq)
        : callback(std::move(cb)), timestamp(std::chrono::system_clock::now()), 
          sequenceNumber(seq) {}
};

// Thread-safe priority queue for async logging
class AsyncLogQueue {
public:
    AsyncLogQueue(size_t maxSize = 10000, size_t workerThreads = 1)
        : maxSize_(maxSize), running_(false) {
        for (size_t i = 0; i < workerThreads; ++i) {
            workers_.emplace_back(&AsyncLogQueue::workerThread, this);
        }
    }
    
    ~AsyncLogQueue() {
        shutdown();
    }
    
    void start() {
        std::lock_guard<std::mutex> lock(mutex_);
        running_ = true;
        condition_.notify_all();
    }
    
    void shutdown() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            running_ = false;
        }
        condition_.notify_all();
        
        for (auto& worker : workers_) {
            if (worker.joinable()) {
                worker.join();
            }
        }
    }
    
    void push(std::function<void()> callback) {
        std::unique_lock<std::mutex> lock(mutex_);
        condition_.wait(lock, [this]() { 
            return running_ && queue_.size() < maxSize_; 
        });
        
        uint64_t seqNum = nextSequence_++;
        queue_.emplace(std::move(callback), seqNum);
        condition_.notify_one();
    }
    
    bool pop(std::function<void()>& callback) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (!running_ || queue_.empty()) {
            return false;
        }
        
        callback = std::move(queue_.front().callback);
        queue_.pop();
        condition_.notify_all();
        return true;
    }
    
    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size();
    }
    
    bool isRunning() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return running_;
    }
    
private:
    void workerThread() {
        std::function<void()> callback;
        while (isRunning()) {
            if (pop(callback)) {
                try {
                    callback();
                } catch (const std::exception& e) {
                    // Silently handle exceptions in callbacks
                    std::cerr << "Error in async log callback: " << e.what() << std::endl;
                }
            }
            std::this_thread::yield(); // Yield to allow other threads to run
        }
    }
    
    size_t maxSize_;
    bool running_;
    std::mutex mutex_;
    std::condition_variable condition_;
    std::queue<AsyncLogEntry> queue_;
    std::vector<std::thread> workers_;
    std::atomic<uint64_t> nextSequence_{0};
};

// Enhanced ConsoleLogger implementation
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger() : ConsoleLogger(LogLevel::Info) {}
    
    explicit ConsoleLogger(LogLevel level) : level_(level), useColors_(true), async_(false) {
        setupConsoleColors();
    }
    
    ~ConsoleLogger() override {
        shutdownAsyncLogger();
    }
    
    void log(const LogEntry& entry) override {
        if (entry.level < level_) {
            return;
        }
        
        std::string formattedMessage = formatEntry(entry);
        
        if (async_) {
            asyncQueue_.push([this, formattedMessage]() {
                outputToConsole(formattedMessage);
            });
        } else {
            outputToConsole(formattedMessage);
        }
    }
    
    void setLevel(LogLevel level) override {
        std::lock_guard<std::mutex> lock(levelMutex_);
        level_ = level;
    }
    
    LogLevel getLevel() const override {
        std::lock_guard<std::mutex> lock(levelMutex_);
        return level_;
    }
    
    void flush() override {
        if (async_) {
            // Wait for queue to empty with timeout
            auto start = std::chrono::steady_clock::now();
            while (asyncQueue_.size() > 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                auto now = std::chrono::steady_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
                if (elapsed.count() > 1000) { // 1 second timeout
                    break;
                }
            }
        }
    }
    
    void setUseColors(bool useColors) override {
        std::lock_guard<std::mutex> lock(colorMutex_);
        useColors_ = useColors;
    }
    
    bool getUseColors() const override {
        std::lock_guard<std::mutex> lock(colorMutex_);
        return useColors_;
    }
    
    // Enhanced file logging with rotation
    void setOutputFile(const std::string& filepath, size_t maxFileSize = 10 * 1024 * 1024,
                       int maxFiles = 5) {
        std::lock_guard<std::mutex> lock(fileMutex_);
        closeOutputFile();
        
        fileConfig_.filepath = filepath;
        fileConfig_.maxFileSize = maxFileSize;
        fileConfig_.maxFiles = maxFiles;
        fileConfig_.currentSize = 0;
        fileConfig_.rotationCount = 0;
        
        rotateFilesIfNeeded();
        fileConfig_.fileOut.open(filepath, std::ios::app);
    }
    
    void closeOutputFile() override {
        std::lock_guard<std::mutex> lock(fileMutex_);
        if (fileConfig_.fileOut.is_open()) {
            fileConfig_.fileOut.close();
        }
    }
    
    // Async logging control
    void enableAsyncLogging(size_t queueSize = 10000, size_t workerThreads = 2) {
        std::lock_guard<std::mutex> lock(asyncMutex_);
        if (!async_) {
            async_ = true;
            asyncQueue_ = AsyncLogQueue(queueSize, workerThreads);
            asyncQueue_.start();
        }
    }
    
    void disableAsyncLogging() {
        std::lock_guard<std::mutex> lock(asyncMutex_);
        if (async_) {
            shutdownAsyncLogger();
            async_ = false;
        }
    }
    
    bool isAsync() const override {
        return async_;
    }
    
    // Performance monitoring
    struct PerformanceStats {
        uint64_t totalLogs{0};
        uint64_t droppedLogs{0};
        std::chrono::steady_clock::time_point startTime;
        
        PerformanceStats() : startTime(std::chrono::steady_clock::now()) {}
    };
    
    PerformanceStats getPerformanceStats() const {
        PerformanceStats stats;
        // Implementation would track actual performance metrics
        return stats;
    }
    
private:
    // Thread-safe color setup
    void setupConsoleColors() {
        // Store ANSI codes for colors
        colorCodes_[LogLevel::Debug] = "\033[36m";    // Cyan
        colorCodes_[LogLevel::Info] = "\033[32m";      // Green
        colorCodes_[LogLevel::Warning] = "\033[33m";   // Yellow
        colorCodes_[LogLevel::Error] = "\033[31m";     // Red
        colorCodes_[LogLevel::Critical] = "\033[35m"; // Magenta
        resetCode_ = "\033[0m";
    }
    
    // Format log entry with enhanced information
    std::string formatEntry(const LogEntry& entry) {
        std::ostringstream oss;
        
        oss << "[" << levelToString(entry.level) << "] ";
        
        // Add timestamp with milliseconds
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            entry.timestamp.time_since_epoch()) % 1000;
        oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        oss << "." << std::setfill('0') << std::setw(3) << ms.count();
        
        // Add thread ID and sequence number for debugging
        oss << " [" << std::this_thread::get_id() << "]";
        oss << " [" << entry.sequenceNumber << "]";
        
        // Add file and line if available
        if (!entry.file.empty()) {
            oss << " " << entry.file;
            if (entry.line > 0) {
                oss << ":" << entry.line;
            }
        }
        
        oss << " " << entry.message;
        
        return oss.str();
    }
    
    // Output to console with color support
    void outputToConsole(const std::string& message) {
        std::lock_guard<std::mutex> lock(consoleMutex_);
        
        if (useColors_) {
            std::cout << colorCodes_[getLevelForMessage(message)] << message << resetCode_ << "\n";
        } else {
            std::cout << message << "\n";
        }
        std::cout << std::flush;
    }
    
    // File rotation logic
    void rotateFilesIfNeeded() {
        if (!std::filesystem::exists(fileConfig_.filepath)) {
            return;
        }
        
        auto fileSize = std::filesystem::file_size(fileConfig_.filepath);
        if (fileSize >= fileConfig_.maxFileSize) {
            rotateFiles();
        }
    }
    
    void rotateFiles() {
        for (int i = fileConfig_.maxFiles - 1; i >= 0; --i) {
            std::string from = (i == 0) ? fileConfig_.filepath : 
                               fileConfig_.filepath + "." + std::to_string(i);
            std::string to = fileConfig_.filepath + "." + std::to_string(i + 1);
            
            if (std::filesystem::exists(from)) {
                std::filesystem::rename(from, to);
            }
        }
        
        fileConfig_.rotationCount++;
        fileConfig_.fileOut.open(fileConfig_.filepath, std::ios::trunc);
    }
    
    // Determine log level from formatted message
    LogLevel getLevelForMessage(const std::string& message) {
        // Simple heuristic: check for level indicators in message
        if (message.find("[DEBUG]") != std::string::npos) return LogLevel::Debug;
        if (message.find("[INFO]") != std::string::npos) return LogLevel::Info;
        if (message.find("[WARN]") != std::string::npos) return LogLevel::Warning;
        if (message.find("[ERROR]") != std::string::npos) return LogLevel::Error;
        if (message.find("[CRIT]") != std::string::npos) return LogLevel::Critical;
        return LogLevel::Info; // Default
    }
    
    void shutdownAsyncLogger() {
        asyncQueue_.shutdown();
    }
    
    // Member variables
    LogLevel level_;
    bool useColors_;
    bool async_;
    
    // Thread safety
    std::mutex levelMutex_;
    std::mutex colorMutex_;
    std::mutex fileMutex_;
    std::mutex asyncMutex_;
    std::mutex consoleMutex_;
    
    // Console colors
    std::array<std::string, 5> colorCodes_;
    std::string resetCode_;
    
    // File logging configuration
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
    
    AsyncLogQueue asyncQueue_;
};

// Main Logger class with enhanced features
class Logger {
public:
    Logger() : globalInstance_(this), async_(false) {
        setupDefaultLoggers();
    }
    
    ~Logger() {
        shutdownAsyncLogger();
    }
    
    // Disable copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // Add logger sink with thread-safe registration
    void addLogger(std::shared_ptr<ILogger> logger) {
        if (!logger) {
            throw LogException("Cannot add null logger");
        }
        
        std::lock_guard<std::shared_mutex> lock(loggersMutex_);
        loggers_.push_back(logger);
        
        // Set initial level for new logger
        logger->setLevel(currentLevel_);
        
        // Enable async if global async is enabled
        if (async_) {
            logger->enableAsyncLogging();
        }
    }
    
    // Remove specific logger
    bool removeLogger(std::shared_ptr<ILogger> logger) {
        std::lock_guard<std::shared_mutex> lock(loggersMutex_);
        auto it = std::find(loggers_.begin(), loggers_.end(), logger);
        if (it != loggers_.end()) {
            loggers_.erase(it);
            return true;
        }
        return false;
    }
    
    // Remove all loggers
    void clearLoggers() {
        std::lock_guard<std::shared_mutex> lock(loggersMutex_);
        for (auto& logger : loggers_) {
            logger->closeOutputFile();
        }
        loggers_.clear();
    }
    
    // Set minimum level for all loggers
    void setLevel(LogLevel level) {
        std::lock_guard<std::shared_mutex> lock(levelMutex_);
        currentLevel_ = level;
    }
    
    // Global logging functions with enhanced error handling
    void log(LogLevel level, const std::string& message,
             const std::string& file = "", int line = 0,
             const std::string& function = "") {
        LogEntry entry;
        entry.level = level;
        entry.message = message;
        entry.file = file;
        entry.line = line;
        entry.function = function;
        
        // Thread-safe logging
        std::shared_lock<std::shared_mutex> lock(loggersMutex_);
        for (const auto& logger : loggers_) {
            try {
                logger->log(entry);
            } catch (const std::exception& e) {
                // Log error to stderr but don't crash
                std::cerr << "Logger error: " << e.what() << std::endl;
            }
        }
    }
    
    // Enhanced convenience methods with context
    void debug(const std::string& message, const std::string& file = "",
               int line = 0, const std::string& function = "") {
        log(LogLevel::Debug, message, file, line, function);
    }
    
    void info(const std::string& message, const std::string& file = "",
              int line = 0, const std::string& function = "") {
        log(LogLevel::Info, message, file, line, function);
    }
    
    void warning(const std::string& message, const std::string& file = "",
                 int line = 0, const std::string& function = "") {
        log(LogLevel::Warning, message, file, line, function);
    }
    
    void error(const std::string& message, const std::string& file = "",
               int line = 0, const std::string& function = "") {
        log(LogLevel::Error, message, file, line, function);
    }
    
    void critical(const std::string& message, const std::string& file = "",
                  int line = 0, const std::string& function = "") {
        log(LogLevel::Critical, message, file, line, function);
    }
    
    // Stream-style logging with more context
    class LogStream {
    public:
        LogStream(LogLevel level, const std::string& file, int line,
                  const std::string& function, Logger& logger)
            : level_(level), file_(file), line_(line), 
              function_(function), logger_(logger) {}
        
        ~LogStream() {
            logger_.log(level_, oss_.str(), file_, line_, function_);
        }
        
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
    
    // Enhanced macro support
    #define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().log(\
        nlm::LogLevel::Debug, msg, __FILE__, __LINE__, __func__)
    
    #define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().log(\
        nlm::LogLevel::Info, msg, __FILE__, __LINE__, __func__)
    
    #define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().log(\
        nlm::LogLevel::Warning, msg, __FILE__, __LINE__, __func__)
    
    #define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().log(\
        nlm::LogLevel::Error, msg, __FILE__, __LINE__, __func__)
    
    #define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().log(\
        nlm::LogLevel::Critical, msg, __FILE__, __LINE__, __func__)
    
    #define NLM_LOG_STREAM(level) nlm::Logger::getGlobal().logStream(level, __FILE__, __LINE__, __func__)
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const {
        std::shared_lock<std::shared_mutex> lock(levelMutex_);
        return level >= currentLevel_;
    }
    
    // Get global logger instance
    static Logger& getGlobal() {
        return *globalInstance_;
    }
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger) {
        std::lock_guard<std::mutex> lock(globalMutex_);
        globalInstance_ = logger;
    }
    
    // Async logging control
    void enableAsyncLogging(size_t queueSize = 10000, size_t workerThreads = 2) {
        std::lock_guard<std::mutex> lock(asyncMutex_);
        if (!async_) {
            async_ = true;
            asyncQueue_ = AsyncLogQueue(queueSize, workerThreads);
            asyncQueue_.start();
            
            // Enable async on all loggers
            std::shared_lock<std::shared_mutex> lock2(loggersMutex_);
            for (auto& logger : loggers_) {
                if (logger->isAsync()) {
                    logger->enableAsyncLogging();
                }
            }
        }
    }
    
    void disableAsyncLogging() {
        std::lock_guard<std::mutex> lock(asyncMutex_);
        if (async_) {
            shutdownAsyncLogger();
            async_ = false;
        }
    }
    
    bool isAsyncEnabled() const {
        return async_;
    }
    
    // Performance monitoring
    struct LoggerPerformanceStats {
        uint64_t totalLogs{0};
        uint64_t droppedLogs{0};
        size_t currentQueueSize{0};
        std::chrono::steady_clock::time_point startTime;
        
        LoggerPerformanceStats() : startTime(std::chrono::steady_clock::now()) {}
    };
    
    LoggerPerformanceStats getPerformanceStats() const {
        LoggerPerformanceStats stats;
        // Implementation would track actual metrics
        stats.currentQueueSize = asyncQueue_.size();
        return stats;
    }
    
    // Log stream interface
    LogStream logStream(LogLevel level, const std::string& file, 
                        int line, const std::string& function) {
        return LogStream(level, file, line, function, *this);
    }
    
    // Configuration presets
    static void setupDevelopmentMode() {
        Logger& logger = getGlobal();
        logger.clearLoggers();
        
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Debug);
        consoleLogger->setUseColors(true);
        consoleLogger->setOutputFile("debug.log");
        logger.addLogger(consoleLogger);
        
        logger.setLevel(LogLevel::Debug);
    }
    
    static void setupProductionMode() {
        Logger& logger = getGlobal();
        logger.clearLoggers();
        
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Warning);
        consoleLogger->setUseColors(false);
        logger.addLogger(consoleLogger);
        
        logger.setLevel(LogLevel::Error);
    }
    
    static void setupFileOnlyMode(const std::string& logFile) {
        Logger& logger = getGlobal();
        logger.clearLoggers();
        
        auto fileLogger = std::make_shared<ConsoleLogger>(LogLevel::Debug);
        fileLogger->setUseColors(false);
        fileLogger->setOutputFile(logFile);
        logger.addLogger(fileLogger);
        
        logger.setLevel(LogLevel::Debug);
    }
    
    // Cleanup
    void shutdown() {
        disableAsyncLogging();
        clearLoggers();
    }
    
private:
    void setupDefaultLoggers() {
        auto consoleLogger = std::make_shared<ConsoleLogger>();
        addLogger(consoleLogger);
    }
    
    void shutdownAsyncLogger() {
        asyncQueue_.shutdown();
    }
    
    // Member variables
    static thread_local std::string currentContext_;
    
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
    std::string levelToString(LogLevel level) const {
        switch (level) {
            case LogLevel::Debug: return "DEBUG";
            case LogLevel::Info: return "INFO";
            case LogLevel::Warning: return "WARN";
            case LogLevel::Error: return "ERROR";
            case LogLevel::Critical: return "CRIT";
            default: return "UNKNOWN";
        }
    }
};

// Initialize static members
std::shared_ptr<Logger> Logger::globalInstance_ = std::make_shared<Logger>();
std::mutex Logger::globalMutex_;
std::string Logger::currentContext_ = "";