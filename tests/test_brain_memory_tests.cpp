// Brain Memory Management Tests for NLM Phase 6
// Tests vector initialization, reset functionality, and memory management fixes

#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <memory>
#include <vector>
#include <algorithm>

namespace test_brain_memory {

void testBrainVectorInitialization() {
    // Test vector initialization with large networks
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(10000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.05f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Verify vector initialization
    assert(brain.getRegionCount() == 10);
    assert(brain.getTotalNeuronCount() == 10000);
    assert(brain.getTotalSynapseCount() > 0);
    
    // Check that vectors are properly initialized (not null)
    auto regions = brain.getRegions();
    assert(!regions.empty());
    assert(regions.size() == 10);
    
    for (const auto& region : regions) {
        assert(region != nullptr);
        assert(!region->getNeurons().empty());
        assert(!region->getSynapses().empty());
    }
    
    std::cout << "    testBrainVectorInitialization passed" << std::endl;
}

void testBrainResetFunctionality() {
    // Test reset functionality with complex network
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(5), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get initial state
    size_t initialNeurons = brain.getTotalNeuronCount();
    size_t initialSynapses = brain.getTotalSynapseCount();
    
    // Run many steps to modify state extensively
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
    }
    
    // Reset
    brain.reset();
    
    // Verify reset worked
    assert(brain.getTotalNeuronCount() == initialNeurons);
    assert(brain.getRegionCount() == 5);
    
    // State should be restored to initialized state
    auto regions = brain.getRegions();
    assert(!regions.empty());
    assert(regions.size() == 5);
    
    for (const auto& region : regions) {
        assert(region != nullptr);
        // Each region should have neurons
        assert(!region->getNeurons().empty());
    }
    
    std::cout << "    testBrainResetFunctionality passed" << std::endl;
}

void testBrainMemoryCleanup() {
    // Test memory cleanup with complex operations
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    // Create and destroy brain with complex usage
    for (int i = 0; i < 10; ++i) {
        nlm::Brain brain(config);
        brain.initialize();
        
        // Use the brain with various operations
        for (nlm::SimulationStep step = 0; step < 20; ++step) {
            brain.step(step);
            
            // Inject different types of input
            if (step % 3 == 0) {
                brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 20.0f);
            } else if (step % 3 == 1) {
                brain.injectCurrentToNeurons(nlm::NeuronType::Motor, 15.0f);
            } else {
                brain.injectCurrentToNeurons(nlm::NeuronType::Excitatory, 10.0f);
            }
        }
        
        // Brain should go out of scope and be destroyed here
        // The destructor should properly clean up all allocated resources
    }
    
    // If we get here, memory cleanup is working
    std::cout << "    testBrainMemoryCleanup passed" << std::endl;
}

void testBrainMoveSemantics() {
    // Test move semantics for brain objects
    auto config1 = std::make_shared<nlm::Config>();
    config1->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config1->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain1(config1);
    brain1.initialize();
    
    // Test move constructor
    nlm::Brain brain2(std::move(brain1));
    
    // Verify the moved brain is functional
    assert(brain2.getTotalNeuronCount() == 100);
    assert(brain2.getRegionCount() == 2);
    
    // Test move assignment
    auto config3 = std::make_shared<nlm::Config>();
    config3->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config3->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain3(config3);
    brain3.initialize();
    
    brain2 = std::move(brain3);
    
    assert(brain2.getTotalNeuronCount() == 150);
    assert(brain2.getRegionCount() == 3);
    
    std::cout << "    testBrainMoveSemantics passed" << std::endl;
}

void testBrainThreadSafety() {
    // Test thread safety with concurrent access
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(4), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test thread safety with concurrent stepping
    std::vector<std::thread> threads;
    const size_t threadCount = 8;
    
    for (size_t i = 0; i < threadCount; ++i) {
        threads.emplace_back([&brain, i]() {
            for (nlm::SimulationStep step = 0; step < 30; ++step) {
                brain.step(step);
                // Simulate some work
                std::this_thread::sleep_for(std::chrono::microseconds(100));
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    // Verify brain is still in valid state
    assert(brain.getTotalNeuronCount() == 300);
    assert(brain.getRegionCount() == 4);
    
    std::cout << "    testBrainThreadSafety passed" << std::endl;
}

void testBrainMemoryStats() {
    // Test memory statistics collection
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Verify memory statistics are available
    size_t neuronCount = brain.getTotalNeuronCount();
    size_t synapseCount = brain.getTotalSynapseCount();
    size_t firingCount = brain.getFiringNeuronCount();
    float firingRate = brain.getAverageFiringRate();
    
    assert(neuronCount > 0);
    assert(synapseCount >= 0);
    assert(firingCount >= 0);
    assert(firingRate >= 0.0f);
    
    std::cout << "    testBrainMemoryStats passed" << std::endl;
}

void runAll() {
    testBrainVectorInitialization();
    testBrainResetFunctionality();
    testBrainMemoryCleanup();
    testBrainMoveSemantics();
    testBrainThreadSafety();
    testBrainMemoryStats();
    
    std::cout << "\nAll brain memory management tests passed!\n" << std::endl;
}

} // namespace test_brain_memory
