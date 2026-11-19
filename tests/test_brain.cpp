// NLM Integration Tests
//
// Comprehensive integration tests for the NLM framework.
// These tests verify that all systems work together properly.

#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "cognition/ConceptFormation.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "development/DevelopmentSystem.hpp"
#include "prediction/PredictionSystem.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <sstream>
#include <chrono>

using namespace nlm;
namespace test_integration {

// Helper function for percentile calculation
double percentile(int percent, const std::vector<double>& data) {
    if (data.empty()) return 0.0;
    
    std::vector<double> sorted = data;
    std::sort(sorted.begin(), sorted.end());
    
    int index = (percent * sorted.size() - 1) / 100;
    if (index < 0) index = 0;
    if (index >= sorted.size()) index = sorted.size() - 1;
    
    return sorted[index];
}

// Test 1: Complete brain initialization and basic functionality
void testCompleteBrainInitialization() {
    std::cout << "=== Test 1: Complete Brain Initialization ===" << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(200), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    
    Brain brain(config);
    
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    // Verify brain properties
    assert(brain.getRegionCount() == 3);
    assert(brain.getTotalNeuronCount() == 200);
    assert(brain.getTotalSynapseCount() > 0);
    
    // Verify all systems are initialized
    assert(brain.getWorkingMemory() != nullptr);
    assert(brain.getEpisodicMemory() != nullptr);
    assert(brain.getDopamine() != nullptr);
    assert(brain.getCuriosity() != nullptr);
    assert(brain.getNovelty() != nullptr);
    assert(brain.getPredictionSystem() != nullptr);
    assert(brain.getPlanner() != nullptr);
    assert(brain.getConceptFormation() != nullptr);
    assert(brain.getDevelopmentSystem() != nullptr);
    
    // Run some simulation steps
    for (int step = 0; step < 100; ++step) {
        brain.step(step, step * 0.001);
    }
    
    // Verify activity
    assert(brain.getTotalSpikeCount() > 0);
    assert(brain.getFiringNeuronCount() > 0);
    
    std::cout << "    ✓ Complete brain initialization test passed" << std::endl;
}

// Test 2: Neuromodulation integration
void testNeuromodulationIntegration() {
    std::cout << "=== Test 2: Neuromodulation Integration ===" << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    
    Brain brain(config);
    assert(brain.initialize());
    
    // Get neuromodulation systems
    auto* dopamine = brain.getDopamine();
    auto* curiosity = brain.getCuriosity();
    auto* novelty = brain.getNovelty();
    
    assert(dopamine != nullptr);
    assert(curiosity != nullptr);
    assert(novelty != nullptr);
    
    // Test neuromodulation signal application
    Neuromodulator rewardSignal(1.0f, NeuromodulatorType::Reward);
    Neuromodulator curiositySignal(0.5f, NeuromodulatorType::Curiosity);
    Neuromodulator noveltySignal(0.3f, NeuromodulatorType::Novelty);
    
    brain.applyNeuromodulation(rewardSignal);
    brain.applyNeuromodulation(curiositySignal);
    brain.applyNeuromodulation(noveltySignal);
    
    // Run steps with neuromodulation
    for (int step = 0; step < 50; ++step) {
        brain.step(step, step * 0.001);
        
        // Apply random neuromodulation signals
        float dopamineLevel = curiosity->getDriveLevel();
        if (dopamineLevel > 0.0f) {
            Neuromodulator signal(dopamineLevel, NeuromodulatorType::Reward);
            brain.applyNeuromodulation(signal);
        }
    }
    
    std::cout << "    ✓ Neuromodulation integration test passed" << std::endl;
}

// Test  Redmond's
// Test 3: Memory systems integration
void testMemorySystemsIntegration() {
    std::cout << "=== Test 3: Memory Systems Integration ===" << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(150), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
    
    Brain brain(config);
    assert(brain.initialize());
    
    // Get memory systems
    auto* workingMemory = brain.getWorkingMemory();
    auto* episodicMemory = brain.getEpisodicMemory();
    auto* associativeMemory = brain.getAssociativeMemory();
    
    assert(workingMemory != nullptr);
    assert(episodicMemory != nullptr);
    assert(associativeMemory != nullptr);
    
    // Test working memory operations
    std::vector<float> testPattern(24, 0.0f);
    std::vector<float> testContext(8, 0.0f);
    
    // Store in working memory
    workingMemory->storePattern(testPattern, testContext);
    assert(workingMemory->getActiveTraces() > 0);
    
    // Test episodic memory operations
    brain.step(0, 0.0);
    auto body = std::make_shared<AgentBody>();
    body->x = 5.0f;
    body->y = 5.0f;
    body->orientation = 0.0f;
    
    episodicMemory->storeEpisode(1000.0f, body->x, body->y, body->orientation, ActionType::MoveForward, 1.0f, 1.0f, 0.0f);
    assert(episodicMemory->getEpisodeCount() > 0);
    
    // Test associative memory operations
    associativeMemory->storeAssociation(testPattern, testPattern, 1.0f);
    
    // Run simulation with memory systems
    for (int step = 1; step < 100; ++step) {
        brain.step(step, step * 0.001);
        
        // Continue storing patterns in working memory
        workingMemory->storePattern(testPattern, testContext);
        
        // Occasionally store episodic episodes
        if (step % 10 == 0) {
            episodicMemory->storeEpisode(step * 0.1f, body->x + step * 0.1f, body->y, body->orientation);
        }
    }
    
    // Verify memory systems are active
    assert(workingMemory->getActiveTraces() > 0);
    assert(episodicMemory->getEpisodeCount() > 0);
    
    std::cout << "    ✓ Memory systems integration test passed" << std::endl;
}

// Test 4: Prediction system integration
void testPredictionSystemIntegration() {
    std::cout << "=== Test 4: Prediction System Integration ===" << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    
    Brain brain(config);
    assert(brain.initialize());
    
    // Get prediction system
    auto* predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    // Test prediction system with sensory input
    std::vector<float> sensoryInput(16, 0.5f);
    std::vector<float> expectedOutput(16, 0.6f);
    
    predictionSystem->setExpectedOutput(expectedOutput);
    
    // Run steps with prediction
    for (int step = 0; step < 100; ++step) {
        brain.step(step, step * 0.001);
        
        // Generate sensory input for prediction
        predictionSystem->setSensoryInput(sensoryInput);
        
        // Get prediction
        std::vector<float> prediction = predictionSystem->getPrediction();
        assert(prediction.size() == expectedOutput.size());
        
        // Compute prediction error
        float error = predictionSystem->getPredictionError();
        assert(error >= 0.0f);
    }
    
    std::cout << "    ✓ Prediction system integration test passed" << std::endl;
}

// Test 5: Cognitive systems integration
void testCognitiveSystemsIntegration() {
    std::cout << "=== Test 5: Cognitive Systems Integration ===" << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(120), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
    
    Brain brain(config);
    assert(brain.initialize());
    
    // Get cognitive systems
    auto* planner = brain.getPlanner();
    auto* conceptFormation = brain.getConceptFormation();
    auto* attention = brain.getAttention();
    
    assert(planner != nullptr);
    assert(conceptFormation != nullptr);
    assert(attention != nullptr);
    
    // Test concept formation
    std::vector<float> pattern1(24, 0.0f);
    std::vector<float> pattern2(24, 0.0f);
    
    concept->presentExperience(pattern1, std::vector<float>(8, 0.0f), 1.0f, 1);
    concept->presentExperience(pattern2, std::vector<float>(8, 0.0f), 0.5f, 2);
    
    assert(conceptFormation->getConceptCount() > 0);
    
    // Test planning system
    for (int step = 0; step < 50; ++step) {
        brain.step(step, step * 0.001);
        
        // Get current state for planning
        auto* workingMemory = brain.getWorkingMemory();
        if (workingMemory) {
            // Plan actions based on current state
            auto action = planner->planAction(workingMemory);
            // action would be used in a real simulation
        }
    }
    
    std::cout << "    ✓ Cognitive systems integration test passed" << std::endl;
}

// Test 6: Development system integration
void testDevelopmentSystemIntegration() {
    std::cout << "=== Test 6: Development System Integration ===" << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(80), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    
    Brain brain(config);
    assert(brain.initialize());
    
    // Get development system
    auto* development = brain.getDevelopmentSystem();
    assert development != nullptr;
    
    // Test developmental stages
    development->setStage(DevelopmentalStage::CriticalPeriod);
    assert(brain.getDevelopmentalStage() == DevelopmentalStage::CriticalPeriod);
    
    development->setStage(DevelopmentalStage::Maturation);
    assert(brain.getDevelopmentalStage() == DevelopmentalStage::Maturation);
    
    development->setStage(DevelopmentalStage::Adult);
    assert(brain.getDevelopmentalStage() == DevelopmentalStage::Adult);
    
    // Run simulation with development
    for (int step = 0; step < 200; ++step) {
        brain.step(step, step * 0.001);
        
        // Update development
        development->update(static_cast<double>(step * 0.001));
        
        // Check developmental stage changes
        if (step % 60 == 0) {
            development->setStage(DevelopmentalStage::CriticalPeriod);
        }
    }
    
    std::cout << "    ✓ Development system integration test passed" << std::endl;
}

// Test 7: Checkpointing and replay integration
void testCheckpointingAndReplayIntegration() {
    std::cout << "=== Test 7: Checkpointing and Replay Integration ===" << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    
    // Create and initialize first brain
    Brain brain1(config);
    assert(brain1.initialize());
    
    // Run simulation
    for (int step = 0; step < 50; ++step) {
        brain1.step(step, step * 0.001);
    }
    
    // Save checkpoint
    std::string checkpointPath = "/tmp/nlm_test_checkpoint.bin";
    bool saveSuccess = brain1.save(checkpointPath);
    assert(saveSuccess);
    
    // Create second brain and load checkpoint
    Brain brain2(config);
    brain2.initialize();
    
    bool loadSuccess = brain2.load(checkpointPath);
    assert(loadSuccess);
    
    // Verify checkpoint functionality
    assert(brain2.getTotalNeuronCount() == brain1.getTotalNeuronCount());
    assert(brain2.getTotalSynapseCount() == brain1.getTotalSynapseCount());
    
    // Test replay system
    auto* episodicMemory = brain2.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    // Store some episodes for replay
    for (int step = 0; step < 100; ++step) {
        brain2.step(step, step * 0.001);
        episodicMemory->storeEpisode(step * 0.1f, 5.0f, 5.0f, 0.0f);
    }
    
    assert(episodicMemory->getEpisodeCount() > 0);
    
    std::cout << "    ✓ Checkpointing and replay integration test passed" << std::endl;
}

// Test 8: Complete Phase 6 integration test
void testPhase6Integration() {
    std::cout << "=== Test 8: Complete Phase 6 Integration ===" << std::endl;
    
    // Create configuration for Phase 6 integration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(200), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(4), ConfigSource::Default);
    config->set("connection_probability", 0.08f, ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    
    // Create brain with all subsystems
    Brain brain(config);
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    // Create agent brain interface
    SimpleWorld world;
    world.configure(20, 20, 16, 16);
    world.reset();
    
    AgentBrain agent(brain);
    agent.initialize(world);
    
    // Enable all subsystems
    agent.enableRewardModulation(true);
    agent.enableStructuralPlasticity(true);
    agent.enableDevelopment(true);
    agent.enableCuriosity(true);
    
    // Run complete Phase 6 simulation
    float totalReward = 0.0f;
    float totalFiringRate = 0.0f;
    uint64_t steps = 500;
    
    for (uint64_t step = 0; step < steps; ++step) {
        // Get observation from world
        SensoryPercept percept = world.getSensoryPercept();
        
        // Process sensory input
        agent.processSensoryInput(percept);
        
        // Brain step
        brain.step(step, step * 0.001);
        
        // Get motor command
        MotorCommand cmd = agent.decodeMotorCommand();
        
        // Apply action to world
        ActionResult actionResult = world.applyMotorCommand(cmd, step * 0.001);
        
        // Compute reward
        float reward = actionResult.reward;
        totalReward += reward;
        
        // Apply reward modulation
        agent.applyRewardModulation(reward, 0.0f);
        
        // Update development
        agent.updateDevelopment(0.001);
        
        // Collect metrics
        totalFiringRate += brain.getAverageFiringRate();
        
        // Update world
        world.update(0.1);
        
        // Print progress every 100 steps
        if (step % 100 == 0) {
            std::cout << "    Phase 6 integration: Step " << step << ", Avg Reward: " 
                      << (totalReward / (step + 1)) << ", Firing Rate: " << brain.getAverageFiringRate() << std::endl;
        }
    }
    
    // Calculate final metrics
    float avgReward = totalReward / steps;
    float avgFiringRate = totalFiringRate / steps;
    
    std::cout << "    Phase 6 integration results:" << std::endl;
    std::cout << "      Average reward: " << avgReward << std::endl;
    std::cout << "      Average firing rate: " << avgFiringRate << " Hz" << std::endl;
    std::cout << "      Total spikes: " << brain.getTotalSpikeCount() << std::endl;
    std::cout << "      Total synapses: " << brain.getTotalSynapseCount() << std::endl;
    
    // Verify integration success
    assert(avgReward > 牛奶自己的系统，再熟悉现实
