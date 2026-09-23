#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <system_error>
#include <chrono>
#include <format>
#include <optional>
#include <span>
#include <thread>
#include <shared_mutex>
#include <filesystem>

namespace nlm {

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::optional<std::string> outputFilepath;
    std::optional<std::ofstream> fileOut;
    mutable std::shared_mutex mutex;
    std::atomic<size_t> logCount{0};
    
    // Thread-local storage for performance
    thread_local static std::string threadId;
    
    Impl() {
        // Initialize thread ID for debugging
        threadId = "Thread-" + std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    }
};

// Initialize thread_local static member
thread_local std::string ConsoleLogger::Impl::threadId;

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
}

ConsoleLogger::~ConsoleLogger() = default;

void ConsoleLogger::log(const LogEntry& entry) {
    // Fast path: check level without lock first (optimization)
    if (entry.level < pImpl->level) {
        return;
    }
    
    // Acquire shared lock for reading
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    
    // Update log count atomically
    ++pImpl->logCount;
    
    // Format log entry with modern C++ format
    std::string formattedMessage = formatLogEntry(entry);
    
    // Console output
    if (pImpl->useColors) {
        std::cout << getColorCode(entry.level) << formattedMessage << "\033[0m\n";
    } else {
        std::cout << formattedMessage << "\n";
    }
    
    // File output if set
    if (pImpl->fileOut && pImpl->fileOut->is_open()) {
        try {
            (*pImpl->fileOut) << formattedMessage << "\n";
            (*pImpl->fileOut).flush();
        } catch (const std::exception& e) {
            // Fallback to console if file logging fails
            std::cerr << "[ERROR] Failed to write to log file: " << e.what() << std::endl;
        }
    }
}

void ConsoleLogger::setLevel(LogLevel level) {
    std::unique_lock<std::shared_mutex> lock(pImpl->mutex);
    pImpl->level = level;
}

LogLevel ConsoleLogger::getLevel() const {
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    return pImpl->level;
}

void ConsoleLogger::setUseColors(bool useColors) {
    std::unique_lock<std::shared_mutex> lock(pImpl->mutex);
    pImpl->useColors = useColors;
}

bool ConsoleLogger::getUseColors() const {
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    return pImpl->useColors;
}

void ConsoleLogger::setOutputFile(const std::string& filepath) {
    std::unique_lock<std::shared_mutex> lock(pImpl->mutex);
    
    try {
        // Close existing file if open
        if (pImpl->fileOut && pImpl->fileOut->is_open()) {
            pImpl->fileOut->close();
        }
        
        // Create output directory if it doesn't exist
        std::filesystem::path filepath_path(filepath);
        if (!filepath_path.parent_path().empty()) {
            std::filesystem::create_directories(filepath_path.parent_path());
        }
        
        // Open file in append mode
        pImpl->fileOut.emplace(filepath, std::ios::app);
        if (!pImpl->fileOut->is_open()) {
            throw std::runtime_error("Failed to open log file: " + filepath);
        }
        
        pImpl->outputFilepath = filepath;
        
        // Log successful file setup
        std::cout << "[INFO] Log file opened: " << filepath << std::endl;
        
    } catch (const std::exception& e) {
        // Clear file stream on error
        pImpl->fileOut.reset();
        pImpl->outputFilepath.reset();
        throw; // Re-throw to allow caller to handle
    }
}

void ConsoleLogger::closeOutputFile() {
    std::unique_lock<std::shared_mutex> lock(pImpl->mutex);
    if (pImpl->fileOut && pImpl->fileOut->is_open()) {
        pImpl->fileOut->close();
        pImpl->fileOut.reset();
        pImpl->outputFilepath.reset();
        std::cout << "[INFO] Log file closed" << std::endl;
    }
}

std::string ConsoleLogger::levelToString(LogLevel level) const {
    // Use format instead of switch for consistency
    const std::string levelStrings[] = {
        "DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL"
    };
    int idx = static_cast<int>(level);
    return (idx >= 0 && idx < 5) ? levelStrings[idx] : "UNKNOWN";
}

std::string ConsoleLogger::levelToColor(LogLevel level) const {
    // Modern C++ approach using array lookup
    const std::string colorCodes[] = {
        "\033[36m",  // Cyan for DEBUG
        "\033[32m",  // Green for INFO
        "\033[33m",  // Yellow for WARNING
        "\033[31m",  // Red for ERROR
        "\033[35m",  // Magenta for CRITICAL
        "\033[0m"    // Reset
    };
    int idx = static_cast<int>(level);
    return (idx >= 0 && idx < 6) ? colorCodes[idx] : colorCodes[5];
}

std::string ConsoleLogger::formatLogEntry(const LogEntry& entry) const {
    std::ostringstream oss;
    
    // Get thread information for better debugging
    oss << "[" << levelToString(entry.level) << "] ";
    
    // Add thread ID for concurrent logging
    oss << "[" << pImpl->threadId << "] ";
    
    // Add timestamp with milliseconds
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        entry.timestamp.time_since_epoch()) % 1000;
    
    // Use modern C++ formatting if available
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    oss << "." << std::setfill('0') << std::setw(3) << ms.count();
    
    // Add source location information
    if (!entry.file.empty()) {
        // Extract filename from full path
        std::filesystem::path filepath(entry.file);
        oss << " " << filepath.filename().string();
        if (entry.line > 0) {
            oss << ":" << entry.line;
        }
    }
    
    // Add function name if available
    if (!entry.function.empty()) {
        oss << " in " << entry.function;
    }
    
    oss << " - " << entry.message;
    
    return oss.str();
}

// Logger class implementation
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    mutable std::shared_mutex mutex;
    std::atomic<size_t> totalLogs{0};
    std::chrono::steady_clock::time_point startTime;
    std::string sessionId;
    
    Impl() : startTime(std::chrono::steady_clock::now()) {
        // Generate unique session ID
        sessionId = "Session-" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
    }
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {}

Logger::~Logger() = default;

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    if (!logger) {
        throw std::invalid_argument("Logger cannot be null");
    }
    
    std::unique_lock<std::shared_mutex> lock(pImpl->mutex);
    pImpl->loggers.push_back(logger);
}

void Logger::clearLoggers() {
    std::unique_lock<std::shared_mutex> lock(pImpl->mutex);
    pImpl->loggers.clear();
}

void Logger::setLevel(LogLevel level) {
    std::unique_lock<std::shared_mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        if (logger) {
            logger->setLevel(level);
        }
    }
}

void Logger::log(LogLevel level, const std::string& message,
                const std::string& file, int line, const std::string& function) {
    // Fast check: if no loggers or level not enabled, skip processing
    {
        std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
        if (pImpl->loggers.empty() || !isEnabled(level)) {
            return;
        }
    }
    
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        if (logger) {
            try {
                logger->log(entry);
                ++pImpl->totalLogs;
            } catch (const std::exception& e) {
                // Log error but continue with other loggers
                std::cerr << "[ERROR] Logger error: " << e.what() << std::endl;
            }
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
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    for (const auto& logger : pImpl->loggers) {
        if (logger && logger->getLevel() <= level) {
            return true;
        }
    }
    return false;
}

Logger& Logger::getGlobal() {
    static Logger globalLogger;
    // Add console logger by default if no loggers are present
    static std::once_flag initFlag;
    std::call_once(initFlag, []() {
        auto& logger = Logger::getGlobal();
        logger.addLogger(std::make_shared<ConsoleLogger>(LogLevel::Info));
    });
    return globalLogger;
}

void Logger::setGlobal(std::shared_ptr<Logger> logger) {
    if (!logger) {
        throw std::invalid_argument("Global logger cannot be null");
    }
    // In practice, we just use getGlobal() which is already static
    // This function provides an alternative way to set global logger
}

// LogStream implementation
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function) {}

LogStream::~LogStream() {
    try {
        Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
    } catch (const std::exception& e) {
        std::cerr << "[CRITICAL] LogStream error: " << e.what() << std::endl;
    }
}

} // namespace nlm
