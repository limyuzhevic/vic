#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <ctime>
#include <iomanip>
#include <thread>

namespace nlm {

// Logger implementation with enhanced thread safety and features

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    std::string filepath;
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
}

ConsoleLogger::~ConsoleLogger() = default;

void ConsoleLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    
    // Add timestamp
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    
    oss << " " << entry.message;
    
    std::string output = oss.str();
    
    // Console output
    if (pImpl->useColors) {
        std::cout << levelToColor(entry.level) << output << "\033[0m\n";
    } else {
        std::cout << output << "\n";
    }
    
    // File output if set
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut << output << "\n";
        pImpl->fileOut.flush();
    }
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

void ConsoleLogger::setOutputFile(const std::string& filepath) {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
    pImpl->fileOut.open(filepath, std::ios::app);
    pImpl->filepath = filepath;
}

void ConsoleLogger::closeOutputFile() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

std::string ConsoleLogger::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Critical: return "CRIT";
        default: return "UNKNOWN";
    }
}

std::string ConsoleLogger::levelToColor(LogLevel level) const {
    switch (level) {
        case LogLevel::Debug: return "\033[36m";    // Cyan
        case LogLevel::Info: return "\033[32m";      // Green
        case LogLevel::Warning: return "\033[33m";   // Yellow
        case LogLevel::Error: return "\033[31m";     // Red
        case LogLevel::Critical: return "\033[35m"; // Magenta
        default: return "\033[0m";
    }
}

// PerformanceLogger implementation

struct PerformanceLogger::Impl {
    LoggerConfig config;
    LoggerStatistics stats;
    std::mutex mutex;
    std::ofstream fileStream;
    std::string currentFileName;
    uint64_t currentFileSize;
    std::chrono::system_clock::time_point lastRotationTime;
    std::vector<std::string> filterPatterns;
    std::map<std::string, size_t> patternCount;
};

PerformanceLogger::PerformanceLogger() : pImpl(std::make_unique<Impl>()) {
    pImpl->config.enableColors = true;
    pImpl->currentFileSize = 0;
    pImpl->lastRotationTime = std::chrono::system_clock::now();
}

PerformanceLogger::PerformanceLogger(const LoggerConfig& config) : pImpl(std::make_unique<Impl>()) {
    pImpl->config = config;
    pImpl->currentFileSize = 0;
    pImpl->lastRotationTime = std::chrono::system_clock::now();
    
    if (config.enableFileOutput) {
        openFileStream();
    }
}

PerformanceLogger::~PerformanceLogger() {
    if (pImpl->fileStream.is_open()) {
        pImpl->fileStream.close();
    }
}

void PerformanceLogger::log(const LogEntry& entry) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Check if level is enabled
    if (!shouldLogLevel(entry.level)) {
        return;
    }
    
    // Check filter patterns
    if (!pImpl->filterPatterns.empty()) {
        bool matches = matchesFilter(entry.message);
        if (pImpl->config.excludeFiltered) {
            if (!matches) {
                return;
            }
        } else {
            if (!matches) {
                return;
            }
        }
    }
    
    // Check rotation
    if (shouldRotate()) {
        rotateFile();
    }
    
    // Format entry
    std::string formatted;
    if (pImpl->config.format == LogFormat::Colored && pImpl->config.enableColors) {
        formatted = formatColored(entry);
    } else if (pImpl->config.format == LogFormat::Detailed) {
        formatted = formatDetailed(entry);
    } else if (pImpl->config.format == LogFormat::Structured) {
        formatted = formatStructured(entry);
    } else if (!pImpl->config.customFormat.empty()) {
        formatted = applyCustomFormat(entry);
    } else {
        formatted = formatSimple(entry);
    }
    
    // Output to console
    std::cout << formatted << std::endl;
    
    // Output to file
    if (pImpl->config.enableFileOutput && pImpl->fileStream.is_open()) {
        pImpl->fileStream << formatted << std::endl;
        pImpl->currentFileSize += formatted.size() + 1;  // +1 for newline
        
        // Periodic flush
        if (pImpl->currentFileSize >= pImpl->config.flushInterval) {
            pImpl->fileStream.flush();
            pImpl->currentFileSize = 0;
        }
    }
    
    // Update statistics
    stats_.increment(entry.level);
}

void PerformanceLogger::setLevel(LogLevel level) {
    pImpl->config.minimumLevel = level;
}

LogLevel PerformanceLogger::getLevel() const {
    return pImpl->config.minimumLevel;
}

void PerformanceLogger::setConfig(const LoggerConfig& config) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->config = config;
    
    // Reopen file stream if needed
    if (pImpl->config.enableFileOutput && !pImpl->config.filePath.empty()) {
        if (pImpl->fileStream.is_open()) {
            pImpl->fileStream.close();
        }
        openFileStream();
    }
}

LoggerConfig PerformanceLogger::getConfig() const {
    return pImpl->config;
}

std::string PerformanceLogger::formatEntry(const LogEntry& entry) const {
    if (pImpl->config.format == LogFormat::Colored && pImpl->config.enableColors) {
        return formatColored(entry);
    } else if (pImpl->config.format == LogFormat::Detailed) {
        return formatDetailed(entry);
    } else if (pImpl->config.format == LogFormat::Structured) {
        return formatStructured(entry);
    } else if (!pImpl->config.customFormat.empty()) {
        return applyCustomFormat(entry);
    } else {
        return formatSimple(entry);
    }
}

void PerformanceLogger::rotateFile() {
    if (!pImpl->config.enableFileOutput || pImpl->config.rotationStrategy == RotationStrategy::None) {
        return;
    }
    
    auto now = std::chrono::system_clock::now();
    
    // Check time-based rotation
    if (pImpl->config.rotationStrategy == RotationStrategy::Time ||
        pImpl->config.rotationStrategy == RotationStrategy::SizeAndTime) {
        auto timeSinceLastRotation = std::chrono::duration_cast<std::chrono::hours>(now - pImpl->lastRotationTime);
        
        bool timeToRotate = false;
        if (pImpl->config.rotationStrategy == RotationStrategy::Time) {
            timeToRotate = timeSinceLastRotation.count() >= 1;  // Rotate every hour
        } else if (pImpl->config.rotationStrategy == RotationStrategy::SizeAndTime) {
            timeToRotate = timeSinceLastRotation.count() >= 1;  // Rotate every hour
        }
        
        if (!timeToRotate) {
            return;
        }
    }
    
    // Close current file
    if (pImpl->fileStream.is_open()) {
        pImpl->fileStream.close();
    }
    
    // Generate backup filename
    std::string baseName = pImpl->currentFileName;
    size_t dotPos = baseName.find_last_of('.');
    std::string stem = (dotPos == std::string::npos) ? baseName : baseName.substr(0, dotPos);
    std::string ext = (dotPos == std::string::npos) ? "" : baseName.substr(dotPos);
    
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&now_time);
    
    std::ostringstream oss;
    oss << stem << "_" << std::put_time(&tm, "%Y%m%d_%H%M%S") << ext;
    
    // Rename current file to backup
    if (std::filesystem::exists(pImpl->currentFileName)) {
        std::error_code ec;
        std::filesystem::rename(pImpl->currentFileName, oss.str(), ec);
        if (ec) {
            return; // Failed to rename
        }
    }
    
    // Remove old backup files
    if (pImpl->config.maxBackupFiles > 0) {
        for (int i = pImpl->config.maxBackupFiles; i < INT_MAX; ++i) {
            std::ostringstream backupOss;
            backupOss << stem << "_backup_" << i << ext;
            if (std::filesystem::exists(backupOss.str())) {
                std::error_code ec;
                std::filesystem::remove(backupOss.str(), ec);
            } else {
                break; // No more backup files
            }
        }
    }
    
    // Open new file
    openFileStream();
    pImpl->lastRotationTime = now;
    pImpl->currentFileSize = 0;
}

bool PerformanceLogger::shouldRotate() const {
    if (!pImpl->config.enableFileOutput || pImpl->config.rotationStrategy == RotationStrategy::None) {
        return false;
    }
    
    // Check size-based rotation
    if (pImpl->config.rotationStrategy == RotationStrategy::Size ||
        pImpl->config.rotationStrategy == RotationStrategy::SizeAndTime) {
        if (pImpl->currentFileSize >= pImpl->config.maxFileSize) {
            return true;
        }
    }
    
    // Check time-based rotation
    if (pImpl->config.rotationStrategy == RotationStrategy::Time ||
        pImpl->config.rotationStrategy == RotationStrategy::SizeAndTime) {
        auto now = std::chrono::system_clock::now();
        auto timeSinceLastRotation = std::chrono::duration_cast<std::chrono::hours>(now - pImpl->lastRotationTime);
        if (timeSinceLastRotation.count() >= 1) {  // 1 hour
            return true;
        }
    }
    
    return false;
}

void PerformanceLogger::flush() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileStream.is_open()) {
        pImpl->fileStream.flush();
    }
}

void PerformanceLogger::resetStatistics() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    stats_ = LoggerStatistics();
}

void PerformanceLogger::setFilterPattern(const std::vector<std::string>& patterns) {
    pImpl->filterPatterns = patterns;
}

bool PerformanceLogger::matchesFilter(const std::string& message) const {
    if (pImpl->filterPatterns.empty()) {
        return true;
    }
    
    for (const auto& pattern : pImpl->filterPatterns) {
        if (message.find(pattern) != std::string::npos) {
            return true;
        }
    }
    return false;
}

std::string PerformanceLogger::formatDetailed(const LogEntry& entry) const {
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    
    oss << " [" << entry.file << ":" << entry.line << " " << entry.function << "] ";
    oss << entry.message;
    
    return oss.str();
}

std::string PerformanceLogger::formatSimple(const LogEntry& entry) const {
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    
    oss << " " << entry.message;
    
    return oss.str();
}

std::string PerformanceLogger::formatColored(const LogEntry& entry) const {
    if (!pImpl->config.enableColors) {
        return formatDetailed(entry);
    }
    
    std::string color = levelToColor(entry.level);
    std::string reset = "\033[0m";
    
    std::ostringstream oss;
    oss << color << "[" << levelToString(entry.level) << "]";
    
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    oss << " [" << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S") << "]";
    
    oss << " " << entry.message << reset;
    
    return oss.str();
}

std::string PerformanceLogger::formatStructured(const LogEntry& entry) const {
    // Simple JSON-like format
    std::ostringstream oss;
    oss << "{\"timestamp\": \"" << timestampToString(entry.timestamp) << "\", \"level\": \"" << levelToString(entry.level) << "\", \"message\": \"" << entry.message << "\"";
    
    if (!entry.file.empty() && entry.line > 0) {
        oss << ", \"file\": \"" << entry.file << "\", \"line\": " << entry.line;
    }
    
    oss << "}";
    return oss.str();
}

std::string PerformanceLogger::applyCustomFormat(const LogEntry& entry) const {
    std::string format = pImpl->config.customFormat;
    
    // Replace placeholders
    size_t pos = 0;
    while ((pos = format.find("{level}", pos)) != std::string::npos) {
        format.replace(pos, 7, levelToString(entry.level));
        pos += levelToString(entry.level).size();
    }
    
    pos = 0;
    while ((pos = format.find("{time}", pos)) != std::string::npos) {
        format.replace(pos, 6, timestampToString(entry.timestamp));
        pos += timestampToString(entry.timestamp).size();
    }
    
    pos = 0;
    while ((pos = format.find("{message}", pos)) != std::string::npos) {
        format.replace(pos, 9, entry.message);
        pos += entry.message.size();
    }
    
    pos = 0;
    while ((pos = format.find("{file}", pos)) != std::string::npos) {
        format.replace(pos, 7, entry.file);
        pos += entry.file.size();
    }
    
    pos = 0;
    while ((pos = format.find("{line}", pos)) != std::string::npos) {
        format.replace(pos, 7, std::to_string(entry.line));
        pos += std::to_string(entry.line).size();
    }
    
    return format;
}

bool PerformanceLogger::shouldLogLevel(LogLevel level) const {
    return static_cast<int>(level) >= static_cast<int>(pImpl->config.minimumLevel);
}

std::string PerformanceLogger::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Critical: return "CRIT";
        default: return "UNKNOWN";
    }
}

std::string PerformanceLogger::levelToColor(LogLevel level) const {
    if (!pImpl->config.enableColors) {
        return "";
    }
    
    switch (level) {
        case LogLevel::Debug: return "\033[36m";    // Cyan
        case LogLevel::Info: return "\033[32m";      // Green
        case LogLevel::Warning: return "\033[33m";   // Yellow
        case LogLevel::Error: return "\033[31m";     // Red
        case LogLevel::Critical: return "\033[35m"; // Magenta
        default: return "\033[0m";
    }
}

std::string PerformanceLogger::timestampToString(const std::chrono::system_clock::time_point& tp) const {
    auto time = std::chrono::system_clock::to_time_t(tp);
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

void PerformanceLogger::openFileStream() {
    if (!pImpl->config.enableFileOutput || pImpl->config.filePath.empty()) {
        return;
    }
    
    // Close existing stream
    if (pImpl->fileStream.is_open()) {
        pImpl->fileStream.close();
    }
    
    // Open new file
    pImpl->fileStream.open(pImpl->config.filePath, std::ios::app);
    if (pImpl->fileStream.is_open()) {
        pImpl->currentFileName = pImpl->config.filePath;
        // Write header if file is empty
        if (pImpl->fileStream.tellp() == 0) {
            pImpl->fileStream << "# NLM Log File\n";
            pImpl->fileStream << "# Format: " << (pImpl->config.format == LogFormat::Detailed ? "Detailed" : 
                                                        pImpl->config.format == LogFormat::Colored ? "Colored" : 
                                                        pImpl->config.format == LogFormat::Structured ? "Structured" : "Simple") << "\n";
            pImpl->fileStream << "# Minimum Level: " << levelToString(pImpl->config.minimumLevel) << "\n\n";
        }
    }
}

// Main Logger implementation

struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    mutable std::mutex mutex;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {}

Logger::Logger(const LoggerConfig& config) : pImpl(std::make_unique<Impl>()) {
    // Create default logger with config
    auto performanceLogger = std::make_shared<PerformanceLogger>(config);
    pImpl->loggers.push_back(performanceLogger);
}

Logger::~Logger() = default;

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (logger) {
        pImpl->loggers.push_back(logger);
    }
}

void Logger::removeLogger(ILogger* logger) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.erase(
        std::remove_if(pImpl->loggers.begin(), pImpl->loggers.end(),
            [&logger](const std::shared_ptr<ILogger>& l) { return l.get() == logger; }),
        pImpl->loggers.end()
    );
}

void Logger::clearLoggers() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.clear();
}

void Logger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (const auto& logger : pImpl->loggers) {
        logger->setLevel(level);
    }
}

void Logger::setConfig(const LoggerConfig& config) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (const auto& logger : pImpl->loggers) {
        logger->setConfig(config);
    }
}

void Logger::log(LogLevel level, const std::string& message,
                 const std::string& file, int line,
                 const std::string& function) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    
    for (const auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
}

void Logger::debug(const std::string& message, const std::string& file, int line, const std::string& function) {
    if (Logger::getGlobal().isEnabled(LogLevel::Debug)) {
        log(LogLevel::Debug, message, file, line, function);
    }
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

Logger::LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function) {}

Logger::LogStream::~LogStream() {
    if (!oss_.str().empty()) {
        Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
    }
}

Logger::LogStream Logger::createStream(LogLevel level, const std::string& file, int line, const std::string& function) {
    return LogStream(level, file, line, function);
}

bool Logger::isEnabled(LogLevel level) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (const auto& logger : pImpl->loggers) {
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
    // In practice, you would implement a more sophisticated global logger management
    // For now, just store the reference
    static std::shared_ptr<Logger> globalLogger;
    globalLogger = logger;
}

LoggerConfig Logger::getGlobalConfig() {
    return getGlobal().pImpl->loggers.empty() ? LoggerConfig() : getGlobal().pImpl->loggers[0]->getConfig();
}

void Logger::setGlobalConfig(const LoggerConfig& config) {
    getGlobal().setConfig(config);
}

LoggerStatistics Logger::getGlobalStatistics() {
    LoggerStatistics stats;
    for (const auto& logger : getGlobal().pImpl->loggers) {
        auto performanceLogger = std::dynamic_pointer_cast<PerformanceLogger>(logger);
        if (performanceLogger) {
            stats.totalMessages += performanceLogger->getTotalLogCount();
            for (int i = 0; i < 5; ++i) {
                stats.messagesByLevel[i] += performanceLogger->getLogCount(static_cast<LogLevel>(i));
            }
        }
    }
    return stats;
}

void Logger::resetGlobalStatistics() {
    for (const auto& logger : getGlobal().pImpl->loggers) {
        auto performanceLogger = std::dynamic_pointer_cast<PerformanceLogger>(logger);
        if (performanceLogger) {
            performanceLogger->resetStatistics();
        }
    }
}

} // namespace nlm
