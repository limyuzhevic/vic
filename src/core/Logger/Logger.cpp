#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <thread>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <ctime>
#include <filesystem>

namespace nlm {

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    
    // Performance monitoring
    bool performanceMonitoringEnabled = false;
    LogPerformanceMetrics metrics;
    std::mutex metricsMutex;
    
    // Logging categories
    std::unordered_map<std::string, bool> categoryEnabled;
    std::mutex categoryMutex;
    
    // Operation timing
    std::vector<std::pair<std::string, std::chrono::steady_clock::time_point>> activeTimers;
    std::mutex timersMutex;
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {
    pImpl->categoryEnabled["default"] = true;
}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
    pImpl->categoryEnabled["default"] = true;
}

ConsoleLogger::~ConsoleLogger() = default;

void ConsoleLogger::log(const LogEntry& entry) {
    // Check category filter
    std::string category = "default";
    if (entry.message.find("[CATEGORY:") == 0) {
        size_t endPos = entry.message.find("]");
        if (endPos != std::string::npos && endPos > 10) {
            category = entry.message.substr(10, endPos - 10);
        }
    }
    
    {
        std::lock_guard<std::mutex> catLock(pImpl->categoryMutex);
        if (!pImpl->categoryEnabled[category] && category != "default") {
            return;
        }
    }
    
    if (entry.level < pImpl->level) {
        return;
    }
    
    // Performance monitoring
    std::chrono::steady_clock::time_point startTime;
    if (pImpl->performanceMonitoringEnabled) {
        startTime = std::chrono::steady_clock::now();
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    
    // Add timestamp
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    
    // Add operation name if in timer
    if (pImpl->performanceMonitoringEnabled) {
        std::lock_guard<std::mutex> timerLock(pImpl->timersMutex);
        for (auto& timer : pImpl->activeTimers) {
            if (timer.second != std::chrono::steady_clock::time_point()) {
                oss << " [" << timer.first << "]";
            }
        }
    }
    
    oss << " " << entry.message;
    
    std::string output = oss.str();
    
    // Console output
    if (pImpl->useColors) {
        std::cout << levelToColor(entry.level) << output << "\033[0m\n";
    } else {
        std::cout << output << "\n";
    }
    
    // Performance metrics
    if (pImpl->performanceMonitoringEnabled) {
        auto endTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        
        std::lock_guard<std::mutex> metricsLock(pImpl->metricsMutex);
        
        // Update count
        switch (entry.level) {
            case LogLevel::Debug: pImpl->metrics.debugCount++; break;
            case LogLevel::Info: pImpl->metrics.infoCount++; break;
            case LogLevel::Warning: pImpl->metrics.warningCount++; break;
            case LogLevel::Error: pImpl->metrics.errorCount++; break;
            case LogLevel::Critical: pImpl->metrics.criticalCount++; break;
        }
        
        // Update latency
        double currentLatency = static_cast<double>(duration.count()) / 1000.0; // Convert to ms
        if (pImpl->metrics.averageLatency == 0.0) {
            pImpl->metrics.averageLatency = currentLatency;
        } else {
            pImpl->metrics.averageLatency = (pImpl->metrics.averageLatency + currentLatency) / 2.0;
        }
        
        // Update bytes written (approximate)
        pImpl->metrics.totalBytesWritten += static_cast<size_t>(output.size() + 1); // +1 for newline
        pImpl->metrics.lastUpdate = std::chrono::steady_clock::now();
        
        // Clean up active timers
        std::lock_guard<std::mutex> timerLock(pImpl->timersMutex);
        for (auto& timer : pImpl->activeTimers) {
            if (timer.second != std::chrono::steady_clock::time_point()) {
                auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - timer.second);
                timer.second = std::chrono::steady_clock::time_point();
            }
        }
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

// Performance monitoring implementation
bool ConsoleLogger::isPerformanceMonitoringEnabled() const {
    return pImpl->performanceMonitoringEnabled;
}

void ConsoleLogger::enablePerformanceMonitoring(bool enable) {
    pImpl->performanceMonitoringEnabled = enable;
    if (enable) {
        pImpl->metrics.lastUpdate = std::chrono::steady_clock::now();
    }
}

LogPerformanceMetrics ConsoleLogger::getPerformanceMetrics() const {
    return pImpl->metrics;
}

void ConsoleLogger::resetPerformanceMetrics() {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    pImpl->metrics = LogPerformanceMetrics{};
}

std::unordered_map<std::string, double> ConsoleLogger::getPerformanceAnalysis() const {
    std::unordered_map<std::string, double> analysis;
    
    if (!pImpl->performanceMonitoringEnabled) {
        return analysis;
    }
    
    analysis["debug_count"] = static_cast<double>(pImpl->metrics.debugCount);
    analysis["info_count"] = static_cast<double>(pImpl->metrics.infoCount);
    analysis["warning_count"] = static_cast<double>(pImpl->metrics.warningCount);
    analysis["error_count"] = static_cast<double>(pImpl->metrics.errorCount);
    analysis["critical_count"] = static_cast<double>(pImpl->metrics.criticalCount);
    analysis["average_latency_ms"] = pImpl->metrics.averageLatency;
    analysis["total_bytes_written"] = static_cast<double>(pImpl->metrics.totalBytesWritten);
    
    // Calculate throughput (logs per second)
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - pImpl->metrics.lastUpdate).count();
    if (elapsed > 0) {
        double totalLogs = static_cast<double>(pImpl->metrics.debugCount + pImpl->metrics.infoCount +
                                              pImpl->metrics.warningCount + pImpl->metrics.errorCount +
                                              pImpl->metrics.criticalCount);
        analysis["logs_per_second"] = totalLogs / elapsed;
    }
    
    return analysis;
}

bool ConsoleLogger::isCategoryEnabled(const std::string& category) const {
    std::lock_guard<std::mutex> lock(pImpl->categoryMutex);
    auto it = pImpl->categoryEnabled.find(category);
    return it != pImpl->categoryEnabled.end() && it->second;
}

void ConsoleLogger::enableCategory(const std::string& category, bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->categoryMutex);
    pImpl->categoryEnabled[category] = enable;
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

std::chrono::steady_clock::time_point ConsoleLogger::measureLogTime(std::chrono::steady_clock::time_point start) const {
    return start;
}

// Logger class implementation
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::shared_mutex mutex;
    
    // Global performance monitoring
    bool globalPerformanceMonitoring = false;
    LogPerformanceMetrics globalMetrics;
    std::mutex globalMetricsMutex;
    
    // Global logging categories
    std::unordered_map<std::string, bool> globalCategoryEnabled;
    std::mutex globalCategoryMutex;
    
    Impl() {
        globalCategoryEnabled["default"] = true;
    }
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {}

Logger::~Logger() = default;

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    std::lock_guard<std::shared_mutex> lock(pImpl->mutex);
    pImpl->loggers.push_back(logger);
}

void Logger::clearLoggers() {
    std::lock_guard<std::shared_mutex> lock(pImpl->mutex);
    pImpl->loggers.clear();
}

void Logger::setLevel(LogLevel level) {
    std::lock_guard<std::shared_mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        logger->setLevel(level);
    }
}

// Performance monitoring implementation
void Logger::enablePerformanceMonitoring(bool enable) {
    pImpl->globalPerformanceMonitoring = enable;
    for (auto& logger : pImpl->loggers) {
        logger->enablePerformanceMonitoring(enable);
    }
    if (enable) {
        std::lock_guard<std::mutex> lock(pImpl->globalMetricsMutex);
        pImpl->globalMetrics.lastUpdate = std::chrono::steady_clock::now();
    }
}

bool Logger::isPerformanceMonitoringEnabled() const {
    return pImpl->globalPerformanceMonitoring;
}

LogPerformanceMetrics Logger::getGlobalPerformanceMetrics() const {
    std::lock_guard<std::mutex> lock(pImpl->globalMetricsMutex);
    return pImpl->globalMetrics;
}

void Logger::resetGlobalPerformanceMetrics() {
    std::lock_guard<std::mutex> lock(pImpl->globalMetricsMutex);
    pImpl->globalMetrics = LogPerformanceMetrics{};
}

std::unordered_map<std::string, double> Logger::getGlobalPerformanceAnalysis() const {
    std::unordered_map<std::string, double> analysis;
    
    if (!pImpl->globalPerformanceMonitoring) {
        return analysis;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->globalMetricsMutex);
    analysis["debug_count"] = static_cast<double>(pImpl->globalMetrics.debugCount);
    analysis["info_count"] = static_cast<double>(pImpl->globalMetrics.infoCount);
    analysis["warning_count"] = static_cast<double>(pImpl->globalMetrics.warningCount);
    analysis["error_count"] = static_cast<double>(pImpl->globalMetrics.errorCount);
    analysis["critical_count"] = static_cast<double>(pImpl->globalMetrics.criticalCount);
    analysis["average_latency_ms"] = pImpl->globalMetrics.averageLatency;
    analysis["total_bytes_written"] = static_cast<double>(pImpl->globalMetrics.totalBytesWritten);
    
    // Calculate throughput
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - pImpl->globalMetrics.lastUpdate).count();
    if (elapsed > 0) {
        double totalLogs = static_cast<double>(pImpl->globalMetrics.debugCount + pImpl->globalMetrics.infoCount +
                                              pImpl->globalMetrics.warningCount + pImpl->globalMetrics.errorCount +
                                              pImpl->globalMetrics.criticalCount);
        analysis["logs_per_second"] = totalLogs / elapsed;
    }
    
    return analysis;
}

// Logging category support implementation
void Logger::enableCategory(const std::string& category, bool enable) {
    {
        std::lock_guard<std::mutex> lock(pImpl->globalCategoryMutex);
        pImpl->globalCategoryEnabled[category] = enable;
    }
    
    std::shared_lock<std::shared_mutex> loggersLock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        logger->enableCategory(category, enable);
    }
}

bool Logger::isCategoryEnabled(const std::string& category) const {
    std::lock_guard<std::mutex> lock(pImpl->globalCategoryMutex);
    auto it = pImpl->globalCategoryEnabled.find(category);
    return it != pImpl->globalCategoryEnabled.end() && it->second;
}

void Logger::log(LogLevel level, const std::string& message,
                 const std::string& file, int line, const std::string& function) {
    // Check category filter
    std::string category = "default";
    if (message.find("[CATEGORY:") == 0) {
        size_t endPos = message.find("]");
        if (endPos != std::string::npos && endPos > 10) {
            category = message.substr(10, endPos - 10);
        }
    }
    
    if (!isCategoryEnabled(category)) {
        return;
    }
    
    std::shared_lock<std::shared_mutex> loggersLock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        if (logger->getLevel() <= level) {
            LogEntry entry;
            entry.level = level;
            entry.message = message;
            entry.file = file;
            entry.line = line;
            entry.function = function;
            entry.timestamp = std::chrono::system_clock::now();
            
            logger->log(entry);
            
            // Update global metrics
            if (pImpl->globalPerformanceMonitoring) {
                std::lock_guard<std::mutex> metricsLock(pImpl->globalMetricsMutex);
                
                switch (level) {
                    case LogLevel::Debug: pImpl->globalMetrics.debugCount++; break;
                    case LogLevel::Info: pImpl->globalMetrics.infoCount++; break;
                    case LogLevel::Warning: pImpl->globalMetrics.warningCount++; break;
                    case LogLevel::Error: pImpl->globalMetrics.errorCount++; break;
                    case LogLevel::Critical: pImpl->globalMetrics.criticalCount++; break;
                }
                
                pImpl->globalMetrics.lastUpdate = std::chrono::steady_clock::now();
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
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
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

// PerformanceTimer implementation
PerformanceTimer::PerformanceTimer(const std::string& operationName) 
    : operationName(operationName) {
    startTime = std::chrono::steady_clock::now();
}

PerformanceTimer::~PerformanceTimer() = default;

void PerformanceTimer::setOperationName(const std::string& name) {
    operationName = name;
}

const std::string& PerformanceTimer::getOperationName() const {
    return operationName;
}

long long PerformanceTimer::getDurationMicroseconds() const {
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    return duration.count();
}

double PerformanceTimer::getDurationSeconds() const {
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime);
    return duration.count();
}

void PerformanceTimer::reset() {
    startTime = std::chrono::steady_clock::now();
}

} // namespace nlm