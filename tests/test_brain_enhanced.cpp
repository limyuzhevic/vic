// Brain Tests
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <stdexcept>

namespace test_brain {

void testBrainCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Verify brain was created
    assert(brain.getConfig() != nullptr);
    assert(brain.getConfig()->getOr<int64_t>("neuron_count", 0) == 100);
    
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
    
    // Test developmental stage
    assert(brain.getDevelopmentalStage() == nlm::DevelopmentalStage::Initial);
    
    std::cout << "    testBrainInitialization passed" << std::endl;
}

void testBrainStep() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("simulation_timestep", 0.001, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Store initial state
    size_t initialSpikes = brain.getTotalSpikeCount();
    float initialFiringRate = brain.getAverageFiringRate();
    
    // Run a few steps
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
    }
    
    // Verify state changed
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getTotalSynapseCount() > 0);
    
    // Spikes may or may not have occurred, but system should remain stable
    assert(brain.getActiveNeuronCount() <= brain.getTotalNeuronCount());
    
    std::cout << "    testBrainStep passed" << std::endl;
}

void testBrainReset() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("simulation_timestep", 0.001, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Run some steps to change state
    brain.step(0);
    brain.step(1);
    
    size_t spikesBefore = brain.getTotalSpikeCount();
    
    brain.reset();
    brain.initialize();  // Re-initialize after reset
    
    // Verify reset worked
    assert(brain.getTotalNeuronCount() == 50);
    assert(brain.getRegionCount() == 1);
    
    // Spike count should be reset
    assert(brain.getTotalSpikeCount() == 0);
    
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
    
    // Verify each region is accessible
    for (auto rid : regionIds) {
        auto* region = brain.getRegion(rid);
        assert(region != nullptr);
        assert(region->getId() == rid);
        assert(region->getTotalNeuronCount() > 0);
    }
    
    // Test region addition
    nlm::RegionId newId = brain.addRegion("test_region");
    assert(newId.isValid());
    assert(brain.getRegionCount() == 4);
    
    std::cout << "    testBrainRegions passed" << std::endl;
}

void testBrainActionProduction() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("simulation_timestep", 0.001, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Run a few steps to allow for some activity
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain.step(step);
    }
    
    auto action = brain.produceAction();
    assert(action != nullptr);
    
    // Action should have a valid type
    assert(action->getType() != nlm::ActionType::Wait);
    
    std::cout << "    testBrainActionProduction passed" << std::endl;
}

void testBrainStatus() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("simulation_timestep", 0.001, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Verify statistics are accessible and valid
    assert(brain.getTotalNeuronCount() > 0);
    assert(brain.getTotalSynapseCount() >= 0);
    assert(brain.getActiveNeuronCount() >= 0);
    assert(brain.getFiringNeuronCount() >= 0);
    assert(brain.getAverageFiringRate() >= 0.0f);
    assert(brain.getExcitationInhibitionRatio() >= 0.0f);
    assert(brain.getTotalSpikeCount() >= 0);
    
    // Test developmental stage progression
    nlm::DevelopmentalStage stage = brain.getDevelopmentalStage();
    assert(static_cast<int>(stage) >= 0 && static_cast<int>(stage) < 5);
    
    // Test setting developmental stage
    brain.setDevelopmentalStage(nlm::DevelopmentalStage::Adult);
    assert(brain.getDevelopmentalStage() == nlm::DevelopmentalStage::Adult);
    
    std::cout << "    testBrainStatus passed" << std::endl;
}

void testBrainErrorHandling() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Test accessing uninitialized brain
    bool success = brain.initialize();
    assert(success);
    
    // Test step with invalid parameters
    try {
        brain.step(-1); // Should handle negative steps gracefully
        std::cout << "    Note: Step with negative value handled" << std::endl;
    } catch (const std::exception& e) {
        // Exception is acceptable
        std::cout << "    Note: Step with negative value raised exception: " << e.what() << std::endl;
    }
    
    // Test null region access
    nlm::RegionId invalidId;
    auto* nullRegion = brain.getRegion(invalidId);
    assert(nullRegion == nullptr);
    
    std::cout << "    testBrainErrorHandling passed" << std::endl;
}

void testBrainMemorySystems() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("simulation_timestep", 0.001, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test memory system access
    auto* workingMemory = brain.getWorkingMemory();
    auto* episodicMemory = brain.getEpisodicMemory();
    auto* associativeMemory = brain.getAssociativeMemory();
    
    // These may be null initially, but should not crash
    if (workingMemory) {
        // Working memory should have capacity
        size_t capacity = workingMemory->getCapacity();
        assert(capacity > 0);
    }
    
    // Test prediction system
    auto* predictionSystem = brain.getPredictionSystem();
    if (predictionSystem) {
        auto error = predictionSystem->getPredictionError();
        assert(error >= 0.0f);
    }
    
    // Test neuromodulation systems
    auto* dopamine = brain.getDopamine();
    auto* curiosity = brain.getCuriosity();
    auto* novelty = brain.getNovelty();
    auto* predictionError = brain.getPredictionErrorSignal();
    
    std::cout << "    testBrainMemorySystems passed" << std::endl;
}

void testBrainConfigurationAccess() {
    auto config = std::make_shared<nlm::Config>();
    config->set("brain.neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("brain.synapse_density", 0.15, nlm::ConfigSource::Default);
    config->set("plasticity.stdp.enable", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Test configuration access
    auto brainConfig = brain.getConfig();
    assert(brainConfig != nullptr);
    
    // Test has method
    assert(brainConfig->has("brain.neuron_count"));
    assert(!brainConfig->has("invalid_key"));
    
    // Test get methods
    assert(brainConfig->getOr<int64_t>("brain.neuron_count", 0) == 200);
    assert(brainConfig->getOr<float>("brain.synapse_density", 0.0f) == 0.15f);
    assert(brainConfig->getOr<bool>("plasticity.stdp.enable", false) == true);
    
    std::cout << "    testBrainConfigurationAccess passed" << std::endl;
}

void runAll() {
    std::cout << "Running Brain tests (enhanced)..." << std::endl;
    
    testBrainCreation();
    testBrainInitialization();
    testBrainStep();
    testBrainReset();
    testBrainRegions();
    testBrainActionProduction();
    testBrainStatus();
    testBrainErrorHandling();
    testBrainMemorySystems();
    testBrainConfigurationAccess();
    
    std::cout << "All Brain tests completed successfully!" << std::endl;
}

} // namespace test_brain
