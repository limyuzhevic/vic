#include "LoggingSystem.hpp"
#include <thread>
#include <filesystem>
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <sstream>
#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>

namespace nlm {

namespace LogUtils {
    static std::string getProcessId() {
        std::ostringstream oss;
        oss << getpid();
        return oss.str();
    }
    
    static std::string getThreadId() {
        std::ostringstream oss;
        oss << std::this_thread::get_id();
        return oss.str();
    }
}

// Logger implementation
LogManager::Logger::Logger(const std::string& name, Level level)
    : name_(name), level_(level), maxFileSize_(Constants::MAX_LOG_FILE_SIZE),
      maxLogFiles_(5), maxHistoryEntries_(Constants::MAX_LOG_ENTRIES) {
    
    spdLogger_ = spdlog::get(name_);
    if (!spdLogger_) {
        // Try to create a color console sink first
        auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        consoleSink->set_level(spdlog::level::level_enum::trace);
        
        // Create logger with console sink
        spdLogger_ = std::make_shared<spdlog::logger>(name_, std::move(consoleSink));
        spdlog::register_logger(spdLogger_);
        
        spdLogger_->set_level(static_cast<spdlog::level::level_enum>(level_));
        spdLogger_->set_pattern("[%Y-%m-%d %H:%M:%S.%f] [%n] [%l] %v");
    }
    
    spdLogger_->set_level(static_cast<spdlog::level::level_enum>(level_));
}

void LogManager::Logger::trace(const std::string& message) {
    if (isLevelEnabled(Level::Trace)) {
        logInternal(Level::Trace, message);
    }
}

void LogManager::Logger::debug(const std::string& message) {
    if (isLevelEnabled(Level::Debug)) {
        logInternal(Level::Debug, message);
    }
}

void LogManager::Logger::info(const std::string& message) {
    if (isLevelEnabled(Level::Info)) {
        logInternal(Level::Info, message);
    }
}

void LogManager::Logger::warn(const std::string& message) {
    if (isLevelEnabled(Level::Warn)) {
        logInternal(Level::Warn, message);
    }
}

void LogManager::Logger::error(const std::string& message) {
    if (isLevelEnabled(Level::Error)) {
        logInternal(Level::Error, message);
    }
}

void LogManager::Logger::critical(const std::string& message) {
    if (isLevelEnabled(Level::Critical)) {
        logInternal(Level::Critical, message);
    }
}

void LogManager::Logger::logInternal(Level level, const std::string& message) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Create log entry
    LogEntry entry;
    entry.level = level;
    entry.message = LogUtils::sanitizeLogMessage(message);
    entry.source = name_;
    entry.timestamp = std::chrono::system_clock::now();
    entry.threadId = std::this_thread::get_id();
    
    // Add to history
    logHistory_.push_back(entry);
    if (logHistory_.size() > maxHistoryEntries_) {
        logHistory_.erase(logHistory_.begin());
    }
    
    // Write to spdlog
    std::string formattedMessage = formatMessage(message, level);
    switch (level) {
        case Level::Trace:
            spdLogger_->trace(formattedMessage);
            break;
        case Level::Debug:
            spdLogger_->debug(formattedMessage);
            break;
        case Level::Info:
            spdLogger_->info(formattedMessage);
            break;
        case Level::Warn:
            spdLogger_->warn(formattedMessage);
            break;
        case Level::Error:
            spdLogger_->error(formattedMessage);
            break;
        case Level::Critical:
            spdLogger_->critical(formattedMessage);
            break;
    }
}

void LogManager::Logger::setLevel(Level level) {
    level_ = level;
    spdLogger_->set_level(static_cast<spdlog::level::level_enum>(level_));
}

std::string LogManager::Logger::formatMessage(const std::string& message, Level level) const {
    std::stringstream ss;
    ss << "[" << getTimestamp() << "] "
       << "[" << name_ << "] "
       << "[" << LogUtils::getLogLevelName(level) << "] "
       << message;
    return ss.str();
}

std::string LogManager::Logger::getTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

void LogManager::Logger::enableFileLogging(const std::string& filename, bool enable) {
    if (enable) {
        setLogFile(filename);
        auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(filename, true);
        fileSink->set_level(spdlog::level::level_enum::trace);
        
        // Create a new logger with both console and file sinks
        auto consoleSink = spdLogger_->sinks()[0];
        std::vector<spdlog::sink_ptr> sinks = {consoleSink, fileSink};
        spdLogger_ = std::make_shared<spdlog::logger>(name_, sinks);
        spdlog::register_logger(spdLogger_);
        spdLogger_->set_level(static_cast<spdlog::level::level_enum>(level_));
    } else {
        setLogFile("");
        // Keep only console sink
        auto consoleSink = spdLogger_->sinks()[0];
        spdLogger_ = std::make_shared<spdlog::logger>(name_, std::vector<spdlog::sink_ptr>{consoleSink});
        spdlog::register_logger(spdLogger_);
        spdLogger_->set_level(static_cast<spdlog::level::level_enum>(level_));
    }
}

void LogManager::Logger::setLogFile(const std::string& filename) {
    logFilename_ = filename;
}

void LogManager::Logger::enableLogHistory(size_t maxEntries) {
    maxHistoryEntries_ = maxEntries;
    logHistory_.clear();
}

void LogManager::Logger::addLogPattern(const std::string& pattern) {
    logPatterns_.push_back(pattern);
}

bool LogManager::Logger::matchesPattern(const std::string& message) const {
    return LogUtils::isLogPatternMatch(message, logPatterns_);
}

void LogManager::Logger::setMaxFileSize(size_t maxSize) {
    maxFileSize_ = maxSize;
}

void LogManager::Logger::setMaxLogFiles(size_t maxFiles) {
    maxLogFiles_ = maxFiles;
}

void LogManager::Logger::flush() {
    spdLogger_->flush();
}

bool LogManager::Logger::isLevelEnabled(Level level) const {
    spdlog::level::level_enum spdLevel = static_cast<spdlog::level::level_enum>(level);
    return spdLogger_->level() <= spdLevel;
}

std::shared_ptr<LogManager::Logger> LogManager::Logger::getInstance(const std::string& name) {
    static std::unordered_map<std::string, std::shared_ptr<Logger>> instances;
    std::lock_guard<std::mutex> lock(globalMutex_);
    
    if (instances.find(name) == instances.end()) {
        instances[name] = std::make_shared<Logger>(name);
    }
    
    return instances[name];
}

void LogManager::Logger::initializeDefaultLogger() {
    getInstance("NLM");
}

void LogManager::Logger::shutdown() {
    spdlog::drop_all();
}

// MultiLogger implementation
LogManager::MultiLogger::MultiLogger(const std::string& name) : name_(name), globalLevel_(Level::Info), synchronousLogging_(false) {}

void LogManager::MultiLogger::addLogger(std::shared_ptr<Logger> logger) {
    std::lock_guard<std::mutex> lock(mutex_);
    loggers_.push_back(logger);
}

void LogManager::MultiLogger::removeLogger(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    loggers_.erase(std::remove_if(loggers_.begin(), loggers_.end(),
                                 [&name](const std::shared_ptr<Logger>& logger) {
                                     return logger->getName() == name;
                                 }),
                   loggers_.end());
}

std::shared_ptr<LogManager::Logger> LogManager::MultiLogger::getLogger(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    for (const auto& logger : loggers_) {
        if (logger->getName() == name) {
            return logger;
        }
    }
    return nullptr;
}

void LogManager::MultiLogger::trace(const std::string& message) {
    for (const auto& logger : loggers_) {
        if (logger->isLevelEnabled(Level::Trace)) {
            logger->trace(message);
        }
    }
}

void LogManager::MultiLogger::debug(const std::string& message) {
    for (const auto& logger : loggers_) {
        if (logger->isLevelEnabled(Level::Debug)) {
            logger->debug(message);
        }
    }
}

void LogManager::MultiLogger::info(const std::string& message) {
    for (const auto& logger : loggers_) {
        if (logger->isLevelEnabled(Level::Info)) {
            logger->info(message);
        }
    }
}

void LogManager::MultiLogger::warn(const std::string& message) {
    for (const auto& logger : loggers_) {
        if (logger->isLevelEnabled(Level::Warn)) {
            logger->warn(message);
        }
    }
}

void LogManager::MultiLogger::error(const std::string& message) {
    for (const auto& logger : loggers_) {
        if (logger->isLevelEnabled(Level::Error)) {
            logger->error(message);
        }
    }
}

void LogManager::MultiLogger::critical(const std::string& message) {
    for (const auto& logger : loggers_) {
        if (logger->isLevelEnabled(Level::Critical)) {
            logger->critical(message);
        }
    }
}

void LogManager::MultiLogger::setGlobalLevel(Level level) {
    globalLevel_ = level;
    for (const auto& logger : loggers_) {
        logger->setLevel(level);
    }
}

void LogManager::MultiLogger::enableSynchronousLogging(bool enable) {
    synchronousLogging_ = enable;
    for (const auto& logger : loggers_) {
        // Flush each logger to ensure synchronous behavior
        logger->flush();
    }
}

size_t LogManager::MultiLogger::getTotalLogCount() const {
    size_t total = 0;
    for (const auto& logger : loggers_) {
        total += logger->getLogHistory().size();
    }
    return total;
}

std::unordered_map<std::string, size_t> LogManager::MultiLogger::getLogCountsByLevel() const {
    std::unordered_map<std::string, size_t> counts;
    for (const auto& logger : loggers_) {
        auto history = logger->getLogHistory();
        for (const auto& entry : history) {
            std::string levelName = LogUtils::getLogLevelName(entry.level);
            counts[levelName]++;
        }
    }
    return counts;
}

std::unordered_map<std::string, size_t> LogManager::MultiLogger::getLogCountsBySource() const {
    std::unordered_map<std::string, size_t> counts;
    for (const auto& logger : loggers_) {
        auto history = logger->getLogHistory();
        for (const auto& entry : history) {
            counts[entry.source]++;
        }
    }
    return counts;
}

// Global logger functions
std::shared_ptr<LogManager::MultiLogger> LogManager::getGlobalLogger() {
    std::lock_guard<std::mutex> lock(globalMutex_);
    if (!globalLogger_) {
        globalLogger_ = std::make_shared<MultiLogger>("GlobalNLM");
        // Add default console logger
        globalLogger_->addLogger(Logger::getInstance("NLM"));
    }
    return globalLogger_;
}

void LogManager::setGlobalLogLevel(Level level) {
    getGlobalLogger()->setGlobalLevel(level);
}

Level LogManager::getGlobalLogLevel() {
    return getGlobalLogger()->getGlobalLevel();
}

void LogManager::enableFileLogging(const std::string& filename, bool enable) {
    getGlobalLogger()->getLogger("NLM")->enableFileLogging(filename, enable);
}

bool LogManager::isConsoleLoggingEnabled() {
    return getGlobalLogger()->getLogger("NLM") != nullptr;
}

bool LogManager::isFileLoggingEnabled() {
    auto logger = getGlobalLogger()->getLogger("NLM");
    return logger && !logger->getLogFile().empty();
}

// ErrorHandler implementation
LogManager::ErrorHandler::ErrorHandler() : defaultAction_(ErrorAction::LogAndContinue), 
                                          errorCount_(0), warningCount_(0) {}

void LogManager::ErrorHandler::handleError(const std::string& message, const std::string& source, ErrorAction action) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Log the error
    reportError(message, source);
    
    // Take appropriate action
    switch (action) {
        case ErrorAction::Abort:
            std::abort();
            break;
        case ErrorAction::GracefulShutdown:
            // Would perform graceful shutdown here
            break;
        case ErrorAction::LogAndAbort:
            // Log and then abort
            reportError("Error caused graceful shutdown", "System");
            std::abort();
            break;
        default:
            // LogAndContinue or default action
            break;
    }
}

void LogManager::ErrorHandler::reportError(const std::string& message, const std::string& source) {
    std::lock_guard<std::mutex> lock(mutex_);
    errorCount_++;
    // Log using global logger
    NLM_LOG(Level::Error, message);
}

void LogManager::ErrorHandler::reportWarning(const std::string& message, const std::string& source) {
    std::lock_guard<std::mutex> lock(mutex_);
    warningCount_++;
    // Log using global logger
    NLM_LOG(Level::Warn, message);
}

void LogManager::ErrorHandler::addContext(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(mutex_);
    errorContext_[key] = value;
}

std::string LogManager::ErrorHandler::getErrorContext() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::stringstream ss;
    ss << "Error Context:" << std::endl;
    for (const auto& pair : errorContext_) {
        ss << "  " << pair.first << ": " << pair.second << std::endl;
    }
    return ss.str();
}

size_t LogManager::ErrorHandler::getErrorCount(ErrorAction action) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return errorCount_;
}

size_t LogManager::ErrorHandler::getWarningCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return warningCount_;
}

bool LogManager::ErrorHandler::recoverFromError() {
    std::lock_guard<std::mutex> lock(mutex_);
    // Implementation would attempt to recover from the error
    // For now, just reset error count
    errorCount_ = 0;
    return true;
}

void LogManager::ErrorHandler::resetErrorCount() {
    std::lock_guard<std::mutex> lock(mutex_);
    errorCount_ = 0;
    warningCount_ = 0;
}

// Global error handler functions
LogManager::ErrorHandler& LogManager::getErrorHandler() {
    std::lock_guard<std::mutex> lock(globalMutex_);
    static ErrorHandler handler;
    return handler;
}

void LogManager::initializeErrorHandler(ErrorHandler::ErrorAction defaultAction) {
    getErrorHandler().setDefaultErrorAction(defaultAction);
}

// PerformanceLogger implementation
LogManager::PerformanceLogger::PerformanceLogger(const std::string& name, bool enable) 
    : name_(name), enabled_(enable), lastDuration_(0.0), averageDuration_(0.0), 
      totalTime_(0.0), callCount_(0), lastMemoryUsage_(0.0), peakMemoryUsage_(0.0),
      warningThreshold_(10.0), errorThreshold_(30.0) {}

LogManager::PerformanceLogger::~PerformanceLogger() {
    if (enabled_) {
        // Log performance summary
        logPerformance("Performance Summary", 0);
    }
}

void LogManager::PerformanceLogger::startTimer() {
    if (!enabled_) return;
    std::lock_guard<std::mutex> lock(mutex_);
    startTime_ = std::chrono::high_resolution_clock::now();
}

void LogManager::PerformanceLogger::stopTimer() {
    if (!enabled_) return;
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto endTime = std::chrono::high_resolution_clock::now();
    lastDuration_ = std::chrono::duration<double>(endTime - startTime_).count();
    
    totalTime_ += lastDuration_;
    callCount_++;
    averageDuration_ = totalTime_ / callCount_;
    
    trackMemoryUsage();
    
    // Check performance thresholds
    if (checkPerformanceThreshold(lastDuration_)) {
        NLM_LOG_WARN_F("Performance threshold exceeded for operation '%s': %.3f seconds", 
                      name_.c_str(), lastDuration_);
    }
}

void LogManager::PerformanceLogger::logPerformance(const std::string& operation, size_t iterations) {
    if (!enabled_) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    NLM_LOG_INFO_F("Performance: %s - %.6f seconds per operation (%.2f ops/sec, iterations: %zu)", 
                  operation.c_str(), averageDuration_, 1.0 / averageDuration_, iterations);
    
    if (lastMemoryUsage_ > 0) {
        NLM_LOG_INFO_F("Memory Usage: %.2f MB (peak: %.2f MB)", 
                      lastMemoryUsage_ / (1024 * 1024), peakMemoryUsage_ / (1024 * 1024));
    }
}

void LogManager::PerformanceLogger::trackMemoryUsage() {
    // Simple memory tracking - in real implementation would use platform-specific APIs
    // For now, just simulate some memory usage based on call count
    lastMemoryUsage_ = static_cast<double>(callCount_) * 1024.0;  // 1 KB per call
    if (lastMemoryUsage_ > peakMemoryUsage_) {
        peakMemoryUsage_ = lastMemoryUsage_;
    }
}

void LogManager::PerformanceLogger::setWarningThreshold(double threshold) {
    warningThreshold_ = threshold;
}

void LogManager::PerformanceLogger::setErrorThreshold(double threshold) {
    errorThreshold_ = threshold;
}

bool LogManager::PerformanceLogger::checkPerformanceThreshold(double currentTime) {
    if (currentTime > errorThreshold_) {
        NLM_LOG_ERROR_F("Performance error threshold exceeded for '%s': %.3f seconds > %.3f seconds", 
                       name_.c_str(), currentTime, errorThreshold_);
        return true;
    } else if (currentTime > warningThreshold_) {
        NLM_LOG_WARN_F("Performance warning threshold exceeded for '%s': %.3f seconds > %.3f seconds", 
                      name_.c_str(), currentTime, warningThreshold_);
        return true;
    }
    return false;
}

// Global performance logger functions
LogManager::PerformanceLogger& LogManager::getPerformanceLogger(const std::string& name) {
    std::lock_guard<std::mutex> lock(globalMutex_);
    if (performanceLoggers_.find(name) == performanceLoggers_.end()) {
        performanceLoggers_[name] = PerformanceLogger(name);
    }
    return performanceLoggers_[name];
}

} // namespace nlm
