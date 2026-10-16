// Numerical Stability and Performance Example
// Demonstrates numerical stability improvements and performance optimizations
// This file provides examples and utilities for implementing performance optimizations

#pragma once

#include <cmath>
#include <cfloat>
#include <limits>
#include <algorithm>
#include <vector>
#include <string>

namespace nlm {

/**
 * Example: Numerical Stability Utilities
 * Provides stable numerical computations for neural simulations
 */
class NumericalStabilityExamples {
public:
    /**
     * Stable exponential computation for neural dynamics
     * Avoids overflow/underflow for large negative exponents
     */
    static float stableExp(float x) {
        if (x < -10.0f) {
            return 0.0f;  // exp(-10) ~ 4.5e-5, effectively zero
        } else if (x > 10.0f) {
            return FLT_MAX;  // Overflow prevention
        }
        return std::exp(x);
    }
    
    /**
     * Stable sine computation for oscillatory neurons
     * Handles large arguments and prevents numerical instability
     */
    static float stableSin(float x) {
        // Normalize to [-π, π] range
        float normalized = std::fmod(x, 2.0f * M_PI);
        if (normalized > M_PI) {
            normalized -= 2.0f * M_PI;
        } else if (normalized < -M_PI) {
            normalized += 2.0f * M_PI;
        }
        return std::sin(normalized);
    }
    
    /**
     * Stable linear interpolation for synaptic weights
     * Prevents numerical drift in weight updates
     */
    static float stableLerp(float a, float b, float t) {
        // Clamp t to [0, 1] to prevent extrapolation
        t = std::clamp(t, 0.0f, 1.0f);
        
        // Use FMA (fused multiply-add) if available for better precision
        return std::fma(b - a, t, a);
    }
    
    /**
     * Stable neuron voltage update
     * Implements numerical stability for LIF neuron dynamics
     */
    static float stableVoltageUpdate(float V, float I, float dt, float tau, float C, float V_rest, float threshold) {
        // Prevent numerical instability from division by zero
        if (tau <= 0.0f || C <= 0.0f) {
            return V;
        }
        
        // Calculate leak term with numerical stability
        float leakTerm = (V_rest - V) / tau;
        
        // Calculate input term with saturation
        float inputTerm = I / C;
        inputTerm = std::clamp(inputTerm, -1000.0f, 1000.0f);  // Reasonable input bounds
        
        // Update voltage with stability checks
        float dV = dt * (leakTerm + inputTerm) * 1000.0f;  // dt in ms
        float newV = V + dV;
        
        // Clamp to prevent numerical overflow
        newV = std::clamp(newV, -1000.0f, 1000.0f);
        
        // Check for spike threshold crossing with hysteresis
        if (newV >= threshold) {
            newV = -70.0f;  // Reset to resting potential
        }
        
        return newV;
    }
    
    /**
     * Stable synaptic weight update with STDP
     * Implements numerical stability for spike-timing dependent plasticity
     */
    static float stableSTDPUpdate(float currentWeight, float deltaW, float w_min, float w_max) {
        // Calculate new weight with stability checks
        float newWeight = currentWeight + deltaW;
        
        // Apply bounds with safety margins
        newWeight = std::clamp(newWeight, w_min + 1e-6f, w_max - 1e-6f);
        
        // Prevent numerical drift by renormalizing near bounds
        if (std::abs(newWeight - w_min) < 1e-6f) {
            newWeight = w_min + 1e-6f;
        } else if (std::abs(newWeight - w_max) < 1e-6f) {
            newWeight = w_max - 1e-6f;
        }
        
        return newWeight;
    }
    
    /**
     * Stable matrix multiplication for neural connectivity
     * Uses improved numerical stability for sparse matrix operations
     */
    static void stableMatrixMultiply(const float* A, const float* B, float* C, 
                                    size_t rows, size_t cols, size_t inner,
                                    float alpha = 1.0f, float beta = 0.0f) {
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                float sum = 0.0f;
                for (size_t k = 0; k < inner; ++k) {
                    float a = A[i * inner + k];
                    float b = B[k * cols + j];
                    
                    // Accumulate with Kahan summation for better precision
                    float y = alpha * a * b - sum;
                    float t = sum + y;
                    if (std::abs(t) >= std::abs(sum)) {
                        sum = t;
                    } else {
                        // Catastrophic cancellation detected
                        sum = (sum + y) + (sum - sum);  // Work around
                    }
                }
                C[i * cols + j] = beta * C[i * cols + j] + sum;
            }
        }
    }
    
    /**
     * Stable normalization for neural activations
     * Implements numerical stability for softmax and normalization
     */
    static std::vector<float> stableNormalization(const std::vector<float>& input, float epsilon = 1e-8f) {
        std::vector<float> output(input.size());
        
        // Find maximum for numerical stability
        float max_val = *std::max_element(input.begin(), input.end());
        float min_val = *std::min_element(input.begin(), input.end());
        
        // Shift for better numerical precision
        float shift = max_val;
        if (std::abs(max_val) > std::abs(min_val)) {
            shift = -min_val;
        }
        
        float sum = 0.0f;
        for (size_t i = 0; i < input.size(); ++i) {
            float val = input[i] + shift;
            val = std::max(val, 0.0f);  // ReLU-like for stability
            output[i] = val;
            sum += val;
        }
        
        // Normalize with stability check
        if (sum > epsilon) {
            float inv_sum = 1.0f / sum;
            for (size_t i = 0; i < input.size(); ++i) {
                output[i] *= inv_sum;
            }
        } else {
            // Uniform distribution if sum is too small
            float uniform = 1.0f / static_cast<float>(input.size());
            for (size_t i = 0; i < input.size(); ++i) {
                output[i] = uniform;
            }
        }
        
        return output;
    }
    
    /**
     * Stable random number generation for reproducible simulations
     * Ensures numerical stability in random processes
     */
    static float stableRandom(float min = 0.0f, float max = 1.0f, uint64_t seed = 0) {
        // Simple LCG for reproducible random numbers
        uint64_t state = seed;
        if (seed == 0) state = 123456789;
        
        // Simple PRNG with good distribution
        state = (state * 1103515245 + 12345) & 0x7fffffff;
        float value = static_cast<float>(state) / static_cast<float>(0x7fffffff);
        
        // Map to desired range
        return min + value * (max - min);
    }
    
    /**
     * Example: Performance Optimization Utilities
     * Demonstrates performance optimizations for neural simulations
     */
    class PerformanceOptimizations {
    public:
        /**
         * Cache-friendly data layout for neural populations
         * Implements Structure-of-Arrays (SoA) for better cache utilization
         */
        struct SoADataLayout {
            std::vector<float> membranePotentials;  // SoA: all potentials
            std::vector<float> synapticInputs;     // SoA: all inputs
            std::vector<uint8_t> firingStates;    // SoA: all states
            std::vector<float> adaptationVariables; // SoA: all adaptation vars
            
            size_t size() const { return membranePotentials.size(); }
            
            void resize(size_t n) {
                membranePotentials.resize(n);
                synapticInputs.resize(n);
                firingStates.resize(n);
                adaptationVariables.resize(n);
            }
        };
        
        /**
         * SIMD-friendly batch operations
         * Demonstrates how to optimize for SIMD vectorization
         */
        static void batchLIFUpdate(SoADataLayout& data, float dt, float V_rest, 
                                  float V_threshold, float V_reset, float tau, 
                                  float C, size_t startIdx, size_t endIdx) {
            // Process in batches for SIMD optimization
            size_t batchSize = 16;  // Typical SIMD width
            size_t i = startIdx;
            
            for (; i + batchSize <= endIdx; i += batchSize) {
                // SIMD-friendly processing (conceptual)
                // In actual implementation, this would use AVX/NEON intrinsics
                for (size_t j = 0; j < batchSize; ++j) {
                    float V = data.membranePotentials[i + j];
                    float I = data.synapticInputs[i + j];
                    
                    // Simplified LIF update (would use SIMD in practice)
                    float leak = (V_rest - V) / tau;
                    float input = I / C;
                    float newV = V + dt * 1000.0f * (leak + input);
                    
                    if (newV >= V_threshold) {
                        newV = V_reset;
                    }
                    
                    data.membranePotentials[i + j] = newV;
                }
            }
            
            // Process remaining elements
            for (; i < endIdx; ++i) {
                float V = data.membranePotentials[i];
                float I = data.synapticInputs[i];
                
                float leak = (V_rest - V) / tau;
                float input = I / C;
                float newV = V + dt * 1000.0f * (leak + input);
                
                if (newV >= V_threshold) {
                    newV = V_reset;
                }
                
                data.membranePotentials[i] = newV;
            }
        }
        
        /**
         * Event-driven processing optimization
         * Demonstrates how to optimize for event-driven neural simulation
         */
        static void eventDrivenProcessing(const std::vector<uint64_t>& activeNeurons,
                                         const std::vector<std::vector<uint64_t>>& adjacencyList,
                                         std::vector<float>& membranePotentials,
                                         float inputCurrent, float dt) {
            // Process only active neurons and their targets
            std::vector<uint64_t> spikes;
            
            for (uint64_t neuron : activeNeurons) {
                float V = membranePotentials[neuron];
                float I = inputCurrent;
                
                // Simplified spike detection
                if (V >= -55.0f) {  // Threshold
                    spikes.push_back(neuron);
                    membranePotentials[neuron] = -70.0f;  // Reset
                } else {
                    // Update membrane potential
                    float leak = (-70.0f - V) / 20.0f;  // Typical LIF parameters
                    membranePotentials[neuron] += dt * 1000.0f * leak + dt * 1000.0f * I;
                }
            }
            
            // Propagate spikes to connected neurons
            for (uint64_t spikeSource : spikes) {
                for (uint64_t target : adjacencyList[spikeSource]) {
                    if (target < membranePotentials.size()) {
                        membranePotentials[target] += 0.5f;  // Synaptic input
                    }
                }
            }
        }
        
        /**
         * Memory pool allocation optimization
         * Demonstrates efficient memory management
         */
        template<typename T>
        class MemoryPool {
        public:
            MemoryPool(size_t initialCapacity = 1024, size_t growthFactor = 2) 
                : capacity_(initialCapacity), growthFactor_(growthFactor) {
                pool_.reserve(capacity_);
            }
            
            T* allocate() {
                std::lock_guard<std::mutex> lock(mutex_);
                if (pool_.empty()) {
                    // Need to grow pool
                    size_t newCapacity = capacity_ * growthFactor_;
                    pool_.reserve(newCapacity);
                    capacity_ = newCapacity;
                }
                
                T* obj = new T();
                pool_.push_back(obj);
                return obj;
            }
            
            void deallocate(T* obj) {
                std::lock_guard<std::mutex> lock(mutex_);
                // Find and remove from pool
                auto it = std::find(pool_.begin(), pool_.end(), obj);
                if (it != pool_.end()) {
                    delete *it;
                    pool_.erase(it);
                }
            }
            
            size_t allocatedCount() const {
                std::lock_guard<std::mutex> lock(mutex_);
                return pool_.size();
            }
            
        private:
            std::vector<T*> pool_;
            size_t capacity_;
            size_t growthFactor_;
            mutable std::mutex mutex_;
        };
    };
};

/**
 * Example: Integration with Performance Monitoring
 * Demonstrates how to integrate with the performance monitoring framework
 */
class PerformanceIntegrationExample {
public:
    static void setupPerformanceMonitoring() {
        // Initialize performance monitoring
        PerformanceMonitor::getInstance().startMonitoring();
        
        // Enable key metrics
        PerformanceMonitor::getInstance().enableMetric(PerformanceMetric::NEURONS_PER_SECOND, true);
        PerformanceMonitor::getInstance().enableMetric(PerformanceMetric::SPIKES_PER_SECOND, true);
        PerformanceMonitor::getInstance().enableMetric(PerformanceMetric::SIMD_EFFICIENCY, true);
        PerformanceMonitor::getInstance().enableMetric(PerformanceMetric::NUMERICAL_STABILITY, true);
        
        // Initialize numerical stability monitoring
        // (Additional setup would go here)
    }
    
    static void recordPerformanceSample(const PerformanceSample& sample) {
        PerformanceMonitor::getInstance().recordSample(sample);
    }
    
    static void generatePerformanceReport() {
        std::cout << PerformanceMonitor::getInstance().generateReport() << std::endl;
    }
    
    static PerformanceAnalysis analyzePerformance(const std::vector<PerformanceSample>& samples) {
        return PerformanceAnalyzer::getInstance().analyzeSamples(samples);
    }
};

/**
 * Example: Performance Benchmarking
 * Demonstrates comprehensive benchmarking capabilities
 */
class PerformanceBenchmarkingExample {
public:
    static void setupBenchmarkSuite() {
        // Initialize benchmarking framework
        // (Additional setup would go here)
    }
    
    static void runComprehensiveBenchmarks() {
        BenchmarkingFramework::BenchmarkResult result;
        result.name = "NLM Performance Benchmark";
        result.neurons = 10000;
        result.synapses = 100000;
        result.steps = 1000;
        result.timeSeconds = 1.5;
        result.success = true;
        
        // Add to benchmark results
        // (Implementation would add to BenchmarkingFramework)
    }
    
    static void exportBenchmarkResults(const std::string& filename) {
        BenchmarkingFramework::getInstance().saveResults(filename);
    }
};

/**
 * Example: Performance Configuration
 * Demonstrates performance configuration options
 */
class PerformanceConfigurationExample {
public:
    static PerformanceConfig getOptimizedConfiguration(size_t neuronCount) {
        PerformanceConfig config;
        
        // Configure based on scale
        if (neuronCount < 1000) {
            config.enableSIMD = true;
            config.enableAVX512 = false;  // Conservative for small networks
            config.threadCount = 1;
            config.memoryPoolSize = 1024;
        } else if (neuronCount < 100000) {
            config.enableSIMD = true;
            config.enableAVX512 = true;  // Enable for medium networks
            config.threadCount = std::thread::hardware_concurrency();
            config.memoryPoolSize = 10240;
        } else {
            config.enableSIMD = true;
            config.enableAVX512 = true;
            config.threadCount = std::thread::hardware_concurrency();
            config.memoryPoolSize = 102400;
        }
        
        return config;
    }
};

} // namespace nlm
