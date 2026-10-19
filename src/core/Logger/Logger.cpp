#include "Logger.hpp"
#include "EnhancedLogger.hpp"
#include "ErrorHandling.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <thread>
#include <filesystem>

namespace nlm {

// Forward declaration
namespace enhanced {
    class Logger;
}

// Original ConsoleLogger implementation
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
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    try {
        pImpl->fileOut.open(filepath, std::ios::app);
        if (!pImpl->fileOut.is_open()) {
            auto error = Errors::ioError(ErrorCode::FILE_ACCESS_DENIED, filepath, 
                                         "Failed to open log file for writing");
            error->getContext().function = "setOutputFile";
            error->getContext().file = __FILE__;
            error->getContext().line = __LINE__;
            error->getContext().module = "Logger";
            nlm::getGlobalErrorManager().recordError(error);
            std::cerr << "[ERROR] Failed to open log file: " << filepath << std::endl;
        }
    } catch (const std::exception& e) {
        auto error = Errors::systemError("setOutputFile", e.what());
        error->getContext().function = "setOutputFile";
        error->getContext().file = __FILE__;
        error->getContext().line = __LINE__;
        error->getContext().module = "Logger";
        nlm::getGlobalErrorManager().recordError(error);
        std::cerr << "[ERROR] Exception in setOutputFile: " << e.what() << std::endl;
    }
}

void ConsoleLogger::closeOutputFile() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    try {
        if (pImpl->fileOut.is_open()) {
            pImpl->fileOut.close();
        }
    } catch (const std::exception& e) {
        auto error = Errors::systemError("closeOutputFile", e.what());
        error->getContext().function = "closeOutputFile";
        error->getContext().file = __FILE__;
        error->getContext().line = __LINE__;
        error->getContext().module = "Logger";
        nlm::getGlobalErrorManager().recordError(error);
        std::cerr << "[ERROR] Exception in closeOutputFile: " << e.what() << std::endl;
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

// Logger implementation
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    
    // Backward compatibility with enhanced logger
    std::shared_ptr<enhanced::Logger> enhancedLogger;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {
    // Try to get the enhanced global logger for backward compatibility
    try {
        enhancedLogger = std::make_shared<enhanced::Logger>();
    } catch (...) {
        // If enhanced logger fails to initialize, continue with basic logger
    }
}

Logger::~Logger() {
    try {
        flush();
    } catch (const std::exception& e) {
        auto error = Errors::systemError("~Logger", e.what());
        error->getContext().function = "~Logger";
        error->getContext().file = __FILE__;
        error->getContext().line = __LINE__;
        error->getContext().module = "Logger";
        nlm::getGlobalErrorManager().recordError(error);
    }
}

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
    
    // Also update enhanced logger if available
    if (pImpl->enhancedLogger) {
        pImpl->enhancedLogger->setLevel(static_cast<enhanced::LogLevel>(level));
    }
}

void Logger::log(LogLevel level, const std::string& message,
                 const std::string& file, int line,
                 const std::string& function) {
    // Convert to enhanced LogEntry
    enhanced::LogEntry enhancedEntry;
    enhancedEntry.level = static_cast<enhanced::LogLevel>(level);
    enhancedEntry.message = message;
    enhancedEntry.file = file;
    enhancedEntry.line = line;
    enhancedEntry.function = function;
    enhancedEntry.timestamp = std::chrono::system_clock::now();
    
    // Migrate to enhanced format
    LogEntry migratedEntry = migrateEntry(message, level, file, line, function);
    
    // Send to basic loggers
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        logger->log(migratedEntry);
    }
    
    // Also send to enhanced logger if available
    if (pImpl->enhancedLogger) {
        pImpl->enhancedLogger->log(enhancedEntry);
    }
}

void Logger::debug(const std::string& message, const std::string& file,
                   int line, const std::string& function) {
    log(LogLevel::Debug, message, file, line, function);
}

void Logger::info(const std::string& message, const std::string& file,
                  int line, const std::string& function) {
    log(LogLevel::Info, message, file, line, function);
}

void Logger::warning(const std::string& message, const std::string& file,
                     int line, const std::string& function) {
    log(LogLevel::Warning, message, file, line, function);
}

void Logger::error(const std::string& message, const std::string& file,
                   int line, const std::string& function) {
    log(LogLevel::Error, message, file, line, function);
}

void Logger::critical(const std::string& message, const std::string& file,
                      int line, const std::string& function) {
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

void Logger::setGlobal(std::shared_ptr<Logger> logger) {
    // This is a placeholder for advanced global logger management
    // In practice, we just use getGlobal() which is already static
}

// Enhanced Logger factory methods
std::unique_ptr<Logger> Logger::createEnhancedLogger() {
    return std::make_unique<enhanced::Logger>();
}

// Migration helper
LogEntry Logger::migrateEntry(const std::string& message, LogLevel level,
                              const std::string& file, int line,
                              const std::string& function) {
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    
    // Add additional fields for enhanced compatibility
    entry.module = "unknown";
    entry.category = "system";
    entry.processId = getpid();
    entry.threadId = std::this_thread::get_id();
    
    return entry;
}

// LogStream implementation
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function) {}

LogStream::~LogStream() {
    Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
}

} // namespace nlm
