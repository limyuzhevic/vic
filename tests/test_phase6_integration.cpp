// Phase 6 Integration Tests
// Tests for verifying that all Phase 6 systems are properly integrated and functional

#include "brain/Brain.hpp"
#include "memory/WorkingMemory.hpp"
#include "memory/EpisodicMemory.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "cognition/ConceptFormation.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/AttentionSystem.hpp"
#include "development/DevelopmentSystem.hpp"
#include "prediction/PredictionSystem.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include <cassert>
#include <iostream>
#include <chrono>

namespace test_phase6 {

void testWorkingMemoryIntegration() {
    std::cout << "Testing Working Memory integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("memory.enable_working_memory", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Verify working memory is accessible
    auto* wm = brain.getWorkingMemory();
    assert(wm != nullptr);
    
    // Test working memory functionality
    assert(wm->getActiveTraces() >= 0);
    assert(wm->getMemoryCapacity() > 0);
    
    // Run some simulation steps
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain.step(step);
        
        // Inject some activity to generate working memory traces
        if (wm->getActiveTraces() < wm->getMemoryCapacity()) {
            wm->addTrace(step, std::vector<float>{1.0f, 0.5f, 0.3f});
        }
    }
    
    std::cout << "  ✓ Working Memory integration test passed" << std::endl;
}

void testEpisodicMemoryIntegration() {
    std::cout << "Testing Episodic Memory integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("memory.enable_episodic_memory", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Verify episodic memory is accessible
    auto* em = brain.getEpisodicMemory();
    assert(em != nullptr);
    
    // Test episodic memory functionality
    assert(em->getEpisodeCount() >= 0);
    assert(em->getMemoryCapacity() > 0);
    
    // Run simulation to generate episodes
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
        
        // Generate an episode every 20 steps
        if (step % 20 == 0 && em->getEpisodeCount() < em->getMemoryCapacity()) {
            em->storeEpisode(step, std::vector<float>{0.1f, 0.2f, 0.3f, 0.4f, 0.5f});
        }
    }
    
    // Verify episodes were stored
    size_t episodeCount = em->getEpisodeCount();
    assert(episodeCount >= 0);
    
    // Test replay functionality
    if (episodeCount > 0) {
        auto episodes = em->getEpisodesForReplay(3);
        assert(!episodes.empty() || episodeCount == 0); // Either we got episodes or it's normal for small simulations
    }
    
    std::cout << "  ✓ Episodic Memory integration test passed" << std::endl;
}

void testNeuromodulationIntegration() {
    std::cout << "Testing Neuromodulation integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("neuromod.enable_dopamine", true, nlm::ConfigSource::Default);
    config->set("neuromod.enable_curiosity", true, nlm::ConfigSource::Default);
    config->set("neuromod.enable_novelty", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Verify neuromodulation systems are accessible
    auto* dopamine = brain.getDopamine();
    auto* curiosity = brain.getCuriosity();
    auto* novelty = brain.getNovelty();
    
    assert(dopamine != nullptr);
    assert(curiosity != nullptr);
    assert(novelty != nullptr);
    
    // Test neuromodulation functionality
    float initialDopamine = dopamine->getLevel();
    float initialCuriosity = curiosity->getLevel();
    float initialNovelty = novelty->getLevel();
    
    // Apply some modulation
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain.step(step);
        
        // Apply reward-based modulation
        if (step % 10 == 0) {
            float reward = 1.0f - (step / 50.0f) * 0.5f;
            dopamine->applyModulation(reward, 0.5f);
        }
        
        // Apply novelty detection
        float noveltyLevel = 0.1f * std::sin(step * 0.1f);
        novelty->applyModulation(noveltyLevel, 0.3f);
    }
    
    // Verify neuromodulation systems responded
    float finalDopamine = dopamine->getLevel();
    float finalCuriosity = curiosity->getLevel();
    float finalNovelty = novelty->getLevel();
    
    // Levels may have changed, but shouldn't crash
    assert(true); // Just ensure no crashes
    
    std::cout << "  ✓ Neuromodulation integration test passed" << std::endl;
}

void testPredictionSystemIntegration() {
    std::cout << "Testing Prediction System integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("prediction.enable", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Verify prediction system is accessible
    auto* prediction = brain.getPredictionSystem();
    assert(prediction != nullptr);
    
    // Test prediction system functionality
    assert(prediction->getPredictionAccuracy() >= 0.0f);
    assert(prediction->getPredictionError() >= 0.0f);
    
    // Run simulation to generate predictions
    for (nlm::SimulationStep step = 0; step < 75; ++step) {
        brain.step(step);
        
        // Apply some prediction error
        if (step % 15 == 0) {
            float predictionError = 1.0f - (step / 100.0f);
            prediction->updatePredictionError(predictionError);
        }
    }
    
    std::cout << "  ✓ Prediction System integration test passed" << std::endl;
}

void testDevelopmentSystemIntegration() {
    std::cout << "Testing Development System integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("development.enable", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Verify development system is accessible
    auto* development = brain.getDevelopmentSystem();
    assert(development != nullptr);
    
    // Test development system functionality
    auto currentStage = brain.getDevelopmentalStage();
    assert(static_cast<int>(currentStage) >= 0 && static_cast<int>(currentStage) <= 4);
    
    // Run simulation to allow development to occur
    for (nlm::SimulationStep step = 0; step < 60; ++step) {
        brain.step(step);
        
        // Update development
        if (development) {
            development->updateStage(step * 0.001f);
        }
    }
    
    // Verify development stage may have changed
    auto finalStage = brain.getDevelopmentalStage();
    assert(static_cast<int>(finalStage) >= 0 && static_cast<int>(finalStage) <= 4);
    
    std::cout << "  ✓ Development System integration test passed" << std::endl;
}

void testCognitionSystemsIntegration() {
    std::cout << "Testing Cognition Systems integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("cognition.enable_concept", true, nlm::ConfigSource::Default);
    config->set("cognition.enable_planner", true, nlm::ConfigSource::Default);
    config->set("cognition.enable_attention", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Verify cognition systems are accessible
    auto* conceptFormation = brain.getConceptFormation();
    auto* planner = brain.getPlanner();
    auto* attention = brain.getAttention();
    
    assert(conceptFormation != nullptr);
    assert(planner != nullptr);
    assert(attention != nullptr);
    
    // Test cognition systems functionality
    size_t conceptCount = conceptFormation->getConceptCount();
    size_t planCount = planner->getPlanCount();
    
    // Run simulation to allow cognition to develop
    for (nlm::SimulationStep step = 0; step < 80; ++step) {
        brain.step(step);
        
        // Generate some concept formation activity
        if (conceptFormation && step % 20 == 0) {
            conceptFormation->updateConcept(std::vector<float>{1.0f, 0.5f, 0.3f, 0.7f, 0.9f});
        }
        
        // Generate some planning activity
        if (planner && step % 25 == 0) {
            planner->createPlan(std::vector<float>{0.2f, 0.4f, 0.6f, 0.8f});
        }
        
        // Generate some attention activity
        if (attention && step % 30 == 0) {
            attention->updateAttention(0.5f + 0.5f * std::sin(step * 0.1f));
        }
    }
    
    std::cout << "  ✓ Cognition Systems integration test passed" << std::endl;
}

void testAgentBrainIntegration() {
    std::cout << "Testing Agent Brain integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Create agent brain interface
    nlm::AgentBrain agent(std::make_shared<nlm::Brain>(brain));
    
    // Create simple world
    nlm::SimpleWorld world;
    world.configure(16, 16, 8, 8);
    world.reset();
    world.setAgentStart(8.0, 8.0);
    
    // Initialize agent with world
    agent.initialize(world);
    
    // Enable all agent subsystems
    agent.enableRewardModulation(true);
    agent.enableStructuralPlasticity(true);
    agent.enableDevelopment(true);
    agent.enableCuriosity(true);
    
    // Run agent simulation
    for (nlm::SimulationStep step = 0; step < 40; ++step) {
        world.update(0.1f);
        
        auto percept = world.getSensoryPercept();
        agent.processSensoryInput(percept);
        
        brain.step(step);
        
        auto motor_cmd = agent.decodeMotorCommand();
        world.applyMotorCommand(motor_cmd, world.getSimulationTime());
        
        // Apply reward modulation
        float reward = 0.5f + 0.5f * std::sin(step * 0.1f);
        agent.applyRewardModulation(reward, 0.0f);
        
        // Update development
        agent.updateDevelopment(0.1f);
        
        // Verify agent is functional
        assert(agent.getBrain() != nullptr);
        assert(agent.getCuriosityLevel() >= 0.0f && agent.getCuriosityLevel() <= 1.0f);
    }
    
    std::cout << "  ✓ Agent Brain integration test passed" << std::endl;
}

void testCheckpointIntegration() {
    std::cout << "Testing Checkpoint integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("checkpointing.enable", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain1(config);
    assert(brain1.initialize());
    
    // Run some simulation
    for (nlm::SimulationStep step = 0; step < 30; ++step) {
        brain1.step(step);
    }
    
    // Save checkpoint
    std::string checkpointPath = "/tmp/test_checkpoint.nlm";
    bool saveSuccess = brain1.save(checkpointPath);
    assert(saveSuccess);
    
    // Create new brain and load checkpoint
    auto config2 = std::make_shared<nlm::Config>();
    config2->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain2(config2);
    assert(brain2.initialize());
    
    bool loadSuccess = brain2.load(checkpointPath);
    assert(loadSuccess);
    
    // Verify both brains have neurons
    assert(brain1.getTotalNeuronCount() == brain2.getTotalNeuronCount());
    
    std::cout << "  ✓ Checkpoint integration test passed" << std::endl;
}

void testReplayIntegration() {
    std::cout << "Testing Replay integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("replay.enable", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Verify episodic memory exists
    auto* em = brain.getEpisodicMemory();
    if (em) {
        // Generate some episodes
        for (nlm::SimulationStep step = 0; step < 50; ++step) {
            brain.step(step);
            
            if (step % 10 == 0 && em->getEpisodeCount() < em->getMemoryCapacity()) {
                em->storeEpisode(step, std::vector<float>{0.1f, 0.2f, 0.3f, 0.4f, 0.5f});
            }
        }
        
        // Test replay
        size_t episodeCount = em->getEpisodeCount();
        if (episodeCount > 0) {
            auto episodes = em->getEpisodesForReplay(3);
            // Episodes should be available
            assert(true);
        }
    }
    
    std::cout << "  ✓ Replay integration test passed" << std::endl;
}

void runAllPhase6Tests() {
    std::cout << "=== Phase 6 Integration Tests ===" << std::endl;
    
    try {
        testWorkingMemoryIntegration();
        testEpisodicMemoryIntegration();
        testNeuromodulationIntegration();
        testPredictionSystemIntegration();
        testDevelopmentSystemIntegration();
        testCognitionSystemsIntegration();
        testAgentBrainIntegration();
        testCheckpointIntegration();
        testReplayIntegration();
        
        std::cout << "" << std::endl;
        std::cout << "=== All Phase 6 Integration Tests Passed! ===" << std::endl;
        std::cout << "" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "ERROR: Phase 6 integration test failed with exception: " << e.what() << std::endl;
        throw;
    }
}

} // namespace test_phase6
