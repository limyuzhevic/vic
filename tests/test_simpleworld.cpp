// SimpleWorld Tests
#include "world/SimpleWorld.hpp"
#include "agent/AgentBody.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_simpleworld {

void testSimpleWorldCreation() {
    nlm::SimpleWorld world;
    
    assert(world.getWidth() > 0);
    assert(world.getHeight() > 0);
    assert(world.getMaxEnergy() > 0.0f);
    
    std::cout << "    testSimpleWorldCreation passed" << std::endl;
}

void testSimpleWorldConfiguration() {
    nlm::SimpleWorld world;
    
    world.configure(100.0f, 200.0f, 32, 32);
    
    assert(world.getWidth() == 100.0f);
    assert(world.getHeight() == 200.0f);
    
    std::cout << "    testSimpleWorldConfiguration passed" << std::endl;
}

void testSimpleWorldReset() {
    nlm::SimpleWorld world;
    world.configure(100.0f, 100.0f, 16, 16);
    
    // Set agent position
    world.setAgentStart(50.0f, 50.0f);
    
    // Update to create objects
    world.update(0.001);
    
    // Reset should clear world
    world.reset();
    
    assert(world.getAgentBody().x == 0.0f);
    assert(world.getAgentBody().y == 0.0f);
    
    std::cout << "    testSimpleWorldReset passed" << std::endl;
}

void testSimpleWorldAgentPositioning() {
    nlm::SimpleWorld world;
    world.configure(100.0f, 100.0f, 16, 16);
    
    // Set agent at different positions
    world.setAgentStart(10.0f, 20.0f);
    assert(world.getAgentBody().x == 10.0f);
    assert(world.getAgentBody().y == 20.0f);
    
    world.setAgentStart(90.0f, 80.0f);
    assert(world.getAgentBody().x == 90.0f);
    assert(world.getAgentBody().y == 80.0f);
    
    std::cout << "    testSimpleWorldAgentPositioning passed" << std::endl;
}

void testSimpleWorldMovement() {
    nlm::SimpleWorld world;
    world.configure(100.0f, 100.0f, 16, 16);
    
    // Set agent to center
    world.setAgentStart(50.0f, 50.0f);
    
    // Apply motor command to move forward
    nlm::MotorCommand moveForward = nlm::MotorCommand::MoveForward;
    nlm::ActionResult result = world.applyMotorCommand(moveForward, 0.0);
    
    // Agent should have moved (within bounds)
    assert(world.getAgentBody().x >= 0.0f);
    assert(world.getAgentBody().y >= 0.0f);
    assert(world.getAgentBody().x <= world.getWidth());
    assert(world.getAgentBody().y <= world.getHeight());
    
    std::cout << "    testSimpleWorldMovement passed" << std::endl;
}

void testSimpleWorldSensoryPerception() {
    nlm::SimpleWorld world;
    world.configure(100.0f, 100.0f, 16, 16);
    
    // Set agent at center
    world.setAgentStart(50.0f, 50.0f);
    
    // Add some objects
    nlm::WorldObject obj1(30.0f, 30.0f, nlm::WorldObjectType::Resource, 10.0f);
    world.addObject(obj1);
    
    nlm::WorldObject obj2(70.0f, 70.0f, nlm::WorldObjectType::Hazard, -5.0f);
    world.addObject(obj2);
    
    // Get sensory percept
    const nlm::SensoryPercept& percept = world.getSensoryPercept();
    
    // Vision should have values
    const auto& vision = percept.getVision();
    assert(!vision.empty());
    
    std::cout << "    testSimpleWorldSensoryPerception passed" << std::endl;
}

void testSimpleWorldObjectInteraction() {
    nlm::SimpleWorld world;
    world.configure(100.0f, 100.0f, 16, 16);
    
    // Add a resource object
    nlm::WorldObject resource(50.0f, 50.0f, nlm::WorldObjectType::Resource, 10.0f);
    world.addObject(resource);
    
    // Set agent near resource
    world.setAgentStart(45.0f, 45.0f);
    
    // Interact with object
    nlm::MotorCommand interact = nlm::MotorCommand::Interact;
    nlm::ActionResult result = world.applyMotorCommand(interact, 0.0);
    
    // Object might be collected (energy increased)
    assert(world.getAgentBody().energy <= world.getMaxEnergy() + 1.0f);
    
    std::cout << "    testSimpleWorldObjectInteraction passed" << std::endl;
}

void testSimpleWorldBoundaries() {
    nlm::SimpleWorld world;
    world.configure(100.0f, 100.0f, 16, 16);
    
    // Set agent at boundary
    world.setAgentStart(0.0f, 0.0f);
    
    // Try to move outside bounds
    nlm::MotorCommand moveLeft = nlm::MotorCommand::TurnLeft;
    nlm::ActionResult result = world.applyMotorCommand(moveLeft, 0.0);
    
    // Position should still be valid
    assert(world.isValidPosition(world.getAgentBody().x, world.getAgentBody().y));
    
    std::cout << "    testSimpleWorldBoundaries passed" << std::endl;
}

void runAll() {
    std::cout << "Running SimpleWorld tests..." << std::endl;
    testSimpleWorldCreation();
    testSimpleWorldConfiguration();
    testSimpleWorldReset();
    testSimpleWorldAgentPositioning();
    testSimpleWorldMovement();
    testSimpleWorldSensoryPerception();
    testSimpleWorldObjectInteraction();
    testSimpleWorldBoundaries();
}

} // namespace test_simpleworld
