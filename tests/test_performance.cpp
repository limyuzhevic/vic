// test_performance.cpp - Performance profiling tests

#include <iostream>
#include <cassert>
#include <vector>
#include <chrono>
#include <thread>
#include <mutex>
#include <random>

// Include performance profiler headers
#include "profiler/PerformanceProfiler.hpp"
#include "profiler/MemoryProfiler.hpp"
#include "profiler/ThreadProfiler.hpp"
#include "profiler/Benchmark.hpp"
#include "profiler/Visualization.hpp"

namespace nlm {

void testPerformanceProfiler() {
    std::cout << "Testing PerformanceProfiler..." << std::endl;
    
    // Test basic performance profiler
    PerformanceProfiler profiler;
    
    // Start profiling
    profiler.startProfiling("test_session");
    assert(profiler.isProfilingActive());
    assert(profiler.getSessionName() == "test_session");
    
    // Record some performance data
    profiler.setCustomMetric("test_metric", 42.0);
    
    // Simulate some work
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    
    // Stop profiling
    profiler.stopProfiling();
    assert(!profiler.isProfilingActive());
    
    // Get performance statistics
    std::string stats = profiler.getPerformanceStatistics();
    assert(!stats.empty());
    
    // Generate report
    std::string report = profiler.generateReport();
    assert(!report.empty());
    
    std::cout << "✓ PerformanceProfiler tests passed" << std::endl;
}

void testMemoryProfiler() {
    std::cout << "Testing MemoryProfiler..." << std::endl;
    
    // Test basic memory profiler
    MemoryProfiler profiler;
    
    // Start profiling
    profiler.startProfiling("memory_test");
    assert(profiler.isProfilingActive());
    assert(profiler.getSessionName() == "memory_test");
    
    // Record memory usage
    MemoryProfiler::MemoryStats stats = profiler.getMemoryStats();
    assert(stats.totalAllocated >= 0);
    assert(stats.currentUsage >= 0);
    assert(stats.peakUsage >= 0);
    
    // Record allocations (simulated)
    profiler.recordAllocation(nullptr, 1024, "test_allocation");
    profiler.recordDeallocation(nullptr, "test_deallocation");
    
    // Stop profiling
    profiler.stopProfiling();
    assert(!profiler.isProfilingActive());
    
    // Get memory statistics
    stats = profiler.getMemoryStats();
    assert(stats.allocationCount >= 0);
    assert(stats.deallocationCount >= 0);
    
    // Generate report
    std::string report = profiler.generateReport();
    assert(!report.empty());
    
    std::cout << "✓ MemoryProfiler tests passed" << std::endl;
}

void testThreadProfiler() {
    std::cout << "Testing ThreadProfiler..." << std::endl;
    
    // Test basic thread profiler
    ThreadProfiler profiler;
    
    // Start profiling
    profiler.startProfiling("thread_test");
    assert(profiler.isProfilingActive());
    assert(profiler.getSessionName() == "thread_test");
    
    // Register current thread
    profiler.registerThread("Main Thread");
    
    // Record thread execution
    profiler.recordThreadExecution(1000); // 1ms
    profiler.recordThreadCPUTime(500);   // 0.5ms
    profiler.recordThreadWaitTime(200);  // 0.2ms
    profiler.recordThreadSleepTime(100); // 0.1ms
    profiler.recordContextSwitch();
    profiler.recordPageFault();
    
    // Record synchronization event
    ThreadProfiler::SynchronizationEvent event;
    event.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    event.threadId = std::this_thread::get_id();
    event.eventType = "lock";
    event.lockName = "mutex";
    event.duration = 50;
    event.successful = true;
    event.threadState = "active";
    
    profiler.recordSynchronizationEvent(event);
    
    // Get thread information
    ThreadProfiler::ThreadInfo info = profiler.getCurrentThreadInfo();
    assert(info.id == std::this_thread::get_id());
    assert(info.name == "Main Thread");
    assert(info.totalExecutionTime >= 0);
    
    // Get thread statistics
    ThreadProfiler::ThreadStats threadStats = profiler.getThreadStats();
    assert(threadStats.totalThreads >= 1);
    assert(threadStats.activeThreads >= 1);
    assert(threadStats.totalExecutionTime >= 0);
    
    // Stop profiling
    profiler.stopProfiling();
    assert(!profiler.isProfilingActive());
    
    // Get synchronization events
    std::vector<ThreadProfiler::SynchronizationEvent> events = profiler.getSynchronizationEvents();
    assert(events.size() >= 1);
    
    // Generate report
    std::string report = profiler.generateReport();
    assert(!report.empty());
    
    std::cout << "✓ ThreadProfiler tests passed" << std::endl;
}

void testBenchmark() {
    std::cout << "Testing Benchmark..." << std::endl;
    
    // Test basic benchmark
    Benchmark benchmark;
    
    // Add a simple benchmark
    Benchmark::BenchmarkConfig config;
    config.name = "simple_benchmark";
    config.setup = []() { std::cout << "Setting up..." << std::endl; };
    config.benchmark = []() {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        std::cout << "Running benchmark..." << std::endl;
    };
    config.teardown = []() { std::cout << "Tearing down..." << std::endl; };
    config.warmupIterations = 2;
    config.benchmarkIterations = 3;
    config.cooldownIterations = 2;
    
    benchmark.addBenchmark(config);
    
    // Run benchmark
    Benchmark::BenchmarkResult result = benchmark.runBenchmark(config);
    assert(result.benchmarkName == "simple_benchmark");
    assert(result.passed);
    assert(result.duration > 0);
    assert(result.throughput > 0);
    assert(result.quality >= 0.0 && result.quality <= 1.0);
    
    // Get benchmark results
    const std::vector<Benchmark::BenchmarkResult>& results = benchmark.getResults();
    assert(results.size() >= 1);
    
    // Calculate statistics
    std::vector<double> values;
    for (const auto& res : results) {
        values.push_back(res.duration);
    }
    
    Benchmark::BenchmarkStats stats = benchmark.calculateStats(values);
    assert(stats.mean >= 0.0);
    assert(stats.stdDev >= 0.0);
    assert(stats.min <= stats.mean);
    assert(stats.max >= stats.mean);
    
    // Generate report
    std::string report = benchmark.generateReport();
    assert(!report.empty());
    
    std::cout << "✓ Benchmark tests passed" << std::endl;
}

void testVisualization() {
    std::cout << "Testing Visualization..." << std::endl;
    
    // Test basic visualization
    Visualization::VisualizationConfig config;
    config.title = "Test Visualization";
    config.generateHtml = true;
    config.generateJson = true;
    config.generateCsv = true;
    config.theme = "light";
    config.chartWidth = 800.0;
    config.chartHeight = 600.0;
    
    Visualization visualizer(config);
    
    // Create test chart data
    std::vector<Visualization::ChartDataPoint> dataPoints;
    for (int i = 0; i < 10; ++i) {
        Visualization::ChartDataPoint point;
        point.timestamp = i * 1000;
        point.value = static_cast<double>(i * 10);
        point.label = "Point " + std::to_string(i);
        point.series = "Series 1";
        dataPoints.push_back(point);
    }
    
    // Create chart
    Visualization::ChartConfig chart = visualizer.createLineChart("Test Line Chart", dataPoints);
    assert(chart.title == "Test Line Chart");
    assert(chart.type == Visualization::ChartType::LINE_CHART);
    assert(chart.data.size() == 10);
    
    // Generate JSON report
    std::string jsonReport = visualizer.generateJsonData({chart});
    assert(!jsonReport.empty());
    assert(jsonReport.find("Test Line Chart") != std::string::npos);
    
    // Generate CSV report
    std::string csvReport = visualizer.generateCsvData({chart});
    assert(!csvReport.empty());
    assert(csvReport.find("Test Line Chart") != std::string::npos);
    
    std::cout << "✓ Visualization tests passed" << std::endl;
}

void testProfilerIntegration() {
    std::cout << "Testing ProfilerIntegration..." << std::endl;
    
    // Test basic profiler integration
    ProfilerIntegration::PerformanceConfig config;
    config.enableCoreProfiler = true;
    config.enableMemoryProfiler = true;
    config.enableThreadProfiler = true;
    config.enableBenchmarking = true;
    config.enableVisualization = true;
    
    ProfilerIntegration integration(config);
    
    // Start profiling
    integration.startProfiling("integration_test");
    assert(integration.isProfilingActive());
    
    // Record some performance data
    PerformanceProfiler::PerformanceData data;
    data.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    data.wallClockTime = 1000.0;
    data.cpuTime = 800.0;
    data.memoryUsage = 10 * 1024 * 1024;
    data.throughput = 1.0;
    
    integration.recordPerformanceData("core", data);
    
    // Get performance data
    PerformanceProfiler::PerformanceData currentData = integration.getCurrentPerformanceData();
    assert(currentData.timestamp > 0);
    assert(currentData.wallClockTime > 0);
    
    // Get memory stats
    MemoryProfiler::MemoryStats memoryStats = integration.getCurrentMemoryStats();
    assert(memoryStats.currentUsage >= 0);
    
    // Get thread stats
    ThreadProfiler::ThreadStats threadStats = integration.getCurrentThreadStats();
    assert(threadStats.totalThreads >= 0);
    
    // Stop profiling
    integration.stopProfiling();
    assert(!integration.isProfilingActive());
    
    // Generate comprehensive report
    std::string report = integration.generateComprehensiveReport();
    assert(!report.empty());
    
    std::cout << "✓ ProfilerIntegration tests passed" << std::endl;
}

void testPerformanceExperiment() {
    std::cout << "Testing PerformanceExperiment..." << std::endl;
    
    // Test basic performance experiment
    PerformanceExperiment::PerformanceExperimentConfig config;
    config.name = "test_experiment";
    config.type = PerformanceExperiment::ExperimentType::BASIC_PERFORMANCE;
    config.duration = 10000; // 10 seconds
    config.warmupDuration = 1000; // 1 second
    config.cooldownDuration = 1000; // 1 second
    config.samplingInterval = 100;
    config.warmupSamplingInterval = 100;
    
    PerformanceExperiment experiment(config);
    
    // Start experiment
    assert(experiment.start());
    assert(experiment.isActive());
    
    // Get experiment result
    const PerformanceExperiment::PerformanceExperimentResult& result = experiment.getResult();
    assert(result.experimentName == "test_experiment");
    assert(result.type == PerformanceExperiment::ExperimentType::BASIC_PERFORMANCE);
    assert(result.duration > 0);
    
    // Get experiment metrics
    const std::vector<std::pair<std::string, double>>& metrics = experiment.getMetrics();
    assert(metrics.size() >= 0);
    
    // Stop experiment
    experiment.stop();
    assert(!experiment.isActive());
    
    // Generate report
    std::string report = experiment.generateReport();
    assert(!report.empty());
    
    std::cout << "✓ PerformanceExperiment tests passed" << std::endl;
}

int main() {
    std::cout << "Running NLM Performance Tests..." << std::endl;
    std::cout << "==================================" << std::endl;
    
    try {
        testPerformanceProfiler();
        testMemoryProfiler();
        testThreadProfiler();
        testBenchmark();
        testVisualization();
        testProfilerIntegration();
        testPerformanceExperiment();
        
        std::cout << std::endl;
        std::cout << "All performance tests passed! ✓" << std::endl;
        std::cout << "==================================" << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception" << std::endl;
        return 1;
    }
}

} // namespace nlm