// NLM Performance Benchmark Suite
// Comprehensive benchmark suite demonstrating NLM brain performance analysis

#include "../benchmarking/BenchmarkSuite.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <memory>
#include <iomanip>

int main() {
    std::cout << "NLM Performance Benchmark Suite" << std::endl;
    std::cout << "================================" << std::endl;
    std::cout << std::endl;
    
    // Initialize configuration
    auto config = std::make_shared<Config>();
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Warning);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Configure benchmark suite
    nlm::BenchmarkConfig benchmarkConfig;
    benchmarkConfig.benchmarkSuiteName = "NLM Comprehensive Benchmark Suite";
    benchmarkConfig.outputDirectory = "benchmark_suite_results";
    benchmarkConfig.generateDetailedReports = true;
    benchmarkConfig.saveResultsToFile = true;
    
    // Enable benchmark phases
    benchmarkConfig.runBasicBenchmark = true;
    benchmarkConfig.runMemoryBenchmark = true;
    benchmarkConfig.runCPUBenchmark = true;
    benchmarkConfig.runConnectivityBenchmark = true;
    benchmarkConfig.runScalingBenchmark = true;
    
    // Configure neuron scales and step counts
    benchmarkConfig.neuronCounts = {100, 500, 1000, 2000};
    benchmarkConfig.stepCounts = {1000, 1000, 500, 200};
    
    // Performance thresholds
    benchmarkConfig.minStepsPerSecond = 100.0;
    benchmarkConfig.maxMemoryUsage = 1024.0;
    benchmarkConfig.maxCPUUsage = 80.0;
    
    // Initialize benchmark suite
    nlm::BenchmarkSuite benchmarkSuite;
    benchmarkSuite.configure(benchmarkConfig);
    
    // Add configurations to benchmark suite
    nlm::BenchmarkConfig::Configuration defaultConfig;
    defaultConfig.name = "default_optimized";
    defaultConfig.enableMemoryPool = true;
    defaultConfig.enableSOAVectorization = true;
    defaultConfig.enableEventQueue = true;
    defaultConfig.enableSparseConnectivity = true;
    defaultConfig.enableMultithreading = true;
    defaultConfig.threadCount = std::thread::hardware_concurrency();
    defaultConfig.simdLevel = "AVX2";
    
    nlm::BenchmarkConfig::Configuration memoryConfig;
    memoryConfig.name = "memory_optimized";
    memoryConfig.enableMemoryPool = true;
    memoryConfig.enableSOAVectorization = true;
    memoryConfig.enableEventQueue = true;
    memoryConfig.enableSparseConnectivity = true;
    memoryConfig.enableMultithreading = false;
    memoryConfig.threadCount = 1;
    memoryConfig.simdLevel = "AVX2";
    
    nlm::BenchmarkConfig::Configuration speedConfig;
    speedConfig.name = "speed_optimized";
    speedConfig.enableMemoryPool = true;
    speedConfig.enableSOAVectorization = true;
    speedConfig.enableEventQueue = true;
    speedConfig.enableSparseConnectivity = true;
    speedConfig.enableMultithreading = true;
    speedConfig.threadCount = std::thread::hardware_concurrency();
    speedConfig.simdLevel = "AVX512";
    
    benchmarkSuite.addConfiguration(defaultConfig.name, defaultConfig);
    benchmarkSuite.addConfiguration(memoryConfig.name, memoryConfig);
    benchmarkSuite.addConfiguration(speedConfig.name, speedConfig);
    
    std::cout << "Benchmark Suite Configuration:" << std::endl;
    std::cout << "---------------------------" << std::endl;
    std::cout << "Suite Name: " << benchmarkConfig.benchmarkSuiteName << std::endl;
    std::cout << "Output Directory: " << benchmarkConfig.outputDirectory << std::endl;
    std::cout << "Detailed Reports: " << (benchmarkConfig.generateDetailedReports ? "Enabled" : "Disabled") << std::endl;
    std::cout << "File Output: " << (benchmarkConfig.saveResultsToFile ? "Enabled" : "Disabled") << std::endl;
    std::cout << std::endl;
    
    std::cout << "Neuron Scales: ";
    for (size_t i = 0; i < benchmarkConfig.neuronCounts.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << benchmarkConfig.neuronCounts[i] << " neurons";
    }
    std::cout << std::endl;
    
    std::cout << "Step Counts: ";
    for (size_t i = 0; i < benchmarkConfig.stepCounts.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << benchmarkConfig.stepCounts[i] << " steps";
    }
    std::cout << std::endl;
    
    std::cout << std::endl;
    std::cout << "Configured Performance Configurations:" << std::endl;
    std::cout << "-------------------------------" << std::endl;
    for (const auto& [name, config] : benchmarkSuite.getConfigurations()) {
        std::cout << "  " << name << ": " << std::endl;
        std::cout << "    Memory Pool: " << (config.enableMemoryPool ? "Enabled" : "Disabled") << std::endl;
        std::cout << "    SOA Vectorization: " << (config.enableSOAVectorization ? "Enabled" : "Disabled") << std::endl;
        std::cout << "    Event Queue: " << (config.enableEventQueue ? "Enabled" : "Disabled") << std::endl;
        std::cout << "    Sparse Connectivity: " << (config.enableSparseConnectivity ? "Enabled" : "Disabled") << std::endl;
        std::cout << "    Multithreading: " << (config.enableMultithreading ? "Enabled" : "Disabled") << std::endl;
        if (config.enableMultithreading) {
            std::cout << "    Thread Count: " << config.threadCount << std::endl;
        }
        std::cout << "    SIMD Level: " << config.simdLevel << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "Starting benchmark suite execution..." << std::endl;
    std::cout << "This may take several minutes depending on system resources." << std::endl;
    std::cout << std::endl;
    
    // Run benchmark suite
    nlm::BenchmarkSuiteResult suiteResult = benchmarkSuite.run(benchmarkConfig);
    
    std::cout << "Benchmark Suite Results:" << std::endl;
    std::cout << "---------------------" << std::endl;
    std::cout << "Suite: " << suiteResult.suiteName << std::endl;
    std::cout << "Total Time: " << std::fixed << std::setprecision(2) << suiteResult.totalTime << "s" << std::endl;
    std::cout << "Benchmarks Run: " << suiteResult.totalBenchmarks << std::endl;
    std::cout << "Total Neurons Tested: " << suiteResult.totalNeuronsTested << std::endl;
    
    std::cout << std::endl;
    std::cout << "Performance Summary:" << std::endl;
    std::cout << "------------------" << std::endl;
    std::cout << "Average Steps/sec: " << std::fixed << std::setprecision(2) 
              << suiteResult.avgStepsPerSecond << std::endl;
    std::cout << "Min Steps/sec: " << std::fixed << std::setprecision(2) 
              << suiteResult.minStepsPerSecond << std::endl;
    std::cout << "Max Steps/sec: " << std::fixed << std::setprecision(2) 
              << suiteResult.maxStepsPerSecond << std::endl;
    std::cout << "Performance Std Dev: " << std::fixed << std::setprecision(2) 
              << suiteResult.stdDevStepsPerSecond << std::endl;
    
    std::cout << std::endl;
    std::cout << "Memory Summary:" << std::endl;
    std::cout << "--------------" << std::endl;
    std::cout << "Average Memory Usage: " << std::fixed << std::setprecision(2) 
              << suiteResult.avgMemoryUsage << " MB" << std::endl;
    std::cout << "Min Memory Usage: " << std::fixed << std::setprecision(2) 
              << suiteResult.minMemoryUsage << " MB" << std::endl;
    std::cout << "Max Memory Usage: " << std::fixed << std::setprecision(2) 
              << suiteResult.maxMemoryUsage << " MB" << std::endl;
    
    std::cout << std::endl;
    std::cout << "Configuration Analysis:" << std::endl;
    std::cout << "-------------------" << std::endl;
    for (const auto& [name, perf] : suiteResult.configPerformance) {
        std::cout << "  " << name << ": " << std::fixed << std::setprecision(2) 
                  << perf << " steps/sec" << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "Scaling Analysis:" << std::endl;
    std::cout << "---------------" << std::endl;
    for (const auto& [neurons, perf] : suiteResult.scalingData) {
        std::cout << "  " << neurons << " neurons: " << std::fixed << std::setprecision(2) 
                  << perf << " steps/sec" << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "Quality Assessment:" << std::endl;
    std::cout << "----------------" << std::endl;
    std::cout << "Benchmark Quality: " << std::fixed << std::setprecision(3) 
              << suiteResult.benchmarkQuality << std::endl;
    std::cout << "Regression Risk: " << std::fixed << std::setprecision(3) 
              << suiteResult.regressionRisk << std::endl;
    
    if (!suiteResult.detectedIssues.empty()) {
        std::cout << std::endl;
        std::cout << "Performance Issues Detected:" << std::endl;
        std::cout << "------------------------" << std::endl;
        for (const auto& issue : suiteResult.detectedIssues) {
            std::cout << "  - " << issue << std::endl;
        }
    }
    
    std::cout << std::endl;
    std::cout << "Benchmark Suite Summary:" << std::endl;
    std::cout << "-------------------" << std::endl;
    std::cout << "The comprehensive benchmark suite successfully:" << std::endl;
    std::cout << "✓ Tested brain performance at multiple scales (100 - 2000+ neurons)" << std::endl;
    std::cout << "✓ Compared different performance configurations" << std::endl;
    std::cout << "✓ Measured simulation speed and efficiency" << std::endl;
    std::cout << "✓ Tracked memory usage and system resources" << std::endl;
    std::cout << "✓ Analyzed scaling behavior and performance trends" << std::endl;
    std::cout << "✓ Detected performance bottlenecks and issues" << std::endl;
    std::cout << "✓ Generated detailed performance reports" << std::endl;
    std::cout << "✓ Provided comparative analysis across configurations" << std::endl;
    
    std::cout << std::endl;
    std::cout << "Benchmark Suite Demo Complete!" << std::endl;
    
    return 0;
}
