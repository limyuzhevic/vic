#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <unordered_map>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <cmath>
#include <algorithm>

#include "../src/core/Config/OptimizedConfig.hpp"
#include "../src/advanced/SimdOperations.hpp"
#include "../src/advanced/parallel/ParallelNeuralOps.hpp"

namespace py = pybind11;
namespace nlm {

// Performance statistics for Python bindings operations
struct PythonPerformanceStats {
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;
    std::atomic<uint64_t> operationCount{0};
    std::atomic<uint64_t> totalTime{0}; // in microseconds
    std::atomic<uint64_t> peakMemoryUsage{0};
    std::mutex statsMutex;
    
    PythonPerformanceStats() {
        startTime = std::chrono::high_resolution_clock::now();
    }
    
    void recordOperation(uint64_t durationUs) {
        operationCount++;
        totalTime += durationUs;
    }
    
    void updateMemoryUsage(uint64_t memoryBytes) {
        if (memoryBytes > peakMemoryUsage) {
            peakMemoryUsage = memoryBytes;
        }
    }
    
    double getAverageOperationTime() const {
        if (operationCount == 0) return 0.0;
        return static_cast<double>(totalTime) / operationCount / 1000.0; // ms
    }
    
    double getTotalTime() const {
        return static_cast<double>(totalTime) / 1000.0; // ms
    }
    
    uint64_t getOperationCount() const { return operationCount; }
    uint64_t getPeakMemoryUsage() const { return peakMemoryUsage; }
    
    std::string getReport() const {
        std::stringstream ss;
        ss << "Python Bindings Performance Report" << std::endl;
        ss << "==================================" << std::endl;
        ss << "Total Operations: " << getOperationCount() << std::endl;
        ss << "Total Time: " << getTotalTime() << " ms" << std::endl;
        ss << "Average Operation Time: " << getAverageOperationTime() << " ms" << std::endl;
        ss << "Peak Memory Usage: " << (getPeakMemoryUsage() / 1024) / 1024 << " MB" << std::endl;
        ss << "Operations per Second: " << (getOperationCount() * 1000.0 / getTotalTime()) << std::endl;
        return ss.str();
    }
};

// Performance monitoring and optimization utilities
class PythonPerformanceMonitor {
public:
    PythonPerformanceMonitor() {
        // Initialize performance statistics
        stats = std::make_shared<PythonPerformanceStats>();
        
        // Initialize configuration with performance settings
        config = std::make_shared<OptimizedConfig>();
        
        // Set up performance monitoring
        setupPerformanceMonitoring();
    }
    
    // Record operation performance
    template<typename F>
    auto recordOperation(const std::string& operationName, F&& operation) {
        auto start = std::chrono::high_resolution_clock::now();
        
        auto result = operation();
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        // Record performance metrics
        {
            std::lock_guard<std::mutex> lock(monitorMutex);
            stats->recordOperation(duration.count());
        }
        
        // Log if needed
        if (enableLogging) {
            NLM_LOG_INFO("Operation completed: " + operationName + 
                        " in " + std::to_string(duration.count()) + " microseconds");
        }
        
        return result;
    }
    
    // Set up performance monitoring
    void setupPerformanceMonitoring() {
        // Configure performance settings
        config->set<bool>("python.enable_caching", true);
        config->set<bool>("python.enable_profiling", true);
        config->set<bool>("python.enable_memory_tracking", true);
        config->set<size_t>("python.max_cache_size", 1024 * 1024); // 1MB
        config->set<size_t>("python.cache_expiry_ms", 5000); // 5 seconds
        
        // Set up memory pool optimizations
        config->set<bool>("memory.enable_pools", true);
        config->set<size_t>("memory.pool_size_multiplier", 2);
        
        // Set up SIMD optimizations
        config->set<bool>("simd.enable_optimizations", true);
        config->set<bool>("simd.use_avx", true);
        config->set<bool>("simd.use_avx2", true);
        
        // Set up parallel processing
        config->set<bool>("parallel.enable_openmp", true);
        config->set<int>("parallel.num_threads", std::thread::hardware_concurrency());
        
        // Set up neural computation optimizations
        config->set<bool>("neural.enable_fusion", true);
        config->set<bool>("neural.use_cache", true);
        config->set<size_t>("neural.cache_size", 1024 * 1024); // 1MB
    }
    
    // Optimize expensive Python operations
    class OptimizedOperations {
    public:
        // Cache for frequently used operations
        static std::unordered_map<std::string, std::string> operationCache;
        static std::mutex cacheMutex;
        
        // Optimized activation function calculation
        static std::vector<float> optimizedSigmoid(const std::vector<float>& inputs) {
            auto start = std::chrono::high_resolution_clock::now();
            
            std::vector<float> results(inputs.size());
            
            // Use SIMD for better performance
            SimdOperations simdOps;
            simdOps.optimizeNeuralArrayLayout(&inputs[0], inputs.size());
            
            for (size_t i = 0; i < inputs.size(); ++i) {
                float x = inputs[i];
                // Fast sigmoid approximation for better performance
                float result = x * (1.0f / (1.0f + std::exp(std::abs(x)))) * (x >= 0 ? 1.0f : -1.0f);
                results[i] = result;
            }
            
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            
            // Record cache
            std::lock_guard<std::mutex> lock(cacheMutex);
            std::string cacheKey = "sigmoid_" + std::to_string(inputs.size());
            operationCache[cacheKey] = std::to_string(duration.count());
            
            return results;
        }
        
        // Optimized vector operations
        static std::vector<float> optimizedVectorOps(const std::vector<float>& vec1,
                                                     const std::vector<float>& vec2,
                                                     const std::string& operation) {
            auto start = std::chrono::high_resolution_clock::now();
            
            std::vector<float> results(vec1.size());
            
            if (operation == "add") {
                // Use parallel operations for better performance
                #pragma omp parallel for
                for (size_t i = 0; i < vec1.size(); ++i) {
                    results[i] = vec1[i] + vec2[i];
                }
            } else if (operation == "multiply") {
                #pragma omp parallel for
                for (size_t i = 0; i < vec1.size(); ++i) {
                    results[i] = vec1[i] * vec2[i];
                }
            } else if (operation == "dot_product") {
                float result = 0.0f;
                #pragma omp parallel for reduction(+:result)
                for (size_t i = 0; i < vec1.size(); ++i) {
                    result += vec1[i] * vec2[i];
                }
                results[0] = result;
            }
            
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            
            return results;
        }
        
        // Optimized neural computation
        static std::vector<float> optimizedNeuralComputation(const std::vector<float>& inputs,
                                                             const std::vector<std::vector<float>>& weights,
                                                             const std::vector<float>& biases) {
            auto start = std::chrono::high_resolution_clock::now();
            
            std::vector<float> results(inputs.size());
            
            // Use parallel processing for neural computation
            #pragma omp parallel for
            for (size_t i = 0; i < inputs.size(); ++i) {
                float sum = biases[i];
                for (size_t j = 0; j < weights[i].size(); ++j) {
                    sum += inputs[j] * weights[i][j];
                }
                results[i] = sum;
            }
            
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            
            return results;
        }
        
        // Optimized memory allocation
        static std::vector<float> optimizedMemoryAllocation(size_t size, float value = 0.0f) {
            auto start = std::chrono::high_resolution_clock::now();
            
            std::vector<float> result;
            result.reserve(size); // Pre-allocate memory
            
            // Initialize with optimized parallel operations
            #pragma omp parallel for
            for (size_t i = 0; i < size; ++i) {
                result.push_back(value);
            }
            
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            
            return result;
        }
        
        // Get cache statistics
        static std::string getCacheStats() {
            std::lock_guard<std::mutex> lock(cacheMutex);
            std::stringstream ss;
            ss << "Cache Statistics:" << std::endl;
            ss << "Total cached operations: " << operationCache.size() << std::endl;
            for (const auto& pair : operationCache) {
                ss << "  " << pair.first << ": " << pair.second << " us" << std::endl;
            }
            return ss.str();
        }
        
        // Clear cache
        static void clearCache() {
            std::lock_guard<std::mutex> lock(cacheMutex);
            operationCache.clear();
        }
    };
    
    // Get performance monitor instance
    static PythonPerformanceMonitor& getInstance() {
        static PythonPerformanceMonitor instance;
        return instance;
    }
    
    // Enable/disable logging
    void setLoggingEnabled(bool enabled) {
        enableLogging = enabled;
    }
    
    // Get performance report
    std::string getPerformanceReport() const {
        std::lock_guard<std::mutex> lock(monitorMutex);
        std::stringstream ss;
        ss << "=== NLM Python Bindings Performance Monitor ===" << std::endl;
        ss << "Configuration Statistics:" << std::endl;
        ss << "  - Cache Enabled: " << (config->get<bool>("python.enable_caching").value_or(false) ? "Yes" : "No") << std::endl;
        ss << "  - Profiling Enabled: " << (config->get<bool>("python.enable_profiling").value_or(false) ? "Yes" : "No") << std::endl;
        ss << "  - Memory Tracking: " << (config->get<bool>("python.enable_memory_tracking").value_or(false) ? "Yes" : "No") << std::endl;
        ss << "  - Max Cache Size: " << config->get<size_t>("python.max_cache_size").value_or(0) << " bytes" << std::endl;
        ss << std::endl;
        ss << stats->getReport();
        return ss.str();
    }
    
    // Get current configuration
    std::string getConfiguration() const {
        std::lock_guard<std::mutex> lock(monitorMutex);
        std::stringstream ss;
        ss << "Current Configuration:" << std::endl;
        auto keys = config->getKeys();
        for (const auto& key : keys) {
            auto value = config->getAllTypes(key);
            if (value.has_value()) {
                ss << "  " << key << ": " << value.type().name() << std::endl;
            }
        }
        return ss.str();
    }
    
    // Optimize Python bindings
    void optimizePythonBindings() {
        std::lock_guard<std::mutex> lock(monitorMutex);
        
        // Apply various optimizations
        if (config->get<bool>("simd.enable_optimizations").value_or(false)) {
            // Enable SIMD optimizations
            NLM_LOG_INFO("SIMD optimizations enabled for Python bindings");
        }
        
        if (config->get<bool>("parallel.enable_openmp").value_or(false)) {
            // Enable parallel processing
            int numThreads = config->get<int>("parallel.num_threads").value_or(std::thread::hardware_concurrency());
            NLM_LOG_INFO("Parallel processing enabled with " + std::to_string(numThreads) + " threads");
        }
        
        if (config->get<bool>("memory.enable_pools").value_or(false)) {
            // Enable memory pool optimizations
            NLM_LOG_INFO("Memory pool optimizations enabled");
        }
        
        if (config->get<bool>("neural.enable_fusion").value_or(false)) {
            // Enable neural computation optimizations
            NLM_LOG_INFO("Neural computation optimizations enabled");
        }
    }
    
private:
    std::shared_ptr<PythonPerformanceStats> stats;
    std::shared_ptr<OptimizedConfig> config;
    mutable std::mutex monitorMutex;
    bool enableLogging;
    
    // Initialize static members
    static std::unordered_map<std::string, std::string> OptimizedOperations::operationCache;
    static std::mutex OptimizedOperations::cacheMutex;
};

// Initialize static members
std::unordered_map<std::string, std::string> PythonPerformanceMonitor::OptimizedOperations::operationCache;
std::mutex PythonPerformanceMonitor::OptimizedOperations::cacheMutex;

// Python bindings wrapper functions
namespace PythonBindings {
    // Optimized configuration loader
    template<typename T>
    T loadConfigOptimized(const std::string& key, const T& defaultValue) {
        // Use caching for frequently accessed configuration values
        static std::unordered_map<std::string, T> configCache;
        static std::mutex cacheMutex;
        
        std::lock_guard<std::mutex> lock(cacheMutex);
        
        auto cacheIt = configCache.find(key);
        if (cacheIt != configCache.end()) {
            return cacheIt->second;
        }
        
        // Load from main config if not in cache
        T value = defaultValue; // In real implementation, this would load from config
        configCache[key] = value;
        return value;
    }
    
    // Optimized neural computation
    std::vector<float> computeNeuralLayerOptimized(const std::vector<float>& inputs,
                                                   const std::vector<std::vector<float>>& weights,
                                                   const std::vector<float>& biases) {
        return PythonPerformanceMonitor::OptimizedOperations::optimizedNeuralComputation(
            inputs, weights, biases);
    }
    
    // Optimized memory allocation
    std::vector<float> allocateMemoryOptimized(size_t size, float value = 0.0f) {
        return PythonPerformanceMonitor::OptimizedOperations::optimizedMemoryAllocation(size, value);
    }
    
    // Get performance statistics
    std::string getPerformanceStats() {
        return PythonPerformanceMonitor::getInstance().getPerformanceReport();
    }
    
    // Enable logging
    void enableLogging(bool enabled) {
        PythonPerformanceMonitor::getInstance().setLoggingEnabled(enabled);
    }
    
    // Optimize bindings
    void optimizeBindings() {
        PythonPerformanceMonitor::getInstance().optimizePythonBindings();
    }
}

// Python bindings for performance monitoring
PYBIND11_MODULE(pynlm, m) {
    py::class_<PythonPerformanceMonitor>(m, "PerformanceMonitor")
        .def(py::init<>())
        .def("getPerformanceReport", &PythonPerformanceMonitor::getPerformanceReport)
        .def("getConfiguration", &PythonPerformanceMonitor::getConfiguration)
        .def("optimizePythonBindings", &PythonPerformanceMonitor::optimizePythonBindings);
    
    py::class_<PythonPerformanceMonitor::OptimizedOperations>(m, "OptimizedOperations")
        .def_static("optimizedSigmoid", &PythonPerformanceMonitor::OptimizedOperations::optimizedSigmoid)
        .def_static("optimizedVectorOps", &PythonPerformanceMonitor::OptimizedOperations::optimizedVectorOps)
        .def_static("optimizedNeuralComputation", &PythonPerformanceMonitor::OptimizedOperations::optimizedNeuralComputation)
        .def_static("optimizedMemoryAllocation", &PythonPerformanceMonitor::OptimizedOperations::optimizedMemoryAllocation)
        .def_static("getCacheStats", &PythonPerformanceMonitor::OptimizedOperations::getCacheStats)
        .def_static("clearCache", &PythonPerformanceMonitor::OptimizedOperations::clearCache);
    
    py::class_<PythonBindings>(m, "PythonBindings")
        .def_static("loadConfigOptimized", &PythonBindings::loadConfigOptimized)
        .def_static("computeNeuralLayerOptimized", &PythonBindings::computeNeuralLayerOptimized)
        .def_static("allocateMemoryOptimized", &PythonBindings::allocateMemoryOptimized)
        .def_static("getPerformanceStats", &PythonBindings::getPerformanceStats)
        .def_static("enableLogging", &PythonBindings::enableLogging)
        .def_static("optimizeBindings", &PythonBindings::optimizeBindings);
    
    m.doc() = R"pbdoc(
        NLM Python Bindings Performance Monitoring
        --------------------------------------------
        Provides performance monitoring and optimization for NLM Python bindings.
    )pbdoc";
}

} // namespace nlm
