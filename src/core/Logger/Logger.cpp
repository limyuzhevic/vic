#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <chrono>
#include <ctime>
#include <vector>
#include <map>

namespace nlm {

struct FileLogger::Impl {
    LogLevel level = LogLevel::Info;
    std::mutex mutex;
    std::ofstream fileOut;
    std::string filepath;
    bool append = true;
    std::string format;
};

FileLogger::FileLogger() : pImpl(std::make_unique<Impl>()) {
    pImpl->format = "[%level] %timestamp %message";
}

FileLogger::FileLogger(const std::string& filepath, LogLevel level, bool append)
    : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
    pImpl->filepath = filepath;
    pImpl->append = append;
    pImpl->format = "[%level] %timestamp %message";
    open();
}

FileLogger::~FileLogger() {
    close();
}

void FileLogger::open() {
    if (!pImpl->filepath.empty()) {
        pImpl->fileOut.open(pImpl->filepath, pImpl->append ? std::ios::app : std::ios::trunc);
    }
}

void FileLogger::close() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

void FileLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    std::string message = entry.message;
    
    // Replace placeholders in format
    std::string format = pImpl->format;
    
    // Replace %level
    size_t pos = format.find("%level");
    while (pos != std::string::npos) {
        format.replace(pos, 7, levelToString(entry.level));
        pos = format.find("%level", pos + 1);
    }
    
    // Replace %timestamp
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    std::tm tm_time;
    localtime_s(&tm_time, &time);
    std::ostringstream time_oss;
    time_oss << std::put_time(&tm_time, "%Y-%m-%d %H:%M:%S");
    pos = format.find("%timestamp");
    while (pos != std::string::npos) {
        format.replace(pos, 11, time_oss.str());
        pos = format.find("%timestamp", pos + 1);
    }
    
    // Replace %message
    pos = format.find("%message");
    while (pos != std::string::npos) {
        format.replace(pos, 8, message);
        pos = format.find("%message", pos + 1);
    }
    
    // Replace %file
    pos = format.find("%file");
    while (pos != std::string::npos) {
        format.replace(pos, 6, entry.file);
        pos = format.find("%file", pos + 1);
    }
    
    // Replace %line
    pos = format.find("%line");
    while (pos != std::string::npos) {
        format.replace(pos, 6, std::to_string(entry.line));
        pos = format.find("%line", pos + 1);
    }
    
    // Replace %function
    pos = format.find("%function");
    while (pos != std::string::npos) {
        format.replace(pos, 9, entry.function);
        pos = format.find("%function", pos + 1);
    }
    
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut << format << "\n";
        pImpl->fileOut.flush();
    }
}

void FileLogger::setLevel(LogLevel level) {
    pImpl->level = level;
}

LogLevel FileLogger::getLevel() const {
    return pImpl->level;
}

void FileLogger::setFilepath(const std::string& filepath, bool append) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    close();
    pImpl->filepath = filepath;
    pImpl->append = append;
    open();
}

void FileLogger::setFormat(const std::string& format) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->format = format;
}

std::string FileLogger::levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Critical: return "CRIT";
        default: return "UNKNOWN";
    }
}

struct Logger::Impl {
    LogLevel level = LogLevel::Info;
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    bool asyncEnabled = false;
    size_t maxQueueSize = 10000;
    std::thread logThread;
    std::queue<LogEntry> logQueue;
    std::condition_variable queueCV;
    bool stopThread = false;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {
    // Start async logging thread if enabled
    if (pImpl->asyncEnabled) {
        pImpl->stopThread = false;
        pImpl->logThread = std::thread(&Logger::logWorker, this);
    }
}

Logger::~Logger() {
    if (pImpl->asyncEnabled && pImpl->logThread.joinable()) {
        {
            std::lock_guard<std::mutex> lock(pImpl->mutex);
            pImpl->stopThread = true;
        }
        pImpl->queueCV.notify_one();
        pImpl->logThread.join();
    }
}

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    if (logger) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        pImpl->loggers.push_back(logger);
    }
}

void Logger::clearLoggers() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.clear();
}

void Logger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->level = level;
    for (auto& logger : pImpl->loggers) {
        logger->setLevel(level);
    }
}

void Logger::log(LogLevel level, const std::string& message,
                 const std::string& file, int line, const std::string& function) {
    if (pImpl->asyncEnabled) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        pImpl->logQueue.push({level, message, file, line, function, std::chrono::system_clock::now()});
        pImpl->queueCV.notify_one();
    } else {
        doLog(level, message, file, line, function);
    }
}

void Logger::doLog(LogLevel level, const std::string& message,
                   const std::string& file, int line, const std::string& function) {
    if (!isEnabled(level)) {
        return;
    }
    
    LogEntry entry{level, message, file, line, function, std::chrono::system_clock::now()};
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        try {
            logger->log(entry);
        } catch (const std::exception& e) {
            std::cerr << "Logger error: " << e.what() << std::endl;
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
    return level >= pImpl->level;
}

Logger& Logger::getGlobal() {
    static Logger globalLogger;
    return globalLogger;
}

void Logger::setGlobal(std::shared_ptr<Logger> logger) {
    // This is a simplified implementation
    // In practice, you'd need to handle global state more carefully
}

void Logger::logWorker() {
    while (true) {
        std::unique_lock<std::mutex> lock(pImpl->mutex);
        pImpl->queueCV.wait(lock, [this]() {
            return !pImpl->logQueue.empty() || pImpl->stopThread;
        });
        
        if (pImpl->stopThread && pImpl->logQueue.empty()) {
            break;
        }
        
        if (!pImpl->logQueue.empty()) {
            LogEntry entry = std::move(pImpl->logQueue.front());
            pImpl->logQueue.pop();
            lock.unlock();
            doLog(entry.level, entry.message, entry.file, entry.line, entry.function);
            lock.lock();
        }
    }
}

LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function) {
}

LogStream::~LogStream() {
    if (!oss_.str().empty()) {
        Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
    }
}

std::string FileLogger::levelToString(LogLevel level) {
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