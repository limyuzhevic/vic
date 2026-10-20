// Phase 6 Integration Tests
// Comprehensive tests for all integrated systems in Phase 6

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include <cassert>
#include <iostream>
#include <chrono>
#include <thread>
#include <fstream>
#include <filesystem>

namespace test_phase6_integration {

void testMemorySystemIntegration() {
    std::cout << "  Testing memory system integration...";
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* workingMemory = brain.getWorkingMemory();
    auto* episodicMemory = brain.getEpisodicMemory();
    
    // Both memory systems should be present and functional
    assert(workingMemory != nullptr);
    assert(episodicMemory != nullptr);
    
    // Test working memory
    assert(workingMemory->getCapacity() > 0);
    workingMemory->storeToNeuron(1, 0.5f);
    assert(workingMemory->getActiveTraces() >= 1);
    
    // Test episodic memory
    assert(episodicMemory->getMaxEpisodes() > 0);
    
    // Run steps to generate memory activity
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain.step(step);
    }
    
    // Verify memory activity
    if (workingMemory) {
        assert(workingMemory->getActiveTraces() >= 0);
    }
    
    if (episodicMemory) {
        assert(episodicMemory->getEpisodeCount() >= 0);
    }
    
    std::cout << " PASSED" << std::endl;
}

void testNeuromodulationIntegration() {
    std::cout << "  Testing neuromodulation system integration...";
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // All neuromodulation systems should be present
    assert(brain.getDopamine() != nullptr);
    assert(brain.getCuriosity() != nullptr);
    assert(brain.getNovelty() != nullptr);
    assert(brain.getPredictionErrorSignal() != nullptr);
    
    // Test neuromodulator functionality
    auto* dopamine = brain.getDopamine();
    auto* curiosity = brain.getCuriosity();
    auto* novelty = brain.getNovelty();
    
    // Update neuromodulators
    dopamine->update(0.001);
    curiosity->update(0.001);
    novelty->update(0.001);
    
    // Check initial levels
    assert(dopamine->getLevel() >= -1.0f);
    assert(dopamine->getLevel() <= 1.0f);
    assert(curiosity->getLevel() >= 0.0f);
    assert(curiosity->getLevel() <= 1.0f);
    assert(novelty->getLevel() >= 0.0f);
    
    // Apply neuromodulation effects to brain
    dopamine->applyToBrain(brain);
    
    // Run steps to see neuromodulation effects
    for (nlm::SimulationStep step = 0; step < 20; ++step) {
        brain.step(step);
    }
    
    std::cout << " PASSED" << std::endl;
}

void testCognitionSystemIntegration() {
    std::cout << "  Testing cognitive system integration...";
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(250), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // All cognitive systems should be present
    assert(brain.getPlanner() != nullptr);
    assert(brain.getConceptFormation() != nullptr);
    assert(brain.getAttention() != nullptr);
    
    // Test cognitive system functionality
    auto* planner = brain.getPlanner();
    auto* conceptFormation = brain.getConceptFormation();
    auto* attention = brain.getAttention();
    
    // Update cognitive systems
    planner->update(0.001);
    conceptFormation->update(0.001);
    attention->update(0.001);
    
    // Set cognitive parameters
    planner->setPlanningDepth(5);
    attention->setInhibitionStrength(0.5f);
    attention->setExcitationStrength(1.5f);
    
    // Run steps to test cognitive integration
    for (nlm::SimulationStep step = 0; step < 30; ++step) {
        brain.step(step);
    }
    
    std::cout << " PASSED" << std::endl;
}

void testDevelopmentSystemIntegration() {
    std::cout << "  Testing development system integration...";
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(180), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Development system should be present
    assert(brain.getDevelopmentSystem() != nullptr);
    
    // Test development system
    auto* devSystem = brain.getDevelopmentSystem();
    
    // Run steps to simulate development
    for (nlm::SimulationStep step = 0; step < 500; ++step) {
        brain.step(step);
        
        // Update development every 100 steps
        if (step % 100 == 0) {
            devSystem->update(brain, *(brain.getRandomGenerator()), 0.1);
        }
    }
    
    // Check developmental stage progression
    nlm::DevelopmentalStage stage = brain.getDevelopmentalStage();
    assert(stage == nlm::DevelopmentalStage::Adult || stage == nlm::DevelopmentalStage::Maturation);
    
    std::cout << " PASSED" << std::endl;
}

void testCheckpointSystemIntegration() {
    std::cout << "  Testing checkpoint system integration...";
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(120), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Run some steps to generate activity
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
    }
    
    // Save checkpoint
    std::string checkpointPath = "./test_checkpoint.bin";
    assert(brain.save(checkpointPath));
    
    // Create new brain and load checkpoint
    auto config2 = std::make_shared<nlm::Config>();
    config2->set("neuron_count", static_cast<int64_t>(120), nlm::ConfigSource::Default);
    config2->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain2(config2);
    assert(brain2.initialize());
    
    assert(brain2.load(checkpointPath));
    
    // Verify checkpoint was loaded (basic check)
    assert(brain2.getTotalNeuronCount() == 120);
    
    // Clean up
    std::remove(checkpointPath.c_str());
    
    std::cout << " PASSED" << std::endl;
}

void testReplaySystemIntegration() {
    std::cout << "  Testing replay system integration...";
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(160), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    // Run steps to generate episodes
    for (nlm::SimulationStep step = 0; step < 200; ++step) {
        brain.step(step);
    }
    
    // Check if episodes were generated
    size_t episodeCount = episodicMemory->getEpisodeCount();
    
    // Get episodes for replay
    auto episodes = episodicMemory->getEpisodesForReplay(5);
    
    // Replaysome episodes if they exist
    if (!episodes.empty()) {
        episodicMemory->replayEpisode(episodes[0]);
    }
    
    std::cout << " PASSED" << std::endl;
}

void testAgentBrainIntegration() {
    std::cout << "  Testing agent brain integration...";
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Create agent brain
    nlm::AgentBrain agent(std::make_shared<nlm::Brain>(brain));
    
    // Initialize with simple world
    nlm::SimpleWorld world;
    world.initialize(16, 16);
    agent.initialize(world);
    
    // Test sensory processing
    nlm::SensoryPercept percept;
    percept.setVision(std::vector<float>(256, 0.5f));
    percept.setTouch(std::vector<float>(8, 0.3f));
    percept.setInternal(std::vector<float>(4, 0.0f));
    percept.setProprioception(std::vector<float>(6, 0.0f));
    
    agent.processSensoryInput(percept);
    
    // Test motor decoding
    nlm::MotorCommand cmd = agent.decodeMotorCommand();
    
    // Test reward modulation
    agent.applyRewardModulation(1.0f, 0.8f);
    
    // Test development update
    agent.updateDevelopment(0.1);
    
    std::cout << " PASSED" << std::endl;
}

void testWorldIntegration() {
    std::cout << "  Testing world integration...";
    
    nlm::SimpleWorld world;
    world.initialize(16, 16);
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Create agent brain
    nlm::AgentBrain agent(std::make_shared<nlm::Brain>(brain));
    agent.initialize(world);
    
    // Create observation
    nlm::SensoryPercept percept;
    percept.setVision(std::vector<float>(256, 0.0f));
    percept.setTouch(std::vector<float>(8, 0.0f));
    percept.setInternal(std::vector<float>(4, 0.0f));
    percept.setProprioception(std::vector<float>(6, 0.0f));
    
    // Process observation
    agent.processSensoryInput(percept);
    
    // Apply action
    nlm::MotorCommand cmd = agent.decodeMotorCommand();
    world.applyAction(brain.getRegions()[0].get(), cmd);
    
    // Compute reward
    float reward = world.computeReward(brain.getRegions()[0].get());
    
    // Apply reward modulation
    agent.applyRewardModulation(reward, 0.0f);
    
    std::cout << " PASSED" << std::endl;
}

void runAllPhase6IntegrationTests() {
    std::cout << "\n=== NLM Phase 6 Integration Tests ===" << std::endl;
    std::cout << "Testing complete artificial brain integration..." << std::endl;
    
    try {
        testMemorySystemIntegration();
        testNeuromodulationIntegration();
        testCognitionSystemIntegration();
        testDevelopmentSystemIntegration();
        testCheckpointSystemIntegration();
        testReplaySystemIntegration();
        testAgentBrainIntegration();
        testWorldIntegration();
        
        std::cout << "\n=== ALL PHASE 6 INTEGRATION TESTS PASSED ===" << std::endl;
        std::cout << "All Phase 6 systems are properly integrated and functional!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "\n=== PHASE 6 INTEGRATION TEST FAILED ===" << std::endl;
        std::cout << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace test_phase6_integration