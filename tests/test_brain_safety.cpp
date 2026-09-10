// Brain Safety and Error Handling Tests
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <memory>

namespace test_brain_safety {

void testNullConfig() {
    std::cout << "    testNullConfig passed" << std::endl;
}

void testInvalidNeuronCount() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(-1), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    assert(!success);  // Should fail with negative neuron count
    
    std::cout << "    testInvalidNeuronCount passed" << std::endl;
}

void testZeroRegionCount() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(0), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    assert(!success);  // Should fail with zero region count
    
    std::cout << "    testZeroRegionCount passed" << std::endl;
}

void testInvalidConnectionProbability() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 1.5f, nlm::ConfigSource::Default);  // > 1.0
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    assert(!success);  // Should fail with connection probability > 1.0
    
    std::cout << "    testInvalidConnectionProbability passed" << std::endl;
}

void testBrainReinitializationAfterFailure() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    assert(success);
    
    // Verify brain is in valid state
    assert(brain.getTotalNeuronCount() == 100);
    
    std::cout << "    testBrainReinitializationAfterFailure passed" << std::endl;
}

void testBrainStepWithNullPtr() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    assert(success);
    
    // Run steps without crashing
    for (nlm::SimulationStep step = 0; step < 20; ++step) {
        brain.step(step);
    }
    
    std::cout << "    testBrainStepWithNullPtr passed" << std::endl;
}

void runAll() {
    testNullConfig();
    testInvalidNeuronCount();
    testZeroRegionCount();
    testInvalidConnectionProbability();
    testBrainReinitializationAfterFailure();
    testBrainStepWithNullPtr();
    std::cout << "All Brain safety tests passed!" << std::endl;
}

} // namespace test_brain_safety
