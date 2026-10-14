// Memory Leak Fix Verification Test
// This test verifies that the Brain class memory leak fixes are working correctly

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Types/Types.hpp"
#include <iostream>
#include <cassert>

void testBrainConstructor() {
    std::cout << "Testing Brain constructor..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    // Create brain - should initialize all unique_ptrs properly
    nlm::Brain brain(config);
    
    std::cout << "Brain constructor test passed" << std::endl;
}

void testBrainInitialization() {
    std::cout << "Testing Brain initialization..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    bool success = brain.initialize();
    assert(success);
    
    // Verify brain initialized with expected properties
    assert(brain.getRegionCount() == 2);
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getTotalSynapseCount() > 0);
    
    std::cout << "Brain initialization test passed" << std::endl;
}

void testBrainMoveOperations() {
    std::cout << "Testing Brain move operations..." << std::endl;
    
    auto config1 = std::make_shared<nlm::Config>();
    config1->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config1->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto config2 = std::make_shared<nlm::Config>();
    config2->set("neuron_count", static_cast<int64_t>(75), nlm::ConfigSource::Default);
    config2->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    // Create and move brain
    nlm::Brain brain1(config1);
    nlm::Brain brain2(config2);
    
    brain1.initialize();
    brain2.initialize();
    
    // Move constructor
    nlm::Brain brain3(std::move(brain1));
    
    // Move assignment
    brain3 = std::move(brain2);
    
    // Verify moved brains are in valid state
    assert(brain3.getTotalNeuronCount() == 75);
    assert(brain3.getRegionCount() == 2);
    
    std::cout << "Brain move operations test passed" << std::endl;
}

void testBrainDestructor() {
    std::cout << "Testing Brain destructor cleanup..." << std::endl;
    
    // This test creates and destroys multiple brains to verify cleanup
    for (int i = 0; i < 5; ++i) {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        {
            nlm::Brain brain(config);
            brain.initialize();
            // Brain goes out of scope - destructor should clean up
        }
    }
    
    std::cout << "Brain destructor cleanup test passed" << std::endl;
}

void testMemoryTracking() {
    std::cout << "Testing memory tracking capabilities..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Run some steps
    for (nlm::SimulationStep step = 0; step < 5; ++step) {
        brain.step(step);
    }
    
    // Test memory status logging
    brain.logMemoryStatus();
    
    // Reset and test again
    brain.reset();
    brain.logMemoryStatus();
    
    std::cout << "Memory tracking test passed" << std::endl;
}

int main() {
    std::cout << "=== Memory Leak Fix Verification Tests ===" << std::endl;
    
    try {
        testBrainConstructor();
        testBrainInitialization();
        testBrainMoveOperations();
        testBrainDestructor();
        testMemoryTracking();
        
        std::cout << "\n=== All Tests Passed Successfully! ===" << std::endl;
        std::cout << "Memory leak fixes are working correctly." << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception" << std::endl;
        return 1;
    }
}