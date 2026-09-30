#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <algorithm>
#include <thread>
#include <chrono>
#include <ctime>
#include <iomanip>

namespace nlm {

// Global performance tracker instance
static PerformanceTracker globalPerformanceTracker;

struct PerformanceTracker::Impl {
    struct OperationTiming {
        std::string operation;
        std::chrono::steady_clock::time_point startTime;
        bool isActive;
        std::string context;
        std::string file;
        int line;
        std::string function;
        double durationMs;
    };
    
    std::vector<PerformanceMetrics> metrics;
    std::vector<OperationTiming> activeOperations;
    std::mutex mutex;
};

PerformanceTracker::PerformanceTracker() : pImpl(std::make_unique<Impl>()) {}

PerformanceTracker::~PerformanceTracker() = default;

void PerformanceTracker::start(const std::string& operation, const std::string& context) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    OperationTiming timing;
    timing.operation = operation;
    timing.startTime = std::chrono::steady_clock::now();
    timing.isActive = true;
    timing.context = context;
    timing.durationMs = 0.0;
    
    pImpl->activeOperations.push_back(timing);
}

double PerformanceTracker::getCurrentDuration() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    double maxDuration = 0.0;
    auto now = std::chrono::steady_clock::now();
    
    for (const auto& op : pImpl->activeOperations) {
        if (op.isActive) {
            auto duration = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(now - op.startTime).count();
            if (duration > maxDuration) {
                maxDuration = duration;
            }
        }
    }
    
    return maxDuration;
}

void PerformanceTracker::stop(const std::string& operation, 
                             const std::string& file, 
                             int line, 
                             const std::string& function) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto now = std::chrono::steady_clock::now();
    
    for (auto& op : pImpl->activeOperations) {
        if (op.isActive && op.operation == operation) {
            op.isActive = false;
            op.durationMs = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(now - op.startTime).count();
            op.file = file;
            op.line = line;
            op.function = function;
            
            // Create performance metrics entry
            PerformanceMetrics metrics;
            metrics.timestamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            metrics.operation = operation;
            metrics.durationMs = op.durationMs;
            metrics.threadId = std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id()));
            metrics.file = file;
            metrics.line = line;
            metrics.function = function;
            metrics.context = op.context;
            
            pImpl->metrics.push_back(metrics);
            break;
        }
    }
}

void PerformanceTracker::clear() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->metrics.clear();
    pImpl->activeOperations.clear();
}

const std::vector<PerformanceMetrics>& PerformanceTracker::getMetrics() const {
    return pImpl->metrics;
}

std::vector<PerformanceMetrics> PerformanceTracker::getMetricsByOperation(const std::string& operation) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<PerformanceMetrics> result;
    
    for (const auto& metric : pImpl->metrics) {
        if (metric.operation == operation) {
            result.push_back(metric);
        }
    }
    
    return result;
}

double PerformanceTracker::getAverageDuration(const std::string& operation) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto metrics = getMetricsByOperation(operation);
    if (metrics.empty()) {
        return 0.0;
    }
    
    double totalDuration = 0.0;
    for (const auto& metric : metrics) {
        totalDuration += metric.durationMs;
    }
    
    return totalDuration / metrics.size();
}

std::string PerformanceTracker::getStatistics() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "Performance Tracker Statistics:\n";
    oss << "Total operations: " << pImpl->metrics.size() << "\n";
    oss << "Active operations: " << std::count_if(pImpl->activeOperations.begin(), pImpl->activeOperations.end(),
                                                  [](const OperationTiming& op) { return op.isActive; }) << "\n";
    
    if (!pImpl->metrics.empty()) {
        oss << "\nOperations by type:\n";
        std::unordered_map<std::string, int> opCount;
        double totalDuration = 0.0;
        
        for (const auto& metric : pImpl->metrics) {
            opCount[metric.operation]++;
            totalDuration += metric.durationMs;
        }
        
        for (const auto& pair : opCount) {
            double avgDuration = getAverageDuration(pair.first);
            oss << "  " << pair.first << ": " << pair.second << " calls, avg duration: " << avgDuration << "ms\n";
        }
        
        oss << "Total duration: " << totalDuration << "ms\n";
        oss << "Average duration: " << (totalDuration / pImpl->metrics.size()) << "ms\n";
    }
    
    return oss.str();
}

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    bool performanceLoggingEnabled = true;
    std::string colorScheme = "default";
    std::ofstream fileOut;
    std::mutex mutex;
    std::string context;
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {
    pImpl->useColors = true;
}

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
    
    if (!entry.context.empty()) {
        oss << " [" << entry.context << "]";
    }
    
    if (!entry.file.empty() && entry.line > 0) {
        oss << " " << entry.file << ":" << entry.line;
    }
    
    if (!entry.function.empty()) {
        oss << " in " << entry.function;
    }
    
    oss << " - " << entry.message;
    
    if (entry.durationMs > 0.0) {
        oss << " (duration: " << entry.durationMs << "ms)";
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
}

void ConsoleLogger::logPerformance(const PerformanceMetrics& metrics) {
    if (!pImpl->performanceLoggingEnabled) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    LogEntry entry;
    entry.level = metrics.durationMs > 100.0 ? LogLevel::Warning : LogLevel::Info;
    entry.message = "Performance: " + metrics.operation + " took " + std::to_string(metrics.durationMs) + "ms";
    entry.file = metrics.file;
    entry.line = metrics.line;
    entry.function = metrics.function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.threadId = metrics.threadId;
    entry.threadHash = std::hash<std::string>{}(metrics.threadId);
    entry.context = metrics.context;
    
    log(entry);
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

void ConsoleLogger::setPerformanceLoggingEnabled(bool enabled) {
    pImpl->performanceLoggingEnabled = enabled;
}

bool ConsoleLogger::isPerformanceLoggingEnabled() const {
    return pImpl->performanceLoggingEnabled;
}

void ConsoleLogger::setColorScheme(const std::string& scheme) {
    pImpl->colorScheme = scheme;
}

std::string ConsoleLogger::getPerformanceStatistics() const {
    return globalPerformanceTracker.getStatistics();
}

std::string ConsoleLogger::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::Trace: return "TRACE";
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Notice: return "NOTICE";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Critical: return "CRIT";
        case LogLevel::Alert: return "ALERT";
        case LogLevel::Emergency: return "EMERG";
        default: return "UNKNOWN";
    }
}

std::string ConsoleLogger::levelToColor(LogLevel level) const {
    switch (level) {
        case LogLevel::Trace: return "\033[37m";    // White
        case LogLevel::Debug: return "\033[36m";    // Cyan
        case LogLevel::Info: return "\033[32m";      // Green
        case LogLevel::Notice: return "\033[94m";   // Blue
        case LogLevel::Warning: return "\033[33m";   // Yellow
        case LogLevel::Error: return "\033[31m";     // Red
        case LogLevel::Critical: return "\033[35m"; // Magenta
        case LogLevel::Alert: return "\033[91m";    // Bright Red
        case LogLevel::Emergency: return "\033[95m"; // Bright Magenta
        default: return "\033[0m";
    }
}

std::string ConsoleLogger::formatPerformance(const PerformanceMetrics& metrics) const {
    std::ostringstream oss;
    oss << "[PERFORMANCE] " << metrics.operation << ": " << metrics.durationMs << "ms";
    if (!metrics.context.empty()) {
        oss << " (context: " << metrics.context << ")";
    }
    return oss.str();
}

void ConsoleLogger::setOutputFile(const std::string& filepath) {
    pImpl->fileOut.open(filepath, std::ios::app);
}

void ConsoleLogger::closeOutputFile() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

struct MemoryLogger::Impl {
    LogLevel level = LogLevel::Info;
    size_t maxEntries = 10000;
    std::vector<LogEntry> entries;
    std::mutex mutex;
    std::string context;
};

MemoryLogger::MemoryLogger() : pImpl(std::make_unique<Impl>()) {}

MemoryLogger::MemoryLogger(size_t maxEntries) : pImpl(std::make_unique<Impl>()) {
    pImpl->maxEntries = maxEntries;
}

MemoryLogger::~MemoryLogger() = default;

void MemoryLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Add thread information
    LogEntry newEntry = entry;
    newEntry.threadId = std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    newEntry.threadHash = std::hash<std::string>{}(newEntry.threadId);
    
    pImpl->entries.push_back(newEntry);
    
    // Limit entries if needed
    if (pImpl->entries.size() > pImpl->maxEntries) {
        size_t excess = pImpl->entries.size() - pImpl->maxEntries;
        pImpl->entries.erase(pImpl->entries.begin(), pImpl->entries.begin() + excess);
    }
}

void MemoryLogger::logPerformance(const PerformanceMetrics& metrics) {
    LogEntry entry;
    entry.level = metrics.durationMs > 100.0 ? LogLevel::Warning : LogLevel::Info;
    entry.message = "Performance: " + metrics.operation + " took " + std::to_string(metrics.durationMs) + "ms";
    entry.file = metrics.file;
    entry.line = metrics.line;
    entry.function = metrics.function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.threadId = metrics.threadId;
    entry.threadHash = std::hash<std::string>{}(metrics.threadId);
    entry.context = metrics.context;
    entry.durationMs = metrics.durationMs;
    
    log(entry);
}

void MemoryLogger::setLevel(LogLevel level) {
    pImpl->level = level;
}

LogLevel MemoryLogger::getLevel() const {
    return pImpl->level;
}

void MemoryLogger::clear() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->entries.clear();
}

size_t MemoryLogger::getEntryCount() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->entries.size();
}

const std::vector<LogEntry>& MemoryLogger::getEntries() const {
    return pImpl->entries;
}

std::vector<LogEntry> MemoryLogger::getEntriesByLevel(LogLevel level) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<LogEntry> result;
    
    for (const auto& entry : pImpl->entries) {
        if (entry.level == level) {
            result.push_back(entry);
        }
    }
    
    return result;
}

std::vector<LogEntry> MemoryLogger::getEntriesByContext(const std::string& context) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<LogEntry> result;
    
    for (const auto& entry : pImpl->entries) {
        if (entry.context == context) {
            result.push_back(entry);
        }
    }
    
    return result;
}

std::vector<LogEntry> MemoryLogger::getEntriesByTimeRange(double startTime, double endTime) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<LogEntry> result;
    
    for (const auto& entry : pImpl->entries) {
        auto timePoint = std::chrono::system_clock::to_time_t(entry.timestamp);
        double entryTime = static_cast<double>(timePoint);
        
        if (entryTime >= startTime && entryTime <= endTime) {
            result.push_back(entry);
        }
    }
    
    return result;
}

std::vector<LogEntry> MemoryLogger::search(const std::string& query) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<LogEntry> result;
    
    std::transform(query.begin(), query.end(), query.begin(), ::tolower);
    
    for (const auto& entry : pImpl->entries) {
        std::string lowerMessage = entry.message;
        std::transform(lowerMessage.begin(), lowerMessage.end(), lowerMessage.begin(), ::tolower);
        
        if (lowerMessage.find(query) != std::string::npos) {
            result.push_back(entry);
        }
    }
    
    return result;
}

std::vector<LogEntry> MemoryLogger::searchByMessage(const std::string& message) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<LogEntry> result;
    
    std::transform(message.begin(), message.end(), message.begin(), ::tolower);
    
    for (const auto& entry : pImpl->entries) {
        std::string lowerMessage = entry.message;
        std::transform(lowerMessage.begin(), lowerMessage.end(), lowerMessage.begin(), ::tolower);
        
        if (lowerMessage.find(message) != std::string::npos) {
            result.push_back(entry);
        }
    }
    
    return result;
}

std::vector<LogEntry> MemoryLogger::searchByFile(const std::string& filename) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<LogEntry> result;
    
    std::transform(filename.begin(), filename.end(), filename.begin(), ::tolower);
    
    for (const auto& entry : pImpl->entries) {
        std::string lowerFile = entry.file;
        std::transform(lowerFile.begin(), lowerFile.end(), lowerFile.begin(), ::tolower);
        
        if (lowerFile.find(filename) != std::string::npos) {
            result.push_back(entry);
        }
    }
    
    return result;
}

std::string MemoryLogger::getStatistics() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "Memory Logger Statistics:\n";
    oss << "Total entries: " << pImpl->entries.size() << "\n";
    oss << "Max entries: " << pImpl->maxEntries << "\n";
    
    if (!pImpl->entries.empty()) {
        oss << "\nEntries by level:\n";
        std::unordered_map<LogLevel, size_t> levelCount;
        
        for (const auto& entry : pImpl->entries) {
            levelCount[entry.level]++;
        }
        
        for (const auto& pair : levelCount) {
            oss << "  " << levelToString(pair.first) << ": " << pair.second << "\n";
        }
        
        oss << "\nEntries by context:\n";
        std::unordered_map<std::string, size_t> contextCount;
        
        for (const auto& entry : pImpl->entries) {
            if (!entry.context.empty()) {
                contextCount[entry.context]++;
            }
        }
        
        for (const auto& pair : contextCount) {
            oss << "  " << pair.first << ": " << pair.second << "\n";
        }
    }
    
    return oss.str();
}

struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    PerformanceTracker perfTracker;
    std::string globalContext;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {
    // Add default console logger
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    addLogger(consoleLogger);
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

PerformanceTracker& Logger::getPerformanceTracker() {
    return pImpl->perfTracker;
}

void Logger::log(LogLevel level, const std::string& message,
                const std::string& file, int line, const std::string& function,
                const std::string& context) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.threadId = std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    entry.threadHash = std::hash<std::string>{}(entry.threadId);
    entry.context = context;
    
    for (auto& logger : pImpl->loggers) {
        if (logger->shouldLog(entry)) {
            logger->log(entry);
        }
    }
}

void Logger::logPerformance(const std::string& operation,
                           double durationMs,
                           const std::string& file,
                           int line,
                           const std::string& function,
                           const std::string& context) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    PerformanceMetrics metrics;
    metrics.timestamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    metrics.operation = operation;
    metrics.durationMs = durationMs;
    metrics.threadId = std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    metrics.file = file;
    metrics.line = line;
    metrics.function = function;
    metrics.context = context;
    
    for (auto& logger : pImpl->loggers) {
        logger->logPerformance(metrics);
    }
}

void Logger::trace(const std::string& message, const std::string& file, int line, const std::string& function, const std::string& context) {
    log(LogLevel::Trace, message, file, line, function, context);
}

void Logger::debug(const std::string& message, const std::string& file, int line, const std::string& function, const std::string& context) {
    log(LogLevel::Debug, message, file, line, function, context);
}

void Logger::info(const std::string& message, const std::string& file, int line, const std::string& function, const std::string& context) {
    log(LogLevel::Info, message, file, line, function, context);
}

void Logger::notice(const std::string& message, const std::string& file, int line, const std::string& function, const std::string& context) {
    log(LogLevel::Notice, message, file, line, function, context);
}

void Logger::warning(const std::string& message, const std::string& file, int line, const std::string& function, const std::string& context) {
    log(LogLevel::Warning, message, file, line, function, context);
}

void Logger::error(const std::string& message, const std::string& file, int line, const std::string& function, const std::string& context) {
    log(LogLevel::Error, message, file, line, function, context);
}

void Logger::critical(const std::string& message, const std::string& file, int line, const std::string& function, const std::string& context) {
    log(LogLevel::Critical, message, file, line, function, context);
}

void Logger::alert(const std::string& message, const std::string& file, int line, const std::string& function, const std::string& context) {
    log(LogLevel::Alert, message, file, line, function, context);
}

void Logger::emergency(const std::string& message, const std::string& file, int line, const std::string& function, const std::string& context) {
    log(LogLevel::Emergency, message, file, line, function, context);
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

LogStatistics Logger::getLogStatistics() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    LogStatistics stats;
    stats.totalLogs = 0;
    std::fill_n(stats.logsByLevel, 8, 0);
    
    auto now = std::chrono::system_clock::now();
    auto timeSinceEpoch = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
    stats.firstLogTime = 0.0;
    stats.lastLogTime = static_cast<double>(timeSinceEpoch);
    
    return stats;
}

std::string Logger::getPerformanceStatistics() const {
    return pImpl->perfTracker.getStatistics();
}

std::string Logger::getSystemStatistics() const {
    std::ostringstream oss;
    oss << "System Statistics:\n";
    oss << "Logger instances: " << pImpl->loggers.size() << "\n";
    oss << "Performance operations tracked: " << pImpl->perfTracker.getMetrics().size() << "\n";
    
    return oss.str();
}

// LogStream implementation
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function, const std::string& context)
    : level_(level), file_(file), line_(line), function_(function), context_(context), timingEnabled_(false), durationMs_(0.0) {}

LogStream::~LogStream() {
    Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_, context_);
}

LogStream& LogStream::withTiming(double durationMs) {
    timingEnabled_ = true;
    durationMs_ = durationMs;
    return *this;
}

// Global performance tracker accessor
PerformanceTracker& getGlobalPerformanceTracker() {
    return globalPerformanceTracker;
}

} // namespace nlm
