#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <vector>
#include <chrono>
#include <mutex>
#include <iostream>
#include <fstream>
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

// Log entry structure
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
};

// Logger interface
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
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
    
    // Helper methods for log level conversion
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// File logger implementation with rotation support
class FileLogger : public ILogger {
public:
    FileLogger(const std::string& filepath);
    explicit FileLogger(const std::string& filepath, LogLevel level);
    ~FileLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Rotation configuration
    void setMaxFileSize(size_t maxSize);
    void setMaxFileCount(size_t maxCount);
    void setRotationTime(const std::chrono::system_clock::time_point& time);
    void rotate();
    
    // Get current log file path
    std::string getLogFilePath() const;
    
    // Helper method for log level conversion
    std::string levelToString(LogLevel level) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Network logger (placeholder for future)
class NetworkLogger : public ILogger {
public:
    NetworkLogger(const std::string& host, int port);
    ~NetworkLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Database logger (placeholder for future)
class DatabaseLogger : public ILogger {
public:
    DatabaseLogger(const std::string& connectionString);
    ~DatabaseLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Async logger implementation (placeholder for future)
class AsyncLogger : public ILogger {
public:
    AsyncLogger(size_t maxQueueSize = 1000);
    ~AsyncLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    void flush();
    void setCurrentLogger(std::shared_ptr<Logger> logger);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Global logger configuration utility
class LoggerConfigurator {
public:
    // Load logging configuration from file
    static bool loadFromFile(const std::string& filepath);
    
    // Parse simple configuration string
    static bool parseFromString(const std::string& config);
    
    // Get current configuration
    static std::map<std::string, std::string> getCurrentConfig();
    
    // Validate log level string
    static bool isValidLogLevel(const std::string& levelStr);
    
    // Convert log level string to enum
    static LogLevel logLevelFromString(const std::string& levelStr);
    
    // Convert log level enum to string
    static std::string logLevelToString(LogLevel level);
    
    // Pattern matcher for filtering
    class PatternMatcher {
    public:
        PatternMatcher(const std::string& pattern);
        bool matches(const std::string& text) const;
        bool isRegex() const { return isRegex_; }
    
    private:
        std::string pattern_;
        bool isRegex_;
    };
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
             const std::string& function = "");
    
    // Convenience methods
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance (thread-safe)
    static Logger& getGlobal();
    
    // Set global logger instance (thread-safe)
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Initialize default global logger
    static void initializeDefaultGlobalLogger();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Thread-safe static mutex for global logger management
    static std::mutex globalMutex_;
    static std::shared_ptr<Logger> globalLogger_;
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
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
};

#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

} // namespace nlm

// Global static members
std::mutex nlm::Logger::globalMutex_;
std::shared_ptr<nlm::Logger> nlm::Logger::globalLogger_ = nullptr;

// ConsoleLogger implementation
struct nlm::ConsoleLogger::Impl {
    nlm::LogLevel level = nlm::LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
};

nlm::ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {}

nlm::ConsoleLogger::ConsoleLogger(nlm::LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
}

nlm::ConsoleLogger::~ConsoleLogger() = default;

void nlm::ConsoleLogger::log(const nlm::LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    
    oss << " " << entry.message;
    
    std::string output = oss.str();
    
    if (pImpl->useColors) {
        std::cout << levelToColor(entry.level) << output << "\033[0m\n";
    } else {
        std::cout << output << "\n";
    }
    
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut << output << "\n";
        pImpl->fileOut.flush();
    }
}

void nlm::ConsoleLogger::setLevel(nlm::LogLevel level) {
    pImpl->level = level;
}

nlm::LogLevel nlm::ConsoleLogger::getLevel() const {
    return pImpl->level;
}

void nlm::ConsoleLogger::setUseColors(bool useColors) {
    pImpl->useColors = useColors;
}

bool nlm::ConsoleLogger::getUseColors() const {
    return pImpl->useColors;
}

void nlm::ConsoleLogger::setOutputFile(const std::string& filepath) {
    pImpl->fileOut.open(filepath, std::ios::app);
}

void nlm::ConsoleLogger::closeOutputFile() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

std::string nlm::ConsoleLogger::levelToString(nlm::LogLevel level) const {
    switch (level) {
        case nlm::LogLevel::Debug: return "DEBUG";
        case nlm::LogLevel::Info: return "INFO";
        case nlm::LogLevel::Warning: return "WARN";
        case nlm::LogLevel::Error: return "ERROR";
        case nlm::LogLevel::Critical: return "CRIT";
        default: return "UNKNOWN";
    }
}

std::string nlm::ConsoleLogger::levelToColor(nlm::LogLevel level) const {
    switch (level) {
        case nlm::LogLevel::Debug: return "\033[36m";
        case nlm::LogLevel::Info: return "\033[32m";
        case nlm::LogLevel::Warning: return "\033[33m";
        case nlm::LogLevel::Error: return "\033[31m";
        case nlm::LogLevel::Critical: return "\033[35m";
        default: return "\033[0m";
    }
}

// FileLogger implementation
struct nlm::FileLogger::Impl {
    nlm::LogLevel level = nlm::LogLevel::Info;
    std::ofstream fileOut;
    std::mutex mutex;
    size_t maxFileSize = 10 * 1024 * 1024;
    size_t maxFileCount = 5;
    std::chrono::system_clock::time_point rotationTime;
    std::string baseFilename;
    size_t currentFileSize = 0;
};

nlm::FileLogger::FileLogger(const std::string& filepath) : pImpl(std::make_unique<Impl>()) {
    pImpl->baseFilename = filepath;
    pImpl->rotationTime = std::chrono::system_clock::now();
    rotate();
}

nlm::FileLogger::FileLogger(const std::string& filepath, nlm::LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
    pImpl->baseFilename = filepath;
    pImpl->rotationTime = std::chrono::system_clock::now();
    rotate();
}

nlm::FileLogger::~FileLogger() = default;

void nlm::FileLogger::log(const nlm::LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    
    oss << " " << entry.message;
    
    std::string output = oss.str();
    pImpl->fileOut << output << "\n";
    pImpl->fileOut.flush();
    pImpl->currentFileSize += output.size() + 1;
    
    if (pImpl->currentFileSize >= pImpl->maxFileSize) {
        rotate();
    }
}

void nlm::FileLogger::setLevel(nlm::LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->level = level;
}

nlm::LogLevel nlm::FileLogger::getLevel() const {
    return pImpl->level;
}

void nlm::FileLogger::setMaxFileSize(size_t maxSize) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->maxFileSize = maxSize;
}

void nlm::FileLogger::setMaxFileCount(size_t maxCount) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->maxFileCount = maxCount;
}

void nlm::FileLogger::setRotationTime(const std::chrono::system_clock::time_point& time) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->rotationTime = time;
}

void nlm::FileLogger::rotate() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
    
    std::string timestamp = getCurrentTimestamp();
    std::string newFilename = rotateFilename(pImpl->baseFilename, timestamp) + ".log";
    
    pImpl->fileOut.open(newFilename, std::ios::app);
    pImpl->currentFileSize = 0;
    pImpl->rotationTime = std::chrono::system_clock::now();
}

std::string nlm::FileLogger::getLogFilePath() const {
    return "";
}

std::string nlm::FileLogger::levelToString(nlm::LogLevel level) const {
    switch (level) {
        case nlm::LogLevel::Debug: return "DEBUG";
        case nlm::LogLevel::Info: return "INFO";
        case nlm::LogLevel::Warning: return "WARN";
        case nlm::LogLevel::Error: return "ERROR";
        case nlm::LogLevel::Critical: return "CRIT";
        default: return "UNKNOWN";
    }
}

// Helper functions for FileLogger
namespace {
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S");
        return ss.str();
    }
    
    std::string rotateFilename(const std::string& original, const std::string& timestamp) {
        size_t dotPos = original.find_last_of('.');
        if (dotPos == std::string::npos) {
            return original + "_" + timestamp;
        }
        return original.substr(0, dotPos) + "_" + timestamp + 
               original.substr(dotPos);
    }
}

// NetworkLogger implementation (placeholder)
struct nlm::NetworkLogger::Impl {
    nlm::LogLevel level = nlm::LogLevel::Info;
    std::string host;
    int port;
    std::mutex mutex;
};

nlm::NetworkLogger::NetworkLogger(const std::string& host, int port) : pImpl(std::make_unique<Impl>()) {
    pImpl->host = host;
    pImpl->port = port;
}

nlm::NetworkLogger::~NetworkLogger() = default;

void nlm::NetworkLogger::log(const nlm::LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::cout << "[NETWORK] " << levelToString(entry.level) << ": " << entry.message << std::endl;
}

void nlm::NetworkLogger::setLevel(nlm::LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->level = level;
}

nlm::LogLevel nlm::NetworkLogger::getLevel() const {
    return pImpl->level;
}

// DatabaseLogger implementation (placeholder)
struct nlm::DatabaseLogger::Impl {
    nlm::LogLevel level = nlm::LogLevel::Info;
    std::string connectionString;
    std::mutex mutex;
};

nlm::DatabaseLogger::DatabaseLogger(const std::string& connectionString) : pImpl(std::make_unique<Impl>()) {
    pImpl->connectionString = connectionString;
}

nlm::DatabaseLogger::~DatabaseLogger() = default;

void nlm::DatabaseLogger::log(const nlm::LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::cout << "[DATABASE] " << levelToString(entry.level) << ": " << entry.message << std::endl;
}

void nlm::DatabaseLogger::setLevel(nlm::LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->level = level;
}

nlm::LogLevel nlm::DatabaseLogger::getLevel() const {
    return pImpl->level;
}

// AsyncLogger implementation (placeholder)
struct nlm::AsyncLogger::Impl {
    std::mutex queueMutex_;
    std::vector<std::pair<std::string, nlm::LogLevel>> logQueue_;
    size_t maxQueueSize_;
    std::shared_ptr<nlm::Logger> currentLogger_;
};

nlm::AsyncLogger::AsyncLogger(size_t maxQueueSize) : pImpl(std::make_unique<Impl>()) {
    pImpl->maxQueueSize_ = maxQueueSize;
}

nlm::AsyncLogger::~AsyncLogger() = default;

void nlm::AsyncLogger::log(const nlm::LogEntry& entry) {
    std::lock_guard<std::mutex> lock(pImpl->queueMutex_);
    pImpl->logQueue_.push_back({entry.message, entry.level});
    if (pImpl->logQueue_.size() >= pImpl->maxQueueSize_) {
        flush();
    }
}

void nlm::AsyncLogger::flush() {
    std::lock_guard<std::mutex> lock(pImpl->queueMutex_);
    if (!pImpl->logQueue_.empty() && pImpl->currentLogger_) {
        for (const auto& logEntry : pImpl->logQueue_) {
            std::thread([this, logEntry]() {
                pImpl->currentLogger_->log(nlm::LogLevel::Info, logEntry.first);
            }).detach();
        }
        pImpl->logQueue_.clear();
    }
}

void nlm::AsyncLogger::setCurrentLogger(std::shared_ptr<nlm::Logger> logger) {
    pImpl->currentLogger_ = logger;
}

// Logger class implementation
struct nlm::Logger::Impl {
    std::vector<std::shared_ptr<nlm::ILogger>> loggers;
    std::mutex mutex;
};

nlm::Logger::Logger() : pImpl(std::make_unique<Impl>()) {}

nlm::Logger::~Logger() = default;

void nlm::Logger::addLogger(std::shared_ptr<nlm::ILogger> logger) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.push_back(logger);
}

void nlm::Logger::clearLoggers() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.clear();
}

void nlm::Logger::setLevel(nlm::LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        logger->setLevel(level);
    }
}

void nlm::Logger::log(nlm::LogLevel level, const std::string& message,
                const std::string& file, int line, const std::string& function) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    nlm::LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    
    for (auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
}

void nlm::Logger::debug(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(nlm::LogLevel::Debug, message, file, line, function);
}

void nlm::Logger::info(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(nlm::LogLevel::Info, message, file, line, function);
}

void nlm::Logger::warning(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(nlm::LogLevel::Warning, message, file, line, function);
}

void nlm::Logger::error(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(nlm::LogLevel::Error, message, file, line, function);
}

void nlm::Logger::critical(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(nlm::LogLevel::Critical, message, file, line, function);
}

bool nlm::Logger::isEnabled(nlm::LogLevel level) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        if (logger->getLevel() <= level) {
            return true;
        }
    }
    return false;
}

nlm::Logger& nlm::Logger::getGlobal() {
    std::lock_guard<std::mutex> lock(globalMutex_);
    if (!globalLogger_) {
        initializeDefaultGlobalLogger();
    }
    return *globalLogger_;
}

void nlm::Logger::setGlobal(std::shared_ptr<Logger> logger) {
    if (!logger) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(globalMutex_);
    globalLogger_ = logger;
}

void nlm::Logger::initializeDefaultGlobalLogger() {
    auto logger = std::make_shared<Logger>();
    logger->addLogger(std::make_shared<ConsoleLogger>());
    setGlobal(logger);
}

// LogStream implementation
nlm::LogStream::LogStream(nlm::LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function) {}

nlm::LogStream::~LogStream() {
    nlm::Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
}

} // namespace nlm

int main() {
    return 0;
}
