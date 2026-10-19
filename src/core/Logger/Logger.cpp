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
#include <unordered_map>
#include <shared_mutex>

namespace nlm {

// Forward declarations for performance monitoring
namespace Performance {
    void logCount(const std::string& level, const std::string& message);
    double getLogRate();
}

// Optimized ConsoleLogger with batched logging and reduced contention
struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    
    // Performance optimizations
    mutable std::shared_mutex mutex;  // Shared mutex for read-heavy operations
    size_t logCount = 0;
    std::chrono::steady_clock::time_point lastLogTime;
    
    // Batched logging for performance
    static constexpr size_t BATCH_SIZE = 64;
    static constexpr size_t FLUSH_INTERVAL_MS = 10;
    
    struct LogEntry {
        LogLevel level;
        std::string message;
        std::string file;
        int line;
        std::string function;
        std::chrono::system_clock::time_point timestamp;
    };
    
    std::queue<LogEntry> logBatch;
    std::atomic<bool> batchProcessing{false};
    std::thread batchProcessorThread;
    std::condition_variable batchCV;
    std::mutex batchMutex;
    std::atomic<size_t> pendingLogs{0};
    
    // Performance monitoring
    std::unordered_map<LogLevel, size_t> levelCounts;
    std::chrono::steady_clock::time_point sessionStart;
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {
    pImpl->sessionStart = std::chrono::steady_clock::now();
    pImpl->lastLogTime = pImpl->sessionStart;
    
    // Start batch processing thread
    pImpl->batchProcessing = true;
    pImpl->batchProcessorThread = std::thread(&ConsoleLogger::batchProcessLogs, this);
}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
    pImpl->sessionStart = std::chrono::steady_clock::now();
    pImpl->lastLogTime = pImpl->sessionStart;
    
    // Start batch processing thread
    pImpl->batchProcessing = true;
    pImpl->batchProcessorThread = std::thread(&ConsoleLogger::batchProcessLogs, this);
}

ConsoleLogger::~ConsoleLogger() {
    // Stop batch processing
    pImpl->batchProcessing = false;
    pImpl->batchCV.notify_all();
    
    if (pImpl->batchProcessorThread.joinable()) {
        pImpl->batchProcessorThread.join();
    }
}

void ConsoleLogger::log(const LogEntry& entry) {
    // Fast path for disabled logging
    if (entry.level < pImpl->level) {
        return;
    }
    
    // Update performance counters
    {
        std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
        pImpl->logCount++;
        pImpl->levelCounts[entry.level]++;
        pImpl->lastLogTime = std::chrono::steady_clock::now();
    }
    
    // Add to batch for processing
    {
        std::lock_guard<std::mutex> lock(pImpl->batchMutex);
        pImpl->logBatch.push(entry);
        size_t batchSize = pImpl->logBatch.size();
        
        // Flush if batch is full or time interval exceeded
        if (batchSize >= Impl::BATCH_SIZE || 
            (std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - pImpl->lastLogTime).count() >= 
             Impl::FLUSH_INTERVAL_MS)) {
            pImpl->batchCV.notify_one();
        }
        
        pendingLogs.fetch_add(1, std::memory_order_relaxed);
    }
}

void ConsoleLogger::batchProcessLogs() {
    while (pImpl->batchProcessing || pImpl->pendingLogs.load(std::memory_order_relaxed) > 0) {
        std::unique_lock<std::mutex> lock(pImpl->batchMutex);
        
        // Wait for logs or timeout
        if (pImpl->logBatch.empty()) {
            auto timeout = std::chrono::milliseconds(Impl::FLUSH_INTERVAL_MS);
            if (pImpl->batchCV.wait_for(lock, timeout, 
                                      [&]() { return !pImpl->logBatch.empty() || !pImpl->batchProcessing; })) {
                if (!pImpl->batchProcessing && pImpl->logBatch.empty()) {
                    break;
                }
            }
        }
        
        if (!pImpl->logBatch.empty()) {
            // Process batch of logs
            std::vector<LogEntry> batch;
            batch.reserve(std::min(Impl::BATCH_SIZE, pImpl->logBatch.size()));
            
            while (!pImpl->logBatch.empty() && batch.size() < Impl::BATCH_SIZE) {
                batch.push_back(std::move(pImpl->logBatch.front()));
                pImpl->logBatch.pop();
            }
            
            // Process batch
            processLogBatch(batch);
            
            // Update pending count
            pendingLogs.fetch_sub(batch.size(), std::memory_order_relaxed);
        }
    }
}

void ConsoleLogger::processLogBatch(const std::vector<LogEntry>& batch) {
    if (batch.empty()) return;
    
    // Build output strings efficiently
    std::vector<std::string> outputs;
    outputs.reserve(batch.size());
    
    for (const auto& entry : batch) {
        std::ostringstream oss;
        oss << "[" << levelToString(entry.level) << "] ";
        
        // Add timestamp
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        
        oss << " " << entry.message;
        outputs.push_back(oss.str());
    }
    
    // Output to console and file
    {
        std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
        
        for (const auto& output : outputs) {
            // Console output
            if (pImpl->useColors) {
                std::cout << levelToColor(batch[0].level) << output << "\033[0m\n";
            } else {
                std::cout << output << "\n";
            }
        }
        
        // File output
        if (pImpl->fileOut.is_open()) {
            for (const auto& output : outputs) {
                pImpl->fileOut << output << "\n";
            }
            // Flush periodically to avoid I/O bottleneck
            if (pImpl->logCount % 100 == 0) {
                pImpl->fileOut.flush();
            }
        }
    }
}

void ConsoleLogger::setLevel(LogLevel level) {
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    pImpl->level = level;
}

LogLevel ConsoleLogger::getLevel() const {
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    return pImpl->level;
}

void ConsoleLogger::setUseColors(bool useColors) {
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    pImpl->useColors = useColors;
}

bool ConsoleLogger::getUseColors() const {
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    return pImpl->useColors;
}

void ConsoleLogger::setOutputFile(const std::string& filepath) {
    std::unique_lock<std::shared_mutex> lock(pImpl->mutex);
    pImpl->fileOut.open(filepath, std::ios::app);
}

void ConsoleLogger::closeOutputFile() {
    std::unique_lock<std::shared_mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

// Static helper functions
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
    mutable std::shared_mutex mutex;  // Shared mutex for read-heavy operations
    
    // Performance monitoring
    size_t totalLogCount = 0;
    size_t batchSize = 0;
    double avgLogRate = 0.0;
    std::chrono::steady_clock::time_point sessionStart;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {
    pImpl->sessionStart = std::chrono::steady_clock::now();
}

Logger::~Logger() = default;

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    std::unique_lock<std::shared_mutex> lock(pImpl->mutex);
    pImpl->loggers.push_back(logger);
}

void Logger::clearLoggers() {
    std::unique_lock<std::shared_mutex> lock(pImpl->mutex);
    pImpl->loggers.clear();
}

void Logger::setLevel(LogLevel level) {
    std::unique_lock<std::shared_mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        logger->setLevel(level);
    }
}

void Logger::log(LogLevel level, const std::string& message,
                const std::string& file, int line, const std::string& function) {
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
    
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    
    // Update performance counters
    pImpl->totalLogCount++;
    pImpl->batchSize++;
    
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
    std::shared_lock<std::shared_mutex> lock(pImpl->mutex);
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

// Performance monitoring functions
namespace Performance {
    void logCount(const std::string& level, const std::string& message) {
        // TODO: Implement performance monitoring
    }
    
    double getLogRate() {
        // TODO: Implement log rate calculation
        return 0.0;
    }
}

// LogStream implementation
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function) {}

LogStream::~LogStream() {
    Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_);
}

} // namespace nlm
