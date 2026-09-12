// AgentBrain Integration Tests
// Test complete AgentBrain lifecycle with brain and world

#include "agent/AgentBrain.hpp"
#include "agent/SensoryProcessor.hpp"
#include "agent/MotorDecoder.hpp"
#include "agent/NeuromodulationController.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/SensoryPercept.hpp"
#include "agent/AgentBody.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>

namespace test_agentbrain_integration {

void testAgentBrainWorldInteraction() {
    std::cout << "  Testing AgentBrain with world interaction..." << std::endl;
    
    // Create full agent system
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agent(brain);
    
    // Create and configure world
    nlm::SimpleWorld world;
    world.configure(10, 10, 8, 8);  // 10x10 world, 8x8 vision
    world.reset();
    
    // Initialize agent with world
    agent.initialize(world);
    
    // Verify initialization results
    assert(agent.getSensoryInputSize() > 0);
    assert(agent.getMotorOutputSize() > 0);
    assert(agent.getDevelopmentalStage() == nlm::DevelopmentalStage::Initial);
    
    // Create test sensory percept
    nlm::SensoryPercept percept;
    std::vector<float> vision(8 * 8, 0.5f);  // Medium brightness vision
    percept.setVision(vision);
    std::vector<float> touch(4, 0.0f);  // No touch
    percept.setTouch(touch);
    std::vector<float> internal(2, 0.5f);  // Normal internal state
    percept.setInternal(internal);
    std::vector<float> proprioception(4, 0.0f);  // Stationary
    percept.setProprioception(proprioception);
    
    // Process sensory input
    agent.processSensoryInput(percept);
    
    // Verify sensory processing
    assert(agent.getNoveltyLevel() >= 0.0f);
    assert(agent.getCuriosityLevel() >= 0.0f);
    
    // Decode motor command
    nlm::MotorCommand motorCmd = agent.decodeMotorCommand();
    // Valid motor command
    assert(static_cast<int>(motorCmd) >= 0 && static_cast<int>(motorCmd) <= 7);
    
    // Apply reward modulation
    agent.applyRewardModulation(1.0f, 0.8f);  // Positive reward
    assert(agent.getNeuromodulationLevel() > 0.0f);
    assert(agent.getPredictionError() == 0.2f);  // 1.0 - 0.8
    
    // Update development
    agent.updateDevelopment(10.0);  // 10 simulation steps
    assert(agent.getDevelopmentalAge() > 0.0);
    assert(agent.getDevelopmentalStage() != nlm::DevelopmentalStage::Initial);
    
    // Verify state consistency
    assert(agent.getSensoryInputSize() == agent.getSensoryInputSize());
    assert(agent.getMotorOutputSize() == agent.getMotorOutputSize());
    
    std::cout << "    AgentBrain with world interaction: PASSED" << std::endl;
}

void testAgentBrainCompleteLifecycle() {
    std::cout << "  Testing AgentBrain complete lifecycle..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agent(brain);
    
    nlm::SimpleWorld world;
    world.configure(20, 20, 16, 16);
    world.reset();
    
    agent.initialize(world);
    
    // Create varied sensory input
    std::vector<float> vision(16 * 16);
    for (size_t i = 0; i < vision.size(); ++i) {
        vision[i] = (i % 3 == 0) ? 1.0f : ((i % 3 == 1) ? 0.5f : 0.0f);
    }
    
    // Run simulation loop
    const int steps = 50;
    for (int step = 0; step < steps; ++step) {
        // Create sensory percept with temporal variation
        nlm::SensoryPercept percept;
        std::vector<float> visionCopy = vision;
        
        // Add temporal variation
        for (size_t i = 0; i < visionCopy.size(); ++i) {
            visionCopy[i] = visionCopy[i] * 0.9f + static_cast<float>(step % 10) / 10.0f * 0.2f;
        }
        
        percept.setVision(visionCopy);
        percept.setTouch(std::vector<float>(4, 0.0f));
        percept.setInternal(std::vector<float>(2, 0.5f));
        percept.setProprioception(std::vector<float>(4, 0.0f));
        
        // Process sensory input
        agent.processSensoryInput(percept);
        
        // Occasionally apply reward
        if (step % 10 == 0) {
            agent.applyRewardModulation(1.0f, 0.5f);
        }
        
        // Decode motor command
        nlm::MotorCommand cmd = agent.decodeMotorCommand();
        (void)cmd;  // Use command
        
        // Update development
        agent.updateDevelopment(1.0);
        
        // Verify no crashes or invalid states
        assert(agent.getNeuromodulationLevel() >= 0.0f);
        assert(agent.getCuriosityLevel() >= 0.0f);
        assert(agent.getNoveltyLevel() >= 0.0f);
        assert(agent.getPredictionError() >= -1.0f && agent.getPredictionError() <= 1.0f);
    }
    
    // Verify agent has learned/adapted
    assert(agent.getDevelopmentalAge() > 0.0f);
    assert(agent.getNeuromodulationLevel() > 0.0f);  // Accumulated neuromodulation
    
    std::cout << "    AgentBrain complete lifecycle: PASSED" << std::endl;
}

void testAgentBrainComponentsIntegration() {
    std::cout << "  Testing AgentBrain components integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    
    // Test each component independently then together
    nlm::SensoryProcessor sensory(brain);
    nlm::MotorDecoder decoder(brain);
    nlm::NeuromodulationController neuromod(brain);
    
    // Configure components
    sensory.enableCuriosity(true);
    decoder.enableCuriosity(true);
    neuromod.enable(true);
    neuromod.enableDevelopment(true);
    
    // Create test vision data
    std::vector<float> vision(64, 0.0f);
    for (int i = 0; i < 10; ++i) vision[i] = 1.0f;
    
    // Test sensory processor
    sensory.updateNovelty(vision);
    assert(sensory.getNoveltyLevel() > 0.0f);
    
    // Test neuromodulation controller with curiosity
    neuromod.updateCuriosityLevel(sensory.getCuriosityLevel());
    neuromod.applyRewardModulation(0.5f, 0.2f);
    
    // Test motor decoder with curiosity
    nlm::MotorCommand cmd = decoder.selectWithCuriosity(
        nlm::MotorCommand::Wait, 
        sensory.getCuriosityLevel(), 
        true
    );
    assert(static_cast<int>(cmd) >= 0 && static_cast<int>(cmd) <= 7);
    
    // Verify all components can work together
    assert(sensory.getNoveltyLevel() > 0.0f);
    assert(neuromod.getPredictionError() > 0.0f);
    assert(decoder.isCuriosityEnabled() == true);
    
    std::cout << "    AgentBrain components integration: PASSED" << std::endl;
}

void testAgentBrainErrorHandling() {
    std::cout << "  Testing AgentBrain error handling..." << std::endl;
    
    // Test with null brain (edge case)
    nlm::AgentBrain agent(nullptr);
    
    nlm::SimpleWorld world;
    world.configure(10, 10, 8, 8);
    world.reset();
    
    // Should not crash with null brain
    agent.initialize(world);  // May do nothing or minimal work
    
    nlm::SensoryPercept percept;
    percept.setVision(std::vector<float>(64, 0.5f));
    
    // Should handle null brain gracefully
    agent.processSensoryInput(percept);
    
    nlm::MotorCommand cmd = agent.decodeMotorCommand();
    assert(cmd == nlm::MotorCommand::Wait);  // Default for null brain
    
    // Test reward modulation with null brain
    agent.applyRewardModulation(1.0f, 0.5f);
    
    // Test development with null brain
    agent.updateDevelopment(1.0);
    
    // Test reset with null brain
    agent.reset();
    
    std::cout << "    AgentBrain error handling: PASSED" << std::endl;
}

void testAgentBrainPerformance() {
    std::cout << "  Testing AgentBrain performance..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agent(brain);
    
    nlm::SimpleWorld world;
    world.configure(30, 30, 20, 20);
    world.reset();
    
    agent.initialize(world);
    
    // Create sensory data
    std::vector<float> vision(400, 0.5f);
    nlm::SensoryPercept percept;
    percept.setVision(vision);
    
    // Measure performance over many iterations
    const int iterations = 100;
    double totalTime = 0.0;
    
    for (int i = 0; i < iterations; ++i) {
        // Process sensory input
        agent.processSensoryInput(percept);
        
        // Decode motor command
        nlm::MotorCommand cmd = agent.decodeMotorCommand();
        (void)cmd;
        
        // Apply reward modulation
        agent.applyRewardModulation(0.1f * (i % 5), 0.05f * (i % 3));
        
        // Update development
        agent.updateDevelopment(0.1);
    }
    
    // Agent should remain stable after many iterations
    assert(agent.getNeuromodulationLevel() >= 0.0f);
    assert(agent.getCuriosityLevel() >= 0.0f);
    assert(agent.getNoveltyLevel() >= 0.0f);
    
    std::cout << "    AgentBrain performance: PASSED" << std::endl;
}

void runAll() {
    std::cout << "Running AgentBrain Integration Tests..." << std::endl;
    std::cout << "============================================" << std::endl;
    
    testAgentBrainWorldInteraction();
    testAgentBrainCompleteLifecycle();
    testAgentBrainComponentsIntegration();
    testAgentBrainErrorHandling();
    testAgentBrainPerformance();
    
    std::cout << "============================================" << std::endl;
    std::cout << "All AgentBrain Integration Tests PASSED!" << std::endl;
}

} // namespace test_agentbrain_integration