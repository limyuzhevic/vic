#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <vector>
#include <chrono>
#include <mutex>
#include <unordered_map>
#include <shared_mutex>
#include <variant>
#include <functional>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <deque>
#include <algorithm>
#include <filesystem>

// Platform-specific headers
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

namespace nlm {

// Enhanced LogLevel enum with additional levels
enum class LogLevel {
    Trace = -1,
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4,
    Alert = 5,
    Emergency = 6,
    Off = 999
};

// Log entry with enhanced metadata
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::string threadId;
    std::string processId;
    std::string module;
    uint64_t correlationId;
    
    // Performance metrics
    std::chrono::microseconds executionTime;
    uint64_t memoryUsage;
    
    // Structured data
    std::unordered_map<std::string, std::string> fields;
    std::unordered_map<std::string, double> numericFields;
    
    // Custom context
    std::unordered_map<std::string, std::string> context;
    
    // Exception information (if any)
    std::string exceptionType;
    std::string exceptionMessage;
    std::string stackTrace;
};

// Log handler interface
class ILogHandler {
public:
    virtual ~ILogHandler() = default;
    virtual void handleLog(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    virtual void flush() = 0;
    virtual void configure(const std::unordered_map<std::string, std::string>& config) = 0;
};

// Structured formatter interface
class ILogFormatter {
public:
    virtual ~ILogFormatter() = default;
    virtual std::string format(const LogEntry& entry) = 0;
    virtual std::string formatStructured(const LogEntry& entry) = 0;
    virtual std::string getSupportedFormat() const = 0;
};

// Console handler with enhanced formatting
class ConsoleLogHandler : public ILogHandler {
public:
    ConsoleLogHandler() : level_(LogLevel::Info), useColors_(true), useEmoji_(false) {
        initializeColors();
    }
    
    explicit ConsoleLogHandler(LogLevel level) : level_(level), useColors_(true), useEmoji_(false) {
        initializeColors();
    }
    
    void handleLog(const LogEntry& entry) override {
        if (entry.level < level_) return;
        
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::string formatted;
        if (useColors_) {
            formatted = formatWithColor(entry);
        } else {
            formatted = formatPlain(entry);
        }
        
        std::cout << formatted << std::endl;
        
        if (logToFile_ && fileOut_.is_open()) {
            fileOut_ << formatted << std::endl;
            fileOut_.flush();
        }
    }
    
    void setLevel(LogLevel level) override { level_ = level; }
    LogLevel getLevel() const override { return level_; }
    
    void flush() override {
        std::lock_guard<std::mutex> lock(mutex_);
        if (fileOut_.is_open()) {
            fileOut_.flush();
        }
    }
    
    void configure(const std::unordered_map<std::string, std::string>& config) override {
        auto it = config.find("level");
        if (it != config.end()) {
            setLevel(stringToLevel(it->second));
        }
        
        it = config.find("colors");
        if (it != config.end()) {
            useColors_ = (it->second == "true" || it->second == "1" || it->second == "yes");
        }
        
        it = config.find("emoji");
        if (it != config.end()) {
            useEmoji_ = (it->second == "true" || it->second == "1" || it->second == "yes");
        }
        
        it = config.find("file");
        if (it != config.end()) {
            setOutputFile(it->second);
        }
    }
    
    void setUseColors(bool use) { useColors_ = use; }
    bool getUseColors() const { return useColors_; }
    
    void setUseEmoji(bool use) { useEmoji_ = use; }
    bool getUseEmoji() const { return useEmoji_; }
    
    void setOutputFile(const std::string& filepath) {
        closeOutputFile();
        if (!filepath.empty()) {
            fileOut_.open(filepath, std::ios::app);
            logToFile_ = fileOut_.is_open();
        }
    }
    
    void closeOutputFile() {
        if (fileOut_.is_open()) {
            fileOut_.close();
            logToFile_ = false;
        }
    }
    
    // Set rotation parameters
    void setMaxFileSize(size_t maxSize) { maxFileSize_ = maxSize; }
    void setMaxFiles(int maxFiles) { maxFiles_ = maxFiles; }
    
    // Check if rotation is needed
    bool needsRotation() const {
        return logToFile_ && fileOut_.is_open() && 
               fileOut_.tellp() >= static_cast<std::streamoff>(maxFileSize_);
    }
    
    void rotateIfNeeded() {
        if (needsRotation()) {
            closeOutputFile();
            rotateFile();
        }
    }
    
private:
    struct Impl {
        std::mutex mutex;
        std::unordered_map<LogLevel, std::string> colors;
        std::unordered_map<LogLevel, std::string> emojis;
    };
    
    std::unique_ptr<Impl> pImpl;
    std::mutex mutex_;
    LogLevel level_;
    bool useColors_;
    bool useEmoji_;
    bool logToFile_ = false;
    std::ofstream fileOut_;
    size_t maxFileSize_ = 10 * 1024 * 1024;  // 10 MB
    int maxFiles_ = 5;
    
    void initializeColors() {
        pImpl->colors[LogLevel::Trace] = "\033[90m";     // Gray
        pImpl->colors[LogLevel::Debug] = "\033[36m";     // Cyan
        pImpl->colors[LogLevel::Info] = "\033[32m";      // Green
        pImpl->colors[LogLevel::Warning] = "\033[33m";   // Yellow
        pImpl->colors[LogLevel::Error] = "\033[31m";     // Red
        pImpl->colors[LogLevel::Critical] = "\033[35m"; // Magenta
        pImpl->colors[LogLevel::Alert] = "\033[91m";     // Bright Red
        pImpl->colors[LogLevel::Emergency] = "\033[95m"; // Bright Magenta
        
        pImpl->emojis[LogLevel::Trace] = "🔍 ";
        pImpl->emojis[LogLevel::Debug] = "🐛 ";
        pImpl->emojis[LogLevel::Info] = "ℹ️ ";
        pImpl->emojis[LogLevel::Warning] = "⚠️ ";
        pImpl->emojis[LogLevel::Error] = "❌ ";
        pImpl->emojis[LogLevel::Critical] = "💥 ";
        pImpl->emojis[LogLevel::Alert] = "🚨 ";
        pImpl->emojis[LogLevel::Emergency] = "🔥 ";
    }
    
    std::string formatWithColor(const LogEntry& entry) {
        std::ostringstream oss;
        
        // Timestamp
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        
        // Level with color and emoji
        oss << " " << pImpl->colors[entry.level];
        if (useEmoji_) {
            oss << pImpl->emojis[entry.level];
        }
        oss << levelToString(entry.level) << " ";
        oss << "\033[0m"; // Reset
        
        // Thread ID
        oss << "[" << entry.threadId << "] ";
        
        // Module
        if (!entry.module.empty()) {
            oss << "[" << entry.module << "] ";
        }
        
        // Message
        oss << entry.message;
        
        // Performance metrics
        if (entry.executionTime.count() > 0) {
            oss << " [t: " << entry.executionTime.count() << "μs]";
        }
        if (entry.memoryUsage > 0) {
            oss << " [mem: " << entry.memoryUsage << "B]";
        }
        
        // File and line
        if (!entry.file.empty() && entry.line > 0) {
            // Show just the filename, not full path
            size_t pos = entry.file.find_last_of("/\\");
            std::string filename = (pos != std::string::npos) ? entry.file.substr(pos + 1) : entry.file;
            oss << " (" << filename << ":" << entry.line << ")";
        }
        
        return oss.str();
    }
    
    std::string formatPlain(const LogEntry& entry) {
        std::ostringstream oss;
        
        // Timestamp
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        
        // Level
        oss << " " << levelToString(entry.level) << " ";
        
        // Thread ID
        oss << "[" << entry.threadId << "] ";
        
        // Module
        if (!entry.module.empty()) {
            oss << "[" << entry.module << "] ";
        }
        
        // Message
        oss << entry.message;
        
        // Performance metrics
        if (entry.executionTime.count() > 0) {
            oss << " [t: " << entry.executionTime.count() << "μs]";
        }
        
        // Fields
        for (const auto& field : entry.fields) {
            oss << " " << field.first << "=" << field.second;
        }
        
        return oss.str();
    }
    
    std::string levelToString(LogLevel level) const {
        switch (level) {
            case LogLevel::Trace: return "TRACE";
            case LogLevel::Debug: return "DEBUG";
            case LogLevel::Info: return "INFO";
            case LogLevel::Warning: return "WARN";
            case LogLevel::Error: return "ERROR";
            case LogLevel::Critical: return "CRIT";
            case LogLevel::Alert: return "ALERT";
            case LogLevel::Emergency: return "EMERG";
            default: return "UNKNOWN";
        }
    }
    
    LogLevel stringToLevel(const std::string& levelStr) const {
        std::string upper = levelStr;
        std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
        
        if (upper == "TRACE") return LogLevel::Trace;
        if (upper == "DEBUG") return LogLevel::Debug;
        if (upper == "INFO") return LogLevel::Info;
        if (upper == "WARN" || upper == "WARNING") return LogLevel::Warning;
        if (upper == "ERROR") return LogLevel::Error;
        if (upper == "CRIT" || upper == "CRITICAL") return LogLevel::Critical;
        if (upper == "ALERT") return LogLevel::Alert;
        if (upper == "EMERG" || upper == "EMERGENCY") return LogLevel::Emergency;
        if (upper == "OFF") return LogLevel::Off;
        
        return LogLevel::Info;  // Default
    }
    
    void rotateFile() {
        std::string filename = fileOut_.rdbuf()->pubseekoff(0, std::ios::out).get();
        // Extract base filename
        size_t pos = filename.find_last_of("/\\");
        std::string basename = (pos != std::string::npos) ? filename.substr(pos + 1) : filename;
        
        // Close current file
        closeOutputFile();
        
        // Rotate existing files
        for (int i = maxFiles_ - 1; i >= 0; --i) {
            std::string oldName;
            if (i == maxFiles_ - 1) {
                oldName = filename;  // Current file
            } else if (i == 0) {
                oldName = filename + ".1";  // First backup
            } else {
                oldName = filename + "." + std::to_string(i);
            }
            
            std::string newName = filename + "." + std::to_string(i + 1);
            
            if (std::filesystem::exists(oldName)) {
                std::filesystem::rename(oldName, newName);
            }
        }
        
        // Reopen for new log
        fileOut_.open(filename, std::ios::app);
    }
};

// JSON formatter
class JSONLogFormatter : public ILogFormatter {
public:
    std::string format(const LogEntry& entry) override {
        std::ostringstream oss;
        oss << "{\n";
        oss << "  \"timestamp\": \"" << timestampToString(entry.timestamp) << "\",\n";
        oss << "  \"level\": \"" << levelToString(entry.level) << "\",\n";
        oss << "  \"threadId\": \"" << entry.threadId << "\",\n";
        oss << "  \"module\": \"" << entry.module << "\",\n";
        oss << "  \"message\": \"" << escapeString(entry.message) << "\",\n";
        oss << "  \"file\": \"" << escapeString(entry.file) << "\",\n";
        oss << "  \"line\": " << entry.line << ",\n";
        oss << "  \"function\": \"" << escapeString(entry.function) << "\",\n";
        
        if (!entry.exceptionType.empty()) {
            oss << "  \"exception\": {\n";
            oss << "    \"type\": \"" << escapeString(entry.exceptionType) << "\",\n";
            oss << "    \"message\": \"" << escapeString(entry.exceptionMessage) << "\",\n";
            oss << "    \"stackTrace\": \"" << escapeString(entry.stackTrace) << "\"\n";
            oss << "  },\n";
        }
        
        if (!entry.fields.empty()) {
            oss << "  \"fields\": {\n";
            bool first = true;
            for (const auto& field : entry.fields) {
                if (!first) oss << ",\n";
                oss << "    \"" << escapeString(field.first) << "\": \"" << escapeString(field.second) << "\"";
                first = false;
            }
            oss << "\n  },\n";
        }
        
        if (!entry.context.empty()) {
            oss << "  \"context\": {\n";
            bool first = true;
            for (const auto& ctx : entry.context) {
                if (!first) oss << ",\n";
                oss << "    \"" << escapeString(ctx.first) << "\": \"" << escapeString(ctx.second) << "\"";
                first = false;
            }
            oss << "\n  },\n";
        }
        
        if (entry.executionTime.count() > 0) {
            oss << "  \"executionTimeUs\": " << entry.executionTime.count() << ",\n";
        }
        
        if (entry.memoryUsage > 0) {
            oss << "  \"memoryUsage\": " << entry.memoryUsage << ",\n";
        }
        
        oss << "}\n";
        return oss.str();
    }
    
    std::string formatStructured(const LogEntry& entry) override {
        return format(entry);
    }
    
    std::string getSupportedFormat() const override { return "json"; }
    
private:
    std::string timestampToString(const std::chrono::system_clock::time_point& tp) {
        auto time = std::chrono::system_clock::to_time_t(tp);
        std::tm tm_time;
        gmtime_r(&time, &tm_time);
        
        std::ostringstream oss;
        oss << std::put_time(&tm_time, "%Y-%m-%dT%H:%M:%SZ");
        return oss.str();
    }
    
    std::string escapeString(const std::string& str) {
        std::ostringstream oss;
        for (char c : str) {
            switch (c) {
                case '\\': oss << "\\\\"; break;
                case '\"': oss << "\\\""; break;
                case '\n': oss << "\\n"; break;
                case '\r': oss << "\\r"; break;
                case '\t': oss << "\\t"; break;
                case '\b': oss << "\\b"; break;
                case '\f': oss << "\\f"; break;
                default: oss << c; break;
            }
        }
        return oss.str();
    }
    
    std::string levelToString(LogLevel level) const {
        switch (level) {
            case LogLevel::Trace: return "TRACE";
            case LogLevel::Debug: return "DEBUG";
            case LogLevel::Info: return "INFO";
            case LogLevel::Warning: return "WARNING";
            case LogLevel::Error: return "ERROR";
            case LogLevel::Critical: return "CRITICAL";
            case LogLevel::Alert: return "ALERT";
            case LogLevel::Emergency: return "EMERGENCY";
            default: return "UNKNOWN";
        }
    }
};

// Performance logger with timing
class PerformanceLogger {
public:
    PerformanceLogger(const std::string& module, uint64_t correlationId = 0)
        : module_(module), correlationId_(correlationId) {
        startTime_ = std::chrono::high_resolution_clock::now();
        activeTimers_.push_back(startTime_);
    }
    
    ~PerformanceLogger() {
        if (!activeTimers_.empty()) {
            activeTimers_.pop_back();
        }
        logPerformance();
    }
    
    // Start a new timing context
    void startTiming(const std::string& operation) {
        std::lock_guard<std::mutex> lock(mutex_);
        TimerInfo timer;
        timer.operation = operation;
        timer.startTime = std::chrono::high_resolution_clock::now();
        timer.correlationId = correlationId_;
        timer.module = module_;
        activeTimers_.push_back(timer.startTime);
    }
    
    // End timing and log the result
    void endTiming(const std::string& operation, LogLevel level = LogLevel::Info) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (activeTimers_.size() < 2) {
            return;  // No timing context to end
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto startTime = std::prev(activeTimers_.end())->startTime;
        
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        
        LogEntry entry;
        entry.level = level;
        entry.message = "Performance: " + operation + " took " + std::to_string(duration.count()) + " μs";
        entry.module = module_;
        entry.correlationId = correlationId_;
        entry.executionTime = duration;
        
        // Log to logger
        if (logger_) {
            logger_->log(entry);
        }
        
        activeTimers_.pop_back();
    }
    
    // Set the logger to use
    void setLogger(std::shared_ptr<Logger> logger) { logger_ = logger; }
    
    // Get current correlation ID
    uint64_t getCorrelationId() const { return correlationId_; }
    
    // Generate new correlation ID
    static uint64_t generateCorrelationId() {
        static std::atomic<uint64_t> idCounter{0};
        return ++idCounter;
    }
    
private:
    void logPerformance() {
        if (activeTimers_.size() > 1) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto startTime = activeTimers_.front().startTime;
            
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
            
            LogEntry entry;
            entry.level = LogLevel::Debug;
            entry.message = "Operation completed in " + std::to_string(duration.count()) + " μs";
            entry.module = module_;
            entry.correlationId = correlationId_;
            entry.executionTime = duration;
            
            if (logger_) {
                logger_->log(entry);
            }
        }
    }
    
    struct TimerInfo {
        std::string operation;
        std::chrono::high_resolution_clock::time_point startTime;
        uint64_t correlationId;
        std::string module;
    };
    
    std::string module_;
    uint64_t correlationId_;
    std::chrono::high_resolution_clock::time_point startTime_;
    std::vector<TimerInfo> activeTimers_;
    std::shared_ptr<Logger> logger_;
    mutable std::mutex mutex_;
};

// Memory tracker for garbage collection logging
class MemoryTracker {
public:
    struct MemoryStats {
        size_t totalAllocated;
        size_t currentAllocated;
        size_t peakAllocated;
        uint64_t allocationCount;
        uint64_t deallocationCount;
        std::chrono::system_clock::time_point lastGC;
        size_t gcCollectionCount;
        double memoryFragmentation;
    };
    
    MemoryTracker() {
        initializeCounters();
        lastGCTime_ = std::chrono::system_clock::now();
    }
    
    void recordAllocation(size_t size) {
        std::lock_guard<std::mutex> lock(mutex_);
        stats_.totalAllocated += size;
        stats_.currentAllocated += size;
        stats_.peakAllocated = std::max(stats_.peakAllocated, stats_.currentAllocated);
        stats_.allocationCount++;
    }
    
    void recordDeallocation(size_t size) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (size <= stats_.currentAllocated) {
            stats_.currentAllocated -= size;
        }
        stats_.deallocationCount++;
    }
    
    void recordGC(std::chrono::system_clock::time_point time, size_t collected, size_t generations) {
        std::lock_guard<std::mutex> lock(mutex_);
        stats_.lastGC = time;
        stats_.gcCollectionCount += collected;
        
        // Calculate fragmentation
        if (stats_.totalAllocated > 0) {
            stats_.memoryFragmentation = 
                1.0 - (static_cast<double>(stats_.currentAllocated) / stats_.totalAllocated);
        }
    }
    
    MemoryStats getStats() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return stats_;
    }
    
    std::string formatStats() const {
std::ostringstream oss;
    oss << "Memory: " << stats_.currentAllocated << "B (peak: " << stats_.peakAllocated
        << "B, allocs: " << stats_.allocationCount << ")";
    return oss.str();
    }
    
private:
    void initializeCounters() {
        stats_.totalAllocated = 0;
        stats_.currentAllocated = 0;
        stats_.peakAllocated = 0;
        stats_.allocationCount = 0;
        stats_.deallocationCount = 0;
        stats_.lastGC = std::chrono::system_clock::now();
        stats_.gcCollectionCount = 0;
        stats_.memoryFragmentation = 0.0;
    }
    
    std::string timeToString(const std::chrono::system_clock::time_point& tp) {
        auto time = std::chrono::system_clock::to_time_t(tp);
        std::tm tm_time;
        gmtime_r(&time, &tm_time);
        
        std::ostringstream oss;
        oss << std::put_time(&tm_time, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }
    
    mutable std::mutex mutex_;
    MemoryStats stats_;
    std::chrono::system_clock::time_point lastGCTime_;
};

// Correlation ID manager for distributed logging
class CorrelationManager {
public:
    CorrelationManager() {
        currentCorrelationId_ = PerformanceLogger::generateCorrelationId();
    }
    
    uint64_t getCurrentCorrelationId() const { return currentCorrelationId_; }
    
    void setCurrentCorrelationId(uint64_t id) { currentCorrelationId_ = id; }
    
    uint64_t generateNewCorrelationId() {
        return PerformanceLogger::generateCorrelationId();
    }
    
    // Start a new correlation context
    void startCorrelation(const std::string& context) {
        correlationContexts_.push_back({generateNewCorrelationId(), context, std::chrono::system_clock::now()});
        currentCorrelationId_ = correlationContexts_.back().id;
    }
    
    // End current correlation context
    void endCorrelation() {
        if (!correlationContexts_.empty()) {
            correlationContexts_.pop_back();
            if (!correlationContexts_.empty()) {
                currentCorrelationId_ = correlationContexts_.back().id;
            } else {
                currentCorrelationId_ = PerformanceLogger::generateCorrelationId();
            }
        }
    }
    
    // Get correlation context stack
    std::vector<std::pair<uint64_t, std::string>> getCorrelationContext() const {
        std::vector<std::pair<uint64_t, std::string>> result;
        for (const auto& ctx : correlationContexts_) {
            result.push_back({ctx.id, ctx.context});
        }
        return result;
    }
    
private:
    struct CorrelationContext {
        uint64_t id;
        std::string context;
        std::chrono::system_clock::time_point startTime;
    };
    
    uint64_t currentCorrelationId_;
    std::vector<CorrelationContext> correlationContexts_;
    mutable std::mutex mutex_;
};

// Global correlation manager
inline CorrelationManager& getCorrelationManager() {
    static CorrelationManager manager;
    return manager;
}

// Enhanced Logger class with enterprise features
class Logger {
public:
    Logger() {
        initialize();
    }
    
    ~Logger() {
        shutdown();
    }
    
    void initialize() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Create default handlers
        auto consoleHandler = std::make_shared<ConsoleLogHandler>(LogLevel::Info);
        consoleHandler->configure({{"colors", "true"}, {"emoji", "true"}});
        
        // Add to handlers
        handlers_.push_back(consoleHandler);
        
        // Set default level
        setLevel(LogLevel::Info);
        
        // Initialize performance logger
        performanceLogger_.setLogger(shared_from_this());
        
        // Initialize memory tracker
        memoryTracker_ = std::make_unique<MemoryTracker>();
        
        // Set thread info for logging
        updateThreadInfo();
    }
    
    void shutdown() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Flush all handlers
        for (auto& handler : handlers_) {
            handler->flush();
        }
        
        // Clear handlers
        handlers_.clear();
    }
    
    // Add log handler
    void addHandler(std::shared_ptr<ILogHandler> handler) {
        std::lock_guard<std::mutex> lock(mutex_);
        handlers_.push_back(handler);
    }
    
    // Remove log handler
    void removeHandler(std::shared_ptr<ILogHandler> handler) {
        std::lock_guard<std::mutex> lock(mutex_);
        handlers_.erase(std::remove(handlers_.begin(), handlers_.end(), handler), handlers_.end());
    }
    
    // Set log level
    void setLevel(LogLevel level) {
        std::lock_guard<std::mutex> lock(mutex_);
        currentLevel_ = level;
        for (auto& handler : handlers_) {
            handler->setLevel(level);
        }
    }
    
    // Log entry with full metadata
    void log(LogLevel level, const std::string& message,
             const std::string& file = "", int line = 0,
             const std::string& function = "",
             const std::string& module = "",
             const std::unordered_map<std::string, std::string>& fields = {}) {
        
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Check if level is enabled
        if (level < currentLevel_) return;
        
        LogEntry entry;
        entry.level = level;
        entry.message = message;
        entry.file = file;
        entry.line = line;
        entry.function = function;
        entry.timestamp = std::chrono::system_clock::now();
        entry.threadId = currentThreadId_;
        entry.processId = currentProcessId_;
        entry.module = module;
        entry.correlationId = getCorrelationManager().getCurrentCorrelationId();
        entry.fields = fields;
        
        // Log to all handlers
        for (auto& handler : handlers_) {
            handler->handleLog(entry);
        }
    }
    
    // Convenience logging methods with structured data
    void debug(const std::string& message, const std::string& file = "",
               int line = 0, const std::string& function = "",
               const std::string& module = "",
               const std::unordered_map<std::string, std::string>& fields = {}) {
        log(LogLevel::Debug, message, file, line, function, module, fields);
    }
    
    void info(const std::string& message, const std::string& file = "",
              int line = 0, const std::string& function = "",
              const std::string& module = "",
              const std::unordered_map<std::string, std::string>& fields = {}) {
        log(LogLevel::Info, message, file, line, function, module, fields);
    }
    
    void warning(const std::string& message, const std::string& file = "",
                 int line = 0, const std::string& function = "",
                 const std::string& module = "",
                 const std::unordered_map<std::string, std::string>& fields = {}) {
        log(LogLevel::Warning, message, file, line, function, module, fields);
    }
    
    void error(const std::string& message, const std::string& file = "",
               int line = 0, const std::string& function = "",
               const std::string& module = "",
               const std::unordered_map<std::string, std::string>& fields = {}) {
        log(LogLevel::Error, message, file, line, function, module, fields);
    }
    
    void critical(const std::string& message, const std::string& file = "",
                  int line = 0, const std::string& function = "",
                  const std::string& module = "",
                  const std::unordered_map<std::string, std::string>& fields = {}) {
        log(LogLevel::Critical, message, file, line, function, module, fields);
    }
    
    // Performance logging with timing
    PerformanceLogger startPerformanceLog(const std::string& module) {
        return PerformanceLogger(module, getCorrelationManager().getCurrentCorrelationId());
    }
    
    // Memory tracking
    void logMemoryStats(const std::string& context = "") {
        if (memoryTracker_) {
            auto stats = memoryTracker_->getStats();
            std::ostringstream oss;
            oss << context << " " << memoryTracker_->formatStats();
            
            LogEntry entry;
            entry.level = LogLevel::Info;
            entry.message = oss.str();
            entry.module = "MemoryTracker";
            entry.correlationId = getCorrelationManager().getCurrentCorrelationId();
            
            log(entry);
        }
    }
    
    // System health check
    bool performHealthCheck() {
        bool healthy = true;
        std::string healthMessage = "Health Check: ";
        
        // Check if error reporter has errors
        auto& errorReporter = getErrorReporter();
        auto stats = errorReporter.getSystemStats();
        
        if (stats.unrecoveredErrors > 100) {  // Threshold
            healthMessage += "HIGH_ERROR_RATE; ";
            healthy = false;
        }
        
        if (memoryTracker_) {
            auto memStats = memoryTracker_->getStats();nif (memStats.memoryFragmentation > 0.5) {  // 50% fragmentation threshold
                healthMessage += "HIGH_FRAGMENTATION; ";
                healthy = false;
            }
        }
        
        LogEntry entry;
        entry.level = healthy ? LogLevel::Info : LogLevel::Warning;
        entry.message = healthMessage;
        entry.module = "HealthMonitor";
        entry.correlationId = getCorrelationManager().getCurrentCorrelationId();
        
        log(entry);
        
        return healthy;
    }
    
    // Get global logger instance
    static Logger& getGlobal() {
        static Logger globalLogger;
        return globalLogger;
    }
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger) {
        // In practice, we just use getGlobal() which is already static
    }
    
private:
    void updateThreadInfo() {
        std::ostringstream threadIdOss;
        threadIdOss << std::this_thread::get_id();
        currentThreadId_ = threadIdOss.str();
        
        // Get process ID
        currentProcessId_ = std::to_string(getpid());
    }
    
    mutable std::mutex mutex_;
    std::vector<std::shared_ptr<ILogHandler>> handlers_;
    LogLevel currentLevel_ = LogLevel::Info;
    std::string currentThreadId_;
    std::string currentProcessId_;
    std::unique_ptr<MemoryTracker> memoryTracker_;
    PerformanceLogger performanceLogger_;
    std::unordered_map<std::string, std::string> levelStrings_;
};

// Memory tracking macros
#define NLM_TRACK_ALLOCATION(size) nlm::getMemoryTracker().recordAllocation(size)
#define NLM_TRACK_DEALLOCATION(size) nlm::getMemoryTracker().recordDeallocation(size)
#define NLM_TRACK_GC(collected, generations) nlm::getMemoryTracker().recordGC(std::chrono::system_clock::now(), collected, generations)

// Performance logging macros
#define NLM_PERFORMANCE_LOG(module) nlm::PerformanceLogger performanceLogger(module)
#define NLM_PERFORMANCE_START(module, operation) performanceLogger.startTiming(operation)
#define NLM_PERFORMANCE_END(operation) performanceLogger.endTiming(operation)

// Correlation ID macros
#define NLM_CORRELATION_START(context) nlm::getCorrelationManager().startCorrelation(context)
#define NLM_CORRELATION_END() nlm::getCorrelationManager().endCorrelation()

// Health check macro
#define NLM_HEALTH_CHECK() nlm::Logger::getGlobal().performHealthCheck()

// Structured logging macros with fields
#define NLM_LOG_WITH_FIELDS(level, message, ...) \
    do { \
        std::unordered_map<std::string, std::string> fields = {__VA_ARGS__}; \
        nlm::Logger::getGlobal().log(level, message, __FILE__, __LINE__, __func__, "", fields); \
    } while(0)

#define NLM_LOG_DEBUG_WITH_FIELDS(message, ...) NLM_LOG_WITH_FIELDS(nlm::LogLevel::Debug, message, __VA_ARGS__)
#define NLM_LOG_INFO_WITH_FIELDS(message, ...) NLM_LOG_WITH_FIELDS(nlm::LogLevel::Info, message, __VA_ARGS__)
#define NLM_LOG_WARNING_WITH_FIELDS(message, ...) NLM_LOG_WITH_FIELDS(nlm::LogLevel::Warning, message, __VA_ARGS__)
#define NLM_LOG_ERROR_WITH_FIELDS(message, ...) NLM_LOG_WITH_FIELDS(nlm::LogLevel::Error, message, __VA_ARGS__)

// Global memory tracker access
inline MemoryTracker& getMemoryTracker() {
    static std::unique_ptr<MemoryTracker> tracker = std::make_unique<MemoryTracker>();
    return *tracker;
}

} // namespace nlm
