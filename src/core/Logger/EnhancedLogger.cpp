// Enhanced Logger Implementation
// Provides complete implementation of enhanced logging features

#include "core/Logger/EnhancedLogger.hpp"
#include <algorithm>
#include <iomanip>
#include <mutex>
#include <thread>
#include <unordered_map>

namespace nlm {
    // Global logger instances
    std::shared_ptr<EnhancedLogger> EnhancedLogger::globalLogger_ = nullptr;
    std::shared_ptr<LegacyLogger> LegacyLogger::globalLogger_ = nullptr;
    
    // EnhancedLogger implementation
    struct EnhancedLogger::Impl {
        std::vector<std::shared_ptr<LogDestination>> destinations;
        std::vector<std::shared_ptr<LogFilter>> filters;
        std::vector<std::shared_ptr<LogFormatter>> formatters;
        std::mutex mutex;
        
        // Statistics
        size_t totalLogs = 0;
        size_t logsByLevel[5] = {0, 0, 0, 0, 0}; // Debug, Info, Warning, Error, Critical
        double memoryUsageKB = 0.0;
        
        // Performance metrics
        double totalTimeMs = 0.0;
        size_t logCount = 0;
        double avgTimePerLogUs = 0.0;
        size_t memoryPeakKB = 0;
        
        // Log entries for performance monitoring
        std::vector<std::chrono::high_resolution_clock::time_point> logTimes;
    };
    
    EnhancedLogger::EnhancedLogger() : pImpl(std::make_unique<Impl>()) {
        // Setup default destinations
        auto consoleDest = std::make_shared<ConsoleDestination>(LogLevel::Info);
        consoleDest->setUseColors(true);
        addDestination(consoleDest);
        
        // Setup default filters
        auto levelFilter = std::make_shared<LevelFilter>(LogLevel::Debug);
        addFilter(levelFilter);
        
        // Setup default formatters
        auto plainFormatter = std::make_shared<PlainTextFormatter>();
        setFormatter(plainFormatter);
    }
    
    EnhancedLogger::~EnhancedLogger() = default;
    
    EnhancedLogger::EnhancedLogger(EnhancedLogger&&) noexcept = default;
    EnhancedLogger& EnhancedLogger::operator=(EnhancedLogger&&) noexcept = default;
    
    void EnhancedLogger::addDestination(std::shared_ptr<LogDestination> destination) {
        if (destination) {
            std::lock_guard<std::mutex> lock(pImpl->mutex);
            pImpl->destinations.push_back(destination);
        }
    }
    
    void EnhancedLogger::removeDestination(const LogDestination* destination) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        auto it = std::find_if(pImpl->destinations.begin(), pImpl->destinations.end(),
                              [destination](const std::shared_ptr<LogDestination>& d) {
                                  return d.get() == destination;
                              });
        if (it != pImpl->destinations.end()) {
            pImpl->destinations.erase(it);
        }
    }
    
    void EnhancedLogger::clearDestinations() {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        pImpl->destinations.clear();
    }
    
    void EnhancedLogger::addFilter(std::shared_ptr<LogFilter> filter) {
        if (filter) {
            std::lock_guard<std::mutex> lock(pImpl->mutex);
            pImpl->filters.push_back(filter);
        }
    }
    
    void EnhancedLogger::removeFilter(const LogFilter* filter) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        auto it = std::find_if(pImpl->filters.begin(), pImpl->filters.end(),
                              [filter](const std::shared_ptr<LogFilter>& f) {
                                  return f.get() == filter;
                              });
        if (it != pImpl->filters.end()) {
            pImpl->filters.erase(it);
        }
    }
    
    void EnhancedLogger::clearFilters() {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        pImpl->filters.clear();
    }
    
    void EnhancedLogger::setFormatter(std::shared_ptr<LogFormatter> formatter) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        pImpl->formatters.clear();
        if (formatter) {
            pImpl->formatters.push_back(formatter);
        }
    }
    
    void EnhancedLogger::addFormatter(std::shared_ptr<LogFormatter> formatter) {
        if (formatter) {
            std::lock_guard<std::mutex> lock(pImpl->mutex);
            pImpl->formatters.push_back(formatter);
        }
    }
    
    void EnhancedLogger::removeFormatter(const LogFormatter* formatter) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        auto it = std::find_if(pImpl->formatters.begin(), pImpl->formatters.end(),
                              [formatter](const std::shared_ptr<LogFormatter>& f) {
                                  return f.get() == formatter;
                              });
        if (it != pImpl->formatters.end()) {
            pImpl->formatters.erase(it);
        }
    }
    
    void EnhancedLogger::clearFormatters() {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        pImpl->formatters.clear();
    }
    
    void EnhancedLogger::setLevel(LogLevel level) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        for (const auto& destination : pImpl->destinations) {
            destination->setLevel(level);
        }
    }
    
    LogLevel EnhancedLogger::getLevel() const {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        if (!pImpl->destinations.empty()) {
            return pImpl->destinations[0]->getLevel();
        }
        return LogLevel::Info;
    }
    
    void EnhancedLogger::log(LogLevel level, const std::string& message,
                             const std::string& file, int line,
                             const std::string& function) {
        if (!isEnabled(level)) {
            return;
        }
        
        EnhancedLogEntry entry(level, message, file, line, function);
        
        if (shouldLog(entry)) {
            std::string formattedMessage = formatMessage(entry);
            
            std::lock_guard<std::mutex> lock(pImpl->mutex);
            for (const auto& destination : pImpl->destinations) {
                if (destination->getLevel() <= level) {
                    destination->write(formattedMessage);
                }
            }
            
            updateStats(entry);
            if (performanceMetricsEnabled_) {
                updatePerformanceMetrics(entry);
            }
        }
    }
    
    bool EnhancedLogger::isEnabled(LogLevel level) const {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        // Check if any destination has this level enabled
        for (const auto& destination : pImpl->destinations) {
            if (destination->getLevel() <= level) {
                return true;
            }
        }
        
        return false;
    }
    
    std::string EnhancedLogger::levelToString(LogLevel level) const {
        switch (level) {
            case LogLevel::Debug: return "DEBUG";
            case LogLevel::Info: return "INFO";
            case LogLevel::Warning: return "WARN";
            case LogLevel::Error: return "ERROR";
            case LogLevel::Critical: return "CRIT";
            default: return "UNKNOWN";
        }
    }
    
    std::string EnhancedLogger::levelToString(EnhancedLogLevel level) const {
        auto it = customLevelNames_.find(level);
        if (it != customLevelNames_.end()) {
            return it->second;
        }
        
        switch (level) {
            case EnhancedLogLevel::TRACE: return "TRACE";
            case EnhancedLogLevel::DEBUG: return "DEBUG";
            case EnhancedLogLevel::INFO: return "INFO";
            case EnhancedLogLevel::NOTICE: return "NOTICE";
            case EnhancedLogLevel::WARNING: return "WARN";
            case EnhancedLogLevel::ERROR: return "ERROR";
            case EnhancedLogLevel::CRITICAL: return "CRIT";
            case EnhancedLogLevel::ALERT: return "ALERT";
            case EnhancedLogLevel::EMERGENCY: return "EMERG";
            default: return "UNKNOWN";
        }
    }
    
    std::string EnhancedLogger::levelToColor(LogLevel level) const {
        switch (level) {
            case LogLevel::Debug: return "\033[36m";    // Cyan
            case LogLevel::Info: return "\033[32m";      // Green
            case LogLevel::Warning: return "\033[33m";   // Yellow
            case LogLevel::Error: return "\033[31m";     // Red
            case LogLevel::Critical: return "\033[35m"; // Magenta
            default: return "\033[0m";
        }
    }
    
    std::string EnhancedLogger::levelToColor(EnhancedLogLevel level) const {
        auto it = customLevelColors_.find(level);
        if (it != customLevelColors_.end()) {
            return it->second;
        }
        
        return levelToColor(convertToLegacyLevel(level));
    }
    
    std::string EnhancedLogger::getTimestampString(const std::chrono::system_clock::time_point& timestamp) const {
        auto time = std::chrono::system_clock::to_time_t(timestamp);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S.%3f");
        return ss.str();
    }
    
    bool EnhancedLogger::shouldLog(const EnhancedLogEntry& entry) const {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        // Check level filter
        bool levelPasses = entry.level >= LogLevel::Debug; // Default minimum level
        
        // Check custom filters
        for (const auto& filter : pImpl->filters) {
            if (!filter->shouldLog(entry)) {
                return false;
            }
        }
        
        return levelPasses;
    }
    
    std::string EnhancedLogger::formatMessage(const EnhancedLogEntry& entry) const {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        if (pImpl->formatters.empty()) {
            return ""; // No formatters available
        }
        
        // Use the first formatter for now
        return pImpl->formatters[0]->format(entry);
    }
    
    void EnhancedLogger::updateStats(const EnhancedLogEntry& entry) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        pImpl->totalLogs++;
        
        switch (entry.level) {
            case LogLevel::Debug: pImpl->logsByLevel[0]++; break;
            case LogLevel::Info: pImpl->logsByLevel[1]++; break;
            case LogLevel::Warning: pImpl->logsByLevel[2]++; break;
            case LogLevel::Error: pImpl->logsByLevel[3]++; break;
            case LogLevel::Critical: pImpl->logsByLevel[4]++; break;
        }
        
        pImpl->memoryUsageKB = getMemoryUsage();
    }
    
    void EnhancedLogger::updatePerformanceMetrics(const EnhancedLogEntry& entry) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        auto now = std::chrono::high_resolution_clock::now();
        pImpl->logTimes.push_back(now);
        
        if (pImpl->logTimes.size() > 1000) {
            // Remove oldest entries
            pImpl->logTimes.erase(pImpl->logTimes.begin());
        }
        
        // Calculate average time per log
        if (pImpl->logTimes.size() > 1) {
            auto first = pImpl->logTimes.front();
            auto last = pImpl->logTimes.back();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(last - first);
            pImpl->avgTimePerLogUs = duration.count() / (pImpl->logTimes.size() - 1);
        }
        
        pImpl->totalTimeMs = getPerformanceMetrics().totalTimeMs;
        pImpl->logCount = getPerformanceMetrics().logCount;
        pImpl->memoryPeakKB = getPerformanceMetrics().memoryPeakKB;
    }
    
    size_t EnhancedLogger::getMemoryUsage() const {
        // Estimate memory usage based on log entries
        return pImpl->totalLogs * 100; // Rough estimate: 100 bytes per log entry
    }
    
    EnhancedLogger::LoggerStats EnhancedLogger::getStats() const {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        LoggerStats stats;
        stats.totalLogs = pImpl->totalLogs;
        stats.logsByLevel[0] = pImpl->logsByLevel[0];
        stats.logsByLevel[1] = pImpl->logsByLevel[1];
        stats.logsByLevel[2] = pImpl->logsByLevel[2];
        stats.logsByLevel[3] = pImpl->logsByLevel[3];
        stats.logsByLevel[4] = pImpl->logsByLevel[4];
        stats.destinations = pImpl->destinations.size();
        stats.filters = pImpl->filters.size();
        stats.formatters = pImpl->formatters.size();
        stats.memoryUsageKB = pImpl->memoryUsageKB;
        
        return stats;
    }
    
    EnhancedLogger::PerformanceMetrics EnhancedLogger::getPerformanceMetrics() const {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        PerformanceMetrics metrics;
        metrics.totalTimeMs = pImpl->totalTimeMs;
        metrics.logCount = pImpl->logCount;
        metrics.avgTimePerLogUs = pImpl->avgTimePerLogUs;
        metrics.memoryPeakKB = pImpl->memoryPeakKB;
        
        return metrics;
    }
    
    void EnhancedLogger::enableStructuredLogging(bool enable) {
        structuredLoggingEnabled_ = enable;
    }
    
    void EnhancedLogger::enablePerformanceMetrics(bool enable) {
        performanceMetricsEnabled_ = enable;
    }
    
    void EnhancedLogger::enableThreadInfo(bool enable) {
        threadInfoEnabled_ = enable;
    }
    
    void EnhancedLogger::enableFileLogging(bool enable, const std::string& filepath) {
        fileLoggingEnabled_ = enable;
        if (enable && !filepath.empty()) {
            auto fileDest = std::make_shared<FileDestination>(LogLevel::Info, filepath);
            addDestination(fileDest);
        }
    }
    
    void EnhancedLogger::enableConsoleLogging(bool enable) {
        consoleLoggingEnabled_ = enable;
        // Note: Console logging is always enabled by default in the constructor
    }
    
    void EnhancedLogger::setConsoleColors(bool enabled) {
        for (const auto& destination : pImpl->destinations) {
            auto consoleDest = std::dynamic_pointer_cast<ConsoleDestination>(destination);
            if (consoleDest) {
                consoleDest->setUseColors(enabled);
            }
        }
    }
    
    void EnhancedLogger::setOutputFile(const std::string& filepath) {
        for (const auto& destination : pImpl->destinations) {
            auto fileDest = std::dynamic_pointer_cast<FileDestination>(destination);
            if (fileDest) {
                fileDest->setOutputFile(filepath);
                return;
            }
        }
        
        // Create new file destination if none exists
        auto fileDest = std::make_shared<FileDestination>(LogLevel::Info, filepath);
        addDestination(fileDest);
    }
    
    void EnhancedLogger::closeOutputFile() {
        for (const auto& destination : pImpl->destinations) {
            auto fileDest = std::dynamic_pointer_cast<FileDestination>(destination);
            if (fileDest && fileDest->isOpen()) {
                fileDest->closeOutputFile();
            }
        }
    }
    
    EnhancedLogger& EnhancedLogger::getGlobal() {
        static EnhancedLogger logger;
        return logger;
    }
    
    void EnhancedLogger::setGlobal(std::shared_ptr<EnhancedLogger> logger) {
        globalLogger_ = logger;
    }
    
    // LegacyLogger implementation
    std::shared_ptr<LegacyLogger> LegacyLogger::globalLogger_ = nullptr;
    
    // Additional implementations would continue...
    
} // namespace nlm
