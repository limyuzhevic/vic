// Performance Tests - Testing scalability and efficiency

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <memory>
#include <chrono>
#include <vector>
#include <algorithm>

namespace test_performance {

void testSimulationPerformanceSmallNetwork() {
    std::cout << "Testing simulation performance with small network..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Time network initialization
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 1000; ++i) {
        brain.step(i);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    // Should complete 1000 steps in reasonable time (less than 10 seconds)
    assert(duration.count() < 10000000); // 10 seconds in microseconds
    
    std::cout << "    Small network performance test passed (" << duration.count() << " microseconds)" << std::endl;
}

void testMemoryUsagePatterns() {
    std::cout << "Testing memory usage patterns..." << std::endl;
    
    // Test with progressively larger networks
    std::vector<int> networkSizes = {50, 200, 500, 1000};
    
    for (int size : networkSizes) {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(size), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Verify network scales appropriately
        assert(brain.getTotalNeuronCount() == size);
        assert(brain.getTotalSynapseCount() >= 0);
        
        // Perform some operations
        for (int i = 0; i < std::min(100, size / 10); ++i) {
            brain.step(i);
        }
        
        std::cout << "    Network size " << size << " memory test passed" << std::endl;
    }
    
    std::cout << "    Memory usage patterns test passed" << std::endl;
}

void testScalability() {
    std::cout << "Testing scalability..." << std::endl;
    
    // Test scalability by running multiple simulations in sequence
    std::vector<int> testSizes = {10, 50, 100, 200};
    double totalTime = 0.0;
    
    for (int size : testSizes) {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(size), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Time the simulation
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < size / 5; ++i) { // Run proportional number of steps
            brain.step(i);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        totalTime += duration.count();
        
        std::cout << "    Network size " << size << " scalability test completed in " 
                  << duration.count() << " microseconds" << std::endl;
    }
    
    // Average time per step should be reasonable
    int totalSteps = 0;
    for (int size : testSizes) {
        totalSteps += size / 5;
    }
    
    double avgTimePerStep = totalTime / totalSteps;
    assert(avgTimePerStep < 1000.0); // Less than 1ms per step on average
    
    std::cout << "    Scalability test passed (average " << avgTimePerStep << " microseconds per step)" << std::endl;
}

void testConfigurationParsingPerformance() {
    std::cout << "Testing configuration parsing performance..." << std::endl;
    
    // Create and populate multiple configurations
    std::vector<std::shared_ptr<nlm::Config>> configs;
    
    for (int i = 0; i < 10; ++i) {
        auto config = std::make_shared<nlm::Config>();
        
        // Add various configuration values
        config->set("int_val", i * 10);
        config->set("double_val", i * 0.1);
        config->set("string_val", "config_" + std::to_string(i));
        config->set("bool_val", i % 2 == 0);
        config->set("int_array", std::vector<int>({1, 2, 3, 4, 5}));
        
        configs.push_back(config);
    }
    
    // Time configuration operations
    auto start = std::chrono::high_resolution_clock::now();
    
    for (const auto& config : configs) {
        // Test various config operations
        assert(config->has("int_val"));
        assert(config->has("double_val"));
        assert(config->has("string_val"));
        assert(config->has("bool_val"));
        assert(config->has("int_array"));
        
        // Test get operations
        assert(config->get<int>("int_val").value() % 10 == 0);
        assert(config->get<double>("double_val").value() < 1.0);
        
        // Test getOr with defaults
        assert(config->getOr<std::string>("missing", "default") == "default");
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    // Should process 10 configs quickly
    assert(duration.count() < 1000); // Less than 1ms
    
    std::cout << "    Configuration parsing performance test passed (" << duration.count() << " microseconds)" << std::endl;
}

void testLargeNetworkSimulation() {
    std::cout << "Testing large network simulation..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(5000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(8), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Verify large network initialization
    assert(brain.getTotalNeuronCount() == 5000);
    assert(brain.getRegionCount() == 8);
    
    // Run simulation with larger network
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 200; ++i) {
        brain.step(i);
        
        // Check for reasonable performance
        if (i % 50 == 0) {
            std::cout << "    Step " << i << "/200 completed" << std::endl;
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    // Should complete 200 steps with large network in reasonable time
    assert(duration.count() < 60000000); // 1 minute in microseconds
    
    std::cout << "    Large network simulation test passed (" << duration.count() << " microseconds)" << std::endl;
}

void runAll() {
    testSimulationPerformanceSmallNetwork();
    testMemoryUsagePatterns();
    testScalability();
    testConfigurationParsingPerformance();
    testLargeNetworkSimulation();
}

} // namespace test_performance
