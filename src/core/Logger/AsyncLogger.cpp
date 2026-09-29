// Async logger implementation - Enhanced with queue
#include "AsyncLogger.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>

namespace nlm {

struct AsyncLogger::Impl {
    std::queue<LogEntry> logQueue;
    std::mutex queueMutex;
    std::condition_variable queueCondition;
    std::vector<std::thread> workerThreads;
    std::atomic<bool> running;
    size_t maxQueueSize;
    size_t numThreads;
    
    // Performance metrics
    std::atomic<uint64_t> processedCount;
    std::atomic<uint64_t> droppedCount;
    std::chrono::system_clock::time_point startTime;
    
    std::shared_ptr<LogFormatter> formatter;
};

AsyncLogger::AsyncLogger(size_t queueSize, size_t numThreads) : BaseLogger() {
    pImpl.reset(new Impl);
    pImpl->maxQueueSize = queueSize;
    pImpl->numThreads = numThreads;
    pImpl->running = true;
    pImpl->processedCount = 0;
    pImpl->droppedCount = 0;
    pImpl->startTime = std::chrono::system_clock::now();
    pImpl->formatter = std::make_shared<DefaultFormatter>();
    
    // Start worker threads
    for (size_t i = 0; i < pImpl->numThreads; ++i) {
        pImpl->workerThreads.emplace_back(&AsyncLogger::workerThread, this);
    }
}

AsyncLogger::AsyncLogger(LogLevel level, size_t queueSize, size_t numThreads) : BaseLogger(level) {
    pImpl.reset(new Impl);
    pImpl->maxQueueSize = queueSize;
    pImpl->numThreads = numThreads;
    pImpl->running = true;
    pImpl->processedCount = 0;
    pImpl->droppedCount = 0;
    pImpl->startTime = std::chrono::system_clock::now();
    pImpl->formatter = std::make_shared<DefaultFormatter>();
    
    // Start worker threads
    for (size_t i = 0; i < pImpl->numThreads; ++i) {
        pImpl->workerThreads.emplace_back(&AsyncLogger::workerThread, this);
    }
}

AsyncLogger::~AsyncLogger() {
    stopThreads();
}

void AsyncLogger::doCustomLog(const LogEntry& entry) {
    std::unique_lock<std::mutex> lock(pImpl->queueMutex);
    
    // Check if queue is full
    if (pImpl->logQueue.size() >= pImpl->maxQueueSize) {
        // Queue is full, drop the entry or block based on configuration
        pImpl->droppedCount++;
        return;
    }
    
    // Add entry to queue
    pImpl->logQueue.push(entry);
    pImpl->queueCondition.notify_one();
    
    pImpl->processedCount++;
}

void AsyncLogger::flush() {
    std::unique_lock<std::mutex> lock(pImpl->queueMutex);
    
    // Wait for queue to be empty
    pImpl->queueCondition.wait(lock, [this]() {
        return pImpl->logQueue.empty();
    });
}

void AsyncLogger::setMaxQueueSize(size_t maxSize) {
    std::lock_guard<std::mutex> lock(pImpl->queueMutex);
    pImpl->maxQueueSize = maxSize;
}

void AsyncLogger::waitForFlush() {
    std::unique_lock<std::mutex> lock(pImpl->queueMutex);
    pImpl->queueCondition.wait(lock, [this]() {
        return pImpl->logQueue.empty();
    });
}

bool AsyncLogger::isQueueFull() const {
    std::lock_guard<std::mutex> lock(pImpl->queueMutex);
    return pImpl->logQueue.size() >= pImpl->maxQueueSize;
}

uint64_t AsyncLogger::getProcessedCount() const {
    return pImpl->processedCount.load();
}

uint64_t AsyncLogger::getDroppedCount() const {
    return pImpl->droppedCount.load();
}

double AsyncLogger::getThroughput() const {
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration<double>(now - pImpl->startTime).count();
    if (duration == 0) return 0.0;
    return static_cast<double>(pImpl->processedCount.load()) / duration;
}

void AsyncLogger::stopThreads() {
    {
        std::lock_guard<std::mutex> lock(pImpl->queueMutex);
        pImpl->running = false;
    }
    
    pImpl->queueCondition.notify_all();
    
    for (auto& thread : pImpl->workerThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    
    pImpl->workerThreads.clear();
}

void AsyncLogger::workerThread() {
    while (pImpl->running) {
        std::unique_lock<std::mutex> lock(pImpl->queueMutex);
        
        // Wait for item in queue or stop signal
        pImpl->queueCondition.wait(lock, [this]() {
            return !pImpl->logQueue.empty() || !pImpl->running;
        });
        
        if (!pImpl->running && pImpl->logQueue.empty()) {
            break;
        }
        
        // Process log entry
        LogEntry entry = std::move(pImpl->logQueue.front());
        pImpl->logQueue.pop();
        lock.unlock();
        
        // Simulate async processing (in real implementation, this could be file I/O, network, etc.)
        std::this_thread::sleep_for(std::chrono::microseconds(100));
        
        // Log the entry (using the formatter)
        std::string formatted = pImpl->formatter->format(entry);
        std::cout << formatted << std::endl;
        
        lock.lock();
    }
}

} // namespace nlm