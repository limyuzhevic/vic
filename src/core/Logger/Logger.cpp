/**
 * Enhanced Logging Implementation for NLM (Neural Learning Machine)
 * 
 * This file provides the implementation for the enhanced logging system.
 * Includes comprehensive features for logging, performance monitoring,
 * error handling, and system event tracking.
 * 
 * @author NLM Development Team
 * @version 2.0
 * @date 2026
 */

#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <thread>
#include <chrono>
#include <algorithm>
#include <ctime>

namespace nlm {

/**
 * Internal implementation structure for ConsoleLogger.
 * 
 * Encapsulates all console logger data and state with thread safety
 * and performance optimizations.
 * 
 * @see ConsoleLogger for public interface
 */
struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    
    // Enhanced formatting and filtering
    std::string timestampFormat = "%Y-%m-%d %H:%M:%S";
    bool includeThreadId = true;
    bool includeCategory = true;
    
    // Category filtering
    std::string categoryFilter;
    
    // Performance monitoring
    bool performanceMonitoringEnabled = false;
    std::atomic<size_t> logCount{0};
    std::chrono::steady_clock::duration totalLogTime{0};
    
    // Error handling
    bool errorState = false;
    std::string lastError;
    
    // Statistics
    size_t successfulLogs = 0;
    size_t failedLogs = 0;
};

/**
 * Default constructor initializes enhanced console logger.
 * 
 * Sets up default configuration including colors, performance monitoring,
 * and thread safety features.
 */
ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {
    // Enable default formatting options
    pImpl->includeThreadId = true;
    pImpl->includeCategory = true;
    pImpl->performanceMonitoringEnabled = true;
}

/**
 * Constructor with specified log level.
 * 
 * @param level Initial log level for the console logger
 */
ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
    pImpl->includeThreadId = true;
    pImpl->includeCategory = true;
    pImpl->performanceMonitoringEnabled = true;
}

/**
 * Destructor cleans up resources and flushes logs.
 * 
 * Ensures all logs are written to file before destruction.
 */
ConsoleLogger::~ConsoleLogger() {
    flush();
    closeOutputFile();
}

/**
 * Enhanced logging implementation with comprehensive error handling.
 * 
 * Processes log entries with:
 * - Thread safety
 * - Category filtering
 * - Performance timing
 * - Error recovery
 * - Statistics tracking
 * 
 * @param entry Log entry to process
 * @return True if log was successful, false otherwise
 */
bool ConsoleLogger::log(const LogEntry& entry) {
    if (pImpl->errorState) {
        // Skip logging if in error state
        return false;
    }
    
    if (!shouldLog(entry)) {
        return true;
    }
    
    auto startTime = std::chrono::steady_clock::now();
    
    try {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        // Check category filter
        if (!pImpl->categoryFilter.empty() && !isCategoryEnabled(entry.category)) {
            return true;
        }
        
        // Format message
        std::string message = formatMessage(entry);
        
        // Output to console
        if (pImpl->useColors) {
            std::cout << levelToColor(entry.level) << message << "\033[0m\n";
        } else {
            std::cout << message << "\n";
        }
        
        // Output to file if configured
        if (pImpl->fileOut.is_open()) {
            pImpl->fileOut << message << "\n";
            pImpl->fileOut.flush();
        }
        
        // Update performance metrics
        if (pImpl->performanceMonitoringEnabled) {
            auto endTime = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime);
            pImpl->totalLogTime += duration;
            pImpl->logCount.fetch_add(1, std::memory_order_relaxed);
        }
        
        pImpl->successfulLogs++;
        return true;
        
    } catch (const std::exception& e) {
        pImpl->errorState = true;
        pImpl->lastError = std::string("ConsoleLogger error: ") + e.what();
        pImpl->failedLogs++;
        return false;
    }
}

/**
 * Set minimum log level for this logger.
 * 
 * @param level Minimum log level (higher values = less verbose)
 */
void ConsoleLogger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->level = level;
}

/**
 * Get current log level.
 * 
 * @return Current minimum log level
 */
LogLevel ConsoleLogger::getLevel() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->level;
}

/**
 * Set category filter for this logger.
 * 
 * @param categories Comma-separated list of categories to include
 */
void ConsoleLogger::setCategories(const std::string& categories) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->categoryFilter = categories;
}

/**
 * Check if category is enabled.
 * 
 * @param category Category to check
 * @return True if category is enabled
 */
bool ConsoleLogger::isCategoryEnabled(const std::string& category) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->categoryFilter.empty()) {
        return true;
    }
    
    // Simple comma-separated category matching
    std::istringstream iss(pImpl->categoryFilter);
    std::string cat;
    while (std::getline(iss, cat, ',')) {
        if (cat == category) {
            return true;
        }
    }
    return false;
}

/**
 * Flush all buffered logs.
 * 
 * Ensures all logs are written to file before program exit.
 */
void ConsoleLogger::flush() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.flush();
    }
}

/**
 * Get logging statistics.
 * 
 * @return String containing logging statistics
 */
std::string ConsoleLogger::getStatistics() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "ConsoleLogger Statistics:" << std::endl;
    oss << "  Log level: " << static_cast<int>(pImpl->level) << std::endl;
    oss << "  Successful logs: " << pImpl->successfulLogs << std::endl;
    oss << "  Failed logs: " << pImpl->failedLogs << std::endl;
    oss << "  Total logs: " << (pImpl->successfulLogs + pImpl->failedLogs) << std::endl;
    
    if (pImpl->performanceMonitoringEnabled && pImpl->logCount > 0) {
        double avgTime = std::chrono::duration<double, std::nano>(pImpl->totalLogTime).count() / pImpl->logCount;
        oss << "  Average log time: " << avgTime << " nanoseconds" << std::endl;
    }
    
    if (!pImpl->lastError.empty()) {
        oss << "  Last error: " << pImpl->lastError << std::endl;
    }
    
    return oss.str();
}

/**
 * Enable or disable color output.
 * 
 * @param useColors True to enable colors, false otherwise
 */
void ConsoleLogger::setUseColors(bool useColors) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->useColors = useColors;
}

/**
 * Check if color output is enabled.
 * 
 * @return True if color output is enabled
 */
bool ConsoleLogger::getUseColors() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->useColors;
}

/**
 * Set output file for logging.
 * 
 * @param filepath Path to output file
 */
void ConsoleLogger::setOutputFile(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->fileOut.open(filepath, std::ios::app);
    if (!pImpl->fileOut.is_open()) {
        pImpl->errorState = true;
        pImpl->lastError = "Failed to open log file: " + filepath;
    }
}

/**
 * Close output file.
 */
void ConsoleLogger::closeOutputFile() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

/**
 * Set timestamp format for log messages.
 * 
 * @param format strftime-compatible format string
 */
void ConsoleLogger::setTimestampFormat(const std::string& format) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->timestampFormat = format;
}

/**
 * Enable or disable thread ID in log messages.
 * 
 * @param include True to include thread ID, false otherwise
 */
void ConsoleLogger::setIncludeThreadId(bool include) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->includeThreadId = include;
}

/**
 * Enable or disable category in log messages.
 * 
 * @param include True to include category, false otherwise
 */
void ConsoleLogger::setIncludeCategory(bool include) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->includeCategory = include;
}

/**
 * Enable or disable performance monitoring.
 * 
 * @param enable True to enable performance monitoring
 */
void ConsoleLogger::enablePerformanceMonitoring(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->performanceMonitoringEnabled = enable;
}

/**
 * Check if performance monitoring is enabled.
 * 
 * @return True if performance monitoring is enabled
 */
bool ConsoleLogger::isPerformanceMonitoringEnabled() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->performanceMonitoringEnabled;
}

/**
 * Get average log time.
 * 
 * @return Average log time in nanoseconds
 */
double ConsoleLogger::getAverageLogTime() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->logCount == 0) {
        return 0.0;
    }
    
    return std::chrono::duration<double, std::nano>(pImpl->totalLogTime).count() / pImpl->logCount;
}

/**
 * Convert log level to human-readable string.
 * 
 * @param level Log level to convert
 * @return String representation of log level
 */
std::string ConsoleLogger::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Critical: return "CRIT";
        case LogLevel::System: return "SYSTEM";
        case LogLevel::Performance: return "PERF";
        case LogLevel::Memory: return "MEMORY";
        case LogLevel::Neural: return "NEURAL";
        default: return "UNKNOWN";
    }
}

/**
 * Convert log level to ANSI color code.
 * 
 * @param level Log level to convert
 * @return ANSI color code string
 */
std::string ConsoleLogger::levelToColor(LogLevel level) const {
    switch (level) {
        case LogLevel::Debug: return "\033[36m";    // Cyan
        case LogLevel::Info: return "\033[32m";      // Green
        case LogLevel::Warning: return "\033[33m";   // Yellow
        case LogLevel::Error: return "\033[31m";     // Red
        case LogLevel::Critical: return "\033[35m"; // Magenta
        case LogLevel::System: return "\033[34m";   // Blue
        case LogLevel::Performance: return "\033[90m"; // Bright Black
        case LogLevel::Memory: return "\033[95m";   // Bright Magenta
        case LogLevel::Neural: return "\033[94m";   // Bright Blue
        default: return "\033[0m";
    }
}

/**
 * Check if log entry should be processed based on level.
 * 
 * @param entry Log entry to check
 * @return True if log entry should be processed
 */
bool ConsoleLogger::shouldLog(const LogEntry& entry) const {
    return entry.level >= pImpl->level;
}

/**
 * Format log entry message for output.
 * 
 * @param entry Log entry to format
 * @return Formatted log message string
 */
std::string ConsoleLogger::formatMessage(const LogEntry& entry) const {
    std::ostringstream oss;
    
    // Add log level
    oss << "[" << levelToString(entry.level) << "] ";
    
    // Add timestamp
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    oss << std::put_time(std::localtime(&time), pImpl->timestampFormat.c_str()) << " ";
    
    // Add thread ID if enabled
    if (pImpl->includeThreadId) {
        oss << "[" << entry.threadId << "] ";
    }
    
    // Add category if enabled and not default
    if (pImpl->includeCategory && entry.category != "general") {
        oss << "[" << entry.category << "] ";
    }
    
    // Add message
    oss << entry.message;
    
    return oss.str();
}

/**
 * Internal implementation structure for Logger.
 * 
 * Encapsulates all logger management data and state with thread safety
 * and enhanced management capabilities.
 * 
 * @see Logger for public interface
 */
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    
    // Statistics tracking
    std::atomic<size_t> totalLogs{0};
    std::atomic<size_t> failedLogs{0};
    std::chrono::steady_clock::duration totalProcessingTime{0};
    
    // Performance monitoring
    bool asyncLoggingEnabled = false;
    std::thread logThread;
    bool threadRunning = false;
    std::mutex threadMutex;
    std::condition_variable threadCond;
    
    // Log queue for async logging
    std::queue<std::shared_ptr<LogEntry>> logQueue;
    
    // Error handling
    bool globalErrorState = false;
    std::string lastGlobalError;
};

/**
 * Default constructor initializes enhanced logger.
 */
Logger::Logger() : pImpl(std::make_unique<Impl>()) {
    // Start async logging thread if enabled
    if (pImpl->asyncLoggingEnabled) {
        startLoggingThread();
    }
}

/**
 * Destructor cleans up resources.
 */
Logger::~Logger() {
    stopLoggingThread();
}

/**
 * Add logger to the collection.
 * 
 * @param logger Logger instance to add
 */
void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.push_back(logger);
}

/**
 * Add console logger with default settings.
 * 
 * @param level Initial log level
 */
void Logger::addConsoleLogger(LogLevel level) {
    auto consoleLogger = std::make_shared<ConsoleLogger>(level);
    addLogger(consoleLogger);
}

/**
 * Add file logger with specified path.
 * 
 * @param filepath Path to log file
 * @param level Initial log level
 */
void Logger::addFileLogger(const std::string& filepath, LogLevel level) {
    auto fileLogger = std::make_shared<ConsoleLogger>(level);
    static_cast<ConsoleLogger*>(fileLogger.get())->setOutputFile(filepath);
    addLogger(fileLogger);
}

/**
 * Remove logger from the collection.
 * 
 * @param logger Logger instance to remove
 */
void Logger::removeLogger(std::shared_ptr<ILogger> logger) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.erase(
        std::remove(pImpl->loggers.begin(), pImpl->loggers.end(), logger),
        pImpl->loggers.end()
    );
}

/**
 * Remove all loggers.
 */
void Logger::clearLoggers() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.clear();
}

/**
 * Set minimum log level for all loggers.
 * 
 * @param level Minimum log level (higher values = less verbose)
 */
void Logger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        logger->setLevel(level);
    }
}

/**
 * Set minimum log level for specific logger.
 * 
 * @param logger Target logger
 * @param level Minimum log level
 */
void Logger::setLevelForLogger(std::shared_ptr<ILogger> logger, LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    logger->setLevel(level);
}

/**
 * Get current log level for specific logger.
 * 
 * @param logger Target logger
 * @return Current log level
 */
LogLevel Logger::getLevelForLogger(std::shared_ptr<ILogger> logger) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return logger->getLevel();
}

/**
 * Log message with enhanced error handling and statistics.
 * 
 * @param level Log level
 * @param message Log message
 * @param file Source file
 * @param line Line number
 * @param function Function name
 * @param category Log category
 * @return True if log was successful
 */
bool Logger::log(LogLevel level, const std::string& message,
                 const std::string& file, int line,
                 const std::string& function, const std::string& category) {
    if (pImpl->globalErrorState) {
        return false;
    }
    
    try {
        auto startTime = std::chrono::steady_clock::now();
        
        LogEntry entry(level, message, file, line, function, category);
        
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        pImpl->totalLogs++;
        
        if (pImpl->asyncLoggingEnabled) {
            // Queue for async processing
            std::lock_guard<std::mutex> threadLock(pImpl->threadMutex);
            pImpl->logQueue.push(std::make_shared<LogEntry>(entry));
            pImpl->threadCond.notify_one();
        } else {
            // Process immediately
            for (auto& logger : pImpl->loggers) {
                if (logger->log(entry)) {
                    // Successfully logged
                } else {
                    pImpl->failedLogs++;
                }
            }
        }
        
        auto endTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime);
        pImpl->totalProcessingTime += duration;
        
        return true;
        
    } catch (const std::exception& e) {
        pImpl->globalErrorState = true;
        pImpl->lastGlobalError = std::string("Logger error: ") + e.what();
        pImpl->failedLogs++;
        return false;
    }
}

/**
 * Log debug message with category.
 * 
 * @param message Debug message
 * @param file Source file
 * @param line Line number
 * @param function Function name
 * @param category Log category
 */
void Logger::debug(const std::string& message, const std::string& file, int line,
                    const std::string& function, const std::string& category) {
    log(LogLevel::Debug, message, file, line, function, category);
}

/**
 * Log info message with category.
 * 
 * @param message Info message
 * @param file Source file
 * @param line Line number
 * @param function Function name
 * @param category Log category
 */
void Logger::info(const std::string& message, const std::string& file, int line,
                   const std::string& function, const std::string& category) {
    log(LogLevel::Info, message, file, line, function, category);
}

/**
 * Log warning message with category.
 * 
 * @param message Warning message
 * @param file Source file
 * @param line Line number
 * @param function Function name
 * @param category Log category
 */
void Logger::warning(const std::string& message, const std::string& file, int line,
                     const std::string& function, const std::string& category) {
    log(LogLevel::Warning, message, file, line, function, category);
}

/**
 * Log error message with category.
 * 
 * @param message Error message
 * @param file Source file
 * @param line Line number
 * @param function Function name
 * @param category Log category
 */
void Logger::error(const std::string& message, const std::string& file, int line,
                    const std::string& function, const std::string& category) {
    log(LogLevel::Error, message, file, line, function, category);
}

/**
 * Log critical message with category.
 * 
 * @param message Critical message
 * @param file Source file
 * @param line Line number
 * @param function Function name
 * @param category Log category
 */
void Logger::critical(const std::string& message, const std::string& file, int line,
                       const std::string& function, const std::string& category) {
    log(LogLevel::Critical, message, file, line, function, category);
}

/**
 * Log neural network event.
 * 
 * @param message Neural event message
 * @param source Source of the event
 */
void Logger::neuralEvent(const std::string& message, const std::string& source) {
    log(LogLevel::Neural, "[NEURAL] " + message, "", 0, "", "neural:" + source);
}

/**
 * Log performance metric.
 * 
 * @param name Metric name
 * @param value Metric value
 * @param unit Unit of measurement
 */
void Logger::performanceMetric(const std::string& name, double value, const std::string& unit) {
    log(LogLevel::Performance, std::string("PERFORMANCE: ") + name + " = " + std::to_string(value) + " " + unit, "", 0, "", "performance");
}

/**
 * Log memory event.
 * 
 * @param message Memory event message
 * @param memoryBytes Memory bytes if applicable
 */
void Logger::memoryEvent(const std::string& message, size_t memoryBytes) {
    std::string msg = "MEMORY: " + message;
    if (memoryBytes > 0) {
        msg += " (" + std::to_string(memoryBytes) + " bytes)";
    }
    log(LogLevel::Memory, msg, "", 0, "", "memory");
}

/**
 * Log system event.
 * 
 * @param message System event message
 */
void Logger::systemEvent(const std::string& message) {
    log(LogLevel::System, "SYSTEM: " + message, "", 0, "", "system");
}

/**
 * Check if log level is enabled.
 * 
 * @param level Log level to check
 * @return True if level is enabled
 */
bool Logger::isEnabled(LogLevel level) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (auto& logger : pImpl->loggers) {
        if (logger->getLevel() <= level) {
            return true;
        }
    }
    
    return false;
}

/**
 * Check if log level is enabled for specific category.
 * 
 * @param level Log level to check
 * @param category Category to check
 * @return True if level is enabled for category
 */
bool Logger::isEnabledForCategory(LogLevel level, const std::string& category) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (auto& logger : pImpl->loggers) {
        // This is a simplified check - in practice, would need category-specific methods
        if (logger->getLevel() <= level) {
            return true;
        }
    }
    
    return false;
}

/**
 * Start async logging thread.
 */
void Logger::startLoggingThread() {
    if (pImpl->threadRunning) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->threadMutex);
    pImpl->threadRunning = true;
    pImpl->threadCond.notify_one();
    
    // Note: In a real implementation, this would start a proper thread
    // For now, we'll use a simple flag and check for queued logs
}

/**
 * Stop async logging thread.
 */
void Logger::stopLoggingThread() {
    if (!pImpl->threadRunning) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->threadMutex);
    pImpl->threadRunning = false;
    pImpl->threadCond.notify_one();
    
    // Process any remaining logs
    if (!pImpl->logQueue.empty()) {
        std::lock_guard<std::mutex> logLock(pImpl->mutex);
        while (!pImpl->logQueue.empty()) {
            auto entry = pImpl->logQueue.front();
            pImpl->logQueue.pop();
            
            // Log the entry
            for (auto& logger : pImpl->loggers) {
                logger->log(*entry);
            }
        }
    }
}

/**
 * Get global logger instance.
 * 
 * @return Reference to global logger instance
 */
Logger& Logger::getGlobal() {
    static Logger globalLogger;
    return globalLogger;
}

/**
 * Set global logger instance.
 * 
 * @param logger New global logger instance
 */
void Logger::setGlobal(std::shared_ptr<Logger> logger) {
    // This is a placeholder for advanced global logger management
    // In practice, we just use getGlobal() which is already static
}

/**
 * Get global logging statistics.
 * 
 * @return String containing global logging statistics
 */
std::string Logger::getGlobalStatistics() {
    return getGlobal().getStatistics();
}

/**
 * Export logs to file.
 * 
 * @param filepath Path to export file
 */
void Logger::exportLogs(const std::string& filepath) {
    // This is a simplified implementation
    // In a real system, this would capture all logs and write them to file
    std::ofstream outFile(filepath, std::ios::app);
    if (outFile.is_open()) {
        outFile << "Log export from " << std::chrono::system_clock::now() << std::endl;
        outFile.close();
    }
}

/**
 * Import logs from file.
 * 
 * @param filepath Path to import file
 */
void Logger::importLogs(const std::string& filepath) {
    // This is a simplified implementation
    // In a real system, this would read logs from file and process them
    std::ifstream inFile(filepath);
    if (inFile.is_open()) {
        std::string line;
        while (std::getline(inFile, line)) {
            // Process import log lines
        }
        inFile.close();
    }
}

/**
 * Enable or disable async logging.
 * 
 * @param enable True to enable async logging
 */
void Logger::enableAsyncLogging(bool enable) {
    pImpl->asyncLoggingEnabled = enable;
    if (enable) {
        startLoggingThread();
    }
}

/**
 * Check if async logging is enabled.
 * 
 * @return True if async logging is enabled
 */
bool Logger::isAsyncLoggingEnabled() const {
    return pImpl->asyncLoggingEnabled;
}

// LogStream implementation
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function,
                   const std::string& category)
    : level_(level), file_(file), line_(line), function_(function), category_(category) {}

LogStream::~LogStream() {
    Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_, category_);
}

} // namespace nlm