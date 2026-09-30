// Enhanced Logger implementation with performance and memory management improvements
// This version includes optimizations for logging speed and memory efficiency

#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <atomic>
#include <chrono>
#include <filesystem>
#include <memory>
#include <cstring>

namespace nlm {
namespace logger_internal {
    
// Thread-local storage for performance
class ThreadLocalLogger {
private:
    bool isInitialized = false;
    std::unique_ptr<Impl> pImpl;
    std::mutex initMutex;
    
public:
    ThreadLocalLogger() {
        std::lock_guard<std::mutex> lock(initMutex);
        if (!isInitialized) {
            pImpl = std::make_unique<Impl>();
            isInitialized = true;
        }
    }
    
    Impl* get() { return pImpl.get(); }
};

// Global logger pool for memory efficiency
class LoggerPool {
private:
    std::vector<std::shared_ptr<ConsoleLogger>> loggerPool;
    std::mutex mutex;
    std::atomic<size_t> nextIndex{0};
    
public:
    std::shared_ptr<ConsoleLogger> getLogger() {
        std::lock_guard<std::mutex> lock(mutex);
        
        if (loggerPool.size() < 100) {  // Pre-allocate pool
            auto logger = std::make_shared<ConsoleLogger>();
            loggerPool.push_back(logger);
            return logger;
        }
        
        // Return from pool
        size_t index = nextIndex++ % loggerPool.size();
        return loggerPool[index];
    }
};

// Global logger pool instance
thread_local ThreadLocalLogger threadLocalLogger;
LoggerPool loggerPool;

// Optimized timestamp generation
class TimestampGenerator {
private:
    std::chrono::system_clock::time_point epoch = std::chrono::system_clock::now();
    uint64_t epochNs = std::chrono::duration_cast<std::chrono::nanoseconds>(epoch.time_since_epoch()).count();
    
public:
    TimestampGenerator() = default;
    
    std::chrono::system_clock::time_point getCurrentTime() {
        return std::chrono::system_clock::now();
    }
    
    std::string formatTimestamp(const std::chrono::system_clock::time_point& timePoint) {
        static const char* days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
        static const char* months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", 
                                      "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
        
        auto time = std::chrono::system_clock::to_time_t(timePoint);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(timePoint.time_since_epoch()) %
                  std::chrono::milliseconds(1).count();
        
        std::tm tmBuf;
        localtime_r(&time, &tmBuf);
        
        char buffer[32];
        std::snprintf(buffer, sizeof(buffer), "%s %s %02d %02d:%02d:%02d.%03d %d",
                     days[tmBuf.tm_wday], months[tmBuf.tm_mon],
                     tmBuf.tm_mday, tmBuf.tm_hour, tmBuf.tm_min, tmBuf.tm_sec,
                     static_cast<int>(ms), tmBuf.tm_year + 1900);
        
        return std::string(buffer);
    }
};

// String interning for memory efficiency
class StringInterner {
private:
    std::unordered_map<std::string, std::string> internPool;
    std::mutex mutex;
    
public:
    const std::string& intern(const std::string& str) {
        std::lock_guard<std::mutex> lock(mutex);
        
        auto it = internPool.find(str);
        if (it != internPool.end()) {
            return it->second;
        }
        
        auto result = internPool.emplace(str, str);
        return result.first->second;
    }
    
    const std::string& intern(const char* str) {
        return intern(std::string(str));
    }
};

// Message formatting optimizations
class MessageFormatter {
private:
    static constexpr size_t MAX_BUFFER_SIZE = 1024;
    char buffer[MAX_BUFFER_SIZE];
    
public:
    std::string format(const std::string& level, const std::string& timestamp, 
                      const std::string& message) {
        // Use snprintf for faster string concatenation
        size_t len = std::snprintf(buffer, sizeof(buffer), 
                                  "[%s] %s %s", 
                                  level.c_str(), timestamp.c_str(), message.c_str());
        
        if (len >= sizeof(buffer) - 1) {
            // Fallback for very long messages
            return "[" + level + "] " + timestamp + " " + message;
        }
        
        return std::string(buffer, len);
    }
};

// File writer with buffering
class BufferedFileWriter {
private:
    std::ofstream file;
    std::mutex mutex;
    std::vector<std::string> buffer;
    size_t maxBufferSize = 1000;
    
public:
    bool open(const std::string& filepath) {
        std::lock_guard<std::mutex> lock(mutex);
        
        // Ensure directory exists
        std::filesystem::path path(filepath);
        if (!path.parent_path().empty()) {
            std::filesystem::create_directories(path.parent_path());
        }
        
        file.open(filepath, std::ios::app);
        return file.is_open();
    }
    
    void write(const std::string& message) {
        std::lock_guard<std::mutex> lock(mutex);
        
        if (!file.is_open()) {
            return;
        }
        
        // Buffered writing
        buffer.push_back(message);
        if (buffer.size() >= maxBufferSize) {
            flush();
        }
    }
    
    void flush() {
        if (!file.is_open() || buffer.empty()) {
            return;
        }
        
        for (const auto& msg : buffer) {
            file << msg << "\n";
        }
        file.flush();
        buffer.clear();
    }
    
    void close() {
        std::lock_guard<std::mutex> lock(mutex);
        flush();
        if (file.is_open()) {
            file.close();
        }
    }
};

// Level to string mapping (const for optimization)
inline const char* getLevelString(LogLevel level) {
    static const char* levelStrings[] = {"DEBUG", "INFO", "WARN", "ERROR", "CRIT"};
    return levelStrings[static_cast<int>(level)];
}

// Color codes for console output
inline const char* getColorCode(LogLevel level) {
    static const char* colorCodes[] = {"\033[36m", "\033[32m", "\033[33m", "\033[31m", "\033[35m"};
    return colorCodes[static_cast<int>(level)];
}

inline const char* getResetCode() {
    return "\033[0m";
}

} // namespace logger_internal

using namespace logger_internal;

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    BufferedFileWriter fileWriter;
    std::mutex mutex;
    TimestampGenerator timestampGen;
    StringInterner stringInterner;
    MessageFormatter messageFormatter;
    
    // Cache for frequently used strings
    const std::string levelDebug = "DEBUG";
    const std::string levelInfo = "INFO";
    const std::string levelWarning = "WARN";
    const std::string levelError = "ERROR";
    const std::string levelCritical = "CRIT";
    const std::string levelReset = "\033[0m";
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
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Use cached level strings for better performance
    const std::string& levelStr = entry.level == LogLevel::Debug ? pImpl->levelDebug :
                                 entry.level == LogLevel::Info ? pImpl->levelInfo :
                                 entry.level == LogLevel::Warning ? pImpl->levelWarning :
                                 entry.level == LogLevel::Error ? pImpl->levelError :
                                 pImpl->levelCritical;
    
    // Format timestamp
    std::string timestamp = pImpl->timestampGen.formatTimestamp(entry.timestamp);
    
    // Format message
    std::string output = pImpl->messageFormatter.format(levelStr, timestamp, entry.message);
    
    // Console output with colors
    if (pImpl->useColors) {
        std::cout << getColorCode(entry.level) << output << getResetCode() << "\n";
    } else {
        std::cout << output << "\n";
    }
    
    // File output with buffering
    if (pImpl->fileWriter.is_open()) {
        pImpl->fileWriter.write(output);
    }
}

void ConsoleLogger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->level = level;
}

LogLevel ConsoleLogger::getLevel() const {
    return pImpl->level;
}

void ConsoleLogger::setUseColors(bool useColors) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->useColors = useColors;
}

bool ConsoleLogger::getUseColors() const {
    return pImpl->useColors;
}

void ConsoleLogger::setOutputFile(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileWriter.open(filepath)) {
        pImpl->fileWriter.flush();
    }
}

void ConsoleLogger::closeOutputFile() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->fileWriter.close();
}

std::string ConsoleLogger::levelToString(LogLevel level) const {
    return logger_internal::getLevelString(level);
}

std::string ConsoleLogger::levelToColor(LogLevel level) const {
    if (pImpl->useColors) {
        return logger_internal::getColorCode(level);
    }
    return "";
}

struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    // Pre-allocate for common case to reduce reallocations
    bool optimizeForSingleThread = true;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {}

Logger::~Logger() = default;

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    // Check if logger already exists
    for (const auto& existing : pImpl->loggers) {
        if (existing == logger) {
            return;
        }
    }
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
    
    // Optimize for common case - global logger
    if (pImpl->loggers.size() == 1 && pImpl->optimizeForSingleThread) {
        pImpl->loggers[0]->log(level, message, file, line, function);
        return;
    }
    
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
    : level_(level), file_(file), line_(line), function_(function) {
}

LogStream::~LogStream() {
    Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
}

} // namespace nlm
