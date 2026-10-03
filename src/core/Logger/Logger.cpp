#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <thread>
#include <queue>
#include <condition_variable>
#include <filesystem>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <iomanip>

namespace nlm {

// Console Logger implementation with rotation support
struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    
    // Rotation configuration
    size_t maxFileSize = 10 * 1024 * 1024; // 10 MB
    size_t maxFiles = 5;
    bool compress = false;
    std::string lastFilename;
    size_t currentFileSize = 0;
    std::chrono::system_clock::time_point lastRotationTime;
    
    // Current timestamp formatting
    std::tm getCurrentTime() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        return *std::localtime(&time);
    }
    
    void rotateFileIfNeeded(const std::string& basePath) {
        if (fileOut.is_open()) {
            fileOut.close();
        }
        
        // Check if we need to rotate
        if (currentFileSize >= maxFileSize) {
            // Rotate existing files
            for (size_t i = maxFiles - 1; i > 0; --i) {
                std::string src = basePath + "." + std::to_string(i - 1);
                std::string dst = basePath + "." + std::to_string(i);
                if (std::filesystem::exists(src)) {
                    std::filesystem::rename(src, dst);
                }
            }
            
            // Create new log file
            std::string newPath = basePath + ".0";
            fileOut.open(newPath, std::ios::app);
            currentFileSize = 0;
            lastRotationTime = std::chrono::system_clock::now();
        }
    }
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
}

ConsoleLogger::~ConsoleLogger() = default;

void ConsoleLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    
    // Add timestamp
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    
    // Add thread ID
    oss << " [" << entry.threadId << "]";
    
    oss << " " << entry.message;
    
    std::string output = oss.str();
    
    // Console output
    if (pImpl->useColors) {
        std::cout << levelToColor(entry.level) << output << "\033[0m\n";
    } else {
        std::cout << output << "\n";
    }
    
    // File output if set
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut << output << "\n";
        pImpl->fileOut.flush();
        pImpl->currentFileSize += output.size() + 1; // +1 for newline
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

void ConsoleLogger::flush() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
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

void ConsoleLogger::configureRotation(const LogRotationConfig& config) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->maxFileSize = config.maxFileSize;
    pImpl->maxFiles = config.maxFiles;
    pImpl->compress = config.compress;
}

void ConsoleLogger::setOutputFile(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->fileOut.close();
    pImpl->fileOut.open(filepath, std::ios::app);
    pImpl->currentFileSize = std::filesystem::exists(filepath) ? 
        std::filesystem::file_size(filepath) : 0;
    pImpl->lastFilename = filepath;
}

void ConsoleLogger::closeOutputFile() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

void ConsoleLogger::clear() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    closeOutputFile();
    pImpl->currentFileSize = 0;
}

void ConsoleLogger::setMaxLogSize(size_t maxSize) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->maxFileSize = maxSize;
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

std::string ConsoleLogger::formatTimestamp(const system_clock::time_point& timestamp) const {
    auto time = system_clock::to_time_t(timestamp);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// Async Logger implementation
struct AsyncLogger::Impl {
    LogLevel level = LogLevel::Info;
    std::queue<LogEntry> logQueue;
    std::mutex queueMutex;
    std::condition_variable queueCondition;
    std::vector<std::thread> workerThreads;
    bool running = true;
    size_t queueSize = 10000;
    std::atomic<size_t> processedCount = 0;
    std::atomic<size_t> queuedCount = 0;
    
    std::unique_ptr<ConsoleLogger> consoleLogger;
};

AsyncLogger::AsyncLogger(size_t queueSize) : pImpl(std::make_unique<Impl>()) {
    pImpl->queueSize = queueSize;
    pImpl->consoleLogger = std::make_unique<ConsoleLogger>();
    
    // Start worker threads
    size_t numThreads = std::thread::hardware_concurrency();
    if (numThreads == 0) numThreads = 2;
    
    for (size_t i = 0; i < numThreads; ++i) {
        pImpl->workerThreads.emplace_back([this]() {
            while (pImpl->running) {
                std::unique_lock<std::mutex> lock(pImpl->queueMutex);
                
                // Wait for log entry or shutdown
                pImpl->queueCondition.wait(lock, [this]() {
                    return !pImpl->logQueue.empty() || !pImpl->running;
                });
                
                if (!pImpl->running && pImpl->logQueue.empty()) {
                    break;
                }
                
                // Process log entry
                LogEntry entry = std::move(pImpl->logQueue.front());
                pImpl->logQueue.pop();
                lock.unlock();
                
                pImpl->consoleLogger->log(entry);
                pImpl->processedCount++;
            }
        });
    }
}

AsyncLogger::~AsyncLogger() {
    {
        std::lock_guard<std::mutex> lock(pImpl->queueMutex);
        pImpl->running = false;
    }
    
    pImpl->queueCondition.notify_all();
    
    for (auto& thread : pImpl->workerThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

void AsyncLogger::log(const LogEntry& entry) {
    {
        std::lock_guard<std::mutex> lock(pImpl->queueMutex);
        
        // Wait if queue is full
        pImpl->queueCondition.wait(lock, [this]() {
            return pImpl->logQueue.size() < pImpl->queueSize;
        });
        
        pImpl->logQueue.push(entry);
        pImpl->queuedCount++;
    }
    
    pImpl->queueCondition.notify_one();
}

void AsyncLogger::setLevel(LogLevel level) {
    pImpl->level = level;
}

LogLevel AsyncLogger::getLevel() const {
    return pImpl->level;
}

void AsyncLogger::flush() {
    // Wait for queue to empty
    std::unique_lock<std::mutex> lock(pImpl->queueMutex);
    pImpl->queueCondition.wait(lock, [this]() {
        return pImpl->logQueue.empty();
    });
    
    pImpl->consoleLogger->flush();
}

void AsyncLogger::setWorkerThreads(size_t threads) {
    // Note: This would require stopping and restarting workers
    // For simplicity, we keep the initial thread count
}

void AsyncLogger::setQueueSize(size_t size) {
    std::lock_guard<std::mutex> lock(pImpl->queueMutex);
    pImpl->queueSize = size;
    pImpl->queueCondition.notify_all();
}

// File Logger implementation
struct FileLogger::Impl {
    LogLevel level = LogLevel::Info;
    std::ofstream fileOut;
    std::mutex mutex;
    
    // Rotation configuration
    size_t maxFileSize = 10 * 1024 * 1024; // 10 MB
    size_t maxFiles = 5;
    bool compress = false;
    std::string basePath;
    size_t currentFileSize = 0;
    std::chrono::system_clock::time_point lastRotationTime;
    
    // File management
    std::string getCurrentLogFile() const {
        if (!fileOut.is_open()) return "";
        
        std::filesystem::path path(basePath);
        return path.filename().string();
    }
    
    void rotateFileIfNeeded() {
        if (fileOut.is_open()) {
            fileOut.close();
        }
        
        // Check if we need to rotate
        if (currentFileSize >= maxFileSize) {
            // Rotate existing files
            for (size_t i = maxFiles - 1; i > 0; --i) {
                std::string src = basePath + "." + std::to_string(i - 1);
                std::string dst = basePath + "." + std::to_string(i);
                if (std::filesystem::exists(src)) {
                    std::filesystem::rename(src, dst);
                }
            }
            
            // Create new log file
            std::string newPath = basePath + ".0";
            fileOut.open(newPath, std::ios::app);
            currentFileSize = 0;
            lastRotationTime = std::chrono::system_clock::now();
        }
    }
};

FileLogger::FileLogger() : pImpl(std::make_unique<Impl>()) {}

FileLogger::FileLogger(const std::string& filepath) : pImpl(std::make_unique<Impl>()) {
    setOutputFile(filepath);
}

FileLogger::~FileLogger() = default;

void FileLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->rotateFileIfNeeded();
    
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    
    // Add timestamp
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    
    // Add thread ID
    oss << " [" << entry.threadId << "]";
    
    oss << " " << entry.message;
    
    // Add metadata if present
    if (!entry.metadata.empty()) {
        oss << " [";
        bool first = true;
        for (const auto& meta : entry.metadata) {
            if (!first) oss << ", ";
            oss << meta.first << "=" << meta.second;
            first = false;
        }
        oss << "]";
    }
    
    std::string output = oss.str();
    
    fileOut << output << "\n";
    fileOut.flush();
    pImpl->currentFileSize += output.size() + 1; // +1 for newline
}

void FileLogger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->level = level;
}

LogLevel FileLogger::getLevel() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->level;
}

void FileLogger::flush() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (fileOut.is_open()) {
        fileOut.flush();
    }
}

void FileLogger::setOutputFile(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    fileOut.close();
    
    // Create directory if it doesn't exist
    std::filesystem::path path(filepath);
    std::filesystem::create_directories(path.parent_path());
    
    fileOut.open(filepath, std::ios::app);
    pImpl->basePath = filepath;
    pImpl->currentFileSize = std::filesystem::exists(filepath) ? 
        std::filesystem::file_size(filepath) : 0;
}

void FileLogger::setFileMode(std::ios_base::openmode mode) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    fileOut.close();
    fileOut.open(pImpl->basePath, mode);
}

void FileLogger::configureRotation(const LogRotationConfig& config) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->maxFileSize = config.maxFileSize;
    pImpl->maxFiles = config.maxFiles;
    pImpl->compress = config.compress;
}

void FileLogger::clear() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    fileOut.close();
    pImpl->currentFileSize = 0;
}

void FileLogger::setMaxLogSize(size_t maxSize) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->maxFileSize = maxSize;
}

size_t FileLogger::getLogFileSize() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->currentFileSize;
}

bool FileLogger::needsRotation() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->currentFileSize >= pImpl->maxFileSize;
}

std::string FileLogger::getCurrentLogFile() const {
    return pImpl->getCurrentLogFile();
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

// Main Logger implementation
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    
    // Statistics
    size_t totalLogCount = 0;
    size_t debugCount = 0;
    size_t infoCount = 0;
    size_t warningCount = 0;
    size_t errorCount = 0;
    size_t criticalCount = 0;
    
    std::chrono::system_clock::time_point startTime;
    
    Logger* globalInstance = nullptr;
    bool asyncEnabled = false;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {
    pImpl->startTime = std::chrono::system_clock::now();
    
    // Add default loggers
    auto consoleLogger = std::make_shared<ConsoleLogger>();
    consoleLogger->setLevel(LogLevel::Info);
    consoleLogger->setUseColors(true);
    addLogger(consoleLogger);
    
    // Add file logger by default
    auto fileLogger = std::make_shared<FileLogger>();
    fileLogger->setLevel(LogLevel::Info);
    fileLogger->configureRotation(LogRotationConfig{10 * 1024 * 1024, 5, false, "00:00"});
    addLogger(fileLogger);
}

Logger::~Logger() = default;

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.push_back(logger);
}

void Logger::removeLogger(const std::shared_ptr<ILogger>& logger) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = std::find(pImpl->loggers.begin(), pImpl->loggers.end(), logger);
    if (it != pImpl->loggers.end()) {
        pImpl->loggers.erase(it);
    }
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

void Logger::setConsoleLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        auto consoleLogger = std::dynamic_pointer_cast<ConsoleLogger>(logger);
        if (consoleLogger) {
            consoleLogger->setLevel(level);
        }
    }
}

void Logger::setFileLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        auto fileLogger = std::dynamic_pointer_cast<FileLogger>(logger);
        if (fileLogger) {
            fileLogger->setLevel(level);
        }
    }
}

void Logger::setAsyncLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        auto asyncLogger = std::dynamic_pointer_cast<AsyncLogger>(logger);
        if (asyncLogger) {
            asyncLogger->setLevel(level);
        }
    }
}

void Logger::log(LogLevel level, const std::string& message,
                 const std::string& file, int line,
                 const std::string& function,
                 const std::map<std::string, std::string>& metadata) {
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.threadId = log_helpers::getThreadId();
    entry.metadata = metadata;
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
    
    // Update statistics
    pImpl->totalLogCount++;
    switch (level) {
        case LogLevel::Debug: pImpl->debugCount++; break;
        case LogLevel::Info: pImpl->infoCount++; break;
        case LogLevel::Warning: pImpl->warningCount++; break;
        case LogLevel::Error: pImpl->errorCount++; break;
        case LogLevel::Critical: pImpl->criticalCount++; break;
    }
}

void Logger::debug(const std::string& message, const std::string& file,
                   int line, const std::string& function,
                   const std::map<std::string, std::string>& metadata) {
    log(LogLevel::Debug, message, file, line, function, metadata);
}

void Logger::info(const std::string& message, const std::string& file,
                  int line, const std::string& function,
                  const std::map<std::string, std::string>& metadata) {
    log(LogLevel::Info, message, file, line, function, metadata);
}

void Logger::warning(const std::string& message, const std::string& file,
                     int line, const std::string& function,
                     const std::map<std::string, std::string>& metadata) {
    log(LogLevel::Warning, message, file, line, function, metadata);
}

void Logger::error(const std::string& message, const std::string& file,
                   int line, const std::string& function,
                   const std::map<std::string, std::string>& metadata) {
    log(LogLevel::Error, message, file, line, function, metadata);
}

void Logger::critical(const std::string& message, const std::string& file,
                      int line, const std::string& function,
                      const std::map<std::string, std::string>& metadata) {
    log(LogLevel::Critical, message, file, line, function, metadata);
}

LogStream Logger::debugStream(const std::string& file,
                              int line, const std::string& function) {
    return LogStream(LogLevel::Debug, file, line, function, this);
}

LogStream Logger::infoStream(const std::string& file,
                             int line, const std::string& function) {
    return LogStream(LogLevel::Info, file, line, function, this);
}

LogStream Logger::warningStream(const std::string& file,
                                int line, const std::string& function) {
    return LogStream(LogLevel::Warning, file, line, function, this);
}

LogStream Logger::errorStream(const std::string& file,
                              int line, const std::string& function) {
    return LogStream(LogLevel::Error, file, line, function, this);
}

LogStream Logger::criticalStream(const std::string& file,
                                 int line, const std::string& function) {
    return LogStream(LogLevel::Critical, file, line, function, this);
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

void Logger::flushAll() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        logger->flush();
    }
}

Logger& Logger::getGlobal() {
    static Logger globalLogger;
    return globalLogger;
}

void Logger::setGlobal(std::shared_ptr<Logger> logger) {
    // In practice, we just use getGlobal() which is already static
}

void Logger::configureDefaultLogging(const std::string& logFile) {
    clearLoggers();
    
    auto consoleLogger = std::make_shared<ConsoleLogger>();
    consoleLogger->setLevel(LogLevel::Info);
    consoleLogger->setUseColors(true);
    addLogger(consoleLogger);
    
    auto fileLogger = std::make_shared<FileLogger>(logFile);
    fileLogger->setLevel(LogLevel::Info);
    fileLogger->configureRotation(LogRotationConfig{10 * 1024 * 1024, 5, false, "00:00"});
    addLogger(fileLogger);
}

void Logger::enableAsyncLogging(size_t queueSize) {
    clearLoggers();
    
    auto asyncLogger = std::make_shared<AsyncLogger>(queueSize);
    asyncLogger->setLevel(LogLevel::Info);
    addLogger(asyncLogger);
    
    pImpl->asyncEnabled = true;
}

size_t Logger::getTotalLogCount() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->totalLogCount;
}

std::string Logger::getLogStatistics() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "Log Statistics:" << std::endl;
    oss << "  Total logs: " << pImpl->totalLogCount << std::endl;
    oss << "  Debug: " << pImpl->debugCount << std::endl;
    oss << "  Info: " << pImpl->infoCount << std::endl;
    oss << "  Warning: " << pImpl->warningCount << std::endl;
    oss << "  Error: " << pImpl->errorCount << std::endl;
    oss << "  Critical: " << pImpl->criticalCount << std::endl;
    
    auto duration = std::chrono::system_clock::now() - pImpl->startTime;
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
    oss << "  Uptime: " << seconds << " seconds" << std::endl;
    
    return oss.str();
}

// LogStream implementation
LogStream::LogStream(LogLevel level, const std::string& file,
                     int line, const std::string& function,
                     Logger* logger)
    : level_(level), file_(file), line_(line), function_(function), logger_(logger) {}

LogStream::~LogStream() {
    if (logger_) {
        std::string message = oss_.str();
        if (!message.empty()) {
            logger_->log(level_, message, file_, line_, function_, metadata_);
        }
    }
}

LogStream& LogStream::withMetadata(const std::string& key, const std::string& value) {
    metadata_[key] = value;
    return *this;
}

// Helper functions namespace
namespace log_helpers {

std::string formatDuration(std::chrono::milliseconds::rep ms) {
    if (ms < 1000) {
        return std::to_string(ms) + "ms";
    } else if (ms < 60000) {
        return std::to_string(ms / 1000) + "." + std::to_string(ms % 1000) + "s";
    } else {
        auto minutes = ms / 60000;
        auto seconds = (ms % 60000) / 1000;
        auto millis = (ms % 1000);
        return std::to_string(minutes) + "m " + std::to_string(seconds) + "." + std::to_string(millis) + "s";
    }
}

std::string formatBytes(size_t bytes) {
    if (bytes < 1024) return std::to_string(bytes) + " B";
    if (bytes < 1024 * 1024) return std::to_string(bytes / 1024) + " KB";
    if (bytes < 1024 * 1024 * 1024) return std::to_string(bytes / (1024 * 1024)) + " MB";
    return std::to_string(bytes / (1024 * 1024 * 1024)) + " GB";
}

std::string getThreadId() {
    std::stringstream ss;
    ss << std::this_thread::get_id();
    return ss.str();
}

std::string getProcessId() {
    std::stringstream ss;
    ss << getpid();
    return ss.str();
}

std::string generateLogFilename(const std::string& baseName) {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << baseName << "_" << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S") << ".log";
    return ss.str();
}

} // namespace log_helpers

} // namespace nlm
