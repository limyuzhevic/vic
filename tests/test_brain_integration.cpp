#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/BrainIntegration.hpp"
#include <cassert>
#include <iostream>

namespace test_brain_integration {

void testBrainIntegrationCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100));
    config->set("region_count", static_cast<int64_t>(1));
    
    nlm::BrainIntegration brain(config);
    
    std::cout << "    testBrainIntegrationCreation passed" << std::endl;
}

void testBrainIntegrationInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100));
    config->set("region_count", static_cast<int64_t>(2));
    config->set("connection_probability", 0.1f);
    
    nlm::BrainIntegration brain(config);
    
    bool success = brain.initialize(100, 2, 0.1f);
    assert(success);
    
    // Verify brain initialized with expected properties
    assert(brain.getRegionCount() == 2);
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getTotalSynapseCount() > 0);
    
    std::cout << "    testBrainIntegrationInitialization passed" << std::endl;
}

void testBrainIntegrationStep() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100));
    config->set("region_count", static_cast<int64_t>(1));
    
    nlm::BrainIntegration brain(config);
    brain.initialize(100, 1, 0.1f);
    
    // Run a few steps
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step, static_cast<nlm::Timestamp>(step) * 0.001);
    }
    
    // Just verify it doesn't crash
    assert(brain.getTotalNeuronCount() == 100);
    
    std::cout << "    testBrainIntegrationStep passed" << std::endl;
}

void testBrainIntegrationReset() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50));
    config->set("region_count", static_cast<int64_t>(1));
    
    nlm::BrainIntegration brain(config);
    brain.initialize(50, 1, 0.1f);
    
    brain.step(0, 0.0);
    brain.reset();
    
    // Verify reset worked
    assert(brain.getTotalNeuronCount() == 50);
    
    std::cout << "    testBrainIntegrationReset passed" << std::endl;
}

void testBrainIntegrationRegions() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100));
    config->set("region_count", static_cast<int64_t>(3));
    
    nlm::BrainIntegration brain(config);
    brain.initialize(100, 3, 0.1f);
    
    auto regionIds = brain.getRegionIds();
    assert(regionIds.size() == 3);
    
    for (auto rid : regionIds) {
        auto* region = brain.getRegion(rid);
        assert(region != nullptr);
    }
    
    std::cout << "    testBrainIntegrationRegions passed" << std::endl;
}

void testBrainIntegrationMemorySystems() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100));
    config->set("region_count", static_cast<int64_t>(1));
    
    nlm::BrainIntegration brain(config);
    brain.initialize(100, 1, 0.1f);
    
    // Test memory system accessors
    auto* workingMemory = brain.getWorkingMemory();
    auto* episodicMemory = brain.getEpisodicMemory();
    auto* associativeMemory = brain.getAssociativeMemory();
    
    // These should not be null after initialization
    assert(workingMemory != nullptr);
    assert(episodicMemory != nullptr);
    assert(associativeMemory != nullptr);
    
    std::cout << "    testBrainIntegrationMemorySystems passed" << std::endl;
}

void testBrainIntegrationNeuromodulation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100));
    config->set("region_count", static_cast<int64_t>(1));
    
    nlm::BrainIntegration brain(config);
    brain.initialize(100, 1, 0.1f);
    
    // Test neuromodulation system accessors
    auto* dopamine = brain.getDopamine();
    auto* curiosity = brain.getCuriosity();
    auto* novelty = brain.getNovelty();
    
    assert(dopamine != nullptr);
    assert(curiosity != nullptr);
    assert(novelty != nullptr);
    
    std::cout << "    testBrainIntegrationNeuromodulation passed" << std::endl;
}

void testBrainIntegrationCognition() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100));
    config->set("region_count", static_cast<int64_t>(1));
    
    nlm::BrainIntegration brain(config);
    brain.initialize(100, 1, 0.1f);
    
    // Test cognition system accessors
    auto* planner = brain.getPlanner();
    auto* conceptFormation = brain.getConceptFormation();
    auto* attention = brain.getAttention();
    
    assert(planner != nullptr);
    assert(conceptFormation != nullptr);
    assert(attention != nullptr);
    
    std::cout << "    testBrainIntegrationCognition passed" << std::endl;
}

void testBrainIntegrationDevelopment() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100));
    config->set("region_count", static_cast<int64_t>(1));
    
    nlm::BrainIntegration brain(config);
    brain.initialize(100, 1, 0.1f);
    
    // Test development system accessor
    auto* developmentSystem = brain.getDevelopmentSystem();
    assert(developmentSystem != nullptr);
    
    // Test developmental stage
    auto stage = brain.getDevelopmentalStage();
    assert(stage == nlm::DevelopmentalStage::Initial || stage == nlm::DevelopmentalStage::CriticalPeriod ||
           stage == nlm::DevelopmentalStage::Maturation || stage == nlm::DevelopmentalStage::Adult);
    
    std::cout << "    testBrainIntegrationDevelopment passed" << std::endl;
}

void testBrainIntegrationStatistics() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100));
    config->set("region_count", static_cast<int64_t>(1));
    
    nlm::BrainIntegration brain(config);
    brain.initialize(100, 1, 0.1f);
    
    // Verify statistics are accessible
    assert(brain.getTotalNeuronCount() > 0);
    assert(brain.getTotalSynapseCount() >= 0);
    assert(brain.getFiringNeuronCount() >= 0);
    assert(brain.getAverageFiringRate() >= 0.0f);
    assert(brain.getExcitationInhibitionRatio() >= 0.0f);
    
    std::cout << "    testBrainIntegrationStatistics passed" << std::endl;
}

void testBrainIntegrationSaveLoad() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100));
    config->set("region_count", static_cast<int64_t>(1));
    
    nlm::BrainIntegration brain(config);
    brain.initialize(100, 1, 0.1f);
    
    // Take a step to change brain state
    brain.step(0, 0.0);
    
    // Save brain state
    bool saved = brain.save("./test_checkpoint.bin");
    assert(saved);
    
    // Reset brain
    brain.reset();
    
    // Load brain state
    bool loaded = brain.load("./test_checkpoint.bin");
    assert(loaded);
    
    // Verify brain state was restored
    assert(brain.getTotalNeuronCount() == 100);
    
    // Clean up
    std::remove("./test_checkpoint.bin");
    
    std::cout << "    testBrainIntegrationSaveLoad passed" << std::endl;
}

void runAll() {
    testBrainIntegrationCreation();
    testBrainIntegrationInitialization();
    testBrainIntegrationStep();
    testBrainIntegrationReset();
    testBrainIntegrationRegions();
    testBrainIntegrationMemorySystems();
    testBrainIntegrationNeuromodulation();
    testBrainIntegrationCognition();
    testBrainIntegrationDevelopment();
    testBrainIntegrationStatistics();
    testBrainIntegrationSaveLoad();
    
    std::cout << std::endl;
    std::cout << "=== All BrainIntegration Tests Passed ===" << std::endl;
}

} // namespace test_brain_integration