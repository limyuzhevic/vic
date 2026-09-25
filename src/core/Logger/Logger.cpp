#pragma once

// Logger.cpp - Enhanced implementation with comprehensive error handling

#include "Logger.hpp"
#include "Error/NLMError.hpp"
#include "Error/ErrorCategory.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <cstring>
#include <chrono>
#include <system_error>
#include <filesystem>

namespace nlm {

// Logger operation constants
static constexpr int MAX_FILE_SIZE = 100 * 1024 * 1024; // 100MB
static constexpr int MAX_RETRY_ATTEMPTS = 3;
static constexpr int RETRY_DELAY_MS = 50;

// File operation safety checks
static bool safeFileOperation(const std::string& filepath, std::ios_base::openmode mode) {
    if (filepath.empty()) {
        return false;
    }
    
    try {
        std::filesystem::path path(filepath);
        std::filesystem::file_status status = std::filesystem::status(path);
        
        if (mode & std::ios_base::in) {
            // Check read permissions
            auto perms = status.permissions();
            if ((perms & std::filesystem::perms::owner_read) == std::filesystem::perms::none &&
                (perms & std::filesystem::perms::group_read) == std::filesystem::perms::none &&
                (perms & std::filesystem::perms::others_read) == std::filesystem::perms::none) {
                return false;
            }
        }
        
        if (mode & std::ios_base::out) {
            // Check write permissions for file or directory
            auto perms = status.permissions();
            if ((perms & std::filesystem::perms::owner_write) == std::filesystem::perms::none &&
                (perms & std::filesystem::perms::group_write) == std::filesystem::perms::none &&
                (perms & std::filesystem::perms::others_write) == std::filesystem::perms::none) {
                return false;
            }
        }
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

static bool safeFileWrite(const std::string& filepath, const std::string& content) {
    if (!safeFileOperation(filepath, std::ios::out)) {
        return false;
    }
    
    try {
        std::ofstream file(filepath, std::ios::app);
        if (!file.is_open()) {
            return false;
        }
        
        // Check file size before writing
        std::filesystem::path path(filepath);
        if (std::filesystem::exists(path)) {
            auto fileSize = std::filesystem::file_size(path);
            if (fileSize > MAX_FILE_SIZE) {
                return false;
            }
        }
        
        file << content << std::endl;
        file.flush();
        
        return file.good();
    } catch (const std::exception&) {
        return false;
    }
}

static bool safeFileRead(const std::string& filepath, std::string& content) {
    if (!safeFileOperation(filepath, std::ios::in)) {
        return false;
    }
    
    try {
        std::ifstream file(filepath, std::ios::in);
        if (!file.is_open()) {
            return false;
        }
        
        // Read file content
        std::istreambuf_iterator<char> begin(file), end;
        content = std::string(begin, end);
        
        // Check content size
        if (content.size() > MAX_FILE_SIZE) {
            return false;
        }
        
        return !content.empty();
    } catch (const std::exception&) {
        return false;
    }
}

static std::string sanitizeLogMessage(const std::string& message) {
    // Remove control characters except newline and tab
    std::string result;
    result.reserve(message.size());
    
    for (char c : message) {
        if (c == '\n' || c == '\t' || (c >= 32 && c != 127)) {
            result += c;
        } else if (c == '') {
            // Skip carriage returns
            continue;
        } else {
            // Replace problematic characters with space
            result += ' ';
        }
    }
    
    return result;
}

static std::string getLogLevelColor(LogLevel level) {
    switch (level) {
        case LogLevel::Debug: return "\033[36m";    // Cyan
        case LogLevel::Info: return "\033[32m";      // Green
        case LogLevel::Warning: return "\033[33m";   // Yellow
        case LogLevel::Error: return "\033[31m";     // Red
        case LogLevel::Critical: return "\033[35m"; // Magenta
        default: return "\033[0m";
    }
}

static std::string getLogLevelString(LogLevel level) {
    switch (level) {
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Critical: return "CRIT";
        default: return "UNKNOWN";
    }
}

} // namespace nlm

// Thread-safe logging with error handling
namespace nlm {

void ConsoleLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    try {
        // Sanitize message to prevent log injection
        std::string sanitizedMessage = sanitizeLogMessage(entry.message);
        
        // Thread-safe log entry creation
        std::ostringstream oss;
        oss << "[" << getLogLevelString(entry.level) << "] ";
        
        // Add timestamp with error handling
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        if (std::localtime(&time) == nullptr) {
            throw LoggerError("log", "Failed to get local time for logging");
        }
        oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        
        oss << " [" << entry.file << ":" << entry.line << "] ";
        oss << " " << sanitizedMessage;
        
        std::string output = oss.str();
        
        // Console output with error handling
        if (pImpl->useColors) {
            std::cout << getLogLevelColor(entry.level) << output << "\033[0m\n";
        } else {
            std::cout << output << "\n";
        }
        
        // File output with retry logic
        if (pImpl->fileOut.is_open()) {
            bool writeSuccess = false;
            for (int attempt = 0; attempt < MAX_RETRY_ATTEMPTS && !writeSuccess; ++attempt) {
                try {
                    pImpl->fileOut << output << std::endl;
                    pImpl->fileOut.flush();
                    writeSuccess = pImpl->fileOut.good();
                    if (!writeSuccess && attempt < MAX_RETRY_ATTEMPTS - 1) {
                        std::this_thread::sleep_for(std::chrono::milliseconds(RETRY_DELAY_MS));
                    }
                } catch (const std::exception&) {
                    if (attempt == MAX_RETRY_ATTEMPTS - 1) {
                        throw LoggerError("log", "Failed to write to log file after " + std::to_string(MAX_RETRY_ATTEMPTS) + " attempts");
                    }
                }
            }
        }
        
    } catch (const LoggerError& e) {
        std::cerr << "Logger error: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error in logger: " << e.what() << std::endl;
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
    if (filepath.empty()) {
        throw LoggerError("setOutputFile", "Empty file path provided");
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Close existing file if open
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
    
    // Open new file with error handling
    try {
        pImpl->fileOut.open(filepath, std::ios::app);
        if (!pImpl->fileOut.is_open()) {
            throw LoggerError("setOutputFile", "Failed to open log file: " + filepath);
        }
        
        // Log file opening
        std::time_t t = std::time(nullptr);
        char mbstr[100];
        if (std::strftime(mbstr, sizeof(mbstr), "%Y-%m-%d %H:%M:%S", std::localtime(&t))) {
            pImpl->fileOut << "[INFO] " << mbstr << " - Logger initialized, output file: " << filepath << std::endl;
        }
        
    } catch (const LoggerError& e) {
        throw;
    } catch (const std::exception& e) {
        throw LoggerError("setOutputFile", "Error opening log file " + filepath + ": " + std::string(e.what()));
    }
}

void ConsoleLogger::closeOutputFile() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

std::string ConsoleLogger::levelToString(LogLevel level) const {
    return getLogLevelString(level);
}

std::string ConsoleLogger::levelToColor(LogLevel level) const {
    return getLogLevelColor(level);
}

} // namespace nlm

// Logger class implementation
namespace nlm {

void Logger::log(LogLevel level, const std::string& message,
                const std::string& file, int line, const std::string& function) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Validate parameters
    if (message.empty() && level != LogLevel::Info) {
        spdlog::warn("Empty log message for level: {}", static_cast<int>(level));
    }
    
    if (line < 0) {
        spdlog::warn("Invalid line number: {} for message: {}", line, message);
    }
    
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file.empty() ? "unknown" : file;
    entry.line = line < 0 ? 0 : line;
    entry.function = function.empty() ? "unknown" : function;
    entry.timestamp = std::chrono::system_clock::now();
    
    for (auto& logger : pImpl->loggers) {
        try {
            logger->log(entry);
        } catch (const LoggerError& e) {
            spdlog::error("Logger error in LogStream: {}", e.what());
        } catch (const std::exception& e) {
            spdlog::error("Unexpected logger error: {}", e.what());
        }
    }
}

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    if (!logger) {
        throw std::invalid_argument("Logger cannot be null");
    }
    
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
namespace nlm {

LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function) {}

LogStream::~LogStream() {
    try {
        Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
    } catch (const LoggerError& e) {
        std::cerr << "Logger error in LogStream destructor: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error in LogStream destructor: " << e.what() << std::endl;
    }
}

} // namespace nlm

// Convenience function implementations
namespace nlm {

std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string generateLogFilename(const std::string& baseName, const std::string& extension) {
    if (baseName.empty()) {
        throw LoggerError("generateLogFilename", "Empty base filename provided");
    }
    
    std::string timestamp = getCurrentTimestamp();
    // Replace colons and spaces with underscores for filesystem safety
    std::replace(timestamp.begin(), timestamp.end(), ':', '_');
    std::replace(timestamp.begin(), timestamp.end(), ' ', '_');
    
    return baseName + "_" + timestamp + "." + extension;
}

} // namespace nlm
