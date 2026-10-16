#pragma once

#include <algorithm>
#include <vector>
#include <memory>
#include <chrono>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace nlm {

// Memory pool for efficient object allocation
template<typename T, size_t PoolSize = 1024>
class MemoryPool {
public:
    MemoryPool() {
        current = 0;
    }
    
    ~MemoryPool() {
        for (auto ptr : allocations) {
            delete ptr;
        }
    }
    
    T* allocate() {
        if (current >= PoolSize) {
            // Fallback to new if pool is full
            T* ptr = new T();
            allocations.push_back(ptr);
            return ptr;
        }
        
        T* ptr = pool[current++];
        return ptr;
    }
    
    void reset() {
        current = 0;
    }
    
private:
    alignas(64) T* pool[PoolSize];
    size_t current;
    std::vector<T*> allocations;
};

// SIMD vector operations for performance-critical computations
class SIMDVectorizer {
public:
    // Fast vector addition using SIMD (if available)
    static void add(const float* a, const float* b, float* result, size_t size) {
        // Implementation depends on platform SIMD support
        // For now, use standard C++ but optimized
        for (size_t i = 0; i < size; ++i) {
            result[i] = a[i] + b[i];
        }
    }
    
    // Fast vector dot product
    static float dotProduct(const float* a, const float* b, size_t size) {
        float result = 0.0f;
        for (size_t i = 0; i < size; ++i) {
            result += a[i] * b[i];
        }
        return result;
    }
    
    // Fast vector normalization
    static void normalize(float* vector, size_t size) {
        float magnitude = 0.0f;
        for (size_t i = 0; i < size; ++i) {
            magnitude += vector[i] * vector[i];
        }
        magnitude = std::sqrt(magnitude);
        
        if (magnitude > 0.0f) {
            float invMagnitude = 1.0f / magnitude;
            for (size_t i = 0; i < size; ++i) {
                vector[i] *= invMagnitude;
            }
        }
    }
};

// Thread pool for parallel processing
class ThreadPool {
public:
    ThreadPool(size_t numThreads = std::thread::hardware_concurrency()) {
        running = true;
        for (size_t i = 0; i < numThreads; ++i) {
            workers.emplace_back(&ThreadPool::workerLoop, this);
        }
    }
    
    ~ThreadPool() {
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            running = false;
        }
        condition.notify_all();
        for (auto& worker : workers) {
            worker.join();
        }
    }
    
    template<typename F>
    void submit(F task) {
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            tasks.emplace(std::forward<F>(task));
        }
        condition.notify_one();
    }
    
    size_t size() const {
        return workers.size();
    }
    
private:
    void workerLoop() {
        while (running) {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(queueMutex);
                condition.wait(lock, [this] { return !tasks.empty() || !running; });
                if (!running && tasks.empty()) break;
                task = std::move(tasks.front());
                tasks.pop();
            }
            task();
        }
    }
    
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queueMutex;
    std::condition_variable condition;
    std::atomic<bool> running;
};

// Event-driven system for efficient spike propagation
class EventQueue {
public:
    struct Event {
        SimulationStep step;
        NeuronId neuronId;
        float value;
        
        Event(SimulationStep s, NeuronId id, float v) : step(s), neuronId(id), value(v) {}
        
        bool operator<(const Event& other) const {
            return step < other.step;
        }
    };
    
    EventQueue() : currentStep(0) {}
    
    void addEvent(SimulationStep step, NeuronId neuronId, float value) {
        std::lock_guard<std::mutex> lock(eventMutex);
        events.emplace(step, neuronId, value);
        eventCondition.notify_one();
    }
    
    bool hasEvents() const {
        std::lock_guard<std::mutex> lock(eventMutex);
        return !events.empty() && events.top().step <= currentStep;
    }
    
    Event popNextEvent() {
        std::lock_guard<std::mutex> lock(eventMutex);
        Event event = events.top();
        events.pop();
        return event;
    }
    
    void advanceStep(SimulationStep step) {
        std::lock_guard<std::mutex> lock(eventMutex);
        currentStep = step;
    }
    
private:
    std::priority_queue<Event> events;
    std::mutex eventMutex;
    std::condition_variable eventCondition;
    SimulationStep currentStep;
};

// Checkpoint system for saving and loading brain state
class CheckpointSystem {
public:
    CheckpointSystem(const std::string& filename) : checkpointFile(filename) {}
    
    bool saveCheckpoint(const Brain& brain, SimulationStep step) {
        // Implement checkpoint saving
        return saveToFile(checkpointFile, brain, step);
    }
    
    bool loadCheckpoint(Brain& brain, SimulationStep& step) {
        // Implement checkpoint loading
        return loadFromFile(checkpointFile, brain, step);
    }
    
    bool createCheckpoint(const std::string& label) {
        // Create a named checkpoint
        return saveToFile(checkpointFile + "_" + label, brain, currentStep);
    }
    
    bool restoreCheckpoint(const std::string& label) {
        // Restore from a named checkpoint
        return loadFromFile(checkpointFile + "_" + label, brain, currentStep);
    }
    
private:
    std::string checkpointFile;
    SimulationStep currentStep;
    
    bool saveToFile(const std::string& filename, const Brain& brain, SimulationStep step);
    bool loadFromFile(const std::string& filename, Brain& brain, SimulationStep& step);
};

// Performance analyzer for benchmarking
class PerformanceAnalyzer {
public:
    struct PerformanceMetrics {
        double cpuUsage;
        double memoryUsage;
        double cacheHitRate;
        size_t activeThreads;
        size_t totalInstructions;
        std::chrono::duration<double> elapsedTime;
        
        PerformanceMetrics()
            : cpuUsage(0.0), memoryUsage(0.0), cacheHitRate(0.0), 
              activeThreads(0), totalInstructions(0), elapsedTime(0.0) {}
        
        void printReport() const {
            std::cout << "=== Performance Report ===" << std::endl;
            std::cout << "CPU Usage: " << (cpuUsage * 100.0) << "%" << std::endl;
            std::cout << "Memory Usage: " << (memoryUsage / 1024.0 / 1024.0) << " MB" << std::endl;
            std::cout << "Cache Hit Rate: " << (cacheHitRate * 100.0) << "%" << std::endl;
            std::cout << "Active Threads: " << activeThreads << std::endl;
            std::cout << "Total Instructions: " << totalInstructions << std::endl;
            std::cout << "Elapsed Time: " << elapsedTime.count() << " seconds" << std::endl;
        }
    };
    
    PerformanceAnalyzer() {
        startTime = std::chrono::high_resolution_clock::now();
    }
    
    void recordEvent(const std::string& eventName) {
        eventTimes[eventName].push_back(std::chrono::high_resolution_clock::now());
    }
    
    PerformanceMetrics getMetrics() const {
        PerformanceMetrics metrics;
        auto endTime = std::chrono::high_resolution_clock::now();
        metrics.elapsedTime = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime);
        
        for (const auto& pair : eventTimes) {
            const auto& times = pair.second;
            if (times.size() >= 2) {
                double totalDuration = 0.0;
                for (size_t i = 1; i < times.size(); ++i) {
                    totalDuration += std::chrono::duration<double>(times[i] - times[i-1]).count();
                }
                metrics.elapsedTime += totalDuration;
            }
        }
        
        return metrics;
    }
    
    void printBenchmarkReport() const {
        auto metrics = getMetrics();
        metrics.printReport();
        
        std::cout << "\n=== Event Timeline ===" << std::endl;
        for (const auto& pair : eventTimes) {
            const auto& times = pair.second;
            if (times.size() >= 2) {
                std::cout << pair.first << ": " << times.size() - 1 << " events in " 
                         << std::chrono::duration<double>(times.back() - times.front()).count() 
                         << " seconds" << std::endl;
            }
        }
    }
    
private:
    std::chrono::high_resolution_clock::time_point startTime;
    std::unordered_map<std::string, std::vector<std::chrono::high_resolution_clock::time_point>> eventTimes;
};

// Optimization strategies for different workload types
class OptimizationStrategy {
public:
    enum class StrategyType {
        SEQUENTIAL,
        PARALLEL,
        PIPELINE,
        EVENT_DRIVEN,
        HYBRID
    };
    
    virtual ~OptimizationStrategy() {}
    
    virtual void optimize(Brain& brain) = 0;
    virtual StrategyType getType() const = 0;
    virtual double getEstimatedImprovement() const = 0;
    
protected:
    void applySIMDOptimizations(Brain& brain);
    void applyMemoryPoolOptimizations(Brain& brain);
    void applyParallelOptimizations(Brain& brain);
    void applyEventDrivenOptimizations(Brain& brain);
};

// Optimization factory for creating appropriate optimization strategies
class OptimizationFactory {
public:
    static std::unique_ptr<OptimizationStrategy> createOptimizationStrategy(
        OptimizationStrategy::StrategyType type, Brain& brain);
};

// Cache-friendly data structures for better memory locality
class CacheFriendlyContainer {
public:
    template<typename T>
    class AlignedVector {
    public:
        AlignedVector() {}
        AlignedVector(size_t size) : data(size) {}
        
        T& operator[](size_t index) { return data[index]; }
        const T& operator[](size_t index) const { return data[index]; }
        
        size_t size() const { return data.size(); }
        void resize(size_t size) { data.resize(size); }
        
        T* begin() { return data.data(); }
        T* end() { return data.data() + data.size(); }
        const T* begin() const { return data.data(); }
        const T* end() const { return data.data() + data.size(); }
        
    private:
        std::vector<T> data;
    };
    
    // Memory-aligned allocation for better cache performance
    static void* alignedAlloc(size_t alignment, size_t size);
    static void alignedFree(void* ptr);
};

} // namespace nlm