#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <filesystem>
#include <ctime>
#include <algorithm>

namespace nlm {

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    bool initialized = false;
    size_t maxFileSize = 10 * 1024 * 1024;  // 10MB
    size_t currentFileSize = 0;
    std::string filepath;
    
    // Enhanced error reporting
    bool logFileErrors = true;
    size_t fileErrorCount = 0;
    static constexpr size_t MAX_FILE_ERRORS = 5;
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {
    initialize();
}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
    initialize();
}

ConsoleLogger::~ConsoleLogger() {
    closeOutputFile();
}

void ConsoleLogger::initialize() {
    pImpl->initialized = true;
    pImpl->filepath = "";
}

void ConsoleLogger::log(const LogEntry& entry) {
    if (!pImpl->initialized || entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    
    // Add thread ID for debugging
    oss << "[tid:" << std::this_thread::get_id() << "] ";
    
    // Add timestamp with milliseconds
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    oss << std::put_time(std::localtime(&time_t_now), "%Y-%m-%d %H:%M:%S")
        << "." << std::setfill('0') << std::setw(3) << ms.count();
    
    // Add source location
    if (!entry.file.empty()) {
        // Extract filename from path
        std::string filename = entry.file;
        size_t lastSlash = filename.find_last_of("/\\");
        if (lastSlash != std::string::npos) {
            filename = filename.substr(lastSlash + 1);
        }
        oss << " [" << filename << ":" << entry.line << "]";
        if (!entry.function.empty()) {
            oss << " " << entry.function << "()";
        }
    }
    
    oss << " " << entry.message;
    
    std::string output = oss.str();
    
    // Handle file rotation and error checking
    if (!pImpl->filepath.empty()) {
        if (!handleFileOutput(output)) {
            return;  // Skip console output if file logging failed
        }
    }
    
    // Console output
    if (pImpl->useColors) {
        std::cout << levelToColor(entry.level) << output << "\033[0m\n";
    } else {
        std::cout << output << "\n";
    }
    
    // Flush if errors occurred
    if (pImpl->fileErrorCount > 0) {
        std::cout.flush();
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
    
    // Close existing file
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
    
    pImpl->filepath = filepath;
    
    // Try to open the new file
    if (!openLogFile()) {
        // Keep old file if new one fails
        pImpl->filepath = "";
        std::cerr << "WARNING: Failed to set output file, logging to console only" << std::endl;
    }
}

void ConsoleLogger::closeOutputFile() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
    pImpl->filepath.clear();
    pImpl->currentFileSize = 0;
}

void ConsoleLogger::setMaxFileSize(size_t maxSize) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->maxFileSize = maxSize;
}

void ConsoleLogger::setLogFileErrors(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->logFileErrors = enable;
}

// Logger class implementation improvements
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    bool initialized;
    
    // Enhanced error handling
    bool logErrorsToStderr;
    size_t maxConcurrentLogs;
    std::chrono::steady_clock::time_point lastLogTime;
    std::chrono::milliseconds minLogInterval;
    
    Impl() : initialized(false), logErrorsToStderr(true), 
             maxConcurrentLogs(100), minLogInterval(std::chrono::milliseconds(1)) {
        initialized = true;
        lastLogTime = std::chrono::steady_clock::now();
    }
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {}

Logger::~Logger() {
    clearLoggers();
}

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    if (!pImpl->initialized || !logger) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->loggers.size() >= pImpl->maxConcurrentLogs) {
        if (pImpl->logErrorsToStderr) {
            std::cerr << "WARNING: Maximum concurrent loggers reached, not adding new logger" << std::endl;
        }
        return;
    }
    
    pImpl->loggers.push_back(logger);
}

void Logger::clearLoggers() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (auto& logger : pImpl->loggers) {
        try {
            // Attempt to close any open resources
            if (auto consoleLogger = std::dynamic_pointer_cast<ConsoleLogger>(logger)) {
                consoleLogger->closeOutputFile();
            }
        } catch (...) {
            // Ignore exceptions during cleanup
        }
    }
    
    pImpl->loggers.clear();
}

void Logger::setLevel(LogLevel level) {
    if (!pImpl->initialized) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (auto& logger : pImpl->loggers) {
        try {
            logger->setLevel(level);
        } catch (...) {
            if (pImpl->logErrorsToStderr) {
                std::cerr << "ERROR: Failed to set log level on logger" << std::endl;
            }
        }
    }
}

void Logger::log(LogLevel level, const std::string& message,
                const std::string& file, int line, const std::string& function) {
    if (!pImpl->initialized) {
        return;
    }
    
    // Rate limiting to prevent log flooding
    if (level < LogLevel::Warning) {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - pImpl->lastLogTime);
        if (elapsed < pImpl->minLogInterval) {
            return;
        }
        pImpl->lastLogTime = now;
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
        } catch (...) {
            if (pImpl->logErrorsToStderr) {
                std::cerr << "ERROR: Failed to log to logger" << std::endl;
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
    if (!pImpl->initialized) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (auto& logger : pImpl->loggers) {
        try {
            if (logger->getLevel() <= level) {
                return true;
            }
        } catch (...) {
            if (pImpl->logErrorsToStderr) {
                std::cerr << "ERROR: Failed to check logger level" << std::endl;
            }
        }
    }
    
    return false;
}

Logger& Logger::getGlobal() {
    static Logger globalLogger;
    return globalLogger;
}

void Logger::setGlobal(std::shared_ptr<Logger> logger) {
    if (logger) {
        // This is a placeholder for advanced global logger management
        // In practice, we just use getGlobal() which is already static
    }
}

// LogStream implementation with enhanced error handling
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function) {
    
    try {
        // Validate inputs
        if (function.empty()) {
            function_ = "unknown";
        }
        if (file.empty()) {
            file_ = "unknown";
        }
    } catch (...) {
        // Handle exceptions in constructor
        level_ = LogLevel::Error;
        function_ = "error";
        file_ = "unknown";
    }
}

LogStream::~LogStream() {
    try {
        if (level_ != LogLevel::Debug || Logger::getGlobal().isEnabled(level_)) {
            Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
        }
    } catch (...) {
        // If logging fails, attempt to log to stderr as a fallback
        std::cerr << "CRITICAL: Failed to log message" << std::endl;
    }
}

} // namespace nlm
