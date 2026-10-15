// Unit Tests for Performance Monitoring
// Comprehensive test suite for advanced performance monitoring features

#include "tests/utils/TestUtils.hpp"
#include "src/performance/PerformanceMonitor.hpp"
#include "src/performance/Profiler.hpp"
#include "src/performance/Benchmark.hpp"
#include "src/performance/Analyzer.hpp"
#include <cassert>
#include <iostream>
#include <thread>
#include <chrono>
#include <numeric>
#include <algorithm>

namespace test_performance_monitoring {

void testPerformanceMonitorCreation() {
    TestUtils::TestSession session("testPerformanceMonitorCreation");
    
    auto config = std::make_shared<nlm::Config>();
    config->set("monitor_enabled", true, nlm::ConfigSource::Runtime);
    config->set("sampling_interval", 100, nlm::ConfigSource::Runtime);
    config->set("max_history_points", 10000, nlm::ConfigSource::Runtime);
    
    auto monitor = std::make_unique<nlm::PerformanceMonitor>(config);
    TestAssert::true(monitor != nullptr, "PerformanceMonitor should be created successfully");
    
    std::cout << "    testPerformanceMonitorCreation passed" << std::endl;
    session.setSuccess(true);
}

void testPerformanceMonitorStartStop() {
    TestUtils::TestSession session("testPerformanceMonitorStartStop");
    
    auto config = std::make_shared<nlm::Config>();
    auto monitor = std::make_unique<nlm::PerformanceMonitor>(config);
    
    bool startSuccess = monitor->startMonitoring();
    TestAssert::true(startSuccess, "PerformanceMonitor should start successfully");
    
    // Monitor should be running
    TestAssert::true(monitor->isMonitoring(), "Monitor should be monitoring after start");
    
    // Wait a bit for data collection
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    bool stopSuccess = monitor->stopMonitoring();
    TestAssert::true(stopSuccess, "PerformanceMonitor should stop successfully");
    
    // Monitor should not be running
    TestAssert::false(monitor->isMonitoring(), "Monitor should not be monitoring after stop");
    
    std::cout << "    testPerformanceMonitorStartStop passed" << std::endl;
    session.setSuccess(true);
}

void testPerformanceMonitorDataCollection() {
    TestUtils::TestSession session("testPerformanceMonitorDataCollection");
    
    auto config = std::make_shared<nlm::Config>();
    auto monitor = std::make_unique<nlm::PerformanceMonitor>(config);
    
    bool startSuccess = monitor->startMonitoring();
    TestAssert::true(startSuccess, "PerformanceMonitor should start successfully");
    
    // Collect data for a short period
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    // Check that we have collected some data
    auto systemMetrics = monitor->getSystemMetrics();
    TestAssert::true(systemMetrics.size() > 0, "Should have collected system metrics");
    
    auto neuralMetrics = monitor->getNeuralMetrics();
    TestAssert::true(neuralMetrics.size() > 0, "Should have collected neural metrics");
    
    bool stopSuccess = monitor->stopMonitoring();
    TestAssert::true(stopSuccess, "PerformanceMonitor should stop successfully");
    
    std::cout << "    testPerformanceMonitorDataCollection passed" << std::endl;
    session.setSuccess(true);
}

void testPerformanceMonitorConfiguration() {
    TestUtils::TestSession session("testPerformanceMonitorConfiguration");
    
    auto config = std::make_shared<nlm::Config>();
    auto monitor = std::make_unique<nlm::PerformanceMonitor>(config);
    
    // Test configuration loading
    config->set("enable_system_metrics", true, nlm::ConfigSource::Runtime);
    config->set("enable_neural_metrics", true, nlm::ConfigSource::Runtime);
    config->set("enable_plasticity_metrics", true, nlm::ConfigSource::Runtime);
    config->set("enable_memory_metrics", true, nlm::ConfigSource::Runtime);
    config->set("enable_prediction_metrics", true, nlm::ConfigSource::Runtime);
    config->set("enable_cognitive_metrics", true, nlm::ConfigSource::Runtime);
    config->set("enable_development_metrics", true, nlm::ConfigSource::Runtime);
    config->set("enable_neuromodulation_metrics", true, nlm::ConfigSource::Runtime);
    config->set("enable_spike_profiling", true, nlm::ConfigSource::Runtime);
    config->set("enable_memory_profiling", true, nlm::ConfigSource::Runtime);
    config->set("enable_function_profiling", true, nlm::ConfigSource::Runtime);
    config->set("enable_line_profiling", true, nlm::ConfigSource::Runtime);
    config->set("profiling_interval", 1000, nlm::ConfigSource::Runtime);
    
    TestAssert::true(monitor->isSystemMetricsEnabled(), "System metrics should be enabled");
    TestAssert::true(monitor->isNeuralMetricsEnabled(), "Neural metrics should be enabled");
    TestAssert::true(monitor->isPlasticityMetricsEnabled(), "Plasticity metrics should be enabled");
    TestAssert::true(monitor->isMemoryMetricsEnabled(), "Memory metrics should be enabled");
    TestAssert::true(monitor->isPredictionMetricsEnabled(), "Prediction metrics should be enabled");
    TestAssert::true(monitor->isCognitiveMetricsEnabled(), "Cognitive metrics should be enabled");
    TestAssert::true(monitor->isDevelopmentMetricsEnabled(), "Development metrics should be enabled");
    TestAssert::true(monitor->isNeuromodulationMetricsEnabled(), "Neuromodulation metrics should be enabled");
    TestAssert::true(monitor->isSpikeProfilingEnabled(), "Spike profiling should be enabled");
    TestAssert::true(monitor->isMemoryProfilingEnabled(), "Memory profiling should be enabled");
    TestAssert::true(monitor->isFunctionProfilingEnabled(), "Function profiling should be enabled");
    TestAssert::true(monitor->isLineProfilingEnabled(), "Line profiling should be enabled");
    
    std::cout << "    testPerformanceMonitorConfiguration passed" << std::endl;
    session.setSuccess(true);
}

void testProfiler() {
    TestUtils::TestSession session("testProfiler");
    
    auto profiler = std::make_unique<nlm::Profiler>("test_profiler");
    
    // Start profiling
    profiler->start();
    
    // Simulate some work
    double sum = 0.0;
    for (int i = 0; i < 1000; ++i) {
        sum += TestUtils::randomDouble();
    }
    
    // Stop profiling
    profiler->stop();
    
    // Get profiling results
    auto results = profiler->getResults();
    TestAssert::true(results.size() > 0, "Profiler should have collected results");
    
    TestAssert::true(std::abs(sum) > 0.0, "Sum should not be zero");
    
    std::cout << "    testProfiler passed" << std::endl;
    session.setSuccess(true);
}

void testBenchmark() {
    TestUtils::TestSession session("testBenchmark");
    
    auto config = std::make_shared<nlm::Config>();
    auto benchmark = std::make_unique<nlm::Benchmark>(config);
    
    // Create a simple benchmark function
    auto benchmarkFunc = []() {
        double sum = 0.0;
        for (int i = 0; i < 10000; ++i) {
            sum += TestUtils::randomDouble();
        }
        return sum;
    };
    
    // Run benchmark
    auto result = benchmark->runBenchmark("simple_benchmark", benchmarkFunc);
    
    TestAssert::true(result.success, "Benchmark should run successfully");
    TestAssert::true(result.iterations > 0, "Benchmark should have iterations");
    TestAssert::true(result.durationMs > 0, "Benchmark should have duration");
    TestAssert::true(result.operationsPerSecond > 0, "Benchmark should have operations per second");
    
    std::cout << "    testBenchmark passed" << std::endl;
    session.setSuccess(true);
}

void testAnalyzer() {
    TestUtils::TestSession session("testAnalyzer");
    
    auto config = std::make_shared<nlm::Config>();
    auto analyzer = std::make_unique<nlm::Analyzer>(config);
    
    // Create test data
    std::vector<double> data = TestUtils::generateNormalData(1000, 0.0, 1.0);
    
    // Analyze data
    auto analysis = analyzer->analyzeData("test_data", data);
    
    TestAssert::true(analysis.success, "Analysis should succeed");
    TestAssert::true(analysis.mean >= -3.0 && analysis.mean <= 3.0, "Mean should be within expected range");
    TestAssert::true(analysis.standardDeviation >= 0.0 && analysis.standardDeviation <= 3.0, "Std dev should be within expected range");
    TestAssert::true(analysis.min <= *std::min_element(data.begin(), data.end()), "Min should match data");
    TestAssert::true(analysis.max >= *std::max_element(data.begin(), data.end()), "Max should match data");
    
    std::cout << "    testAnalyzer passed" << std::endl;
    session.setSuccess(true);
}

void testPerformanceMetricsCalculation() {
    TestUtils::TestSession session("testPerformanceMetricsCalculation");
    
    auto config = std::make_shared<nlm::Config>();
    auto monitor = std::make_unique<nlm::PerformanceMonitor>(config);
    
    bool startSuccess = monitor->startMonitoring();
    TestAssert::true(startSuccess, "PerformanceMonitor should start successfully");
    
    // Collect data for a period
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    
    // Get performance metrics
    auto metrics = monitor->getPerformanceMetrics();
    
    TestAssert::true(metrics.cpuUsage >= 0.0 && metrics.cpuUsage <= 100.0, "CPU usage should be between 0 and 100");
    TestAssert::true(metrics.memoryUsageMB >= 0.0, "Memory usage should be non-negative");
    TestAssert::true(metrics.peakMemoryUsageMB >= metrics.memoryUsageMB, "Peak memory should be >= current memory");
    TestAssert::true(metrics.totalTimeMs > 0, "Total time should be positive");
    
    bool stopSuccess = monitor->stopMonitoring();
    TestAssert::true(stopSuccess, "PerformanceMonitor should stop successfully");
    
    std::cout << "    testPerformanceMetricsCalculation passed" << std::endl;
    session.setSuccess(true);
}

void testPerformanceMonitorThresholds() {
    TestUtils::TestSession session("testPerformanceMonitorThresholds");
    
    auto config = std::make_shared<nlm::Config>();
    auto monitor = std::make_unique<nlm::PerformanceMonitor>(config);
    
    // Set thresholds
    monitor->setCPUMonitoringThreshold(80.0); // 80% threshold
    monitor->setMemoryMonitoringThreshold(512 * 1024 * 1024); // 512MB threshold
    
    TestAssert::true(monitor->getCPUMonitoringThreshold() == 80.0, "CPU threshold should be set correctly");
    TestAssert::true(monitor->getMemoryMonitoringThreshold() == 512 * 1024 * 1024, "Memory threshold should be set correctly");
    
    // Test warning state (in a real scenario, this would be set based on actual metrics)
    // For this test, we'll just verify the thresholds are set
    
    std::cout << "    testPerformanceMonitorThresholds passed" << std::endl;
    session.setSuccess(true);
}

void runAll() {
    std::cout << "=== Performance Monitoring Tests ===" << std::endl;
    
    testPerformanceMonitorCreation();
    testPerformanceMonitorStartStop();
    testPerformanceMonitorDataCollection();
    testPerformanceMonitorConfiguration();
    testProfiler();
    testBenchmark();
    testAnalyzer();
    testPerformanceMetricsCalculation();
    testPerformanceMonitorThresholds();
    
    std::cout << std::endl;
    std::cout << "=== All Performance Monitoring Tests PASSED ===" << std::endl;
}