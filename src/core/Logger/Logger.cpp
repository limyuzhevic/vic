#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <chrono>
#include <algorithm>

namespace nlm {

// Thread-local context storage
thread_local LogContext::ContextData LogContext::context_;

// ConsoleLogger implementation
struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    
    // Category filtering
    std::unordered_map<LogCategory, bool> categoryFilters;
    
    // Context filtering
    std::unordered_map<std::string, std::string> contextFilters;
    
    // Performance monitoring
    double logSamplingRate = 1.0;  // Sample 1 in N messages
    size_t maxLogSize = 0;  // 0 = unlimited
    
    // Statistics
    size_t totalMessagesLogged = 0;
    size_t messagesFiltered = 0;
    
    Impl() {
        // Enable all categories by default
        for (int i = 0; i < static_cast<int>(LogCategory::Visualization); ++i) {
            categoryFilters[static_cast<LogCategory>(i)] = true;
        }
    }
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
}

ConsoleLogger::~ConsoleLogger() = default;

void ConsoleLogger::log(const LogEntry& entry) {
    if (!shouldLogEntry(entry)) {
        pImpl->messagesFiltered++;
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    
    // Add context information
    if (!entry.correlationId.empty()) {
        oss << "[corr:" << entry.correlationId << "] ";
    }
    if (!entry.userId.empty()) {
        oss << "[user:" << entry.userId << "] ";
    }
    
    // Add timestamp
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    
    // Add source location
    if (!entry.file.empty()) {
        oss << " " << entry.file;
        if (entry.line > 0) {
            oss << ":" << entry.line;
        }
    }
    
    oss << " " << entry.message;
    
    // Add custom context fields
    for (const auto& [key, value] : entry.context) {
        oss << " [" << key << ":" << value << "]";
    }
    
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
    
    pImpl->totalMessagesLogged++;
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

void ConsoleLogger::setCategoryFilter(LogCategory category, bool enabled) {
    pImpl->categoryFilters[category] = enabled;
}

bool ConsoleLogger::isCategoryEnabled(LogCategory category) const {
    auto it = pImpl->categoryFilters.find(category);
    return it != pImpl->categoryFilters.end() && it->second;
}

void ConsoleLogger::setContextFilter(const std::string& key, const std::string& value) {
    pImpl->contextFilters[key] = value;
}

void ConsoleLogger::setLogSamplingRate(double rate) {
    pImpl->logSamplingRate = rate;
}

void ConsoleLogger::setMaxLogSize(size_t maxBytes) {
    pImpl->maxLogSize = maxBytes;
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

bool ConsoleLogger::shouldLogEntry(const LogEntry& entry) const {
    // Level check
    if (entry.level < pImpl->level) {
        return false;
    }
    
    // Category check (assuming all categories are enabled by default)
    // Note: This would need category information in LogEntry for proper filtering
    
    // Context filtering
    for (const auto& [key, filterValue] : pImpl->contextFilters) {
        auto it = entry.context.find(key);
        if (it != entry.context.end() && it->second != filterValue) {
            return false;
        }
    }
    
    // Sampling check
    if (pImpl->logSamplingRate > 1.0) {
        size_t sampleIndex = pImpl->totalMessagesLogged % static_cast<size_t>(pImpl->logSamplingRate);
        if (sampleIndex != 0) {
            return false;
        }
    }
    
    // Size check (simple implementation)
    if (pImpl->maxLogSize > 0) {
        // This would require tracking current log size
        // For now, we'll skip this check
    }
    
    return true;
}

// StructuredLogger implementation
struct StructuredLogger::Impl {
    LogLevel level = LogLevel::Info;
    std::mutex mutex;
    
    std::ofstream jsonFile;
    std::string filepath;
    
    std::unordered_map<LogCategory, bool> categoryFilters;
};

StructuredLogger::StructuredLogger() : pImpl(std::make_unique<Impl>()) {
    // Enable all categories by default
    for (int i = 0; i < static_cast<int>(LogCategory::Visualization); ++i) {
        pImpl->categoryFilters[static_cast<LogCategory>(i)] = true;
    }
}

StructuredLogger::StructuredLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
    // Enable all categories by default
    for (int i = 0; i < static_cast<int>(LogCategory::Visualization); ++i) {
        pImpl->categoryFilters[static_cast<LogCategory>(i)] = true;
    }
}

StructuredLogger::~StructuredLogger() = default;

void StructuredLogger::log(const LogEntry& entry) {
    // For structured logging, we'll focus on JSON output
    // Implementation details depend on specific requirements
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Convert to structured format and write to JSON file
    StructuredLogEntry structuredEntry;
    structuredEntry.timestamp = entry.timestamp;
    structuredEntry.level = levelToString(entry.level);
    structuredEntry.message = entry.message;
    structuredEntry.fields = entry.context;
    
    // TODO: Implement proper JSON serialization
    std::string jsonLine = structuredEntry.toJSON();
    
    if (pImpl->jsonFile.is_open()) {
        pImpl->jsonFile << jsonLine << "\n";
    }
}

void StructuredLogger::setLevel(LogLevel level) {
    pImpl->level = level;
}

LogLevel StructuredLogger::getLevel() const {
    return pImpl->level;
}

void StructuredLogger::setCategoryFilter(LogCategory category, bool enabled) {
    pImpl->categoryFilters[category] = enabled;
}

bool StructuredLogger::isCategoryEnabled(LogCategory category) const {
    auto it = pImpl->categoryFilters.find(category);
    return it != pImpl->categoryFilters.end() && it->second;
}

void StructuredLogger::setContextFilter(const std::string& key, const std::string& value) {
    // Context filtering for structured logger
    // Implementation depends on specific requirements
}

void StructuredLogger::setJSONOutput(const std::string& filepath) {
    pImpl->filepath = filepath;
    pImpl->jsonFile.open(filepath, std::ios::app);
    if (pImpl->jsonFile.is_open()) {
        pImpl->jsonFile << "[\n";  // Start JSON array
    }
}

void StructuredLogger::closeJSONOutput() {
    if (pImpl->jsonFile.is_open()) {
        pImpl->jsonFile << "\n]\n";  // Close JSON array
        pImpl->jsonFile.close();
    }
}

void StructuredLogger::logEvent(const std::string& eventType, const std::unordered_map<std::string, std::string>& data) {
    StructuredLogEntry entry;
    entry.timestamp = std::chrono::system_clock::now();
    entry.level = "INFO";
    entry.message = "Event: " + eventType;
    entry.fields = data;
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->jsonFile.is_open()) {
        pImpl->jsonFile << entry.toJSON() << "\n";
        pImpl->jsonFile.flush();
    }
}

std::string StructuredLogEntry::toJSON() const {
    std::ostringstream oss;
    oss << "  {\"timestamp\": \"" << std::chrono::system_clock::to_time_t(timestamp) << \"\", \"level\": \"" 
        << level << "\", \"message\": \"" << message << "\", \"fields\": {";
    
    bool first = true;
    for (const auto& [key, value] : fields) {
        if (!first) oss << ", ";
        oss << "\"" << key << "\": \"" << value << "\"";
        first = false;
    }
    oss << "}}";
    
    return oss.str();
}

std::string StructuredLogEntry::toString() const {
    std::ostringstream oss;
    oss << "[" << std::chrono::system_clock::to_time_t(timestamp) << "] " << level << ": " << message;
    for (const auto& [key, value] : fields) {
        oss << " [" << key << ":" << value << "]";
    }
    return oss.str();
}

// Logger implementation
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    
    // Performance monitoring
    std::unordered_map<std::string, std::chrono::high_resolution_clock::time_point> performanceTimers;
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> performanceMetrics;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {}

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
    
    // Add context information from LogContext
    entry.correlationId = LogContext::getCorrelationId();
    entry.userId = LogContext::getUserId();
    entry.sessionId = LogContext::getSessionId();
    entry.threadId = std::to_string(std::hash<std::thread::id>()(std::this_thread::get_id()));
    
    for (const auto& [key, value] : LogContext::getCustomContext()) {
        entry.context[key] = value;
    }
    
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

void Logger::logErrorWithContext(const std::string& errorCode, const std::string& errorMessage,
                                const std::string& errorCategory,
                                const std::vector<std::string>& stackTrace) {
    error(errorMessage);
    
    // TODO: Store error details in internal error log
    // This could be an error tracking system
}

void Logger::logPerformance(const std::string& operation,
                           std::chrono::high_resolution_clock::duration duration,
                           const std::unordered_map<std::string, std::string>& metrics) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "Operation: " << operation << " | Duration: " << duration.count() << " ns";
    
    for (const auto& [key, value] : metrics) {
        oss << " | " << key << ": " << value;
    }
    
    info(oss.str());
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

void Logger::setCorrelationId(const std::string& id) {
    LogContext::setCorrelationId(id);
}

std::string Logger::getCorrelationId() {
    return LogContext::getCorrelationId();
}

void Logger::setUserId(const std::string& id) {
    LogContext::setUserId(id);
}

std::string Logger::getUserId() {
    return LogContext::getUserId();
}

void Logger::setSessionId(const std::string& id) {
    LogContext::setSessionId(id);
}

std::string Logger::getSessionId() {
    return LogContext::getSessionId();
}

void Logger::setContext(const std::string& key, const std::string& value) {
    LogContext::setContext(key, value);
}

std::string Logger::getContext(const std::string& key) {
    return LogContext::getContext(key);
}

void Logger::clearContext() {
    LogContext::clear();
}

void Logger::reportError(const std::string& component, const std::string& errorCode,
                        const std::string& errorMessage, const std::string& file,
                        int line, const std::string& function) {
    error("[" + component + "] " + errorCode + ": " + errorMessage);
    
    // TODO: Implement error tracking and reporting system
    // This could include error aggregation, alerting, etc.
}

void Logger::startPerformanceTimer(const std::string& operation) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->performanceTimers[operation] = std::chrono::high_resolution_clock::now();
}

void Logger::endPerformanceTimer(const std::string& operation,
                                 const std::unordered_map<std::string, std::string>& metrics) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->performanceTimers.find(operation);
    if (it != pImpl->performanceTimers.end()) {
        auto duration = std::chrono::high_resolution_clock::now() - it->second;
        
        pImpl->performanceMetrics[operation] = metrics;
        
        logPerformance(operation, duration, metrics);
        
        pImpl->performanceTimers.erase(it);
    }
}

// LogContext implementation
thread_local LogContext::ContextData LogContext::context_;

std::unordered_map<std::string, std::string> LogContext::getCustomContext() {
    return context_.customContext;
}

// LogStream implementation
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function) {}

LogStream::~LogStream() {
    Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
}

LogStream& LogStream::field(const std::string& key, const std::string& value) {
    fields_[key] = value;
    return *this;
}

} // namespace nlm