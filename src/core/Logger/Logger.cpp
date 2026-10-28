#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <chrono>
#include <ctime>
#include <algorithm>
#include <filesystem>
#include <thread>
#include <unordered_map>
#include <nlohmann/json.hpp>

namespace nlm {

// Helper functions
namespace {
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S.%f");
        return ss.str();
    }
    
    std::string getThreadId() {
        std::stringstream ss;
        ss << std::this_thread::get_id();
        return ss.str();
    }
    
    std::string getProcessId() {
        std::stringstream ss;
        ss << getpid();
        return ss.str();
    }
    
    std::string getCurrentDirectory() {
        char buffer[256];
        if (getcwd(buffer, sizeof(buffer)) != nullptr) {
            return std::string(buffer);
        }
        return "unknown";
    }
}

// StructuredLogEntry implementations
std::string StructuredLogEntry::toJSON() const {
    nlohmann::json json;
    json["timestamp"] = std::chrono::system_clock::to_time_t(timestamp);
    json["level"] = static_cast<int>(level);
    json["message"] = message;
    json["file"] = file;
    json["line"] = line;
    json["function"] = function;
    json["threadId"] = threadId;
    json["processId"] = processId;
    
    if (!metadata.empty()) {
        nlohmann::json metaJson = nlohmann::json::object();
        for (const auto& pair : metadata) {
            metaJson[pair.first] = pair.second;
        }
        json["metadata"] = metaJson;
    }
    
    return json.dump(2);
}

std::string StructuredLogEntry::toYAML() const {
    YAML::Emitter emitter;
    emitter << YAML::BeginMap;
    emitter << YAML::Key << "timestamp" << YAML::Value << std::chrono::system_clock::to_time_t(timestamp);
    emitter << YAML::Key << "level" << YAML::Value << static_cast<int>(level);
    emitter << YAML::Key << "message" << YAML::Value << message;
    emitter << YAML::Key << "file" << YAML::Value << file;
    emitter << YAML::Key << "line" << YAML::Value << line;
    emitter << YAML::Key << "function" << YAML::Value << function;
    emitter << YAML::Key << "threadId" << YAML::Value << threadId;
    emitter << YAML::Key << "processId" << YAML::Value << processId;
    
    if (!metadata.empty()) {
        emitter << YAML::Key << "metadata" << YAML::Value << YAML::BeginMap;
        for (const auto& pair : metadata) {
            emitter << YAML::Key << pair.first << YAML::Value << pair.second;
        }
        emitter << YAML::EndMap;
    }
    
    emitter << YAML::EndMap;
    return emitter.c_str();
}

std::string StructuredLogEntry::toCSV() const {
    std::stringstream ss;
    ss << std::chrono::system_clock::to_time_t(timestamp) << ","
       << static_cast<int>(level) << ","
       << "\"" << message << "\"" << ","
       << file << ","
       << line << ","
       << function << ","
       << threadId << ","
       << processId;
    
    if (!metadata.empty()) {
        ss << ",";
        bool first = true;
        for (const auto& pair : metadata) {
            if (!first) ss << ";";
            ss << pair.first << "=" << pair.second;
            first = false;
        }
    }
    
    return ss.str();
}

std::string StructuredLogEntry::toPlainText() const {
    std::stringstream ss;
    ss << "[" << getLevelString(level) << "] "
       << getCurrentTimestamp() << " "
       << message;
    
    if (!file.empty()) {
        ss << " (" << file << ":" << line << ")";
    }
    
    if (!function.empty()) {
        ss << " in " << function;
    }
    
    if (!threadId.empty()) {
        ss << " [thread:" << threadId << "]";
    }
    
    if (!metadata.empty()) {
        ss << " [";
        bool first = true;
        for (const auto& pair : metadata) {
            if (!first) ss << ", ";
            ss << pair.first << "=" << pair.second;
            first = false;
        }
        ss << "]";
    }
    
    return ss.str();
}

std::string getLevelString(LogLevel level) {
    switch (level) {
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Critical: return "CRIT";
        default: return "UNKNOWN";
    }
}

bool LogRotationConfig::shouldRotate(const std::string& filepath, uint64_t fileSize) const {
    switch (strategy) {
        case LogRotationStrategy::Never:
            return false;
        
        case LogRotationStrategy::SizeBased:
            return fileSize >= maxFileSize;
        
        case LogRotationStrategy::TimeBased: {
            auto lastWrite = std::filesystem::last_write_time(filepath);
            auto now = std::chrono::system_clock::now();
            auto age = std::chrono::duration_cast<std::chrono::hours>(now - lastWrite);
            return age >= rotationInterval;
        }
        
        case LogRotationStrategy::TimeBasedWithSize:
            return fileSize >= maxFileSize || 
                   (std::filesystem::exists(filepath) && 
                    std::chrono::duration_cast<std::chrono::hours>(
                        std::chrono::system_clock::now() - 
                        std::filesystem::last_write_time(filepath)
                    ) >= rotationInterval);
        
        default:
            return false;
    }
}

// ConsoleLogger implementations
struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    LogRotationConfig rotationConfig;
    std::string currentFilepath;
    uint64_t currentFileSize = 0;
    bool structuredOutput = false;
    std::string outputFormat = "PLAIN";  // PLAIN, JSON, YAML, CSV
    bool asyncLogging = false;
    std::mutex asyncMutex;
    std::vector<std::pair<std::string, std::string>> metadata;
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
}

ConsoleLogger::~ConsoleLogger() = default;

void ConsoleLogger::log(const StructuredLogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    if (pImpl->asyncLogging) {
        std::lock_guard<std::mutex> lock(pImpl->asyncMutex);
        // For async logging, we would typically use a queue and worker thread
        // For simplicity, we'll log directly
        outputEntry(entry);
    } else {
        outputEntry(entry);
    }
}

void ConsoleLogger::outputEntry(const StructuredLogEntry& entry) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::string output;
    if (pImpl->structuredOutput) {
        if (pImpl->outputFormat == "JSON") {
            output = entry.toJSON();
        } else if (pImpl->outputFormat == "YAML") {
            output = entry.toYAML();
        } else if (pImpl->outputFormat == "CSV") {
            output = entry.toCSV();
        } else {
            output = entry.toPlainText();
        }
    } else {
        output = entry.toPlainText();
    }
    
    // Console output
    if (pImpl->useColors) {
        std::cout << getLevelColor(entry.level) << output << "\033[0m\n";
    } else {
        std::cout << output << "\n";
    }
    
    // File output with rotation
    if (pImpl->fileOut.is_open()) {
        // Check if rotation is needed
        pImpl->fileOut << output << "\n";
        pImpl->fileOut.flush();
        pImpl->currentFileSize += output.size() + 1; // +1 for newline
        
        if (pImpl->rotationConfig.shouldRotate(pImpl->currentFilepath, pImpl->currentFileSize)) {
            rotateLogFile();
        }
    }
}

void ConsoleLogger::rotateLogFile() {
    if (!pImpl->fileOut.is_open()) {
        return;
    }
    
    pImpl->fileOut.close();
    
    // Generate new filename with timestamp
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S");
    
    std::string newFilename = pImpl->currentFilepath.substr(0, pImpl->currentFilepath.find_last_of('.'));
    newFilename += "_" + ss.str() + pImpl->currentFilepath.substr(pImpl->currentFilepath.find_last_of('.'));
    
    // Backup old file
    if (pImpl->rotationConfig.compressOldFiles) {
        std::string backupName = newFilename + pImpl->rotationConfig.backupExtension;
        std::rename(pImpl->currentFilepath.c_str(), backupName.c_str());
    } else {
        std::string backupName = newFilename + pImpl->rotationConfig.backupExtension;
        std::rename(pImpl->currentFilepath.c_str(), backupName.c_str());
    }
    
    // Open new file
    pImpl->fileOut.open(pImpl->currentFilepath, std::ios::trunc);
    pImpl->currentFileSize = 0;
}

void ConsoleLogger::setLevel(LogLevel level) {
    pImpl->level = level;
}

LogLevel ConsoleLogger::getLevel() const {
    return pImpl->level;
}

void ConsoleLogger::setUseColors(bool useColors) {
    pImpl->useColors = useColors;
}

bool ConsoleLogger::getUseColors() const {
    return pImpl->useColors;
}

std::string ConsoleLogger::getLevelColor(LogLevel level) const {
    switch (level) {
        case LogLevel::Debug: return "\033[36m";    // Cyan
        case LogLevel::Info: return "\033[32m";      // Green
        case LogLevel::Warning: return "\033[33m";   // Yellow
        case LogLevel::Error: return "\033[31m";     // Red
        case LogLevel::Critical: return "\033[35m"; // Magenta
        default: return "\033[0m";
    }
}

void ConsoleLogger::setRotationConfig(const LogRotationConfig& config) {
    pImpl->rotationConfig = config;
}

LogRotationConfig ConsoleLogger::getRotationConfig() const {
    return pImpl->rotationConfig;
}

void ConsoleLogger::enableStructuredOutput(bool enable) {
    pImpl->structuredOutput = enable;
}

bool ConsoleLogger::isStructuredOutputEnabled() const {
    return pImpl->structuredOutput;
}

void ConsoleLogger::setOutputFormat(const std::string& format) {
    pImpl->outputFormat = format;
}

std::string ConsoleLogger::getOutputFormat() const {
    return pImpl->outputFormat;
}

void ConsoleLogger::enableAsyncLogging(bool enable) {
    pImpl->asyncLogging = enable;
}

bool ConsoleLogger::isAsyncLoggingEnabled() const {
    return pImpl->asyncLogging;
}

void ConsoleLogger::setOutputFile(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
    
    pImpl->fileOut.open(filepath, std::ios::app);
    if (pImpl->fileOut.is_open()) {
        pImpl->currentFilepath = filepath;
        // Get current file size for rotation tracking
        pImpl->currentFileSize = std::filesystem::exists(filepath) ? 
            std::filesystem::file_size(filepath) : 0;
    }
}

void ConsoleLogger::closeOutputFile() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

void ConsoleLogger::addMetadata(const std::string& key, const std::string& value) {
    pImpl->metadata.push_back(std::make_pair(key, value));
}

void ConsoleLogger::clearMetadata() {
    pImpl->metadata.clear();
}

std::vector<std::pair<std::string, std::string>> ConsoleLogger::getMetadata() const {
    return pImpl->metadata;
}

// Logger implementations
struct Logger::Impl {
    std::vector<std::shared_ptr<IAdvancedLogger>> loggers;
    std::mutex mutex;
    LogStatistics statistics;
    bool performanceMonitoringEnabled = false;
    std::string logPattern = "[%level] %timestamp - %message";
    LogRotationConfig globalRotationConfig;
    std::unordered_map<std::string, std::shared_ptr<IAdvancedLogger>> fileLoggers;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {
    pImpl->statistics.firstLogTime = std::chrono::system_clock::now();
}

Logger::~Logger() = default;

void Logger::addLogger(std::shared_ptr<IAdvancedLogger> logger) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.push_back(logger);
}

void Logger::clearLoggers() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.clear();
}

void Logger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        logger->setLevel(level);
    }
}

void Logger::log(LogLevel level, const std::string& message,
                 const std::string& file, int line,
                 const std::string& function) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    StructuredLogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.threadId = getThreadId();
    entry.processId = getProcessId();
    
    // Add metadata from all loggers
    for (auto& logger : pImpl->loggers) {
        auto metadata = logger->getMetadata();
        for (const auto& pair : metadata) {
            entry.metadata.push_back(pair);
        }
    }
    
    for (auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
    
    // Update statistics
    pImpl->statistics.totalCount++;
    switch (level) {
        case LogLevel::Debug: pImpl->statistics.debugCount++; break;
        case LogLevel::Info: pImpl->statistics.infoCount++; break;
        case LogLevel::Warning: pImpl->statistics.warningCount++; break;
        case LogLevel::Error: pImpl->statistics.errorCount++; break;
        case LogLevel::Critical: pImpl->statistics.criticalCount++; break;
    }
    
    if (pImpl->performanceMonitoringEnabled) {
        auto now = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now - entry.timestamp).count();
        pImpl->statistics.avgLogTime = 
            (pImpl->statistics.avgLogTime * (pImpl->statistics.totalCount - 1) + duration) / 
            pImpl->statistics.totalCount;
    }
    
    pImpl->statistics.lastLogTime = now;
}

void Logger::logWithMetadata(LogLevel level, const std::string& message,
                             const std::vector<std::pair<std::string, std::string>>& metadata,
                             const std::string& file, int line,
                             const std::string& function) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    StructuredLogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.threadId = getThreadId();
    entry.processId = getProcessId();
    entry.metadata = metadata;
    
    for (auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
    
    // Update statistics
    pImpl->statistics.totalCount++;
    switch (level) {
        case LogLevel::Debug: pImpl->statistics.debugCount++; break;
        case LogLevel::Info: pImpl->statistics.infoCount++; break;
        case LogLevel::Warning: pImpl->statistics.warningCount++; break;
        case LogLevel::Error: pImpl->statistics.errorCount++; break;
        case LogLevel::Critical: pImpl->statistics.criticalCount++; break;
    }
}

void Logger::structuredLog(LogLevel level, const std::string& message,
                           const std::vector<std::pair<std::string, std::string>>& metadata) {
    logWithMetadata(level, message, metadata);
}

void Logger::debug(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Debug, message, file, line, function);
}

void Logger::info(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Info, message, file, line, function);
}

void Logger::warning(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Warning, message, file, line, function);
}

void Logger::error(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Error, message, file, line, function);
}

void Logger::critical(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Critical, message, file, line, function);
}

bool Logger::isEnabled(LogLevel level) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        if (logger->getLevel() <= level) {
            return true;
        }
    }
    return false;
}

Logger& Logger::getGlobal() {
    static Logger globalLogger;
    return globalLogger;
}

void Logger::setGlobal(std::shared_ptr<Logger> logger) {
    // This would typically replace the global logger
    // For now, we just store it but continue using getGlobal()
    // In practice, this could set a global pointer
}

void Logger::enablePerformanceMonitoring(bool enable) {
    pImpl->performanceMonitoringEnabled = enable;
}

bool Logger::isPerformanceMonitoringEnabled() const {
    return pImpl->performanceMonitoringEnabled;
}

Logger::LogStatistics Logger::getStatistics() const {
    return pImpl->statistics;
}

void Logger::LogStatistics::reset() {
    debugCount = infoCount = warningCount = errorCount = criticalCount = totalCount = 0;
    avgLogTime = 0.0;
    firstLogTime = lastLogTime = std::chrono::system_clock::now();
}

std::string Logger::LogStatistics::toString() const {
    std::stringstream ss;
    ss << "Log Statistics:" << std::endl;
    ss << "  Total logs: " << totalCount << std::endl;
    ss << "  Debug: " << debugCount << std::endl;
    ss << "  Info: " << infoCount << std::endl;
    ss << "  Warning: " << warningCount << std::endl;
    ss << "  Error: " << errorCount << std::endl;
    ss << "  Critical: " << criticalCount << std::endl;
    ss << "  Average log time: " << avgLogTime << " microseconds" << std::endl;
    ss << "  First log: " << std::chrono::system_clock::to_time_t(firstLogTime) << std::endl;
    ss << "  Last log: " << std::chrono::system_clock::to_time_t(lastLogTime) << std::endl;
    return ss.str();
}

void Logger::flushAll() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        // If the logger has a flush method, call it
        // For now, we assume file logging is automatically flushed
    }
}

void Logger::setGlobalRotationConfig(const LogRotationConfig& config) {
    pImpl->globalRotationConfig = config;
}

LogRotationConfig Logger::getGlobalRotationConfig() const {
    return pImpl->globalRotationConfig;
}

void Logger::closeAllFiles() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        // Cast to AdvancedConsoleLogger to access file operations
        auto consoleLogger = std::dynamic_pointer_cast<AdvancedConsoleLogger>(logger);
        if (consoleLogger) {
            consoleLogger->closeOutputFile();
        }
    }
}

std::vector<std::string> Logger::getActiveLogFiles() const {
    std::vector<std::string> files;
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (auto& logger : pImpl->loggers) {
        auto consoleLogger = std::dynamic_pointer_cast<AdvancedConsoleLogger>(logger);
        if (consoleLogger) {
            // This is a simplification - in practice, we'd need access to the current filepath
            // For now, we'll just return an empty list
        }
    }
    
    return files;
}

void Logger::setLogPattern(const std::string& pattern) {
    pImpl->logPattern = pattern;
}

std::string Logger::getLogPattern() const {
    return pImpl->logPattern;
}

// LogStream implementations
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function) {}

LogStream::~LogStream() {
    Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
}

// StructuredLogStream implementations
StructuredLogStream::StructuredLogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function) {}

StructuredLogStream::~StructuredLogStream() {
    Logger::getGlobal().logWithMetadata(level_, oss_.str(), metadata_, file_, line_, function_);
}

StructuredLogStream& StructuredLogStream::addMetadata(const std::string& key, const std::string& value) {
    metadata_.push_back(std::make_pair(key, value));
    return *this;
}

// Performance monitoring implementation (simplified)
void enablePerformanceMonitoring(bool enable) {
    Logger::getGlobal().enablePerformanceMonitoring(enable);
}

Logger::LogStatistics getLogStatistics() {
    return Logger::getGlobal().getStatistics();
}

void flushAllLogs() {
    Logger::getGlobal().flushAll();
}

void setGlobalLogRotationConfig(const LogRotationConfig& config) {
    Logger::getGlobal().setGlobalRotationConfig(config);
}

} // namespace nlm