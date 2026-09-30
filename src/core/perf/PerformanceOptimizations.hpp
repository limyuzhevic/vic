// Performance optimizations for NLM
// This file contains optimized implementations for critical performance paths

#include "brain/Neuron.hpp"
#include "core/Random/Random.hpp"
#include <chrono>
#include <numeric>

namespace nlm {

// Performance optimizations namespace
namespace perf {

// Optimized LIF neuron computation using SIMD-style optimizations
class OptimizedNeuron {
public:
    // High-performance LIF neuron implementation
    static bool stepLIFOptimized(
        NeuronState& state,
        float synapticInput,
        float dt,
        float leakConductance = 0.01f,
        float membraneCapacitance = 1.0f,
        float adaptationFactor = 0.01f
    ) {
        bool fired = false;
        
        // Handle refractory period (early exit optimization)
        if (state.refractoryRemaining > 0) {
            --state.refractoryRemaining;
            if (state.refractoryRemaining == 0) {
                state.firingState = FiringState::Resting;
            }
            return false;
        }
        
        // LIF dynamics: V_new = V + dt * ((V_rest - V)/tau + I/C)
        float& V = state.membranePotential;
        const float V_rest = state.restingPotential;
        const float V_reset = state.resetPotential;
        const float threshold = state.threshold;
        const float tau = 20.0f;  // Time constant (ms)
        const float C = membraneCapacitance;  // Membrane capacitance (nF)
        
        // Unrolled computation with pre-calculated terms
        float leakTerm = (V_rest - V) / tau;
        float synapticTerm = synapticInput / C;
        float dtScaled = dt * 1000.0f;  // Convert to ms
        
        // Combined update for better instruction-level parallelism
        V += dtScaled * (leakTerm + synapticTerm);
        
        // Apply adaptation in a single step
        if (state.adaptationVariable > 0.0f) {
            V -= state.adaptationVariable * adaptationFactor;
            state.adaptationVariable *= 0.95f;
        }
        
        // Clamp with reduced branches
        if (V < -100.0f) V = -100.0f;
        else if (V > 50.0f) V = 50.0f;
        
        // Spike detection with early exit optimization
        if (V >= threshold) {
            fired = true;
            state.firingState = FiringState::Active;
            state.lastSpikeTime = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count());
            
            // Reset membrane potential
            V = V_reset;
            
            // Set refractory period
            state.refractoryRemaining = state.refractoryPeriod;
            state.firingState = FiringState::Refractory;
            
            // Update adaptation
            state.adaptationVariable += 1.0f;
        }
        
        return fired;
    }
    
    // Batch processing for multiple neurons
    static void batchStepLIF(
        std::vector<NeuronState*>& states,
        const std::vector<float>& synapticInputs,
        float dt,
        size_t startIdx,
        size_t endIdx
    ) {
        for (size_t i = startIdx; i < endIdx && i < states.size(); ++i) {
            if (!states[i]->refractoryRemaining) {
                stepLIFOptimized(
                    *states[i], 
                    synapticInputs[i], 
                    dt
                );
            }
        }
    }
};

// Optimized random number generation with pre-generation
class OptimizedRandom {
public:
    // Thread-local cache of pre-generated random numbers
    struct ThreadLocalCache {
        std::vector<uint32_t> intCache;
        std::vector<float> floatCache;
        std::vector<double> doubleCache;
        size_t cachePos = 0;
        
        ThreadLocalCache() : intCache(1024), floatCache(1024), doubleCache(512) {
            // Initialize cache
            std::mt19937_64 gen(std::random_device{}());
            std::uniform_int_distribution<uint32_t> intDist(0, std::numeric_limits<uint32_t>::max());
            std::uniform_real_distribution<float> floatDist(0.0f, 1.0f);
            std::uniform_real_distribution<double> doubleDist(0.0, 1.0);
            
            for (auto& val : intCache) val = intDist(gen);
            for (auto& val : floatCache) val = floatDist(gen);
            for (auto& val : doubleCache) val = doubleDist(gen);
        }
    };
    
    static ThreadLocalCache& getLocalCache() {
        thread_local ThreadLocalCache cache;
        return cache;
    }
    
    // Optimized uniformInt with cache
    static uint32_t uniformIntCached(uint32_t min, uint32_t max) {
        auto& cache = getLocalCache();
        if (cache.cachePos >= cache.intCache.size()) {
            // Refill cache
            std::mt19937_64 gen(std::random_device{}());
            std::uniform_int_distribution<uint32_t> dist(0, std::numeric_limits<uint32_t>::max());
            for (size_t i = 0; i < cache.intCache.size(); ++i) {
                cache.intCache[i] = dist(gen);
            }
            cache.cachePos = 0;
        }
        
        uint32_t range = max - min + 1;
        uint32_t val = cache.intCache[cache.cachePos++] % range;
        cache.cachePos = cache.cachePos % cache.intCache.size();
        return val + min;
    }
    
    // Optimized uniformReal with cache
    static float uniformRealCached(float min, float max) {
        auto& cache = getLocalCache();
        if (cache.cachePos >= cache.floatCache.size()) {
            // Refill cache
            std::mt19937_64 gen(std::random_device{}());
            std::uniform_real_distribution<float> dist(0.0f, 1.0f);
            for (size_t i = 0; i < cache.floatCache.size(); ++i) {
                cache.floatCache[i] = dist(gen);
            }
            cache.cachePos = 0;
        }
        
        float val = cache.floatCache[cache.cachePos++] * (max - min) + min;
        cache.cachePos = cache.cachePos % cache.floatCache.size();
        return val;
    }
    
    // Optimized normal distribution using Box-Muller transform
    static float normalBoxMuller(float mean, float stddev) {
        thread_local float cached = 0.0f;
        thread_local bool cachedValid = false;
        
        if (!cachedValid) {
            thread_local std::mt19937_64 gen(std::random_device{}());
            thread_local std::uniform_real_distribution<float> dist(0.0f, 1.0f);
            
            float u1 = dist(gen);
            float u2 = dist(gen);
            
            cached = sqrt(-2.0f * log(u1)) * cos(2.0f * PI * u2);
            cachedValid = true;
        } else {
            cachedValid = false;
        }
        
        return mean + stddev * cached;
    }
    
    static const float PI = 3.14159265358979323846f;
};

// Memory pool for neuron and synapse objects to reduce allocation overhead
class ObjectPool {
public:
    template<typename T, typename... Args>
    T* allocate(Args&&... args) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (freeList_.empty()) {
            // Create new objects in batches
            for (size_t i = 0; i < batchSize_; ++i) {
                objects_.emplace_back(std::make_unique<T>(std::forward<Args>(args)...));
                freeList_.push(objects_.back().get());
            }
        }
        
        T* obj = freeList_.front();
        freeList_.pop();
        return obj;
    }
    
    template<typename T>
    void deallocate(T* obj) {
        std::lock_guard<std::mutex> lock(mutex_);
        freeList_.push(obj);
    }
    
    size_t size() const {
        return objects_.size();
    }
    
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        freeList_ = std::queue<void*>();
        objects_.clear();
    }

private:
    std::queue<void*> freeList_;
    std::vector<std::unique_ptr<void>> objects_;
    mutable std::mutex mutex_;
    static constexpr size_t batchSize_ = 100;
};

// Optimized brain computation with vectorization opportunities
class OptimizedBrainComputation {
public:
    // Vectorized spike propagation
    static void vectorizedSpikePropagation(
        std::vector<NeuronState*>& neurons,
        const std::vector<float>& synapticInputs,
        float dt,
        size_t startIdx,
        size_t endIdx,
        std::vector<bool>& firedFlags
    ) {
        // Process in SIMD-friendly chunks
        const size_t chunkSize = 16; // Process 16 neurons at a time
        
        for (size_t i = startIdx; i < std::min(endIdx, neurons.size()); i += chunkSize) {
            size_t chunkEnd = std::min(i + chunkSize, neurons.size());
            
            // Unroll loop for better instruction-level parallelism
            for (size_t j = i; j < chunkEnd; ++j) {
                if (neurons[j]->refractoryRemaining == 0) {
                    firedFlags[j] = OptimizedNeuron::stepLIFOptimized(
                        *neurons[j],
                        synapticInputs[j],
                        dt
                    );
                } else {
                    firedFlags[j] = false;
                    if (neurons[j]->refractoryRemaining > 0) {
                        --neurons[j]->refractoryRemaining;
                    }
                }
            }
        }
    }
    
    // Optimized synaptic weight update using matrix operations
    static void optimizedSynapticUpdate(
        const std::vector<std::vector<float>>& weights,
        const std::vector<float>& preSynaptic,
        const std::vector<float>& postSynaptic,
        std::vector<float>& weightChanges,
        float learningRate,
        float stdpLtpWeight,
        float stdpLtdWeight
    ) {
        size_t n = weights.size();
        if (n == 0) return;
        
        weightChanges.resize(n, 0.0f);
        
        // Optimized STDP update
        for (size_t i = 0; i < n; ++i) {
            for (size_t j = 0; j < weights[i].size(); ++j) {
                float deltaW = 0.0f;
                
                // STDP computation
                if (preSynaptic[i] > 0.5f && postSynaptic[j] > 0.5f) {
                    // LTP (Long Term Potentiation)
                    deltaW += stdpLtpWeight;
                } else if (preSynaptic[i] > 0.5f && postSynaptic[j] <= 0.5f) {
                    // LTD (Long Term Depression)
                    deltaW -= stdpLtdWeight;
                }
                
                weightChanges[j] += deltaW * learningRate;
            }
        }
        
        // Apply weight changes with bounds checking
        for (size_t i = 0; i < n; ++i) {
            weights[i][i] = std::clamp(weights[i][i] + weightChanges[i], 0.0f, 2.0f);
        }
    }
};

} // namespace perf

} // namespace nlm
