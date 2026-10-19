// Performance Tests
// Tests for brain performance, memory usage, and efficiency

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <chrono>
#include <vector>

namespace test_performance {

void testBrainPerformance() {
    std::cout << "    Testing brain performance..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("random_seed", 42);
    
    nlm::Brain brain(config);
    
    if (!brain.initialize()) {
        std::cerr << "Failed to initialize brain for performance test" << std::endl;
        assert(false);
    }
    
    // Test performance with different neuron counts
    std::vector<size_t> neuronCounts = {100, 500, 1000};
    
    for (size_t neuronCount : neuronCounts) {
        config->set("neuron_count", static_cast<int64_t>(neuronCount));
        
        auto perfBrain = std::make_shared<nlm::Brain>(config);
        assert(perfBrain->initialize());
        
        // Measure initialization time
        auto start = std::chrono::high_resolution_clock::now();
        
        for (nlm::SimulationStep step = 0; step < 10; ++step) {
            perfBrain->step(step);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        std::cout << "    Performance with " << neuronCount 
                  << " neurons: " << duration.count() << " microseconds for 10 steps" 
                  << std::endl;
        
        // Performance should be reasonable (less than 1 second for 10 steps)
        assert(duration.count() < 1000000); // 1 second
    }
    
    std::cout << "    Brain performance test passed" << std::endl;
}

void testMemoryUsage() {
    std::cout << "    Testing memory usage..." << std::endl;
    
    // Test that memory usage grows predictably with brain size
    std::vector<size_t> neuronCounts = {50, 200, 500};
    
    for (size_t neuronCount : neuronCounts) {
        auto config = std::make_shared<nlm::Config>();
        config->set("random_seed", 42);
        config->set("neuron_count", static_cast<int64_t>(neuronCount));
        config->set("region_count", 1);
        
        nlm::Brain brain(config);
        
        if (brain.initialize()) {
            size_t memoryNeurons = brain.getTotalNeuronCount();
            size_t memorySynapses = brain.getTotalSynapseCount();
            
            std::cout << "    Memory usage with " << neuronCount 
                      << " neurons: " << memoryNeurons 
                      << " neurons, " << memorySynapses << " synapses" 
                      << std::endl;
            
            // Memory should scale roughly linearly with neuron count
            assert(memoryNeurons >= neuronCount * 0.9); // Allow some tolerance
        }
    }
    
    std::cout << "    Memory usage test passed" << std::endl;
}

void testStepPerformance() {
    std::cout << "    Testing step performance..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("random_seed", 42);
    config->set("neuron_count", 200);
    config->set("region_count", 1);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Measure performance of individual steps
    auto start = std::chrono::high_resolution_clock::now();
    
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "    Step performance: " << duration.count() / 100 << " microseconds per step" << std::endl;
    
    // Each step should be reasonably fast
    assert(duration.count() < 1000000); // 1 second for 100 steps
    
    std::cout << "    Step performance test passed" << std::endl;
}

void testScalability() {
    std::cout << "    Testing scalability..." << std::endl;
    
    // Test that brain performance scales reasonably with size
    std::vector<std::pair<size_t, std::string>> testCases = {
        {50, "small"},
        {200, "medium"},
        {500, "large"}
    };
    
    for (auto& testCase : testCases) {
        size_t neuronCount = testCase.first;
        std::string sizeLabel = testCase.second;
        
        auto config = std::make_shared<nlm::Config>();
        config->set("random_seed", 42);
        config->set("neuron_count", static_cast<int64_t>(neuronCount));
        config->set("region_count", 1);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Measure memory efficiency
        size_t neurons = brain.getTotalNeuronCount();
        size_t synapses = brain.getTotalSynapseCount();
        
        std::cout << "    Scalability test " << sizeLabel << ": "
                  << neurons << " neurons, " << synapses << " synapses" << std::endl;
        
        // Efficiency should be reasonable (not too many synapses per neuron)
        if (neurons > 0) {
            float synPerNeuron = static_cast<float>(synapses) / neurons;
            assert(synPerNeuron < 10.0f); // Reasonable synapse density
        }
    }
    
    std::cout << "    Scalability test passed" << std::endl;
}

void testRealTimePerformance() {
    std::cout << "    Testing real-time performance..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("random_seed", 42);
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    config->set("simulation_timestep", 0.001);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test real-time simulation
    auto start = std::chrono::high_resolution_clock::now();
    
    for (nlm::SimulationStep step = 0; step < 1000; ++step) {
        brain.step(step);
        
        // Real-time constraint: should complete step within reasonable time
        auto current = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(current - start);
        
        // Step should not take too long (allow 10ms per step for real-time)
        assert(elapsed.count() < step * 10); // Each step should take less than 10ms
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto totalDuration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "    Real-time performance: " << totalDuration.count() << " ms for 1000 steps" << std::endl;
    std::cout << "    Real-time performance test passed" << std::endl;
}

} // namespace test_performance
