#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <chrono>
#include <thread>
#include <atomic>
#include <queue>
#include <string>
#include <vector>
#include <regex>

namespace nlm {

struct LogFilter {
    std::vector<std::string> includePatterns;
    std::vector<std::string> excludePatterns;
    std::vector<LogLevel> includeLevels;
    bool includeAllMessages = false;
    
    bool shouldLog(const LogEntry& entry) const {
        if (!includeAllMessages && !includeLevels.empty()) {
            bool levelMatches = std::find(includeLevels.begin(), includeLevels.end(), entry.level) != includeLevels.end();
            if (!levelMatches) return false;
        }
        
        for (const auto& pattern : excludePatterns) {
            if (std::regex_search(entry.message, std::regex(pattern, std::regex::icase))) {
                return false;
            }
        }
        
        if (!includePatterns.empty()) {
            bool matchesAny = false;
            for (const auto& pattern : includePatterns) {
                if (std::regex_search(entry.message, std::regex(pattern, std::regex::icase))) {
                    matchesAny = true;
                    break;
                }
            }
            if (!matchesAny) return false;
        }
        
        return true;
    }
};

struct AsyncConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    std::queue<LogEntry> messageQueue;
    std::atomic<bool> running{true};
    std::thread workerThread;
    size_t maxQueueSize = 1000;
    LogFilter filter;
    
    AsyncConsoleLogger::Impl() {
        workerThread = std::thread(&Impl::workerLoop, this);
    }
    
    ~Impl() {
        running = false;
        if (workerThread.joinable()) {
            workerThread.join();
        }
    }
    
    void workerLoop() {
        while (running) {
            std::unique_lock<std::mutex> lock(mutex);
            if (messageQueue.empty()) {
                lock.unlock();
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                continue;
            }
            
            LogEntry entry = std::move(messageQueue.front());
            messageQueue.pop();
            lock.unlock();
            
            processEntry(entry);
        }
    }
    
    void processEntry(const LogEntry& entry) {
        if (entry.level < level) {
            return;
        }
        
        if (!filter.shouldLog(entry)) {
            return;
        }
        
        std::lock_guard<std::mutex> lock(mutex);
        
        std::ostringstream oss;
        oss << "[" << levelToString(entry.level) << "] ";
        
        // Add timestamp with milliseconds
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            entry.timestamp.time_since_epoch()) % 1000;
        oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S")
            << "." << std::setfill('0') << std::setw(3) << ms.count();
        
        // Add thread ID for debugging
        oss << " [thread:" << std::this_thread::get_id() << "]";
        
        // Add source location if available
        if (!entry.file.empty() && entry.line > 0) {
            oss << " [" << entry.file << ":" << entry.line << "]";
        }
        
        // Add function name if available
        if (!entry.function.empty()) {
            oss << " [" << entry.function << "]";
        }
        
        oss << " " << entry.message;
        
        std::string output = oss.str();
        
        // Console output
        if (useColors) {
            std::cout << levelToColor(entry.level) << output << "\033[0m\n";
        } else {
            std::cout << output << "\n";
        }
        
        // File output if set
        if (fileOut.is_open()) {
            fileOut << output << "\n";
            fileOut.flush();
        }
    }
    
    void addMessage(LogEntry entry) {
        std::lock_guard<std::mutex> lock(mutex);
        if (messageQueue.size() < maxQueueSize) {
            messageQueue.push(std::move(entry));
        } else {
            // Drop oldest message if queue is full
            messageQueue.pop();
            messageQueue.push(std::move(entry));
        }
    }
};

AsyncConsoleLogger::AsyncConsoleLogger() : pImpl(std::make_unique<Impl>()) {}

AsyncConsoleLogger::AsyncConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
}

AsyncConsoleLogger::~AsyncConsoleLogger() = default;

void AsyncConsoleLogger::log(const LogEntry& entry) {
    LogEntry copy = entry;
    pImpl->addMessage(std::move(copy));
}

void AsyncConsoleLogger::setLevel(LogLevel level) {
    pImpl->level = level;
}

LogLevel AsyncConsoleLogger::getLevel() const {
    return pImpl->level;
}

void AsyncConsoleLogger::setUseColors(bool useColors) {
    pImpl->useColors = useColors;
}

bool AsyncConsoleLogger::getUseColors() const {
    return pImpl->useColors;
}

void AsyncConsoleLogger::setOutputFile(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->fileOut.open(filepath, std::ios::app);
}

void AsyncConsoleLogger::closeOutputFile() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

void AsyncConsoleLogger::setMaxQueueSize(size_t maxSize) {
    pImpl->maxQueueSize = maxSize;
}

void AsyncConsoleLogger::addIncludePattern(const std::string& pattern) {
    pImpl->filter.includePatterns.push_back(pattern);
}

void AsyncConsoleLogger::addExcludePattern(const std::string& pattern) {
    pImpl->filter.excludePatterns.push_back(pattern);
}

void AsyncConsoleLogger::addIncludeLevel(LogLevel level) {
    pImpl->filter.includeLevels.push_back(level);
}

void AsyncConsoleLogger::setIncludeAllMessages(bool include) {
    pImpl->filter.includeAllMessages = include;
}

std::string AsyncConsoleLogger::Impl::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Critical: return "CRIT";
        default: return "UNKNOWN";
    }
}

std::string AsyncConsoleLogger::Impl::levelToColor(LogLevel level) const {
    switch (level) {
        case LogLevel::Debug: return "\033[36m";    // Cyan
        case LogLevel::Info: return "\033[32m";      // Green
        case LogLevel::Warning: return "\033[33m";   // Yellow
        case LogLevel::Error: return "\033[31m";     // Red
        case LogLevel::Critical: return "\033[35m"; // Magenta
        default: return "\033[0m";
    }
}

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
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
    
    // Add timestamp with milliseconds
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        entry.timestamp.time_since_epoch()) % 1000;
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S")
        << "." << std::setfill('0') << std::setw(3) << ms.count();
    
    // Add source location if available
    if (!entry.file.empty() && entry.line > 0) {
        oss << " [" << entry.file << ":" << entry.line << "]";
    }
    
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
    pImpl->fileOut.open(filepath, std::ios::app);
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

// Logger class implementation
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
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
    for (auto& logger : pImpl->loggers) {
        logger->setLevel(level);
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
    
    for (auto& logger : pImpl->loggers) {
        logger->log(entry);
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

bool Logger::isEnabled(LogLevel level) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        if (logger->getLevel() <= level) {
            return true;
        }
    }
    return false;
}

static std::shared_ptr<Logger>& globalLoggerPtr() {
    static std::shared_ptr<Logger> ptr = std::make_shared<Logger>();
    return ptr;
}

Logger& Logger::getGlobal() {
    return *globalLoggerPtr();
}

void Logger::setGlobal(std::shared_ptr<Logger> logger) {
    if (logger) {
        globalLoggerPtr() = logger;
    }
}

// LogStream implementation
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function) {}

LogStream::~LogStream() {
    Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
}

} // namespace nlm
