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
}

void ConsoleLogger::setLevel(LogLevel level) {
    pImpl->level = level;
}

LogLevel ConsoleLogger::getLevel() const {
    return pImpl->level;
}

void ConsoleLogger::setUseColors(bool useColors) {
    pImpl->useColors = useColors;
}

bool ConsoleLogger::getUseColors() const {
    return pImpl->useColors;
}

void ConsoleLogger::setOutputFile(const std::string& filepath) {
    pImpl->fileOut.open(filepath, std::ios::app);
}

void ConsoleLogger::closeOutputFile() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
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

#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <chrono>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <queue>

namespace nlm {

// Log entry queue for async logging
struct LogQueueEntry {
    LogEntry entry;
    std::chrono::system_clock::time_point timestamp;
};

// Async logger implementation
struct AsyncLogger::Impl {
    LogLevel level = LogLevel::Info;
    size_t queueSize;
    std::queue<LogQueueEntry> messageQueue;
    std::mutex mutex;
    std::condition_variable cv;
    std::atomic<bool> running;
    std::thread workerThread;
    
    // Statistics
    std::atomic<size_t> processedCount;
    std::atomic<size_t> droppedCount;
    std::atomic<size_t> queueSize;
    
    // Worker thread function
    void worker() {
        while (running.load(std::memory_order_relaxed)) {
            std::unique_lock<std::mutex> lock(mutex);
            
            // Wait for items or shutdown
            cv.wait(lock, [this]() {
                return !messageQueue.empty() || !running.load(std::memory_order_relaxed);
            });
            
            if (!running.load(std::memory_order_relaxed) && messageQueue.empty()) {
                break;
            }
            
            if (!messageQueue.empty()) {
                // Process next item
                LogQueueEntry entry = std::move(messageQueue.front());
                messageQueue.pop();
                
                // Output to console (simplified - would use proper logger)
                std::cout << "[" << levelToString(entry.entry.level) << "] "
                         << std::chrono::system_clock::to_time_t(entry.entry.timestamp)
                         << " " << entry.entry.message << "\n";
                
                processedCount.fetch_add(1, std::memory_order_relaxed);
            }
            
            lock.unlock();
            cv.notify_one();
        }
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
};

AsyncLogger::AsyncLogger(size_t queueSize) : pImpl(std::make_unique<Impl>()) {
    pImpl->queueSize = queueSize;
    pImpl->running = true;
    pImpl->processedCount = 0;
    pImpl->droppedCount = 0;
    pImpl->queueSize = 0;
    
    // Start worker thread
    pImpl->workerThread = std::thread(&AsyncLogger::Impl::worker, pImpl.get());
}

AsyncLogger::~AsyncLogger() {
    if (pImpl->running.load(std::memory_order_relaxed)) {
        pImpl->running.store(false, std::memory_order_relaxed);
        pImpl->cv.notify_one();
        if (pImpl->workerThread.joinable()) {
            pImpl->workerThread.join();
        }
    }
}

void AsyncLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Check if queue is full
    if (pImpl->messageQueue.size() >= pImpl->queueSize) {
        // Drop oldest entry
        pImpl->messageQueue.pop();
        pImpl->droppedCount.fetch_add(1, std::memory_order_relaxed);
    }
    
    // Add new entry
    pImpl->messageQueue.push({entry, std::chrono::system_clock::now()});
    pImpl->queueSize.store(pImpl->messageQueue.size(), std::memory_order_relaxed);
    
    pImpl->cv.notify_one();
}

void AsyncLogger::setLevel(LogLevel level) {
    pImpl->level = level;
}

LogLevel AsyncLogger::getLevel() const {
    return pImpl->level;
}

void AsyncLogger::flush() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    // Clear queue by popping all items
    while (!pImpl->messageQueue.empty()) {
        pImpl->messageQueue.pop();
    }
    pImpl->queueSize.store(0, std::memory_order_relaxed);
}

AsyncLogger::QueueStats AsyncLogger::getQueueStats() const {
    QueueStats stats;
    stats.size = pImpl->messageQueue.size();
    stats.capacity = pImpl->queueSize;
    stats.dropped = pImpl->droppedCount.load(std::memory_order_relaxed);
    stats.utilization = (stats.capacity > 0) ? 
        static_cast<double>(stats.size) / stats.capacity : 0.0;
    return stats;
}

void AsyncLogger::setWorkerThreadCount(size_t count) {
    // Note: This implementation uses a single worker thread
    // For multiple threads, would need a thread pool implementation
    (void)count; // Avoid unused parameter warning
}

// Advanced console logger implementation
struct AdvancedConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    
    // Performance tracking
    bool performanceTrackingEnabled = false;
    std::chrono::microseconds totalLogDuration = std::chrono::microseconds::zero();
    size_t logCount = 0;
    
    // Log formatting
    std::string logFormat = "[%level] %timestamp %message";
    std::string timestampFormat = "%Y-%m-%d %H:%M:%S";
    
    // File rotation
    bool fileRotationEnabled = false;
    size_t maxFileSizeMB = 10;
    int maxBackupFiles = 5;
    std::string currentFilepath;
    size_t currentFileSize = 0;
    int fileCounter = 0;
    
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
        if (!useColors) return "";
        
        switch (level) {
            case LogLevel::Debug: return "\033[36m";    // Cyan
            case LogLevel::Info: return "\033[32m";      // Green
            case LogLevel::Warning: return "\033[33m";   // Yellow
            case LogLevel::Error: return "\033[31m";     // Red
            case LogLevel::Critical: return "\033[35m"; // Magenta
            default: return "\033[0m";
        }
    }
    
    std::string formatTimestamp(const std::chrono::system_clock::time_point& timestamp) const {
        auto time = std::chrono::system_clock::to_time_t(timestamp);
        std::ostringstream oss;
        oss << std::put_time(std::localtime(&time), timestampFormat.c_str());
        return oss.str();
    }
    
    std::string formatMessage(const LogEntry& entry) const {
        std::string message = entry.message;
        
        // Replace placeholders
        std::string result = logFormat;
        size_t pos;
        
        while ((pos = result.find("%level")) != std::string::npos) {
            result.replace(pos, 7, levelToString(entry.level));
        }
        
        while ((pos = result.find("%timestamp")) != std::string::npos) {
            result.replace(pos, 11, formatTimestamp(entry.timestamp));
        }
        
        while ((pos = result.find("%message")) != std::string::npos) {
            result.replace(pos, 8, message);
        }
        
        return result;
    }
    
    bool shouldRotateFile() const {
        return fileRotationEnabled && 
               currentFileSize >= maxFileSizeMB * 1024 * 1024 &&
               !currentFilepath.empty();
    }
    
    void rotateFile() {
        if (currentFilepath.empty()) return;
        
        // Close current file
        if (fileOut.is_open()) {
            fileOut.close();
        }
        
        // Rename current file
        std::string newName = currentFilepath + "." + std::to_string(fileCounter++);
        std::rename(currentFilepath.c_str(), newName.c_str());
        
        // Remove old backup files if too many
        if (fileCounter > maxBackupFiles) {
            std::string oldName = currentFilepath + "." + std::to_string(fileCounter - maxBackupFiles - 1);
            std::remove(oldName.c_str());
        }
        
        // Reopen file
        fileOut.open(currentFilepath, std::ios::app);
        currentFileSize = 0;
    }
};

AdvancedConsoleLogger::AdvancedConsoleLogger() : pImpl(std::make_unique<Impl>()) {}

AdvancedConsoleLogger::AdvancedConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
}

AdvancedConsoleLogger::~AdvancedConsoleLogger() = default;

void AdvancedConsoleLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Performance tracking
    if (pImpl->performanceTrackingEnabled) {
        pImpl->totalLogDuration += entry.logDuration;
        pImpl->logCount++;
    }
    
    // Format message
    std::string output = pImpl->formatMessage(entry);
    
    // Console output
    if (pImpl->useColors) {
        std::cout << pImpl->levelToColor(entry.level) << output << "\033[0m\n";
    } else {
        std::cout << output << "\n";
    }
    
    // File output if set
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut << output << "\n";
        pImpl->currentFileSize += output.size() + 1; // +1 for newline
        
        // Check for rotation
        if (pImpl->shouldRotateFile()) {
            pImpl->rotateFile();
        }
    }
}

void AdvancedConsoleLogger::setLevel(LogLevel level) {
    pImpl->level = level;
}

LogLevel AdvancedConsoleLogger::getLevel() const {
    return pImpl->level;
}

void AdvancedConsoleLogger::setUseColors(bool useColors) {
    pImpl->useColors = useColors;
}

bool AdvancedConsoleLogger::getUseColors() const {
    return pImpl->useColors;
}

void AdvancedConsoleLogger::setOutputFile(const std::string& filepath) {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
    
    pImpl->fileOut.open(filepath, std::ios::app);
    pImpl->currentFilepath = filepath;
    pImpl->currentFileSize = 0;
    pImpl->fileCounter = 0;
}

void AdvancedConsoleLogger::closeOutputFile() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

void AdvancedConsoleLogger::enablePerformanceTracking(bool enable) {
    pImpl->performanceTrackingEnabled = enable;
}

bool AdvancedConsoleLogger::isPerformanceTrackingEnabled() const {
    return pImpl->performanceTrackingEnabled;
}

void AdvancedConsoleLogger::setLogFormat(const std::string& format) {
    pImpl->logFormat = format;
}

void AdvancedConsoleLogger::setTimestampFormat(const std::string& format) {
    pImpl->timestampFormat = format;
}

void AdvancedConsoleLogger::enableFileRotation(size_t maxSizeMB, int maxFiles) {
    pImpl->fileRotationEnabled = true;
    pImpl->maxFileSizeMB = maxSizeMB;
    pImpl->maxBackupFiles = maxFiles;
}

void AdvancedConsoleLogger::rotateNow() {
    pImpl->rotateFile();
}

// Logger class implementation
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::vector<std::shared_ptr<ILogFilter>> filters;
    std::mutex mutex;
    
    // Statistics
    uint64_t totalLogs = 0;
    uint64_t debugLogs = 0;
    uint64_t infoLogs = 0;
    uint64_t warningLogs = 0;
    uint64_t errorLogs = 0;
    uint64_t criticalLogs = 0;
    std::chrono::microseconds averageLogDuration = std::chrono::microseconds::zero();
    double logsPerSecond = 0.0;
    std::chrono::system_clock::time_point lastStatsReset;
    
    // Async logging
    bool asyncLoggingEnabled = false;
    std::shared_ptr<AsyncLogger> asyncLogger;
    
    // Configuration
    size_t logBufferSize = 4096;
    std::string optimizationMode = "throughput"; // "throughput" or "latency"
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {
    pImpl->lastStatsReset = std::chrono::system_clock::now();
}

Logger::~Logger() = default;

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.push_back(logger);
}

void Logger::addFilter(std::shared_ptr<ILogFilter> filter) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->filters.push_back(filter);
}

void Logger::clearAll() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.clear();
    pImpl->filters.clear();
    pImpl->asyncLoggingEnabled = false;
    pImpl->asyncLogger.reset();
}

void Logger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        logger->setLevel(level);
    }
}

void Logger::log(LogLevel level, const std::string& message,
                 const std::string& file, int line, const std::string& function) {
    // Create log entry
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.threadId = std::this_thread::get_id();
    entry.simulationStep = 0; // Default, would be set by caller
    
    logWithContext(entry);
}

void Logger::log(LogLevel level, const std::string& message, uint64_t step,
                 const std::string& agent, const std::string& experiment,
                 const std::string& file, int line, const std::string& function) {
    // Create log entry with context
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.threadId = std::this_thread::get_id();
    entry.simulationStep = step;
    entry.agentId = agent;
    entry.experimentId = experiment;
    
    logWithContext(entry);
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

void Logger::debugWithContext(const std::string& message, uint64_t step, const std::string& agent, const std::string& experiment) {
    log(LogLevel::Debug, message, step, agent, experiment);
}

void Logger::infoWithContext(const std::string& message, uint64_t step, const std::string& agent, const std::string& experiment) {
    log(LogLevel::Info, message, step, agent, experiment);
}

void Logger::warningWithContext(const std::string& message, uint64_t step, const std::string& agent, const std::string& experiment) {
    log(LogLevel::Warning, message, step, agent, experiment);
}

void Logger::errorWithContext(const std::string& message, uint64_t step, const std::string& agent, const std::string& experiment) {
    log(LogLevel::Error, message, step, agent, experiment);
}

void Logger::criticalWithContext(const std::string& message, uint64_t step, const std::string& agent, const std::string& experiment) {
    log(LogLevel::Critical, message, step, agent, experiment);
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

void Logger::logWithContext(const LogEntry& entry) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Apply filters
    bool shouldLog = true;
    for (auto& filter : pImpl->filters) {
        if (!filter->shouldLog(entry)) {
            shouldLog = false;
            break;
        }
    }
    
    if (!shouldLog) {
        return;
    }
    
    // Update statistics
    pImpl->totalLogs++;
    switch (entry.level) {
        case LogLevel::Debug: pImpl->debugLogs++; break;
        case LogLevel::Info: pImpl->infoLogs++; break;
        case LogLevel::Warning: pImpl->warningLogs++; break;
        case LogLevel::Error: pImpl->errorLogs++; break;
        case LogLevel::Critical: pImpl->criticalLogs++; break;
    }
    
    // Log using configured loggers
    if (pImpl->asyncLoggingEnabled && pImpl->asyncLogger) {
        pImpl->asyncLogger->log(entry);
    } else {
        for (auto& logger : pImpl->loggers) {
            logger->log(entry);
        }
    }
}

Logger::LogStats Logger::getStats() const {
    LogStats stats;
    stats.totalLogs = pImpl->totalLogs;
    stats.debugLogs = pImpl->debugLogs;
    stats.infoLogs = pImpl->infoLogs;
    stats.warningLogs = pImpl->warningLogs;
    stats.errorLogs = pImpl->errorLogs;
    stats.criticalLogs = pImpl->criticalLogs;
    stats.averageLogDuration = pImpl->averageLogDuration;
    
    // Calculate logs per second
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - pImpl->lastStatsReset);
    if (duration.count() > 0) {
        stats.logsPerSecond = static_cast<double>(pImpl->totalLogs) / duration.count();
    } else {
        stats.logsPerSecond = 0.0;
    }
    
    return stats;
}

void Logger::enableAsyncLogging(bool enable) {
    if (enable && !pImpl->asyncLogger) {
        pImpl->asyncLogger = std::make_shared<AsyncLogger>();
    }
    pImpl->asyncLoggingEnabled = enable;
}

bool Logger::isAsyncLoggingEnabled() const {
    return pImpl->asyncLoggingEnabled && pImpl->asyncLogger;
}

void Logger::setLogBufferSize(size_t size) {
    pImpl->logBufferSize = size;
    if (pImpl->asyncLogger) {
        // Note: AsyncLogger doesn't expose buffer size directly
        // In a real implementation, would expose this configuration
    }
}

void Logger::optimizeForThroughput() {
    pImpl->optimizationMode = "throughput";
    // Configure for high throughput: larger buffers, fewer workers
    setLogBufferSize(8192);
    if (pImpl->asyncLogger) {
        pImpl->asyncLogger->setWorkerThreadCount(1); // Single thread for throughput
    }
}

void Logger::optimizeForLatency() {
    pImpl->optimizationMode = "latency";
    // Configure for low latency: smaller buffers, more workers
    setLogBufferSize(1024);
    if (pImpl->asyncLogger) {
        pImpl->asyncLogger->setWorkerThreadCount(2); // Two threads for latency
    }
}

// Performance monitor implementation
struct PerformanceMonitor::Impl {
    bool monitoringEnabled = true;
    
    struct OperationStats {
        std::string name;
        std::chrono::microseconds duration;
        size_t callCount;
        double averageDuration;
        double totalDuration;
    };
    
    std::vector<OperationStats> stats;
    std::mutex mutex;
};

PerformanceMonitor::PerformanceMonitor() : pImpl(std::make_unique<Impl>()) {}

PerformanceMonitor::~PerformanceMonitor() = default;

void PerformanceMonitor::startOperation(const std::string& operationName) {
    if (!pImpl->monitoringEnabled) return;
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Create or reset operation stats
    OperationStats stats;
    stats.name = operationName;
    stats.duration = std::chrono::microseconds::zero();
    stats.callCount = 0;
    stats.averageDuration = 0.0;
    stats.totalDuration = 0.0;
    
    // Add to stats (simplified - would track actual timing)
    pImpl->stats.push_back(stats);
}

void PerformanceMonitor::endOperation(const std::string& operationName) {
    if (!pImpl->monitoringEnabled) return;
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Find and update operation (simplified implementation)
    for (auto& stats : pImpl->stats) {
        if (stats.name == operationName) {
            // In a real implementation, would measure actual duration
            stats.callCount++;
            stats.totalDuration += 1000; // Dummy duration
            stats.averageDuration = stats.totalDuration / stats.callCount;
            break;
        }
    }
}

std::vector<PerformanceMonitor::OperationStats> PerformanceMonitor::getStats() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->stats;
}

void PerformanceMonitor::clearStats() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->stats.clear();
}

void PerformanceMonitor::enableMonitoring(bool enable) {
    pImpl->monitoringEnabled = enable;
}

bool PerformanceMonitor::isMonitoringEnabled() const {
    return pImpl->monitoringEnabled;
}

PerformanceMonitor& getPerformanceMonitor() {
    static PerformanceMonitor monitor;
    return monitor;
}

} // namespace nlm
