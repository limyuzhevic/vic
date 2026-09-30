#pragma once

#include <chrono>
#include <string>
#include <unordered_map>
#include <vector>
#include <atomic>
#include <memory>
#include <functional>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <thread>
#include <mutex>

namespace nlm {

// High-resolution timer for performance measurement
class Timer {
public:
    Timer() : start_(std::chrono::high_resolution_clock::now()) {}
    
    void reset() {
        start_ = std::chrono::high_resolution_clock::now();
    }
    
    double elapsed() const {
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double>(end - start_).count();
    }
    
    double elapsedMilliseconds() const {
        return elapsed() * 1000.0;
    }
    
    double elapsedMicroseconds() const {
        return elapsed() * 1000000.0;
    }
    
private:
    std::chrono::high_resolution_clock::time_point start_;
};

// Scoped timer for automatic timing
class ScopedTimer {
public:
    ScopedTimer(std::string name) : name_(std::move(name)), timer_() {}
    
    ~ScopedTimer() {
        double elapsed = timer_.elapsed();
        // In real implementation, would log to performance monitor
        // std::cout << "Timer '" << name_ << "': " << elapsed << "s" << std::endl;
    }
    
    double elapsed() const { return timer_.elapsed(); }
    
private:
    std::string name_;
    Timer timer_;
};

// Performance counter for tracking operation frequency
class PerformanceCounter {
public:
    PerformanceCounter() : count_(0), lastReset_(std::chrono::high_resolution_clock::now()) {}
    
    void increment() {
        ++count_;
    }
    
    void add(size_t n) {
        count_ += n;
    }
    
    size_t count() const { return count_; }
    
    double rate() const {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(now - lastReset_).count();
        return duration > 0.0 ? count_ / duration : 0.0;
    }
    
    void reset() {
        count_ = 0;
        lastReset_ = std::chrono::high_resolution_clock::now();
    }
    
private:
    std::atomic<size_t> count_;
    std::chrono::high_resolution_clock::time_point lastReset_;
};

// Performance statistics aggregator
class PerformanceStats {
public:
    struct CounterStats {
        std::string name;
        size_t totalCount = 0;
        double rate = 0.0;
        double avgTimePerOperation = 0.0;
    };
    
    struct TimerStats {
        std::string name;
        double totalTime = 0.0;
        double minTime = std::numeric_limits<double>::max();
        double maxTime = 0.0;
        double avgTime = 0.0;
        size_t sampleCount = 0;
    };
    
    void recordTimer(const std::string& name, double time) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto& stats = timers_[name];
        stats.name = name;
        stats.totalTime += time;
        stats.minTime = std::min(stats.minTime, time);
        stats.maxTime = std::max(stats.maxTime, time);
        stats.sampleCount++;
        stats.avgTime = stats.totalTime / stats.sampleCount;
    }
    
    void recordCounter(const std::string& name, size_t count, double timePerOp = 0.0) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto& stats = counters_[name];
        stats.name = name;
        stats.totalCount += count;
        stats.rate = stats.totalCount / std::max(timePerOp, 0.001);
        stats.avgTimePerOperation = timePerOp;
    }
    
    void printReport(std::ostream& out = std::cout) const {
        out << "Performance Statistics Report" << std::endl;
        out << "===========================" << std::endl;
        
        out << "\nTimer Statistics:" << std::endl;
        out << std::left << std::setw(20) << "Operation" 
            << std::setw(12) << "Total(s)" 
            << std::setw(12) << "Min(s)" 
            << std::setw(12) << "Max(s)" 
            << std::setw(12) << "Avg(s)" 
            << std::setw(10) << "Samples" << std::endl;
        out << std::string(80, '-') << std::endl;
        
        for (const auto& pair : timers_) {
            const auto& stats = pair.second;
            out << std::left << std::setw(20) << stats.name
                << std::setw(12) << std::fixed << std::setprecision(6) << stats.totalTime
                << std::setw(12) << std::fixed << std::setprecision(6) << stats.minTime
                << std::setw(12) << std::fixed << std::setprecision(6) << stats.maxTime
                << std::setw(12) << std::fixed << std::setprecision(6) << stats.avgTime
                << std::setw(10) << stats.sampleCount << std::endl;
        }
        
        out << "\nCounter Statistics:" << std::endl;
        out << std::left << std::setw(20) << "Operation" 
            << std::setw(15) << "Total Count" 
            << std::setw(15) << "Rate (/s)" 
            << std::setw(15) << "Avg Time/op" << std::endl;
        out << std::string(65, '-') << std::endl;
        
        for (const auto& pair : counters_) {
            const auto& stats = pair.second;
            out << std::left << std::setw(20) << stats.name
                << std::setw(15) << stats.totalCount
                << std::setw(15) << std::fixed << std::setprecision(2) << stats.rate
                << std::setw(15) << std::fixed << std::setprecision(6) << stats.avgTimePerOperation << std::endl;
        }
    }
    
    void reset() {
        std::lock_guard<std::mutex> lock(mutex_);
        timers_.clear();
        counters_.clear();
    }
    
    static PerformanceStats& getInstance() {
        static PerformanceStats instance;
        return instance;
    }
    
private:
    PerformanceStats() = default;
    
    mutable std::mutex mutex_;
    std::unordered_map<std::string, TimerStats> timers_;
    std::unordered_map<std::string, CounterStats> counters_;
};

// Memory alignment for SIMD operations
template<typename T>
alignas(32) T* alignMemory(T* ptr, size_t alignment = 32) {
    return reinterpret_cast<T*>((reinterpret_cast<uintptr_t>(ptr) + alignment - 1) & ~(alignment - 1));
}

// Simple memory pool for small allocations
class MemoryPool {
public:
    MemoryPool(size_t blockSize, size_t numBlocks) 
        : blockSize_(blockSize), numBlocks_(numBlocks) {
        allocateMemory();
    }
    
    ~MemoryPool() {
        if (memory_) {
            std::free(memory_);
        }
    }
    
    void* allocate() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (currentBlock_ >= numBlocks_) {
            return nullptr;
        }
        void* ptr = static_cast<char*>(memory_) + currentBlock_ * blockSize_;
        currentBlock_++;
        return ptr;
    }
    
    void deallocate(void* ptr) {
        // Memory pool uses simple LIFO, just mark block as free
        std::lock_guard<std::mutex> lock(mutex_);
        size_t blockIndex = (static_cast<char*>(ptr) - static_cast<char*>(memory_)) / blockSize_;
        if (blockIndex < currentBlock_) {
            // Could implement free list here
        }
    }
    
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        currentBlock_ = 0;
    }
    
private:
    void allocateMemory() {
        memory_ = std::aligned_alloc(alignof(std::max_align_t), blockSize_ * numBlocks_);
        currentBlock_ = 0;
    }
    
    size_t blockSize_;
    size_t numBlocks_;
    void* memory_ = nullptr;
    std::atomic<size_t> currentBlock_;
    mutable std::mutex mutex_;
};

// Progress tracker for long-running operations
class ProgressTracker {
public:
    ProgressTracker(const std::string& description, size_t totalSteps) 
        : description_(description), totalSteps_(totalSteps), currentStep_(0) {
        startTime_ = std::chrono::high_resolution_clock::now();
    }
    
    void advance(size_t steps = 1) {
        std::lock_guard<std::mutex> lock(mutex_);
        currentStep_ += steps;
        if (currentStep_ > totalSteps_) {
            currentStep_ = totalSteps_;
        }
    }
    
    double getProgress() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return totalSteps_ > 0 ? static_cast<double>(currentStep_) / totalSteps_ : 1.0;
    }
    
    std::string getStatus() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::stringstream ss;
        ss << "Progress: " << currentStep_ << "/" << totalSteps_ << " ("
           << (getProgress() * 100.0) << "%)";
        
        if (auto remaining = getEstimatedRemainingTime()) {
            ss << " Remaining: " << (*remaining) << "s";
        }
        
        return ss.str();
    }
    
    double getEstimatedRemainingTime() const {
        auto now = std::chrono::high_resolution_clock::now();
        double elapsed = std::chrono::duration<double>(now - startTime_).count();
        
        if (currentStep_ == 0) return 0.0;
        
        double timePerStep = elapsed / currentStep_;
        double remainingSteps = totalSteps_ - currentStep_;
        return remainingSteps * timePerStep;
    }
    
    void reset() {
        std::lock_guard<std::mutex> lock(mutex_);
        currentStep_ = 0;
        startTime_ = std::chrono::high_resolution_clock::now();
    }
    
private:
    std::string description_;
    size_t totalSteps_;
    std::atomic<size_t> currentStep_;
    std::chrono::high_resolution_clock::time_point startTime_;
    mutable std::mutex mutex_;
};

// Basic thread pool for parallel operations
class ThreadPool {
public:
    ThreadPool(size_t numThreads) {
        for (size_t i = 0; i < numThreads; ++i) {
            workers_.emplace_back([this] {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(queueMutex_);
                        condition_.wait(lock, [this] {
                            return stop_ || !tasks_.empty();
                        });
                        
                        if (stop_ && tasks_.empty()) {
                            return;
                        }
                        
                        task = std::move(tasks_.front());
                        tasks_.pop();
                    }
                    task();
                }
            });
        }
    }
    
    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queueMutex_);
            stop_ = true;
        }
        condition_.notify_all();
        for (auto& worker : workers_) {
            worker.join();
        }
    }
    
    template<typename F>
    void enqueue(F&& f) {
        {
            std::unique_lock<std::mutex> lock(queueMutex_);
            tasks_.emplace(std::forward<F>(f));
        }
        condition_.notify_one();
    }
    
    size_t size() const {
        return workers_.size();
    }
    
private:
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    std::mutex queueMutex_;
    std::condition_variable condition_;
    bool stop_ = false;
};

// Global performance monitoring instance
PerformanceStats& getPerformanceStats() {
    return PerformanceStats::getInstance();
}

} // namespace nlm
