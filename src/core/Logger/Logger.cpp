#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <thread>
#include <chrono>
#include <atomic>
#include <condition_variable>
#include <sys/types.h>  // For memory usage
#include <sys/sysinfo.h>  // For memory usage (Linux specific)
#include <unistd.h>  // For getpid
#include <csignal>   // For signal handling

namespace nlm {

// Forward declaration for AsyncLogger implementation
struct AsyncLogger::Impl;

// Performance monitoring implementation
namespace {
    // Simple platform-independent memory usage
    size_t getMemoryUsageLinux() {
        long pages = sysconf(_SC_PHYS_PAGES);
        long page_size = sysconf(_SC_PAGESIZE);
        return pages * page_size;
    }
    
    // Get current thread count (simplified implementation)
    int getThreadCount() {
        return std::thread::hardware_concurrency();
    }
}

size_t PerformanceMonitor::getMemoryUsage() const {
    return getMemoryUsageLinux();
}

float PerformanceMonitor::getCPUUsage() const {
    // Simplified CPU usage calculation
    // In a real implementation, this would read from /proc/stat or similar
    return 0.0f;  // Placeholder
}

int PerformanceMonitor::getThreadCount() const {
    return getThreadCount();
}

void PerformanceMonitor::updatePerformanceMetrics(PerformanceMetrics& metrics) const {
    metrics.memoryUsage = getMemoryUsage();
    metrics.cpuUsage = static_cast<int>(getCPUUsage() * 100);
    metrics.threadCount = getThreadCount();
    
    // Get current time for execution time
    auto currentTime = std::chrono::high_resolution_clock::now();
    metrics.executionTime = std::chrono::duration<double>(currentTime - startTime);
}

// AsyncLogger implementation
struct AsyncLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    
    // Async logging components
    bool asyncEnabled = true;
    bool structuredEnabled = false;
    bool performanceEnabled = false;
    bool logRotation = false;
    size_t maxFileSize = 10 * 1024 * 1024;  // 10 MB
    size_t currentFileSize = 0;
    std::string outputFile;
    
    // Rate limiting
    std::unordered_map<LogLevel, int> rateLimits;
    std::unordered_map<LogLevel, std::chrono::steady_clock::time_point> lastLogTime;
    std::unordered_map<LogLevel, size_t> messageCount;
    
    // Queue for async logging
    std::queue<LogEntry> logQueue;
    std::mutex queueMutex;
    std::condition_variable queueCond;
    std::atomic<bool> running{true};
    std::thread workerThread;
    
    // Statistics
    size_t processedMessages = 0;
    size_t droppedMessages = 0;
    
    // File rotation
    void rotateFile() {
        if (!logRotation || !fileOut.is_open()) return;
        
        fileOut.close();
        
        // Find next available filename
        std::string baseName = outputFile;
        std::string::size_t pos = baseName.find_last_of('.');
        std::string ext = (pos != std::string::npos) ? baseName.substr(pos) : "";
        std::string stem = (pos != std::string::npos) ? baseName.substr(0, pos) : baseName;
        
        int counter = 1;
        std::string rotatedName = baseName;
        while (std::ifstream(rotatedName).good()) {
            rotatedName = stem + "_" + std::to_string(counter++) + ext;
        }
        
        outputFile = rotatedName;
        fileOut.open(outputFile, std::ios::trunc);
        currentFileSize = 0;
    }
    
    bool shouldLog(LogLevel level) {
        if (level < level) return false;
        
        // Rate limiting
        if (rateLimits.count(level)) {
            auto now = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - lastLogTime[level]);
            
            if (duration.count() >= 1) {
                messageCount[level] = 0;
                lastLogTime[level] = now;
            }
            
            if (messageCount[level] >= rateLimits[level]) {
                droppedMessages++;
                return false;
            }
            
            messageCount[level]++;
        }
        
        return true;
    }
    
    void worker() {
        while (running) {
            std::unique_lock<std::mutex> lock(queueMutex);
            queueCond.wait_for(lock, std::chrono::milliseconds(10), [this]() {
                return !logQueue.empty();
            });
            
            if (!logQueue.empty()) {
                LogEntry entry = std::move(logQueue.front());
                logQueue.pop();
                processLogEntry(entry);
            }
        }
    }
    
    void processLogEntry(const LogEntry& entry) {
        if (!shouldLog(entry.level)) return;
        
        std::ostringstream oss;
        oss << "[" << levelToString(entry.level) << "] ";
        
        // Add timestamp
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        oss << " " << entry.message;
        
        std::string output = oss.str();
        
        // Console output
        if (useColors) {
            std::cout << levelToColor(entry.level) << output << "\033[0m\n";
        } else {
            std::cout << output << "\n";
        }
        
        // File output if set
        if (fileOut.is_open()) {
            fileOut << output << "\n";
            currentFileSize += output.size() + 1;  // +1 for newline
            
            // Check for file rotation
            if (logRotation && currentFileSize >= maxFileSize) {
                rotateFile();
            }
            
            fileOut.flush();
        }
        
        processedMessages++;
    }
    
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

AsyncLogger::AsyncLogger(size_t queueSize) : pImpl(std::make_unique<Impl>()) {
    pImpl->workerThread = std::thread(&AsyncLogger::Impl::worker, pImpl.get());
}

AsyncLogger::~AsyncLogger() {
    pImpl->running = false;
    pImpl->queueCond.notify_all();
    
    if (pImpl->workerThread.joinable()) {
        pImpl->workerThread.join();
    }
}

void AsyncLogger::log(const LogEntry& entry) {
    if (asyncEnabled && !pImpl->logQueue.empty()) {
        std::lock_guard<std::mutex> lock(pImpl->queueMutex);
        pImpl->logQueue.push(entry);
    } else {
        pImpl->processLogEntry(entry);
    }
}

void AsyncLogger::setLevel(LogLevel level) {
    pImpl->level = level;
}

LogLevel AsyncLogger::getLevel() const {
    return pImpl->level;
}

void AsyncLogger::setStructuredLogging(bool enabled) {
    pImpl->structuredEnabled = enabled;
}

bool AsyncLogger::getStructuredLogging() const {
    return pImpl->structuredEnabled;
}

void AsyncLogger::setPerformanceMetrics(bool enabled) {
    pImpl->performanceEnabled = enabled;
}

bool AsyncLogger::getPerformanceMetrics() const {
    return pImpl->performanceEnabled;
}

void AsyncLogger::setAsyncLogging(bool enabled) {
    pImpl->asyncEnabled = enabled;
}

bool AsyncLogger::getAsyncLogging() const {
    return pImpl->asyncEnabled;
}

void AsyncLogger::setLogRotation(bool enabled) {
    pImpl->logRotation = enabled;
}

bool AsyncLogger::getLogRotation() const {
    return pImpl->logRotation;
}

void AsyncLogger::setMaxFileSize(size_t maxSize) {
    pImpl->maxFileSize = maxSize;
}

size_t AsyncLogger::getMaxFileSize() const {
    return pImpl->maxFileSize;
}

void AsyncLogger::setRateLimit(LogLevel level, int messagesPerSecond) {
    pImpl->rateLimits[level] = messagesPerSecond;
    pImpl->lastLogTime[level] = std::chrono::steady_clock::now();
    pImpl->messageCount[level] = 0;
}

int AsyncLogger::getRateLimit(LogLevel level) const {
    auto it = pImpl->rateLimits.find(level);
    return it != pImpl->rateLimits.end() ? it->second : 0;
}

void AsyncLogger::flush() {
    std::lock_guard<std::mutex> lock(pImpl->queueMutex);
    queueCond.notify_all();
}

size_t AsyncLogger::getQueueSize() const {
    std::lock_guard<std::mutex> lock(pImpl->queueMutex);
    return pImpl->logQueue.size();
}

size_t AsyncLogger::getProcessedMessages() const {
    return pImpl->processedMessages;
}

// Logger class implementation
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    
    // Configuration
    bool structuredLoggingEnabled = false;
    bool performanceMetricsEnabled = false;
    bool asyncLoggingEnabled = true;
    bool logRotationEnabled = false;
    size_t maxFileSize = 10 * 1024 * 1024;
    LogLevel minLevel = LogLevel::Info;
    std::string outputFile;
    bool useColors = true;
    
    // Rate limiting
    std::unordered_map<LogLevel, int> rateLimits;
    
    // Performance monitoring
    PerformanceMonitor performanceMonitor;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {}

Logger::~Logger() = default;

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    if (!logger) return;
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Apply current configuration to the logger
    logger->setLevel(pImpl->minLevel);
    logger->setStructuredLogging(pImpl->structuredLoggingEnabled);
    logger->setPerformanceMetrics(pImpl->performanceMetricsEnabled);
    logger->setAsyncLogging(pImpl->asyncLoggingEnabled);
    logger->setLogRotation(pImpl->logRotationEnabled);
    logger->setMaxFileSize(pImpl->maxFileSize);
    
    // Set rate limits
    for (const auto& [level, limit] : pImpl->rateLimits) {
        logger->setRateLimit(level, limit);
    }
    
    pImpl->loggers.push_back(logger);
}

void Logger::clearLoggers() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.clear();
}

void Logger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->minLevel = level;
    
    for (auto& logger : pImpl->loggers) {
        logger->setLevel(level);
    }
}

void Logger::log(LogLevel level, const std::string& message,
                const std::string& file, int line, const std::string& function,
                const PerformanceMonitor& monitor) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    
    if (pImpl->performanceMetricsEnabled) {
        monitor.updatePerformanceMetrics(entry.performanceMetrics);
    }
    
    for (auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
}

void Logger::debug(const std::string& message, const std::string& file, int line, const std::string& function,
                   const PerformanceMonitor& monitor) {
    log(LogLevel::Debug, message, file, line, function, monitor);
}

void Logger::info(const std::string& message, const std::string& file, int line, const std::string& function,
                  const PerformanceMonitor& monitor) {
    log(LogLevel::Info, message, file, line, function, monitor);
}

void Logger::warning(const std::string& message, const std::string& file, int line, const std::string& function,
                     const PerformanceMonitor& monitor) {
    log(LogLevel::Warning, message, file, line, function, monitor);
}

void Logger::error(const std::string& message, const std::string& file, int line, const std::string& function,
                   const PerformanceMonitor& monitor) {
    log(LogLevel::Error, message, file, line, function, monitor);
}

void Logger::critical(const std::string& message, const std::string& file, int line, const std::string& function,
                      const PerformanceMonitor& monitor) {
    log(LogLevel::Critical, message, file, line, function, monitor);
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

static std::shared_ptr<Logger>& globalLoggerPtr() {
    static std::shared_ptr<Logger> ptr = std::make_shared<Logger>();
    return ptr;
}

Logger& Logger::getGlobal() {
    return *globalLoggerPtr();
}

void Logger::setGlobal(std::shared_ptr<Logger> logger) {
    if (logger) {
        globalLoggerPtr() = logger;
    }
}

// Performance monitor implementation
PerformanceMonitor Logger::createPerformanceMonitor() {
    return PerformanceMonitor();
}

// LogStream implementation
Logger::LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function), monitor_(nullptr) {}

Logger::LogStream::~LogStream() {
    Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_, *monitor_);
}

// Static configuration
namespace {
    Logger::Config globalConfig;
}

void Logger::setConfig(const Config& config) {
    std::lock_guard<std::mutex> lock(globalLoggerPtr()->pImpl->mutex);
    globalConfig = config;
    
    // Apply to existing loggers
    for (auto& logger : globalLoggerPtr()->pImpl->loggers) {
        logger->setLevel(config.minLevel);
        logger->setStructuredLogging(config.enableStructuredLogging);
        logger->setPerformanceMetrics(config.enablePerformanceMetrics);
        logger->setAsyncLogging(config.enableAsyncLogging);
        logger->setLogRotation(config.enableLogRotation);
        logger->setMaxFileSize(config.maxFileSize);
    }
}

Logger::Config Logger::getConfig() {
    return globalConfig;
}

} // namespace nlm
