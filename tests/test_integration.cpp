// Integration Tests - Complete system workflow and component interactions

#include "agent/AgentBrain.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "world/SimpleWorld.hpp"
#include "sensory/SensoryPercept.hpp"
#include "motor/Action.hpp"
#include <cassert>
#include <iostream>
#include <memory>
#include <chrono>

namespace test_integration {

void testCompleteSimulationWorkflow() {
    std::cout << "Testing complete simulation workflow..." << std::endl;
    
    // Create simulation components
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(4), nlm::ConfigSource::Default);
    
    // Create brain, world, and agent
    auto brain = std::make_shared<nlm::Brain>(config);
    auto world = std::make_shared<nlm::SimpleWorld>();
    auto agentBrain = std::make_shared<nlm::AgentBrain>(brain);
    
    // Initialize components
    brain->initialize();
    world->configure(20, 20, 8, 8);
    agentBrain->initialize(*world);
    
    // Run simulation for multiple steps
    for (int step = 0; step < 50; ++step) {
        // Update world
        world->update(step * 0.1f);
        
        // Get sensory perception
        auto percept = world->getSensoryPercept();
        
        // Process through agent brain
        agentBrain->processSensoryInput(*percept);
        
        // Step the brain
        brain->step(step, step * 0.1f);
        
        // Generate action
        auto action = agentBrain->decodeMotorCommand();
        
        // Apply action to world
        world->applyMotorCommand(action, step * 0.1f);
        
        // Check for early termination
        if (brain->getTotalSpikeCount() > 10000) {
            break;
        }
    }
    
    // Verify system is still functional
    assert(brain->getTotalNeuronCount() == 500);
    assert(brain->getRegionCount() == 4);
    assert(agentBrain->getSensoryInputSize() > 0);
    assert(agentBrain->getMotorOutputSize() > 0);
    
    std::cout << "    Complete simulation workflow test passed" << std::endl;
}

void testComponentInteraction() {
    std::cout << "Testing component interactions..." << std::endl;
    
    // Create configuration with multiple components
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    config->set("vision_size", static_cast<int64_t>(64), nlm::ConfigSource::Default);
    config->set("touch_size", static_cast<int64_t>(8), nlm::ConfigSource::Default);
    
    // Create and initialize brain
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Create and initialize agent brain
    auto agentBrain = std::make_shared<nlm::AgentBrain>(brain);
    
    // Create world
    auto world = std::make_shared<nlm::SimpleWorld>();
    world->configure(10, 10, 4, 4);
    agentBrain->initialize(*world);
    
    // Create sensory percept with multiple input types
    nlm::SensoryPercept percept;
    
    // Add vision data
    std::vector<float> visionData(64, 0.5f);
    percept.setVision(visionData);
    
    // Add touch data
    std::vector<float> touchData(8, 0.2f);
    percept.setTouch(touchData);
    
    // Add internal signals
    std::vector<float> internalData(4, 0.1f);
    percept.setInternal(internalData);
    
    // Add proprioception
    std::vector<float> proprioceptionData(6, 0.3f);
    percept.setProprioception(proprioceptionData);
    
    // Process the percept
    agentBrain->processSensoryInput(percept);
    
    // Decode motor command
    auto action = agentBrain->decodeMotorCommand();
    assert(action != nullptr);
    
    // Verify sizes match configuration
    assert(agentBrain->getSensoryInputSize() == 82); // 64 + 8 + 4 + 6
    assert(agentBrain->getMotorOutputSize() == 6);
    
    std::cout << "    Component interaction test passed" << std::endl;
}

void testEndToEndScenario() {
    std::cout << "Testing end-to-end scenario..." << std::endl;
    
    // Create simulation with various components
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("learning_rate", 0.01f, nlm::ConfigSource::Default);
    
    // Create main components
    auto brain = std::make_shared<nlm::Brain>(config);
    auto world = std::make_shared<nlm::SimpleWorld>();
    auto agentBrain = std::make_shared<nlm::AgentBrain>(brain);
    
    // Configure world
    world->configure(15, 15, 6, 6);
    world->setAgentStart(7, 7);
    
    // Initialize everything
    brain->initialize();
    agentBrain->initialize(*world);
    
    // Run simulation episodes
    for (int episode = 0; episode < 3; ++episode) {
        std::cout << "    Running episode " << episode + 1 << "..." << std::endl;
        
        // Reset for new episode
        agentBrain->reset();
        brain->reset();
        
        // Run episode steps
        for (int step = 0; step < 20; ++step) {
            // Update world
            world->update(step * 0.05f);
            
            // Get sensory input
            auto percept = world->getSensoryPercept();
            
            // Process through agent
            agentBrain->processSensoryInput(*percept);
            
            // Step brain
            brain->step(step, step * 0.05f);
            
            // Generate and apply action
            auto action = agentBrain->decodeMotorCommand();
            world->applyMotorCommand(action, step * 0.05f);
            
            // Apply reward modulation
            float reward = 0.0f;
            if (step % 5 == 0) reward = 1.0f; // Small reward every 5 steps
            agentBrain->applyRewardModulation(reward, 0.0f);
            
            // Update development
            agentBrain->updateDevelopment(step * 0.05f);
        }
        
        // Verify episode completed successfully
        assert(brain->getTotalNeuronCount() == 300);
        assert(agentBrain->getNeuromodulationLevel() >= -1.0f && agentBrain->getNeuromodulationLevel() <= 1.0f);
    }
    
    std::cout << "    End-to-end scenario test passed" << std::endl;
}

void testErrorHandlingInComplexScenarios() {
    std::cout << "Testing error handling in complex scenarios..." << std::endl;
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    // Create brain and agent
    auto brain = std::make_shared<nlm::Brain>(config);
    auto agentBrain = std::make_shared<nlm::AgentBrain>(brain);
    auto world = std::make_shared<nlm::SimpleWorld>();
    
    world->configure(10, 10, 4, 4);
    agentBrain->initialize(*world);
    brain->initialize();
    
    // Test 1: Invalid or empty sensory inputs
    nlm::SensoryPercept emptyPercept;
    agentBrain->processSensoryInput(emptyPercept); // Should not crash
    
    // Test 2: Rapid reset calls
    for (int i = 0; i < 5; ++i) {
        agentBrain->reset();
    }
    
    // Test 3: Development with null values
    for (int i = 0; i < 10; ++i) {
        agentBrain->updateDevelopment(i * 0.01f);
    }
    
    // Test 4: Reward modulation with extreme values
    agentBrain->applyRewardModulation(1000.0f, -1000.0f);
    agentBrain->applyRewardModulation(-1000.0f, 1000.0f);
    
    // Should not crash and should handle gracefully
    assert(agentBrain->getPredictionError() != std::numeric_limits<float>::infinity());
    
    std::cout << "    Error handling test passed" << std::endl;
}

void runAll() {
    testCompleteSimulationWorkflow();
    testComponentInteraction();
    testEndToEndScenario();
    testErrorHandlingInComplexScenarios();
}

} // namespace test_integration
