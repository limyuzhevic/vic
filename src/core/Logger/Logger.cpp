#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>

namespace nlm {

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    
    // Memory leak detection
    static inline std::atomic<size_t> instances{0};
    static inline std::atomic<size_t> peakInstances{0};
    
    Impl() { 
        instances++; 
        if (instances > peakInstances) peakInstances = instances; 
    }
    
    ~Impl() { instances--; }
    
    static size_t getInstanceCount() { return instances.load(); }
    static size_t getPeakInstanceCount() { return peakInstances.load(); }
    static void resetStats() { instances = 0; peakInstances = 0; }
};

// Custom deleter for thread-safe cleanup
struct ConsoleLogger::ImplDeleter {
    std::mutex& getMutex;
    
    explicit ImplDeleter(std::mutex& mutex) : getMutex(mutex) {}
    
    void operator()(Impl* impl) {
        if (impl) {
            // Close file safely before deletion
            if (impl->fileOut.is_open()) {
                std::lock_guard<std::mutex> lock(getMutex);
                impl->fileOut.close();
            }
            delete impl;
        }
    }
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
    
    // Add timestamp
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    
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

void ConsoleLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    // Thread-safe reference counting for read operations
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "[" << levelToString(entry.level) << "] ";
    
    // Add timestamp
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    
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
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->level = level;
}

LogLevel ConsoleLogger::getLevel() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->level;
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

// Global logger statistics for memory leak detection
namespace {
    struct LoggerStats {
        std::atomic<size_t> instances{0};
        std::atomic<size_t> peakInstances{0};
        std::mutex mutex;
        
        void increment() {
            std::lock_guard<std::mutex> lock(mutex);
            instances++;
            if (instances > peakInstances) peakInstances = instances;
        }
        
        void decrement() {
            std::lock_guard<std::mutex> lock(mutex);
            instances--;
        }
        
        size_t getInstanceCount() const {
            std::lock_guard<std::mutex> lock(mutex);
            return instances.load();
        }
        
        size_t getPeakInstanceCount() const {
            std::lock_guard<std::mutex> lock(mutex);
            return peakInstances.load();
        }
        
        void reset() {
            std::lock_guard<std::mutex> lock(mutex);
            instances = 0;
            peakInstances = 0;
        }
    };
    
    LoggerStats loggerStats;
}}

// Logger class implementation with enhanced RAII
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
    
    Impl() {
        loggerStats.increment();
    }
    
    ~Impl() {
        loggerStats.decrement();
    }
    
    static size_t getInstanceCount() {
        return loggerStats.getInstanceCount();
    }
    
    static size_t getPeakInstanceCount() {
        return loggerStats.getPeakInstanceCount();
    }
    
    static void resetStats() {
        loggerStats.reset();
    }
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

// Global helper functions for memory leak detection
namespace nlm {
    size_t getLoggerInstanceCount() {
        return Logger::Impl::getInstanceCount();
    }
    
    size_t getLoggerPeakInstanceCount() {
        return Logger::Impl::getPeakInstanceCount();
    }
    
    void resetLoggerStats() {
        Logger::Impl::resetStats();
    }
    
    void checkMemoryLeaks() {
        if (Logger::Impl::getInstanceCount() > 0) {
            getGlobal().warning("Memory leaks detected in Logger system: " + 
                              std::to_string(Logger::Impl::getInstanceCount()) + " instances still active");
        }
    }
}} // namespace nlm
