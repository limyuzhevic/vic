#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <queue>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <deque>

#include "nlm/AdvancedLogger.hpp"
#include "nlm/Constants.hpp"

namespace nlm {

/**
 * Thread-safe logging queue with batch processing
 */
class BatchLogger {
public:
    // Configuration for batch logger
    struct Config {
        size_t batchSize = 100;
        std::chrono::milliseconds flushInterval{1000}; // 1 second
        bool enableFileLogging = false;
        std::string logFile;
        bool enableConsoleLogging = true;
        AdvancedLogger::Level level = AdvancedLogger::Level::Info;
        bool enableMetrics = true;
    };

    BatchLogger(const std::string& name = "BatchLogger", const Config& config = Config());
    ~BatchLogger();

    // Log a message
    void log(AdvancedLogger::Level level, const std::string& message,
            const std::map<std::string, std::string>& fields = {});

    // Log with formatted message
    template<typename... Args>
    void log(AdvancedLogger::Level level, const std::string& format, Args&&... args) {
        log(level, formatMessage(format, std::forward<Args>(args)...));
    }

    // Flush all pending logs
    void flush();

    // Stop the logger
    void stop();

    // Get statistics
    struct Statistics {
        uint64_t totalLogged = 0;
        uint64_t batchCount = 0;
        uint64_t droppedLogs = 0;
        uint64_t averageBatchSize = 0;
        std::chrono::milliseconds totalProcessingTime{0};
    };

    Statistics getStatistics() const;

    // Get current queue size
    size_t getQueueSize() const;

private:
    struct LogEntry {
        AdvancedLogger::Level level;
        std::chrono::system_clock::time_point timestamp;
        std::string message;
        std::map<std::string, std::string> fields;
    };

    std::string name;
    Config config;
    std::atomic<bool> running{true};
    std::atomic<size_t> queueSize{0};
    
    // Thread-safe queues
    std::queue<LogEntry> logQueue;
    std::mutex queueMutex;
    std::condition_variable queueCondition;
    
    // Worker thread
    std::thread workerThread;
    
    // Statistics
    mutable std::mutex statsMutex;
    Statistics stats;
    std::chrono::steady_clock::time_point lastFlushTime;
    
    // File logging
    std::unique_ptr<std::ofstream> logFile;
    
    // Batch processing
    std::deque<LogEntry> batch;
    std::mutex batchMutex;
    
    // Helper functions
    void workerLoop();
    void processBatch();
    std::string formatMessage(const std::string& format, std::vector<std::string> args);
    void writeToFile(const std::vector<LogEntry>& batch);
    void updateStatistics(size_t processedCount, auto processingTime);
};

} // namespace nlm
