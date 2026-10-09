#pragma once

#include "Enhanced/LogFormatter.hpp"
#include "Enhanced/LogFilter.hpp"
#include "Enhanced/AsyncProcessor.hpp"
#include "Enhanced/Analysis.hpp"
#include "Enhanced/Management.hpp"
#include "Enhanced/Performance.hpp"
#include "Enhanced/Examples.hpp"
#include "Enhanced/EnhancedLogger.hpp"

namespace nlm {

// Main enhanced logging system interface
class EnhancedLoggingSystem {
public:
    EnhancedLoggingSystem() {
        // Initialize components
        setupDefaultFormatters();
        setupDefaultFilters();
        setupAsyncProcessor();
        setupLogAnalyzer();
        setupLogManager();
        setupPerformanceLogger();
    }
    
    ~EnhancedLoggingSystem() {
        shutdown();
    }
    
    // Core logging methods
    void log(const std::string& message, LogLevel level = LogLevel::Info) {
        EnhancedLogEntry entry;
        entry.level = level;
        entry.message = message;
        entry.file = "";
        entry.line = 0;
        entry.function = "";
        entry.timestamp = std::chrono::system_clock::now();
        entry.threadId = "main";
        entry.sequenceNumber = ++sequenceCounter_;
        
        logEntry(entry);
    }
    
    void log(const EnhancedLogEntry& entry) {
        logEntry(entry);
    }
    
    // Convenience logging methods
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "") {
        EnhancedLogEntry entry;
        entry.level = LogLevel::Debug;
        entry.message = message;
        entry.file = file;
        entry.line = line;
        entry.function = function;
        entry.timestamp = std::chrono::system_clock::now();
        entry.threadId = "main";
        entry.sequenceNumber = ++sequenceCounter_;
        
        logEntry(entry);
    }
    
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "") {
        EnhancedLogEntry entry;
        entry.level = LogLevel::Info;
        entry.message = message;
        entry.file = file;
        entry.line = line;
        entry.function = function;
        entry.timestamp = std::chrono::system_clock::now();
        entry.threadId = "main";
        entry.sequenceNumber = ++sequenceCounter_;
        
        logEntry(entry);
    }
    
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "") {
        EnhancedLogEntry entry;
        entry.level = LogLevel::Warning;
        entry.message = message;
        entry.file = file;
        entry.line = line;
        entry.function = function;
        entry.timestamp = std::chrono::system_clock::now();
        entry.threadId = "main";
        entry.sequenceNumber = ++sequenceCounter_;
        
        logEntry(entry);
    }
    
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "") {
        EnhancedLogEntry entry;
        entry.level = LogLevel::Error;
        entry.message = message;
        entry.file = file;
        entry.line = line;
        entry.function = function;
        entry.timestamp = std::chrono::system_clock::now();
        entry.threadId = "main";
        entry.sequenceNumber = ++sequenceCounter_;
        
        logEntry(entry);
    }
    
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "") {
        EnhancedLogEntry entry;
        entry.level = LogLevel::Critical;
        entry.message = message;
        entry.file = file;
        entry.line = line;
        entry.function = function;
        entry.timestamp = std::chrono::system_clock::now();
        entry.threadId = "main";
        entry.sequenceNumber = ++sequenceCounter_;
        
        logEntry(entry);
    }
    
    // Advanced logging methods
    void logWithTags(const std::string& message, const std::map<std::string, std::string>& tags, LogLevel level = LogLevel::Info) {
        EnhancedLogEntry entry;
        entry.level = level;
        entry.message = message;
        entry.file = "";
        entry.line = 0;
        entry.function = "";
        entry.timestamp = std::chrono::system_clock::now();
        entry.threadId = "main";
        entry.tags = tags;
        entry.sequenceNumber = ++sequenceCounter_;
        
        logEntry(entry);
    }
    
    void logWithMetadata(const std::string& message, const std::map<std::string, std::string>& metadata, LogLevel level = LogLevel::Info) {
        EnhancedLogEntry entry;
        entry.level = level;
        entry.message = message;
        entry.file = "";
        entry.line = 0;
        entry.function = "";
        entry.timestamp = std::chrono::system_clock::now();
        entry.threadId = "main";
        entry.metadata = metadata;
        entry.sequenceNumber = ++sequenceCounter_;
        
        logEntry(entry);
    }
    
    // System management methods
    void setLogLevel(LogLevel level) {
        currentLogLevel_ = level;
        if (enhancedLogger_) {
            enhancedLogger_->setLevel(level);
        }
    }
    
    LogLevel getLogLevel() const {
        return currentLogLevel_;
    }
    
    void enableAsyncProcessing(bool enabled) {
        asyncEnabled_ = enabled;
        if (enhancedLogger_) {
            enhancedLogger_->enableMonitoring(enabled);
        }
    }
    
    bool isAsyncProcessingEnabled() const {
        return asyncEnabled_;
    }
    
    void setOutputDirectory(const std::string& directory) {
        if (enhancedLogger_) {
            enhancedLogger_->setOutputDirectory(directory);
        }
    }
    
    void enableLogRotation(bool enabled, size_t maxFileSize = 10 * 1024 * 1024) {
        if (enabled && enhancedLogger_) {
            SizeRotationPolicy policy(maxFileSize);
            enhancedLogger_->setRotationPolicy(std::make_unique<SizeRotationPolicy>(policy));
        }
    }
    
    // Analysis and reporting
    std::string generateSystemReport() const {
        if (enhancedLogger_) {
            return enhancedLogger_->generateReport();
        }
        return "Enhanced logger not initialized.";
    }
    
    LogStatistics getStatistics() const {
        if (logAnalyzer_) {
            return logAnalyzer_->getStatistics();
        }
        return LogStatistics();
    }
    
    // Export/import functionality
    bool exportLogs(const std::string& filename, const std::string& format = "json") {
        if (enhancedLogger_) {
            return enhancedLogger_->exportLogs(filename, format);
        }
        return false;
    }
    
    bool importLogs(const std::string& filename, const std::string& format = "json") {
        if (enhancedLogger_) {
            return enhancedLogger_->importLogs(filename, format);
        }
        return false;
    }
    
    // Log search
    std::vector<EnhancedLogEntry> searchLogs(const std::string& pattern,
                                            const std::string& filePattern = "",
                                            LogLevel minLevel = LogLevel::Debug,
                                            const std::chrono::system_clock::time_point& startTime = std::chrono::system_clock::time_point(),
                                            const std::chrono::system_clock::time_point& endTime = std::chrono::system_clock::time_point()) {
        if (enhancedLogger_) {
            return enhancedLogger_->searchLogs(pattern, filePattern, minLevel, startTime, endTime);
        }
        return std::vector<EnhancedLogEntry>();
    }
    
    // Performance monitoring
    std::string getPerformanceReport() const {
        if (performanceLogger_) {
            return performanceLogger_->getPerformanceReport();
        }
        return "Performance logger not initialized.";
    }
    
    // Initialization and shutdown
    void initialize() {
        initialized_ = true;
        std::cout << "Enhanced logging system initialized." << std::endl;
    }
    
    void shutdown() {
        if (enhancedLogger_) {
            enhancedLogger_->enableMonitoring(false);
        }
        
        if (monitoringThread_.joinable()) {
            monitoringThread_.join();
        }
        
        initialized_ = false;
        std::cout << "Enhanced logging system shutdown." << std::endl;
    }
    
    bool isInitialized() const {
        return initialized_;
    }
    
private:
    void setupDefaultFormatters() {
        // Create and add default formatters
        auto jsonFormatter = LogFormatterFactory::createFormatter("JSON");
        auto csvFormatter = LogFormatterFactory::createFormatter("CSV");
        auto xmlFormatter = LogFormatterFactory::createFormatter("XML");
        
        // Add to enhanced logger (will be set up in setupAsyncProcessor)
    }
    
    void setupDefaultFilters() {
        // Create and add default filters
        auto levelFilter = std::make_unique<LevelFilter>(LogLevel::Info);
        defaultFilters_.push_back(std::move(levelFilter));
    }
    
    void setupAsyncProcessor() {
        if (enhancedLogger_) {
            enhancedLogger_->addFormatter(LogFormatterFactory::createFormatter("JSON"));
            enhancedLogger_->addFormatter(LogFormatterFactory::createFormatter("CSV"));
            enhancedLogger_->addFormatter(LogFormatterFactory::createFormatter("XML"));
        }
    }
    
    void setupLogAnalyzer() {
        if (enhancedLogger_) {
            // Log analyzer is already set up in enhanced logger
        }
    }
    
    void setupLogManager() {
        if (enhancedLogger_) {
            // Log manager is already set up in enhanced logger
        }
    }
    
    void setupPerformanceLogger() {
        if (enhancedLogger_) {
            // Performance logger is already set up in enhanced logger
        }
    }
    
    void logEntry(const EnhancedLogEntry& entry) {
        if (!initialized_) {
            return;
        }
        
        // Convert to LogEntry for backward compatibility
        LogEntry legacyEntry;
        legacyEntry.level = entry.level;
        legacyEntry.message = entry.message;
        legacyEntry.file = entry.file;
        legacyEntry.line = entry.line;
        legacyEntry.function = entry.function;
        legacyEntry.timestamp = entry.timestamp;
        
        // Log via enhanced logger
        if (enhancedLogger_) {
            enhancedLogger_->log(legacyEntry);
        }
        
        // Also add to analyzer
        if (logAnalyzer_) {
            logAnalyzer_->addLog(entry);
        }
        
        // Also log to performance logger
        if (performanceLogger_) {
            std::string operation = "log_" + levelToString(entry.level);
            performanceLogger_->logPerformance(operation, 0.0, sizeof(EnhancedLogEntry));
        }
    }
    
    // Components
    std::unique_ptr<EnhancedLogger> enhancedLogger_;
    std::unique_ptr<LogAnalyzer> logAnalyzer_;
    std::unique_ptr<PerformanceLogger> performanceLogger_;
    std::vector<std::unique_ptr<LogFilter>> defaultFilters_;
    
    // State
    bool initialized_ = false;
    bool asyncEnabled_ = true;
    LogLevel currentLogLevel_ = LogLevel::Info;
    uint64_t sequenceCounter_ = 0;
    std::thread monitoringThread_;
};

// Global enhanced logging system instance
getEnhancedLoggingSystem() {
    static EnhancedLoggingSystem system;
    return system;
}

// Convenience functions for backward compatibility
inline void log(const std::string& message, LogLevel level = LogLevel::Info) {
    getEnhancedLoggingSystem().log(message, level);
}

inline void logDebug(const std::string& message) {
    getEnhancedLoggingSystem().debug(message);
}

inline void logInfo(const std::string& message) {
    getEnhancedLoggingSystem().info(message);
}

inline void logWarning(const std::string& message) {
    getEnhancedLoggingSystem().warning(message);
}

inline void logError(const std::string& message) {
    getEnhancedLoggingSystem().error(message);
}

inline void logCritical(const std::string& message) {
    getEnhancedLoggingSystem().critical(message);
}

// Macro compatibility with original Logger class
#define NLM_LOG_ENHANCED_DEBUG(msg) logDebug(msg)
#define NLM_LOG_ENHANCED_INFO(msg) logInfo(msg)
#define NLM_LOG_ENHANCED_WARNING(msg) logWarning(msg)
#define NLM_LOG_ENHANCED_ERROR(msg) logError(msg)
#define NLM_LOG_ENHANCED_CRITICAL(msg) logCritical(msg)

} // namespace nlm
