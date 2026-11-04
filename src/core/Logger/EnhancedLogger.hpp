// nlm - Neural Learning Machine
// Enhanced Logging Infrastructure
// Phase 2: Robust Runtime Logging
//
// This phase implements enhanced logging with:
// - Advanced log levels and formatting
// - Thread-safe logging operations
// - Performance monitoring integration
// - Memory usage tracking
// - Error logging with context
// - Structured logging support

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <chrono>
#include <mutex>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <fstream>
#include <algorithm>
#include <optional>

namespace nlm {

// Enhanced logging levels with severity hierarchy
enum class LogLevel {
    Trace = 0,      // Detailed debug information
    Debug = 1,      // General debugging information
    Info = 2,        // General informational messages
    Notice = 3,     // Important but normal events
    Warning = 4,     // Warnings about potential issues
    Error = 5,       // Errors that don't prevent execution
    Critical = 6,    // Critical errors that affect core functionality
    Alert = 7,       // Immediate action required
    Emergency = 8    // System is unusable
};

// Enhanced log entry with context and metadata
struct EnhancedLogEntry {
    LogLevel level;
    std::string message;
    std::string source;
    std::string function;
    std::string file;
    int line;
    std::chrono::system_clock::time_point timestamp;
    std::string threadId;
    uint64_t memoryUsage;
    std::map<std::string, std::string> context;
    std::vector<std::string> tags;
    std::string correlationId;
    
    EnhancedLogEntry() : level(LogLevel::Info), line(0), memoryUsage(0) {
        timestamp = std::chrono::system_clock::now();
        threadId = std::to_string(std::hash<std::thread::id>()(std::this_thread::get_id()));
        generateCorrelationId();
    }
    
    EnhancedLogEntry(LogLevel lvl, const std::string& msg, 
                    const std::string& src = "",
                    const std::string& func = "",
                    const std::string& f = "",
                    int ln = 0,
                    const std::map<std::string, std::string>& ctx = {})
        : level(lvl), message(msg), source(src), function(func), 
          file(f), line(ln), memoryUsage(0), context(ctx) {
        timestamp = std::chrono::system_clock::now();
        threadId = std::to_string(std::hash<std::thread::id>()(std::this_thread::get_id()));
        generateCorrelationId();
    }
    
    std::string format() const {
        std::ostringstream oss;
        oss << "[" << levelToString(level) << "] ";
        oss << "[" << correlationId << "] ";
        oss << "[" << threadId << "] ";
        
        // Add timestamp
        auto time = std::chrono::system_clock::to_time_t(timestamp);
        oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        
        // Add source location if available
        if (!file.empty() && line > 0) {
            oss << " " << file << ":" << line;
        }
        if (!function.empty()) {
            oss << " " << function << "()";
        }
        
        oss << " " << message;
        
        // Add context
        if (!context.empty()) {
            oss << " [Context:";
            bool first = true;
            for (const auto& pair : context) {
                if (!first) oss << ",";
                oss << " " << pair.first << "=" << pair.second;
                first = false;
            }
            oss << "]";
        }
        
        // Add tags
        if (!tags.empty()) {
            oss << " [Tags:";
            bool first = true;
            for (const auto& tag : tags) {
                if (!first) oss << ",";
                oss << " " << tag;
                first = false;
            }
            oss << "]";
        }
        
        return oss.str();
    }
    
    std::string levelToString(LogLevel level) {
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
    
private:
    void generateCorrelationId() {
        std::ostringstream oss;
        auto now = std::chrono::system_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()).count();
        oss << "LOG-" << ms << "-" << std::rand();
        correlationId = oss.str();
    }
};

// Performance metrics for logging
struct LoggingMetrics {
    size_t totalLogs = 0;
    std::map<LogLevel, size_t> logsByLevel;
    std::chrono::system_clock::time_point firstLog;
    std::chrono::system_clock::time_point lastLog;
    size_t logSize = 0;  // Estimated size in bytes
    double averageLogSize = 0.0;
    
    void record(const EnhancedLogEntry& entry) {
        totalLogs++;
        logsByLevel[entry.level]++;
        
        if (!firstLog.time_since_epoch().count()) {
            firstLog = entry.timestamp;
        }
        lastLog = entry.timestamp;
        
        // Update size metrics
        size_t entrySize = estimateSize(entry);
        logSize += entrySize;
        averageLogSize = static_cast<double>(logSize) / totalLogs;
    }
    
private:
    size_t estimateSize(const EnhancedLogEntry& entry) const {
        size_t size = 0;
        size += entry.message.size() * 2;  // Account for encoding
        size += entry.source.size();
        size += entry.function.size();
        size += entry.file.size();
        size += std::to_string(entry.line).size() * 2;
        size += entry.context.size() * 50;  // Average context entry size
        size += entry.tags.size() * 20;  // Average tag size
        size += entry.correlationId.size() * 2;
        return size;
    }
};

// Thread-safe log buffer for batch processing
class LogBuffer {
public:
    struct Config {
        size_t maxBufferSize = 10000;
        size_t maxBufferTimeMs = 100;
        size_t maxBatchSize = 100;
        bool flushOnLevel = true;
        LogLevel flushLevel = LogLevel::Critical;
    };
    
    LogBuffer() = default;
    explicit LogBuffer(const Config& config) : config_(config) {}
    
    // Add entry to buffer
    void add(const EnhancedLogEntry& entry) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        buffer_.push_back(entry);
        currentBatchSize_++;
        currentBatchTime_ = std::chrono::system_clock::now();
        
        // Flush if buffer is full
        if (buffer_.size() >= config_.maxBufferSize) {
            flush();
        }
        
        // Flush if batch time exceeded
        if (shouldFlushByTime()) {
            flush();
        }
        
        // Flush if level threshold reached
        if (config_.flushOnLevel && entry.level >= config_.flushLevel) {
            flush();
        }
    }
    
    // Flush buffer to logger
    void flush() {
        if (buffer_.empty() || !logger_) return;
        
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (buffer_.empty()) return;
        
        // Send batch to logger
        logger_->logBatch(buffer_);
        
        // Clear buffer
        buffer_.clear();
        currentBatchSize_ = 0;
        currentBatchTime_ = std::chrono::system_clock::time_point();
    }
    
    // Set logger
    void setLogger(std::shared_ptr<class BatchLogger> logger) {
        std::lock_guard<std::mutex> lock(mutex_);
        logger_ = logger;
    }
    
    // Get buffer size
    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return buffer_.size();
    }
    
    // Clear buffer
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        buffer_.clear();
        currentBatchSize_ = 0;
        currentBatchTime_ = std::chrono::system_clock::time_point();
    }
    
private:
    bool shouldFlushByTime() const {
        if (buffer_.empty()) return false;
        
        auto now = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - currentBatchTime_);
        
        return elapsed.count() >= config_.maxBufferTimeMs;
    }
    
    Config config_;
    std::vector<EnhancedLogEntry> buffer_;
    std::shared_ptr<class BatchLogger> logger_;
    size_t currentBatchSize_ = 0;
    std::chrono::system_clock::time_point currentBatchTime_;
    mutable std::mutex mutex_;
};

// Batch logger interface for processing log batches
class BatchLogger {
public:
    virtual ~BatchLogger() = default;
    
    // Process a batch of log entries
    virtual void logBatch(const std::vector<EnhancedLogEntry>& entries) = 0;
    
    // Get logger statistics
    virtual struct LoggerStats {
        size_t totalBatches = 0;
        size_t totalEntries = 0;
        size_t batchesProcessed = 0;
        double averageBatchSize = 0.0;
        std::chrono::system_clock::time_point firstBatch;
        std::chrono::system_clock::time_point lastBatch;
    } getStatistics() const = 0;
    
    // Flush pending logs
    virtual void flush() = 0;
    
    // Close logger
    virtual void close() = 0;
};

// Memory tracker for memory usage logging
class MemoryTracker {
public:
    struct MemoryInfo {
        size_t allocatedBytes = 0;
        size_t allocatedObjects = 0;
        size_t peakAllocatedBytes = 0;
        std::chrono::system_clock::time_point lastAllocation;
        std::map<std::string, size_t> allocationsBySource;
    };
    
    static MemoryTracker& getInstance() {
        static MemoryTracker instance;
        return instance;
    }
    
    void trackAllocation(size_t size, const std::string& source = "") {
        std::lock_guard<std::mutex> lock(mutex_);
        info_.allocatedBytes += size;
        info_.allocatedObjects++;
        info_.peakAllocatedBytes = std::max(info_.peakAllocatedBytes, info_.allocatedBytes);
        info_.lastAllocation = std::chrono::system_clock::now();
        info_.allocationsBySource[source]++;
    }
    
    void trackDeallocation(size_t size, const std::string& source = "") {
        std::lock_guard<std::mutex> lock(mutex_);
        if (info_.allocatedBytes >= size) {
            info_.allocatedBytes -= size;
        }
        info_.allocatedObjects--;
        if (info_.allocationsBySource.find(source) != info_.allocationsBySource.end()) {
            info_.allocationsBySource[source]--;
            if (info_.allocationsBySource[source] == 0) {
                info_.allocationsBySource.erase(source);
            }
        }
    }
    
    MemoryInfo getInfo() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return info_;
    }
    
    // Check for memory issues
    bool checkMemoryHealth(size_t warnThreshold = 100 * 1024 * 1024, // 100 MB
                          size_t errorThreshold = 500 * 1024 * 1024) const { // 500 MB
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (info_.allocatedBytes > errorThreshold) {
            NLM_LOG_ERROR("Memory usage critical: " + std::to_string(info_.allocatedBytes) + 
                         " bytes (threshold: " + std::to_string(errorThreshold) + ")");
            return false;
        }
        
        if (info_.allocatedBytes > warnThreshold) {
            NLM_LOG_WARNING("Memory usage warning: " + std::to_string(info_.allocatedBytes) + 
                           " bytes (threshold: " + std::to_string(warnThreshold) + ")");
        }
        
        return true;
    }
    
private:
    MemoryTracker() = default;
    
    MemoryInfo info_;
    mutable std::mutex mutex_;
};

// Performance metrics collector for logging
class PerformanceMetrics {
public:
    struct Metric {
        std::string name;
        double value;
        std::string unit;
        std::chrono::system_clock::time_point timestamp;
        std::map<std::string, std::string> tags;
    };
    
    static PerformanceMetrics& getInstance() {
        static PerformanceMetrics instance;
        return instance;
    }
    
    void record(const std::string& name, double value,
                const std::string& unit = "",
                const std::map<std::string, std::string>& tags = {}) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        Metric metric;
        metric.name = name;
        metric.value = value;
        metric.unit = unit;
        metric.timestamp = std::chrono::system_clock::now();
        metric.tags = tags;
        
        metrics_.push_back(metric);
        
        // Limit metrics history
        if (metrics_.size() > maxMetrics_) {
            metrics_.erase(metrics_.begin());
        }
        
        // Log performance warnings
        if (value > warnThresholds_[name]) {
            NLM_LOG_WARNING("Performance metric \"" + name + "\" exceeds warning threshold: " +
                           std::to_string(value) + " " + unit +
                           " (warning: " + std::to_string(warnThresholds_[name]) + " " + unit + ")");
        }
        
        if (value > errorThresholds_[name]) {
            NLM_LOG_ERROR("Performance metric \"" + name + "\" exceeds error threshold: " +
                         std::to_string(value) + " " + unit +
                         " (error: " + std::to_string(errorThresholds_[name]) + " " + unit + ")");
        }
    }
    
    // Get recent metrics
    std::vector<Metric> getRecentMetrics(size_t limit = 100) const {
        std::lock_guard<std::mutex> lock(mutex_);
        if (limit > metrics_.size()) {
            limit = metrics_.size();
        }
        
        std::vector<Metric> recent;
        recent.reserve(limit);
        
        auto start = metrics_.end();
        std::advance(start, -static_cast<std::ptrdiff_t>(limit));
        
        for (auto it = start; it != metrics_.end(); ++it) {
            recent.push_back(*it);
        }
        
        return recent;
    }
    
    // Configure thresholds
    void setWarningThreshold(const std::string& name, double threshold) {
        std::lock_guard<std::mutex> lock(mutex_);
        warnThresholds_[name] = threshold;
    }
    
    void setErrorThreshold(const std::string& name, double threshold) {
        std::lock_guard<std::mutex> lock(mutex_);
        errorThresholds_[name] = threshold;
    }
    
private:
    PerformanceMetrics() = default;
    
    std::vector<Metric> metrics_;
    std::map<std::string, double> warnThresholds_;
    std::map<std::string, double> errorThresholds_;
    mutable std::mutex mutex_;
    static inline const size_t maxMetrics_ = 10000;
};

// Enhanced logger interface with memory and performance tracking
class EnhancedLogger {
public:
    virtual ~EnhancedLogger() = default;
    
    // Standard logging methods
    virtual void trace(const std::string& message,
                      const std::string& source = "",
                      const std::string& function = "",
                      const std::string& file = "",
                      int line = 0,
                      const std::map<std::string, std::string>& context = {}) = 0;
    
    virtual void debug(const std::string& message,
                      const std::string& source = "",
                      const std::string& function = "",
                      const std::string& file = "",
                      int line = 0,
                      const std::map<std::string, std::string>& context = {}) = 0;
    
    virtual void info(const std::string& message,
                     const std::string& source = "",
                     const std::string& function = "",
                     const std::string& file = "",
                     int line = 0,
                     const std::map<std::string, std::string>& context = {}) = 0;
    
    virtual void notice(const std::string& message,
                       const std::string& source = "",
                       const std::string& function = "",
                       const std::string& file = "",
                       int line = 0,
                       const std::map<std::string, std::string>& context = {}) = 0;
    
    virtual void warning(const std::string& message,
                        const std::string& source = "",
                        const std::string& function = "",
                        const std::string& file = "",
                        int line = 0,
                        const std::map<std::string, std::string>& context = {}) = 0;
    
    virtual void error(const std::string& message,
                      const std::string& source = "",
                      const std::string& function = "",
                      const std::string& file = "",
                      int line = 0,
                      const std::string& details = "",
                      const std::map<std::string, std::string>& context = {}) = 0;
    
    virtual void critical(const std::string& message,
                         const std::string& source = "",
                         const std::string& function = "",
                         const std::string& file = "",
                         int line = 0,
                         const std::string& details = "",
                         const std::map<std::string, std::string>& context = {}) = 0;
    
    virtual void alert(const std::string& message,
                      const std::string& source = "",
                      const std::string& function = "",
                      const std::string& file = "",
                      int line = 0,
                      const std::string& details = "",
                      const std::map<std::string, std::string>& context = {}) = 0;
    
    virtual void emergency(const std::string& message,
                          const std::string& source = "",
                          const std::string& function = "",
                          const std::string& file = "",
                          int line = 0,
                          const std::string& details = "",
                          const std::map<std::string, std::string>& context = {}) = 0;
    
    // Log with memory tracking
    void logWithMemory(const std::string& message,
                      const std::string& source = "",
                      const std::string& function = "",
                      const std::string& file = "",
                      int line = 0,
                      LogLevel level = LogLevel::Info) {
        // Get current memory usage
        auto& memoryTracker = MemoryTracker::getInstance();
        auto memInfo = memoryTracker.getInfo();n
        
        EnhancedLogEntry entry(level, message, source, function, file, line);
        entry.memoryUsage = memInfo.allocatedBytes;
        
        logEnhanced(entry);
    }
    
    // Log with performance context
    void logWithPerformance(const std::string& message,
                           const std::string& source = "",
                           const std::string& function = "",
                           const std::string& file = "",
                           int line = 0,
                           LogLevel level = LogLevel::Info,
                           const std::vector<std::string>& tags = {}) {
        EnhancedLogEntry entry(level, message, source, function, file, line);
        entry.tags = tags;
        
        // Add performance metrics if available
        auto& perfMetrics = PerformanceMetrics::getInstance();
        auto recentMetrics = perfMetrics.getRecentMetrics();
        for (const auto& metric : recentMetrics) {
            entry.context[metric.name] = std::to_string(metric.value);
        }
        
        logEnhanced(entry);
    }
    
    // Log error with full context
    void logErrorWithContext(const Error& error,
                           const std::map<std::string, std::string>& additionalContext = {}) {
        EnhancedLogEntry entry(error.getSeverity(), error.getMessage(), 
                              error.getSource(), error.getFunction(), 
                              error.getFile(), error.getLine());
        
        // Add error context
        entry.context["error_severity"] = std::to_string(static_cast<int>(error.getSeverity()));
        entry.context["error_category"] = std::to_string(static_cast<int>(error.getCategory()));
        entry.context["error_correlation"] = error.getCorrelationId();
        
        // Add additional context
        for (const auto& pair : additionalContext) {
            entry.context[pair.first] = pair.second;
        }
        
        // Add error details
        if (!error.getDetails().empty()) {
            entry.context["error_details"] = error.getDetails();
        }
        
        // Add recommendation
        if (!error.getRecommendation().empty()) {
            entry.context["error_recommendation"] = error.getRecommendation();
        }
        
        logEnhanced(entry);
    }
    
    virtual void logEnhanced(const EnhancedLogEntry& entry) = 0;
    
    // Get logger statistics
    virtual struct LoggerStats {
        size_t totalLogs = 0;
        std::map<LogLevel, size_t> logsByLevel;
        size_t totalMemoryTracked = 0;
        size_t totalPerformanceMetrics = 0;
        std::chrono::system_clock::time_point firstLog;
        std::chrono::system_clock::time_point lastLog;
    } getStatistics() const = 0;
    
    // Flush logs
    virtual void flush() = 0;
    
    // Close logger
    virtual void close() = 0;
};

// Performance monitoring for logging
class LoggingPerformanceMonitor {
public:
    struct PerformanceStats {
        double averageLogTime = 0.0;  // Average time to process a log entry
        double maxLogTime = 0.0;       // Maximum time to process a log entry
        double minLogTime = 0.0;       // Minimum time to process a log entry
        size_t totalLogs = 0;
        std::chrono::system_clock::time_point firstLog;
        std::chrono::system_clock::time_point lastLog;
    };
    
    static LoggingPerformanceMonitor& getInstance() {
        static LoggingPerformanceMonitor instance;
        return instance;
    }
    
    void recordLogTime(double timeMs) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        logTimes_.push_back(timeMs);
        totalLogTime_ += timeMs;
        
        if (!firstLog_.time_since_epoch().count()) {
            firstLog_ = std::chrono::system_clock::now();
        }
        lastLog_ = std::chrono::system_clock::now();
        
        if (logTimes_.size() > maxLogTimes_) {
            logTimes_.erase(logTimes_.begin());
        }
        
        // Log performance warning
        if (timeMs > warnLogTimeMs_) {
            NLM_LOG_WARNING("Log processing time warning: " + std::to_string(timeMs) + 
                           " ms (warning threshold: " + std::to_string(warnLogTimeMs_) + " ms)");
        }
        
        if (timeMs > errorLogTimeMs_) {
            NLM_LOG_ERROR("Log processing time error: " + std::to_string(timeMs) + 
                         " ms (error threshold: " + std::to_string(errorLogTimeMs_) + " ms)");
        }
    }
    
    PerformanceStats getStatistics() const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        PerformanceStats stats;
        stats.totalLogs = logTimes_.size();
        
        if (stats.totalLogs > 0) {
            stats.averageLogTime = totalLogTime_ / stats.totalLogs;
            stats.maxLogTime = *std::max_element(logTimes_.begin(), logTimes_.end());
            stats.minLogTime = *std::min_element(logTimes_.begin(), logTimes_.end());
        }
        
        stats.firstLog = firstLog_;
        stats.lastLog = lastLog_;
        
        return stats;
    }
    
    // Configure thresholds
    void setWarningThreshold(double timeMs) {
        std::lock_guard<std::mutex> lock(mutex_);
        warnLogTimeMs_ = timeMs;
    }
    
    void setErrorThreshold(double timeMs) {
        std::lock_guard<std::mutex> lock(mutex_);
        errorLogTimeMs_ = timeMs;
    }
    
private:
    LoggingPerformanceMonitor() = default;
    
    std::vector<double> logTimes_;
    double totalLogTime_ = 0.0;
    size_t maxLogTimes_ = 1000;
    std::chrono::system_clock::time_point firstLog_;
    std::chrono::system_clock::time_point lastLog_;
    double warnLogTimeMs_ = 10.0;  // 10 ms
    double errorLogTimeMs_ = 100.0;  // 100 ms
    mutable std::mutex mutex_;
};

// Global logging setup
namespace Logging {
    // Initialize logging system
    void initialize() {
        // Initialize performance monitor
        LoggingPerformanceMonitor::getInstance();
        
        // Initialize memory tracker
        MemoryTracker::getInstance();
        
        // Initialize performance metrics
        PerformanceMetrics::getInstance();
    }
    
    // Get memory tracker
    MemoryTracker::MemoryInfo getMemoryInfo() {
        return MemoryTracker::getInstance().getInfo();
    }
    
    // Get performance metrics
    PerformanceMetrics::Metric recordMetric(const std::string& name, double value,
                                           const std::string& unit = "",
                                           const std::map<std::string, std::string>& tags = {}) {
        PerformanceMetrics::Metric metric;
        metric.name = name;
        metric.value = value;
        metric.unit = unit;
        metric.timestamp = std::chrono::system_clock::now();
        metric.tags = tags;
        
        PerformanceMetrics::getInstance().record(name, value, unit, tags);
        return metric;
    }
    
    // Get logging performance
    LoggingPerformanceMonitor::PerformanceStats getPerformanceStats() {
        return LoggingPerformanceMonitor::getInstance().getStatistics();
    }
    
    // Log system state
    void logSystemState() {
        NLM_LOG_INFO("=== System State ===");
        
        // Log memory information
        auto& memoryTracker = MemoryTracker::getInstance();
        auto memInfo = memoryTracker.getInfo();
        NLM_LOG_INFO("Memory: " + std::to_string(memInfo.allocatedBytes) + 
                    " bytes allocated, " + std::to_string(memInfo.allocatedObjects) + 
                    " objects (peak: " + std::to_string(memInfo.peakAllocatedBytes) + ")");
        
        // Log performance metrics
        auto& perfMetrics = PerformanceMetrics::getInstance();
        auto recentMetrics = perfMetrics.getRecentMetrics();
        NLM_LOG_INFO("Performance metrics: " + std::to_string(recentMetrics.size()) + " recent metrics");
        
        // Log logging performance
        auto& logPerf = LoggingPerformanceMonitor::getInstance();
        auto perfStats = logPerf.getStatistics();
        NLM_LOG_INFO("Logging performance: " + std::to_string(perfStats.averageLogTime) + 
                    " ms average log time");
        
        NLM_LOG_INFO("======================");
    }
    
    // Graceful shutdown
    void shutdown() {
        NLM_LOG_INFO("Shutting down logging system");
        
        // Log final statistics
        logSystemState();
    }
}

// Logging macros with enhanced features
#define NLM_LOG_WITH_MEMORY(level, msg, ...) \
    do { \
        nlm::EnhancedLogger::logWithMemory(level, msg, __VA_ARGS__); \
    } while(0)

#define NLM_LOG_WITH_PERFORMANCE(level, msg, tags, ...) \
    do { \
        nlm::EnhancedLogger::logWithPerformance(level, msg, tags, __VA_ARGS__); \
    } while(0)

#define NLM_LOG_ERROR_WITH_CONTEXT(msg, details, recommendation) \
    do { \
        Error error = Error(ErrorSeverity::Error, msg, ErrorCategory::None, __func__, __FILE__, __LINE__, details, recommendation); \
        EnhancedLogger::logErrorWithContext(error); \
    } while(0)

#define NLM_LOG_WARNING_WITH_CONTEXT(msg, details) \
    do { \
        Error error = Error(ErrorSeverity::Warning, msg, ErrorCategory::None, __func__, __FILE__, __LINE__, details); \
        EnhancedLogger::logErrorWithContext(error); \
    } while(0)

#define NLM_LOG_CRITICAL_WITH_CONTEXT(msg, details, recommendation) \
    do { \
        Error error = Error(ErrorSeverity::Critical, msg, ErrorCategory::None, __func__, __FILE__, __LINE__, details, recommendation); \
        EnhancedLogger::logErrorWithContext(error); \
    } while(0)

// Memory allocation tracking macros
#define NLM_TRACK_ALLOCATION(size, source) MemoryTracker::getInstance().trackAllocation(size, source)
#define NLM_TRACK_DEALLOCATION(size, source) MemoryTracker::getInstance().trackDeallocation(size, source)

// Performance metrics macros
#define NLM_RECORD_METRIC(name, value, unit) PerformanceMetrics::getInstance().record(name, value, unit)
#define NLM_RECORD_METRIC_WITH_TAGS(name, value, unit, tags) PerformanceMetrics::getInstance().record(name, value, unit, tags)

// System state logging macro
#define NLM_LOG_SYSTEM_STATE() Logging::logSystemState()

// Memory health check macro
#define NLM_CHECK_MEMORY_HEALTH(warnThreshold, errorThreshold) \
    MemoryTracker::getInstance().checkMemoryHealth(warnThreshold, errorThreshold)

// Performance threshold check macro
#define NLM_CHECK_PERFORMANCE_THRESHOLD(name, value, warnThreshold, errorThreshold) \
    do { \
        if (value > errorThreshold) { \
            NLM_LOG_ERROR("Performance threshold exceeded for \"" + std::string(name) + "\": " + \
                         std::to_string(value) + " (error threshold: " + std::to_string(errorThreshold) + ")"); \
        } else if (value > warnThreshold) { \
            NLM_LOG_WARNING("Performance threshold exceeded for \"" + std::string(name) + "\": " + \
                           std::to_string(value) + " (warning threshold: " + std::to_string(warnThreshold) + ")"); \
        } \
    } while(0)

} // namespace Logging

// Global initialization for logging system
namespace {
    class LoggingInitializer {
    public:
        LoggingInitializer() {
            Logging::initialize();
        }
    };
    
    LoggingInitializer loggingInitializer_;
} // anonymous namespace

} // namespace nlm