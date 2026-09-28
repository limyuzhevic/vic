#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <algorithm>
#include <random>
#include <ctime>
#include <memory>
#include <utility>

namespace nlm
{

// Global memory pools for performance
static std::unique_ptr<LogEntryPool> globalEntryPool;
static std::unique_ptr<StringPool> globalStringPool;
static std::mutex poolMutex;

// Global registry for loggers
static std::unordered_map<std::string, std::shared_ptr<Logger>> globalLoggers;
static std::shared_mutex globalLoggersMutex;

// Configuration and control static members
static std::atomic<uint64_t> globalSequenceNumber{0};

//============================================================================
// ConsoleAppender Implementation
//============================================================================

struct ConsoleAppender::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    LogMetrics metrics;
    std::string outputFilepath;
    std::chrono::steady_clock::time_point lastWriteTime;
    size_t writeCount = 0;
};

ConsoleAppender::ConsoleAppender() : pImpl(std::make_unique<Impl>()) {}

ConsoleAppender::~ConsoleAppender() {
    close();
}

void ConsoleAppender::append(const std::string& formattedMessage) {
    auto startTime = std::chrono::steady_clock::now();
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Check if message level is enabled
    // This is handled by the appender's level set by the Logger
    
    // Console output with colors if enabled
    if (pImpl->useColors) {
        // Use color codes based on message content if possible
        // For now, just output without color in append to keep it simple
        std::cout << formattedMessage << "\n";
    } else {
        std::cout << formattedMessage << "\n";
    }
    
    // File output if set
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut << formattedMessage << "\n";
        pImpl->writeCount++;
    }
    
    pImpl->lastWriteTime = std::chrono::steady_clock::now();
    pImpl->metrics.recordLog(std::chrono::steady_clock::now() - startTime);
}

void ConsoleAppender::flush() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::cout << std::flush;
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.flush();
    }
}

void ConsoleAppender::close() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

bool ConsoleAppender::isOpen() const {
    return pImpl->fileOut.is_open();
}

void ConsoleAppender::setFormat(const std::string& format) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    // Format is handled by the formatter, not the appender directly
}

void ConsoleAppender::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->level = level;
}

LogLevel ConsoleAppender::getLevel() const {
    return pImpl->level;
}

void ConsoleAppender::setUseColors(bool useColors) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->useColors = useColors;
}

bool ConsoleAppender::getUseColors() const {
    return pImpl->useColors;
}

void ConsoleAppender::setOutputFile(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
    pImpl->fileOut.open(filepath, std::ios::app);
    pImpl->outputFilepath = filepath;
}

void ConsoleAppender::closeOutputFile() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
        pImpl->outputFilepath.clear();
    }
}

//============================================================================
// Logger Implementation
//============================================================================

struct Logger::Impl {
    std::string name;
    std::vector<std::shared_ptr<LogAppender>> appenders;
    std::shared_ptr<LogFormatter> formatter;
    std::shared_ptr<ILogFilter> filter;
    LogLevel level = LogLevel::Info;
    bool flushOnEachLog = false;
    LogMetrics metrics;
    std::mutex mutex;
    std::atomic<uint64_t> sequenceCounter = 0;
    bool asyncEnabled = false;
    std::shared_ptr<AsyncLogger> asyncLogger;
};

Logger::Logger(const std::string& name) : pImpl(std::make_unique<Impl>()) {
    pImpl->name = name;
    pImpl->formatter = std::make_shared<PatternFormatter>();
    
    // Set up default console appender
    auto consoleAppender = std::make_shared<ConsoleAppender>();
    consoleAppender->setLevel(pImpl->level);
    consoleAppender->setUseColors(true);
    pImpl->appenders.push_back(consoleAppender);
}

Logger::~Logger() {
    // Ensure all logs are flushed before destruction
    for (auto& appender : pImpl->appenders) {
        if (appender) {
            appender->flush();
        }
    }
}

void Logger::setName(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->name = name;
}

std::string Logger::getName() const {
    return pImpl->name;
}

void Logger::log(LogLevel level, const std::string& message,
                 const std::string& file, int line,
                 const std::string& function, const std::string& module) {
    LogEntry entry(level, message, file, line, function);
    entry.module = module;
    entry.sequenceNumber = ++pImpl->sequenceCounter;
    log(entry);
}

void Logger::log(const LogEntry& originalEntry) {
    auto startTime = std::chrono::steady_clock::now();
    
    // Create log entry with pooling if available
    LogEntry entry = originalEntry;
    
    // Check if logging is enabled
    if (entry.level < pImpl->level) {
        return;
    }
    
    // Apply filter if set
    if (pImpl->filter && !pImpl->filter->isEnabled()) {
        return;
    }
    if (pImpl->filter && !pImpl->filter->shouldLog(entry)) {
        return;
    }
    
    // Format the log message
    std::string formattedMessage;
    if (pImpl->formatter) {
        formattedMessage = pImpl->formatter->format(entry);
    } else {
        formattedMessage = entry.message;
    }
    
    // Record metrics
    pImpl->metrics.recordLog(std::chrono::steady_clock::now() - startTime);
    
    // Use async logger if enabled
    if (pImpl->asyncEnabled && pImpl->asyncLogger) {
        pImpl->asyncLogger->log(entry);
    } else {
        // Synchronous logging
        for (auto& appender : pImpl->appenders) {
            if (appender) {
                appender->append(formattedMessage);
            }
        }
        
        if (pImpl->flushOnEachLog) {
            for (auto& appender : pImpl->appenders) {
                if (appender) {
                    appender->flush();
                }
            }
        }
    }
}

template<typename... Args>
void Logger::logStructured(LogLevel level, const std::string& message,
                           const std::string& file, int line,
                           const std::string& function, const std::string& module,
                           const std::unordered_map<std::string, std::string>& fields) {
    LogEntry entry(level, message, file, line, function);
    entry.module = module;
    entry.fields = fields;
    entry.sequenceNumber = ++pImpl->sequenceCounter;
    log(entry);
}

void Logger::logBatch(const std::vector<LogEntry>& entries) {
    if (entries.empty()) return;
    
    auto startTime = std::chrono::steady_clock::now();
    
    // Process batch efficiently
    for (const auto& entry : entries) {
        if (entry.level < pImpl->level) continue;
        
        if (pImpl->filter && !pImpl->filter->isEnabled()) continue;
        if (pImpl->filter && !pImpl->filter->shouldLog(entry)) continue;
        
        std::string formattedMessage;
        if (pImpl->formatter) {
            formattedMessage = pImpl->formatter->format(entry);
        } else {
            formattedMessage = entry.message;
        }
        
        for (auto& appender : pImpl->appenders) {
            if (appender) {
                appender->append(formattedMessage);
            }
        }
    }
    
    pImpl->metrics.recordLog(std::chrono::steady_clock::now() - startTime);
    
    if (pImpl->flushOnEachLog) {
        for (auto& appender : pImpl->appenders) {
            if (appender) {
                appender->flush();
            }
        }
    }
}

void Logger::debug(const std::string& message, const std::string& file, int line, const std::string& function, const std::string& module) {
    log(LogLevel::Debug, message, file, line, function, module);
}

void Logger::info(const std::string& message, const std::string& file, int line, const std::string& function, const std::string& module) {
    log(LogLevel::Info, message, file, line, function, module);
}

void Logger::warning(const std::string& message, const std::string& file, int line, const std::string& function, const std::string& module) {
    log(LogLevel::Warning, message, file, line, function, module);
}

void Logger::error(const std::string& message, const std::string& file, int line, const std::string& function, const std::string& module) {
    log(LogLevel::Error, message, file, line, function, module);
}

void Logger::critical(const std::string& message, const std::string& file, int line, const std::string& function, const std::string& module) {
    log(LogLevel::Critical, message, file, line, function, module);
}

void Logger::setPattern(const std::string& pattern) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->formatter) {
        pImpl->formatter->setPattern(pattern);
    }
}

std::string Logger::getPattern() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->formatter ? pImpl->formatter->getPattern() : "";
}

void Logger::setFormatter(std::shared_ptr<LogFormatter> formatter) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->formatter = std::move(formatter);
}

std::shared_ptr<LogFormatter> Logger::getFormatter() const {
    return pImpl->formatter;
}

void Logger::setFilter(std::shared_ptr<ILogFilter> filter) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->filter = std::move(filter);
}

std::shared_ptr<ILogFilter> Logger::getFilter() const {
    return pImpl->filter;
}

void Logger::addAppender(std::shared_ptr<LogAppender> appender) {
    if (!appender) return;
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->appenders.push_back(std::move(appender));
}

void Logger::removeAppender(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->appenders.erase(std::remove_if(pImpl->appenders.begin(), pImpl->appenders.end(),
        [&name](const std::shared_ptr<LogAppender>& appender) {
            // Assuming appender has a getName method
            return true; // Placeholder - would need name comparison
        }), pImpl->appenders.end());
}

void Logger::clearAppenders() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->appenders.clear();
}

void Logger::setAppenders(const std::vector<std::shared_ptr<LogAppender>>& appenders) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->appenders = appenders;
}

void Logger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->level = level;
    for (auto& appender : pImpl->appenders) {
        if (appender) {
            appender->setLevel(level);
        }
    }
}

LogLevel Logger::getLevel() const {
    return pImpl->level;
}

void Logger::setFlushOnEachLog(bool flush) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->flushOnEachLog = flush;
}

bool Logger::getFlushOnEachLog() const {
    return pImpl->flushOnEachLog;
}

bool Logger::isEnabled(LogLevel level) const {
    return level >= pImpl->level;
}

LogMetrics Logger::getMetrics() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->metrics;
}

void Logger::resetMetrics() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->metrics.logCount = 0;
    pImpl->metrics.totalLatencyNanos = 0;
    pImpl->metrics.startTime = std::chrono::steady_clock::now();
    pImpl->metrics.lastLogTime = std::chrono::steady_clock::time_point();
}

Logger& Logger::getGlobal(const std::string& name) {
    std::shared_lock<std::shared_mutex> lock(globalLoggersMutex);
    auto it = globalLoggers.find(name);
    if (it != globalLoggers.end()) {
        return *(it->second.get());
    }
    lock.unlock();
    
    std::unique_lock<std::shared_mutex> uniqueLock(globalLoggersMutex);
    it = globalLoggers.find(name);
    if (it == globalLoggers.end()) {
        globalLoggers[name] = std::make_shared<Logger>(name);
        it = globalLoggers.find(name);
    }
    uniqueLock.unlock();
    
    return *(it->second.get());
}

void Logger::setGlobal(const std::string& name, std::shared_ptr<Logger> logger) {
    if (!logger) return;
    std::unique_lock<std::shared_mutex> lock(globalLoggersMutex);
    globalLoggers[name] = std::move(logger);
}

void Logger::clearGlobal(const std::string& name) {
    std::unique_lock<std::shared_mutex> lock(globalLoggersMutex);
    globalLoggers.erase(name);
}

// LogStream implementation
Logger::LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function) {}

Logger::LogStream::~LogStream() {
    // Create log entry from stream contents
    LogEntry entry(level_, oss_.str(), file_, line_, function_);
    entry.module = module_;
    entry.fields = fields_;
    entry.tags = tags_;
    
    // Get the appropriate logger (use global for simplicity)
    Logger::getGlobal().log(entry);
}

Logger::LogStream& Logger::LogStream::addField(const std::string& key, const std::string& value) {
    fields_[key] = value;
    return *this;
}

Logger::LogStream& Logger::LogStream::addTag(const std::string& tag) {
    tags_.insert(tag);
    return *this;
}

Logger::LogStream& Logger::LogStream::setModule(const std::string& module) {
    module_ = module;
    return *this;
}

// Static method implementations
void Logger::cleanupPools() {
    std::lock_guard<std::mutex> lock(poolMutex);
    globalEntryPool.reset();
    globalStringPool.reset();
}

size_t Logger::getEntryPoolSize() {
    std::lock_guard<std::mutex> lock(poolMutex);
    return globalEntryPool ? globalEntryPool->size() : 0;
}

size_t Logger::getStringPoolSize() {
    std::lock_guard<std::mutex> lock(poolMutex);
    return globalStringPool ? globalStringPool->size() : 0;
}

std::shared_ptr<Logger> Logger::fromConfig(const std::string& configFile) {
    // Simple implementation - would be more sophisticated in real use
    auto logger = std::make_shared<Logger>("config_logger");
    
    // Parse configuration file and configure logger
    // This is a placeholder implementation
    
    return logger;
}

//============================================================================
// AsyncLogger Implementation
//============================================================================

struct AsyncLogger::Impl {
    std::vector<std::thread> workers;
    std::deque<LogEntry> messageQueue;
    std::mutex queueMutex;
    std::condition_variable cv;
    std::atomic<bool> shutdown{false};
    std::atomic<size_t> processedCount{0};
    LogLevel level = LogLevel::Info;
    std::string pattern = "%timestamp %level %message";
    LogMetrics metrics;
    std::thread::id mainThreadId;
};

AsyncLogger::AsyncLogger(size_t queueSize, size_t workerThreads) : pImpl(std::make_unique<Impl>()) {
    pImpl->mainThreadId = std::this_thread::get_id();
    
    for (size_t i = 0; i < workerThreads; ++i) {
        pImpl->workers.emplace_back([this]() {
            workerThread();
        });
    }
}

AsyncLogger::~AsyncLogger() {
    shutdown();
    for (auto& worker : pImpl->workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

void AsyncLogger::workerThread() {
    while (!pImpl->shutdown) {
        std::unique_lock<std::mutex> lock(pImpl->queueMutex);
        
        if (pImpl->messageQueue.empty()) {
            // Wait for messages or shutdown
            auto timeout = std::chrono::milliseconds(100);
            if (cv.wait_for(lock, timeout, [this]() { return pImpl->shutdown; })) {
                break;
            }
            continue;
        }
        
        // Process message
        LogEntry entry = std::move(pImpl->messageQueue.front());
        pImpl->messageQueue.pop_front();
        
        // Log the entry (using synchronous logging for now)
        // In a real implementation, this would use appenders
        
        lock.unlock();
        pImpl->processedCount++;
    }
}

void AsyncLogger::log(const LogEntry& entry) {
    if (pImpl->shutdown || entry.level < pImpl->level) return;
    
    auto startTime = std::chrono::steady_clock::now();
    
    {
        std::lock_guard<std::mutex> lock(pImpl->queueMutex);
        pImpl->messageQueue.push_back(entry);
        pImpl->metrics.recordLog(std::chrono::steady_clock::now() - startTime);
    }
    
    pImpl->cv.notify_one();
}

void AsyncLogger::logBatch(const std::vector<LogEntry>& entries) {
    if (pImpl->shutdown) return;
    
    auto startTime = std::chrono::steady_clock::now();
    
    {
        std::lock_guard<std::mutex> lock(pImpl->queueMutex);
        for (const auto& entry : entries) {
            if (entry.level >= pImpl->level) {
                pImpl->messageQueue.push_back(entry);
            }
        }
        
        pImpl->metrics.recordLog(std::chrono::steady_clock::now() - startTime);
    }
    
    pImpl->cv.notify_all();
}

void AsyncLogger::shutdown() {
    {
        std::lock_guard<std::mutex> lock(pImpl->queueMutex);
        pImpl->shutdown = true;
    }
    
    pImpl->cv.notify_all();
}

bool AsyncLogger::isShutdown() const {
    return pImpl->shutdown;
}

void AsyncLogger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->queueMutex);
    pImpl->level = level;
}

LogLevel AsyncLogger::getLevel() const {
    return pImpl->level;
}

void AsyncLogger::setPattern(const std::string& pattern) {
    std::lock_guard<std::mutex> lock(pImpl->queueMutex);
    pImpl->pattern = pattern;
}

std::string AsyncLogger::getPattern() const {
    return pImpl->pattern;
}

uint64_t AsyncLogger::getQueueSize() const {
    std::lock_guard<std::mutex> lock(pImpl->queueMutex);
    return pImpl->messageQueue.size();
}

uint6464_t AsyncLogger::getProcessedCount() const {
    return pImpl->processedCount;
}

double AsyncLogger::getAverageLatency() const {
    return pImpl->metrics.getAverageLatency();
}

//============================================================================
// Global Logger Management
//============================================================================

Logger& getGlobalLogger(const std::string& name) {
    return Logger::getGlobal(name);
}

void setGlobalLogger(const std::string& name, std::shared_ptr<Logger> logger) {
    Logger::setGlobal(name, std::move(logger));
}

void clearGlobalLogger(const std::string& name) {
    Logger::clearGlobal(name);
}

} // namespace nlm
