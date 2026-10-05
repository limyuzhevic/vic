#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <chrono>
#include <ctime>
#include <algorithm>

namespace nlm {

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    bool useStructuredFormat = false;
    std::string formatString = "[{timestamp}] [{level}] {message}";
    std::ofstream fileOut;
    std::mutex mutex;
    
    // Performance optimization: buffer for batch output
    std::vector<std::string> outputBuffer;
    size_t bufferSize = 100;
    std::chrono::steady_clock::time_point lastFlush;
    std::chrono::milliseconds flushInterval{100};
    
    Impl() {
        lastFlush = std::chrono::steady_clock::now();
        outputBuffer.reserve(bufferSize);
    }
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
}

ConsoleLogger::~ConsoleLogger() {
    flushBuffer();
}

void ConsoleLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::string output;
    
    if (pImpl->useStructuredFormat) {
        // Structured JSON-like format
        output = formatStructuredLog(entry);
    } else {
        // Human-readable format
        output = formatHumanLog(entry);
    }
    
    // Add to buffer for performance
    pImpl->outputBuffer.push_back(output);
    
    // Flush if buffer is full or interval elapsed
    auto now = std::chrono::steady_clock::now();
    if (pImpl->outputBuffer.size() >= pImpl->bufferSize || 
        (now - pImpl->lastFlush) >= pImpl->flushInterval) {
        flushBuffer();
    }
}

void ConsoleLogger::flushBuffer() {
    if (pImpl->outputBuffer.empty()) return;
    
    std::ostringstream oss;
    
    for (const auto& line : pImpl->outputBuffer) {
        oss << line << "\n";
    }
    
    std::string output = oss.str();
    
    // Console output
    if (pImpl->useColors && !pImpl->useStructuredFormat) {
        // Colorize based on level for human format
        std::cout << levelToColor(entry.level) << output << "\033[0m";
    } else {
        std::cout << output;
    }
    
    // File output if set
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut << output;
        pImpl->fileOut.flush();
    }
    
    pImpl->outputBuffer.clear();
    pImpl->lastFlush = std::chrono::steady_clock::now();
}

void ConsoleLogger::flush() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    flushBuffer();
}

std::string ConsoleLogger::formatStructuredLog(const LogEntry& entry) const {
    // JSON-like structured format
    std::ostringstream oss;
    oss << "{";
    oss << "\"timestamp\": \"" << formatTimestamp(entry.timestamp) << "\", ";
    oss << "\"level\": \"" << levelToString(entry.level) << "\", ";
    oss << "\"message\": \"" << escapeString(entry.message) << "\"";
    if (!entry.file.empty()) {
        oss << ", \"file\": \"" << escapeString(entry.file) << "\"";
        oss << ", \"line\": " << entry.line;
    }
    if (!entry.function.empty()) {
        oss << ", \"function\": \"" << escapeString(entry.function) << "\"";
    }
    oss << "}";
    return oss.str();
}

std::string ConsoleLogger::formatHumanLog(const LogEntry& entry) const {
    std::ostringstream oss;
    
    // Format timestamp
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        entry.timestamp.time_since_epoch()) % 1000;
    
    char timeBuffer[20];
    std::strftime(timeBuffer, sizeof(timeBuffer), "%H:%M:%S", std::localtime(&time));
    
    // Apply custom format string
    std::string formatted = pImpl->formatString;
    
    // Simple formatting - replace placeholders
    size_t pos = 0;
    while ((pos = formatted.find("{timestamp}", pos)) != std::string::npos) {
        formatted.replace(pos, 11, std::string(timeBuffer) + "." + 
            std::to_string(ms).substr(0, 3));
        pos += strlen(timeBuffer) + 4;
    }
    while ((pos = formatted.find("{level}", pos)) != std::string::npos) {
        formatted.replace(pos, 8, levelToString(entry.level));
        pos += levelToString(entry.level).length();
    }
    while ((pos = formatted.find("{message}", pos)) != std::string::npos) {
        formatted.replace(pos, 9, escapeString(entry.message));
        pos += escapeString(entry.message).length();
    }
    while ((pos = formatted.find("{file}", pos)) != std::string::npos) {
        if (!entry.file.empty()) {
            formatted.replace(pos, 7, escapeString(entry.file));
            pos += escapeString(entry.file).length();
        } else {
            formatted.erase(pos, 7);
        }
    }
    while ((pos = formatted.find("{line}", pos)) != std::string::npos) {
        formatted.replace(pos, 7, std::to_string(entry.line));
        pos += std::to_string(entry.line).length();
    }
    while ((pos = formatted.find("{function}", pos)) != std::string::npos) {
        if (!entry.function.empty()) {
            formatted.replace(pos, 10, escapeString(entry.function));
            pos += escapeString(entry.function).length();
        } else {
            formatted.erase(pos, 10);
        }
    }
    
    // Add to output
    oss << "[" << formatted << "]";
    
    return oss.str();
}

std::string ConsoleLogger::formatTimestamp(const std::chrono::system_clock::time_point& timestamp) const {
    auto time = std::chrono::system_clock::to_time_t(timestamp);
    char buffer[25];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%S", std::localtime(&time));
    return std::string(buffer);
}

std::string ConsoleLogger::escapeString(const std::string& str) const {
    std::ostringstream oss;
    for (char c : str) {
        switch (c) {
            case '\0': oss << "\\0"; break;
            case '\n': oss << "\\n"; break;
            case '\r': oss << "\\r"; break;
            case '\t': oss << "\\t"; break;
            case '\"': oss << "\\\""; break;
            case '\\': oss << "\\\\"; break;
            default: oss << c; break;
        }
    }
    return oss.str();
}

void ConsoleLogger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->level = level;
}

LogLevel ConsoleLogger::getLevel() const {
    return pImpl->level;
}

void ConsoleLogger::setUseColors(bool useColors) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->useColors = useColors;
}

bool ConsoleLogger::getUseColors() const {
    return pImpl->useColors;
}

void ConsoleLogger::setUseStructuredFormat(bool useStructured) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->useStructuredFormat = useStructured;
}

bool ConsoleLogger::getUseStructuredFormat() const {
    return pImpl->useStructuredFormat;
}

void ConsoleLogger::setFormatString(const std::string& format) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->formatString = format;
}

std::string ConsoleLogger::getFormatString() const {
    return pImpl->formatString;
}

void ConsoleLogger::setBufferSize(size_t size) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->bufferSize = size;
    pImpl->outputBuffer.reserve(size);
}

void ConsoleLogger::setFlushInterval(std::chrono::milliseconds interval) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->flushInterval = interval;
}

void ConsoleLogger::setOutputFile(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    // Flush any remaining output before opening new file
    flushBuffer();
    pImpl->fileOut.open(filepath, std::ios::app);
    if (!pImpl->fileOut.is_open()) {
        std::cerr << "Warning: Failed to open log file: " << filepath << std::endl;
    }
}

void ConsoleLogger::closeOutputFile() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
        flushBuffer();
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
    if (!pImpl->useColors || pImpl->useStructuredFormat) return "";
    
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

Logger::~Logger() {
    // Clean up all loggers
    for (auto& logger : pImpl->loggers) {
        if (auto consoleLogger = std::dynamic_pointer_cast<ConsoleLogger>(logger)) {
            consoleLogger->closeOutputFile();
        }
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