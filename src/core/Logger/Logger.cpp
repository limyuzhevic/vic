#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <mutex>
#include <algorithm>
#include <thread>
#include <queue>
#include <functional>
#include <iomanip>
#include <limits>

namespace nlm {

// Thread-safe queue for async logging
struct LogMessage {
    LogEntry entry;
    std::chrono::system_clock::time_point processedTime;
};

class AsyncLogger {
public:
    AsyncLogger(size_t queueSize = 1000) : queueSize_(queueSize), running_(true) {
        workerThread_ = std::thread(&AsyncLogger::processQueue, this);
    }
    
    ~AsyncLogger() {
        running_ = false;
        queueCond_.notify_all();
        if (workerThread_.joinable()) {
            workerThread_.join();
        }
    }
    
    void log(const LogEntry& entry) {
        std::unique_lock<std::mutex> lock(queueMutex_);
        queue_.push({entry, std::chrono::system_clock::now()});
        queueCond_.notify_one();
        
        if (queue_.size() > queueSize_) {
            // Drop oldest message if queue is full
            queue_.pop();
        }
    }
    
    void setLevel(LogLevel level) {
        std::lock_guard<std::mutex> lock(levelMutex_);
        level_ = level;
    }
    
    LogLevel getLevel() const {
        std::lock_guard<std::mutex> lock(levelMutex_);
        return level_;
    }
    
    void setLevelEnabled(LogLevel level, bool enabled) {
        std::lock_guard<std::mutex> lock(enabledLevelsMutex_);
        if (enabled) {
            enabledLevels_.insert(level);
        } else {
            enabledLevels_.erase(level);
        }
    }
    
    bool isLevelEnabled(LogLevel level) const {
        std::lock_guard<std::mutex> lock(enabledLevelsMutex_);
        return enabledLevels_.find(level) != enabledLevels_.end();
    }
    
    void setThreadId(const std::string& threadId) {
        std::lock_guard<std::mutex> lock(threadIdMutex_);
        currentThreadId_ = threadId;
    }
    
    std::string getCurrentThreadId() const {
        std::lock_guard<std::mutex> lock(threadIdMutex_);
        return currentThreadId_;
    }
    
    void addFilter(std::function<bool(const LogEntry&)> filter) {
        std::lock_guard<std::mutex> lock(filtersMutex_);
        filters_.push_back(filter);
    }
    
    void setFormat(const std::string& format) {
        std::lock_guard<std::mutex> lock(formatMutex_);
        format_ = format;
    }
    
    std::string getFormat() const {
        std::lock_guard<std::mutex> lock(formatMutex_);
        return format_;
    }
    
    void setUseColors(bool useColors) {
        std::lock_guard<std::mutex> lock(colorsMutex_);
        useColors_ = useColors;
    }
    
    bool getUseColors() const {
        std::lock_guard<std::mutex> lock(colorsMutex_);
        return useColors_;
    }
    
    void setColor(LogLevel level, const std::string& colorCode) {
        std::lock_guard<std::mutex> lock(colorsMutex_);
        colors_[level] = colorCode;
    }
    
    std::string getColor(LogLevel level) const {
        std::lock_guard<std::mutex> lock(colorsMutex_);
        auto it = colors_.find(level);
        return (it != colors_.end()) ? it->second : "";
    }
    
    void setOutputFile(const std::string& filepath) {
        std::lock_guard<std::mutex> lock(fileMutex_);
        outputFile_ = filepath;
        if (outputFile_.empty()) {
            closeOutputFile();
        } else {
            openOutputFile();
        }
    }
    
    void closeOutputFile() {
        std::lock_guard<std::mutex> lock(fileMutex_);
        if (outputFileStream_.is_open()) {
            outputFileStream_.close();
        }
    }
    
    bool isEnabled(LogLevel level) const {
        return isLevelEnabled(level) && level >= level_;
    }
    
    size_t getLogCount(LogLevel level) const {
        std::lock_guard<std::mutex> lock(countsMutex_);
        auto it = counts_.find(level);
        return (it != counts_.end()) ? it->second : 0;
    }
    
    size_t getTotalLogCount() const {
        std::lock_guard<std::mutex> lock(countsMutex_);
        size_t total = 0;
        for (const auto& pair : counts_) {
            total += pair.second;
        }
        return total;
    }
    
    void resetStatistics() {
        std::lock_guard<std::mutex> lock(countsMutex_);
        counts_.clear();
    }
    
    std::map<LogLevel, size_t> getLogStatistics() const {
        std::lock_guard<std::mutex> lock(countsMutex_);
        return counts_;
    }
    
    void processQueue() {
        while (running_) {
            std::unique_lock<std::mutex> lock(queueMutex_);
            queueCond_.wait_for(lock, std::chrono::milliseconds(100), 
                              [this]() { return !queue_.empty() || !running_; });
            
            while (!queue_.empty()) {
                LogMessage msg = queue_.front();
                queue_.pop();
                lock.unlock();
                
                processMessage(msg);
                
                lock.lock();
            }
        }
    }
    
    void processMessage(const LogMessage& msg) {
        if (shouldProcessMessage(msg.entry)) {
            outputMessage(msg.entry);
            incrementLogCount(msg.entry.level);
        }
    }
    
    bool shouldProcessMessage(const LogEntry& entry) const {
        // Check level filtering
        if (!isEnabled(entry.level)) {
            return false;
        }
        
        // Check tag filtering
        if (!tagFilter_.empty()) {
            bool hasMatchingTag = false;
            for (const auto& tag : entry.tags) {
                if (tagFilter_.find(tag) != tagFilter_.end()) {
                    hasMatchingTag = true;
                    break;
                }
            }
            if (!hasMatchingTag) {
                return false;
            }
        }
        
        // Check custom filters
        std::lock_guard<std::mutex> lock(filtersMutex_);
        for (const auto& filter : filters_) {
            if (!filter(entry)) {
                return false;
            }
        }
        
        return true;
    }
    
    void outputMessage(const LogEntry& entry) {
        std::string formatted = formatMessage(entry);
        
        // Output to console
        logToConsole(formatted);
        
        // Output to file
        logToFile(formatted);
    }
    
    std::string formatMessage(const LogEntry& entry) const {
        std::stringstream ss;
        
        // Thread ID
        ss << "[" << entry.threadId << "] ";
        
        // Timestamp
        ss << "[" << entry.getFormattedTimestamp() << "] ";
        
        // Level
        ss << "[" << levelToString(entry.level) << "] ";
        
        // File and line
        if (!entry.file.empty()) {
            ss << "(" << entry.file << ":" << entry.line << ") ";
        }
        
        // Function
        if (!entry.function.empty()) {
            ss << "{" << entry.function << "} ";
        }
        
        // Context
        if (!entry.context.empty()) {
            ss << "<" << entry.context << "> ";
        }
        
        // Message
        ss << entry.message;
        
        return ss.str();
    }
    
    void logToConsole(const std::string& message) {
        if (getUseColors()) {
            LogLevel level = getLevelFromString(message);
            std::string color = getColor(level);
            if (!color.empty()) {
                std::cout << color << message << "\033[0m" << std::endl;
                return;
            }
        }
        std::cout << message << std::endl;
    }
    
    void logToFile(const std::string& message) {
        if (outputFileStream_.is_open()) {
            outputFileStream_ << message << std::endl;
            outputFileStream_.flush();
        }
    }
    
    LogLevel getLevelFromString(const std::string& message) const {
        // Simple parsing - extract level from message
        size_t start = message.find("[]");
        if (start != std::string::npos) {
            size_t end = message.find("]", start);
            if (end != std::string::npos) {
                std::string levelStr = message.substr(start + 2, end - start - 2);
                return stringToLevel(levelStr);
            }
        }
        return LogLevel::Info;
    }
    
    void incrementLogCount(LogLevel level) {
        std::lock_guard<std::mutex> lock(countsMutex_);
        counts_[level]++;
    }
    
    void openOutputFile() {
        if (!outputFile_.empty()) {
            outputFileStream_.open(outputFile_, std::ios::app);
            if (!outputFileStream_.is_open()) {
                // Fall back to std::cerr if file opening fails
                outputFile_.clear();
            }
        }
    }
    
    void setThreadIdFromCurrent() {
        std::hash<std::thread::id> hasher;
        std::thread::id this_id = std::this_thread::get_id();
        setThreadId(std::to_string(hasher(this_id)));
    }
    
    std::set<std::string> tagFilter_;
    bool running_;
    size_t queueSize_;
    
    std::queue<LogMessage> queue_;
    std::mutex queueMutex_;
    std::condition_variable queueCond_;
    std::thread workerThread_;
    
    LogLevel level_;
    std::mutex levelMutex_;
    
    std::set<LogLevel> enabledLevels_;
    std::mutex enabledLevelsMutex_;
    
    std::string currentThreadId_;
    std::mutex threadIdMutex_;
    
    std::vector<std::function<bool(const LogEntry&)>> filters_;
    std::mutex filtersMutex_;
    
    std::string format_;
    std::mutex formatMutex_;
    
    bool useColors_;
    std::mutex colorsMutex_;
    std::map<LogLevel, std::string> colors_;
    
    std::string outputFile_;
    std::ofstream outputFileStream_;
    std::mutex fileMutex_;
    
    std::map<LogLevel, size_t> counts_;
    std::mutex countsMutex_;
    
    bool enablePerformanceMonitoring_;
};

namespace {
    std::mutex globalLoggerMutex_;
    std::shared_ptr<Logger> globalLogger_ = nullptr;
    std::mutex globalLoggerGuard_;
}

struct ConsoleLogger::Impl {
    bool useColors;
    std::map<LogLevel, std::string> colors;
    std::string outputFormat;
    bool asyncLogging;
    size_t queueSize;
    std::unique_ptr<AsyncLogger> asyncLogger;
    
    Impl() : useColors(true), asyncLogging(false), queueSize(1000) {
        setDefaultColors();
    }
    
    void setDefaultColors() {
        colors[LogLevel::Emergency] = "\033[1;31m";     // Bold red
        colors[LogLevel::Alert] = "\033[1;35m";        // Bold magenta
        colors[LogLevel::Critical] = "\033[1;31m";    // Bold red
        colors[LogLevel::Error] = "\033[31m";          // Red
        colors[LogLevel::Warning] = "\033[33m";        // Yellow
        colors[LogLevel::Notice] = "\033[36m";         // Cyan
        colors[LogLevel::Info] = "\033[32m";           // Green
        colors[LogLevel::Debug] = "\033[90m";          // Gray
        colors[LogLevel::Trace] = "\033[90m";          // Light gray
        colors[LogLevel::Perf] = "\033[94m";           // Blue
    }
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    setLevel(level);
}

ConsoleLogger::~ConsoleLogger() = default;

void ConsoleLogger::log(const LogEntry& entry) {
    if (pImpl->asyncLogger) {
        pImpl->asyncLogger->log(entry);
        return;
    }
    
    std::string formatted = formatMessage(entry);
    
    if (pImpl->useColors) {
        std::string color = pImpl->colors[entry.level];
        if (!color.empty()) {
            std::cout << color << formatted << "\033[0m" << std::endl;
        } else {
            std::cout << formatted << std::endl;
        }
    } else {
        std::cout << formatted << std::endl;
    }
}

void ConsoleLogger::setLevel(LogLevel level) {
    setLevelEnabled(level, true);
}

LogLevel ConsoleLogger::getLevel() const {
    // Find the highest enabled level
    LogLevel highestLevel = LogLevel::Off;
    for (int i = static_cast<int>(LogLevel::Off); i <= static_cast<int>(LogLevel::Perf); ++i) {
        LogLevel level = static_cast<LogLevel>(i);
        if (isLevelEnabled(level) && level > highestLevel) {
            highestLevel = level;
        }
    }
    return highestLevel;
}

void ConsoleLogger::setUseColors(bool useColors) {
    pImpl->useColors = useColors;
}

bool ConsoleLogger::getUseColors() const {
    return pImpl->useColors;
}

void ConsoleLogger::setColor(LogLevel level, const std::string& colorCode) {
    pImpl->colors[level] = colorCode;
}

std::string ConsoleLogger::getColor(LogLevel level) const {
    auto it = pImpl->colors.find(level);
    return (it != pImpl->colors.end()) ? it->second : "";
}

void ConsoleLogger::setOutputFile(const std::string& filepath) {
    if (filepath.empty()) {
        closeOutputFile();
    } else {
        // Simple file output implementation
        std::ofstream file(filepath, std::ios::app);
        if (file.is_open()) {
            file << "Log file redirected to: " << filepath << std::endl;
        }
    }
}

void ConsoleLogger::closeOutputFile() {
    // File handling is simplified for console logger
}

void ConsoleLogger::setOutputFormat(const std::string& format) {
    pImpl->outputFormat = format;
}

std::string ConsoleLogger::getOutputFormat() const {
    return pImpl->outputFormat;
}

void ConsoleLogger::setAsyncLogging(bool enabled, size_t queueSize) {
    if (enabled) {
        pImpl->asyncLogger = std::make_unique<AsyncLogger>(queueSize);
        pImpl->asyncLogger->setLevel(getLevel());
        pImpl->asyncLogger->setThreadIdFromCurrent();
    } else {
        pImpl->asyncLogger.reset();
    }
}

bool ConsoleLogger::isAsyncLogging() const {
    return pImpl->asyncLogger != nullptr;
}

void ConsoleLogger::setPerformanceMonitoring(bool enabled) {
    // Performance monitoring is handled by the global logger
    pImpl->asyncLogger->setUseColors(pImpl->useColors);
}

bool ConsoleLogger::isPerformanceMonitoring() const {
    return pImpl->asyncLogger != nullptr;
}

struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex loggersMutex;
    std::string currentContext;
    std::mutex contextMutex;
    std::mutex globalMutex;
    bool locked;
    LogLevel globalLevel;
    size_t maxDepth;
    
    Impl() : locked(false), globalLevel(LogLevel::Info), maxDepth(10) {}
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {
    // Add default console logger
    auto consoleLogger = std::make_shared<ConsoleLogger>();
    consoleLogger->setLevel(LogLevel::Info);
    addLogger(consoleLogger);
}

Logger::~Logger() = default;

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    if (!logger) return;
    
    std::lock_guard<std::mutex> lock(pImpl->loggersMutex);
    pImpl->loggers.push_back(logger);
    
    // Set the level on the new logger
    logger->setLevel(pImpl->globalLevel);
}

bool Logger::removeLogger(ILogger* logger) {
    if (!logger) return false;
    
    std::lock_guard<std::mutex> lock(pImpl->loggersMutex);
    for (auto it = pImpl->loggers.begin(); it != pImpl->loggers.end(); ++it) {
        if (it->get() == logger) {
            pImpl->loggers.erase(it);
            return true;
        }
    }
    return false;
}

void Logger::clearLoggers() {
    std::lock_guard<std::mutex> lock(pImpl->loggersMutex);
    pImpl->loggers.clear();
}

void Logger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock1(pImpl->loggersMutex);
    std::lock_guard<std::mutex> lock2(pImpl->contextMutex);
    
    pImpl->globalLevel = level;
    
    for (const auto& logger : pImpl->loggers) {
        logger->setLevel(level);
    }
}

void Logger::setLevel(const std::string& levelName) {
    LogLevel level = stringToLevel(levelName);
    setLevel(level);
}

LogLevel Logger::getLevel() const {
    std::lock_guard<std::mutex> lock(pImpl->loggersMutex);
    return pImpl->globalLevel;
}

void Logger::log(LogLevel level, const std::string& message,
                 const std::string& file, int line,
                 const std::string& function, const std::string& context) {
    logInternal(level, message, file, line, function, context);
}

void Logger::logWithMetadata(LogLevel level, const std::string& message,
                            const std::map<std::string, std::string>& metadata,
                            const std::string& file, int line,
                            const std::string& function) {
    logInternal(level, message, file, line, function, "", metadata);
}

void Logger::emergency(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Emergency, message, file, line, function, getCurrentContext());
}

void Logger::alert(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Alert, message, file, line, function, getCurrentContext());
}

void Logger::critical(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Critical, message, file, line, function, getCurrentContext());
}

void Logger::error(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Error, message, file, line, function, getCurrentContext());
}

void Logger::warning(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Warning, message, file, line, function, getCurrentContext());
}

void Logger::notice(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Notice, message, file, line, function, getCurrentContext());
}

void Logger::info(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Info, message, file, line, function, getCurrentContext());
}

void Logger::debug(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Debug, message, file, line, function, getCurrentContext());
}

void Logger::trace(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Trace, message, file, line, function, getCurrentContext());
}

void Logger::perf(const std::string& operation, double duration, int samples) {
    std::stringstream ss;
    ss << operation << ": " << duration << "s (avg: " << (duration / samples) << "s, samples: " << samples << ")";
    log(LogLevel::Perf, ss.str());
}

bool Logger::isEnabled(LogLevel level) const {
    return static_cast<int>(level) >= static_cast<int>(pImpl->globalLevel);
}

bool Logger::isEnabled(const std::string& levelName) const {
    LogLevel level = stringToLevel(levelName);
    return isEnabled(level);
}

size_t Logger::getLogCount(LogLevel level) const {
    size_t total = 0;
    std::lock_guard<std::mutex> lock(pImpl->loggersMutex);
    
    for (const auto& logger : pImpl->loggers) {
        total += logger->getLogCount(level);
    }
    
    return total;
}

size_t Logger::getTotalLogCount() const {
    size_t total = 0;
    std::lock_guard<std::mutex> lock(pImpl->loggersMutex);
    
    for (const auto& logger : pImpl->loggers) {
        total += logger->getTotalLogCount();
    }
    
    return total;
}

std::map<LogLevel, size_t> Logger::getLogStatistics() const {
    std::map<LogLevel, size_t> totalStats;
    std::lock_guard<std::mutex> lock(pImpl->loggersMutex);
    
    for (const auto& logger : pImpl->loggers) {
        std::map<LogLevel, size_t> loggerStats = logger->getLogStatistics();
        
        for (const auto& pair : loggerStats) {
            totalStats[pair.first] += pair.second;
        }
    }
    
    return totalStats;
}

Logger& Logger::getGlobal() {
    std::lock_guard<std::mutex> lock(globalLoggerMutex_);
    if (!globalLogger_) {
        globalLogger_ = std::make_shared<Logger>();
    }
    return *globalLogger_;
}

void Logger::setGlobal(std::shared_ptr<Logger> logger) {
    std::lock_guard<std::mutex> lock(globalLoggerMutex_);
    globalLogger_ = logger;
}

void Logger::pushContext(const std::string& context) {
    std::lock_guard<std::mutex> lock(pImpl->contextMutex);
    if (pImpl->maxDepth > 0 && pImpl->currentContext.empty()) {
        pImpl->currentContext = context;
    } else {
        // Handle nested contexts
        if (!pImpl->currentContext.empty()) {
            pImpl->currentContext += "::" + context;
        } else {
            pImpl->currentContext = context;
        }
    }
}

void Logger::popContext() {
    stdüche lock(pImpl->contextMutex);
    if (!pImpl->currentContext.empty()) {
        size_t lastPos = pImpl->currentContext.find_last_of("::");
        if (lastPos != std::string::npos) {
            pImpl->currentContext.erase(lastPos);
        } else {
            pImpl->currentContext.clear();
        }
    }
}

std::string Logger::getCurrentContext() const {
    std::lock_guard<std::mutex> lock(pImpl->contextMutex);
    return pImpl->currentContext;
}

void Logger::lock() {
    std::lock_guard<std::mutex> lock(pImpl->globalMutex);
    pImpl->locked = true;
}

void Logger::unlock() {
    std::lock_guard<std::mutex> lock(pImpl->globalMutex);
    pImpl->locked = false;
}

bool Logger::tryLock() {
    std::unique_lock<std::mutex> lock(pImpl->globalMutex, std::try_to_lock);
    if (lock.owns_lock()) {
        pImpl->locked = true;
        return true;
    }
    return false;
}

std::string Logger::levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::Off: return "OFF";
        case LogLevel::Emergency: return "EMERGENCY";
        case LogLevel::Alert: return "ALERT";
        case LogLevel::Critical: return "CRITICAL";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Warning: return "WARNING";
        case LogLevel::Notice: return "NOTICE";
        case LogLevel::Info: return "INFO";
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Trace: return "TRACE";
        case LogLevel::Perf: return "PERF";
        case LogLevel::Custom: return "CUSTOM";
        default: return "UNKNOWN";
    }
}

LogLevel Logger::stringToLevel(const std::string& levelName) {
    std::string upperName = levelName;
    std::transform(upperName.begin(), upperName.end(), upperName.begin(), ::toupper);
    
    if (upperName == "OFF") return LogLevel::Off;
    if (upperName == "EMERGENCY") return LogLevel::Emergency;
    if (upperName == "ALERT") return LogLevel::Alert;
    if (upperName == "CRITICAL") return LogLevel::Critical;
    if (upperName == "ERROR") return LogLevel::Error;
    if (upperName == "WARNING") return LogLevel::Warning;
    if (upperName == "NOTICE") return LogLevel::Notice;
    if (upperName == "INFO") return LogLevel::Info;
    if (upperName == "DEBUG") return LogLevel::Debug;
    if (upperName == "TRACE") return LogLevel::Trace;
    if (upperName == "PERF") return LogLevel::Perf;
    if (upperName == "CUSTOM") return LogLevel::Custom;
    
    return LogLevel::Info;
}

bool Logger::isValidLogLevel(const std::string& levelName) {
    return stringToLevel(levelName) != LogLevel::Info || levelName.empty();
}

void Logger::logInternal(LogLevel level, const std::string& message,
                         const std::string& file, int line,
                         const std::string& function,
                         const std::string& context,
                         const std::map<std::string, std::string>& metadata) {
    if (!isEnabled(level)) {
        return;
    }
    
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.context = context;
    entry.metadata = metadata;
    entry.threadId = std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    
    std::lock_guard<std::mutex> lock(pImpl->loggersMutex);
    
    for (const auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
}

std::string Logger::formatMessage(const LogEntry& entry) const {
    std::stringstream ss;
    
    // Thread ID
    ss << "[" << entry.threadId << "] ";
    
    // Timestamp
    ss << "[" << entry.getFormattedTimestamp() << "] ";
    
    // Level
    ss << "[" << levelToString(entry.level) << "] ";
    
    // File and line
    if (!entry.file.empty()) {
        ss << "(" << entry.file << ":" << entry.line << ") ";
    }
    
    // Function
    if (!entry.function.empty()) {
        ss << "{" << entry.function << "} ";
    }
    
    // Context
    if (!entry.context.empty()) {
        ss << "<" << entry.context << "> ";
    }
    
    // Metadata
    if (!entry.metadata.empty()) {
        ss << "[METADATA: ";
        bool first = true;
        for (const auto& pair : entry.metadata) {
            if (!first) ss << ", ";
            ss << pair.first << "=" << pair.second;
            first = false;
        }
        ss << "] ";
    }
    
    // Message
    ss << entry.message;
    
    return ss.str();
}

LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function), stopped_(false) {
}

LogStream::~LogStream() {
    if (!stopped_ && !oss_.str().empty()) {
        try {
            Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
        } catch (...) {
            // Silent catch to avoid exceptions in destructors
        }
    }
}

LogStream& LogStream::withMetadata(const std::string& key, const std::string& value) {
    metadata_[key] = value;
    return *this;
}

LogStream& LogStream::withContext(const std::string& context) {
    context_ = context;
    return *this;
}

LogStream& LogStream::withTag(const std::string& tag) {
    tags_.push_back(tag);
    return *this;
}

PerfProfiler::PerfProfiler(const std::string& file, int line, const std::string& function, const std::string& operation)
    : file_(file), line_(line), function_(function), operation_(operation), stopped_(false) {
    startTime_ = std::chrono::high_resolution_clock::now();
}

PerfProfiler::~PerfProfiler() {
    if (!stopped_) {
        stop();
    }
}

void PerfProfiler::stop() {
    if (!stopped_) {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime_);
        
        double durationMs = duration.count() / 1000.0;
        
        Logger::getGlobal().perf(operation_, durationMs, 1);
        
        stopped_ = true;
    }
}

double PerfProfiler::getDuration() const {
    if (stopped_) {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime_);
        return duration.count() / 1000.0;
    }
    return 0.0;
}

std::string PerfProfiler::getOperation() const {
    return operation_;
}

void initializeGlobalLogger() {
    Logger::getGlobal();
}

} // namespace nlm

// Initialize global logger at program start
void initializeNLMLogger() {
    Logger::getGlobal();
    
    // Set up default logging
    Logger::getGlobal().setLevel(LogLevel::Info);
    
    // Configure console logger
    auto globalLogger = Logger::getGlobal();
    for (const auto& logger : globalLogger->pImpl->loggers) {
        if (auto consoleLogger = std::dynamic_pointer_cast<ConsoleLogger>(logger)) {
            consoleLogger->setUseColors(true);
            consoleLogger->setLevelEnabled(LogLevel::Warning, true);
            consoleLogger->setLevelEnabled(LogLevel::Error, true);
            break;
        }
    }
}

void shutdownNLMLogger() {
    auto globalLogger = Logger::getGlobal();
    globalLogger->clearLoggers();
}