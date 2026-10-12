#pragma once

/**
 * SIMDProcessor - SIMD vectorization for neural computation
 * 
 * Provides vectorized operations for neural simulation using:
 * - SSE (Streaming SIMD Extensions) - 4 floats
 * - AVX2 (Advanced Vector Extensions 2) - 8 floats  
 * - AVX-512 - 16 floats
 * - Fallback to scalar operations
 * 
 * Targets:
 * - Membrane potential updates (LIF dynamics)
 * - Synaptic integration (post-synaptic potentials)
 * - Plasticity calculations (STDP, Hebbian)
 * - Short-term plasticity
 * 
 * Features:
 * - Automatic detection of best SIMD width
 * - Portable high-level interface
 * - Scalar fallback for correctness
 * - Aligned memory operations
 * - Performance monitoring and metrics
 */

#include <cstddef>
#include <cstdint>
#include <cmath>
#include <algorithm>
#include <type_traits>
#include <immintrin.h>
#include <xmmintrin.h>
#include <pmmintrin.h>

namespace nlm {

// Forward declarations
class SIMDProcessor;

/**
 * SIMD configuration and feature detection
 */
struct SIMDFeatures {
    bool avx512 = false;           // AVX-512 support
    bool avx2 = false;             // AVX2 support
    bool sse = false;              // SSE support
    bool scalar = false;           // Scalar only
    
    size_t simdWidth = 1;          // Number of floats per SIMD register
    size_t registerSize = 16;      // Register size in bytes
    size_t cacheLineSize = 64;     // Cache line size
    
    size_t l1CacheSize = 32 * 1024;    // L1 cache size (32KB)
    size_t l2CacheSize = 256 * 1024;   // L2 cache size (256KB)
    size_t l3CacheSize = 8 * 1024 * 1024; // L3 cache size (8MB)
    
    double memoryBandwidthGBs = 25.6;  // Memory bandwidth
};

/**
 * SIMD processor for vectorized neural computation
 */
class SIMDProcessor {
public:
    /**
     * Create SIMD processor with automatic feature detection
     */
    SIMDProcessor();
    
    ~SIMDProcessor();
    
    // Disable copying
    SIMDProcessor(const SIMDProcessor&) = delete;
    SIMDProcessor& operator=(const SIMDProcessor&) = delete;
    
    /**
     * Update membrane potentials for a batch of neurons using SIMD
     * @param v_inout Array of membrane potentials (modified in place)
     * @param u Array of adaptation variables
     * @param I_ext Array of external inputs
     * @param I_syn Array of synaptic inputs
     * @param I_total Array for accumulated synaptic input (output)
     * @param restingPotential Resting membrane potential
     * @param threshold Firing threshold
     * @param tau Time constant
     * @param dt Time step
     * @param count Number of neurons
     */
    void updateNeuronBatch(float* v_inout, float* u, float* I_ext,
                          float* I_syn, float* I_total,
                          float restingPotential, float threshold,
                          float tau, float dt, size_t count);
    
    /**
     * Calculate synaptic input for neurons using SIMD
     * @param I_syn Array of synaptic inputs to accumulate
     * @param weights Array of synaptic weights
     * @param v_pre Array of pre-synaptic voltages
     * @param preNeurons Array of pre-synaptic neuron indices
     * @param connectionCount Number of connections
     */
    void calculateSynapticInput(float* I_syn, float* weights, float* v_pre,
                               NeuronId* preNeurons, size_t connectionCount);
    
    /**
     * Apply spike-timing dependent plasticity (STDP) using SIMD
     * @param weights Array of synaptic weights to update
     * @param v_pre Array of pre-synaptic spike voltages
     * @param v_post Array of post-synaptic spike voltages
     * @param t_pre Array of pre-synaptic spike times
     * @param t_post Array of post-synaptic spike times
     * @param dt Array of time differences
     * @param learningRate Array of learning rates
     * @param count Number of synapses
     */
    void applySTDPVector(float* weights, float* v_pre, float* v_post,
                        float* t_pre, float* t_post, float* dt,
                        float* learningRate, size_t count);
    
    /**
     * Apply Hebbian learning using SIMD
     * @param weights Array of synaptic weights to update
     * @param v_pre Array of pre-synaptic voltages
     * @param v_post Array of post-synaptic voltages
     * @param learningRate Array of learning rates
     * @param count Number of synapses
     */
    void applyHebbianVector(float* weights, float* v_pre, float* v_post,
                           float* learningRate, size_t count);
    
    /**
     * Check if memory is aligned for SIMD operations
     * @param ptr Memory pointer to check
     * @param alignment Required alignment
     * @return true if memory is properly aligned
     */
    bool isAligned(void* ptr, size_t alignment = 32);
    
    /**
     * Allocate aligned memory for SIMD operations
     * @param size Memory size to allocate
     * @param alignment Memory alignment
     * @return Pointer to aligned memory
     */
    void* alignedAllocate(size_t size, size_t alignment = 32);
    
    /**
     * Deallocate aligned memory
     * @param ptr Pointer to memory to deallocate
     */
    void alignedDeallocate(void* ptr);
    
    // Configuration methods
    
    /**
     * Enable/disable SIMD instructions
     */
    void enableSIMD(bool enable);
    
    /**
     * Enable/disable vectorized LIF updates
     */
    void enableVectorizedLIF(bool enable);
    
    /**
     * Enable/disable vectorized synaptic calculations
     */
    void enableVectorizedSynaptic(bool enable);
    
    /**
     * Enable/disable vectorized plasticity calculations
     */
    void enableVectorizedPlasticity(bool enable);
    
    /**
     * Enable/disable vectorized Hebbian learning
     */
    void enableVectorizedHebbian(bool enable);
    
    /**
     * Enable/disable aligned memory allocation
     */
    void setAlignmentEnabled(bool enable);
    
    /**
     * Enable/disable SIMD alignment checks
     */
    void setSIMDAlignment(bool enable);
    
    // Performance monitoring
    
    /**
     * Get SIMD performance percentage
     */
    double getSIMDPerformance() const;
    
    /**
     * Get estimated instructions per cycle
     */
    size_t getInstructionsPerCycle() const;
    
    /**
     * Get number of SIMD instructions executed
     */
    size_t getSIMDInstructionsCount() const { return simdInstructionsCount_; }
    
    /**
     * Get number of scalar operations performed
     */
    size_t getScalarFallbackCount() const { return scalarFallbackCount_; }
    
    /**
     * Get peak SIMD memory usage
     */
    size_t getPeakSIMDUsage() const { return peakSIMDUsage_; }
    
private:
    /**
     * Detect available SIMD features
     */
    static SIMDFeatures detectSIMDFeatures();
    
    /**
     * Setup alignment optimizations
     */
    void setupAlignmentOptimizations();
    
    /**
     * Cleanup alignment structures
     */
    void clearAlignmentStructures();
    
    /**
     * Scalar fallback for neuron updates
     */
    static void updateNeuronScalar(float& v, float& u, float I_ext, float I_syn,
                                  float& I_total, float restingPotential, float threshold,
                                  float tau, float dt);
    
    /**
     * Scalar fallback for STDP
     */
    static void applySTDPScalar(float& weight, float v_pre, float v_post,
                               float t_pre, float t_post, float dt,
                               float learningRate);
    
    // SIMD helper functions for different instruction sets
    
    static __m512 calculateSTDPDeltaAVX512(__m512 dt, __m512 learningRate);
    static __m256 calculateSTDPDeltaAVX2(__m256 dt, __m256 learningRate);
    static __m128 calculateSTDPDeltaSSE(__m128 dt, __m128 learningRate);
    
    // Implementation functions for different SIMD instruction sets
    
    void updateNeuronBatchAVX512(float* v, float* u, float* I_ext,
                                float* I_syn, float* I_total,
                                float restingPotential, float threshold,
                                float tau, float dt, size_t count);
    
    void calculateSynapticInputAVX512(float* I_syn, float* weights, float* v_pre,
                                      NeuronId* preNeurons, size_t connectionCount);
    
    void applySTDPVectorAVX512(float* weights, float* v_pre, float* v_post,
                              float* t_pre, float* t_post, float* dt,
                              float* learningRate, size_t count);
    
    void applyHebbianVectorAVX512(float* weights, float* v_pre, float* v_post,
                                 float* learningRate, size_t count);
    
    void updateNeuronBatchAVX2(float* v, float* u, float* I_ext,
                              float* I_syn, float* I_total,
                              float restingPotential, float threshold,
                              float tau, float dt, size_t count);
    
    void calculateSynapticInputAVX2(float* I_syn, float* weights, float* v_pre,
                                    NeuronId* preNeurons, size_t connectionCount);
    
    void applySTDPVectorAVX2(float* weights, float* v_pre, float* v_post,
                            float* t_pre, float* t_post, float* dt,
                            float* learningRate, size_t count);
    
    void applyHebbianVectorAVX2(float* weights, float* v_pre, float* v_post,
                                float* learningRate, size_t count);
    
    void updateNeuronBatchSSE(float* v, float* u, float* I_ext,
                              float* I_syn, float* I_total,
                              float restingPotential, float threshold,
                              float tau, float dt, size_t count);
    
    void calculateSynapticInputSSE(float* I_syn, float* weights, float* v_pre,
                                   NeuronId* preNeurons, size_t connectionCount);
    
    void applySTDPVectorSSE(float* weights, float* v_pre, float* v_post,
                           float* t_pre, float* t_post, float* dt,
                           float* learningRate, size_t count);
    
    void applyHebbianVectorSSE(float* weights, float* v_pre, float* v_post,
                               float* learningRate, size_t count);
    
    // Friend declaration for event pool iterator
    friend class EventSystem;
    
    // SIMD features
    SIMDFeatures simdFeatures_;
    
    // Performance counters
    size_t simdInstructionsCount_;
    size_t scalarFallbackCount_;
    size_t allocationCount_;
    size_t deallocationCount_;
    size_t peakSIMDUsage_;
    
    // Configuration flags
    bool enableSIMDInstructions_;
    bool enableVectorizedLIF_;
    bool enableVectorizedSynaptic_;
    bool enableVectorizedPlasticity_;
    bool enableVectorizedHebbian_;
    bool enableAlignedAllocation_;
    bool enableSIMDAlignments_;
    
    // Alignment structures
    size_t alignSize_;
    std::vector<float*> alignmentBuffer_;
};

} // namespace nlm