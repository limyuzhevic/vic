#include "EnhancedLogger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <unordered_map>
#include <thread>
#include <chrono>
#include <ctime>

namespace nlm {

struct EnhancedConsoleLogger::Impl {
    EnhancedLogLevel level = EnhancedLogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    std::string timestampFormat = "%Y-%m-%d %H:%M:%S";
    bool enableStructuredLogging = false;
    std::vector<std::function<bool(const EnhancedLogEntry&)>> logFilters;
    std::unordered_map<std::string, std::string> formatTemplates;
};

EnhancedConsoleLogger::EnhancedConsoleLogger() : pImpl(std::make_unique<Impl>()) {}

EnhancedConsoleLogger::EnhancedConsoleLogger(EnhancedLogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
}

EnhancedConsoleLogger::~EnhancedConsoleLogger() = default;

void EnhancedConsoleLogger::log(const EnhancedLogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    // Apply filters
    for (const auto& filter : pImpl->logFilters) {
        if (!filter(entry)) {
            return;
        }
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    
    // Add timestamp
    oss << "[" << formatTimestamp(entry.timestamp) << "] ";
    
    // Add level
    oss << "[" << levelToString(entry.level) << "] ";
    
    // Add thread/process ID
    oss << "[T:" << entry.threadId << " P:" << entry.processId << "] ";
    
    // Add source location
    if (!entry.file.empty()) {
        oss << "[" << entry.file;
        if (entry.line > 0) oss << ":" << entry.line;
        oss << "] ";
    }
    
    // Add message
    oss << entry.message;
    
    // Add metadata if structured logging is enabled
    if (pImpl->enableStructuredLogging && !entry.metadata.empty()) {
        oss << " " << formatMetadata(entry.metadata);
    }
    
    // Add performance/memory info
    if (entry.isPerformanceMetric) {
        oss << " (perf: " << std::fixed << std::setprecision(4) << entry.performanceValue << ")";
    }
    if (entry.memoryUsage > 0) {
        oss << " (mem: " << entry.memoryUsage << " bytes)";
    }
    
    std::string output = oss.str();
    
    // Console output with colors
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

void EnhancedConsoleLogger::setLevel(EnhancedLogLevel level) {
    pImpl->level = level;
}

EnhancedLogLevel EnhancedConsoleLogger::getLevel() const {
    return pImpl->level;
}

void EnhancedConsoleLogger::setUseColors(bool useColors) {
    pImpl->useColors = useColors;
}

bool EnhancedConsoleLogger::getUseColors() const {
    return pImpl->useColors;
}

void EnhancedConsoleLogger::setOutputFile(const std::string& filepath) {
    pImpl->fileOut.open(filepath, std::ios::app);
}

void EnhancedConsoleLogger::closeOutputFile() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

void EnhancedConsoleLogger::setTimestampFormat(const std::string& format) {
    pImpl->timestampFormat = format;
}

std::string EnhancedConsoleLogger::levelToString(EnhancedLogLevel level) const {
    switch (level) {
        case EnhancedLogLevel::Trace: return "TRACE";
        case EnhancedLogLevel::Debug: return "DEBUG";
        case EnhancedLogLevel::Info: return "INFO";
        case EnhancedLogLevel::Warning: return "WARN";
        case EnhancedLogLevel::Error: return "ERROR";
        case EnhancedLogLevel::Critical: return "CRIT";
        case EnhancedLogLevel::Performance: return "PERF";
        case EnhancedLogLevel::Memory: return "MEM";
        case EnhancedLogLevel::Network: return "NET";
        default: return "UNKNOWN";
    }
}

std::string EnhancedConsoleLogger::levelToColor(EnhancedLogLevel level) const {
    switch (level) {
        case EnhancedLogLevel::Trace: return "\033[90m";    // Bright black
        case EnhancedLogLevel::Debug: return "\033[36m";     // Cyan
        case EnhancedLogLevel::Info: return "\033[32m";       // Green
        case EnhancedLogLevel::Warning: return "\033[33m";    // Yellow
        case EnhancedLogLevel::Error: return "\033[31m";      // Red
        case EnhancedLogLevel::Critical: return "\033[35m";   // Magenta
        case EnhancedLogLevel::Performance: return "\033[34m"; // Blue
        case EnhancedLogLevel::Memory: return "\033[92m";    // Bright green
        case EnhancedLogLevel::Network: return "\033[95m";   // Bright magenta
        default: return "\033[0m";
    }
}

std::string EnhancedConsoleLogger::formatTimestamp(const std::chrono::system_clock::time_point& timestamp) const {
    auto time = std::chrono::system_clock::to_time_t(timestamp);
    std::tm tm_time;
    gmtime_r(&time, &tm_time);
    
    std::ostringstream oss;
    oss << std::put_time(&tm_time, pImpl->timestampFormat.c_str());
    return oss.str();
}

std::string EnhancedConsoleLogger::formatMetadata(const std::unordered_map<std::string, std::string>& metadata) const {
    std::ostringstream oss;
    oss << "{";
    bool first = true;
    for (const auto& pair : metadata) {
        if (!first) oss << ", ";
        oss << pair.first << "=" << pair.second;
        first = false;
    }
    oss << "}";
    return oss.str();
}

struct EnhancedLogger::Impl {
    std::vector<std::shared_ptr<IEnhancedLogger>> loggers;
    EnhancedLogLevel minLevel;
    bool loggingEnabled;
    std::mutex logMutex;
    
    // Error recovery
    int maxRetries;
    int retryDelay;
    
    // Session tracking
    std::string currentSessionId;
    std::chrono::system_clock::time_point sessionStartTime;
};

EnhancedLogger::EnhancedLogger() : pImpl(std::make_unique<Impl>()) {
    pImpl->minLevel = EnhancedLogLevel::Info;
    pImpl->loggingEnabled = true;
    pImpl->maxRetries = 3;
    pImpl->retryDelay = 100; // milliseconds
}

EnhancedLogger::~EnhancedLogger() = default;

void EnhancedLogger::addLogger(std::shared_ptr<IEnhancedLogger> logger) {
    if (logger) {
        std::lock_guard<std::mutex> lock(pImpl->logMutex);
        pImpl->loggers.push_back(std::move(logger));
    }
}

void EnhancedLogger::clearLoggers() {
    std::lock_guard<std::mutex> lock(pImpl->logMutex);
    pImpl->loggers.clear();
}

void EnhancedLogger::setLevel(EnhancedLogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->logMutex);
    pImpl->minLevel = level;
    for (const auto& logger : pImpl->loggers) {
        logger->setLevel(level);
    }
}

void EnhancedLogger::log(EnhancedLogLevel level, const std::string& message,
                         const std::string& file, int line, const std::string& function) {
    if (!pImpl->loggingEnabled) return;
    
    EnhancedLogEntry entry(level, message, file, line, function);
    
    std::lock_guard<std::mutex> lock(pImpl->logMutex);
    for (const auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
}

void EnhancedLogger::trace(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(EnhancedLogLevel::Trace, message, file, line, function);
}

void EnhancedLogger::debug(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(EnhancedLogLevel::Debug, message, file, line, function);
}

void EnhancedLogger::info(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(EnhancedLogLevel::Info, message, file, line, function);
}

void EnhancedLogger::warning(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(EnhancedLogLevel::Warning, message, file, line, function);
}

void EnhancedLogger::error(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(EnhancedLogLevel::Error, message, file, line, function);
}

void EnhancedLogger::critical(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(EnhancedLogLevel::Critical, message, file, line, function);
}

void EnhancedLogger::logPerformance(const std::string& metric, double value, const std::string& unit) {
    EnhancedLogEntry entry(EnhancedLogLevel::Performance, metric + " = " + std::to_string(value) + " " + unit);
    entry.isPerformanceMetric = true;
    entry.performanceValue = value;
    
    std::lock_guard<std::mutex> lock(pImpl->logMutex);
    for (const auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
}

void EnhancedLogger::logMemory(size_t memoryBytes, const std::string& context) {
    EnhancedLogEntry entry(EnhancedLogLevel::Memory, "Memory usage: " + std::to_string(memoryBytes) + " bytes");
    if (!context.empty()) {
        entry.metadata["context"] = context;
    }
    entry.memoryUsage = memoryBytes;
    
    std::lock_guard<std::mutex> lock(pImpl->logMutex);
    for (const auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
}

void EnhancedLogger::logStructured(EnhancedLogLevel level, const std::string& message,
                                   const std::unordered_map<std::string, std::string>& metadata,
                                   const std::string& file, int line, const std::string& function) {
    EnhancedLogEntry entry(level, message, file, line, function);
    entry.metadata = metadata;
    
    std::lock_guard<std::mutex> lock(pImpl->logMutex);
    for (const auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
}

bool EnhancedLogger::isEnabled(EnhancedLogLevel level) const {
    return level >= pImpl->minLevel;
}

EnhancedLogger& EnhancedLogger::getGlobal() {
    static std::shared_ptr<EnhancedLogger> globalLogger;
    static std::mutex mutex;
    
    std::lock_guard<std::mutex> lock(mutex);
    if (!globalLogger) {
        globalLogger = std::make_shared<EnhancedLogger>();
        
        // Add default console logger
        auto consoleLogger = std::make_shared<EnhancedConsoleLogger>(EnhancedLogLevel::Info);
        consoleLogger->setUseColors(true);
        globalLogger->addLogger(consoleLogger);
    }
    
    return *globalLogger;
}

void EnhancedLogger::setGlobal(std::shared_ptr<EnhancedLogger> logger) {
    std::lock_guard<std::mutex> lock(pImpl->logMutex);
    // Note: This would require a global mutex in the getGlobal() function
    // For simplicity, we'll just set the default logger
    if (logger) {
        pImpl = std::make_unique<Impl>(*logger->pImpl);
    }
}

bool EnhancedLogger::tryLog(const std::function<void()>& logOperation, int maxRetries, int retryDelay) {
    int retryCount = 0;
    while (retryCount <= maxRetries) {
        try {
            logOperation();
            return true;
        } catch (const std::exception& e) {
            retryCount++;
            if (retryCount > maxRetries) {
                // Log the final error
                error("Log operation failed after " + std::to_string(maxRetries) + " retries: " + std::string(e.what()));
                return false;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(retryDelay * retryCount));
        } catch (...) {
            retryCount++;
            if (retryCount > maxRetries) {
                error("Log operation failed after " + std::to_string(maxRetries) + " retries (unknown error)");
                return false;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(retryDelay * retryCount));
        }
    }
    return false;
}

void EnhancedLogger::startLogSession(const std::string& sessionId) {
    pImpl->currentSessionId = sessionId;
    pImpl->sessionStartTime = std::chrono::system_clock::now();
    info("Log session started: " + sessionId);
}

void EnhancedLogger::endLogSession(const std::string& sessionId) {
    if (pImpl->currentSessionId == sessionId) {
        auto duration = std::chrono::duration<double>(std::chrono::system_clock::now() - pImpl->sessionStartTime).count();
        info("Log session ended: " + sessionId + " (duration: " + std::to_string(duration) + "s)");
        pImpl->currentSessionId.clear();
    }
}

void EnhancedLogger::pauseLogging() {
    pImpl->loggingEnabled = false;
    info("Logging paused");
}

void EnhancedLogger::resumeLogging() {
    pImpl->loggingEnabled = true;
    info("Logging resumed");
}

} // namespace nlm
