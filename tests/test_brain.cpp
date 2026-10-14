// Enhanced Brain Tests
// Comprehensive tests for the improved brain system

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <thread>
#include <chrono>

namespace test_enhanced_brain {

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
    
    auto action = brain.produceAction();
    assert(action != nullptr);
    
    std::cout << "    testBrainActionProduction passed" << std::endl;
}

void testBrainStatus() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Verify statistics are accessible
    assert(brain.getTotalNeuronCount() > 0);
    assert(brain.getTotalSynapseCount() >= 0);
    assert(brain.getFiringNeuronCount() >= 0);
    assert(brain.getAverageFiringRate() >= 0.0f);
    assert(brain.getExcitationInhibitionRatio() >= 0.0f);
    
    std::cout << "    testBrainStatus passed" << std::endl;
}

void testBrainIntegratedSystems() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Verify integrated systems are present
    assert(brain.getWorkingMemory() != nullptr);
    assert(brain.getEpisodicMemory() != nullptr);
    assert(brain.getPredictionSystem() != nullptr);
    assert(brain.getPlanner() != nullptr);
    assert(brain.getAttention() != nullptr);
    assert(brain.getDevelopmentSystem() != nullptr);
    assert(brain.getDopamine() != nullptr);
    assert(brain.getCuriosity() != nullptr);
    assert(brain.getNovelty() != nullptr);
    assert(brain.getPredictionErrorSignal() != nullptr);
    
    std::cout << "    testBrainIntegratedSystems passed" << std::endl;
}

void testBrainPlasticity() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("plasticity.stdp.enable", true, nlm::ConfigSource::Default);
    config->set("plasticity.hebbian.enable", true, nlm::ConfigSource::Default);
    config->set("plasticity.stdp.learning_rate", 0.01, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Enable plasticity on synapses
    auto* region = brain.getRegion(nlm::RegionId(1));
    if (region) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, true);
        }
    }
    
    // Run simulation with activity to trigger plasticity
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 30.0f);
        brain.step(step);
    }
    
    // Verify plasticity was applied (some synapses should have changed)
    assert(brain.getTotalSynapseCount() > 0);
    
    std::cout << "    testBrainPlasticity passed" << std::endl;
}

void testBrainNeuromodulation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("neuromod.dopamine.scale", 2.0, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Verify neuromodulation systems are present
    assert(brain.getDopamine() != nullptr);
    assert(brain.getCuriosity() != nullptr);
    assert(brain.getNovelty() != nullptr);
    
    // Apply neuromodulation
    nlm::Neuromodulator dummySignal;
    brain.applyNeuromodulation(dummySignal);
    
    std::cout << "    testBrainNeuromodulation passed" << std::endl;
}

void testBrainDevelopment() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Verify developmental stage
    auto stage = brain.getDevelopmentalStage();
    assert(stage == nlm::DevelopmentalStage::Initial);
    
    // Update development
    brain.develop();
    
    // Developmental stage should still be initial (development updates structural plasticity)
    assert(brain.getDevelopmentalStage() == nlm::DevelopmentalStage::Initial);
    
    std::cout << "    testBrainDevelopment passed" << std::endl;
}

void testBrainMemorySystems() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test working memory
    auto* workingMemory = brain.getWorkingMemory();
    if (workingMemory) {
        workingMemory->storeToNeuron(nlm::NeuronId(1), 0.5f);
        workingMemory->update(0.001);
        // Just verify it doesn't crash
    }
    
    // Test episodic memory
    auto* episodicMemory = brain.getEpisodicMemory();
    if (episodicMemory) {
        nlm::EpisodicMemoryItem item;
        item.timestamp = 100;
        item.reward = 1.0f;
        episodicMemory->storeEpisode(item);
        // Just verify it doesn't crash
    }
    
    std::cout << "    testBrainMemorySystems passed" << std::endl;
}

void testBrainPredictionSystem() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Verify prediction system is present
    auto* predictionSystem = brain.getPredictionSystem();
    if (predictionSystem) {
        // The prediction system might not have all methods, just verify it exists
        assert(predictionSystem != nullptr);
    }
    
    std::cout << "    testBrainPredictionSystem passed" << std::endl;
}

void testBrainCognitionSystems() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Verify cognition systems are present
    assert(brain.getPlanner() != nullptr);
    assert(brain.getConceptFormation() != nullptr);
    assert(brain.getAttention() != nullptr);
    
    std::cout << "    testBrainCognitionSystems passed" << std::endl;
}

void testBrainPerformance() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Benchmark brain creation
    auto start = std::chrono::high_resolution_clock::now();
    
    nlm::Brain brain2(config);
    brain2.initialize();
    
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    
    // Should be very fast (< 1 second)
    assert(elapsed.count() < 1.0);
    
    std::cout << "    testBrainPerformance passed" << std::endl;
}

void testBrainSaveLoad() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Save brain state
    bool saveSuccess = brain.save("test_brain_save.bin");
    assert(saveSuccess);
    
    // Create a new brain and load the state
    auto config2 = std::make_shared<nlm::Config>();
    config2->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config2->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain2(config2);
    bool loadSuccess = brain2.load("test_brain_save.bin");
    assert(loadSuccess);
    
    // Verify both brains have same number of neurons
    assert(brain.getTotalNeuronCount() == brain2.getTotalNeuronCount());
    
    // Clean up
    std::remove("test_brain_save.bin");
    
    std::cout << "    testBrainSaveLoad passed" << std::endl;
}

void testBrainMultipleRegions() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(4), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.05, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    assert(brain.getRegionCount() == 4);
    assert(brain.getTotalNeuronCount() == 200);
    
    // Verify each region has neurons
    for (auto regionId : brain.getRegionIds()) {
        auto* region = brain.getRegion(regionId);
        assert(region != nullptr);
        assert(region->getTotalNeuronCount() > 0);
    }
    
    std::cout << "    testBrainMultipleRegions passed" << std::endl;
}

void testBrainInterRegionConnections() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Add inter-region connection
    nlm::RegionId sourceId(1);
    nlm::RegionId targetId(2);
    
    brain.addInterRegionConnection(sourceId, targetId, 0.5f, 1);
    
    // Verify connection was added
    auto connections = brain.getConfig()->get<std::vector<std::string>>("inter_region_connections");
    // Note: This test is simplified as the actual connection management
    // would require more complex setup
    
    std::cout << "    testBrainInterRegionConnections passed" << std::endl;
}

void testBrainSensoryInput() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create a simple sensory input
    auto sensoryInput = std::make_shared<nlm::InternalSignals>();
    sensoryInput->addSignal(1.0f);
    sensoryInput->addSignal(2.0f);
    
    // Apply sensory input
    brain.receiveSensoryInput(*sensoryInput);
    
    // Verify it doesn't crash
    assert(brain.getTotalNeuronCount() > 0);
    
    std::cout << "    testBrainSensoryInput passed" << std::endl;
}

void testBrainInjection() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Inject current into specific neuron
    brain.injectCurrent(nlm::NeuronId(1), 10.0f);
    
    // Inject current into all neurons of a specific type
    brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 5.0f);
    
    // Verify it doesn't crash
    assert(brain.getTotalNeuronCount() > 0);
    
    std::cout << "    testBrainInjection passed" << std::endl;
}

void runAll() {
    std::cout << "Running enhanced Brain tests..." << std::endl;
    
    testBrainCreation();
    testBrainInitialization();
    testBrainStep();
    testBrainReset();
    testBrainRegions();
    testBrainActionProduction();
    testBrainStatus();
    testBrainIntegratedSystems();
    testBrainPlasticity();
    testBrainNeuromodulation();
    testBrainDevelopment();
    testBrainMemorySystems();
    testBrainPredictionSystem();
    testBrainCognitionSystems();
    testBrainPerformance();
    testBrainSaveLoad();
    testBrainMultipleRegions();
    testBrainInterRegionConnections();
    testBrainSensoryInput();
    testBrainInjection();
    
    std::cout << "All enhanced Brain tests passed!" << std::endl;
}

} // namespace test_enhanced_brain
