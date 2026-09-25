#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <algorithm>
#include <chrono>
#include <thread>
#include <functional>
#include <stdexcept>

namespace nlm {

// Console Logger Implementation
struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    std::string filename;
    bool hasFile = false;
    
    // ANSI color codes
    const std::string RESET = "\033[0m";
    const std::string RED = "\033[31m";
    const std::string GREEN = "\033[32m";
    const std::string YELLOW = "\033[33m";
    const std::string BLUE = "\033[34m";
    const std::string MAGENTA = "\033[35m";
    const std::string CYAN = "\033[36m";
    const std::string WHITE = "\033[37m";
};

ConsoleLogger::ConsoleLogger(LogLevel level, bool useColors) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
    pImpl->useColors = useColors;
}

ConsoleLogger::~ConsoleLogger() = default;

void ConsoleLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    
    // Add timestamp
    oss << "[" << entry.formatTimestamp() << "] ";
    
    // Add level
    oss << "[" << LogUtils::levelToString(entry.level) << "] ";
    
    // Add message
    oss << entry.message;
    
    std::string output = oss.str();
    
    // Console output
    if (pImpl->useColors) {
        std::cout << LogUtils::levelToColor(entry.level) << output << LogUtils::levelToColor(LogLevel::Info) << std::endl;
    } else {
        std::cout << output << std::endl;
    }
    
    // File output if set
    if (pImpl->hasFile && pImpl->fileOut.is_open()) {
        pImpl->fileOut << output << std::endl;
        pImpl->fileOut.flush();
    }
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
    
    // Close existing file if open
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
    
    pImpl->filename = filepath;
    pImpl->fileOut.open(filepath, std::ios::app);
    pImpl->hasFile = pImpl->fileOut.is_open();
    
    if (pImpl->hasFile) {
        // Write header if file is newly created
        if (pImpl->fileOut.tellp() == 0) {
            pImpl->fileOut << "=== NLM Log File ===" << std::endl;
            pImpl->fileOut << "Format: [timestamp] [level] message" << std::endl;
            pImpl->fileOut << std::endl;
        }
    }
}

void ConsoleLogger::closeOutputFile() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
    pImpl->hasFile = false;
}

bool ConsoleLogger::hasOutputFile() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->hasFile && pImpl->fileOut.is_open();
}

std::string ConsoleLogger::levelToString(LogLevel level) const {
    return LogUtils::levelToString(level);
}

std::string ConsoleLogger::levelToColor(LogLevel level) const {
    return LogUtils::levelToColor(level);
}

std::string ConsoleLogger::formatEntry(const LogEntry& entry) const {
    std::ostringstream oss;
    oss << "[" << entry.formatTimestamp() << "] ";
    oss << "[" << LogUtils::levelToString(entry.level) << "] ";
    oss << entry.message;
    return oss.str();
}

// Async Logger Implementation
struct AsyncLogger::Impl {
    std::queue<LogEntry> messageQueue;
    std::mutex mutex;
    std::condition_variable cv;
    std::vector<std::thread> workers;
    std::atomic<bool> stopFlag{false};
    size_t maxQueueSize = 10000;
    LogLevel level = LogLevel::Info;
    size_t processedCount = 0;
    std::chrono::system_clock::time_point lastFlush;
    
    // Worker thread function
    void workerFunction(AsyncLogger* asyncLogger) {
        std::unique_ptr<ConsoleLogger> consoleLogger = std::make_unique<ConsoleLogger>();
        
        while (!stopFlag.load(std::memory_order_acquire)) {
            LogEntry entry;
            {
                std::unique_lock<std::mutex> lock(mutex);
                
                // Wait for message or stop signal
                cv.wait(lock, [this]() {
                    return !messageQueue.empty() || stopFlag.load(std::memory_order_acquire);
                });
                
                if (stopFlag.load(std::memory_order_acquire) && messageQueue.empty()) {
                    break;
                }
                
                entry = std::move(messageQueue.front());
                messageQueue.pop();
            }
            
            // Process the log entry
            consoleLogger->log(entry);
            
            {
                std::lock_guard<std::mutex> lock(mutex);
                processedCount++;
            }
        }
    }
};

AsyncLogger::AsyncLogger(size_t maxQueueSize, size_t numThreads, LogLevel level) 
    : BaseLogger(level) {
    pImpl->maxQueueSize = maxQueueSize;
    pImpl->level = level;
    
    // Create worker threads
    for (size_t i = 0; i < numThreads; ++i) {
        pImpl->workers.emplace_back(&AsyncLogger::Impl::workerFunction, this, this);
    }
    
    pImpl->lastFlush = std::chrono::system_clock::now();
}

AsyncLogger::~AsyncLogger() {
    shutdown();
}

void AsyncLogger::log(const LogEntry& entry) {
    if (!isEnabled(entry.level)) {
        return;
    }
    
    {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        // Wait if queue is full
        if (pImpl->messageQueue.size() >= pImpl->maxQueueSize) {
            cv.wait(lock, [this]() {
                return pImpl->messageQueue.size() < pImpl->maxQueueSize || pImpl->isStopping();
            });
        }
        
        // Add entry to queue
        pImpl->messageQueue.push(entry);
    }
    
    // Notify worker threads
    pImpl->cv.notify_one();
}

void AsyncLogger::flush() {
    std::unique_lock<std::mutex> lock(pImpl->mutex);
    pImpl->cv.wait(lock, [this]() {
        return pImpl->messageQueue.empty();
    });
}

void AsyncLogger::shutdown() noexcept {
    if (pImpl->isStopping()) {
        return;
    }
    
    // Set stop flag
    {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        stopFlag.store(true, std::memory_order_release);
    }
    
    // Notify all workers
    pImpl->cv.notify_all();
    
    // Join all threads
    for (auto& worker : pImpl->workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

bool AsyncLogger::isStopping() const noexcept {
    return stopFlag.load(std::memory_order_acquire);
}

// File Logger Implementation
struct FileLogger::Impl {
    std::string filepath;
    std::ofstream fileOut;
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    bool enableRotation = true;
    size_t maxFileSize = 10 * 1024 * 1024; // 10MB
    size_t maxFiles = 5;
    bool appendMode = true;
    bool hasFile = false;
    size_t currentFileIndex = 0;
    
    std::mutex mutex;
};

FileLogger::FileLogger(const std::string& filepath, bool enableRotation, 
                       size_t maxFileSize, size_t maxFiles, LogLevel level)
    : BaseLogger(level) {
    pImpl->filepath = filepath;
    pImpl->enableRotation = enableRotation;
    pImpl->maxFileSize = maxFileSize;
    pImpl->maxFiles = maxFiles;
    pImpl->level = level;
    pImpl->appendMode = true;
    
    // Open file for appending
    pImpl->fileOut.open(filepath, std::ios::app);
    pImpl->hasFile = pImpl->fileOut.is_open();
    
    if (pImpl->hasFile) {
        // Write header if file is newly created
        if (pImpl->fileOut.tellp() == 0) {
            pImpl->fileOut << "=== NLM Log File ===" << std::endl;
            pImpl->fileOut << "Format: [timestamp] [level] message" << std::endl;
            pImpl->fileOut << std::endl;
        }
    }
}

FileLogger::~FileLogger() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

void FileLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Check if we need to rotate
    if (pImpl->enableRotation && pImpl->shouldRotate(pImpl->fileOut.tellp())) {
        rotate();
    }
    
    // Format and write entry
    std::ostringstream oss;
    oss << "[" << entry.formatTimestamp() << "] ";
    oss << "[" << LogUtils::levelToString(entry.level) << "] ";
    oss << entry.message << std::endl;
    
    pImpl->fileOut << oss.str();
    pImpl->fileOut.flush();
}

void FileLogger::setUseColors(bool useColors) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->useColors = useColors;
}

bool FileLogger::getUseColors() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->useColors;
}

void FileLogger::rotate() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->hasFile || !pImpl->fileOut.is_open()) {
        return;
    }
    
    // Close current file
    pImpl->fileOut.close();
    
    // Delete oldest files if we have too many
    if (pImpl->maxFiles > 0) {
        for (size_t i = pImpl->maxFiles; i < 100; ++i) { // Check up to 100 files
            std::string oldFile = generateFileName(pImpl->filepath, i);
            if (std::filesystem::exists(oldFile)) {
                std::filesystem::remove(oldFile);
            } else {
                break;
            }
        }
    }
    
    // Shift existing files
    if (pImpl->maxFiles > 0) {
        for (int i = pImpl->maxFiles - 1; i >= 0; --i) {
            std::string src = generateFileName(pImpl->filepath, i);
            std::string dst = generateFileName(pImpl->filepath, i + 1);
            
            if (std::filesystem::exists(src)) {
                std::filesystem::rename(src, dst);
            }
        }
    }
    
    // Open new current file
    std::string newFile = generateFileName(pImpl->filepath, 0);
    pImpl->fileOut.open(newFile, std::ios::app);
    pImpl->hasFile = pImpl->fileOut.is_open();
    
    if (pImpl->hasFile && pImpl->fileOut.tellp() == 0) {
        pImpl->fileOut << "=== NLM Log File ===" << std::endl;
        pImpl->fileOut << "Format: [timestamp] [level] message" << std::endl;
        pImpl->fileOut << std::endl;
    }
}

std::string FileLogger::getCurrentFilePath() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return generateFileName(pImpl->filepath, 0);
}

void FileLogger::setAppendMode(bool append) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->appendMode = append;
    
    if (pImpl->hasFile && pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
        pImpl->fileOut.open(pImpl->filepath, append ? std::ios::app : std::ios::trunc);
        pImpl->hasFile = pImpl->fileOut.is_open();
    }
}

std::string FileLogger::levelToString(LogLevel level) const {
    return LogUtils::levelToString(level);
}

std::string FileLogger::formatEntry(const LogEntry& entry) const {
    std::ostringstream oss;
    oss << "[" << entry.formatTimestamp() << "] ";
    oss << "[" << LogUtils::levelToString(entry.level) << "] ";
    oss << entry.message << std::endl;
    return oss.str();
}

std::string FileLogger::generateFileName(const std::string& basePath, int index) const {
    if (index == 0) {
        return basePath;
    } else {
        return basePath + "." + std::to_string(index);
    }
}

bool FileLogger::shouldRotate(size_t currentSize) const {
    if (!pImpl->enableRotation) {
        return false;
    }
    
    if (currentSize >= pImpl->maxFileSize) {
        return true;
    }
    
    // Check if we have too many backup files
    if (pImpl->maxFiles > 0) {
        size_t fileCount = 0;
        for (int i = 0; i < 100; ++i) {
            std::string filepath = generateFileName(pImpl->filepath, i);
            if (std::filesystem::exists(filepath)) {
                fileCount++;
            } else {
                break;
            }
        }
        if (fileCount >= pImpl->maxFiles) {
            return true;
        }
    }
    
    return false;
}

// Multi-Logger Implementation
struct MultiLogger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    LogLevel level = LogLevel::Info;
    std::mutex mutex;
    bool shutdownFlag = false;
};

MultiLogger::MultiLogger(LogLevel level) : BaseLogger(level) {
    pImpl->level = level;
}

MultiLogger::~MultiLogger() {
    shutdown();
}

void MultiLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (const auto& logger : pImpl->loggers) {
        if (!pImpl->shutdownFlag && logger->isEnabled(entry.level)) {
            logger->log(entry);
        }
    }
}

void MultiLogger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->level = level;
    
    // Propagate level to all child loggers
    for (const auto& logger : pImpl->loggers) {
        logger->setLevel(level);
    }
}

LogLevel MultiLogger::getLevel() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->level;
}

void MultiLogger::addConsoleLogger(LogLevel level, bool useColors) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (!pImpl->shutdownFlag) {
        auto consoleLogger = std::make_shared<ConsoleLogger>(level, useColors);
        pImpl->loggers.push_back(consoleLogger);
    }
}

void MultiLogger::addFileLogger(const std::string& filepath, bool enableRotation,
                               size_t maxFileSize, size_t maxFiles, LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (!pImpl->shutdownFlag) {
        auto fileLogger = std::make_shared<FileLogger>(filepath, enableRotation, maxFileSize, maxFiles, level);
        pImpl->loggers.push_back(fileLogger);
    }
}

void MultiLogger::addAsyncLogger(size_t maxQueueSize, size_t numThreads, LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (!pImpl->shutdownFlag) {
        auto asyncLogger = std::make_shared<AsyncLogger>(maxQueueSize, numThreads, level);
        pImpl->loggers.push_back(asyncLogger);
    }
}

void MultiLogger::clearLoggers() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.clear();
}

const std::vector<std::shared_ptr<ILogger>>& MultiLogger::getLoggers() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->loggers;
}

void MultiLogger::shutdown() noexcept {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->shutdownFlag = true;
    
    for (const auto& logger : pImpl->loggers) {
        logger->shutdown();
    }
    
    pImpl->loggers.clear();
}

// Logger Implementation
struct Logger::Impl {
    std::shared_ptr<ILogger> globalLogger;
    std::mutex mutex;
};

Logger::Logger() {
    pImpl = std::make_unique<Impl>();
    pImpl->globalLogger = std::make_shared<MultiLogger>();
}

Logger::~Logger() {
    if (pImpl->globalLogger) {
        pImpl->globalLogger->shutdown();
    }
}

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->globalLogger) {
        static_cast<MultiLogger*>(pImpl->globalLogger.get())->addLogger(logger);
    }
}

void Logger::clearLoggers() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->globalLogger) {
        static_cast<MultiLogger*>(pImpl->globalLogger.get())->clearLoggers();
    }
}

void Logger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->globalLogger) {
        pImpl->globalLogger->setLevel(level);
    }
}

void Logger::log(LogLevel level, const std::string& message,
                 const std::string& file, int line, const std::string& function) {
    LogEntry entry(level, message, file, line, function);
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->globalLogger && pImpl->globalLogger->isEnabled(level)) {
        pImpl->globalLogger->log(entry);
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
    if (pImpl->globalLogger) {
        return pImpl->globalLogger->isEnabled(level);
    }
    return false;
}

Logger& Logger::getGlobal() {
    static Logger globalLogger;
    return globalLogger;
}

void Logger::setGlobal(std::shared_ptr<Logger> logger) {
    if (logger) {
        std::lock_guard<std::mutex> lock(getGlobal().pImpl->mutex);
        getGlobal().pImpl->globalLogger = logger;
    }
}

void Logger::setupDefault() {
    clearLoggers();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info, true);
    addLogger(consoleLogger);
}

void Logger::setupMinimal() {
    clearLoggers();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Warning, false);
    addLogger(consoleLogger);
}

void Logger::setupSilent() {
    clearLoggers();
}

void Logger::setupFile(const std::string& filepath, bool rotation) {
    clearLoggers();
    auto fileLogger = std::make_shared<FileLogger>(filepath, rotation);
    addLogger(fileLogger);
}

void Logger::setupConsole(LogLevel level, bool colors) {
    clearLoggers();
    auto consoleLogger = std::make_shared<ConsoleLogger>(level, colors);
    addLogger(consoleLogger);
}

// Utility functions
namespace LogUtils {
    std::string formatTimestamp(const std::chrono::system_clock::time_point& timestamp) {
        auto time = std::chrono::system_clock::to_time_t(timestamp);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    std::string levelToString(LogLevel level) {
        switch (level) {
            case LogLevel::Debug: return "DEBUG";
            case LogLevel::Info: return "INFO";
            case LogLevel::Warning: return "WARN";
            case LogLevel::Error: return "ERROR";
            case LogLevel::Critical: return "CRIT";
            default: return "UNKNOWN";
        }
    }
    
    std::string levelToColor(LogLevel level) {
        switch (level) {
            case LogLevel::Debug: return "\033[36m";    // Cyan
            case LogLevel::Info: return "\033[32m";      // Green
            case LogLevel::Warning: return "\033[33m";   // Yellow
            case LogLevel::Error: return "\033[31m";     // Red
            case LogLevel::Critical: return "\033[35m"; // Magenta
            default: return "\033[0m";
        }
    }
    
    std::string escapeLogMessage(const std::string& message) {
        std::string result;
        for (char c : message) {
            switch (c) {
                case '\\': result += "\\\\"; break;
                case '\"': result += "\\\""; break;
                case '\b': result += "\\b"; break;
                case '\f': result += "\\f"; break;
                case '\n': result += "\\n"; break;
                case '\r': result += "\\r"; break;
                case '\t': result += "\\t"; break;
                default:
                    if (c < 32 || c > 126) {
                        result += "\\u" + std::to_string(static_cast<int>(c));
                    } else {
                        result += c;
                    }
                    break;
            }
        }
        return result;
    }
    
    bool containsControlChars(const std::string& str) {
        for (char c : str) {
            if (c < 32 && c != '\t' && c != '\n' && c != '\r') {
                return true;
            }
        }
        return false;
    }
    
    std::chrono::system_clock::time_point now() {
        return std::chrono::system_clock::now();
    }
}

} // namespace nlm