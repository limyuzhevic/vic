// Benchmarking Example
// This example demonstrates how to use the NLM Benchmarking System

#include "BenchmarkRunner.hpp"
#include "BenchmarkSuite.hpp"
#include "PerformanceBenchmarker.hpp"
#include "HardwareBenchmarker.hpp"
#include "ScalabilityBenchmarker.hpp"
#include <iostream>
#include <vector>
#include <memory>

class SimpleBenchmark : public Benchmark {
private:
    std::string name;
    BenchmarkCategory category;
    size_t neurons;
    size_t steps;
    
public:
    SimpleBenchmark(const std::string& n, BenchmarkCategory c, size_t neu, size_t s)
        : name(n), category(c), neurons(neu), steps(s) {}
    
    std::string getName() const override { return name; }
    BenchmarkCategory getCategory() const override { return category; }
    
    void setup() override {
        std::cout << "Setting up benchmark: " << name << std::endl;
        // Setup benchmark environment
    }
    
    BenchmarkMetrics run() override {
        std::cout << "Running benchmark: " << name << " (" << neurons << " neurons, " << steps << " steps)" << std::endl;
        
        BenchmarkMetrics metrics;
        metrics.benchmarkName = name;
        metrics.category = category;
        metrics.timestamp = std::chrono::high_resolution_clock::now();
        
        // Simulate benchmark workload
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < steps; ++step) {
            // Simulate neural network computation
            double total = 0.0;
            for (size_t neuron = 0; neuron < neurons; ++neuron) {
                for (size_t synapse = 0; synapse < 10; ++synapse) {
                    total += std::sin(neuron * 0.01 + synapse * 0.001);
                }
            }
            
            // Record some metrics
            metrics.operations++;
            if (step % 100 == 0) {
                metrics.memoryUsage = neurons * sizeof(double);
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        metrics.executionTime = std::chrono::duration<double>(endTime - startTime).count();
        metrics.throughput = metrics.operations / metrics.executionTime;
        metrics.stepsPerSecond = steps / metrics.executionTime;
        metrics.memoryPerStep = metrics.memoryUsage / steps;
        
        return metrics;
    }
    
    void teardown() override {
        std::cout << "Tearing down benchmark: " << name << std::endl;
        // Cleanup benchmark environment
    }
    
    bool validate() const override {
        return neurons > 0 && steps > 0;
    }
    
    size_t getExpectedNeurons() const override { return neurons; }
    size_t getExpectedSteps() const override { return steps; }
    std::string getDescription() const override {
        return "Simple benchmark simulating neural network computation";
    }
};

void demonstrateBasicBenchmarking() {
    std::cout << "=== Basic Benchmarking Example ===" << std::endl;
    
    // Create benchmark runner
    BenchmarkRunner runner("BasicBenchmarks");
    
    // Create benchmark suites
    auto cpuSuite = std::make_unique<BenchmarkSuite>("CPUBenchmarks");
    auto memorySuite = std::make_unique<BenchmarkSuite>("MemoryBenchmarks");
    auto simulationSuite = std::make_unique<BenchmarkSuite>("SimulationBenchmarks");
    
    // Add benchmarks to suites
    cpuSuite->addBenchmark(std::make_unique<SimpleBenchmark>("CPU_SingleThread", BenchmarkCategory::CPU_PERFORMANCE, 100, 100));
    cpuSuite->addBenchmark(std::make_unique<SimpleBenchmark>("CPU_MultiThread", BenchmarkCategory::CPU_PERFORMANCE, 500, 100));
    
    memorySuite->addBenchmark(std::make_unique<SimpleBenchmark>("Memory_Small", BenchmarkCategory::MEMORY_PERFORMANCE, 200, 50));
    memorySuite->addBenchmark(std::make_unique<SimpleBenchmark>("Memory_Large", BenchmarkCategory::MEMORY_PERFORMANCE, 1000, 50));
    
    simulationSuite->addBenchmark(std::make_unique<SimpleBenchmark>("Simulation_Small", BenchmarkCategory::SIMULATION_PERFORMANCE, 100, 200));
    simulationSuite->addBenchmark(std::make_unique<SimpleBenchmark>("Simulation_Large", BenchmarkCategory::SIMULATION_PERFORMANCE, 1000, 200));
    
    // Add suites to runner
    runner.addSuite(std::move(cpuSuite));
    runner.addSuite(std::move(memorySuite));
    runner.addSuite(std::move(simulationSuite));
    
    // Run all benchmarks
    std::cout << "Running all benchmarks..." << std::endl;
    auto results = runner.runAllBenchmarks();
    
    // Display results
    std::cout << "\nBenchmark Results Summary:" << std::endl;
    for (const auto& result : results) {
        std::cout << "  " << result.benchmarkName << " (" << BenchmarkSuite::benchmarkCategoryToString(result.category) << "):" << std::endl;
        std::cout << "    Status: " << (result.passed ? "PASSED" : "FAILED") << std::endl;
        std::cout << "    Average Time: " << result.averageExecutionTime << "s" << std::endl;
        std::cout << "    Throughput: " << result.metrics.empty() ? 0.0 : result.metrics[0].throughput << " ops/s" << std::endl;
        std::cout << "    Steps per Second: " << result.metrics.empty() ? 0.0 : result.metrics[0].stepsPerSecond << std::endl;
    }
}

void demonstratePerformanceBenchmarking() {
    std::cout << "\n=== Performance Benchmarking Example ===" << std::endl;
    
    // Create performance benchmarker
    auto perfBenchmarker = std::make_unique<PerformanceBenchmarker>(1000, 500, 30.0, 0.01, 0.012);
    
    // Setup and run performance benchmark
    perfBenchmarker->setup();
    auto metrics = perfBenchmarker->run();
    perfBenchmarker->teardown();
    
    // Display performance metrics
    std::cout << "Performance Benchmark Results:" << std::endl;
    std::cout << "  Benchmark Name: " << metrics.benchmarkName << std::endl;
    std::cout << "  Category: " << BenchmarkSuite::benchmarkCategoryToString(metrics.category) << std::endl;
    std::cout << "  Execution Time: " << metrics.executionTime << "s" << std::endl;
    std::cout << "  CPU Time: " << metrics.cpuTime << "s" << std::endl;
    std::cout << "  Memory Usage: " << metrics.memoryUsage << " bytes" << std::endl;
    std::cout << "  Throughput: " << metrics.throughput << " ops/s" << std::endl;
    std::cout << "  Steps per Second: " << metrics.stepsPerSecond << std::endl;
    std::cout << "  Performance Score: " << perfBenchmarker->calculatePerformanceScore() << std::endl;
}

void demonstrateHardwareBenchmarking() {
    std::cout << "\n=== Hardware Benchmarking Example ===" << std::endl;
    
    // Create hardware config
    HardwareConfig config;
    config.platform = "Linux";
    config.architecture = "x86_64";
    config.compiler = "GCC";
    config.compilerVersion = "9.4.0";
    config.buildType = "Release";
    config.enableSIMD = true;
    config.enableMultithreading = true;
    config.enableMemoryPool = true;
    config.enableEventQueue = true;
    config.enableSparseConnectivity = true;
    config.threadCount = 4;
    config.memoryPoolSize = 1024; // MB
    config.simdExtension = "AVX2";
    
    // Create hardware benchmarker
    auto hardwareBenchmarker = std::make_unique<HardwareBenchmarker>(500, 200, config);
    
    // Setup and run hardware benchmark
    hardwareBenchmarker->setup();
    auto metrics = hardwareBenchmarker->run();
    hardwareBenchmarker->teardown();
    
    // Display hardware information
    std::cout << "Hardware Information:" << std::endl;
    std::cout << "  Platform: " << hardwareBenchmarker->getPlatform() << std::endl;
    std::cout << "  Architecture: " << hardwareBenchmarker->getArchitecture() << std::endl;
    std::cout << "  Compiler: " << hardwareBenchmarker->getCompiler() << " " << hardwareBenchmarker->getCompilerVersion() << std::endl;
    std::cout << "  CPU Frequency: " << hardwareBenchmarker->getCpuFrequency() << " GHz" << std::endl;
    std::cout << "  CPU Cores: " << hardwareBenchmarker->getCpuCores() << std::endl;
    std::cout << "  CPU Threads: " << hardwareBenchmarker->getCpuThreads() << std::endl;
    std::cout << "  Total RAM: " << hardwareBenchmarker->getTotalRAM() / (1024*1024) << " MB" << std::endl;
    
    if (hardwareBenchmarker->getGpuAvailable()) {
        std::cout << "  GPU Available: Yes" << std::endl;
        std::cout << "  GPU Model: " << hardwareBenchmarker->getGpuModel() << std::endl;
        std::cout << "  GPU Memory: " << hardwareBenchmarker->getGpuMemory() << " GB" << std::endl;
    } else {
        std::cout << "  GPU Available: No" << std::endl;
    }
    
    // Display hardware optimization recommendations
    auto recommendations = hardwareBenchmarker->getOptimizationRecommendations();
    std::cout << "\nOptimization Recommendations:" << std::endl;
    for (const auto& recommendation : recommendations) {
        std::cout << "  - " << recommendation << std::endl;
    }
}

void demonstrateScalabilityBenchmarking() {
    std::cout << "\n=== Scalability Benchmarking Example ===" << std::endl;
    
    // Create scalability config
    ScalabilityConfig config;
    config.neuronScales = {100, 500, 1000, 2000};
    config.stepScales = {100, 500, 1000};
    config.scalabilityDimension = "neurons";
    config.includeOverheadAnalysis = true;
    config.includeCommunicationCost = true;
    config.includeAmdahlsLawAnalysis = true;
    config.includeGustafsonsLawAnalysis = true;
    config.measurementInterval = 1.0;
    config.warmUpSteps = 10;
    config.enableParallelExecution = true;
    config.maxConcurrency = 4;
    
    // Create scalability benchmarker
    auto scalabilityBenchmarker = std::make_unique<ScalabilityBenchmarker>(100, 100, config);
    
    // Setup and run scalability tests
    scalabilityBenchmarker->setup();
    auto metrics = scalabilityBenchmarker->run();
    scalabilityBenchmarker->teardown();
    
    // Get scalability results
    auto results = scalabilityBenchmarker->getScalabilityResults();
    
    std::cout << "Scalability Results:" << std::endl;
    std::cout << "  Number of Configurations Tested: " << results.size() << std::endl;
    
    // Display detailed results
    for (const auto& result : results) {
        std::cout << "    Neurons: " << result.neuronCount << ", Steps: " << result.simulationSteps << std::endl;
        std::cout << "      Execution Time: " << result.executionTime << "s" << std::endl;
        std::cout << "      Throughput: " << result.throughput << " ops/s" << std::endl;
        std::cout << "      Memory per Neuron: " << result.memoryPerNeuron << " bytes" << std::endl;
        std::cout << "      Time per Neuron: " << result.timePerNeuron << "s" << std::endl;
        std::cout << "      Scalability Factor: " << result.scalabilityFactor << std::endl;
        std::cout << "      Efficiency: " << result.efficiency << std::endl;
        std::cout << "      Scalability Class: " << result.scalabilityClass << std::endl;
    }
    
    // Display scalability analysis
    double strongScaling = scalabilityBenchmarker->calculateStrongScaling(results);
    double weakScaling = scalabilityBenchmarker->calculateWeakScaling(results);
    
    std::cout << "\nScalability Analysis:" << std::endl;
    std::cout << "  Strong Scaling Efficiency: " << strongScaling << std::endl;
    std::cout << "  Weak Scaling Efficiency: " << weakScaling << std::endl;
    
    // Get optimization recommendations
    auto recommendations = scalabilityBenchmarker->getRecommendedScalabilityConfiguration(1000000); // 1M ops target
    std::cout << "\nRecommended Configuration for 1M ops:" << std::endl;
    for (size_t neurons : recommendations) {
        std::cout << "  " << neurons << " neurons" << std::endl;
    }
}

void demonstrateRegressionTesting() {
    std::cout << "\n=== Regression Testing Example ===" << std::endl;
    
    // Create benchmark runner
    BenchmarkRunner runner("RegressionTest");
    
    // Create baseline suite
    auto baselineSuite = std::make_unique<BenchmarkSuite>("Baseline");
    baselineSuite->addBenchmark(std::make_unique<SimpleBenchmark>("Baseline_Test", BenchmarkCategory::CPU_PERFORMANCE, 1000, 100));
    
    // Create test suite
    auto testSuite = std::make_unique<BenchmarkSuite>("Test");
    testSuite->addBenchmark(std::make_unique<SimpleBenchmark>("Test_Test", BenchmarkCategory::CPU_PERFORMANCE, 1000, 100));
    
    // Add suites to runner
    runner.addSuite(std::move(baselineSuite));
    runner.addSuite(std::move(testSuite));
    
    // Run regression tests
    std::cout << "Running regression tests..." << std::endl;
    bool regressionTestPassed = runner.runRegressionTest("Baseline", 0.05); // 5% threshold
    
    std::cout << "Regression Test Result: " << (regressionTestPassed ? "PASSED" : "FAILED") << std::endl;
    
    // Detect regressions
    std::vector<BenchmarkComparison> regressions = runner.detectRegressions(0.05);
    
    if (regressions.empty()) {
        std::cout << "No performance regressions detected." << std::endl;
    } else {
        std::cout << "Performance Regressions Detected:" << std::endl;
        for (const auto& regression : regressions) {
            std::cout << "  " << regression.baselineBenchmark << " -> " << regression.testBenchmark << std::endl;
            std::cout << "    Performance Change: " << (regression.performanceImprovement * 100) << "%" << std::endl;
            std::cout << "    Regression Status: " << (regression.isRegressed ? "REGRESSION" : "IMPROVEMENT") << std::endl;
            std::cout << "    Confidence Level: " << regression.confidenceLevel << std::endl;
        }
    }
}

int main() {
    std::cout << "NLM Benchmarking Example" << std::endl;
    std::cout << "==========================" << std::endl;
    
    demonstrateBasicBenchmarking();
    demonstratePerformanceBenchmarking();
    demonstrateHardwareBenchmarking();
    demonstrateScalabilityBenchmarking();
    demonstrateRegressionTesting();
    
    std::cout << "\n=== Example Complete ===" << std::endl;
    std::cout << "Benchmarking examples completed successfully." << std::endl;
    
    return 0;
}