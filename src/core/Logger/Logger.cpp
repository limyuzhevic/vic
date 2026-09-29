#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <thread>
#include <queue>
#include <condition_variable>
#include <atomic>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <iomanip>

namespace nlm {

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    
    // Async logging
    bool enableAsync = false;
    size_t maxQueueSize = 1000;
    std::queue<LogEntry> logQueue;
    std::thread workerThread;
    std::condition_variable cv;
    std::mutex queueMutex;
    std::atomic<bool> stopWorker{false};
    
    // Performance stats
    bool enablePerformanceStats = false;
    std::atomic<size_t> totalLogs{0};
    std::atomic<double> averageLogTime{0.0};
    std::chrono::steady_clock::time_point startTime;
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {
    pImpl->startTime = std::chrono::steady_clock::now();
    
    // Start worker thread if async logging is enabled
    if (pImpl->enableAsync) {
        pImpl->stopWorker = false;
        pImpl->workerThread = std::thread(&ConsoleLogger::workerLoop, this);
    }
}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
    pImpl->startTime = std::chrono::steady_clock::now();
    
    if (pImpl->enableAsync) {
        pImpl->stopWorker = false;
        pImpl->workerThread = std::thread(&ConsoleLogger::workerLoop, this);
    }
}

ConsoleLogger::~ConsoleLogger() {
    if (pImpl->enableAsync && pImpl->workerThread.joinable()) {
        pImpl->stopWorker = true;
        pImpl->cv.notify_one();
        pImpl->workerThread.join();
    }
}

void ConsoleLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    // Async logging
    if (pImpl->enableAsync) {
        std::lock_guard<std::mutex> lock(pImpl->queueMutex);
        if (pImpl->logQueue.size() >= pImpl->maxQueueSize) {
            // Drop oldest log to prevent memory issues
            pImpl->logQueue.pop();
        }
        pImpl->logQueue.push(entry);
        pImpl->cv.notify_one();
        return;
    }
    
    // Synchronous logging
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto startTime = std::chrono::steady_clock::now();
    
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    
    // Add timestamp
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    
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
    }
    
    // Update performance stats
    if (pImpl->enablePerformanceStats) {
        auto endTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
        updatePerformanceStats(static_cast<double>(duration));
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

bool ConsoleLogger::isEnabled(LogLevel level) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return level >= pImpl->level;
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
    pImpl->fileOut.open(filepath, std::ios::app);
}

void ConsoleLogger::closeOutputFile() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

void ConsoleLogger::setLogQueueSize(size_t maxQueueSize) {
    std::lock_guard<std::mutex> lock(pImpl->queueMutex);
    pImpl->maxQueueSize = maxQueueSize;
}

size_t ConsoleLogger::getLogQueueSize() const {
    std::lock_guard<std::mutex> lock(pImpl->queueMutex);
    return pImpl->logQueue.size();
}

void ConsoleLogger::setEnablePerformanceStats(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->enablePerformanceStats = enable;
    if (enable) {
        pImpl->totalLogs = 0;
        pImpl->averageLogTime = 0.0;
        pImpl->startTime = std::chrono::steady_clock::now();
    }
}

bool ConsoleLogger::getEnablePerformanceStats() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->enablePerformanceStats;
}

double ConsoleLogger::getAverageLogTime() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->averageLogTime;
}

void ConsoleLogger::updatePerformanceStats(double duration) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->totalLogs++;
    
    // Simple exponential moving average
    if (pImpl->totalLogs == 1) {
        pImpl->averageLogTime = duration;
    } else {
        pImpl->averageLogTime = (pImpl->averageLogTime * (pImpl->totalLogs - 1) + duration) / pImpl->totalLogs;
    }
}

void ConsoleLogger::workerLoop() {
    while (!pImpl->stopWorker) {
        std::unique_lock<std::mutex> lock(pImpl->queueMutex);
        pImpl->cv.wait(lock, [this] { 
            return !pImpl->logQueue.empty() || pImpl->stopWorker; 
        });
        
        if (pImpl->stopWorker && pImpl->logQueue.empty()) {
            break;
        }
        
        LogEntry entry = pImpl->logQueue.front();
        pImpl->logQueue.pop();
        lock.unlock();
        
        // Log the entry synchronously in worker thread
        log(entry);
    }
}

// FileLogger implementation
struct FileLogger::Impl {
    LogLevel level = LogLevel::Info;
    std::string filepath;
    std::ofstream fileOut;
    std::mutex mutex;
    
    // Rotation support
    bool enableRotation = false;
    size_t maxFileSizeMB = 100;  // 100MB
    int maxFiles = 5;
    size_t currentFileSize = 0;
    
    // Performance stats
    bool enablePerformanceStats = false;
    std::atomic<size_t> totalLogs{0};
    std::atomic<double> averageLogTime{0.0};
    std::chrono::steady_clock::time_point startTime;
};

FileLogger::FileLogger() : pImpl(std::make_unique<Impl>()) {
    pImpl->startTime = std::chrono::steady_clock::now();
}

FileLogger::FileLogger(const std::string& filepath) : pImpl(std::make_unique<Impl>()) {
    setFilepath(filepath);
    pImpl->startTime = std::chrono::steady_clock::now();
}

FileLogger::FileLogger(const std::string& filepath, LogLevel level) : pImpl(std::make_unique<Impl>()) {
    setFilepath(filepath);
    pImpl->level = level;
    pImpl->startTime = std::chrono::steady_clock::now();
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
    
    if (!pImpl->fileOut.is_open()) {
        return;
    }
    
    auto startTime = std::chrono::steady_clock::now();
    
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    
    // Add timestamp
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    
    oss << " " << entry.message;
    
    std::string output = oss.str();
    
    // Check if we need to rotate file
    if (pImpl->enableRotation) {
        if (pImpl->currentFileSize + output.size() > pImpl->maxFileSizeMB * 1024 * 1024) {
            rotateFile();
        }
    }
    
    pImpl->fileOut << output << "\n";
    pImpl->fileOut.flush();
    pImpl->currentFileSize += output.size() + 1;  // +1 for newline
    
    // Update performance stats
    if (pImpl->enablePerformanceStats) {
        auto endTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
        updatePerformanceStats(static_cast<double>(duration));
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

bool FileLogger::isEnabled(LogLevel level) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return level >= pImpl->level;
}

void FileLogger::setFilepath(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->filepath = filepath;
    pImpl->fileOut.close();
    pImpl->fileOut.open(filepath, std::ios::app);
    pImpl->currentFileSize = pImpl->fileOut.tellp();
}

std::string FileLogger::getFilepath() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->filepath;
}

void FileLogger::setFileRotation(size_t maxFileSizeMB, int maxFiles) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->maxFileSizeMB = maxFileSizeMB;
    pImpl->maxFiles = maxFiles;
    pImpl->enableRotation = true;
}

void FileLogger::enableFileRotation(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->enableRotation = enable;
}

void FileLogger::rotateFile() {
    pImpl->fileOut.close();
    
    // Close oldest files if we have too many
    std::vector<std::string> files;
    for (int i = pImpl->maxFiles - 1; i >= 0; --i) {
        std::string filename = pImpl->filepath;
        if (i > 0) {
            filename += "." + std::to_string(i);
        }
        if (std::filesystem::exists(filename)) {
            files.push_back(filename);
        }
    }
    
    // Rename files
    for (size_t i = files.size() - 1; i > 0; --i) {
        std::filesystem::rename(files[i], files[i - 1]);
    }
    
    // Rename current file
    if (std::filesystem::exists(pImpl->filepath)) {
        std::filesystem::rename(pImpl->filepath, pImpl->filepath + ".1");
    }
    
    // Reopen file
    pImpl->fileOut.open(pImpl->filepath, std::ios::trunc);
    pImpl->currentFileSize = 0;
}

void FileLogger::updatePerformanceStats(double duration) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->totalLogs++;
    
    // Simple exponential moving average
    if (pImpl->totalLogs == 1) {
        pImpl->averageLogTime = duration;
    } else {
        pImpl->averageLogTime = (pImpl->averageLogTime * (pImpl->totalLogs - 1) + duration) / pImpl->totalLogs;
    }
}

// Logger class implementation
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    
    // Async logging
    bool enableAsync = false;
    size_t maxQueueSize = 1000;
    size_t workerThreads = std::thread::hardware_concurrency();
    std::vector<std::thread> workerThreadsVec;
    std::queue<std::pair<LogEntry, std::string>> logQueue; // Entry + file info
    std::condition_variable cv;
    std::mutex queueMutex;
    std::atomic<bool> stopWorkers{false};
    
    // Performance statistics
    bool enablePerformanceStats = false;
    std::atomic<size_t> totalLogs{0};
    std::atomic<double> averageLogTime{0.0};
    std::atomic<size_t> logsPerSecond{0};
    std::atomic<size_t> memoryUsage{0};
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point lastStatsUpdate;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {
    pImpl->startTime = std::chrono::steady_clock::now();
    pImpl->lastStatsUpdate = std::chrono::steady_clock::now();
    
    // Start worker threads if async logging is enabled
    if (pImpl->enableAsync) {
        pImpl->stopWorkers = false;
        for (size_t i = 0; i < pImpl->workerThreads; ++i) {
            pImpl->workerThreadsVec.emplace_back(&Logger::workerLoop, this);
        }
    }
}

Logger::~Logger() {
    if (pImpl->enableAsync) {
        pImpl->stopWorkers = true;
        pImpl->cv.notify_all();
        for (auto& thread : pImpl->workerThreadsVec) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }
}

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    if (!logger) return;
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.push_back(logger);
}

bool Logger::removeLogger(const std::shared_ptr<ILogger>& logger) {
    if (!logger) return false;
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto it = pImpl->loggers.begin(); it != pImpl->loggers.end(); ++it) {
        if (*it == logger) {
            pImpl->loggers.erase(it);
            return true;
        }
    }
    return false;
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
                 const std::string& file, int line,
                 const std::string& function, bool async) {
    // Async logging
    if (async && pImpl->enableAsync) {
        std::lock_guard<std::mutex> lock(pImpl->queueMutex);
        if (pImpl->logQueue.size() >= pImpl->maxQueueSize) {
            pImpl->logQueue.pop();  // Drop oldest
        }
        pImpl->logQueue.emplace(LogEntry{level, message, file, line, function, std::chrono::system_clock::now()}, function);
        pImpl->cv.notify_one();
        return;
    }
    
    // Synchronous logging
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
    
    // Update performance stats
    if (pImpl->enablePerformanceStats) {
        updatePerformanceStats();
    }
}

void Logger::debug(const std::string& message, const std::string& file, int line, const std::string& function, bool async) {
    log(LogLevel::Debug, message, file, line, function, async);
}

void Logger::info(const std::string& message, const std::string& file, int line, const std::string& function, bool async) {
    log(LogLevel::Info, message, file, line, function, async);
}

void Logger::warning(const std::string& message, const std::string& file, int line, const std::string& function, bool async) {
    log(LogLevel::Warning, message, file, line, function, async);
}

void Logger::error(const std::string& message, const std::string& file, int line, const std::string& function, bool async) {
    log(LogLevel::Error, message, file, line, function, async);
}

void Logger::critical(const std::string& message, const std::string& file, int line, const std::string& function, bool async) {
    log(LogLevel::Critical, message, file, line, function, async);
}

bool Logger::isEnabled(LogLevel level) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        if (logger->isEnabled(level)) {
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

void Logger::setAsyncLogging(bool enable, size_t maxQueueSize, size_t workerThreads) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (enable && !pImpl->enableAsync) {
        pImpl->enableAsync = true;
        pImpl->maxQueueSize = maxQueueSize;
        pImpl->workerThreads = workerThreads;
        pImpl->stopWorkers = false;
        
        for (size_t i = 0; i < pImpl->workerThreads; ++i) {
            pImpl->workerThreadsVec.emplace_back(&Logger::workerLoop, this);
        }
    } else if (!enable && pImpl->enableAsync) {
        pImpl->enableAsync = false;
        pImpl->stopWorkers = true;
        pImpl->cv.notify_all();
        for (auto& thread : pImpl->workerThreadsVec) {
            if (thread.joinable()) {
                thread.join();
            }
        }
        pImpl->workerThreadsVec.clear();
    }
}

bool Logger::isAsyncLoggingEnabled() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->enableAsync;
}

Logger::PerformanceStats Logger::getPerformanceStats() const {
    PerformanceStats stats;
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    stats.totalLogs = pImpl->totalLogs;
    stats.averageLogTime = pImpl->averageLogTime;
    stats.logsPerSecond = pImpl->logsPerSecond;
    stats.memoryUsage = pImpl->memoryUsage;
    
    return stats;
}

void Logger::resetPerformanceStats() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->totalLogs = 0;
    pImpl->averageLogTime = 0.0;
    pImpl->logsPerSecond = 0;
    pImpl->memoryUsage = 0;
    pImpl->startTime = std::chrono::steady_clock::now();
    pImpl->lastStatsUpdate = std::chrono::steady_clock::now();
}

void Logger::updatePerformanceStats() {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - pImpl->lastStatsUpdate).count();
    
    if (elapsed >= 1) {
        // Update logs per second
        pImpl->logsPerSecond = pImpl->totalLogs.exchange(0);
        
        // Estimate memory usage (rough approximation)
        size_t totalLoggersSize = 0;
        for (auto& logger : pImpl->loggers) {
            // This is a rough estimate - in practice you'd measure actual memory
            totalLoggersSize += sizeof(ILogger*) + 128; // Rough estimate
        }
        pImpl->memoryUsage = totalLoggersSize + pImpl->logQueue.size() * 256; // Rough estimate
        
        pImpl->lastStatsUpdate = now;
    }
}

void Logger::workerLoop() {
    while (!pImpl->stopWorkers) {
        std::unique_lock<std::mutex> lock(pImpl->queueMutex);
        pImpl->cv.wait(lock, [this] { 
            return !pImpl->logQueue.empty() || pImpl->stopWorkers; 
        });
        
        if (pImpl->stopWorkers && pImpl->logQueue.empty()) {
            break;
        }
        
        auto task = pImpl->logQueue.front();
        pImpl->logQueue.pop();
        lock.unlock();
        
        // Log the entry synchronously in worker thread
        log(task.first.level, task.first.message, task.first.file, 
            task.first.line, task.second, false);
    }
}

// LogStream implementation
Logger::LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function) {}

Logger::LogStream::~LogStream() {
    Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
}

} // namespace nlm
