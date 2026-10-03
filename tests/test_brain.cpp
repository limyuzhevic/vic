// Brain Tests
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_brain {

void testBrainCreation(nullptr) {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    std::cout << "    testBrainCreation passed" << std::endl;
}

void testBrainInitialization(nullptr) {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    bool success = brain.initialize(nullptr);
    assert(success);
    
    // Verify brain initialized with expected properties
    assert(brain.getRegionCount(nullptr) == 2);
    assert(brain.getTotalNeuronCount(nullptr) == 100);
    assert(brain.getTotalSynapseCount(nullptr) > 0);
    
    std::cout << "    testBrainInitialization passed" << std::endl;
}

void testBrainStep(nullptr) {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize(nullptr);
    
    // Run a few steps
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    // Just verify it doesn't crash
    assert(brain.getTotalNeuronCount(nullptr) == 100);
    
    std::cout << "    testBrainStep passed" << std::endl;
}

void testBrainReset(nullptr) {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize(nullptr);
    
    brain.step(0);
    brain.reset(nullptr);
    
    // Verify reset worked
    assert(brain.getTotalNeuronCount(nullptr) == 50);
    
    std::cout << "    testBrainReset passed" << std::endl;
}

void testBrainRegions(nullptr) {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize(nullptr);
    
    auto regionIds = brain.getRegionIds(nullptr);
    assert(regionIds.size(nullptr) == 3);
    
    for (auto rid : regionIds) {
        auto* region = brain.getRegion(rid);
        assert(region != nullptr);
    }
    
    std::cout << "    testBrainRegions passed" << std::endl;
}

void testBrainActionProduction(nullptr) {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize(nullptr);
    
    auto action = brain.produceAction(nullptr);
    assert(action != nullptr);
    
    std::cout << "    testBrainActionProduction passed" << std::endl;
}

void testBrainStatus(nullptr) {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize(nullptr);
    
    // Verify statistics are accessible
    assert(brain.getTotalNeuronCount(nullptr) > 0);
    assert(brain.getTotalSynapseCount(nullptr) >= 0);
    assert(brain.getFiringNeuronCount(nullptr) >= 0);
    assert(brain.getAverageFiringRate(nullptr) >= 0.0f);
    
    std::cout << "    testBrainStatus passed" << std::endl;
}

void runAll(nullptr) {
    testBrainCreation(nullptr);
    testBrainInitialization(nullptr);
    testBrainStep(nullptr);
    testBrainReset(nullptr);
    testBrainRegions(nullptr);
    testBrainActionProduction(nullptr);
    testBrainStatus(nullptr);
}

} // namespace test_brain
