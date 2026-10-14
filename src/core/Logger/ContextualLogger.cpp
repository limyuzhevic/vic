#include "ContextualLogger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <ctime>
#include <algorithm>
#include <shared_mutex>
#include <iomanip>

namespace nlm {

struct ContextualLogger::Impl {
    LoggerConfiguration config;
    std::unordered_map<std::string, std::string> globalContext;
    thread_local static std::unordered_map<std::string, std::string> threadContext;
    std::unordered_map<std::string, std::chrono::system_clock::time_point> performanceTimers;
    
    std::shared_mutex contextMutex;
    std::shared_mutex configMutex;
    
    std::ofstream logFile;
    std::mutex fileMutex;
    std::chrono::system_clock::time_point lastRotation;
    
    std::shared_ptr<Logger> existingLogger;
};

ContextualLogger::ContextualLogger() : pImpl(std::make_unique<Impl>()) {
    // Set default configuration
    pImpl->config.globalLevel = LogLevel::Info;
    pImpl->config.enableStructuredLogging = false;
    pImpl->config.enablePerformanceMonitoring = true;
    pImpl->config.enableContextualLogging = true;
    pImpl->config.enableThreadLocalContext = true;
    pImpl->config.trackExecutionTime = true;
    pImpl->config.enableErrorReporting = true;
    pImpl->config.enableSanitization = true;
    pImpl->config.enableColors = true;
    pImpl->config.includeLocation = true;
    pImpl->config.includeFunction = true;
    pImpl->config.includeThreadId = false;
    
    // Default component levels
    pImpl->config.componentLevels["brain"] = LogLevel::Warning;
    pImpl->config.componentLevels["plasticity"] = LogLevel::Info;
    pImpl->config.componentLevels["memory"] = LogLevel::Info;
    pImpl->config.componentLevels["sensory"] = LogLevel::Info;
    pImpl->config.componentLevels["motor"] = LogLevel::Info;
    pImpl->config.componentLevels["environment"] = LogLevel::Info;
    pImpl->config.componentLevels["neuromodulation"] = LogLevel::Info;
    pImpl->config.componentLevels["development"] = LogLevel::Info;
    pImpl->config.componentLevels["cognition"] = LogLevel::Info;
    pImpl->config.componentLevels["performance"] = LogLevel::Performance;
    pImpl->config.componentLevels["system"] = LogLevel::System;
    pImpl->config.componentLevels["experiment"] = LogLevel::Experiment;
    
    // Default error categories
    pImpl->config.errorCategories["neural"] = "Neural Processing Error";
    pImpl->config.errorCategories["plastic"] = "Plasticity Error";
    pImpl->config.errorCategories["memory"] = "Memory System Error";
    pImpl->config.errorCategories["performance"] = "Performance Error";
    pImpl->config.errorCategories["system"] = "System Error";
    pImpl->config.errorCategories["config"] = "Configuration Error";
}

ContextualLogger::~ContextualLogger() = default;

void ContextualLogger::configure(const LoggerConfiguration& config) {
    std::unique_lock<std::shared_mutex> lock(pImpl->configMutex);
    pImpl->config = config;
}

LoggerConfiguration ContextualLogger::getConfiguration() const {
    std::shared_lock<std::shared_mutex> lock(pImpl->configMutex);
    return pImpl->config;
}

void ContextualLogger::setGlobalContext(const std::string& key, const std::string& value) {
    std::unique_lock<std::shared_mutex> lock(pImpl->contextMutex);
    pImpl->globalContext[key] = value;
}

std::string ContextualLogger::getGlobalContext(const std::string& key) const {
    std::shared_lock<std::shared_mutex> lock(pImpl->contextMutex);
    auto it = pImpl->globalContext.find(key);
    if (it != pImpl->globalContext.end()) {
        return it->second;
    }
    return "";
}

void ContextualLogger::clearGlobalContext() {
    std::unique_lock<std::shared_mutex> lock(pImpl->contextMutex);
    pImpl->globalContext.clear();
}

void ContextualLogger::clearGlobalContext(const std::string& key) {
    std::unique_lock<std::shared_mutex> lock(pImpl->contextMutex);
    pImpl->globalContext.erase(key);
}

void ContextualLogger::setThreadLocalContext(const std::string& key, const std::string& value) {
    pImpl->threadContext[key] = value;
}

std::string ContextualLogger::getThreadLocalContext(const std::string& key) const {
    auto it = pImpl->threadContext.find(key);
    if (it != pImpl->threadContext.end()) {
        return it->second;
    }
    return "";
}

void ContextualLogger::clearThreadLocalContext() {
    pImpl->threadContext.clear();
}

void ContextualLogger::clearThreadLocalContext(const std::string& key) {
    pImpl->threadContext.erase(key);
}

void ContextualLogger::setComponentLevel(const std::string& component, LogLevel level) {
    std::unique_lock<std::shared_mutex> lock(pImpl->configMutex);
    pImpl->config.componentLevels[component] = level;
}

LogLevel ContextualLogger::getComponentLevel(const std::string& component) const {
    std::shared_lock<std::shared_mutex> lock(pImpl->configMutex);
    auto it = pImpl->config.componentLevels.find(component);
    if (it != pImpl->config.componentLevels.end()) {
        return it->second;
    }
    return LogLevel::Info;
}

void ContextualLogger::log(LogLevel level, const std::string& message,
                            const std::string& file, int line,
                            const std::string& function,
                            const std::string& component) {
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.component = component;
    entry.timestamp = std::chrono::system_clock::now();
    
    // Add context
    if (pImpl->config.enableContextualLogging) {
        // Add global context
        for (const auto& pair : pImpl->globalContext) {
            entry.context[pair.first] = pair.second;
        }
        
        // Add thread-local context
        if (pImpl->config.enableThreadLocalContext) {
            for (const auto& pair : pImpl->threadContext) {
                entry.context[pair.first] = pair.second;
            }
        }
        
        // Add component-specific context
        if (!component.empty()) {
            entry.context["component"] = component;
        }
    }
    
    // Apply sanitization
    if (pImpl->config.enableSanitization) {
        entry.message = applySanitization(entry.message, pImpl->config);
        for (auto& pair : entry.context) {
            pair.second = applySanitization(pair.second, pImpl->config);
        }
    }
    
    // Check if we should log this entry
    if (!shouldLog(entry, pImpl->config)) {
        return;
    }
    
    // Format the message
    std::string formatted = formatMessage(entry, pImpl->config);
    
    // Output to console
    std::cout << formatted;
    
    // Output to file if configured
    if (pImpl->config.enableFileOutput && !pImpl->config.logFile.empty()) {
        std::lock_guard<std::mutex> fileLock(pImpl->fileMutex);
        if (!pImpl->logFile.is_open()) {
            pImpl->logFile.open(pImpl->config.logFile, std::ios::app);
            pImpl->lastRotation = std::chrono::system_clock::now();
        }
        
        pImpl->logFile << formatted;
        pImpl->logFile.flush();
        
        // Check if rotation is needed
        if (pImpl->config.maxFileSize > 0 && 
            pImpl->logFile.tellp() > static_cast<std::streamsize>(pImpl->config.maxFileSize)) {
            // Simple rotation: add timestamp to filename
            auto now = std::chrono::system_clock::now();
            auto time = std::chrono::system_clock::to_time_t(now);
            std::stringstream ss;
            ss << pImpl->config.logFile << "." 
               << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S");
            
            pImpl->logFile.close();
            std::rename(pImpl->config.logFile.c_str(), ss.str().c_str());
            pImpl->logFile.open(pImpl->config.logFile, std::ios::app);
            pImpl->lastRotation = now;
        }
    }
    
    // Log to existing logger if integrated
    if (pImpl->existingLogger) {
        pImpl->existingLogger->log(entry.level, formatted, entry.file, entry.line, entry.function);
    }
}

void ContextualLogger::logWithDuration(LogLevel level, const std::string& message,
                                      double durationMs,
                                      const std::string& file, int line,
                                      const std::string& function,
                                      const std::string& component) {
    LogEntry entry;
    entry.level = level;
    entry.message = message + " [" + std::to_string(durationMs) + "ms]";
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.component = component;
    entry.timestamp = std::chrono::system_clock::now();
    
    if (pImpl->config.trackExecutionTime && durationMs > pImpl->config.performanceThresholdMs) {
        // Log performance warning
        entry.context["performance_warning"] = "true";
        entry.context["duration_ms"] = std::to_string(durationMs);
    }
    
    log(entry.level, entry.message, entry.file, entry.line, entry.function, entry.component);
}

void ContextualLogger::logError(const std::string& errorCode, const std::string& errorMessage,
                                const std::string& errorCategory,
                                const std::string& stackTrace,
                                const std::string& component) {
    if (!pImpl->config.enableErrorReporting) {
        return;
    }
    
    LogEntry entry;
    entry.level = LogLevel::Error;
    entry.message = errorMessage;
    entry.errorCode = errorCode;
    entry.errorCategory = errorCategory.empty() ? 
                         (pImpl->config.errorCategories.find(errorCode) != pImpl->config.errorCategories.end() ? 
                          pImpl->config.errorCategories[errorCode] : "Unknown") : errorCategory;
    entry.stackTrace = stackTrace;
    entry.component = component;
    entry.timestamp = std::chrono::system_clock::now();
    
    entry.context["error_code"] = errorCode;
    entry.context["error_category"] = entry.errorCategory;
    
    log(entry.level, entry.message, entry.file, entry.line, entry.function, entry.component);
}

void ContextualLogger::startPerformanceTimer(const std::string& operationId, const std::string& category) {
    if (!pImpl->config.trackExecutionTime) {
        return;
    }
    
    std::unique_lock<std::shared_mutex> lock(pImpl->contextMutex);
    pImpl->performanceTimers[operationId] = std::chrono::system_clock::now();
}

void ContextualLogger::endPerformanceTimer(const std::string& operationId) {
    auto it = pImpl->performanceTimers.find(operationId);
    if (it != pImpl->performanceTimers.end()) {
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now() - it->second).count();
        
        std::string category = "general";
        auto categoryIt = pImpl->performanceTimers.find(category);
        if (categoryIt != pImpl->performanceTimers.end()) {
            category = categoryIt->first;
        }
        
        logPerformance(operationId, static_cast<double>(duration), category);
        
        pImpl->performanceTimers.erase(it);
    }
}

void ContextualLogger::logPerformance(const std::string& operationId, double durationMs, const std::string& category) {
    if (!pImpl->config.trackExecutionTime) {
        return;
    }
    
    std::string message = "Performance: " + operationId + " took " + std::to_string(durationMs) + "ms";
    
    if (durationMs > pImpl->config.performanceThresholdMs) {
        performance(message + " [SLOW]", durationMs, "performance");
    } else {
        performance(message, durationMs, "performance");
    }
}

void ContextualLogger::neuralDebug(const std::string& message, const std::string& component) {
    log(LogLevel::NeuralDebug, message, "", 0, "", component);
}

void ContextualLogger::plasticityDebug(const std::string& message, const std::string& component) {
    log(LogLevel::PlasticityDebug, message, "", 0, "", component);
}

void ContextualLogger::memoryDebug(const std::string& message, const std::string& component) {
    log(LogLevel::MemoryDebug, message, "", 0, "", component);
}

void ContextualLogger::performance(const std::string& message, double durationMs, const std::string& component) {
    logWithDuration(LogLevel::Performance, message, durationMs, "", 0, "", component);
}

void ContextualLogger::system(const std::string& message, const std::string& component) {
    log(LogLevel::System, message, "", 0, "", component);
}

void ContextualLogger::userAction(const std::string& message, const std::string& userId, const std::string& component) {
    LogEntry entry;
    entry.level = LogLevel::UserAction;
    entry.message = message;
    entry.component = component;
    entry.timestamp = std::chrono::system_clock::now();
    
    if (!userId.empty()) {
        entry.context["user_id"] = userId;
    }
    
    log(entry.level, entry.message, entry.file, entry.line, entry.function, entry.component);
}

void ContextualLogger::validation(const std::string& message, const std::string& component) {
    log(LogLevel::Validation, message, "", 0, "", component);
}

void ContextualLogger::security(const std::string& message, const std::string& component) {
    log(LogLevel::Security, message, "", 0, "", component);
}

void ContextualLogger::config(const std::string& message, const std::string& component) {
    log(LogLevel::Config, message, "", 0, "", component);
}

void ContextualLogger::experiment(const std::string& message, const std::string& experimentId, const std::string& component) {
    LogEntry entry;
    entry.level = LogLevel::Experiment;
    entry.message = message;
    entry.component = component;
    entry.timestamp = std::chrono::system_clock::now();
    
    if (!experimentId.empty()) {
        entry.context["experiment_id"] = experimentId;
    }
    
    log(entry.level, entry.message, entry.file, entry.line, entry.function, entry.component);
}

void ContextualLogger::debug(const std::string& message, const std::string& file, int line, const std::string& function, const std::string& component) {
    log(LogLevel::Debug, message, file, line, function, component);
}

void ContextualLogger::info(const std::string& message, const std::string& file, int line, const std::string& function, const std::string& component) {
    log(LogLevel::Info, message, file, line, function, component);
}

void ContextualLogger::warning(const std::string& message, const std::string& file, int line, const std::string& function, const std::string& component) {
    log(LogLevel::Warning, message, file, line, function, component);
}

void ContextualLogger::error(const std::string& message, const std::string& file, int line, const std::string& function, const std::string& component) {
    log(LogLevel::Error, message, file, line, function, component);
}

void ContextualLogger::critical(const std::string& message, const std::string& file, int line, const std::string& function, const std::string& component) {
    log(LogLevel::Critical, message, file, line, function, component);
}

ContextualLogger& ContextualLogger::getGlobal() {
    static ContextualLogger globalLogger;
    return globalLogger;
}

void ContextualLogger::setGlobal(std::shared_ptr<ContextualLogger> logger) {
    // This is a placeholder for advanced global logger management
    // In practice, we just use getGlobal() which is already static
}

void ContextualLogger::integrateWithExistingLogger(std::shared_ptr<Logger> existingLogger) {
    std::unique_lock<std::shared_mutex> lock(pImpl->configMutex);
    pImpl->existingLogger = existingLogger;
}

std::string ContextualLogger::formatMessage(const LogEntry& entry, const LoggerConfiguration& config) const {
    std::ostringstream oss;
    
    // Timestamp
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    oss << "[" << std::put_time(std::localtime(&time), config.timestampFormat.c_str()) << "]";
    
    // Level
    oss << "[" << levelToString(entry.level) << "]";
    
    // Component
    if (!entry.component.empty()) {
        oss << "[" << entry.component << "]";
    }
    
    // Thread ID
    if (config.includeThreadId) {
        oss << "[thread:" << std::this_thread::get_id() << "]";
    }
    
    // Message
    oss << " " << entry.message;
    
    // Location
    if (config.includeLocation && !entry.file.empty()) {
        oss << " (" << entry.file << ":" << entry.line << ")";
    }
    
    // Function
    if (config.includeFunction && !entry.function.empty()) {
        oss << " in " << entry.function;
    }
    
    // Context
    if (!entry.context.empty()) {
        oss << " {";
        bool first = true;
        for (const auto& pair : entry.context) {
            if (!first) oss << ", ";
            oss << pair.first << "=" << pair.second;
            first = false;
        }
        oss << "}";
    }
    
    // Duration
    if (entry.durationMs > 0) {
        oss << " [" << entry.durationMs << "ms]";
    }
    
    oss << "\n";
    return oss.str();
}

std::string ContextualLogger::applySanitization(const std::string& message, const LoggerConfiguration& config) const {
    std::string result = message;
    
    // Apply sensitive patterns
    for (const auto& pattern : config.sensitivePatterns) {
        try {
            std::regex regex(pattern);
            result = std::regex_replace(result, regex, "***");
        } catch (const std::regex_error&) {
            // Skip invalid patterns
        }
    }
    
    // Check for redact fields in context (not applicable to message itself, but included for completeness)
    
    return result;
}

bool ContextualLogger::shouldLog(const LogEntry& entry, const LoggerConfiguration& config) const {
    // Check global level
    if (entry.level < config.globalLevel) {
        return false;
    }
    
    // Check component level
    LogLevel componentLevel = LogLevel::Info;
    auto it = config.componentLevels.find(entry.component);
    if (it != config.componentLevels.end()) {
        componentLevel = it->second;
    }
    
    if (entry.level < componentLevel) {
        return false;
    }
    
    // Check performance warnings
    if (entry.context.find("performance_warning") != entry.context.end()) {
        return true;
    }
    
    return true;
}

std::string ContextualLogger::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Critical: return "CRIT";
        case LogLevel::NeuralDebug: return "N-DBG";
        case LogLevel::PlasticityDebug: return "P-DBG";
        case LogLevel::MemoryDebug: return "M-DBG";
        case LogLevel::Performance: return "PERF";
        case LogLevel::System: return "SYS";
        case LogLevel::UserAction: return "USER";
        case LogLevel::Validation: return "VAL";
        case LogLevel::Security: return "SEC";
        case LogLevel::Network: return "NET";
        case LogLevel::Config: return "CONF";
        case LogLevel::Experiment: return "EXP";
        default: return "UNKNOWN";
    }
}

// Configure global logger function
void configureGlobalLogger(const LoggerConfiguration& config) {
    ContextualLogger::getGlobal().configure(config);
}

// PerformanceTimer implementation
struct PerformanceTimer::Impl {
    std::string operationId;
    std::string category;
    std::chrono::system_clock::time_point startTime;
    bool stopped;
    ContextualLogger* logger;
};

PerformanceTimer::PerformanceTimer(const std::string& operationId, const std::string& category, ContextualLogger* logger)
    : pImpl(std::make_unique<Impl>()) {
    pImpl->operationId = operationId;
    pImpl->category = category;
    pImpl->startTime = std::chrono::system_clock::now();
    pImpl->stopped = false;
    pImpl->logger = logger;
    
    if (pImpl->logger) {
        pImpl->logger->startPerformanceTimer(operationId, category);
    }
}

PerformanceTimer::~PerformanceTimer() {
    stop();
}

void PerformanceTimer::stop() {
    if (!pImpl->stopped) {
        pImpl->stopped = true;
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now() - pImpl->startTime).count();
        
        if (pImpl->logger) {
            pImpl->logger->endPerformanceTimer(pImpl->operationId);
        }
    }
}

double PerformanceTimer::getDuration() const {
    if (pImpl->stopped) {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now() - pImpl->startTime).count();
    }
    return 0.0;
}

std::string PerformanceTimer::getOperationId() const {
    return pImpl->operationId;
}

// Initialize thread-local static member
thread_local std::unordered_map<std::string, std::string> ContextualLogger::Impl::threadContext;

} // namespace nlm
