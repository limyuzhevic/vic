#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <chrono>
#include <ctime>
#include <thread>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <cmath>

namespace nlm {

// Helper functions for logging
namespace LogInternal {
    std::string getCurrentThreadName() {
        std::string name = "main";
        // Try to get thread name if available (platform-specific)
        if (std::this_thread::get_id() != std::thread::id()) {
            name = "thread-" + std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id()));
        }
        return name;
    }
    
    uint64_t getCurrentThreadId() {
        auto id = std::this_thread::get_id();
        return std::hash<std::thread::id>{}(id);
    }
    
    std::string formatTimestamp(const std::chrono::system_clock::time_point& timestamp, const std::string& format) {
        auto time = std::chrono::system_clock::to_time_t(timestamp);
        std::tm tm_time;
        localtime_r(&time, &tm_time);
        
        std::stringstream ss;
        ss << std::put_time(&tm_time, format.c_str());
        return ss.str();
    }
    
    std::string replacePatternTokens(const std::string& pattern, const std::string& token, const std::string& replacement) {
        std::string result = pattern;
        size_t pos = 0;
        while ((pos = result.find(token, pos)) != std::string::npos) {
            result.replace(pos, token.length(), replacement);
            pos += replacement.length();
        }
        return result;
    }
    
    bool validatePattern(const std::string& pattern) {
        // Basic validation - check for valid pattern characters
        // % is the escape character, others should be treated as literals
        return true;
    }
}

// ConsoleLogger implementation
struct ConsoleLogger::Impl {
    ConsoleConfig config;
    std::mutex mutex;
    std::mutex fileMutex;
    
    // Performance statistics
    uint64_t totalLogCount = 0;
    uint64_t errorCount = 0;
    uint64_t warningCount = 0;
    double totalLogTime = 0.0;
    std::ofstream fileOut;
    std::string currentFilepath;
    size_t maxFileSize = 10 * 1024 * 1024; // 10MB default
    int maxFiles = 5;
    std::string filePattern = "%Y%m%d.log";
    size_t currentFileSize = 0;
    
    // Asynchronous logging support
    bool asyncEnabled = false;
    std::vector<LogEntry> logBuffer;
    std::mutex bufferMutex;
    size_t bufferSize = 1024;
    std::condition_variable bufferCV;
    std::atomic<bool> stopAsync = false;
    std::thread asyncWriter;
    
    Impl(const ConsoleConfig& cfg) : config(cfg) {
        if (config.async) {
            startAsyncWriter();
        }
    }
    
    ~Impl() {
        stopAsync = true;
        bufferCV.notify_all();
        if (asyncWriter.joinable()) {
            asyncWriter.join();
        }
    }
    
    void startAsyncWriter() {
        stopAsync = false;
        asyncWriter = std::thread(&Impl::asyncWriteLoop, this);
    }
    
    void asyncWriteLoop() {
        while (!stopAsync) {
            std::unique_lock<std::mutex> lock(bufferMutex);
            bufferCV.wait_for(lock, std::chrono::milliseconds(100), [this]() {
                return !logBuffer.empty() || stopAsync;
            });
            
            if (stopAsync && logBuffer.empty()) {
                break;
            }
            
            // Process buffered logs
            while (!logBuffer.empty()) {
                LogEntry entry = std::move(logBuffer.back());
                logBuffer.pop_back();
                lock.unlock();
                
                // Log synchronously
                writeToConsole(entry);
                
                lock.lock();
            }
        }
    }
    
    void writeToConsole(const LogEntry& entry) {
        if (entry.level < config.level) {
            return;
        }
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        std::lock_guard<std::mutex> lock(mutex);
        std::ostringstream oss;
        
        // Build message using pattern
        std::string message = formatMessage(entry);
        
        // Output to console
        if (config.useColors) {
            std::cout << levelToColor(entry.level) << message << "\033[0m\n";
        } else {
            std::cout << message << "\n";
        }
        
        // Output to file if configured
        if (fileOut.is_open()) {
            fileOut << message << "\n";
            fileOut.flush();
            currentFileSize += message.length() + 1; // +1 for newline
            
            // Check if file rotation is needed
            if (currentFileSize > maxFileSize) {
                rotateFile();
            }
        }
        
        // Update statistics
        totalLogCount++;
        if (entry.level >= LogLevel::Error) errorCount++;
        if (entry.level >= LogLevel::Warning) warningCount++;
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        totalLogTime += duration;
    }
    
    std::string formatMessage(const LogEntry& entry) const {
        std::string message = entry.message;
        
        // Apply pattern formatting
        std::string formatted = applyPattern(config.pattern, entry);
        
        return formatted;
    }
    
    std::string applyPattern(const std::string& pattern, const LogEntry& entry) const {
        std::string result = pattern;
        
        // Replace tokens
        result = LogInternal::replacePatternTokens(result, "%L", levelToString(entry.level));
        result = LogInternal::replacePatternTokens(result, "%T", formatTimestamp(entry.timestamp, config.timestampFormat));
        result = LogInternal::replacePatternTokens(result, "%M", entry.message);
        result = LogInternal::replacePatternTokens(result, "%m", entry.message);
        result = LogInternal::replacePatternTokens(result, "%f", entry.file);
        result = LogInternal::replacePatternTokens(result, "%l", std::to_string(entry.line));
        result = LogInternal::replacePatternTokens(result, "%F", entry.file + ":" + std::to_string(entry.line));
        result = LogInternal::replacePatternTokens(result, "%n", entry.function);
        result = LogInternal::replacePatternTokens(result, "%p", getCurrentThreadName());
        result = LogInternal::replacePatternTokens(result, "%i", std::to_string(entry.threadId));
        
        // Add duration if performance monitoring is enabled
        if (config.async) {
            double durationMs = std::chrono::duration<double, std::milli>(entry.duration).count();
            result = LogInternal::replacePatternTokens(result, "%.3fms", std::to_string(durationMs) + "ms");
        }
        
        return result;
    }
    
    std::string levelToString(LogLevel level) const {
        switch (level) {
            case LogLevel::Emergency: return "EMERG";
            case LogLevel::Alert: return "ALERT";
            case LogLevel::Critical: return "CRIT";
            case LogLevel::Error: return "ERROR";
            case LogLevel::Warning: return "WARN";
            case LogLevel::Notice: return "NOTICE";
            case LogLevel::Info: return "INFO";
            case LogLevel::Debug: return "DEBUG";
            case LogLevel::Trace: return "TRACE";
            default: return "UNKNOWN";
        }
    }
    
    std::string levelToColor(LogLevel level) const {
        if (!config.useColors) return "";
        
        switch (level) {
            case LogLevel::Emergency: return "\033[1;35m"; // Bright Magenta
            case LogLevel::Alert: return "\033[1;31m";    // Bright Red
            case LogLevel::Critical: return "\033[1;31m"; // Bright Red
            case LogLevel::Error: return "\033[31m";      // Red
            case LogLevel::Warning: return "\033[33m";   // Yellow
            case LogLevel::Notice: return "\033[36m";    // Cyan
            case LogLevel::Info: return "\033[32m";      // Green
            case LogLevel::Debug: return "\033[90m";     // Bright Black
            case LogLevel::Trace: return "\033[90m";     // Bright Black
            default: return "\033[0m";
        }
    }
    
    void rotateFile() {
        if (!fileOut.is_open()) return;
        
        fileOut.close();
        
        // Close current file and create new one
        time_t now = std::time(nullptr);
        std::tm tm_time;
        localtime_r(&now, &tm_time);
        
        std::stringstream ss;
        ss << "log_" << std::put_time(&tm_time, "%Y%m%d_%H%M%S") << ".log";
        std::string newFilepath = ss.str();
        
        fileOut.open(newFilepath, std::ios::app);
        currentFileSize = 0;
        currentFilepath = newFilepath;
        
        // Clean up old files if we exceed max count
        if (maxFiles > 0) {
            cleanOldFiles();
        }
    }
    
    void cleanOldFiles() {
        std::vector<std::string> files;
        // Implementation would scan directory and remove old log files
        // For now, just close the current file
    }
    
    void flush() {
        std::lock_guard<std::mutex> lock(mutex);
        if (fileOut.is_open()) {
            fileOut.flush();
        }
    }
};

ConsoleLogger::ConsoleLogger(const ConsoleConfig& config) : pImpl(std::make_unique<Impl>(config)) {
    if (config.async) {
        NLM_LOG_INFO("ConsoleLogger initialized with async logging enabled");
    } else {
        NLM_LOG_INFO("ConsoleLogger initialized with sync logging");
    }
}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->config.level = level;
    NLM_LOG_INFO("ConsoleLogger initialized with level: " + pImpl->levelToString(level));
}

ConsoleLogger::~ConsoleLogger() = default;

void ConsoleLogger::log(const LogEntry& entry) {
    if (pImpl->config.async) {
        std::lock_guard<std::mutex> lock(pImpl->bufferMutex);
        pImpl->logBuffer.push_back(entry);
        pImpl->bufferCV.notify_one();
    } else {
        pImpl->writeToConsole(entry);
    }
}

void ConsoleLogger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->config.level = level;
}

LogLevel ConsoleLogger::getLevel() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->config.level;
}

void ConsoleLogger::setPattern(const std::string& pattern) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->config.pattern = pattern;
}

void ConsoleLogger::setUseColors(bool useColors) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->config.useColors = useColors;
}

void ConsoleLogger::setUseTimestamp(bool useTimestamp) {
    // This is handled via pattern, but can be kept for backward compatibility
    if (useTimestamp) {
        if (pImpl->config.pattern.find("%T") == std::string::npos) {
            pImpl->config.pattern = "[%L] %T " + pImpl->config.pattern;
        }
    } else {
        size_t pos = pImpl->config.pattern.find("%T");
        if (pos != std::string::npos) {
            pImpl->config.pattern.erase(pos - 1, 7); // Remove "%T " or " %T"
        }
    }
}

void ConsoleLogger::setUseThreadInfo(bool useThreadInfo) {
    if (useThreadInfo) {
        if (pImpl->config.pattern.find("%p") == std::string::npos) {
            size_t pos = pImpl->config.pattern.find("[]");
            if (pos != std::string::npos) {
                pImpl->config.pattern.replace(pos, 2, "[%p]");
            }
        }
    } else {
        size_t pos = pImpl->config.pattern.find("%p");
        if (pos != std::string::npos) {
            pImpl->config.pattern.erase(pos, 3);
        }
    }
}

void ConsoleLogger::setUseFunctionInfo(bool useFunctionInfo) {
    if (useFunctionInfo) {
        if (pImpl->config.pattern.find("%f") == std::string::npos) {
            pImpl->config.pattern += " %f:%l";
        }
    } else {
        size_t pos = pImpl->config.pattern.find("%f:%l");
        if (pos != std::string::npos) {
            pImpl->config.pattern.erase(pos, 6);
        }
    }
}

void ConsoleLogger::setTimestampFormat(const std::string& format) {
    pImpl->config.timestampFormat = format;
}

void ConsoleLogger::enableAsyncLogging(bool enable) {
    if (enable == pImpl->config.async) return;
    
    pImpl->config.async = enable;
    if (enable) {
        pImpl->startAsyncWriter();
    } else {
        // Stop async writer
        pImpl->stopAsync = true;
        pImpl->bufferCV.notify_all();
        if (pImpl->asyncWriter.joinable()) {
            pImpl->asyncWriter.join();
        }
    }
}

void ConsoleLogger::setOutputFile(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->fileMutex);
    pImpl->fileOut.close();
    
    pImpl->fileOut.open(filepath, std::ios::app);
    if (pImpl->fileOut.is_open()) {
        pImpl->currentFilepath = filepath;
        pImpl->currentFileSize = 0;
    }
}

void ConsoleLogger::setMaxFileSize(size_t maxBytes) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->maxFileSize = maxBytes;
}

void ConsoleLogger::setMaxFiles(int maxCount) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->maxFiles = maxCount;
}

void ConsoleLogger::setFilePattern(const std::string& pattern) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->filePattern = pattern;
}

void ConsoleLogger::closeOutputFile() {
    std::lock_guard<std::mutex> lock(pImpl->fileMutex);
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

bool ConsoleLogger::isFileLoggingEnabled() const {
    std::lock_guard<std::mutex> lock(pImpl->fileMutex);
    return pImpl->fileOut.is_open();
}

double ConsoleLogger::getAverageLogTime() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->totalLogCount > 0 ? (pImpl->totalLogTime / pImpl->totalLogCount) : 0.0;
}

uint64_t ConsoleLogger::getTotalLogCount() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->totalLogCount;
}

uint64_t ConsoleLogger::getErrorCount() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->errorCount;
}

uint64_t ConsoleLogger::getWarningCount() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->warningCount;
}

void ConsoleLogger::flush() {
    pImpl->flush();
}

void ConsoleLogger::setBufferSize(size_t size) {
    std::lock_guard<std::mutex> lock(pImpl->bufferMutex);
    pImpl->bufferSize = size;
    pImpl->logBuffer.reserve(size);
}

// Logger implementation
struct Logger::Impl {
    LoggerConfig config;
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    
    // Global statistics
    uint64_t totalLogCount = 0;
    uint64_t errorCount = 0;
    uint64_t warningCount = 0;
    double totalLogTime = 0.0;
    bool performanceMonitoringEnabled = false;
    std::string defaultPattern = LoggerConfig{}.defaultPattern;
    
    // File logging configuration
    std::string fileLogPath;
    size_t fileMaxSize = 10 * 1024 * 1024;
    int fileMaxFiles = 5;
    bool fileLoggingEnabled = false;
};

Logger::Logger(const LoggerConfig& config) : pImpl(std::make_unique<Impl>(config)) {
    setDefaultPattern(config.defaultPattern);
    setPerformanceMonitoring(config.enablePerformanceMonitoring);
}

Logger::~Logger() = default;

Logger::Logger(Logger&&) noexcept = default;
Logger& Logger::operator=(Logger&&) noexcept = default;

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    if (!logger) {
        NLM_LOG_ERROR("Attempted to add null logger");
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    logger->setLevel(pImpl->config.level); // Apply global level
    pImpl->loggers.push_back(std::move(logger));
}

void Logger::removeLogger(ILogger* logger) {
    if (!logger) return;
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = std::remove_if(pImpl->loggers.begin(), pImpl->loggers.end(),
                           [logger](const std::shared_ptr<ILogger>& lg) {
                               return lg.get() == logger;
                           });
    pImpl->loggers.erase(it, pImpl->loggers.end());
}

void Logger::clearLoggers() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.clear();
}

void Logger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->config.level = level;
    
    // Update all existing loggers
    for (auto& logger : pImpl->loggers) {
        logger->setLevel(level);
    }
}

void Logger::setGlobalLevel(LogLevel level) {
    setLevel(level);
}

void Logger::log(LogLevel level, const std::string& message,
                 const std::string& file, int line, const std::string& function) {
    LogEntry entry(level, message, file, line, function);
    entry.threadName = LogInternal::getCurrentThreadName();
    entry.threadId = LogInternal::getCurrentThreadId();
    
    log(entry);
}

void Logger::log(const LogEntry& entry) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (auto& logger : pImpl->loggers) {
        try {
            logger->log(entry);
        } catch (const std::exception& e) {
            // If logging fails, create a minimal log entry
            std::cerr << "[ERROR] Failed to log: " << e.what() << std::endl;
        }
    }
    
    // Update statistics
    pImpl->totalLogCount++;
    if (entry.level >= LogLevel::Error) pImpl->errorCount++;
    if (entry.level >= LogLevel::Warning) pImpl->warningCount++;
    
    if (pImpl->performanceMonitoringEnabled) {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        pImpl->totalLogTime += duration;
    }
}

void Logger::emergency(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Emergency, message, file, line, function);
}

void Logger::alert(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Alert, message, file, line, function);
}

void Logger::critical(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Critical, message, file, line, function);
}

void Logger::error(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Error, message, file, line, function);
}

void Logger::warning(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Warning, message, file, line, function);
}

void Logger::notice(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Notice, message, file, line, function);
}

void Logger::info(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Info, message, file, line, function);
}

void Logger::debug(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Debug, message, file, line, function);
}

void Logger::trace(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Trace, message, file, line, function);
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

double Logger::getAverageLogTime() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->totalLogCount > 0 ? (pImpl->totalLogTime / pImpl->totalLogCount) : 0.0;
}

uint64_t Logger::getTotalLogCount() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->totalLogCount;
}

uint64_t Logger::getErrorCount() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->errorCount;
}

uint64_t Logger::getWarningCount() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->warningCount;
}

std::string Logger::getStatistics() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::stringstream ss;
    ss << "=== Logger Statistics ===\n";
    ss << "Total log count: " << pImpl->totalLogCount << "\n";
    ss << "Error count: " << pImpl->errorCount << "\n";
    ss << "Warning count: " << pImpl->warningCount << "\n";
    ss << "Average log time: " << getAverageLogTime() << " ms\n";
    ss << "Number of loggers: " << pImpl->loggers.size() << "\n";
    if (pImpl->performanceMonitoringEnabled) {
        ss << "Performance monitoring: Enabled\n";
    } else {
        ss << "Performance monitoring: Disabled\n";
    }
    return ss.str();
}

void Logger::setDefaultPattern(const std::string& pattern) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->defaultPattern = pattern;
}

void Logger::setPerformanceMonitoring(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->performanceMonitoringEnabled = enable;
}

void Logger::flushAll() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        // Try to cast to ConsoleLogger to access flush method
        if (auto consoleLogger = std::dynamic_pointer_cast<ConsoleLogger>(logger)) {
            consoleLogger->flush();
        }
    }
}

void Logger::configureFileLogging(const std::string& filepath, size_t maxSize, int maxFiles) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->fileLogPath = filepath;
    pImpl->fileMaxSize = maxSize;
    pImpl->fileMaxFiles = maxFiles;
    pImpl->fileLoggingEnabled = true;
    
    // Add a file logger
    auto config = ConsoleConfig{};
    config.level = pImpl->config.level;
    config.pattern = "%Y-%m-%d %H:%M:%S [%L] %M";
    config.useColors = false;
    config.useTimestamp = true;
    config.timestampFormat = "%Y-%m-%d %H:%M:%S";
    
    auto fileLogger = std::make_shared<ConsoleLogger>(config);
    fileLogger->setOutputFile(filepath);
    fileLogger->setMaxFileSize(maxSize);
    fileLogger->setMaxFiles(maxFiles);
    
    addLogger(fileLogger);
}

void Logger::disableFileLogging() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->fileLoggingEnabled = false;
    
    // Find and remove file logger
    auto it = std::remove_if(pImpl->loggers.begin(), pImpl->loggers.end(),
                           [this](const std::shared_ptr<ILogger>& logger) {
                               // Try to detect file logger by checking if it has a file open
                               if (auto consoleLogger = std::dynamic_pointer_cast<ConsoleLogger>(logger)) {
                                   return consoleLogger->isFileLoggingEnabled();
                               }
                               return false;
                           });
    pImpl->loggers.erase(it, pImpl->loggers.end());
}

std::string Logger::getCurrentThreadName() const {
    return LogInternal::getCurrentThreadName();
}

uint64_t Logger::getCurrentThreadId() const {
    return LogInternal::getCurrentThreadId();
}

std::string Logger::formatTimestamp(const std::chrono::system_clock::time_point& timestamp) const {
    return LogInternal::formatTimestamp(timestamp, "%Y-%m-%d %H:%M:%S");
}

// LogStream implementation
Logger::LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function) {
}

Logger::LogStream::~LogStream() {
    Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
}

// LogUtils namespace implementation
namespace LogUtils {
    std::string escapePattern(const std::string& pattern) {
        return pattern;  // No escaping needed for current implementation
    }
    
    bool validatePattern(const std::string& pattern) {
        if (pattern.empty()) return false;
        
        // Check for null characters
        if (pattern.find('\0') != std::string::npos) {
            return false;
        }
        
        // Basic pattern validation - ensure no invalid characters
        // This is a simplified validation
        return true;
    }
    
    std::vector<std::string> extractTokens(const std::string& pattern) {
        std::vector<std::string> tokens;
        std::string current;
        
        for (char c : pattern) {
            if (c == '%' && !current.empty()) {
                tokens.push_back(current);
                current.clear();
            } else {
                current += c;
            }
        }
        
        if (!current.empty()) {
            tokens.push_back(current);
        }
        
        return tokens;
    }
}

} // namespace nlm