#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <algorithm>
#include <unordered_set>
#include <ctime>
#include <iomanip>
#include <filesystem>

namespace nlm {

// ConsoleLogger implementation
struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::string colorScheme = "default";
    std::ofstream fileOut;
    std::mutex mutex;
    uint64_t maxFileSize = 10 * 1024 * 1024;  // 10MB
    std::string filepath;
    
    // Statistics
    LogStatistics stats;
    std::chrono::system_clock::time_point lastRotation;
    
    // Async logging
    std::mutex asyncMutex;
    std::deque<LogEntry> asyncQueue;
    bool asyncEnabled = false;
    std::thread asyncWorker;
    std::atomic<bool> stopAsyncWorker{false};
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {
    updateStatsInitial();
    startAsyncWorker();
}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
    updateStatsInitial();
    startAsyncWorker();
}

ConsoleLogger::~ConsoleLogger() {
    stopAsyncWorker = true;
    if (pImpl->asyncWorker.joinable()) {
        pImpl->asyncWorker.join();
    }
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

void ConsoleLogger::log(const LogEntry& entry) {
    if (!shouldLog(entry)) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::string formattedMessage = formatMessage(entry);
    
    // Update statistics
    pImpl->stats.totalLogs++;
    pImpl->stats.logsByLevel[static_cast<int>(entry.level)]++;
    pImpl->stats.lastLog = std::chrono::system_clock::now();
    
    // Console output
    if (pImpl->useColors) {
        std::cout << levelToColor(entry.level, pImpl->colorScheme) << formattedMessage << "\033[0m\n";
    } else {
        std::cout << formattedMessage << "\n";
    }
    
    // File output
    writeToFile(formattedMessage);
}

void ConsoleLogger::logAsync(const LogEntry& entry) {
    if (!asyncEnabled) {
        log(entry);
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->asyncMutex);
    pImpl->asyncQueue.push_back(entry);
}

void ConsoleLogger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->level = level;
}

LogLevel ConsoleLogger::getLevel() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->level;
}

void ConsoleLogger::setFormat(LogFormat format) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->format = format;
}

void ConsoleLogger::setFilter(const LogFilter& filter) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->filter = filter;
}

void ConsoleLogger::setPattern(const std::string& pattern) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->pattern = pattern;
}

void ConsoleLogger::setUseColors(bool useColors) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->useColors = useColors;
}

bool ConsoleLogger::getUseColors() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->useColors;
}

void ConsoleLogger::setColorScheme(const std::string& scheme) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->colorScheme = scheme;
}

void ConsoleLogger::setOutputFile(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->filepath = filepath;
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
    pImpl->fileOut.open(filepath, std::ios::app);
    if (pImpl->fileOut.is_open()) {
        pImpl->lastRotation = std::chrono::system_clock::now();
    }
}

void ConsoleLogger::setRotationSize(uint64_t bytes) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->maxFileSize = bytes;
}

void ConsoleLogger::rotateLog() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
        std::filesystem::path path(pImpl->filepath);
        std::string rotatedPath = pImpl->filepath + "." + 
            std::to_string(std::chrono::system_clock::now().time_since_epoch()) + ".bak";
        std::filesystem::rename(pImpl->filepath, rotatedPath);
        pImpl->fileOut.open(pImpl->filepath, std::ios::app);
        if (pImpl->fileOut.is_open()) {
            pImpl->lastRotation = std::chrono::system_clock::now();
        }
    }
}

void ConsoleLogger::closeOutputFile() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

LogStatistics ConsoleLogger::getStatistics() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->stats;
}

void ConsoleLogger::resetStatistics() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    updateStatsInitial();
}

bool ConsoleLogger::isAsync() const {
    return asyncEnabled;
}

void ConsoleLogger::startAsyncWorker() {
    asyncEnabled = true;
    stopAsyncWorker = false;
    
    pImpl->asyncWorker = std::thread([this]() {
        while (!stopAsyncWorker.load()) {
            std::deque<LogEntry> queue;
            {
                std::lock_guard<std::mutex> lock(pImpl->asyncMutex);
                queue = std::move(pImpl->asyncQueue);
            }
            
            for (const auto& entry : queue) {
                // Process async log entry
                log(entry);
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    });
}

std::string ConsoleLogger::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Critical: return "CRIT";
        case LogLevel::Fatal: return "FATAL";
        default: return "UNKNOWN";
    }
}

std::string ConsoleLogger::levelToColor(LogLevel level, const std::string& scheme) const {
    switch (level) {
        case LogLevel::Debug: 
            return (scheme == "dark") ? "\033[36m" : "\033[36m";  // Cyan
        case LogLevel::Info: 
            return (scheme == "dark") ? "\033[32m" : "\033[32m";    // Green
        case LogLevel::Warning: 
            return (scheme == "dark") ? "\033[33m" : "\033[93m";   // Yellow/Dark yellow
        case LogLevel::Error: 
            return (scheme == "dark") ? "\033[31m" : "\033[91m";   // Red/Bright red
        case LogLevel::Critical: 
            return (scheme == "dark") ? "\033[35m" : "\033[95m"; // Magenta/Bright magenta
        case LogLevel::Fatal: 
            return (scheme == "dark") ? "\033[41m" : "\033[101m";  // Red background/Bright red background
        default: return "\033[0m";
    }
}

std::string ConsoleLogger::formatMessage(const LogEntry& entry) const {
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    
    // Add timestamp
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    
    // Add file and line info
    if (!entry.file.empty()) {
        oss << " " << entry.file;
        if (entry.line > 0) {
            oss << ":" << entry.line;
        }
    }
    
    // Add function info
    if (!entry.function.empty()) {
        oss << " " << entry.function;
    }
    
    // Add thread ID
    if (!entry.threadId.empty()) {
        oss << " [" << entry.threadId << "]";
    }
    
    // Add correlation ID
    if (entry.correlationId != 0) {
        oss << " [corr:" << entry.correlationId << "]";
    }
    
    oss << " " << entry.message;
    
    // Add error details
    if (!entry.errorCode.empty()) {
        oss << " [Error: " << entry.errorCode << "]";
    }
    if (!entry.errorCategory.empty()) {
        oss << " [Category: " << entry.errorCategory << "]";
    }
    
    // Add stack trace
    if (!entry.stackTrace.empty()) {
        oss << "\nStack trace:\n" << entry.stackTrace;
    }
    
    // Add context
    if (!entry.context.empty()) {
        oss << "\nContext:";
        for (const auto& pair : entry.context) {
            oss << " " << pair.first << "=" << pair.second;
        }
    }
    
    return oss.str();
}

bool ConsoleLogger::shouldLog(const LogEntry& entry) const {
    // Check level
    if (entry.level < pImpl->level) {
        return false;
    }
    
    // Check module filter
    if (!pImpl->filter.includeModules.empty() && 
        std::find(pImpl->filter.includeModules.begin(), pImpl->filter.includeModules.end(), entry.file) == pImpl->filter.includeModules.end()) {
        return false;
    }
    
    // Check exclude modules
    if (!pImpl->filter.excludeModules.empty() && 
        std::find(pImpl->filter.excludeModules.begin(), pImpl->filter.excludeModules.end(), entry.file) != pImpl->filter.excludeModules.end()) {
        return false;
    }
    
    // Check keywords
    std::string lowerMessage = entry.message;
    std::transform(lowerMessage.begin(), lowerMessage.end(), lowerMessage.begin(), ::tolower);
    
    bool matchesKeyword = true;
    if (!pImpl->filter.includeKeywords.empty()) {
        matchesKeyword = false;
        for (const auto& keyword : pImpl->filter.includeKeywords) {
            std::string lowerKeyword = keyword;
            std::transform(lowerKeyword.begin(), lowerKeyword.end(), lowerKeyword.begin(), ::tolower);
            if (lowerMessage.find(lowerKeyword) != std::string::npos) {
                matchesKeyword = true;
                break;
            }
        }
    }
    
    if (pImpl->filter.matchAllKeywords) {
        if (!matchesKeyword) {
            return false;
        }
    }
    
    // Check exclude keywords
    for (const auto& keyword : pImpl->filter.excludeKeywords) {
        std::string lowerKeyword = keyword;
        std::transform(lowerKeyword.begin(), lowerKeyword.end(), lowerKeyword.begin(), ::tolower);
        if (lowerMessage.find(lowerKeyword) != std::string::npos) {
            return false;
        }
    }
    
    return true;
}

void ConsoleLogger::writeToFile(const std::string& message) {
    if (pImpl->fileOut.is_open()) {
        // Check file size and rotate if needed
        pImpl->fileOut.seekp(0, std::ios::end);
        if (pImpl->fileOut.tellp() > static_cast<std::streampos>(pImpl->maxFileSize)) {
            rotateLog();
        }
        
        pImpl->fileOut << message << "\n";
        pImpl->fileOut.flush();
    }
}

void ConsoleLogger::updateStatsInitial() {
    pImpl->stats.totalLogs = 0;
    for (int i = 0; i < 6; ++i) {
        pImpl->stats.logsByLevel[i] = 0;
    }
    pImpl->stats.logsPerMinute = 0;
    pImpl->stats.avgMessageLength = 0.0;
    pImpl->stats.firstLog = std::chrono::system_clock::now();
    pImpl->stats.lastLog = std::chrono::system_clock::now();
    pImpl->stats.mostFrequentModule = "";
    pImpl->stats.mostFrequentKeyword = "";
}

// FileLogger implementation
struct FileLogger::Impl {
    LogLevel level = LogLevel::Info;
    std::ofstream fileOut;
    std::mutex mutex;
    std::string filepath;
    uint64_t maxFileSize = 10 * 1024 * 1024;  // 10MB
    std::chrono::system_clock::time_point lastRotation;
    LogStatistics stats;
};

FileLogger::FileLogger(const std::string& filepath) : pImpl(std::make_unique<Impl>()) {
    pImpl->filepath = filepath;
    pImpl->fileOut.open(filepath, std::ios::app);
    if (pImpl->fileOut.is_open()) {
        pImpl->lastRotation = std::chrono::system_clock::now();
    }
}

FileLogger::~FileLogger() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

void FileLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    oss << " " << entry.message << "\n";
    
    if (pImpl->fileOut.is_open()) {
        // Check file size
        pImpl->fileOut.seekp(0, std::ios::end);
        if (pImpl->fileOut.tellp() > static_cast<std::streampos>(pImpl->maxFileSize)) {
            rotateLog();
        }
        
        pImpl->fileOut << oss.str();
        pImpl->fileOut.flush();
    }
}

void FileLogger::logAsync(const LogEntry& entry) {
    log(entry);
}

void FileLogger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->level = level;
}

LogLevel FileLogger::getLevel() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->level;
}

void FileLogger::setFormat(LogFormat format) {
    // File logger doesn't support different formats for now
}

void FileLogger::setFilter(const LogFilter& filter) {
    // File logger doesn't support filtering for now
}

void FileLogger::setPattern(const std::string& pattern) {
    // File logger doesn't support patterns for now
}

void FileLogger::setOutputFile(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->filepath = filepath;
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
    pImpl->fileOut.open(filepath, std::ios::app);
    if (pImpl->fileOut.is_open()) {
        pImpl->lastRotation = std::chrono::system_clock::now();
    }
}

void FileLogger::setRotationSize(uint64_t bytes) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->maxFileSize = bytes;
}

void FileLogger::rotateLog() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
        std::filesystem::path path(pImpl->filepath);
        std::string rotatedPath = pImpl->filepath + "." + 
            std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + ".bak";
        std::filesystem::rename(pImpl->filepath, rotatedPath);
        pImpl->fileOut.open(pImpl->filepath, std::ios::app);
        if (pImpl->fileOut.is_open()) {
            pImpl->lastRotation = std::chrono::system_clock::now();
        }
    }
}

void FileLogger::closeOutputFile() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

LogStatistics FileLogger::getStatistics() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->stats;
}

void FileLogger::resetStatistics() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->stats.totalLogs = 0;
    for (int i = 0; i < 6; ++i) {
        pImpl->stats.logsByLevel[i] = 0;
    }
}

bool FileLogger::isAsync() const {
    return false;
}

std::string FileLogger::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Critical: return "CRIT";
        case LogLevel::Fatal: return "FATAL";
        default: return "UNKNOWN";
    }
}

// Logger implementation
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    
    // Global statistics
    LogStatistics globalStats;
    
    // Configuration
    LogFilter globalFilter;
    std::string globalPattern;
    
    // Performance tracking
    std::unordered_map<std::string, std::chrono::system_clock::time_point> activeTimedLogs;
    std::mutex timedLogsMutex;
    
    // Health monitoring
    std::chrono::system_clock::time_point lastLogTime;
    bool healthy = true;
    std::vector<std::string> healthIssues;
    std::mutex healthMutex;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {
    updateStatsInitial();
    pImpl->lastLogTime = std::chrono::system_clock::now();
}

Logger::~Logger() = default;

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.push_back(logger);
}

void Logger::removeLogger(ILogger* logger) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.erase(
        std::remove_if(pImpl->loggers.begin(), pImpl->loggers.end(),
            [logger](const std::shared_ptr<ILogger>& l) { return l.get() == logger; }),
        pImpl->loggers.end()
    );
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

void Logger::setGlobalFilter(const LogFilter& filter) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->globalFilter = filter;
}

void Logger::setGlobalPattern(const std::string& pattern) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->globalPattern = pattern;
}

void Logger::log(LogLevel level, const std::string& message,
                 const std::string& file, int line, const std::string& function,
                 const std::string& errorCode, const std::string& errorCategory) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.threadId = std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    entry.correlationId = ++correlationIdCounter;
    entry.errorCode = errorCode;
    entry.errorCategory = errorCategory;
    
    // Capture stack trace for error levels
    if (level >= LogLevel::Error) {
        try {
            entry.stackTrace = std::stacktrace::current().to_string();
        } catch (...) {
            entry.stackTrace = "Unable to capture stack trace";
        }
    }
    
    // Update global statistics
    pImpl->globalStats.totalLogs++;
    pImpl->globalStats.logsByLevel[static_cast<int>(entry.level)]++;
    pImpl->globalStats.lastLog = entry.timestamp;
    
    // Calculate average message length
    pImpl->globalStats.avgMessageLength = 
        (pImpl->globalStats.avgMessageLength * (pImpl->globalStats.totalLogs - 1) + message.length()) / 
        pImpl->globalStats.totalLogs;
    
    // Update health status
    pImpl->healthy = true;
    pImpl->healthIssues.clear();
    
    if (pImpl->loggers.empty()) {
        pImpl->healthy = false;
        pImpl->healthIssues.push_back("No loggers configured");
    }
    
    for (auto& logger : pImpl->loggers) {
        // Apply global filter if logger supports it
        LogFilter effectiveFilter = pImpl->globalFilter;
        
        // Forward to all loggers
        if (logger) {
            logger->log(entry);
        }
    }
}

void Logger::logError(const std::string& message, const std::string& file,
                      int line, const std::string& function,
                      const std::string& errorCode, const std::string& errorCategory,
                      std::exception_ptr exception) {
    std::string enhancedMessage = message;
    
    if (exception) {
        try {
            std::rethrow_exception(exception);
        } catch (const std::exception& e) {
            enhancedMessage += "\nException: " + std::string(e.what());
        } catch (...) {
            enhancedMessage += "\nException: Unknown exception";
        }
    }
    
    log(LogLevel::Error, enhancedMessage, file, line, function, errorCode, errorCategory);
}

void Logger::logWithContext(LogLevel level, const std::string& message,
                            const std::unordered_map<std::string, std::string>& context,
                            const std::string& file, int line, const std::string& function) {
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.threadId = std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    entry.correlationId = ++correlationIdCounter;
    entry.context = context;
    
    log(level, message, file, line, function);
}

void Logger::startTimedLog(const std::string& operation) {
    std::lock_guard<std::mutex> lock(pImpl->timedLogsMutex);
    pImpl->activeTimedLogs[operation] = std::chrono::system_clock::now();
}

void Logger::endTimedLog(const std::string& operation, LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->timedLogsMutex);
    
    auto it = pImpl->activeTimedLogs.find(operation);
    if (it != pImpl->activeTimedLogs.end()) {
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::system_clock::now() - it->second);
        
        log(level, "Operation '" + operation + "' completed in " + 
            std::to_string(duration.count()) + " microseconds", "", 0, "");
        
        pImpl->activeTimedLogs.erase(it);
    }
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

void Logger::fatal(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Fatal, message, file, line, function);
}

bool Logger::isEnabled(LogLevel level, const std::string& module) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (const auto& logger : pImpl->loggers) {
        if (logger->getLevel() <= level) {
            return true;
        }
    }
    
    return false;
}

LogStatistics Logger::getGlobalStatistics() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->globalStats;
}

void Logger::resetGlobalStatistics() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    updateStatsInitial();
}

std::string Logger::getLogReport() const {
    std::ostringstream oss;
    oss << "=== Logger Statistics ===\n";
    oss << "Total logs: " << pImpl->globalStats.totalLogs << "\n";
    oss << "First log: " << std::chrono::system_clock::to_time_t(pImpl->globalStats.firstLog) << "\n";
    oss << "Last log: " << std::chrono::system_clock::to_time_t(pImpl->globalStats.lastLog) << "\n";
    oss << "Average message length: " << pImpl->globalStats.avgMessageLength << "\n";
    
    oss << "\nLogs by level:\n";
    oss << "  Debug: " << pImpl->globalStats.logsByLevel[static_cast<int>(LogLevel::Debug)] << "\n";
    oss << "  Info: " << pImpl->globalStats.logsByLevel[static_cast<int>(LogLevel::Info)] << "\n";
    oss << "  Warning: " << pImpl->globalStats.logsByLevel[static_cast<int>(LogLevel::Warning)] << "\n";
    oss << "  Error: " << pImpl->globalStats.logsByLevel[static_cast<int>(LogLevel::Error)] << "\n";
    oss << "  Critical: " << pImpl->globalStats.logsByLevel[static_cast<int>(LogLevel::Critical)] << "\n";
    oss << "  Fatal: " << pImpl->globalStats.logsByLevel[static_cast<int>(LogLevel::Fatal)] << "\n";
    
    oss << "\n=== End Statistics ===";
    return oss.str();
}

Logger& Logger::getGlobal() {
    static Logger logger;
    return logger;
}

void Logger::setGlobal(std::shared_ptr<Logger> logger) {
    if (logger) {
        static Logger* globalLogger = nullptr;
        if (globalLogger) {
            delete globalLogger;
        }
        globalLogger = logger.release();
    }
}

void Logger::configureGlobalLogger(const std::string& configFile) {
    // Configuration loading would go here
    // For now, just create a basic console logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    setGlobal(logger);
}

bool Logger::isHealthy() const {
    std::lock_guard<std::mutex> lock(pImpl->healthMutex);
    return pImpl->healthy;
}

std::vector<std::string> Logger::getHealthIssues() const {
    std::lock_guard<std::mutex> lock(pImpl->healthMutex);
    return pImpl->healthIssues;
}

void Logger::updateStatsInitial() {
    pImpl->globalStats.totalLogs = 0;
    for (int i = 0; i < 6; ++i) {
        pImpl->globalStats.logsByLevel[i] = 0;
    }
    pImpl->globalStats.logsPerMinute = 0;
    pImpl->globalStats.avgMessageLength = 0.0;
    pImpl->globalStats.firstLog = std::chrono::system_clock::now();
    pImpl->globalStats.lastLog = std::chrono::system_clock::now();
    pImpl->globalStats.mostFrequentModule = "";
    pImpl->globalStats.mostFrequentKeyword = "";
}

// LogStream implementation
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function,
                   const std::string& operation)
    : level_(level), file_(file), line_(line), function_(function), operation_(operation),
      correlationId_(++correlationIdCounter) {
}

LogStream::~LogStream() {
    Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
}

LogStream& LogStream::withContext(const std::string& key, const std::string& value) {
    context_[key] = value;
    return *this;
}

LogStream& LogStream::withCorrelationId(uint64_t id) {
    correlationId_ = id;
    return *this;
}

LogStream& LogStream::withException(std::exception_ptr ex) {
    try {
        if (ex) {
            std::rethrow_exception(ex);
        }
    } catch (const std::exception& e) {
        oss_ << "\nException: " << e.what();
    } catch (...) {
        oss_ << "\nException: Unknown exception";
    }
    return *this;
}

// Global correlation ID counter
std::atomic<uint64_t> Logger::correlationIdCounter{0};

} // namespace nlm