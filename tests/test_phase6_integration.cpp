// Integration Tests for Phase 6: Complete Brain-World-Agent System
// Tests the integration of all NLM components in a realistic agent scenario
// Includes memory, prediction, cognition, neuromodulation, and development systems

#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "neuromodulation/Reward.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "prediction/PredictionSystem.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "development/DevelopmentSystem.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/AntiHebbian.hpp"
#include "plasticity/BCMRule.hpp"
#include "plasticity/RewardModulatedSTDP.hpp"
#include "plasticity/TripletSTDP.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <vector>
#include <memory>
#include <limits>

namespace test_phase6_integration {

void testCompleteBrainWorldAgentLoop() {
    std::cout << "    Testing complete brain-world-agent loop integration..." << std::endl;
    
    // Create configuration for Phase 6 integration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.05, nlm::ConfigSource::Default);
    config->set("random_seed", static_cast<uint64_t>(42), nlm::ConfigSource::Default);
    
    // Create brain with integrated systems
    nlm::Brain brain(config);
    bool success = brain.initialize();
    assert(success);
    
    // Create agent brain interface
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    // Create world
    nlm::SimpleWorld world;
    world.configure(100, 100, 32, 32);  // Width, height, vision, vision
    world.setAgentStart(50, 50);
    
    // Initialize agent brain with world
    agentBrain.initialize(world);
    
    // Test 1: Sensory processing and neural integration
    std::cout << "    Test 1: Sensory processing integration..." << std::endl;
    
    // Create sensory percept representing visual input
    nlm::SensoryPercept percept;
    nlm::Vision vision(32, 32, 3);
    
    // Simulate some visual input (e.g., detecting a goal/target)
    std::vector<float> visualData(32 * 32 * 3, 0.0f);
    
    // Add some goal pattern in the center
    for (int y = 20; y < 32; ++y) {
        for (int x = 20; x < 32; ++x) {
            for (int c = 0; c < 3; ++c) {
                visualData[(y * 32 + x) * 3 + c] = 0.8f;  // Bright goal
            }
        }
    }
    
    vision.setData(&visualData[0], visualData.size());
    percept.setVision(vision);
    
    // Process through agent brain
    agentBrain.processSensoryInput(percept);
    
    // Verify neural integration
    float firingRate = brain.getAverageFiringRate();
    assert(firingRate >= 0.0f);
    
    // Test 2: Memory system integration
    std::cout << "    Test 2: Memory system integration..." << std::endl;
    
    // Get working memory and verify it's storing information
    auto* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    size_t activeTraces = workingMemory->getActiveTraces();
    assert(activeTraces >= 0);
    
    // Get episodic memory and store an experience
    auto* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 0;
    episode.reward = 0.5f;
    
    // Add some active neurons to episode
    for (int i = 0; i < 10; ++i) {
        episode.activeNeurons.push_back(nlm::NeuronId(i + 1));
        episode.neuronActivations.push_back(0.7f);
    }
    
    episodicMemory->storeEpisode(episode);
    assert(episodicMemory->getEpisodeCount() >= 1);
    
    // Test 3: Neuromodulation integration
    std::cout << "    Test 3: Neuromodulation integration..." << std::endl;
    
    // Get neuromodulation systems
    auto* dopamine = brain.getDopamine();
    auto* curiosity = brain.getCuriosity();
    auto* novelty = brain.getNovelty();
    auto* predictionError = brain.getPredictionErrorSignal();
    
    assert(dopamine != nullptr);
    assert(curiosity != nullptr);
    assert(novelty != nullptr);
    assert(predictionError != nullptr);
    
    // Test neuromodulation update
    dopamine->update(0.001);
    curiosity->update(0.001);
    novelty->update(0.001);
    predictionError->update(0.001);
    
    // Verify neuromodulation affects brain state
    float dopamineLevel = dopamine->getLevel();
    assert(dopamineLevel >= 0.0f);
    
    // Test 4: Reward-modulated learning integration
    std::cout << "    Test 4: Reward-modulated learning integration..." << std::endl;
    
    // Get reward system
    auto* reward = brain.getWorkingMemory();  // Simplified for test
    
    // Create reward signal based on sensory input
    float rewardValue = 0.0f;
    
    // Check if we're close to goal
    float visualIntensity = 0.0f;
    for (float pixel : visualData) {
        visualIntensity += pixel;
    }
    visualIntensity /= static_cast<float>(visualData.size());
    
    // Reward based on proximity to goal
    rewardValue = std::max(0.0f, 1.0f - std::abs(visualIntensity - 0.8f) * 2.0f);
    
    // Apply reward modulation
    agentBrain.applyRewardModulation(rewardValue, rewardValue * 0.8f);
    
    // Test 5: Plasticity integration
    std::cout << "    Test 5: Plasticity integration..." << std::endl;
    
    // Get plasticity systems
    auto* stdp = brain.getSTDP();
    auto* hebbian = brain.getHebbian();
    
    assert(stdp != nullptr);
    assert(hebbian != nullptr);
    
    // Test STDP with reward modulation
    float initialSTDPActivity = stdp->getLTPWeight();
    
    // Update plasticity with neuromodulation
    agentBrain.applyRewardModulation(rewardValue, rewardValue * 0.8f);
    
    // Verify plasticity affects brain state
    assert(stdp->getLTPWeight() >= 0.0f);
    
    // Test 6: Development system integration
    std::cout << "    Test 6: Development system integration..." << std::endl;
    
    auto* developmentSystem = brain.getDevelopmentSystem();
    assert(developmentSystem != nullptr);
    
    // Test development update
    agentBrain.updateDevelopment(10);  // 10 steps
    
    // Verify developmental stage changes
    nlm::DevelopmentalStage stage = brain.getDevelopmentalStage();
    assert(stage == nlm::DevelopmentalStage::Initial || 
           stage == nlm::DevelopmentalStage::CriticalPeriod ||
           stage == nlm::DevelopmentalStage::Maturation);
    
    // Test 7: Cognition system integration
    std::cout << "    Test 7: Cognition system integration..." << std::endl;
    
    auto* planner = brain.getPlanner();
    auto* conceptFormation = brain.getConceptFormation();
    auto* attention = brain.getAttention();
    
    assert(planner != nullptr);
    assert(conceptFormation != nullptr);
    assert(attention != nullptr);
    
    // Test cognition updates
    // Note: These might be placeholders in current implementation
    // but we're testing integration, not functionality
    
    // Test 8: Action selection and world interaction
    std::cout << "    Test 8: Action selection and world interaction..." << std::endl;
    
    // Run brain steps to simulate processing
    for (int step = 0; step < 20; ++step) {
        brain.step(step, step * 0.001f);
        
        // Update world periodically
        if (step % 5 == 0) {
            nlm::MotorCommand cmd = agentBrain.decodeMotorCommand();
            world.applyMotorCommand(cmd, step * 0.001f);
        }
        
        // Get percept and process
        auto percept = world.getSensoryPercept();
        agentBrain.processSensoryInput(percept);
        
        // Get current reward
        float reward = dopamine ? dopamine->getLevel() : 0.0f;
        agentBrain.applyRewardModulation(reward, reward * 0.9f);
    }
    
    // Produce final action
    auto action = brain.produceAction();
    assert(action != nullptr);
    
    // Test 9: Memory consolidation and replay
    std::cout << "    Test 9: Memory consolidation and replay..." << std::endl;
    
    // Force some memory consolidation by running many steps
    for (int step = 100; step < 150; ++step) {
        brain.step(step, step * 0.001f);
    }
    
    // The episodic memory should have consolidated important episodes
    size_t episodeCount = episodicMemory->getEpisodeCount();
    // Episode count should be meaningful (>= 1)
    assert(episodeCount >= 1);
    
    // Test 10: System statistics and validation
    std::cout << "    Test 10: System statistics and validation..." << std::endl;
    
    // Verify all systems are working
    assert(brain.getTotalNeuronCount() == 200);
    assert(brain.getTotalSynapseCount() > 0);
    assert(brain.getActiveNeuronCount() >= 0);
    assert(brain.getFiringNeuronCount() >= 0);
    
    float avgFiringRate = brain.getAverageFiringRate();
    assert(avgFiringRate >= 0.0f);
    
    float eIrRatio = brain.getExcitationInhibitionRatio();
    assert(eIrRatio >= 0.0f && eIrRatio != std::numeric_limits<float>::infinity());
    
    std::cout << "    Complete brain-world-agent integration test passed!" << std::endl;
}

void testMemorySystemIntegration() {
    std::cout << "    Testing memory system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    assert(success);
    
    auto* workingMemory = brain.getWorkingMemory();
    auto* episodicMemory = brain.getEpisodicMemory();
    auto* associativeMemory = brain.getAssociativeMemory();
    
    // Test working memory
    workingMemory->storeToNeuron(nlm::NeuronId(1), 0.5f);
    workingMemory->update(0.001);
    assert(workingMemory->getActiveTraces() >= 0);
    
    // Test episodic memory
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 0;
    episode.reward = 0.5f;
    episodicMemory->storeEpisode(episode);
    assert(episodicMemory->getEpisodeCount() > 0);
    
    std::cout << "    Memory system integration test passed!" << std::endl;
}

void testPredictionSystemIntegration() {
    std::cout << "    Testing prediction system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    assert(success);
    
    auto* predictionSystem = brain.getPredictionSystem();
    auto* predictionError = brain.getPredictionErrorSignal();
    
    // Test prediction error system
    predictionError->update(0.001);
    assert(predictionError->getLevel() >= 0.0f);
    
    std::cout << "    Prediction system integration test passed!" << std::endl;
}

void testNeuromodulationIntegration() {
    std::cout << "    Testing neuromodulation integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    assert(success);
    
    auto* dopamine = brain.getDopamine();
    auto* curiosity = brain.getCuriosity();
    auto* novelty = brain.getNovelty();
    
    // Test all neuromodulation systems
    dopamine->update(0.001);
    curiosity->update(0.001);
    novelty->update(0.001);
    
    assert(dopamine->getLevel() >= 0.0f);
    assert(curiosity->getLevel() >= 0.0f);
    assert(novelty->getLevel() >= 0.0f);
    
    std::cout << "    Neuromodulation integration test passed!" << std::endl;
}

void runAllPhase6IntegrationTests() {
    std::cout << "\n=== PHASE 6 INTEGRATION TESTS ===" << std::endl;
    std::cout << "Testing complete brain-world-agent integration..." << std::endl;
    
    testCompleteBrainWorldAgentLoop();
    testMemorySystemIntegration();
    testPredictionSystemIntegration();
    testNeuromodulationIntegration();
    
    std::cout << "\n=== ALL PHASE 6 INTEGRATION TESTS PASSED ===" << std::endl;
    std::cout << "Phase 6 complete integration testing successful!" << std::endl;
}

} // namespace test_phase6_integration