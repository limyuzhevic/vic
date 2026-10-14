#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <map>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <thread>
#include <deque>
#include <fstream>
#include <iomanip>
#include <shared_mutex>
#include <filesystem>
#include <optional>

namespace nlm {

// Log severity levels
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4
};

// Log level hierarchy and filtering
class LogLevelHierarchy {
public:
    LogLevelHierarchy() {
        enabledLevels[static_cast<size_t>(LogLevel::Debug)] = true;
        enabledLevels[static_cast<size_t>(LogLevel::Info)] = true;
        enabledLevels[static_cast<size_t>(LogLevel::Warning)] = true;
        enabledLevels[static_cast<size_t>(LogLevel::Error)] = true;
        enabledLevels[static_cast<size_t>(LogLevel::Critical)] = true;
    }

    void enableLevel(LogLevel level, bool enable = true) {
        std::lock_guard<std::mutex> lock(mutex_);
        enabledLevels[static_cast<size_t>(level)] = enable;
    }

    bool isLevelEnabled(LogLevel level) const {
        std::shared_lock<shared_mutex> lock(mutex_);
        return enabledLevels[static_cast<size_t>(level)];
    }

    void setAllLevels(bool enable) {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto& enabled : enabledLevels) {
            enabled = enable;
        }
    }

private:
    std::array<bool, 5> enabledLevels;
    mutable shared_mutex mutex_;
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

// Log formatter interface (Strategy pattern)
class ILogFormatter {
public:
    virtual ~ILogFormatter() = default;
    virtual std::string format(const LogEntry& entry) = 0;
};

// Default formatter
class DefaultLogFormatter : public ILogFormatter {
public:
    std::string format(const LogEntry& entry) override {
        std::ostringstream oss;
        oss << "[" << levelToString(entry.level) << "] ";
        
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        
        oss << " " << entry.file << ":" << entry.line << " ";
        oss << entry.message;
        
        return oss.str();
    }
};

// Simple formatter (no file/line)
class SimpleLogFormatter : public ILogFormatter {
public:
    std::string format(const LogEntry& entry) override {
        std::ostringstream oss;
        oss << "[" << levelToString(entry.level) << "] ";
        
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        
        oss << " " << entry.message;
        
        return oss.str();
    }
};

// Log rotation strategy
class ILogRotationStrategy {
public:
    virtual ~ILogRotationStrategy() = default;
    virtual bool shouldRotate(const std::string& filepath, size_t currentSize) = 0;
    virtual std::string getRotatedFilename(const std::string& filepath, size_t index = 0) = 0;
};

// Size-based rotation
class SizeRotationStrategy : public ILogRotationStrategy {
public:
    explicit SizeRotationStrategy(size_t maxSizeMB = 10) : maxSize_(maxSizeMB * 1024 * 1024) {}
    
    bool shouldRotate(const std::string& filepath, size_t currentSize) override {
        return currentSize >= maxSize_;
    }
    
    std::string getRotatedFilename(const std::string& filepath, size_t index = 0) override {
        if (index == 0) return filepath;
        
        auto ext = std::filesystem::path(filepath).extension();
        auto name = std::filesystem::path(filepath).stem();
        return name.string() + "_" + std::to_string(index) + ext.string();
    }
    
private:
    size_t maxSize_;
};

// Time-based rotation
class TimeRotationStrategy : public ILogRotationStrategy {
public:
    explicit TimeRotationStrategy(const std::string& pattern = "daily") 
        : pattern_(pattern) {}
    
    bool shouldRotate(const std::string& filepath, size_t currentSize) override {
        auto now = std::chrono::system_clock::now();
        auto today = std::chrono::system_clock::to_time_t(now);
        tm timeInfo;
        localtime_r(&today, &timeInfo);
        
        if (pattern_ == "daily") {
            return rotationDate_ != today;
        } else if (pattern_ == "weekly") {
            return rotationDate_ != today || timeInfo.tm_wday != static_cast<int>(now.time_since_epoch().count() % 7);
        } else if (pattern_ == "monthly") {
            if (rotationDate_ == today) return false;
            tm rotationTm;
            localtime_r(&rotationDate_, &rotationTm);
            tm todayTm;
            localtime_r(&today, &todayTm);
            return rotationTm.tm_year != todayTm.tm_year || rotationTm.tm_mon != todayTm.tm_mon;
        }
        return false;
    }
    
    std::string getRotatedFilename(const std::string& filepath, size_t index = 0) override {
        if (index == 0) return filepath;
        
        auto ext = std::filesystem::path(filepath).extension();
        auto name = std::filesystem::path(filepath).stem();
        return name.string() + "_" + std::to_string(index) + ext.string();
    }
    
    void updateRotationDate(const std::chrono::system_clock::time_point& time) {
        rotationDate_ = std::chrono::system_clock::to_time_t(time);
    }
    
private:
    std::string pattern_;
    time_t rotationDate_ = 0;
};

// Logger sink interface (Observer pattern)
class ILogSink {
public:
    virtual ~ILogSink() = default;
    virtual void write(const std::string& formattedMessage) = 0;
    virtual void flush() = 0;
    virtual void close() = 0;
};

// Console sink
class ConsoleSink : public ILogSink {
public:
    ConsoleSink() : useColors_(true) {}
    explicit ConsoleSink(bool useColors) : useColors_(useColors) {}
    
    void write(const std::string& formattedMessage) override {
        std::cout << formattedMessage << "\n";
    }
    
    void flush() override {
        std::cout << std::flush;
    }
    
    void close() override {
        // Nothing to close for console
    }
    
    void setUseColors(bool useColors) {
        useColors_ = useColors;
    }
    
private:
    bool useColors_;
    std::string levelToColor(LogLevel level) const {
        switch (level) {
            case LogLevel::Debug: return "\033[36m";    // Cyan
            case LogLevel::Info: return "\033[32m";      // Green
            case LogLevel::Warning: return "\033[33m";   // Yellow
            case LogLevel::Error: return "\033[31m";     // Red
            case LogLevel::Critical: return "\033[35m"; // Magenta
            default: return "\033[0m";
        }
    }
};

// File sink with rotation
class FileSink : public ILogSink {
public:
    FileSink(const std::string& filepath, 
             std::shared_ptr<ILogRotationStrategy> rotationStrategy = nullptr,
             bool append = true)
        : filepath_(filepath), rotationStrategy_(rotationStrategy), append_(append) {
        rotateFile();
    }
    
    void write(const std::string& formattedMessage) override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        currentFile_ << formattedMessage << "\n";
        currentSize_ += formattedMessage.size() + 1; // +1 for newline
        
        // Check for rotation
        if (rotationStrategy_ && rotationStrategy_->shouldRotate(filepath_, currentSize_)) {
            rotateFile();
        }
        
        currentFile_.flush();
    }
    
    void flush() override {
        std::lock_guard<std::mutex> lock(mutex_);
        if (currentFile_.is_open()) {
            currentFile_.flush();
        }
    }
    
    void close() override {
        std::lock_guard<std::mutex> lock(mutex_);
        if (currentFile_.is_open()) {
            currentFile_.close();
        }
    }
    
    ~FileSink() {
        close();
    }
    
private:
    void rotateFile() {
        if (currentFile_.is_open()) {
            currentFile_.close();
        }
        
        // Find next available filename
        size_t index = 0;
        std::string rotatedPath = filepath_;
        
        while (std::filesystem::exists(rotatedPath)) {
            rotatedPath = rotationStrategy_ ? 
                rotationStrategy_->getRotatedFilename(filepath_, ++index) : filepath_;
        }
        
        currentFile_.open(rotatedPath, append_ ? std::ios::app : std::ios::trunc);
        if (!currentFile_.is_open()) {
            throw std::runtime_error("Failed to open log file: " + filepath_);
        }
        
        currentSize_ = 0;
        
        if (rotationStrategy_) {
            auto now = std::chrono::system_clock::now();
            if (auto timeStrategy = std::dynamic_pointer_cast<TimeRotationStrategy>(rotationStrategy_)) {
                timeStrategy->updateRotationDate(now);
            }
        }
    }
    
    std::string filepath_;
    std::shared_ptr<ILogRotationStrategy> rotationStrategy_;
    std::ofstream currentFile_;
    size_t currentSize_ = 0;
    bool append_;
    mutable std::mutex mutex_;
};

// Log statistics
struct LogStatistics {
    std::map<LogLevel, size_t> countByLevel;
    std::chrono::system_clock::time_point firstLogTime;
    std::chrono::system_clock::time_point lastLogTime;
    size_t totalLogs = 0;
    
    void reset() {
        countByLevel.clear();
        firstLogTime = std::chrono::system_clock::now();
        lastLogTime = firstLogTime;
        totalLogs = 0;
    }
};

// Async log buffer
class AsyncLogBuffer {
public:
    AsyncLogBuffer(size_t maxBufferSize = 1000, size_t flushIntervalMs = 100)
        : maxBufferSize_(maxBufferSize), flushIntervalMs_(flushIntervalMs) {
        flushThread_ = std::thread(&AsyncLogBuffer::flushThreadFunc, this);
    }
    
    ~AsyncLogBuffer() {
        shutdown_ = true;
        flushCondition_.notify_all();
        if (flushThread_.joinable()) {
            flushThread_.join();
        }
    }
    
    void add(const std::string& message) {
        std::lock_guard<std::mutex> lock(mutex_);
        buffer_.push_back(message);
        
        if (buffer_.size() >= maxBufferSize_) {
            flushCondition_.notify_all();
        }
    }
    
    void setFlushCallback(std::function<void(const std::vector<std::string>&)> callback) {
        flushCallback_ = std::move(callback);
    }
    
private:
    void flushThreadFunc() {
        while (!shutdown_) {
            std::vector<std::string> toFlush;
            {
                std::unique_lock<std::mutex> lock(mutex_);
                flushCondition_.wait_for(lock, 
                    std::chrono::milliseconds(flushIntervalMs_), 
                    [this]() { return !buffer_.empty() || shutdown_; });
                
                if (shutdown_ && buffer_.empty()) {
                    break;
                }
                
                toFlush = std::move(buffer_);
            }
            
            if (!toFlush.empty() && flushCallback_) {
                flushCallback_(toFlush);
            }
        }
    }
    
    std::deque<std::string> buffer_;
    std::mutex mutex_;
    std::condition_variable flushCondition_;
    std::thread flushThread_;
    std::atomic<bool> shutdown_{false};
    size_t maxBufferSize_;
    size_t flushIntervalMs_;
    std::function<void(const std::vector<std::string>&)> flushCallback_;
};

// Pattern-based filter
class PatternFilter {
public:
    bool shouldLog(const LogEntry& entry) const {
        if (!filePattern_.empty() && !matchesPattern(entry.file, filePattern_)) {
            return false;
        }
        
        if (!functionPattern_.empty() && !matchesPattern(entry.function, functionPattern_)) {
            return false;
        }
        
        if (!messagePattern_.empty() && !matchesPattern(entry.message, messagePattern_)) {
            return false;
        }
        
        return true;
    }
    
    void setFilePattern(const std::string& pattern) { filePattern_ = pattern; }
    void setFunctionPattern(const std::string& pattern) { functionPattern_ = pattern; }
    void setMessagePattern(const std::string& pattern) { messagePattern_ = pattern; }
    
private:
    bool matchesPattern(const std::string& text, const std::string& pattern) const {
        if (pattern.empty()) return true;
        
        // Simple wildcard matching
        for (size_t i = 0, j = 0; i < text.size() && j < pattern.size(); ++i, ++j) {
            if (pattern[j] == '*') {
                while (j + 1 < pattern.size() && pattern[j + 1] == '*') {
                    ++j;
                }
                if (j + 1 == pattern.size()) return true;
                
                size_t k = text.find(pattern[j + 1], i);
                if (k == std::string::npos) return false;
                i = k - 1;
                ++j;
            } else if (pattern[j] == '?' || pattern[j] == text[i]) {
                continue;
            } else {
                return false;
            }
        }
        
        return j == pattern.size() && i == text.size();
    }
    
    std::string filePattern_;
    std::string functionPattern_;
    std::string messagePattern_;
};

// Enhanced logger with all features
class EnhancedLogger {
public:
    EnhancedLogger(std::shared_ptr<ILogFormatter> formatter = nullptr,
                   std::shared_ptr<ILogRotationStrategy> rotationStrategy = nullptr,
                   bool async = false,
                   size_t bufferSize = 1000)
        : formatter_(formatter ? std::move(formatter) : std::make_shared<DefaultLogFormatter>()),
          rotationStrategy_(std::move(rotationStrategy)),
          async_(async),
          bufferSize_(bufferSize) {
        
        if (async_) {
            buffer_ = std::make_unique<AsyncLogBuffer>(bufferSize);
            buffer_->setFlushCallback([this](const std::vector<std::string>& messages) {
                for (const auto& msg : messages) {
                    writeToSinks(msg);
                }
            });
        }
    }
    
    void log(const LogEntry& entry) {
        if (!shouldLog(entry)) return;
        
        auto formatted = formatEntry(entry);
        
        if (async_ && buffer_) {
            buffer_->add(formatted);
        } else {
            writeToSinks(formatted);
        }
        
        updateStatistics(entry);
    }
    
    void addSink(std::shared_ptr<ILogSink> sink) {
        std::lock_guard<std::mutex> lock(sinksMutex_);
        sinks_.push_back(std::move(sink));
    }
    
    void removeSink(ILogSink* sink) {
        std::lock_guard<std::mutex> lock(sinksMutex_);
        sinks_.erase(std::remove_if(sinks_.begin(), sinks_.end(),
            [sink](const std::shared_ptr<ILogSink>& s) { return s.get() == sink; }),
            sinks_.end());
    }
    
    void clearSinks() {
        std::lock_guard<std::mutex> lock(sinksMutex_);
        sinks_.clear();
    }
    
    void setLevel(LogLevel level) {
        levelHierarchy_.setAllLevels(level == LogLevel::Debug);
        currentLevel_ = level;
    }
    
    LogLevel getLevel() const {
        return currentLevel_;
    }
    
    void enableLevel(LogLevel level, bool enable = true) {
        levelHierarchy_.enableLevel(level, enable);
    }
    
    bool isLevelEnabled(LogLevel level) const {
        return levelHierarchy_.isLevelEnabled(level);
    }
    
    void setFormatter(std::shared_ptr<ILogFormatter> formatter) {
        std::lock_guard<std::mutex> lock(formatterMutex_);
        formatter_ = std::move(formatter);
    }
    
    void setFilePattern(const std::string& pattern) {
        filter_.setFilePattern(pattern);
    }
    
    void setFunctionPattern(const std::string& pattern) {
        filter_.setFunctionPattern(pattern);
    }
    
    void setMessagePattern(const std::string& pattern) {
        filter_.setMessagePattern(pattern);
    }
    
    LogStatistics getStatistics() const {
        std::shared_lock<shared_mutex> lock(statsMutex_);
        return statistics_;
    }
    
    void resetStatistics() {
        std::lock_guard<shared_mutex> lock(statsMutex_);
        statistics_.reset();
    }
    
    void flush() {
        if (buffer_) {
            buffer_->add(""); // Signal flush
        }
        
        std::lock_guard<std::mutex> lock(sinksMutex_);
        for (auto& sink : sinks_) {
            sink->flush();
        }
    }
    
    void close() {
        flush();
        std::lock_guard<std::mutex> lock(sinksMutex_);
        for (auto& sink : sinks_) {
            sink->close();
        }
    }
    
    ~EnhancedLogger() {
        close();
    }
    
private:
    bool shouldLog(const LogEntry& entry) const {
        // Check log level hierarchy
        if (!levelHierarchy_.isLevelEnabled(entry.level)) {
            return false;
        }
        
        // Check pattern filter
        if (!filter_.shouldLog(entry)) {
            return false;
        }
        
        return true;
    }
    
    std::string formatEntry(const LogEntry& entry) {
        std::lock_guard<std::mutex> lock(formatterMutex_);
        return formatter_->format(entry);
    }
    
    void writeToSinks(const std::string& formattedMessage) {
        std::lock_guard<std::mutex> lock(sinksMutex_);
        for (auto& sink : sinks_) {
            sink->write(formattedMessage);
        }
    }
    
    void updateStatistics(const LogEntry& entry) {
        std::lock_guard<shared_mutex> lock(statsMutex_);
        statistics_.countByLevel[static_cast<size_t>(entry.level)]++;
        statistics_.totalLogs++;
        
        if (statistics_.totalLogs == 1) {
            statistics_.firstLogTime = entry.timestamp;
        }
        statistics_.lastLogTime = entry.timestamp;
    }
    
    LogLevelHierarchy levelHierarchy_;
    LogLevel currentLevel_ = LogLevel::Info;
    PatternFilter filter_;
    
    std::shared_ptr<ILogFormatter> formatter_;
    std::shared_ptr<ILogRotationStrategy> rotationStrategy_;
    std::vector<std::shared_ptr<ILogSink>> sinks_;
    
    bool async_;
    size_t bufferSize_;
    std::unique_ptr<AsyncLogBuffer> buffer_;
    
    mutable std::mutex sinksMutex_;
    mutable std::mutex formatterMutex_;
    mutable shared_mutex statsMutex_;
    
    LogStatistics statistics_;
};

// Logger factory (Factory pattern)
class LoggerFactory {
public:
    static std::shared_ptr<ILogger> createConsoleLogger(
        LogLevel level = LogLevel::Info,
        bool useColors = true,
        const std::string& outputFile = "") {
        auto logger = std::make_shared<ConsoleLogger>();
        logger->setLevel(level);
        logger->setUseColors(useColors);
        if (!outputFile.empty()) {
            logger->setOutputFile(outputFile);
        }
        return logger;
    }
    
    static std::shared_ptr<ILogger> createFileLogger(
        const std::string& filepath,
        LogLevel level = LogLevel::Info,
        std::shared_ptr<ILogRotationStrategy> rotationStrategy = nullptr,
        bool append = true) {
        auto sink = std::make_shared<FileSink>(filepath, rotationStrategy, append);
        auto logger = std::make_shared<FileLogger>(sink);
        logger->setLevel(level);
        return logger;
    }
    
    static std::shared_ptr<ILogger> createCombinedLogger(
        std::shared_ptr<ILogFormatter> formatter = nullptr,
        std::shared_ptr<ILogRotationStrategy> rotationStrategy = nullptr,
        bool async = false,
        size_t bufferSize = 1000) {
        auto enhancedLogger = std::make_shared<EnhancedLogger>(formatter, rotationStrategy, async, bufferSize);
        return enhancedLogger;
    }
    
    static std::shared_ptr<ILogger> createFromConfig(const std::string& configPath) {
        // Load configuration from file and create appropriate logger
        // Implementation depends on configuration format
        throw std::runtime_error("Config-based logger creation not yet implemented");
    }
};

// FileLogger (simplified version for compatibility)
class FileLogger : public ILogger {
public:
    explicit FileLogger(std::shared_ptr<ILogSink> sink) : sink_(std::move(sink)) {}
    
    void log(const LogEntry& entry) override {
        // Format the entry manually for compatibility
        std::ostringstream oss;
        oss << "[" << levelToString(entry.level) << "] ";
        
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        
        oss << " " << entry.message;
        
        sink_->write(oss.str());
    }
    
    void setLevel(LogLevel level) override {
        // This is a simplified implementation
        // In practice, you would need to track levels per sink
    }
    
    LogLevel getLevel() const override {
        return LogLevel::Info; // Default level
    }
    
    void flush() {
        if (sink_) {
            sink_->flush();
        }
    }
    
private:
    std::shared_ptr<ILogSink> sink_;
    
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

// Original ILogger interface implementation for backward compatibility
class ILoggerAdapter : public ILogger {
public:
    explicit ILoggerAdapter(std::shared_ptr<EnhancedLogger> enhancedLogger)
        : enhancedLogger_(std::move(enhancedLogger)) {}
    
    void log(const LogEntry& entry) override {
        enhancedLogger_->log(entry);
    }
    
    void setLevel(LogLevel level) override {
        enhancedLogger_->setLevel(level);
    }
    
    LogLevel getLevel() const override {
        return enhancedLogger_->getLevel();
    }
    
private:
    std::shared_ptr<EnhancedLogger> enhancedLogger_;
};

// Main Logger class (maintains backward compatibility)
class Logger {
public:
    Logger() {
        auto enhancedLogger = std::make_shared<EnhancedLogger>();
        auto consoleSink = std::make_shared<ConsoleSink>(true);
        enhancedLogger->addSink(consoleSink);
        logger_ = std::make_shared<ILoggerAdapter>(enhancedLogger);
    }
    
    Logger(std::shared_ptr<ILogger> logger) : logger_(std::move(logger)) {}
    
    ~Logger() = default;
    
    // Disable copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // Add logger sink
    void addLogger(std::shared_ptr<ILogger> logger) {
        enhancedLogger_->addSink(logger);
    }
    
    // Remove all loggers
    void clearLoggers() {
        enhancedLogger_->clearSinks();
    }
    
    // Set minimum level for all loggers
    void setLevel(LogLevel level) {
        enhancedLogger_->setLevel(level);
    }
    
    // Global logging functions
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "") {
        LogEntry entry;
        entry.level = level;
        entry.message = message;
        entry.file = file;
        entry.line = line;
        entry.function = function;
        entry.timestamp = std::chrono::system_clock::now();
        
        logger_->log(entry);
    }
    
    // Convenience methods
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "") {
        log(LogLevel::Debug, message, file, line, function);
    }
    
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "") {
        log(LogLevel::Info, message, file, line, function);
    }
    
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "") {
        log(LogLevel::Warning, message, file, line, function);
    }
    
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "") {
        log(LogLevel::Error, message, file, line, function);
    }
    
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "") {
        log(LogLevel::Critical, message, file, line, function);
    }
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const {
        return logger_->getLevel() <= level;
    }
    
    // Get global logger instance
    static Logger& getGlobal() {
        static Logger globalLogger;
        return globalLogger;
    }
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger) {
        // This is a placeholder for advanced global logger management
        // In practice, we just use getGlobal() which is already static
    }
    
    // Advanced configuration methods
    void enableLevel(LogLevel level, bool enable = true) {
        enhancedLogger_->enableLevel(level, enable);
    }
    
    void setFilePattern(const std::string& pattern) {
        enhancedLogger_->setFilePattern(pattern);
    }
    
    void setFunctionPattern(const std::string& pattern) {
        enhancedLogger_->setFunctionPattern(pattern);
    }
    
    void setMessagePattern(const std::string& pattern) {
        enhancedLogger_->setMessagePattern(pattern);
    }
    
    void setFormatter(std::shared_ptr<ILogFormatter> formatter) {
        enhancedLogger_->setFormatter(std::move(formatter));
    }
    
    LogStatistics getStatistics() const {
        return enhancedLogger_->getStatistics();
    }
    
    void resetStatistics() {
        enhancedLogger_->resetStatistics();
    }
    
    void flush() {
        enhancedLogger_->flush();
    }
    
private:
    std::shared_ptr<ILogger> logger_;
    std::shared_ptr<EnhancedLogger> enhancedLogger_;
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
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
        : level_(level), file_(file), line_(line), function_(function) {}
    
    ~LogStream() {
        Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
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
};

#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

} // namespace nlm
