#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <chrono>
#include <filesystem>
#include <stdexcept>

namespace nlm {

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    bool threadSafetyEnabled = true;
    size_t maxFileSize = 10 * 1024 * 1024;  // 10 MB default
    size_t currentFileSize = 0;
    std::ofstream fileOut;
    std::mutex mutex;
    std::atomic<bool> isHealthy{true};
    std::string filepath;
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
}

ConsoleLogger::~ConsoleLogger() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

void ConsoleLogger::validateEntry(const LogEntry& entry) const {
    if (!entry.message.empty() && entry.message.size() > 10000) {
        throw LoggerException("Log message too long: " + std::to_string(entry.message.size()) + " characters");
    }
}

void ConsoleLogger::writeToFile(const std::string& message) {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut << message << "\n";
        pImpl->currentFileSize += message.size() + 1;  // +1 for newline
        
        // Check file size and rotate if needed
        if (pImpl->currentFileSize > pImpl->maxFileSize) {
            pImpl->fileOut.close();
            pImpl->fileOut.open(pImpl->filepath, std::ios::app);
            if (!pImpl->fileOut.is_open()) {
                NLM_LOG_ERROR("Failed to reopen log file after size limit");
            }
            pImpl->currentFileSize = 0;
        }
    }
}

void ConsoleLogger::log(const LogEntry& entry) {
    if (!isHealthy()) {
        return;  // Silently drop logs if logger is unhealthy
    }
    
    try {
        validateEntry(entry);
        
        if (entry.level < pImpl->level) {
            return;
        }
        
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        std::ostringstream oss;
        oss << "[" << levelToString(entry.level) << "] ";
        
        // Add timestamp
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        
        // Add file info if provided
        if (!entry.file.empty()) {
            oss << " " << entry.file;
            if (entry.line > 0) {
                oss << ":" << entry.line;
            }
            if (!entry.function.empty()) {
                oss << " " << entry.function;
            }
            oss << " - ";
        }
        
        oss << entry.message;
        
        std::string output = oss.str();
        
        // Console output
        if (pImpl->useColors) {
            std::cout << levelToColor(entry.level) << output << "\033[0m\n";
        } else {
            std::cout << output << "\n";
        }
        
        std::cout.flush();
        
        // File output if set
        if (pImpl->fileOut.is_open()) {
            writeToFile(output);
            pImpl->fileOut.flush();
        }
        
    } catch (const std::exception& e) {
        // Log error to stderr but don't throw to avoid recursion
        std::cerr << "[Logger ERROR] Failed to log message: " << e.what() << std::endl;
        pImpl->isHealthy.store(false);
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

void ConsoleLogger::flush() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.flush();
    }
}

bool ConsoleLogger::isHealthy() const {
    return pImpl->isHealthy.load();
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
    
    // Close existing file if open
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
    
    // Open new file
    pImpl->fileOut.open(filepath, std::ios::app);
    if (!pImpl->fileOut.is_open()) {
        NLM_LOG_ERROR("Failed to open log file: " + filepath);
    } else {
        pImpl->filepath = filepath;
        // Try to get current file size
        auto fileSize = std::filesystem::file_size(filepath);
        pImpl->currentFileSize = static_cast<size_t>(fileSize);
        NLM_LOG_INFO("Log file output enabled: " + filepath);
    }
}

void ConsoleLogger::closeOutputFile() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
        pImpl->filepath.clear();
        pImpl->currentFileSize = 0;
    }
}

void ConsoleLogger::setMaxFileSize(size_t maxBytes) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->maxFileSize = maxBytes;
}

size_t ConsoleLogger::getCurrentFileSize() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->currentFileSize;
}

void ConsoleLogger::setThreadSafetyEnabled(bool enabled) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->threadSafetyEnabled = enabled;
}

bool ConsoleLogger::isThreadSafetyEnabled() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->threadSafetyEnabled;
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

struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    std::atomic<size_t> totalLogs{0};
    std::atomic<size_t> totalErrors{0};
    std::atomic<size_t> totalDropped{0};
    std::chrono::system_clock::time_point startTime;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {
    pImpl->startTime = std::chrono::system_clock::now();
    
    // Add default console logger
    auto consoleLogger = std::make_shared<ConsoleLogger>();
    consoleLogger->setLevel(LogLevel::Info);
    addLogger(consoleLogger);
}

Logger::~Logger() {
    clearLoggers();
}

void Logger::validateMessage(const std::string& message) const {
    if (message.empty()) {
        throw LoggerException("Log message cannot be empty");
    }
    if (message.size() > 10000) {
        throw LoggerException("Log message too long: " + std::to_string(message.size()) + " characters");
    }
}

void Logger::updateStats(bool success) {
    pImpl->totalLogs++;
    if (!success) {
        pImpl->totalErrors++;
    }
}

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    if (!logger) {
        throw LoggerException("Cannot add null logger");
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    logger->setLevel(LogLevel::Info);  // Default level
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
    if (!isHealthy()) {
        pImpl->totalDropped++;
        return;  // Silently drop logs if logger is unhealthy
    }
    
    try {
        validateMessage(message);
        
        bool success = true;
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
            } catch (const std::exception& e) {
                // Log individual logger errors but continue
                std::cerr << "[Logger ERROR] Failed to log to one of the loggers: " << e.what() << std::endl;
                success = false;
            }
        }
        
        updateStats(success);
        
    } catch (const LoggerException& e) {
        std::cerr << "[Logger ERROR] " << e.what() << std::endl;
        updateStats(false);
    } catch (const std::exception& e) {
        std::cerr << "[Logger ERROR] Unexpected error logging message: " << e.what() << std::endl;
        updateStats(false);
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
    if (logger) {
        // This is a placeholder for advanced global logger management
        // In practice, we just use getGlobal() which is already static
    }
}

bool Logger::isHealthy() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (const auto& logger : pImpl->loggers) {
        if (!logger->isHealthy()) {
            return false;
        }
    }
    return !pImpl->loggers.empty();
}

void Logger::flushAll() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        try {
            logger->flush();
        } catch (const std::exception& e) {
            std::cerr << "[Logger ERROR] Failed to flush logger: " << e.what() << std::endl;
        }
    }
}

Logger::LoggerStats Logger::getStats() const {
    LoggerStats stats;
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    stats.totalLogs = pImpl->totalLogs.load();
    stats.totalErrors = pImpl->totalErrors.load();
    stats.totalDropped = pImpl->totalDropped.load();
    stats.startTime = pImpl->startTime;
    stats.healthyLoggers = std::count_if(pImpl->loggers.begin(), pImpl->loggers.end(),
                                        [](const auto& logger) { return logger->isHealthy(); });
    stats.totalLoggers = pImpl->loggers.size();
    
    return stats;
}

// LogStream implementation
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function), active_(true) {
    
    // Get global logger
    logger_ = std::make_shared<Logger>(&Logger::getGlobal());
    
    if (!logger_->isHealthy()) {
        active_ = false;
    }
}

LogStream::~LogStream() {
    if (active_ && logger_ && logger_->isHealthy()) {
        std::string message = oss_.str();
        if (!message.empty()) {
            logger_->log(level_, message, file_, line_, function_);
        }
    }
}

} // namespace nlm