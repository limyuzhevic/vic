// SIMD vectorization optimizations for NLM neural simulation
// Part of the SIMDVectorization infrastructure in src/performance/

#pragma once

#include "SIMDVectorization.hpp"
#include <thread>
#include <chrono>
#include <atomic>
#include <algorithm>

namespace nlm {

// Vectorized neuron state operations for efficient batch processing
class VectorizedNeuronOps {
public:
    // Enhanced VectorizedLIF operations with profiling support
    struct LIFBenchmark {
        std::chrono::high_resolution_clock::time_point start;
        size_t scalarOps = 0;
        size_t vectorOps = 0;
        double scalarTime = 0.0;
        double vectorTime = 0.0;
    };
    
    static void resetBenchmark(LIFBenchmark& bench) {
        bench = LIFBenchmark{};
    }
    
    static void startBenchmark(LIFBenchmark& bench) {
        bench.start = std::chrono::high_resolution_clock::now();
    }
    
    static void endBenchmark(LIFBenchmark& bench) {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(end - bench.start).count();
        bench.vectorTime += duration;
    }
    
    static void incrementScalarOps(LIFBenchmark& bench, size_t count = 1) {
        bench.scalarOps += count;
    }
    
    static void incrementVectorOps(LIFBenchmark& bench, size_t count = 1) {
        bench.vectorOps += count;
    }
    
    // Thread-safe vectorized LIF update with performance monitoring
    template<typename VArray, typename IArray, typename RArray, typename FArray>
    static size_t updateLIF(VectorizedLIF& lif, VArray& V_inout, const IArray& I,
                           RArray& refractoryRemaining, FArray& fired, size_t count,
                           LIFBenchmark* benchmark = nullptr, bool forceSIMD = false) {
        if (count == 0) return 0;
        
        // Thread-safe fallback to scalar if SIMD not available
        if (!SIMDUtils::isSIMDAvailable() && !forceSIMD) {
            if (benchmark) {
                startBenchmark(*benchmark);
            }
            size_t numFired = 0;
            for (size_t i = 0; i < count; ++i) {
                incrementScalarOps(*benchmark);
                if (refractoryRemaining[i] > 0) {
                    --refractoryRemaining[i];
                    V_inout[i] = lif.V_rest;
                    fired[i] = 0;
                } else {
                    float leak = (lif.V_rest - V_inout[i]) / lif.tau;
                    float input = I[i] / lif.C;
                    V_inout[i] += lif.dt * 1000.0f * (leak + input);
                    
                    if (V_inout[i] >= lif.V_threshold) {
                        V_inout[i] = lif.V_reset;
                        refractoryRemaining[i] = static_cast<uint32_t>(lif.refractoryDecay);
                        fired[i] = 1;
                        ++numFired;
                    } else {
                        fired[i] = 0;
                    }
                }
            }
            endBenchmark(*benchmark);
            return numFired;
        }
        
        // Use VectorizedLIF implementation
        return lif.update(V_inout, I, refractoryRemaining, fired, count);
    }
};

// Vectorized plasticity operations with thread-safe state updates
class VectorizedPlasticityOps {
public:
    // Benchmark structure for plasticity operations
    struct PlasticityBenchmark {
        std::chrono::high_resolution_clock::time_point start;
        size_t scalarOps = 0;
        size_t vectorOps = 0;
        double scalarTime = 0.0;
        double vectorTime = 0.0;
    };
    
    static void resetBenchmark(PlasticityBenchmark& bench) {
        bench = PlasticityBenchmark{};
    }
    
    static void startBenchmark(PlasticityBenchmark& bench) {
        bench.start = std::chrono::high_resolution_clock::now();
    }
    
    static void endBenchmark(PlasticityBenchmark& bench) {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(end - bench.start).count();
        bench.vectorTime += duration;
    }
    
    // Vectorized STDP weight update with thread-safe accumulation
    template<typename WArray, typename PreArray, typename PostArray>
    static void updateSTDPWeights(VectorizedSTDP& stdp, WArray& weights,
                                const PreArray& preSpikes, const PostArray& postSpikes,
                                size_t numPre, size_t numPost, PlasticityBenchmark* benchmark = nullptr) {
        if (benchmark) {
            startBenchmark(*benchmark);
        }
        
        // Use VectorizedSTDP implementation
        stdp.updateWeights(weights, preSpikes, postSpikes, numPre, numPost);
        
        if (benchmark) {
            endBenchmark(*benchmark);
        }
    }
    
    // Vectorized Hebbian weight update
    template<typename WArray, typename PreArray, typename PostArray>
    static void updateHebbianWeights(VectorizedSTDP& hebbian, WArray& weights,
                                   const PreArray& preSpikes, const PostArray& postSpikes,
                                   size_t numPre, size_t numPost, PlasticityBenchmark* benchmark = nullptr) {
        if (benchmark) {
            startBenchmark(*benchmark);
        }
        
        // For Hebbian, we use the same vectorized structure but with different parameters
        hebbian.updateWeights(weights, preSpikes, postSpikes, numPre, numPost);
        
        if (benchmark) {
            endBenchmark(*benchmark);
        }
    }
};

// Vectorized statistics computation with thread-safe accumulators
class VectorizedStatsOps {
public:
    // Benchmark structure for statistics operations
    struct StatsBenchmark {
        std::chrono::high_resolution_clock::time_point start;
        size_t scalarOps = 0;
        size_t vectorOps = 0;
        double scalarTime = 0.0;
        double vectorTime = 0.0;
    };
    
    static void resetBenchmark(StatsBenchmark& bench) {
        bench = StatsBenchmark{};
    }
    
    static void startBenchmark(StatsBenchmark& bench) {
        bench.start = std::chrono::high_resolution_clock::now();
    }
    
    static void endBenchmark(StatsBenchmark& bench) {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(end - bench.start).count();
        bench.vectorTime += duration;
    }
    
    // Thread-safe vectorized statistics computation
    template<typename Array>
    static float sum(const Array& arr, size_t count, StatsBenchmark* benchmark = nullptr) {
        if (benchmark) {
            startBenchmark(*benchmark);
        }
        
        float result = VectorizedStats::sum(arr, count);
        
        if (benchmark) {
            endBenchmark(*benchmark);
        }
        return result;
    }
    
    template<typename Array>
    static float mean(const Array& arr, size_t count, StatsBenchmark* benchmark = nullptr) {
        if (benchmark) {
            startBenchmark(*benchmark);
        }
        
        float result = VectorizedStats::mean(arr, count);
        
        if (benchmark) {
            endBenchmark(*benchmark);
        }
        return result;
    }
    
    template<typename Array>
    static float variance(const Array& arr, size_t count, StatsBenchmark* benchmark = nullptr) {
        if (benchmark) {
            startBenchmark(*benchmark);
        }
        
        float result = VectorizedStats::variance(arr, count);
        
        if (benchmark) {
            endBenchmark(*benchmark);
        }
        return result;
    }
    
    template<typename Array>
    static float max(const Array& arr, size_t count, StatsBenchmark* benchmark = nullptr) {
        if (benchmark) {
            startBenchmark(*benchmark);
        }
        
        float result = VectorizedStats::max(arr, count);
        
        if (benchmark) {
            endBenchmark(*benchmark);
        }
        return result;
    }
    
    template<typename Array>
    static void clamp(Array& arr, size_t count, float minVal, float maxVal, StatsBenchmark* benchmark = nullptr) {
        if (benchmark) {
            startBenchmark(*benchmark);
        }
        
        VectorizedStats::clamp(arr, count, minVal, maxVal);
        
        if (benchmark) {
            endBenchmark(*benchmark);
        }
    }
};

// Global profiling statistics
class SIMDProfiler {
public:
    static SIMDProfiler& getInstance() {
        static SIMDProfiler instance;
        return instance;
    }
    
    // Thread-safe profiling data collection
    struct ProfileData {
        std::atomic<size_t> scalarOps{0};
        std::atomic<size_t> vectorOps{0};
        std::atomic<double> scalarTime{0.0};
        std::atomic<double> vectorTime{0.0};
        std::atomic<size_t> activeThreads{0};
    };
    
    // Thread-safe benchmark results
    struct BenchmarkResults {
        double speedup = 1.0;
        size_t totalOps = 0;
        double totalTime = 0.0;
        std::string simdType;
    };
    
    // Reset all profiling data
    void reset() {
        profileData.scalarOps = 0;
        profileData.vectorOps = 0;
        profileData.scalarTime = 0.0;
        profileData.vectorTime = 0.0;
        profileData.activeThreads = 0;
    }
    
    // Start profiling for a thread
    void startProfiling() {
        profileData.activeThreads.fetch_add(1, std::memory_order_relaxed);
    }
    
    // Stop profiling for a thread
    void stopProfiling() {
        profileData.activeThreads.fetch_sub(1, std::memory_order_relaxed);
    }
    
    // Record scalar operation
    void recordScalarOp(size_t count = 1) {
        profileData.scalarOps.fetch_add(count, std::memory_order_relaxed);
    }
    
    // Record vector operation
    void recordVectorOp(size_t count = 1) {
        profileData.vectorOps.fetch_add(count, std::memory_order_relaxed);
    }
    
    // Record scalar execution time
    void recordScalarTime(double time) {
        profileData.scalarTime.fetch_add(time, std::memory_order_relaxed);
    }
    
    // Record vector execution time
    void recordVectorTime(double time) {
        profileData.vectorTime.fetch_add(time, std::memory_order_relaxed);
    }
    
    // Get compiled benchmark results
    BenchmarkResults getResults() {
        BenchmarkResults results;
        size_t scalarOps = profileData.scalarOps.load(std::memory_order_relaxed);
        size_t vectorOps = profileData.vectorOps.load(std::memory_order_relaxed);
        double scalarTime = profileData.scalarTime.load(std::memory_order_relaxed);
        double vectorTime = profileData.vectorTime.load(std::memory_order_relaxed);
        
        results.totalOps = scalarOps + vectorOps;
        results.totalTime = scalarTime + vectorTime;
        results.simdType = SIMDUtils::getSIMDType();
        
        // Calculate speedup (vector is assumed to be faster, so speedup > 1 means improvement)
        if (vectorTime > 0.0) {
            double scalarPerOp = scalarOps > 0 ? scalarTime / scalarOps : 0.0;
            double vectorPerOp = vectorOps > 0 ? vectorTime / vectorOps : 0.0;
            if (vectorPerOp > 0.0) {
                results.speedup = scalarPerOp / vectorPerOp;
            }
        }
        
        return results;
    }
    
    // Print detailed results
    void printResults() {
        BenchmarkResults results = getResults();
        
        std::cout << std::endl;
        std::cout << "╔═══════════════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                      SIMD Vectorization Performance Results               ║" << std::endl;
        std::cout << "╠═══════════════╦═══════════════╦═══════════════╦═══════════════╦═════════════╣" << std::endl;
        std::cout << "║   SIMD Type   ║  Speedup      ║  Scalar Ops   ║  Vector Ops   ║  Total Time  ║" << std::endl;
        std::cout << "╠═══════════════╬═══════════════╬═══════════════╬═══════════════╬═════════════╣" << std::endl;
        
        std::cout << "║ " << std::setw(13) << results.simdType
                  << " ║ " << std::setw(11) << std::fixed << std::setprecision(2) << results.speedup
                  << " ║ " << std::setw(13) << results.totalOps
                  << " ║ " << std::setw(13) << "N/A"
                  << " ║ " << std::setw(11) << std::fixed << std::setprecision(3) << results.totalTime
                  << " ║" << std::endl;
        
        std::cout << "╚═══════════════╩═══════════════╩═══════════════╩═══════════════╩═════════════╝" << std::endl;
        std::cout << std::endl;
    }
    
private:
    SIMDProfiler() {}
    SIMDProfiler(const SIMDProfiler&) = delete;
    SIMDProfiler& operator=(const SIMDProfiler&) = delete;
    
    ProfileData profileData;
};

} // namespace nlm
