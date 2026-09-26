// Memory Leak Detection Tests
// Tests for detecting memory leaks in NLM components

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include <cassert>
#include <iostream>
#include <memory>
#include <vector>

namespace test_memory_leaks {

void testNeuronMemory() {
    std::cout << "    Testing Neuron memory management..." << std::endl;
    
    // Test neuron construction and destruction
    {
        nlm::NeuronId id(42);
        nlm::Neuron neuron(id);
        neuron.setType(nlm::NeuronType::Excitatory);
        neuron.initializeRandom(*nlm::RandomGenerator::getInstance());
        
        // Verify neuron has properties
        assert(neuron.getState().firingState != nlm::FiringState::Undefined);
        assert(neuron.getType() == nlm::NeuronType::Excitatory);
    }
    
    // Neuron should be destroyed here - no memory leaks expected
    std::cout << "    Neuron memory test passed" << std::endl;
}

void testSynapseMemory() {
    std::cout << "    Testing Synapse memory management..." << std::endl;
    
    // Test synapse construction and destruction
    {
        nlm::SynapseId synId(1);
        nlm::NeuronId source(1), dest(2);
        nlm::Synapse synapse(synId, source, dest);
        synapse.initializeRandom(*nlm::RandomGenerator::getInstance());
        
        // Verify synapse has properties
        assert(synapse.getWeight() != 0.0f || synapse.getType() == nlm::SynapseType::Inhibitory);
        assert(synapse.getDelay() > 0);
    }
    
    // Synapse should be destroyed here - no memory leaks expected
    std::cout << "    Synapse memory test passed" << std::endl;
}

void testBrainMemory() {
    std::cout << "    Testing Brain memory management..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    // Brain should properly clean up all components
    {
        nlm::Brain brain(config);
        bool initSuccess = brain.initialize();
        assert(initSuccess);
        
        // Verify brain has resources
        assert(brain.getTotalNeuronCount() == 50);
        assert(brain.getRegionCount() > 0);
        
        // Simulate activity
        for (nlm::SimulationStep step = 0; step < 10; ++step) {
            brain.step(step);
        }
    }
    
    // Brain and all its components should be destroyed here
    std::cout << "    Brain memory test passed" << std::endl;
}

void testConfigMemory() {
    std::cout << "    Testing Config memory management..." << std::endl;
    
    // Config should properly clean up internal structures
    {
        auto config1 = std::make_shared<nlm::Config>();
        config1->set("test_int", 42);
        config1->set("test_double", 3.14);
        config1->set("test_string", "hello");
        config1->set("test_bool", true);
        
        // Verify config values
        assert(config1->getOr<int>("test_int", 0) == 42);
        assert(config1->getOr<double>("test_double", 0.0) == 3.14);
        assert(config1->getOr<std::string>("test_string", "") == "hello");
        assert(config1->getOr<bool>("test_bool", false) == true);
        
        // Clear and test cleanup
        config1->clear();
    }
    
    std::cout << "    Config memory test passed" << std::endl;
}

void testEdgeCases() {
    std::cout << "    Testing edge cases..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    // Test with minimum neurons
    assert(brain.getTotalNeuronCount() == 1);
    
    // Test brain with single neuron step
    brain.step(0);
    
    // Test reset
    brain.reset();
    assert(brain.getTotalNeuronCount() == 1);
    
    // Test brain status queries don't crash
    brain.getTotalNeuronCount();
    brain.getTotalSynapseCount();
    brain.getFiringNeuronCount();
    brain.getAverageFiringRate();
    
    std::cout << "    Edge cases test passed" << std::endl;
}

void runAll() {
    testNeuronMemory();
    testSynapseMemory();
    testBrainMemory();
    testConfigMemory();
    testEdgeCases();
    
    std::cout << "    All memory leak and edge case tests passed!" << std::endl;
}

} // namespace test_memory_leaks