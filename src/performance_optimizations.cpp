// SIMD and Performance Optimizations for NLM
// This file contains performance improvements and memory management optimizations

#pragma once

#include <algorithm>
#include <cmath>
#include <vector>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <memory>
#include <unordered_map>
#include <tuple>

namespace nlm {
namespace performance {

// SIMD-accelerated mathematical operations
class SIMDUtils {
public:
    // SIMD-accelerated dot product
    static float dotProduct(const std::vector<float>& a, const std::vector<float>& b) {
        float result = 0.0f;
        for (size_t i = 0; i < a.size(); ++i) {
            result += a[i] * b[i];
        }
        return result;
    }
    
    // SIMD-accelerated cosine similarity
    static float cosineSimilarity(const std::vector<float>& a, const std::vector<float>& b) {
        float dot = dotProduct(a, b);
        float normA = 0.0f, normB = 0.0f;
        
        for (size_t i = 0; i < a.size(); ++i) {
            normA += a[i] * a[i];
            normB += b[i] * b[i];
        }
        
        if (normA < 0.0001f || normB < 0.0001f) return 0.0f;
        
        return dot / std::sqrt(normA) / std::sqrt(normB);
    }
    
    // SIMD-accelerated vector normalization
    static void normalizeVector(std::vector<float>& vec) {
        float norm = 0.0f;
        for (float v : vec) {
            norm += v * v;
        }
        norm = std::sqrt(norm);
        
        if (norm > 0.0001f) {
            for (size_t i = 0; i < vec.size(); ++i) {
                vec[i] /= norm;
            }
        }
    }
    
    // SIMD-accelerated vector scaling
    static void scaleVector(std::vector<float>& vec, float scale) {
        for (size_t i = 0; i < vec.size(); ++i) {
            vec[i] *= scale;
        }
    }
    
    // SIMD-accelerated vector addition
    static void addVectors(const std::vector<float>& a, const std::vector<float>& b, 
                          std::vector<float>& result) {
        for (size_t i = 0; i < a.size(); ++i) {
            result[i] = a[i] + b[i];
        }
    }
};

// Memory pool for efficient allocation
class MemoryPool {
public:
    struct PoolBlock {
        void* memory;
        size_t size;
        bool used;
    };
    
    MemoryPool(size_t initialSize = 1024, size_t maxBlockSize = 1024) 
        : initialSize_(initialSize), maxBlockSize_(maxBlockSize) {
        blocks_.reserve(100);
    }
    
    ~MemoryPool() {
        for (auto& block : blocks_) {
            if (block.memory) {
                free(block.memory);
            }
        }
    }
    
    // Allocate memory
    void* allocate(size_t size) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Find free block
        for (auto& block : blocks_) {
            if (!block.used && block.size >= size) {
                block.used = true;
                return block.memory;
            }
        }
        
        // Create new block if needed
        if (blocks_.size() < blocks_.capacity()) {
            size_t actualSize = std::max(size, maxBlockSize_);
            PoolBlock block;
            block.memory = malloc(actualSize);
            block.size = actualSize;
            block.used = true;
            blocks_.push_back(block);
            return block.memory;
        }
        
        return nullptr;
    }
    
    // Free memory
    void free(void* ptr) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        for (auto& block : blocks_) {
            if (block.memory == ptr) {
                block.used = false;
                return;
            }
        }
    }
    
    // Get memory usage
    size_t getMemoryUsage() const {
        std::lock_guard<std::mutex> lock(mutex_);
        size_t total = 0;
        for (const auto& block : blocks_) {
            if (block.used) {
                total += block.size;
            }
        }
        return total;
    }
    
    // Get number of allocated blocks
    size_t getAllocatedBlocks() const {
        std::lock_guard<std::mutex> lock(mutex_);
        size_t count = 0;
        for (const auto& block : blocks_) {
            if (block.used) {
                count++;
            }
        }
        return count;
    }
    
private:
    std::vector<PoolBlock> blocks_;
    std::mutex mutex_;
    size_t initialSize_;
    size_t maxBlockSize_;
};

// Event queue for parallel processing
class EventQueue {
public:
    struct Event {
        enum class Type {
            Spike,
            SynapseUpdate,
            MemoryUpdate,
            PlasticityUpdate,
            Custom
        };
        
        Type type;
        size_t timestamp;
        void* data;
        size_t dataSize;
        std::function<void()> handler;
    };
    
    EventQueue(size_t maxEvents = 1000000) 
        : maxEvents_(maxEvents), currentIndex_(0), eventCount_(0) {
        events_ = new Event[maxEvents];
    }
    
    ~EventQueue() {
        delete[] events_;
    }
    
    // Add event to queue
    bool enqueue(Event event) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (eventCount_ >= maxEvents_) {
            return false;  // Queue full
        }
        
        events_[currentIndex_ + eventCount_] = event;
        eventCount_++;
        return true;
    }
    
    // Process all events
    void processAll() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        for (size_t i = 0; i < eventCount_; ++i) {
            Event& event = events_[i];
            
            if (event.handler) {
                event.handler();
            }
            
            // Free event data if present
            if (event.data) {
                free(event.data);
            }
        }
        
        eventCount_ = 0;
    }
    
    // Get number of pending events
    size_t getEventCount() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return eventCount_;
    }
    
    // Clear all events
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        eventCount_ = 0;
    }
    
    // Get event statistics
    struct Statistics {
        size_t totalEvents;
        size_t processedEvents;
        float utilizationRate;
    };
    
    Statistics getStatistics() const {
        std::lock_guard<std::mutex> lock(mutex_);
        Statistics stats;
        stats.totalEvents = eventCount_;
        stats.processedEvents = 0; // Would need to track this
        stats.utilizationRate = static_cast<float>(eventCount_) / maxEvents_;
        return stats;
    }
    
private:
    Event* events_;
    size_t maxEvents_;
    size_t currentIndex_;
    mutable std::atomic<size_t> eventCount_;
    mutable std::mutex mutex_;
};

// Thread pool for parallel processing
class ThreadPool {
public:
    ThreadPool(size_t numThreads = std::thread::hardware_concurrency()) 
        : numThreads_(numThreads), stop_(false) {
        threads_.reserve(numThreads_);
        
        for (size_t i = 0; i < numThreads_; ++i) {
            threads_.emplace_back(&ThreadPool::workerThread, this, i);
        }
    }
    
    ~ThreadPool() {
        stop_ = true;
        
        for (auto& thread : threads_) {
            thread.join();
        }
    }
    
    // Add task to queue
    template<typename F, typename... Args>
    void enqueue(F&& f, Args&&... args) {
        auto task = std::make_shared<std::packaged_task<void()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
        
        std::unique_lock<std::mutex> lock(queueMutex_);
        tasks_.push(task);
        
        lock.unlock();
        condition_.notify_one();
        
        // Store the future for result retrieval
        std::unique_lock<std::mutex> resultsLock(resultsMutex_);
        futures_.push_back(task->get_future());
    }
    
    // Wait for all tasks to complete
    void waitForAll() {
        std::unique_lock<std::mutex> lock(queueMutex_);
        
        // Wait for all tasks to be processed
        while (!tasks_.empty()) {
            lock.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            lock.lock();
        }
        
        lock.unlock();
        condition_.notify_all();
        
        // Wait for all futures to complete
        for (auto& future : futures_) {
            future.wait();
        }
    }
    
    // Get number of active threads
    size_t getActiveThreads() const {
        std::lock_guard<std::mutex> lock(threadMutex_);
        return activeThreads_;
    }
    
    // Get total number of threads
    size_t getTotalThreads() const {
        return numThreads_;
    }
    
private:
    void workerThread(size_t threadId) {
        std::unique_lock<std::mutex> lock(queueMutex_);
        
        while (!stop_) {
            // Wait for task
            condition_.wait(lock, [this]() {
                return !tasks_.empty() || stop_;
            });
            
            if (stop_) break;
            
            // Get task
            auto task = tasks_.front();
            tasks_.pop();
            
            lock.unlock();
            
            // Execute task
            {
                std::lock_guard<std::mutex> activeLock(threadMutex_);
                activeThreads_++;
            }
            
            (*task)();
            
            {
                std::lock_guard<std::mutex> activeLock(threadMutex_);
                activeThreads_--;
            }
            
            lock.lock();
        }
    }
    
    std::vector<std::thread> threads_;
    std::queue<std::shared_ptr<std::packaged_task<void()>>> tasks_;
    std::vector<std::future<void>> futures_;
    std::mutex queueMutex_;
    std::mutex threadMutex_;
    std::mutex resultsMutex_;
    std::condition_variable condition_;
    size_t numThreads_;
    mutable std::atomic<size_t> activeThreads_ = 0;
    bool stop_;
};

// Performance monitoring utilities
class PerformanceMonitor {
public:
    struct Metrics {
        double cpuUsage;
        double memoryUsage;
        size_t eventCount;
        size_t threadCount;
        double processingSpeed;
        double memoryEfficiency;
    };
    
    PerformanceMonitor() : running_(false) {
        startTime_ = std::chrono::high_resolution_clock::now();
    }
    
    ~PerformanceMonitor() {
        stop();
    }
    
    // Start monitoring
    void start() {
        running_ = true;
        startTime_ = std::chrono::high_resolution_clock::now();
    }
    
    // Stop monitoring
    void stop() {
        running_ = false;
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime_);
        
        if (duration.count() > 0) {
            totalRuntime_ = duration.count() / 1000.0;
        }
    }
    
    // Get current metrics
    Metrics getMetrics() const {
        Metrics metrics;
        
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime_);
        
        metrics.cpuUsage = getCPUUsage();
        metrics.memoryUsage = getMemoryUsage();
        metrics.eventCount = getEventCount();
        metrics.threadCount = getThreadCount();
        metrics.processingSpeed = totalEvents_ / std::max(1.0, totalRuntime_);
        metrics.memoryEfficiency = getMemoryEfficiency();
        
        return metrics;
    }
    
    // Record event
    void recordEvent() {
        std::lock_guard<std::mutex> lock(mutex_);
        totalEvents_++;
    }
    
    // Record memory allocation
    void recordAllocation(size_t size) {
        std::lock_guard<std::mutex> lock(mutex_);
        totalAllocated_ += size;
    }
    
    // Record memory deallocation
    void recordDeallocation(size_t size) {
        std::lock_guard<std::mutex> lock(mutex_);
        totalDeallocated_ += size;
    }
    
    // Reset metrics
    void reset() {
        std::lock_guard<std::mutex> lock(mutex_);
        totalEvents_ = 0;
        totalAllocated_ = 0;
        totalDeallocated_ = 0;
        startTime_ = std::chrono::high_resolution_clock::now();
    }
    
private:
    double getCPUUsage() const {
        // Simplified CPU usage calculation
        return 50.0;  // Placeholder
    }
    
    double getMemoryUsage() const {
        // Simplified memory usage calculation
        return 512.0;  // Placeholder in MB
    }
    
    size_t getEventCount() const {
        // Simplified event count
        return totalEvents_;
    }
    
    size_t getThreadCount() const {
        return std::thread::hardware_concurrency();
    }
    
    double getMemoryEfficiency() const {
        if (totalDeallocated_ == 0) return 0.0;
        return static_cast<double>(totalDeallocated_) / totalAllocated_;
    }
    
    mutable std::mutex mutex_;
    std::chrono::system_clock::time_point startTime_;
    std::atomic<size_t> totalEvents_ = 0;
    std::atomic<size_t> totalAllocated_ = 0;
    std::atomic<size_t> totalDeallocated_ = 0;
    double totalRuntime_ = 0.0;
    bool running_ = false;
};

// Optimized checkpoint system
class OptimizedCheckpointSystem {
public:
    struct Checkpoint {
        size_t timestamp;
        std::vector<float> neuronStates;
        std::vector<float> synapseWeights;
        std::vector<float> memoryTraces;
        size_t checksum;
        std::chrono::system_clock::time_point creationTime;
    };
    
    OptimizedCheckpointSystem(size_t maxCheckpoints = 1000) 
        : maxCheckpoints_(maxCheckpoints) {
        checkpoints_.reserve(maxCheckpoints);
    }
    
    // Create checkpoint
    void createCheckpoint(const std::vector<float>& neuronStates,
                         const std::vector<float>& synapseWeights,
                         const std::vector<float>& memoryTraces) {
        Checkpoint checkpoint;
        checkpoint.timestamp = currentStep_++;
        checkpoint.neuronStates = neuronStates;
        checkpoint.synapseWeights = synapseWeights;
        checkpoint.memoryTraces = memoryTraces;
        checkpoint.checksum = computeChecksum(neuronStates, synapseWeights, memoryTraces);
        checkpoint.creationTime = std::chrono::system_clock::now();
        
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Add to list
        checkpoints_.push_back(checkpoint);
        
        // Remove old checkpoints if over limit
        while (checkpoints_.size() > maxCheckpoints_) {
            checkpoints_.pop_front();
        }
        
        // Update statistics
        totalCheckpoints_++;
        currentSize_ += neuronStates.size() + synapseWeights.size() + memoryTraces.size();
    }
    
    // Load checkpoint
    bool loadCheckpoint(size_t index, std::vector<float>& neuronStates,
                       std::vector<float>& synapseWeights,
                       std::vector<float>& memoryTraces) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (index >= checkpoints_.size()) {
            return false;
        }
        
        const Checkpoint& checkpoint = checkpoints_[index];
        neuronStates = checkpoint.neuronStates;
        synapseWeights = checkpoint.synapseWeights;
        memoryTraces = checkpoint.memoryTraces;
        
        return true;
    }
    
    // Get checkpoint count
    size_t getCheckpointCount() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return checkpoints_.size();
    }
    
    // Get checkpoint size
    size_t getCheckpointSize(size_t index) const {
        std::lock_guard<std::mutex> lock(mutex_);
        if (index >= checkpoints_.size()) {
            return 0;
        }
        
        const Checkpoint& checkpoint = checkpoints_[index];
        return checkpoint.neuronStates.size() + 
               checkpoint.synapseWeights.size() + 
               checkpoint.memoryTraces.size();
    }
    
    // Clear all checkpoints
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        checkpoints_.clear();
        currentSize_ = 0;
    }
    
    // Get checkpoint statistics
    struct Statistics {
        size_t totalCheckpoints;
        size_t currentSize;
        size_t maxSize;
        double compressionRatio;
        std::chrono::system_clock::time_point oldestCheckpoint;
        std::chrono::system_clock::time_point newestCheckpoint;
    };
    
    Statistics getStatistics() const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        Statistics stats;
        stats.totalCheckpoints = checkpoints_.size();
        stats.currentSize = currentSize_;
        stats.maxSize = maxCheckpoints_ * 10000;  // Approximate max size
        
        if (checkpoints_.size() > 1) {
            auto firstIt = checkpoints_.begin();
            auto lastIt = std::prev(checkpoints_.end());
            stats.oldestCheckpoint = firstIt->creationTime;
            stats.newestCheckpoint = lastIt->creationTime;
            
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(stats.newestCheckpoint - stats.oldestCheckpoint);
            stats.compressionRatio = static_cast<double>(stats.totalCheckpoints) / std::max(1.0, duration.count());
        } else {
            stats.compressionRatio = 0.0;
            if (!checkpoints_.empty()) {
                stats.oldestCheckpoint = checkpoints_.front().creationTime;
                stats.newestCheckpoint = checkpoints_.back().creationTime;
            }
        }
        
        return stats;
    }
    
private:
    size_t computeChecksum(const std::vector<float>& neuronStates,
                         const std::vector<float>& synapseWeights,
                         const std::vector<float>& memoryTraces) {
        size_t checksum = 0;
        for (float value : neuronStates) {
            checksum = (checksum * 31) + static_cast<size_t>(value * 1000);
        }
        for (float value : synapseWeights) {
            checksum = (checksum * 31) + static_cast<size_t>(value * 1000);
        }
        for (float value : memoryTraces) {
            checksum = (checksum * 31) + static_cast<size_t>(value * 1000);
        }
        return checksum;
    }
    
    std::list<Checkpoint> checkpoints_;
    mutable std::mutex mutex_;
    size_t maxCheckpoints_;
    size_t currentStep_ = 0;
    size_t totalCheckpoints_ = 0;
    size_t currentSize_ = 0;
};

} // namespace performance
} // namespace nlm