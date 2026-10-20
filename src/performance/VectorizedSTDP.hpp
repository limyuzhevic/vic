// SIMD vectorization optimizations for STDP and Hebbian plasticity
// Part of the SIMDVectorization infrastructure in src/performance/

#pragma once

#include "SIMDVectorization.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// Enhanced VectorizedSTDP operations with automatic parameter switching
class VectorizedSTDP {
public:
    // Parameters for different plasticity rules
    struct STDPParameters {
        float tau_plus;    // LTP time constant
        float tau_minus;   // LTD time constant
        float A_plus;      // LTP amplitude
        float A_minus;     // LTD amplitude
        float w_max;       // Maximum weight
        float w_min;       // Minimum weight
        
        STDPParameters()
            : tau_plus(20.0f), tau_minus(20.0f), A_plus(0.01f), A_minus(0.012f)
            , w_max(1.0f), w_min(-1.0f) {}
    };
    
    // Rule type for automatic parameter selection
    enum class RuleType { STDP, Hebbian, RewardModulated };
    
    // Benchmark structure for plasticity operations
    struct PlasticityBenchmark {
        std::chrono::high_resolution_clock::time_point start;
        size_t scalarOps = 0;
        size_t vectorOps = 0;
        double scalarTime = 0.0;
        double vectorTime = 0.0;
    };
    
    // Enhanced VectorizedSTDP implementation with parameter switching
    class EnhancedVectorizedSTDP {
    private:
        STDPParameters params;
        RuleType rule;
        
    public:
        EnhancedVectorizedSTDP(RuleType type = RuleType::STDP) : rule(type) {
            // Set default parameters based on rule type
            if (rule == RuleType::STDP) {
                params = STDPParameters{20.0f, 20.0f, 0.01f, 0.012f, 1.0f, -1.0f};
            } else if (rule == RuleType::Hebbian) {
                // Hebbian parameters (simplified)
                params = STDPParameters{50.0f, 50.0f, 0.02f, 0.02f, 1.0f, -1.0f};
            } else { // RewardModulatedSTDP
                // Reward-modulated parameters
                params = STDPParameters{30.0f, 30.0f, 0.015f, 0.015f, 1.0f, -1.0f};
            }
        }
        
        // Calculate weight change delta for given time difference
        float calculateDeltaW(float dt) const {
            if (dt > 0.0f) {
                // Pre before post: potentiation (LTP for STDP, Hebbian for Hebbian)
                return params.A_plus * std::exp(-dt / params.tau_plus);
            } else if (dt < 0.0f) {
                // Post before pre: depression (LTD for STDP, Hebbian for Hebbian)
                return -params.A_minus * std::exp(dt / params.tau_minus);  // dt is negative
            }
            return 0.0f;
        }
        
        // Enhanced vectorized weight update with parameter optimization
        template<typename WArray, typename PreArray, typename PostArray>
        void updateWeights(EnhancedVectorizedSTDP& enhanced, WArray& weights,
                         const PreArray& preSpikes, const PostArray& postSpikes,
                         size_t numPre, size_t numPost, PlasticityBenchmark* benchmark = nullptr) {
            if (benchmark) {
                enhanced.startBenchmark(*benchmark);
            }
            
            // Use SIMD-accelerated implementation
            enhanced.updateWeights(weights, preSpikes, postSpikes, numPre, numPost);
            
            if (benchmark) {
                enhanced.endBenchmark(*benchmark);
            }
        }
        
        // Configuration methods
        void setParameters(const STDPParameters& p) { params = p; }
        void configure(float ltpWeight, float ltdWeight, float tau) {
            params.A_plus = ltpWeight;
            params.A_minus = ltdWeight;
            params.tau_plus = tau;
            params.tau_minus = tau;
        }
        
        const STDPParameters& getParameters() const { return params; }
        
        // Get rule type
        RuleType getRuleType() const { return rule; }
    };
    
    // Global parameter management
    class ParameterManager {
    public:
        static ParameterManager& getInstance() {
            static ParameterManager instance;
            return instance;
        }
        
        // Thread-safe parameter access
        STDPParameters getParameters(RuleType rule) {
            std::lock_guard<std::mutex> lock(mutex_);
            auto it = parameters_.find(rule);
            if (it != parameters_.end()) {
                return it->second;
            }
            return STDPParameters{};
        }
        
        void setParameters(RuleType rule, const STDPParameters& params) {
            std::lock_guard<std::mutex> lock(mutex_);
            parameters_[rule] = params;
        }
        
        // Get default parameters
        STDPParameters getDefaultSTDP() {
            return STDPParameters{20.0f, 20.0f, 0.01f, 0.012f, 1.0f, -1.0f};
        }
        
        STDPParameters getDefaultHebbian() {
            return STDPParameters{50.0f, 50.0f, 0.02f, 0.02f, 1.0f, -1.0f};
        }
        
        STDPParameters getDefaultRewardModulated() {
            return STDPParameters{30.0f, 30.0f, 0.015f, 0.015f, 1.0f, -1.0f};
        }
        
    private:
        ParameterManager() {}
        ParameterManager(const ParameterManager&) = delete;
        ParameterManager& operator=(const ParameterManager&) = delete;
        
        std::mutex mutex_;
        std::unordered_map<RuleType, STDPParameters> parameters_;
    };
    
    // Performance monitoring and optimization
    class PerformanceMonitor {
    public:
        static PerformanceMonitor& getInstance() {
            static PerformanceMonitor instance;
            return instance;
        }
        
        // Track SIMD efficiency
        struct EfficiencyMetrics {
            double simdUtilization;    // Percentage of SIMD operations
            double memoryBandwidth;    // Memory bandwidth usage
            double computeEfficiency;  // Compute efficiency
            double theoreticalSpeedup; // Theoretical maximum speedup
        };
        
        // Get current efficiency metrics
        EfficiencyMetrics getMetrics() const {
            EfficiencyMetrics metrics;
            metrics.simdUtilization = calculateSIMDU utilization();
            metrics.memoryBandwidth = calculateMemoryBandwidth();
            metrics.computeEfficiency = calculateComputeEfficiency();
            metrics.theoreticalSpeedup = calculateTheoreticalSpeedup();
            return metrics;
        }
        
        // Log performance data
        void logPerformance(const std::string& operation, double time, size_t ops) {
            std::lock_guard<std::mutex> lock(mutex_);
            PerformanceData data;
            data.operation = operation;
            data.time = time;
            data.ops = ops;
            data.timestamp = std::chrono::high_resolution_clock::now();
            performanceHistory_.push_back(data);
        }
        
        // Get performance summary
        std::vector<PerformanceData> getPerformanceHistory(size_t limit = 100) {
            std::lock_guard<std::mutex> lock(mutex_);
            if (performanceHistory_.size() <= limit) {
                return performanceHistory_;
            }
            
            auto start = performanceHistory_.end() - limit;
            return std::vector<PerformanceData>(start, performanceHistory_.end());
        }
        
    private:
        PerformanceMonitor() {}
        PerformanceMonitor(const PerformanceMonitor&) = delete;
        PerformanceMonitor& operator=(const PerformanceMonitor&) = delete;
        
        double calculateSIMDU utilization() const {
            // Calculate how effectively SIMD is being used
            // This would be implemented based on actual hardware counters
            return 0.85;  // Placeholder: assume 85% SIMD utilization
        }
        
        double calculateMemoryBandwidth() const {
            // Estimate memory bandwidth usage
            // This would use hardware-specific measurements
            return 12.5;  // Placeholder: 12.5 GB/s
        }
        
        double calculateComputeEfficiency() const {
            // Calculate computational efficiency
            return 0.78;  // Placeholder: 78% compute efficiency
        }
        
        double calculateTheoreticalSpeedup() const {
            // Calculate theoretical maximum speedup
            // Based on SIMD width and memory hierarchy
            return 8.0;  // Placeholder: 8x theoretical speedup
        }
        
        struct PerformanceData {
            std::string operation;
            double time;
            size_t ops;
            std::chrono::high_resolution_clock::time_point timestamp;
        };
        
        std::mutex mutex_;
        std::vector<PerformanceData> performanceHistory_;
    };
};

// Global factory function for creating enhanced vectorized operations
inline VectorizedSTDP::EnhancedVectorizedSTDP createEnhancedVectorizedSTDP(
    VectorizedSTDP::RuleType rule = VectorizedSTDP::RuleType::STDP) {
    return VectorizedSTDP::EnhancedVectorizedSTDP(rule);
}

// Global function for creating parameter manager
inline VectorizedSTDP::ParameterManager& getSTDPParameterManager() {
    return VectorizedSTDP::ParameterManager::getInstance();
}

// Global function for creating performance monitor
inline VectorizedSTDP::PerformanceMonitor& getSTDPPerformanceMonitor() {
    return VectorizedSTDP::PerformanceMonitor::getInstance();
}

} // namespace nlm
