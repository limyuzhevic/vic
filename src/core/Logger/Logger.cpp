#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <chrono>
#include <unordered_map>
#include <vector>
#include <iomanip>
#include <ctime>

namespace nlm {

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    std::string format = "[%level] %timestamp %message";
    std::unordered_map<std::string, bool> categoryEnabled;
    std::string outputFilepath;
    size_t maxFileSize = 10 * 1024 * 1024;  // 10MB
    size_t currentFileSize = 0;
};

struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    bool validationEnabled = true;
    bool logErrors = true;
    bool performanceMonitoringEnabled = false;
    bool recoveryEnabled = false;
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> timingMap;
    std::vector<std::string> errorHistory;
    std::vector<std::string> performanceHistory;
    size_t maxHistorySize = 1000;
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {}

ConsoleLogger::ConsoleLogger(LogLevel level, LogLevel minLevel) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
    // Store minLevel for initialization
}

ConsoleLogger::~ConsoleLogger() = default;

void ConsoleLogger::log(const LogEntry& entry) {
    // Check category filtering first
    if (entry.level < pImpl->level) {
        return;
    }
    
    // Category-based filtering
    bool shouldLogEntry = true;
    if (!entry.metadata.empty()) {
        auto categoryIt = entry.metadata.find("category");
        if (categoryIt != entry.metadata.end()) {
            std::string category = categoryIt->second;
            if (!pImpl->categoryEnabled.empty() && !pImpl->categoryEnabled[category]) {
                shouldLogEntry = false;
            }
        }
    }
    if (!shouldLogEntry) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    
    // Format the log entry
    oss << formatEntry(entry);
    
    std::string output = oss.str();
    
    // Console output
    if (pImpl->useColors) {
        std::cout << levelToColor(entry.level) << output << "\033[0m\n";
    } else {
        std::cout << output << "\n";
    }
    
    // File output if set
    if (pImpl->fileOut.is_open()) {
        // Check file rotation
        if (pImpl->currentFileSize + output.size() + 1 > pImpl->maxFileSize) {
            // Rotate file
            pImpl->fileOut.close();
            // Generate rotated filename
            size_t dotPos = pImpl->outputFilepath.find_last_of('.');
            std::string rotatedName = pImpl->outputFilepath;
            if (dotPos != std::string::npos) {
                rotatedName.insert(dotPos, "_rotated_" + std::to_string(std::time(nullptr)));
            }
            pImpl->fileOut.open(rotatedName, std::ios::trunc);
        }
        
        pImpl->fileOut << output << "\n";
        pImpl->fileOut.flush();
        pImpl->currentFileSize += output.size() + 1;
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
    // Close current file if open
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
    
    pImpl->fileOut.open(filepath, std::ios::app);
    pImpl->outputFilepath = filepath;
    pImpl->currentFileSize = 0;
}

void ConsoleLogger::closeOutputFile() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
        pImpl->outputFilepath.clear();
        pImpl->currentFileSize = 0;
    }
}

void ConsoleLogger::setFormat(const std::string& format) {
    pImpl->format = format;
}

std::string ConsoleLogger::getFormat() const {
    return pImpl->format;
}

void ConsoleLogger::setLogCategory(const std::string& category, bool enable) {
    pImpl->categoryEnabled[category] = enable;
}

bool ConsoleLogger::shouldLogCategory(const std::string& category) const {
    auto it = pImpl->categoryEnabled.find(category);
    return (it == pImpl->categoryEnabled.end()) || it->second;
}

std::string ConsoleLogger::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::Trace: return "TRACE";
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Notice: return "NOTICE";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Critical: return "CRIT";
        case LogLevel::Alert: return "ALERT";
        case LogLevel::Emergency: return "EMERG";
        default: return "UNKNOWN";
    }
}

std::string ConsoleLogger::levelToColor(LogLevel level) const {
    switch (level) {
        case LogLevel::Trace: return "\033[36m";    // Cyan
        case LogLevel::Debug: return "\033[36m";     // Cyan
        case LogLevel::Info: return "\033[32m";       // Green
        case LogLevel::Notice: return "\033[34m";    // Blue
        case LogLevel::Warning: return "\033[33m";   // Yellow
        case LogLevel::Error: return "\033[31m";     // Red
        case LogLevel::Critical: return "\033[35m"; // Magenta
        case LogLevel::Alert: return "\033[91m";    // Bright Red
        case LogLevel::Emergency: return "\033[95m"; // Bright Magenta
        default: return "\033[0m";
    }
}

std::string ConsoleLogger::formatEntry(const LogEntry& entry) const {
    std::ostringstream oss;
    std::string format = pImpl->format;
    
    // Replace placeholders
    size_t pos = 0;
    while ((pos = format.find("%level")) != std::string::npos) {
        format.replace(pos, 7, levelToString(entry.level));
    }
    
    pos = 0;
    while ((pos = format.find("%timestamp")) != std::string::npos) {
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        char buf[100];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&time));
        format.replace(pos, 11, buf);
    }
    
    pos = 0;
    while ((pos = format.find("%message")) != std::string::npos) {
        format.replace(pos, 8, entry.message);
    }
    
    pos = 0;
    while ((pos = format.find("%file")) != std::string::npos) {
        format.replace(pos, 6, entry.file);
    }
    
    pos = 0;
    while ((pos = format.find("%line")) != std::string::npos) {
        format.replace(pos, 6, std::to_string(entry.line));
    }
    
    pos = 0;
    while ((pos = format.find("%function")) != std::string::npos) {
        format.replace(pos, 9, entry.function);
    }
    
    return format;
}

// Logger class implementation
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    bool validationEnabled = true;
    bool logErrors = true;
    bool performanceMonitoringEnabled = false;
    bool recoveryEnabled = false;
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> timingMap;
    std::vector<std::string> errorHistory;
    std::vector<std::string> performanceHistory;
    size_t maxHistorySize = 1000;
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
    
    // Check validation if enabled
    if (pImpl->validationEnabled) {
        validateEntry(entry);
    }
    
    // Log errors to history if enabled
    if (pImpl->logErrors && level >= LogLevel::Error) {
        std::ostringstream oss;
        oss << "[" << levelToString(entry.level) << "] " << entry.message;
        errorHistory.push_back(oss.str());
        if (errorHistory.size() > maxHistorySize) {
            errorHistory.erase(errorHistory.begin());
        }
    }
    
    for (auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
}

void Logger::logWithMetadata(LogLevel level, const std::string& message,
                            const std::string& file, int line,
                            const std::string& function,
                            const std::unordered_map<std::string, std::string>& metadata) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.metadata = metadata;
    
    for (auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
}

void Logger::trace(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Trace, message, file, line, function);
}

void Logger::debug(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Debug, message, file, line, function);
}

void Logger::info(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Info, message, file, line, function);
}

void Logger::notice(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Notice, message, file, line, function);
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

void Logger::alert(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Alert, message, file, line, function);
}

void Logger::emergency(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Emergency, message, file, line, function);
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

void Logger::setValidationEnabled(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->validationEnabled = enable;
}

bool Logger::isValidationEnabled() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->validationEnabled;
}

void Logger::setLogErrors(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->logErrors = enable;
}

bool Logger::getLogErrors() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->logErrors;
}

void Logger::setPerformanceMonitoring(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->performanceMonitoringEnabled = enable;
}

bool Logger::isPerformanceMonitoringEnabled() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->performanceMonitoringEnabled;
}

void Logger::setRecoveryEnabled(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->recoveryEnabled = enable;
}

bool Logger::isRecoveryEnabled() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->recoveryEnabled;
}

Logger& Logger::getGlobal() {
    static Logger globalLogger;
    return globalLogger;
}

void Logger::setGlobal(std::shared_ptr<Logger> logger) {
    // This is a placeholder for advanced global logger management
    // In practice, we just use getGlobal() which is already static
}

void Logger::validateConfiguration() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    // Basic validation
    if (pImpl->errorHistory.size() > 100) {
        // Handle too many errors
        std::cerr << "[VALIDATION] Too many errors in history (" << pImpl->errorHistory.size() << ")" << std::endl;
    }
    
    if (pImpl->performanceHistory.size() > 1000) {
        // Trim performance history
        pImpl->performanceHistory.erase(pImpl->performanceHistory.begin(),
                                       pImpl->performanceHistory.begin() + 500);
    }
}

void Logger::validateEntry(const LogEntry& entry) const {
    // Basic validation
    if (entry.message.empty()) {
        std::cerr << "[VALIDATION] Empty log message" << std::endl;
    }
    
    if (entry.level < LogLevel::Trace || entry.level > LogLevel::Emergency) {
        std::cerr << "[VALIDATION] Invalid log level: " << static_cast<int>(entry.level) << std::endl;
    }
}

std::string Logger::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::Trace: return "TRACE";
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Notice: return "NOTICE";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Critical: return "CRIT";
        case LogLevel::Alert: return "ALERT";
        case LogLevel::Emergency: return "EMERG";
        default: return "UNKNOWN";
    }
}

void Logger::startTiming(const std::string& operation) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->performanceMonitoringEnabled) {
        pImpl->timingMap[operation] = std::chrono::steady_clock::now();
    }
}

void Logger::stopTiming(const std::string& operation) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->performanceMonitoringEnabled) {
        auto it = pImpl->timingMap.find(operation);
        if (it != pImpl->timingMap.end()) {
            auto start = it->second;
            auto end = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            
            std::ostringstream oss;
            oss << "[PERFORMANCE] " << operation << ": " << duration.count() << " microseconds";
            
            performanceHistory.push_back(oss.str());
            if (performanceHistory.size() > maxHistorySize) {
                performanceHistory.erase(performanceHistory.begin());
            }
            
            pImpl->timingMap.erase(it);
        }
    }
}

void Logger::logPerformanceStats() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::cout << "[PERFORMANCE STATS]" << std::endl;
    for (const auto& entry : performanceHistory) {
        std::cout << "  " << entry << std::endl;
    }
}

// LogStream implementation
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function) {}

LogStream::~LogStream() {
    Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
}

// SafeLogger implementation
SafeLogger::SafeLogger(LogLevel level, const std::string& operation, const std::string& file, 
                      int line, const std::string& function)
    : level_(level), operation_(operation), successLogged_(false), failureLogged_(false) {
    std::lock_guard<std::mutex> lock(mutex);
}

SafeLogger::~SafeLogger() {
    // Ensure any pending log is completed
    if (!successLogged_ && !failureLogged_) {
        // If neither success nor failure was explicitly logged, consider it a failure
        logFailure("Operation completed without explicit success/failure log");
    }
}

template<typename T>
SafeLogger& SafeLogger::operator<<(const T& value) {
    std::lock_guard<std::mutex> lock(mutex);
    stream_ << value;
    return *this;
}

SafeLogger& SafeLogger::operator<<(const std::string& value) {
    std::lock_guard<std::mutex> lock(mutex);
    stream_ << value;
    return *this;
}

SafeLogger& SafeLogger::operator<<(char value) {
    std::lock_guard<std::mutex> lock(mutex);
    stream_ << value;
    return *this;
}

void SafeLogger::logSuccess() {
    std::lock_guard<std::mutex> lock(mutex);
    if (!successLogged_) {
        Logger::getGlobal().info("[SUCCESS] " + operation_ + ": " + stream_.str(), 
                               file_, line_, function_);
        successLogged_ = true;
    }
}

void SafeLogger::logFailure(const std::string& error) {
    std::lock_guard<std::mutex> lock(mutex);
    if (!failureLogged_) {
        Logger::getGlobal().error("[FAILURE] " + operation_ + ": " + stream_.str() + 
                                 " - Error: " + error, file_, line_, function_);
        failureLogged_ = true;
    }
}

} // namespace nlm
