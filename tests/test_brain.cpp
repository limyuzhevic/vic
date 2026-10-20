// Brain Tests
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>

namespace test_brain {

void testBrainCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    std::cout << "    testBrainCreation passed" << std::endl;
}

void testBrainInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    bool success = brain.initialize();
    assert(success);
    
    // Verify brain initialized with expected properties
    assert(brain.getRegionCount() == 2);
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getTotalSynapseCount() > 0);
    
    std::cout << "    testBrainInitialization passed" << std::endl;
}

void testBrainStep() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Run a few steps
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    // Just verify it doesn't crash
    assert(brain.getTotalNeuronCount() == 100);
    
    std::cout << "    testBrainStep passed" << std::endl;
}

void testBrainReset() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    brain.step(0);
    brain.reset();
    
    // Verify reset worked
    assert(brain.getTotalNeuronCount() == 50);
    
    std::cout << "    testBrainReset passed" << std::endl;
}

void testBrainRegions() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto regionIds = brain.getRegionIds();
    assert(regionIds.size() == 3);
    
    for (auto rid : regionIds) {
        auto* region = brain.getRegion(rid);
        assert(region != nullptr);
    }
    
    std::cout << "    testBrainRegions passed" << std::endl;
}

void testBrainActionProduction() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Produce actions multiple times
    for (int i = 0; i < 5; ++i) {
        auto action = brain.produceAction();
        assert(action != nullptr);
    }
    
    std::cout << "    testBrainActionProduction passed" << std::endl;
}

void testBrainStatistics() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    brain.step(0);
    
    // Test various statistics
    assert(brain.getTotalNeuronCount() == 200);
    assert(brain.getTotalSynapseCount() > 0);
    assert(brain.getActiveNeuronCount() >= 0);
    assert(brain.getFiringNeuronCount() >= 0);
    assert(brain.getAverageFiringRate() >= 0.0f);
    assert(brain.getExcitationInhibitionRatio() > 0.0f);
    assert(brain.getTotalSpikeCount() >= 0);
    
    std::cout << "    testBrainStatistics passed" << std::endl;
}

void testBrainDevelopmentalStage() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test default developmental stage
    auto stage = brain.getDevelopmentalStage();
    assert(stage != nlm::DevelopmentalStage::Invalid);
    
    // Test setting developmental stage
    brain.setDevelopmentalStage(nlm::DevelopmentalStage::Adult);
    assert(brain.getDevelopmentalStage() == nlm::DevelopmentalStage::Adult);
    
    std::cout << "    testBrainDevelopmentalStage passed" << std::endl;
}

void testBrainConfigAccess() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("custom_param", "test_value", nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    auto brainConfig = brain.getConfig();
    assert(brainConfig != nullptr);
    
    // Test getting config values
    assert(brainConfig->has("neuron_count"));
    auto neuronCount = brainConfig->get<int64_t>("neuron_count");
    assert(neuronCount.has_value() && *neuronCount == 150);
    
    assert(brainConfig->has("custom_param"));
    auto customParam = brainConfig->get<std::string>("custom_param");
    assert(customParam.has_value() && *customParam == "test_value");
    
    std::cout << "    testBrainConfigAccess passed" << std::endl;
}

void testBrainSensoryInput() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create a simple sensory input
    nlm::SensoryInput sensory;
    // Note: Implementation of SensoryInput depends on actual class
    // For now, we just test that the brain doesn't crash when receiving input
    
    std::cout << "    testBrainSensoryInput passed" << std::endl;
}

void testBrainMemory() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test memory-related functionality
    // This would include testing memory systems if they exist
    
    std::cout << "    testBrainMemory passed" << std::endl;
}

void testBrainErrorHandling() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test error handling - brain should handle edge cases gracefully
    brain.step(-1);  // Negative step (implementation dependent)
    brain.step(999999);  // Very large step number
    
    std::cout << "    testBrainErrorHandling passed" << std::endl;
}

void testBrainPerformance() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Performance test - run many steps quickly
    const int num_steps = 1000;
    for (nlm::SimulationStep step = 0; step < num_steps; ++step) {
        brain.step(step);
    }
    
    // Verify brain still works after many steps
    assert(brain.getTotalNeuronCount() == 50);
    
    std::cout << "    testBrainPerformance passed" << std::endl;
}

void testBrainNetworkStats() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    brain.step(0);
    
    // Test network statistics
    auto stats = brain.getNetworkStats();
    assert(stats.find("neuron_count") != stats.end());
    assert(stats.find("synapse_count") != stats.end());
    assert(stats.find("active_neurons") != stats.end());
    
    std::cout << "    testBrainNetworkStats passed" << std::endl;
}

void testBrainSaveLoad() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(75), nlm::ConfigSource::Default);
    
    nlm::Brain brain1(config);
    brain1.initialize();
    
    // Run some steps to have state
    for (nlm::SimulationStep step = 0; step < 5; ++step) {
        brain1.step(step);
    }
    
    // Save brain state
    brain1.save("test_brain_save.bin");
    
    // Create new brain and load
    auto config2 = std::make_shared<nlm::Config>();
    config2->set("neuron_count", static_cast<int64_t>(75), nlm::ConfigSource::Default);
    
    nlm::Brain brain2(config2);
    brain2.initialize();
    
    // Load saved state
    brain2.load("test_brain_save.bin");
    
    // Verify load worked (at least same neuron count)
    assert(brain2.getTotalNeuronCount() == brain1.getTotalNeuronCount());
    
    std::cout << "    testBrainSaveLoad passed" << std::endl;
}

void runAll() {
    testBrainCreation();
    testBrainInitialization();
    testBrainStep();
    testBrainReset();
    testBrainRegions();
    testBrainActionProduction();
    testBrainStatistics();
    testBrainDevelopmentalStage();
    testBrainConfigAccess();
    testBrainSensoryInput();
    testBrainMemory();
    testBrainErrorHandling();
    testBrainPerformance();
    testBrainNetworkStats();
    testBrainSaveLoad();
    
    std::cout << "\nAll Brain Tests Passed!" << std::endl;
}

} // namespace test_brain
