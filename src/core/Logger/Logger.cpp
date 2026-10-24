#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <filesystem>
#include <ctime>
#include <algorithm>
#include <chrono>

namespace nlm {

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    std::string filepath;
    size_t maxFileSize = 10 * 1024 * 1024; // 10MB
    size_t currentFileSize = 0;
    bool enableRotation = false;
    int maxFiles = 5;
    bool fileErrorReported = false;
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {
    // Initialize with default values
}

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
    
    // Add timestamp with milliseconds
    auto timePoint = std::chrono::system_clock::to_time_t(entry.timestamp);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        entry.timestamp.time_since_epoch()) % 1000;
    
    // Use localtime_s for thread safety on Windows, localtime_r on Linux
    std::tm tm_buf;
#ifdef _WIN32
    localtime_s(&tm_buf, &timePoint);
#else
    localtime_r(&timePoint, &tm_buf);
#endif
    
    oss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
    oss << "." << std::setfill('0') << std::setw(3) << ms.count();
    
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
        pImpl->currentFileSize += output.length() + 1; // +1 for newline
        
        // Check if file rotation is needed
        if (pImpl->enableRotation && pImpl->currentFileSize >= pImpl->maxFileSize) {
            rotateLogFile();
        }
        
        // Periodic flush for important entries (error/critical)
        if (entry.level >= LogLevel::Error) {
            pImpl->fileOut.flush();
        }
    }
}

void ConsoleLogger::setLevel(LogLevel level) {
    pImpl->level = level;
}

LogLevel ConsoleLogger::getLevel() const {
    return pImpl->level;
}

void ConsoleLogger::setFileRotation(bool enable, size_t maxSize, int maxFiles) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->enableRotation = enable;
    pImpl->maxFileSize = maxSize;
    pImpl->maxFiles = maxFiles;
}

bool ConsoleLogger::getFileRotation() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->enableRotation;
}

size_t ConsoleLogger::getMaxFileSize() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->maxFileSize;
}

int ConsoleLogger::getMaxFiles() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->maxFiles;
}
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Close current file if open
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
    
    pImpl->filepath = filepath;
    
    // Create directory if it doesn't exist
    std::filesystem::path dirPath = std::filesystem::path(filepath).parent_path();
    if (!dirPath.empty() && !std::filesystem::exists(dirPath)) {
        try {
            std::filesystem::create_directories(dirPath);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to create log directory " + dirPath.string() + ": " + e.what());
            return;
        }
    }
    
    // Open file in append mode
    pImpl->fileOut.open(filepath, std::ios::app);
    if (pImpl->fileOut.is_open()) {
        // Get current file size for rotation tracking
        pImpl->currentFileSize = std::filesystem::file_size(filepath);
        pImpl->fileErrorReported = false;
        NLM_LOG_INFO("Log file opened: " + filepath);
    } else {
        if (!pImpl->fileErrorReported) {
            NLM_LOG_ERROR("Failed to open log file: " + filepath);
            pImpl->fileErrorReported = true;
        }
    }
}

void ConsoleLogger::closeOutputFile() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
        pImpl->filepath.clear();
        pImpl->currentFileSize = 0;
        NLM_LOG_INFO("Log file closed");
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

void ConsoleLogger::rotateLogFile() {
    if (!pImpl->enableRotation || !pImpl->fileOut.is_open()) {
        return;
    }
    
    // Close current file
    pImpl->fileOut.close();
    
    // Rotate existing files
    for (int i = pImpl->maxFiles - 1; i >= 0; --i) {
        std::string source, destination;
        if (i == pImpl->maxFiles - 1) {
            source = destination = pImpl->filepath;
        } else {
            source = pImpl->filepath + "." + std::to_string(i + 1);
            destination = pImpl->filepath + "." + std::to_string(i);
        }
        
        if (std::filesystem::exists(source)) {
            if (i < pImpl->maxFiles - 1 && std::filesystem::exists(destination)) {
                std::filesystem::remove(destination);
            }
            std::filesystem::rename(source, destination);
        }
    }
    
    // Reopen the main log file
    pImpl->fileOut.open(pImpl->filepath, std::ios::trunc);
    if (pImpl->fileOut.is_open()) {
        pImpl->currentFileSize = 0;
        NLM_LOG_INFO("Log file rotated: " + pImpl->filepath);
    } else {
        NLM_LOG_ERROR("Failed to reopen log file after rotation: " + pImpl->filepath);
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
