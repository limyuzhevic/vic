#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <thread>
#include <atomic>
#include <filesystem>

namespace nlm {

// TextLogFormatter implementation
TextLogFormatter::TextLogFormatter() : formatPattern("[%(level)s] %(thread)s: %(message)s"), useColors(true) {}

TextLogFormatter::TextLogFormatter(const std::string& format) : formatPattern(format), useColors(true) {}

std::string TextLogFormatter::format(const LogEntry& entry) {
    std::string result = formatPattern;
    
    // Replace placeholders
    size_t pos = 0;
    while ((pos = result.find("%(", pos)) != std::string::npos) {
        size_t end = result.find(")", pos);
        if (end == std::string::npos) break;
        
        std::string placeholder = result.substr(pos + 2, end - pos - 2);
        std::string value;
        
        if (placeholder == "level") {
            value = levelToString(entry.level);
        } else if (placeholder == "message") {
            value = entry.message;
        } else if (placeholder == "thread") {
            value = entry.threadName.empty() ? "unknown" : entry.threadName;
        } else if (placeholder == "file") {
            value = entry.file;
        } else if (placeholder == "line") {
            value = std::to_string(entry.line);
        } else if (placeholder == "function") {
            value = entry.function;
        } else if (placeholder == "timestamp") {
            value = formatTimestamp(entry);
        }
        
        result.replace(pos, end - pos + 1, value);
        pos += value.length();
    }
    
    return result;
}

void TextLogFormatter::setFormat(const std::string& format) {
    formatPattern = format;
}

std::string TextLogFormatter::formatTimestamp(const LogEntry& entry) const {
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string TextLogFormatter::formatLevel(const LogEntry& entry) const {
    return levelToString(entry.level);
}

std::string TextLogFormatter::formatThread(const LogEntry& entry) const {
    return entry.threadName.empty() ? "unknown" : entry.threadName;
}

std::string TextLogFormatter::formatLocation(const LogEntry& entry) const {
    if (entry.file.empty() && entry.line == 0) return "";
    std::stringstream ss;
    ss << entry.file << ":" << entry.line;
    if (!entry.function.empty()) ss << " (" << entry.function << ")";
    return ss.str();
}

// ConsoleLogger implementation
struct ConsoleLogger::Impl {
    LogLevel level;
    bool useColors;
    std::ofstream fileOut;
    std::mutex mutex;
    
    // Statistics
    std::atomic<size_t> logCounts[5]; // Debug, Info, Warning, Error, Critical
    
    // Filtering
    std::vector<std::function<bool(const LogEntry&)>> filters;
    
    // Formatter
    std::unique_ptr<LogFormatter> formatter;
    
    Impl() : level(LogLevel::Info), useColors(true), formatter(std::make_unique<TextLogFormatter>()) {
        for (int i = 0; i < 5; ++i) {
            logCounts[i] = 0;
        }
    }
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
}

ConsoleLogger::~ConsoleLogger() = default;

void ConsoleLogger::log(const LogEntry& entry) {
    if (!shouldLog(entry)) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Update statistics
    int levelIndex = static_cast<int>(entry.level);
    if (levelIndex >= 0 && levelIndex < 5) {
        pImpl->logCounts[levelIndex]++;
    }
    
    // Apply filters
    for (const auto& filter : pImpl->filters) {
        if (!filter(entry)) {
            return;
        }
    }
    
    std::string formattedMessage;
    if (pImpl->formatter) {
        formattedMessage = pImpl->formatter->format(entry);
    } else {
        formattedMessage = entry.message;
    }
    
    // Console output
    if (pImpl->useColors) {
        std::cout << levelToColor(entry.level) << formattedMessage << "\033[0m\n";
    } else {
        std::cout << formattedMessage << "\n";
    }
    
    // File output if set
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut << formattedMessage << "\n";
        pImpl->fileOut.flush();
    }
}

void ConsoleLogger::setLevel(LogLevel level) {
    pImpl->level = level;
}

LogLevel ConsoleLogger::getLevel() const {
    return pImpl->level;
}

void ConsoleLogger::setFormatter(std::unique_ptr<LogFormatter> formatter) {
    pImpl->formatter = std::move(formatter);
}

void ConsoleLogger::setMinLevel(LogLevel level) {
    pImpl->level = level;
}

LogLevel ConsoleLogger::getMinLevel() const {
    return pImpl->level;
}

void ConsoleLogger::logAsync(const LogEntry& entry) {
    // For now, just log synchronously (can be enhanced with thread pool)
    log(entry);
}

void ConsoleLogger::setUseColors(bool useColors) {
    pImpl->useColors = useColors;
}

bool ConsoleLogger::getUseColors() const {
    return pImpl->useColors;
}

void ConsoleLogger::setOutputFile(const std::string& filepath) {
    pImpl->fileOut.open(filepath, std::ios::app);
}

void ConsoleLogger::closeOutputFile() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

size_t ConsoleLogger::getLogCount(LogLevel level) const {
    int levelIndex = static_cast<int>(level);
    if (levelIndex >= 0 && levelIndex < 5) {
        return pImpl->logCounts[levelIndex];
    }
    return 0;
}

void ConsoleLogger::resetLogCount() {
    for (int i = 0; i < 5; ++i) {
        pImpl->logCounts[i] = 0;
    }
}

void ConsoleLogger::addFilter(std::function<bool(const LogEntry&)> filter) {
    pImpl->filters.push_back(std::move(filter));
}

void ConsoleLogger::removeFilter(std::function<bool(const LogEntry&)> filter) {
    pImpl->filters.erase(std::remove(pImpl->filters.begin(), pImpl->filters.end(), filter), pImpl->filters.end());
}

bool ConsoleLogger::shouldLog(const LogEntry& entry) const {
    if (entry.level < pImpl->level) {
        return false;
    }
    return true;
}

// FileLogger implementation
struct FileLogger::Impl {
    std::string filepath;
    std::ofstream fileOut;
    LogLevel level;
    
    // Formatting
    std::unique_ptr<LogFormatter> formatter;
    
    // Rotation and size limits
    size_t maxFileSize;
    size_t maxFiles;
    
    // Statistics
    std::atomic<size_t> logCounts[5];
    
    Impl(const std::string& filepath) 
        : filepath(filepath), level(LogLevel::Info), maxFileSize(10 * 1024 * 1024), // 10MB
          maxFiles(5), formatter(std::make_unique<TextLogFormatter>()) {
        for (int i = 0; i < 5; ++i) {
            logCounts[i] = 0;
        }
        
        // Ensure directory exists
        std::filesystem::path dir = std::filesystem::path(filepath).parent_path();
        if (!dir.empty()) {
            std::filesystem::create_directories(dir);
        }
        
        rotateFiles();
    }
};

FileLogger::FileLogger(const std::string& filepath) : pImpl(std::make_unique<Impl>(filepath)) {}

FileLogger::~FileLogger() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

void FileLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex); // mutex from Impl
    
    // Check file rotation
    if (pImpl->fileOut.tellp() > pImpl->maxFileSize) {
        rotateFiles();
    }
    
    // Update statistics
    int levelIndex = static_cast<int>(entry.level);
    if (levelIndex >= 0 && levelIndex < 5) {
        pImpl->logCounts[levelIndex]++;
    }
    
    std::string formattedMessage;
    if (pImpl->formatter) {
        formattedMessage = pImpl->formatter->format(entry);
    } else {
        formattedMessage = entry.message;
    }
    
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut << formattedMessage << "\n";
        pImpl->fileOut.flush();
    }
}

void FileLogger::setLevel(LogLevel level) {
    pImpl->level = level;
}

LogLevel FileLogger::getLevel() const {
    return pImpl->level;
}

void FileLogger::setFormatter(std::unique_ptr<LogFormatter> formatter) {
    pImpl->formatter = std::move(formatter);
}

void FileLogger::setMinLevel(LogLevel level) {
    pImpl->level = level;
}

LogLevel FileLogger::getMinLevel() const {
    return pImpl->level;
}

void FileLogger::logAsync(const LogEntry& entry) {
    // For now, just log synchronously
    log(entry);
}

void FileLogger::setMaxFileSize(size_t maxSize) {
    pImpl->maxFileSize = maxSize;
}

void FileLogger::setMaxFiles(size_t maxFiles) {
    pImpl->maxFiles = maxFiles;
}

void FileLogger::rotateFiles() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
    
    std::string currentFile = pImpl->filepath;
    
    // Rotate existing files
    for (size_t i = pImpl->maxFiles - 1; i > 0; --i) {
        std::string oldFile = pImpl->filepath + "." + std::to_string(i - 1);
        if (std::filesystem::exists(oldFile)) {
            std::filesystem::rename(oldFile, pImpl->filepath + "." + std::to_string(i));
        }
    }
    
    // Rename current file
    if (std::filesystem::exists(currentFile)) {
        std::filesystem::rename(currentFile, currentFile + ".1");
    }
    
    // Reopen current file
    pImpl->fileOut.open(pImpl->filepath, std::ios::app);
}

size_t FileLogger::getLogCount(LogLevel level) const {
    int levelIndex = static_cast<int>(level);
    if (levelIndex >= 0 && levelIndex < 5) {
        return pImpl->logCounts[levelIndex];
    }
    return 0;
}

void FileLogger::resetLogCount() {
    for (int i = 0; i < 5; ++i) {
        pImpl->logCounts[i] = 0;
    }
}

// Logger implementation
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    
    // Global statistics
    std::atomic<size_t> globalLogCounts[5];
    LogLevel globalMinLevel;
    
    Impl() : globalMinLevel(LogLevel::Info) {
        for (int i = 0; i < 5; ++i) {
            globalLogCounts[i] = 0;
        }
    }
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {}

Logger::~Logger() = default;

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.push_back(logger);
}

void Logger::removeLogger(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (name.empty()) {
        // Remove last added logger
        if (!pImpl->loggers.empty()) {
            pImpl->loggers.pop_back();
        }
    } else {
        // Remove by name - this is simplified as loggers don't typically have names
        pImpl->loggers.erase(
            std::remove_if(pImpl->loggers.begin(), pImpl->loggers.end(),
                [name](const std::shared_ptr<ILogger>& logger) {
                    // For now, we can't easily identify loggers by name without adding support
                    return false;
                }),
            pImpl->loggers.end()
        );
    }
}

void Logger::clearLoggers() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.clear();
}

void Logger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->globalMinLevel = level;
    
    for (auto& logger : pImpl->loggers) {
        logger->setLevel(level);
    }
}

void Logger::log(LogLevel level, const std::string& message, 
                 const std::string& file, int line, 
                 const std::string& function, const std::string& thread) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Update global statistics
    int levelIndex = static_cast<int>(level);
    if (levelIndex >= 0 && levelIndex < 5) {
        pImpl->globalLogCounts[levelIndex]++;
    }
    
    LogEntry entry(level, message, file, line, function, thread);
    
    for (auto& logger : pImpl->loggers) {
        if (logger->getLevel() <= level) {
            logger->log(entry);
        }
    }
}

void Logger::debug(const std::string& message, const std::string& file, int line, 
                   const std::string& function, const std::string& thread) {
    log(LogLevel::Debug, message, file, line, function, thread);
}

void Logger::info(const std::string& message, const std::string& file, int line, 
                  const std::string& function, const std::string& thread) {
    log(LogLevel::Info, message, file, line, function, thread);
}

void Logger::warning(const std::string& message, const std::string& file, int line, 
                     const std::string& function, const std::string& thread) {
    log(LogLevel::Warning, message, file, line, function, thread);
}

void Logger::error(const std::string& message, const std::string& file, int line, 
                   const std::string& function, const std::string& thread) {
    log(LogLevel::Error, message, file, line, function, thread);
}

void Logger::critical(const std::string& message, const std::string& file, int line, 
                      const std::string& function, const std::string& thread) {
    log(LogLevel::Critical, message, file, line, function, thread);
}

bool Logger::isEnabled(LogLevel level) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (level < pImpl->globalMinLevel) {
        return false;
    }
    
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
    (void)logger; // Suppress unused parameter warning
}

size_t Logger::getTotalLogCount(LogLevel level) {
    Logger& global = getGlobal();
    std::lock_guard<std::mutex> lock(global.pImpl->mutex);
    
    int levelIndex = static_cast<int>(level);
    if (levelIndex >= 0 && levelIndex < 5) {
        return global.pImpl->globalLogCounts[levelIndex];
    }
    return 0;
}

void Logger::resetTotalLogCount() {
    Logger& global = getGlobal();
    std::lock_guard<std::mutex> lock(global.pImpl->mutex);
    
    for (int i = 0; i < 5; ++i) {
        global.pImpl->globalLogCounts[i] = 0;
    }
}

void Logger::setGlobalLogLevel(LogLevel level) {
    Logger& global = getGlobal();
    global.setLevel(level);
}

LogLevel Logger::getGlobalLogLevel() {
    Logger& global = getGlobal();
    std::lock_guard<std::mutex> lock(global.pImpl->mutex);
    return global.pImpl->globalMinLevel;
}

void Logger::initializeDefaultLogger(const std::string& logFile, LogLevel level) {
    Logger& global = getGlobal();
    global.clearLoggers();
    
    auto consoleLogger = std::make_shared<ConsoleLogger>(level);
    consoleLogger->setLevel(level);
    consoleLogger->setUseColors(true);
    
    global.addLogger(consoleLogger);
    
    if (!logFile.empty()) {
        auto fileLogger = std::make_shared<FileLogger>(logFile);
        fileLogger->setLevel(level);
        global.addLogger(fileLogger);
    }
}

std::string Logger::logLevelToString(LogLevel level) {
    switch (level) {
        case LogLevel::Trace: return "TRACE";
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Notice: return "NOTICE";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Critical: return "CRIT";
        default: return "UNKNOWN";
    }
}

// LogStream implementation
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function) {}

LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function, const std::string& thread)
    : level_(level), file_(file), line_(line), function_(function), thread_(thread) {}

LogStream::~LogStream() {
    Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_, thread_);
}

} // namespace nlm
