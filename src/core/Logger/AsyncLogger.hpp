// Async logger implementation - Enhanced with queue
#pragma once

#include "Logger.hpp"
#include <thread>
#include <queue>
#include <condition_variable>
#include <atomic>
#include <chrono>
#include <functional>

namespace nlm {

class AsyncLogger : public BaseLogger {
public:
    AsyncLogger(size_t queueSize = 10000, size_t numThreads = 2);
    explicit AsyncLogger(LogLevel level, size_t queueSize = 10000, size_t numThreads = 2);
    ~AsyncLogger() override;
    
    void doCustomLog(const LogEntry& entry) override;
    void flush() override;
    
    // Backpressure handling
    void setMaxQueueSize(size_t maxSize);
    void waitForFlush();
    bool isQueueFull() const;
    
    // Performance metrics
    uint64_t getProcessedCount() const;
    uint64_t getDroppedCount() const;
    double getThroughput() const;
    
    // Thread control
    void stopThreads();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    void workerThread();
};

} // namespace nlm