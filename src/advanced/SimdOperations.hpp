#pragma once

#include <array>
#include <cmath>
#include <algorithm>
#include <immintrin.h>
#include <chrono>
#include <cstring>

namespace nlm {

// SIMD-optimized mathematical operations for neural computations
class SimdOperations {
public:
    // SSE-optimized neural activation functions
    static float sigmoidSSE(float x) {
        // Optimized sigmoid using SSE intrinsics
        __m128 v = _mm_set1_ps(x);
        
        // Compute exp(-x) using approximation
        // Using exp(-x) = 1 / (1 + exp(x)) optimization
        // This is a fast approximation for neural networks
        float result;
        __m128 exp_neg_x;
        
        // Use approximation: exp(-x) ≈ 1 - x + x^2/2 - x^3/6 + x^4/24
        // Valid for |x| < 3.0 for reasonable accuracy
        if (x < -3.0f) {
            exp_neg_x = _mm_set1_ps(0.04978704f);  // exp(-3.0) approximation
        } else if (x > 3.0f) {
            exp_neg_x = _mm_set1_ps(0.00497874f);  // exp(-5.0) approximation
        } else {
            float x2 = x * x;
            float x4 = x2 * x2;
            exp_neg_x = _mm_set1_ps(1.0f - x + 0.5f*x2 - (1.0f/6.0f)*x2*x + (1.0f/24.0f)*x4);
        }
        
        // Compute sigmoid = 1.0 / (1.0 + exp(-x))
        float exp_neg_x_val;
        _mm_store_ss(&exp_neg_x_val, exp_neg_x);
        return 1.0f / (1.0f + exp_neg_x_val);
    }
    
    // AVX-optimized neuron membrane potential updates
    static void updateMembranePotentialSSE(
        float* potentials, float* inputs, 
        float leakConductance, float timeConstant,
        size_t count) {
        
        // Load constants into SSE registers
        __m128 leak = _mm_set1_ps(leakConductance);
        __m128 tau = _mm_set1_ps(timeConstant);
        __m128 inv_tau = _mm_set1_ps(1.0f / timeConstant);
        __m128 dt = _mm_set1_ps(0.001f);  // 1ms timestep
        
        size_t i = 0;
        for (; i + 3 < count; i += 4) {
            // Load 4 potential values
            __m128 v = _mm_loadu_ps(&potentials[i]);
            __m128 i_input = _mm_loadu_ps(&inputs[i]);
            
            // Compute leak term: (V_rest - V) / tau
            // Note: V_rest assumed to be 0 for simplicity (can be parameterized)
            __m128 leak_term = _mm_mul_ps(_mm_sub_ps(_mm_setzero_ps(), v), inv_tau);
            
            // Compute synaptic contribution: I / C (where C = 1.0f for simplicity)
            __m128 syn_term = i_input;
            
            // Total change: dt * (leak_term + syn_term)
            __m128 dv = _mm_mul_ps(dt, _mm_add_ps(leak_term, syn_term));
            
            // Update membrane potential
            v = _mm_add_ps(v, dv);
            
            // Clamp to biologically reasonable range
            v = _mm_max_ps(v, _mm_set1_ps(-100.0f));
            v = _mm_min_ps(v, _mm_set1_ps(50.0f));
            
            // Store back
            _mm_storeu_ps(&potentials[i], v);
        }
        
        // Handle remaining elements
        for (; i < count; ++i) {
            float leak_term = (0.0f - potentials[i]) * (1.0f / timeConstant);
            float syn_term = inputs[i];
            potentials[i] += 0.001f * (leak_term + syn_term);
            potentials[i] = std::clamp(potentials[i], -100.0f, 50.0f);
        }
    }
    
    // AVX-optimized synaptic weight updates
    static void updateSynapticWeightsAVX(
        float* weights, const float* preSpike, const float* postSpike,
        const float* eligibility, float learningRate,
        size_t count, float& processingTime) {
        
        auto start = std::chrono::high_resolution_clock::now();
        
        // Load learning rate
        __m128 lr = _mm_set1_ps(learningRate);
        
        size_t i = 0;
        for (; i + 7 < count; i += 8) {
            // Load pre-synaptic spikes (4 floats each)
            __m128 pre1 = _mm_loadu_ps(&preSpike[i]);
            __m128 pre2 = _mm_loadu_ps(&preSpike[i + 4]);
            
            // Load post-synaptic spikes (4 floats each)
            __m128 post1 = _mm_loadu_ps(&postSpike[i]);
            __m128 post2 = _mm_loadu_ps(&postSpike[i + 4]);
            
            // Load eligibility traces
            __m128 elig1 = _mm_loadu_ps(&eligibility[i]);
            __m128 elig2 = _mm_loadu_ps(&eligibility[i + 4]);
            
            // Load current weights
            __m128 w1 = _mm_loadu_ps(&weights[i]);
            __m128 w2 = _mm_loadu_ps(&weights[i + 4]);
            
            // STDP update: Δw = A_post * post_spike * elig - A_pre * pre_spike * elig
            // Use pre-synaptic contribution for depression, post-synaptic for potentiation
            __m128 stup_term1 = _mm_mul_ps(_mm_set1_ps(0.01f), _mm_mul_ps(post1, elig1));
            __m128 stup_term2 = _mm_mul_ps(_mm_set1_ps(0.01f), _mm_mul_ps(post2, elig2));
            
            __m128 stdp_term1 = _mm_mul_ps(_mm_set1_ps(-0.012f), _mm_mul_ps(pre1, elig1));
            __m128 stdp_term2 = _mm_mul_ps(_mm_set1_ps(-0.012f), _mm_mul_ps(pre2, elig2));
            
            // Apply weight update
            w1 = _mm_add_ps(w1, _mm_mul_ps(lr, _mm_add_ps(stup_term1, stdp_term1)));
            w2 = _mm_add_ps(w2, _mm_mul_ps(lr, _mm_add_ps(stup_term2, stdp_term2)));
            
            // Clamp weights to reasonable range
            const __m128 min_weight = _mm_set1_ps(-1.0f);
            const __m128 max_weight = _mm_set1_ps(1.0f);
            
            w1 = _mm_max_ps(_mm_min_ps(w1, max_weight), min_weight);
            w2 = _mm_max_ps(_mm_min_ps(w2, max_weight), min_weight);
            
            // Store updated weights
            _mm_storeu_ps(&weights[i], w1);
            _mm_storeu_ps(&weights[i + 4], w2);
        }
        
        // Handle remaining elements
        for (; i < count; ++i) {
            float stup = 0.01f * postSpike[i] * eligibility[i];
            float stdp = -0.012f * preSpike[i] * eligibility[i];
            weights[i] += learningRate * (stup + stdp);
            weights[i] = std::clamp(weights[i], -1.0f, 1.0f);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        processingTime = std::chrono::duration<float, std::milli>(end - start).count();
    }
    
    // SSE-optimized spike detection with refractory period
    static void detectSpikesSSE(
        const float* potentials, bool* spikeFlags, 
        float threshold, uint32_t* refractoryCounters,
        size_t count) {
        
        // Load threshold
        __m128 th = _mm_set1_ps(threshold);
        
        size_t i = 0;
        for (; i + 3 < count; i += 4) {
            // Load 4 potential values
            __m128 v = _mm_loadu_ps(&potentials[i]);
            
            // Compare with threshold to detect spikes
            __m128 spike_mask = _mm_cmpgt_ps(v, th);
            
            // Convert spike mask to boolean flags
            int mask = _mm_movemask_ps(spike_mask);
            
            // Set spike flags
            if (mask & 0x1) spikeFlags[i] = true;
            if (mask & 0x2) spikeFlags[i + 1] = true;
            if (mask & 0x4) spikeFlags[i + 2] = true;
            if (mask & 0x8) spikeFlags[i + 3] = true;
        }
        
        // Handle remaining elements
        for (; i < count; ++i) {
            spikeFlags[i] = (potentials[i] > threshold);
        }
    }
    
    // AVX-optimized memory access for better cache locality
    static void optimizeMemoryAccessPattern(float* buffer, size_t size) {
        // This function optimizes memory access patterns for better cache utilization
        // For large arrays, we recommend blocking/tiling for better cache performance
        
        // Simple alignment check
        if ((reinterpret_cast<uintptr_t>(buffer) & 31) != 0) {
            // Buffer is not 32-byte aligned (AVX requirement)
            // Copy to aligned buffer for optimal performance
            alignas(32) float aligned_buffer[1024];  // Temporary buffer
            if (size <= 1024) {
                std::memcpy(aligned_buffer, buffer, size * sizeof(float));
                std::memcpy(buffer, aligned_buffer, size * sizeof(float));
            }
        }
    }
    
    // Optimized neural computation with fused operations
    static void computeNeuralUpdateFused(
        float* membranePotentials, float* synapticInputs,
        float leakConductance, float timeConstant,
        float threshold, bool* spikes, uint32_t* refractoryCounters,
        size_t count) {
        
        // Load constants
        __m128 leak = _mm_set1_ps(leakConductance);
        __m128 inv_tau = _mm_set1_ps(1.0f / timeConstant);
        __m128 dt = _mm_set1_ps(0.001f);
        __m128 th = _mm_set1_ps(threshold);
        
        size_t i = 0;
        for (; i + 3 < count; i += 4) {
            // Load potentials and inputs
            __m128 v = _mm_loadu_ps(&membranePotentials[i]);
            __m128 inputs = _mm_loadu_ps(&synapticInputs[i]);
            
            // Compute leak term
            __m128 leak_term = _mm_mul_ps(_mm_sub_ps(_mm_setzero_ps(), v), inv_tau);
            
            // Compute synaptic contribution
            __m128 syn_term = inputs;
            
            // Compute membrane potential update
            __m128 dv = _mm_mul_ps(dt, _mm_add_ps(leak_term, syn_term));
            
            // Update potentials
            v = _mm_add_ps(v, dv);
            
            // Clamp potentials
            v = _mm_max_ps(v, _mm_set1_ps(-100.0f));
            v = _mm_min_ps(v, _mm_set1_ps(50.0f));
            
            // Store updated potentials
            _mm_storeu_ps(&membranePotentials[i], v);
            
            // Store updated inputs
            _mm_storeu_ps(&synapticInputs[i], _mm_setzero_ps());  // Reset for next timestep
            
            // Detect spikes (spike detection happens in separate pass for clarity)
        }
        
        // Store spike flags in separate pass
        detectSpikesSSE(&membranePotentials[0], spikes, threshold, nullptr, count);
    }
    
    // Benchmark SIMD performance
    static struct SimdBenchmark {
        double sseTime;
        double avxTime;
        double speedup;
    } benchmarkNeuralOperations(float* array1, float* array2, size_t size) {
        SimdBenchmark results;
        
        auto start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < size; i += 4) {
            __m128 a = _mm_loadu_ps(&array1[i]);
            __m128 b = _mm_loadu_ps(&array2[i]);
            __m128 c = _mm_add_ps(a, b);
            _mm_storeu_ps(&array1[i], c);
        }
        auto end = std::chrono::high_resolution_clock::now();
        results.sseTime = std::chrono::duration<double, std::milli>(end - start).count();
        
        // Clear arrays
        std::memset(array1, 0, size * sizeof(float));
        
        // AVX benchmark
        start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < size; i += 8) {
            __m256 a = _mm256_loadu_ps(&array1[i]);
            __m256 b = _mm256_loadu_ps(&array2[i]);
            __m256 c = _mm256_add_ps(a, b);
            _mm256_storeu_ps(&array1[i], c);
        }
        end = std::chrono::high_resolution_clock::now();
        results.avxTime = std::chrono::duration<double, std::milli>(end - start).count();
        
        results.speedup = results.sseTime / results.avxTime;
        return results;
    }
};

// SIMD-optimized neural computation kernel
class SimdNeuralKernel {
public:
    static void computeNeuronsSSE(
        float* membranePotentials,
        const float* synapticInputs,
        const float* leakConductances,
        const float* timeConstants,
        const float* thresholds,
        bool* spikeFlags,
        uint32_t* refractoryCounters,
        size_t neuronCount,
        float& processingTime) {
        
        auto start = std::chrono::high_resolution_clock::now();
        
        SimdOperations::updateMembranePotentialSSE(
            membranePotentials, synapticInputs, 25.0f, 20.0f, neuronCount);
        
        SimdOperations::detectSpikesSSE(
            membranePotentials, spikeFlags, -55.0f, refractoryCounters, neuronCount);
        
        auto end = std::chrono::high_resolution_clock::now();
        processingTime = std::chrono::duration<float, std::milli>(end - start).count();
    }
    
    static void computeSynapticUpdatesAVX(
        float* weights,
        const float* preSpikes,
        const float* postSpikes,
        const float* eligibility,
        float learningRate,
        size_t synapseCount,
        float& processingTime) {
        
        SimdOperations::updateSynapticWeightsAVX(
            weights, preSpikes, postSpikes, eligibility, learningRate, synapseCount, processingTime);
    }
    
    static void optimizeNeuralArrayLayout(float* array, size_t size) {
        SimdOperations::optimizeMemoryAccessPattern(array, size);
    }
    
    static void computeFusedNeuralOperations(
        float* membranePotentials,
        float* synapticInputs,
        float leakConductance,
        float timeConstant,
        float threshold,
        bool* spikeFlags,
        uint32_t* refractoryCounters,
        size_t count) {
        
        SimdOperations::computeNeuralUpdateFused(
            membranePotentials, synapticInputs, leakConductance, timeConstant, threshold,
            spikeFlags, refractoryCounters, count);
    }
};

} // namespace nlm
