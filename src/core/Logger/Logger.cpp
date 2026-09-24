#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <shared_mutex>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <algorithm>

namespace nlm {

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    size_t maxFileSize = 10 * 1024 * 1024; // 10MB
    std::string currentFile;
    
    // Thread-safe timestamp formatting
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        
        // Use thread-safe time functions
        std::tm tm_buf;
        std::mutex time_mutex;
        {
            std::lock_guard<std::mutex> lock(time_mutex);
            std::localtime_s(&tm_buf, &time_t);
        }
        
        std::stringstream ss;
        ss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
        
        // Add milliseconds
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;
        ss << "." << std::setfill('0') << std::setw(3) << ms.count();
        
        return ss.str();
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
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    
    // Add timestamp
    oss << pImpl->getCurrentTimestamp() << " ";
    
    oss << entry.message;
    
    std::string output = oss.str();
    
    // Console output
    if (pImpl->useColors) {
        std::cout << levelToColor(entry.level) << output << "\033[0m\n";
    } else {
        std::cout << output << "\n";
    }
    
    // File output with rotation
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut << output << "\n";
        pImpl->fileOut.flush();
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
    
    if (filepath.empty()) {
        if (pImpl->fileOut.is_open()) {
            pImpl->fileOut.close();
        }
        pImpl->currentFile.clear();
        return;
    }
    
    // Open file for appending
    pImpl->fileOut.open(filepath, std::ios::app);
    if (pImpl->fileOut.is_open()) {
        pImpl->currentFile = filepath;
        // Check if rotation is needed
        auto fileSize = std::filesystem::file_size(filepath);
        if (fileSize > pImpl->maxFileSize) {
            pImpl->fileOut.close();
            // Implement rotation logic here
        }
    }
}

void ConsoleLogger::closeOutputFile() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
        pImpl->currentFile.clear();
    }
}

void ConsoleLogger::setMaxFileSize(size_t maxBytes) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->maxFileSize = maxBytes;
}

size_t ConsoleLogger::getMaxFileSize() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->maxFileSize;
}

void ConsoleLogger::flush() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.flush();
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

std::string ConsoleLogger::formatTimestamp(const std::chrono::system_clock::time_point& timestamp) const {
    auto time_t = std::chrono::system_clock::to_time_t(timestamp);
    std::tm tm_buf;
    std::mutex time_mutex;
    {
        std::lock_guard<std::mutex> lock(time_mutex);
        std::localtime_s(&tm_buf, &time_t);
    }
    
    std::stringstream ss;
    ss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
    
    // Add milliseconds
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        timestamp.time_since_epoch()) % 1000;
    ss << "." << std::setfill('0') << std::setw(3) << ms.count();
    
    return ss.str();
}

// FileLogger implementation
struct FileLogger::Impl {
    LogLevel level = LogLevel::Info;
    std::ofstream fileOut;
    std::mutex mutex;
    std::string filepath;
    size_t maxFileSize = 10 * 1024 * 1024; // 10MB
    size_t rotationCount = 5;
    size_t currentRotation = 0;
    
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        
        std::tm tm_buf;
        std::mutex time_mutex;
        {
            std::lock_guard<std::mutex> lock(time_mutex);
            std::localtime_s(&tm_buf, &time_t);
        }
        
        std::stringstream ss;
        ss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
        
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;
        ss << "." << std::setfill('0') << std::setw(3) << ms.count();
        
        return ss.str();
    }
};

FileLogger::FileLogger() : pImpl(std::make_unique<Impl>()) {}

FileLogger::FileLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
}

FileLogger::~FileLogger() = default;

void FileLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    oss << pImpl->getCurrentTimestamp() << " ";
    oss << entry.message << "\n";
    
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut << oss.str();
        pImpl->fileOut.flush();
    }
}

void FileLogger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->level = level;
}

LogLevel FileLogger::getLevel() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->level;
}

void FileLogger::setOutputFile(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Close current file if open
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
    
    if (filepath.empty()) {
        pImpl->filepath.clear();
        return;
    }
    
    // Implement rotation logic
    std::string rotatedPath = filepath;
    if (pImpl->currentRotation > 0) {
        rotatedPath = filepath + "." + std::to_string(pImpl->currentRotation);
    }
    
    pImpl->fileOut.open(rotatedPath, std::ios::app);
    if (pImpl->fileOut.is_open()) {
        pImpl->filepath = filepath;
        pImpl->currentRotation = (pImpl->currentRotation + 1) % pImpl->rotationCount;
    }
}

void FileLogger::setMaxFileSize(size_t maxBytes) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->maxFileSize = maxBytes;
}

void FileLogger::setFileRotationCount(size_t rotationCount) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->rotationCount = rotationCount;
}

void FileLogger::flush() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.flush();
    }
}

// BufferedLogger implementation
struct BufferedLogger::Impl {
    LogLevel level = LogLevel::Info;
    std::mutex mutex;
    size_t bufferSize = 1000;
    std::chrono::milliseconds flushInterval{100}; // 100ms
    std::vector<LogEntry> buffer;
    std::thread flushThread;
    std::atomic<bool> running{false};
    std::condition_variable cv;
    std::mutex cv_mutex;
    
    ~Impl() {
        running = false;
        cv.notify_all();
        if (flushThread.joinable()) {
            flushThread.join();
        }
    }
    
    void startFlushThread() {
        running = true;
        flushThread = std::thread(&BufferedLogger::Impl::flushWorker, this);
    }
    
    void flushWorker() {
        while (running) {
            std::unique_lock<std::mutex> lock(cv_mutex);
            if (cv.wait_for(lock, flushInterval, [this]() { return !running || !buffer.empty(); })) {
                if (!running) break;
                flush();
            }
        }
    }
    
    void flush() {
        if (buffer.empty()) return;
        
        // Process buffered entries
        for (const auto& entry : buffer) {
            // Send to all registered loggers (in main Logger class)
            // This would need a callback mechanism
        }
        buffer.clear();
    }
};

BufferedLogger::BufferedLogger() : pImpl(std::make_unique<Impl>()) {
    pImpl->startFlushThread();
}

BufferedLogger::BufferedLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
    pImpl->startFlushThread();
}

BufferedLogger::~BufferedLogger() = default;

void BufferedLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    buffer.push_back(entry);
    
    if (buffer.size() >= pImpl->bufferSize) {
        pImpl->flush();
    }
}

void BufferedLogger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->level = level;
}

LogLevel BufferedLogger::getLevel() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->level;
}

void BufferedLogger::setBufferSize(size_t bufferSize) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->bufferSize = bufferSize;
}

void BufferedLogger::setFlushInterval(std::chrono::milliseconds interval) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->flushInterval = interval;
}

void BufferedLogger::flush() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->flush();
}

// Main Logger implementation
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogPolicy>> loggers;
    std::mutex mutex;
    std::atomic<size_t> totalLogCount{0};
    std::chrono::system_clock::time_point startTime;
    
    Impl() : startTime(std::chrono::system_clock::now()) {}
    
    void incrementLogCount() {
        totalLogCount++;
    }
    
    size_t getLogCount() const {
        return totalLogCount;
    }
    
    std::chrono::duration<double> getUptime() const {
        return std::chrono::system_clock::now() - startTime;
    }
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {}

Logger::~Logger() = default;

void Logger::addLogger(std::shared_ptr<ILogPolicy> logger) {
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
                 const std::string& file, int line, const std::string& function) {
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->incrementLogCount();
    
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

void Logger::flushAll() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        if (auto consoleLogger = std::dynamic_pointer_cast<ConsoleLogger>(logger)) {
            consoleLogger->flush();
        } else if (auto fileLogger = std::dynamic_pointer_cast<FileLogger>(logger)) {
            fileLogger->flush();
        } else if (auto bufferedLogger = std::dynamic_pointer_cast<BufferedLogger>(logger)) {
            bufferedLogger->flush();
        }
    }
}

size_t Logger::getLoggerCount() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->loggers.size();
}

std::string Logger::getStatistics() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "Logger Statistics:\n";
    oss << "  Total loggers: " << pImpl->loggers.size() << "\n";
    oss << "  Total log entries: " << pImpl->getLogCount() << "\n";
    oss << "  Uptime: " << pImpl->getUptime().count() << " seconds\n";
    
    // Count by level (would need tracking in ILogPolicy interface)
    oss << "  Log level distribution: [Not implemented]\n";
    
    return oss.str();
}

// LogStream implementation
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function) {}

LogStream::~LogStream() {
    Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
}

LogStream& LogStream::operator<<(const char* str) {
    oss_ << str;
    return *this;
}

LogStream& LogStream::operator<<(const std::string& str) {
    oss_ << str;
    return *this;
}

} // namespace nlm
