#include "EnhancedLogger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <thread>
#include <unordered_map>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <map>
#include <set>
#include <cstring>

namespace nlm {

// EnhancedConsoleLogger implementation
struct AdvancedConsoleLogger::Impl {
    DebugLogLevel level = DebugLogLevel::INFO;
    bool useColors = true;
    std::map<std::string, std::string> colorMap;
    std::ofstream fileOut;
    std::mutex mutex;
    std::vector<ComponentFilter> filters;
    bool performanceTrackingEnabled = false;
    std::vector<PerformanceMetrics> performanceMetrics;
    std::string timestampFormat = "%Y-%m-%d %H:%M:%S";
    bool includeThreadId = true;
    bool includeSequenceNumbers = true;
    uint64_t sequenceCounter = 0;
    size_t maxFileSize = 10 * 1024 * 1024;  // 10MB
    size_t backupCount = 3;
    std::string currentLogFile;
};

AdvancedConsoleLogger::AdvancedConsoleLogger() : pImpl(std::make_unique<Impl>()) {
    // Initialize default colors
    pImpl->colorMap = {
        "DEBUG", "\033[36m",    // Cyan
        "INFO", "\033[32m",     // Green
        "WARNING", "\033[33m",  // Yellow
        "ERROR", "\033[31m",    // Red
        "CRITICAL", "\033[35m", // Magenta
        "Thread", "\033[90m",   // Gray
        "Sequence", "\033[90m"  // Gray
    };
}

AdvancedConsoleLogger::AdvancedConsoleLogger(DebugLogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
    // Initialize default colors
    pImpl->colorMap = {
        "DEBUG", "\033[36m",    // Cyan
        "INFO", "\033[32m",     // Green
        "WARNING", "\033[33m",  // Yellow
        "ERROR", "\033[31m",    // Red
        "CRITICAL", "\033[35m", // Magenta
        "Thread", "\033[90m",   // Gray
        "Sequence", "\033[90m"  // Gray
    };
}

AdvancedConsoleLogger::~AdvancedConsoleLogger() = default;

void AdvancedConsoleLogger::log(const DebugLogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    if (!shouldLogComponent(entry)) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Increment sequence number
    if (pImpl->includeSequenceNumbers) {
        pImpl->sequenceCounter++;
    }
    
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "]";
    
    if (!entry.component.empty()) {
        oss << "[" << entry.component << "]";
    }
    
    if (pImpl->includeThreadId) {
        oss << "[Thread:" << entry.threadId << "]";
    }
    
    // Format timestamp
    oss << "[" << formatTimestamp(entry.timestamp) << "]";
    
    if (pImpl->includeSequenceNumbers) {
        oss << "[Seq:" << pImpl->sequenceCounter << "]";
    }
    
    if (!entry.tags.empty()) {
        oss << "[";
        for (size_t i = 0; i < entry.tags.size(); ++i) {
            if (i > 0) oss << ",";
            oss << entry.tags[i];
        }
        oss << "]";
    }
    
    oss << " " << entry.message;
    
    std::string output = oss.str();
    
    // Console output with colors
    if (pImpl->useColors) {
        std::cout << applyColors(output, entry.component, entry.level) << "\033[0m\n";
    } else {
        std::cout << output << "\n";
    }
    
    // File output with rotation
    if (!pImpl->currentLogFile.empty()) {
        writeToFile(output);
    }
    
    // Record performance metrics if enabled
    if (pImpl->performanceTrackingEnabled) {
        recordPerformanceMetric(entry.component, entry.level, entry.message.length());
    }
}

void AdvancedConsoleLogger::setLevel(DebugLogLevel level) {
    pImpl->level = level;
}

DebugLogLevel AdvancedConsoleLogger::getLevel() const {
    return pImpl->level;
}

void AdvancedConsoleLogger::setUseColors(bool useColors) {
    pImpl->useColors = useColors;
}

bool AdvancedConsoleLogger::getUseColors() const {
    return pImpl->useColors;
}

void AdvancedConsoleLogger::setColorMap(const std::map<std::string, std::string>& colors) {
    pImpl->colorMap = colors;
}

void AdvancedConsoleLogger::addComponentFilter(const ComponentFilter& filter) {
    pImpl->filters.push_back(filter);
}

void AdvancedConsoleLogger::removeComponentFilter(const std::string& name) {
    pImpl->filters.erase(std::remove_if(pImpl->filters.begin(), pImpl->filters.end(),
                                      [&name](const ComponentFilter& f) { return f.name == name; }),
                        pImpl->filters.end());
}

void AdvancedConsoleLogger::clearComponentFilters() {
    pImpl->filters.clear();
}

void AdvancedConsoleLogger::setOutputFile(const std::string& filepath) {
    pImpl->currentLogFile = filepath;
    // Rotate if file exists and is too large
    if (std::filesystem::exists(filepath) && std::filesystem::file_size(filepath) > pImpl->maxFileSize) {
        rotateLogFile();
    }
    pImpl->fileOut.open(filepath, std::ios::app);
}

void AdvancedConsoleLogger::closeOutputFile() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
    pImpl->currentLogFile.clear();
}

void AdvancedConsoleLogger::setMaxFileSize(size_t maxSize) {
    pImpl->maxFileSize = maxSize;
}

void AdvancedConsoleLogger::setBackupCount(size_t count) {
    pImpl->backupCount = count;
}

void AdvancedConsoleLogger::setTimestampFormat(const std::string& format) {
    pImpl->timestampFormat = format;
}

void AdvancedConsoleLogger::setIncludeThreadId(bool include) {
    pImpl->includeThreadId = include;
}

void AdvancedConsoleLogger::setIncludeSequenceNumbers(bool include) {
    pImpl->includeSequenceNumbers = include;
}

void AdvancedConsoleLogger::enablePerformanceTracking(bool enable) {
    pImpl->performanceTrackingEnabled = enable;
}

bool AdvancedConsoleLogger::isPerformanceTrackingEnabled() const {
    return pImpl->performanceTrackingEnabled;
}

std::vector<PerformanceMetrics> AdvancedConsoleLogger::getRecentPerformanceMetrics(size_t limit) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<PerformanceMetrics> result(pImpl->performanceMetrics.begin(),
                                           pImpl->performanceMetrics.end());
    if (result.size() > limit) {
        result.resize(limit);
    }
    return result;
}

std::string AdvancedConsoleLogger::levelToString(DebugLogLevel level) const {
    switch (level) {
        case DebugLogLevel::DEBUG: return "DEBUG";
        case DebugLogLevel::INFO: return "INFO";
        case DebugLogLevel::WARNING: return "WARN";
        case DebugLogLevel::ERROR: return "ERROR";
        case DebugLogLevel::CRITICAL: return "CRIT";
        default: return "UNKNOWN";
    }
}

std::string AdvancedConsoleLogger::componentToColor(const std::string& component) const {
    // Use hash to get deterministic color
    std::hash<std::string> hasher;
    int hash = static_cast<int>(hasher(component)) % 6;
    std::string colors[] = {"\033[31m", "\033[32m", "\033[33m", "\033[34m", "\033[35m", "\033[36m"};
    return colors[hash];
}

std::string AdvancedConsoleLogger::formatTimestamp(const std::chrono::system_clock::time_point& timestamp) const {
    auto time = std::chrono::system_clock::to_time_t(timestamp);
    std::tm tm_buf;
    localtime_r(&time, &tm_buf); // thread-safe version
    
    if (pImpl->timestampFormat == "%Y-%m-%d %H:%M:%S") {
        std::stringstream ss;
        ss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
        return ss.str();
    } else if (pImpl->timestampFormat == "%H:%M:%S") {
        std::stringstream ss;
        ss << std::put_time(&tm_buf, "%H:%M:%S");
        return ss.str();
    } else {
        std::stringstream ss;
        ss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
}

bool AdvancedConsoleLogger::shouldLogComponent(const DebugLogEntry& entry) const {
    // If no filters, log everything
    if (pImpl->filters.empty()) {
        return true;
    }
    
    // Check each filter
    for (const auto& filter : pImpl->filters) {
        if (!filter.enabled) continue;
        
        bool componentMatches = false;
        if (filter.name.empty() || entry.component == filter.name) {
            componentMatches = true;
        } else {
            for (const auto& pattern : filter.includePatterns) {
                if (entry.component.find(pattern) != std::string::npos) {
                    componentMatches = true;
                    break;
                }
            }
        }
        
        if (!componentMatches) continue;
        
        // Check if entry matches exclude patterns
        for (const auto& pattern : filter.excludePatterns) {
            if (entry.component.find(pattern) != std::string::npos) {
                return false;
            }
        }
        
        // Check level
        if (entry.level < filter.minLevel) {
            return false;
        }
        
        // Check tags
        bool tagMatches = filter.tags.empty();
        if (!tagMatches) {
            for (const auto& tag : filter.tags) {
                if (std::find(entry.tags.begin(), entry.tags.end(), tag) != entry.tags.end()) {
                    tagMatches = true;
                    break;
                }
            }
        }
        
        if (!tagMatches) continue;
        
        return true;
    }
    
    return false;
}

std::string AdvancedConsoleLogger::applyColors(const std::string& text, const std::string& component, DebugLogLevel level) const {
    std::string result;
    
    // Apply level color
    auto levelIt = pImpl->colorMap.find(levelToString(level));
    if (levelIt != pImpl->colorMap.end()) {
        result += levelIt->second;
    }
    
    // Apply component color if different from level
    if (!component.empty()) {
        std::string componentColor = componentToColor(component);
        // Check if this component would get the same color as level
        std::string levelColor = (levelIt != pImpl->colorMap.end()) ? levelIt->second : "\033[0m";
        
        // Simple heuristic: if component would get different color, apply it
        bool shouldApplyComponentColor = true;
        
        if (shouldApplyComponentColor && componentColor != levelColor) {
            result += componentColor;
        }
    }
    
    result += text;
    return result;
}

void AdvancedConsoleLogger::rotateLogFile() {
    if (!pImpl->currentLogFile.empty() && pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
    
    // Create backup files
    for (size_t i = pImpl->backupCount - 1; i > 0; --i) {
        std::string from = pImpl->currentLogFile + "." + std::to_string(i - 1);
        std::string to = pImpl->currentLogFile + "." + std::to_string(i);
        if (std::filesystem::exists(from)) {
            std::filesystem::rename(from, to);
        }
    }
    
    // Move current log to backup
    if (std::filesystem::exists(pImpl->currentLogFile)) {
        std::filesystem::rename(pImpl->currentLogFile, pImpl->currentLogFile + ".0");
    }
    
    // Reopen for appending
    pImpl->fileOut.open(pImpl->currentLogFile, std::ios::app);
}

void AdvancedConsoleLogger::writeToFile(const std::string& message) {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut << message << "\n";
        if (std::filesystem::file_size(pImpl->currentLogFile) > pImpl->maxFileSize) {
            rotateLogFile();
        }
    }
}

void AdvancedConsoleLogger::recordPerformanceMetric(const std::string& operation, DebugLogLevel level, size_t messageSize) {
    PerformanceMetrics metrics;
    metrics.operationName = operation + "_" + levelToString(level);
    metrics.duration = std::chrono::duration<double>(0.001); // Placeholder duration
    metrics.memoryUsage = messageSize;
    metrics.callCount = 1;
    metrics.avgDuration = metrics.duration.count();
    metrics.minDuration = metrics.duration.count();
    metrics.maxDuration = metrics.duration.count();
    
    std::stringstream ss;
    ss << std::this_thread::get_id();
    metrics.threadId = ss.str();
    
    metrics.timestamp = std::chrono::system_clock::now();
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->performanceMetrics.push_back(metrics);
}

// EnhancedLogger implementation
struct EnhancedLogger::Impl {
    bool debugLoggingEnabled = false;
    DebugLogLevel logLevel = DebugLogLevel::INFO;
    std::shared_ptr<AdvancedConsoleLogger> consoleLogger;
    std::mutex mutex;
    std::unordered_map<std::string, std::pair<DebugLogLevel, bool>> componentFilters;
    bool memoryTrackingEnabled = false;
    bool configValidationEnabled = false;
    size_t maxRecoveryAttempts = 10;
    std::unordered_map<std::string, std::chrono::system_clock::time_point> performanceTimers;
    std::vector<std::string> performanceLoggers;
};

EnhancedLogger::EnhancedLogger() : pImpl(std::make_unique<Impl>()) {
    pImpl->consoleLogger = std::make_shared<AdvancedConsoleLogger>();
    pImpl->consoleLogger->setLevel(DebugLogLevel::INFO);
    pImpl->consoleLogger->setUseColors(true);
}

EnhancedLogger::~EnhancedLogger() = default;

EnhancedLogger& EnhancedLogger::getGlobal() {
    static EnhancedLogger globalLogger;
    return globalLogger;
}

void EnhancedLogger::initialize(bool enableDebugLogging, DebugLogLevel level) {
    enableDebugLogging(enableDebugLogging);
    setLogLevel(level);
}

void EnhancedLogger::enableDebugLogging(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->debugLoggingEnabled = enable;
    if (enable) {
        pImpl->consoleLogger->setLevel(DebugLogLevel::DEBUG);
    } else {
        pImpl->consoleLogger->setLevel(DebugLogLevel::INFO);
    }
}

bool EnhancedLogger::isDebugLoggingEnabled() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->debugLoggingEnabled;
}

void EnhancedLogger::setLogLevel(DebugLogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->logLevel = level;
    pImpl->consoleLogger->setLevel(level);
}

DebugLogLevel EnhancedLogger::getLogLevel() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->logLevel;
}

void EnhancedLogger::addComponentFilter(const ComponentFilter& filter) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->componentFilters[filter.name] = std::make_pair(filter.minLevel, filter.enabled);
}

void EnhancedLogger::removeComponentFilter(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->componentFilters.erase(name);
}

void EnhancedLogger::clearComponentFilters() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->componentFilters.clear();
}

void EnhancedLogger::startPerformanceMeasurement(const std::string& operation) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->performanceTimers[operation] = std::chrono::system_clock::now();
}

void EnhancedLogger::stopPerformanceMeasurement(const std::string& operation) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->performanceTimers.find(operation);
    if (it != pImpl->performanceTimers.end()) {
        auto startTime = it->second;
        auto endTime = std::chrono::system_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        // Log performance
        std::ostringstream oss;
        oss << "Performance: " << operation << " took " << duration.count() << " ms";
        info("performance", oss.str());
        
        pImpl->performanceTimers.erase(it);
    }
}

void EnhancedLogger::enableMemoryTracking(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->memoryTrackingEnabled = enable;
}

bool EnhancedLogger::isMemoryTrackingEnabled() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->memoryTrackingEnabled;
}

void EnhancedLogger::log(DebugLogLevel level, const std::string& message, 
                         const std::string& file, int line,
                         const std::string& function,
                         const std::string& component) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!isLoggable(level, component)) {
        return;
    }
    
    DebugLogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.component = component;
    entry.threadId = std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    entry.filtered = false;
    
    pImpl->consoleLogger->log(entry);
}

void EnhancedLogger::debug(const std::string& message, const std::string& component,
                           const std::string& file, int line,
                           const std::string& function) {
    log(DebugLogLevel::DEBUG, message, file, line, function, component);
}

void EnhancedLogger::info(const std::string& message, const std::string& component,
                          const std::string& file, int line,
                          const std::string& function) {
    log(DebugLogLevel::INFO, message, file, line, function, component);
}

void EnhancedLogger::warning(const std::string& message, const std::string& component,
                             const std::string& file, int line,
                             const std::string& function) {
    log(DebugLogLevel::WARNING, message, file, line, function, component);
}

void EnhancedLogger::error(const std::string& message, const std::string& component,
                           const std::string& file, int line,
                           const std::string& function) {
    log(DebugLogLevel::ERROR, message, file, line, function, component);
}

void EnhancedLogger::critical(const std::string& message, const std::string& component,
                              const std::string& file, int line,
                              const std::string& function) {
    log(DebugLogLevel::CRITICAL, message, file, line, function, component);
}

bool EnhancedLogger::isLoggable(DebugLogLevel level, const std::string& component) const {
    if (!pImpl->debugLoggingEnabled) {
        return false;
    }
    
    // Check if level is enabled
    if (level < pImpl->logLevel) {
        return false;
    }
    
    // Check component filters
    auto it = pImpl->componentFilters.find(component);
    if (it != pImpl->componentFilters.end()) {
        if (!it->second.second) { // Not enabled
            return false;
        }
        if (level < it->second.first) { // Below minimum level
            return false;
        }
    }
    
    return true;
}

EnhancedLogger::LogStream::LogStream(DebugLogLevel level, const std::string& component,
                                     const std::string& file, int line,
                                     const std::string& function)
    : level_(level), component_(component), file_(file), line_(line), function_(function) {}

EnhancedLogger::LogStream::~LogStream() {
    EnhancedLogger::getGlobal().log(level_, oss_.str(), component_, file_, line_, function_);
}

EnhancedLogger::LogStream EnhancedLogger::debugStream(const std::string& component,
                                                     const std::string& file, int line,
                                                     const std::string& function) {
    return LogStream(DebugLogLevel::DEBUG, component, file, line, function);
}

EnhancedLogger::LogStream EnhancedLogger::infoStream(const std::string& component,
                                                    const std::string& file, int line,
                                                    const std::string& function) {
    return LogStream(DebugLogLevel::INFO, component, file, line, function);
}

EnhancedLogger::LogStream EnhancedLogger::warningStream(const std::string& component,
                                                       const std::string& file, int line,
                                                       const std::string& function) {
    return LogStream(DebugLogLevel::WARNING, component, file, line, function);
}

EnhancedLogger::LogStream EnhancedLogger::errorStream(const std::string& component,
                                                     const std::string& file, int line,
                                                     const std::string& function) {
    return LogStream(DebugLogLevel::ERROR, component, file, line, function);
}

EnhancedLogger::LogStream EnhancedLogger::criticalStream(const std::string& component,
                                                        const std::string& file, int line,
                                                        const std::string& function) {
    return LogStream(DebugLogLevel::CRITICAL, component, file, line, function);
}

void EnhancedLogger::logPhase6Step(const std::string& phase, const std::string& component,
                                   const std::string& operation, bool success,
                                   const std::string& error) {
    std::stringstream ss;
    ss << "Phase6 " << phase << ": " << component << " -> " << operation;
    if (success) {
        ss << " SUCCESS";
        info("integration", ss.str());
    } else {
        ss << " FAILED: " << error;
        error("integration", ss.str());
    }
}

void EnhancedLogger::logSystemConnection(const std::string& source, const std::string& target,
                                        bool connected, const std::string& details) {
    std::stringstream ss;
    ss << "Connection: " << source << " -> " << target;
    if (connected) {
        ss << " ESTABLISHED";
        info("connection", ss.str());
        if (!details.empty()) {
            info("connection", "Details: " + details);
        }
    } else {
        ss << " FAILED";
        error("connection", ss.str());
        if (!details.empty()) {
            error("connection", "Details: " + details);
        }
    }
}

void EnhancedLogger::logMemoryTrace(const std::string& type, size_t size,
                                    const std::vector<float>& data) {
    std::stringstream ss;
    ss << "Memory " << type << ": " << size << " bytes";
    info("memory", ss.str());
    
    if (!data.empty()) {
        double min = *std::min_element(data.begin(), data.end());
        double max = *std::max_element(data.begin(), data.end());
        double avg = std::accumulate(data.begin(), data.end(), 0.0) / data.size();
        info("memory", "  Min: " + std::to_string(min) + ", Max: " + std::to_string(max) + ", Avg: " + std::to_string(avg));
    }
}

void EnhancedLogger::logNeuromodulation(const std::string& system, float value, float rate) {
    std::stringstream ss;
    ss << "Neuromodulation " << system << ": " << value;
    if (rate > 0) {
        ss << " (" << rate << " Hz)";
    }
    info("neuromodulation", ss.str());
}

void EnhancedLogger::logDevelopmentStage(const std::string& stage, float progress,
                                        const std::string& details) {
    std::stringstream ss;
    ss << "Development " << stage << ": " << (progress * 100) << "%";
    if (!details.empty()) {
        ss << " - " << details;
    }
    info("development", ss.str());
}

void EnhancedLogger::validateConfigParameter(const std::string& key, const ConfigValue& value,
                                             bool isValid, const std::string& error,
                                             const std::string& source) {
    std::stringstream ss;
    ss << "Config validation: " << key << " = " << configValueToString(value);
    if (isValid) {
        ss << " [VALID]";
        debug("config", ss.str());
    } else {
        ss << " [INVALID]";
        error("config", ss.str());
        if (!error.empty()) {
            error("config", "  Error: " + error);
        }
    }
    
    if (!source.empty()) {
        debug("config", "  Source: " + source);
    }
}

void EnhancedLogger::enableConfigValidation(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->configValidationEnabled = enable;
}

bool EnhancedLogger::isConfigValidationEnabled() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->configValidationEnabled;
}

std::vector<ConfigValidationResult> EnhancedLogger::getValidationHistory() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    // This would need to be stored in pImpl
    return std::vector<ConfigValidationResult>();
}

void EnhancedLogger::logError(const ErrorContext& error) {
    std::stringstream ss;
    ss << "Error " << error.errorId << " [" << error.errorType << "]";
    error("error", ss.str());
    
    if (!error.errorMessage.empty()) {
        error("error", "  Message: " + error.errorMessage);
    }
    
    if (!error.contextData.empty()) {
        error("error", "  Context:");
        for (const auto& [key, value] : error.contextData) {
            error("error", "    " + key + ": " + value);
        }
    }
}

void EnhancedLogger::logRecoveryAttempt(const RecoveryAttempt& attempt) {
    std::stringstream ss;
    ss << "Recovery: " << attempt.attemptId << " - " << attempt.attemptType;
    ss << " - " << (attempt.success ? "SUCCESS" : "FAILED");
    info("recovery", ss.str());
    
    if (!attempt.errorMessage.empty()) {
        info("recovery", "  Error: " + attempt.errorMessage);
    }
}

void EnhancedLogger::setMaxRecoveryAttempts(size_t max) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->maxRecoveryAttempts = max;
}

std::vector<ErrorContext> EnhancedLogger::getRecentErrors(size_t limit) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    // This would need to be stored in pImpl
    return std::vector<ErrorContext>();
}

std::vector<RecoveryAttempt> EnhancedLogger::getRecoveryAttempts(const std::string& errorId) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    // This would need to be stored in pImpl
    return std::vector<RecoveryAttempt>();
}

std::string EnhancedLogger::getStatisticsReport() const {
    std::stringstream ss;
    ss << "=== Enhanced Logger Statistics ===\n";
    ss << "Debug logging enabled: " << (pImpl->debugLoggingEnabled ? "YES" : "NO") << "\n";
    ss << "Log level: " << static_cast<int>(pImpl->logLevel) << "\n";
    ss << "Component filters: " << pImpl->componentFilters.size() << "\n";
    ss << "Memory tracking: " << (pImpl->memoryTrackingEnabled ? "ENABLED" : "DISABLED") << "\n";
    ss << "Config validation: " << (pImpl->configValidationEnabled ? "ENABLED" : "DISABLED") << "\n";
    ss << "Active performance timers: " << pImpl->performanceTimers.size() << "\n";
    return ss.str();
}

void EnhancedLogger::generateDebugReport(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (file.is_open()) {
        file << getStatisticsReport();
        file << "\n=== Log File Output ===\n";
        
        // Add recent log entries if available
        file.close();
    }
}

void EnhancedLogger::clearDebugData() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->performanceTimers.clear();
    // Clear other debug data as needed
}

std::string EnhancedLogger::configValueToString(const ConfigValue& value) const {
    std::stringstream ss;
    // Handle different variant types
    if (std::holds_alternative<int>(value)) {
        ss << std::get<int>(value);
    } else if (std::holds_alternative<int64_t>(value)) {
        ss << std::get<int64_t>(value);
    } else if (std::holds_alternative<double>(value)) {
        ss << std::get<double>(value);
    } else if (std::holds_alternative<bool>(value)) {
        ss << (std::get<bool>(value) ? "true" : "false");
    } else if (std::holds_alternative<std::string>(value)) {
        ss << "\"" << std::get<std::string>(value) << "\"";
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        ss << "[";
        for (size_t i = 0; i < std::get<std::vector<int>>(value).size(); ++i) {
            if (i > 0) ss << ", ";
            ss << std::get<std::vector<int>>(value)[i];
        }
        ss << "]";
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        ss << "[";
        for (size_t i = 0; i < std::get<std::vector<double>>(value).size(); ++i) {
            if (i > 0) ss << ", ";
            ss << std::get<std::vector<double>>(value)[i];
        }
        ss << "]";
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        ss << "[";
        for (size_t i = 0; i < std::get<std::vector<std::string>>(value).size(); ++i) {
            if (i > 0) ss << ", ";
            ss << "\"" << std::get<std::vector<std::string>>(value)[i] << "\"";
        }
        ss << "]";
    } else {
        ss << "null";
    }
    return ss.str();
}

EnhancedLogger& getEnhancedLogger() {
    static EnhancedLogger globalLogger;
    return globalLogger;
}

// PerformanceTimer implementation
PerformanceTimer::PerformanceTimer(const std::string& operationName, bool log)
    : operationName_(operationName), log_(log), stopped_(false), duration_(0.0) {
    startTime_ = std::chrono::system_clock::now();
}

PerformanceTimer::~PerformanceTimer() {
    if (!stopped_) {
        stop();
    }
}

void PerformanceTimer::stop() {
    if (!stopped_) {
        auto endTime = std::chrono::system_clock::now();
        duration_ = std::chrono::duration<double>(endTime - startTime_).count();
        stopped_ = true;
        
        if (log_) {
            std::cout << "Performance: " << operationName_ << " took " << duration_ * 1000 << " ms" << std::endl;
        }
    }
}

double PerformanceTimer::getDuration() const {
    if (stopped_) {
        return duration_;
    } else {
        auto now = std::chrono::system_clock::now();
        return std::chrono::duration<double>(now - startTime_).count();
    }
}

// MemoryTracker implementation
MemoryTracker::MemoryTracker(const std::string& operationName)
    : operationName_(operationName), stopped_(false), memoryUsage_(0) {}

MemoryTracker::~MemoryTracker() {
    if (!stopped_) {
        stop();
    }
}

void MemoryTracker::stop() {
    if (!stopped_) {
        // Estimate memory usage (platform-specific implementation would be needed)
        memoryUsage_ = 0; // Placeholder
        stopped_ = true;
        
        std::cout << "Memory: " << operationName_ << " used approximately " << (memoryUsage_ / 1024) << " KB" << std::endl;
    }
}

size_t MemoryTracker::getMemoryUsage() const {
    return memoryUsage_;
}

} // namespace nlm
