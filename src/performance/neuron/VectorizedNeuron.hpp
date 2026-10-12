#pragma once

/**
 * VectorizedNeuron - SIMD-accelerated neuron updates
 * 
 * Provides vectorized operations for neural computation using SIMD instructions.
 * Designed for high-performance neural simulation with automatic SIMD detection.
 * 
 * Targets:
 * - Membrane potential updates
 * - Synaptic integration
 * - Refractory calculations
 * - Spike detection and reset
 * 
 * Features:
 * - Automatic SIMD detection (AVX-512, AVX2, SSE)
 * - Vectorized LIF (Leaky Integrate-and-Fire) updates
 * - Cache-efficient memory access patterns
 * - Automatic fallback to scalar operations
 * - Thread-safe for parallel processing
 */

#include "../SIMDVectorization.hpp"
#include "../../brain/Neuron.hpp"
#include <vector>
#include <cstddef>
#include <algorithm>
#include <cmath>
#include <atomic>

namespace nlm {

/**
 * VectorizedNeuron - SIMD-accelerated neuron state updates
 * 
 * Provides batch processing of multiple neurons using SIMD instructions.
 * Optimized for cache efficiency and parallel processing.
 */
class VectorizedNeuron {
public:
    VectorizedNeuron();
    ~VectorizedNeuron();
    
    /**
     * Update multiple neurons using SIMD vectorization
     * 
     * Performs batch updates of neuron states using SIMD instructions for
     * parallel processing of multiple neurons simultaneously.
     * 
     * @param neurons Vector of neuron pointers to update
     * @param count Number of neurons to update
     * @param dt Simulation timestep
     * @param currentTime Current simulation time
     * @return Vector of spike counts per SIMD lane
     */
    template<typename VArray, typename IArray, typename RArray, typename FArray>
    std::vector<uint32_t> updateNeurons(std::vector<Neuron*>& neurons, VArray& membranePotential,
                                       const IArray& synapticInput, RArray& refractoryRemaining,
                                       FArray& firingFlags, size_t count, float dt, float currentTime);
    
    /**
     * Get SIMD utilization statistics
     */
    struct SIMDStats {
        size_t vectorizedOperations = 0;
        size_t scalarFallbacks = 0;
        double averageSIMDWidth = 1.0;
        double efficiency = 100.0;
    };
    
    SIMDStats getSIMDStats() const { return simdStats_; }
    
    /**
     * Check if SIMD is available and enabled
     */
    static bool isSIMDAvailable() { return SIMDUtils::isSIMDAvailable(); }
    
    /**
     * Get current SIMD type
     */
    static const char* getSIMDType() { return SIMDUtils::getSIMDType(); }
    
    /**
     * Get optimal SIMD width for current hardware
     */
    static constexpr size_t getSIMDWidth() { return SIMDUtils::SIMD_WIDTH; }
    
    /**
     * Align memory for SIMD operations
     */
    template<typename T>
    static T* alignForSIMD(T* ptr, size_t alignment = SIMDUtils::alignment()) {
        uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
        size_t offset = alignment - (addr % alignment);
        if (offset == alignment) offset = 0;
        return reinterpret_cast<T*>(addr + offset);
    }
    
private:
    SIMDStats simdStats_;
    
    // SIMD processing functions
    template<typename VArray, typename IArray, typename RArray, typename FArray>
    std::vector<uint32_t> processWithSIMD(std::vector<Neuron*>& neurons, VArray& membranePotential,
                                         const IArray& synapticInput, RArray& refractoryRemaining,
                                         FArray& firingFlags, size_t count, float dt, float currentTime);
    
    template<typename VArray, typename IArray, typename RArray, typename FArray>
    std::vector<uint32_t> processScalar(std::vector<Neuron*>& neurons, VArray& membranePotential,
                                        const IArray& synapticInput, RArray& refractoryRemaining,
                                        FArray& firingFlags, size_t count, float dt, float currentTime);
    
    // SIMD LIF update implementations
    template<typename VArray, typename IArray, typename RArray, typename FArray>
    size_t vectorizedLIFUpdate(VArray& V_inout, const IArray& I, RArray& refractoryRemaining,
                              FArray& fired, size_t startIndex, size_t count, float dt, float currentTime);
    
    // Memory alignment helpers
    static bool isAligned(const void* ptr, size_t alignment) {
        return (reinterpret_cast<uintptr_t>(ptr) % alignment) == 0;
    }
    
    // Cache line alignment
    static constexpr size_t CACHE_LINE_SIZE = 64;
    
    // SIMD-specific constants
    static constexpr size_t MAX_SIMD_WIDTH = SIMDUtils::SIMD_WIDTH;
    static constexpr size_t MIN_SIMD_WIDTH = 1;
};

/**
 * VectorizedPlasticityRule - SIMD-accelerated plasticity updates
 * 
 * Provides vectorized operations for synaptic plasticity computations.
 * Optimized for STDP, Hebbian, and other plasticity rules.
 */
class VectorizedPlasticityRule {
public:
    VectorizedPlasticityRule();
    ~VectorizedPlasticityRule();
    
    /**
     * Update synaptic weights using vectorized plasticity rules
     * 
     * @param weights Weight array to update
     * @param preSpikeTimes Pre-synaptic spike times
     * @param postSpikeTimes Post-synaptic spike times
     * @param weightChange Array for weight changes
     * @param count Number of synapses
     * @param dt Simulation timestep
     */
    template<typename WArray, typename PreArray, typename PostArray, typename DeltaWArray>
    void updateWeights(WArray& weights, const PreArray& preSpikeTimes,
                     const PostArray& postSpikeTimes, DeltaWArray& weightChanges,
                     size_t numPre, size_t numPost, float dt);
    
    /**
     * Apply STDP (Spike-Timing-Dependent Plasticity) using SIMD
     */
    template<typename WArray, typename PreArray, typename PostArray>
    void applySTDP(WArray& weights, const PreArray& preSpikeTimes,
                  const PostArray& postSpikeTimes, size_t numPre, size_t numPost,
                  float tauPlus, float tauMinus, float APlus, float AMinus, float dt);
    
    /**
     * Apply Hebbian learning using SIMD
     */
    template<typename WArray, typename PreArray, typename PostArray>
    void applyHebbian(WArray& weights, const PreArray& preSpikeTimes,
                     const PostArray& postSpikeTimes, size_t numPre, size_t numPost,
                     float learningRate);
    
    /**
     * Get plasticity computation statistics
     */
    struct PlasticityStats {
        uint64_t stddpUpdates = 0;
        uint64_t hebbianUpdates = 0;
        uint64_t vectorizedOperations = 0;
        uint64_t scalarOperations = 0;
        double averageEfficiency = 100.0;
    };
    
    PlasticityStats getPlasticityStats() const { return plasticityStats_; }
    
private:
    PlasticityStats plasticityStats_;
    
    // Vectorized STDP implementation
    template<typename WArray, typename PreArray, typename PostArray>
    void vectorizedSTDPUUpdate(WArray& weights, const PreArray& preSpikeTimes,
                             const PostArray& postSpikeTimes, size_t numPre, size_t numPost,
                             float tauPlus, float tauMinus, float APlus, float AMinus, float dt);
    
    // Vectorized Hebbian implementation
    template<typename WArray, typename PreArray, typename PostArray>
    void vectorizedHebbianUpdate(WArray& weights, const PreArray& preSpikeTimes,
                               const PostArray& postSpikeTimes, size_t numPre, size_t numPost,
                               float learningRate);
};

/**
 * VectorizedStatistics - SIMD-accelerated statistical computations
 * 
 * Provides vectorized operations for statistical analysis of neural data.
 */
class VectorizedStatistics {
public:
    VectorizedStatistics();
    ~VectorizedStatistics();
    
    /**
     * Compute sum of array elements using SIMD
     */
    template<typename Array>
    static float sum(const Array& arr, size_t count);
    
    /**
     * Compute mean of array elements using SIMD
     */
    template<typename Array>
    static float mean(const Array& arr, size_t count);
    
    /**
     * Compute variance of array elements using SIMD
     */
    template<typename Array>
    static float variance(const Array& arr, size_t count, float mean);
    
    /**
     * Compute standard deviation using SIMD
     */
    template<typename Array>
    static float stddev(const Array& arr, size_t count);
    
    /**
     * Find maximum value using SIMD
     */
    template<typename Array>
    static float max(const Array& arr, size_t count);
    
    /**
     * Find minimum value using SIMD
     */
    template<typename Array>
    static float min(const Array& arr, size_t count);
    
    /**
     * Clamp array values to range using SIMD
     */
    template<typename Array>
    static void clamp(Array& arr, size_t count, float minVal, float maxVal);
    
    /**
     * Normalize array values to range [0,1] using SIMD
     */
    template<typename Array>
    static void normalize(Array& arr, size_t count, float minVal, float maxVal);
    
    /**
     * Calculate exponential moving average using SIMD
     */
    template<typename Array>
    static void exponentialMovingAverage(Array& arr, size_t count, float alpha);
    
    /**
     * Apply sigmoid function using SIMD
     */
    template<typename Array>
    static void sigmoid(Array& arr, size_t count);
    
    /**
     * Apply ReLU function using SIMD
     */
    template<typename Array>
    static void relu(Array& arr, size_t count, float alpha = 0.0f);
    
    /**
     * Calculate correlation between two arrays using SIMD
     */
    template<typename Array1, typename Array2>
    static float correlation(const Array1& arr1, const Array2& arr2, size_t count);
    
    /**
     * Calculate covariance between two arrays using SIMD
     */
    template<typename Array1, typename Array2>
    static float covariance(const Array1& arr1, const Array2& arr2, size_t count, float mean1, float mean2);
    
    /**
     * Calculate entropy of array using SIMD
     */
    template<typename Array>
    static float entropy(const Array& arr, size_t count);
    
    /**
     * Calculate mutual information between two arrays using SIMD
     */
    template<typename Array1, typename Array2>
    static float mutualInformation(const Array1& arr1, const Array2& arr2, size_t count);
    
    /**
     * Get SIMD computation statistics
     */
    struct SIMDStats {
        uint64_t simdOperations = 0;
        uint64_t scalarOperations = 0;
        double averageSIMDWidth = 1.0;
        double efficiency = 100.0;
        uint64_t memoryLoads = 0;
        uint64_t memoryStores = 0;
    };
    
    SIMDStats getSIMDStats() const { return simdStats_; }
    
private:
    SIMDStats simdStats_;
    
    // SIMD-specific helper functions
    template<typename Array>
    static void safeNormalize(Array& arr, size_t count, float minVal, float maxVal);
    
    template<typename Array>
    static float safeMax(const Array& arr, size_t count);
    
    template<typename Array>
    static float safeMin(const Array& arr, size_t count);
};

} // namespace nlm