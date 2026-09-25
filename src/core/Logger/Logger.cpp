#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <ctime>
#include <string>
#include <unordered_map>
#include <chrono>
#include <vector>
#include <optional>
#include <regex>
#include <thread>
#include <condition_variable>
#include <queue>

namespace nlm {

// Thread-local buffer for thread-safe time formatting
static thread_local std::tm threadLocalTime;

// Helper function to safely get local time
static std::tm& getLocalTime(time_t time) {
    std::tm& result = threadLocalTime;
    // Use localtime_s if available (MSVC), otherwise use gmtime_r for POSIX
#if defined(_WIN32) && !defined(__CYGWIN__)
    localtime_s(&result, &time);
#else
    localtime_r(&time, &result);
#endif
    return result;
}

struct LogEntryExtra {
    std::unordered_map<std::string, std::string> keyValues;
    std::string structuredFormat; // "json", "keyvalue", "default"
};

struct LogSampler {
    double probability = 1.0; // Probability (0.0 to 1.0)
    uint64_t interval = 0; // Number of logs between samples
    uint64_t counter = 0;
};

struct LogFilter {
    std::regex pattern;
    bool include = true;
    LogLevel minLevel = LogLevel::Debug;
};

struct LogRotationInfo {
    uint64_t maxSize = 0; // 0 means no limit
    uint64_t currentSize = 0;
    uint32_t maxFiles = 1;
    std::string extension = ".log";
};

struct AsyncLogger {
    std::queue<LogEntry> logQueue;
    std::mutex queueMutex;
    std::condition_variable queueCV;
    std::thread workerThread;
    std::atomic<bool> running{false};
    std::function<void(const LogEntry&)> callback;
    std::atomic<uint32_t> batchSize{100};
    
    AsyncLogger() {
        running = true;
        workerThread = std::thread(&AsyncLogger::processLogs, this);
    }
    
    ~AsyncLogger() {
        running = false;
        queueCV.notify_one();
        if (workerThread.joinable()) {
            workerThread.join();
        }
    }
    
    void processLogs() {
        while (running) {
            std::unique_lock<std::mutex> lock(queueMutex);
            if (logQueue.empty()) {
                queueCV.wait_for(lock, std::chrono::milliseconds(100));
                continue;
            }
            
            LogEntry entry = logQueue.front();
            logQueue.pop();
            lock.unlock();
            
            if (callback) {
                callback(entry);
            }
        }
    }
    
    void addLog(const LogEntry& entry) {
        std::lock_guard<std::mutex> lock(queueMutex);
        logQueue.push(entry);
        queueCV.notify_one();
    }
};

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    LogRotationInfo rotationInfo;
    LogSampler sampler;
    std::vector<LogFilter> filters;
    LogEntryExtra extraInfo;
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
}

ConsoleLogger::~ConsoleLogger() = default;

void ConsoleLogger::log(const LogEntry& entry) {
        // Check if sampling allows this log
        if (pImpl->sampler.probability < 1.0) {
            std::mt19937 rng(std::chrono::system_clock::now().time_since_epoch().count());
            std::uniform_real_distribution<double> dist(0.0, 1.0);
            if (dist(rng) > pImpl->sampler.probability) {
                return;
            }
        }
        
        // Check interval sampling
        if (pImpl->sampler.interval > 0) {
            if (++pImpl->sampler.counter % pImpl->sampler.interval != 0) {
                return;
            }
        }
        
        // Check pattern filters
        bool passesFilters = true;
        for (const auto& filter : pImpl->filters) {
            if (entry.level < filter.minLevel) {
                passesFilters = false;
                break;
            }
            
            bool matches = std::regex_search(entry.message, filter.pattern);
            if (filter.include != matches) {
                passesFilters = false;
                break;
            }
        }
        
        if (!passesFilters) {
            return;
        }
        
        // Check log level filter
        if (entry.level < pImpl->level) {
            return;
        }
        
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        // Use thread-safe time formatting
        std::ostringstream oss;
        oss << "[" << levelToString(entry.level) << "] ";
        
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        std::tm timeInfo = getLocalTime(time);
        oss << std::put_time(&timeInfo, "%Y-%m-%d %H:%M:%S");
        
        oss << " " << entry.message;
        
        std::string output = oss.str();
        
        // Console output
        if (pImpl->useColors) {
            std::cout << levelToColor(entry.level) << output << "\033[0m\n";
        } else {
            std::cout << output << "\n";
        }
        
        // File output with rotation
        if (pImpl->fileOut.is_open()) {
            pImpl->fileOut << output << "\n";
            pImpl->fileOut.flush();
            
            // Check rotation
            if (pImpl->rotationInfo.maxSize > 0) {
                pImpl->rotationInfo.currentSize += output.size() + 1; // +1 for newline
                if (pImpl->rotationInfo.currentSize >= pImpl->rotationInfo.maxSize) {
                    rotateLogFile();
                }
            }
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

void ConsoleLogger::rotateLogFile() {
        if (!pImpl->fileOut.is_open()) return;
        
        pImpl->fileOut.close();
        
        // Close existing file
        pImpl->fileOut.close();
        
        // Rotate existing files
        for (int i = pImpl->rotationInfo.maxFiles - 1; i >= 0; --i) {
            std::string src = (i > 0) ? 
                ("" + pImpl->fileOut.close()) : 
                pImpl->fileOut.close();
            std::string dst = src + "." + std::to_string(i + 1);
            if (i > 0 && std::filesystem::exists(src)) {
                std::filesystem::rename(src, dst);
            }
        }
        
        // Create new file
        pImpl->fileOut.open(pImpl->fileOut.close(), std::ios::app);
        pImpl->rotationInfo.currentSize = 0;
    }
    
    void ConsoleLogger::setSampler(double probability, uint64_t interval) {
        pImpl->sampler.probability = probability;
        pImpl->sampler.interval = interval;
        pImpl->sampler.counter = 0;
    }
    
    void ConsoleLogger::addFilter(const std::string& pattern, bool include, LogLevel minLevel) {
        LogFilter filter;
        filter.pattern = std::regex(pattern);
        filter.include = include;
        filter.minLevel = minLevel;
        pImpl->filters.push_back(filter);
    }
    
    void ConsoleLogger::setLogRotation(uint64_t maxSize, uint32_t maxFiles) {
        pImpl->rotationInfo.maxSize = maxSize;
        pImpl->rotationInfo.maxFiles = maxFiles;
        pImpl->rotationInfo.currentSize = 0;
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

Logger& Logger::getGlobal() {
    static Logger globalLogger;
    return globalLogger;
}

void Logger::setGlobal(std::shared_ptr<Logger> logger) {
    // This is a placeholder for advanced global logger management
    // In practice, we just use getGlobal() which is already static
}

// LogStream implementation
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function) {}

LogStream::~LogStream() {
    Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
}

} // namespace nlm
