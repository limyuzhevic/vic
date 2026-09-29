// Enhanced logger implementation with unified backend
#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <thread>
#include <chrono>
#include <condition_variable>
#include <queue>
#include <atomic>
#include <map>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <filesystem>
#include <algorithm>
#include <cstring>

namespace nlm {

// Helper functions for formatting and escaping
std::string escapeJson(const std::string& input);
std::string escapeXml(const std::string& input);
std::string levelToString(LogLevel level);

// Default formatter implementation
std::string DefaultFormatter::format(const LogEntry& entry) const {
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    oss << entry.getFormattedTimestamp() << " ";
    oss << entry.message;
    
    if (!entry.file.empty()) {
        oss << " [" << entry.file << ":" << entry.line << "]";
    }
    if (!entry.function.empty()) {
        oss << " in " << entry.function;
    }
    
    // Add context
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
    
    return oss.str();
}

// JSON formatter implementation
std::string JSONFormatter::format(const LogEntry& entry) const {
    std::ostringstream oss;
    oss << "{";
    oss << "\"timestamp\": \"" << entry.getTimestamp() << "\", ";
    oss << "\"level\": \"" << levelToString(entry.level) << "\", ";
    oss << "\"message\": \"" << escapeJson(entry.message) << "\", ";
    oss << "\"file\": \"" << escapeJson(entry.file) << "\", ";
    oss << "\"line\": " << entry.line << ", ";
    oss << "\"function\": \"" << escapeJson(entry.function) << "\", ";
    oss << "\"thread_id\": " << entry.threadId << ", ";
    oss << "\"process_id\": " << entry.processId << ", ";
    
    oss << "\"context\": {";
    bool first = true;
    for (const auto& pair : entry.context) {
        if (!first) oss << ", ";
        oss << "\"" << escapeJson(pair.first) << "\": \"" << escapeJson(pair.second) << "\"";
        first = false;
    }
    oss << "}";
    
    oss << "}";
    return oss.str();
}

// XML formatter implementation
std::string XMLFormatter::format(const LogEntry& entry) const {
    std::ostringstream oss;
    oss << "<log>";
    oss << "<timestamp>" << escapeXml(entry.getTimestamp()) << "</timestamp>";
    oss << "<level>" << escapeXml(levelToString(entry.level)) << "</level>";
    oss << "<message>" << escapeXml(entry.message) << "</message>";
    oss << "<file>" << escapeXml(entry.file) << "</file>";
    oss << "<line>" << entry.line << "</line>";
    oss << "<function>" << escapeXml(entry.function) << "</function>";
    oss << "<thread_id>" << entry.threadId << "</thread_id>";
    oss << "<process_id>" << entry.processId << "</process_id>";
    
    if (!entry.context.empty()) {
        oss << "<context>";
        for (const auto& pair : entry.context) {
            oss << "<" << escapeXml(pair.first) << ">" << escapeXml(pair.second) << "</" << escapeXml(pair.first) << ">";
        }
        oss << "</context>";
    }
    
    oss << "</log>";
    return oss.str();
}

// Escape JSON string
std::string escapeJson(const std::string& input) {
    std::ostringstream oss;
    for (char c : input) {
        switch (c) {
            case '\\': oss << "\\\\"; break;
            case '\"': oss << "\\\""; break;
            case '\b': oss << "\\b"; break;
            case '\f': oss << "\\f"; break;
            case '\n': oss << "\\n"; break;
            case '\r': oss << "\\r"; break;
            case '\t': oss << "\\t"; break;
            default:
                if (c < 32 || c == 127) {
                    oss << "\\u" << std::hex << std::setw(4) << std::setfill('0') << (int)(unsigned char)c;
                } else {
                    oss << c;
                }
        }
    }
    return oss.str();
}

// Escape XML string
std::string escapeXml(const std::string& input) {
    std::ostringstream oss;
    for (char c : input) {
        switch (c) {
            case '&': oss << "&amp;"; break;
            case '<': oss << "&lt;"; break;
            case '>': oss << "&gt;"; break;
            case '"': oss << "&quot;"; break;
            case '\'': oss << "&apos;"; break;
            default: oss << c;
        }
    }
    return oss.str();
}

// Helper function to convert LogLevel to string
std::string levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Critical: return "CRIT";
        default: return "UNKNOWN";
    }
}

// Base logger implementation with common functionality
struct BaseLogger::Impl {
    LogLevel level;
    std::shared_ptr<LogFormatter> formatter;
    std::unordered_map<std::string, std::string> context;
    std::mutex contextMutex;
    pid_t threadId;
    pid_t processId;
};

BaseLogger::BaseLogger() : level_(LogLevel::Info), formatter_(std::make_shared<DefaultFormatter>()) {
    pImpl.reset(new Impl);
    pImpl->level = level_;
    pImpl->formatter = formatter_;
    pImpl->threadId = getpid();
    pImpl->processId = getpid();
}

BaseLogger::BaseLogger(LogLevel level) : level_(level), formatter_(std::make_shared<DefaultFormatter>()) {
    pImpl.reset(new Impl);
    pImpl->level = level_;
    pImpl->formatter = formatter_;
    pImpl->threadId = getpid();
    pImpl->processId = getpid();
}

BaseLogger::~BaseLogger() = default;

void BaseLogger::doLog(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    doCustomLog(entry);
}

void BaseLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    // Merge global context with entry context
    LogEntry mergedEntry = entry;
    std::lock_guard<std::mutex> lock(pImpl->contextMutex);
    for (const auto& pair : pImpl->context) {
        mergedEntry.context[pair.first] = pair.second;
    }
    mergedEntry.context.insert(entry.context.begin(), entry.context.end());
    mergedEntry.threadId = pImpl->threadId;
    mergedEntry.processId = pImpl->processId;
    
    doCustomLog(mergedEntry);
}

void BaseLogger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->contextMutex);
    pImpl->level = level;
}

LogLevel BaseLogger::getLevel() const {
    std::lock_guard<std::mutex> lock(pImpl->contextMutex);
    return pImpl->level;
}

void BaseLogger::setFormatter(std::shared_ptr<LogFormatter> formatter) {
    std::lock_guard<std::mutex> lock(pImpl->contextMutex);
    pImpl->formatter = formatter;
}

std::shared_ptr<LogFormatter> BaseLogger::getFormatter() const {
    std::lock_guard<std::mutex> lock(pImpl->contextMutex);
    return pImpl->formatter;
}

void BaseLogger::addContext(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(pImpl->contextMutex);
    pImpl->context[key] = value;
}

void BaseLogger::removeContext(const std::string& key) {
    std::lock_guard<std::mutex> lock(pImpl->contextMutex);
    pImpl->context.erase(key);
}

std::unordered_map<std::string, std::string> BaseLogger::getContext() const {
    std::lock_guard<std::mutex> lock(pImpl->contextMutex);
    return pImpl->context;
}

// ConsoleLogger implementation
struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    uint64_t logCount = 0;
    double totalLogTime = 0.0;
    std::unordered_map<std::string, std::string> context;
    std::shared_ptr<LogFormatter> formatter;
    pid_t threadId = getpid();
    pid_t processId = getpid();
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {
    pImpl->formatter = std::make_shared<DefaultFormatter>();
}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
    pImpl->formatter = std::make_shared<DefaultFormatter>();
}

ConsoleLogger::~ConsoleLogger() = default;

void ConsoleLogger::doCustomLog(const LogEntry& entry) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Update performance metrics
    auto startTime = std::chrono::high_resolution_clock::now();
    pImpl->logCount++;
    
    // Format the log entry
    std::string formatted = pImpl->formatter->format(entry);
    
    // Console output
    if (pImpl->useColors) {
        std::cout << levelToColor(entry.level) << formatted << "\033[0m\n";
    } else {
        std::cout << formatted << "\n";
    }
    
    // File output if set
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut << formatted << "\n";
        pImpl->fileOut.flush();
    }
    
    // Update timing metrics
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(endTime - startTime).count();
    pImpl->totalLogTime += duration;
}

void ConsoleLogger::flush() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.flush();
    }
}

void ConsoleLogger::setUseColors(bool useColors) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->useColors = useColors;
}

bool ConsoleLogger::getUseColors() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->useColors;
}

void ConsoleLogger::setOutputFile(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->fileOut.open(filepath, std::ios::app);
}

void ConsoleLogger::closeOutputFile() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

uint64_t ConsoleLogger::getLogCount() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->logCount;
}

double ConsoleLogger::getAverageLogTime() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->logCount == 0) return 0.0;
    return pImpl->totalLogTime / pImpl->logCount;
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

// Logger implementation with unified backend
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    LogLevel globalLevel;
    bool performanceMonitoring;
    uint64_t totalLogCount;
    double totalLogTime;
    std::unordered_map<std::string, std::string> globalContext;
    std::shared_ptr<LoggerManager> manager;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {
    pImpl->manager = &LoggerManager::getInstance();
    pImpl->globalLevel = LogLevel::Info;
    pImpl->performanceMonitoring = false;
    pImpl->totalLogCount = 0;
    pImpl->totalLogTime = 0.0;
}

Logger::~Logger() = default;

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.push_back(logger);
    logger->setLevel(pImpl->globalLevel);
}

void Logger::addLogger(const std::string& name) {
    auto logger = pImpl->manager->getLogger(name);
    if (logger) {
        addLogger(logger);
    }
}

void Logger::clearLoggers() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.clear();
}

void Logger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->globalLevel = level;
    for (auto& logger : pImpl->loggers) {
        logger->setLevel(level);
    }
}

void Logger::log(LogLevel level, const std::string& message,
                 const std::string& file, int line, const std::string& function) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Check global level filter
    if (level < pImpl->globalLevel) {
        return;
    }
    
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.threadId = getpid();
    entry.processId = getpid();
    
    // Add global context
    for (const auto& pair : pImpl->globalContext) {
        entry.context[pair.first] = pair.second;
    }
    
    // Log to all registered loggers
    for (auto& logger : pImpl->loggers) {
        auto startTime = std::chrono::high_resolution_clock::now();
        logger->log(entry);
        if (pImpl->performanceMonitoring) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double>(endTime - startTime).count();
            pImpl->totalLogTime += duration;
            pImpl->totalLogCount++;
        }
    }
}

template<typename... Args>
void Logger::logFormat(LogLevel level, const std::string& format, Args&&... args) {
    std::ostringstream oss;
    oss << "[" << level << "] " << format;
    log(level, oss.str(), "", 0, "");
}

template<typename... Args>
void Logger::debug(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Debug, message, file, line, function);
}

template<typename... Args>
void Logger::info(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Info, message, file, line, function);
}

template<typename... Args>
void Logger::warning(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Warning, message, file, line, function);
}

template<typename... Args>
void Logger::error(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Error, message, file, line, function);
}

template<typename... Args>
void Logger::critical(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Critical, message, file, line, function);
}

bool Logger::isEnabled(LogLevel level) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return level >= pImpl->globalLevel;
}

Logger& Logger::getGlobal() {
    static Logger globalLogger;
    return globalLogger;
}

void Logger::setGlobal(std::shared_ptr<Logger> logger) {
    // This is a placeholder for advanced global logger management
    // In practice, we just use getGlobal() which is already static
}

void Logger::enablePerformanceMonitoring(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->performanceMonitoring = enable;
}

bool Logger::isPerformanceMonitoringEnabled() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->performanceMonitoring;
}

void Logger::configureLogger(const std::string& name, LogLevel level, 
                            std::shared_ptr<LogFormatter> formatter) {
    auto logger = pImpl->manager->getLogger(name);
    if (logger) {
        logger->setLevel(level);
        if (formatter) {
            logger->setFormatter(formatter);
        }
    }
}

void Logger::addGlobalContext(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->globalContext[key] = value;
}

void Logger::removeGlobalContext(const std::string& key) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->globalContext.erase(key);
}

std::unordered_map<std::string, std::string> Logger::getGlobalContext() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->globalContext;
}

void Logger::rotateAllFileLoggers() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    // Implementation would iterate through all loggers and rotate file loggers
}

void Logger::flushAll() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        logger->flush();
    }
}

// LogStream implementation
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function) {}

LogStream::~LogStream() {
    Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
}

// LoggerManager implementation
struct LoggerManager::Impl {
    std::unordered_map<std::string, std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    LogLevel globalLevel;
    std::unordered_map<std::string, std::string> globalContext;
    uint64_t totalLogCount;
    double totalLogTime;
};

LoggerManager::LoggerManager() : pImpl(std::make_unique<Impl>()) {
    pImpl->globalLevel = LogLevel::Info;
    pImpl->totalLogCount = 0;
    pImpl->totalLogTime = 0.0;
}

LoggerManager::~LoggerManager() = default;

LoggerManager& LoggerManager::getInstance() {
    static LoggerManager instance;
    return instance;
}

void LoggerManager::registerLogger(const std::string& name, std::shared_ptr<ILogger> logger) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers[name] = logger;
    logger->setLevel(pImpl->globalLevel);
}

std::shared_ptr<ILogger> LoggerManager::getLogger(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = pImpl->loggers.find(name);
    if (it != pImpl->loggers.end()) {
        return it->second;
    }
    return nullptr;
}

void LoggerManager::unregisterLogger(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.erase(name);
}

std::vector<std::string> LoggerManager::getLoggerNames() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<std::string> names;
    for (const auto& pair : pImpl->loggers) {
        names.push_back(pair.first);
    }
    return names;
}

void LoggerManager::clearLoggers() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.clear();
}

void LoggerManager::setGlobalLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->globalLevel = level;
    for (auto& pair : pImpl->loggers) {
        pair.second->setLevel(level);
    }
}

LogLevel LoggerManager::getGlobalLevel() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->globalLevel;
}

uint64_t LoggerManager::getTotalLogCount() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->totalLogCount;
}

double LoggerManager::getSystemAverageLogTime() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->totalLogCount == 0) return 0.0;
    return pImpl->totalLogTime / pImpl->totalLogCount;
}

void LoggerManager::addGlobalContext(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->globalContext[key] = value;
}

void LoggerManager::removeGlobalContext(const std::string& key) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->globalContext.erase(key);
}

std::unordered_map<std::string, std::string> LoggerManager::getGlobalContext() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->globalContext;
}

// Logger creation helpers
std::shared_ptr<ConsoleLogger> LoggerManager::createConsoleLogger(const std::string& name) {
    auto logger = std::make_shared<ConsoleLogger>();
    registerLogger(name, logger);
    return logger;
}

std::shared_ptr<FileLogger> LoggerManager::createFileLogger(const std::string& filepath, const std::string& name) {
    auto logger = std::make_shared<FileLogger>(filepath);
    registerLogger(name, logger);
    return logger;
}

std::shared_ptr<AsyncLogger> LoggerManager::createAsyncLogger(size_t queueSize, size_t numThreads, const std::string& name) {
    auto logger = std::make_shared<AsyncLogger>(queueSize, numThreads);
    registerLogger(name, logger);
    return logger;
}

std::shared_ptr<StructuredLogger> LoggerManager::createStructuredLogger(const std::string& name) {
    auto logger = std::make_shared<StructuredLogger>();
    registerLogger(name, logger);
    return logger;
}

std::shared_ptr<MetricsLogger> LoggerManager::createMetricsLogger(const std::string& name) {
    auto logger = std::make_shared<MetricsLogger>();
    registerLogger(name, logger);
    return logger;
}

// Factory functions
namespace LoggerFactory {
    std::shared_ptr<ConsoleLogger> createConsoleLogger(const std::string& name, LogLevel level) {
        return LoggerManager::getInstance().createConsoleLogger(name);
    }
    
    std::shared_ptr<FileLogger> createFileLogger(const std::string& filepath, const std::string& name, LogLevel level) {
        auto logger = std::make_shared<FileLogger>(filepath, level);
        LoggerManager::getInstance().registerLogger(name, logger);
        return logger;
    }
    
    std::shared_ptr<AsyncLogger> createAsyncLogger(size_t queueSize, size_t numThreads, const std::string& name, LogLevel level) {
        auto logger = std::make_shared<AsyncLogger>(level, queueSize, numThreads);
        LoggerManager::getInstance().registerLogger(name, logger);
        return logger;
    }
    
    std::shared_ptr<StructuredLogger> createStructuredLogger(const std::string& name, LogLevel level) {
        auto logger = std::make_shared<StructuredLogger>(level);
        LoggerManager::getInstance().registerLogger(name, logger);
        return logger;
    }
    
    std::shared_ptr<MetricsLogger> createMetricsLogger(const std::string& name, LogLevel level) {
        auto logger = std::make_shared<MetricsLogger>(level);
        LoggerManager::getInstance().registerLogger(name, logger);
        return logger;
    }
}

} // namespace nlm