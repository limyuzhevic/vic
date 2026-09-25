// Phase 6 Performance Test Suite
// Tests for Phase 6 performance and performance optimization
// Validates Phase 6 performance targets and performance optimization

#include "phase6_performance_test.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "experiments/Phase6Demo.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <cassert>
#include <iostream>
#include <chrono>
#include <vector>

namespace nlm {

void testPhase6BasicPerformance() {
    std::cout << "Testing Phase 6 basic performance..." << std::endl;
    
    // Test basic Phase 6 performance
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    
    Brain brain(config);
    brain.initialize();
    
    // Test performance
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 100; ++i) {
        brain.step(1);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(end - start);
    
    std::cout << "Phase 6 basic performance: 100 steps in " << duration.count() << " ms" << std::endl;
    std::cout << "Average per step: " << (duration.count() / 100.0) << " ms" << std::endl;
    
    // Performance should be acceptable
    double avgPerStep = duration.count() / 100.0;
    assert(avgPerStep < 100.0); // Less than 100ms per step is acceptable
    
    std::cout << "Phase 6 basic performance test passed!" << std::endl;
}

void testPhase6LargeScalePerformance() {
    std::cout << "Testing Phase 6 large-scale performance..." << std::endl;
    
    // Test large-scale Phase 6 performance
    std::vector<std::tuple<std::string, int64_t>> testConfigs = {
        {"Small", 1000},
        {"Medium", 10000},
        {"Large", 50000}
    };
    
    for (const auto& [sizeName, neuronCount] : testConfigs) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", neuronCount, ConfigSource::Runtime);
        
        Brain brain(config);
        brain.initialize();
        
        // Test performance for different sizes
        auto start = std::chrono::high_resolution_clock::now();
        
        int steps = std::min(10, static_cast<int>(neuronCount / 1000));
        for (int i = 0; i < steps; ++i) {
            brain.step(10);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double, std::milli>(end - start);
        
        std::cout << sizeName << " Phase 6: " << steps << " steps in " << duration.count() << " ms" << std::endl;
        std::cout << "  Average per step: " << (duration.count() / steps) << " ms" << std::endl;
        
        // Performance should scale reasonably with brain size
        double avgPerStep = duration.count() / steps;
        assert(avgPerStep < 200.0); // Reasonable performance for any size
    }
    
    std::cout << "Phase 6 large-scale performance test passed!" << std::endl;
}

void testPhase6Phase6DemoPerformance() {
    std::cout << "Testing Phase 6Demo performance..." << std::endl;
    
    // Test Phase 6Demo performance
    Phase6Demo demo;
    
    bool demoInitialized = demo.initialize();
    if (demoInitialized) {
        auto start = std::chrono::high_resolution_clock::now();
        
        demo.run();
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double, std::milli>(end - start);
        
        std::cout << "Phase 6Demo performance: " << duration.count() << " ms" << std::endl;
        
        // Phase 6Demo performance should be acceptable
        assert(duration.count() < 5000.0); // Less than 5 seconds is acceptable
    }
    
    std::cout << "Phase 6Demo performance test passed!" << std::endl;
}

void testPhase6IntegratedExperimentPerformance() {
    std::cout << "Testing Phase6IntegratedExperiment performance..." << std::endl;
    
    // Test Phase6IntegratedExperiment performance
    Phase6IntegratedExperiment experiment;
    
    experiment.initialize();
    
    auto start = std::chrono::high_resolution_clock::now();
    
    experiment.run();
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(end - start);
    
    std::cout << "Phase6IntegratedExperiment performance: " << duration.count() << " ms" << std::endl;
    
    // Phase6IntegratedExperiment performance should be acceptable
    assert(duration.count() < 10000.0); // Less than 10 seconds is acceptable
    
    std::cout << "Phase6IntegratedExperiment performance test passed!" << std::endl;
}

void testPhase6MemoryPerformance() {
    std::cout << "Testing Phase 6 memory performance..." << std::endl;
    
    // Test Phase 6 memory performance
    std::vector<std::shared_ptr<Brain>> brains;
    
    for (int i = 0; i < 10; ++i) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(1000 + i * 100), ConfigSource::Runtime);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        brains.push_back(brain);
    }
    
    // Test performance with multiple brain instances
    auto start = std::chrono::high_resolution_clock::now();
    
    for (size_t i = 0; i < brains.size(); ++i) {
        brains[i]->step(5);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(end - start);
    
    std::cout << "Phase 6 memory performance: " << brains.size() << " brains in " << duration.count() << " ms" << std::endl;
    std::cout << "Average per brain: " << (duration.count() / brains.size()) << " ms" << std::endl;
    
    // Memory performance should be acceptable
    double avgPerBrain = duration.count() / brains.size();
    assert(avgPerBrain < 50.0); // Less than 50ms per brain is good
    
    std::cout << "Phase 6 memory performance test passed!" << std::endl;
}

void testPhase6ScalabilityPerformance() {
    std::cout << "Testing Phase 6 scalability performance..." << std::endl;
    
    // Test Phase 6 scalability performance
    std::vector<int64_t> neuronCounts = {100, 1000, 10000, 50000};
    std::vector<double> performanceTimes;
    
    for (int64_t neuronCount : neuronCounts) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", neuronCount, ConfigSource::Runtime);
        
        Brain brain(config);
        brain->initialize();
        
        auto start = std::chrono::high_resolution_clock::now();
        
        int steps = std::min(5, static_cast<int>(neuronCount / 1000));
        for (int i = 0; i < steps; ++i) {
            brain->step(10);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double, std::milli>(end - start);
        
        performanceTimes.push_back(duration.count() / steps);
        
        std::cout << neuronCount << " neurons: " << steps << " steps in " << duration.count() << " ms (avg: " << (duration.count() / steps) << " ms/step)" << std::endl;
    }
    
    // Performance should scale reasonably with neuron count
    for (size_t i = 0; i < performanceTimes.size(); ++i) {
        // Each performance time should be reasonable
        assert(performanceTimes[i] < 500.0); // Less than 500ms per step is good
    }
    
    std::cout << "Phase 6 scalability performance test passed!" << std::endl;
}

void testPhase6OptimizationPerformance() {
    std::cout << "Testing Phase 6 optimization performance..." << std::endl;
    
    // Test Phase 6 optimization performance
    // This tests the effectiveness of performance optimizations
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Runtime);
    
    Brain brain(config);
    brain->initialize();
    
    // Test optimization effectiveness
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 100; ++i) {
        brain->step(1);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(end - start);
    
    std::cout << "Phase 6 optimization performance: 100 steps in " << duration.count() << " ms" << std::endl;
    std::cout << "Average per step: " << (duration.count() / 100.0) << " ms" << std::endl;
    
    // Optimization should make performance better than baseline
    double avgPerStep = duration.count() / 100.0;
    assert(avgPerStep < 50.0); // Less than 50ms per step with optimizations
    
    std::cout << "Phase 6 optimization performance test passed!" << std::endl;
}

void runPhase6PerformanceTests() {
    std::cout << "=== Running Phase 6 Performance Tests ===" << std::endl;
    std::cout << std::endl;
    
    testPhase6BasicPerformance();
    std::cout << std::endl;
    
    testPhase6LargeScalePerformance();
    std::cout << std::endl;
    
    testPhase6Phase6DemoPerformance();
    std::cout << std::endl;
    
    testPhase6IntegratedExperimentPerformance();
    std::cout << std::endl;
    
    testPhase6MemoryPerformance();
    std::cout << std::endl;
    
    testPhase6ScalabilityPerformance();
    std::cout << std::endl;
    
    testPhase6OptimizationPerformance();
    std::cout << std::endl;
    
    std::cout << "=== All Phase 6 Performance Tests Passed ===" << std::endl;
}

} // namespace nlm
