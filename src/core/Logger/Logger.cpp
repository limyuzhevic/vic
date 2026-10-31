#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>

namespace nlm {

// Enhanced Console logger implementation
struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    
    // Enhanced logging features
    std::unordered_map<std::string, std::string> errorContext;
    std::unordered_map<std::string, std::function<bool(const LogEntry&)>> filters;
    
    // Performance monitoring
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> activeProfiles;
    std::unordered_map<std::string, std::chrono::duration<double>> completedProfiles;
    
    // Statistics
    LoggerStats stats;
    uint64_t sequenceCounter{0};
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

void ConsoleLogger::logDebug(const std::string& message, const std::string& context) {
    LogEntry entry;
    entry.level = LogLevel::Debug;
    entry.message = message;
    if (!context.empty()) {
        entry.context["source"] = "application";
        entry.context["category"] = "debug";
    }
    entry.sequenceId = ++pImpl->sequenceCounter;
    log(entry);
}

void ConsoleLogger::logInfo(const std::string& message, const std::string& context) {
    LogEntry entry;
    entry.level = LogLevel::Info;
    entry.message = message;
    if (!context.empty()) {
        entry.context["source"] = "application";
        entry.context["category"] = "info";
    }
    entry.sequenceId = ++pImpl->sequenceCounter;
    log(entry);
}

void ConsoleLogger::logWarning(const std::string& message, const std::string& context) {
    LogEntry entry;
    entry.level = LogLevel::Warning;
    entry.message = message;
    if (!context.empty()) {
        entry.context["source"] = "application";
        entry.context["category"] = "warning";
    }
    entry.sequenceId = ++pImpl->sequenceCounter;
    log(entry);
}

void ConsoleLogger::logError(const std::string& message, const std::string& context) {
    LogEntry entry;
    entry.level = LogLevel::Error;
    entry.message = message;
    if (!context.empty()) {
        entry.context["source"] = "application";
        entry.context["category"] = "error";
    }
    entry.sequenceId = ++pImpl->sequenceCounter;
    log(entry);
}

void ConsoleLogger::logCritical(const std::string& message, const std::string& context) {
    LogEntry entry;
    entry.level = LogLevel::Critical;
    entry.message = message;
    if (!context.empty()) {
        entry.context["source"] = "application";
        entry.context["category"] = "critical";
    }
    entry.sequenceId = ++pImpl->sequenceCounter;
    log(entry);
}

// Logger class implementation
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    
    // Global configuration
    LoggerConfig config;
    
    // Global statistics and monitoring
    LoggerStats globalStats;
    std::unordered_map<std::string, std::chrono::duration<double>> globalProfiles;
    std::unordered_map<std::string, std::string> globalErrorContext;
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
                 const std::string& file, int line, const std::string& function,
                 const std::string& module,
                 const std::unordered_map<std::string, std::string>& context) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.module = module;
    entry.context = context;
    entry.sequenceId = ++pImpl->globalStats.totalMessages;
    entry.threadId = getFormattedThreadId();
    
    // Update statistics
    pImpl->globalStats.totalMessages++;
    if (entry.level <= LogLevel::Debug && entry.level >= LogLevel::Critical) {
        int index = static_cast<int>(entry.level) - static_cast<int>(LogLevel::Debug);
        if (index >= 0 && index < 5) {
            pImpl->globalStats.messagesByLevel[index]++;
        }
    }
    
    auto startTime = std::chrono::steady_clock::now();
    
    for (auto& logger : pImpl->loggers) {
        if (logger->shouldLog(entry)) {
            logger->log(entry);
        }
    }
    
    auto endTime = std::chrono::steady_clock::now();
    pImpl->globalStats.totalProcessingTime += (endTime - startTime);
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
    for (auto& logger : pImpl->loggers) {
        if (logger->getLevel() <= level) {
            return true;
        }
    }
    return false;
}

void Logger::configure(const LoggerConfig& config) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->config = config;
    
    // Apply configuration to all loggers
    for (auto& logger : pImpl->loggers) {
        logger->configure(config);
    }
}

LoggerConfig Logger::getGlobalConfig() {
    return getGlobal().pImpl->config;
}

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.push_back(logger);
    
    // Apply current configuration to new logger
    if (logger) {
        logger->configure(pImpl->config);
    }
}

void Logger::removeLogger(std::shared_ptr<ILogger> logger) {
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

LoggerConfig Logger::getGlobalConfig();

void Logger::debug(const std::string& message, const std::string& file, int line, const std::string& function, const std::string& module) {
    log(LogLevel::Debug, message, file, line, function, module);
}

void Logger::info(const std::string& message, const std::string& file, int line, const std::string& function, const std::string& module) {
    log(LogLevel::Info, message, file, line, function, module);
}

void Logger::warning(const std::string& message, const std::string& file, int line, const std::string& function, const std::string& module) {
    log(LogLevel::Warning, message, file, line, function, module);
}

void Logger::error(const std::string& message, const std::string& file, int line, const std::string& function, const std::string& module) {
    log(LogLevel::Error, message, file, line, function, module);
}

void Logger::critical(const std::string& message, const std::string& file, int line, const std::string& function, const std::string& module) {
    log(LogLevel::Critical, message, file, line, function, module);
}

// Global logging functions
void Logger::log(LogLevel level, const std::string& message,
                 const std::string& file, int line, const std::string& function,
                 const std::string& module,
                 const std::unordered_map<std::string, std::string>& context) {
    getGlobal().log(level, message, file, line, function, module, context);
}

void Logger::debug(const std::string& message, const std::string& file, int line,
                    const std::string& function, const std::string& module) {
    getGlobal().debug(message, file, line, function, module);
}

void Logger::info(const std::string& message, const std::string& file, int line,
                   const std::string& function, const std::string& module) {
    getGlobal().info(message, file, line, function, module);
}

void Logger::warning(const std::string& message, const std::string& file, int line,
                      const std::string& function, const std::string& module) {
    getGlobal().warning(message, file, line, function, module);
}

void Logger::error(const std::string& message, const std::string& file, int line,
                    const std::string& function, const std::string& module) {
    getGlobal().error(message, file, line, function, module);
}

void Logger::critical(const std::string& message, const std::string& file, int line,
                        const std::string& function, const std::string& module) {
    getGlobal().critical(message, file, line, function, module);
}

LoggerStats Logger::getGlobalStats() {
    return getGlobal().pImpl->globalStats;
}

void Logger::resetGlobalStats() {
    getGlobal().pImpl->globalStats.reset();
}

std::unordered_map<std::string, std::chrono::duration<double>> Logger::getGlobalProfiles() const {
    return getGlobal().pImpl->globalProfiles;
}

void Logger::setGlobalErrorContext(const std::string& key, const std::string& value) {
    getGlobal().pImpl->globalErrorContext[key] = value;
}

std::string Logger::getGlobalErrorContext(const std::string& key) {
    auto it = getGlobal().pImpl->globalErrorContext.find(key);
    return it != getGlobal().pImpl->globalErrorContext.end() ? it->second : "";
}

void Logger::startGlobalProfile(const std::string& name) {
    getGlobal().pImpl->globalProfiles[name] = std::chrono::steady_clock::now();
}

void Logger::endGlobalProfile(const std::string& name) {
    auto it = getGlobal().pImpl->globalProfiles.find(name);
    if (it != getGlobal().pImpl->globalProfiles.end()) {
        auto duration = std::chrono::steady_clock::now() - it->second;
        it->second = duration;
    }
}

bool Logger::isSystemHealthy() {
    LoggerStats stats = getGlobalStats();
    return checkLoggerHealth(stats);
}

std::string Logger::getSystemHealthReport() {
    LoggerStats stats = getGlobalStats();
    LoggerConfig config = getGlobal().pImpl->config;
    return generateHealthReport(stats, config);
}
// Implementation of missing Logger methods
Logger::Logger(const LoggerConfig& config) : pImpl(std::make_unique<Impl>()) {
    pImpl->config = config;
}

Logger::~Logger() = default;

Logger& Logger::getGlobal() {
    static Logger globalLogger;
    return globalLogger;
}

void Logger::setGlobal(std::shared_ptr<Logger> logger) {
    // This is a placeholder for advanced global logger management
    // In practice, we just use getGlobal() which is already static
}

Logger::Logger() : pImpl(std::make_unique<Impl>()) {}

Logger::~Logger() = default;

// Initialize global logger
Logger& Logger::getGlobalInstance() {
    static Logger globalLogger;
    return globalLogger;
}

void Logger::configure(const LoggerConfig& config) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->config = config;
    
    // Apply configuration to all loggers
    for (auto& logger : pImpl->loggers) {
        logger->configure(config);
    }
}

LoggerConfig Logger::getGlobalConfig() {
    return getGlobalInstance().pImpl->config;
}

void Logger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        logger->setLevel(level);
    }
}

void Logger::log(LogLevel level, const std::string& message,
                 const std::string& file, int line, const std::string& function) {
    log(level, message, file, line, function, "", {});
}
// Global functions for backward compatibility
void Logger::log(LogLevel level, const std::string& message,
                 const std::string& file, int line, const std::string& function,
                 const std::string& module,
                 const std::unordered_map<std::string, std::string>& context) {
    getGlobalInstance().log(level, message, file, line, function, module, context);
}
void Logger::debug(const std::string& message, const std::string& file, int line,
                    const std::string& function, const std::string& module) {
    getGlobalInstance().debug(message, file, line, function, module);
}

void Logger::info(const std::string& message, const std::string& file, int line,
                   const std::string& function, const std::string& module) {
    getGlobalInstance().info(message, file, line, function, module);
}

void Logger::warning(const std::string& message, const std::string& file, int line,
                      const std::string& function, const std::string& module) {
    getGlobalInstance().warning(message, file, line, function, module);
}

void Logger::error(const std::string& message, const std::string& file, int line,
                    const std::string& function, const std::string& module) {
    getGlobalInstance().error(message, file, line, function, module);
}

void Logger::critical(const std::string& message, const std::string& file, int line,
                        const std::string& function, const std::string& module) {
    getGlobalInstance().critical(message, file, line, function, module);
}

LoggerStats Logger::getGlobalStats() {
    return getGlobalInstance().pImpl->globalStats;
}

void Logger::resetGlobalStats() {
    getGlobalInstance().pImpl->globalStats.reset();
}

std::unordered_map<std::string, std::chrono::duration<double>> Logger::getGlobalProfiles() const {
    return getGlobalInstance().pImpl->globalProfiles;
}

void Logger::setGlobalErrorContext(const std::string& key, const std::string& value) {
    getGlobalInstance().pImpl->globalErrorContext[key] = value;
}

std::string Logger::getGlobalErrorContext(const std::string& key) {
    auto it = getGlobalInstance().pImpl->globalErrorContext.find(key);
    return it != getGlobalInstance().pImpl->globalErrorContext.end() ? it->second : "";
}

void Logger::startGlobalProfile(const std::string& name) {
    getGlobalInstance().pImpl->globalProfiles[name] = std::chrono::steady_clock::now();
}

void Logger::endGlobalProfile(const std::string& name) {
    auto it = getGlobalInstance().pImpl->globalProfiles.find(name);
    if (it != getGlobalInstance().pImpl->globalProfiles.end()) {
        auto duration = std::chrono::steady_clock::now() - it->second;
        it->second = duration;
    }
}

bool Logger::isSystemHealthy() {
    return checkLoggerHealth(getGlobalStats());
}

std::string Logger::getSystemHealthReport() {
    return generateHealthReport(getGlobalStats(), getGlobalConfig());
}

// Forward declarations for helper functions
namespace nlm {
    std::string getFormattedThreadId();
    LogLevel stringToLogLevel(const std::string& levelStr);
    std::string logLevelToString(LogLevel level);
    bool isLevelEnabled(LogLevel level, LogLevel minLevel);
    
    // Additional helper functions
    LogCategory categorizeLog(const LogEntry& entry);
    bool checkLoggerHealth(const LoggerStats& stats);
    std::string generateHealthReport(const LoggerStats& stats, const LoggerConfig& config);
} // namespace nlm

// Update global logger instance
Logger& getGlobalLogger() {
    static Logger globalLogger;
    return globalLogger;
}

// Initialize global logger
Logger& globalLogger = getGlobalLogger();

// Global configuration manager
struct GlobalLoggerManager {
    LoggerConfig config;
    std::mutex mutex;
    
    GlobalLoggerManager() {
        config = LoggerConfig();
    }
    
    void reconfigure(const LoggerConfig& newConfig) {
        std::lock_guard<std::mutex> lock(mutex);
        config = newConfig;
    }
    
    LoggerConfig getConfig() const {
        std::lock_guard<std::mutex> lock(mutex);
        return config;
    }
};

// Global logger manager instance
GlobalLoggerManager globalLoggerManager;

// Global configuration access
void setLoggerConfig(const LoggerConfig& config) {
    globalLoggerManager.reconfigure(config);
}

LoggerConfig getLoggerConfig() {
    return globalLoggerManager.getConfig();
}

// Global statistics access
LoggerStats getGlobalLoggerStats() {
    return globalLogger.getGlobalStats();
}

void resetGlobalLoggerStats() {
    globalLogger.resetGlobalStats();
}

// Global profiling access
std::unordered_map<std::string, std::chrono::duration<double>> getGlobalProfiles() {
    return globalLogger.getGlobalProfiles();
}

void startGlobalProfile(const std::string& name) {
    globalLogger.startGlobalProfile(name);
}

void endGlobalProfile(const std::string& name) {
    globalLogger.endGlobalProfile(name);
}

// Global error context access
void setGlobalErrorContext(const std::string& key, const std::string& value) {
    globalLogger.setGlobalErrorContext(key, value);
}

std::string getGlobalErrorContext(const std::string& key) {
    return globalLogger.getGlobalErrorContext(key);
}

// System health checks
bool isSystemHealthy() {
    return globalLogger.isSystemHealthy();
}

std::string getSystemHealthReport() {
    return globalLogger.getSystemHealthReport();
}

// Convenience macros for backward compatibility
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobalInstance().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobalInstance().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobalInstance().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobalInstance().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobalInstance().critical(msg, __FILE__, __LINE__, __func__)

// Stream-style logging helper
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function) {}

LogStream::~LogStream() {
    nlm::Logger::getGlobalInstance().log(level_, oss_.str(), file_, line_, function_);
}

// ProfilerScope implementation
nlm::ProfilerScope::ProfilerScope(const std::string& name)
    : name_(name), startTime_(std::chrono::steady_clock::now()) {}

nlm::ProfilerScope::~ProfilerScope() {
    auto endTime = std::chrono::steady_clock::now();
    auto duration = endTime - startTime_;
    
    // Add to global profiler
    getGlobalLogger().pImpl->globalProfiles[name_] += duration;
}
