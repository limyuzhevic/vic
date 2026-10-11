// Performance Stress Tests
// Tests large-scale simulations and performance under load

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <algorithm>

namespace test_performance {

void testSmallScalePerformance() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100, nlm::ConfigSource::Runtime);
    config->set("region_count", 2, nlm::ConfigSource::Runtime);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    bool initialized = brain.initialize();
    assert(initialized);
    
    // Measure performance of small simulation
    auto start = std::chrono::high_resolution_clock::now();
    
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "    Small scale (100 neurons, 100 steps): " << duration.count() << " ms" << std::endl;
    
    std::cout << "    testSmallScalePerformance passed" << std::endl;
}

void testMediumScalePerformance() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 1000, nlm::ConfigSource::Runtime);
    config->set("region_count", 5, nlm::ConfigSource::Runtime);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    bool initialized = brain.initialize();
    assert(initialized);
    
    // Measure performance of medium simulation
    auto start = std::chrono::high_resolution_clock::now();
    
    for (nlm::SimulationStep step = 0; step < 500; ++step) {
        brain.step(step);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "    Medium scale (1000 neurons, 500 steps): " << duration.count() << " ms" << std::endl;
    
    std::cout << "    testMediumScalePerformance passed" << std::endl;
}

void testLargeScalePerformance() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 10000, nlm::ConfigSource::Runtime);
    config->set("region_count", 10, nlm::ConfigSource::Runtime);
    config->set("connection_probability", 0.01, nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    bool initialized = brain.initialize();
    assert(initialized);
    
    // Measure performance of large simulation
    auto start = std::chrono::high_resolution_clock::now();
    
    for (nlm::SimulationStep step = 0; step < 1000; ++step) {
        brain.step(step);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "    Large scale (10000 neurons, 1000 steps): " << duration.count() << " ms" << std::endl;
    
    std::cout << "    testLargeScalePerformance passed" << std::endl;
}

void testExtendedSimulation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 1000, nlm::ConfigSource::Runtime);
    config->set("region_count", 5, nlm::ConfigSource::Runtime);
    config->set("connection_probability", 0.05, nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    bool initialized = brain.initialize();
    assert(initialized);
    
    // Run extended simulation
    auto start = std::chrono::high_resolution_clock::now();
    
    for (nlm::SimulationStep step = 0; step < 10000; ++step) {
        brain.step(step);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);
    
    std::cout << "    Extended (1000 neurons, 10000 steps): " << duration.count() << " seconds" << std::endl;
    
    std::cout << "    testExtendedSimulation passed" << std::endl;
}

void testPerformanceConsistency() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 500, nlm::ConfigSource::Runtime);
    config->set("region_count", 3, nlm::ConfigSource::Runtime);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    bool initialized = brain.initialize();
    assert(initialized);
    
    // Run simulation multiple times to check consistency
    std::vector<double> durations;
    
    for (int run = 0; run < 5; ++run) {
        auto start = std::chrono::high_resolution_clock::now();
        
        for (nlm::SimulationStep step = 0; step < 200; ++step) {
            brain.step(step);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        durations.push_back(static_cast<double>(duration));
    }
    
    // Calculate average and variance
    double sum = std::accumulate(durations.begin(), durations.end(), 0.0);
    double avg = sum / durations.size();
    
    double sq_sum = 0.0;
    for (double d : durations) {
        sq_sum += (d - avg) * (d - avg);
    }
    double stddev = std::sqrt(sq_sum / durations.size());
    
    std::cout << "    Performance consistency: avg=" << avg << "ms, stddev=" << stddev << "ms" << std::endl;
    
    std::cout << "    testPerformanceConsistency passed" << std::endl;
}

void runAll() {
    testSmallScalePerformance();
    testMediumScalePerformance();
    testLargeScalePerformance();
    testExtendedSimulation();
    testPerformanceConsistency();
    
    std::cout << "Performance stress tests completed successfully!" << std::endl;
}

} // namespace test_performance
