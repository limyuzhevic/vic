#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>

namespace nlm {

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

void ConsoleLogger::setRotation(const RotationConfig& config) {
    // This would be implemented in the Impl
}

void ConsoleLogger::enableAsynchronousOutput(bool enabled) {
    // This would be implemented in the Impl
}

// Logger implementation

struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    std::shared_ptr<LogFormatter> defaultFormatter;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {
    pImpl->defaultFormatter = std::make_shared<SimpleFormatter>();
}

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
                 const std::string& file, int line, const std::string& function,
                 const std::string& category) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.category = category.empty() ? LogCategory::Default : category;
    
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

void Logger::logCategory(LogLevel level, const std::string& category, const std::string& message,
                         const std::string& file, int line, const std::string& function) {
    log(level, message, file, line, function, category);
}

void Logger::debugCategory(const std::string& category, const std::string& message,
                           const std::string& file, int line, const std::string& function) {
    logCategory(LogLevel::Debug, category, message, file, line, function);
}

void Logger::infoCategory(const std::string& category, const std::string& message,
                          const std::string& file, int line, const std::string& function) {
    logCategory(LogLevel::Info, category, message, file, line, function);
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

bool Logger::isCategoryEnabled(const std::string& category) const {
    return GlobalLogManager::isCategoryEnabled(category);
}

Logger& Logger::getGlobal() {
    static Logger globalLogger;
    return globalLogger;
}

void Logger::setGlobal(std::shared_ptr<Logger> logger) {
    // This is a placeholder for advanced global logger management
    // In practice, we just use getGlobal() which is already static
}

void Logger::logMemoryStats() {
    // Log memory statistics
    MemoryLogger memLogger;
    memLogger.logMemoryStats();
}

void Logger::logPerformanceMetrics() {
    // Log performance metrics
    auto metrics = PerformanceMetrics::getMetrics();
    
    LogEntry entry;
    entry.level = LogLevel::Info;
    entry.message = PerformanceMetrics::getMetricsReport();
    entry.category = LogCategory::Performance;
    
    Logger::getGlobal().log(entry);
}

void Logger::generateBacktrace(const std::string& context) {
    Backtrace::generate(context);
}

void Logger::loadConfigFromFile(const std::string& configFile) {
    // Load configuration from file
}

void Logger::initializeFromConfig(const std::string& configKey) {
    // Initialize from configuration
    GlobalLogManager::initializeFromConfig();
}

std::string Logger::sanitizeLogMessage(const std::string& message) {
    // Basic sanitization - remove sensitive patterns
    std::string sanitized = message;
    
    // Remove potential passwords
    size_t pos = 0;
    while ((pos = sanitized.find("password", pos)) != std::string::npos) {
        size_t end = sanitized.find('\n', pos);
        if (end != std::string::npos) {
            sanitized.replace(pos, end - pos, "password=[REDACTED]");
            pos += 18; // length of "password=[REDACTED]"
        }
    }
    
    // Remove potential API keys
    pos = 0;
    while ((pos = sanitized.find("api_key", pos)) != std::string::npos) {
        size_t end = sanitized.find('\n', pos);
        if (end != std::string::npos) {
            sanitized.replace(pos, end - pos, "api_key=[REDACTED]");
            pos += 17; // length of "api_key=[REDACTED]"
        }
    }
    
    return sanitized;
}

// Global functions

void initializeLogging(const std::string& configFile) {
    if (!configFile.empty()) {
        Logger::loadConfigFromFile(configFile);
    }
    GlobalLogManager::initializeFromConfig();
}

void shutdownLogging() {
    // Cleanup logging resources
}

void setGlobalLogLevel(LogLevel level) {
    GlobalLogManager::setDefaultLevel(level);
    Logger::getGlobal().setLevel(level);
}

LogLevel getGlobalLogLevel() {
    return GlobalLogManager::getDefaultLevel();
}

void logEvent(const std::string& category, LogLevel level, const std::string& message) {
    Logger::getGlobal().logCategory(level, category, message);
}

// Backtrace implementation

namespace Backtrace {
    std::string backtraceData;
    bool enabled = false;
    
    void generate(const std::string& context) {
        if (!enabled) {
            return;
        }
        
        // Generate backtrace (simplified implementation)
        backtraceData = "Backtrace not implemented in this build\n";
        if (!context.empty()) {
            backtraceData += "Context: " + context + "\n";
        }
        
        // Log the backtrace
        Logger::getGlobal().logCategory(LogLevel::Error, LogCategory::Backtrace, 
                                        "Error with backtrace: " + backtraceData);
    }
    
    std::string getCurrentBacktrace() {
        return backtraceData;
    }
    
    void enableAutomaticGeneration(bool enabled) {
        Backtrace::enabled = enabled;
    }
    
    bool isEnabled() {
        return enabled;
    }
}
}