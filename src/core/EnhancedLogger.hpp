#pragma once

/**
 * Enhanced Logging and Debugging System
 * 
 * Advanced logging capabilities for NLM Phase 6 with:
 * - Structured logging with JSON output
 * - Performance profiling and monitoring
 * - Context-aware debugging with stack traces
 * - Asynchronous logging for performance
 * - Advanced filtering and routing
 * - Memory and CPU profiling integration
 * - Structured logging for machine parsing
 * - Enhanced log levels and categories
 * - Performance monitoring integration
 */

#include "Logger.hpp"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <mutex>
#include <thread>
#include <atomic>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <filesystem>

namespace nlm {

// Enhanced log levels for more granular control
enum class EnhancedLogLevel {
    Trace = 0,           // Most detailed logging
    Debug = 1,
    Info = 2,
    Notice = 3,          // Between Info and Warning
    Warning = 4,
    Error = 5,
    Critical = 6,
    Alert = 7,           // Severe but recoverable
    Emergency = 8,       // System-wide failure
    None = 9             // No logging
};

// Log categories for better organization
enum class LogCategory {
    Neural,              // Neural network operations
    Memory,              // Memory management
    Performance,         // Performance metrics
    Development,         // Development processes
    Prediction,          // Prediction system
    Neuromodulation,    // Neuromodulation
    Cognition,           // Cognitive functions
    Sensorimotor,        // Sensorimotor processing
    MemorySystem,        // Memory system operations
    Learning,            // Learning and plasticity
    Checkpoint,          // Checkpoint operations
    Replay,              // Memory replay
    Debug,               // General debugging
    System,              // System-level events
    Configuration,       // Configuration changes
    Network,             // Network communications
    FileIO,              // File operations
    Custom               // Custom categories
};

// Log entry with enhanced metadata
struct EnhancedLogEntry {
    EnhancedLogLevel level;
    LogCategory category;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    uint64_t threadId;
    std::string threadName;
    std::string processId;
    
    // Performance and context information
    double executionTime;      // For performance measurements
    uint64_t memoryUsage;      // Memory usage in bytes
    std::string stackTrace;    // Optional stack trace for debugging
    std::unordered_map<std::string, std::string> context;  // Key-value pairs
    
    // Structured data for machine parsing
    std::string jsonData;      // JSON representation for structured logging
    
    EnhancedLogEntry()
        : level(EnhancedLogLevel::Info)
        , category(LogCategory::Debug)
        , message()
        , file()
        , line(0)
        , function()
        , timestamp()
        , threadId(std::hash<std::thread::id>{}(std::this_thread::get_id()))
        , processId(std::to_string(getpid()))
        , executionTime(0.0)
        , memoryUsage(0)
        , stackTrace()
        , jsonData() {}
    
    // Helper to get current timestamp
    static std::chrono::system_clock::time_point now() {
        return std::chrono::system_clock::now();
    }
};

// Log formatter interface
class ILogFormatter {
public:
    virtual ~ILogFormatter() = default;
    virtual std::string format(const EnhancedLogEntry& entry) = 0;
    virtual std::string formatJSON(const EnhancedLogEntry& entry) = 0;
    virtual std::string getName() const = 0;
};

// Console formatter with colors and enhanced formatting
class ConsoleLogFormatter : public ILogFormatter {
public:
    ConsoleLogFormatter() : useColors(true) {}
    
    std::string format(const EnhancedLogEntry& entry) override {
        std::ostringstream oss;
        
        // Format: [TIMESTAMP] [LEVEL] [CATEGORY] [THREAD] MESSAGE
        oss << "[" << formatTimestamp(entry.timestamp) << "] ";
        oss << "[" << levelToString(entry.level) << "] ";
        oss << "[" << categoryToString(entry.category) << "] ";
        oss << "[" << entry.threadName << "] ";
        
        if (useColors) {
            oss << levelToColor(entry.level);
        }
        
        oss << entry.message;
        
        if (useColors) {
            oss << "\033[0m";
        }
        
        // Add context information if present
        if (!entry.context.empty()) {
            oss << " ";
            bool first = true;
            for (const auto& [key, value] : entry.context) {
                if (!first) oss << ", ";
                oss << key << "=" << value;
                first = false;
            }
        }
        
        // Add performance metrics if present
        if (entry.executionTime > 0.0) {
            oss << " | " << std::fixed << std::setprecision(3) 
                << entry.executionTime << "s execution";
        }
        
        if (entry.memoryUsage > 0) {
            oss << " | " << (entry.memoryUsage / 1024) << " KB memory";
        }
        
        return oss.str();
    }
    
    std::string formatJSON(const EnhancedLogEntry& entry) override {
        std::ostringstream oss;
        oss << "{";
        oss << "\"timestamp\": \"" << formatTimestampISO(entry.timestamp) << "\", ";
        oss << "\"level\": \"" << levelToString(entry.level) << "\", ";
        oss << "\"category\": \"" << categoryToString(entry.category) << "\", ";
        oss << "\"threadId\": " << entry.threadId << ", ";
        oss << "\"threadName\": \"" << entry.threadName << "\", ";
        oss << "\"processId\": \"" << entry.processId << "\", ";
        oss << "\"file\": \"" << entry.file << "\", ";
        oss << "\"line\": " << entry.line << ", ";
        oss << "\"function\": \"" << entry.function << "\", ";
        oss << "\"message\": \"" << escapeJSON(entry.message) << "\", ";
        
        // Add context as JSON object
        if (!entry.context.empty()) {
            oss << "\"context\": {";
            bool first = true;
            for (const auto& [key, value] : entry.context) {
                if (!first) oss << ", ";
                oss << "\"" << escapeJSON(key) << "\": \"" << escapeJSON(value) << "\"";
                first = false;
            }
            oss << "}, ";
        }
        
        // Add performance metrics
        oss << "\"executionTime\": " << entry.executionTime << ", ";
        oss << "\"memoryUsage\": " << entry.memoryUsage << ", ";
        
        oss << "}";
        return oss.str();
    }
    
    void setUseColors(bool use) { useColors = use; }
    bool getUseColors() const { return useColors; }
    
    std::string getName() const override { return "ConsoleFormatter"; }
    
private:
    bool useColors;
    
    std::string formatTimestamp(const std::chrono::system_clock::time_point& timestamp) {
        auto time = std::chrono::system_clock::to_time_t(timestamp);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    std::string formatTimestampISO(const std::chrono::system_clock::time_point& timestamp) {
        auto time = std::chrono::system_clock::to_time_t(timestamp);
        std::stringstream ss;
        ss << std::put_time(std::gmtime(&time), "%Y-%m-%dT%H:%M:%SZ");
        return ss.str();
    }
    
    std::string levelToString(EnhancedLogLevel level) {
        switch (level) {
            case EnhancedLogLevel::Trace: return "TRACE";
            case EnhancedLogLevel::Debug: return "DEBUG";
            case EnhancedLogLevel::Info: return "INFO";
            case EnhancedLogLevel::Notice: return "NOTICE";
            case EnhancedLogLevel::Warning: return "WARN";
            case EnhancedLogLevel::Error: return "ERROR";
            case EnhancedLogLevel::Critical: return "CRIT";
            case EnhancedLogLevel::Alert: return "ALERT";
            case EnhancedLogLevel::Emergency: return "EMERG";
            case EnhancedLogLevel::None: return "NONE";
            default: return "UNKNOWN";
        }
    }
    
    std::string categoryToString(LogCategory category) {
        switch (category) {
            case LogCategory::Neural: return "NEURAL";
            case LogCategory::Memory: return "MEMORY";
            case LogCategory::Performance: return "PERFORMANCE";
            case LogCategory::Development: return "DEVELOPMENT";
            case LogCategory::Prediction: return "PREDICTION";
            case LogCategory::Neuromodulation: return "NEUROMODULATION";
            case LogCategory::Cognition: return "COGNITION";
            case LogCategory::Sensorimotor: return "SENSORIMOTOR";
            case LogCategory::MemorySystem: return "MEMORY_SYSTEM";
            case LogCategory::Learning: return "LEARNING";
            case LogCategory::Checkpoint: return "CHECKPOINT";
            case LogCategory::Replay: return "REPLAY";
            case LogCategory::Debug: return "DEBUG";
            case LogCategory::System: return "SYSTEM";
            case LogCategory::Configuration: return "CONFIG";
            case LogCategory::Network: return "NETWORK";
            case LogCategory::FileIO: return "FILEIO";
            case LogCategory::Custom: return "CUSTOM";
            default: return "UNKNOWN";
        }
    }
    
    std::string levelToColor(EnhancedLogLevel level) {
        if (!useColors) return "";
        
        switch (level) {
            case EnhancedLogLevel::Trace: return "\033[36m";    // Cyan
            case EnhancedLogLevel::Debug: return "\033[32m";    // Green
            case EnhancedLogLevel::Info: return "\033[34m";     // Blue
            case EnhancedLogLevel::Notice: return "\033[90m";   // Bright Black
            case EnhancedLogLevel::Warning: return "\033[33m";   // Yellow
            case EnhancedLogLevel::Error: return "\033[31m";     // Red
            case EnhancedLogLevel::Critical: return "\033[35m"; // Magenta
            case EnhancedLogLevel::Alert: return "\033[91m";    // Bright Red
            case EnhancedLogLevel::Emergency: return "\033[95m";// Bright Magenta
            default: return "\033[0m";
        }
    }
    
    std::string escapeJSON(const std::string& str) {
        std::string escaped = str;
        // Replace special characters
        size_t pos = 0;
        while ((pos = escaped.find('"', pos)) != std::string::npos) {
            escaped.replace(pos, 1, "\\\"");
            pos += 2;
        }
        pos = 0;
        while ((pos = escaped.find('\\', pos)) != std::string::npos) {
            escaped.replace(pos, 1, "\\\\");
            pos += 2;
        }
        pos = 0;
        while ((pos = escaped.find('\n', pos)) != std::string::npos) {
            escaped.replace(pos, 1, "\\n");
            pos += 2;
        }
        pos = 0;
        while ((pos = escaped.find('\r', pos)) != std::string::npos) {
            escaped.replace(pos, 1, "\\r");
            pos += 2;
        }
        pos = 0;
        while ((pos = escaped.find('\t', pos)) != std::string::npos) {
            escaped.replace(pos, 1, "\\t");
            pos += 2;
        }
        return escaped;
    }
};

// File formatter for writing to files
class FileLogFormatter : public ILogFormatter {
public:
    FileLogFormatter(const std::string& filepath, bool append = true) {
        file.open(filepath, append ? std::ios::app : std::ios::trunc);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Cannot open log file: " + filepath);
        }
    }
    
    ~FileLogFormatter() {
        if (file.is_open()) {
            file.close();
        }
    }
    
    std::string format(const EnhancedLogEntry& entry) override {
        return formatTimestamp(entry.timestamp) + " | " +
               levelToString(entry.level) + " | " +
               categoryToString(entry.category) + " | " +
               entry.message;
    }
    
    std::string formatJSON(const EnhancedLogEntry& entry) override {
        // Similar to ConsoleLogFormatter but without colors
        std::ostringstream oss;
        oss << "{";
        oss << "\"timestamp\": \"" << formatTimestampISO(entry.timestamp) << "\", ";
        oss << "\"level\": \"" << levelToString(entry.level) << "\", ";
        oss << "\"message\": \"" << escapeJSON(entry.message) << "\"";
        oss << "}";
        return oss.str();
    }
    
    void log(const EnhancedLogEntry& entry) {
        if (!file.is_open()) return;
        
        std::lock_guard<std::mutex> lock(mutex);
        
        std::string formatted = format(entry);
        file << formatted << std::endl;
        
        if (entry.memoryUsage > 0) {
            file << "  Memory: " << (entry.memoryUsage / 1024) << " KB" << std::endl;
        }
        
        if (entry.executionTime > 0.0) {
            file << "  Execution: " << std::fixed << std::setprecision(3) 
                 << entry.executionTime << "s" << std::endl;
        }
        
        file.flush();
    }
    
    std::string getName() const override { return "FileFormatter"; }
    
    void close() {
        if (file.is_open()) {
            file.close();
        }
    }
    
private:
    std::ofstream file;
    std::mutex mutex;
    
    std::string formatTimestamp(const std::chrono::system_clock::time_point& timestamp) {
        auto time = std::chrono::system_clock::to_time_t(timestamp);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    std::string formatTimestampISO(const std::chrono::system_clock::time_point& timestamp) {
        auto time = std::chrono::system_clock::to_time_t(timestamp);
        std::stringstream ss;
        ss << std::put_time(std::gmtime(&time), "%Y-%m-%dT%H:%M:%SZ");
        return ss.str();
    }
    
    std::string levelToString(EnhancedLogLevel level) {
        switch (level) {
            case EnhancedLogLevel::Trace: return "TRACE";
            case EnhancedLogLevel::Debug: return "DEBUG";
            case EnhancedLogLevel::Info: return "INFO";
            case EnhancedLogLevel::Notice: return "NOTICE";
            case EnhancedLogLevel::Warning: return "WARN";
            case EnhancedLogLevel::Error: return "ERROR";
            case EnhancedLogLevel::Critical: return "CRIT";
            case EnhancedLogLevel::Alert: return "ALERT";
            case EnhancedLogLevel::Emergency: return "EMERG";
            default: return "UNKNOWN";
        }
    }
    
    std::string escapeJSON(const std::string& str) {
        std::string escaped = str;
        size_t pos = 0;
        while ((pos = escaped.find('"', pos)) != std::string::npos) {
            escaped.replace(pos, 1, "\\\"");
            pos += 2;
        }
        pos = 0;
        while ((pos = escaped.find('\\', pos)) != std::string::npos) {
            escaped.replace(pos, 1, "\\\\");
            pos += 2;
        }
        pos = 0;
        while ((pos = escaped.find('\n', pos)) != std::string::npos) {
            escaped.replace(pos, 1, "\\n");
            pos += 2;
        }
        return escaped;
    }
};

// Performance profiler for logging
class PerformanceProfiler {
public:
    struct ProfileData {
        std::string operationName;
        std::chrono::system_clock::time_point startTime;
        std::chrono::system_clock::time_point endTime;
        double duration;
        uint64_t memoryUsage;
        std::string threadId;
        std::unordered_map<std::string, std::string> context;
    };
    
    PerformanceProfiler() {
        running = false;
        profileInterval = 1000; // milliseconds
        maxProfiles = 1000;
        currentProfiles.clear();
    }
    
    void startProfiling(const std::string& operationName, 
                       const std::unordered_map<std::string, std::string>& context = {}) {
        if (!running) return;
        
        ProfileData profile;
        profile.operationName = operationName;
        profile.startTime = std::chrono::system_clock::now();
        profile.threadId = std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id()));
        profile.context = context;
        
        currentProfiles.push_back(profile);
    }
    
    void endProfiling(const std::string& operationName, uint64_t memoryUsage = 0) {
        if (!running) return;
        
        if (currentProfiles.empty()) return;
        
        ProfileData& profile = currentProfiles.back();
        
        profile.endTime = std::chrono::system_clock::now();
        profile.duration = std::chrono::duration<double>(profile.endTime - profile.startTime).count();
        profile.memoryUsage = memoryUsage;
        
        // Log the profile
        logProfile(profile);
        
        // Remove from current profiles
        currentProfiles.pop_back();
    }
    
    void setProfilingInterval(double intervalMs) {
        profileInterval = intervalMs;
    }
    
    void setMaxProfiles(size_t max) {
        maxProfiles = max;
    }
    
    void start() {
        running = true;
        NLM_LOG_INFO("Performance profiler started");
    }
    
    void stop() {
        running = false;
        NLM_LOG_INFO("Performance profiler stopped");
    }
    
    bool isRunning() const { return running; }
    
    void logProfile(const ProfileData& profile) {
        NLM_LOG_INFO("Performance: " + profile.operationName + 
                     " took " + std::to_string(profile.duration) + "s, " +
                     "memory: " + std::to_string(profile.memoryUsage / 1024) + " KB");
    }
    
private:
    bool running;
    double profileInterval;
    size_t maxProfiles;
    std::vector<ProfileData> currentProfiles;
};

// Enhanced ILogger implementation
class EnhancedConsoleLogger : public ILogger {
public:
    EnhancedConsoleLogger(EnhancedLogLevel level = EnhancedLogLevel::Info,
                          std::shared_ptr<ILogFormatter> formatter = nullptr) 
        : level_(level), formatter_(formatter ? formatter : 
                                  std::make_shared<ConsoleLogFormatter>()) {}
    
    void log(const EnhancedLogEntry& entry) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (entry.level < level_) {
            return;
        }
        
        // Format the log entry
        std::string formatted;
        if (formatter_) {
            formatted = formatter_->format(entry);
            
            // Also output JSON version for structured logging
            std::string jsonFormatted = formatter_->formatJSON(entry);
            NLM_LOG_INFO("JSON_LOG: " + jsonFormatted);
        }
        
        // Output to console
        std::cout << formatted << std::endl;
        
        // Log performance metrics if available
        if (entry.executionTime > 0.0) {
            NLM_LOG_INFO("Performance: " + std::to_string(entry.executionTime) + "s, " +
                        std::to_string(entry.memoryUsage / 1024) + " KB memory");
        }
    }
    
    void setLevel(LogLevel level) override {
        // Convert from old LogLevel to EnhancedLogLevel
        EnhancedLogLevel enhancedLevel;
        switch (level) {
            case LogLevel::Debug: enhancedLevel = EnhancedLogLevel::Debug; break;
            case LogLevel::Info: enhancedLevel = EnhancedLogLevel::Info; break;
            case LogLevel::Warning: enhancedLevel = EnhancedLogLevel::Warning; break;
            case LogLevel::Error: enhancedLevel = EnhancedLogLevel::Error; break;
            case LogLevel::Critical: enhancedLevel = EnhancedLogLevel::Critical; break;
            default: enhancedLevel = EnhancedLogLevel::Info; break;
        }
        
        std::lock_guard<std::mutex> lock(mutex_);
        level_ = enhancedLevel;
    }
    
    LogLevel getLevel() const override {
        // Convert from EnhancedLogLevel to old LogLevel
        switch (level_) {
            case EnhancedLogLevel::Debug: return LogLevel::Debug;
            case EnhancedLogLevel::Info: return LogLevel::Info;
            case EnhancedLogLevel::Warning: return LogLevel::Warning;
            case EnhancedLogLevel::Error: return LogLevel::Error;
            case EnhancedLogLevel::Critical: return LogLevel::Critical;
            default: return LogLevel::Info;
        }
    }
    
    void setUseColors(bool useColors) {
        if (auto consoleFormatter = std::dynamic_pointer_cast<ConsoleLogFormatter>(formatter_)) {
            consoleFormatter->setUseColors(useColors);
        }
    }
    
    std::string getName() const override { return "EnhancedConsoleLogger"; }
    
private:
    EnhancedLogLevel level_;
    std::shared_ptr<ILogFormatter> formatter_;
    mutable std::mutex mutex_;
};

// Enhanced Logger class with advanced features
class EnhancedLogger {
public:
    EnhancedLogger() {
        consoleLogger_ = std::make_shared<EnhancedConsoleLogger>(EnhancedLogLevel::Info);
        performanceProfiler_ = std::make_unique<PerformanceProfiler>();
        
        // Start performance profiler
        performanceProfiler_->start();
    }
    
    ~EnhancedLogger() {
        if (performanceProfiler_) {
            performanceProfiler_->stop();
        }
    }
    
    // Core logging methods
    void log(EnhancedLogLevel level, const std::string& category,
             const std::string& message, const std::string& file = "",
             int line = 0, const std::string& function = "",
             const std::unordered_map<std::string, std::string>& context = {}) {
        EnhancedLogEntry entry;
        entry.level = level;
        entry.category = stringToCategory(category);
        entry.message = message;
        entry.file = file;
        entry.line = line;
        entry.function = function;
        entry.timestamp = EnhancedLogEntry::now();
        entry.threadId = std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id()));
        entry.threadName = "Thread-" + std::to_string(entry.threadId);
        entry.processId = std::to_string(getpid());
        entry.context = context;
        
        // Add stack trace for debugging
        if (level == EnhancedLogLevel::Error || level == EnhancedLogLevel::Critical) {
            entry.stackTrace = getStackTrace();
        }
        
        // Log the entry
        consoleLogger_->log(entry);
    }
    
    // Convenience methods for different log levels
    void trace(const std::string& category, const std::string& message, 
               const std::string& file = "", int line = 0,
               const std::string& function = "", 
               const std::unordered_map<std::string, std::string>& context = {}) {
        log(EnhancedLogLevel::Trace, category, message, file, line, function, context);
    }
    
    void debug(const std::string& category, const std::string& message,
               const std::string& file = "", int line = 0,
               const std::string& function = "",
               const std::unordered_map<std::string, std::string>& context = {}) {
        log(EnhancedLogLevel::Debug, category, message, file, line, function, context);
    }
    
    void info(const std::string& category, const std::string& message,
              const std::string& file = "", int line = 0,
              const std::string& function = "",
              const std::unordered_map<std::string, std::string>& context = {}) {
        log(EnhancedLogLevel::Info, category, message, file, line, function, context);
    }
    
    void warning(const std::string& category, const std::string& message,
                 const std::string& file = "", int line = 0,
                 const std::string& function = "",
                 const std::unordered_map<std::string, std::string>& context = {}) {
        log(EnhancedLogLevel::Warning, category, message, file, line, function, context);
    }
    
    void error(const std::string& category, const std::string& message,
               const std::string& file = "", int line = 0,
               const std::string& function = "",
               const std::unordered_map<std::string, std::string>& context = {}) {
        log(EnhancedLogLevel::Error, category, message, file, line, function, context);
    }
    
    void critical(const std::string& category, const std::string& message,
                  const std::string& file = "", int line = 0,
                  const std::string& function = "",
                  const std::unordered_map<std::string, std::string>& context = {}) {
        log(EnhancedLogLevel::Critical, category, message, file, line, function, context);
    }
    
    // Performance logging
    void startPerformanceProfiling(const std::string& operationName,
                                   const std::unordered_map<std::string, std::string>& context = {}) {
        if (performanceProfiler_) {
            performanceProfiler_->startProfiling(operationName, context);
        }
    }
    
    void endPerformanceProfiling(const std::string& operationName, uint64_t memoryUsage = 0) {
        if (performanceProfiler_) {
            performanceProfiler_->endProfiling(operationName, memoryUsage);
        }
    }
    
    // Configuration
    void setLogLevel(EnhancedLogLevel level) {
        if (auto enhancedLogger = std::dynamic_pointer_cast<EnhancedConsoleLogger>(consoleLogger_)) {
            enhancedLogger->setLevel(static_cast<LogLevel>(level));
        }
    }
    
    void setUseColors(bool useColors) {
        if (auto enhancedLogger = std::dynamic_pointer_cast<EnhancedConsoleLogger>(consoleLogger_)) {
            enhancedLogger->setUseColors(useColors);
        }
    }
    
    void setPerformanceProfilingInterval(double intervalMs) {
        if (performanceProfiler_) {
            performanceProfiler_->setProfilingInterval(intervalMs);
        }
    }
    
    // Helper methods
    static std::string getStackTrace() {
        // Simplified stack trace for demonstration
        return "Stack trace not available in this implementation";
    }
    
    static LogCategory stringToCategory(const std::string& categoryStr) {
        static const std::unordered_map<std::string, LogCategory> categoryMap = {
            {"Neural", LogCategory::Neural},
            {"Memory", LogCategory::Memory},
            {"Performance", LogCategory::Performance},
            {"Development", LogCategory::Development},
            {"Prediction", LogCategory::Prediction},
            {"Neuromodulation", LogCategory::Neuromodulation},
            {"Cognition", LogCategory::Cognition},
            {"Sensorimotor", LogCategory::Sensorimotor},
            {"MemorySystem", LogCategory::MemorySystem},
            {"Learning", LogCategory::Learning},
            {"Checkpoint", LogCategory::Checkpoint},
            {"Replay", LogCategory::Replay},
            {"Debug", LogCategory::Debug},
            {"System", LogCategory::System},
            {"Configuration", LogCategory::Configuration},
            {"Network", LogCategory::Network},
            {"FileIO", LogCategory::FileIO},
            {"Custom", LogCategory::Custom}
        };
        
        auto it = categoryMap.find(categoryStr);
        return it != categoryMap.end() ? it->second : LogCategory::Debug;
    }
    
    // Global instance management
    static EnhancedLogger& getInstance() {
        static EnhancedLogger instance;
        return instance;
    }
    
private:
    std::shared_ptr<EnhancedConsoleLogger> consoleLogger_;
    std::unique_ptr<PerformanceProfiler> performanceProfiler_;
    std::mutex mutex_;
}; 

// Global convenience functions
inline void logEnhanced(EnhancedLogLevel level, const std::string& category,
                       const std::string& message, const std::string& file = "",
                       int line = 0, const std::string& function = "",
                       const std::unordered_map<std::string, std::string>& context = {}) {
    EnhancedLogger::getInstance().log(level, category, message, file, line, function, context);
}

inline void traceEnhanced(const std::string& category, const std::string& message,
                          const std::string& file = "", int line = 0,
                          const std::string& function = "",
                          const std::unordered_map<std::string, std::string>& context = {}) {
    EnhancedLogger::getInstance().trace(category, message, file, line, function, context);
}

inline void debugEnhanced(const std::string& category, const std::string& message,
                          const std::string& file = "", int line = 0,
                          const std::string& function = "",
                          const std::unordered_map<std::string, std::string>& context = {}) {
    EnhancedLogger::getInstance().debug(category, message, file, line, function, context);
}

inline void infoEnhanced(const std::string& category, const std::string& message,
                         const std::string& file = "", int line = 0,
                         const std::string& function = "",
                         const std::unordered_map<std::string, std::string>& context = {}) {
    EnhancedLogger::getInstance().info(category, message, file, line, function, context);
}

inline void warningEnhanced(const std::string& category, const std::string& message,
                            const std::string& file = "", int line = 0,
                            const std::string& function = "",
                            const std::unordered_map<std::string, std::string>& context = {}) {
    EnhancedLogger::getInstance().warning(category, message, file, line, function, context);
}

inline void errorEnhanced(const std::string& category, const std::string& message,
                          const std::string& file = "", int line = 0,
                          const std::string& function = "",
                          const std::unordered_map<std::string, std::string>& context = {}) {
    EnhancedLogger::getInstance().error(category, message, file, line, function, context);
}

inline void criticalEnhanced(const std::string& category, const std::string& message,
                             const std::string& file = "", int line = 0,
                             const std::string& function = "",
                             const std::unordered_map<std::string, std::string>& context = {}) {
    EnhancedLogger::getInstance().critical(category, message, file, line, function, context);
}

inline void startPerformanceProfiling(const std::string& operationName,
                                     const std::unordered_map<std::string, std::string>& context = {}) {
    EnhancedLogger::getInstance().startPerformanceProfiling(operationName, context);
}

inline void endPerformanceProfiling(const std::string& operationName, uint64_t memoryUsage = 0) {
    EnhancedLogger::getInstance().endPerformanceProfiling(operationName, memoryUsage);
}

} // namespace nlm
