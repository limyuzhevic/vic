#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <algorithm>
#include <cstdarg>
#include <execinfo.h>
#include <cxxabi.h>

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

// LogStream implementation methods
LogStream& LogStream::addMetadata(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(mutex_);
    metadata_[key] = value;
    return *this;
}

LogStream& LogStream::addException(std::shared_ptr<nlm::NLMException> exception) {
    std::lock_guard<std::mutex> lock(mutex_);
    exception_ = exception;
    return *this;
}

LogStream& LogStream::addStackTrace(const std::string& stackTrace) {
    std::lock_guard<std::mutex> lock(mutex_);
    stackTrace_ = stackTrace;
    return *this;
}

LogStream& LogStream::enableStructuredLogging() {
    std::lock_guard<std::mutex> lock(mutex_);
    isStructured_ = true;
    return *this;
}

std::string LogStream::formatWithParams(const std::string& format) const {
    std::string result = format;
    size_t pos = 0;
    
    for (const auto& pair : metadata_) {
        while ((pos = result.find("{'" + pair.first + "'}", pos)) != std::string::npos) {
            result.replace(pos, pair.first.length() + 4, pair.second);
            pos += pair.second.length();
        }
        pos = 0;
    }
    
    return result;
}

// ConsoleLogger log method enhancement to handle structured logging
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
    
    // Handle different log entry types
    if (entry.isStructured && !entry.metadata.empty()) {
        // Structured logging format
        oss << " [";
        bool first = true;
        for (const auto& pair : entry.metadata) {
            if (!first) oss << ", ";
            oss << pair.first << "=" << pair.second;
            first = false;
        }
        oss << "]";
    }
    
    if (entry.exception) {
        oss << " [Exception: " << entry.exception->getMessage() << "]";
    }
    
    if (!entry.stackTrace.empty()) {
        oss << " [Stack trace: " << entry.stackTrace.substr(0, 50) << "...]";
    }
    
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

// Implementation of exception-specific logging methods
void Logger::logException(const std::shared_ptr<nlm::NLMException>& exception, 
                         const std::string& context) {
    if (!exception) return;
    
    LogEntry entry;
    entry.level = LogLevel::Error;
    entry.message = exception->formatWithCause();
    entry.file = exception->getFile();
    entry.line = exception->getLine();
    entry.function = exception->getFunction();
    entry.timestamp = std::chrono::system_clock::now();
    entry.exception = exception;
    entry.isStructured = true;
    entry.stackTrace = exception->getMetadata("stackTrace");
    
    if (!context.empty()) {
        entry.metadata["context"] = context;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
}

void Logger::throwOnError(const std::shared_ptr<nlm::NLMException>& exception, 
                         const std::string& context) {
    if (!exception) return;
    
    logException(exception, context);
    throw *exception;
}

void Logger::logExceptionChain(const std::shared_ptr<nlm::NLMException>& exception,
                              bool includeStackTrace) {
    if (!exception) return;
    
    LogEntry entry;
    entry.level = LogLevel::Error;
    entry.message = exception->formatWithCause();
    entry.file = exception->getFile();
    entry.line = exception->getLine();
    entry.function = exception->getFunction();
    entry.timestamp = std::chrono::system_clock::now();
    entry.exception = exception;
    entry.isStructured = true;
    
    if (includeStackTrace) {
        entry.stackTrace = captureStackTrace();
        entry.exception->addMetadata("stackTrace", entry.stackTrace);
    }
    
    // Add exception metadata to entry
    const auto& metadata = exception->getMetadata();
    entry.metadata.insert(metadata.begin(), metadata.end());
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
}

void Logger::formatLog(const std::string& format, const std::vector<std::string>& params,
                      LogLevel level, const std::string& file, int line, 
                      const std::string& function) {
    std::string message = format;
    size_t pos = 0;
    int paramIndex = 0;
    
    while ((pos = message.find("{}", pos)) != std::string::npos && paramIndex < params.size()) {
        message.replace(pos, 2, params[paramIndex]);
        pos += params[paramIndex].length();
        paramIndex++;
    }
    
    log(level, message, file, line, function);
}

void Logger::logWithContext(const std::string& message, LogLevel level,
                           const std::unordered_map<std::string, std::string>& context,
                           const std::string& file, int line,
                           const std::string& function) {
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.metadata = context;
    entry.isStructured = !context.empty();
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
}

void Logger::logBatch(const std::vector<LogEntry>& entries) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        for (const auto& entry : entries) {
            logger->log(entry);
        }
    }
}

LogStream Logger::createLogStream(LogLevel level, 
                                 const std::string& file,
                                 int line,
                                 const std::string& function) {
    return LogStream(level, file, line, function);
}

std::string Logger::captureStackTrace() {
    const int max_depth = 100;
    void* buffer[max_depth];
    
    int depth = backtrace(buffer, max_depth);
    char** symbols = backtrace_symbols(buffer, depth);
    
    std::stringstream ss;
    ss << "Stack trace (" << depth << " frames):\n";
    
    for (int i = 0; i < depth; ++i) {
        ss << i << ": " << symbols[i] << "\n";
    }
    
    free(symbols);
    
    // Demangle C++ symbols
    std::string trace = ss.str();
    // In a real implementation, we would use cxxabi to demangle symbols
    return trace;
}

std::source_location Logger::getCurrentSourceLocation() {
#if __cplusplus >= 202002L
    return std::source_location::current();
#else
    // Fallback for C++17
    return std::source_location();
#endif
}

// LogStream implementation methods
LogStream& LogStream::addMetadata(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(mutex_);
    metadata_[key] = value;
    return *this;
}

LogStream& LogStream::addException(std::shared_ptr<nlm::NLMException> exception) {
    std::lock_guard<std::mutex> lock(mutex_);
    exception_ = exception;
    return *this;
}

LogStream& LogStream::addStackTrace(const std::string& stackTrace) {
    std::lock_guard<std::mutex> lock(mutex_);
    stackTrace_ = stackTrace;
    return *this;
}

LogStream& LogStream::enableStructuredLogging() {
    std::lock_guard<std::mutex> lock(mutex_);
    isStructured_ = true;
    return *this;
}

std::string LogStream::formatWithParams(const std::string& format) const {
    std::string result = format;
    size_t pos = 0;
    
    for (const auto& pair : metadata_) {
        while ((pos = result.find("{'" + pair.first + "'}", pos)) != std::string::npos) {
            result.replace(pos, pair.first.length() + 4, pair.second);
            pos += pair.second.length();
        }
        pos = 0;
    }
    
    return result;
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

// LogStream implementation
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function) {}

LogStream::~LogStream() {
    Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
}

} // namespace nlm
