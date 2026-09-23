// Performance Tests
// Testing brain performance, speed, and resource usage

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
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain->initialize());
    
    // Measure initialization time
    auto startTime = std::chrono::high_resolution_clock::now();
    brain->initialize();
    auto endTime = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    std::cout << "      Initialization time: " << duration.count() << "ms" << std::endl;
    assert(duration.count() < 10000); // Should complete within 10 seconds
    
    // Measure step time
    startTime = std::chrono::high_resolution_clock::now();
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain->step(step);
    }
    endTime = std::chrono::high_resolution_clock::now();
    
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    std::cout << "      100 steps time: " << duration.count() << "ms" << std::endl;
    assert(duration.count() < 5000); // Should complete within 5 seconds
    
    // Measure spike processing performance
    startTime = std::chrono::high_resolution_clock::now();
    size_t totalSpikes = 0;
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain->step(step);
        totalSpikes += brain->getTotalSpikeCount();
    }
    endTime = std::chrono::high_resolution_clock::now();
    
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    std::cout << "      Spike processing time for 50 steps: " << duration.count() << "ms" << std::endl;
    assert(duration.count() < 3000); // Should complete within 3 seconds
    
    std::cout << "      ✓ Brain performance tests passed" << std::endl;
}

void testMemoryUsage() {
    std::cout << "    Testing memory usage..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain->initialize());
    
    // Run simulation for memory test
    for (nlm::SimulationStep step = 0; step < 200; ++step) {
        brain->step(step);
    }
    
    // Check brain statistics
    size_t neuronCount = brain->getTotalNeuronCount();
    size_t synapseCount = brain->getTotalSynapseCount();
    size_t activeNeurons = brain->getActiveNeuronCount();
    
    std::cout << "      Neuron count: " << neuronCount << std::endl;
    std::cout << "      Synapse count: " << synapseCount << std::endl;
    std::cout << "      Active neurons: " << activeNeurons << std::endl;
    
    assert(neuronCount == 1000);
    assert(synapseCount > 0);
    assert(activeNeurons < neuronCount);
    
    std::cout << "      ✓ Memory usage tests passed" << std::endl;
}

void testScalability() {
    std::cout << "    Testing scalability..." << std::endl;
    
    // Test with different brain sizes
    std::vector<size_t> neuronCounts = {100, 500, 1000};
    
    for (size_t neuronCount : neuronCounts) {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
        
        auto brain = std::make_shared<nlm::Brain>(config);
        assert(brain->initialize());
        
        // Run short simulation
        for (nlm::SimulationStep step = 0; step < 20; ++step) {
            brain->step(step);
        }
        
        std::cout << "      " << neuronCount << " neurons: " << brain->getTotalSynapseCount() << " synapses" << std::endl;
        assert(brain->getTotalNeuronCount() == neuronCount);
        assert(brain->getTotalSynapseCount() > 0);
    }
    
    std::cout << "      ✓ Scalability tests passed" << std::endl;
}

void testCheckpointPerformance() {
    std::cout << "    Testing checkpoint performance..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain->initialize());
    
    // Run simulation
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain->step(step);
    }
    
    // Test checkpoint save performance
    auto startTime = std::chrono::high_resolution_clock::now();
    std::string checkpointFile = "performance_test_checkpoint.bin";
    bool saveSuccess = brain->save(checkpointFile);
    auto endTime = std::chrono::high_resolution_clock::now();
    
    auto saveDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    std::cout << "      Save time: " << saveDuration.count() << "ms" << std::endl;
    assert(saveSuccess);
    
    // Test checkpoint load performance
    brain->reset();
    assert(brain->initialize());
    
    startTime = std::chrono::high_resolution_clock::now();
    bool loadSuccess = brain->load(checkpointFile);
    endTime = std::chrono::high_resolution_clock::now();
    
    auto loadDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    std::cout << "      Load time: " << loadDuration.count() << "ms" << std::endl;
    assert(loadSuccess);
    
    // Clean up
    std::remove(checkpointFile.c_str());
    
    std::cout << "      ✓ Checkpoint performance tests passed" << std::endl;
}

void runAll() {
    std::cout << "=== Performance Tests ===" << std::endl;
    
    testBrainPerformance();
    testMemoryUsage();
    testScalability();
    testCheckpointPerformance();
    
    std::cout << "=== All Performance Tests PASSED ===" << std::endl;
}

} // namespace test_performance
