#pragma once

#include <string>
#include <chrono>
#include <memory>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <functional>
#include <map>
#include <deque>
#include <shared_mutex>
#include "Enhanced/LogFormatter.hpp"
#include "Enhanced/LogFilter.hpp"

namespace nlm {

// Log processing priority levels
enum class LogPriority {
    Low = 0,
    Normal = 1,
    High = 2,
    Critical = 3
};

// Circular log buffer for memory-constrained environments
class CircularLogBuffer {
public:
    CircularLogBuffer(size_t capacity) : capacity_(capacity), size_(0), head_(0), tail_(0) {}
    
    void add(const EnhancedLogEntry& entry) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Convert entry to string representation
        std::stringstream ss;
        ss << "[" << levelToString(entry.level) << "] "
           << formatTimestamp(entry.timestamp) << " "
           << entry.message << "\n";
        
        std::string logLine = ss.str();
        
        // Add to buffer
        if (size_ == capacity_) {
            // Buffer is full, overwrite oldest entry
            tail_ = (tail_ + 1) % capacity_;
        }
        
        buffer_[head_] = logLine;
        head_ = (head_ + 1) % capacity_;
        
        if (size_ < capacity_) {
            size_++;
        }
    }
    
    std::vector<std::string> getAllLogs() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<std::string> result;
        result.reserve(size_);
        
        size_t current = tail_;
        for (size_t i = 0; i < size_; ++i) {
            result.push_back(buffer_[current]);
            current = (current + 1) % capacity_;
        }
        
        return result;
    }
    
    std::vector<std::string> getRecentLogs(size_t count) const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<std::string> result;
        result.reserve(std::min(count, size_));
        
        size_t start = (tail_ + size_ - std::min(count, size_)) % capacity_;
        size_t current = start;
        for (size_t i = 0; i < std::min(count, size_); ++i) {
            result.push_back(buffer_[current]);
            current = (current + 1) % capacity_;
        }
        
        return result;
    }
    
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        size_ = 0;
        head_ = 0;
        tail_ = 0;
    }
    
    size_t getSize() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return size_;
    }
    
    size_t getCapacity() const { return capacity_; }
    
private:
    std::string formatTimestamp(const std::chrono::system_clock::time_point& timestamp) const {
        auto time = std::chrono::system_clock::to_time_t(timestamp);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S.%f");
        return ss.str();
    }
    
    std::vector<std::string> buffer_;
    size_t capacity_;
    size_t size_;
    size_t head_;
    size_t tail_;
    mutable std::mutex mutex_;
};

// Async log processor with thread pool
class AsyncLogProcessor {
public:
    AsyncLogProcessor(size_t numThreads = std::thread::hardware_concurrency())
        : stop_(false), processedCount_(0), errorCount_(0) {
        for (size_t i = 0; i < numThreads; ++i) {
            workers_.emplace_back(&AsyncLogProcessor::workerThread, this, i);
        }
    }
    
    ~AsyncLogProcessor() {
        stop_ = true;
        condition_.notify_all();
        for (auto& worker : workers_) {
            if (worker.joinable()) {
                worker.join();
            }
        }
    }
    
    void addLog(std::unique_ptr<EnhancedLogEntry> entry, LogPriority priority = LogPriority::Normal) {
        // Apply filters if any
        if (filter_) {
            if (!filter_->shouldLog(*entry)) {
                return;
            }
        }
        
        // Add to queue based on priority
        {
            std::lock_guard<std::mutex> lock(queueMutex_);
            logQueue_.push({std::move(entry), priority});
        }
        
        condition_.notify_one();
    }
    
    void addFormatter(std::unique_ptr<LogFormatter> formatter) {
        std::lock_guard<std::mutex> lock(formatterMutex_);
        formatters_.push_back(std::move(formatter));
    }
    
    void setFilter(std::unique_ptr<LogFilter> filter) {
        std::lock_guard<std::mutex> lock(filterMutex_);
        filter_ = std::move(filter);
    }
    
    void setOutputCallback(std::function<void(const std::string&)> callback) {
        outputCallback_ = std::move(callback);
    }
    
    size_t getProcessedCount() const { return processedCount_.load(); }
    size_t getErrorCount() const { return errorCount_.load(); }
    
    void waitForCompletion() {
        std::unique_lock<std::mutex> lock(queueMutex_);
        completionCondition_.wait(lock, [this] { return logQueue_.empty() && processingCount_ == 0; });
    }
    
private:
    struct LogItem {
        std::unique_ptr<EnhancedLogEntry> entry;
        LogPriority priority;
    };
    
    void workerThread(size_t threadId) {
        while (!stop_) {
            std::unique_lock<std::mutex> lock(queueMutex_);
            condition_.wait(lock, [this] {
                return !logQueue_.empty() || stop_;
            });
            
            if (stop_) break;
            
            // Get next log entry
            LogItem item = std::move(logQueue_.front());
            logQueue_.pop();
            lock.unlock();
            
            // Process the log entry
            try {
                processLogEntry(std::move(item.entry), item.priority);
                processedCount_++;
            } catch (const std::exception& e) {
                errorCount_++;
                // Log processing error
                if (outputCallback_) {
                    outputCallback_("Log processing error: " + std::string(e.what()));
                }
            }
        }
    }
    
    void processLogEntry(std::unique_ptr<EnhancedLogEntry> entry, LogPriority priority) {
        // Determine processing order based on priority
        std::vector<std::unique_ptr<LogFormatter>> formatters;
        {
            std::lock_guard<std::mutex> lock(formatterMutex_);
            formatters = std::move(formatters_);
            // Store for next processing cycle
            formatters_ = std::move(storedFormatters_);
            storedFormatters_ = std::move(formatters);
        }
        
        // Apply all formatters
        for (const auto& formatter : formatters) {
            std::string formatted = formatter->format(*entry);
            
            // Output the formatted log
            if (outputCallback_) {
                outputCallback_(formatted);
            }
            
            // Store in circular buffer if configured
            if (circularBuffer_) {
                // Store the raw entry for later analysis
                circularBuffer_->add(*entry);
            }
        }
        
        // Store formatters for next cycle
        if (!formatters.empty()) {
            std::lock_guard<std::mutex> lock(formatterMutex_);
            for (auto& formatter : formatters) {
                storedFormatters_.push_back(std::move(formatter));
            }
        }
    }
    
    std::vector<std::thread> workers_;
    std::queue<LogItem> logQueue_;
    std::mutex queueMutex_;
    std::condition_variable condition_;
    std::mutex formatterMutex_;
    std::mutex filterMutex_;
    std::mutex completionMutex_;
    std::condition_variable completionCondition_;
    
    std::atomic<bool> stop_;
    std::atomic<size_t> processedCount_;
    std::atomic<size_t> errorCount_;
    
    std::vector<std::unique_ptr<LogFormatter>> formatters_;
    std::vector<std::unique_ptr<LogFormatter>> storedFormatters_;
    std::unique_ptr<LogFilter> filter_;
    std::unique_ptr<CircularLogBuffer> circularBuffer_;
    
    std::function<void(const std::string&)> outputCallback_;
    std::atomic<size_t> processingCount_;
};

// Async log sink for high-performance logging
class AsyncLogSink {
public:
    AsyncLogSink(size_t bufferSize = 10000, size_t numThreads = std::thread::hardware_concurrency())
        : bufferSize_(bufferSize), stop_(false) {
        buffer_.reserve(bufferSize);
        for (size_t i = 0; i < numThreads; ++i) {
            threads_.emplace_back(&AsyncLogSink::workerThread, this, i);
        }
    }
    
    ~AsyncLogSink() {
        stop_ = true;
        condition_.notify_all();
        for (auto& thread : threads_) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }
    
    void log(const EnhancedLogEntry& entry) {
        std::lock_guard<std::mutex> lock(bufferMutex_);
        
        if (buffer_.size() >= bufferSize_) {
            // Buffer is full, drop oldest entry (configurable)
            buffer_.pop_front();
        }
        
        buffer_.push_back(entry);
    }
    
    bool flush() {
        std::unique_lock<std::mutex> lock(bufferMutex_);
        
        if (buffer_.empty()) {
            return true;
        }
        
        // Create copy of buffer for processing
        std::vector<EnhancedLogEntry> bufferCopy(std::move(buffer_));
        lock.unlock();
        
        // Process buffer
        for (const auto& entry : bufferCopy) {
            if (!processEntry(entry)) {
                // Failed to process, return false
                return false;
            }
        }
        
        return true;
    }
    
    void setProcessor(std::function<bool(const EnhancedLogEntry&)> processor) {
        processor_ = std::move(processor);
    }
    
    size_t getBufferSize() const {
        std::lock_guard<std::mutex> lock(bufferMutex_);
        return buffer_.size();
    }
    
    size_t getMaxBufferSize() const { return bufferSize_; }
    
private:
    bool processEntry(const EnhancedLogEntry& entry) {
        if (processor_) {
            return processor_(entry);
        }
        return true;
    }
    
    void workerThread(size_t threadId) {
        while (!stop_) {
            std::unique_lock<std::mutex> lock(bufferMutex_);
            condition_.wait_for(lock, std::chrono::milliseconds(100), [this] {
                return !buffer_.empty() || stop_;
            });
            
            if (stop_) break;
            
            if (!buffer_.empty()) {
                // Process one entry
                EnhancedLogEntry entry = buffer_.front();
                buffer_.pop_front();
                lock.unlock();
                
                processEntry(entry);
                lock.lock();
            }
        }
    }
    
    std::vector<std::thread> threads_;
    std::deque<EnhancedLogEntry> buffer_;
    std::mutex bufferMutex_;
    std::condition_variable condition_;
    
    std::atomic<bool> stop_;
    size_t bufferSize_;
    
    std::function<bool(const EnhancedLogEntry&)> processor_;
};

// Priority-based log routing system
class PriorityLogRouter {
public:
    PriorityLogRouter() {
        // Create routers for each priority level
        for (int i = 0; i < static_cast<int>(LogPriority::Critical) + 1; ++i) {
            priorityRouters_[i] = std::make_unique<AsyncLogSink>(10000);
        }
    }
    
    void addLog(std::unique_ptr<EnhancedLogEntry> entry, LogPriority priority = LogPriority::Normal) {
        int priorityIndex = static_cast<int>(priority);
        if (priorityIndex >= 0 && priorityIndex < static_cast<int>(LogPriority::Critical) + 1) {
            priorityRouters_[priorityIndex]->log(*entry);
        }
    }
    
    void setProcessorForPriority(LogPriority priority, std::function<bool(const EnhancedLogEntry&)> processor) {
        int priorityIndex = static_cast<int>(priority);
        if (priorityIndex >= 0 && priorityIndex < static_cast<int>(LogPriority::Critical) + 1) {
            priorityRouters_[priorityIndex]->setProcessor(processor);
        }
    }
    
    bool flushAll() {
        bool allSuccess = true;
        for (auto& router : priorityRouters_) {
            if (!router->flush()) {
                allSuccess = false;
            }
        }
        return allSuccess;
    }
    
    size_t getBufferSizeForPriority(LogPriority priority) const {
        int priorityIndex = static_cast<int>(priority);
        if (priorityIndex >= 0 && priorityIndex < static_cast<int>(LogPriority::Critical) + 1) {
            return priorityRouters_[priorityIndex]->getBufferSize();
        }
        return 0;
    }
    
private:
    std::array<std::unique_ptr<AsyncLogSink>, 4> priorityRouters_;
};

} // namespace nlm
