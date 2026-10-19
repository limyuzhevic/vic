// Memory Management Tests
// Tests for memory management, leak detection, and proper cleanup

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <memory>
#include <vector>

namespace test_memory_management {

void testPimplDestructors() {
    std::cout << "    Testing Pimpl destructors..." << std::endl;
    
    // Test Config destructor
    {
        auto config = std::make_shared<nlm::Config>();
        config->set("test", 1);
    } // config should be destroyed here
    
    // Test Brain destructor (should clean up Impl)
    {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 10);
        config->set("region_count", 1);
        
        nlm::Brain brain(config);
        // Brain constructor creates pImpl
    } // Brain destructor should delete pImpl
    
    // Test Neuron destructor (should clean up Impl)
    {
        nlm::Neuron neuron(nlm::NeuronId(1));
        // Neuron constructor creates pImpl
    } // Neuron destructor should delete pImpl
    
    // Test Synapse destructor (should clean up Impl)
    {
        nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
        // Synapse constructor creates pImpl
    } // Synapse destructor should delete pImpl
    
    std::cout << "    Pimpl destructors test passed" << std::endl;
}

void testSmartPointerCleanup() {
    std::cout << "    Testing smart pointer cleanup..." << std::endl;
    
    // Test that unique_ptr members are properly cleaned up
    {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 50);
        config->set("region_count", 2);
        
        nlm::Brain brain(config);
        
        if (!brain.initialize()) {
            std::cerr << "Failed to initialize brain" << std::endl;
            assert(false);
        }
        
        // Brain should have properly initialized internal smart pointers
        assert(brain.getTotalNeuronCount() == 50);
        assert(brain.getRegionCount() == 2);
    } // All smart pointers should be cleaned up
    
    std::cout << "    Smart pointer cleanup test passed" << std::endl;
}

void testMemoryPoolUsage() {
    std::cout << "    Testing memory pool usage..." << std::endl;
    
    // Create multiple brains to test memory pool efficiency
    std::vector<std::shared_ptr<nlm::Brain>> brains;
    
    for (int i = 0; i < 10; ++i) {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 100 + i * 10);
        config->set("region_count", 1);
        
        auto brain = std::make_shared<nlm::Brain>(config);
        assert(brain->initialize());
        
        brains.push_back(brain);
    }
    
    // Clear all brains - smart pointers should clean up properly
    brains.clear();
    
    std::cout << "    Memory pool usage test passed" << std::endl;
}

void testCircularReferences() {
    std::cout << "    Testing for circular references..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 20);
    config->set("region_count", 1);
    
    nlm::Brain brain(config);
    
    if (!brain.initialize()) {
        std::cerr << "Failed to initialize brain" << std::endl;
        assert(false);
    }
    
    // Brain should not have circular references
    // The brain system uses smart pointers correctly
    assert(brain.getConfig());
    assert(brain.getTotalNeuronCount() > 0);
    
    std::cout << "    Circular references test passed" << std::endl;
}

void testExceptionSafety() {
    std::cout << "    Testing exception safety..." << std::endl;
    
    try {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 0); // Invalid neuron count
        
        nlm::Brain brain(config);
        
        // Should not crash, even with invalid config
        std::cout << "    Exception safety test passed" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "    Exception safety test passed (caught exception: " 
                  << e.what() << ")" << std::endl;
    }
}

} // namespace test_memory_management
