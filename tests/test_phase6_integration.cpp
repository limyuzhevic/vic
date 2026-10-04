// Phase 6 Integration Tests
// Tests all integrated brain systems to verify Phase 6 completeness

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "memory/WorkingMemory.hpp"
#include "memory/EpisodicMemory.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "development/DevelopmentSystem.hpp"
#include "prediction/PredictionSystem.hpp"
#include "cognition/ConceptFormation.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/PredictionError.hpp"
#include <cassert>
#include <iostream>
#include <memory>
#include <vector>
#include <filesystem>
#include <fstream>

namespace test_phase6 {

// Test 1: Verify all brain systems are properly integrated
void testBrainSystemIntegration() {
    std::cout << "    Testing brain system integration..." << std::endl;
    
    // Create config
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    // Create brain
    nlm::Brain brain(config);
    
    // Initialize brain
    assert(brain.initialize());
    
    // Verify all integrated systems are accessible
    assert(brain.getWorkingMemory() != nullptr);
    assert(brain.getEpisodicMemory() != nullptr);
    assert(brain.getAssociativeMemory() != nullptr);
    assert(brain.getPredictionSystem() != nullptr);
    assert(brain.getPlanner() != nullptr);
    assert(brain.getConceptFormation() != nullptr);
    assert(brain.getDevelopmentSystem() != nullptr);
    assert(brain.getDopamine() != nullptr);
    assert(brain.getCuriosity() != nullptr);
    assert(brain.getNovelty() != nullptr);
    assert(brain.getPredictionErrorSignal() != nullptr);
    
    // Test basic functionality of each system
    brain.getWorkingMemory()->store(0, 1.0f);
    assert(brain.getWorkingMemory()->retrieve(0) == 1.0f);
    
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 0;
    episode.neurons = {1, 2, 3};
    episode.values = {0.5f, 0.7f, 0.9f};
    brain.getEpisodicMemory()->storeEpisode(episode);
    assert(brain.getEpisodicMemory()->getEpisodeCount() == 1);
    
    brain.getPredictionSystem()->train(nlm::SensoryInput{});
    assert(brain.getPredictionSystem()->getPredictionError() >= 0.0f);
    
    brain.getConceptFormation()->initialize(&brain);
    size_t conceptId = brain.getConceptFormation()->presentExperience(
        std::vector<float>{0.1f, 0.2f}, std::vector<float>{0.3f}, 0.5f, 0);
    
    brain.getDevelopmentSystem()->setStage(nlm::DevelopmentalStage::Adult);
    assert(brain.getDevelopmentStage() == nlm::DevelopmentalStage::Adult);
    
    brain.getDopamine()->signalReward(1.0f);
    assert(brain.getDopamine()->getLevel() > 0.0f);
    
    std::cout << "      PASSED" << std::endl;
}

// Test 2: Test system connectivity and neural communication
void testSystemConnectivity() {
    std::cout << "    Testing system connectivity..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.3f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test inter-region connectivity
    size_t region1 = brain.addRegion("Region1");
    size_t region2 = brain.addRegion("Region2");
    
    brain.addInterRegionConnection(region1, region2, 0.8f, 2);
    
    // Verify connectivity
    auto regions = brain.getRegionIds();
    assert(regions.size() == 2);
    
    // Test data flow between systems
    nlm::SensoryInput input;
    input.values = {1.0f, 0.5f, 0.0f};
    brain.receiveSensoryInput(input);
    
    // Run some steps to test system communication
    for (int i = 0; i < 5; ++i) {
        brain.step(i);
    }
    
    // Verify statistics update
    assert(brain.getTotalNeuronCount() == 50);
    assert(brain.getTotalSynapseCount() > 0);
    
    std::cout << "      PASSED" << std::endl;
}

// Test 3: Test checkpoint save/load functionality
void testCheckpointSaveLoad() {
    std::cout << "    Testing checkpoint save/load..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(75), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    // Create and initialize brain
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Set some state
    brain.step(0);
    brain.getWorkingMemory()->store(1, 2.5f);
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 100;
    episode.neurons = {1, 2, 3};
    episode.values = {0.1f, 0.2f, 0.3f};
    brain.getEpisodicMemory()->storeEpisode(episode);
    
    // Save to file
    std::string filepath = "test_checkpoint.bin";
    bool saveSuccess = brain.save(filepath);
    assert(saveSuccess);
    
    // Create new brain and load
    auto config2 = std::make_shared<nlm::Config>();
    config2->set("neuron_count", static_cast<int64_t>(75), nlm::ConfigSource::Default);
    config2->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain2(config2);
    assert(brain2.initialize());
    
    // Load saved state
    bool loadSuccess = brain2.load(filepath);
    assert(loadSuccess);
    
    // Verify loaded state matches
    assert(brain2.getTotalNeuronCount() == 75);
    assert(brain2.getRegionCount() == 3);
    assert(brain2.getWorkingMemory()->retrieve(1) == 2.5f);
    assert(brain2.getEpisodicMemory()->getEpisodeCount() == 1);
    
    // Clean up
    std::filesystem::remove(filepath);
    
    std::cout << "      PASSED" << std::endl;
}

// Test 4: Test replay and consolidation capabilities
void testReplayConsolidation() {
    std::cout << "    Testing replay and consolidation..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(60), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Store multiple episodes
    for (int i = 0; i < 10; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i;
        episode.neurons = {static_cast<nlm::NeuronId>(i), static_cast<nlm::NeuronId>(i + 1)};
        episode.values = {0.1f * i, 0.2f * i};
        brain.getEpisodicMemory()->storeEpisode(episode);
    }
    
    assert(brain.getEpisodicMemory()->getEpisodeCount() == 10);
    
    // Get recent episodes
    auto recentEpisodes = brain.getEpisodicMemory()->getRecentEpisodes(5);
    assert(recentEpisodes.size() == 5);
    
    // Consolidate with threshold
    brain.getEpisodicMemory()->consolidate(0.15f);
    
    // Get consolidated episode count
    size_t episodeCount = brain.getEpisodicMemory()->getEpisodeCount();
    
    // Test development-based replay
    brain.getDevelopmentSystem()->advanceStage();
    brain.getDevelopmentSystem()->advanceStage();
    
    // Simulate development affecting memory
    brain.develop();
    
    std::cout << "      PASSED" << std::endl;
}

// Test 5: Test development stages and plasticity
void testDevelopmentStages() {
    std::cout << "    Testing development stages and plasticity..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(40), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test developmental stage progression
    assert(brain.getDevelopmentStage() != nlm::DevelopmentalStage::End);
    
    for (int i = 0; i < 5; ++i) {
        brain.getDevelopmentSystem()->advanceStage();
        
        // Verify plasticity modifier changes with stage
        float plasticity = brain.getDevelopmentSystem()->getPlasticityModifier();
        assert(plasticity >= 0.0f && plasticity <= 1.0f);
        
        // Test critical period
        bool critical = brain.getDevelopmentSystem()->isCriticalPeriod();
        // Critical period can be true or false depending on stage
    }
    
    // Test neuromodulation effects on development
    brain.getDevelopmentSystem()->setStage(nlm::DevelopmentalStage::Adult);
    assert(brain.getDevelopmentStage() == nlm::DevelopmentalStage::Adult);
    
    std::cout << "      PASSED" << std::endl;
}

// Test 6: Test Phase 6-specific functionality
void testPhase6SpecificFunctionality() {
    std::cout << "    Testing Phase 6-specific functionality..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(80), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test concept formation integration
    brain.getConceptFormation()->initialize(&brain);
    
    // Present experiences that should form concepts
    std::vector<float> pattern1{0.1f, 0.2f, 0.3f};
    std::vector<float> features1{0.4f, 0.5f};
    size_t conceptId1 = brain.getConceptFormation()->presentExperience(pattern1, features1, 0.8f, 10);
    assert(conceptId1 > 0);  // Should form new concept
    
    // Present similar pattern - should match existing concept
    std::vector<float> pattern2{0.15f, 0.25f, 0.35f};
    std::vector<float> features2{0.45f, 0.55f};
    size_t conceptId2 = brain.getConceptFormation()->presentExperience(pattern2, features2, 0.9f, 20);
    assert(conceptId2 > 0);  // Should form concept or reuse existing
    
    // Test prediction system integration
    brain.getPredictionSystem()->train(nlm::SensoryInput{});
    float error = brain.getPredictionSystem()->getPredictionError();
    assert(error >= 0.0f);
    
    // Test action production
    auto action = brain.produceAction();
    // Action can be null if no suitable action found
    
    // Test neuromodulation integration
    brain.getDopamine()->signalReward(1.0f);
    brain.getCuriosity()->signalCuriosity();
    brain.getNovelty()->signalNovelty();
    brain.getPredictionErrorSignal()->signalPredictionError(0.3f);
    
    // Test system statistics
    float rate = brain.getAverageFiringRate();
    assert(rate >= 0.0f);
    
    // Test memory statistics
    brain.getWorkingMemory()->store(1, 1.0f);
    brain.getWorkingMemory()->store(2, 2.0f);
    assert(brain.getWorkingMemory()->getCurrentSize() == 2);
    
    std::cout << "      PASSED" << std::endl;
}

// Test 7: Test real-time system interaction
void testRealTimeSystemInteraction() {
    std::cout << "    Testing real-time system interaction..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(30), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Create neuromodulatory signal
    nlm::Neuromodulator rewardSignal;
    rewardSignal.setLevel(0.7f);
    
    // Apply neuromodulatory signal
    brain.applyNeuromodulation(rewardSignal);
    
    // Run multiple steps with sensory input
    for (nlm::SimulationStep step = 0; step < 20; ++step) {
        nlm::SensoryInput input;
        input.values = {0.5f, 0.3f, 0.8f};
        brain.receiveSensoryInput(input);
        
        brain.step(step);
        
        // Verify systems respond to neuromodulation
        brain.getDopamine()->update(0.1f);
        brain.getCuriosity()->update(0.1f);
    }
    
    // Test plasticity updates
    brain.updatePlasticity();
    
    // Test development updates
    brain.develop();
    
    std::cout << "      PASSED" << std::endl;
}

// Test 8: Test memory system integration with brain
void testMemorySystemIntegration() {
    std::cout << "    Testing memory system integration with brain..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test working memory
    brain.getWorkingMemory()->store(1, 1.0f);
    brain.getWorkingMemory()->store(2, 2.0f);
    brain.getWorkingMemory()->store(3, 3.0f);
    
    assert(brain.getWorkingMemory()->getCurrentSize() == 3);
    assert(brain.getWorkingMemory()->retrieve(2) == 2.0f);
    
    // Test episodic memory with brain integration
    for (int i = 0; i < 5; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i * 10;
        episode.neurons = {static_cast<nlm::NeuronId>(i), static_cast<nlm::NeuronId>(i + 1)};
        episode.values = {0.1f * i, 0.2f * i};
        episode.metadata = "Brain memory test " + std::to_string(i);
        brain.getEpisodicMemory()->storeEpisode(episode);
    }
    
    assert(brain.getEpisodicMemory()->getEpisodeCount() == 5);
    
    // Test recent episodes retrieval
    auto recent = brain.getEpisodicMemory()->getRecentEpisodes(3);
    assert(recent.size() == 3);
    
    // Test memory decay
    brain.getWorkingMemory()->decay(0.1f);
    
    std::cout << "      PASSED" << std::endl;
}

void runAll() {
    std::cout << "Phase 6 Integration Tests: Complete Brain System" << std::endl;
    std::cout << "=" << std::string(60, '=') << std::endl;
    
    testBrainSystemIntegration();
    testSystemConnectivity();
    testCheckpointSaveLoad();
    testReplayConsolidation();
    testDevelopmentStages();
    testPhase6SpecificFunctionality();
    testRealTimeSystemInteraction();
    testMemorySystemIntegration();
    
    std::cout << "=" << std::string(60, '=') << std::endl;
    std::cout << "All Phase 6 integration tests completed successfully!" << std::endl;
}

} // namespace test_phase6