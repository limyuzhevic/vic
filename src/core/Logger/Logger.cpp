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
#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <algorithm>
#include <filesystem>
#include <ctime>
#include <chrono>

namespace nlm {

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    
    // File rotation settings
    std::string filepath;
    size_t maxFileSize = 10 * 1024 * 1024;  // 10 MB
    int maxFiles = 5;
    size_t currentFileSize = 0;
    int fileNumber = 0;
    
    // Thread pool for async logging (future enhancement)
    std::vector<std::function<void()>> logQueue;
    std::mutex queueMutex;
    bool asyncMode = false;
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
    
    std::string formattedMessage;
    if (pImpl->useColors) {
        formattedMessage = "\033[" + std::to_string(static_cast<int>(entry.level) + 30) + "m";
    }
    
    formattedMessage += entry.getFormattedMessage();
    
    if (pImpl->useColors) {
        formattedMessage += "\033[0m";
    }
    
    formattedMessage += "\n";
    
    // Console output
    std::cout << formattedMessage;
    std::cout.flush();
    
    // File output with rotation
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut << formattedMessage;
        pImpl->fileOut.flush();
        pImpl->currentFileSize += formattedMessage.size();
        
        // Check if rotation is needed
        if (pImpl->shouldRotate()) {
            pImpl->rotateFile();
        }
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

void ConsoleLogger::setOutputFile(const std::string& filepath, size_t maxSizeMB, int maxFiles) {
    pImpl->fileOut.close();
    
    pImpl->filepath = filepath;
    pImpl->maxFileSize = maxSizeMB * 1024 * 1024;
    pImpl->maxFiles = maxFiles;
    pImpl->currentFileSize = 0;
    pImpl->fileNumber = 0;
    
    // Find the next available file number
    std::string basePath = filepath;
    size_t dotPos = basePath.find_last_of('.');
    std::string extension = (dotPos != std::string::npos) ? basePath.substr(dotPos) : "";
    std::string filename = (dotPos != std::string::npos) ? basePath.substr(0, dotPos) : basePath;
    
    for (int i = 0; i < maxFiles; ++i) {
        std::string numberedPath = filename + "_" + std::to_string(i) + extension;
        if (!std::filesystem::exists(numberedPath)) {
            pImpl->filepath = numberedPath;
            pImpl->fileNumber = i;
            break;
        }
    }
    
    pImpl->fileOut.open(pImpl->filepath, std::ios::app);
}

void ConsoleLogger::closeOutputFile() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

void ConsoleLogger::logError(const std::string& errorCode, const std::string& message,
                             const std::string& module, const std::string& operation,
                             const std::unordered_map<std::string, std::string>& metadata) {
    ErrorContext context(errorCode, module, operation);
    for (const auto& [key, value] : metadata) {
        context.addMeta(key, value);
    }
    
    LogEntry entry(LogLevel::Error, message);
    entry.errorContext = context;
    log(entry);
}

void ConsoleLogger::logPerformance(const std::string& operation, double duration, int64_t dataPoints) {
    ErrorContext context("PERF", "System", operation);
    context.addMeta("duration_ms", std::to_string(duration))
          .addMeta("data_points", std::to_string(dataPoints));
    
    std::ostringstream oss;
    oss << "Performance: " << operation << " took " << duration << "ms for " << dataPoints << " data points";
    
    LogEntry entry(LogLevel::Info, oss.str());
    entry.errorContext = context;
    log(entry);
}

std::string ConsoleLogger::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Critical: return "CRIT";
        case LogLevel::Trace: return "TRACE";
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
        case LogLevel::Trace: return "\033[90m";    // Bright Black
        default: return "\033[0m";
    }
}

void ConsoleLogger::writeToFile(const std::string& message) {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut << message;
        pImpl->fileOut.flush();
        pImpl->currentFileSize += message.size();
    }
}

bool ConsoleLogger::shouldRotate() const {
    return pImpl->currentFileSize >= pImpl->maxFileSize;
}

void ConsoleLogger::rotateFile() {
    pImpl->fileOut.close();
    
    // Close current file and create next one
    for (int i = pImpl->fileNumber; i < pImpl->maxFiles - 1; ++i) {
        std::string oldPath = pImpl->filepath;
        std::string newPath = oldPath.substr(0, oldPath.find_last_of('.')) + "_" + 
                            std::to_string(i) + ".log";
        
        std::string nextPath = oldPath.substr(0, oldPath.find_last_of('.')) + 
                              "_" + std::to_string(i + 1) + ".log";
        
        if (std::filesystem::exists(newPath)) {
            std::filesystem::rename(newPath, nextPath);
        }
    }
    
    // Create new empty file
    pImpl->fileOut.open(pImpl->filepath, std::ios::trunc);
    pImpl->currentFileSize = 0;
}

// Logger class implementation
struct Logger::Impl {
    std::vector<std::pair<std::shared_ptr<ILogger>, std::string>> loggers;  // name can be empty
    std::mutex mutex;
    
    // Default logger for system-wide errors
    std::shared_ptr<ConsoleLogger> defaultLogger;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {
    pImpl->defaultLogger = std::make_shared<ConsoleLogger>();
    addLogger(pImpl->defaultLogger, "global");
}

Logger::~Logger() = default;

void Logger::addLogger(std::shared_ptr<ILogger> logger, const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.emplace_back(logger, name.empty() ? "unnamed" : name);
}

bool Logger::removeLogger(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto it = pImpl->loggers.begin(); it != pImpl->loggers.end(); ++it) {
        if (it->second == name) {
            pImpl->loggers.erase(it);
            return true;
        }
    }
    return false;
}

std::shared_ptr<ILogger> Logger::getLogger(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (const auto& pair : pImpl->loggers) {
        if (pair.second == name) {
            return pair.first;
        }
    }
    return nullptr;
}

std::vector<std::string> Logger::getLoggerNames() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<std::string> names;
    for (const auto& pair : pImpl->loggers) {
        names.push_back(pair.second);
    }
    return names;
}

void Logger::clearLoggers() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.clear();
}

void Logger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& pair : pImpl->loggers) {
        pair.first->setLevel(level);
    }
}

void Logger::setLevel(const std::string& name, LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& pair : pImpl->loggers) {
        if (pair.second == name) {
            pair.first->setLevel(level);
            break;
        }
    }
}

void Logger::log(LogLevel level, const std::string& message, 
                 const std::string& file, int line, 
                 const std::string& function, const ErrorContext& context) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    LogEntry entry(level, message, file, line, function);
    entry.errorContext = context;
    
    for (auto& pair : pImpl->loggers) {
        pair.first->log(entry);
    }
}

void Logger::logError(const std::string& errorCode, const std::string& message,
                      const std::string& module, const std::string& operation,
                      const std::string& file, int line,
                      const std::unordered_map<std::string, std::string>& metadata) {
    ErrorContext context(errorCode, module, operation);
    for (const auto& [key, value] : metadata) {
        context.addMeta(key, value);
    }
    
    std::string fullMessage = message;
    if (!module.empty()) fullMessage += " [" + module + "]";
    if (!operation.empty()) fullMessage += " {" + operation + "}";
    
    log(LogLevel::Error, fullMessage, file, line, "", context);
}

void Logger::logPerformance(const std::string& operation, double duration, int64_t dataPoints,
                           const std::string& file, int line) {
    ErrorContext context("PERF", "System", operation);
    context.addMeta("duration_ms", std::to_string(duration))
          .addMeta("data_points", std::to_string(dataPoints));
    
    std::ostringstream oss;
    oss << "Performance: " << operation << " took " << duration << "ms for " << dataPoints << " data points";
    
    log(LogLevel::Info, oss.str(), file, line, "", context);
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

bool Logger::isEnabled(LogLevel level, const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (const auto& pair : pImpl->loggers) {
        if (pair.second == name || name.empty()) {
            if (pair.first->getLevel() <= level) {
                return true;
            }
        }
    }
    return false;
}

Logger& Logger::getGlobal() {
    static Logger globalLogger;
    return globalLogger;
}

void Logger::setGlobal(std::shared_ptr<Logger> logger) {
    static std::shared_ptr<Logger> globalLogger;
    globalLogger = logger;
}

void Logger::logSystemError(const std::string& errorCode, const std::string& message,
                           const std::string& module, const std::string& operation,
                           const std::string& file, int line) {
    getGlobal().logError(errorCode, message, module, operation, file, line);
}

void Logger::logPerformanceMetric(const std::string& operation, double duration,
                                 int64_t dataPoints, const std::string& file, int line) {
    getGlobal().logPerformance(operation, duration, dataPoints, file, line);
}

// LogStream implementation
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function,
                   const ErrorContext& context)
    : level_(level), file_(file), line_(line), function_(function), context_(context) {}

LogStream::~LogStream() {
    Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_, context_);
}

} // namespace nlm
