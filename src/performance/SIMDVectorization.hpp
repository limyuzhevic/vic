// SIMD vectorization system for NLM
// Provides SIMD optimizations for neural computations

#pragma once

#include <immintrin.h>
#include <xmmintrin.h>

namespace nlm {

/**
 * SIMD vectorization system for neural computations
 * Optimizes neural network operations using SIMD instructions
 */
class SIMDVectorization {
public:
    SIMDVectorization();
    ~SIMDVectorization();
    
    static bool isSupported();
    static SIMDVectorization& getInstance();
    
    // Vector operations
    void vectorAdd(const float* a, const float* b, float* result, size_t n);
    void vectorMultiply(const float* a, const float* b, float* result, size_t n);
    void vectorDotProduct(const float* a, const float* b, float& result, size_t n);
    
    // Neural network operations
    void neuralUpdateWeights(const float* input, const float* error, float* weights, size_t size);
    void neuralForwardPropagate(const float* input, float* output, size_t size);
    void neuralBackPropagate(const float* output, const float* target, float* error, size_t size);
    
    // Neuron operations
    void neuronUpdateMembranePotential(float* v, float* I, float* tau, size_t n);
    void neuronUpdateSynapticWeights(float* w, float* delta, float learningRate, size_t n);
    
    // Performance
    static size_t getVectorWidth();
    static std::string getCapabilities();
    
private:
    SIMDVectorization(const SIMDVectorization&) = delete;
    SIMDVectorization& operator=(const SIMDVectorization&) = delete;
};

} // namespace nlm
