#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <chrono>
#include <map>
#include <vector>
#include <atomic>

namespace nlm {

struct FileLogger::Impl {
    LogLevel level = LogLevel::Info;
    std::ofstream fileOut;
    std::string filepath;
    size_t maxFileSize = 10 * 1024 * 1024; // 10MB
    size_t maxFiles = 5;
    std::mutex mutex;
    size_t currentFileSize = 0;
    unsigned int fileIndex = 0;
};

FileLogger::FileLogger(const std::string& filepath) : pImpl(std::make_unique<Impl>()) {
    setFilepath(filepath);
    setMaxFileSize(10 * 1024 * 1024); // 10MB default
    setMaxFiles(5);
}

FileLogger::~FileLogger() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

void FileLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Check if we need to rotate files
    if (pImpl->currentFileSize >= pImpl->maxFileSize) {
        pImpl->fileOut.close();
        pImpl->fileIndex++;
        setFilepath(pImpl->filepath);
    }
    
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    
    // Add timestamp
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    
    oss << " " << entry.message;
    
    std::string output = oss.str();
    
    pImpl->fileOut << output << "\n";
    pImpl->fileOut.flush();
    pImpl->currentFileSize += output.size() + 1; // +1 for newline
}

void FileLogger::setLevel(LogLevel level) {
    pImpl->level = level;
}

LogLevel FileLogger::getLevel() const {
    return pImpl->level;
}

void FileLogger::setFilepath(const std::string& filepath) {
    pImpl->filepath = filepath;
    pImpl->fileIndex = 0;
    pImpl->currentFileSize = 0;
    
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
    
    // Create filename with index if needed
    std::string fullPath = filepath;
    if (pImpl->fileIndex > 0) {
        size_t dotPos = filepath.find_last_of('.');
        if (dotPos != std::string::npos) {
            fullPath = filepath.substr(0, dotPos) + "_" + std::to_string(pImpl->fileIndex) + filepath.substr(dotPos);
        } else {
            fullPath = filepath + "_" + std::to_string(pImpl->fileIndex);
        }
    }
    
    pImpl->fileOut.open(fullPath, std::ios::app);
}

void FileLogger::setMaxFileSize(size_t maxSize) {
    pImpl->maxFileSize = maxSize;
}

void FileLogger::setMaxFiles(size_t maxFiles) {
    pImpl->maxFiles = maxFiles;
}

std::string FileLogger::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Critical: return "CRIT";
        default: return "UNKNOWN";
    }
}

struct BufferedLogger::Impl {
    LogLevel level = LogLevel::Info;
    std::vector<LogEntry> buffer;
    size_t maxBufferSize = 1000;
    std::chrono::milliseconds flushInterval{1000};
    std::mutex mutex;
    std::chrono::steady_clock::time_point lastFlush;
    bool flushRequested = false;
};

BufferedLogger::BufferedLogger(size_t maxBufferSize) : pImpl(std::make_unique<Impl>()) {
    pImpl->maxBufferSize = maxBufferSize;
    pImpl->lastFlush = std::chrono::steady_clock::now();
}

BufferedLogger::~BufferedLogger() {
    flush();
}

void BufferedLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    buffer.push_back(entry);
    
    // Check if we need to flush
    if (buffer.size() >= pImpl->maxBufferSize || flushRequested) {
        flush();
        flushRequested = false;
    }
    
    // Check if periodic flush is needed
    auto now = std::chrono::steady_clock::now();
    if (std::chrono::duration_cast<std::chrono::milliseconds>(now - pImpl->lastFlush) >= pImpl->flushInterval) {
        flush();
        pImpl->lastFlush = now;
    }
}

void BufferedLogger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->level = level;
}

LogLevel BufferedLogger::getLevel() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->level;
}

void BufferedLogger::flush() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (buffer.empty()) {
        return;
    }
    
    // Flush to console (for now)
    for (const auto& entry : buffer) {
        // Send to console logger
        std::cout << "[" << levelToString(entry.level) << "] ";
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        std::cout << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        std::cout << " " << entry.message << "\n";
    }
    
    buffer.clear();
    pImpl->lastFlush = std::chrono::steady_clock::now();
}

void BufferedLogger::setFlushInterval(std::chrono::milliseconds interval) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->flushInterval = interval;
}

struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    bool enabled = true;
    
    // Log statistics
    size_t totalLogs = 0;
    std::map<LogLevel, size_t> logsByLevel;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {
    pImpl->startTime = std::chrono::system_clock::now();
    pImpl->endTime = pImpl->startTime;
    
    // Add a default console logger
    addLogger(std::make_shared<ConsoleLogger>(LogLevel::Info));
}

Logger::~Logger() = default;

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    if (!logger || !enabled) return;
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.push_back(logger);
}

void Logger::clearLoggers() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.clear();
}

void Logger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        logger->setLevel(level);
    }
}

void Logger::setEnabled(bool enabled) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    this->enabled = enabled;
    if (!enabled) {
        clearLoggers();
    }
}

bool Logger::isEnabled() const {
    return pImpl->enabled;
}

void Logger::log(LogLevel level, const std::string& message,
                const std::string& file, int line, const std::string& function,
                const std::unordered_map<std::string, std::string>& metadata) {
    if (!enabled || level < LogLevel::Info) {
        return; // Filter out debug messages unless explicitly enabled
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.metadata = metadata;
    
    pImpl->totalLogs++;
    pImpl->logsByLevel[level]++;
    pImpl->endTime = entry.timestamp;
    
    for (auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
}

void Logger::debug(const std::string& message, const std::string& file, int line, const std::string& function,
                   const std::unordered_map<std::string, std::string>& metadata) {
    log(LogLevel::Debug, message, file, line, function, metadata);
}

void Logger::info(const std::string& message, const std::string& file, int line, const std::string& function,
                  const std::unordered_map<std::string, std::string>& metadata) {
    log(LogLevel::Info, message, file, line, function, metadata);
}

void Logger::warning(const std::string& message, const std::string& file, int line, const std::string& function,
                     const std::unordered_map<std::string, std::string>& metadata) {
    log(LogLevel::Warning, message, file, line, function, metadata);
}

void Logger::error(const std::string& message, const std::string& file, int line, const std::string& function,
                   const std::unordered_map<std::string, std::string>& metadata) {
    log(LogLevel::Error, message, file, line, function, metadata);
}

void Logger::critical(const std::string& message, const std::string& file, int line, const std::string& function,
                      const std::unordered_map<std::string, std::string>& metadata) {
    log(LogLevel::Critical, message, file, line, function, metadata);
}

bool Logger::isEnabled(LogLevel level) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        if (logger->isEnabled() && logger->getLevel() <= level) {
            return true;
        }
    }
    return false;
}

Logger& Logger::getGlobal() {
    static Logger globalLogger;
    return globalLogger;
}

void Logger::setGlobal(std::shared_ptr<Logger> logger) {
    // This is a placeholder for advanced global logger management
    // In practice, we just use getGlobal() which is already static
}

void Logger::configureFromEnv() {
    const char* logLevel = std::getenv("NLM_LOG_LEVEL");
    if (logLevel) {
        std::string level = logLevel;
        LogLevel logLevelEnum;
        if (level == "DEBUG") logLevelEnum = LogLevel::Debug;
        else if (level == "INFO") logLevelEnum = LogLevel::Info;
        else if (level == "WARN") logLevelEnum = LogLevel::Warning;
        else if (level == "ERROR") logLevelEnum = LogLevel::Error;
        else if (level == "CRITICAL") logLevelEnum = LogLevel::Critical;
        else logLevelEnum = LogLevel::Info;
        
        getGlobal().setLevel(logLevelEnum);
    }
    
    const char* logEnabled = std::getenv("NLM_LOG_ENABLED");
    if (logEnabled) {
        bool enabled = std::string(logEnabled) == "true" || std::string(logEnabled) == "1";
        getGlobal().setEnabled(enabled);
    }
}

Logger::LogStats Logger::getLogStats() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    LogStats stats;
    stats.totalLogs = pImpl->totalLogs;
    stats.logsByLevel = pImpl->logsByLevel;
    stats.startTime = pImpl->startTime;
    stats.endTime = pImpl->endTime;
    return stats;
}

void Logger::resetStats() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->totalLogs = 0;
    pImpl->logsByLevel.clear();
    pImpl->startTime = std::chrono::system_clock::now();
    pImpl->endTime = pImpl->startTime;
}

void Logger::logPerformanceMetrics() const {
    auto stats = getLogStats();
    info("Log performance metrics: " + std::to_string(stats.totalLogs) + " total logs, " +
         std::to_string(stats.logsByLevel[LogLevel::Info]) + " info, " +
         std::to_string(stats.logsByLevel[LogLevel::Error]) + " errors in " +
         std::to_string(std::chrono::duration<double>(stats.endTime - stats.startTime).count()) + "s");
}

// LogStream implementation
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function,
                   const std::unordered_map<std::string, std::string>& metadata)
    : level_(level), file_(file), line_(line), function_(function), metadata_(metadata) {}

LogStream::~LogStream() {
    Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_, metadata_);
}

LogStream& LogStream::withMetadata(const std::string& key, const std::string& value) {
    metadata_[key] = value;
    return *this;
}

LogStream& LogStream::context(const std::string& operation, const std::string& component) {
    metadata_["operation"] = operation;
    metadata_["component"] = component;
    return *this;
}

// Timer implementation
Timer::Timer(const std::string& operation, const std::string& component)
    : operation_(operation), component_(component), stopped_(false), durationMs_(0.0) {
    startTime_ = std::chrono::steady_clock::now();
}

Timer::~Timer() {
    if (!stopped_) {
        stop();
    }
    // Log the timing if logging is enabled
    if (Logger::getGlobal().isEnabled()) {
        Logger::getGlobal().info(std::string("Timer completed: ") + operation_ + 
                                " in " + std::to_string(durationMs_) + "ms");
    }
}

void Timer::stop() {
    if (!stopped_) {
        auto endTime = std::chrono::steady_clock::now();
        durationMs_ = std::chrono::duration<double, std::milli>(endTime - startTime_).count();
        stopped_ = true;
    }
}

double Timer::getDurationMs() const {
    if (!stopped_) {
        auto endTime = std::chrono::steady_clock::now();
        return std::chrono::duration<double, std::milli>(endTime - startTime_).count();
    }
    return durationMs_;
}

} // namespace nlm