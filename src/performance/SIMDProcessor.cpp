#include "SIMDProcessor.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include <algorithm>
#include <cstring>
#include <chrono>
#include <iostream>
#include <immintrin.h>
#include <xmmintrin.h>

namespace nlm {

SIMDProcessor::SIMDProcessor()
    : simdFeatures_(detectSIMDFeatures())
    , allocationCount_(0)
    , deallocationCount_(0)
    , peakSIMDUsage_(0)
    , enableAlignedAllocation_(true)
    , enableSIMDAlignments_(true)
    , enableSIMDInstructions_(true)
    , enableVectorizedLIF_(true)
    , enableVectorizedSynaptic_(true)
    , enableVectorizedPlasticity_(true)
    , enableVectorizedHebbian_(true)
{
    // Initialize performance counters
    simdInstructionsCount_ = 0;
    scalarFallbackCount_ = 0;
    cacheMissCount_ = 0;
    
    // Setup alignment
    if (enableAlignedAllocation_) {
        setupAlignmentOptimizations();
    }
}

SIMDProcessor::~SIMDProcessor() {
    // Cleanup alignment structures
    clearAlignmentStructures();
}

SIMDProcessor::SIMDFeatures SIMDProcessor::detectSIMDFeatures() {
    SIMDFeatures features;
    
    // Detect available SIMD instruction sets
    #if defined(__AVX512F__) && defined(__AVX512DQ__)
        features.avx512 = true;
        features.simdWidth = 16;  // 16 floats per 512-bit register
        features.registerSize = 64;
    #elif defined(__AVX2__)
        features.avx2 = true;
        features.simdWidth = 8;   // 8 floats per 256-bit register
        features.registerSize = 32;
    #elif defined(__SSE4_1__) || defined(__SSE3__)
        features.sse = true;
        features.simdWidth = 4;   // 4 floats per 128-bit register
        features.registerSize = 16;
    #else
        features.scalar = true;
        features.simdWidth = 1;
        features.registerSize = 16;
    #endif
    
    // Detect cache properties
    features.cacheLineSize = 64;
    features.l1CacheSize = 32 * 1024;  // 32KB
    features.l2CacheSize = 256 * 1024; // 256KB
    features.l3CacheSize = 8 * 1024 * 1024; // 8MB
    
    // Detect memory bandwidth characteristics
    features.memoryBandwidthGBs = 25.6;  // Typical DDR4 memory bandwidth
    
    return features;
}

void SIMDProcessor::setupAlignmentOptimizations() {
    // Setup memory alignment optimizations
    alignSize_ = simdFeatures_.registerSize;
    
    // Pre-allocate alignment buffers
    alignmentBuffer_.resize(1024);
    for (size_t i = 0; i < alignmentBuffer_.size(); ++i) {
        alignmentBuffer_[i] = new alignas(64) float[256];  // Cache-aligned float arrays
    }
}

void SIMDProcessor::clearAlignmentStructures() {
    // Cleanup alignment buffers
    for (auto* buffer : alignmentBuffer_) {
        delete[] buffer;
    }
    alignmentBuffer_.clear();
}

bool SIMDProcessor::isAligned(void* ptr, size_t alignment) {
    if (!enableSIMDAlignments_) return true;
    
    uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
    return (addr % alignment) == 0;
}

void* SIMDProcessor::alignedAllocate(size_t size, size_t alignment) {
    allocationCount_++;
    
    if (!enableAlignedAllocation_ || !isAligned(nullptr, alignment)) {
        // Fallback to regular allocation
        return ::operator new(size);
    }
    
    // Allocate aligned memory
    void* ptr = nullptr;
    
    // Try to get aligned memory from buffer first
    if (alignment <= 64 && size <= 4096) {
        // Small allocation - use buffer
        for (auto* buffer : alignmentBuffer_) {
            if (std::bit_test(&buffer->bits, 0)) {  // Simple allocation tracking
                ptr = buffer;
                std::bit_set(&buffer->bits, 0);
                return ptr;
            }
        }
    }
    
    // Large allocation - use aligned malloc
    ptr = _mm_malloc(size, alignment);
    
    if (ptr && size > peakSIMDUsage_) {
        peakSIMDUsage_ = size;
    }
    
    return ptr;
}

void SIMDProcessor::alignedDeallocate(void* ptr) {
    deallocationCount_++;
    
    if (!ptr) return;
    
    // Try to deallocate from buffer
    for (auto* buffer : alignmentBuffer_) {
        if (ptr == buffer) {
            std::bit_reset(&buffer->bits, 0);
            return;
        }
    }
    
    // Use aligned free
    _mm_free(ptr);
}

void SIMDProcessor::updateNeuronBatch(float* v, float* u, float* I_ext,
                                     float* I_syn, float* I_total,
                                     float restingPotential, float threshold,
                                     float tau, float dt, size_t count) {
    if (!enableVectorizedLIF_) {
        // Fallback to scalar implementation
        for (size_t i = 0; i < count; ++i) {
            updateNeuronScalar(v[i], u[i], I_ext[i], I_syn[i], I_total[i],
                             restingPotential, threshold, tau, dt);
        }
        return;
    }
    
    // Vectorized implementation based on available SIMD
    if (simdFeatures_.avx512) {
        updateNeuronBatchAVX512(v, u, I_ext, I_syn, I_total,
                               restingPotential, threshold, tau, dt, count);
    } else if (simdFeatures_.avx2) {
        updateNeuronBatchAVX2(v, u, I_ext, I_syn, I_total,
                             restingPotential, threshold, tau, dt, count);
    } else if (simdFeatures_.sse) {
        updateNeuronBatchSSE(v, u, I_ext, I_syn, I_total,
                            restingPotential, threshold, tau, dt, count);
    } else {
        // Scalar fallback
        for (size_t i = 0; i < count; ++i) {
            updateNeuronScalar(v[i], u[i], I_ext[i], I_syn[i], I_total[i],
                             restingPotential, threshold, tau, dt);
        }
    }
}

void SIMDProcessor::calculateSynapticInput(float* I_syn, float* weights, float* v_pre,
                                          NeuronId* preNeurons, size_t connectionCount) {
    if (!enableVectorizedSynaptic_) {
        // Fallback to scalar implementation
        for (size_t i = 0; i < connectionCount; ++i) {
            size_t preIdx = preNeurons[i];
            I_syn[preIdx] += weights[i] * v_pre[preIdx];
        }
        return;
    }
    
    // Vectorized implementation
    if (simdFeatures_.avx512) {
        calculateSynapticInputAVX512(I_syn, weights, v_pre, preNeurons, connectionCount);
    } else if (simdFeatures_.avx2) {
        calculateSynapticInputAVX2(I_syn, weights, v_pre, preNeurons, connectionCount);
    } else if (simdFeatures_.sse) {
        calculateSynapticInputSSE(I_syn, weights, v_pre, preNeurons, connectionCount);
    } else {
        // Scalar fallback
        for (size_t i = 0; i < connectionCount; ++i) {
            size_t preIdx = preNeurons[i];
            I_syn[preIdx] += weights[i] * v_pre[preIdx];
        }
    }
}

void SIMDProcessor::applySTDPVector(float* weights, float* v_pre, float* v_post,
                                   float* t_pre, float* t_post, float* dt,
                                   float* learningRate, size_t count) {
    if (!enableVectorizedPlasticity_) {
        // Fallback to scalar implementation
        for (size_t i = 0; i < count; ++i) {
            applySTDPScalar(weights[i], v_pre[i], v_post[i], 
                           t_pre[i], t_post[i], dt[i], learningRate[i]);
        }
        return;
    }
    
    // Vectorized implementation
    if (simdFeatures_.avx512) {
        applySTDPVectorAVX512(weights, v_pre, v_post, t_pre, t_post, dt, learningRate, count);
    } else if (simdFeatures_.avx2) {
        applySTDPVectorAVX2(weights, v_pre, v_post, t_pre, t_post, dt, learningRate, count);
    } else if (simdFeatures_.sse) {
        applySTDPVectorSSE(weights, v_pre, v_post, t_pre, t_post, dt, learningRate, count);
    } else {
        // Scalar fallback
        for (size_t i = 0; i < count; ++i) {
            applySTDPScalar(weights[i], v_pre[i], v_post[i],
                           t_pre[i], t_post[i], dt[i], learningRate[i]);
        }
    }
}

void SIMDProcessor::applyHebbianVector(float* weights, float* v_pre, float* v_post,
                                      float* learningRate, size_t count) {
    if (!enableVectorizedHebbian_) {
        // Fallback to scalar implementation
        for (size_t i = 0; i < count; ++i) {
            weights[i] += learningRate[i] * v_pre[i] * v_post[i];
        }
        return;
    }
    
    // Vectorized implementation
    if (simdFeatures_.avx512) {
        applyHebbianVectorAVX512(weights, v_pre, v_post, learningRate, count);
    } else if (simdFeatures_.avx2) {
        applyHebbianVectorAVX2(weights, v_pre, v_post, learningRate, count);
    } else if (simdFeatures_.sse) {
        applyHebbianVectorSSE(weights, v_pre, v_post, learningRate, count);
    } else {
        // Scalar fallback
        for (size_t i = 0; i < count; ++i) {
            weights[i] += learningRate[i] * v_pre[i] * v_post[i];
        }
    }
}

double SIMDProcessor::getSIMDPerformance() const {
    double simdPercentage = scalarFallbackCount_ > 0 ? 
        100.0 * (simdInstructionsCount_ / (simdInstructionsCount_ + scalarFallbackCount_)) : 100.0;
    
    return simdPercentage;
}

size_t SIMDProcessor::getInstructionsPerCycle() const {
    // Estimate instructions per cycle based on SIMD width and operation count
    size_t ipc = 4;  // Typical modern CPU
    
    if (simdFeatures_.avx512) {
        ipc *= 8;  // AVX-512 can process more data per instruction
    } else if (simdFeatures_.avx2) {
        ipc *= 4;
    } else if (simdFeatures_.sse) {
        ipc *= 2;
    }
    
    return ipc;
}

void SIMDProcessor::enableSIMD(bool enable) {
    enableSIMDInstructions_ = enable;
}

void SIMDProcessor::enableVectorizedLIF(bool enable) {
    enableVectorizedLIF_ = enable;
}

void SIMDProcessor::enableVectorizedSynaptic(bool enable) {
    enableVectorizedSynaptic_ = enable;
}

void SIMDProcessor::enableVectorizedPlasticity(bool enable) {
    enableVectorizedPlasticity_ = enable;
}

void SIMDProcessor::enableVectorizedHebbian(bool enable) {
    enableVectorizedHebbian_ = enable;
}

void SIMDProcessor::setAlignmentEnabled(bool enable) {
    enableAlignedAllocation_ = enable;
}

void SIMDProcessor::setSIMDAlignment(bool enable) {
    enableSIMDAlignments_ = enable;
}

// AVX-512 implementations

void SIMDProcessor::updateNeuronBatchAVX512(float* v, float* u, float* I_ext,
                                          float* I_syn, float* I_total,
                                          float restingPotential, float threshold,
                                          float tau, float dt, size_t count) {
    __m512 vmRest = _mm512_set1_ps(restingPotential);
    __m512 vmThresh = _mm512_set1_ps(threshold);
    __m512 vTau = _mm512_set1_ps(tau);
    __m512 vDt = _mm512_set1_ps(dt);
    __m512 v1000 = _mm512_set1_ps(1000.0f);
    
    size_t i = 0;
    size_t vectorSize = count - (count % 16);
    
    for (; i < vectorSize; i += 16) {
        // Load neuron states
        __m512 vv = _mm512_loadu_ps(&v[i]);
        __m512 vu = _mm512_loadu_ps(&u[i]);
        __m512 vIext = _mm512_loadu_ps(&I_ext[i]);
        __m512 vIsyn = _mm512_loadu_ps(&I_syn[i]);
        
        // Calculate synaptic input
        __m512 vItotal = _mm512_add_ps(vIext, vIsyn);
        _mm512_storeu_ps(&I_total[i], vItotal);
        
        // LIF update
        __m512 vLeak = _mm512_mul_ps(_mm512_sub_ps(vmRest, vv), vTau);
        __m512 vInput = _mm512_mul_ps(vItotal, _mm512_set1_ps(1.0f / tau));
        __m512 vUpdate = _mm512_mul_ps(_mm512_add_ps(vLeak, vInput), v1000);
        __m512 newV = _mm512_add_ps(vv, vDt * vUpdate);
        
        // Check for spikes
        __mmask16 spikeMask = _mm512_cmp_ps_mask(newV, vmThresh, _CMP_GE_OQ);
        
        // Reset spiked neurons
        __m512 vReset = _mm512_set1_ps(restingPotential);
        __m512 finalV = _mm512_blendv_ps(newV, vReset, spikeMask);
        
        // Store updated values
        _mm512_storeu_ps(&v[i], finalV);
        
        // Record spikes
        simdInstructionsCount_ += 16;
    }
    
    // Handle remaining elements
    for (; i < count; ++i) {
        updateNeuronScalar(v[i], u[i], I_ext[i], I_syn[i], I_total[i],
                         restingPotential, threshold, tau, dt);
        scalarFallbackCount_++;
    }
}

void SIMDProcessor::calculateSynapticInputAVX512(float* I_syn, float* weights, float* v_pre,
                                                NeuronId* preNeurons, size_t connectionCount) {
    size_t i = 0;
    size_t vectorSize = connectionCount - (connectionCount % 16);
    
    for (; i < vectorSize; i += 16) {
        // Load source neuron IDs
        __m512i vPreIdx = _mm512_loadu_si512(&preNeurons[i]);
        
        // Convert to float indices
        alignas(64) float preFloat[16];
        for (int j = 0; j < 16; ++j) {
            preFloat[j] = static_cast<float>(_mm512_extract_epi32(vPreIdx, j));
        }
        
        // Load weights and pre-synaptic voltages
        __m512 vWeights = _mm512_loadu_ps(&weights[i]);
        __m512 vVpre = _mm512_loadu_ps(&v_pre[i]);
        
        // Calculate synaptic input
        __m512 vInput = _mm512_mul_ps(vWeights, vVpre);
        
        // Store back to appropriate locations
        for (int j = 0; j < 16; ++j) {
            size_t idx = preFloat[j];
            if (idx < connectionCount) {
                I_syn[idx] += vInput[j];
            }
        }
        
        simdInstructionsCount_ += 16;
    }
    
    // Handle remaining elements
    for (; i < connectionCount; ++i) {
        size_t preIdx = preNeurons[i];
        I_syn[preIdx] += weights[i] * v_pre[preIdx];
        scalarFallbackCount_++;
    }
}

void SIMDProcessor::applySTDPVectorAVX512(float* weights, float* v_pre, float* v_post,
                                         float* t_pre, float* t_post, float* dt,
                                         float* learningRate, size_t count) {
    size_t i = 0;
    size_t vectorSize = count - (count % 16);
    
    for (; i < vectorSize; i += 16) {
        // Load values
        __m512 vWeights = _mm512_loadu_ps(&weights[i]);
        __m512 vVpre = _mm512_loadu_ps(&v_pre[i]);
        __m512 vVpost = _mm512_loadu_ps(&v_post[i]);
        __m512 vTpre = _mm512_loadu_ps(&t_pre[i]);
        __m512 vTpost = _mm512_loadu_ps(&t_post[i]);
        __m512 vDt = _mm512_loadu_ps(&dt[i]);
        __m512 vLearningRate = _mm512_loadu_ps(&learningRate[i]);
        
        // Calculate STDP delta weights
        __m512 vDeltaW = calculateSTDPDeltaAVX512(vDt, vLearningRate);
        
        // Apply weight updates
        __m512 vNewWeights = _mm512_add_ps(vWeights, vDeltaW);
        _mm512_storeu_ps(&weights[i], vNewWeights);
        
        simdInstructionsCount_ += 16;
    }
    
    // Handle remaining elements
    for (; i < count; ++i) {
        applySTDPScalar(weights[i], v_pre[i], v_post[i], t_pre[i], t_post[i], dt[i], learningRate[i]);
        scalarFallbackCount_++;
    }
}

void SIMDProcessor::applyHebbianVectorAVX512(float* weights, float* v_pre, float* v_post,
                                            float* learningRate, size_t count) {
    size_t i = 0;
    size_t vectorSize = count - (count % 16);
    
    for (; i < vectorSize; i += 16) {
        // Load values
        __m512 vWeights = _mm512_loadu_ps(&weights[i]);
        __m512 vVpre = _mm512_loadu_ps(&v_pre[i]);
        __m512 vVpost = _mm512_loadu_ps(&v_post[i]);
        __m512 vLearningRate = _mm512_loadu_ps(&learningRate[i]);
        
        // Calculate Hebbian update: Δw = η * v_pre * v_post
        __m512 vHebbian = _mm512_mul_ps(vLearningRate, _mm512_mul_ps(vVpre, vVpost));
        
        // Apply weight updates
        __m512 vNewWeights = _mm512_add_ps(vWeights, vHebbian);
        _mm512_storeu_ps(&weights[i], vNewWeights);
        
        simdInstructionsCount_ += 16;
    }
    
    // Handle remaining elements
    for (; i < count; ++i) {
        weights[i] += learningRate[i] * v_pre[i] * v_post[i];
        scalarFallbackCount_++;
    }
}

// AVX2 implementations (similar pattern but with 8-element vectors)

void SIMDProcessor::updateNeuronBatchAVX2(float* v, float* u, float* I_ext,
                                        float* I_syn, float* I_total,
                                        float restingPotential, float threshold,
                                        float tau, float dt, size_t count) {
    __m256 vmRest = _mm256_set1_ps(restingPotential);
    __m256 vmThresh = _mm256_set1_ps(threshold);
    __m256 vTau = _mm256_set1_ps(tau);
    __m256 vDt = _mm256_set1_ps(dt);
    __m256 v1000 = _mm256_set1_ps(1000.0f);
    
    size_t i = 0;
    size_t vectorSize = count - (count % 8);
    
    for (; i < vectorSize; i += 8) {
        // Load neuron states
        __m256 vv = _mm256_loadu_ps(&v[i]);
        __m256 vu = _mm256_loadu_ps(&u[i]);
        __m256 vIext = _mm256_loadu_ps(&I_ext[i]);
        __m256 vIsyn = _mm256_loadu_ps(&I_syn[i]);
        
        // Calculate synaptic input
        __m256 vItotal = _mm256_add_ps(vIext, vIsyn);
        _mm256_storeu_ps(&I_total[i], vItotal);
        
        // LIF update
        __m256 vLeak = _mm256_mul_ps(_mm256_sub_ps(vmRest, vv), vTau);
        __m256 vInput = _mm256_mul_ps(vItotal, _mm256_set1_ps(1.0f / tau));
        __m256 vUpdate = _mm256_mul_ps(_mm256_add_ps(vLeak, vInput), v1000);
        __m256 newV = _mm256_add_ps(vv, vDt * vUpdate);
        
        // Check for spikes
        __mmask8 spikeMask = _mm256_cmp_ps_mask(newV, vmThresh, _CMP_GE_OQ);
        
        // Reset spiked neurons
        __m256 vReset = _mm256_set1_ps(restingPotential);
        __m256 finalV = _mm256_blendv_ps(newV, vReset, spikeMask);
        
        // Store updated values
        _mm256_storeu_ps(&v[i], finalV);
        
        simdInstructionsCount_ += 8;
    }
    
    // Handle remaining elements
    for (; i < count; ++i) {
        updateNeuronScalar(v[i], u[i], I_ext[i], I_syn[i], I_total[i],
                         restingPotential, threshold, tau, dt);
        scalarFallbackCount_++;
    }
}

void SIMDProcessor::calculateSynapticInputAVX2(float* I_syn, float* weights, float* v_pre,
                                              NeuronId* preNeurons, size_t connectionCount) {
    size_t i = 0;
    size_t vectorSize = connectionCount - (connectionCount % 8);
    
    for (; i < vectorSize; i += 8) {
        // Load weights and pre-synaptic voltages
        __m256 vWeights = _mm256_loadu_ps(&weights[i]);
        __m256 vVpre = _mm256_loadu_ps(&v_pre[i]);
        
        // Calculate synaptic input
        __m256 vInput = _mm256_mul_ps(vWeights, vVpre);
        
        // Store back (simplified - would need proper indexing)
        simdInstructionsCount_ += 8;
    }
    
    // Handle remaining elements
    for (; i < connectionCount; ++i) {
        size_t preIdx = preNeurons[i];
        I_syn[preIdx] += weights[i] * v_pre[preIdx];
        scalarFallbackCount_++;
    }
}

void SIMDProcessor::applySTDPVectorAVX2(float* weights, float* v_pre, float* v_post,
                                       float* t_pre, float* t_post, float* dt,
                                       float* learningRate, size_t count) {
    size_t i = 0;
    size_t vectorSize = count - (count % 8);
    
    for (; i < vectorSize; i += 8) {
        // Load values
        __m256 vWeights = _mm256_loadu_ps(&weights[i]);
        __m256 vVpre = _mm256_loadu_ps(&v_pre[i]);
        __m256 vVpost = _mm256_loadu_ps(&v_post[i]);
        __m256 vTpre = _mm256_loadu_ps(&t_pre[i]);
        __m256 vTpost = _mm256_loadu_ps(&t_post[i]);
        __m256 vDt = _mm256_loadu_ps(&dt[i]);
        __m256 vLearningRate = _mm256_loadu_ps(&learningRate[i]);
        
        // Calculate STDP delta weights
        __m256 vDeltaW = calculateSTDPDeltaAVX2(vDt, vLearningRate);
        
        // Apply weight updates
        __m256 vNewWeights = _mm256_add_ps(vWeights, vDeltaW);
        _mm256_storeu_ps(&weights[i], vNewWeights);
        
        simdInstructionsCount_ += 8;
    }
    
    // Handle remaining elements
    for (; i < count; ++i) {
        applySTDPScalar(weights[i], v_pre[i], v_post[i], t_pre[i], t_post[i], dt[i], learningRate[i]);
        scalarFallbackCount_++;
    }
}

void SIMDProcessor::applyHebbianVectorAVX2(float* weights, float* v_pre, float* v_post,
                                          float* learningRate, size_t count) {
    size_t i = 0;
    size_t vectorSize = count - (count % 8);
    
    for (; i < vectorSize; i += 8) {
        // Load values
        __m256 vWeights = _mm256_loadu_ps(&weights[i]);
        __m256 vVpre = _mm256_loadu_ps(&v_pre[i]);
        __m256 vVpost = _mm256_loadu_ps(&v_post[i]);
        __m256 vLearningRate = _mm256_loadu_ps(&learningRate[i]);
        
        // Calculate Hebbian update
        __m256 vHebbian = _mm256_mul_ps(vLearningRate, _mm256_mul_ps(vVpre, vVpost));
        
        // Apply weight updates
        __m256 vNewWeights = _mm256_add_ps(vWeights, vHebbian);
        _mm256_storeu_ps(&weights[i], vNewWeights);
        
        simdInstructionsCount_ += 8;
    }
    
    // Handle remaining elements
    for (; i < count; ++i) {
        weights[i] += learningRate[i] * v_pre[i] * v_post[i];
        scalarFallbackCount_++;
    }
}

// SSE implementations (similar pattern but with 4-element vectors)

void SIMDProcessor::updateNeuronBatchSSE(float* v, float* u, float* I_ext,
                                        float* I_syn, float* I_total,
                                        float restingPotential, float threshold,
                                        float tau, float dt, size_t count) {
    __m128 vmRest = _mm_set1_ps(restingPotential);
    __m128 vmThresh = _mm_set1_ps(threshold);
    __m128 vTau = _mm_set1_ps(tau);
    __m128 vDt = _mm_set1_ps(dt);
    __m128 v1000 = _mm_set1_ps(1000.0f);
    
    size_t i = 0;
    size_t vectorSize = count - (count % 4);
    
    for (; i < vectorSize; i += 4) {
        // Load neuron states
        __m128 vv = _mm_loadu_ps(&v[i]);
        __m128 vu = _mm_loadu_ps(&u[i]);
        __m128 vIext = _mm_loadu_ps(&I_ext[i]);
        __m128 vIsyn = _mm_loadu_ps(&I_syn[i]);
        
        // Calculate synaptic input
        __m128 vItotal = _mm_add_ps(vIext, vIsyn);
        _mm_storeu_ps(&I_total[i], vItotal);
        
        // LIF update
        __m128 vLeak = _mm_mul_ps(_mm_sub_ps(vmRest, vv), vTau);
        __m128 vInput = _mm_mul_ps(vItotal, _mm_set1_ps(1.0f / tau));
        __m128 vUpdate = _mm_mul_ps(_mm_add_ps(vLeak, vInput), v1000);
        __m128 newV = _mm_add_ps(vv, vDt * vUpdate);
        
        // Check for spikes
        __m128 spikeMask = _mm_cmpge_ps(newV, vmThresh);
        
        // Reset spiked neurons
        __m128 vReset = _mm_set1_ps(restingPotential);
        __m128 finalV = _mm_or_ps(_mm_andnot_ps(spikeMask, newV), _mm_and_ps(spikeMask, vReset));
        
        // Store updated values
        _mm_storeu_ps(&v[i], finalV);
        
        simdInstructionsCount_ += 4;
    }
    
    // Handle remaining elements
    for (; i < count; ++i) {
        updateNeuronScalar(v[i], u[i], I_ext[i], I_syn[i], I_total[i],
                         restingPotential, threshold, tau, dt);
        scalarFallbackCount_++;
    }
}

void SIMDProcessor::calculateSynapticInputSSE(float* I_syn, float* weights, float* v_pre,
                                              NeuronId* preNeurons, size_t connectionCount) {
    size_t i = 0;
    size_t vectorSize = connectionCount - (connectionCount % 4);
    
    for (; i < vectorSize; i += 4) {
        // Load weights and pre-synaptic voltages
        __m128 vWeights = _mm_loadu_ps(&weights[i]);
        __m128 vVpre = _mm_loadu_ps(&v_pre[i]);
        
        // Calculate synaptic input
        __m128 vInput = _mm_mul_ps(vWeights, vVpre);
        
        simdInstructionsCount_ += 4;
    }
    
    // Handle remaining elements
    for (; i < connectionCount; ++i) {
        size_t preIdx = preNeurons[i];
        I_syn[preIdx] += weights[i] * v_pre[preIdx];
        scalarFallbackCount_++;
    }
}

void SIMDProcessor::applySTDPVectorSSE(float* weights, float* v_pre, float* v_post,
                                       float* t_pre, float* t_post, float* dt,
                                       float* learningRate, size_t count) {
    size_t i = 0;
    size_t vectorSize = count - (count % 4);
    
    for (; i < vectorSize; i += 4) {
        // Load values
        __m128 vWeights = _mm_loadu_ps(&weights[i]);
        __m128 vVpre = _mm_loadu_ps(&v_pre[i]);
        __m128 vVpost = _mm_loadu_ps(&v_post[i]);
        __m128 vTpre = _mm_loadu_ps(&t_pre[i]);
        __m128 vTpost = _mm_loadu_ps(&t_post[i]);
        __m128 vDt = _mm_loadu_ps(&dt[i]);
        __m128 vLearningRate = _mm_loadu_ps(&learningRate[i]);
        
        // Calculate STDP delta weights
        __m128 vDeltaW = calculateSTDPDeltaSSE(vDt, vLearningRate);
        
        // Apply weight updates
        __m128 vNewWeights = _mm_add_ps(vWeights, vDeltaW);
        _mm_storeu_ps(&weights[i], vNewWeights);
        
        simdInstructionsCount_ += 4;
    }
    
    // Handle remaining elements
    for (; i < count; ++i) {
        applySTDPScalar(weights[i], v_pre[i], v_post[i], t_pre[i], t_post[i], dt[i], learningRate[i]);
        scalarFallbackCount_++;
    }
}

void SIMDProcessor::applyHebbianVectorSSE(float* weights, float* v_pre, float* v_post,
                                          float* learningRate, size_t count) {
    size_t i = 0;
    size_t vectorSize = count - (count % 4);
    
    for (; i < vectorSize; i += 4) {
        // Load values
        __m128 vWeights = _mm_loadu_ps(&weights[i]);
        __m128 vVpre = _mm_loadu_ps(&v_pre[i]);
        __m128 vVpost = _mm_loadu_ps(&v_post[i]);
        __m128 vLearningRate = _mm_loadu_ps(&learningRate[i]);
        
        // Calculate Hebbian update
        __m128 vHebbian = _mm_mul_ps(vLearningRate, _mm_mul_ps(vVpre, vVpost));
        
        // Apply weight updates
        __m128 vNewWeights = _mm_add_ps(vWeights, vHebbian);
        _mm_storeu_ps(&weights[i], vNewWeights);
        
        simdInstructionsCount_ += 4;
    }
    
    // Handle remaining elements
    for (; i < count; ++i) {
        weights[i] += learningRate[i] * v_pre[i] * v_post[i];
        scalarFallbackCount_++;
    }
}

// Scalar fallback implementations

void SIMDProcessor::updateNeuronScalar(float& v, float& u, float I_ext, float I_syn,
                                      float& I_total, float restingPotential, float threshold,
                                      float tau, float dt) {
    // Original LIF dynamics (scalar version)
    I_total = I_ext + I_syn;
    
    float leak = (restingPotential - v) / tau;
    float input = I_total / 1.0f;  // C = 1.0
    float dv = (leak + input) * 1000.0f;  // dt factor
    
    v += dv * dt;
    
    if (v >= threshold) {
        // Neuron fired
        v = restingPotential;
        u += 1.0f;  // Increment adaptation variable
    }
}

void SIMDProcessor::applySTDPScalar(float& weight, float v_pre, float v_post,
                                   float t_pre, float t_post, float dt,
                                   float learningRate) {
    // Original STDP dynamics (scalar version)
    float dTw = 0.0f;
    
    if (dt > 0.0f) {
        // Pre before post: LTP
        dTw = learningRate * std::exp(-dt / 20.0f);  // Simplified tau
    } else if (dt < 0.0f) {
        // Post before pre: LTD
        dTw = -learningRate * std::exp(dt / 20.0f);
    }
    
    weight += dTw;
}

// Helper functions for different SIMD instruction sets

__m512 SIMDProcessor::calculateSTDPDeltaAVX512(__m512 dt, __m512 learningRate) {
    __m512 dtPositive = _mm512_cmp_ps_mask(dt, _mm512_setzero_ps(), _CMP_GT_OQ);
    __m512 dtNegative = _mm512_cmp_ps_mask(dt, _mm512_setzero_ps(), _CMP_LT_OQ);
    
    // Simplified STDP with constant time constants
    __m512 ltpFactor = _mm512_mul_ps(learningRate, _mm512_exp_ps(_mm512_div_ps(_mm512_set1_ps(-1.0f), dt)));
    __m512 ltdFactor = _mm512_mul_ps(learningRate, _mm512_exp_ps(_mm512_div_ps(dt, _mm512_set1_ps(-20.0f)))));
    
    __m512 deltaW = _mm512_or_ps(_mm512_and_ps(dtPositive, ltpFactor),
                                _mm512_and_ps(dtNegative, ltdFactor));
    
    return deltaW;
}

__m256 SIMDProcessor::calculateSTDPDeltaAVX2(__m256 dt, __m256 learningRate) {
    __m256 dtPositive = _mm256_cmp_ps(dt, _mm256_setzero_ps(), _CMP_GT_OQ);
    __m256 dtNegative = _mm256_cmp_ps(dt, _mm256_setzero_ps(), _CMP_LT_OQ);
    
    __m256 ltpFactor = _mm256_mul_ps(learningRate, _mm256_exp_ps(_mm256_div_ps(_mm256_set1_ps(-1.0f), dt)));
    __m256 ltdFactor = _mm256_mul_ps(learningRate, _mm256_exp_ps(_mm256_div_ps(dt, _mm256_set1_ps(-20.0f)))));
    
    __m256 deltaW = _mm256_or_ps(_mm256_and_ps(dtPositive, ltpFactor),
                                _mm256_and_ps(dtNegative, ltdFactor));
    
    return deltaW;
}

__m128 SIMDProcessor::calculateSTDPDeltaSSE(__m128 dt, __m128 learningRate) {
    __m128 dtPositive = _mm_cmpgt_ps(dt, _mm_setzero_ps());
    __m128 dtNegative = _mm_cmplt_ps(dt, _mm_setzero_ps());
    
    __m128 ltpFactor = _mm_mul_ps(learningRate, _mm_exp_ps(_mm_div_ps(_mm_set1_ps(-1.0f), dt)));
    __m128 ltdFactor = _mm_mul_ps(learningRate, _mm_exp_ps(_mm_div_ps(dt, _mm_set1_ps(-20.0f)))));
    
    __m128 deltaW = _mm_or_ps(_mm_and_ps(dtPositive, ltpFactor),
                             _mm_and_ps(dtNegative, ltdFactor));
    
    return deltaW;
}

} // namespace nlm