#include "NLMError.hpp"

namespace nlm {

// Console logger implementation
ConsoleLogger::ConsoleLogger() {
    // Implementation would use pImpl
}

ConsoleLogger::ConsoleLogger(LogLevel level) {
    // Implementation would use pImpl
}

ConsoleLogger::~ConsoleLogger() = default;

void ConsoleLogger::log(const LogEntry& entry) {
    // Implementation would log to console
}

void ConsoleLogger::setLevel(LogLevel level) {
    // Implementation would set log level
}

LogLevel ConsoleLogger::getLevel() const {
    // Implementation would return log level
    return LogLevel::Info;
}

void ConsoleLogger::setUseColors(bool useColors) {
    // Implementation would enable/disable colors
}

bool ConsoleLogger::getUseColors() const {
    // Implementation would return color setting
    return true;
}

void ConsoleLogger::setOutputFile(const std::string& filepath) {
    // Implementation would set output file
}

void ConsoleLogger::closeOutputFile() {
    // Implementation would close output file
}

// Main Logger implementation
Logger::Logger() {
    // Implementation would initialize default console logger
}

Logger::~Logger() = default;

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    // Implementation would add logger sink
}

void Logger::clearLoggers() {
    // Implementation would remove all loggers
}

void Logger::setLevel(LogLevel level) {
    // Implementation would set minimum level for all loggers
}

void Logger::log(LogLevel level, const std::string& message, 
                const std::string& file, int line, 
                const std::string& function) {
    // Implementation would create LogEntry and log it
}

void Logger::debug(const std::string& message, const std::string& file, 
                   int line, const std::string& function) {
    if (isEnabled(LogLevel::Debug)) {
        log(LogLevel::Debug, message, file, line, function);
    }
}

void Logger::info(const std::string& message, const std::string& file, 
                  int line, const std::string& function) {
    if (isEnabled(LogLevel::Info)) {
        log(LogLevel::Info, message, file, line, function);
    }
}

void Logger::warning(const std::string& message, const std::string& file, 
                     int line, const std::string& function) {
    if (isEnabled(LogLevel::Warning)) {
        log(LogLevel::Warning, message, file, line, function);
    }
}

void Logger::error(const std::string& message, const std::string& file, 
                   int line, const std::string& function) {
    if (isEnabled(LogLevel::Error)) {
        log(LogLevel::Error, message, file, line, function);
    }
}

void Logger::critical(const std::string& message, const std::string& file, 
                      int line, const std::string& function) {
    if (isEnabled(LogLevel::Critical)) {
        log(LogLevel::Critical, message, file, line, function);
    }
}

bool Logger::isEnabled(LogLevel level) const {
    // Implementation would check if level is enabled
    return static_cast<int>(level) >= static_cast<int>(LogLevel::Warning);
}

} // namespace nlm
