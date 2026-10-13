#include "SimdOperations.hpp"
#include <cstring>
#include <chrono>

namespace nlm {

void SimdOperations::updateMembranePotentialSSE(
    float* potentials, float* inputs, 
    float leakConductance, float timeConstant,
    size_t count) {
    
    __m128 leak = _mm_set1_ps(leakConductance);
    __m128 tau = _mm_set1_ps(timeConstant);
    __m128 inv_tau = _mm_set1_ps(1.0f / timeConstant);
    __m128 dt = _mm_set1_ps(0.001f);
    
    size_t i = 0;
    for (; i + 3 < count; i += 4) {
        __m128 v = _mm_loadu_ps(&potentials[i]);
        __m128 i_input = _mm_loadu_ps(&inputs[i]);
        
        __m128 leak_term = _mm_mul_ps(_mm_sub_ps(_mm_setzero_ps(), v), inv_tau);
        __m128 syn_term = i_input;
        __m128 dv = _mm_mul_ps(dt, _mm_add_ps(leak_term, syn_term));
        
        v = _mm_add_ps(v, dv);
        v = _mm_max_ps(v, _mm_set1_ps(-100.0f));
        v = _mm_min_ps(v, _mm_set1_ps(50.0f));
        
        _mm_storeu_ps(&potentials[i], v);
    }
    
    for (; i < count; ++i) {
        float leak_term = (0.0f - potentials[i]) * (1.0f / timeConstant);
        float syn_term = inputs[i];
        potentials[i] += 0.001f * (leak_term + syn_term);
        potentials[i] = std::clamp(potentials[i], -100.0f, 50.0f);
    }
}

void SimdOperations::updateSynapticWeightsAVX(
    float* weights, const float* preSpike, const float* postSpike,
    const float* eligibility, float learningRate,
    size_t count, float& processingTime) {
    
    auto start = std::chrono::high_resolution_clock::now();
    
    __m128 lr = _mm_set1_ps(learningRate);
    
    size_t i = 0;
    for (; i + 7 < count; i += 8) {
        __m128 pre1 = _mm_loadu_ps(&preSpike[i]);
        __m128 pre2 = _mm_loadu_ps(&preSpike[i + 4]);
        
        __m128 post1 = _mm_loadu_ps(&postSpike[i]);
        __m128 post2 = _mm_loadu_ps(&postSpike[i + 4]);
        
        __m128 elig1 = _mm_loadu_ps(&eligibility[i]);
        __m128 elig2 = _mm_loadu_ps(&eligibility[i + 4]);
        
        __m128 w1 = _mm_loadu_ps(&weights[i]);
        __m128 w2 = _mm_loadu_ps(&weights[i + 4]);
        
        __m128 stup_term1 = _mm_mul_ps(_mm_set1_ps(0.01f), _mm_mul_ps(post1, elig1));
        __m128 stup_term2 = _mm_mul_ps(_mm_set1_ps(0.01f), _mm_mul_ps(post2, elig2));
        
        __m128 stdp_term1 = _mm_mul_ps(_mm_set1_ps(-0.012f), _mm_mul_ps(pre1, elig1));
        __m128 stdp_term2 = _mm_mul_ps(_mm_set1_ps(-0.012f), _mm_mul_ps(pre2, elig2));
        
        w1 = _mm_add_ps(w1, _mm_mul_ps(lr, _mm_add_ps(stup_term1, stdp_term1)));
        w2 = _mm_add_ps(w2, _mm_mul_ps(lr, _mm_add_ps(stup_term2, stdp_term2)));
        
        const __m128 min_weight = _mm_set1_ps(-1.0f);
        const __m128 max_weight = _mm_set1_ps(1.0f);
        
        w1 = _mm_max_ps(_mm_min_ps(w1, max_weight), min_weight);
        w2 = _mm_max_ps(_mm_min_ps(w2, max_weight), min_weight);
        
        _mm_storeu_ps(&weights[i], w1);
        _mm_storeu_ps(&weights[i + 4], w2);
    }
    
    for (; i < count; ++i) {
        float stup = 0.01f * postSpike[i] * eligibility[i];
        float stdp = -0.012f * preSpike[i] * eligibility[i];
        weights[i] += learningRate * (stup + stdp);
        weights[i] = std::clamp(weights[i], -1.0f, 1.0f);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    processingTime = std::chrono::duration<float, std::milli>(end - start).count();
}

void SimdOperations::detectSpikesSSE(
    const float* potentials, bool* spikeFlags, 
    float threshold, uint32_t* refractoryCounters,
    size_t count) {
    
    __m128 th = _mm_set1_ps(threshold);
    
    size_t i = 0;
    for (; i + 3 < count; i += 4) {
        __m128 v = _mm_loadu_ps(&potentials[i]);
        __m128 spike_mask = _mm_cmpgt_ps(v, th);
        int mask = _mm_movemask_ps(spike_mask);
        
        if (mask & 0x1) spikeFlags[i] = true;
        if (mask & 0x2) spikeFlags[i + 1] = true;
        if (mask & 0x4) spikeFlags[i + 2] = true;
        if (mask & 0x8) spikeFlags[i + 3] = true;
    }
    
    for (; i < count; ++i) {
        spikeFlags[i] = (potentials[i] > threshold);
    }
}

void SimdOperations::optimizeMemoryAccessPattern(float* buffer, size_t size) {
    if ((reinterpret_cast<uintptr_t>(buffer) & 31) != 0) {
        alignas(32) float aligned_buffer[1024];
        if (size <= 1024) {
            std::memcpy(aligned_buffer, buffer, size * sizeof(float));
            std::memcpy(buffer, aligned_buffer, size * sizeof(float));
        }
    }
}

void SimdOperations::computeNeuralUpdateFused(
    float* membranePotentials, float* synapticInputs,
    float leakConductance, float timeConstant,
    float threshold, bool* spikeFlags, uint32_t* refractoryCounters,
    size_t count) {
    
    __m128 leak = _mm_set1_ps(leakConductance);
    __m128 inv_tau = _mm_set1_ps(1.0f / timeConstant);
    __m128 dt = _mm_set1_ps(0.001f);
    __m128 th = _mm_set1_ps(threshold);
    
    size_t i = 0;
    for (; i + 3 < count; i += 4) {
        __m128 v = _mm_loadu_ps(&membranePotentials[i]);
        __m128 inputs = _mm_loadu_ps(&synapticInputs[i]);
        
        __m128 leak_term = _mm_mul_ps(_mm_sub_ps(_mm_setzero_ps(), v), inv_tau);
        __m128 syn_term = inputs;
        __m128 dv = _mm_mul_ps(dt, _mm_add_ps(leak_term, syn_term));
        
        v = _mm_add_ps(v, dv);
        v = _mm_max_ps(v, _mm_set1_ps(-100.0f));
        v = _mm_min_ps(v, _mm_set1_ps(50.0f));
        
        _mm_storeu_ps(&membranePotentials[i], v);
        _mm_storeu_ps(&synapticInputs[i], _mm_setzero_ps());
    }
    
    detectSpikesSSE(&membranePotentials[0], spikeFlags, threshold, nullptr, count);
}

} // namespace nlm
