#include "ErrorHandling.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <cstring>

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
    // Validate entry parameters
    if (entry.level < LogLevel::Debug || entry.level > LogLevel::Critical) {
        ExceptionManager::throwError<NLMException>(
            "Invalid log level: " + std::to_string(static_cast<int>(entry.level)));
    }
    
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    try {
        std::ostringstream oss;
        oss << "[" << levelToString(entry.level) << "] ";
        
        // Add timestamp
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        
        if (entry.message.empty()) {
            ExceptionManager::throwError<NLMException>(
                "Log message is empty");
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
            try {
                pImpl->fileOut << output << "\n";
                pImpl->fileOut.flush();
            } catch (const std::exception& e) {
                ExceptionManager::throwError<FileError>(
                    "Failed to write log to file: " + std::string(e.what()),
                    FileError::Type::WriteError);
            }
        }
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to log entry: " + std::string(e.what()));
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
    // Validate input parameters
    if (!Validation::isFilePathValid(filepath)) {
        ExceptionManager::throwError<FileError>(
            "Invalid file path for log output: " + filepath, FileError::Type::InvalidFormat, filepath);
    }
    
    try {
        // Check if file exists and is a directory
        std::filesystem::path path(filepath);
        if (std::filesystem::exists(path) && std::filesystem::is_directory(path)) {
            ExceptionManager::throwError<FileError>(
                "Path is a directory, not a file: " + filepath, FileError::Type::InvalidFormat, filepath);
        }
        
        // Check if parent directory exists and is writable
        if (std::filesystem::exists(path.parent_path()) && 
            !std::filesystem::is_directory(path.parent_path())) {
            ExceptionManager::throwError<FileError>(
                "Parent path is not a directory: " + path.parent_path().string(),
                FileError::Type::InvalidFormat, filepath);
        }
        
        if (!std::filesystem::exists(path.parent_path()) || 
            !std::filesystem::is_writable(path.parent_path())) {
            ExceptionManager::throwError<FileError>(
                "Parent directory is not writable: " + path.parent_path().string(),
                FileError::Type::PermissionDenied, filepath);
        }
        
        // Close existing file if open
        if (pImpl->fileOut.is_open()) {
            pImpl->fileOut.close();
        }
        
        // Open file in append mode
        pImpl->fileOut.open(filepath, std::ios::app);
        
        if (!pImpl->fileOut.is_open()) {
            ExceptionManager::throwError<FileError>(
                "Failed to open log file: " + filepath, FileError::Type::WriteError, filepath);
        }
        
        // Write timestamp to file on open
        std::time_t now = std::time(nullptr);
        char timeBuf[100];
        std::strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
        pImpl->fileOut << "[Log file opened at " << timeBuf << "]\n";
        pImpl->fileOut.flush();
        
    } catch (const std::filesystem::filesystem_error& e) {
        ExceptionManager::throwError<FileError>(
            "File system error opening log file " + filepath + ": " + e.what(),
            FileError::Type::InvalidFormat, filepath);
    } catch (const std::exception& e) {
        ExceptionManager::throwError<FileError>(
            "Failed to open log file " + filepath + ": " + e.what(),
            FileError::Type::WriteError, filepath);
    }
}

void ConsoleLogger::closeOutputFile() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->fileOut.is_open()) {
        try {
            // Write closing timestamp
            std::time_t now = std::time(nullptr);
            char timeBuf[100];
            std::strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
            pImpl->fileOut << "[Log file closed at " << timeBuf << "]\n";
            pImpl->fileOut.flush();
            
            pImpl->fileOut.close();
        } catch (const std::exception& e) {
            ExceptionManager::throwError<FileError>(
                "Error closing log file: " + std::string(e.what()),
                FileError::Type::WriteError);
        }
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
    if (!logger) {
        ExceptionManager::throwError<NLMException>(
            "Cannot add null logger");
    }
    
    try {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        pImpl->loggers.push_back(logger);
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to add logger: " + std::string(e.what()));
    }
}

void Logger::clearLoggers() {
    try {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        pImpl->loggers.clear();
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to clear loggers: " + std::string(e.what()));
    }
}

void Logger::setLevel(LogLevel level) {
    // Validate level
    if (level < LogLevel::Debug || level > LogLevel::Critical) {
        ExceptionManager::throwError<NLMException>(
            "Invalid log level: " + std::to_string(static_cast<int>(level)));
    }
    
    try {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        for (auto& logger : pImpl->loggers) {
            logger->setLevel(level);
        }
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to set log level: " + std::string(e.what()));
    }
}

void Logger::log(LogLevel level, const std::string& message,
                const std::string& file, int line, const std::string& function) {
    // Validate input parameters
    if (level < LogLevel::Debug || level > LogLevel::Critical) {
        ExceptionManager::throwError<NLMException>(
            "Invalid log level: " + std::to_string(static_cast<int>(level)));
    }
    
    if (message.empty()) {
        ExceptionManager::throwError<NLMException>(
            "Log message is empty");
    }
    
    if (line < 0) {
        ExceptionManager::throwError<NLMException>(
            "Invalid line number: " + std::to_string(line));
    }
    
    try {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        LogEntry entry;
        entry.level = level;
        entry.message = message;
        entry.file = file;
        entry.line = line;
        entry.function = function;
        entry.timestamp = std::chrono::system_clock::now();
        
        for (auto& logger : pImpl->loggers) {
            try {
                logger->log(entry);
            } catch (const std::exception& e) {
                // Log but don't fail the entire operation
                std::cerr << "Error logging to logger: " << e.what() << std::endl;
            }
        }
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to create log entry: " + std::string(e.what()));
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
    if (level < LogLevel::Debug || level > LogLevel::Critical) {
        ExceptionManager::throwError<NLMException>(
            "Invalid log level: " + std::to_string(static_cast<int>(level)));
    }
    
    try {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        for (auto& logger : pImpl->loggers) {
            if (logger->getLevel() <= level) {
                return true;
            }
        }
        return false;
    } catch (const std::exception& e) {
        ExceptionManager::throwError<NLMException>(
            "Failed to check if log level is enabled: " + std::string(e.what()));
    }
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
