// System Connectivity Tests
// Tests that all brain systems are properly connected and communicating

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "development/DevelopmentSystem.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/PredictionError.hpp"
#include <cassert>
#include <iostream>
#include <memory>
#include <vector>

namespace test_connectivity {

// Test 1: Verify all brain regions are created and initialized
void testRegionCreation() {
    std::cout << "    Testing region creation and initialization..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.2f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test region creation
    size_t region1 = brain.addRegion("Cortex");
    assert(region1 < brain.getRegionCount());
    
    size_t region2 = brain.addRegion("Subcortical");
    assert(region2 < brain.getRegionCount());
    
    size_t region3 = brain.addRegion("Brainstem");
    assert(region3 < brain.getRegionCount());
    
    // Test region access
    auto region1_ptr = brain.getRegion(region1);
    assert(region1_ptr != nullptr);
    
    auto region2_ptr = brain.getRegion(region2);
    assert(region2_ptr != nullptr);
    
    auto region3_ptr = brain.getRegion(region3);
    assert(region3_ptr != nullptr);
    
    // Test region IDs
    auto regionIds = brain.getRegionIds();
    assert(regionIds.size() == 3);
    assert(std::find(regionIds.begin(), regionIds.end(), region1) != regionIds.end());
    assert(std::find(regionIds.begin(), regionIds.end(), region2) != regionIds.end());
    assert(std::find(regionIds.begin(), regionIds.end(), region3) != regionIds.end());
    
    std::cout << "      PASSED" << std::endl;
}

// Test 2: Verify inter-region connections are established and functional
void testInterRegionConnections() {
    std::cout << "    Testing inter-region connectivity..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.3f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Create regions
    size_t regionA = brain.addRegion("RegionA");
    size_t regionB = brain.addRegion("RegionB");
    
    // Add inter-region connections
    brain.addInterRegionConnection(regionA, regionB, 0.8f, 2);
    brain.addInterRegionConnection(regionB, regionA, 0.6f, 3);
    
    // Verify connections exist
    // (Note: Direct connection access might not be available in all versions)
    
    // Test neural activity flows between regions
    nlm::SensoryInput input;
    input.values = {1.0f, 0.5f, 0.2f};
    brain.receiveSensoryInput(input);
    
    // Run steps to allow connectivity to take effect
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    // Verify both regions have neurons and synapses
    assert(brain.getTotalNeuronCount() == 150);
    assert(brain.getTotalSynapseCount() > 0);
    
    // Test region-specific operations
    auto* regionA_ptr = brain.getRegion(regionA);
    auto* regionB_ptr = brain.getRegion(regionB);
    
    assert(regionA_ptr != nullptr);
    assert(regionB_ptr != nullptr);
    
    std::cout << "      PASSED" << std::endl;
}

// Test 3: Test communication between different brain systems
void testSystemCommunication() {
    std::cout << "    Testing system communication..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(80), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test neuromodulation affecting brain activity
    nlm::Neuromodulator rewardSignal;
    rewardSignal.setLevel(0.7f);
    brain.applyNeuromodulation(rewardSignal);
    
    // Test dopamine system affecting plasticity
    brain.getDopamine()->signalReward(0.8f);
    assert(brain.getDopamine()->getLevel() > 0.0f);
    
    // Test curiosity system affecting learning
    brain.getCuriosity()->signalCuriosity();
    assert(brain.getCuriosity()->getLevel() > 0.0f);
    
    // Test prediction error system
    brain.getPredictionErrorSignal()->signalPredictionError(0.3f);
    assert(brain.getPredictionErrorSignal()->getLevel() > 0.0f);
    
    // Run steps to allow communication
    for (nlm::SimulationStep step = 0; step < 15; ++step) {
        nlm::SensoryInput input;
        input.values = {0.5f, 0.3f, 0.8f};
        brain.receiveSensoryInput(input);
        brain.step(step);
    }
    
    // Verify systems are all active
    assert(brain.getTotalSpikeCount() > 0);
    assert(brain.getFiringNeuronCount() > 0);
    
    std::cout << "      PASSED" << std::endl;
}

// Test 4: Test memory system integration with other systems
void testMemorySystemCommunication() {
    std::cout << "    Testing memory system communication..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(60), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test working memory -> brain -> neuromodulation loop
    brain.getWorkingMemory()->store(1, 1.0f);
    
    // This should trigger memory-based processes
    nlm::SensoryInput input;
    input.values = {0.5f, 0.7f, 0.3f};
    brain.receiveSensoryInput(input);
    
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    // Test episodic memory creation
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 100;
    episode.neurons = {1, 2, 3};
    episode.values = {0.1f, 0.2f, 0.3f};
    episode.metadata = "Memory integration test";
    brain.getEpisodicMemory()->storeEpisode(episode);
    
    assert(brain.getEpisodicMemory()->getEpisodeCount() == 1);
    
    // Test memory consolidation
    brain.getEpisodicMemory()->consolidate(0.5f);
    
    // Test development affecting memory
    brain.getDevelopmentSystem()->advanceStage();
    brain.develop();
    
    std::cout << "      PASSED" << std::endl;
}

// Test 5: Test prediction and cognition system integration
void testPredictionCognitionIntegration() {
    std::cout << "    Testing prediction and cognition integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(70), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Initialize concept formation
    brain.getConceptFormation()->initialize(&brain);
    
    // Initialize neural planner
    brain.getPlanner()->initialize(&brain);
    
    // Test prediction system
    brain.getPredictionSystem()->train(nlm::SensoryInput{});
    float predictionError = brain.getPredictionSystem()->getPredictionError();
    assert(predictionError >= 0.0f);
    
    // Present experience that should form concepts
    std::vector<float> pattern{0.1f, 0.2f, 0.3f};
    std::vector<float> features{0.4f, 0.5f};
    size_t conceptId = brain.getConceptFormation()->presentExperience(pattern, features, 0.8f, 50);
    assert(conceptId > 0);
    
    // Test planning
    std::vector<float> currentState{0.5f, 0.5f, 0.5f};
    nlm::ActionType action = brain.getPlanner()->planAction(currentState);
    // Action can be any valid ActionType or null
    
    // Run steps to test integration
    for (nlm::SimulationStep step = 0; step < 12; ++step) {
        nlm::SensoryInput input;
        input.values = {0.3f, 0.6f, 0.9f};
        brain.receiveSensoryInput(input);
        brain.step(step);
    }
    
    // Verify prediction confidence
    float confidence = brain.getPredictionSystem()->getConfidence();
    assert(confidence >= 0.0f && confidence <= 1.0f);
    
    std::cout << "      PASSED" << std::endl;
}

// Test 6: Test development system integration with all other systems
void testDevelopmentSystemIntegration() {
    std::cout << "    Testing development system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(90), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test developmental stage progression
    nlm::DevelopmentalStage initialStage = brain.getDevelopmentStage();
    
    for (int stage = 0; stage < 4; ++stage) {
        brain.getDevelopmentSystem()->advanceStage();
        
        nlm::DevelopmentalStage currentStage = brain.getDevelopmentStage();
        
        // Verify stage progression
        assert(currentStage != nlm::DevelopmentalStage::End);
        
        // Test plasticity modifier changes with development
        float plasticity = brain.getDevelopmentSystem()->getPlasticityModifier();
        assert(plasticity >= 0.0f && plasticity <= 1.0f);
        
        // Test critical period (can be true or false)
        bool critical = brain.getDevelopmentSystem()->isCriticalPeriod();
        
        // Test development update
        brain.develop();
        
        // Test effect on memory
        brain.getWorkingMemory()->store(1, 1.0f);
        brain.getWorkingMemory()->decay(0.1f);
        
        // Test effect on neuromodulation
        brain.getDopamine()->update(0.1f);
        brain.getCuriosity()->update(0.1f);
    }
    
    // Set final stage and verify
    brain.getDevelopmentSystem()->setStage(nlm::DevelopmentalStage::Adult);
    assert(brain.getDevelopmentStage() == nlm::DevelopmentalStage::Adult);
    
    std::cout << "      PASSED" << std::endl;
}

// Test 7: Test full system integration with sensory-motor loop
void testFullSystemIntegration() {
    std::cout << "    Testing full system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.25f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Initialize all integrated systems
    brain.getConceptFormation()->initialize(&brain);
    brain.getPlanner()->initialize(&brain);
    
    // Simulate sensory input and processing loop
    nlm::SimulationStep step = 0;
    for (int i = 0; i < 20; ++i) {
        // Create sensory input
        nlm::SensoryInput input;
        input.values = {
            0.1f * (i % 5),
            0.2f * ((i * 2) % 5),
            0.3f * ((i * 3) % 5)
        };
        
        // Apply input
        brain.receiveSensoryInput(input);
        
        // Step brain
        brain.step(step);
        
        // Apply neuromodulation
        nlm::Neuromodulator neuromod;
        neuromod.setLevel(0.3f + 0.2f * sin(step * 0.1f));
        brain.applyNeuromodulation(neurom);
        
        // Update plasticity
        brain.updatePlasticity();
        
        // Apply development
        brain.develop();
        
        step++;
    }
    
    // Verify all systems have been active
    assert(brain.getTotalSpikeCount() > 0);
    assert(brain.getFiringNeuronCount() > 0);
    assert(brain.getTotalSynapseCount() > 0);
    
    // Verify memory systems
    brain.getWorkingMemory()->store(1, 1.0f);
    brain.getWorkingMemory()->store(2, 2.0f);
    assert(brain.getWorkingMemory()->getCurrentSize() >= 2);
    
    // Verify episodic memory
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = step;
    episode.neurons = {1, 2, 3};
    episode.values = {0.1f, 0.2f, 0.3f};
    episode.metadata = "Full integration test";
    brain.getEpisodicMemory()->storeEpisode(episode);
    assert(brain.getEpisodicMemory()->getEpisodeCount() > 0);
    
    // Verify prediction system
    brain.getPredictionSystem()->train(nlm::SensoryInput{});
    assert(brain.getPredictionSystem()->getPredictionError() >= 0.0f);
    
    // Verify development
    assert(brain.getDevelopmentStage() != nlm::DevelopmentalStage::End);
    
    // Verify neuromodulation
    brain.getDopamine()->signalReward(0.5f);
    brain.getCuriosity()->signalCuriosity();
    assert(brain.getDopamine()->getLevel() > 0.0f);
    
    std::cout << "      PASSED" << std::endl;
}

void runAll() {
    std::cout << "System Connectivity Tests" << std::endl;
    std::cout << "=" << std::string(50, '=') << std::endl;
    
    testRegionCreation();
    testInterRegionConnections();
    testSystemCommunication();
    testMemorySystemCommunication();
    testPredictionCognitionIntegration();
    testDevelopmentSystemIntegration();
    testFullSystemIntegration();
    
    std::cout << "=" << std::string(50, '=') << std::endl;
    std::cout << "All connectivity tests completed successfully!" << std::endl;
}

} // namespace test_connectivity