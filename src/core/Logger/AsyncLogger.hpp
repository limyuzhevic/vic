#include "AsyncLogger.hpp"
#include <thread>
#include <queue>
#include <condition_variable>
#include <atomic>
#include <chrono>

namespace nlm {

struct AsyncLogger::Impl {
    std::shared_ptr<ILogger> targetLogger;
    LogLevel level = LogLevel::Info;
    
    // Thread-safe queue
    std::queue<LogEntry> logQueue;
    std::mutex queueMutex;
    std::condition_variable queueCondition;
    std::atomic<bool> running{true};
    
    // Configuration
    size_t maxQueueSize = 10000;
    double flushInterval = 1.0; // seconds
    size_t processedCount = 0;
    
    // Worker thread
    std::thread workerThread;
    
    // Statistics
    std::atomic<size_t> droppedLogs{0};
};

AsyncLogger::AsyncLogger(std::shared_ptr<ILogger> targetLogger, size_t queueSize)
    : pImpl(std::make_unique<Impl>()) {
    pImpl->targetLogger = targetLogger;
    pImpl->maxQueueSize = queueSize;
    
    // Start worker thread
    pImpl->workerThread = std::thread(&AsyncLogger::workerFunction, this);
}

AsyncLogger::~AsyncLogger() {
    // Stop worker thread
    pImpl->running = false;
    pImpl->queueCondition.notify_one();
    
    if (pImpl->workerThread.joinable()) {
        pImpl->workerThread.join();
    }
}

void AsyncLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->queueMutex);
    
    // Check queue size
    if (pImpl->logQueue.size() >= pImpl->maxQueueSize) {
        // Drop oldest log entry if queue is full
        pImpl->logQueue.pop();
        pImpl->droppedLogs++;
    }
    
    pImpl->logQueue.push(entry);
    pImpl->queueCondition.notify_one();
}

void AsyncLogger::setLevel(LogLevel level) {
    pImpl->level = level;
}

LogLevel AsyncLogger::getLevel() const {
    return pImpl->level;
}

void AsyncLogger::setQueueSize(size_t size) {
    std::lock_guard<std::mutex> lock(pImpl->queueMutex);
    pImpl->maxQueueSize = size;
}

void AsyncLogger::setFlushInterval(double interval) {
    pImpl->flushInterval = interval;
}

void AsyncLogger::flush() {
    std::unique_lock<std::mutex> lock(pImpl->queueMutex);
    pImpl->queueCondition.wait(lock, [this]() {
        return pImpl->logQueue.empty();
    });
}

void AsyncLogger::workerFunction() {
    while (pImpl->running) {
        LogEntry entry;
        bool hasEntry = false;
        
        {
            std::unique_lock<std::mutex> lock(pImpl->queueMutex);
            pImpl->queueCondition.wait_for(lock, 
                std::chrono::duration<double>(pImpl->flushInterval),
                [this]() { return !pImpl->logQueue.empty() || !pImpl->running; }
            );
            
            if (!pImpl->logQueue.empty() && pImpl->running) {
                entry = pImpl->logQueue.front();
                pImpl->logQueue.pop();
                hasEntry = true;
            }
        }
        
        if (hasEntry) {
            pImpl->targetLogger->log(entry);
            pImpl->processedCount++;
        }
    }
}

} // namespace nlm
