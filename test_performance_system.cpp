//!/usr/bin/env g++
// Test program to verify NLM Performance Benchmarking and Profiling System
// This demonstrates the integration of all performance components

#include "src/performance/PerformanceProfiler.hpp"
#include "src/performance/BenchmarkSuite.hpp"
#include "src/performance/PerformanceAnalyzer.hpp"
#include "src/performance/PerformanceAPI.hpp"
#include "src/brain/Brain.hpp"
#include <iostream>
#include <memory>
#include <chrono>

int main() {
    std::cout << "=== NLM Performance Benchmarking and Profiling Test ===" << std::endl;
    std::cout << std::endl;
    
    // Initialize Brain with configuration
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize Brain!" << std::endl;
        return 1;
    }
    
    std::cout << "Brain initialized successfully" << std::endl;
    std::cout << "Total neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "Total synapses: " << brain->getTotalSynapseCount() << std::endl;
    std::cout << std::endl;
    
    // Test 1: Performance Profiler
    std::cout << "=== Test 1: Performance Profiler ===" << std::endl;
    auto profiler = std::make_unique<nlm::PerformanceProfiler>();
    
    if (profiler->initialize()) {
        std::cout << "Performance profiler initialized successfully" << std::endl;
        
        // Start profiling a section
        profiler->startProfile("brain_step_simulation");
        
        // Simulate some work
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        // Stop profiling
        profiler->stopProfile("brain_step_simulation");
        
        // Get profile statistics
        auto stats = profiler->getProfileStats();
        std::cout << "Total execution time: " << stats.totalExecutionTime.count() << "s" << std::endl;
        std::cout << "Memory usage: " << stats.totalMemoryUsage << " bytes" << std::endl;
        std::cout << "Current CPU usage: " << stats.cpuUsage << "%" << std::endl;
        std::cout << "FPS: " << stats.averageFPS << std::endl;
        
        // Get bottleneck sections
        auto bottlenecks = profiler->getBottleneckSections();
        std::cout << "Bottleneck sections: ";
        for (const auto& section : bottlenecks) {
            std::cout << section << " ";
        }
        std::cout << std::endl;
        
        // Export profile data
        profiler->exportProfile("performance_test.csv", "csv");
        std::cout << "Profile data exported to performance_test.csv" << std::endl;
        
    } else {
        std::cout << "Failed to initialize performance profiler" << std::endl;
    }
    
    std::cout << std::endl;
    
    // Test 2: Benchmark Suite
    std::cout << "=== Test 2: Benchmark Suite ===" << std::endl;
    auto benchmarkSuite = std::make_unique<nlm::BenchmarkSuite>(config);
    
    if (benchmarkSuite->initialize()) {
        std::cout << "Benchmark suite initialized successfully" << std::endl;
        
        // Add a timed benchmark
        benchmarkSuite->addTimedBenchmark(
            "brain_simulation",
            "Simulate brain operations",
            [&brain]() {
                // Simulate brain step
                brain->profileBrainStep("simulation");
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            },
            50
        );
        
        // Add a memory benchmark
        benchmarkSuite->addMemoryBenchmark(
            "memory_allocation",
            "Test memory allocation performance",
            [](size_t size) { return malloc(size); },
            [](void* ptr) { free(ptr); },
            1024 * 1024  // 1MB
        );
        
        // Add a CPU benchmark
        benchmarkSuite->addCPUBenchmark(
            "cpu_stress_test",
            "CPU-intensive computation",
            [](size_t iterations) {
                volatile double result = 0.0;
                for (size_t i = 0; i < iterations; ++i) {
                    result += std::sin(i) * std::cos(i);
                }
            },
            1000000
        );
        
        // Run benchmarks
        auto results = benchmarkSuite->runAllBenchmarks();
        
        std::cout << "Benchmark results:" << std::endl;
        for (const auto& result : results) {
            std::cout << "  " << result.benchmarkName << ": " << result.executionTime << "s, "
                      << result.status << ", " << result.throughput << " ops/sec" << std::endl;
        }
        
        // Save benchmark results
        benchmarkSuite->saveResults("benchmark_results.json", "json");
        std::cout << "Benchmark results saved to benchmark_results.json" << std::endl;
        
    } else {
        std::cout << "Failed to initialize benchmark suite" << std::endl;
    }
    
    std::cout << std::endl;
    
    // Test 3: Performance Analyzer
    std::cout << "=== Test 3: Performance Analyzer ===" << std::endl;
    auto analyzer = std::make_unique<nlm::PerformanceAnalyzer>();
    
    if (analyzer->initialize()) {
        std::cout << "Performance analyzer initialized successfully" << std::endl;
        
        // Add some benchmark results for analysis
        analyzer->addBenchmarkRun("brain_simulation", {
            {"execution_time", 1.5},
            {"memory_usage", 1024 * 1024},
            {"cpu_usage", 25.5},
            {"throughput", 1000}
        });
        
        analyzer->addBenchmarkRun("memory_allocation", {
            {"execution_time", 0.8},
            {"memory_usage", 512 * 1024},
            {"cpu_usage", 10.2},
            {"throughput", 5000}
        });
        
        analyzer->addBenchmarkRun("cpu_stress_test", {
            {"execution_time", 2.2},
            {"memory_usage", 2048 * 1024},
            {"cpu_usage", 85.3},
            {"throughput", 2000}
        });
        
        // Calculate statistics
        auto stats = analyzer->calculateStatisticsForAllMetrics();
        std::cout << "Performance statistics:" << std::endl;
        for (const auto& stat : stats) {
            std::cout << "  " << stat.metricName << ": mean=" << stat.mean 
                      << ", std_dev=" << stat.standardDeviation 
                      << ", min=" << stat.minValue 
                      << ", max=" << stat.maxValue << std::endl;
        }
        
        // Analyze trends
        auto trends = analyzer->analyzeTrendsForAllMetrics();
        std::cout << "Performance trends:" << std::endl;
        for (const auto& trend : trends) {
            std::cout << "  " << trend.metricName << ": " << trend.trend 
                      << " (slope=" << trend.slope << ")" << std::endl;
        }
        
        // Generate optimization suggestions
        auto suggestions = analyzer->generateOptimizationSuggestions();
        std::cout << "Optimization suggestions:" << std::endl;
        for (const auto& suggestion : suggestions) {
            std::cout << "  " << suggestion.metricName << ": " << suggestion.suggestedAction 
                      << " (" << suggestion.priority << " priority)" << std::endl;
        }
        
        // Export analysis results
        analyzer->exportAnalysisResults("analysis_results.json", "json");
        std::cout << "Analysis results exported to analysis_results.json" << std::endl;
        
    } else {
        std::cout << "Failed to initialize performance analyzer" << std::endl;
    }
    
    std::cout << std::endl;
    
    // Test 4: Performance API (Unified Interface)
    std::cout << "=== Test 4: Performance API (Unified Interface) ===" << std::endl;
    auto& api = nlm::PerformanceAPI::getInstance();
    
    if (api.initialize(brain)) {
        std::cout << "Performance API initialized successfully" << std::endl;
        
        // Start profiling
        api.startProfiling("brain_operations");
        
        // Simulate some operations
        brain->profileBrainStep("test_operation");
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        // Stop profiling
        api.stopProfiling("brain_operations");
        
        // Get performance summary
        std::cout << "Performance summary:" << std::endl;
        std::cout << api.getPerformanceSummary() << std::endl;
        
        // Get bottleneck sections
        auto bottlenecks = api.getBottleneckSections();
        std::cout << "Bottleneck sections: ";
        for (const auto& section : bottlenecks) {
            std::cout << section << " ";
        }
        std::cout << std::endl;
        
        // Get performance metrics
        auto metrics = api.getPerformanceMetrics();
        std::cout << "Performance metrics:" << std::endl;
        for (const auto& metric : metrics) {
            std::cout << "  " << metric.first << ": " << metric.second << std::endl;
        }
        
        // Generate performance report
        std::cout << api.generatePerformanceReport() << std::endl;
        
        // Export performance data
        api.exportPerformanceData("unified_performance.json", "json");
        std::cout << "Unified performance data exported to unified_performance.json" << std::endl;
        
        // Cleanup
        api.shutdownAPI();
        
    } else {
        std::cout << "Failed to initialize Performance API" << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "=== All Performance Tests Completed Successfully ===" << std::endl;
    
    return 0;
}
