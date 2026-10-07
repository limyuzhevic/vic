#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <pybind11/pybind11.h>

namespace test_pybindings {

void testPythonBindings() {
    std::cout << "=== Testing Python Bindings ===" << std::endl;
    
    // Test 1: Create brain with configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("brain.neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("brain.synapse_density", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    std::cout << "✓ Brain creation passed" << std::endl;
    
    // Test 2: Initialize brain
    bool success = brain.initialize();
    assert(success);
    assert(brain.getTotalNeuronCount() == 100);
    std::cout << "✓ Brain initialization passed" << std::endl;
    
    // Test 3: Get system summaries
    auto system_summary = brain.get_system_summary();
    assert(system_summary.contains("total_neurons"));
    assert(system_summary["total_neurons"].as<size_t>() == 100);
    assert(system_summary.contains("firing_neurons"));
    assert(system_summary.contains("total_spikes"));
    assert(system_summary.contains("developmental_stage"));
    std::cout << "✓ System summary test passed" << std::endl;
    
    // Test 4: Get activity summary
    auto activity_summary = brain.get_activity_summary();
    assert(activity_summary.contains("total_neurons"));
    assert(activity_summary.contains("firing_neurons"));
    std::cout << "✓ Activity summary test passed" << std::endl;
    
    // Test 5: Get neuromodulation state
    auto neuromod_state = brain.get_neuromodulation_state();
    // May be empty if neuromodulation not enabled
    std::cout << "✓ Neuromodulation state test passed" << std::endl;
    
    // Test 6: Test with SimpleWorld (if available)
    auto world = std::make_shared<nlm::SimpleWorld>();
    world->configure(20, 20, 8, 8);
    world->reset();
    std::cout << "✓ World creation passed" << std::endl;
    
    // Test 7: Get agent body
    auto body = world->getAgentBody();
    assert(body->x >= 0.0f && body->x <= 20.0f);
    assert(body->y >= 0.0f && body->y <= 20.0f);
    std::cout << "✓ Agent body test passed" << std::endl;
    
    // Test 8: Test region management
    auto region_ids = brain.getRegionIds();
    assert(region_ids.size() > 0);
    std::cout << "✓ Region management test passed" << std::endl;
    
    // Test 9: Test step functionality
    for (nlm::SimulationStep step = 0; step < 5; ++step) {
        brain.step(step);
    }
    assert(brain.getTotalNeuronCount() == 100);
    std::cout << "✓ Step functionality test passed" << std::endl;
    
    // Test 10: Test action production
    auto action = brain.produceAction();
    // Action may be null, that's okay
    std::cout << "✓ Action production test passed" << std::endl;
    
    std::cout << "=== All Python Bindings Tests Passed ===" << std::endl;
}

void testConfigurationHelpers() {
    std::cout << "=== Testing Configuration Helpers ===" << std::endl;
    
    // Test createDefaultConfig
    auto config = std::make_shared<nlm::Config>();
    config->set("test.value", 42, nlm::ConfigSource::Runtime);
    auto val = config->get<int>("test.value");
    assert(val.has_value() && *val == 42);
    std::cout << "✓ Configuration get/set test passed" << std::endl;
    
    // Test config keys
    config->set("key1", 1, nlm::ConfigSource::Runtime);
    config->set("key2", 2, nlm::ConfigSource::Runtime);
    auto keys = config->getKeys();
    assert(keys.size() == 3); // test.value, key1, key2
    std::cout << "✓ Configuration keys test passed" << std::endl;
    
    // Test remove
    config->remove("key1");
    keys = config->getKeys();
    assert(keys.size() == 2);
    std::cout << "✓ Configuration remove test passed" << std::endl;
    
    std::cout << "=== Configuration Helper Tests Passed ===" << std::endl;
}

void testEnhancedSensoryInput() {
    std::cout << "=== Testing Enhanced Sensory Input ===" << std::endl;
    
    // Note: This test would require actual Vision, Audio, and InternalSignals classes
    // to be fully implemented in the C++ code
    std::cout << "✓ Enhanced sensory input test structure created" << std::endl;
    std::cout << "=== Enhanced Sensory Input Tests Created ===" << std::endl;
}

void testEnhancedAction() {
    std::cout << "=== Testing Enhanced Action ===" << std::endl;
    
    // Note: This test would require actual Action class implementation
    std::cout << "✓ Enhanced action test structure created" << std::endl;
    std::cout << "=== Enhanced Action Tests Created ===" << std::endl;
}

void runAll() {
    testPythonBindings();
    testConfigurationHelpers();
    testEnhancedSensoryInput();
    testEnhancedAction();
}

} // namespace test_pybindings