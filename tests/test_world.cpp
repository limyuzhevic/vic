// World Tests
#include "world/SimpleWorld.hpp"
#include "agent/AgentBody.hpp"
#include "agent/SensoryPercept.hpp"
#include <cassert>
#include <iostream>

namespace test_world {

void testWorldCreation() {
    nlm::SimpleWorld world;
    
    std::cout << "    testWorldCreation passed" << std::endl;
}

void testWorldConfiguration() {
    nlm::SimpleWorld world;
    
    // Configure world
    world.configure(20, 20, 8, 8);
    
    // Verify configuration
    assert(world.getWidth() == 20);
    assert(world.getHeight() == 20);
    assert(world.getSensoryInputSize() == 64); // 8x8 vision
    
    std::cout << "    testWorldConfiguration passed" << std::endl;
}

void testWorldReset() {
    nlm::SimpleWorld world;
    
    // Configure and reset
    world.configure(20, 20, 8, 8);
    world.reset();
    
    // Set agent position
    world.setAgentStart(5.0, 5.0);
    
    // Update world
    world.update(0.1);
    
    // Reset world
    world.reset();
    
    // Verify reset (should still have valid state after reset)
    assert(world.getSimulationTime() >= 0.0);
    
    std::cout << "    testWorldReset passed" << std::endl;
}

void testWorldAgentPosition() {
    nlm::SimpleWorld world;
    
    // Configure world
    world.configure(20, 20, 8, 8);
    world.reset();
    
    // Set agent position
    world.setAgentStart(5.0, 5.0);
    
    // Get agent body
    nlm::AgentBody body = world.getAgentBody();
    
    // Verify agent position
    assert(body.x == 5.0f);
    assert(body.y == 5.0f);
    
    std::cout << "    testWorldAgentPosition passed" << std::endl;
}

void testWorldSensoryPercept() {
    nlm::SimpleWorld world;
    
    // Configure world
    world.configure(20, 20, 8, 8);
    world.reset();
    
    // Get sensory percept
    nlm::SensoryPercept percept = world.getSensoryPercept();
    
    // Verify percept structure
    assert(pert.getVisionWidth() == 8);
    assert(pert.getVisionHeight() == 8);
    assert(pert.getVision().size() == 64);
    
    // Verify percept data
    std::vector<float> vision = percept.getVision();
    for (float value : vision) {
        assert(value >= 0.0f && value <= 1.0f);
    }
    
    std::cout << "    testWorldSensoryPercept passed" << std::endl;
}

void testWorldObjectManagement() {
    nlm::SimpleWorld world;
    
    // Configure world
    world.configure(20, 20, 8, 8);
    world.reset();
    
    // Add object
    nlm::WorldObject obj(10.0f, 10.0f, nlm::WorldObjectType::Resource, 1.0f, 0.5f);
    world.addObject(obj);
    
    // Remove object
    world.removeObject(10.0f, 10.0f);
    
    // Verify object can be added again (should not crash)
    nlm::WorldObject obj2(15.0f, 15.0f, nlm::WorldObjectType::Hazard, 0.5f, 0.3f);
    world.addObject(obj2);
    
    // Verify position is valid
    assert(world.isValidPosition(15.0f, 15.0f));
    assert(!world.isValidPosition(30.0f, 30.0f)); // Outside world bounds
    
    std::cout << "    testWorldObjectManagement passed" << std::endl;
}

void testWorldEnergyManagement() {
    nlm::SimpleWorld world;
    
    // Configure world
    world.configure(20, 20, 8, 8);
    world.reset();
    
    // Get initial energy
    float initialEnergy = world.getMaxEnergy();
    
    // Set energy
    world.setMaxEnergy(200.0f);
    
    // Verify energy changed
    assert(world.getMaxEnergy() == 200.0f);
    
    // Reset energy (should be back to original)
    world.reset();
    assert(world.getMaxEnergy() == initialEnergy);
    
    std::cout << "    testWorldEnergyManagement passed" << std::endl;
}

void testWorldRandomSeed() {
    nlm::SimpleWorld world1;
    nlm::SimpleWorld world2;
    
    // Configure worlds
    world1.configure(20, 20, 8, 8);
    world2.configure(20, 20, 8, 8);
    
    // Set different random seeds
    world1.setRandomSeed(123);
    world2.setRandomSeed(456);
    
    // Verify seeds are different
    assert(world1.getRandomSeed() != world2.getRandomSeed());
    
    // Verify seeds can be retrieved
    assert(world1.getRandomSeed() == 123);
    assert(world2.getRandomSeed() == 456);
    
    std::cout << "    testWorldRandomSeed passed" << std::endl;
}

void testWorldSimulationTime() {
    nlm::SimpleWorld world;
    
    // Configure world
    world.configure(20, 20, 8, 8);
    world.reset();
    
    // Get initial time
    double initialTime = world.getSimulationTime();
    
    // Update world
    world.update(0.5);
    
    // Verify time increased
    assert(world.getSimulationTime() > initialTime);
    
    // Reset world
    world.reset();
    
    // Verify time reset
    assert(world.getSimulationTime() == initialTime);
    
    std::cout << "    testWorldSimulationTime passed" << std::endl;
}

void runAll() {
    testWorldCreation();
    testWorldConfiguration();
    testWorldReset();
    testWorldAgentPosition();
    testWorldSensoryPercept();
    testWorldObjectManagement();
    testWorldEnergyManagement();
    testWorldRandomSeed();
    testWorldSimulationTime();
}

} // namespace test_world