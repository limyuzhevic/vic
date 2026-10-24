#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>

namespace nlm {

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
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
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    
    oss << " " << entry.message;
    
    std::string output = oss.str();
    
    // Console output
    if (pImpl->useColors) {
        std::cout << levelToColor(entry.level) << output << "\033[0m\n";
    } else {
        std::cout << output << "\n";
    }
    
    // File output if set
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut << output << "\n";
        pImpl->fileOut.flush();
    }
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
    pImpl->fileOut.open(filepath, std::ios::app);
}

void ConsoleLogger::closeOutputFile() {
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

#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <chrono>
#include <map>
#include <algorithm>

namespace nlm {

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    
    // Performance monitoring
    struct PerformanceStats {
        double total_time_ms = 0.0;
        size_t log_count = 0;
        std::chrono::system_clock::time_point start_time;
        std::map<std::string, double> operation_times;
    };
    
    PerformanceStats stats;
    bool performance_tracking_enabled = true;
    size_t max_log_entries = 10000;
    std::string performance_log_file = "performance.log";
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {
    pImpl->stats.start_time = std::chrono::high_resolution_clock::now();
}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
    pImpl->stats.start_time = std::chrono::high_resolution_clock::now();
}

ConsoleLogger::~ConsoleLogger() = default;

void ConsoleLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Track performance stats
    auto currentTime = std::chrono::high_resolution_clock::now();
    double elapsed_ms = std::chrono::duration<double, std::milli>(currentTime - pImpl->stats.start_time).count();
    pImpl->stats.total_time_ms = elapsed_ms;
    pImpl->stats.log_count++;
    
    // Log operation time
    std::string operation_key = entry.function.empty() ? "unknown" : entry.function;
    pImpl->stats.operation_times[operation_key] += elapsed_ms;
    
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    
    // Add timestamp with higher precision
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    auto fractional = std::chrono::duration_cast<std::chrono::microseconds>(
        entry.timestamp.time_since_epoch()) % 1000000;
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S")
        << "." << std::setfill('0') << std::setw(6) << fractional.count()
        << " " << entry.message;
    
    std::string output = oss.str();
    
    // Console output
    if (pImpl->useColors) {
        std::cout << levelToColor(entry.level) << output << "\033[0m\n";
    } else {
        std::cout << output << "\n";
    }
    
    // File output if set
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut << output << "\n";
        pImpl->fileOut.flush();
    }
    
    // Performance monitoring file output
    if (pImpl->performance_tracking_enabled) {
        std::ofstream perfFile(pImpl->performance_log_file, std::ios::app);
        if (perfFile.is_open()) {
            perfFile << output << "\n";
            perfFile.flush();
        }
    }
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
    pImpl->fileOut.open(filepath, std::ios::app);
}

void ConsoleLogger::closeOutputFile() {
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

// Performance monitoring methods
void ConsoleLogger::startPerformanceTracking() {
    pImpl->stats.start_time = std::chrono::high_resolution_clock::now();
    pImpl->stats.total_time_ms = 0.0;
    pImpl->stats.log_count = 0;
    pImpl->stats.operation_times.clear();
}

double ConsoleLogger::getTotalExecutionTime() const {
    auto currentTime = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(currentTime - pImpl->stats.start_time).count();
}

size_t ConsoleLogger::getLogCount() const {
    return pImpl->stats.log_count;
}

double ConsoleLogger::getAverageLogTime() const {
    return pImpl->stats.log_count > 0 ? pImpl->stats.total_time_ms / pImpl->stats.log_count : 0.0;
}

std::map<std::string, double> ConsoleLogger::getOperationTimes() const {
    return pImpl->stats.operation_times;
}

void ConsoleLogger::setPerformanceTrackingEnabled(bool enabled) {
    pImpl->performance_tracking_enabled = enabled;
}

bool ConsoleLogger::isPerformanceTrackingEnabled() const {
    return pImpl->performance_tracking_enabled;
}

void ConsoleLogger::setMaxLogEntries(size_t maxEntries) {
    pImpl->max_log_entries = maxEntries;
}

size_t ConsoleLogger::getMaxLogEntries() const {
    return pImpl->max_log_entries;
}

void ConsoleLogger::setPerformanceLogFile(const std::string& filepath) {
    pImpl->performance_log_file = filepath;
}

std::string ConsoleLogger::getPerformanceLogFile() const {
    return pImpl->performance_log_file;
}

void ConsoleLogger::generatePerformanceReport() const {
    std::cout << "=== Performance Report ===" << std::endl;
    std::cout << "Total execution time: " << pImpl->stats.total_time_ms << " ms" << std::endl;
    std::cout << "Total log entries: " << pImpl->stats.log_count << std::endl;
    std::cout << "Average log time: " << getAverageLogTime() << " ms" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Operation times:" << std::endl;
    for (const auto& [operation, time] : pImpl->stats.operation_times) {
        std::cout << "  " << operation << ": " << time << " ms" << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "=== End Performance Report ===" << std::endl;
}

void ConsoleLogger::resetPerformanceStats() {
    startPerformanceTracking();
}

// Logger class implementation
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {
    // Create default performance monitoring logger
    auto performanceLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    performanceLogger->startPerformanceTracking();
    addLogger(performanceLogger);
}

Logger::~Logger() = default;

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
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
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
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
    : level_(level), file_(file), line_(line), function_(function) {}

LogStream::~LogStream() {
    Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
}

} // namespace nlm
