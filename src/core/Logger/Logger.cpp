#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <filesystem>
#include <chrono>
#include <ctime>
#include <algorithm>

namespace nlm {

// Simplified but enhanced ConsoleLogger implementation
struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    
    // Enhanced logging options
    bool enableTimestamps = true;
    bool enableFileInfo = true;  // Include file and line info
    std::string timestampFormat = "%Y-%m-%d %H:%M:%S";
    size_t maxFileSize = 0;  // 0 means unlimited
    size_t currentFileSize = 0;
    std::string rotationPrefix = "";
    bool enableLogRotation = false;
    bool enableAsyncLogging = true;  // Enable async logging for better performance
    
    // Thread pool for async logging (simplified implementation)
    std::mutex logQueueMutex;
    std::deque<LogEntry> logQueue;
    bool shouldStop = false;
    std::thread logThread;
    
    // Custom formatting
    std::string messageFormat = "[%LEVEL] %TIMESTAMP %MESSAGE";
    
    Impl() : level(LogLevel::Info), useColors(true), enableTimestamps(true), 
             enableFileInfo(true), enableAsyncLogging(true) {
        // Start async logging thread if enabled
        if (enableAsyncLogging) {
            shouldStop = false;
            logThread = std::thread(&ConsoleLogger::Impl::loggingThread, this);
        }
    }
    
    ~Impl() {
        shouldStop = true;
        if (enableAsyncLogging && logThread.joinable()) {
            logThread.join();
        }
        if (fileOut.is_open()) {
            fileOut.close();
        }
    }
    
    void loggingThread() {
        while (!shouldStop) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            
            std::lock_guard<std::mutex> lock(logQueueMutex);
            if (!logQueue.empty()) {
                LogEntry entry = logQueue.front();
                logQueue.pop_front();
                processLogEntry(entry);
            }
        }
    }
    
    void processLogEntry(const LogEntry& entry) {
        std::lock_guard<std::mutex> lock(mutex);
        
        // Format the log message
        std::string formattedMessage = formatMessage(entry);
        
        // Output to console
        if (useColors) {
            std::cout << levelToColor(entry.level) << formattedMessage << "\033[0m\n";
        } else {
            std::cout << formattedMessage << "\n";
        }
        
        // Output to file if set
        if (fileOut.is_open()) {
            fileOut << formattedMessage << "\n";
            currentFileSize += formattedMessage.size() + 1;  // +1 for newline
            
            // Check for file rotation
            if (enableLogRotation && maxFileSize > 0 && currentFileSize >= maxFileSize) {
                rotateLogFile();
            }
            
            fileOut.flush();
        }
    }
    
    std::string formatMessage(const LogEntry& entry) {
        std::string formatted;
        std::string levelStr = levelToString(entry.level);
        
        // Replace placeholders in message format
        formatted = messageFormat;
        
        // Replace %LEVEL
        size_t pos = formatted.find("%LEVEL");
        while (pos != std::string::npos) {
            formatted.replace(pos, 7, levelStr);
            pos = formatted.find("%LEVEL", pos + levelStr.size());
        }
        
        // Replace %TIMESTAMP
        std::string timestamp;
        if (enableTimestamps) {
            auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
            timestamp = std::put_time(std::localtime(&time), timestampFormat.c_str());
        } else {
            timestamp = "";
        }
        
        pos = formatted.find("%TIMESTAMP");
        while (pos != std::string::npos) {
            formatted.replace(pos, 11, timestamp);
            pos = formatted.find("%TIMESTAMP", pos + timestamp.size());
        }
        
        // Replace %MESSAGE
        pos = formatted.find("%MESSAGE");
        while (pos != std::string::npos) {
            formatted.replace(pos, 8, entry.message);
            pos = formatted.find("%MESSAGE", pos + entry.message.size());
        }
        
        // Replace %FILE (if enabled)
        if (enableFileInfo) {
            pos = formatted.find("%FILE");
            while (pos != std::string::npos) {
                formatted.replace(pos, 6, entry.file);
                pos = formatted.find("%FILE", pos + entry.file.size());
            }
            
            // Replace %LINE (if enabled)
            pos = formatted.find("%LINE");
            while (pos != std::string::npos) {
                formatted.replace(pos, 6, std::to_string(entry.line));
                pos = formatted.find("%LINE", pos + std::to_string(entry.line).size());
            }
            
            // Replace %FUNCTION (if enabled)
            pos = formatted.find("%FUNCTION");
            while (pos != std::string::npos) {
                formatted.replace(pos, 9, entry.function);
                pos = formatted.find("%FUNCTION", pos + entry.function.size());
            }
        }
        
        return formatted;
    }
    
    void rotateLogFile() {
        if (fileOut.is_open()) {
            fileOut.close();
        }
        
        // Generate new filename with rotation prefix
        std::string currentPath = "/tmp/nlm.log";  // Simplified - would need actual path from setOutputFile
        std::string rotatedPath = currentPath;
        if (!rotationPrefix.empty()) {
            rotatedPath = rotationPrefix + "_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + ".log";
        }
        
        // Rename current log file to rotated version
        if (std::filesystem::exists(currentPath)) {
            std::filesystem::rename(currentPath, rotatedPath);
        }
        
        // Open new log file
        fileOut.open(currentPath, std::ios::app);
        currentFileSize = 0;
        
        // Write rotation notice
        if (fileOut.is_open()) {
            fileOut << "# Log file rotated at " << formatMessage(LogEntry{LogLevel::Info, "Log rotation", "", 0, "", std::chrono::system_clock::now()}) << "\n";
        }
    }
    
    bool addToLogQueue(const LogEntry& entry) {
        if (enableAsyncLogging) {
            std::lock_guard<std::mutex> lock(logQueueMutex);
            logQueue.push_back(entry);
            return true;
        } else {
            processLogEntry(entry);
            return false;
        }
    }
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
}

ConsoleLogger::~ConsoleLogger() = default;

void ConsoleLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    pImpl->addToLogQueue(entry);
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
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->fileOut.close();
    pImpl->fileOut.open(filepath, std::ios::app);
    pImpl->currentFileSize = 0;
    
    if (pImpl->fileOut.is_open()) {
        NLM_LOG_INFO("Log file opened: " + filepath);
    } else {
        NLM_LOG_ERROR("Failed to open log file: " + filepath);
    }
}

void ConsoleLogger::closeOutputFile() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
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
    
    // Global logging configuration
    bool enableTimestamps = true;
    std::string timestampFormat = "%Y-%m-%d %H:%M:%S";
    bool enableColors = true;
    LogLevel minLevel = LogLevel::Info;
    bool enableAsyncLogging = true;
    
    // Performance monitoring
    std::chrono::system_clock::time_point startTime;
    uint64_t totalLogMessages = 0;
    uint64_t totalLogSize = 0;
    
    Impl() : startTime(std::chrono::system_clock::now()) {}
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {}

Logger::~Logger() = default;

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    if (!logger) {
        NLM_LOG_ERROR("Attempted to add null logger");
        return;
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
    pImpl->minLevel = level;
    
    for (auto& logger : pImpl->loggers) {
        logger->setLevel(level);
    }
    
    NLM_LOG_INFO("Global log level set to: " + ConsoleLogger::Impl().levelToString(level));
}

void Logger::log(LogLevel level, const std::string& message,
                const std::string& file, int line, const std::string& function) {
    // Check if this level is enabled
    if (level < pImpl->minLevel) {
        return;
    }
    
    // Update performance metrics
    {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        pImpl->totalLogMessages++;
        pImpl->totalLogSize += message.size();
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
            // Error in one logger shouldn't affect others
            std::cerr << "ERROR in logger: " << e.what() << std::endl;
        }
    }
}

void Logger::debug(const std::string& message, const std::string& file, int line, const std::string& function) {
    if (pImpl->minLevel <= LogLevel::Debug) {
        log(LogLevel::Debug, message, file, line, function);
    }
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
    return level >= pImpl->minLevel;
}

Logger& Logger::getGlobal() {
    static Logger globalLogger;
    return globalLogger;
}

void Logger::setGlobal(std::shared_ptr<Logger> logger) {
    if (logger) {
        // This would require changing the static global instance, which is complex
        // For simplicity, we just log a warning and use the existing global
        NLM_LOG_WARNING("Global logger replacement not supported - using existing global logger");
    }
}

// Enhanced configuration methods for ConsoleLogger
void ConsoleLogger::setLogOptions(const std::string& timestampFormat, bool enableTimestamps, 
                                 bool enableFileInfo, const std::string& messageFormat,
                                 bool enableColors, bool enableAsync, bool enableRotation,
                                 size_t maxFileSize, const std::string& rotationPrefix) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->timestampFormat = timestampFormat;
    pImpl->enableTimestamps = enableTimestamps;
    pImpl->enableFileInfo = enableFileInfo;
    pImpl->messageFormat = messageFormat;
    pImpl->useColors = enableColors;
    pImpl->enableAsyncLogging = enableAsync;
    pImpl->enableLogRotation = enableRotation;
    pImpl->maxFileSize = maxFileSize;
    pImpl->rotationPrefix = rotationPrefix;
    
    if (enableRotation && maxFileSize > 0) {
        NLM_LOG_INFO("Log rotation enabled - max file size: " + std::to_string(maxFileSize) + " bytes");
    }
}

void ConsoleLogger::setMinLogLevel(LogLevel level) {
    setLevel(level);
}

LogLevel ConsoleLogger::getMinLogLevel() const {
    return getLevel();
}

void ConsoleLogger::setEnableTimestamps(bool enable) {
    pImpl->enableTimestamps = enable;
}

bool ConsoleLogger::getEnableTimestamps() const {
    return pImpl->enableTimestamps;
}

void ConsoleLogger::setEnableFileInfo(bool enable) {
    pImpl->enableFileInfo = enable;
}

bool ConsoleLogger::getEnableFileInfo() const {
    return pImpl->enableFileInfo;
}

void ConsoleLogger::setMessageFormat(const std::string& format) {
    pImpl->messageFormat = format;
}

std::string ConsoleLogger::getMessageFormat() const {
    return pImpl->messageFormat;
}

void ConsoleLogger::setLogRotation(bool enable, size_t maxSize, const std::string& prefix) {
    pImpl->enableLogRotation = enable;
    pImpl->maxFileSize = maxSize;
    pImpl->rotationPrefix = prefix;
    
    if (enable) {
        NLM_LOG_INFO("Log rotation configured - max size: " + std::to_string(maxSize) + " bytes");
    }
}

void ConsoleLogger::setEnableAsyncLogging(bool enable) {
    if (enable != pImpl->enableAsyncLogging) {
        pImpl->enableAsyncLogging = enable;
        if (enable) {
            // Restart logging thread
            shouldStop = false;
            logThread = std::thread(&ConsoleLogger::Impl::loggingThread, this);
        } else {
            shouldStop = true;
            if (logThread.joinable()) {
                logThread.join();
            }
        }
    }
}

bool ConsoleLogger::getEnableAsyncLogging() const {
    return pImpl->enableAsyncLogging;
}

// Performance monitoring methods
uint64_t Logger::getTotalLogMessages() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->totalLogMessages;
}

uint64_t Logger::getTotalLogSize() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->totalLogSize;
}

std::chrono::system_clock::time_point Logger::getStartTime() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->startTime;
}

std::string Logger::getUptime() const {
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - getStartTime());
    
    int hours = duration.count() / 3600;
    int minutes = (duration.count() % 3600) / 60;
    int seconds = duration.count() % 60;
    
    return std::to_string(hours) + "h " + std::to_string(minutes) + "m " + std::to_string(seconds) + "s";
}

// Global logging configuration functions
void Logger::configureGlobal(bool enableTimestamps, const std::string& timestampFormat,
                           bool enableColors, LogLevel minLevel, bool enableAsync) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->enableTimestamps = enableTimestamps;
    pImpl->timestampFormat = timestampFormat;
    pImpl->enableColors = enableColors;
    pImpl->minLevel = minLevel;
    pImpl->enableAsyncLogging = enableAsync;
    
    NLM_LOG_INFO("Global logging configuration updated");
}

// Enhanced LogStream implementation
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function) {}

LogStream::~LogStream() {
    Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
}

} // namespace nlm
