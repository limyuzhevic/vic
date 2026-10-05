#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>

namespace nlm {

#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <thread>
#include <queue>
#include <condition_variable>
#include <chrono>
#include <atomic>
#include <ctime>
#include <iomanip>
#include <vector>

namespace nlm {

// LogLevel string conversion utility
std::string LogLevelToString(LogLevel level) {
    switch (level) {
        case LogLevel::Trace: return "TRACE";
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Critical: return "CRIT";
        default: return "UNKNOWN";
    }
}

std::string LogLevelToColor(LogLevel level) {
    switch (level) {
        case LogLevel::Trace: return "\033[37m";    // White
        case LogLevel::Debug: return "\033[36m";    // Cyan
        case LogLevel::Info: return "\033[32m";      // Green
        case LogLevel::Warning: return "\033[33m";   // Yellow
        case LogLevel::Error: return "\033[31m";     // Red
        case LogLevel::Critical: return "\033[35m"; // Magenta
        default: return "\033[0m";
    }
}

// ColorFormatter implementation
std::string ColorFormatter::format(const LogEntry& entry) {
    std::ostringstream oss;
    
    if (useColors_) {
        oss << levelToColor(entry.level);
    }
    
    // Timestamp
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    if (timeFormat_ == "%Y-%m-%d %H:%M:%S") {
        oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    } else if (timeFormat_ == "%H:%M:%S") {
        oss << std::put_time(std::localtime(&time), "%H:%M:%S");
    } else {
        oss << std::put_time(std::localtime(&time), timeFormat_.c_str());
    }
    oss << " ";
    
    // Level
    oss << "[" << levelToString(entry.level) << "] ";
    
    // Thread ID (simplified)
    oss << "[thread:" << std::this_thread::get_id() << "] ";
    
    // Message
    oss << entry.message;
    
    if (useColors_) {
        oss << "\033[0m";
    }
    
    // Context
    if (includeContext_ && !entry.context.empty()) {
        oss << " {";
        bool first = true;
        for (const auto& pair : entry.context) {
            if (!first) oss << ", ";
            oss << pair.first << "=" << pair.second;
            first = false;
        }
        oss << "}";
    }
    
    oss << "\n";
    return oss.str();
}

std::string ColorFormatter::formatMinimal(const LogEntry& entry) {
    std::ostringstream oss;
    
    // Simple format: [LEVEL] message
    oss << "[" << levelToString(entry.level) << "] " << entry.message << "\n";
    return oss.str();
}

std::string ColorFormatter::formatJson(const LogEntry& entry) {
    std::ostringstream oss;
    oss << "{\"timestamp\": \"";
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    oss << "\", \"level\": \"" << levelToString(entry.level) << "\", \"message\": \"";
    // Escape JSON string
    std::string msg = entry.message;
    // Simple escaping - in production would use proper JSON library
    for (char c : msg) {
        if (c == '\\') oss << "\\\\";
        else if (c == '\"') oss << "\\\"";
        else if (c == '\n') oss << "\\n";
        else oss << c;
    }
    oss << "\"";
    
    // Add context if present
    if (!entry.context.empty()) {
        oss << ", \"context\": ";
        oss << "{";
        bool first = true;
        for (const auto& pair : entry.context) {
            if (!first) oss << ", ";
            oss << "\"" << pair.first << "\": \"";
            // Escape context value
            std::string val = pair.second;
            for (char c : val) {
                if (c == '\\') oss << "\\\\";
                else if (c == '\"') oss << "\\\"";
                else oss << c;
            }
            oss << "\"";
            first = false;
        }
        oss << "}";
    }
    
    oss << "}\n";
    return oss.str();
}

void ColorFormatter::setUseColors(bool useColors) {
    useColors_ = useColors;
}

bool ColorFormatter::getUseColors() const {
    return useColors_;
}

void ColorFormatter::setTimeFormat(const std::string& format) {
    timeFormat_ = format;
}

std::string ColorFormatter::getTimeFormat() const {
    return timeFormat_;
}

void ColorFormatter::setIncludeContext(bool includeContext) {
    includeContext_ = includeContext;
}

bool ColorFormatter::getIncludeContext() const {
    return includeContext_;
}

std::string ColorFormatter::levelToString(LogLevel level) const {
    return LogLevelToString(level);
}

std::string ColorFormatter::levelToColor(LogLevel level) const {
    return LogLevelToColor(level);
}

// SimpleFormatter implementation
std::string SimpleFormatter::format(const LogEntry& entry) {
    std::ostringstream oss;
    oss << "[" << LogLevelToString(entry.level) << "] ";
    oss << entry.message;
    
    if (includeContext_ && !entry.context.empty()) {
        oss << " {";
        bool first = true;
        for (const auto& pair : entry.context) {
            if (!first) oss << ", ";
            oss << pair.first << "=" << pair.second;
            first = false;
        }
        oss << "}";
    }
    
    oss << "\n";
    return oss.str();
}

std::string SimpleFormatter::formatMinimal(const LogEntry& entry) {
    return "[" + LogLevelToString(entry.level) + "] " + entry.message + "\n";
}

std::string SimpleFormatter::formatJson(const LogEntry& entry) {
    // Reuse ColorFormatter's JSON formatting for consistency
    ColorFormatter colorFormatter;
    return colorFormatter.formatJson(entry);
}

void SimpleFormatter::setIncludeContext(bool includeContext) {
    includeContext_ = includeContext;
}

bool SimpleFormatter::getIncludeContext() const {
    return includeContext_;
}

// AsyncLogger implementation
struct AsyncLogger::Impl {
    std::vector<std::thread> workers;
    std::queue<LogEntry> messageQueue;
    std::mutex queueMutex;
    std::condition_variable queueCondition;
    std::atomic<bool> stopFlag{false};
    std::atomic<LogLevel> level{LogLevel::Info};
    std::shared_ptr<LogFormatter> formatter;
    size_t maxQueueSize = 8192;
    
    Impl() {
        // Create worker threads
        for (size_t i = 0; i < 2; ++i) {
            workers.emplace_back([this]() {
                while (!stopFlag.load(std::memory_order_acquire)) {
                    std::unique_lock<std::mutex> lock(this->queueMutex);
                    this->queueCondition.wait(lock, [this]() {
                        return !this->messageQueue.empty() || stopFlag.load(std::memory_order_acquire);
                    });
                    
                    if (!this->messageQueue.empty()) {
                        LogEntry entry = std::move(this->messageQueue.front());
                        this->messageQueue.pop();
                        lock.unlock();
                        
                        // Format and output
                        if (formatter) {
                            std::string formatted = formatter->format(entry);
                            std::cout << formatted;
                        }
                    }
                }
            });
        }
    }
    
    ~Impl() {
        stopFlag.store(true, std::memory_order_release);
        queueCondition.notify_all();
        for (auto& worker : workers) {
            if (worker.joinable()) {
                worker.join();
            }
        }
    }
};

AsyncLogger::AsyncLogger(size_t queueSize, size_t workerThreads) {
    pImpl = std::make_unique<Impl>();
    pImpl->maxQueueSize = queueSize;
    pImpl->formatter = std::make_shared<ColorFormatter>();
}

AsyncLogger::~AsyncLogger() = default;

void AsyncLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level.load(std::memory_order_acquire)) {
        return;
    }
    
    std::unique_lock<std::mutex> lock(pImpl->queueMutex);
    pImpl->queueCondition.wait(lock, [this]() {
        return pImpl->messageQueue.size() < pImpl->maxQueueSize;
    });
    
    pImpl->messageQueue.push(entry);
    lock.unlock();
    pImpl->queueCondition.notify_one();
}

void AsyncLogger::setLevel(LogLevel level) {
    pImpl->level.store(level, std::memory_order_release);
}

LogLevel AsyncLogger::getLevel() const {
    return pImpl->level.load(std::memory_order_acquire);
}

void AsyncLogger::setFormatter(std::shared_ptr<LogFormatter> formatter) {
    pImpl->formatter = formatter;
}

std::shared_ptr<LogFormatter> AsyncLogger::getFormatter() const {
    return pImpl->formatter;
}

void AsyncLogger::flush() {
    std::unique_lock<std::mutex> lock(pImpl->queueMutex);
    pImpl->queueCondition.notify_all();
}

// Logger class implementation
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    std::map<std::string, std::string> context;
    PerformanceStats stats;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {
    // Add default console logger
    auto consoleLogger = std::make_shared<ConsoleLogger>();
    consoleLogger->setLevel(LogLevel::Info);
    auto colorFormatter = std::make_shared<ColorFormatter>();
    colorFormatter->setUseColors(true);
    // Note: ConsoleLogger doesn't currently use formatter, but we could extend it
    pImpl->loggers.push_back(consoleLogger);
}

Logger::~Logger() = default;

Logger::Logger(Logger&&) noexcept = default;

Logger& Logger::operator=(Logger&&) noexcept = default;

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

void Logger::setGlobalFormatter(std::shared_ptr<LogFormatter> formatter) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    // Apply to all loggers that support it
    for (auto& logger : pImpl->loggers) {
        // In a real implementation, ILogger would have setFormatter method
        // For now, we'll just use the global formatter for console output
    }
}

void Logger::log(LogLevel level, const std::string& message,
                 const std::string& file, int line, const std::string& function) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.threadId = std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    entry.processId = getpid();
    entry.context = pImpl->context;
    
    pImpl->stats.totalLogs++;
    pImpl->stats.logSizeBytes += message.size();
    pImpl->stats.lastLogTime = entry.timestamp;
    
    for (auto& logger : pImpl->loggers) {
        try {
            logger->log(entry);
        } catch (const std::exception& e) {
            std::cerr << "Logger error: " << e.what() << std::endl;
        }
    }
}

void Logger::addContext(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->context[key] = value;
}

void Logger::clearContext() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->context.clear();
}

void Logger::trace(const std::string& message, const std::string& file, int line, const std::string& function) {
    if (isEnabled(LogLevel::Trace)) {
        log(LogLevel::Trace, message, file, line, function);
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
    // For now, we'll use the default implementation
    // In practice, this could replace the global logger
}

Logger::PerformanceStats Logger::getStats() const {
    return pImpl->stats;
}

void Logger::resetStats() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->stats = PerformanceStats();
}

std::string Logger::formatString(const std::string& format, ...) {
    // Simple format string implementation
    // In production, would use fmtlib or similar
    va_list args;
    va_start(args, format);
    
    std::string result;
    size_t pos = 0;
    size_t argIndex = 0;
    
    while (pos < format.size()) {
        if (format[pos] == '{' && pos + 1 < format.size() && format[pos + 1] == '}') {
            // Simple placeholder replacement
            va_list copyArgs;
            va_copy(copyArgs, args);
            
            // Try different types
            if (argIndex < 10) { // Limit argument count for safety
                // This is a simplified implementation
                // Real implementation would use proper type dispatching
                result += std::to_string(va_arg(copyArgs, int));
            }
            
            va_end(copyArgs);
            pos += 2;
            argIndex++;
        } else {
            result += format[pos];
            pos++;
        }
    }
    
    va_end(args);
    return result;
}

// Global logger instance
std::shared_ptr<Logger> globalLogger = nullptr;

// LogStream implementation
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function) {}

LogStream::~LogStream() {
    Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
}

LogStream& LogStream::withContext(const std::string& key, const std::string& value) {
    context_[key] = value;
    return *this;
}

} // namespace nlm
