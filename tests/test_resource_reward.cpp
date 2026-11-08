// Resource Reward Tests
// Tests for the resource reward system fix (0 reward issue)

#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_resource_reward {

void testWorldResourceInteraction() {
    // Test that the world provides rewards for resource interactions
    nlm::SimpleWorld world;
    world.configure(16, 16, 16, 16);
    world.reset();
    
    // Add a resource object to the world
    world.addObject(nlm::WorldObject(5.0f, 5.0f, nlm::WorldObjectType::Resource, 10.0f));
    
    // Set agent position near the resource
    nlm::AgentBody agent;
    agent.x = 4.5f;
    agent.y = 4.5f;
    agent.orientation = 0.0f;
    agent.energy = 1.0f;
    agent.health = 1.0f;
    agent.age = 0.0f;
    agent.isMoving = false;
    agent.isTurning = false;
    agent.lastActionTime = -1000.0f;
    
    // Test interaction with resource
    auto result = world.interact();
    
    // The result should have a positive reward for the resource
    assert(result.success);
    assert(result.reward > 0.0f);
    assert(result.message == "consumed_resource");
    assert(agent.energy > 1.0f);  // Energy should increase from consuming resource
    
    std::cout << "    testWorldResourceInteraction passed" << std::endl;
}

void testWorldHazardInteraction() {
    // Test that the world provides negative rewards for hazard interactions
    nlm::SimpleWorld world;
    world.configure(16, 16, 16, 16);
    world.reset();
    
    // Add a hazard object to the world
    world.addObject(nlm::WorldObject(5.0f, 5.0f, nlm::WorldObjectType::Hazard, -5.0f));
    
    // Set agent position near the hazard
    nlm::AgentBody agent;
    agent.x = 4.5f;
    agent.y = 4.5f;
    agent.orientation = 0.0f;
    agent.energy = 1.0f;
    agent.health = 1.0f;
    agent.age = 0.0f;
    agent.isMoving = false;
    agent.isTurning = false;
    agent.lastActionTime = -1000.0f;
    
    // Test interaction with hazard
    auto result = world.interact();
    
    // The result should have a negative reward for the hazard
    assert(result.success);
    assert(result.reward < 0.0f);
    assert(result.message == "hit_hazard");
    assert(agent.health < 1.0f);  // Health should decrease from hitting hazard
    
    std::cout << "    testWorldHazardInteraction passed" << std::endl;
}

void testSimpleWorldMovement() {
    // Test that SimpleWorld provides movement rewards
    nlm::SimpleWorld world;
    world.configure(16, 16, 16, 16);
    world.reset();
    
    // Set agent position
    world.setAgentStart(8.0f, 8.0f);
    
    // Apply a movement command
    nlm::MotorCommand moveForward = nlm::MotorCommand::MoveForward;
    auto result = world.applyMotorCommand(moveForward, 0.0);
    
    // Movement should have a small negative reward (cost for movement)
    assert(result.success);
    assert(result.reward < 0.0f);
    assert(result.reward > -1.0f);  // Should be a small cost
    
    std::cout << "    testSimpleWorldMovement passed" << std::endl;
}

} // namespace test_resource_reward