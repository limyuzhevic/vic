#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <thread>
#include <chrono>
#include <filesystem>
#include <algorithm>
#include <functional>
#include <queue>
#include <condition_variable>

namespace nlm {

// AsyncLogger implementation
struct AsyncLogger::Impl {
    LogLevel level = LogLevel::Info;
    std::shared_ptr<IFormatter> formatter = std::make_shared<KeyValueFormatter>();
    std::queue<LogEntry> logQueue;
    std::vector<std::thread> workerThreads;
    std::mutex queueMutex;
    std::condition_variable queueCondVar;
    std::atomic<bool> running{true};
    std::atomic<size_t> processedCount{0};
    std::atomic<size_t> queuedCount{0};
};

AsyncLogger::AsyncLogger(size_t queueSize, size_t workerThreads) : pImpl(std::make_unique<Impl>()) {
    for (size_t i = 0; i < workerThreads; ++i) {
        pImpl->workerThreads.emplace_back([this]() {
            while (pImpl->running) {
                std::unique_lock<std::mutex> lock(pImpl->queueMutex);
                if (pImpl->logQueue.empty()) {
                    lock.unlock();
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    continue;
                }
                
                LogEntry entry = std::move(pImpl->logQueue.front());
                pImpl->logQueue.pop();
                lock.unlock();
                
                // Process the log entry
                pImpl->formatter->format(entry); // In real implementation, would actually log
                pImpl->processedCount++;
            }
        });
    }
}

AsyncLogger::~AsyncLogger() {
    pImpl->running = false;
    pImpl->queueCondVar.notify_all();
    for (auto& thread : pImpl->workerThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

void AsyncLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->queueMutex);
    pImpl->logQueue.push(entry);
    pImpl->queuedCount++;
}

void AsyncLogger::setLevel(LogLevel level) {
    pImpl->level = level;
}

LogLevel AsyncLogger::getLevel() const {
    return pImpl->level;
}

void AsyncLogger::setFormatter(std::shared_ptr<IFormatter> formatter) {
    pImpl->formatter = formatter;
}

std::shared_ptr<IFormatter> AsyncLogger::getFormatter() const {
    return pImpl->formatter;
}

// CategoryLogger implementation
CategoryLogger::CategoryLogger(const std::string& category, std::shared_ptr<ILogger> baseLogger)
    : category_(category), baseLogger_(baseLogger) {}

void CategoryLogger::log(const LogEntry& entry) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Check if this category should be logged
    auto it = categoryFilters_.find(category_);
    if (it != categoryFilters_.end() && entry.level < it->second) {
        return;
    }
    
    // Add category to entry if not already set
    LogEntry filteredEntry = entry;
    if (filteredEntry.category.empty()) {
        filteredEntry.category = category_;
    }
    
    baseLogger_->log(filteredEntry);
}

void CategoryLogger::setLevel(LogLevel level) {
    baseLogger_->setLevel(level);
}

LogLevel CategoryLogger::getLevel() const {
    return baseLogger_->getLevel();
}

void CategoryLogger::setFormatter(std::shared_ptr<IFormatter> formatter) {
    baseLogger_->setFormatter(formatter);
}

std::shared_ptr<IFormatter> CategoryLogger::getFormatter() const {
    return baseLogger_->getFormatter();
}

void CategoryLogger::addCategoryFilter(const std::string& category, LogLevel level) {
    std::lock_guard<std::mutex> lock(mutex_);
    categoryFilters_[category] = level;
}

void CategoryLogger::removeCategoryFilter(const std::string& category) {
    std::lock_guard<std::mutex> lock(mutex_);
    categoryFilters_.erase(category);
}

void CategoryLogger::setCategoryFilter(const std::unordered_map<std::string, LogLevel>& filters) {
    std::lock_guard<std::mutex> lock(mutex_);
    categoryFilters_ = filters;
}

// LogRotation implementation
LogRotation::LogRotation(const std::string& baseFilename, size_t maxFiles, uint64_t maxSize)
    : baseFilename_(baseFilename), maxFiles_(maxFiles), maxSize_(maxSize) {
    if (!baseFilename.empty()) {
        currentFile_ = baseFilename;
        if (std::filesystem::exists(currentFile_)) {
            currentSize_ = std::filesystem::file_size(currentFile_);
        }
    }
}

LogRotation::~LogRotation() {
    if (!currentFile_.empty() && fileOut_.is_open()) {
        fileOut_.close();
    }
}

void LogRotation::rotateIfNeeded() {
    if (baseFilename_.empty()) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (currentFile_.empty()) {
        currentFile_ = baseFilename_;
        fileOut_.open(currentFile_, std::ios::app);
        return;
    }
    
    if (currentSize_.load() >= maxSize_) {
        std::string oldFile = currentFile_;
        size_t index = 1;
        
        // Rotate existing files
        while (index < maxFiles_) {
            std::string rotatedFile = getRotatedFilename(index);
            if (std::filesystem::exists(rotatedFile)) {
                std::filesystem::rename(rotatedFile, getRotatedFilename(index + 1));
            }
            index++;
        }
        
        // Move current file to .1
        std::filesystem::rename(oldFile, getRotatedFilename(1));
        
        // Create new current file
        currentFile_ = baseFilename_;
        currentSize_ = 0;
        fileOut_.close();
        fileOut_.open(currentFile_, std::ios::app);
        
        cleanupOldFiles();
    }
}

void LogRotation::cleanupOldFiles() {
    for (size_t i = maxFiles_; i < 1000; ++i) { // Reasonable upper bound
        std::string oldFile = getRotatedFilename(i);
        if (std::filesystem::exists(oldFile)) {
            std::filesystem::remove(oldFile);
        } else {
            break; // Stop when we find first non-existent file
        }
    }
}

std::string LogRotation::getRotatedFilename(size_t index) const {
    if (index == 0) return currentFile_;
    
    size_t dotPos = baseFilename_.find_last_of('.');
    if (dotPos == std::string::npos) {
        return baseFilename_ + "." + std::to_string(index);
    }
    
    std::string name = baseFilename_.substr(0, dotPos);
    std::string ext = baseFilename_.substr(dotPos);
    return name + "." + std::to_string(index) + ext;
}

void LogRotation::write(const std::string& data) {
    if (baseFilename_.empty()) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    rotateIfNeeded();
    
    if (!fileOut_.is_open() || !fileOut_.good()) {
        fileOut_.open(currentFile_, std::ios::app);
    }
    
    if (fileOut_.is_open()) {
        fileOut_ << data << "\n";
        currentSize_ += data.size() + 1; // +1 for newline
        fileOut_.flush();
    }
}

// LogStatistics implementation
LogStatistics::LogStatistics() {
    stats_.firstLog = std::chrono::system_clock::now();
}

void LogStatistics::recordLog(const LogEntry& entry) {
    std::lock_guard<std::mutex> lock(mutex_);
    stats_.count++;
    stats_.lastLog = std::chrono::system_clock::now();
    
    if (stats_.firstLog.time_since_epoch().count() == 0) {
        stats_.firstLog = entry.timestamp;
    }
    
    stats_.levelCounts[entry.level]++;
    if (!entry.category.empty()) {
        stats_.categoryCounts[entry.category]++;
    }
    stats_.messageCounts[entry.message]++;
}

size_t LogStatistics::getLogCount(LogLevel level) const {
    std::lock_guard<std::mutex> lock(mutex_);
    if (level == LogLevel::Custom) {
        return stats_.count.load();
    }
    return stats_.levelCounts.at(level);
}

double LogStatistics::getLogRate() const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto duration = std::chrono::duration<double>(
        stats_.lastLog.time_since_epoch() - stats_.firstLog.time_since_epoch()
    ).count();
    
    if (duration <= 0) return 0.0;
    return stats_.count.load() / duration;
}

std::unordered_map<LogLevel, size_t> LogStatistics::getLevelCounts() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return stats_.levelCounts;
}

std::unordered_map<std::string, size_t> LogStatistics::getCategoryCounts() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return stats_.categoryCounts;
}

std::vector<std::pair<std::string, uint64_t>> LogStatistics::getTopMessages(size_t limit) const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::pair<std::string, uint64_t>> topMessages;
    
    for (const auto& pair : stats_.messageCounts) {
        topMessages.emplace_back(pair);
    }
    
    std::partial_sort(topMessages.begin(), 
                      std::min(topMessages.begin() + limit, topMessages.end()),
                      topMessages.end(),
                      [](const auto& a, const auto& b) { return a.second > b.second; });
    
    topMessages.resize(std::min(topMessages.size(), limit));
    return topMessages;
}

std::string LogStatistics::getFormattedStats() const {
    std::ostringstream oss;
    oss << "=== Log Statistics ===" << std::endl;
    oss << "Total logs: " << stats_.count.load() << std::endl;
    oss << "Log rate: " << getLogRate() << " logs/sec" << std::endl;
    oss << std::endl;
    
    oss << "By level:" << std::endl;
    auto levelCounts = getLevelCounts();
    for (const auto& pair : levelCounts) {
        oss << "  " << static_cast<int>(pair.first) << ": " << pair.second << std::endl;
    }
    
    oss << std::endl;
    oss << "By category:" << std::endl;
    auto categoryCounts = getCategoryCounts();
    for (const auto& pair : categoryCounts) {
        oss << "  " << pair.first << ": " << pair.second << std::endl;
    }
    
    oss << std::endl;
    oss << "Top messages:" << std::endl;
    auto topMessages = getTopMessages(5);
    for (const auto& pair : topMessages) {
        oss << "  " << pair.second << ": " << pair.first << std::endl;
    }
    
    return oss.str();
}

void LogStatistics::reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    stats_.count = 0;
    stats_.firstLog = std::chrono::system_clock::now();
    stats_.lastLog = stats_.firstLog;
    stats_.levelCounts.clear();
    stats_.categoryCounts.clear();
    stats_.messageCounts.clear();
}

// LogContext implementation
thread_local LogContext* LogContext::currentContext_ = nullptr;

LogContext::LogContext(const std::string& operation, const std::string& category)
    : operation_(operation), category_(category) {
    setCurrentContext(this);
}

LogContext::~LogContext() {
    setCurrentContext(nullptr);
}

void LogContext::setProperty(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(mutex_);
    properties_[key] = value;
}

std::string LogContext::getProperty(const std::string& key) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = properties_.find(key);
    return it != properties_.end() ? it->second : "";
}

LogContext* getCurrentContext() {
    return LogContext::currentContext_;
}

void setCurrentContext(LogContext* context) {
    LogContext::currentContext_ = context;
}

// LogPerformanceTimer implementation
LogPerformanceTimer::LogPerformanceTimer(const std::string& operation, const std::string& category)
    : operation_(operation), category_(category), running_(true) {
    startTime_ = std::chrono::high_resolution_clock::now();
}

LogPerformanceTimer::~LogPerformanceTimer() {
    if (running_) {
        stop();
    }
}

void LogPerformanceTimer::stop() {
    if (!running_) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime_);
    
    running_ = false;
    
    // Log the performance metrics
    PerformanceMetrics metrics;
    metrics.duration = duration;
    metrics.operation = operation_;
    metrics.memoryUsage = 0; // Would need system-specific code to get actual memory usage
    
    // In a real implementation, would log to logger
    // For now, just store in tempData_
    tempData_ << "duration=" << duration.count() << "ns, operation=" << operation_;
}

// ConsoleLogger implementation (original implementation with enhanced features)
struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    std::shared_ptr<IFormatter> formatter;
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {
    pImpl->formatter = std::make_shared<KeyValueFormatter>();
}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
    pImpl->formatter = std::make_shared<KeyValueFormatter>();
}

ConsoleLogger::~ConsoleLogger() = default;

void ConsoleLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Use configured formatter or default to key-value
    std::string output = pImpl->formatter->format(entry);
    
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
    pImpl->fileOut.open(filepath, std::ios::app);
}

void ConsoleLogger::closeOutputFile() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

void ConsoleLogger::setFormatter(std::shared_ptr<IFormatter> formatter) {
    pImpl->formatter = formatter;
}

std::shared_ptr<IFormatter> ConsoleLogger::getFormatter() const {
    return pImpl->formatter;
}

std::string ConsoleLogger::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Critical: return "CRIT";
        case LogLevel::Notice: return "NOTICE";
        case LogLevel::Alert: return "ALERT";
        case LogLevel::Emergency: return "EMERG";
        case LogLevel::Custom: return "CUSTOM";
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
        case LogLevel::Notice: return "\033[94m";   // Blue
        case LogLevel::Alert: return "\033[91m";    // Bright Red
        case LogLevel::Emergency: return "\033[95m"; // Bright Magenta
        default: return "\033[0m";
    }
}

// Logger implementation
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    std::shared_ptr<IFormatter> defaultFormatter;
    LogStatistics stats;
    std::shared_ptr<LogContext> currentContext;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {
    pImpl->defaultFormatter = std::make_shared<KeyValueFormatter>();
    pImpl->currentContext = std::make_shared<LogContext>("default", "system");
}

Logger::~Logger() = default;

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
}

void Logger::log(LogLevel level, const std::string& message,
                 const std::string& file, int line,
                 const std::string& function,
                 const std::string& category,
                 const std::vector<std::string>& tags) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Create log entry
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.category = category;
    entry.threadId = std::this_thread::get_id();
    entry.processId = getpid();
    entry.logTime = entry.timestamp;
    entry.tags = tags;
    
    // Add to statistics
    pImpl->stats.recordLog(entry);
    
    // Apply current context if available
    if (pImpl->currentContext) {
        if (!pImpl->currentContext->getOperation().empty()) {
            entry.operationContext = pImpl->currentContext->getOperation();
        }
        if (!pImpl->currentContext->getCategory().empty()) {
            if (entry.category.empty()) {
                entry.category = pImpl->currentContext->getCategory();
            }
        }
    }
    
    // Log through all loggers
    for (auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
}

void Logger::logStructured(LogLevel level, const std::string& message,
                           const StructuredData& structured,
                           const std::string& file, int line,
                           const std::string& function,
                           const std::string& category) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.category = category;
    entry.threadId = std::this_thread::get_id();
    entry.processId = getpid();
    entry.logTime = entry.timestamp;
    entry.structuredData = structured;
    
    // Add to statistics
    pImpl->stats.recordLog(entry);
    
    // Log through all loggers
    for (auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
}

void Logger::logPerformance(const PerformanceMetrics& metrics,
                            const std::string& file, int line,
                            const std::string& function) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    LogEntry entry;
    entry.level = LogLevel::Info;
    entry.message = "Performance metrics logged";
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.category = "performance";
    entry.threadId = std::this_thread::get_id();
    entry.processId = getpid();
    entry.logTime = entry.timestamp;
    entry.performanceMetrics = metrics;
    
    // Add to statistics
    pImpl->stats.recordLog(entry);
    
    // Log through all loggers
    for (auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
}

void Logger::trace(const std::string& message, const std::string& file,
                   int line, const std::string& function) {
    log(LogLevel::Trace, message, file, line, function);
}

void Logger::debug(const std::string& message, const std::string& file,
                   int line, const std::string& function) {
    log(LogLevel::Debug, message, file, line, function);
}

void Logger::info(const std::string& message, const std::string& file,
                  int line, const std::string& function) {
    log(LogLevel::Info, message, file, line, function);
}

void Logger::notice(const std::string& message, const std::string& file,
                    int line, const std::string& function) {
    log(LogLevel::Notice, message, file, line, function);
}

void Logger::warning(const std::string& message, const std::string& file,
                     int line, const std::string& function) {
    log(LogLevel::Warning, message, file, line, function);
}

void Logger::error(const std::string& message, const std::string& file,
                   int line, const std::string& function) {
    log(LogLevel::Error, message, file, line, function);
}

void Logger::critical(const std::string& message, const std::string& file,
                      int line, const std::string& function) {
    log(LogLevel::Critical, message, file, line, function);
}

void Logger::alert(const std::string& message, const std::string& file,
                   int line, const std::string& function) {
    log(LogLevel::Alert, message, file, line, function);
}

void Logger::emergency(const std::string& message, const std::string& file,
                       int line, const std::string& function) {
    log(LogLevel::Emergency, message, file, line, function);
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
    // This is a placeholder for advanced global logger management
    // In practice, we just use getGlobal() which is already static
}

LogStatistics& Logger::getStatistics() {
    static LogStatistics globalStats;
    return globalStats;
}

LogContext& Logger::getCurrentContext() {
    return *getGlobal().pImpl->currentContext;
}

void Logger::setCurrentContext(LogContext& context) {
    getGlobal().pImpl->currentContext = std::shared_ptr<LogContext>(&context, [](LogContext*) {});
}

void Logger::logWithContext(LogLevel level, const std::string& message,
                            const std::string& operation,
                            const std::string& category,
                            const std::vector<std::string>& tags) {
    log(level, message, "", 0, "", category, tags);
    // In real implementation, would set operation context
}

void Logger::logWithProperties(LogLevel level, const std::string& message,
                               const std::unordered_map<std::string, std::string>& properties,
                               const std::string& file, int line,
                               const std::string& function) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Create log entry with metadata
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.threadId = std::this_thread::get_id();
    entry.processId = getpid();
    entry.logTime = entry.timestamp;
    entry.metadata = properties;
    
    // Add to statistics
    pImpl->stats.recordLog(entry);
    
    // Log through all loggers
    for (auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
}

std::string Logger::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::Trace: return "TRACE";
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Notice: return "NOTICE";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Critical: return "CRIT";
        case LogLevel::Alert: return "ALERT";
        case LogLevel::Emergency: return "EMERG";
        case LogLevel::Custom: return "CUSTOM";
        default: return "UNKNOWN";
    }
}

std::string Logger::levelToColor(LogLevel level) const {
    switch (level) {
        case LogLevel::Trace: return "\033[36m";    // Cyan
        case LogLevel::Debug: return "\033[36m";    // Cyan
        case LogLevel::Info: return "\033[32m";      // Green
        case LogLevel::Notice: return "\033[94m";   // Blue
        case LogLevel::Warning: return "\033[33m";   // Yellow
        case LogLevel::Error: return "\033[31m";     // Red
        case LogLevel::Critical: return "\033[35m"; // Magenta
        case LogLevel::Alert: return "\033[91m";    // Bright Red
        case LogLevel::Emergency: return "\033[95m"; // Bright Magenta
        default: return "\033[0m";
    }
}

// Helper functions
void initializeLogging() {
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    consoleLogger->setUseColors(true);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
}

std::shared_ptr<Logger> createDefaultLogger() {
    return std::make_shared<Logger>();
}

void configureDefaultLogger(const std::string& logfile, LogLevel level) {
    auto logger = createDefaultLogger();
    
    // Create console logger
    auto consoleLogger = std::make_shared<ConsoleLogger>(level);
    consoleLogger->setUseColors(true);
    
    if (!logfile.empty()) {
        auto fileLogger = std::make_shared<ConsoleLogger>(level);
        fileLogger->setOutputFile(logfile);
        logger->addLogger(fileLogger);
    }
    
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
}

// LogStream implementation
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function), timestamp_(std::chrono::system_clock::now()) {}

LogStream::~LogStream() {
    Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
}

} // namespace nlm