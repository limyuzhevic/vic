#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <system_error>
#include <chrono>
#include <stdexcept>

namespace nlm {

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    bool fileErrorLogged = false;
    std::string lastFilePath;
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
}

ConsoleLogger::~ConsoleLogger() {
    closeOutputFile();
}

void ConsoleLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    
    // Add timestamp with high precision
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        entry.timestamp.time_since_epoch()) % 1000;
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    oss << "." << std::setfill('0') << std::setw(3) << ms.count();
    
    oss << " " << entry.message;
    
    // Add source location if provided
    if (!entry.file.empty() && entry.line > 0) {
        oss << " (" << entry.file << ":" << entry.line << ")";
    }
    
    if (!entry.function.empty()) {
        oss << " in " << entry.function;
    }
    
    std::string output = oss.str();
    
    // Console output with error handling
    try {
        // Console output
        if (pImpl->useColors) {
            std::cout << levelToColor(entry.level) << output << "\033[0m\n";
            std::cout.flush();
        } else {
            std::cout << output << "\n";
            std::cout.flush();
        }
    } catch (const std::exception& e) {
        // If console output fails, try to output to cerr as fallback
        std::cerr << "Logger error: " << e.what() << "\n";
        std::cerr << "Failed log entry: " << output << "\n";
    }
    
    // File output if set with improved error handling
    if (pImpl->fileOut.is_open()) {
        try {
            pImpl->fileOut << output << "\n";
            pImpl->fileOut.flush();
            pImpl->fileErrorLogged = false; // Reset error flag on success
        } catch (const std::exception& e) {
            if (!pImpl->fileErrorLogged) {
                std::cerr << "Logger file error: " << e.what() << "\n";
                pImpl->fileErrorLogged = true;
            }
        }
    }
}

void ConsoleLogger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->level = level;
}

LogLevel ConsoleLogger::getLevel() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->level;
}

void ConsoleLogger::setUseColors(bool useColors) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->useColors = useColors;
}

bool ConsoleLogger::getUseColors() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->useColors;
}

void ConsoleLogger::setOutputFile(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Close existing file if open
    closeOutputFile();
    
    try {
        // Open file with better error handling
        pImpl->fileOut.open(filepath, std::ios::app);
        if (!pImpl->fileOut.is_open()) {
            std::cerr << "Logger: Failed to open log file: " << filepath << "\n";
            return;
        }
        
        pImpl->lastFilePath = filepath;
        pImpl->fileErrorLogged = false;
        
        // Write header if file is empty
        pImpl->fileOut.seekp(0, std::ios::end);
        if (pImpl->fileOut.tellp() == 0) {
            pImpl->fileOut << "=== NLM Logger Session ===\n";
            pImpl->fileOut.flush();
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Logger: Exception opening log file " << filepath << ": " << e.what() << "\n";
    }
}

void ConsoleLogger::closeOutputFile() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
        try {
            pImpl->fileOut << "=== Log file closed ===\n";
            pImpl->fileOut.flush();
            pImpl->fileOut.close();
        } catch (const std::exception& e) {
            std::cerr << "Logger: Error closing log file: " << e.what() << "\n";
        }
    }
    pImpl->fileOut.clear(); // Clear error flags
    pImpl->fileErrorLogged = false;
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
    bool shutdownInitiated = false;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {}

Logger::~Logger() {
    clearLoggers();
}

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    if (!logger) {
        throw std::invalid_argument("Logger cannot be null");
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (!pImpl->shutdownInitiated) {
        pImpl->loggers.push_back(logger);
    }
}

void Logger::clearLoggers() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->shutdownInitiated = true;
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
    
    // Validate inputs
    if (message.empty()) {
        return; // Don't log empty messages
    }
    
    if (line < 0) {
        line = 0;
    }
    
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
            // If a logger fails, try to log to stderr as fallback
            std::cerr << "Logger error: " << e.what() << "\n";
            std::cerr << "Failed to log: " << message << "\n";
        }
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
    if (!logger) {
        throw std::invalid_argument("Global logger cannot be null");
    }
    
    // This is a placeholder for advanced global logger management
    // In practice, we just use getGlobal() which is already static
    // However, we can implement a simple version that replaces the global logger
    static std::shared_ptr<Logger> globalLoggerPtr;
    globalLoggerPtr = logger;
}

// LogStream implementation
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function) {}

LogStream::~LogStream() {
    try {
        Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
    } catch (const std::exception& e) {
        std::cerr << "LogStream error: " << e.what() << "\n";
        std::cerr << "LogStream failed: " << oss_.str() << "\n";
    }
}

} // namespace nlm
