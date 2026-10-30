#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <algorithm>
#include <chrono>
#include <thread>

namespace nlm {

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    bool useTimestamps = true;
    bool useFileInfo = true;
    LogFormat format = LogFormat::Standard;
    std::ofstream fileOut;
    std::string filepath;
    size_t maxFileSize = 10 * 1024 * 1024; // 10MB
    size_t maxFileCount = 5;
    std::mutex mutex;
    std::chrono::steady_clock::time_point lastRotation;
    std::atomic<size_t> currentFileIndex{0};
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
    
    std::ostringstream oss;
    
    // Format message according to configured format
    switch (pImpl->format) {
        case LogFormat::Standard:
            formatStandardMessage(oss, entry);
            break;
        case LogFormat::Detailed:
            formatDetailedMessage(oss, entry);
            break;
        case LogFormat::Compact:
            formatCompactMessage(oss, entry);
            break;
        case LogFormat::JSON:
            formatJSONMessage(oss, entry);
            break;
    }
    
    std::string output = oss.str();
    
    // Console output with colors
    if (pImpl->useColors) {
        output = levelToColor(entry.level) + output + "\033[0m";
    }
    
    std::cout << output << "\n";
    
    // File output if set and file rotation needed
    if (pImpl->fileOut.is_open()) {
        rotateIfNeeded();
        pImpl->fileOut << output << "\n";
        if (pImpl->maxFileSize > 0) {
            pImpl->fileOut.flush();
        }
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
    pImpl->useColors = useColors;
}

bool ConsoleLogger::getUseColors() const {
    return pImpl->useColors;
}

void ConsoleLogger::setOutputFile(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->filepath = filepath;
    pImpl->fileOut.close();
    pImpl->fileOut.open(filepath, std::ios::app);
    if (!pImpl->fileOut.is_open()) {
        NLM_LOG_ERROR("ConsoleLogger: Failed to open file: " + filepath);
    }
}

void ConsoleLogger::closeOutputFile() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->fileOut.close();
}

void ConsoleLogger::setFormat(LogFormat format) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->format = format;
}

LogFormat ConsoleLogger::getFormat() const {
    return pImpl->format;
}

void ConsoleLogger::setMaxFileSize(size_t maxSize) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->maxFileSize = maxSize;
}

void ConsoleLogger::setMaxFileCount(size_t maxCount) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->maxFileCount = maxCount;
}

void ConsoleLogger::rotateIfNeeded() {
    if (pImpl->maxFileSize > 0) {
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - pImpl->lastRotation).count();
        
        if (elapsed >= 60 || pImpl->fileOut.tellp() >= static_cast<std::streamsize>(pImpl->maxFileSize)) {
            pImpl->fileOut.close();
            std::string rotatedPath = pImpl->filepath + "." + std::to_string(pImpl->currentFileIndex++);
            pImpl->fileOut.open(rotatedPath, std::ios::trunc);
            pImpl->lastRotation = currentTime;
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

void ConsoleLogger::formatStandardMessage(std::ostringstream& oss, const LogEntry& entry) {
    if (pImpl->useTimestamps) {
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S") << " ";
    }
    
    oss << "[" << levelToString(entry.level) << "] ";
    
    if (pImpl->useFileInfo) {
        oss << "(" << entry.file << ":" << entry.line << ") ";
    }
    
    oss << entry.message;
}

void ConsoleLogger::formatDetailedMessage(std::ostringstream& oss, const LogEntry& entry) {
    formatStandardMessage(oss, entry);
    
    oss << " [" << entry.function << "]";
}

void ConsoleLogger::formatCompactMessage(std::ostringstream& oss, const LogEntry& entry) {
    if (pImpl->useTimestamps) {
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        oss << std::put_time(std::localtime(&time), "%H:%M:%S");
        oss << " ";
    }
    
    oss << "[" << levelToString(entry.level).substr(0, 1) << "] ";
    oss << entry.message;
}

void ConsoleLogger::formatJSONMessage(std::ostringstream& oss, const LogEntry& entry) {
    oss << "{";
    oss << "\"timestamp\": \"" << getTimestampString(entry.timestamp) << "\", ";
    oss << "\"level\": \"" << levelToString(entry.level) << "\", ";
    oss << "\"message\": \"" << escapeJSON(entry.message) << "\", ";
    
    if (pImpl->useFileInfo) {
        oss << "\"file\": \"" << escapeJSON(entry.file) << "\", ";
        oss << "\"line\": " << entry.line << ", ";
        oss << "\"function\": \"" << escapeJSON(entry.function) << "\"";
    }
    
    oss << "}";
}

std::string ConsoleLogger::getTimestampString(const std::chrono::system_clock::time_point& timestamp) {
    auto time = std::chrono::system_clock::to_time_t(timestamp);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%dT%H:%M:%S.%3fZ");
    return ss.str();
}

std::string ConsoleLogger::escapeJSON(const std::string& input) {
    std::string result;
    result.reserve(input.size() * 2);
    
    for (char c : input) {
        switch (c) {
            case '"': result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\b': result += "\\b"; break;
            case '\f': result += "\\f"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default:
                if (c < 32 || c > 126) {
                    result += "\\u" + getHexEscape(c);
                } else {
                    result += c;
                }
                break;
        }
    }
    
    return result;
}

std::string ConsoleLogger::getHexEscape(char c) {
    std::stringstream ss;
    ss << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(static_cast<unsigned char>(c));
    return ss.str();
}

// Logger class implementation
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    bool enableAsync = false;
    size_t asyncQueueSize = 1024;
    std::thread asyncThread;
    std::atomic<bool> stopAsyncThread;
    std::deque<LogEntry> asyncQueue;
    std::condition_variable asyncCondition;
    
    Impl() : stopAsyncThread(false) {
        if (enableAsync) {
            asyncThread = std::thread(&Logger::Impl::asyncLoggerThread, this);
        }
    }
    
    ~Impl() {
        if (enableAsync && asyncThread.joinable()) {
            stopAsyncThread = true;
            asyncCondition.notify_one();
            asyncThread.join();
        }
    }
    
    void asyncLoggerThread() {
        while (!stopAsyncThread) {
            std::unique_lock<std::mutex> lock(mutex);
            if (asyncQueue.empty()) {
                asyncCondition.wait_for(lock, std::chrono::milliseconds(100));
                continue;
            }
            
            LogEntry entry = asyncQueue.front();
            asyncQueue.pop_front();
            lock.unlock();
            
            // Process log entry
            for (auto& logger : loggers) {
                if (logger->getLevel() <= entry.level) {
                    logger->log(entry);
                }
            }
        }
    }
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {}

Logger::~Logger() = default;

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    if (!logger) return;
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
                 const std::string& file, int line, 
                 const std::string& function) {
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    
    if (pImpl->enableAsync) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        if (pImpl->asyncQueue.size() >= pImpl->asyncQueueSize) {
            // Drop oldest entry if queue is full
            pImpl->asyncQueue.pop_front();
        }
        pImpl->asyncQueue.push_back(entry);
        pImpl->asyncCondition.notify_one();
    } else {
        // Synchronous logging
        for (auto& logger : pImpl->loggers) {
            if (logger->getLevel() <= level) {
                logger->log(entry);
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
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (const auto& logger : pImpl->loggers) {
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

} // namespace nlm