#pragma once

/**
 * Enhanced SIMD Vectorization with Configuration Integration and Performance Monitoring
 * 
 * Advanced SIMD optimization system with configuration-driven settings and performance monitoring.
 * Integrates with performance configuration and provides adaptive SIMD optimization.
 * 
 * Features:
 * - Configuration-driven SIMD settings
 * - Performance-aware SIMD optimization
 * - Memory alignment optimization based on configuration
 * - Runtime SIMD capability detection
 * - Performance monitoring and statistics
 * - Adaptive SIMD width selection
 * - Integration with performance profiler
 * 
 * Biological motivation:
 * - Like neural systems that adapt synaptic strength based on activity
 * - SIMD optimization adapts based on computational demands
 * - Parallel processing optimization like neural parallelization
 */

#include <cstddef>
#include <cstdint>
#include <cmath>
#include <array>
#include <algorithm>
#include <type_traits>
#include <string>
#include <memory>
#include <atomic>
#include <chrono>
#include <functional>

namespace nlm {

/**
 * Enhanced SIMD configuration options
 */
enum class SIMDOptimizationLevel {
    None = 0,         // No SIMD
    Basic = 1,        // Basic SIMD with fallback
    Aggressive = 2,   // Aggressive SIMD optimization
    Maximum = 3       // Maximum SIMD optimization
};

/**
 * SIMD performance statistics
 */
struct SIMDPerformanceStats {
    uint64_t totalSIMDOperations;     // Total number of SIMD operations
    uint64_t totalScalarOperations;   // Total number of scalar operations
    uint64_t simdSpeedup;             // SIMD speedup factor
    float simdUtilization;           // SIMD utilization ratio (0.0 - 1.0)
    uint64_t instructionCacheMisses;  // Instruction cache misses
    uint64_t memoryLoadMisses;        // Memory load misses
    uint64_t computeCycles;           // Total compute cycles
    uint64_t totalCycles;             // Total cycles
    
    SIMDPerformanceStats() : totalSIMDOperations(0), totalScalarOperations(0),
                             simdSpeedup(0), simdUtilization(0.0f),
                             instructionCacheMisses(0), memoryLoadMisses(0),
                             computeCycles(0), totalCycles(0) {}
};

/**
 * SIMD vectorization configuration
 */
struct SIMDVectorizationConfig {
    // Optimization level
    SIMDOptimizationLevel optimizationLevel;        // SIMD optimization level
    SIMDInstructionSet simdInstructionSet;          // SIMD instruction set
    size_t simdAlignment;                           // Memory alignment for SIMD
    bool enableRuntimeDetection;                   // Enable runtime SIMD detection
    bool enableAutoVectorization;                   // Enable automatic vectorization
    size_t minVectorSize;                           // Minimum vector size for SIMD
    
    // Performance monitoring
    bool enableSIMDPerformanceStats;                // Enable SIMD performance statistics
    uint32_t simdStatsUpdateInterval;               // SIMD stats update interval (operations)
    
    // Advanced optimization
    bool enableCacheAwareOptimization;             // Enable cache-aware optimization
    bool enableMemoryBandwidthOptimization;        // Enable memory bandwidth optimization
    float simdHotnessThreshold;                    // SIMD hotness threshold (0.0 - 1.0)
    
    // Fallback and error handling
    bool enableScalarFallback;                     // Enable scalar fallback
    float simdFailureThreshold;                    // SIMD failure threshold
    
    // Integration with other systems
    bool integrateWithPerformanceProfiler;         // Integrate with performance profiler
    bool integrateWithMemoryPool;                   // Integrate with memory pool
    
    SIMDVectorizationConfig()
        : optimizationLevel(SIMDOptimizationLevel::Aggressive),
          simdInstructionSet(SIMDInstructionSet::Auto),
          simdAlignment(64),
          enableRuntimeDetection(true),
          enableAutoVectorization(true),
          minVectorSize(4),
          enableSIMDPerformanceStats(true),
          simdStatsUpdateInterval(1000),
          enableCacheAwareOptimization(true),
          enableMemoryBandwidthOptimization(true),
          simdHotnessThreshold(0.8f),
          enableScalarFallback(true),
          simdFailureThreshold(0.95f),
          integrateWithPerformanceProfiler(true),
          integrateWithMemoryPool(true) {}
    
    SIMDVectorizationConfig(SIMDOptimizationLevel level, SIMDInstructionSet set,
                           size_t alignment, bool runtimeDetect, bool autoVectorize)
        : optimizationLevel(level), simdInstructionSet(set), simdAlignment(alignment),
          enableRuntimeDetection(runtimeDetect), enableAutoVectorization(autoVectorize),
          minVectorSize(4), enableSIMDPerformanceStats(true), simdStatsUpdateInterval(1000),
          enableCacheAwareOptimization(true), enableMemoryBandwidthOptimization(true),
          simdHotnessThreshold(0.8f), enableScalarFallback(true), simdFailureThreshold(0.95f),
          integrateWithPerformanceProfiler(true), integrateWithMemoryPool(true) {}
};

/**
 * SIMD performance monitor
 */
class SIMDPerformanceMonitor {
public:
    SIMDPerformanceMonitor();
    ~SIMDPerformanceMonitor();
    
    /**
     * Record SIMD operation performance
     */
    void recordSIMDOperation(uint64_t operations, uint64_t cycles, 
                           uint64_t loadMisses, uint64_t instrMisses);
    
    /**
     * Record scalar operation performance
     */
    void recordScalarOperation(uint64_t operations, uint64_t cycles);
    
    /**
     * Get SIMD performance statistics
     */
    SIMDPerformanceStats getStats() const { return stats_; }
    
    /**
     * Get SIMD speedup factor
     */
    double getSpeedupFactor() const {
        if (stats_.totalScalarOperations == 0) return 1.0;
        return static_cast<double>(stats_.totalSIMDOperations) / 
               static_cast<double>(stats_.totalScalarOperations);
    }
    
    /**
     * Get SIMD utilization
     */
    float getSIMDUtilization() const { return stats_.simdUtilization; }
    
    /**
     * Reset statistics
     */
    void reset() {
        std::lock_guard<std::mutex> lock(mutex_);
        stats_ = SIMDPerformanceStats();
    }
    
    /**
     * Set stats update interval
     */
    void setUpdateInterval(uint32_t interval) { statsUpdateInterval_ = interval; }

private:
    mutable std::mutex mutex_;
    SIMDPerformanceStats stats_;
    uint32_t statsUpdateInterval_;
    uint64_t totalOperationsSinceUpdate_;
};

/**
 * Enhanced SIMD utilities with configuration and monitoring
 */
class EnhancedSIMDUtils {
public:
    EnhancedSIMDUtils();
    ~EnhancedSIMDUtils();
    
    /**
     * Configure SIMD settings
     */
    void configure(const SIMDVectorizationConfig& config) { config_ = config; }
    
    /**
     * Get current SIMD configuration
     */
    SIMDVectorizationConfig getConfig() const { return config_; }
    
    /**
     * Set performance monitor
     */
    void setPerformanceMonitor(SIMDPerformanceMonitor* monitor) { performanceMonitor_ = monitor; }
    
    /**
     * Configure SIMD based on runtime conditions
     */
    SIMDInstructionSet configureRuntimeSIMD() const;
    
    /**
     * Get optimal SIMD width based on configuration
     */
    static size_t getOptimalSIMDWidth(SIMDInstructionSet instructionSet);
    
    /**
     * Check if SIMD is available and configured
     */
    bool isSIMDEnabled() const {
        return config_.enableSIMD && 
               (SIMDInstructionSet::None != config_.simdInstructionSet);
    }
    
    /**
     * Get optimal memory alignment based on configuration
     */
    size_t getOptimalAlignment() const { return config_.simdAlignment; }
    
    /**
     * Check if cache-aware optimization is enabled
     */
    bool isCacheAwareOptimizationEnabled() const { return config_.enableCacheAwareOptimization; }
    
    /**
     * Check if memory bandwidth optimization is enabled
     */
    bool isMemoryBandwidthOptimizationEnabled() const { return config_.enableMemoryBandwidthOptimization; }
    
    /**
     * Get SIMD hotness threshold
     */
    float getSIMDHotnessThreshold() const { return config_.simdHotnessThreshold; }
    
    /**
     * Get minimum vector size for SIMD operations
     */
    size_t getMinVectorSize() const { return config_.minVectorSize; }
    
    /**
     * Check if scalar fallback is enabled
     */
    bool isScalarFallbackEnabled() const { return config_.enableScalarFallback; }
    
    /**
     * Check if SIMD failure threshold is met
     */
    bool checkSIMDFailure(float utilization) const {
        return utilization > config_.simdFailureThreshold;
    }
    
    /**
     * Get SIMD type name with configuration
     */
    static std::string getSIMDTypeName(SIMDInstructionSet instructionSet);
    
    /**
     * Get SIMD width for instruction set
     */
    static size_t getSIMDWidth(SIMDInstructionSet instructionSet);
    
    /**
     * Get SIMD performance statistics
     */
    SIMDPerformanceStats getPerformanceStats() const {
        if (performanceMonitor_) {
            return performanceMonitor_->getStats();
        }
        return SIMDPerformanceStats();
    }
    
    /**
     * Record operation in performance monitor
     */
    void recordOperation(uint64_t simdOps, uint64_t cycles, uint64_t loadMisses, 
                        uint64_t instrMisses, uint64_t scalarOps = 0);
    
    /**
     * Calculate SIMD efficiency
     */
    double calculateSIMDEfficiency(const SIMDPerformanceStats& stats) const;
    
    /**
     * Generate SIMD configuration report
     */
    std::string generateConfigReport() const;

private:
    SIMDVectorizationConfig config_;
    SIMDPerformanceMonitor* performanceMonitor_;
    mutable std::mutex mutex_;
};

/**
 * SIMD-accelerated neural computation functions
 */
namespace Vectorized {

/**
 * Vectorized Leaky Integrate-and-Fire (LIF) update
 * 
 * Enhanced SIMD implementation with configuration support and performance monitoring
 */
template<typename VArray, typename IArray, typename RArray, typename FArray>
size_t vectorizedLIFUpdate(VArray& V_inout, const IArray& I,
                         RArray& refractoryRemaining, FArray& fired,
                         size_t count, EnhancedSIMDUtils& simdUtils);

/**
 * Vectorized synaptic weight update
 * 
 * Enhanced SIMD implementation for synaptic plasticity
 */
template<typename WArray, typename PreArray, typename PostArray>
void vectorizedSTDPUpdate(WArray& weights, const PreArray& preSpikes,
                         const PostArray& postSpikes, size_t numPre, size_t numPost,
                         EnhancedSIMDUtils& simdUtils);

/**
 * Vectorized membrane potential calculation
 * 
 * Enhanced SIMD implementation for membrane potential updates
 */
template<typename VArray, typename IArray, typename RArray>
void vectorizedMembranePotential(VArray& V, const IArray& I,
                                 const RArray& refractory, size_t count,
                                 float V_rest, float V_reset, float threshold,
                                 float tau, float C, float dt, float refractoryDecay,
                                 EnhancedSIMDUtils& simdUtils);

/**
 * Vectorized synaptic efficacy update
 * 
 * Enhanced SIMD implementation for synaptic efficacy
 */
template<typename EArray, typename PreArray, typename PostArray>
void vectorizedSynapticEfficacy(EArray& efficacy, const PreArray& preSpikes,
                               const PostArray& postSpikes, float currentTime,
                               float tau_facilitation, float tau_depression, float U_max,
                               size_t count, EnhancedSIMDUtils& simdUtils);

/**
 * Vectorized statistics computation
 * 
 * Enhanced SIMD implementation for statistics calculation
 */
template<typename Array>
struct VectorizedStats {
    static float sum(const Array& arr, size_t count, EnhancedSIMDUtils& simdUtils);
    static float mean(const Array& arr, size_t count, EnhancedSIMDUtils& simdUtils);
    static float variance(const Array& arr, size_t count, EnhancedSIMDUtils& simdUtils);
    static float max(const Array& arr, size_t count, EnhancedSIMDUtils& simdUtils);
    static void clamp(Array& arr, size_t count, float minVal, float maxVal,
                     EnhancedSIMDUtils& simdUtils);
};

} // namespace Vectorized

// SIMD configuration macros with runtime detection
#define NLM_SIMD_AVX512_DETECTED (__AVX512F__ && __AVX512DQ__)
#define NLM_SIMD_AVX2_DETECTED (__AVX2__)
#define NLM_SIMD_SSE_DETECTED (__SSE4_1__ || __SSE3__)

// Enhanced SIMD configuration macros
#if defined(NLM_SIMD_AVX512_DETECTED) && defined(ENABLE_AVX512)
    #define NLM_ENHANCED_SIMD_AVX512 1
    #define NLM_ENHANCED_SIMD_WIDTH 16
    #define NLM_ENHANCED_SIMD_FLOAT_MUL_ADD _mm512_mul_ps
    #define NLM_ENHANCED_SIMD_FLOAT_ADD _mm512_add_ps
    #define NLM_ENHANCED_SIMD_FLOAT_STORE _mm512_storeu_ps
    #define NLM_ENHANCED_SIMD_FLOAT_LOAD _mm512_loadu_ps
    #define NLM_ENHANCED_SIMD_MASK_STORE _mm512_mask_storeu_ps
    #include <immintrin.h>
#elif defined(NLM_SIMD_AVX2_DETECTED) && defined(ENABLE_AVX2)
    #define NLM_ENHANCED_SIMD_AVX2 1
    #define NLM_ENHANCED_SIMD_WIDTH 8
    #define NLM_ENHANCED_SIMD_FLOAT_MUL_ADD _mm256_mul_ps
    #define NLM_ENHANCED_SIMD_FLOAT_ADD _mm256_add_ps
    #define NLM_ENHANCED_SIMD_FLOAT_STORE _mm256_storeu_ps
    #define NLM_ENHANCED_SIMD_FLOAT_LOAD _mm256_loadu_ps
    #include <immintrin.h>
#elif defined(NLM_SIMD_SSE_DETECTED) && defined(ENABLE_SSE)
    #define NLM_ENHANCED_SIMD_SSE 1
    #define NLM_ENHANCED_SIMD_WIDTH 4
    #define NLM_ENHANCED_SIMD_FLOAT_MUL_ADD _mm_mul_ps
    #define NLM_ENHANCED_SIMD_FLOAT_ADD _mm_add_ps
    #define NLM_ENHANCED_SIMD_FLOAT_STORE _mm_storeu_ps
    #define NLM_ENHANCED_SIMD_FLOAT_LOAD _mm_loadu_ps
    #include <xmmintrin.h>
    #include <pmmintrin.h>
#else
    #define NLM_ENHANCED_SIMD_SCALAR 1
    #define NLM_ENHANCED_SIMD_WIDTH 1
#endif

// Enhanced SIMD type detection
inline SIMDInstructionSet detectEnhancedSIMDType() {
#if defined(NLM_ENHANCED_SIMD_AVX512)
    return SIMDInstructionSet::AVX512;
#elif defined(NLM_ENHANCED_SIMD_AVX2)
    return SIMDInstructionSet::AVX2;
#elif defined(NLM_ENHANCED_SIMD_SSE)
    return SIMDInstructionSet::SSE;
#else
    return SIMDInstructionSet::None;
#endif
}

// Enhanced SIMD implementation includes
#include "EnhancedSIMDVectorizedLIF.hpp"
#include "EnhancedSIMDVectorizedSTDP.hpp"
#include "EnhancedSIMDVectorizedSynapticEfficacy.hpp"
#include "EnhancedSIMDVectorizedStats.hpp"

} // namespace nlm

// SIMD performance monitoring includes
#include "SIMDPerformanceMonitor.hpp"
#include "EnhancedSIMDUtils.hpp"