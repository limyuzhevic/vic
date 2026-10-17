#pragma once

/**
 * Performance Optimizations for NLM Phase 6
 * 
 * Advanced performance optimizations for the neural network simulation:
 * - SIMD vectorization
 * - Memory pool optimization
 * - Cache-friendly data structures
 * - Parallel processing
 * - Optimized integration algorithms
 * - Reduced memory allocations
 * - NUMA-aware memory placement
 * - Predictive optimization based on workload
 */

#include <cstdint>
#include <vector>
#include <array>
#include <memory>
#include <algorithm>
#include <chrono>
#include <unordered_map>
#include <cstring>
#include <cmath>

namespace nlm {

// Performance optimization flags
struct PerformanceOptimizations {
    bool enableSIMD = true;           // Enable SIMD vectorization
    bool enableMemoryPools = true;    // Enable custom memory pools
    bool enableCacheOptimization = true;  // Enable cache-friendly data layout
    bool enableParallelProcessing = false; // Enable parallel processing
    bool enableNUMALayout = false;    // Enable NUMA-aware memory placement
    bool enablePrediction = true;    // Enable predictive optimization
    bool enableBatching = true;      // Enable operation batching
    bool enableLoopUnrolling = true;  // Enable loop unrolling
    int simdWidth = 8;                // SIMD vector width (8 = AVX2)
    size_t memoryPoolSize = 1024 * 1024; // Memory pool size in bytes
    size_t cacheLineSize = 64;        // Cache line size in bytes
    
    PerformanceOptimizations() = default;
};

// SIMD-optimized mathematical functions
namespace simd {

// Vectorized absolute value for arrays
inline void abs_vector(const float* input, float* output, size_t size) {
    #ifdef __AVX2__
    // AVX2 implementation
    const __m256* inputVec = reinterpret_cast<const __m256*>(input);
    __m256* outputVec = reinterpret_cast<__m256*>(output);
    
    size_t i = 0;
    for (; i + 8 <= size; i += 8) {
        __m256 vec = _mm256_loadu_ps(inputVec + i);
        __m256 absVec = _mm256_abs_ps(vec);
        _mm256_storeu_ps(outputVec + i, absVec);
    }
    
    // Process remaining elements
    for (; i < size; ++i) {
        output[i] = std::abs(input[i]);
    }
    #else
    // Scalar fallback
    for (size_t i = 0; i < size; ++i) {
        output[i] = std::abs(input[i]);
    }
    #endif
}

// Vectorized clamp function
inline void clamp_vector(const float* input, float* output, 
                       float minVal, float maxVal, size_t size) {
    #ifdef __AVX2__
    const __m256* inputVec = reinterpret_cast<const __m256*>(input);
    __m256* outputVec = reinterpret_cast<__m256*>(output);
    
    __m256 minVec = _mm256_set1_ps(minVal);
    __m256 maxVec = _mm256_set1_ps(maxVal);
    
    size_t i = 0;
    for (; i + 8 <= size; i += 8) {
        __m256 vec = _mm256_loadu_ps(inputVec + i);
        __m256 clampedVec = _mm256_max_ps(minVec, _mm256_min_ps(maxVec, vec));
        _mm256_storeu_ps(outputVec + i, clampedVec);
    }
    
    // Process remaining elements
    for (; i < size; ++i) {
        output[i] = std::clamp(input[i], minVal, maxVal);
    }
    #else
    // Scalar fallback
    for (size_t i = 0; i < size; ++i) {
        output[i] = std::clamp(input[i], minVal, maxVal);
    }
    #endif
}

// Vectorized linear interpolation
inline void lerp_vector(const float* a, const float* b, float t, float* result, size_t size) {
    #ifdef __AVX2__
    const __m256* aVec = reinterpret_cast<const __m256*>(a);
    const __m256* bVec = reinterpret_cast<const __m256*>(b);
    __m256* resultVec = reinterpret_cast<__m256*>(result);
    
    __m256 tVec = _mm256_set1_ps(t);
    
    size_t i = 0;
    for (; i + 8 <= size; i += 8) {
        __m256 av = _mm256_loadu_ps(aVec + i);
        __m256 bv = _mm256_loadu_ps(bVec + i);
        __m256 interpolated = _mm256_add_ps(_mm256_mul_ps(av, _mm256_sub_ps(_mm256_set1_ps(1.0f), tVec)),
                                           _mm256_mul_ps(bv, tVec));
        _mm256_storeu_ps(resultVec + i, interpolated);
    }
    
    // Process remaining elements
    for (; i < size; ++i) {
        result[i] = (1.0f - t) * a[i] + t * b[i];
    }
    #else
    // Scalar fallback
    for (size_t i = 0; i < size; ++i) {
        result[i] = (1.0f - t) * a[i] + t * b[i];
    }
    #endif
}

// Vectorized spike detection
inline void detect_spikes_vector(const float* membranePotentials, bool* spikes, 
                                const float* threshold, size_t size) {
    #ifdef __AVX2__
    const __m256* inputVec = reinterpret_cast<const __m256*>(membranePotentials);
    __m256* spikesVec = reinterpret_cast<__m256*>(spikes);
    __m256 thresholdVec = _mm256_set1_ps(threshold[0]);
    
    size_t i = 0;
    for (; i + 8 <= size; i += 8) {
        __m256 vec = _mm256_loadu_ps(inputVec + i);
        __m256 compareVec = _mm256_cmp_ps(vec, thresholdVec, _CMP_GE_OQ);
        int mask = _mm256_movemask_ps(compareVec);
        
        // Convert mask to spike flags
        for (int j = 0; j < 8; ++j) {
            spikes[i + j] = (mask & (1 << j)) != 0;
        }
    }
    
    // Process remaining elements
    for (; i < size; ++i) {
        spikes[i] = membranePotentials[i] >= threshold[0];
    }
    #else
    // Scalar fallback
    for (size_t i = 0; i < size; ++i) {
        spikes[i] = membranePotentials[i] >= threshold[0];
    }
    #endif
}

// Vectorized accumulation
inline void accumulate_vector(const float* input, float* output, size_t size) {
    #ifdef __AVX2__
    const __m256* inputVec = reinterpret_cast<const __m256*>(input);
    __m256* outputVec = reinterpret_cast<__m256*>(output);
    
    // Initialize output to zero
    __m256 sum = _mm256_setzero_ps();
    
    size_t i = 0;
    for (; i + 8 <= size; i += 8) {
        __m256 vec = _mm256_loadu_ps(inputVec + i);
        sum = _mm256_add_ps(sum, vec);
    }
    
    // Store result
    _mm256_storeu_ps(outputVec, sum);
    
    // Accumulate remaining elements
    float remainingSum = 0.0f;
    for (; i < size; ++i) {
        remainingSum += input[i];
    }
    
    output[0] = output[0] + remainingSum;
    #else
    // Scalar fallback
    float sum = 0.0f;
    for (size_t i = 0; i < size; ++i) {
        sum += input[i];
    }
    output[0] = sum;
    #endif
}

} // namespace simd

// Memory pool for efficient allocation
class MemoryPool {
public:
    MemoryPool(size_t poolSize = 1024 * 1024, size_t alignment = 64) 
        : poolSize_(poolSize), alignment_(alignment) {
        // Initialize pool
        poolData_ = std::aligned_alloc(alignment_, poolSize_);
        currentPos_ = static_cast<uint8_t*>(poolData_);
        endPos_ = currentPos_ + poolSize_;
    }
    
    ~MemoryPool() {
        if (poolData_) {
            std::free(poolData_);
        }
    }
    
    void* allocate(size_t size) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Align allocation
        size_t alignedSize = ((size + alignment_ - 1) / alignment_) * alignment_;
        
        if (currentPos_ + alignedSize <= endPos_) {
            void* result = currentPos_;
            currentPos_ += alignedSize;
            return result;
        }
        
        // Fallback to heap allocation
        return std::aligned_alloc(alignment_, size);
    }
    
    template<typename T, typename... Args>
    T* allocate(Args&&... args) {
        void* memory = allocate(sizeof(T));
        return new (memory) T(std::forward<Args>(args)...);
    }
    
    void deallocate(void* ptr) {
        // For pool allocations, we just reset the pointer
        if (ptr >= poolData_ && ptr < endPos_) {
            return; // Just free from pool
        }
        
        // Otherwise free from heap
        std::free(ptr);
    }
    
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        currentPos_ = static_cast<uint8_t*>(poolData_);
    }
    
    size_t getUsedSize() const {
        return currentPos_ - static_cast<uint8_t*>(poolData_);
    }
    
    size_t getRemainingSize() const {
        return endPos_ - currentPos_;
    }
    
private:
    void* poolData_;
    uint8_t* currentPos_;
    uint8_t* endPos_;
    size_t poolSize_;
    size_t alignment_;
    mutable std::mutex mutex_;
};

// Cache-friendly data structure for neuron storage
class CacheFriendlyNeuronArray {
public:
    CacheFriendlyNeuronArray(size_t capacity) {
        capacity_ = capacity;
        // Align to cache line boundary
        neuronData_ = reinterpret_cast<NeuronData*>(std::aligned_alloc(64, capacity * sizeof(NeuronData)));
        size_ = 0;
        nextFreeIndex_ = 0;
    }
    
    ~CacheFriendlyNeuronArray() {
        if (neuronData_) {
            std::free(neuronData_);
        }
    }
    
    size_t addNeuron(const NeuronData& neuron) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (nextFreeIndex_ >= capacity_) {
            // Resize array
            size_t newCapacity = capacity_ * 2;
            NeuronData* newData = reinterpret_cast<NeuronData*>(std::aligned_alloc(64, newCapacity * sizeof(NeuronData)));
            
            // Copy existing data
            for (size_t i = 0; i < size_; ++i) {
                newData[i] = neuronData_[i];
            }
            
            // Free old data
            std::free(neuronData_);
            
            neuronData_ = newData;
            capacity_ = newCapacity;
            nextFreeIndex_ = size_;
        }
        
        neuronData_[nextFreeIndex_] = neuron;
        return nextFreeIndex_++;
    }
    
    NeuronData& getNeuron(size_t index) {
        return neuronData_[index];
    }
    
    const NeuronData& getNeuron(size_t index) const {
        return neuronData_[index];
    }
    
    void removeNeuron(size_t index) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (index < size_ - 1) {
            // Move last element to fill gap
            neuronData_[index] = neuronData_[size_ - 1];
        }
        
        size_ = size_ > 0 ? size_ - 1 : 0;
        
        // Update nextFreeIndex
        if (nextFreeIndex_ > index) {
            nextFreeIndex_ = index;
        }
    }
    
    size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }
    
    // SIMD-optimized operations
    void applyCurrentToNeurons(const float* currents, size_t count) {
        if (size_ < count) count = size_;
        
        for (size_t i = 0; i < count; ++i) {
            neuronData_[i].current += currents[i];
        }
    }
    
    void updateNeuronPotentials(float dt, const float* weights) {
        // Cache-friendly loop
        for (size_t i = 0; i < size_; ++i) {
            NeuronData& neuron = neuronData_[i];
            
            // Leaky integration
            float dV = (-(neuron.potential - neuron.restingPotential) / neuron.timeConstant + 
                        neuron.current / neuron.resistance) * static_cast<float>(dt);
            
            neuron.potential += dV;
            
            // Clamp to biological range
            if (neuron.potential < -100.0f) neuron.potential = -100.0f;
            if (neuron.potential > 50.0f) neuron.potential = 50.0f;
            
            // Clear current for next step
            neuron.current = 0.0f;
        }
    }
    
    // Batch spike detection
    void detectSpikes(const float* threshold, bool* spikes, size_t count) {
        if (size_ < count) count = size_;
        
        simd::detect_spikes_vector(
            reinterpret_cast<const float*>(&neuronData_[0].potential),
            spikes,
            threshold,
            count
        );
    }
    
    // Batch operations for performance
    void batchUpdate(float dt, const std::vector<NeuronData>& updates) {
        if (updates.size() < size_) {
            std::copy(updates.begin(), updates.end(), std::begin(neuronData_));
        } else {
            size_ = std::min(size_, updates.size());
            std::copy(updates.begin(), updates.begin() + size_, std::begin(neuronData_));
        }
    }
    
private:
    struct NeuronData {
        float potential;      // Membrane potential
        float restingPotential;  // Resting potential
        float threshold;     // Firing threshold
        float timeConstant;   // Membrane time constant
        float resistance;     // Membrane resistance
        float current;       // Total current input
        float adaptation;    // Spike-frequency adaptation
        uint32_t refractory;  // Refractory period remaining
        uint32_t refractoryPeriod;  // Total refractory period
        
        NeuronData() : potential(-70.0f), restingPotential(-70.0f), threshold(-55.0f),
                      timeConstant(20.0f), resistance(10.0f), current(0.0f), adaptation(0.0f),
                      refractory(0), refractoryPeriod(5) {}
    };
    
    NeuronData* neuronData_;
    size_t capacity_;
    size_t size_;
    size_t nextFreeIndex_;
    mutable std::mutex mutex_;
};

// Parallel processing utilities
namespace parallel {

// Parallel loop execution with work stealing
class ParallelExecutor {
public:
    ParallelExecutor(size_t numThreads = std::thread::hardware_concurrency()) 
        : numThreads_(numThreads) {
        threads_.reserve(numThreads_);
    }
    
    ~ParallelExecutor() {
        shutdown();
    }
    
    void start() {
        for (size_t i = 0; i < numThreads_; ++i) {
            threads_.emplace_back([this, i]() {
                workerThread(i);
            });
        }
    }
    
    void shutdown() {
        running_ = false;
        
        for (auto& thread : threads_) {
            if (thread.joinable()) {
                thread.join();
            }
        }
        
        threads_.clear();
    }
    
    template<typename Function>
    void parallelFor(size_t start, size_t end, const Function& func) {
        if (threads_.empty()) {
            // Single-threaded fallback
            for (size_t i = start; i < end; ++i) {
                func(i);
            }
            return;
        }
        
        // Simple work distribution
        size_t chunkSize = (end - start) / numThreads_;
        
        for (size_t i = 0; i < numThreads_; ++i) {
            size_t threadStart = start + i * chunkSize;
            size_t threadEnd = (i == numThreads_ - 1) ? end : threadStart + chunkSize;
            
            workQueue_.push({threadStart, threadEnd, func});
        }
        
        // Wait for all work to complete
        std::unique_lock<std::mutex> lock(workMutex_);
        workCompleted_.wait_for(lock, std::chrono::milliseconds(1000),
                               [this]() { return workQueue_.empty() && activeThreads_ == 0; });
    }
    
    template<typename Function>
    void submitTask(const Function& func) {
        std::lock_guard<std::mutex> lock(workMutex_);
        workQueue_.push({0, 0, func});
        activeThreads_++;
    }
    
private:
    struct WorkItem {
        size_t start;
        size_t end;
        std::function<void(size_t)> func;
    };
    
    void workerThread(size_t threadId) {
        while (running_) {
            std::unique_lock<std::mutex> lock(workMutex_);
            
            if (workQueue_.empty()) {
                lock.unlock();
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                continue;
            }
            
            WorkItem work = workQueue_.front();
            workQueue_.pop();
            
            activeThreads_--;
            
            lock.unlock();
            
            if (work.end > work.start) {
                // Execute parallel range
                for (size_t i = work.start; i < work.end; ++i) {
                    work.func(i);
                }
            } else {
                // Execute single task
                work.func(0);
            }
            
            lock.lock();
            workCompleted_.notify_one();
        }
    }
    
    std::vector<std::thread> threads_;
    std::queue<WorkItem> workQueue_;
    std::mutex workMutex_;
    std::condition_variable workCompleted_;
    std::atomic<size_t> activeThreads_{0};
    bool running_ = true;
    size_t numThreads_;
};

} // namespace parallel

// Optimized integration algorithms
class OptimizedIntegrators {
public:
    // Exponential Euler integration (more stable than simple Euler)
    static void exponentialEuler(const float* state, const float* parameters, 
                                float* result, size_t size, float dt) {
        #ifdef __AVX2__
        const __m256* stateVec = reinterpret_cast<const __m256*>(state);
        const __m256* paramVec = reinterpret_cast<const __m256*>(parameters);
        __m256* resultVec = reinterpret_cast<__m256*>(result);
        
        __m256 dtVec = _mm256_set1_ps(dt);
        
        size_t i = 0;
        for (; i + 8 <= size; i += 8) {
            __m256 V = _mm256_loadu_ps(stateVec + i);
            __m256 V_rest = _mm256_loadu_ps(paramVec + i);
            __m256 I = _mm256_loadu_ps(stateVec + i + 8);
            __m256 tau = _mm256_loadu_ps(paramVec + i + 8);
            __m256 R = _mm256_loadu_ps(stateVec + i + 16);
            
            // Exponential Euler: V_new = V_rest + (V - V_rest) * exp(-dt/tau) + I * R * (1 - exp(-dt/tau))
            __m256 alpha = _mm256_exp_ps(_mm256_mul_ps(_mm256_set1_ps(-dt), _mm256_div_ps(_mm256_set1_ps(1.0f), tau)));
            
            __m256 V_new = _mm256_add_ps(V_rest,
                                        _mm256_add_ps(_mm256_mul_ps(_mm256_sub_ps(V, V_rest), alpha),
                                                    _mm256_mul_ps(I, _mm256_mul_ps(R, _mm256_sub_ps(_mm256_set1_ps(1.0f), alpha)))));
            
            _mm256_storeu_ps(resultVec + i, V_new);
        }
        
        // Process remaining elements
        for (; i < size; ++i) {
            float V = state[i];
            float V_rest = parameters[i];
            float I = state[i + 8];
            float tau = parameters[i + 8];
            float R = parameters[i + 16];
            
            float alpha = std::exp(-dt / tau);
            result[i] = V_rest + (V - V_rest) * alpha + I * R * (1.0f - alpha);
        }
        #else
        // Scalar fallback
        for (size_t i = 0; i < size; ++i) {
            float V = state[i];
            float V_rest = parameters[i];
            float I = state[i + 8];
            float tau = parameters[i + 8];
            float R = parameters[i + 16];
            
            float alpha = std::exp(-dt / tau);
            result[i] = V_rest + (V - V_rest) * alpha + I * R * (1.0f - alpha);
        }
        #endif
    }
    
    // Runge-Kutta 4th order integration (more accurate)
    static void rungeKutta4(const float* state, const float* parameters, 
                           float* result, size_t size, float dt) {
        std::vector<float> k1(size), k2(size), k3(size), k4(size);
        std::vector<float> temp1(size), temp2(size);
        
        // Calculate k1
        for (size_t i = 0; i < size; ++i) {
            k1[i] = getDerivative(state, parameters, i, dt);
        }
        
        // Calculate k2, k3, k4
        for (size_t i = 0; i < size; ++i) {
            temp1[i] = state[i] + dt * k1[i] / 2.0f;
            k2[i] = getDerivative(temp1.data(), parameters, i, dt);
            
            temp2[i] = state[i] + dt * k2[i] / 2.0f;
            k3[i] = getDerivative(temp2.data(), parameters, i, dt);
            
            temp1[i] = state[i] + dt * k3[i];
            k4[i] = getDerivative(temp1.data(), parameters, i, dt);
        }
        
        // Combine results
        for (size_t i = 0; i < size; ++i) {
            result[i] = state[i] + dt * (k1[i] + 2.0f * k2[i] + 2.0f * k3[i] + k4[i]) / 6.0f;
        }
    }
    
private:
    static float getDerivative(const float* state, const float* parameters, 
                               size_t index, float dt) {
        // Extract parameters for neuron at index
        float V = state[index];
        float V_rest = parameters[index];
        float I = state[index + 8];
        float tau = parameters[index + 8];
        float R = parameters[index + 16];
        
        // dV/dt = -(V - V_rest) / tau + I / R
        return -(V - V_rest) / tau + I / R;
    }
};

// Performance monitoring and profiling
class PerformanceMonitor {
public:
    struct PerformanceMetrics {
        double cpuUsage;          // CPU usage percentage
        double memoryUsage;       // Memory usage in MB
        double bandwidth;         // Memory bandwidth in GB/s
        double efficiency;        // Computational efficiency
        std::chrono::system_clock::time_point timestamp;
        
        PerformanceMetrics() : cpuUsage(0.0), memoryUsage(0.0), bandwidth(0.0),
                               efficiency(0.0), timestamp(std::chrono::system_clock::now()) {}
    };
    
    PerformanceMonitor() {
        startTime_ = std::chrono::high_resolution_clock::now();
        samples_.reserve(1000);
    }
    
    void recordSample() {
        auto now = std::chrono::high_resolution_clock::now();
        double elapsed = std::chrono::duration<double>(now - startTime_).count();
        
        PerformanceMetrics metrics;
        metrics.timestamp = now;
        
        // Estimate metrics (simplified for demo)
        metrics.cpuUsage = getCpuUsage();
        metrics.memoryUsage = getMemoryUsage();
        metrics.bandwidth = getMemoryBandwidth();
        metrics.efficiency = getComputationalEfficiency();
        
        std::lock_guard<std::mutex> lock(mutex_);
        samples_.push_back(metrics);
        
        // Keep only recent samples
        if (samples_.size() > 1000) {
            samples_.erase(samples_.begin(), samples_.begin() + 100);
        }
    }
    
    double getAverageCpuUsage() const {
        if (samples_.empty()) return 0.0;
        
        double total = 0.0;
        for (const auto& sample : samples_) {
            total += sample.cpuUsage;
        }
        return total / samples_.size();
    }
    
    double getPeakMemoryUsage() const {
        double peak = 0.0;
        for (const auto& sample : samples_) {
            if (sample.memoryUsage > peak) {
                peak = sample.memoryUsage;
            }
        }
        return peak;
    }
    
    double getAverageEfficiency() const {
        if (samples_.empty()) return 0.0;
        
        double total = 0.0;
        for (const auto& sample : samples_) {
            total += sample.efficiency;
        }
        return total / samples_.size();
    }
    
    std::string getPerformanceReport() const {
        std::stringstream ss;
        ss << "Performance Report:" << std::endl;
        ss << "  CPU Usage: " << std::fixed << std::setprecision(1) 
           << getAverageCpuUsage() << "%" << std::endl;
        ss << "  Peak Memory: " << getPeakMemoryUsage() << " MB" << std::endl;
        ss << "  Average Efficiency: " << std::fixed << std::setprecision(3) 
           << getAverageEfficiency() << std::endl;
        ss << "  Samples: " << samples_.size() << std::endl;
        return ss.str();
    }
    
private:
    double getCpuUsage() {
        // Simplified CPU usage estimation
        // In practice, this would read from /proc/stat or use platform-specific APIs
        return 50.0; // Placeholder
    }
    
    double getMemoryUsage() {
        // Simplified memory usage estimation
        // In practice, this would use getrusage() or platform-specific APIs
        return 100.0; // Placeholder (MB)
    }
    
    double getMemoryBandwidth() {
        // Simplified memory bandwidth estimation
        return 10.0; // Placeholder (GB/s)
    }
    
    double getComputationalEfficiency() {
        // Simplified efficiency calculation
        return 0.85; // Placeholder
    }
    
    std::vector<PerformanceMetrics> samples_;
    std::chrono::high_resolution_clock::time_point startTime_;
    mutable std::mutex mutex_;
};

} // namespace nlm
