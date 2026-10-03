// Integration Tests
// This file tests the system-wide integration and end-to-end simulation of the NLM framework
// Covers brain system integration, environment interaction, and complete simulation validation

#include "brain/Brain.hpp"
#include "environment/SimpleWorld/SimpleWorld.hpp"
#include "environment/World/World.hpp"
#include "agent/AgentBrain/AgentBrain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>

namespace test_integration {

void testBrainSystemIntegration() {
    auto config = std::make_shared<nlm::Config>();
    
    config->set("brain.neuron_count", 100, nlm::ConfigSource::Default);
    config->set("brain.region_count", 2, nlm::ConfigSource::Default);
    config->set("brain.connection_probability", 0.15, nlm::ConfigSource::Default);
    config->set("working_memory.capacity", 50, nlm::ConfigSource::Default);
    config->set("episodic_memory.max_episodes", 20, nlm::ConfigSource::Default);
    config->set("cognition.planner.action_count", 5, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test that all brain systems are properly integrated
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getRegionCount() == 2);
    assert(brain.getTotalSynapseCount() > 0);
    
    // Test memory systems integration
    assert(brain.getWorkingMemory() != nullptr);
    assert(brain.getEpisodicMemory() != nullptr);
    assert(brain.getAssociativeMemory() != nullptr);
    
    // Test cognitive systems integration
    assert(brain.getNeuralPlanner() != nullptr);
    assert(brain.getConceptFormation() != nullptr);
    assert(brain.getAttentionalSelection() != nullptr);
    
    // Test neuromodulation systems integration
    assert(brain.getDopamineSystem() != nullptr);
    assert(brain.getCuriositySystem() != nullptr);
    assert(brain.getNoveltySystem() != nullptr);
    assert(brain.getPredictionErrorSystem() != nullptr);
    
    std::cout << "    testBrainSystemIntegration passed" << std::endl;
}

void testEnvironmentInteraction() {
    auto config = std::make_shared<nlm::Config>();
    
    config->set("environment.world.width", 20, nlm::ConfigSource::Default);
    config->set("environment.world.height", 20, nlm::ConfigSource::Default);
    config->set("environment.world.vision_width", 8, nlm::ConfigSource::Default);
    config->set("environment.world.vision_height", 8, nlm::ConfigSource::Default);
    
    nlm::SimpleWorld world;
    world.configure(20, 20, 8, 8);
    
    // Test world initialization
    assert(world.getWidth() == 20);
    assert(world.getHeight() == 20);
    assert(world.getVisionWidth() == 8);
    assert(world.getVisionHeight() == 8);
    
    // Test world state updates
    world.reset();
    assert(world.getStep() == 0);
    
    // Update world state
    world.update(0.1);
    assert(world.getStep() > 0);
    
    // Test sensory percept
    auto percept = world.getSensoryPercept();
    assert(percept.size() > 0);
    
    // Test world agent interaction
    auto config = std::make_shared<nlm::Config>();
    config->set("brain.neuron_count", 50, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::AgentBrain agent(brain);
    agent.initialize(world);
    
    // Test agent processing
    agent.processSensoryInput(percept);
    nlm::Action action = agent.decodeMotorCommand();
    assert(action.type != nlm::ActionType::Invalid);
    
    // Test world response to action
    world.applyMotorCommand(action, world.getSimulationTime());
    
    std::cout << "    testEnvironmentInteraction passed" << std::endl;
}

void testEndToEndSimulation() {
    auto config = std::make_shared<nlm::Config>();
    
    config->set("brain.neuron_count", 200, nlm::ConfigSource::Default);
    config->set("brain.region_count", 1, nlm::ConfigSource::Default);
    config->set("working_memory.capacity", 100, nlm::ConfigSource::Default);
    config->set("environment.world.width", 10, nlm::ConfigSource::Default);
    config->set("environment.world.height", 10, nlm::ConfigSource::Default);
    config->set("environment.world.vision_width", 5, nlm::ConfigSource::Default);
    config->set("environment.world.vision_height", 5, nlm::ConfigSource::Default);
    config->set("cognition.planner.action_count", 3, nlm::ConfigSource::Default);
    
    // Create brain and world
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::SimpleWorld world;
    world.configure(10, 10, 5, 5);
    
    // Create and initialize agent
    nlm::AgentBrain agent(brain);
    agent.initialize(world);
    
    // Enable all neuromodulation systems
    agent.enableRewardModulation(true);
    agent.enableCuriosity(true);
    agent.enableNoveltyDetection(true);
    agent.enableStructuralPlasticity(true);
    agent.enableDevelopment(true);
    
    // Run complete simulation
    for (int step = 0; step < 50; ++step) {
        // Update world
        world.update(0.1);
        
        // Get sensory input
        auto percept = world.getSensoryPercept();
        
        // Process sensory input
        agent.processSensoryInput(percept);
        
        // Brain step
        brain.step(step);
        
        // Get action from agent
        nlm::Action action = agent.decodeMotorCommand();
        
        // Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime());
        
        // Verify all systems are still functional
        assert(brain.getWorkingMemory() != nullptr);
        assert(agent.getCuriositySystem() != nullptr);
        assert(agent.getDopamineSystem() != nullptr);
    }
    
    // Verify final state
    assert(world.getStep() == 50);
    assert(brain.getTotalNeuronCount() == 200);
    
    std::cout << "    testEndToEndSimulation passed" << std::endl;
}

void testSystemMemoryManagement() {
    auto config = std::make_shared<nlm::Config>();
    
    config->set("brain.neuron_count", 100, nlm::ConfigSource::Default);
    config->set("working_memory.capacity", 30, nlm::ConfigSource::Default);
    config->set("episodic_memory.max_episodes", 10, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto workingMemory = brain.getWorkingMemory();
    auto episodicMemory = brain.getEpisodicMemory();
    
    // Store many patterns to test memory management
    for (int i = 0; i < 50; ++i) {
        std::vector<double> pattern{i, i * 2, i * 3, i * 4};
        workingMemory->storePattern(pattern);
    }
    
    // Store many episodes
    for (int i = 0; i < 15; ++i) {
        std::vector<double> neuralActivity(10, 0.5 + (i % 3) * 0.1);
        episodicMemory->storeEpisode(neuralActivity, 1000.0 + i * 100.0);
    }
    
    // Verify memory systems are still functional
    assert(workingMemory->getActiveTraces() <= 30); // Capacity limit enforced
    assert(episodicMemory->getRecentEpisodes(5).size() == 5); // Recent episodes accessible
    
    // Test memory cleanup
    brain.getWorkingMemory()->clear();
    brain.getEpisodicMemory()->clear();
    
    assert(brain.getWorkingMemory()->getActiveTraces() == 0);
    assert(brain.getEpisodicMemory()->getRecentEpisodes(5).size() == 0);
    
    std::cout << "    testSystemMemoryManagement passed" << std::endl;
}

void testRealTimeSystemConsistency() {
    auto config = std::make_shared<nlm::Config>();
    
    config->set("brain.neuron_count", 150, nlm::ConfigSource::Default);
    config->set("neuromodulation.curiosity.exploration_rate", 0.2, nlm::ConfigSource::Default);
    config->set("cognition.attention.focus_strength", 0.6, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::SimpleWorld world;
    world.configure(15, 15, 7, 7);
    
    nlm::AgentBrain agent(brain);
    agent.initialize(world);
    
    // Test system consistency over time
    for (int step = 0; step < 20; ++step) {
        world.update(0.05);
        
        auto percept = world.getSensoryPercept();
        agent.processSensoryInput(percept);
        brain.step(step);
        
        nlm::Action action = agent.decodeMotorCommand();
        world.applyMotorCommand(action, world.getSimulationTime());
        
        // Verify all systems are still in valid states
        assert(brain.getTotalNeuronCount() == 150);
        assert(brain.getWorkingMemory() != nullptr);
        assert(agent.getCuriositySystem() != nullptr);
        assert(agent.getDopamineSystem() != nullptr);
        assert(agent.getNoveltySystem() != nullptr);
        
        // Check that neuromodulation levels are reasonable
        double curiosityLevel = agent.getCuriositySystem()->getCurrentLevel();
        assert(curiosityLevel >= 0.0 && curiosityLevel <= 1.0);
        
        double dopamineLevel = agent.getDopamineSystem()->getCurrentLevel();
        assert(dopamineLevel >= 0.0 && dopamineLevel <= 1.0);
    }
    
    std::cout << "    testRealTimeSystemConsistency passed" << std::endl;
}

void runAll() {
    testBrainSystemIntegration();
    testEnvironmentInteraction();
    testEndToEndSimulation();
    testSystemMemoryManagement();
    testRealTimeSystemConsistency();
    
    std::cout << "All integration tests passed!" << std::endl;
}

} // namespace test_integration
