#include "MemoryPool.hpp"
#include <algorithm>
#include <numeric>
#include <chrono>
#include <thread>
#include <atomic>
#include <future>
#include <immintrin.h>

namespace nlm {

// Global memory pools for system-wide neuron and synapse management
static std::unique_ptr<NeuronPool> g_neuronPool;
static std::unique_ptr<SynapsePool> g_synapsePool;
static std::mutex g_poolMutex;
static std::atomic<bool> g_poolsInitialized{false};

// Performance statistics
struct PerformanceStats {
    std::atomic<size_t> totalAllocations{0};
    std::atomic<size_t> totalDeallocations{0};
    std::atomic<size_t> activeObjects{0};
    std::chrono::high_resolution_clock::time_point lastUpdate;
    double avgAllocationTime{0.0};
    size_t maxAllocated{0};
    
    PerformanceStats() : lastUpdate(std::chrono::high_resolution_clock::now()) {}
    
    void updateAllocation(size_t count = 1) {
        totalAllocations.fetch_add(count, std::memory_order_relaxed);
        activeObjects.fetch_add(count, std::memory_order_relaxed);
        size_t current = activeObjects.load(std::memory_order_relaxed);
        if (current > maxAllocated) {
            maxAllocated = current;
        }
    }
    
    void updateDeallocation(size_t count = 1) {
        totalDeallocations.fetch_add(count, std::memory_order_relaxed);
        activeObjects.fetch_sub(count, std::memory_order_relaxed);
    }
    
    double getUtilizationRate() const {
        size_t totalOperations = totalAllocations.load(std::memory_order_relaxed) +
                                 totalDeallocations.load(std::memory_order_relaxed);
        if (totalOperations == 0) return 0.0;
        return static_cast<double>(activeObjects.load(std::memory_order_relaxed)) / 
               static_cast<double>(maxAllocated + 1);
    }
};

static PerformanceStats g_performanceStats;

// Initialize global memory pools
void initializeMemoryPools(size_t neuronCapacity, size_t synapseCapacity) {
    std::lock_guard<std::mutex> lock(g_poolMutex);
    
    if (g_poolsInitialized) {
        return;
    }
    
    try {
        g_neuronPool = std::make_unique<NeuronPool>();
        g_synapsePool = std::make_unique<SynapsePool>();
        
        // Reserve initial capacity
        g_neuronPool->reserve(neuronCapacity);
        g_synapsePool->reserve(synapseCapacity);
        
        g_poolsInitialized = true;
        NLM_LOG_INFO("Global memory pools initialized");
        NLM_LOG_INFO("Neuron pool capacity: " + std::to_string(neuronCapacity));
        NLM_LOG_INFO("Synapse pool capacity: " + std::to_string(synapseCapacity));
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to initialize memory pools: " + std::string(e.what()));
        g_neuronPool.reset();
        g_synapsePool.reset();
        g_poolsInitialized = false;
    }
}

void shutdownMemoryPools() {
    std::lock_guard<std::mutex> lock(g_poolMutex);
    
    if (!g_poolsInitialized) {
        return;
    }
    
    NLM_LOG_INFO("Shutting down memory pools...");
    
    // Log final statistics
    NLM_LOG_INFO("Memory pool statistics:");
    NLM_LOG_INFO("  Total allocations: " + std::to_string(g_performanceStats.totalAllocations));
    NLM_LOG_INFO("  Total deallocations: " + std::to_string(g_performanceStats.totalDeallocations));
    NLM_LOG_INFO("  Active objects: " + std::to_string(g_performanceStats.activeObjects));
    NLM_LOG_INFO("  Max allocated: " + std::to_string(g_performanceStats.maxAllocated));
    NLM_LOG_INFO("  Utilization rate: " + std::to_string(g_performanceStats.getUtilizationRate() * 100) + "%");
    
    g_neuronPool.reset();
    g_synapsePool.reset();
    g_poolsInitialized = false;
}

// Global access functions
NeuronPool* getNeuronPool() {
    std::lock_guard<std::mutex> lock(g_poolMutex);
    return g_neuronPool.get();
}

SynapsePool* getSynapsePool() {
    std::lock_guard<std::mutex> lock(g_poolMutex);
    return g_synapsePool.get();
}

// Advanced SIMD optimizations for neural computations
namespace SIMD {
    
    // AVX2-optimized neural state updates
    inline void updateNeuronStatesAVX2(
        float* membranePotentials,
        const float* synapticInputs,
        const float* adaptationVariables,
        float leakConductance,
        float resetPotential,
        float threshold,
        size_t count,
        uint8_t* firingStates,
        float* lastSpikeTimes,
        uint32_t* refractoryRemainings,
        uint32_t* refractoryPeriods,
        float* adaptationVars,
        float* lastSpikeTimesOut,
        float currentTime
    ) {
        if (count < 8) {
            // Fallback to scalar for small arrays
            return updateNeuronStatesScalar(
                membranePotentials, synapticInputs, adaptationVariables,
                leakConductance, resetPotential, threshold,
                count, firingStates, lastSpikeTimes,
                refractoryRemainings, refractoryPeriods, adaptationVars,
                lastSpikeTimesOut, currentTime
            );
        }
        
        __m256 leakVec = _mm256_set1_ps(leakConductance);
        __m256 resetVec = _mm256_set1_ps(resetPotential);
        __m256 thresholdVec = _mm256_set1_ps(threshold);
        __m256 currentTimeVec = _mm256_set1_ps(currentTime);
        
        size_t i = 0;
        for (; i + 8 <= count; i += 8) {
            // Load neuron states
            __m256 v = _mm256_loadu_ps(&membranePotentials[i]);
            __m256 synIn = _mm256_loadu_ps(&synapticInputs[i]);
            __m256 adapt = _mm256_loadu_ps(&adaptationVariables[i]);
            
            // LIF dynamics: V = V + dt * ((V_rest - V)/tau + I/C)
            __m256 leakTerm = _mm256_mul_ps(leakVec, v);
            __m256 inputTerm = _mm256_mul_ps(synIn, leakVec); // I/C (assuming C=1)
            __m256 deltaV = _mm256_mul_ps(_mm256_set1_ps(0.001f),  // dt
                                        _mm256_sub_ps(leakTerm, inputTerm));
            
            v = _mm256_add_ps(v, deltaV);
            
            // Apply adaptation
            if (*adaptationVariables > 0.0f) {
                v = _mm256_sub_ps(v, _mm256_mul_ps(adapt, _mm256_set1_ps(0.01f)));
                adapt = _mm256_mul_ps(adapt, _mm256_set1_ps(0.95f));
            }
            
            // Clamp membrane potential
            v = _mm256_max_ps(v, _mm256_set1_ps(-100.0f));
            v = _mm256_min_ps(v, _mm256_set1_ps(50.0f));
            
            // Check for spike
            __m256 spikeMask = _mm256_cmpge_ps(v, thresholdVec);
            
            // Store results
            _mm256_storeu_ps(&membranePotentials[i], v);
            _mm256_storeu_ps(&adaptationVariables[i], adapt);
            
            // Handle spikes
            int spikeMaskInt = _mm256_movemask_ps(spikeMask);
            if (spikeMaskInt != 0) {
                for (int j = 0; j < 8; ++j) {
                    if (spikeMaskInt & (1 << j)) {
                        size_t idx = i + j;
                        if (idx < count) {
                            firingStates[idx] = 2; // Active
                            lastSpikeTimes[idx] = currentTime;
                            membranePotentials[idx] = resetPotential;
                            refractoryRemainings[idx] = refractoryPeriods[idx];
                            adaptationVars[idx] += 1.0f;
                        }
                    }
                }
            }
        }
        
        // Handle remaining elements with scalar
        updateNeuronStatesScalar(
            &membranePotentials[i], &synapticInputs[i], &adaptationVariables[i],
            leakConductance, resetPotential, threshold,
            count - i, &firingStates[i], &lastSpikeTimes[i],
            &refractoryRemainings[i], &refractoryPeriods[i], &adaptationVars[i],
            &lastSpikeTimesOut[i], currentTime
        );
    }
    
    // Scalar fallback for small arrays or non-AVX2 systems
    inline void updateNeuronStatesScalar(
        float* membranePotentials,
        const float* synapticInputs,
        const float* adaptationVariables,
        float leakConductance,
        float resetPotential,
        float threshold,
        size_t count,
        uint8_t* firingStates,
        float* lastSpikeTimes,
        uint32_t* refractoryRemainings,
        uint32_t* refractoryPeriods,
        float* adaptationVars,
        float* lastSpikeTimesOut,
        float currentTime
    ) {
        for (size_t i = 0; i < count; ++i) {
            float& v = membranePotentials[i];
            const float I = synapticInputs[i];
            float& adapt = adaptationVars[i];
            
            // LIF dynamics: V = V + dt * ((V_rest - V)/tau + I/C)
            // Assuming V_rest = -70, tau = 20ms, C = 1nF
            float leakTerm = v * leakConductance;
            float inputTerm = I * leakConductance; // I/C (assuming C=1)
            float deltaV = 0.001f * (( -70.0f - v) / 20.0f + inputTerm);
            
            v += deltaV;
            
            // Apply adaptation
            if (adapt > 0.0f) {
                v -= adapt * 0.01f;
                adapt *= 0.95f;
            }
            
            // Clamp membrane potential
            v = std::clamp(v, -100.0f, 50.0f);
            
            // Check for spike
            if (v >= threshold) {
                firingStates[i] = 2; // Active
                lastSpikeTimes[i] = currentTime;
                v = resetPotential;
                refractoryRemainings[i] = refractoryPeriods[i];
                adapt += 1.0f;
            } else if (refractoryRemainings[i] > 0) {
                refractoryRemainings[i]--;
                firingStates[i] = 1; // Refractory
                v = resetPotential; // Hold at reset during refractory
            } else {
                firingStates[i] = 0; // Resting
            }
        }
    }
    
    // Vectorized synaptic weight updates with eligibility traces
    inline void updateSynapticWeightsAVX2(
        float* weights,
        const float* eligibilityTraces,
        const float* preSpikes,
        const float* postSpikes,
        float ltpWeight,
        float ltdWeight,
        float stdpTau,
        size_t count
    ) {
        if (count < 8) {
            return updateSynapticWeightsScalar(
                weights, eligibilityTraces, preSpikes, postSpikes,
                ltpWeight, ltdWeight, stdpTau, count
            );
        }
        
        __m256 ltpVec = _mm256_set1_ps(ltpWeight);
        __m256 ltdVec = _mm256_set1_ps(ltdWeight);
        __m256 stdpTauVec = _mm256_set1_ps(stdpTau);
        
        size_t i = 0;
        for (; i + 8 <= count; i += 8) {
            // Load synaptic states
            __m256 w = _mm256_loadu_ps(&weights[i]);
            __m256 e = _mm256_loadu_ps(&eligibilityTraces[i]);
            __m256 pre = _mm256_loadu_ps(&preSpikes[i]);
            __m256 post = _mm256_loadu_ps(&postSpikes[i]);
            
            // STDP update: dw/dt = e * (LTP * pre - LTD * post)
            __m256 deltaW = _mm256_mul_ps(e, 
                _mm256_sub_ps(_mm256_mul_ps(ltpVec, pre),
                            _mm256_mul_ps(ltdVec, post)));
            
            // Exponential decay of eligibility trace
            e = _mm256_mul_ps(e, _mm256_set1_ps(std::exp(-1.0f / stdpTau)));
            
            // Update weight with bounds
            w = _mm256_add_ps(w, deltaW);
            w = _mm256_max_ps(w, _mm256_set1_ps(0.0f));
            w = _mm256_min_ps(w, _mm256_set1_ps(1.0f));
            
            // Store updated values
            _mm256_storeu_ps(&weights[i], w);
            _mm256_storeu_ps(&eligibilityTraces[i], e);
        }
        
        // Handle remaining elements with scalar
        updateSynapticWeightsScalar(
            &weights[i], &eligibilityTraces[i], &preSpikes[i], &postSpikes[i],
            ltpWeight, ltdWeight, stdpTau, count - i
        );
    }
    
    // Scalar fallback for synaptic weight updates
    inline void updateSynapticWeightsScalar(
        float* weights,
        const float* eligibilityTraces,
        const float* preSpikes,
        const float* postSpikes,
        float ltpWeight,
        float ltdWeight,
        float stdpTau,
        size_t count
    ) {
        float stdpDecay = std::exp(-1.0f / stdpTau);
        
        for (size_t i = 0; i < count; ++i) {
            float& w = weights[i];
            float& e = eligibilityTraces[i];
            const float pre = preSpikes[i];
            const float post = postSpikes[i];
            
            // STDP update: dw = e * (LTP * pre - LTD * post)
            float deltaW = e * (ltpWeight * pre - ltdWeight * post);
            
            // Update weight with bounds
            w += deltaW;
            if (w < 0.0f) w = 0.0f;
            if (w > 1.0f) w = 1.0f;
            
            // Exponential decay of eligibility trace
            e *= stdpDecay;
        }
    }
}

// Event-driven spike processing with optimized event queue
class SpikeEventProcessor {
public:
    SpikeEventProcessor(size_t capacity = 1024 * 1024) {
        events_.reserve(capacity);
        active_ = true;
    }
    
    ~SpikeEventProcessor() {
        active_ = false;
        signal_.notify_all();
        processorThread_.join();
    }
    
    void start() {
        if (!processorThread_.joinable()) {
            processorThread_ = std::thread(&SpikeEventProcessor::processEvents, this);
        }
    }
    
    void stop() {
        active_ = false;
        signal_.notify_all();
        if (processorThread_.joinable()) {
            processorThread_.join();
        }
    }
    
    // Add spike event to queue
    void addSpikeEvent(NeuronId neuron, float time, float weight, NeuronId target) {
        std::lock_guard<std::mutex> lock(queueMutex_);
        
        // Pre-allocate event if possible
        if (events_.size() < events_.capacity()) {
            events_.push_back({neuron, time, weight, target});
            signal_.notify_one();
        }
        // Otherwise drop event (throttling)
    }
    
    // Get next event (non-blocking)
    bool getNextEvent(SpikeEvent& event) {
        std::lock_guard<std::mutex> lock(queueMutex_);
        
        if (events_.empty()) {
            return false;
        }
        
        event = events_.front();
        events_.erase(events_.begin());
        return true;
    }
    
    size_t pendingEvents() const {
        std::lock_guard<std::mutex> lock(queueMutex_);
        return events_.size();
    }
    
    // Clear all events
    void clear() {
        std::lock_guard<std::mutex> lock(queueMutex_);
        events_.clear();
    }
    
private:
    struct SpikeEvent {
        NeuronId neuron;
        float time;
        float weight;
        NeuronId target;
        
        SpikeEvent() : neuron(0), time(0.0f), weight(0.0f), target(0) {}
        
        SpikeEvent(NeuronId n, float t, float w, NeuronId tgt)
            : neuron(n), time(t), weight(w), target(tgt) {}
    };
    
    std::vector<SpikeEvent> events_;
    std::mutex queueMutex_;
    std::condition_variable signal_;
    std::thread processorThread_;
    std::atomic<bool> active_;
    
    // Process events in optimized batches
    void processEvents() {
        SpikeEvent event;
        
        while (active_ || !events_.empty()) {
            {
                std::unique_lock<std::mutex> lock(queueMutex_);
                
                // Wait for event or shutdown
                signal_.wait(lock, [this]() { return !active_ || !events_.empty(); });
                
                if (!active_ && events_.empty()) {
                    break;
                }
                
                event = events_.front();
                events_.erase(events_.begin());
            }
            
            // Process spike event (synchronized with main thread)
            processSpikeEvent(event);
        }
    }
    
    void processSpikeEvent(const SpikeEvent& event) {
        // Get target synapse pool
        auto* synapsePool = getSynapsePool();
        if (!synapsePool) {
            return;
        }
        
        // Process spike with optimized lookup
        for (SynapseIndex i = 0; i < synapsePool->size(); ++i) {
            NeuronId source = synapsePool->getSource(i);
            NeuronId target = synapsePool->getDestination(i);
            
            if (source == event.neuron) {
                // Apply synaptic transmission
                float& weight = synapsePool->getState<float>(i);
                weight += event.weight * 0.01f; // STDP-like update
            }
        }
    }
};

// Parallel processing utilities
namespace Parallel {
    
    // Parallel reduction for large arrays
    template<typename Iterator, typename T, typename BinaryOp>
    T parallel_reduce(Iterator begin, Iterator end, T init, BinaryOp op, size_t chunkSize = 0) {
        size_t length = std::distance(begin, end);
        
        if (length == 0) {
            return init;
        }
        
        if (length <= 1024 || std::thread::hardware_concurrency() == 0) {
            // Sequential reduction for small arrays
            return std::accumulate(begin, end, init, op);
        }
        
        // Parallel reduction
        size_t numThreads = std::thread::hardware_concurrency();
        if (chunkSize == 0) {
            chunkSize = std::max<size_t>(1, length / numThreads);
        }
        
        std::vector<T> results(numThreads, init);
        std::vector<std::future<T>> futures;
        
        auto processChunk = [&](size_t start, size_t end) {
            T result = init;
            for (size_t i = start; i < end; ++i) {
                result = op(result, *(begin + i));
            }
            return result;
        };
        
        size_t chunkCount = (length + chunkSize - 1) / chunkSize;
        
        for (size_t i = 0; i < chunkCount; ++i) {
            size_t start = i * chunkSize;
            size_t end = std::min(start + chunkSize, length);
            
            futures.push_back(std::async(std::launch::async, processChunk, start, end));
        }
        
        // Collect results
        size_t idx = 0;
        for (auto& future : futures) {
            results[idx++] = future.get();
        }
        
        // Final reduction
        return std::accumulate(results.begin(), results.end(), init, op);
    }
    
    // Parallel simulation steps
    class ParallelSimulator {
    public:
        ParallelSimulator(size_t numThreads = 0) {
            if (numThreads == 0) {
                numThreads = std::thread::hardware_concurrency();
            }
            numThreads_ = std::max<size_t>(1, numThreads);
            threads_.reserve(numThreads_);
        }
        
        ~ParallelSimulator() {
            stop();
        }
        
        void start() {
            stopped_ = false;
            for (size_t i = 0; i < numThreads_; ++i) {
                threads_.emplace_back(&ParallelSimulator::workerThread, this, i);
            }
        }
        
        void stop() {
            stopped_ = true;
            condition_.notify_all();
            for (auto& thread : threads_) {
                if (thread.joinable()) {
                    thread.join();
                }
            }
            threads_.clear();
        }
        
        // Submit work for processing
        template<typename Function>
        void submit(Function&& func) {
            std::unique_lock<std::mutex> lock(queueMutex_);
            workQueue_.push(std::forward<Function>(func));
            condition_.notify_one();
        }
        
        // Wait for all work to complete
        void wait() {
            std::unique_lock<std::mutex> lock(queueMutex_);
            while (!workQueue_.empty()) {
                condition_.wait(lock);
            }
        }
        
    private:
        struct WorkItem {
            std::function<void()> func;
            WorkItem(std::function<void()> f) : func(std::move(f)) {}
        };
        
        std::vector<std::thread> threads_;
        std::queue<WorkItem> workQueue_;
        std::mutex queueMutex_;
        std::condition_variable condition_;
        std::atomic<bool> stopped_;
        size_t numThreads_;
        
        void workerThread(size_t threadId) {
            while (!stopped_) {
                std::unique_lock<std::mutex> lock(queueMutex_);
                
                // Wait for work or shutdown
                condition_.wait(lock, [this]() { 
                    return stopped_ || !workQueue_.empty(); 
                });
                
                if (stopped_ && workQueue_.empty()) {
                    break;
                }
                
                if (!workQueue_.empty()) {
                    WorkItem work = std::move(workQueue_.front());
                    workQueue_.pop();
                    lock.unlock();
                    
                    // Execute work
                    work.func();
                    
                    lock.lock();
                    condition_.notify_one();
                }
            }
        }
    };
    
    // Thread-local storage for performance optimization
    class ThreadLocalStorage {
    public:
        ThreadLocalStorage(size_t capacity = 1024) {
            threadLocals_.reserve(capacity);
        }
        
        template<typename T>
        T& getLocal(const std::string& key) {
            std::lock_guard<std::mutex> lock(globalMutex_);
            
            size_t threadId = std::hash<std::thread::id>{}(std::this_thread::get_id());
            
            if (threadLocals_.find(threadId) == threadLocals_.end()) {
                threadLocals_[threadId] = std::make_unique<std::unordered_map<std::string, std::any>>();
            }
            
            auto& localMap = threadLocals_[threadId];
            
            if (localMap->find(key) == localMap->end()) {
                // Create new value (default-constructed)
                localMap->emplace(key, T());
            }
            
            return std::any_cast<T&>((*localMap)[key]);
        }
        
        void clear() {
            std::lock_guard<std::mutex> lock(globalMutex_);
            threadLocals_.clear();
        }
        
    private:
        std::unordered_map<size_t, std::unique_ptr<std::unordered_map<std::string, std::any>>> threadLocals_;
        std::mutex globalMutex_;
    };
}

// Performance monitoring and profiling
class PerformanceMonitor {
public:
    PerformanceMonitor() {
        reset();
    }
    
    void reset() {
        startTime_ = std::chrono::high_resolution_clock::now();
        totalSteps_ = 0;
        totalMemoryAllocated_ = 0;
        totalMemoryDeallocated_ = 0;
        peakMemoryUsage_ = 0;
        totalComputationTime_ = 0.0;
    }
    
    void recordStep(size_t steps = 1) {
        std::lock_guard<std::mutex> lock(mutex_);
        totalSteps_ += steps;
        auto now = std::chrono::high_resolution_clock::now();
        totalComputationTime_ += std::chrono::duration<double>(now - lastStepTime_).total_seconds();
        lastStepTime_ = now;
    }
    
    void recordMemoryAllocation(size_t bytes) {
        std::lock_guard<std::mutex> lock(mutex_);
        totalMemoryAllocated_ += bytes;
        peakMemoryUsage_ = std::max(peakMemoryUsage_, totalMemoryAllocated_ - totalMemoryDeallocated_);
    }
    
    void recordMemoryDeallocation(size_t bytes) {
        std::lock_guard<std::mutex> lock(mutex_);
        totalMemoryDeallocated_ += bytes;
    }
    
    double getStepsPerSecond() const {
        auto now = std::chrono::high_resolution_clock::now();
        auto totalTime = std::chrono::duration<double>(now - startTime_).total_seconds();
        if (totalTime == 0.0) return 0.0;
        return static_cast<double>(totalSteps_) / totalTime;
    }
    
    double getMemoryAllocationRate() const {
        auto now = std::chrono::high_resolution_clock::now();
        auto totalTime = std::chrono::duration<double>(now - startTime_).total_seconds();
        if (totalTime == 0.0) return 0.0;
        return static_cast<double>(totalMemoryAllocated_) / totalTime;
    }
    
    double getMemoryDeallocationRate() const {
        auto now = std::chrono::high_resolution_clock::now();
        auto totalTime = std::chrono::duration<double>(now - startTime_).total_seconds();
        if (totalTime == 0.0) return 0.0;
        return static_cast<double>(totalMemoryDeallocated_) / totalTime;
    }
    
    double getAverageComputationTime() const {
        auto now = std::chrono::high_resolution_clock::now();
        auto totalTime = std::chrono::duration<double>(now - startTime_).total_seconds();
        if (totalSteps_ == 0) return 0.0;
        return totalComputationTime_ / static_cast<double>(totalSteps_);
    }
    
    void printReport() const {
        std::cout << "=== PERFORMANCE REPORT ===" << std::endl;
        std::cout << "Total steps executed: " << totalSteps_ << std::endl;
        std::cout << "Steps per second: " << getStepsPerSecond() << std::endl;
        std::cout << "Total memory allocated: " << totalMemoryAllocated_ << " bytes" << std::endl;
        std::cout << "Total memory deallocated: " << totalMemoryDeallocated_ << " bytes" << std::endl;
        std::cout << "Peak memory usage: " << peakMemoryUsage_ << " bytes" << std::endl;
        std::cout << "Memory allocation rate: " << getMemoryAllocationRate() << " bytes/sec" << std::endl;
        std::cout << "Memory deallocation rate: " << getMemoryDeallocationRate() << " bytes/sec" << std::endl;
        std::cout << "Average computation time per step: " << getAverageComputationTime() * 1000 << " ms" << std::endl;
        std::cout << "============================" << std::endl;
    }
    
private:
    std::chrono::high_resolution_clock::time_point startTime_;
    std::chrono::high_resolution_clock::time_point lastStepTime_;
    std::mutex mutex_;
    
    size_t totalSteps_;
    size_t totalMemoryAllocated_;
    size_t totalMemoryDeallocated_;
    size_t peakMemoryUsage_;
    double totalComputationTime_;
};

} // namespace nlm
