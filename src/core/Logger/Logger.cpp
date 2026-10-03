// Enhanced implementation with async and structured logging support
#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <queue>
#include <thread>
#include <chrono>
#include <atomic>
#include <iomanip>
#include <ctime>
#include <functional>
#include <map>

namespace nlm {

// AsyncLogger implementation
struct AsyncLogger::Impl {
    std::queue<LogEntry> messageQueue;
    std::thread workerThread;
    std::mutex queueMutex;
    std::condition_variable queueCondition;
    std::atomic<bool> running{false};
    std::atomic<size_t> maxQueueSize{10000};
    std::chrono::milliseconds flushInterval{100};
    
    std::shared_ptr<ILogger> actualLogger;
    
    std::atomic<size_t> processedCount{0};
    std::atomic<size_t> droppedCount{0};
    
    LogLevel minLevel = LogLevel::Info;
    
    void worker() {
        while (running) {
            std::unique_lock<std::mutex> lock(queueMutex);
            
            if (messageQueue.empty()) {
                auto timeout = std::chrono::milliseconds(flushInterval.count());
                if (queueCondition.wait_for(lock, timeout, [this]() { return !messageQueue.empty(); })) {
                    // Process messages
                    processNextMessage(lock);
                }
            } else {
                processNextMessage(lock);
            }
        }
    }
    
    void processNextMessage(std::unique_lock<std::mutex>& lock) {
        if (!messageQueue.empty()) {
            LogEntry entry = std::move(messageQueue.front());
            messageQueue.pop();
            lock.unlock();
            
            try {
                actualLogger->log(entry);
                processedCount++;
            } catch (const std::exception& e) {
                std::cerr << "AsyncLogger error: " << e.what() << std::endl;
                droppedCount++;
            }
            
            lock.lock();
        }
    }
};

AsyncLogger::AsyncLogger(size_t maxQueueSize, LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->maxQueueSize = maxQueueSize;
    pImpl->minLevel = level;
    pImpl->actualLogger = std::make_shared<EnhancedConsoleLogger>(level);
}

AsyncLogger::~AsyncLogger() {
    stop();
}

void AsyncLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->minLevel) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->queueMutex);
    
    if (pImpl->messageQueue.size() >= pImpl->maxQueueSize) {
        // Queue full - drop message
        pImpl->droppedCount++;
        return;
    }
    
    pImpl->messageQueue.push(entry);
    pImpl->queueCondition.notify_one();
}

void AsyncLogger::setLevel(LogLevel level) {
    pImpl->minLevel = level;
}

LogLevel AsyncLogger::getLevel() const {
    return pImpl->minLevel;
}

void AsyncLogger::start() {
    if (!pImpl->running) {
        pImpl->running = true;
        pImpl->workerThread = std::thread(&AsyncLogger::Impl::worker, pImpl.get());
    }
}

void AsyncLogger::stop() {
    if (pImpl->running) {
        pImpl->running = false;
        pImpl->queueCondition.notify_one();
        if (pImpl->workerThread.joinable()) {
            pImpl->workerThread.join();
        }
    }
}

bool AsyncLogger::isRunning() const {
    return pImpl->running;
}

void AsyncLogger::setQueueSize(size_t size) {
    pImpl->maxQueueSize = size;
}

void AsyncLogger::setFlushInterval(const std::chrono::milliseconds& interval) {
    pImpl->flushInterval = interval;
}

size_t AsyncLogger::getProcessedCount() const {
    return pImpl->processedCount;
}

size_t AsyncLogger::getQueueSize() const {
    std::lock_guard<std::mutex> lock(pImpl->queueMutex);
    return pImpl->messageQueue.size();
}

// EnhancedConsoleLogger implementation
struct EnhancedConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    bool useLevelColors = true;
    std::string format = "[%level] [%timestamp] [%source] %message";
    std::ofstream fileOut;
    std::mutex mutex;
    
    size_t bufferSize = 1024;
    std::string buffer;
    bool bufferEnabled = true;
    
    std::map<LogLevel, std::string> levelStrings = {
        {LogLevel::Debug, "DEBUG"},
        {LogLevel::Info, "INFO"},
        {LogLevel::Warning, "WARN"},
        {LogLevel::Error, "ERROR"},
        {LogLevel::Critical, "CRIT"}
    };
    
    std::map<LogLevel, std::string> levelColors = {
        {LogLevel::Debug, "\033[36m"},  // Cyan
        {LogLevel::Info, "\033[32m"},   // Green
        {LogLevel::Warning, "\033[33m"}, // Yellow
        {LogLevel::Error, "\033[31m"},  // Red
        {LogLevel::Critical, "\033[35m"} // Magenta
    };
};

EnhancedConsoleLogger::EnhancedConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
}

EnhancedConsoleLogger::~EnhancedConsoleLogger() {
    flush();
}

void EnhancedConsoleLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Format the entry
    std::string formatted = formatEntry(entry);
    
    // Add to buffer if enabled
    if (pImpl->bufferEnabled) {
        pImpl->buffer += formatted + "\n";
        if (pImpl->buffer.size() >= pImpl->bufferSize) {
            std::cout << pImpl->buffer;
            pImpl->buffer.clear();
        }
    } else {
        std::cout << formatted << "\n";
    }
    
    // Also write to file if set
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut << formatted << "\n";
        pImpl->fileOut.flush();
    }
}

void EnhancedConsoleLogger::setLevel(LogLevel level) {
    pImpl->level = level;
}

LogLevel EnhancedConsoleLogger::getLevel() const {
    return pImpl->level;
}

void EnhancedConsoleLogger::setFormat(const std::string& format) {
    pImpl->format = format;
}

void EnhancedConsoleLogger::setUseColors(bool useColors) {
    pImpl->useColors = useColors;
}

bool EnhancedConsoleLogger::getUseColors() const {
    return pImpl->useColors;
}

void EnhancedConsoleLogger::setColorByLevel(bool useLevelColors) {
    pImpl->useLevelColors = useLevelColors;
}

void EnhancedConsoleLogger::setOutputFile(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->fileOut.open(filepath, std::ios::app);
}

void EnhancedConsoleLogger::closeOutputFile() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

void EnhancedConsoleLogger::setBufferSize(size_t size) {
    pImpl->bufferSize = size;
}

void EnhancedConsoleLogger::flush() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (!pImpl->buffer.empty()) {
        std::cout << pImpl->buffer;
        if (pImpl->fileOut.is_open()) {
            pImpl->fileOut << pImpl->buffer;
            pImpl->fileOut.flush();
        }
        pImpl->buffer.clear();
    }
}

std::string EnhancedConsoleLogger::formatEntry(const LogEntry& entry) const {
    std::string result = pImpl->format;
    
    // Replace placeholders
    auto replacePlaceholder = [&](const std::string& placeholder, const std::string& value) {
        size_t pos = result.find(placeholder);
        while (pos != std::string::npos) {
            result.replace(pos, placeholder.length(), value);
            pos = result.find(placeholder, pos + value.length());
        }
    };
    
    replacePlaceholder("%level", pImpl->levelStrings[entry.level]);
    replacePlaceholder("%timestamp", getTimestamp(entry.timestamp));
    replacePlaceholder("%source", getSourceInfo(entry));
    replacePlaceholder("%message", entry.message);
    replacePlaceholder("%category", entry.category);
    
    // Add color if enabled
    if (pImpl->useColors && pImpl->useLevelColors) {
        result = pImpl->levelColors[entry.level] + result + "\033[0m";
    }
    
    return result;
}

std::string EnhancedConsoleLogger::levelToString(LogLevel level) const {
    auto it = pImpl->levelStrings.find(level);
    return it != pImpl->levelStrings.end() ? it->second : "UNKNOWN";
}

std::string EnhancedConsoleLogger::levelToColor(LogLevel level) const {
    auto it = pImpl->levelColors.find(level);
    return it != pImpl->levelColors.end() ? it->second : "";
}

std::string getTimestamp(const std::chrono::system_clock::time_point& tp) {
    auto time = std::chrono::system_clock::to_time_t(tp);
    std::tm tm_time;
    
    // Thread-safe version of localtime
#ifdef _WIN32
    localtime_s(&tm_time, &time);
#else
    localtime_r(&time, &tm_time);
#endif
    
    std::stringstream ss;
    ss << std::put_time(&tm_time, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string getSourceInfo(const LogEntry& entry) {
    if (entry.file.empty()) {
        return "";
    }
    
    size_t lastSlash = entry.file.find_last_of("/\\");
    std::string filename = (lastSlash != std::string::npos) ? 
                          entry.file.substr(lastSlash + 1) : entry.file;
    
    std::stringstream ss;
    ss << filename << ":" << entry.line;
    if (!entry.function.empty()) {
        ss << " in " << entry.function;
    }
    
    return ss.str();
}

// Main Logger implementation
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    
    // Statistics
    std::atomic<size_t> totalLogCount{0};
    std::atomic<size_t> errorCount{0};
    std::atomic<size_t> performanceLogCount{0};
    
    LogLevel minLevel = LogLevel::Info;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {}

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
    pImpl->minLevel = level;
    for (auto& logger : pImpl->loggers) {
        logger->setLevel(level);
    }
}

void Logger::log(LogLevel level, const std::string& message,
                 const std::string& file, int line,
                 const std::string& function, const std::string& category) {
    if (level < pImpl->minLevel) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.category = category;
    entry.timestamp = std::chrono::system_clock::now();
    
    pImpl->totalLogCount++;
    if (level >= LogLevel::Error) {
        pImpl->errorCount++;
    }
    
    for (auto& logger : pImpl->loggers) {
        try {
            logger->log(entry);
        } catch (const std::exception& e) {
            std::cerr << "Logger error: " << e.what() << std::endl;
        }
    }
}

void Logger::logStructured(LogLevel level, const std::string& category,
                          const std::string& message,
                          const std::unordered_map<std::string, std::string>& metadata) {
    log(level, message, "", 0, "", category);
    
    // TODO: Store metadata in the log entry
    // For now, just log metadata as part of message
    if (!metadata.empty()) {
        std::string metadataStr = " [METADATA: ";
        bool first = true;
        for (const auto& kv : metadata) {
            if (!first) metadataStr += ", ";
            metadataStr += kv.first + "=" + kv.second;
            first = false;
        }
        metadataStr += "]";
        // Append metadata to the log message
        log(level, metadataStr, "", 0, "", category);
    }
}

void Logger::logPerformance(const std::string& operation,
                           const std::chrono::steady_clock::duration& duration,
                           const std::string& metadata) {
    pImpl->performanceLogCount++;
    
    std::stringstream ss;
    ss << "PERFORMANCE: " << operation << 
          " took " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << 
          "ms";
    if (!metadata.empty()) {
        ss << " [" << metadata << "]";
    }
    
    log(LogLevel::Info, ss.str(), "", 0, "", "PERFORMANCE");
}

void Logger::debug(const std::string& message, const std::string& file, int line,
                  const std::string& function, const std::string& category) {
    log(LogLevel::Debug, message, file, line, function, category);
}

void Logger::info(const std::string& message, const std::string& file, int line,
                 const std::string& function, const std::string& category) {
    log(LogLevel::Info, message, file, line, function, category);
}

void Logger::warning(const std::string& message, const std::string& file, int line,
                    const std::string& function, const std::string& category) {
    log(LogLevel::Warning, message, file, line, function, category);
}

void Logger::error(const std::string& message, const std::string& file, int line,
                  const std::string& function, const std::string& category) {
    log(LogLevel::Error, message, file, line, function, category);
}

void Logger::critical(const std::string& message, const std::string& file, int line,
                     const std::string& function, const std::string& category) {
    log(LogLevel::Critical, message, file, line, function, category);
}

bool Logger::isEnabled(LogLevel level) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return level >= pImpl->minLevel;
}

Logger& Logger::getGlobal() {
    static Logger globalLogger;
    return globalLogger;
}

void Logger::setGlobal(std::shared_ptr<Logger> logger) {
    // This is a placeholder for advanced global logger management
    // In practice, we just use getGlobal() which is already static
}

size_t Logger::getTotalLogCount() const {
    return pImpl->totalLogCount;
}

size_t Logger::getErrorCount() const {
    return pImpl->errorCount;
}

size_t Logger::getPerformanceLogCount() const {
    return pImpl->performanceLogCount;
}

std::string Logger::getLogStatistics() const {
    std::stringstream ss;
    ss << "Log Statistics:" << std::endl;
    ss << "  Total logs: " << pImpl->totalLogCount << std::endl;
    ss << "  Errors: " << pImpl->errorCount << std::endl;
    ss << "  Performance logs: " << pImpl->performanceLogCount << std::endl;
    ss << "  Minimum level: " << static_cast<int>(pImpl->minLevel) << std::endl;
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    ss << "  Active loggers: " << pImpl->loggers.size() << std::endl;
    
    return ss.str();
}

// ILogger interface implementation for performance logging
void ILogger::logStructured(LogLevel level, const std::string& category,
                          const std::string& message,
                          const std::unordered_map<std::string, std::string>& metadata) {
    // Default implementation - can be overridden
    std::stringstream ss;
    ss << "[" << category << "] " << message;
    if (!metadata.empty()) {
        ss << " {";
        bool first = true;
        for (const auto& kv : metadata) {
            if (!first) ss << ", ";
            ss << kv.first << "=" << kv.second;
            first = false;
        }
        ss << "}";
    }
    log(LogEntry{level, ss.str()});
}

void ILogger::logPerformance(const std::string& operation,
                           const std::chrono::steady_clock::duration& duration,
                           const std::string& metadata) {
    // Default implementation
    std::stringstream ss;
    ss << "PERFORMANCE: " << operation << 
          " took " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms";
    if (!metadata.empty()) {
        ss << " [" << metadata << "]";
    }
    log(LogEntry{LogLevel::Info, ss.str()});
}

// LogStream implementation
LogStream::LogStream(LogLevel level, const std::string& file, int line,
                   const std::string& function, const std::string& category)
    : level_(level), file_(file), line_(line), function_(function), category_(category) {}

LogStream::~LogStream() {
    Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_, category_);
}

LogStream& LogStream::operator<<(const std::pair<std::string, std::string>& kv) {
    oss_ << "[" << kv.first << "=" << kv.second << "] ";
    return *this;
}

} // namespace nlm