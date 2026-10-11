// NLM Testing Framework
// Performance Testing Example

#include "framework/TestFramework.hpp"
#include "framework/TestFixtures.hpp"
#include "brain/PerformanceProfiler.hpp"
#include <cassert>
#include <chrono>

class ExamplePerformanceTest : public TestCase {
public:
    ExamplePerformanceTest() : TestCase("ExamplePerformanceTest") {}
    
    void runTest() override {
        // Example: Test system performance
        PerformanceProfiler profiler;
        
        // Test 1: Memory performance
        profiler.startProfile("memory_performance");
        
        std::vector<std::vector<double>> large_matrix(1000, std::vector<double>(1000, 0.0));
        
        // Process matrix
        for (int i = 0; i < 1000; ++i) {
            for (int j = 0; j < 1000; ++j) {
                large_matrix[i][j] = std::sin(i * 0.001) * std::cos(j * 0.001);
            }
        }
        
        // Access matrix
        double matrix_sum = 0.0;
        for (int i = 0; i < 1000; ++i) {
            for (int j = 0; j < 1000; ++j) {
                matrix_sum += large_matrix[i][j];
            }
        }
        
        profiler.endProfile();
        
        auto memory_metrics = profiler.getMetrics("memory_performance");
        assert(memory_metrics.memory_usage > 0);
        assert(memory_metrics.duration.count() > 0);
        
        // Test 2: CPU performance
        profiler.startProfile("cpu_performance");
        
        double cpu_result = 0.0;
        for (int i = 0; i < 10000000; ++i) {
            cpu_result += std::sin(i * 0.000001) * std::cos(i * 0.000001);
        }
        
        profiler.endProfile();
        
        auto cpu_metrics = profiler.getMetrics("cpu_performance");
        assert(cpu_metrics.cpu_cycles > 0);
        assert(cpu_metrics.gflops > 0);
        assert(cpu_metrics.duration.count() > 0);
        
        // Test 3: Network performance
        profiler.startProfile("network_performance");
        
        std::vector<double> network_data(1000000, 0.0);
        for (size_t i = 0; i < network_data.size(); ++i) {
            network_data[i] = std::sin(i * 0.000001);
        }
        
        // Process network data
        std::vector<double> processed_data(network_data.size());
        for (size_t i = 0; i < network_data.size(); ++i) {
            processed_data[i] = network_data[i] * 1.1 + 0.5;
        }
        
        profiler.endProfile();
        
        auto network_metrics = profiler.getMetrics("network_performance");
        assert(network_metrics.duration.count() > 0);
        
        // Test 4: Integration performance
        profiler.startProfile("integration_performance");
        
        TestFramework framework;
        BrainSystemFixture brain_fixture(framework, "PerformanceTest");
        NeuromodulationFixture neuromod_fixture(framework, "NeuromodulationTest");
        
        Brain& brain = brain_fixture.getBrain();
        NeuromodulationSystem& neuromod = neuromod_fixture.getNeuromodulation();
        
        // Simulate integration performance
        std::vector<double> integration_input = {0.5, 0.6, 0.7, 0.8, 0.9};
        std::vector<double> integration_output = brain.processSensoryInput(integration_input);
        neuromod.processSignals(integration_output);
        
        profiler.endProfile();
        
        auto integration_metrics = profiler.getMetrics("integration_performance");
        assert(integration_metrics.duration.count() > 0);
        assert(std::isfinite(integration_metrics.gflops));
        
        // Test 5: Combined performance
        profiler.startProfile("combined_performance");
        
        double combined_result = 0.0;
        for (int i = 0; i < 5000000; ++i) {
            combined_result += std::sin(i * 0.0000001) * std::cos(i * 0.0000001);
        }
        
        profiler.endProfile();
        
        auto combined_metrics = profiler.getMetrics("combined_performance");
        assert(combined_metrics.cpu_cycles > 0);
        assert(combined_metrics.gflops > 0);
        assert(combined_metrics.duration.count() > 0);
        
        // Test 6: Performance regression detection
        profiler.startProfile("regression_test");
        
        // Run performance test with baseline comparison
        std::vector<double> baseline_times = {100.0, 105.0, 110.0, 108.0, 112.0}; // ms
        double current_time = combined_metrics.duration.count();
        
        // Check for performance regression
        bool regression_detected = false;
        for (double baseline_time : baseline_times) {
            if (current_time > baseline_time * 1.5) { // 50% slower
                regression_detected = true;
                break;
            }
        }
        
        assert(!regression_detected); // Should not detect regression
    }
};

// Example usage
void example_performance_test_usage() {
    TestFramework framework;
    ExamplePerformanceTest test;
    
    framework.registerTest(test);
    framework.executeTest(test);
    framework.reportResults();
}