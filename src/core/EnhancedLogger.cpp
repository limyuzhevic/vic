#include "EnhancedLogger.hpp"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <thread>
#include <filesystem>

namespace nlm {

struct StructuredConsoleLogger::Impl {
    std::shared_ptr<spdlog::logger> spdlogLogger;
    LogLevel level;
    bool colorsEnabled;
    std::string format;
    enum class Preset { Simple, Detailed, Json } preset;
    bool performanceOutput;
    std::unordered_map<std::string, std::string> formatCache;
    std::mutex formatMutex;
};

StructuredConsoleLogger::StructuredConsoleLogger() : pImpl(std::make_unique<Impl>()) {
    pImpl->level = LogLevel::Info;
    pImpl->colorsEnabled = true;
    pImpl->format = "%datetime %level %message";
    pImpl->preset = Preset::Detailed;
    pImpl->performanceOutput = true;
    
    // Create spdlog logger with color output
    auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    consoleSink->set_pattern("%^[%L] %v%$");
    
    pImpl->spdlogLogger = std::make_shared<spdlog::logger>("nlm_console", consoleSink);
    spdlog::register_logger(pImpl->spdlogLogger);
    pImpl->spdlogLogger->set_level(spdlog::level::info);
}

StructuredConsoleLogger::StructuredConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
    pImpl->colorsEnabled = true;
    pImpl->format = "%datetime %level %message";
    pImpl->preset = Preset::Detailed;
    pImpl->performanceOutput = true;
    
    auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    consoleSink->set_pattern("%^[%L] %v%$");
    
    pImpl->spdlogLogger = std::make_shared<spdlog::logger>("nlm_console", consoleSink);
    spdlog::register_logger(pImpl->spdlogLogger);
    
    // Map LogLevel to spdlog level
    spdlog::level::level_enum spdlogLevel;
    switch (level) {
        case LogLevel::Trace: spdlogLevel = spdlog::level::trace; break;
        case LogLevel::Debug: spdlogLevel = spdlog::level::debug; break;
        case LogLevel::Info: spdlogLevel = spdlog::level::info; break;
        case LogLevel::Warning: spdlogLevel = spdlog::level::warn; break;
        case LogLevel::Error: spdlogLevel = spdlog::level::err; break;
        case LogLevel::Critical: spdlogLevel = spdlog::level::critical; break;
        case LogLevel::Off: spdlogLevel = spdlog::level::off; break;
    }
    pImpl->spdlogLogger->set_level(spdlogLevel);
}

StructuredConsoleLogger::~StructuredConsoleLogger() = default;

void StructuredConsoleLogger::logStructured(const StructuredLogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::string formattedMessage = formatStructuredEntry(entry);
    
    // Map LogLevel to spdlog level
    spdlog::level::level_enum spdlogLevel;
    switch (entry.level) {
        case LogLevel::Trace: spdlogLevel = spdlog::level::trace; break;
        case LogLevel::Debug: spdlogLevel = spdlog::level::debug; break;
        case LogLevel::Info: spdlogLevel = spdlog::level::info; break;
        case LogLevel::Warning: spdlogLevel = spdlog::level::warn; break;
        case LogLevel::Error: spdlogLevel = spdlog::level::err; break;
        case LogLevel::Critical: spdlogLevel = spdlog::level::critical; break;
        case LogLevel::Off: spdlogLevel = spdlog::level::off; break;
    }
    
    if (pImpl->performanceOutput && entry.performanceMetric) {
        pImpl->spdlogLogger->log(spdlogLevel, "[PERFORMANCE] " + formattedMessage);
    } else {
        pImpl->spdlogLogger->log(spdlogLevel, formattedMessage);
    }
}

void StructuredConsoleLogger::setLevel(LogLevel level) {
    pImpl->level = level;
    
    // Map LogLevel to spdlog level
    spdlog::level::level_enum spdlogLevel;
    switch (level) {
        case LogLevel::Trace: spdlogLevel = spdlog::level::trace; break;
        case LogLevel::Debug: spdlogLevel = spdlog::level::debug; break;
        case LogLevel::Info: spdlogLevel = spdlog::level::info; break;
        case LogLevel::Warning: spdlogLevel = spdlog::level::warn; break;
        case LogLevel::Error: spdlogLevel = spdlog::level::err; break;
        case LogLevel::Critical: spdlogLevel = spdlog::level::critical; break;
        case LogLevel::Off: spdlogLevel = spdlog::level::off; break;
    }
    pImpl->spdlogLogger->set_level(spdlogLevel);
}

LogLevel StructuredConsoleLogger::getLevel() const {
    return pImpl->level;
}

void StructuredConsoleLogger::setFormat(const std::string& format) {
    pImpl->format = format;
}

std::string StructuredConsoleLogger::getFormat() const {
    return pImpl->format;
}

void StructuredConsoleLogger::setColorEnabled(bool enabled) {
    pImpl->colorsEnabled = enabled;
    auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    if (enabled) {
        consoleSink->set_pattern("%^[%L] %v%$");
    } else {
        consoleSink->set_pattern("[%L] %v");
    }
    pImpl->spdlogLogger->sinks().clear();
    pImpl->spdlogLogger->sinks().push_back(consoleSink);
}

bool StructuredConsoleLogger::isColorEnabled() const {
    return pImpl->colorsEnabled;
}

std::string StructuredConsoleLogger::formatStructuredEntry(const StructuredLogEntry& entry) const {
    std::ostringstream oss;
    
    switch (pImpl->preset) {
        case Preset::Simple:
            oss << "[" << LogLevelHelper::toString(entry.level) << "] " << entry.message;
            break;
            
        case Preset::Detailed:
            oss << "[" << entry.timestamp << "] [" << LogLevelHelper::toString(entry.level) << "] "
                << entry.threadName << "(" << entry.threadId << ") " << entry.message;
            if (!entry.file.empty()) {
                oss << " in " << entry.file << ":" << entry.line;
            }
            if (!entry.function.empty()) {
                oss << " from " << entry.function;
            }
            if (entry.processingTimeMs > 0.0) {
                oss << " [processed in " << entry.processingTimeMs << "ms]";
            }
            break;
            
        case Preset::Json:
            oss << "{\"timestamp\": \"" << getTimestampString(entry.timestamp) << "\", \"level\": \""
                << LogLevelHelper::toString(entry.level) << "\", \"message\": \"" << entry.message
                << "\", \"thread\": \"" << entry.threadName << "\", \"metadata\": ";
            
            oss << serializeMetadata(entry.metadata);
            oss << "}";
            break;
    }
    
    // Add metadata if present
    if (!entry.metadata.empty() && pImpl->preset != Preset::Json) {
        oss << " [";
        bool first = true;
        for (const auto& pair : entry.metadata) {
            if (!first) oss << ", ";
            oss << pair.first << "=" << pair.second;
            first = false;
        }
        oss << "]";
    }
    
    return oss.str();
}

std::string StructuredConsoleLogger::getPresetName() const {
    switch (pImpl->preset) {
        case Preset::Simple: return "Simple";
        case Preset::Detailed: return "Detailed";
        case Preset::Json: return "Json";
        default: return "Detailed";
    }
}

void StructuredConsoleLogger::enablePerformanceOutput(bool enabled) {
    pImpl->performanceOutput = enabled;
}

bool StructuredConsoleLogger::isPerformanceOutputEnabled() const {
    return pImpl->performanceOutput;
}

struct EnhancedLogger::Impl {
    std::shared_ptr<spdlog::logger> spdlogLogger;
    std::vector<std::shared_ptr<IStructuredLogger>> structuredLoggers;
    LogLevel level;
    bool performanceMonitoringEnabled;
    std::string logFile;
    std::mutex logMutex;
    std::unordered_map<std::string, std::chrono::high_resolution_clock::time_point> activeTimers;
    std::unordered_map<std::string, double> lastLogTimes;
    std::chrono::system_clock::time_point lastLogTime;
    std::string format;
    bool initialized;
};

EnhancedLogger::EnhancedLogger() : pImpl(std::make_unique<Impl>()) {
    pImpl->level = LogLevel::Info;
    pImpl->performanceMonitoringEnabled = true;
    pImpl->initialized = false;
    pImpl->format = "%datetime %level %message";
    pImpl->lastLogTime = std::chrono::system_clock::now();
    
    // Initialize spdlog logger
    try {
        auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        consoleSink->set_pattern("%^[%L] %v%$");
        
        pImpl->spdlogLogger = std::make_shared<spdlog::logger>("nlm_enhanced", consoleSink);
        spdlog::register_logger(pImpl->spdlogLogger);
        pImpl->spdlogLogger->set_level(spdlog::level::info);
        pImpl->spdlogLogger->flush_on(spdlog::level::info);
        
        pImpl->initialized = true;
        
        NLM_LOG_INFO("Enhanced logging system initialized");
    } catch (const spdlog::spdlog_ex& ex) {
        std::cerr << "Failed to initialize enhanced logging system: " << ex.what() << std::endl;
        pImpl->initialized = false;
    }
}

EnhancedLogger::~EnhancedLogger() {
    flush();
}

void EnhancedLogger::addStructuredLogger(std::shared_ptr<IStructuredLogger> logger) {
    if (logger) {
        pImpl->structuredLoggers.push_back(logger);
    }
}

void EnhancedLogger::clearLoggers() {
    std::lock_guard<std::mutex> lock(pImpl->logMutex);
    pImpl->structuredLoggers.clear();
}

void EnhancedLogger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->logMutex);
    pImpl->level = level;
    
    if (pImpl->spdlogLogger) {
        spdlog::level::level_enum spdlogLevel;
        switch (level) {
            case LogLevel::Trace: spdlogLevel = spdlog::level::trace; break;
            case LogLevel::Debug: spdlogLevel = spdlog::level::debug; break;
            case LogLevel::Info: spdlogLevel = spdlog::level::info; break;
            case LogLevel::Warning: spdlogLevel = spdlog::level::warn; break;
            case LogLevel::Error: spdlogLevel = spdlog::level::err; break;
            case LogLevel::Critical: spdlogLevel = spdlog::level::critical; break;
            case LogLevel::Off: spdlogLevel = spdlog::level::off; break;
        }
        pImpl->spdlogLogger->set_level(spdlogLevel);
    }
    
    for (auto& structuredLogger : pImpl->structuredLoggers) {
        structuredLogger->setLevel(level);
    }
}

void EnhancedLogger::logStructured(LogLevel level, const std::string& message,
                                   const std::string& file, int line,
                                   const std::string& function) {
    if (!isEnabled(level)) {
        return;
    }
    
    StructuredLogEntry entry;
    entry.timestamp = std::chrono::system_clock::now();
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.threadName = std::this_thread::get_id();
    entry.threadId = std::hash<std::thread::id>{}(entry.threadName);
    entry.processingTimeMs = 0.0;
    entry.performanceMetric = false;
    
    // Update statistics
    LogStatistics::recordLog(entry);
    
    // Log to structured loggers
    for (auto& structuredLogger : pImpl->structuredLoggers) {
        structuredLogger->logStructured(entry);
    }
    
    // Log to spdlog
    if (pImpl->spdlogLogger) {
        std::string spdlogMessage = formatStructuredMessage(entry);
        spdlog::level::level_enum spdlogLevel;
        switch (level) {
            case LogLevel::Trace: spdlogLevel = spdlog::level::trace; break;
            case LogLevel::Debug: spdlogLevel = spdlog::level::debug; break;
            case LogLevel::Info: spdlogLevel = spdlog::level::info; break;
            case LogLevel::Warning: spdlogLevel = spdlog::level::warn; break;
            case LogLevel::Error: spdlogLevel = spdlog::level::err; break;
            case LogLevel::Critical: spdlogLevel = spdlog::level::critical; break;
            case LogLevel::Off: spdlogLevel = spdlog::level::off; break;
        }
        pImpl->spdlogLogger->log(spdlogLevel, spdlogMessage);
    }
}

void EnhancedLogger::logWithTiming(LogLevel level, const std::string& operation,
                                   const std::function<void()>& operationFunc,
                                   const std::string& file, int line,
                                   const std::string& function) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    operationFunc();
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    double durationMs = static_cast<double>(duration.count()) / 1000.0;
    
    std::ostringstream oss;
    oss << operation << " completed in " << durationMs << "ms";
    
    StructuredLogEntry entry;
    entry.timestamp = std::chrono::system_clock::now();
    entry.level = level;
    entry.message = oss.str();
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.threadName = std::this_thread::get_id();
    entry.threadId = std::hash<std::thread::id>{}(entry.threadName);
    entry.processingTimeMs = durationMs;
    entry.performanceMetric = true;
    
    // Log the timing result
    logStructured(level, oss.str(), file, line, function);
}

template<typename... Args>
void EnhancedLogger::logWithMetadata(LogLevel level, const std::string& message,
                                     const std::string& file, int line,
                                     const std::string& function,
                                     const std::unordered_map<std::string, std::string>& metadata) {
    if (!isEnabled(level)) {
        return;
    }
    
    StructuredLogEntry entry;
    entry.timestamp = std::chrono::system_clock::now();
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.threadName = std::this_thread::get_id();
    entry.threadId = std::hash<std::thread::id>{}(entry.threadName);
    entry.metadata = metadata;
    entry.processingTimeMs = 0.0;
    entry.performanceMetric = false;
    
    // Log to structured loggers
    for (auto& structuredLogger : pImpl->structuredLoggers) {
        structuredLogger->logStructured(entry);
    }
    
    // Log to spdlog
    if (pImpl->spdlogLogger) {
        std::string spdlogMessage = formatStructuredMessage(entry);
        spdlog::level::level_enum spdlogLevel;
        switch (level) {
            case LogLevel::Trace: spdlogLevel = spdlog::level::trace; break;
            case LogLevel::Debug: spdlogLevel = spdlog::level::debug; break;
            case LogLevel::Info: spdlogLevel = spdlog::level::info; break;
            case LogLevel::Warning: spdlogLevel = spdlog::level::warn; break;
            case LogLevel::Error: spdlogLevel = spdlog::level::err; break;
            case LogLevel::Critical: spdlogLevel = spdlog::level::critical; break;
            case LogLevel::Off: spdlogLevel = spdlog::level::off; break;
        }
        pImpl->spdlogLogger->log(spdlogLevel, spdlogMessage);
    }
}

void EnhancedLogger::debugWithTiming(const std::string& operation,
                                     const std::function<void()>& operationFunc,
                                     const std::string& file, int line,
                                     const std::string& function) {
    logWithTiming(LogLevel::Debug, operation, operationFunc, file, line, function);
}

void EnhancedLogger::infoWithTiming(const std::string& operation,
                                    const std::function<void()>& operationFunc,
                                    const std::string& file, int line,
                                    const std::string& function) {
    logWithTiming(LogLevel::Info, operation, operationFunc, file, line, function);
}

void EnhancedLogger::log(LogLevel level, const std::string& message,
                         const std::string& file, int line,
                         const std::string& function) {
    logStructured(level, message, file, line, function);
}

void EnhancedLogger::trace(const std::string& message, const std::string& file,
                           int line, const std::string& function) {
    log(LogLevel::Trace, message, file, line, function);
}

void EnhancedLogger::debug(const std::string& message, const std::string& file,
                           int line, const std::string& function) {
    log(LogLevel::Debug, message, file, line, function);
}

void EnhancedLogger::info(const std::string& message, const std::string& file,
                          int line, const std::string& function) {
    log(LogLevel::Info, message, file, line, function);
}

void EnhancedLogger::warning(const std::string& message, const std::string& file,
                             int line, const std::string& function) {
    log(LogLevel::Warning, message, file, line, function);
}

void EnhancedLogger::error(const std::string& message, const std::string& file,
                           int line, const std::string& function) {
    log(LogLevel::Error, message, file, line, function);
}

void EnhancedLogger::critical(const std::string& message, const std::string& file,
                              int line, const std::string& function) {
    log(LogLevel::Critical, message, file, line, function);
}

bool EnhancedLogger::isEnabled(LogLevel level) const {
    return level >= pImpl->level;
}

EnhancedLogger& EnhancedLogger::getGlobal() {
    static EnhancedLogger globalLogger;
    return globalLogger;
}

void EnhancedLogger::setGlobal(std::shared_ptr<EnhancedLogger> logger) {
    // Implementation would handle setting the global logger
}

void EnhancedLogger::enablePerformanceMonitoring(bool enabled) {
    pImpl->performanceMonitoringEnabled = enabled;
}

bool EnhancedLogger::isPerformanceMonitoringEnabled() const {
    return pImpl->performanceMonitoringEnabled;
}

std::string EnhancedLogger::getStatistics() const {
    return LogStatistics::generateStatisticsReport();
}

void EnhancedLogger::flush() {
    if (pImpl->spdlogLogger) {
        pImpl->spdlogLogger->flush();
    }
}

void EnhancedLogger::setLogFile(const std::string& filepath, bool append) {
    std::lock_guard<std::mutex> lock(pImpl->logMutex);
    
    pImpl->logFile = filepath;
    
    try {
        auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(filepath, append);
        fileSink->set_pattern("%^[%L] %v%$");
        
        if (pImpl->spdlogLogger) {
            pImpl->spdlogLogger->sinks().clear();
            pImpl->spdlogLogger->sinks().push_back(fileSink);
            pImpl->spdlogLogger->flush_on(spdlog::level::info);
        }
    } catch (const spdlog::spdlog_ex& ex) {
        NLM_LOG_ERROR(std::string("Failed to set log file: ") + ex.what());
    }
}

std::string EnhancedLogger::getLogFile() const {
    return pImpl->logFile;
}

std::string EnhancedLogger::formatStructuredMessage(const StructuredLogEntry& entry) const {
    std::ostringstream oss;
    
    switch (pImpl->format.empty() ? LogLevel::Info : pImpl->level) {
        case LogLevel::Trace:
            oss << "[" << entry.timestamp << "] [TRACE] " << entry.message;
            break;
        case LogLevel::Debug:
            oss << "[" << entry.timestamp << "] [DEBUG] " << entry.message;
            break;
        case LogLevel::Info:
            oss << "[" << entry.timestamp << "] [INFO] " << entry.message;
            break;
        case LogLevel::Warning:
            oss << "[" << entry.timestamp << "] [WARN] " << entry.message;
            break;
        case LogLevel::Error:
            oss << "[" << entry.timestamp << "] [ERROR] " << entry.message;
            break;
        case LogLevel::Critical:
            oss << "[" << entry.timestamp << "] [CRIT] " << entry.message;
            break;
        default:
            oss << "[" << entry.timestamp << "] [INFO] " << entry.message;
    }
    
    // Add performance metrics if applicable
    if (entry.performanceMetric && entry.processingTimeMs > 0.0) {
        oss << " [PERF: " << entry.processingTimeMs << "ms]";
    }
    
    return oss.str();
}

std::string EnhancedLogger::getTimestampString(std::chrono::system_clock::time_point timestamp) const {
    auto time = std::chrono::system_clock::to_time_t(timestamp);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string EnhancedLogger::serializeMetadata(const std::unordered_map<std::string, std::string>& metadata) const {
    std::ostringstream oss;
    oss << "{";
    bool first = true;
    for (const auto& pair : metadata) {
        if (!first) oss << ", ";
        oss << "\"" << pair.first << "\": \"" << pair.second << "\"";
        first = false;
    }
    oss << "}";
    return oss.str();
}

std::string LogLevelHelper::toString(LogLevel level) {
    switch (level) {
        case LogLevel::Trace: return "TRACE";
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARNING";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Critical: return "CRITICAL";
        case LogLevel::Off: return "OFF";
        default: return "UNKNOWN";
    }
}

LogLevel LogLevelHelper::fromString(const std::string& levelStr) {
    std::string upper = levelStr;
    std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
    
    if (upper == "TRACE") return LogLevel::Trace;
    if (upper == "DEBUG") return LogLevel::Debug;
    if (upper == "INFO") return LogLevel::Info;
    if (upper == "WARNING") return LogLevel::Warning;
    if (upper == "WARN") return LogLevel::Warning;
    if (upper == "ERROR") return LogLevel::Error;
    if (upper == "CRITICAL") return LogLevel::Critical;
    if (upper == "CRIT") return LogLevel::Critical;
    if (upper == "OFF") return LogLevel::Off;
    
    return LogLevel::Info; // Default
}

std::string LogLevelHelper::levelColorCode(LogLevel level) {
    switch (level) {
        case LogLevel::Trace: return "\033[37m"; // White
        case LogLevel::Debug: return "\033[36m"; // Cyan
        case LogLevel::Info: return "\033[32m"; // Green
        case LogLevel::Warning: return "\033[33m"; // Yellow
        case LogLevel::Error: return "\033[31m"; // Red
        case LogLevel::Critical: return "\033[35m"; // Magenta
        default: return "\033[0m";
    }
}

std::vector<std::string> LogLevelHelper::getAllLevelNames() {
    return {"TRACE", "DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL", "OFF"};
}

bool LogLevelHelper::isLevelEnabled(LogLevel level, LogLevel threshold) {
    return static_cast<int>(level) >= static_cast<int>(threshold);
}

void setLoggingConfig(const LoggingConfig& config) {
    EnhancedLogger::getGlobal().setLevel(config.level);
    EnhancedLogger::getGlobal().setLogFile(config.logFile, config.appendToFile);
    
    // Additional configuration would be applied here
}

LoggingConfig getLoggingConfig() {
    LoggingConfig config;
    // Configuration would be read from global logger
    return config;
}

void resetLoggingConfig() {
    LoggingConfig defaultConfig;
    setLoggingConfig(defaultConfig);
}

bool LogFileRotator::rotateIfNeeded(const std::string& filepath, size_t maxFileSize, size_t maxFiles) {
    std::filesystem::path path(filepath);
    
    if (!std::filesystem::exists(path)) {
        return false;
    }
    
    auto fileSize = std::filesystem::file_size(path);
    
    if (fileSize < maxFileSize) {
        return false;
    }
    
    // Rotate existing logs
    for (size_t i = maxFiles; i > 0; --i) {
        std::filesystem::path source = filepath + "." + std::to_string(i - 1);
        std::filesystem::path destination = filepath + "." + std::to_string(i);
        
        if (std::filesystem::exists(source)) {
            std::filesystem::rename(source, destination);
        }
    }
    
    // Move current log to backup
    std::filesystem::rename(filepath, filepath + ".0");
    
    return true;
}

std::vector<std::string> LogFileRotator::getLogFiles(const std::string& directory) {
    std::vector<std::string> logFiles;
    
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (entry.is_regular_file() && entry.path().extension() == ".log") {
            logFiles.push_back(entry.path().string());
        }
    }
    
    return logFiles;
}

bool LogFileRotator::cleanupOldLogs(const std::string& directory, size_t maxFiles) {
    auto logFiles = getLogFiles(directory);
    
    if (logFiles.size() <= maxFiles) {
        return true;
    }
    
    // Sort by modification time (oldest first)
    std::sort(logFiles.begin(), logFiles.end(),
              [](const std::string& a, const std::string& b) {
                  return std::filesystem::last_write_time(a) < std::filesystem::last_write_time(b);
              });
    
    // Delete excess files (keep newest maxFiles)
    size_t filesToDelete = logFiles.size() - maxFiles;
    for (size_t i = 0; i < filesToDelete; ++i) {
        try {
            std::filesystem::remove(logFiles[i]);
        } catch (const std::filesystem::filesystem_error& e) {
            return false;
        }
    }
    
    return true;
}

void LogStatistics::recordLog(const StructuredLogEntry& entry) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    stats_.totalLogs++;
    stats_.logsByLevel[static_cast<int>(entry.level)]++;
    
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastLogTime_);
    lastLogTime_ = now;
    
    if (duration.count() > 0) {
        stats_.averageLogIntervalMs = static_cast<double>(duration.count());
    }
    
    if (entry.processingTimeMs > stats_.peakProcessingTimeMs) {
        stats_.peakProcessingTimeMs = entry.processingTimeMs;
    }
    
    stats_.averageProcessingTimeMs = 
        (stats_.averageProcessingTimeMs * (stats_.totalLogs - 1) + entry.processingTimeMs) / stats_.totalLogs;
}

LogStatistics::Statistics LogStatistics::getStatistics() {
    std::lock_guard<std::mutex> lock(mutex_);
    return stats_;
}

void LogStatistics::resetStatistics() {
    std::lock_guard<std::mutex> lock(mutex_);
    stats_ = Statistics();
    lastLogTime_ = std::chrono::system_clock::now();
}

std::string LogStatistics::generateStatisticsReport() {
    std::ostringstream oss;
    oss << "=== Log Statistics ===" << std::endl;
    oss << "Total logs: " << stats_.totalLogs << std::endl;
    oss << "Average log interval: " << stats_.averageLogIntervalMs << "ms" << std::endl;
    oss << "Average processing time: " << stats_.averageProcessingTimeMs << "ms" << std::endl;
    oss << "Peak processing time: " << stats_.peakProcessingTimeMs << "ms" << std::endl;
    
    oss << std::endl << "Logs by level:" << std::endl;
    oss << "  TRACE: " << stats_.logsByLevel[0] << std::endl;
    oss << "  DEBUG: " << stats_.logsByLevel[1] << std::endl;
    oss << "  INFO:  " << stats_.logsByLevel[2] << std::endl;
    oss << "  WARN:  " << stats_.logsByLevel[3] << std::endl;
    oss << "  ERROR: " << stats_.logsByLevel[4] << std::endl;
    oss << "  CRIT:  " << stats_.logsByLevel[5] << std::endl;
    
    return oss.str();
}

void logSystemInfo() {
    NLM_LOG_INFO("System: NLM - Neural Learning Machine");
    NLM_LOG_INFO("Version: 0.1.0");
    NLM_LOG_INFO("Architecture: Experimental Artificial Developmental Brain");
    NLM_LOG_INFO("Phase: 6 - Final Integration");
    NLM_LOG_INFO("Components: Memory Systems, Neuromodulation, Prediction, Cognition, Development");
}

void logPerformanceMetrics() {
    NLM_LOG_INFO("Performance monitoring enabled");
    NLM_LOG_INFO("Timing enabled for neural network operations");
    NLM_LOG_INFO("Structured logging enabled");
}

void logConfiguration() {
    NLM_LOG_INFO("Logging configuration:");
    NLM_LOG_INFO("  Level: INFO");
    NLM_LOG_INFO("  Colors: Enabled");
    NLM_LOG_INFO("  Performance metrics: Enabled");
    NLM_LOG_INFO("  Structured logging: Enabled");
}

void logStartupInfo(const std::string& version, const std::string& buildInfo) {
    NLM_LOG_INFO("NLM Startup:");
    NLM_LOG_INFO(std::string("  Version: ") + version);
    NLM_LOG_INFO(std::string("  Build: ") + buildInfo);
    NLM_LOG_INFO("  Logging: Enhanced with performance monitoring");
    NLM_LOG_INFO("  Ready for neural computation");
}

} // namespace nlm