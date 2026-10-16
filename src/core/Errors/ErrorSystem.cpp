// Error system implementation
#include "ErrorSystem.hpp"

namespace nlm {

// Implementation details for ConsoleErrorReporter
struct ConsoleErrorReporter::Impl {
    // Private implementation details can go here
};

ConsoleErrorReporter::ConsoleErrorReporter() : ConsoleErrorReporter(ErrorSeverity::Warning) {}

ConsoleErrorReporter::ConsoleErrorReporter(LogLevel level) {
    // Map LogLevel to ErrorSeverity
    switch (level) {
        case LogLevel::Debug: errorThreshold_ = ErrorSeverity::Debug; break;
        case LogLevel::Info: errorThreshold_ = ErrorSeverity::Info; break;
        case LogLevel::Warning: errorThreshold_ = ErrorSeverity::Warning; break;
        case LogLevel::Error: errorThreshold_ = ErrorSeverity::Error; break;
        case LogLevel::Critical: errorThreshold_ = ErrorSeverity::Critical; break;
    }
}

ConsoleErrorReporter::~ConsoleErrorReporter() = default;

void ConsoleErrorReporter::log(const LogEntry& entry) {
    // Log entry to console with formatting
    std::string severityStr;
    switch (entry.level) {
        case LogLevel::Debug: severityStr = "[DEBUG]"; break;
        case LogLevel::Info: severityStr = "[INFO]"; break;
        case LogLevel::Warning: severityStr = "[WARNING]"; break;
        case LogLevel::Error: severityStr = "[ERROR]"; break;
        case LogLevel::Critical: severityStr = "[CRITICAL]"; break;
    }
    
    auto time_t = std::chrono::system_clock::to_time_t(entry.timestamp);
    std::cout << severityStr << " " << std::ctime(&time_t) << entry.message << std::endl;
}

void ConsoleErrorReporter::setLevel(LogLevel level) {
    setErrorThreshold(static_cast<ErrorSeverity>(level));
}

LogLevel ConsoleErrorReporter::getLevel() const {
    switch (errorThreshold_) {
        case ErrorSeverity::Debug: return LogLevel::Debug;
        case ErrorSeverity::Info: return LogLevel::Info;
        case ErrorSeverity::Warning: return LogLevel::Warning;
        case ErrorSeverity::Error: return LogLevel::Error;
        case ErrorSeverity::Critical: return LogLevel::Critical;
        default: return LogLevel::Error;
    }
}

void ConsoleErrorReporter::setUseColors(bool useColors) {
    // Implementation can be added if color output is desired
}

bool ConsoleErrorReporter::getUseColors() const {
    return false; // Colors not implemented
}

void ConsoleErrorReporter::setOutputFile(const std::string& filepath) {
    // Implementation can be added for file output
}

void ConsoleErrorReporter::closeOutputFile() {
    // Implementation can be added for file output
}

// Implementation details for Logger
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers_;
    ErrorHandler errorHandler_;
};

Logger::Logger() {
    pImpl = std::make_unique<Impl>();
}

Logger::~Logger() = default;

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    pImpl->loggers_.push_back(logger);
}

void Logger::clearLoggers() {
    pImpl->loggers_.clear();
}

void Logger::setLevel(LogLevel level) {
    for (auto& logger : pImpl->loggers_) {
        logger->setLevel(level);
    }
}

void Logger::log(LogLevel level, const std::string& message, 
                const std::string& file, int line, 
                const std::string& function) {
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    
    for (auto& logger : pImpl->loggers_) {
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
    return !pImpl->loggers_.empty();
}

Logger& Logger::getGlobal() {
    static Logger globalLogger;
    return globalLogger;
}

void Logger::setGlobal(std::shared_ptr<Logger> logger) {
    // This is a simplified implementation
    // In practice, you'd need thread-safe access
    // For now, we'll just document the API
}

// Implementation details for LogStream
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function) {}

LogStream::~LogStream() {
    // Log the accumulated stream content
    nlm::Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
}

// Implementation details for other error system classes
// (other classes have minimal or no implementation in this header)

} // namespace nlm
