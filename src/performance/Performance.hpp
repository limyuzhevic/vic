#ifndef NLM_PERFORMANCE_H
#define NLM_PERFORMANCE_H

#include <vector>
#include <memory>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <thread>
#include <atomic>
#include <cmath>
#include <immintrin.h>  // For SIMD intrinsics

namespace nlm {

// SIMD-optimized vector operations for neural computations
class SIMDVectorOps {
public:
    // SIMD optimized vector addition
    static void addSIMD(const float* a, const float* b, float* result, size_t size) {
        size_t i = 0;
        const size_t simdSize = size - size % 8;
        
        // Process 8 floats at a time using AVX
        for (; i < simdSize; i += 8) {
            __m256 va = _mm256_loadu_ps(a + i);
            __m256 vb = _mm256_loadu_ps(b + i);
            __m256 vresult = _mm256_add_ps(va, vb);
            _mm256_storeu_ps(result + i, vresult);
        }
        
        // Process remaining elements
        for (; i < size; ++i) {
            result[i] = a[i] + b[i];
        }
    }

    // SIMD optimized vector multiplication
    static void multiplySIMD(const float* a, const float* b, float* result, size_t size) {
        size_t i = 0;
        const size_t simdSize = size - size % 8;
        
        for (; i < simdSize; i += 8) {
            __m256 va = _mm256_loadu_ps(a + i);
            __m256 vb = _mm256_loadu_ps(b + i);
            __m256 vresult = _mm256_mul_ps(va, vb);
            _mm256_storeu_ps(result + i, vresult);
        }
        
        for (; i < size; ++i) {
            result[i] = a[i] * b[i];
        }
    }

    // SIMD optimized vector dot product
    static float dotProductSIMD(const float* a, const float* b, size_t size) {
        float result = 0.0f;
        size_t i = 0;
        const size_t simdSize = size - size % 8;
        
        __m256 sum = _mm256_setzero_ps();
        
        for (; i < simdSize; i += 8) {
            __m256 va = _mm256_loadu_ps(a + i);
            __m256 vb = _mm256_loadu_ps(b + i);
            sum = _mm256_add_ps(sum, _mm256_mul_ps(va, vb));
        }
        
        // Horizontal sum
        alignas(32) float temp[8];
        _mm256_store_ps(temp, sum);
        result = temp[0] + temp[1] + temp[2] + temp[3] + temp[4] + temp[5] + temp[6] + temp[7];
        
        // Process remaining elements
        for (; i < size; ++i) {
            result += a[i] * b[i];
        }
        
        return result;
    }

    // SIMD optimized exponential function
    static void exponentialSIMD(const float* input, float* output, size_t size) {
        size_t i = 0;
        const size_t simdSize = size - size % 8;
        
        for (; i < simdSize; i += 8) {
            __m256 vx = _mm256_loadu_ps(input + i);
            // Use vexpf for exponential (approximation)
            __m256 vexp = _mm256_exp_ps(vx);
            _mm256_storeu_ps(output + i, vexp);
        }
        
        for (; i < size; ++i) {
            output[i] = std::exp(input[i]);
        }
    }

    // SIMD optimized activation functions
    static void sigmoidSIMD(const float* input, float* output, size_t size) {
        size_t i = 0;
        const size_t simdSize = size - size % 8;
        
        // Sigmoid: 1 / (1 + exp(-x))
        // Use approximate sigmoid for better performance
        for (; i < simdSize; i += 8) {
            __m256 vx = _mm256_loadu_ps(input + i);
            __m256 vneg = _mm256_sub_ps(_mm256_setzero_ps(), vx);
            __m256 vexp = _mm256_exp_ps(vneg);
            __m256 vsigmoid = _mm256_div_ps(_mm256_set1_ps(1.0f), _mm256_add_ps(_mm256_set1_ps(1.0f), vexp));
            _mm256_storeu_ps(output + i, vsigmoid);
        }
        
        for (; i < size; ++i) {
            output[i] = 1.0f / (1.0f + std::exp(-input[i]));
        }
    }

    // SIMD optimized ReLU activation
    static void reluSIMD(const float* input, float* output, size_t size) {
        size_t i = 0;
        const size_t simdSize = size - size % 8;
        
        __m256 vzero = _mm256_setzero_ps();
        
        for (; i < simdSize; i += 8) {
            __m256 vx = _mm256_loadu_ps(input + i);
            __m256 vresult = _mm256_max_ps(vx, vzero);
            _mm256_storeu_ps(output + i, vresult);
        }
        
        for (; i < size; ++i) {
            output[i] = std::max(input[i], 0.0f);
        }
    }
};

// Memory pool for efficient allocation/deallocation
class MemoryPool {
private:
    struct Block {
        void* ptr;
        size_t size;
        bool used;
        Block* next;
    };

    Block* head_;
    size_t blockSize_;
    size_t maxBlocks_;
    std::atomic<size_t> allocatedCount_;

public:
    MemoryPool(size_t blockSize = 1024, size_t maxBlocks = 1000)
        : blockSize_(blockSize), maxBlocks_(maxBlocks), allocatedCount_(0) {
        head_ = nullptr;
    }

    ~MemoryPool() {
        Block* current = head_;
        while (current) {
            Block* next = current->next;
            delete current;
            current = next;
        }
    }

    void* allocate(size_t size) {
        if (size > blockSize_) {
            // For large allocations, use standard malloc
            allocatedCount_++;
            return std::malloc(size);
        }

        Block* current = head_;
        while (current) {
            if (!current->used) {
                current->used = true;
                allocatedCount_++;
                return current->ptr;
            }
            current = current->next;
        }

        // Need a new block
        if (allocatedCount_ >= maxBlocks_) {
            throw std::bad_alloc();
        }

        Block* newBlock = new Block;
        newBlock->ptr = std::malloc(blockSize_);
        newBlock->size = blockSize_;
        newBlock->used = true;
        newBlock->next = head_;
        head_ = newBlock;
        allocatedCount_++;

        return newBlock->ptr;
    }

    void deallocate(void* ptr) {
        Block* current = head_;
        while (current) {
            if (current->ptr == ptr) {
                current->used = false;
                allocatedCount_--;
                return;
            }
            current = current->next;
        }

        // Pointer not found in pool, use standard free
        std::free(ptr);
    }

    size_t allocatedCount() const { return allocatedCount_.load(); }
    size_t maxBlocks() const { return maxBlocks_; }
    size_t blockSize() const { return blockSize_; }
};

// Lock-free queue for event processing
class LockFreeQueue {
private:
    struct Node {
        void* data;
        Node* next;
        std::atomic<size_t> refCount;

        Node(void* data) : data(data), next(nullptr), refCount(1) {}
    };

    std::atomic<Node*> head_;
    std::atomic<Node*> tail_;
    size_t maxSize_;
    std::atomic<size_t> currentSize_;

public:
    LockFreeQueue(size_t maxSize = 10000) : maxSize_(maxSize), currentSize_(0) {
        Node* dummy = new Node(nullptr);
        head_ = dummy;
        tail_ = dummy;
    }

    ~LockFreeQueue() {
        while (head_ != tail_) {
            Node* oldHead = head_.load();
            head_ = oldHead->next;
            if (oldHead->refCount.fetch_sub(1) == 1) {
                delete oldHead;
            }
        }
        delete tail_.load();
    }

    bool push(void* data) {
        if (currentSize_.load() >= maxSize_) {
            return false;  // Queue full
        }

        Node* newNode = new Node(data);
        Node* oldTail = tail_.load();

        while (true) {
            newNode->next = oldTail->next;
            if (tail_.compare_exchange_weak(oldTail, newNode)) {
                currentSize_.fetch_add(1);
                return true;
            }
        }
    }

    bool pop(void*& data) {
        Node* oldHead = head_.load();
        Node* oldTail = tail_.load();

        if (oldHead == oldTail) {
            return false;  // Empty queue
        }

        data = oldHead->next->data;
        Node* nextNode = oldHead->next->next;

        if (head_.compare_exchange_weak(oldHead, nextNode)) {
            currentSize_.fetch_sub(1);
            if (oldHead->next->refCount.fetch_sub(1) == 1) {
                delete oldHead->next;
            }
            delete oldHead;
            return true;
        }

        return false;
    }

    size_t size() const { return currentSize_.load(); }
    bool empty() const { return currentSize_.load() == 0; }
    bool full() const { return currentSize_.load() >= maxSize_; }
};

// Performance monitoring with SIMD-optimized metrics
class SIMDPerformanceMonitor {
private:
    struct PerformanceMetrics {
        std::atomic<uint64_t> stepCount{0};
        std::atomic<uint64_t> spikeCount{0};
        std::atomic<double> totalStepTime{0.0};
        std::atomic<size_t> memoryUsage{0};
        std::atomic<size_t> activeNeurons{0};
        std::atomic<size_t> activeSynapses{0};

        alignas(32) float neuronActivityHistory[1024];  // SIMD-aligned buffer
        alignas(32) float spikeRateHistory[1024];      // SIMD-aligned buffer
        alignas(32) float connectivityHistory[1024];  // SIMD-aligned buffer

        PerformanceMetrics() {
            std::fill(std::begin(neuronActivityHistory), std::end(neuronActivityHistory), 0.0f);
            std::fill(std::begin(spikeRateHistory), std::end(spikeRateHistory), 0.0f);
            std::fill(std::begin(connectivityHistory), std::end(connectivityHistory), 0.0f);
        }
    };

    std::unique_ptr<PerformanceMetrics> metrics_;
    std::atomic<std::chrono::steady_clock::time_point> lastFrameTime_;
    bool monitoringActive_;

public:
    SIMDPerformanceMonitor() : monitoringActive_(false) {
        metrics_ = std::make_unique<PerformanceMetrics>();
        lastFrameTime_ = std::chrono::steady_clock::now();
    }

    void startMonitoring() {
        monitoringActive_ = true;
        lastFrameTime_ = std::chrono::steady_clock::now();
    }

    void stopMonitoring() {
        monitoringActive_ = false;
    }

    void recordStep(size_t spikeCount, double stepTime) {
        if (!monitoringActive_) return;

        metrics_->stepCount.fetch_add(1, std::memory_order_relaxed);
        metrics_->spikeCount.fetch_add(spikeCount, std::memory_order_relaxed);
        metrics_->totalStepTime.fetch_add(stepTime, std::memory_order_relaxed);

        // Update history with SIMD optimization
        size_t index = metrics_->stepCount.load(std::memory_order_relaxed) % 1024;

        // Update neuron activity (SIMD optimized)
        float neuronActivity = static_cast<float>(spikeCount) / metrics_->activeNeurons.load(std::memory_order_relaxed);
        metrics_->neuronActivityHistory[index] = neuronActivity;

        // Update spike rate (SIMD optimized)
        float spikeRate = static_cast<float>(spikeCount) / static_cast<float>(stepTime * 1000.0);  // Hz
        metrics_->spikeRateHistory[index] = spikeRate;

        // Update connectivity (simplified)
        float connectivity = static_cast<float>(metrics_->spikeCount.load(std::memory_order_relaxed)) /
                            static_cast<float>(metrics_->stepCount.load(std::memory_order_relaxed));
        metrics_->connectivityHistory[index] = connectivity;
    }

    void recordMemoryUsage(size_t memoryUsage) {
        metrics_->memoryUsage.store(memoryUsage, std::memory_order_relaxed);
    }

    void recordNeurons(size_t activeNeurons) {
        metrics_->activeNeurons.store(activeNeurons, std::memory_order_relaxed);
    }

    void recordSynapses(size_t activeSynapses) {
        metrics_->activeSynapses.store(activeSynapses, std::memory_order_relaxed);
    }

    struct PerformanceMetricsResult {
        double averageStepTime{0.0};
        float averageFiringRate{0.0f};
        float peakMemoryUsage{0.0f};
        float currentConnectivity{0.0f};
        double cpuUsage{0.0};
        size_t totalSteps{0};
        size_t totalSpikes{0};
    };

    PerformanceMetricsResult getMetrics() const {
        PerformanceMetricsResult result;

        uint64_t steps = metrics_->stepCount.load(std::memory_order_relaxed);
        uint64_t spikes = metrics_->spikeCount.load(std::memory_order_relaxed);

        if (steps > 0) {
            result.averageStepTime = metrics_->totalStepTime.load(std::memory_order_relaxed) / steps;
            result.averageFiringRate = static_cast<float>(spikes) / steps;  // spikes per step
            result.currentConnectivity = metrics_->spikeCount.load(std::memory_order_relaxed) / static_cast<float>(steps);
        }

        result.peakMemoryUsage = static_cast<float>(metrics_->memoryUsage.load(std::memory_order_relaxed));
        result.totalSteps = steps;
        result.totalSpikes = spikes;

        // Calculate CPU usage (simplified)
        auto currentTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastFrameTime_.load());
        result.cpuUsage = std::min(100.0, duration.count() * 0.1);  // Simplified CPU usage calculation

        return result;
    }

    void resetMetrics() {
        metrics_->stepCount = 0;
        metrics_->spikeCount = 0;
        metrics_->totalStepTime = 0.0;
        metrics_->memoryUsage = 0;
        metrics_->activeNeurons = 0;
        metrics_->activeSynapses = 0;

        std::fill(std::begin(metrics_->neuronActivityHistory), std::end(metrics_->neuronActivityHistory), 0.0f);
        std::fill(std::begin(metrics_->spikeRateHistory), std::end(metrics_->spikeRateHistory), 0.0f);
        std::fill(std::begin(metrics_->connectivityHistory), std::end(metrics_->connectivityHistory), 0.0f);
    }

    bool isMonitoring() const { return monitoringActive_; }
};

// Parallel processing utilities for large-scale simulations
class ParallelProcessor {
public:
    // Process neurons in parallel using threads
    static void processNeuronsParallel(std::vector<Neuron*>& neurons,
                                       std::function<void(Neuron*)> processor,
                                       size_t maxThreads = std::thread::hardware_concurrency()) {
        size_t neuronCount = neurons.size();
        if (neuronCount == 0) return;

        size_t threadsToUse = std::min(maxThreads, neuronCount);
        size_t neuronsPerThread = neuronCount / threadsToUse;

        std::vector<std::thread> threads;
        threads.reserve(threadsToUse);

        for (size_t t = 0; t < threadsToUse; ++t) {
            size_t startIdx = t * neuronsPerThread;
            size_t endIdx = (t == threadsToUse - 1) ? neuronCount : (t + 1) * neuronsPerThread;

            threads.emplace_back([&, startIdx, endIdx]() {
                for (size_t i = startIdx; i < endIdx; ++i) {
                    processor(neurons[i]);
                }
            });
        }

        for (auto& thread : threads) {
            thread.join();
        }
    }

    // Process synapses in parallel with SIMD optimization
    static void processSynapsesParallel(std::vector<Synapse*>& synapses,
                                        std::function<void(Synapse*, size_t)> processor,
                                        size_t maxThreads = std::thread::hardware_concurrency()) {
        size_t synapseCount = synapses.size();
        if (synapseCount == 0) return;

        size_t threadsToUse = std::min(maxThreads, synapseCount);
        size_t synapsesPerThread = synapseCount / threadsToUse;

        std::vector<std::thread> threads;
        threads.reserve(threadsToUse);

        for (size_t t = 0; t < threadsToUse; ++t) {
            size_t startIdx = t * synapsesPerThread;
            size_t endIdx = (t == threadsToUse - 1) ? synapseCount : (t + 1) * synapsesPerThread;

            threads.emplace_back([&, startIdx, endIdx]() {
                for (size_t i = startIdx; i < endIdx; ++i) {
                    processor(synapses[i], i);
                }
            });
        }

        for (auto& thread : threads) {
            thread.join();
        }
    }

    // SIMD-optimized matrix operations for neural network computations
    static void matrixMultiplySIMD(const std::vector<std::vector<float>>& A,
                                   const std::vector<std::vector<float>>& B,
                                   std::vector<std::vector<float>>& C) {
        size_t rowsA = A.size();
        size_t colsA = A[0].size();
        size_t colsB = B[0].size();

        C.resize(rowsA, std::vector<float>(colsB, 0.0f));

        for (size_t i = 0; i < rowsA; ++i) {
            for (size_t j = 0; j < colsB; ++j) {
                float sum = 0.0f;
                for (size_t k = 0; k < colsA; ++k) {
                    sum += A[i][k] * B[k][j];
                }
                C[i][j] = sum;
            }
        }
    }

    // Event-driven parallel processing
    static void processEventsParallel(std::vector<std::function<void()>>& events,
                                      size_t maxThreads = std::thread::hardware_concurrency()) {
        size_t eventCount = events.size();
        if (eventCount == 0) return;

        size_t threadsToUse = std::min(maxThreads, eventCount);
        size_t eventsPerThread = eventCount / threadsToUse;

        std::vector<std::thread> threads;
        threads.reserve(threadsToUse);

        for (size_t t = 0; t < threadsToUse; ++t) {
            size_t startIdx = t * eventsPerThread;
            size_t endIdx = (t == threadsToUse - 1) ? eventCount : (t + 1) * eventsPerThread;

            threads.emplace_back([&, startIdx, endIdx]() {
                for (size_t i = startIdx; i < endIdx; ++i) {
                    try {
                        events[i]();
                    } catch (const std::exception& e) {
                        std::cerr << "Error processing event " << i << ": " << e.what() << std::endl;
                    }
                }
            });
        }

        for (auto& thread : threads) {
            thread.join();
        }
    }
};

} // namespace nlm

#endif // NLM_PERFORMANCE_H
