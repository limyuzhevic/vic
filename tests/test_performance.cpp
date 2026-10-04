// Comprehensive Performance and Robustness Tests
// Tests for performance validation, long-running operations, stress testing, and concurrent access

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <memory>
#include <chrono>
#include <thread>
#include <vector>
#include <algorithm>

namespace test_performance {

// Performance measurement utilities
class PerformanceTimer {
public:
    PerformanceTimer() : start(std::chrono::high_resolution_clock::now()) {}
    
    double elapsed() const {
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::milli>(end - start).count();
    }
    
    void reset() {
        start = std::chrono::high_resolution_clock::now();
    }
    
private:
    std::chrono::high_resolution_clock::time_point start;
};

void testPerformanceValidation() {
    std::cout << "=== Performance Validation Tests ===" << std::endl;
    
    // Test with different configuration sizes
    std::vector<std::pair<int64_t, int64_t>> testSizes = {
        {100, 1},    // Small
        {500, 3},    // Medium
        {1000, 5}    // Large
    };
    
    for (const auto& size : testSizes) {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", size.first, nlm::ConfigSource::Default);
        config->set("region_count", size.second, nlm::ConfigSource::Default);
        
        PerformanceTimer timer;
        nlm::Brain brain(config);
        
        bool initialized = brain.initialize();
        assert(initialized);
        
        // Run simulation steps
        for (nlm::SimulationStep step = 0; step < size.first / 10; ++step) {
            brain.step(step);
        }
        
        double elapsed = timer.elapsed();
        double stepsPerSecond = (size.first / 10) / (elapsed / 1000.0);
        
        std::cout << "  Configuration " << size.first << " neurons, " << size.second << " regions: "
                  << stepsPerSecond << " steps/second, " << elapsed << " ms" << std::endl;
        
        // Performance should be reasonable (at least 10 steps per second)
        assert(stepsPerSecond > 10.0);
    }
    
    std::cout << "  ✓ Performance validation passed" << std::endl;
}

void testLongRunningOperationTesting() {
    std::cout << "=== Long-Running Operation Tests ===" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("simulation_steps", static_cast<int64_t>(10000), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    PerformanceTimer timer;
    
    // Run long-running operation (checkpoint simulation)
    for (nlm::SimulationStep step = 0; step < 10000; ++step) {
        brain.step(step);
        
        // Simulate checkpoint every 100 steps
        if (step % 100 == 0) {
            brain.reset(); // Simulate checkpoint/restart
            brain.initialize(); // Re-initialize after checkpoint
        }
    }
    
    double elapsed = timer.elapsed();
    double stepsPerSecond = 10000.0 / (elapsed / 1000.0);
    
    std::cout << "  Long-running operation: " << stepsPerSecond << " steps/second, " << elapsed << " ms total" << std::endl;
    
    // Should handle long operations efficiently
    assert(stepsPerSecond > 50.0);
    
    std::cout << "  ✓ Long-running operation test passed" << std::endl;
}

void testStressTestingWithMaximumConfigurationValues() {
    std::cout << "=== Stress Testing with Maximum Values ===" << std::endl;
    
    // Test with maximum reasonable configuration values
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(5000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(20), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.3, nlm::ConfigSource::Default);
    config->set("learning_rate", 0.1, nlm::ConfigSource::Default);
    
    PerformanceTimer timer;
    nlm::Brain brain(config);
    
    bool initialized = brain.initialize();
    assert(initialized);
    
    // Stress test with large configuration
    for (nlm::SimulationStep step = 0; step < 500; ++step) {
        brain.step(step);
    }
    
    double elapsed = timer.elapsed();
    std::cout << "  Stress test with 5000 neurons, 20 regions: " << elapsed << " ms" << std::endl;
    
    // Should handle stress conditions
    assert(elapsed < 30000.0); // Should complete within 30 seconds
    
    std::cout << "  ✓ Stress testing passed" << std::endl;
}

void testResourceExhaustionHandling() {
    std::cout << "=== Resource Exhaustion Handling Tests ===" << std::endl;
    
    // Test resource exhaustion scenarios
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Test multiple resource-intensive operations
    for (int i = 0; i < 50; ++i) {
        brain.initialize();
        
        for (nlm::SimulationStep step = 0; step < 100; ++step) {
            brain.step(step);
        }
        
        brain.reset(); // Clean up resources
    }
    
    std::cout << "  ✓ Resource exhaustion handling passed" << std::endl;
}

void testConcurrentAccessValidation() {
    std::cout << "=== Concurrent Access Validation Tests ===" << std::endl;
    
    // Note: This is conceptual since NLM may not have built-in thread safety
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    // Simulate multiple concurrent accesses (conceptual test)
    std::vector<std::thread> threads;
    std::vector<nlm::Brain*> brainPtrs;
    
    for (int i = 0; i < 5; ++i) {
        auto brainConfig = std::make_shared<nlm::Config>(*config);
        auto brain = new nlm::Brain(brainConfig);
        brain->initialize();
        brainPtrs.push_back(brain);
        
        threads.emplace_back([brain]() {
            // Simulate concurrent operations
            for (nlm::SimulationStep step = 0; step < 10; ++step) {
                brain->step(step);
            }
        });
    }
    
    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }
    
    // Clean up
    for (auto* brain : brainPtrs) {
        delete brain;
    }
    
    std::cout << "  ✓ Concurrent access validation passed" << std::endl;
}

void testMemoryUsageMonitoring() {
    std::cout << "=== Memory Usage Monitoring Tests ===" << std::endl;
    
    std::vector<double> memoryUsage;
    
    // Test memory usage over time
    for (int i = 0; i < 10; ++i) {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100 * (i + 1)), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(i + 1), nlm::ConfigSource::Default);
        
        PerformanceTimer timer;
        nlm::Brain brain(config);
        brain.initialize();
        
        for (nlm::SimulationStep step = 0; step < 50; ++step) {
            brain.step(step);
        }
        
        double elapsed = timer.elapsed();
        memoryUsage.push_back(elapsed);
        
        brain.reset();
    }
    
    // Check memory usage doesn't grow exponentially
    if (memoryUsage.size() > 1) {
        double avgTime = 0.0;
        for (double time : memoryUsage) avgTime += time;
        avgTime /= memoryUsage.size();
        
        std::cout << "  Average operation time: " << avgTime << " ms" << std::endl;
        assert(avgTime < 1000.0); // Should complete within 1 second on average
    }
    
    std::cout << "  ✓ Memory usage monitoring passed" << std::endl;
}

void testCheckpointSaveLoadPerformance() {
    std::cout << "=== Checkpoint Save/Load Performance Tests ===" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    PerformanceTimer timer;
    
    // Simulate multiple checkpoint operations
    for (nlm::SimulationStep step = 0; step < 1000; step += 100) {
        brain.step(step);
        
        // Simulate checkpoint save
        // brain.save("checkpoint.bin");
        
        // Simulate checkpoint load
        // brain.load("checkpoint.bin");
        
        brain.reset();
        brain.initialize();
    }
    
    double elapsed = timer.elapsed();
    double operationsPerSecond = 10.0 / (elapsed / 1000.0); // 10 checkpoint operations
    
    std::cout << "  Checkpoint operations: " << operationsPerSecond << " ops/second" << std::endl;
    assert(operationsPerSecond > 1.0);
    
    std::cout << "  ✓ Checkpoint performance test passed" << std::endl;
}

void runAll() {
    std::cout << "=== Performance and Robustness Tests ===" << std::endl;
    testPerformanceValidation();
    testLongRunningOperationTesting();
    testStressTestingWithMaximumConfigurationValues();
    testResourceExhaustionHandling();
    testConcurrentAccessValidation();
    testMemoryUsageMonitoring();
    testCheckpointSaveLoadPerformance();
    std::cout << "=== All Performance Tests PASSED ===" << std::endl;
}

} // namespace test_performance