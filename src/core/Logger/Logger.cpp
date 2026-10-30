#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <numeric>

namespace nlm {

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    size_t maxFileSize = 10 * 1024 * 1024;  // 10 MB
    size_t currentFileSize = 0;
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
    
    // Add timestamp with milliseconds
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        entry.timestamp.time_since_epoch()).time_since_epoch() % 1000;
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    oss << "." << std::setfill('0') << std::setw(3) << ms.count();
    
    oss << " " << entry.message;
    
    // Add source location for debug level
    if (entry.level == LogLevel::Debug && !entry.file.empty()) {
        oss << " (" << entry.file << ":" << entry.line << ")";
        if (!entry.function.empty()) {
            oss << " in " << entry.function;
        }
    }
    
    std::string output = oss.str();
    
    // Console output with immediate flush for critical errors
    if (pImpl->useColors) {
        std::cout << levelToColor(entry.level) << output << "\033[0m\n";
    } else {
        std::cout << output << "\n";
    }
    
    if (entry.level == LogLevel::Critical) {
        std::cout.flush();
    }
    
    // File output with log rotation
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut << output << "\n";
        pImpl->currentFileSize += output.size() + 1;  // +1 for newline
        
        // Log rotation if file is too large
        if (pImpl->currentFileSize > pImpl->maxFileSize) {
            pImpl->fileOut.close();
            // Rotate: log_1.txt -> log_2.txt, etc.
            for (int i = 4; i >= 1; --i) {
                std::string from = "log_" + std::to_string(i) + ".txt";
                std::string to = "log_" + std::to_string(i + 1) + ".txt";
                std::rename(from.c_str(), to.c_str());
            }
            std::rename("log_0.txt", "log_1.txt");
            pImpl->fileOut.open("log_0.txt", std::ios::app);
            pImpl->currentFileSize = 0;
        }
        
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
    pImpl->currentFileSize = 0;
    if (!pImpl->fileOut.is_open()) {
        NLM_LOG_ERROR("Failed to open log file: " + filepath);
    }
}

void ConsoleLogger::closeOutputFile() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
        pImpl->currentFileSize = 0;
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
        case LogLevel::Critical: return "\033[35;1m"; // Bold Magenta
        default: return "\033[0m";
    }
}

// Logger class implementation
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    std::unordered_map<std::string, size_t> callCounts;  // Performance tracking
    std::chrono::system_clock::time_point sessionStart;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {
    pImpl->sessionStart = std::chrono::system_clock::now();
    NLM_LOG_INFO("Logger initialized");
}

Logger::~Logger() {
    NLM_LOG_INFO("Logger shutting down");
}

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    if (!logger) {
        NLM_LOG_WARNING("Attempted to add null logger");
        return;
    }
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.push_back(logger);
    NLM_LOG_INFO("Added logger: " + std::to_string(reinterpret_cast<uintptr_t>(logger.get())));
}

void Logger::clearLoggers() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    size_t count = pImpl->loggers.size();
    pImpl->loggers.clear();
    NLM_LOG_INFO("Cleared " + std::to_string(count) + " loggers");
}

void Logger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        logger->setLevel(level);
    }
    NLM_LOG_INFO("Set log level to " + levelToString(level));
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
    
    // Track call counts for performance monitoring
    std::string key = levelToString(level) + "::" + message.substr(0, std::min<size_t>(50, message.size()));
    pImpl->callCounts[key]++;
    
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
    : level_(level), file_(file), line_(line), function_(function) {
    
    // Fast path: skip if level is disabled to avoid stringstream overhead
    if (!Logger::getGlobal().isEnabled(level_)) {
        // Create a null stream that discards everything
        class NullStream : public std::ostringstream {
        public:
            NullStream() : std::ostringstream() {}
            ~NullStream() {
                // Explicitly destroy to avoid partial writes
                str();
            }
        };
        oss_ = std::move(NullStream());
    }
}

LogStream::~LogStream() {
    Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
}

// Convenience functions for structured logging
void logSystemEvent(const std::string& eventType, const std::string& details = "") {
    Logger::getGlobal().info("SYSTEM_EVENT: " + eventType + (details.empty() ? "" : " - " + details));
}

void logPerformanceMetric(const std::string& metricName, double value, const std::string& unit = "") {
    std::ostringstream oss;
    oss << "PERF_METRIC: " << metricName << " = " << value;
    if (!unit.empty()) {
        oss << " " << unit;
    }
    Logger::getGlobal().info(oss.str());
}

void logMemoryUsage(size_t memoryBytes) {
    double memoryMB = memoryBytes / (1024.0 * 1024.0);
    Logger::getGlobal().info("MEMORY_USAGE: " + std::to_string(memoryMB) + " MB");
}

void logThreadInfo(const std::string& threadName, std::thread::id threadId) {
    Logger::getGlobal().debug("THREAD: " + threadName + " (ID: " + std::to_string(threadId) + ")");
}

// Fast logging macros for performance-critical code
#define NLM_LOG_FAST_INFO(msg) do { \
    if (nlm::Logger::getGlobal().isEnabled(nlm::LogLevel::Info)) { \
        nlm::Logger::getGlobal().info(msg, "", 0, ""); \
    } \
} while(0)

#define NLM_LOG_FAST_DEBUG(msg) do { \
    if (nlm::Logger::getGlobal().isEnabled(nlm::LogLevel::Debug)) { \
        nlm::Logger::getGlobal().debug(msg, "", 0, ""); \
    } \
} while(0)

// Performance monitoring utilities
class PerformanceMonitor {
public:
    struct PerformanceStats {
        uint64_t totalCalls = 0;
        uint64_t totalTime = 0;  // nanoseconds
        uint64_t maxTime = 0;
        uint64_t minTime = std::numeric_limits<uint64_t>::max();
        uint64_t lastTime = 0;
        double averageTime() const {
            return totalCalls > 0 ? static_cast<double>(totalTime) / totalCalls : 0.0;
        }
        double getLastTime() const {
            return static_cast<double>(lastTime) / 1000000.0;  // Convert to milliseconds
        }
    };
    
    static PerformanceMonitor& getInstance() {
        static PerformanceMonitor instance;
        return instance;
    }
    
    // Measure execution time of a function
    template<typename F, typename... Args>
    auto measureTime(F&& func, Args&&... args) {
        auto start = std::chrono::high_resolution_clock::now();
        auto result = std::forward<F>(func)(std::forward<Args>(args)...);
        auto end = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
        
        std::lock_guard<std::mutex> lock(mutex_);
        stats_[std::string(typeid(F).name())].totalCalls++;
        stats_[std::string(typeid(F).name())].totalTime += duration;
        stats_[std::string(typeid(F).name())].maxTime = std::max(stats_[std::string(typeid(F).name())].maxTime, duration);
        stats_[std::string(typeid(F).name())].minTime = std::min(stats_[std::string(typeid(F).name())].minTime, duration);
        stats_[std::string(typeid(F).name())].lastTime = duration;
        
        return result;
    }
    
    // Log performance statistics
    void logPerformanceStats() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        NLM_LOG_INFO("=== Performance Monitor Statistics ===");
        NLM_LOG_INFO("Session started: " + getTimestampString(pImpl->sessionStart));
        
        for (const auto& [name, stats] : stats_) {
            NLM_LOG_INFO("  " + name + ": calls=" + std::to_string(stats.totalCalls) +
                        " avg_time_ms=" + std::to_string(stats.averageTime() / 1000000.0) +
                        " min_ms=" + std::to_string(stats.minTime / 1000000.0) +
                        " max_ms=" + std::to_string(stats.maxTime / 1000000.0) +
                        " last_ms=" + std::to_string(stats.getLastTime()));
        }
    }
    
    // Clear all statistics
    void clearStats() {
        std::lock_guard<std::mutex> lock(mutex_);
        stats_.clear();
    }
    
    // Get statistics for a specific function type
    PerformanceStats getStats(const std::string& functionName) const {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = stats_.find(functionName);
        if (it != stats_.end()) {
            return it->second;
        }
        return PerformanceStats();
    }
    
private:
    PerformanceMonitor() {}
    ~PerformanceMonitor() {}
    PerformanceMonitor(const PerformanceMonitor&) = delete;
    PerformanceMonitor& operator=(const PerformanceMonitor&) = delete;
    
    std::mutex mutex_;
    std::unordered_map<std::string, PerformanceStats> stats_;
};

// Global performance monitor instance
PerformanceMonitor& getPerformanceMonitor() {
    return PerformanceMonitor::getInstance();
}

// Convenience macros for performance monitoring
#define NLM_PERF_MEASURE(name, code) do { \
    auto _start = std::chrono::high_resolution_clock::now(); \
    code; \
    auto _end = std::chrono::high_resolution_clock::now(); \
    auto _duration = std::chrono::duration_cast<std::chrono::microseconds>(_end - _start).count(); \
    nlm::Logger::getGlobal().debug("Performance[" << name << "]: " << _duration << "us"); \
    nlm::getPerformanceMonitor().getStats("Performance_" + std::string(name)).totalCalls++; \
    nlm::getPerformanceMonitor().getStats("Performance_" + std::string(name)).lastTime = _duration * 1000; \
} while(0)

#define NLM_PERF_SCOPE(name) nlm::ScopedTimer _scopedTimer_##name(#name, __func__);

class ScopedTimer {
public:
    ScopedTimer(const std::string& name, const std::string& function)
        : name_(name), function_(function) {
        start_ = std::chrono::high_resolution_clock::now();
    }
    
    ~ScopedTimer() {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start_).count();
        NLM_LOG_DEBUG("ScopedTimer[" << name_ << "](" << function_ << "): " << duration << "us");
        
        // Update performance monitor
        PerformanceMonitor::getInstance().getStats("ScopedTimer_" + name_).totalCalls++;
        PerformanceMonitor::getInstance().getStats("ScopedTimer_" + name_).lastTime = duration * 1000;
    }
    
private:
    std::string name_;
    std::string function_;
    std::chrono::high_resolution_clock::time_point start_;
};

} // namespace nlm

// Initialize global performance monitor on first use
nlm::PerformanceMonitor& getPerformanceMonitor() {
    return nlm::PerformanceMonitor::getInstance();
}

// Additional logging convenience functions
void nlm::Logger::debug(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Debug, message, file, line, function);
}

void nlm::Logger::info(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Info, message, file, line, function);
}

void nlm::Logger::warning(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Warning, message, file, line, function);
}

void nlm::Logger::error(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Error, message, file, line, function);
}

void nlm::Logger::critical(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Critical, message, file, line, function);
}

// Helper function for getting timestamp string
std::string getTimestampString(const std::chrono::system_clock::time_point& tp) {
    auto time = std::chrono::system_clock::to_time_t(tp);
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

// Global performance logging helper
void logPerformanceStats() {
    nlm::getPerformanceMonitor().logPerformanceStats();
}