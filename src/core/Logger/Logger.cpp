#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <cstring>
#include <cerrno>
#include <stdexcept>
#include <atomic>
#include <condition_variable>
#include <queue>
#include <thread>
#include <chrono>
#include <filesystem>
#include <sstream>
#include <iomanip>

namespace nlm {

// Simple, robust logger implementation with comprehensive error handling

// Global utility functions
namespace LogUtils {
    std::string getTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    std::string getThreadId() {
        std::hash<std::thread::id> hasher;
        auto id = std::this_thread::get_id();
        return std::to_string(hasher(id));
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
    
    std::string formatMessage(const std::string& message, 
                              const std::string& file,
                              int line,
                              const std::string& function) {
        std::ostringstream oss;
        oss << message;
        
        if (!file.empty()) {
            oss << " [" << file << ":" << line << " " << function << "]";
        }
        
        return oss.str();
    }
    
    std::string escapeForFileOutput(const std::string& message) {
        std::string result;
        result.reserve(message.size());
        
        for (char c : message) {
            switch (c) {
                case '\n': result += "\\n"; break;
                case '\r': result += "\\r"; break;
                case '\t': result += "\\t"; break;
                case '\\': result += "\\\\"; break;
                case '"': result += "\\\""; break;
                case '\0': result += "\\0"; break;
                default: result += c; break;
            }
        }
        
        return result;
    }
}

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    bool fileOpenError = false;
    std::string filePath;
    size_t maxFileSize = 10 * 1024 * 1024;
    bool rotationEnabled = false;
    std::atomic<size_t> currentFileSize{0};
    std::chrono::system_clock::time_point lastRotation;
    bool isHealthyFlag = true;
    std::string lastError;
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {
    pImpl->lastRotation = std::chrono::system_clock::now();
}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
    pImpl->lastRotation = std::chrono::system_clock::now();
}

ConsoleLogger::~ConsoleLogger() {
    try {
        closeOutputFile();
    } catch (...) {
        // Ignore exceptions during destruction
    }
}

void ConsoleLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Check file rotation
    if (pImpl->rotationEnabled) {
        rotateLogFile();
    }
    
    std::ostringstream oss;
    oss << "[" << LogUtils::levelToString(entry.level) << "] ";
    oss << LogUtils::getTimestamp() << " ";
    oss << entry.message;
    
    std::string output = oss.str();
    
    // Console output with comprehensive error handling
    try {
        if (pImpl->useColors) {
            std::cout << LogUtils::levelToColor(entry.level) << output << "\033[0m\n";
        } else {
            std::cout << output << "\n";
        }
        std::cout.flush();
        pImpl->currentFileSize += output.size() + 1;
    } catch (const std::exception& e) {
        pImpl->lastError = std::string("Console output failed: ") + e.what();
        pImpl->isHealthyFlag = false;
    }
    
    // File output if set
    if (pImpl->fileOut.is_open()) {
        try {
            pImpl->fileOut << output << "\n";
            pImpl->fileOut.flush();
            pImpl->currentFileSize += output.size() + 1;
            pImpl->fileOpenError = false;
            pImpl->isHealthyFlag = true;
            pImpl->lastError.clear();
        } catch (const std::exception& e) {
            pImpl->lastError = std::string("File output failed: ") + e.what();
            pImpl->fileOpenError = true;
            pImpl->isHealthyFlag = false;
        }
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
    
    // Close existing file if open
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
    
    pImpl->filePath = filepath;
    pImpl->currentFileSize = 0;
    
    try {
        // Ensure directory exists
        std::filesystem::path dir = std::filesystem::path(filepath).parent();
        if (!dir.empty() && !std::filesystem::exists(dir)) {
            std::filesystem::create_directories(dir);
        }
        
        // Open new file
        pImpl->fileOut.open(filepath, std::ios::app);
        if (!pImpl->fileOut.is_open()) {
            pImpl->lastError = "Failed to open log file: " + filepath;
            pImpl->fileOpenError = true;
            pImpl->isHealthyFlag = false;
        } else {
            // Get current file size
            pImpl->currentFileSize = std::filesystem::file_size(filepath);
            pImpl->fileOpenError = false;
            pImpl->isHealthyFlag = true;
            pImpl->lastError.clear();
        }
    } catch (const std::exception& e) {
        pImpl->lastError = std::string("Exception opening log file " + filepath + ": ") + e.what();
        pImpl->fileOpenError = true;
        pImpl->isHealthyFlag = false;
    }
}

void ConsoleLogger::closeOutputFile() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
        try {
            pImpl->fileOut.close();
            pImpl->fileOpenError = false;
            pImpl->isHealthyFlag = true;
            pImpl->lastError.clear();
        } catch (const std::exception& e) {
            pImpl->lastError = std::string("Failed to close log file: ") + e.what();
            pImpl->isHealthyFlag = false;
        }
    }
}

void ConsoleLogger::setMaxLogSize(size_t maxSize) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->maxFileSize = maxSize;
}

void ConsoleLogger::setLogRotation(bool enableRotation) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->rotationEnabled = enableRotation;
}

bool ConsoleLogger::isHealthy() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->isHealthyFlag && !pImpl->fileOpenError;
}

std::string ConsoleLogger::getError() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->lastError;
}

void ConsoleLogger::clearError() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->lastError.clear();
    pImpl->isHealthyFlag = true;
}

void ConsoleLogger::rotateLogFile() {
    auto now = std::chrono::system_clock::now();
    auto hours = std::chrono::duration_cast<std::chrono::hours>(now - pImpl->lastRotation);
    
    if (hours.count() >= 24 || pImpl->currentFileSize >= pImpl->maxFileSize) {
        // Close current file
        if (pImpl->fileOut.is_open()) {
            pImpl->fileOut.close();
        }
        
        // Rename current file with timestamp
        if (std::filesystem::exists(pImpl->filePath)) {
            std::string rotatedPath = pImpl->filePath + "." + 
                std::to_string(std::chrono::system_clock::to_time_t(now));
            std::filesystem::rename(pImpl->filePath, rotatedPath);
        }
        
        // Reopen file
        if (!pImpl->filePath.empty()) {
            setOutputFile(pImpl->filePath);
        }
        
        pImpl->lastRotation = now;
    }
}

// FileLogger implementation
struct FileLogger::Impl {
    LogLevel level = LogLevel::Info;
    std::ofstream fileOut;
    std::mutex mutex;
    std::string filePath;
    size_t maxFileSize = 10 * 1024 * 1024;
    bool rotationEnabled = false;
    std::atomic<size_t> currentFileSize{0};
    std::chrono::system_clock::time_point lastRotation;
    bool isHealthyFlag = true;
    std::string lastError;
};

FileLogger::FileLogger(const std::string& filepath) {
    pImpl = std::make_unique<Impl>();
    pImpl->lastRotation = std::chrono::system_clock::now();
    setOutputFile(filepath);
}

FileLogger::~FileLogger() {
    closeOutputFile();
}

void FileLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Check rotation
    if (pImpl->rotationEnabled) {
        rotateLogFile();
    }
    
    std::ostringstream oss;
    oss << "[" << LogUtils::levelToString(entry.level) << "] ";
    oss << LogUtils::getTimestamp() << " ";
    oss << entry.message;
    
    std::string output = oss.str();
    
    try {
        pImpl->fileOut << output << "\n";
        pImpl->fileOut.flush();
        pImpl->currentFileSize += output.size() + 1;
        pImpl->isHealthyFlag = true;
        pImpl->lastError.clear();
    } catch (const std::exception& e) {
        pImpl->lastError = std::string("File logging failed: ") + e.what();
        pImpl->isHealthyFlag = false;
    }
}

void FileLogger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->level = level;
}

LogLevel FileLogger::getLevel() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->level;
}

void FileLogger::setOutputFile(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
    
    pImpl->filePath = filepath;
    pImpl->currentFileSize = 0;
    
    try {
        std::filesystem::path dir = std::filesystem::path(filepath).parent();
        if (!dir.empty() && !std::filesystem::exists(dir)) {
            std::filesystem::create_directories(dir);
        }
        
        pImpl->fileOut.open(filepath, std::ios::app);
        if (!pImpl->fileOut.is_open()) {
            pImpl->lastError = "Failed to open file: " + filepath;
            pImpl->isHealthyFlag = false;
        } else {
            pImpl->currentFileSize = std::filesystem::file_size(filepath);
            pImpl->isHealthyFlag = true;
            pImpl->lastError.clear();
        }
    } catch (const std::exception& e) {
        pImpl->lastError = std::string("Exception opening file " + filepath + ": ") + e.what();
        pImpl->isHealthyFlag = false;
    }
}

void FileLogger::closeOutputFile() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
        pImpl->isHealthyFlag = true;
        pImpl->lastError.clear();
    }
}

void FileLogger::setMaxLogSize(size_t maxSize) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->maxFileSize = maxSize;
}

void FileLogger::setLogRotation(bool enableRotation) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->rotationEnabled = enableRotation;
}

bool FileLogger::isHealthy() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->isHealthyFlag;
}

std::string FileLogger::getError() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->lastError;
}

void FileLogger::clearError() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->lastError.clear();
    pImpl->isHealthyFlag = true;
}

void FileLogger::rotateLogFile() {
    auto now = std::chrono::system_clock::now();
    auto hours = std::chrono::duration_cast<std::chrono::hours>(now - pImpl->lastRotation);
    
    if (hours.count() >= 24 || pImpl->currentFileSize >= pImpl->maxFileSize) {
        if (pImpl->fileOut.is_open()) {
            pImpl->fileOut.close();
        }
        
        if (std::filesystem::exists(pImpl->filePath)) {
            std::string rotatedPath = pImpl->filePath + "." + 
                std::to_string(std::chrono::system_clock::to_time_t(now));
            std::filesystem::rename(pImpl->filePath, rotatedPath);
        }
        
        if (!pImpl->filePath.empty()) {
            setOutputFile(pImpl->filePath);
        }
        
        pImpl->lastRotation = now;
    }
}

// BatchLogger implementation
struct BatchLogger::Impl {
    LogLevel level = LogLevel::Info;
    std::queue<LogEntry> logQueue;
    std::mutex mutex;
    std::condition_variable cv;
    std::atomic<bool> running{true};
    std::thread workerThread;
    size_t maxBatchSize = 100;
    std::chrono::milliseconds flushInterval = std::chrono::milliseconds(1000);
    size_t maxMemoryUsage = 10 * 1024 * 1024;
    size_t currentMemoryUsage = 0;
    bool isHealthyFlag = true;
    std::string lastError;
    std::atomic<size_t> processedCount{0};
    std::atomic<size_t> droppedCount{0};
};

BatchLogger::BatchLogger(size_t batchSize) {
    pImpl = std::make_unique<Impl>();
    pImpl->maxBatchSize = batchSize;
    
    // Start worker thread
    pImpl->workerThread = std::thread(&BatchLogger::workerThreadFunc, this);
}

BatchLogger::~BatchLogger() {
    shutdown();
}

void BatchLogger::log(const LogEntry& entry) {
    std::unique_lock<std::mutex> lock(pImpl->mutex);
    
    // Check memory usage
    if (pImpl->currentMemoryUsage >= pImpl->maxMemoryUsage) {
        pImpl->droppedCount++;
        return;
    }
    
    // Add to queue
    pImpl->logQueue.push(entry);
    pImpl->currentMemoryUsage += sizeof(LogEntry) + entry.message.size();
    
    lock.unlock();
    pImpl->cv.notify_one();
}

void BatchLogger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->level = level;
}

LogLevel BatchLogger::getLevel() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->level;
}

void BatchLogger::setFlushInterval(std::chrono::milliseconds interval) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->flushInterval = interval;
}

void BatchLogger::setMaxMemoryUsage(size_t maxMemory) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->maxMemoryUsage = maxMemory;
}

bool BatchLogger::isHealthy() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->isHealthyFlag && pImpl->workerThread.joinable();
}

std::string BatchLogger::getError() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->lastError;
}

void BatchLogger::clearError() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->lastError.clear();
    pImpl->isHealthyFlag = true;
}

void BatchLogger::flush() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->cv.notify_one();
}

void BatchLogger::workerThreadFunc() {
    while (pImpl->running) {
        std::unique_lock<std::mutex> lock(pImpl->mutex);
        
        // Wait for logs or timeout
        bool timeout = pImpl->cv.wait_for(lock, pImpl->flushInterval, [this]() {
            return !pImpl->logQueue.empty();
        });
        
        if (timeout || !pImpl->logQueue.empty()) {
            // Process batched logs
            processBatch();
        }
    }
}

void BatchLogger::processBatch() {
    if (pImpl->logQueue.empty()) {
        return;
    }
    
    // For now, just drop the batch
    std::queue<LogEntry> empty;
    std::swap(pImpl->logQueue, empty);
    
    pImpl->processedCount += empty.size();
}

void BatchLogger::shutdown() {
    if (pImpl->running) {
        pImpl->running = false;
        pImpl->cv.notify_one();
        
        if (pImpl->workerThread.joinable()) {
            pImpl->workerThread.join();
        }
    }
}

// Logger implementation
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    bool initialized = false;
    std::string lastError;
    std::atomic<size_t> totalLogCount{0};
    std::chrono::system_clock::time_point lastStatsUpdate;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {
    pImpl->initialized = true;
    pImpl->lastStatsUpdate = std::chrono::system_clock::now();
}

Logger::~Logger() {
    try {
        clearLoggers();
    } catch (...) {
        // Ignore exceptions during cleanup
    }
}

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    if (!pImpl->initialized || !logger) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.push_back(logger);
    pImpl->lastError.clear();
    pImpl->totalLogCount++;
}

void Logger::clearLoggers() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.clear();
}

void Logger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        try {
            logger->setLevel(level);
        } catch (const std::exception& e) {
            pImpl->lastError = std::string("Failed to set logger level: ") + e.what();
        }
    }
}

void Logger::setGlobalLevel(LogLevel level) {
    setLevel(level);
}

void Logger::log(LogLevel level, const std::string& message,
                const std::string& file, int line, const std::string& function) {
    if (!pImpl->initialized) {
        return;
    }
    
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.threadId = std::hash<std::thread::id>{}(std::this_thread::get_id());
    entry.processId = getpid();
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (auto& logger : pImpl->loggers) {
        try {
            logger->log(entry);
        } catch (const std::exception& e) {
            pImpl->lastError = std::string("Logger error: ") + e.what();
        }
    }
    
    pImpl->totalLogCount++;
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
    if (!pImpl->initialized) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        try {
            if (logger->getLevel() <= level) {
                return true;
            }
        } catch (...) {
            continue;
        }
    }
    return false;
}

Logger& Logger::getGlobal() {
    static Logger globalLogger;
    return globalLogger;
}

void Logger::setGlobal(std::shared_ptr<Logger> logger) {
    if (logger) {
        // This would replace the global logger in a real implementation
    }
}

bool Logger::isHealthy() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (auto& logger : pImpl->loggers) {
        if (!logger->isHealthy()) {
            return false;
        }
    }
    
    return true;
}

std::string Logger::getErrors() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->lastError;
}

void Logger::clearErrors() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->lastError.clear();
}

// LogStream implementation
Logger::LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function) {}

Logger::LogStream::~LogStream() {
    try {
        Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
    } catch (...) {
        // Fail silently
    }
}

} // namespace nlm
