#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>

namespace nlm {

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
}

ConsoleLogger::~ConsoleLogger() = default;

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    bool asyncLogging = false;
    std::string format = "[%level] [%timestamp] %message";
    std::ofstream fileOut;
    std::mutex mutex;
    
    // Log rotation
    size_t maxFileSize = 10 * 1024 * 1024;  // 10 MB
    std::string filepath;
    size_t currentFileSize = 0;
    
    // Async logging queue
    std::queue<LogEntry> logQueue;
    std::condition_variable queueCondition;
    bool stopAsyncThread = false;
    std::thread asyncThread;
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {
    if (pImpl->asyncLogging) {
        pImpl->asyncThread = std::thread(&ConsoleLogger::asyncLogWorker, this);
    }
}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
    if (pImpl->asyncLogging) {
        pImpl->asyncThread = std::thread(&ConsoleLogger::asyncLogWorker, this);
    }
}

ConsoleLogger::~ConsoleLogger() {
    if (pImpl->asyncLogging && pImpl->asyncThread.joinable()) {
        pImpl->stopAsyncThread = true;
        pImpl->queueCondition.notify_one();
        pImpl->asyncThread.join();
    }
}

void ConsoleLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    if (pImpl->asyncLogging) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        pImpl->logQueue.push(entry);
        pImpl->queueCondition.notify_one();
    } else {
        syncLog(entry);
    }
}

void ConsoleLogger::syncLog(const LogEntry& entry) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::string formatted = formatMessage(entry);
    
    // Console output
    if (pImpl->useColors) {
        std::cout << levelToColor(entry.level) << formatted << "\033[0m\n";
    } else {
        std::cout << formatted << "\n";
    }
    
    // File output if set
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut << formatted << "\n";
        pImpl->fileOut.flush();
        pImpl->currentFileSize += formatted.length() + 1;
        
        // Check for log rotation
        if (pImpl->currentFileSize >= pImpl->maxFileSize) {
            rotateLogFile();
        }
    }
}

void ConsoleLogger::asyncLogWorker() {
    while (!pImpl->stopAsyncThread) {
        std::unique_lock<std::mutex> lock(pImpl->mutex);
        
        if (pImpl->logQueue.empty()) {
            pImpl->queueCondition.wait_for(lock, std::chrono::milliseconds(100));
            continue;
        }
        
        LogEntry entry = pImpl->logQueue.front();
        pImpl->logQueue.pop();
        lock.unlock();
        
        syncLog(entry);
    }
}

void ConsoleLogger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->level = level;
}

LogLevel ConsoleLogger::getLevel() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->level;
}

void ConsoleLogger::setUseColors(bool useColors) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->useColors = useColors;
}

bool ConsoleLogger::getUseColors() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->useColors;
}

void ConsoleLogger::setOutputFile(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Close existing file
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
    
    // Open new file
    pImpl->fileOut.open(filepath, std::ios::app);
    if (pImpl->fileOut.is_open()) {
        pImpl->filepath = filepath;
        pImpl->currentFileSize = pImpl->fileOut.tellp();
    }
}

void ConsoleLogger::closeOutputFile() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

void ConsoleLogger::setMaxFileSize(size_t maxBytes) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->maxFileSize = maxBytes;
}

size_t ConsoleLogger::getMaxFileSize() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->maxFileSize;
}

void ConsoleLogger::rotateLogFile() {
    if (!pImpl->fileOut.is_open() || pImpl->filepath.empty()) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->fileOut.close();
    
    // Create backup filename
    size_t pos = pImpl->filepath.find_last_of('.');
    std::string backupPath = pImpl->filepath;
    if (pos != std::string::npos) {
        backupPath.insert(pos, "_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count()));
    }
    
    // Rename current file
    std::rename(pImpl->filepath.c_str(), backupPath.c_str());
    
    // Reopen file
    pImpl->fileOut.open(pImpl->filepath, std::ios::app);
    if (pImpl->fileOut.is_open()) {
        pImpl->currentFileSize = 0;
    }
}

void ConsoleLogger::setFormat(const std::string& format) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->format = format;
}

std::string ConsoleLogger::getFormat() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->format;
}

void ConsoleLogger::enableAsyncLogging(bool enable) {
    if (pImpl->asyncLogging != enable) {
        pImpl->asyncLogging = enable;
        if (!enable && pImpl->asyncThread.joinable()) {
            pImpl->stopAsyncThread = true;
            pImpl->queueCondition.notify_one();
            pImpl->asyncThread.join();
            pImpl->stopAsyncThread = false;
        } else if (enable && !pImpl->asyncThread.joinable()) {
            pImpl->asyncThread = std::thread(&ConsoleLogger::asyncLogWorker, this);
        }
    }
}

bool ConsoleLogger::isAsyncLoggingEnabled() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->asyncLogging;
}

std::string ConsoleLogger::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Critical: return "CRIT";
        default: return "UNKNOWN";
    }
}

std::string ConsoleLogger::levelToColor(LogLevel level) const {
    switch (level) {
        case LogLevel::Debug: return "\033[36m";    // Cyan
        case LogLevel::Info: return "\033[32m";      // Green
        case LogLevel::Warning: return "\033[33m";   // Yellow
        case LogLevel::Error: return "\033[31m";     // Red
        case LogLevel::Critical: return "\033[35m"; // Magenta
        default: return "\033[0m";
    }
}

std::string ConsoleLogger::formatMessage(const LogEntry& entry) const {
    std::string result = pImpl->format;
    
    // Replace placeholders
    size_t pos = 0;
    while ((pos = result.find("%level")) != std::string::npos) {
        result.replace(pos, 7, levelToString(entry.level));
    }
    
    while ((pos = result.find("%message")) != std::string::npos) {
        result.replace(pos, 9, entry.message);
    }
    
    while ((pos = result.find("%timestamp")) != std::string::npos) {
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        char timestamp[20];
        std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", std::localtime(&time));
        result.replace(pos, 12, timestamp);
    }
    
    while ((pos = result.find("%file")) != std::string::npos) {
        result.replace(pos, 6, entry.file);
    }
    
    while ((pos = result.find("%line")) != std::string::npos) {
        result.replace(pos, 6, std::to_string(entry.line));
    }
    
    while ((pos = result.find("%function")) != std::string::npos) {
        result.replace(pos, 10, entry.function);
    }
    
    return result;
}

// Logger class implementation
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {}

Logger::~Logger() = default;

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
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

void Logger::log(LogLevel level, const std::string& message,
                const std::string& file, int line, const std::string& function) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    LogEntry entry;
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

void Logger::debug(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Debug, message, file, line, function);
}

void Logger::info(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Info, message, file, line, function);
}

void Logger::warning(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Warning, message, file, line, function);
}

void Logger::error(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Error, message, file, line, function);
}

void Logger::critical(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Critical, message, file, line, function);
}

bool Logger::isEnabled(LogLevel level) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        if (logger->getLevel() <= level) {
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

// LogStream implementation
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function) {}

LogStream::~LogStream() {
    Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
}

} // namespace nlm
