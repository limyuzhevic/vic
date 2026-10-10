#include "AsyncLogger.hpp"
#include "LogEntry.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <thread>
#include <unordered_set>

namespace nlm {

struct AsyncLogger::Impl {
    std::queue<LogEntry> entryQueue;
    std::mutex queueMutex;
    std::condition_variable queueCond;
    bool stop = false;
    bool async = true;
    LogLevel level = LogLevel::INFO;
    size_t maxQueueSize = 10000;
    size_t workerThreads = 2;
    size_t batchSize = 100;
    std::chrono::milliseconds flushInterval = std::chrono::milliseconds(100);
    
    std::vector<std::shared_ptr<ILogDestination>> destinations;
    std::vector<std::thread> workers;
    std::atomic<size_t> queueSize = 0;
    uint64_t processedCount = 0;
    std::chrono::steady_clock::time_point lastFlush = std::chrono::steady_clock::now();
};

AsyncLogger::AsyncLogger(size_t maxQueueSize, size_t workerThreads) : pImpl(std::make_unique<Impl>()) {
    pImpl->maxQueueSize = maxQueueSize;
    pImpl->workerThreads = workerThreads;
    
    for (size_t i = 0; i < workerThreads; ++i) {
        pImpl->workers.emplace_back([this]() {
            while (!pImpl->stop) {
                std::unique_lock<std::mutex> lock(pImpl->queueMutex);
                
                // Wait for entry or timeout
                auto timeout = pImpl->flushInterval;
                if (pImpl->entryQueue.empty()) {
                    if (pImpl->queueCond.wait_for(lock, timeout, [this]() { return pImpl->stop || !pImpl->entryQueue.empty(); })) {
                        continue;
                    }
                }
                
                if (pImpl->stop && pImpl->entryQueue.empty()) {
                    break;
                }
                
                // Process batch
                std::vector<LogEntry> batch;
                batch.reserve(pImpl->batchSize);
                
                while (!pImpl->entryQueue.empty() && batch.size() < pImpl->batchSize) {
                    batch.push_back(std::move(pImpl->entryQueue.front()));
                    pImpl->entryQueue.pop();
                }
                
                lock.unlock();
                
                // Process batch
                for (auto& entry : batch) {
                    for (auto& dest : pImpl->destinations) {
                        if (dest->isEnabled(entry.level)) {
                            dest->write(entry);
                        }
                    }
                }
            }
        });
    }
}

AsyncLogger::~AsyncLogger() {
    {
        std::lock_guard<std::mutex> lock(pImpl->queueMutex);
        pImpl->stop = true;
    }
    
    pImpl->queueCond.notify_all();
    
    for (auto& worker : pImpl->workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
    
    flush();
}

void AsyncLogger::log(const LogEntry& entry) {
    if (!pImpl->async) {
        for (auto& dest : pImpl->destinations) {
            if (dest->isEnabled(entry.level)) {
                dest->write(entry);
            }
        }
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->queueMutex);
    
    if (pImpl->entryQueue.size() >= pImpl->maxQueueSize) {
        pImpl->entryQueue.pop();
    }
    
    pImpl->entryQueue.push(entry);
    pImpl->queueCond.notify_one();
}

void AsyncLogger::flush() {
    std::lock_guard<std::mutex> lock(pImpl->queueMutex);
    
    // Process remaining entries synchronously
    while (!pImpl->entryQueue.empty()) {
        LogEntry entry = std::move(pImpl->entryQueue.front());
        pImpl->entryQueue.pop();
        
        for (auto& dest : pImpl->destinations) {
            if (dest->isEnabled(entry.level)) {
                dest->write(entry);
            }
        }
    }
    
    for (auto& dest : pImpl->destinations) {
        dest->flush();
    }
}

void AsyncLogger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->queueMutex);
    pImpl->level = level;
    
    for (auto& dest : pImpl->destinations) {
        dest->setLevel(level);
    }
}

LogLevel AsyncLogger::getLevel() const {
    return pImpl->level;
}

bool AsyncLogger::isEnabled(LogLevel level) const {
    return level >= pImpl->level;
}

void AsyncLogger::addDestination(std::shared_ptr<ILogDestination> destination) {
    std::lock_guard<std::mutex> lock(pImpl->queueMutex);
    pImpl->destinations.push_back(destination);
    destination->setLevel(pImpl->level);
}

void AsyncLogger::setAsync(bool enabled) {
    std::lock_guard<std::mutex> lock(pImpl->queueMutex);
    pImpl->async = enabled;
}

void AsyncLogger::setBatchSize(size_t batchSize) {
    std::lock_guard<std::mutex> lock(pImpl->queueMutex);
    pImpl->batchSize = batchSize;
}

void AsyncLogger::setFlushInterval(std::chrono::milliseconds interval) {
    std::lock_guard<std::mutex> lock(pImpl->queueMutex);
    pImpl->flushInterval = interval;
}

} // namespace nlm