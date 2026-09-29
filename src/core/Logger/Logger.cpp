#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <thread>
#include <chrono>
#include <ctime>
#include <unordered_map>
#include <sstream>

namespace nlm {

struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    LogLevel level = LogLevel::Info;
    std::string correlationId;
    std::string component = "Default";
    std::mutex mutex;
    uint64_t sequenceNumber = 0;
    
    struct PerformanceMetrics {
        uint64_t totalLogs = 0;
        uint64_t logsByLevel[6] = {0}; // Debug, Info, Warning, Error, Critical, Fatal
        std::chrono::system_clock::time_point startTime = std::chrono::system_clock::now();
    } perfMetrics;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {
    // Create default console logger
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    pImpl->loggers.push_back(consoleLogger);
}

Logger::~Logger() = default;

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
                 const std::string& file, int line,
                 const std::string& function,
                 const std::map<std::string, std::string>& metadata) {
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.correlationId = pImpl->correlationId;
    entry.component = pImpl->component;
    entry.metadata = metadata;
    entry.threadId = std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    entry.sequenceNumber = ++pImpl->sequenceNumber;
    
    logStructured(entry);
}

void Logger::debug(const std::string& message, const std::string& file, int line, 
                   const std::string& function, const std::map<std::string, std::string>& metadata) {
    log(LogLevel::Debug, message, file, line, function, metadata);
}

void Logger::info(const std::string& message, const std::string& file, int line,
                  const std::string& function, const std::map<std::string, std::string>& metadata) {
    log(LogLevel::Info, message, file, line, function, metadata);
}

void Logger::warning(const std::string& message, const std::string& file, int line,
                     const std::string& function, const std::map<std::string, std::string>& metadata) {
    log(LogLevel::Warning, message, file, line, function, metadata);
}

void Logger::error(const std::string& message, const std::string& file, int line,
                   const std::string& function, const std::map<std::string, std::string>& metadata) {
    log(LogLevel::Error, message, file, line, function, metadata);
}

void Logger::critical(const std::string& message, const std::string& file, int line,
                      const std::string& function, const std::map<std::string, std::string>& metadata) {
    log(LogLevel::Critical, message, file, line, function, metadata);
}

void Logger::fatal(const std::string& message, const std::string& file, int line,
                   const std::string& function, const std::map<std::string, std::string>& metadata) {
    log(LogLevel::Fatal, message, file, line, function, metadata);
}

bool Logger::isEnabled(LogLevel level) const {
    return level >= pImpl->level;
}

void Logger::logStructured(const LogEntry& entry) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Update performance metrics
    pImpl->perfMetrics.totalLogs++;
    pImpl->perfMetrics.logsByLevel[static_cast<int>(entry.level)]++;
    
    // Log to all registered loggers
    for (auto& logger : pImpl->loggers) {
        if (entry.level >= logger->getLevel()) {
            logger->log(entry);
        }
    }
}

void Logger::logJson(const json& entry) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (auto& logger : pImpl->loggers) {
        logger->logJson(entry);
    }
}

void Logger::setCorrelationId(const std::string& correlationId) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->correlationId = correlationId;
}

std::string Logger::getCorrelationId() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->correlationId;
}

void Logger::setComponent(const std::string& component) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->component = component;
}

std::string Logger::getComponent() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->component;
}

Logger::PerformanceMetrics Logger::getPerformanceMetrics() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    PerformanceMetrics metrics;
    metrics.totalLogs = pImpl->perfMetrics.totalLogs;
    std::copy(std::begin(pImpl->perfMetrics.logsByLevel),
              std::end(pImpl->perfMetrics.logsByLevel),
              std::begin(metrics.logsByLevel));
    
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - pImpl->perfMetrics.startTime);
    metrics.logsPerSecond = duration.count() > 0 ? 
        static_cast<double>(pImpl->perfMetrics.totalLogs) / duration.count() : 0.0;
    metrics.lastResetTime = pImpl->perfMetrics.startTime;
    
    return metrics;
}

void Logger::resetPerformanceMetrics() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->perfMetrics.totalLogs = 0;
    std::fill(std::begin(pImpl->perfMetrics.logsByLevel), std::end(pImpl->perfMetrics.logsByLevel), 0);
    pImpl->perfMetrics.startTime = std::chrono::system_clock::now();
}

} // namespace nlm
