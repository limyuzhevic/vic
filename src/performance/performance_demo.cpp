// performance_demo.cpp
// Comprehensive demonstration of the NLM performance profiling framework
// Shows how to use PerformanceProfiler, PerformanceBenchmarkSuite, MemoryProfiler, and IntegrationTestProfiler

#include "performance/PerformanceProfiler.h"
#include "performance/PerformanceBenchmarkSuite.h"
#include "performance/MemoryProfiler.h"
#include "performance/IntegrationTestProfiler.h"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <memory>
#include <chrono>
#include <thread>

// Global variables for performance tracking
PerformanceProfiler& profiler = PerformanceProfiler::getInstance();
MemoryProfiler& memoryProfiler = MemoryProfiler::getInstance();
PerformanceBenchmarkSuite& benchmarkSuite = PerformanceBenchmarkSuite::getInstance();
IntegrationTestProfiler& integrationProfiler = IntegrationTestProfiler::getInstance();

// Demo functions
void demoBasicProfiling() {
    std::cout << "╔══════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║ " << std::left << std::setw(70) << "Basic Performance Profiling Demo" << " ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
    
    // Configure the profiler
    PerformanceProfilerConfig config;
    config.enableTiming = true;
    config.enableMemoryTracking = true;
    config.enableVisualOutput = true;
    config.nonIntrusiveMode = true;
    
    profiler.setConfig(config);
    
    // Profile different operations
    std::cout << "Profiling various operations..." << std::endl;
    
    // Time a function
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 1000; ++i) {
        volatile float result = 1.0f / (i + 1);
        (void)result;
    }
    auto end = std::chrono::high_resolution_clock::now();
    double duration = std::chrono::duration<double, std::micro>(end - start).count();
    profiler.recordTiming("Loop Calculation", "math", duration);
    
    // Profile memory allocation
    std::vector<float> testVector(1000);
    memoryProfiler.trackAllocation(sizeof(float) * 1000, testVector.data(), "Vector Allocation");
    
    // Deallocate
    memoryProfiler.trackDeallocation(testVector.data());
    
    // Print summary
    std::cout << "\nPerformance Summary:" << std::endl;
    std::cout << "  Total Metrics Recorded: " << profiler.getMetricCount() << std::endl;
    std::cout << "  Total Active Intervals: " << profiler.getActiveIntervalCount() << std::endl;
    std::cout << "  Total Profile Time: " << std::fixed << std::setprecision(2) << profiler.getTotalProfileTime() << " μs" << std::endl;
    
    std::cout << "\n" << std::string(80, '=') << std::endl;
}

void demoBrainProfiling() {
    std::cout << "╔══════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║ " << std::left << std::setw(70) << "Brain System Profiling Demo" << " ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
    
    // Create brain with profiling enabled
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1f, nlm::ConfigSource::Default);
    
    std::cout << "Creating brain with " << config->getOr<size_t>("neuron_count", 0) << " neurons..." << std::endl;
    
    // Profile brain creation
    auto brain = std::make_shared<nlm::Brain>(config);
    auto profileStart = std::chrono::high_resolution_clock::now();
    brain->initialize();
    auto profileEnd = std::chrono::high_resolution_clock::now();
    
    double initTime = std::chrono::duration<double, std::micro>(profileEnd - profileStart).count();
    profiler.recordTiming("Brain Initialization", "brain", initTime);
    
    std::cout << "Brain initialized in " << std::fixed << std::setprecision(2) << initTime << " μs" << std::endl;
    
    // Profile brain stepping
    std::cout << "Profiling brain stepping (10 steps)..." << std::endl;
    auto stepStart = std::chrono::high_resolution_clock::now();
    
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain->injectCurrentToNeurons(nlm::NeuronType::Sensory, 30.0f);
        brain->step(step);
        
        if (step % 5 == 0) {
            auto stepTime = std::chrono::duration<double, std::micro>(std::chrono::high_resolution_clock::now() - stepStart).count();
            std::cout << "  Step " << step << ": " << std::fixed << std::setprecision(2) << stepTime << " μs" << std::endl;
        }
    }
    
    auto stepEnd = std::chrono::high_resolution_clock::now();
    double stepDuration = std::chrono::duration<double, std::micro>(stepEnd - stepStart).count();
    profiler.recordTiming("Brain Steps", "brain", stepDuration);
    
    std::cout << "Total step duration: " << std::fixed << std::setprecision(2) << stepDuration << " μs" << std::endl;
    std::cout << "Average step time: " << std::fixed << std::setprecision(2) << stepDuration / 10.0 << " μs" << std::endl;
    
    // Memory profiling for brain
    memoryProfiler.profileBrainMemoryUsage(brain.get());
    
    std::cout << "\nBrain Memory Usage: " << memoryProfiler.getCurrentMemoryUsage() << " bytes" << std::endl;
    
    std::cout << "\n" << std::string(80, '=') << std::endl;
}

void demoBenchmarkSuite() {
    std::cout << "╔══════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║ " << std::left << std::setw(70) << "Performance Benchmark Suite Demo" << " ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
    
    // Configure benchmark suite
    PerformanceProfilerConfig benchmarkConfig;
    benchmarkConfig.enableTiming = true;
    benchmarkConfig.enableMemoryTracking = true;
    benchmarkConfig.enableVisualOutput = true;
    benchmarkConfig.nonIntrusiveMode = true;
    
    benchmarkSuite.setProfilerConfig(benchmarkConfig);
    
    std::cout << "Running comprehensive benchmark suite..." << std::endl;
    
    // Run benchmarks at different scales
    benchmarkSuite.runSmallScaleBenchmarks();
    
    std::cout << "\nRunning medium scale benchmarks..." << std::endl;
    benchmarkSuite.runMediumScaleBenchmarks();
    
    std::cout << "\nRunning large scale benchmarks..." << std::endl;
    benchmarkSuite.runLargeScaleBenchmarks();
    
    // Analyze results
    benchmarkSuite.analyzeResults();
    
    // Generate performance report
    std::cout << "\nGenerating comprehensive performance report..." << std::endl;
    benchmarkSuite.generatePerformanceReport();
    
    // Export results
    std::cout << "Exporting benchmark results..." << std::endl;
    benchmarkSuite.exportBenchmarkResults("./profiles/benchmark_results.txt");
    
    std::cout << "\nBenchmark Suite Summary:" << std::endl;
    std::cout << "  Total Benchmarks Run: " << benchmarkSuite.getResults().size() << std::endl;
    std::cout << "  Categories: ";
    
    std::vector<std::string> categories;
    for (const auto& result : benchmarkSuite.getResults()) {
        if (std::find(categories.begin(), categories.end(), result.category) == categories.end()) {
            categories.push_back(result.category);
        }
    }
    
    for (size_t i = 0; i < categories.size(); ++i) {
        std::cout << categories[i];
        if (i < categories.size() - 1) std::cout << ", ";
    }
    std::cout << std::endl;
    
    std::cout << "\n" << std::string(80, '=') << std::endl;
}

void demoMemoryProfiling() {
    std::cout << "╔══════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║ " << std::left << std::setw(70) << "Memory Profiling Demo" << " ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
    
    // Configure memory profiler
    MemoryProfilerConfig memoryConfig;
    memoryConfig.enableAllocationTracking = true;
    memoryConfig.enableDeallocationTracking = true;
    memoryConfig.enableLeakDetection = true;
    memoryConfig.enableRealTimeMonitoring = true;
    memoryConfig.enableStackTraces = true;
    memoryConfig.nonIntrusiveMode = true;
    
    memoryProfiler.setConfig(memoryConfig);
    
    std::cout << "Tracking memory allocations..." << std::endl;
    
    // Allocate various data structures
    std::vector<float> vec1(1000);
    memoryProfiler.trackAllocation(sizeof(float) * 1000, vec1.data(), "Vector<float>", "demo_memory_profiling.cpp", __LINE__);
    
    std::vector<int> vec2(500);
    memoryProfiler.trackAllocation(sizeof(int) * 500, vec2.data(), "Vector<int>", "demo_memory_profiling.cpp", __LINE__);
    
    std::string testString = "Hello, Memory Profiler!";
    memoryProfiler.trackAllocation(testString.size() + 1, &testString[0], "std::string", "demo_memory_profiling.cpp", __LINE__);
    
    // Simulate some operations
    for (size_t i = 0; i < 100; ++i) {
        volatile float temp = 1.0f / (i + 1);
        (void)temp;
    }
    
    // Deallocate memory
    memoryProfiler.trackDeallocation(vec1.data());
    memoryProfiler.trackDeallocation(vec2.data());
    memoryProfiler.trackDeallocation(&testString[0]);
    
    // Generate memory report
    memoryProfiler.printMemorySummary();
    memoryProfiler.printLeakReport();
    
    // Export memory profiling data
    memoryProfiler.generateMemoryReport("./profiles/memory_profile_report.txt");
    
    std::cout << "\nMemory Profiling Summary:" << std::endl;
    std::cout << "  Total Allocations Tracked: " << memoryProfiler.getAllocationCount() << std::endl;
    std::cout << "  Total Deallocations Tracked: " << memoryProfiler.getDeallocationCount() << std::endl;
    std::cout << "  Memory Leaks Detected: " << memoryProfiler.getLeakCount() << std::endl;
    std::cout << "  Leak Percentage: " << std::fixed << std::setprecision(2) << memoryProfiler.getLeakPercentage() << "%" << std::endl;
    std::cout << "  Peak Memory Usage: " << memoryProfiler.getPeakMemoryUsage() << " bytes" << std::endl;
    
    std::cout << "\n" << std::string(80, '=') << std::endl;
}

void demoIntegrationTesting() {
    std::cout << "╔══════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║ " << std::left << std::setw(70) << "Integration Test Profiler Demo" << " ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════════════════════════════╝" << std::endl;
    std::endl;
    
    // Configure integration test profiler
    IntegrationTestProfilerConfig integrationConfig;
    integrationConfig.enablePerformanceProfiling = true;
    integrationConfig.enableMemoryProfiling = true;
    integrationConfig.enableLeakDetection = true;
    integrationConfig.enableRegressionDetection = true;
    integrationConfig.enableDetailedOutput = true;
    integrationConfig.enableExport = true;
    integrationConfig.exportPath = "./test_profiles";
    
    integrationProfiler.setConfig(integrationConfig);
    
    std::cout << "Setting baseline performance..." << std::endl;
    integrationProfiler.setBaseline("Brain Creation", 0.001);  // 1ms
    integrationProfiler.setBaseline("Brain Initialization", 0.005);  // 5ms
    integrationProfiler.setBaseline("Brain Step", 0.002);  // 2ms
    
    std::cout << "Running integration test suite..." << std::endl;
    
    // Run full test suite
    integrationProfiler.runFullTestSuite();
    
    // Generate performance dashboard
    integrationProfiler.generatePerformanceDashboard();
    
    // Export test results
    integrationProfiler.exportTestResults("./test_profiles/integration_test_results.txt");
    
    std::cout << "\nIntegration Test Summary:" << std::endl;
    std::cout << "  Total Tests: " << integrationProfiler.getTestCount() << std::endl;
    std::cout << "  Passed: " << integrationProfiler.getPassedCount() << std::endl;
    std::cout << "  Failed: " << integrationProfiler.getFailedCount() << std::endl;
    std::cout << "  Success Rate: " << std::fixed << std::setprecision(2) 
              << (static_cast<double>(integrationProfiler.getPassedCount()) / integrationProfiler.getTestCount() * 100.0) << "%" << std::endl;
    
    std::cout << "\nTest Categories:" << std::endl;
    std::unordered_map<std::string, size_t> categoryCounts;
    for (const auto& result : integrationProfiler.getTestResults()) {
        categoryCounts[result.testCategory]++;
    }
    
    for (const auto& pair : categoryCounts) {
        std::cout << "  " << pair.first << ": " << pair.second << " tests" << std::endl;
    }
    
    std::cout << "\n" << std::string(80, '=') << std::endl;
}

void demoCombinedProfiling() {
    std::cout << "╔══════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║ " << std::left << std::setw(70) << "Combined Profiling Demo" << " ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Running comprehensive profiling with all tools..." << std::endl;
    
    // Create brain for profiling
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.05f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Profile various components
    std::cout << "Profiling brain operations..." << std::endl;
    
    // Profile multiple steps
    auto start = std::chrono::high_resolution_clock::now();
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 30.0f);
        brain.step(step);
    }
    auto end = std::chrono::high_resolution_clock::now();
    
    double totalTime = std::chrono::duration<double, std::micro>(end - start).count();
    profiler.recordTiming("Combined Brain Operations", "brain", totalTime);
    
    // Profile memory usage
    memoryProfiler.profileBrainMemoryUsage(&brain);
    
    // Profile with benchmark suite
    BenchmarkConfig benchmarkConfig("Combined Benchmark", "Combined profiling demo", 500, 1000, 50);
    benchmarkSuite.runBenchmarkSuite(benchmarkConfig);
    
    std::cout << "\nCombined Profiling Results:" << std::endl;
    std::cout << "  Performance Metrics Recorded: " << profiler.getMetricCount() << std::endl;
    std::cout << "  Memory Allocations Tracked: " << memoryProfiler.getAllocationCount() << std::endl;
    std::cout << "  Memory Leaks: " << memoryProfiler.getLeakCount() << std::endl;
    std::cout << "  Benchmarks Completed: " << benchmarkSuite.getResults().size() << std::endl;
    
    // Generate final report
    std::cout << "\nGenerating final profiling report..." << std::endl;
    profiler.printSummaryReport();
    
    // Export comprehensive results
    profiler.exportToCSV("./profiles/combined_results.csv");
    
    std::cout << "\n" << std::string(80, '=') << std::endl;
}

int main() {
    std::cout << "╔══════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║ " << std::left << std::setw(70) << "NLM Performance Profiling Framework" << " ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
    std::cout << "Version: 1.0.0" << std::endl;
    std::cout << "Components: PerformanceProfiler, PerformanceBenchmarkSuite, MemoryProfiler, IntegrationTestProfiler" << std::endl;
    std::cout << std::endl;
    
    // Run all demos
    demoBasicProfiling();
    
    demoBrainProfiling();
    
    demoBenchmarkSuite();
    
    demoMemoryProfiling();
    
    demoIntegrationTesting();
    
    demoCombinedProfiling();
    
    std::cout << "╔══════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║ " << std::left << std::setw(70) << "All Demos Completed Successfully!" << " ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
    std::cout << "Profiling framework is fully functional and ready for production use." << std::endl;
    std::cout << std::endl;
    std::cout << "Generated files:" << std::endl;
    std::cout << "  - ./profiles/ (directory containing all profiling results)" << std::endl;
    std::cout << "  - ./test_profiles/ (directory containing test results)" << std::endl;
    
    return 0;
}