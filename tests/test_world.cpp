// World System Tests
// Tests for world simulation, environment, agent interaction

#include "world/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include "agent/SensoryPercept.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_world {

void testSimpleWorld() {
    auto config = std::make_shared<nlm::Config>();
    
    nlm::SimpleWorld world;
    
    // Test initialization
    world.configure(640, 480, 320, 240);
    
    assert(world.getWidth() == 640);
    assert(world.getHeight() == 480);
    assert(world.getVisionWidth() == 320);
    assert(world.getVisionHeight() == 240);
    
    // Test reset
    world.reset();
    assert(world.getSimulationTime() == 0.0f);
    
    // Test agent start
    world.setAgentStart(320.0f, 240.0f);
    auto body = world.getAgentBody();
    assert(body.x == 320.0f);
    assert(body.y == 240.0f);
    
    // Test world update
    world.update(0.1f);
    assert(world.getSimulationTime() == 0.1f);
    
    std::cout << "    testSimpleWorld passed" << std::endl;
}

void testAgentInteraction() {
    auto config = std::make_shared<nlm::Config>();
    
    nlm::SimpleWorld world;
    world.configure(100, 100, 50, 50);
    world.reset();
    world.setAgentStart(50.0f, 50.0f);
    
    // Create brain and agent
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    nlm::AgentBrain agent(brain);
    agent.initialize(world);
    
    // Test sensory input
    auto percept = world.getSensoryPercept();
    agent.processSensoryInput(percept);
    
    // Test motor command
    auto action = agent.decodeMotorCommand();
    world.applyMotorCommand(action, world.getSimulationTime());
    
    // Test reward
    float reward = world.computeReward(brain->getRegions()[0].get());
    
    assert(reward >= 0.0f); // Reward should be non-negative
    
    std::cout << "    testAgentInteraction passed" << std::endl;
}

void runAll() {
    testSimpleWorld();
    testAgentInteraction();
}

} // namespace test_world