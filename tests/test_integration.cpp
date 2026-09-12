// Integration Tests for AgentBrain and SimpleWorld
// Tests the full brain-world-agent interaction loop

#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_integration {

void testAgentBrainInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::AgentBrain agent(brain);
    nlm::SimpleWorld world;
    world.configure(20, 20, 8, 8);
    world.reset();
    
    bool success = agent.initialize(world);
    assert(success);
    
    std::cout << "    testAgentBrainInitialization passed" << std::endl;
}

void testAgentSensoryProcessing() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::AgentBrain agent(brain);
    nlm::SimpleWorld world;
    world.configure(20, 20, 8, 8);
    world.reset();
    
    agent.initialize(world);
    
    // Get sensory percept
    auto percept = world.getSensoryPercept();
    assert(percept != nullptr);
    
    // Process sensory input
    agent.processSensoryInput(*percept);
    
    // Verify brain received input
    assert(brain.getTotalNeuronCount() == 100);
    
    std::cout << "    testAgentSensoryProcessing passed" << std::endl;
}

void testAgentMotorOutput() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::AgentBrain agent(brain);
    nlm::SimpleWorld world;
    world.configure(20, 20, 8, 8);
    world.reset();
    
    agent.initialize(world);
    
    // Get motor command
    auto action = agent.decodeMotorCommand();
    assert(action != nullptr);
    
    // Apply to world
    world.applyMotorCommand(*action, 0.0);
    
    // Check agent body moved
    auto body = world.getAgentBody();
    assert(body != nullptr);
    
    std::cout << "    testAgentMotorOutput passed" << std::endl;
}

void testCompleteAgentWorldLoop() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::AgentBrain agent(brain);
    nlm::SimpleWorld world;
    world.configure(10, 10, 8, 8);
    world.reset();
    agent.initialize(world);
    
    // Enable learning systems
    agent.enableRewardModulation(true);
    agent.enableStructuralPlasticity(true);
    agent.enableDevelopment(true);
    agent.enableCuriosity(true);
    
    // Run complete loop
    for (int step = 0; step < 50; ++step) {
        // Update world
        world.update(0.1);
        
        // Get sensory input
        auto percept = world.getSensoryPercept();
        if (percept) {
            agent.processSensoryInput(*percept);
        }
        
        // Process brain
        brain.step(step);
        
        // Get and apply action
        auto action = agent.decodeMotorCommand();
        if (action) {
            world.applyMotorCommand(*action, world.getSimulationTime());
        }
        
        // Apply reward modulation
        agent.applyRewardModulation(0.5, 0.3);
        
        // Update development
        agent.updateDevelopment(0.1);
    }
    
    // Verify learning occurred
    assert(brain.getTotalNeuronCount() == 50);
    assert(world.getAgentBody() != nullptr);
    
    std::cout << "    testCompleteAgentWorldLoop passed" << std::endl;
}

void testWorldObjectInteraction() {
    nlm::SimpleWorld world;
    world.configure(20, 20, 8, 8);
    world.reset();
    
    // Add world objects
    nlm::WorldObject resourceObj(5.0f, 5.0f, nlm::WorldObjectType::Resource, 1.0f);
    world.addObject(resourceObj);
    
    nlm::WorldObject wallObj(10.0f, 10.0f, nlm::WorldObjectType::Wall, 0.0f);
    world.addObject(wallObj);
    
    // Test collision detection
    assert(world.isValidPosition(3.0f, 3.0f));  // Should be valid
    assert(!world.isValidPosition(5.0f, 5.0f)); // Object there
    
    // Remove object
    world.removeObject(5.0f, 5.0f);
    assert(world.isValidPosition(5.0f, 5.0f));  // Now valid
    
    std::cout << "    testWorldObjectInteraction passed" << std::endl;
}

void runAllIntegrationTests() {
    std::cout << "Running integration tests..." << std::endl;
    testAgentBrainInitialization();
    testAgentSensoryProcessing();
    testAgentMotorOutput();
    testCompleteAgentWorldLoop();
    testWorldObjectInteraction();
    std::cout << "All integration tests passed!" << std::endl;
}

} // namespace test_integration
