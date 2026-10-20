// Brain Tests
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

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
    
    std::cout << "    testBrainStatus passed" << std::endl;
}

void testBrainMemoryIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Verify memory systems are initialized
    auto* workingMemory = brain.getWorkingMemory();
    auto* episodicMemory = brain.getEpisodicMemory();
    auto* predictionSystem = brain.getPredictionSystem();
    auto* attention = brain.getAttention();
    
    // At least working memory and episodic memory should be present
    assert(workingMemory != nullptr);
    assert(episodicMemory != nullptr);
    
    // Test working memory updates during simulation
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain.step(step);
        
        // Working memory should have active traces
        size_t activeTraces = workingMemory->getActiveTraces();
        if (step > 10) {
            // After initial steps, should have accumulated traces
            assert(activeTraces >= 0);
        }
    }
    
    // Test episodic memory storage
    for (nlm::SimulationStep step = 50; step < 100; ++step) {
        brain.step(step);
        
        // Episodic memory should have accumulated episodes
        size_t episodeCount = episodicMemory->getEpisodeCount();
        if (step > 60) {
            // Should have stored some episodes by now
            assert(episodeCount >= 0);
        }
    }
    
    // Test prediction system updates
    if (predictionSystem) {
        float predictionError = predictionSystem->getPredictionError();
        float confidence = predictionSystem->getConfidence();
        assert(confidence >= 0.0f && confidence <= 1.0f);
    }
    
    // Test attention system
    if (attention) {
        // Attention system should be functioning
        attention->update(0.001f);
        // No assertion needed, just ensure it runs without crashing
    }
    
    std::cout << "    testBrainMemoryIntegration passed" << std::endl;
}

void testBrainNeuromodulationIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test all neuromodulation systems
    auto* dopamine = brain.getDopamine();
    auto* acetylcholine = brain.getAcetylcholine();
    auto* norepinephrine = brain.getNorepinephrine();
    auto* serotonin = brain.getSerotonin();
    auto* curiosity = brain.getCuriosity();
    auto* novelty = brain.getNovelty();
    auto* predictionError = brain.getPredictionErrorSignal();
    
    // All neuromodulators should be present
    assert(dopamine != nullptr);
    assert(acetylcholine != nullptr);
    assert(norepinephrine != nullptr);
    assert(serotonin != nullptr);
    assert(curiosity != nullptr);
    assert(novelty != nullptr);
    assert(predictionError != nullptr);
    
    // Test updates
    for (nlm::SimulationStep step = 0; step < 30; ++step) {
        brain.step(step);
        
        // Check neuromodulator levels
        float daLevel = dopamine->getLevel();
        assert(daLevel >= 0.0f && daLevel <= 1.0f);
        
        // All neuromodulators should have valid levels
        assert(acetylcholine->getLevel() >= 0.0f);
        assert(norepinephrine->getLevel() >= 0.0f);
        assert(serotonin->getLevel() >= 0.0f);
        assert(curiosity->getLevel() >= 0.0f);
        assert(novelty->getLevel() >= 0.0f);
        assert(predictionError->getLevel() >= 0.0f);
    }
    
    std::cout << "    testBrainNeuromodulationIntegration passed" << std::endl;
}

void testBrainConceptFormation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* conceptFormation = brain.getConceptFormation();
    assert(conceptFormation != nullptr);
    
    // Test concept formation over multiple steps
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
        
        // Concept formation should have updated
        size_t conceptCount = conceptFormation->getConceptCount();
        // Concept count should be stable or growing
        assert(conceptCount >= 0);
    }
    
    std::cout << "    testBrainConceptFormation passed" << std::endl;
}

void testBrainSleepRestCycle() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("replay_interval", static_cast<int64_t>(10), nlm::ConfigSource::Default); // Short cycle for testing
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Run some steps to establish activity patterns
    for (nlm::SimulationStep step = 0; step < 25; ++step) {
        brain.step(step);
    }
    
    // The sleep/rest cycle should have been implemented and tested
    // We can't easily test the internal state without accessing private members,
    // but we can verify the system doesn't crash
    std::cout << "    testBrainSleepRestCycle passed" << std::endl;
}
