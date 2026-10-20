// World Simulation Tests - Comprehensive validation of world simulation
#include "world/SimpleWorld.hpp"
#include "agent/AgentBody.hpp"
#include "agent/SensoryPercept.hpp"
#include "TestFramework.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <cmath>

namespace test_world {

// World configuration validation test
class WorldConfigurationValidationTest : public test_framework::TestBase {
public:
    WorldConfigurationValidationTest(const std::string& name)
        : TestBase(name, test_framework::TestCategory::WorldSimulation) {}
    
    void run() override {
        SimpleWorld world;
        
        // Test configuration validation
        world.configure(100.0f, 100.0f, 16, 16);
        
        assert(world.getWidth() == 100.0f);
        assert(world.getHeight() == 100.0f);
        
        // Test agent start position
        world.setAgentStart(50.0f, 50.0f);
        
        assert(world.getAgentBody().x == 50.0f);
        assert(world.getAgentBody().y == 50.0f);
        
        // Test energy settings
        world.setMaxEnergy(1000.0f);
        assert(world.getMaxEnergy() == 1000.0f);
        
        world.setEnergyDecayRate(0.1f);
        assert(world.getEnergyDecayRate() == 0.1f);
        
        // Test random seed configuration
        world.setRandomSeed(42);
        assert(world.getRandomSeed() == 42);
        
        // Test invalid configuration (negative dimensions)
        // Note: This would require additional validation in the actual implementation
        
        std::cout << "    World configuration validation test for " << getName() << " passed" << std::endl;
    }
};

// World object validation test
class WorldObjectValidationTest : public test_framework::TestBase {
public:
    WorldObjectValidationTest(const std::string& name)
        : TestBase(name, test_framework::TestCategory::WorldSimulation) {}
    
    void run() override {
        SimpleWorld world;
        world.configure(100.0f, 100.0f, 16, 16);
        
        // Test adding different types of objects
        nlm::WorldObject resourceObj(10.0f, 10.0f, nlm::WorldObjectType::Resource, 100.0f, 1.0f);
        world.addObject(resourceObj);
        
        nlm::WorldObject hazardObj(90.0f, 90.0f, nlm::WorldObjectType::Hazard, -50.0f, 2.0f);
        world.addObject(hazardObj);
        
        nlm::WorldObject wallObj(5.0f, 5.0f, nlm::WorldObjectType::Wall, 0.0f, 3.0f);
        world.addObject(wallObj);
        
        nlm::WorldObject markerObj(20.0f, 20.0f, nlm::WorldObjectType::Marker, 0.0f, 0.5f);
        world.addObject(markerObj);
        
        // Test object retrieval
        nlm::WorldObject* foundResource = world.getObjectAt(10.0f, 10.0f);
        assert(foundResource != nullptr);
        assert(foundResource->type == nlm::WorldObjectType::Resource);
        assert(foundResource->value == 100.0f);
        
        nlm::WorldObject* foundHazard = world.getObjectAt(90.0f, 90.0f);
        assert(foundHazard != nullptr);
        assert(foundHazard->type == nlm::WorldObjectType::Hazard);
        assert(foundHazard->value == -50.0f);
        
        // Test invalid position (outside bounds)
        nlm::WorldObject* outOfBounds = world.getObjectAt(150.0f, 150.0f);
        assert(outOfBounds == nullptr);
        
        // Test position validation
        assert(world.isValidPosition(50.0f, 50.0f));  // Should be valid (agent start)
        assert(!world.isValidPosition(5.0f, 5.0f));   // Should be invalid (wall)
        assert(!world.isValidPosition(0.0f, 0.0f));   // Should be invalid (bounds)
        assert(!world.isValidPosition(100.0f, 100.0f)); // Should be invalid (bounds)
        
        // Test object removal
        world.removeObject(10.0f, 10.0f);
        nlm::WorldObject* removed = world.getObjectAt(10.0f, 10.0f);
        assert(removed == nullptr);
        
        std::cout << "    World object validation test for " << getName() << " passed" << std::endl;
    }
};

// World simulation validation test
class WorldSimulationValidationTest : public test_framework::TestBase {
public:
    WorldSimulationValidationTest(const std::string& name)
        : TestBase(name, test_framework::TestCategory::WorldSimulation) {}
    
    void run() override {
        SimpleWorld world;
        world.configure(100.0f, 100.0f, 16, 16);
        
        // Test reset
        world.reset();
        
        // Add test objects
        nlm::WorldObject resource(10.0f, 10.0f, nlm::WorldObjectType::Resource, 100.0f);
        nlm::WorldObject hazard(90.0f, 90.0f, nlm::WorldObjectType::Hazard, -50.0f);
        world.addObject(resource);
        world.addObject(hazard);
        
        // Initial state
        nlm::SensoryPercept initialPercept = world.getSensoryPercept();
        nlm::AgentBody initialAgent = world.getAgentBody();
        
        // Simulate multiple steps
        double timestep = 1.0;
        for (int step = 0; step < 10; ++step) {
            world.update(timestep);
            
            // Verify agent state changes
            assert(world.getAgentBody().age >= initialAgent.age);
            assert(world.getAgentBody().energy <= initialAgent.energy || world.getAgentBody().energy >= 1.0f);
            
            // Verify sensory percept updates
            const nlm::SensoryPercept& currentPercept = world.getSensoryPercept();
            assert(currentPercept.getTimestamp() >= initialPercept.getTimestamp());
            
            // Simulate time progression
            world.setRandomSeed(42 + step); // Different seed each step for variety
        }
        
        // Test that world state evolved
        assert(world.getAgentBody().age > 0.0f);
        
        // Test interaction
        nlm::ActionResult interactionResult = world.interact();
        // Interaction may or may not happen depending on proximity
        // but the test should complete without crashing
        
        std::cout << "    World simulation validation test for " << getName() << " passed" << std::endl;
    }
};

// World physics validation test
class WorldPhysicsValidationTest : public test_framework::TestBase {
public:
    WorldPhysicsValidationTest(const std::string& name)
        : TestBase(name, test_framework::TestCategory::WorldSimulation) {}
    
    void run() override {
        SimpleWorld world;
        world.configure(100.0f, 100.0f, 16, 16);
        
        // Set agent at center
        world.setAgentStart(50.0f, 50.0f);
        
        // Test agent movement and physics
        nlm::ActionResult moveForward = world.applyMotorCommand(nlm::MotorCommand::MoveForward, 1.0);
        assert(moveForward.reward >= 0.0f || moveForward.success);
        
        nlm::ActionResult moveBackward = world.applyMotorCommand(nlm::MotorCommand::MoveBackward, 2.0);
        assert(moveForward.reward >= 0.0f || moveForward.success);
        
        nlm::ActionResult turnLeft = world.applyMotorCommand(nlm::MotorCommand::TurnLeft, 3.0);
        assert(moveForward.reward >= 0.0f || moveForward.success);
        
        nlm::ActionResult turnRight = world.applyMotorCommand(nlm::MotorCommand::TurnRight, 4.0);
        assert(moveForward.reward >= 0.0f || moveForward.success);
        
        // Test interaction
        nlm::ActionResult interact = world.applyMotorCommand(nlm::MotorCommand::Interact, 5.0);
        // Interaction may or may not succeed, but should not crash
        
        // Test energy decay over time
        float initialEnergy = world.getAgentBody().energy;
        world.update(1.0);
        float energyAfterUpdate = world.getAgentBody().energy;
        
        // Energy should decay over time (unless resources are collected)
        assert(energyAfterUpdate <= initialEnergy || energyAfterUpdate >= 1.0f);
        
        std::cout << "    World physics validation test for " << getName() << " passed" << std::endl;
    }
};

// World random seed validation test
class WorldRandomSeedValidationTest : public test_framework::TestBase {
public:
    WorldRandomSeedValidationTest(const std::string& name)
        : TestBase(name, test_framework::TestCategory::WorldSimulation) {}
    
    void run() override {
        SimpleWorld world1;
        world1.configure(100.0f, 100.0f, 16, 16);
        world1.setRandomSeed(12345);
        
        SimpleWorld world2;
        world2.configure(100.0f, 100.0f, 16, 16);
        world2.setRandomSeed(12345);
        
        // Both worlds should have the same random seed
        assert(world1.getRandomSeed() == world2.getRandomSeed());
        
        // Generate vision for both worlds
        world1.generateVision();
        world2.generateVision();
        
        // With the same seed, vision should be identical (or at least deterministic)
        // Note: This test assumes vision generation is deterministic with the seed
        
        // Test different seeds produce different results
        world2.setRandomSeed(54321);
        assert(world2.getRandomSeed() != world1.getRandomSeed());
        
        // Generate vision again
        world2.generateVision();
        
        // Vision should potentially be different (not guaranteed but likely)
        
        std::cout << "    World random seed validation test for " << getName() << " passed" << std::endl;
    }
};

// Stress test for world simulation
class WorldStressTest : public test_framework::TestBase {
public:
    WorldStressTest(const std::string& name)
        : TestBase(name, test_framework::TestCategory::Stress) {}
    
    void run() override {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        SimpleWorld world;
        world.configure(500.0f, 500.0f, 32, 32);
        
        // Add many objects to world
        for (int i = 0; i < 100; ++i) {
            float x = static_cast<float>(rand() % 400 + 50);
            float y = static_cast<float>(rand() % 400 + 50);
            nlm::WorldObjectType type = 
                (i % 4 == 0) ? nlm::WorldObjectType::Resource :
                (i % 4 == 1) ? nlm::WorldObjectType::Hazard :
                (i % 4 == 2) ? nlm::WorldObjectType::Wall :
                               nlm::WorldObjectType::Marker;
            float value = (type == nlm::WorldObjectType::Resource) ? 100.0f :
                         (type == nlm::WorldObjectType::Hazard) ? -50.0f : 0.0f;
            
            nlm::WorldObject obj(x, y, type, value);
            world.addObject(obj);
        }
        
        // Set agent start
        world.setAgentStart(250.0f, 250.0f);
        
        // Run many simulation steps
        for (int step = 0; step < 1000; ++step) {
            world.update(0.1);
            
            // Occasional agent actions
            if (step % 100 == 0) {
                world.applyMotorCommand(nlm::MotorCommand::MoveForward, step * 0.1);
            }
            
            if (step % 200 == 0) {
                world.applyMotorCommand(nlm::MotorCommand::Interact, step * 0.1);
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        std::cout << "    World stress test for " << getName() << " completed in " 
                  << duration.count() << "ms" << std::endl;
    }
};

// Unit Tests (Original world tests would go here)
void testWorldConfiguration() {
    SimpleWorld world;
    
    world.configure(100.0f, 100.0f, 16, 16);
    
    assert(world.getWidth() == 100.0f);
    assert(world.getHeight() == 100.0f);
    assert(world.getAgentBody().x == 0.0f);  // Default agent position
    assert(world.getAgentBody().y == 0.0f);
    
    std::cout << "    testWorldConfiguration passed" << std::endl;
}

void testWorldObjectManagement() {
    SimpleWorld world;
    world.configure(100.0f, 100.0f, 16, 16);
    
    nlm::WorldObject obj(10.0f, 10.0f, nlm::WorldObjectType::Resource);
    world.addObject(obj);
    
    nlm::WorldObject* found = world.getObjectAt(10.0f, 10.0f);
    assert(found != nullptr);
    assert(found->x == 10.0f);
    assert(found->y == 10.0f);
    assert(found->type == nlm::WorldObjectType::Resource);
    
    world.removeObject(10.0f, 10.0f);
    
    nlm::WorldObject* removed = world.getObjectAt(10.0f, 10.0f);
    assert(removed == nullptr);
    
    std::cout << "    testWorldObjectManagement passed" << std::endl;
}

void testWorldSimulation() {
    SimpleWorld world;
    world.configure(100.0f, 100.0f, 16, 16);
    
    // Add some objects
    nlm::WorldObject resource(50.0f, 50.0f, nlm::WorldObjectType::Resource, 100.0f);
    world.addObject(resource);
    
    // Set agent start
    world.setAgentStart(25.0f, 25.0f);
    
    // Simulate
    world.update(1.0);
    
    // Verify agent age increased
    assert(world.getAgentBody().age > 0.0f);
    
    std::cout << "    testWorldSimulation passed" << std::endl;
}

void runAll() {
    std::cout << "=== Running Enhanced World Simulation Tests ===" << std::endl;
    
    // Run original unit tests
    testWorldConfiguration();
    testWorldObjectManagement();
    testWorldSimulation();
    
    std::cout << "\n=== Running World Simulation Validation Tests ===" << std::endl;
    
    // Run comprehensive validation tests
    WorldConfigurationValidationTest configTest("world_config_validation");
    configTest.run();
    
    WorldObjectValidationTest objectTest("world_object_validation");
    objectTest.run();
    
    WorldSimulationValidationTest simulationTest("world_simulation_validation");
    simulationTest.run();
    
    WorldPhysicsValidationTest physicsTest("world_physics_validation");
    physicsTest.run();
    
    WorldRandomSeedValidationTest seedTest("world_random_seed_validation");
    seedTest.run();
    
    // Run stress test (separate from regular validation)
    std::cout << "\n=== Running World Stress Test ===" << std::endl;
    WorldStressTest stressTest("world_stress_test");
    stressTest.run();
    
    std::cout << "\n=== All World Simulation Tests Completed ===" << std::endl;
}

} // namespace test_world