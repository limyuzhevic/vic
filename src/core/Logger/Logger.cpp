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
    
    // Add source location for better debugging
    if (!entry.file.empty()) {
        // Extract filename from full path
        size_t pos = entry.file.find_last_of("/\\");
        std::string filename = (pos != std::string::npos) ? entry.file.substr(pos + 1) : entry.file;
        oss << " " << filename;
        if (entry.line > 0) {
            oss << ":" << entry.line;
        }
        if (!entry.function.empty()) {
            oss << " in " << entry.function;
        }
    }
    
    oss << " " << entry.message;
    
    std::string output = oss.str();
    
    // Console output with better formatting
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
    
    // Also log to stderr for error and critical messages
    if (entry.level >= LogLevel::Error) {
        std::cerr << levelToColor(entry.level) << output << "\033[0m\n";
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

Logger& Logger::getGlobal() {
    static Logger globalLogger;
    return globalLogger;
}

// Enhanced error handling and logging functions
void Logger::logError(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Error, message, file, line, function);
}

void Logger::logError(const std::string& message) {
    // Use current file and line information
    log(LogLevel::Error, message, "", 0, "");
}

void Logger::logCritical(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Critical, message, file, line, function);
}

void Logger::logCritical(const std::string& message) {
    log(LogLevel::Critical, message, "", 0, "");
}

void Logger::logWarning(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Warning, message, file, line, function);
}

void Logger::logWarning(const std::string& message) {
    log(LogLevel::Warning, message, "", 0, "");
}

void Logger::logInfo(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Info, message, file, line, function);
}

void Logger::logInfo(const std::string& message) {
    log(LogLevel::Info, message, "", 0, "");
}

void Logger::logDebug(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Debug, message, file, line, function);
}

void Logger::logDebug(const std::string& message) {
    log(LogLevel::Debug, message, "", 0, "");
}

bool Logger::isLoggingEnabled(LogLevel level) const {
    return isEnabled(level);
}

void Logger::setLoggingLevel(LogLevel level) {
    setLevel(level);
}

LogLevel Logger::getLoggingLevel() const {
    // Get the minimum level from all loggers
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    LogLevel minLevel = LogLevel::Debug; // Start with most verbose
    for (const auto& logger : pImpl->loggers) {
        if (logger->getLevel() > minLevel) {
            minLevel = logger->getLevel();
        }
    }
    return minLevel;
}

void Logger::setGlobalLogger(std::shared_ptr<Logger> logger) {
    // This allows overriding the global logger if needed
    // In practice, getGlobal() is already static and thread-safe
    setGlobal(logger);
}

void Logger::enableDebugLogging() {
    setLevel(LogLevel::Debug);
}

void Logger::disableDebugLogging() {
    setLevel(LogLevel::Info);
}

void Logger::setOutputFile(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        if (auto consoleLogger = std::dynamic_pointer_cast<ConsoleLogger>(logger)) {
            consoleLogger->setOutputFile(filepath);
        }
    }
}

void Logger::closeOutputFile() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        if (auto consoleLogger = std::dynamic_pointer_cast<ConsoleLogger>(logger)) {
            consoleLogger->closeOutputFile();
        }
    }
}

void Logger::setColorsEnabled(bool enabled) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        if (auto consoleLogger = std::dynamic_pointer_cast<ConsoleLogger>(logger)) {
            consoleLogger->setUseColors(enabled);
        }
    }
}

void Logger::logSystemStartup(const std::string& component, const std::string& version) {
    logInfo("Starting " + component + " v" + version);
}

void Logger::logSystemShutdown(const std::string& component) {
    logInfo("Shutting down " + component);
}

void Logger::logConfiguration(const std::string& component, const std::string& configFile) {
    logInfo("Loading configuration from " + configFile + " for " + component);
}

void Logger::logExperimentStart(const std::string& experimentName, uint64_t steps) {
    logInfo("Starting experiment: " + experimentName + " (" + std::to_string(steps) + " steps)");
}

void Logger::logExperimentComplete(const std::string& experimentName, float finalScore) {
    logInfo("Completed experiment: " + experimentName + " (score: " + std::to_string(finalScore) + ")");
}

void Logger::logErrorWithContext(const std::string& context, const std::string& message) {
    logError("[".append(context).append("] ").append(message));
}

void Logger::logWarningWithContext(const std::string& context, const std::string& message) {
    logWarning("[".append(context).append("] ").append(message));
}

void Logger::logInfoWithContext(const std::string& context, const std::string& message) {
    logInfo("[".append(context).append("] ").append(message));
}

// Enhanced LogStream implementation with better error handling
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function), context_("") {
    
    // Add context information for better debugging
    if (!file.empty()) {
        size_t pos = file.find_last_of("/\\");
        file_ = (pos != std::string::npos) ? file.substr(pos + 1) : file;
    }
}

LogStream::LogStream(LogLevel level, const std::string& context)
    : level_(level), context_(context), file_(""), line_(0), function_("") {
}

LogStream::~LogStream() {
    std::string message = oss_.str();
    if (message.empty()) {
        return;
    }
    
    // Add context prefix if available
    if (!context_.empty()) {
        message = "[" + context_ + "] " + message;
    }
    
    Logger::getGlobal().log(level_, message, file_, line_, function_);
}

std::ostringstream& LogStream::getStream() {
    return oss_;
}

LogLevel LogStream::getLevel() const {
    return level_;
}

std::string LogStream::getContext() const {
    return context_;
}

void LogStream::setContext(const std::string& context) {
    context_ = context;
}

// Backward compatibility
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : LogStream(level, file, line, function) {
}

LogStream::~LogStream() = default;
