// Performance Monitoring Infrastructure Tests
// Phase 1: Core Infrastructure Tests

#include "Monitoring.hpp"
#include "PerformanceProfiler.hpp"
#include "MetricCollector.hpp"
#include "PerformanceReporter.hpp"
#include <cassert>
#include <iostream>
#include <thread>
#include <chrono>

namespace test_performance_monitoring {

void testMonitoringSingleton() {
    std::cout << "Testing Performance Monitor Singleton..." << std::endl;
    
    // Test singleton pattern
    auto& monitor1 = PerformanceMonitor::getInstance();
    auto& monitor2 = PerformanceMonitor::getInstance();
    
    assert(&monitor1 == &monitor2);
    std::cout << "  ✓ Singleton pattern works" << std::endl;
}

void testMetricRecording() {
    std::cout << "Testing Metric Recording..." << std::endl;
    
    auto& monitor = PerformanceMonitor::getInstance();
    
    // Record various metric types
    monitor.recordMetric("test_gauge", 42.5, MetricType::Gauge, "test");
    monitor.recordMetric("test_counter", 100, MetricType::Counter, "test");
    monitor.recordMetric("test_timer", 1.5, MetricType::Timer, "test");
    
    // Record timing
    auto start = std::chrono::high_resolution_clock::now();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    auto end = std::chrono::high_resolution_clock::now();
    monitor.recordTiming("sleep_duration", end - start, "test");
    
    std::cout << "  ✓ Metrics recorded successfully" << std::endl;
}

void testComponentRegistration() {
    std::cout << "Testing Component Registration..." << std::endl;
    
    auto& monitor = PerformanceMonitor::getInstance();
    
    // Register components
    monitor.registerComponent("brain", ComponentType::Brain);
    monitor.registerComponent("memory", ComponentType::Memory);
    monitor.registerComponent("environment", ComponentType::Environment);
    
    // Test metric recording with components
    monitor.recordMetric("neuron_activity", 0.75, MetricType::Gauge, "brain");
    monitor.recordMetric("memory_usage", 0.5, MetricType::Gauge, "memory");
    monitor.recordMetric("environment_state", 1.0, MetricType::Gauge, "environment");
    
    std::cout << "  ✓ Component registration and metrics work" << std::endl;
}

void testProfilingScope() {
    std::cout << "Testing Profiling Scope..." << std::endl;
    
    auto& monitor = PerformanceMonitor::getInstance();
    
    {
        auto scope = monitor.profile("test_operation", "test");
        
        // Simulate work
        double sum = 0.0;
        for (int i = 0; i < 1000; ++i) {
            sum += i * 0.1;
        }
        (void)sum; // Avoid unused variable warning
    }
    
    std::cout << "  ✓ Profiling scope works" << std::endl;
}

void testPerformanceProfiler() {
    std::cout << "Testing Performance Profiler..." << std::endl;
    
    auto& profiler = PerformanceProfiler::getInstance();
    
    // Test template profiling
    auto result = profiler.profile("fibonacci", []() {
        long long fib(int n) {
            return n <= 1 ? n : fib(n-1) + fib(n-2);
        }
        return fib(10);
    });
    
    assert(result == 55);
    
    // Test manual profiling
    auto scope = profiler.createManualProfile("manual_test", "test");
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    scope.complete();
    
    std::cout << "  ✓ Performance profiler works" << std::endl;
}

void testMetricCollector() {
    std::cout << "Testing Metric Collector..." << std::endl;
    
    auto& collector = MetricCollector::getInstance();
    
    // Add metrics
    collector.addMetric("test_metric", 10.5, "test_component");
    collector.addMetric("test_metric", 20.5, "test_component");
    collector.addMetric("test_metric", 15.5, "test_component");
    
    // Calculate statistics
    MetricStats stats = collector.calculateStats("test_metric", std::chrono::hours(1));
    
    assert(stats.count == 3);
    assert(stats.min == 10.5);
    assert(stats.max == 20.5);
    assert(stats.mean > 0);
    
    std::cout << "  ✓ Metric collector works" << std::endl;
}

void testPerformanceReporter() {
    std::cout << "Testing Performance Reporter..." << std::endl;
    
    PerformanceReporter reporter;
    
    // Test report generation
    auto& monitor = PerformanceMonitor::getInstance();
    monitor.recordMetric("report_test", 99.9, MetricType::Gauge, "test");
    
    std::string textReport = reporter.generateDetailedReport("report_test");
    assert(!textReport.empty());
    assert(textReport.find("report_test") != std::string::npos);
    
    std::string jsonReport = reporter.generateJSONReport("report_test");
    assert(!jsonReport.empty());
    assert(jsonReport.find("report_test") != std::string::npos);
    
    std::string csvReport = reporter.generateCSVReport("report_test");
    assert(!csvReport.empty());
    assert(csvReport.find("report_test") != std::string::npos);
    
    std::string htmlReport = reporter.generateHTMLReport("report_test");
    assert(!htmlReport.empty());
    assert(htmlReport.find("report_test") != std::string::npos);
    
    std::cout << "  ✓ Performance reporter works" << std::endl;
}

void testIntegrationAdapters() {
    std::cout << "Testing Integration Adapters..." << std::endl;
    
    // Test BrainPerformanceAdapter
    BrainPerformanceAdapter brainAdapter(nullptr);
    brainAdapter.setComponentName("test_brain");
    assert(brainAdapter.getComponentName() == "test_brain");
    
    // Test MemoryPerformanceAdapter
    MemoryPerformanceAdapter memoryAdapter(nullptr, nullptr);
    memoryAdapter.setComponentName("test_memory");
    assert(memoryAdapter.getComponentName() == "test_memory");
    
    // Test EnvironmentPerformanceAdapter
    EnvironmentPerformanceAdapter envAdapter(nullptr);
    envAdapter.setComponentName("test_environment");
    assert(envAdapter.getComponentName() == "test_environment");
    
    std::cout << "  ✓ Integration adapters work" << std::endl;
}

void testConfiguration() {
    std::cout << "Testing Configuration..." << std::endl;
    
    PerformanceReporter reporter;
    
    // Test configuration
    ReportConfig config;
    config.includeTimestamps = true;
    config.includeComponents = true;
    config.exportRawData = true;
    config.exportStatistics = true;
    config.exportAnomalies = true;
    
    reporter.setReportConfig(config);
    
    ReportConfig retrievedConfig = reporter.getReportConfig();
    assert(retrievedConfig.includeTimestamps == true);
    assert(retrievedConfig.includeComponents == true);
    assert(retrievedConfig.exportRawData == true);
    
    std::cout << "  ✓ Configuration works" << std::endl;
}

void testUtilities() {
    std::cout << "Testing Utilities..." << std::endl;
    
    // Test Timer
    HighResTimer timer("test_timer");
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    double elapsedSeconds = timer.elapsedSeconds();
    assert(elapsedSeconds > 0.005);
    
    // Test ManualTimer
    ManualTimer manualTimer("manual_test");
    double measuredTime = manualTimer.measure([]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    });
    assert(measuredTime > 0.002);
    
    // Test PerformanceCounter
    PerformanceCounter counter;
    counter.increment("test_counter", 5);
    counter.increment("test_counter", 3);
    assert(counter.get("test_counter") == 8);
    
    // Test StatsCalculator
    std::vector<double> values = {1.0, 2.0, 3.0, 4.0, 5.0};
    double mean = StatsCalculator::calculateMean(values);
    assert(mean == 3.0);
    
    double variance = StatsCalculator::calculateVariance(values);
    assert(variance > 0);
    
    std::cout << "  ✓ Utilities work" << std::endl;
}

void runAllTests() {
    std::cout << "=== NLM Performance Monitoring Tests ===" << std::endl;
    std::cout << std::endl;
    
    // Run individual tests
    testMonitoringSingleton();
    testMetricRecording();
    testComponentRegistration();
    testProfilingScope();
    testPerformanceProfiler();
    testMetricCollector();
    testPerformanceReporter();
    testIntegrationAdapters();
    testConfiguration();
    testUtilities();
    
    std::cout << std::endl;
    std::cout << "=== All Performance Monitoring Tests PASSED ===" << std::endl;
}

} // namespace test_performance_monitoring

int main() {
    try {
        test_performance_monitoring::runAllTests();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception" << std::endl;
        return 1;
    }
}
