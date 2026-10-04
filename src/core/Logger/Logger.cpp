#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <thread>
#include <sys/types.h>
#include <unistd.h>

namespace nlm {

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    
    // Enhanced logging options
    std::string format = "[LEVEL] [TIMESTAMP] MESSAGE";
    std::function<bool(const LogEntry&)> filter;
    std::set<std::string> categories;
    std::map<std::string, std::string> metadata;
    size_t maxMessageSize = 10000;
    size_t maxFileSize = 10 * 1024 * 1024; // 10MB
    bool performanceTracking = false;
    bool memoryTracking = false;
    bool threadingInfo = false;
    size_t bufferSize = 1024;
    bool buffered = false;
    size_t fileRotationCount = 5;
    size_t maxFileSizeForRotation = 100 * 1024 * 1024; // 100MB
    bool asyncLogging = false;
    size_t asyncQueueSize = 1000;
    std::string outputFile;
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {
    // Set up default file rotation
    pImpl->maxFileSize = 10 * 1024 * 1024;
    pImpl->fileRotationCount = 5;
    pImpl->maxFileSizeForRotation = 100 * 1024 * 1024;
}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
    pImpl->maxFileSize = 10 * 1024 * 1024;
    pImpl->fileRotationCount = 5;
    pImpl->maxFileSizeForRotation = 100 * 1024 * 1024;
}

ConsoleLogger::~ConsoleLogger() = default;

void ConsoleLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    // Apply filter if set
    if (pImpl->filter && !pImpl->filter(entry)) {
        return;
    }
    
    // Apply category filter if categories are set
    if (!pImpl->categories.empty() && !entry.category.empty()) {
        if (pImpl->categories.find(entry.category) == pImpl->categories.end()) {
            return;
        }
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << formatMessage(entry);
    
    std::string output = oss.str();
    
    // Check message size
    if (output.size() > pImpl->maxMessageSize) {
        output = output.substr(0, pImpl->maxMessageSize - 3) + "...";
    }
    
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
        
        // Check if we need to rotate files
        if (pImpl->fileOut.tellp() >= static_cast<std::streampos>(pImpl->maxFileSizeForRotation)) {
            rotateFile();
        }
    }
    
    // Track performance if enabled
    if (pImpl->performanceTracking && entry.elapsedTime > 0) {
        // Could log performance statistics here
    }
    
    // Track memory if enabled
    if (pImpl->memoryTracking && entry.memoryDelta != 0) {
        // Could log memory statistics here
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
    // Close existing file if open
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
    
    // Set new file
    pImpl->outputFile = filepath;
    pImpl->fileOut.open(filepath, std::ios::app);
    
    if (!pImpl->fileOut.is_open()) {
        std::cerr << "Warning: Could not open log file: " << filepath << std::endl;
    }
}

void ConsoleLogger::closeOutputFile() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

bool ConsoleLogger::isFileOutputOpen() const {
    return pImpl->fileOut.is_open();
}

std::string ConsoleLogger::getOutputFile() const {
    return pImpl->outputFile;
}

void ConsoleLogger::setFormat(const std::string& format) {
    pImpl->format = format;
}

std::string ConsoleLogger::getFormat() const {
    return pImpl->format;
}

void ConsoleLogger::setFilter(std::function<bool(const LogEntry&)> filter) {
    pImpl->filter = std::move(filter);
}

void ConsoleLogger::addCategory(const std::string& category) {
    pImpl->categories.insert(category);
}

void ConsoleLogger::removeCategory(const std::string& category) {
    pImpl->categories.erase(category);
}

std::set<std::string> ConsoleLogger::getCategories() const {
    return pImpl->categories;
}

void ConsoleLogger::setMetadata(const std::string& key, const std::string& value) {
    pImpl->metadata[key] = value;
}

std::string ConsoleLogger::getMetadata(const std::string& key) const {
    auto it = pImpl->metadata.find(key);
    return it != pImpl->metadata.end() ? it->second : "";
}

void ConsoleLogger::clearMetadata() {
    pImpl->metadata.clear();
}

void ConsoleLogger::enableColors(bool enable) {
    pImpl->useColors = enable;
}

bool ConsoleLogger::getUseColors() const {
    return pImpl->useColors;
}

void ConsoleLogger::setMaxMessageSize(size_t maxSize) {
    pImpl->maxMessageSize = maxSize;
}

size_t ConsoleLogger::getMaxMessageSize() const {
    return pImpl->maxMessageSize;
}

void ConsoleLogger::setMaxFileSize(size_t maxSize) {
    pImpl->maxFileSizeForRotation = maxSize;
}

size_t ConsoleLogger::getMaxFileSize() const {
    return pImpl->maxFileSizeForRotation;
}

void ConsoleLogger::enablePerformanceTracking(bool enable) {
    pImpl->performanceTracking = enable;
}

bool ConsoleLogger::isPerformanceTrackingEnabled() const {
    return pImpl->performanceTracking;
}

void ConsoleLogger::enableMemoryTracking(bool enable) {
    pImpl->memoryTracking = enable;
}

bool ConsoleLogger::isMemoryTrackingEnabled() const {
    return pImpl->memoryTracking;
}

void ConsoleLogger::enableThreadingInfo(bool enable) {
    pImpl->threadingInfo = enable;
}

bool ConsoleLogger::isThreadingInfoEnabled() const {
    return pImpl->threadingInfo;
}

void ConsoleLogger::setBufferSize(size_t bufferSize) {
    pImpl->bufferSize = bufferSize;
    pImpl->buffered = true;
}

size_t ConsoleLogger::getBufferSize() const {
    return pImpl->bufferSize;
}

void ConsoleLogger::flush() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.flush();
    }
}

bool ConsoleLogger::isBuffered() const {
    return pImpl->buffered;
}

void ConsoleLogger::setFileRotation(size_t maxFiles, size_t maxSizePerFile) {
    pImpl->fileRotationCount = maxFiles;
    pImpl->maxFileSizeForRotation = maxSizePerFile;
}

void ConsoleLogger::rotateFile() {
    if (!pImpl->fileOut.is_open()) {
        return;
    }
    
    pImpl->fileOut.close();
    
    // Implement file rotation logic here
    // For simplicity, we'll just close for now
    // In a real implementation, you'd rename files and create new ones
}

size_t ConsoleLogger::getFileCount() const {
    // Return the number of log files in the current directory
    // This is a simplified implementation
    return 1; // Currently only one file is tracked
}

void ConsoleLogger::enableAsyncLogging(bool enable) {
    pImpl->asyncLogging = enable;
}

bool ConsoleLogger::isAsyncLoggingEnabled() const {
    return pImpl->asyncLogging;
}

void ConsoleLogger::setAsyncQueueSize(size_t queueSize) {
    pImpl->asyncQueueSize = queueSize;
}

size_t ConsoleLogger::getAsyncQueueSize() const {
    return pImpl->asyncQueueSize;
}

std::string ConsoleLogger::formatMessage(const LogEntry& entry) const {
    std::ostringstream oss;
    
    // Replace format tokens
    std::string formatted = pImpl->format;
    
    // Simple token replacement
    size_t pos = 0;
    while ((pos = formatted.find("[LEVEL]")) != std::string::npos) {
        formatted.replace(pos, 7, levelToString(entry.level));
    }
    
    while ((pos = formatted.find("[TIMESTAMP]")) != std::string::npos) {
        formatted.replace(pos, 11, getTimestamp(entry));
    }
    
    while ((pos = formatted.find("[MESSAGE]")) != std::string::npos) {
        formatted.replace(pos, 8, entry.message);
    }
    
    while ((pos = formatted.find("[FILE]")) != std::string::npos) {
        formatted.replace(pos, 6, entry.file);
    }
    
    while ((pos = formatted.find("[LINE]")) != std::string::npos) {
        formatted.replace(pos, 6, std::to_string(entry.line));
    }
    
    while ((pos = formatted.find("[FUNCTION]")) != std::string::npos) {
        formatted.replace(pos, 9, entry.function);
    }
    
    while ((pos = formatted.find("[MODULE]")) != std::string::npos) {
        formatted.replace(pos, 8, entry.module);
    }
    
    while ((pos = formatted.find("[CATEGORY]")) != std::string::npos) {
        formatted.replace(pos, 9, entry.category);
    }
    
    // Add thread info if enabled
    if (pImpl->threadingInfo) {
        std::string threadInfo = getThreadInfo();
        while ((pos = formatted.find("[THREAD]")) != std::string::npos) {
            formatted.replace(pos, 8, threadInfo);
        }
    }
    
    // Add metadata if present
    for (const auto& meta : entry.metadata) {
        while ((pos = formatted.find("[METADATA_" + meta.first + "]")) != std::string::npos) {
            formatted.replace(pos, meta.first.size() + 12, meta.second);
        }
    }
    
    return formatted;
}

std::string ConsoleLogger::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::Critical: return "CRIT";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Info: return "INFO";
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Trace: return "TRACE";
        case LogLevel::Performance: return "PERF";
        case LogLevel::Memory: return "MEM";
        case LogLevel::Network: return "NET";
        case LogLevel::Security: return "SEC";
        case LogLevel::Development: return "DEV";
        case LogLevel::Config: return "CONF";
        case LogLevel::Neural: return "NEU";
        case LogLevel::Plasticity: return "PLT";
        case LogLevel::MemoryManagement: return "MEMM";
        case LogLevel::Threading: return "THR";
        case LogLevel::FileIO: return "FILEIO";
        case LogLevel::Database: return "DB";
        case LogLevel::HTTP: return "HTTP";
        case LogLevel::UI: return "UI";
        case LogLevel::Off: return "OFF";
        default: return "UNKNOWN";
    }
}

std::string ConsoleLogger::levelToColor(LogLevel level) const {
    if (!pImpl->useColors) {
        return "";
    }
    
    switch (level) {
        case LogLevel::Critical: return "\033[35m";    // Magenta
        case LogLevel::Error: return "\033[31m";     // Red
        case LogLevel::Warning: return "\033[33m";   // Yellow
        case LogLevel::Info: return "\033[32m";      // Green
        case LogLevel::Debug: return "\033[36m";     // Cyan
        case LogLevel::Trace: return "\033[90m";    // Gray
        case LogLevel::Performance: return "\033[34m";  // Blue
        case LogLevel::Memory: return "\033[92m";   // Bright Green
        case LogLevel::Network: return "\033[94m";  // Bright Blue
        case LogLevel::Security: return "\033[91m"; // Bright Red
        case LogLevel::Development: return "\033[95m"; // Bright Magenta
        case LogLevel::Config: return "\033[93m";    // Yellow
        case LogLevel::Neural: return "\033[96m";   // Cyan
        case LogLevel::Plasticity: return "\033[95m"; // Magenta
        case LogLevel::MemoryManagement: return "\033[92m"; // Bright Green
        case LogLevel::Threading: return "\033[34m";  // Blue
        case LogLevel::FileIO: return "\033[90m";    // Gray
        case LogLevel::Database: return "\033[96m"; // Cyan
        case LogLevel::HTTP: return "\033[94m";     // Bright Blue
        case LogLevel::UI: return "\033[93m";       // Yellow
        case LogLevel::Off: return "\033[0m";
        default: return "\033[0m";
    }
}

bool ConsoleLogger::shouldLog(const LogEntry& entry) const {
    // Check level
    if (entry.level < pImpl->level) {
        return false;
    }
    
    // Check categories
    if (!pImpl->categories.empty() && !entry.category.empty()) {
        if (pImpl->categories.find(entry.category) == pImpl->categories.end()) {
            return false;
        }
    }
    
    // Check filter
    if (pImpl->filter && !pImpl->filter(entry)) {
        return false;
    }
    
    return true;
}

std::string ConsoleLogger::getThreadInfo() const {
    std::ostringstream oss;
    
    // Get thread ID
    std::thread::id tid = std::this_thread::get_id();
    oss << "T:" << std::hash<std::thread::id>{}(tid);
    
    // Get thread name if available
    // This is a simplified version - in practice you'd use platform-specific APIs
    oss << "/" << std::this_thread::get_id();
    
    return oss.str();
}

std::string ConsoleLogger::getTimestamp(const LogEntry& entry) const {
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    std::tm tm_time;
    
    // Thread-safe version of localtime
    std::tm tmp;
    localtime_r(&time, &tmp);
    tm_time = tmp;
    
    std::ostringstream oss;
    oss << std::put_time(&tm_time, "%Y-%m-%d %H:%M:%S");
    
    // Add milliseconds
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        entry.timestamp.time_since_epoch()) % 1000;
    oss << "." << std::setfill('0') << std::setw(3) << ms.count();
    
    return oss.str();
}

std::string ConsoleLogger::escapeHtml(const std::string& text) const {
    std::string escaped;
    for (char c : text) {
        switch (c) {
            case '&': escaped += "&amp;"; break;
            case '<': escaped += "&lt;"; break;
            case '>': escaped += "&gt;"; break;
            case '"': escaped += "&quot;"; break;
            case '\'': escaped += "&#x27;"; break;
            case '/': escaped += "&#x2F;"; break;
            default: escaped += c; break;
        }
    }
    return escaped;
}

// Logger class implementation
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    std::atomic<uint64_t> logCounts[(int)LogLevel::All] = {0};
    uint64_t totalLogCount = 0;
    std::set<LogLevel> enabledLevels;
    std::set<std::string> categories;
    std::map<std::string, std::string> metadata;
    std::string defaultModule;
    std::string defaultCategory;
    bool performanceTracking = false;
    bool memoryTracking = false;
    bool threadingInfo = false;
    size_t bufferSize = 1024;
    bool buffered = false;
    size_t fileRotationCount = 5;
    size_t maxFileSizePerFile = 100 * 1024 * 1024;
    bool asyncLogging = false;
    size_t asyncQueueSize = 1000;
    std::string outputFile;
    std::chrono::steady_clock::time_point lastCleanup;
    
    Impl() {
        // Initialize default enabled levels
        enabledLevels.insert(LogLevel::Info);
        enabledLevels.insert(LogLevel::Warning);
        enabledLevels.insert(LogLevel::Error);
        enabledLevels.insert(LogLevel::Critical);
        
        lastCleanup = std::chrono::steady_clock::now();
    }
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {
    initializeDefaultLoggers();
}

Logger::~Logger() = default;

void Logger::initializeDefaultLoggers() {
    // Add a default console logger with standard settings
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    consoleLogger->enableColors(true);
    consoleLogger->setFormat("[TIMESTAMP] [LEVEL] [MODULE] [CATEGORY] MESSAGE");
    addLogger(consoleLogger);
}

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
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
    pImpl->enabledLevels.clear();
    pImpl->enabledLevels.insert(level);
}

void Logger::log(LogLevel level, const std::string& message,
                 const std::string& file, int line,
                 const std::string& function, const std::string& module,
                 const std::string& category) {
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.module = module.empty() ? pImpl->defaultModule : module;
    entry.category = category.empty() ? pImpl->defaultCategory : category;
    
    // Get thread information if enabled
    if (pImpl->threadingInfo) {
        entry.threadId = std::hash<std::thread::id>{}(std::this_thread::get_id());
        entry.threadName = getThreadInfo();
    }
    
    // Set process ID
    entry.processId = getpid();
    
    // Add metadata
    entry.metadata = pImpl->metadata;
    
    updateStatistics(entry);
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        if (logger->isEnabled(level)) {
            logger->log(entry);
        }
    }
}

void Logger::debug(const std::string& message, const std::string& file, int line, const std::string& function) {
    if (isEnabled(LogLevel::Debug)) {
        log(LogLevel::Debug, message, file, line, function);
    }
}

void Logger::info(const std::string& message, const std::string& file, int line, const std::string& function) {
    if (isEnabled(LogLevel::Info)) {
        log(LogLevel::Info, message, file, line, function);
    }
}

void Logger::warning(const std::string& message, const std::string& file, int line, const std::string& function) {
    if (isEnabled(LogLevel::Warning)) {
        log(LogLevel::Warning, message, file, line, function);
    }
}

void Logger::error(const std::string& message, const std::string& file, int line, const std::string& function) {
    if (isEnabled(LogLevel::Error)) {
        log(LogLevel::Error, message, file, line, function);
    }
}

void Logger::critical(const std::string& message, const std::string& file, int line, const std::string& function) {
    if (isEnabled(LogLevel::Critical)) {
        log(LogLevel::Critical, message, file, line, function);
    }
}

void Logger::trace(const std::string& message, const std::string& file, int line, const std::string& function) {
    if (isEnabled(LogLevel::Trace)) {
        log(LogLevel::Trace, message, file, line, function);
    }
}

void Logger::logPerformance(const std::string& operation, double elapsedTime,
                           const std::string& file, int line,
                           const std::string& function) {
    if (pImpl->performanceTracking) {
        log(LogLevel::Performance, "PERFORMANCE: " + operation + " took " + std::to_string(elapsedTime) + "s",
            file, line, function, pImpl->defaultModule, "performance");
    }
}

void Logger::logMemory(const std::string& operation, uint64_t memoryDelta,
                       const std::string& file, int line,
                       const std::string& function) {
    if (pImpl->memoryTracking) {
        log(LogLevel::Memory, "MEMORY: " + operation + " changed by " + std::to_string(memoryDelta) + " bytes",
            file, line, function, pImpl->defaultModule, "memory");
    }
}

void Logger::logDevelopment(const std::string& message, const std::string& file, int line,
                           const std::string& function, const std::string& module) {
    if (isEnabled(LogLevel::Development)) {
        log(LogLevel::Development, "DEV: " + message, file, line, function, module, "development");
    }
}

void Logger::logConfig(const std::string& key, const std::string& value,
                       const std::string& file, int line,
                       const std::string& function) {
    if (isEnabled(LogLevel::Config)) {
        log(LogLevel::Config, "CONFIG: " + key + " = " + value, file, line, function, pImpl->defaultModule, "config");
    }
}

bool Logger::isEnabled(LogLevel level) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->enabledLevels.find(level) != pImpl->enabledLevels.end();
}

Logger& Logger::getGlobal() {
    static Logger globalLogger;
    return globalLogger;
}

void Logger::setGlobal(std::shared_ptr<Logger> logger) {
    // This is a placeholder for advanced global logger management
    // In practice, we just use getGlobal() which is already static
}

std::set<LogLevel> Logger::getLogLevels() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->enabledLevels;
}

std::set<std::string> Logger::getCategories() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::set<std::string> allCategories;
    for (auto& logger : pImpl->loggers) {
        auto loggerCategories = logger->getCategories();
        allCategories.insert(loggerCategories.begin(), loggerCategories.end());
    }
    return allCategories;
}

std::map<std::string, std::string> Logger::getMetadata() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->metadata;
}

std::string Logger::getConfigurationSummary() const {
    std::ostringstream oss;
    oss << "Logger Configuration:\n";
    oss << "  Log Levels: ";
    bool first = true;
    for (auto level : pImpl->enabledLevels) {
        if (!first) oss << ", ";
        oss << static_cast<int>(level);
        first = false;
    }
    oss << "\n";
    oss << "  Categories: ";
    for (const auto& category : getCategories()) {
        oss << category << " ";
    }
    oss << "\n";
    oss << "  Loggers: " << pImpl->loggers.size() << "\n";
    return oss.str();
}

void Logger::rotateLogs() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        if (logger->isFileOutputOpen()) {
            logger->rotateFile();
        }
    }
}

void Logger::cleanupOldLogs() {
    auto now = std::chrono::steady_clock::now();
    auto timeSinceLastCleanup = std::chrono::duration_cast<std::chrono::seconds>(now - pImpl->lastCleanup).count();
    
    if (timeSinceLastCleanup >= 3600) { // Clean up every hour
        // Implement log cleanup logic here
        // This would typically involve deleting old log files based on age/size
        pImpl->lastCleanup = now;
    }
}

bool Logger::exportConfiguration(const std::string& filepath) const {
    // Export logger configuration to a file
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << getConfigurationSummary() << "\n";
    
    // Export categories
    auto categories = getCategories();
    file << "Categories: ";
    for (const auto& category : categories) {
        file << category << " ";
    }
    file << "\n";
    
    // Export metadata
    auto metadata = getMetadata();
    file << "Metadata:\n";
    for (const auto& pair : metadata) {
        file << "  " << pair.first << " = " << pair.second << "\n";
    }
    
    return true;
}

bool Logger::importConfiguration(const std::string& filepath) {
    // Import logger configuration from a file
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Parse configuration lines (simplified implementation)
        if (line.find("Categories:") != std::string::npos) {
            // Parse categories
            size_t pos = line.find(":");
            if (pos != std::string::npos) {
                std::string categoriesStr = line.substr(pos + 1);
                // Parse individual categories (simplified)
            }
        }
    }
    
    return true;
}

uint64_t Logger::getLogCount(LogLevel level) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->logCounts[(int)level];
}

uint64_t Logger::getTotalLogCount() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->totalLogCount;
}

double Logger::getAverageLogSize() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    // This is a simplified calculation
    return 100.0; // Placeholder
}

void Logger::updateStatistics(const LogEntry& entry) {
    pImpl->logCounts[(int)entry.level]++;
    pImpl->totalLogCount++;
}

bool Logger::shouldLogByCategory(const LogEntry& entry) const {
    if (pImpl->categories.empty()) {
        return true;
    }
    return entry.category.empty() || pImpl->categories.find(entry.category) != pImpl->categories.end();
}

bool Logger::shouldLogByMetadata(const LogEntry& entry) const {
    // Check if entry metadata matches required metadata
    for (const auto& pair : pImpl->metadata) {
        if (entry.metadata.find(pair.first) == entry.metadata.end() ||
            entry.metadata.at(pair.first) != pair.second) {
            return false;
        }
    }
    return true;
}

std::string Logger::formatStatistics() const {
    std::ostringstream oss;
    oss << "Log Statistics:\n";
    oss << "  Total Logs: " << getTotalLogCount() << "\n";
    for (int i = 0; i < (int)LogLevel::All; ++i) {
        LogLevel level = static_cast<LogLevel>(i);
        uint64_t count = getLogCount(level);
        if (count > 0) {
            oss << "  " << static_cast<int>(level) << ": " << count << "\n";
        }
    }
    return oss.str();
}

// LogStream implementation
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function,
                    const std::string& module, const std::string& category)
    : level_(level)
    , file_(file)
    , line_(line)
    , function_(function)
    , module_(module)
    , category_(category)
    , elapsedTime_(0.0)
    , memoryDelta_(0)
    , startTime_(std::chrono::system_clock::now()) {}

LogStream::~LogStream() {
    try {
        std::chrono::duration<double> elapsed = std::chrono::system_clock::now() - startTime_;
        elapsedTime_ = elapsed.count();
        
        std::string message = oss_.str();
        if (!message.empty()) {
            Logger::getGlobal().log(level_, message, file_, line_, function_, module_, category_);
        }
    } catch (const std::exception& e) {
        // Ignore exceptions during logging to prevent crashes
        std::cerr << "Error during logging: " << e.what() << std::endl;
    }
}

} // namespace nlm
