// End-to-End Brain Loop Integration Tests
// Tests for Phase 6 end-to-end brain integration

#include "brain/Brain.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_e2e_integration {

void testBasicBrainLoop() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    config->set("connection_probability", 0.05f);
    
    // Create brain
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Create simple world
    SimpleWorld world;
    world.initialize(16, 16);
    
    // Create agent
    AgentBrain agent(brain);
    agent.initialize(world);
    agent.enableRewardModulation(true);
    
    // Run basic brain loop
    float totalReward = 0.0f;
    float totalFiringRate = 0.0f;
    
    for (int step = 0; step < 100; ++step) {
        // Get observation from world
        SensoryPercept percept = world.observe(agent.getBrain()->getRegions()[0].get());
        
        // Process sensory input
        agent.processSensoryInput(percept);
        
        // Brain step
        brain.step(step, step * 0.001f);
        
        // Get motor command
        MotorCommand cmd = agent.decodeMotorCommand();
        
        // Apply action to world
        world.applyAction(agent.getBrain()->getRegions()[0].get(), cmd);
        
        // Compute reward
        float reward = world.computeReward(agent.getBrain()->getRegions()[0].get());
        totalReward += reward;
        
        // Apply reward modulation
        agent.applyRewardModulation(reward, 0.0f);
        
        // Collect metrics
        totalFiringRate += brain.getAverageFiringRate();
    }
    
    // Verify basic loop worked
    assert(totalReward >= 0.0f);  // Should not have negative total reward
    assert(totalFiringRate >= 0.0f);  // Firing rate should be non-negative
    assert(brain.getTotalNeuronCount() > 0);  // Should have neurons
    
    std::cout << "    testBasicBrainLoop passed (reward: " << totalReward << ")" << std::endl;
}

void testFullIntegrationWithDevelopment() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 150);
    config->set("region_count", 2);
    config->set("synaptogenesis_rate", 0.0001f);
    config->set("pruning_rate", 0.00001f);
    
    // Create brain
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Create world
    SimpleWorld world;
    world.initialize(32, 32);
    
    // Create agent with development enabled
    AgentBrain agent(brain);
    agent.initialize(world);
    agent.enableRewardModulation(true);
    agent.enableStructuralPlasticity(true);
    agent.enableDevelopment(true);
    agent.enableCuriosity(true);
    
    // Run longer simulation with development
    float avgReward = 0.0f;
    size_t activeRegionCount = 0;
    
    for (int step = 0; step < 200; ++step) {
        // Get observation
        SensoryPercept percept = world.observe(agent.getBrain()->getRegions()[0].get());
        
        // Process input
        agent.processSensoryInput(percept);
        
        // Brain step
        brain.step(step, step * 0.001f);
        
        // Motor command
        MotorCommand cmd = agent.decodeMotorCommand();
        world.applyAction(agent.getBrain()->getRegions()[0].get(), cmd);
        
        // Reward
        float reward = world.computeReward(agent.getBrain()->getRegions()[0].get());
        avgReward += reward;
        agent.applyRewardModulation(reward, 0.0f);
        
        // Development update
        agent.updateDevelopment(0.001f);
        
        // Collect metrics
        if (brain.getFiringNeuronCount() > 0) activeRegionCount++;
    }
    
    avgReward /= 200;
    
    // Verify integration with development
    assert(avgReward >= 0.0f);
    assert(activeRegionCount > 0);
    
    // Check development effects
    auto* devSys = brain.getDevelopmentSystem();
    if (devSys) {
        assert(devSys->getStage() != nlm::DevelopmentalStage::NotSet);
        assert(devSys->getPlasticityModifier() >= 0.0f);
    }
    
    std::cout << "    testFullIntegrationWithDevelopment passed" << std::endl;
}

void testNeuromodulationIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 120);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    SimpleWorld world;
    world.initialize(16, 16);
    
    AgentBrain agent(brain);
    agent.initialize(world);
    agent.enableRewardModulation(true);
    agent.enableCuriosity(true);
    
    // Run with varying reward to trigger neuromodulation
    for (int step = 0; step < 100; ++step) {
        SensoryPercept percept = world.observe(agent.getBrain()->getRegions()[0].get());
        agent.processSensoryInput(percept);
        
        brain.step(step, step * 0.001f);
        
        MotorCommand cmd = agent.decodeMotorCommand();
        world.applyAction(agent.getBrain()->getRegions()[0].get(), cmd);
        
        // Variable reward to trigger neuromodulation
        float variableReward = 0.5f + (step % 5) * 0.2f;
        float actualReward = world.computeReward(agent.getBrain()->getRegions()[0].get());
        
        agent.applyRewardModulation(actualReward, variableReward);
    }
    
    // Check that neuromodulation is integrated
    assert(brain.getDopamine() != nullptr);
    assert(brain.getCuriosity() != nullptr);
    
    std::cout << "    testNeuromodulationIntegration passed" << std::endl;
}

void testMemoryIntegrationInLoop() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 200);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    SimpleWorld world;
    world.initialize(16, 16);
    
    AgentBrain agent(brain);
    agent.initialize(world);
    
    // Check memory systems are present
    assert(brain.getWorkingMemory() != nullptr);
    assert(brain.getEpisodicMemory() != nullptr);
    
    // Run simulation with memory
    for (int step = 0; step < 150; ++step) {
        // Inject some activity to form memories
        brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 2.0f);
        
        SensoryPercept percept = world.observe(agent.getBrain()->getRegions()[0].get());
        agent.processSensoryInput(percept);
        
        brain.step(step, step * 0.001f);
        
        MotorCommand cmd = agent.decodeMotorCommand();
        world.applyAction(agent.getBrain()->getRegions()[0].get(), cmd);
        
        float reward = world.computeReward(agent.getBrain()->getRegions()[0].get());
        agent.applyRewardModulation(reward, 0.0f);
        
        // Periodically check memory systems
        if (step % 50 == 0) {
            auto* wm = brain.getWorkingMemory();
            if (wm) {
                assert(wm->getMemoryActivity() >= 0.0f);
            }
        }
    }
    
    std::cout << "    testMemoryIntegrationInLoop passed" << std::endl;
}

void runAll() {
    std::cout << "Running End-to-End Integration Tests..." << std::endl;
    testBasicBrainLoop();
    testFullIntegrationWithDevelopment();
    testNeuromodulationIntegration();
    testMemoryIntegrationInLoop();
}

} // namespace test_e2e_integration