// nlm - Neural Learning Machine
// Enhanced Logger Implementation
// Phase 2: Robust Runtime Logging Implementation
//
// This file implements the EnhancedLogger class and related logging functionality.

#include "EnhancedLogger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <algorithm>
#include <thread>
#include <chrono>
#include <csignal>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {

class EnhancedLoggerImpl {
public:
    struct Config {
        std::string logFile = "nlm.log";
        size_t maxFileSize = 10 * 1024 * 1024;  // 10 MB
        size_t maxLogHistory = 10000;
        bool enableColors = true;
        bool enableFileOutput = true;
        bool enablePerformanceMetrics = true;
        bool enableMemoryTracking = true;
        LogLevel minLevel = LogLevel::Debug;
        std::string format = "enhanced";  // "simple", "enhanced", "json"
    };
    
    EnhancedLoggerImpl(const Config& config) : config_(config) {
        if (config_.enableFileOutput) {
            setupLogFile();
        }
    }
    
    ~EnhancedLoggerImpl() {
        flush();
        if (logFile_.is_open()) {
            logFile_.close();
        }
    }
    
    // Core logging methods
    void log(const EnhancedLogEntry& entry) {
        // Check log level
        if (entry.level < config_.minLevel) {
            return;
        }
        
        // Format entry based on config
        std::string formattedMessage = formatEntry(entry);
        
        // Output to console
        if (config_.enableColors) {
            std::cout << formatWithColors(entry, formattedMessage) << std::endl;
        } else {
            std::cout << formattedMessage << std::endl;
        }
        
        // Output to file
        if (config_.enableFileOutput && logFile_.is_open()) {
            logFile_ << formattedMessage << std::endl;
            logFile_.flush();n
            // Check file size and rotate if needed
            if (logFile_.tellp() > static_cast<std::streamoff>(config_.maxFileSize)) {
                rotateLogFile();
            }
        }
        
        // Update metrics
        metrics_.record(entry);
        
        // Track performance
        if (config_.enablePerformanceMetrics) {
            auto& perfMonitor = LoggingPerformanceMonitor::getInstance();
            // TODO: Track log processing time
        }
        
        // Check memory health
        if (config_.enableMemoryTracking) {
            auto& memoryTracker = MemoryTracker::getInstance();
            memoryTracker.checkMemoryHealth();
        }
        
        // Add to history
        addToHistory(entry);
    }
    
    // Formatted logging methods
    void trace(const std::string& message,
              const std::string& source = "",
              const std::string& function = "",
              const std::string& file = "",
              int line = 0,
              const std::map<std::string, std::string>& context = {}) {
        EnhancedLogEntry entry(LogLevel::Trace, message, source, function, file, line, context);
        log(entry);
    }
    
    void debug(const std::string& message,
              const std::string& source = "",
              const std::string& function = "",
              const std::string& file = "",
              int line = 0,
              const std::map<std::string, std::string>& context = {}) {
        EnhancedLogEntry entry(LogLevel::Debug, message, source, function, file, line, context);
        log(entry);
    }
    
    void info(const std::string& message,
             const std::string& source = "",
             const std::string& function = "",
             const std::string& file = "",
             int line = 0,
             const std::map<std::string, std::string>& context = {}) {
        EnhancedLogEntry entry(LogLevel::Info, message, source, function, file, line, context);
        log(entry);
    }
    
    void notice(const std::string& message,
               const std::string& source = "",
               const std::string& function = "",
               const std::string& file = "",
               int line = 0,
               const std::map<std::string, std::string>& context = {}) {
        EnhancedLogEntry entry(LogLevel::Notice, message, source, function, file, line, context);
        log(entry);
    }
    
    void warning(const std::string& message,
                const std::string& source = "",
                const std::string& function = "",
                const std::string& file = "",
                int line = 0,
                const std::map<std::string, std::string>& context = {}) {
        EnhancedLogEntry entry(LogLevel::Warning, message, source, function, file, line, context);
        log(entry);
    }
    
    void error(const std::string& message,
              const std::string& source = "",
              const std::string& function = "",
              const std::string& file = "",
              int line = 0,
              const std::string& details = "",
              const std::map<std::string, std::string>& context = {}) {
        EnhancedLogEntry entry(LogLevel::Error, message, source, function, file, line, context);
        if (!details.empty()) {
            entry.context["details"] = details;
        }
        log(entry);
    }
    
    void critical(const std::string& message,
                 const std::string& source = "",
                 const std::string& function = "",
                 const std::string& file = "",
                 int line = 0,
                 const std::string& details = "",
                 const std::map<std::string, std::string>& context = {}) {
        EnhancedLogEntry entry(LogLevel::Critical, message, source, function, file, line, context);
        if (!details.empty()) {
            entry.context["details"] = details;
        }
        log(entry);
    }
    
    void alert(const std::string& message,
              const std::string& source = "",
              const std::string& function = "",
              const std::string& file = "",
              int line = 0,
              const std::string& details = "",
              const std::map<std::string, std::string>& context = {}) {
        EnhancedLogEntry entry(LogLevel::Alert, message, source, function, file, line, context);
        if (!details.empty()) {
            entry.context["details"] = details;
        }
        log(entry);
    }
    
    void emergency(const std::string& message,
                  const std::string& source = "",
                  const std::string& function = "",
                  const std::string& file = "",
                  int line = 0,
                  const std::string& details = "",
                  const std::map<std::string, std::string>& context = {}) {
        EnhancedLogEntry entry(LogLevel::Emergency, message, source, function, file, line, context);
        if (!details.empty()) {
            entry.context["details"] = details;
        }
        log(entry);
    }
    
    // Log with memory tracking
    void logWithMemory(const std::string& message,
                      const std::string& source = "",
                      const std::string& function = "",
                      const std::string& file = "",
                      int line = 0,
                      LogLevel level = LogLevel::Info) {
        auto& memoryTracker = MemoryTracker::getInstance();
        auto memInfo = memoryTracker.getInfo();
        
        EnhancedLogEntry entry(level, message, source, function, file, line);
        entry.memoryUsage = memInfo.allocatedBytes;
        
        log(entry);
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
        
        log(entry);
    }
    
    // Log error with context
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
        
        log(entry);
    }
    
    // Configuration
    void configure(const Config& config) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Close old log file
        if (logFile_.is_open()) {
            logFile_.close();
        }
        
        // Update config
        config_ = config;
        
        // Setup log file if needed
        if (config_.enableFileOutput) {
            setupLogFile();
        }
    }
    
    // Get statistics
    struct LoggerStats {
        size_t totalLogs = 0;
        std::map<LogLevel, size_t> logsByLevel;
        size_t totalMemoryTracked = 0;
        size_t totalPerformanceMetrics = 0;
        std::chrono::system_clock::time_point firstLog;
        std::chrono::system_clock::time_point lastLog;
    } getStatistics() const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        LoggerStats stats;
        stats.totalLogs = metrics_.totalLogs;
        stats.logsByLevel = metrics_.logsByLevel;
        stats.firstLog = metrics_.firstLog;
        stats.lastLog = metrics_.lastLog;
        
        // Get memory info
        auto& memoryTracker = MemoryTracker::getInstance();
        auto memInfo = memoryTracker.getInfo();
        stats.totalMemoryTracked = memInfo.allocatedBytes;
        
        // Get performance metrics
        auto& perfMetrics = PerformanceMetrics::getInstance();
        auto recentMetrics = perfMetrics.getRecentMetrics();
        stats.totalPerformanceMetrics = recentMetrics.size();
        
        return stats;
    }
    
    // Flush logs
    void flush() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (logFile_.is_open()) {
            logFile_.flush();
        }
    }
    
    // Close logger
    void close() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        flush();
        if (logFile_.is_open()) {
            logFile_.close();
        }
    }
    
    // Add to history
    void addToHistory(const EnhancedLogEntry& entry) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        history_.push_back(entry);
        
        // Limit history size
        if (history_.size() > config_.maxLogHistory) {
            history_.erase(history_.begin());
        }
    }
    
    // Get history
    std::vector<EnhancedLogEntry> getHistory(size_t limit = 100) const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (limit > history_.size()) {
            limit = history_.size();
        }
        
        std::vector<EnhancedLogEntry> recent;
        recent.reserve(limit);
        
        auto start = history_.end();
        std::advance(start, -static_cast<std::ptrdiff_t>(limit));
        
        for (auto it = start; it != history_.end(); ++it) {
            recent.push_back(*it);
        }
        
        return recent;
    }
    
private:
    // Format entry
    std::string formatEntry(const EnhancedLogEntry& entry) const {
        switch (config_.format) {
            case LogFormat::Simple:
                return formatSimple(entry);
            case LogFormat::Enhanced:
                return entry.format();
            case LogFormat::JSON:
                return formatJSON(entry);
            default:
                return entry.format();
        }
    }
    
    std::string formatSimple(const EnhancedLogEntry& entry) const {
        std::ostringstream oss;
        oss << "[" << entry.levelToString(entry.level) << "] ";
        oss << entry.message;
        return oss.str();
    }
    
    std::string formatJSON(const EnhancedLogEntry& entry) const {
        json j;
        j["timestamp"] = std::chrono::system_clock::to_time_t(entry.timestamp);
        j["level"] = entry.levelToString(entry.level);
        j["message"] = entry.message;
        j["thread"] = entry.threadId;
        j["source"] = entry.source;
        j["function"] = entry.function;
        j["file"] = entry.file;
        j["line"] = entry.line;
        j["memory_usage"] = entry.memoryUsage;
        
        if (!entry.context.empty()) {
            j["context"] = entry.context;
        }
        
        if (!entry.tags.empty()) {
            j["tags"] = entry.tags;
        }
        
        j["correlation_id"] = entry.correlationId;
        
        return j.dump();
    }
    
    // Format with colors
    std::string formatWithColors(const EnhancedLogEntry& entry, const std::string& message) const {
        std::string colorCode = getLevelColorCode(entry.level);
        std::string resetCode = "\033[0m";
        
        return colorCode + message + resetCode;
    }
    
    std::string getLevelColorCode(LogLevel level) const {
        switch (level) {
            case LogLevel::Trace: return "\033[36m";  // Cyan
            case LogLevel::Debug: return "\033[90m";  // Bright Black
            case LogLevel::Info: return "\033[32m";   // Green
            case LogLevel::Notice: return "\033[34m"; // Blue
            case LogLevel::Warning: return "\033[33m"; // Yellow
            case LogLevel::Error: return "\033[31m";   // Red
            case LogLevel::Critical: return "\033[35m"; // Magenta
            case LogLevel::Alert: return "\033[91m";  // Bright Red
            case LogLevel::Emergency: return "\033[95m"; // Bright Magenta
            default: return "\033[0m";
        }
    }
    
    // Setup log file
    void setupLogFile() {
        std::string timestamp = getTimestampString();
        std::string filename = "nlm_" + timestamp + ".log";
        
        if (logFile_.is_open()) {
            logFile_.close();
        }
        
        logFile_.open(filename, std::ios::app);
        if (logFile_.is_open()) {
            logFile_ << "=== NLM Log File ===" << std::endl;
            logFile_ << "Started at: " << timestamp << std::endl;
            logFile_ << "Format: " << config_.format << std::endl;
            logFile_ << std::endl;
        }
    }
    
    // Rotate log file
    void rotateLogFile() {
        if (!logFile_.is_open()) return;
        
        logFile_.close();
        
        std::string timestamp = getTimestampString();
        std::string oldFilename = "nlm.log";
        std::string newFilename = "nlm_" + timestamp + ".log";
        
        std::rename(oldFilename.c_str(), newFilename.c_str());
        
        logFile_.open(newFilename, std::ios::app);
        if (logFile_.is_open()) {
            logFile_ << "=== NLM Log File (Rotated) ===" << std::endl;
            logFile_ << "Rotated at: " << timestamp << std::endl;
            logFile_ << "Format: " << config_.format << std::endl;
            logFile_ << std::endl;
        }
    }
    
    std::string getTimestampString() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S");
        return ss.str();
    }
    
    Config config_;
    std::ofstream logFile_;
    std::vector<EnhancedLogEntry> history_;
    LoggingMetrics metrics_;
    mutable std::mutex mutex_;
};

// EnhancedLogger implementation
class EnhancedLogger::Impl {
public:
    Impl(const Config& config) : loggerImpl_(std::make_unique<EnhancedLoggerImpl>(config)) {}
    
    ~Impl() {
        loggerImpl_->close();
    }
    
    void trace(const std::string& message,
              const std::string& source = "",
              const std::string& function = "",
              const std::string& file = "",
              int line = 0,
              const std::map<std::string, std::string>& context = {}) {
        loggerImpl_->trace(message, source, function, file, line, context);
    }
    
    void debug(const std::string& message,
              const std::string& source = "",
              const std::string& function = "",
              const std::string& file = "",
              int line = 0,
              const std::map<std::string, std::string>& context = {}) {
        loggerImpl_->debug(message, source, function, file, line, context);
    }
    
    void info(const std::string& message,
             const std::string& source = "",
             const std::string& function = "",
             const std::string& file = "",
             int line = 0,
             const std::map<std::string, std::string>& context = {}) {
        loggerImpl_->info(message, source, function, file, line, context);
    }
    
    void notice(const std::string& message,
               const std::string& source = "",
               const std::string& function = "",
               const std::string& file = "",
               int line = 0,
               const std::map<std::string, std::string>& context = {}) {
        loggerImpl_->notice(message, source, function, file, line, context);
    }
    
    void warning(const std::string& message,
                const std::string& source = "",
                const std::string& function = "",
                const std::string& file = "",
                int line = 0,
                const std::map<std::string, std::string>& context = {}) {
        loggerImpl_->warning(message, source, function, file, line, context);
    }
    
    void error(const std::string& message,
              const std::string& source = "",
              const std::string& function = "",
              const std::string& file = "",
              int line = 0,
              const std::string& details = "",
              const std::map<std::string, std::string>& context = {}) {
        loggerImpl_->error(message, source, function, file, line, details, context);
    }
    
    void critical(const std::string& message,
                 const std::string& source = "",
                 const std::string& function = "",
                 const std::string& file = "",
                 int line = 0,
                 const std::string& details = "",
                 const std::map<std::string, std::string>& context = {}) {
        loggerImpl_->critical(message, source, function, file, line, details, context);
    }
    
    void alert(const std::string& message,
              const std::string& source = "",
              const std::string& function = "",
              const std::string& file = "",
              int line = 0,
              const std::string& details = "",
              const std::map<std::string, std::string>& context = {}) {
        loggerImpl_->alert(message, source, function, file, line, details, context);
    }
    
    void emergency(const std::string& message,
                  const std::string& source = "",
                  const std::string& function = "",
                  const std::string& file = "",
                  int line = 0,
                  const std::string& details = "",
                  const std::map<std::string, std::string>& context = {}) {
        loggerImpl_->emergency(message, source, function, file, line, details, context);
    }
    
    void logWithMemory(const std::string& message,
                      const std::string& source = "",
                      const std::string& function = "",
                      const std::string& file = "",
                      int line = 0,
                      LogLevel level = LogLevel::Info) {
        loggerImpl_->logWithMemory(message, source, function, file, line, level);
    }
    
    void logWithPerformance(const std::string& message,
                           const std::string& source = "",
                           const std::string& function = "",
                           const std::string& file = "",
                           int line = 0,
                           LogLevel level = LogLevel::Info,
                           const std::vector<std::string>& tags = {}) {
        loggerImpl_->logWithPerformance(message, source, function, file, line, level, tags);
    }
    
    void logErrorWithContext(const Error& error,
                           const std::map<std::string, std::string>& additionalContext = {}) {
        loggerImpl_->logErrorWithContext(error, additionalContext);
    }
    
    void logEnhanced(const EnhancedLogEntry& entry) {
        loggerImpl_->log(entry);
    }
    
    struct LoggerStats {
        size_t totalLogs = 0;
        std::map<LogLevel, size_t> logsByLevel;
        size_t totalMemoryTracked = 0;
        size_t totalPerformanceMetrics = 0;
        std::chrono::system_clock::time_point firstLog;
        std::chrono::system_clock::time_point lastLog;
    } getStatistics() const {
        return loggerImpl_->getStatistics();
    }
    
    void flush() {
        loggerImpl_->flush();
    }
    
    void close() {
        loggerImpl_->close();
    }
    
    // Configure logger
    void configure(const Config& config) {
        loggerImpl_->configure(config);
    }
    
    // Get history
    std::vector<EnhancedLogEntry> getHistory(size_t limit = 100) const {
        return loggerImpl_->getHistory(limit);
    }
    
private:
    std::unique_ptr<EnhancedLoggerImpl> loggerImpl_;
};

// EnhancedLogger implementation
EnhancedLogger::EnhancedLogger() : pImpl(std::make_unique<Impl>(Config())) {}

EnhancedLogger::EnhancedLogger(const Config& config) : pImpl(std::make_unique<Impl>(config)) {}

EnhancedLogger::~EnhancedLogger() = default;

void EnhancedLogger::trace(const std::string& message,
                         const std::string& source,
                         const std::string& function,
                         const std::string& file,
                         int line,
                         const std::map<std::string, std::string>& context) {
    pImpl->trace(message, source, function, file, line, context);
}

void EnhancedLogger::debug(const std::string& message,
                         const std::string& source,
                         const std::string& function,
                         const std::string& file,
                         int line,
                         const std::map<std::string, std::string>& context) {
    pImpl->debug(message, source, function, file, line, context);
}

void EnhancedLogger::info(const std::string& message,
                         const std::string& source,
                         const std::string& function,
                         const std::string& file,
                         int line,
                         const std::map<std::string, std::string>& context) {
    pImpl->info(message, source, function, file, line, context);
}

void EnhancedLogger::notice(const std::string& message,
                           const std::string& source,
                           const std::string& function,
                           const std::string& file,
                           int line,
                           const std::map<std::string, std::string>& context) {
    pImpl->notice(message, source, function, file, line, context);
}

void EnhancedLogger::warning(const std::string& message,
                            const std::string& source,
                            const std::string& function,
                            const std::string& file,
                            int line,
                            const std::map<std::string, std::string>& context) {
    pImpl->warning(message, source, function, file, line, context);
}

void EnhancedLogger::error(const std::string& message,
                          const std::string& source,
                          const std::string& function,
                          const std::string& file,
                          int line,
                          const std::string& details,
                          const std::map<std::string, std::string>& context) {
    pImpl->error(message, source, function, file, line, details, context);
}

void EnhancedLogger::critical(const std::string& message,
                             const std::string& source,
                             const std::string& function,
                             const std::string& file,
                             int line,
                             const std::string& details,
                             const std::map<std::string, std::string>& context) {
    pImpl->critical(message, source, function, file, line, details, context);
}

void EnhancedLogger::alert(const std::string& message,
                          const std::string& source,
                          const std::string& function,
                          const std::string& file,
                          int line,
                          const std::string& details,
                          const std::map<std::string, std::string>& context) {
    pImpl->alert(message, source, function, file, line, details, context);
}

void EnhancedLogger::emergency(const std::string& message,
                              const std::string& source,
                              const std::string& function,
                              const std::string& file,
                              int line,
                              const std::string& details,
                              const std::map<std::string, std::string>& context) {
    pImpl->emergency(message, source, function, file, line, details, context);
}

void EnhancedLogger::logWithMemory(const std::string& message,
                                 const std::string& source,
                                 const std::string& function,
                                 const std::string& file,
                                 int line,
                                 LogLevel level) {
    pImpl->logWithMemory(message, source, function, file, line, level);
}

void EnhancedLogger::logWithPerformance(const std::string& message,
                                       const std::string& source,
                                       const std::string& function,
                                       const std::string& file,
                                       int line,
                                       LogLevel level,
                                       const std::vector<std::string>& tags) {
    pImpl->logWithPerformance(message, source, function, file, line, level, tags);
}

void EnhancedLogger::logErrorWithContext(const Error& error,
                                       const std::map<std::string, std::string>& additionalContext) {
    pImpl->logErrorWithContext(error, additionalContext);
}

void EnhancedLogger::logEnhanced(const EnhancedLogEntry& entry) {
    pImpl->logEnhanced(entry);
}

EnhancedLogger::LoggerStats EnhancedLogger::getStatistics() const {
    return pImpl->getStatistics();
}

void EnhancedLogger::flush() {
    pImpl->flush();
}

void EnhancedLogger::close() {
    pImpl->close();
}

// Global logging setup
namespace Logging {
    // Initialize logging system
    void initialize() {
        ErrorHandling::initialize();
        
        EnhancedLogger::Config config;
        globalLogger_ = std::make_shared<EnhancedLogger>(config);
    }
    
    // Get global logger
    std::shared_ptr<EnhancedLogger> getLogger() {
        if (!globalLogger_) {
            initialize();
        }
        return globalLogger_;
    }
    
    // Configure global logger
    void configure(const EnhancedLogger::Config& config) {
        if (globalLogger_) {
            globalLogger_->configure(config);
        }
    }
    
    // Log functions
    void trace(const std::string& message,
              const std::string& source = "",
              const std::string& function = "",
              const std::string& file = "",
              int line = 0,
              const std::map<std::string, std::string>& context = {}) {
        if (globalLogger_) {
            globalLogger_->trace(message, source, function, file, line, context);
        }
    }
    
    void debug(const std::string& message,
              const std::string& source = "",
              const std::string& function = "",
              const std::string& file = "",
              int line = 0,
              const std::map<std::string, std::string>& context = {}) {
        if (globalLogger_) {
            globalLogger_->debug(message, source, function, file, line, context);
        }
    }
    
    void info(const std::string& message,
             const std::string& source = "",
             const std::string& function = "",
             const std::string& file = "",
             int line = 0,
             const std::map<std::string, std::string>& context = {}) {
        if (globalLogger_) {
            globalLogger_->info(message, source, function, file, line, context);
        }
    }
    
    void notice(const std::string& message,
               const std::string& source = "",
               const std::string& function = "",
               const std::string& file = "",
               int line = 0,
               const std::map<std::string, std::string>& context = {}) {
        if (globalLogger_) {
            globalLogger_->notice(message, source, function, file, line, context);
        }
    }
    
    void warning(const std::string& message,
                const std::string& source = "",
                const std::string& function = "",
                const std::string& file = "",
                int line = 0,
                const std::map<std::string, std::string>& context = {}) {
        if (globalLogger_) {
            globalLogger_->warning(message, source, function, file, line, context);
        }
    }
    
    void error(const std::string& message,
              const std::string& source = "",
              const std::string& function = "",
              const std::string& file = "",
              int line = 0,
              const std::string& details = "",
              const std::map<std::string, std::string>& context = {}) {
        if (globalLogger_) {
            globalLogger_->error(message, source, function, file, line, details, context);
        }
    }
    
    void critical(const std::string& message,
                 const std::string& source = "",
                 const std::string& function = "",
                 const std::string& file = "",
                 int line = 0,
                 const std::string& details = "",
                 const std::map<std::string, std::string>& context = {}) {
        if (globalLogger_) {
            globalLogger_->critical(message, source, function, file, line, details, context);
        }
    }
    
    void alert(const std::string& message,
              const std::string& source = "",
              const std::string& function = "",
              const std::string& file = "",
              int line = 0,
              const std::string& details = "",
              const std::map<std::string, std::string>& context = {}) {
        if (globalLogger_) {
            globalLogger_->alert(message, source, function, file, line, details, context);
        }
    }
    
    void emergency(const std::string& message,
                  const std::string& source = "",
                  const std::string& function = "",
                  const std::string& file = "",
                  int line = 0,
                  const std::string& details = "",
                  const std::map<std::string, std::string>& context = {}) {
        if (globalLogger_) {
            globalLogger_->emergency(message, source, function, file, line, details, context);
        }
    }
    
    // Log with memory tracking
    void logWithMemory(const std::string& message,
                      const std::string& source = "",
                      const std::string& function = "",
                      const std::string& file = "",
                      int line = 0,
                      LogLevel level = LogLevel::Info) {
        if (globalLogger_) {
            globalLogger_->logWithMemory(message, source, function, file, line, level);
        }
    }
    
    // Log with performance
    void logWithPerformance(const std::string& message,
                           const std::string& source = "",
                           const std::string& function = "",
                           const std::string& file = "",
                           int line = 0,
                           LogLevel level = LogLevel::Info,
                           const std::vector<std::string>& tags = {}) {
        if (globalLogger_) {
            globalLogger_->logWithPerformance(message, source, function, file, line, level, tags);
        }
    }
    
    // Log error with context
    void logErrorWithContext(const Error& error,
                           const std::map<std::string, std::string>& additionalContext = {}) {
        if (globalLogger_) {
            globalLogger_->logErrorWithContext(error, additionalContext);
        }
    }
    
    // Get statistics
    EnhancedLogger::LoggerStats getStatistics() {
        if (globalLogger_) {
            return globalLogger_->getStatistics();
        }
        return EnhancedLogger::LoggerStats();
    }
    
    // Flush
    void flush() {
        if (globalLogger_) {
            globalLogger_->flush();
        }
    }
    
    // Close
    void close() {
        if (globalLogger_) {
            globalLogger_->close();
        }
        globalLogger_.reset();
    }
    
    // Graceful shutdown
    void shutdown() {
        flush();
        close();
        ErrorHandling::shutdown();
    }
    
private:
    static std::shared_ptr<EnhancedLogger> globalLogger_;
}

std::shared_ptr<EnhancedLogger> Logging::globalLogger_;

} // namespace nlm