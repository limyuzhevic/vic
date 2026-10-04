#!/usr/bin/env cpp
// Example demonstrating the comprehensive performance monitoring system
// This example shows how to use all the new performance analysis tools

#include "src/performance/PerformanceProfiler.hpp"
#include "src/performance/FunctionCallProfiler.hpp"
#include "src/performance/AlgorithmProfiler.hpp"
#include "src/performance/BottleneckDetector.hpp"
#include "src/performance/MemoryProfiler.hpp"
#include "src/performance/ResourceMonitor.hpp"
#include "src/performance/ScalingAnalyzer.hpp"
#include "src/performance/ParallelPerformanceAnalyzer.hpp"
#include "src/performance/VectorizationAnalyzer.hpp"
#include "src/performance/CacheAnalyzer.hpp"
#include "src/performance/MemoryAccessPatternAnalyzer.hpp"
#include "src/performance/OptimizationRecommender.hpp"
#include "src/debug/DebuggingSystem.hpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <string>

// Example operations to profile
void exampleNeuralNetworkOperation(int size) {
    // Simulate a neural network operation
    volatile double result = 0.0;
    for (int i = 0; i < size; ++i) {
        result += i * 0.5;
        result /= (i + 1);
    }
    std::cout << "Neural network operation result: " << result << std::endl;
}

void exampleMatrixMultiplication(int n) {
    // Simulate matrix multiplication
    volatile double result = 0.0;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            result += i * j;
        }
    }
    std::cout << "Matrix multiplication result: " << result << std::endl;
}

void exampleMemoryAllocation(size_t count) {
    // Simulate memory allocation
    std::vector<double*> allocations;
    for (size_t i = 0; i < count; ++i) {
        allocations.push_back(new double(i * 1.5));
    }
    
    // Simulate usage
    volatile double sum = 0.0;
    for (size_t i = 0; i < count; ++i) {
        sum += *allocations[i];
    }
    std::cout << "Memory allocation sum: " << sum << std::endl;
    
    // Clean up
    for (auto ptr : allocations) {
        delete ptr;
    }
}

int main() {
    std::cout << "=== Comprehensive Performance Monitoring Example ===\n\n";
    
    // Create performance monitoring components
    auto debuggingSystem = std::make_shared<nlm::DebuggingSystem>();
    auto performanceProfiler = std::make_shared<nlm::PerformanceProfiler>(debuggingSystem);
    auto functionCallProfiler = std::make_shared<nlm::FunctionCallProfiler>();
    auto algorithmProfiler = std::make_shared<nlm::AlgorithmProfiler>();
    auto bottleneckDetector = std::make_shared<nlm::BottleneckDetector>(performanceProfiler);
    auto memoryProfiler = std::make_shared<nlm::MemoryProfiler>(debuggingSystem);
    auto resourceMonitor = std::make_shared<nlm::ResourceMonitor>();
    auto scalingAnalyzer = std::make_shared<nlm::ScalingAnalyzer>();
    auto parallelPerformanceAnalyzer = std::make_shared<nlm::ParallelPerformanceAnalyzer>();
    auto vectorizationAnalyzer = std::make_shared<nlm::VectorizationAnalyzer>();
    auto cacheAnalyzer = std::make_shared<nlm::CacheAnalyzer>();
    auto memoryAccessPatternAnalyzer = std::make_shared<nlm::MemoryAccessPatternAnalyzer>();
    auto optimizationRecommender = std::make_shared<nlm::OptimizationRecommender>();
    
    // Integrate all components
    performanceProfiler->setFunctionCallProfiler(functionCallProfiler);
    performanceProfiler->setAlgorithmProfiler(algorithmProfiler);
    performanceProfiler->setBottleneckDetector(bottleneckDetector);
    
    bottleneckDetector->setPerformanceProfiler(performanceProfiler);
    bottleneckDetector->setFunctionCallProfiler(functionCallProfiler);
    bottleneckDetector->setAlgorithmProfiler(algorithmProfiler);
    
    optimizationRecommender->setPerformanceProfiler(performanceProfiler);
    optimizationRecommender->setFunctionCallProfiler(functionCallProfiler);
    optimizationRecommender->setAlgorithmProfiler(algorithmProfiler);
    optimizationRecommender->setBottleneckDetector(bottleneckDetector);
    optimizationRecommender->setMemoryProfiler(memoryProfiler);
    optimizationRecommender->setResourceMonitor(resourceMonitor);
    optimizationRecommender->setScalingAnalyzer(scalingAnalyzer);
    optimizationRecommender->setParallelPerformanceAnalyzer(parallelPerformanceAnalyzer);
    optimizationRecommender->setVectorizationAnalyzer(vectorizationAnalyzer);
    optimizationRecommender->setCacheAnalyzer(cacheAnalyzer);
    optimizationRecommender->setMemoryAccessPatternAnalyzer(memoryAccessPatternAnalyzer);
    
    // Start resource monitoring
    resourceMonitor->start();
    
    // Start performance measurements
    std::cout << "Starting performance monitoring...\n\n";
    
    // Example 1: Neural network operations
    std::cout << "--- Example 1: Neural Network Operations ---\n";
    {
        auto measurement = performanceProfiler->startMeasurement("neural_network", "example_neural");
        exampleNeuralNetworkOperation(1000);
        performanceProfiler->stopMeasurement(measurement);
        
        performanceProfiler->recordFunctionCall("exampleNeuralNetworkOperation", 0.1, "neural_network");
        algorithmProfiler->recordAlgorithmOperation("neural_network", "forward_pass", 0.15, "input_size=1000");
    }
    
    // Example 2: Matrix multiplication
    std::cout << "--- Example 2: Matrix Multiplication ---\n";
    {
        auto measurement = performanceProfiler->startMeasurement("matrix_multiplication", "example_matrix");
        exampleMatrixMultiplication(100);
        performanceProfiler->stopMeasurement(measurement);
        
        performanceProfiler->recordFunctionCall("exampleMatrixMultiplication", 0.5, "matrix_multiplication");
        algorithmProfiler->recordAlgorithmOperation("matrix_multiplication", "multiply", 0.6, "matrix_size=100");
    }
    
    // Example 3: Memory allocation
    std::cout << "--- Example 3: Memory Allocation ---\n";
    {
        auto measurement = performanceProfiler->startMeasurement("memory_allocation", "example_memory");
        exampleMemoryAllocation(1000);
        performanceProfiler->stopMeasurement(measurement);
        
        performanceProfiler->recordFunctionCall("exampleMemoryAllocation", 0.3, "memory_allocation");
        algorithmProfiler->recordAlgorithmOperation("memory_allocation", "allocate", 0.2, "allocation_count=1000");
    }
    
    // Record resource usage
    performanceProfiler->recordResourceUsage(75.5, 512.0, 150, 8);
    
    // Take memory snapshots
    memoryProfiler->takeSnapshot();
    
    // Simulate scaling analysis
    scalingAnalyzer->recordScaleMeasurement(1, "neural_network", 1.0, 512.0, 1, 10.0);
    scalingAnalyzer->recordScaleMeasurement(2, "neural_network", 0.6, 256.0, 2, 15.0);
    scalingAnalyzer->recordScaleMeasurement(4, "neural_network", 0.4, 128.0, 4, 20.0);
    
    // Simulate parallel performance
    parallelPerformanceAnalyzer->recordParallelExecution("matrix_multiplication", 1, 1.0, 0.8, 0.9, 0.1, "parallel_example");
    parallelPerformanceAnalyzer->recordParallelExecution("matrix_multiplication", 2, 0.6, 0.9, 0.8, 0.2, "parallel_example");
    parallelPerformanceAnalyzer->recordParallelExecution("matrix_multiplication", 4, 0.4, 0.85, 0.7, 0.3, "parallel_example");
    
    // Simulate vectorization analysis
    vectorizationAnalyzer->recordVectorizationMeasurement("matrix_multiplication", 8, 1.0, 0.3, 80.0, 90.0, "vector_example");
    vectorizationAnalyzer->recordVectorizationMeasurement("matrix_multiplication", 4, 1.0, 0.5, 60.0, 70.0, "vector_example");
    
    // Simulate cache analysis
    cacheAnalyzer->recordCacheMeasurement("matrix_multiplication", 1, 0.001, 0.95, 10000, 100, 50, "cache_example");
    cacheAnalyzer->recordCacheMeasurement("matrix_multiplication", 2, 0.01, 0.85, 10000, 500, 200, "cache_example");
    
    // Simulate memory access pattern analysis
    memoryAccessPatternAnalyzer->recordMemoryAccess("matrix_multiplication", 4096, 0.005, true, 64, "sequential", 0.9, 0.8, "memory_example");
    memoryAccessPatternAnalyzer->recordMemoryAccess("matrix_multiplication", 4096, 0.01, false, 64, "random", 0.3, 0.4, "memory_example");
    
    // Wait for a moment to collect more data
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    // Stop resource monitoring
    resourceMonitor->stop();
    
    // Generate comprehensive reports
    std::cout << "\n=== Performance Analysis Reports ===\n\n";
    
    // Performance profiler report
    std::cout << "1. Performance Profiler Report:\n";
    std::cout << performanceProfiler->exportProfileAsString() << "\n";
    
    // Bottleneck detector report
    std::cout << "2. Bottleneck Detector Report:\n";
    std::cout << bottleneckDetector->getBottleneckReport() << "\n";
    
    // Function call profiler report
    std::cout << "3. Function Call Profiler Report:\n";
    std::cout << functionCallProfiler->exportProfileAsString() << "\n";
    
    // Algorithm profiler report
    std::cout << "4. Algorithm Profiler Report:\n";
    std::cout << algorithmProfiler->exportProfileAsString() << "\n";
    
    // Memory profiler report
    std::cout << "5. Memory Profiler Report:\n";
    std::cout << memoryProfiler->getLeakReport() << "\n";
    
    // Resource monitor report
    std::cout << "6. Resource Monitor Report:\n";
    std::cout << resourceMonitor->getResourceReport() << "\n";
    
    // Scaling analyzer report
    std::cout << "7. Scaling Analyzer Report:\n";
    std::cout << scalingAnalyzer->analyzeScalingBehavior() << "\n";
    
    // Parallel performance analyzer report
    std::cout << "8. Parallel Performance Analyzer Report:\n";
    std::cout << parallelPerformanceAnalyzer->analyzeParallelEfficiency() << "\n";
    
    // Vectorization analyzer report
    std::cout << "9. Vectorization Analyzer Report:\n";
    std::cout << vectorizationAnalyzer->analyzeVectorizationEfficiency() << "\n";
    
    // Cache analyzer report
    std::cout << "10. Cache Analyzer Report:\n";
    std::cout << cacheAnalyzer->analyzeCachePerformance() << "\n";
    
    // Memory access pattern analyzer report
    std::cout << "11. Memory Access Pattern Analyzer Report:\n";
    std::cout << memoryAccessPatternAnalyzer->analyzeMemoryAccessPatterns() << "\n";
    
    // Optimization recommendations
    std::cout << "12. Optimization Recommendations:\n";
    auto recommendations = optimizationRecommender->generateRecommendations();
    for (size_t i = 0; i < recommendations.size(); ++i) {
        std::cout << i + 1 << ". " << recommendations[i] << "\n";
    }
    
    // Comprehensive performance report
    std::cout << "\n13. Comprehensive Performance Report:\n";
    std::cout << performanceProfiler->getComprehensivePerformanceReport() << "\n";
    
    // Optimization recommendation report
    std::cout << "\n14. Optimization Recommendation Report:\n";
    std::cout << optimizationRecommender->generateOptimizationReport() << "\n";
    
    // Export all profiles to files
    std::cout << "\n=== Exporting Profiles to Files ===\n";
    performanceProfiler->exportProfile("performance_profile.txt");
    bottleneckDetector->getBottleneckReport();
    functionCallProfiler->exportProfile("function_call_profile.txt");
    algorithmProfiler->exportProfile("algorithm_profile.txt");
    memoryProfiler->exportMemoryProfile("memory_profile.txt");
    resourceMonitor->exportResourceProfile("resource_profile.txt");
    scalingAnalyzer->exportScalingProfile("scaling_profile.txt");
    parallelPerformanceAnalyzer->exportParallelProfile("parallel_profile.txt");
    vectorizationAnalyzer->exportVectorizationProfile("vectorization_profile.txt");
    cacheAnalyzer->exportCacheProfile("cache_profile.txt");
    memoryAccessPatternAnalyzer->exportAccessPatternProfile("memory_access_pattern_profile.txt");
    optimizationRecommender->exportRecommendations("optimization_recommendations.txt");
    
    std::cout << "\n=== Performance Monitoring Complete ===\n";
    std::cout << "All performance metrics have been collected and analyzed.\n";
    std::cout << "Reports have been generated and exported to files.\n";
    
    return 0;
}
