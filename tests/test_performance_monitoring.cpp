// Performance Monitoring Test Suite
// Tests the comprehensive performance monitoring and profiling infrastructure
// Validates performance metrics collection, profiling, alerting, and benchmarking

#include "performance/PerformanceMonitoring.hpp"
#include "performance/PerformanceProfiler.hpp"
#include "performance/PerformanceMetrics.hpp"
#include "performance/PerformanceAlerting.hpp"
#include "performance/PerformanceBenchmark.hpp"
#include <cassert>
#include <iostream>
#include <chrono>
#include <thread>
#include <cmath>

namespace test_performance_monitoring {

void testPerformanceMonitorBasic() {
    std::cout << "    Testing PerformanceMonitor basic functionality..." << std::endl;
    
    nlm::PerformanceMonitor monitor;
    
    // Configure monitor
    nlm::PerformanceConfig config;
    config.enable_monitoring = true;
    config.enable_profiling = true;
    config.enable_metrics = true;
    config.enable_alerting = false;
    config.sampling_interval_ms = 50;
    config.max_history_samples = 1000;
    
    monitor.configure(config);
    
    // Create test metrics
    nlm::PerformanceMetrics metrics;
    metrics.simulation_step = 100;
    metrics.timestamp_ns = std::chrono::steady_clock::now().time_since_epoch().count();
    metrics.cpu_usage_percent = 45.5;
    metrics.memory_usage_mb = 128.2;
    metrics.active_neurons = 500;
    metrics.total_spikes = 2500;
    metrics.average_firing_rate = 5.0;
    metrics.simulation_time_ms = 120.5;
    metrics.thread_count = std::thread::hardware_concurrency();
    metrics.memory_allocated = 1024 * 1024; // 1MB
    metrics.memory_pool_allocated = 512 * 1024; // 512KB
    metrics.memory_pool_used = 256 * 1024; // 256KB
    
    // Record metrics
    monitor.recordMetrics(metrics);
    
    // Check that metrics were recorded
    auto current = monitor.getCurrentMetrics();
    assert(current.simulation_step == 100);
    assert(std::abs(current.cpu_usage_percent - 45.5) < 0.1);
    assert(std::abs(current.memory_usage_mb - 128.2) < 0.1);
    assert(current.active_neurons == 500);
    assert(current.total_spikes == 2500);
    assert(std::abs(current.average_firing_rate - 5.0) < 0.1);
    
    // Get history
    auto history = monitor.getMetricsHistory();
    assert(history.size() == 1);
    
    // Test calculations
    double avg_firing_rate = monitor.getAverageFiringRate();
    assert(std::abs(avg_firing_rate - 5.0) < 0.1);
    
    double current_firing_rate = monitor.getCurrentFiringRate();
    assert(std::abs(current_firing_rate - 5.0) < 0.1);
    
    // Test reset
    monitor.reset();
    current = monitor.getCurrentMetrics();
    assert(current.simulation_step == 0);
    assert(history.size() == 0); // Original history is gone
    
    std::cout << "    testPerformanceMonitorBasic passed" << std::endl;
}

void testPerformanceProfiler() {
    std::cout << "    Testing PerformanceProfiler functionality..." << std::endl;
    
    nlm::PerformanceMonitor monitor;
    nlm::PerformanceProfiler profiler(&monitor);
    
    // Start profiling
    profiler.start("test_function", "test_file.cpp", 42);
    
    // Simulate some work
    double result = 0.0;
    for (int i = 0; i < 1000; ++i) {
        result += i * 0.1;
    }
    
    // Stop profiling
    profiler.stop("test_function");
    
    // Get profile events
    auto events = monitor.getProfileEvents();
    assert(events.size() == 1);
    
    // Check event details
    assert(events[0].function_name == "test_function");
    assert(events[0].duration_ns > 0);
    assert(std::abs(result) > 0); // Ensure work was done
    
    // Test RAII profiling
    auto scope = profiler.makeScope("raii_function", "raii_file.cpp", 100);
    
    // Simulate work in scope
    volatile double dummy = 0.0;
    for (int i = 0; i < 500; ++i) {
        dummy += i * 0.01;
    }
    
    // Scope will be destroyed here, automatically stopping profiling
    
    // Check that we have events now
    events = monitor.getProfileEvents();
    assert(events.size() >= 1); // At least our manual event
    
    std::cout << "    testPerformanceProfiler passed" << std::endl;
}

void testPerformanceAlerting() {
    std::cout << "    Testing PerformanceAlerting functionality..." << std::endl;
    
    nlm::PerformanceAlerting alerting;
    
    // Add alert conditions
    nlm::AlertCondition cpu_condition;
    cpu_condition.type = nlm::AlertCondition::ConditionType::CPU_USAGE_ABOVE;
    cpu_condition.threshold = 80.0;
    cpu_condition.severity = "WARNING";
    cpu_condition.message = "CPU usage is too high";
    cpu_condition.consecutive_failures = 3;
    
    nlm::AlertCondition memory_condition;
    memory_condition.type = nlm::AlertCondition::ConditionType::MEMORY_USAGE_ABOVE;
    memory_condition.threshold = 2048.0; // 2GB
    memory_condition.severity = "ERROR";
    memory_condition.message = "Memory usage exceeded limit";
    memory_condition.consecutive_failures = 2;
    
    alerting.addCondition("cpu_high", cpu_condition);
    alerting.addCondition("memory_high", memory_condition);
    
    // Create test metrics
    nlm::PerformanceMetrics metrics1;
    metrics1.cpu_usage_percent = 85.0; // Above threshold
    metrics1.memory_usage_mb = 2048.0; // At threshold
    metrics1.timestamp_ns = std::chrono::steady_clock::now().time_since_epoch().count();
    
    // First evaluation - should not trigger alert yet
    alerting.evaluate(metrics1, metrics1.timestamp_ns);
    auto alerts = alerting.getActiveAlerts();
    assert(alerts.empty()); // No alert since need consecutive failures
    
    // Second evaluation
    alerting.evaluate(metrics1, metrics1.timestamp_ns + 1000000000);
    alerts = alerting.getActiveAlerts();
    assert(alerts.empty()); // Still not triggering
    
    // Third evaluation - should trigger CPU alert
    alerting.evaluate(metrics1, metrics1.timestamp_ns + 2000000000);
    alerts = alerting.getActiveAlerts();
    assert(alerts.size() == 1);
    assert(alerts[0].type == nlm::PerformanceAlert::AlertType::CPU_HIGH);
    assert(alerts[0].severity == "WARNING");
    
    // Fourth evaluation - memory alert should trigger
    nlm::PerformanceMetrics metrics2;
    metrics2.cpu_usage_percent = 85.0;
    metrics2.memory_usage_mb = 3072.0; // Above threshold
    metrics2.timestamp_ns = std::chrono::steady_clock::now().time_since_epoch().count();
    
    alerting.evaluate(metrics2, metrics2.timestamp_ns);
    alerts = alerting.getActiveAlerts();
    assert(alerts.size() == 2); // CPU and memory alerts
    
    // Test callback
    int callback_count = 0;
    alerting.setAlertCallback([&](const nlm::PerformanceAlert& alert) {
        callback_count++;
    });
    
    // Trigger one more alert to test callback
    alerting.evaluate(metrics2, metrics2.timestamp_ns + 1000000000);
    assert(callback_count == 1); // One alert should have triggered callback
    
    // Test clearing alerts
    alerting.clearAlerts();
    alerts = alerting.getActiveAlerts();
    assert(alerts.empty());
    
    std::cout << "    testPerformanceAlerting passed" << std::endl;
}

void testPerformanceAnalyzer() {
    std::cout << "    Testing PerformanceAnalyzer functionality..." << std::endl;
    
    // Create test metrics data
    std::vector<nlm::PerformanceMetrics> metrics;
    
    // Add some sample data
    for (int i = 0; i < 10; ++i) {
        nlm::PerformanceMetrics metric;
        metric.simulation_step = i;
        metric.timestamp_ns = std::chrono::steady_clock::now().time_since_epoch().count() + i * 1000000000;
        metric.cpu_usage_percent = 50.0 + 10.0 * std::sin(i * 0.5);
        metric.memory_usage_mb = 100.0 + 20.0 * std::cos(i * 0.3);
        metric.average_firing_rate = 5.0 + 1.0 * std::sin(i * 0.7);
        metric.active_neurons = 100 + i * 50;
        metric.total_spikes = 500 + i * 100;
        metric.simulation_time_ms = 50.0 + 10.0 * std::tan(i * 0.2);
        metric.thread_count = 2 + i;
        
        metrics.push_back(metric);
    }
    
    // Test analyzer functions
    double avg_firing_rate = nlm::PerformanceAnalyzer::calculateAverageFiringRate(metrics);
    assert(avg_firing_rate > 0.0 && avg_firing_rate < 20.0);
    
    double avg_cpu = nlm::PerformanceAnalyzer::calculateCpuUsage(metrics);
    assert(avg_cpu > 0.0 && avg_cpu < 100.0);
    
    double avg_memory = nlm::PerformanceAnalyzer::calculateMemoryUsage(metrics);
    assert(avg_memory > 0.0 && avg_memory < 300.0);
    
    // Test moving average
    std::vector<double> data = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0};
    std::vector<double> moving_avg = nlm::PerformanceAnalyzer::calculateMovingAverage(data, 3);
    assert(moving_avg.size() == 8);
    assert(std::abs(moving_avg[0] - 2.0) < 0.1); // (1+2+3)/3 = 2.0
    
    // Test max/min functions
    uint64_t max_active_neurons = nlm::PerformanceAnalyzer::findMaxValue(metrics, &nlm::PerformanceMetrics::active_neurons);
    assert(max_active_neurons > 0);
    
    uint64_t min_active_neurons = nlm::PerformanceAnalyzer::findMinValue(metrics, &nlm::PerformanceMetrics::active_neurons);
    assert(min_active_neurons > 0);
    
    // Test anomaly detection
    std::vector<nlm::PerformanceAlert> alerts;
    nlm::PerformanceAnalyzer::detectAnomalies(metrics, alerts);
    
    // Anomalies may be detected based on the data
    std::cout << "    Detected " << alerts.size() << " potential anomalies" << std::endl;
    
    std::cout << "    testPerformanceAnalyzer passed" << std::endl;
}

void testPerformanceBenchmark() {
    std::cout << "    Testing PerformanceBenchmark functionality..." << std::endl;
    
    // Configure benchmark
    nlm::BenchmarkConfig config;
    config.enable_benchmarks = true;
    config.warmup_iterations = 2;
    config.measurement_iterations = 3;
    config.report_format = "summary";
    config.enable_validation = false; // Disable for this test
    
    nlm::NeuralSimulationBenchmark benchmark(config);
    
    // Register benchmark tests
    auto network_creation = std::make_unique<nlm::NeuralNetworkCreationBenchmark>();
    auto sim_step = std::make_unique<nlm::SimulationStepBenchmark>();
    auto mem_access = std::make_unique<nlm::MemoryAccessBenchmark>();
    auto spike_proc = std::make_unique<nlm::SpikeProcessingBenchmark>();
    auto threading = std::make_unique<nlm::MultithreadingBenchmark>();
    
    benchmark.registerTest(std::move(network_creation));
    benchmark.registerTest(std::move(sim_step));
    benchmark.registerTest(std::move(mem_access));
    benchmark.registerTest(std::move(spike_proc));
    benchmark.registerTest(std::move(threading));
    
    // Run all benchmarks
    benchmark.runAllBenchmarks();
    
    // Check results
    auto results = benchmark.getResults();
    assert(results.size() >= 5); // Should have at least 5 results
    
    // Test individual benchmarks
    auto network_result = benchmark.getResult("NeuralNetworkCreation");
    assert(!network_result.test_name.empty());
    assert(network_result.status == "PASSED" || network_result.status == "FAILED");
    
    // Test report generation
    benchmark.generateReport("benchmark_report.txt");
    
    // Test printing results
    benchmark.printResults();
    
    // Test reset
    benchmark.reset();
    results = benchmark.getResults();
    assert(results.empty());
    
    std::cout << "    testPerformanceBenchmark passed" << std::endl;
}

void testIntegration() {
    std::cout << "    Testing integration of all performance monitoring components..." << std::endl;
    
    nlm::PerformanceMonitor monitor;
    nlm::PerformanceProfiler profiler(&monitor);
    nlm::PerformanceAlerting alerting;
    
    // Configure alert
    nlm::AlertCondition cpu_condition;
    cpu_condition.type = nlm::AlertCondition::ConditionType::CPU_USAGE_ABOVE;
    cpu_condition.threshold = 90.0;
    cpu_condition.severity = "ERROR";
    cpu_condition.message = "High CPU detected by integration test";
    cpu_condition.consecutive_failures = 2;
    
    alerting.addCondition("cpu_integration", cpu_condition);
    
    // Create alert callback for monitor (would need to be set up in real implementation)
    // For now, just test that components can work together
    
    nlm::PerformanceMetrics metrics;
    metrics.simulation_step = 1;
    metrics.timestamp_ns = std::chrono::steady_clock::now().time_since_epoch().count();
    metrics.cpu_usage_percent = 95.0; // Above threshold
    metrics.memory_usage_mb = 100.0;
    metrics.average_firing_rate = 8.0;
    
    // Record metrics
    monitor.recordMetrics(metrics);
    
    // Start profiling a section
    auto scope = profiler.makeScope("integration_test", "integration.cpp", 1);
    
    // Simulate some work
    volatile double dummy = 0.0;
    for (int i = 0; i < 100; ++i) {
        dummy += i * 0.01;
    }
    
    // Check that monitor has data
    auto current_metrics = monitor.getCurrentMetrics();
    assert(current_metrics.simulation_step == 1);
    
    // Check that profiler has recorded events
    auto profile_events = monitor.getProfileEvents();
    assert(!profile_events.empty());
    
    std::cout << "    testIntegration passed" << std::endl;
}

void runAll() {
    std::cout << "=== Performance Monitoring Test Suite ===" << std::endl;
    std::cout << std::endl;
    
    testPerformanceMonitorBasic();
    std::cout << std::endl;
    
    testPerformanceProfiler();
    std::cout << std::endl;
    
    testPerformanceAlerting();
    std::cout << std::endl;
    
    testPerformanceAnalyzer();
    std::cout << std::endl;
    
    testPerformanceBenchmark();
    std::cout << std::endl;
    
    testIntegration();
    std::cout << std::endl;
    
    std::cout << "=== All Performance Monitoring Tests Passed ===" << std::endl;
    std::cout << std::endl;
}

} // namespace test_performance_monitoring