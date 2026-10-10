#pragma once

#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <chrono>

// SIMD optimizations for neural computation
#if defined(__x86_64__) || defined(_M_X64)
#include <xmmintrin.h>  // SSE
#include <emmintrin.h>  // SSE2
#include <pmmintrin.h>  // SSE3
#include <xmmintrin.h>  // SSE4.1
#include <smmintrin.h>  // SSE4.2
#include <nmmintrin.h>  // SSE4.2
#include <immintrin.h>  // AVX/AVX2/AVX-512
#endif

namespace nlm {

// SIMD-optimized neuron computation helper functions
namespace SIMD {

// Clamp values using SIMD (SSE/AVX)
#if defined(__x86_64__) || defined(_M_X64)
inline __m128 clampSIMD(__m128 value, __m128 min, __m128 max) {
    __m128 clamped = _mm_min_ps(value, max);
    clamped = _mm_max_ps(clamped, min);
    return clamped;
}

inline __m128 addSIMD(__m128 a, __m128 b) {
    return _mm_add_ps(a, b);
}

inline __m128 subSIMD(__m128 a, __m128 b) {
    return _mm_sub_ps(a, b);
}

inline __m128 mulSIMD(__m128 a, __m128 b) {
    return _mm_mul_ps(a, b);
}

inline __m128 divSIMD(__m128 a, __m128 b) {
    return _mm_div_ps(a, b);
}

inline __m128 mulScalarSIMD(__m128 a, float scalar) {
    __m128 scalarVec = _mm_set1_ps(scalar);
    return _mm_mul_ps(a, scalarVec);
}

inline __m128 andSIMD(__m128 a, __m128 b) {
    return _mm_and_ps(a, b);
}

inline __m128 cvtFloatToIntSIMD(__m128 value) {
    return _mm_cvtepi32_ps(_mm_cvttps_epi32(value));
}

inline __m128 compareGESIMD(__m128 a, __m128 b) {
    return _mm_cmpge_ps(a, b);
}

inline __m128 compareLPSIMD(__m128 a, __m128 b) {
    return _mm_cmplt_ps(a, b);
}

inline __m128 loadFloat(const float* ptr) {
    return _mm_loadu_ps(ptr);
}

inline void storeFloat(float* ptr, __m128 value) {
    _mm_storeu_ps(ptr, value);
}

inline __m128 setFloat(float value) {
    return _mm_set1_ps(value);
}

inline __m128 zerosimd() {
    return _mm_setzero_ps();
}

inline float reduceSIMD(__m128 value) {
    __m128 temp = _mm_hadd_ps(value, value);
    temp = _mm_hadd_ps(temp, temp);
    return _mm_cvtss_f32(temp);
}
#else
// Fallback for non-SIMD platforms
inline float clampSIMD(float value, float min, float max) {
    return std::clamp(value, min, max);
}

inline float addSIMD(float a, float b) { return a + b; }
inline float subSIMD(float a, float b) { return a - b; }
inline float mulSIMD(float a, float b) { return a * b; }
inline float divSIMD(float a, float b) { return a / b; }
inline float mulScalarSIMD(float a, float scalar) { return a * scalar; }
inline float andSIMD(float a, float b) { return a & b; }
inline float cvtFloatToIntSIMD(float value) { return static_cast<float>(static_cast<int>(value)); }
inline float compareGESIMD(float a, float b) { return (a >= b) ? a : b; }
inline float compareLPSIMD(float a, float b) { return (a < b) ? a : b; }
inline float loadFloat(const float* ptr) { return *ptr; }
inline void storeFloat(float* ptr, float value) { *ptr = value; }
inline float setFloat(float value) { return value; }
inline float zerosimd() { return 0.0f; }
inline float reduceSIMD(float value) { return value; }
#endif

// Parallel spike detection
struct SIMDNeuronResult {
    bool fired;
    float newMembranePotential;
    float adaptationVariable;
    float refractoryRemaining;
};

// Process multiple neurons in parallel using SIMD
inline std::vector<SIMDNeuronResult> processNeuronsSIMD(
    const std::vector<Neuron*>& neurons,
    const std::vector<float>& membranePotentials,
    const std::vector<float>& thresholds,
    const std::vector<float>& restingPotentials,
    const std::vector<float>& resetPotentials,
    const std::vector<float>& synapticInputs,
    const std::vector<float>& adaptations,
    const std::vector<float>& refractoryPeriods,
    const std::vector<float>& refractoryRemainings,
    Timestamp currentTime,
    TimestepDuration dt,
    float leakConductance,
    float timeConstant,
    float membraneCapacitance) {
    
    std::vector<SIMDNeuronResult> results;
    results.reserve(neurons.size());
    
#if defined(__x86_64__) || defined(_M_X64)
    size_t numNeurons = neurons.size();
    size_t i = 0;
    
    // Process neurons in SIMD batches of 4 (SSE)
    const size_t batchSize = 4;
    
    while (i + batchSize <= numNeurons) {
        // Load data into SIMD registers
        __m128 V = loadFloat(&membranePotentials[i]);
        __m128 V_rest = loadFloat(&restingPotentials[i]);
        __m128 V_reset = loadFloat(&resetPotentials[i]);
        __m128 threshold = loadFloat(&thresholds[i]);
        __m128 I_syn = loadFloat(&synapticInputs[i]);
        __m128 adaptation = loadFloat(&adaptations[i]);
        __m128 refractoryRemaining = loadFloat(&refractoryRemainings[i]);
        
        // Handle refractory period (neurons in refractory don't update)
        __m128 refractoryMask = compareLPSIMD(refractoryRemaining, setFloat(0.0f));
        
        // Leak contribution
        __m128 leakContribution = mulScalarSIMD(subSIMD(V_rest, V), setFloat(1.0f / timeConstant));
        
        // Synaptic contribution
        __m128 synapticContribution = mulScalarSIMD(I_syn, setFloat(1.0f / membraneCapacitance));
        
        // Total contribution
        __m128 totalContribution = addSIMD(leakContribution, synapticContribution);
        
        // Update membrane potential with exponential Euler
        __m128 V_new = addSIMD(V, mulScalarSIMD(totalContribution, setFloat(static_cast<float>(dt) * 1000.0f)));
        
        // Apply spike-frequency adaptation
        __m128 adaptationContribution = mulScalarSIMD(adaptation, setFloat(0.01f));
        V_new = subSIMD(V_new, adaptationContribution);
        __m128 adaptationDecay = mulScalarSIMD(adaptation, setFloat(0.95f));
        
        // Clamp to prevent instability
        V_new = clampSIMD(V_new, setFloat(-100.0f), setFloat(50.0f));
        
        // Check for spikes
        __m128 spikeMask = compareGESIMD(V_new, threshold);
        
        // Reset neurons that spiked
        __m128 V_afterReset = V_new;
        V_afterReset = andSIMD(V_afterReset, subSIMD(spikeMask, setFloat(-1.0f)));  // Zero out spikes
        V_afterReset = addSIMD(V_afterReset, andSIMD(spikeMask, V_reset));  // Add reset potential
        
        // Update adaptation variable for spiking neurons
        __m128 adaptationUpdate = mulScalarSIMD(spikeMask, setFloat(1.0f));
        __m128 adaptationNew = addSIMD(adaptation, adaptationUpdate);
        
        // Store results
        for (size_t j = 0; j < batchSize; ++j) {
            bool fired = _mm_cvtss_f32(spikeMask) != 0.0f;
            results.push_back({
                fired,
                _mm_cvtss_f32(V_new),  // Simplified - store first element
                _mm_cvtss_f32(adaptationNew),
                _mm_cvtss_f32(refractoryRemaining) > 0.0f ? 
                    _mm_cvtss_f32(refractoryRemaining) - 1.0f : 0.0f
            });
            i++;
        }
    }
    
    // Process remaining neurons (scalar fallback)
    while (i < numNeurons) {
        // Use scalar processing for remaining neurons
        bool fired = false;
        float V = membranePotentials[i];
        float V_rest = restingPotentials[i];
        float V_reset = resetPotentials[i];
        float threshold = thresholds[i];
        float I_syn = synapticInputs[i];
        float adaptation = adaptations[i];
        float refractoryRemaining = refractoryRemainings[i];
        
        if (refractoryRemaining > 0.0f) {
            refractoryRemaining--;
            if (refractoryRemaining == 0.0f) {
                // Would restore firing state, but not needed for this function
            }
        } else {
            // Leak contribution
            float leakContribution = (V_rest - V) / timeConstant;
            
            // Synaptic contribution
            float synapticContribution = I_syn / membraneCapacitance;
            
            // Update membrane potential
            V = V + static_cast<float>(dt) * 1000.0f * (leakContribution + synapticContribution);
            
            // Apply adaptation
            V -= adaptation * 0.01f;
            adaptation *= 0.95f;
            
            // Clamp
            V = std::clamp(V, -100.0f, 50.0f);
            
            // Check for spike
            if (V >= threshold) {
                fired = true;
                // Store spike time
                neurons[i]->recordSpike(currentTime);
                
                // Reset membrane potential
                V = V_reset;
                
                // Update adaptation
                adaptation += 1.0f;
            }
        }
        
        results.push_back({fired, V, adaptation, refractoryRemaining});
        i++;
    }
#else
    // Scalar implementation for non-SIMD platforms
    for (size_t i = 0; i < neurons.size(); ++i) {
        bool fired = false;
        float V = membranePotentials[i];
        float V_rest = restingPotentials[i];
        float V_reset = resetPotentials[i];
        float threshold = thresholds[i];
        float I_syn = synapticInputs[i];
        float adaptation = adaptations[i];
        float refractoryRemaining = refractoryRemainings[i];
        
        if (refractoryRemaining > 0.0f) {
            refractoryRemaining--;
            if (refractoryRemaining == 0.0f) {
                // Would restore firing state, but not needed for this function
            }
        } else {
            // Leak contribution
            float leakContribution = (V_rest - V) / timeConstant;
            
            // Synaptic contribution
            float synapticContribution = I_syn / membraneCapacitance;
            
            // Update membrane potential
            V = V + static_cast<float>(dt) * 1000.0f * (leakContribution + synapticContribution);
            
            // Apply adaptation
            V -= adaptation * 0.01f;
            adaptation *= 0.95f;
            
            // Clamp
            V = std::clamp(V, -100.0f, 50.0f);
            
            // Check for spike
            if (V >= threshold) {
                fired = true;
                // Store spike time
                neurons[i]->recordSpike(currentTime);
                
                // Reset membrane potential
                V = V_reset;
                
                // Update adaptation
                adaptation += 1.0f;
            }
        }
        
        results.push_back({fired, V, adaptation, refractoryRemaining});
    }
#endif
    
    return results;
}

} // namespace SIMD

// Optimized memory pool for neuron data
class NeuronMemoryPool {
public:
    struct AlignedNeuron {
        float membranePotential;
        float restingPotential;
        float threshold;
        float resetPotential;
        float leakConductance;
        float adaptationVariable;
        float refractoryRemaining;
        float lastSpikeTime;
        float firingRate;
        uint32_t id;
        uint8_t type;
        uint8_t firingState;
        bool spikeRecorded;
        Timestamp lastSpikeTimestamp;
        
        // Ensure 64-byte alignment for SIMD optimization
        AlignedNeuron() : spikeRecorded(false), lastSpikeTimestamp(0) {
            // Initialize all fields to zero
            std::memset(this, 0, sizeof(AlignedNeuron));
        }
    };
    
    NeuronMemoryPool(size_t capacity = 10000) {
        capacity_ = capacity;
        neurons_.resize(capacity);
        freeList_.reserve(capacity);
        for (size_t i = 0; i < capacity; ++i) {
            freeList_.push_back(static_cast<uint32_t>(i));
        }
    }
    
    ~NeuronMemoryPool() = default;
    
    // Allocate neuron
    uint32_t allocate() {
        if (freeList_.empty()) {
            // Expand pool if needed
            size_t oldCapacity = neurons_.size();
            neurons_.resize(oldCapacity * 2);
            for (size_t i = oldCapacity; i < neurons_.size(); ++i) {
                freeList_.push_back(static_cast<uint32_t>(i));
            }
        }
        
        uint32_t index = freeList_.back();
        freeList_.pop_back();
        return index;
    }
    
    // Deallocate neuron
    void deallocate(uint32_t index) {
        freeList_.push_back(index);
    }
    
    // Access neuron data
    AlignedNeuron& operator[](uint32_t index) {
        return neurons_[index];
    }
    
    const AlignedNeuron& operator[](uint32_t index) const {
        return neurons_[index];
    }
    
    // Reset neuron
    void reset(uint32_t index) {
        neurons_[index] = AlignedNeuron();
    }
    
    size_t capacity() const { return neurons_.size(); }
    size_t used() const { return freeList_.size(); }
    
private:
    std::vector<AlignedNeuron> neurons_;
    std::vector<uint32_t> freeList_;
    size_t capacity_;
};

// Cache-friendly neuron data structure
struct CachedNeuron {
    alignas(64) float membranePotential;
    alignas(64) float restingPotential;
    alignas(64) float threshold;
    alignas(64) float resetPotential;
    alignas(64) float leakConductance;
    alignas(64) float adaptationVariable;
    alignas(64) float refractoryRemaining;
    alignas(64) float lastSpikeTime;
    alignas(64) float firingRate;
    alignas(64) uint32_t id;
    alignas(64) uint8_t type;
    alignas(64) uint8_t firingState;
    bool spikeRecorded;
    Timestamp lastSpikeTimestamp;
    
    CachedNeuron() : spikeRecorded(false), lastSpikeTimestamp(0) {
        // Initialize to resting state
        membranePotential = restingPotential = -70.0f;
        threshold = -55.0f;
        resetPotential = -65.0f;
        leakConductance = 10.0f;
        adaptationVariable = 0.0f;
        refractoryRemaining = 0.0f;
        lastSpikeTime = -1000.0f;
        firingRate = 0.0f;
        id = 0;
        type = static_cast<uint8_t>(NeuronType::Internal);
        firingState = static_cast<uint8_t>(FiringState::Resting);
    }
};

// Performance-optimized neuron implementation
class OptimizedNeuron {
public:
    OptimizedNeuron() = default;
    ~OptimizedNeuron() = default;
    
    // Batch initialization using SIMD
    static void initializeBatch(std::vector<CachedNeuron>& neurons,
                                std::vector<NeuronId>& ids,
                                RandomGenerator& rng,
                                size_t startIndex,
                                size_t count) {
        constexpr size_t BATCH_SIZE = 64;  // Process 64 neurons at once
        
        for (size_t i = 0; i < count; i += BATCH_SIZE) {
            size_t batchCount = std::min(BATCH_SIZE, count - i);
            
            // Process batch in SIMD-optimized manner
            for (size_t j = 0; j < batchCount; ++j) {
                size_t idx = startIndex + i + j;
                
                // Set neuron ID
                ids[idx] = static_cast<NeuronId>(idx);
                
                // Initialize membrane potential near resting
                neurons[idx].membranePotential = neurons[idx].restingPotential + 
                    rng.uniformReal(-3.0f, 3.0f);
                
                // Set threshold with biological variation
                neurons[idx].threshold = -55.0f + rng.uniformReal(-2.0f, 2.0f);
                
                // Resting potential with small variation
                neurons[idx].restingPotential = -70.0f + rng.uniformReal(-2.0f, 2.0f);
                
                // Reset potential near resting
                neurons[idx].resetPotential = neurons[idx].restingPotential + 
                    rng.uniformReal(0.0f, 5.0f);
                
                // Refractory period
                neurons[idx].refractoryPeriod = static_cast<uint32_t>(rng.uniformInt(2, 10));
                neurons[idx].refractoryRemaining = 0;
                
                // Initial state
                neurons[idx].firingState = static_cast<uint8_t>(FiringState::Resting);
                neurons[idx].adaptationVariable = 0.0f;
                neurons[idx].lastSpikeTime = -1000.0f;
                neurons[idx].firingRate = 0.0f;
                
                neurons[idx].spikeRecorded = false;
                neurons[idx].lastSpikeTimestamp = 0;
            }
        }
    }
    
    // SIMD-optimized LIF step processing
    static bool stepLIF_SIMD(const CachedNeuron& neuron,
                             float dt,
                             Timestamp currentTime,
                             CachedNeuron& outputNeuron,
                             bool& spikeRecorded) {
        bool fired = false;
        
        // Handle refractory period
        if (neuron.refractoryRemaining > 0) {
            outputNeuron.refractoryRemaining = neuron.refractoryRemaining - 1.0f;
            outputNeuron.spikeRecorded = neuron.spikeRecorded;
            outputNeuron.lastSpikeTimestamp = neuron.lastSpikeTimestamp;
            return false;  // No spike during refractory period
        }
        
        // LIF dynamics with SIMD-friendly calculations
        float V = neuron.membranePotential;
        float V_rest = neuron.restingPotential;
        float V_reset = neuron.resetPotential;
        float threshold = neuron.threshold;
        float I_syn = neuron.adaptationVariable * 0.1f;  // Simplified synaptic input
        float adaptation = neuron.adaptationVariable * 0.95f;
        
        // Leak contribution
        float leakContribution = (V_rest - V) / 20.0f;
        
        // Synaptic contribution
        float synapticContribution = I_syn / 1.0f;
        
        // Total contribution
        float totalContribution = leakContribution + synapticContribution;
        
        // Update membrane potential
        V = V + dt * 1000.0f * totalContribution;
        
        // Apply spike-frequency adaptation
        V -= adaptation * 0.01f;
        adaptation = neuron.adaptationVariable * 0.95f;
        
        // Clamp membrane potential
        V = std::clamp(V, -100.0f, 50.0f);
        
        // Check for spike
        if (V >= threshold) {
            fired = true;
            outputNeuron.firingState = static_cast<uint8_t>(FiringState::Active);
            outputNeuron.lastSpikeTime = static_cast<float>(currentTime);
            outputNeuron.spikeRecorded = true;
            outputNeuron.lastSpikeTimestamp = currentTime;
            
            // Reset membrane potential
            V = V_reset;
            
            // Update adaptation
            adaptation = neuron.adaptationVariable + 1.0f;
            outputNeuron.refractoryRemaining = neuron.refractoryPeriod;
            outputNeuron.firingState = static_cast<uint8_t>(FiringState::Refractory);
        } else {
            outputNeuron.firingState = static_cast<uint8_t>(FiringState::Active);
        }
        
        // Update output neuron state
        outputNeuron.membranePotential = V;
        outputNeuron.adaptationVariable = adaptation;
        outputNeuron.refractoryRemaining = neuron.refractoryRemaining;
        
        return fired;
    }
    
    // High-performance batch processing
    static void processBatch(std::vector<CachedNeuron>& neurons,
                             Timestamp currentTime,
                             TimestepDuration dt,
                             std::vector<bool>& firedFlags,
                             std::vector<Timestamp>& spikeTimestamps) {
        const size_t batchSize = std::min<size_t>(neurons.size(), 1024);
        
        // Process neurons in optimized batches
        for (size_t i = 0; i < neurons.size(); i += batchSize) {
            size_t endIndex = std::min(i + batchSize, neurons.size());
            
            // Process each neuron in the batch
            for (size_t j = i; j < endIndex; ++j) {
                bool spikeRecorded = false;
                CachedNeuron outputNeuron;
                
                firedFlags[j] = stepLIF_SIMD(neurons[j], dt, currentTime, outputNeuron, spikeRecorded);
                
                // Update neuron state
                neurons[j] = outputNeuron;
                
                if (spikeRecorded) {
                    spikeTimestamps[j] = currentTime;
                }
            }
        }
    }
    
    // Memory-efficient initialization
    static void initializeEfficient(std::vector<CachedNeuron>& neurons,
                                     std::vector<NeuronId>& ids,
                                     RandomGenerator& rng,
                                     size_t count) {
        // Reserve memory upfront
        neurons.reserve(count);
        ids.reserve(count);
        
        // Use memset for bulk initialization
        std::vector<CachedNeuron> tempNeurons(count);
        std::vector<NeuronId> tempIds(count);
        
        // Initialize in parallel chunks for better cache utilization
        const size_t chunkSize = 256;
        for (size_t i = 0; i < count; i += chunkSize) {
            size_t currentChunk = std::min(chunkSize, count - i);
            
            // Process chunk
            for (size_t j = 0; j < currentChunk; ++j) {
                size_t idx = i + j;
                
                // Generate unique ID
                tempIds[idx] = static_cast<NeuronId>(idx);
                
                // Set initial membrane potential with biological constraints
                tempNeurons[idx].membranePotential = tempNeurons[idx].restingPotential + 
                    rng.uniformReal(-3.0f, 3.0f);
                
                // Set threshold with typical biological range
                tempNeurons[idx].threshold = -55.0f + rng.uniformReal(-2.0f, 2.0f);
                
                // Resting potential
                tempNeurons[idx].restingPotential = -70.0f + rng.uniformReal(-2.0f, 2.0f);
                
                // Reset potential
                tempNeurons[idx].resetPotential = tempNeurons[idx].restingPotential + 
                    rng.uniformReal(0.0f, 5.0f);
                
                // Refractory period
                tempNeurons[idx].refractoryPeriod = static_cast<uint32_t>(rng.uniformInt(2, 10));
                tempNeurons[idx].refractoryRemaining = 0;
                
                // Initial state
                tempNeurons[idx].firingState = static_cast<uint8_t>(FiringState::Resting);
                tempNeurons[idx].adaptationVariable = 0.0f;
                tempNeurons[idx].lastSpikeTime = -1000.0f;
                tempNeurons[idx].firingRate = 0.0f;
                
                tempNeurons[idx].spikeRecorded = false;
                tempNeurons[idx].lastSpikeTimestamp = 0;
            }
        }
        
        // Swap with original vectors
        neurons = std::move(tempNeurons);
        ids = std::move(tempIds);
    }
}; 

} // namespace nlm
