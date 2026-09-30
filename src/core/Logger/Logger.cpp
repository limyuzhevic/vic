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
    // Validate log entry
    if (static_cast<int>(entry.level) < 0 || static_cast<int>(entry.level) > 4) {
        NLM_LOG_ERROR("Invalid log level in LogEntry: " + std::to_string(static_cast<int>(entry.level)));
        return;
    }
    
    if (entry.message.empty()) {
        NLM_LOG_WARNING("Empty log message for level: " + levelToString(entry.level));
    }
    
    if (entry.message.length() > 10000) {  // Reasonable message length limit
        NLM_LOG_WARNING("Very long log message (" + std::to_string(entry.message.length()) + " chars), truncating");
    }
    
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    
    // Add timestamp - thread-safe version using std::gmtime for consistency
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    std::tm tm_time;
    if (std::gmtime_s(&tm_time, &time) == 0) {
        oss << std::put_time(&tm_time, "%Y-%m-%d %H:%M:%S");
    } else {
        // Fallback to epoch if time conversion fails
        oss << "[TIMESTAMP_ERROR]";
    }
    
    // Sanitize message to prevent formatting issues
    std::string safeMessage = entry.message;
    
    // Remove potential newlines that could break log formatting
    std::replace(safeMessage.begin(), safeMessage.end(), '\n', ' ');
    
    // Remove other control characters that could cause issues
    for (char& c : safeMessage) {
        if (c < ' ' && c != '\t') {
            c = ' ';
        }
    }
    
    // Limit message length for display
    if (safeMessage.length() > 1000) {
        safeMessage = safeMessage.substr(0, 997) + "...";
    }
    
    oss << " " << safeMessage;
    
    std::string output = oss.str();
    
    // Console output with better error handling
    try {
        if (pImpl->useColors) {
            std::cout << levelToColor(entry.level) << output << "\033[0m\n";
        } else {
            std::cout << output << "\n";
        }
        std::cout.flush();  // Ensure output is written immediately
    } catch (const std::exception& e) {
        // If console output fails, try to continue with file output only
        NLM_LOG_ERROR("Console output failed: " + std::string(e.what()));
    }
    
    // File output if set with better error handling
    if (pImpl->fileOut.is_open()) {
        try {
            pImpl->fileOut << output << "\n";
            pImpl->fileOut.flush();
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("File output failed: " + std::string(e.what()));
            // Try to close and remove problematic file
            try {
                pImpl->fileOut.close();
                // Optionally remove corrupted file
            } catch (...) {}
        }
    }
}

void ConsoleLogger::setLevel(LogLevel level) {
    // Validate log level
    if (static_cast<int>(level) < 0 || static_cast<int>(level) > 4) {
        NLM_LOG_ERROR("Invalid log level in ConsoleLogger::setLevel: " + std::to_string(static_cast<int>(level)));
        return;
    }
    
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

void ConsoleLogger::setUseColors(bool useColors) {
    pImpl->useColors = useColors;
}

void ConsoleLogger::setOutputFile(const std::string& filepath) {
    // Validate filepath
    if (filepath.empty()) {
        NLM_LOG_ERROR("Cannot set output file with empty path");
        return;
    }
    
    std::error_code ec;
    
    // Check if path is valid
    if (!std::filesystem::path(filepath).is_absolute()) {
        NLM_LOG_WARNING("Setting relative output file path: " + filepath);
    }
    
    try {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        // Close existing file if open
        if (pImpl->fileOut.is_open()) {
            pImpl->fileOut.close();
        }
        
        // Create parent directories if they don't exist
        std::filesystem::path filePath(filepath);
        if (!filePath.parent_path().empty()) {
            if (!std::filesystem::exists(filePath.parent_path())) {
                if (!std::filesystem::create_directories(filePath.parent_path(), ec)) {
                    NLM_LOG_ERROR("Cannot create log directory " + filePath.parent_path().string() + ": " + ec.message());
                    return;
                }
            }
        }
        
        // Open file for appending
        pImpl->fileOut.open(filepath, std::ios::app);
        if (!pImpl->fileOut.is_open()) {
            NLM_LOG_ERROR("Failed to open log file for writing: " + filepath);
        } else {
            NLM_LOG_INFO("Log output redirected to file: " + filepath);
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception setting output file: " + std::string(e.what()));
    }
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
        case LogLevel::Warning: return "WARNING";  // Fixed: Use full name instead of "WARN"
        case LogLevel::Error: return "ERROR";
        case LogLevel::Critical: return "CRITICAL";  // Fixed: Use full name instead of "CRIT"
        default: return "UNKNOWN";
    }
}

std::string ConsoleLogger::levelToColor(LogLevel level) const {
    // Only enable colors if explicitly requested and supported
    if (!pImpl->useColors) {
        return "";
    }
    
    switch (level) {
        case LogLevel::Debug: return "\033[36m";    // Cyan
        case LogLevel::Info: return "\033[32m";      // Green
        case LogLevel::Warning: return "\033[33m";   // Yellow
        case LogLevel::Error: return "\033[31m";     // Red
        case LogLevel::Critical: return "\033[35m"; // Magenta
        default: return "";
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
    if (!logger) {
        NLM_LOG_ERROR("Cannot add null logger");
        return;
    }
    
    // Validate logger is properly initialized
    try {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        pImpl->loggers.push_back(logger);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception adding logger: " + std::string(e.what()));
    }
}

void Logger::clearLoggers() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        try {
            // Call closeOutputFile if the logger supports it
            if (auto consoleLogger = std::dynamic_pointer_cast<ConsoleLogger>(logger)) {
                consoleLogger->closeOutputFile();
            }
        } catch (...) {}
    }
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
    // Validate log level
    if (static_cast<int>(level) < 0 || static_cast<int>(level) > 4) {
        NLM_LOG_ERROR("Invalid log level accessed: " + std::to_string(static_cast<int>(level)));
        return false;
    }
    
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
