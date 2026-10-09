#pragma once

#include <string>
#include <memory>
#include <mutex>
#include <vector>
#include "Logger.hpp"
#include "Enhanced/LogFormatter.hpp"
#include "Enhanced/LogFilter.hpp"
#include "Enhanced/AsyncProcessor.hpp"
#include "Enhanced/Analysis.hpp"
#include "Enhanced/Management.hpp"
#include "Enhanced/Performance.hpp"

namespace nlm {

// Enhanced logger that maintains backward compatibility with original Logger class
class EnhancedLogger : public ILogger {
public:
    EnhancedLogger() : asyncProcessor_(std::make_unique<AsyncLogProcessor>(std::thread::hardware_concurrency())),
                      logAnalyzer_(std::make_unique<LogAnalyzer>(100000)),
                      logManager_(std::make_unique<LogManager>()),
                      performanceLogger_(std::make_unique<PerformanceLogger>()) {
        
        // Set up default formatters
        asyncProcessor_->addFormatter(LogFormatterFactory::createFormatter("JSON"));
        asyncProcessor_->addFormatter(LogFormatterFactory::createFormatter("CSV"));
        asyncProcessor_->addFormatter(LogFormatterFactory::createFormatter("XML"));
        
        // Set up async processor callback
        asyncProcessor_->setOutputCallback([this](const std::string& formatted) {
            // Store for analysis
            EnhancedLogEntry entry;
            parseLogEntry(entry, formatted);
            logAnalyzer_->addLog(entry);
        });
    }
    
    ~EnhancedLogger() override {
        asyncProcessor_->waitForCompletion();
    }
    
    // ILogger interface implementation
    void log(const LogEntry& entry) override {
        // Convert to enhanced log entry
        EnhancedLogEntry enhancedEntry;
        enhancedEntry.level = entry.level;
        enhancedEntry.message = entry.message;
        enhancedEntry.file = entry.file;
        enhancedEntry.line = entry.line;
        enhancedEntry.function = entry.function;
        enhancedEntry.timestamp = entry.timestamp;
        enhancedEntry.sequenceNumber = ++sequenceCounter_;
        
        // Add to async processor
        asyncProcessor_->addLog(std::make_unique<EnhancedLogEntry>(enhancedEntry));
        
        // Add to analyzer
        logAnalyzer_->addLog(enhancedEntry);
        
        // Log performance metrics
        performanceLogger_->logPerformance("log_processing", 0.0, sizeof(EnhancedLogEntry));
    }
    
    void setLevel(LogLevel level) override {
        currentLevel_ = level;
        asyncProcessor_->setFilter(std::make_unique<LevelFilter>(level));
    }
    
    LogLevel getLevel() const override {
        return currentLevel_;
    }
    
    // Enhanced logging features
    void addFormatter(std::unique_ptr<LogFormatter> formatter) {
        asyncProcessor_->addFormatter(std::move(formatter));
    }
    
    void removeFormatter(const std::string& name) {
        // Implementation would remove formatter by name
    }
    
    void addFilter(std::unique_ptr<LogFilter> filter) {
        asyncProcessor_->setFilter(std::move(filter));
    }
    
    void clearFilters() {
        asyncProcessor_->setFilter(nullptr);
    }
    
    void setOutputDirectory(const std::string& directory) {
        logManager_->setLogDirectory(directory);
    }
    
    void rotateLogs() {
        // Create a default size-based rotation policy
        SizeRotationPolicy policy(10 * 1024 * 1024); // 10MB
        logManager_->rotateLogs(policy);
    }
    
    // Log analysis and reporting
    std::string generateReport() const {
        std::stringstream report;
        report << "=== Enhanced Logger Report ===\n";
        
        // Add statistics
        LogStatistics stats = logAnalyzer_->getStatistics();
        report << "Statistics:\n";
        report << "  Total logs: " << stats.totalCount << "\n";
        report << "  Errors: " << stats.errorCount << "\n";
        report << "  Warnings: " << stats.warningCount << "\n";
        report << "  Info: " << stats.infoCount << "\n";
        report << "  Debug: " << stats.debugCount << "\n";
        report << "  Critical: " << stats.criticalCount << "\n";
        
        // Add pattern matches
        std::vector<PatternMatch> patterns = logAnalyzer_->getPatternMatches();
        report << "\nPattern Matches: " << patterns.size() << "\n";
        for (const auto& pattern : patterns) {
            report << "  - " << pattern.type << ": " << pattern.pattern << " (confidence: " 
                   << pattern.confidence << ")\n";
        }
        
        // Add performance metrics
        std::string perfReport = performanceLogger_->getPerformanceReport();
        report << "\nPerformance Metrics:\n" << perfReport << "\n";
        
        return report.str();
    }
    
    // Export/import functionality
    bool exportLogs(const std::string& filename, const std::string& format = "json") {
        // Get logs from analyzer
        std::vector<EnhancedLogEntry> logs = getAllLogs();
        
        // Export using log manager
        return logManager_->exportLogs(filename, logs, format);
    }
    
    bool importLogs(const std::string& filename, const std::string& format = "json") {
        // Import using log manager
        std::vector<EnhancedLogEntry> logs = logManager_->importLogs(filename, format);
        
        // Add logs to analyzer
        for (const auto& log : logs) {
            logAnalyzer_->addLog(log);
        }
        
        return true;
    }
    
    // Search functionality
    std::vector<EnhancedLogEntry> searchLogs(const std::string& pattern,
                                            const std::string& filePattern = "",
                                            LogLevel minLevel = LogLevel::Debug,
                                            const std::chrono::system_clock::time_point& startTime = std::chrono::system_clock::time_point(),
                                            const std::chrono::system_clock::time_point& endTime = std::chrono::system_clock::time_point()) {
        return logManager_->searchLogs(pattern, filePattern, minLevel, startTime, endTime);
    }
    
    // Get all logs
    std::vector<EnhancedLogEntry> getAllLogs() const {
        // This would need to be implemented to retrieve logs from the analyzer
        // For now, return empty vector
        return std::vector<EnhancedLogEntry>();
    }
    
    // Performance monitoring
    void enableMonitoring(bool enabled) {
        monitoringEnabled_ = enabled;
        if (enabled) {
            monitoringThread_ = std::thread(&EnhancedLogger::monitoringThread, this);
        } else {
            if (monitoringThread_.joinable()) {
                monitoringThread_.join();
            }
        }
    }
    
    void setLogDirectory(const std::string& directory) {
        logManager_->setLogDirectory(directory);
    }
    
    void setMaxLogFiles(size_t maxFiles) {
        logManager_->setMaxLogFiles(maxFiles);
    }
    
    void setRotationPolicy(std::unique_ptr<RotationPolicy> policy) {
        rotationPolicy_ = std::move(policy);
    }
    
private:
    void monitoringThread() {
        while (monitoringEnabled_) {
            std::this_thread::sleep_for(std::chrono::minutes(5));
            
            // Generate and log monitoring report
            std::string report = generateReport();
            std::cout << "[Monitoring] " << report << std::endl;
        }
    }
    
    void parseLogEntry(EnhancedLogEntry& entry, const std::string& formattedLog) const {
        // Parse formatted log entry back to EnhancedLogEntry
        // This is a simplified implementation
        entry.level = LogLevel::Info;
        entry.message = formattedLog;
        entry.sequenceNumber = ++parseCounter_;
    }
    
    // Helper function to convert string to LogLevel
    LogLevel stringToLogLevel(const std::string& levelStr) const {
        if (levelStr == "DEBUG") return LogLevel::Debug;
        if (levelStr == "INFO") return LogLevel::Info;
        if (levelStr == "WARNING") return LogLevel::Warning;
        if (levelStr == "ERROR") return LogLevel::Error;
        if (levelStr == "CRITICAL") return LogLevel::Critical;
        return LogLevel::Info;
    }
    
    // Components
    std::unique_ptr<AsyncLogProcessor> asyncProcessor_;
    std::unique_ptr<LogAnalyzer> logAnalyzer_;
    std::unique_ptr<LogManager> logManager_;
    std::unique_ptr<PerformanceLogger> performanceLogger_;
    
    // State
    LogLevel currentLevel_ = LogLevel::Info;
    std::atomic<uint64_t> sequenceCounter_ = 0;
    std::atomic<uint64_t> parseCounter_ = 0;
    bool monitoringEnabled_ = false;
    std::thread monitoringThread_;
    std::unique_ptr<RotationPolicy> rotationPolicy_;
};

// Global enhanced logger instance
EnhancedLogger& getEnhancedLogger() {
    static EnhancedLogger enhancedLogger;
    return enhancedLogger;
}

// Convenience functions for enhanced logging
void logEnhanced(const EnhancedLogEntry& entry) {
    getEnhancedLogger().logEnhancedEntry(entry);
}

void logWithFilter(const EnhancedLogEntry& entry, std::unique_ptr<LogFilter> filter) {
    getEnhancedLogger().addFilter(std::move(filter));
    getEnhancedLogger().logEnhancedEntry(entry);
}

void exportEnhancedLogs(const std::string& filename, const std::string& format = "json") {
    getEnhancedLogger().exportLogs(filename, format);
}

void importEnhancedLogs(const std::string& filename, const std::string& format = "json") {
    getEnhancedLogger().importLogs(filename, format);
}

} // namespace nlm
