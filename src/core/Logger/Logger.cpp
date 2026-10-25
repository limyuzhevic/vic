#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <chrono>
#include <filesystem>
#include <thread>

namespace nlm {

// Console logger implementation
struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    size_t maxFileSize = 10 * 1024 * 1024;  // 10MB
    size_t currentFileSize = 0;
    std::string filepath;
    bool logRotationEnabled = true;
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
    
    // Add timestamp
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    
    // Add thread ID for debugging
    oss << " [Thread:" << std::this_thread::get_id() << "]";
    
    // Add source location if provided
    if (!entry.file.empty() && entry.line > 0) {
        // Extract filename from path
        size_t lastSlash = entry.file.find_last_of("/\\");
        std::string filename = (lastSlash != std::string::npos) ? 
            entry.file.substr(lastSlash + 1) : entry.file;
        oss << " " << filename << ":" << entry.line;
        if (!entry.function.empty()) {
            oss << " (" << entry.function << ")";
        }
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
        // Check if we need to rotate the log file
        if (pImpl->logRotationEnabled && 
            pImpl->currentFileSize + output.size() + 1 > pImpl->maxFileSize) {
            rotateLogFile();
        }
        
        pImpl->fileOut << output << "\n";
        pImpl->currentFileSize += output.size() + 1;
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

void ConsoleLogger::setOutputFile(const std::string& filepath, size_t maxSizeMB, bool enableRotation) {
    // Close current file if open
    closeOutputFile();
    
    pImpl->filepath = filepath;
    pImpl->maxFileSize = maxSizeMB * 1024 * 1024;
    pImpl->logRotationEnabled = enableRotation;
    pImpl->currentFileSize = 0;
    
    // Open file in append mode
    pImpl->fileOut.open(filepath, std::ios::app);
    if (!pImpl->fileOut.is_open()) {
        // Try to create directory if it doesn't exist
        size_t lastSlash = filepath.find_last_of("/\\");
        if (lastSlash != std::string::npos) {
            std::string dir = filepath.substr(0, lastSlash);
            // Note: In a production system, we would check if directory exists
            // and create it if needed. For now, we'll just report an error.
            NLM_LOG_ERROR("Cannot create log directory: " + dir);
        }
        NLM_LOG_ERROR("Cannot open log file: " + filepath);
    } else {
        NLM_LOG_INFO("Log file opened: " + filepath);
        
        // Try to get current file size
        std::error_code ec;
        pImpl->currentFileSize = std::filesystem::file_size(filepath, ec);
        if (ec) {
            pImpl->currentFileSize = 0;  // Reset if we can't get size
        }
    }
}

void ConsoleLogger::closeOutputFile() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
        NLM_LOG_DEBUG("Log file closed: " + pImpl->filepath);
    }
}

void ConsoleLogger::rotateLogFile() {
    if (!pImpl->logRotationEnabled || !pImpl->fileOut.is_open()) {
        return;
    }
    
    // Close current file
    pImpl->fileOut.close();
    
    // Generate backup filename
    std::string backupFilepath = pImpl->filepath + "." + 
        std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
    
    // Rename current file to backup
    bool renamed = std::rename(pImpl->filepath.c_str(), backupFilepath.c_str()) == 0;
    
    if (renamed) {
        NLM_LOG_INFO("Log file rotated: " + pImpl->filepath + " -> " + backupFilepath);
        
        // Reopen file
        pImpl->fileOut.open(pImpl->filepath, std::ios::app);
        if (pImpl->fileOut.is_open()) {
            pImpl->currentFileSize = 0;
        } else {
            NLM_LOG_ERROR("Failed to reopen log file after rotation: " + pImpl->filepath);
        }
    } else {
        // If rename failed, try to create a new file
        std::error_code ec;
        std::filesystem::remove(pImpl->filepath, ec);  // Delete old file
        pImpl->fileOut.open(pImpl->filepath, std::ios::app);
        if (!pImpl->fileOut.is_open()) {
            NLM_LOG_ERROR("Failed to recreate log file after rotation: " + pImpl->filepath);
        } else {
            pImpl->currentFileSize = 0;
            NLM_LOG_INFO("Log file recreated: " + pImpl->filepath);
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
    bool initialized = false;
    std::chrono::system_clock::time_ptr logStartTime;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {
    pImpl->logStartTime = std::make_shared<std::chrono::system_clock::time_point>(
        std::chrono::system_clock::now()
    );
    pImpl->initialized = true;
}

Logger::~Logger() {
    // Clean up all loggers
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        // Try to close file output if it's a ConsoleLogger
        auto consoleLogger = std::dynamic_pointer_cast<ConsoleLogger>(logger);
        if (consoleLogger) {
            consoleLogger->closeOutputFile();
        }
    }
}

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    if (!logger) {
        NLM_LOG_WARNING("Attempted to add null logger");
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.push_back(logger);
    
    // Set initial log level
    logger->setLevel(pImpl->loggers.size() > 1 ? LogLevel::Warning : LogLevel::Info);
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
    if (!pImpl->initialized) {
        return;  // Skip if not initialized
    }
    
    // Quick check for level before acquiring lock
    if (!isEnabled(level)) {
        return;
    }
    
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
            // Log error to stderr if logging fails
            std::cerr << "[ERROR] Failed to log message: " << e.what() << std::endl;
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
    // This is a placeholder for advanced global logger management
    // In practice, we just use getGlobal() which is already static
}

// LogStream implementation
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function) {}

LogStream::~LogStream() {
    try {
        Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Failed to log stream message: " << e.what() << std::endl;
    }
}

} // namespace nlm
