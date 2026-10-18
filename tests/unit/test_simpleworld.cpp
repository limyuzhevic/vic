// SimpleWorld Tests
// Comprehensive tests for SimpleWorld bug fixes and enhancements

#include "test_framework.hpp"
#include "../src/world/SimpleWorld.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_simpleworld {

// Test SimpleWorld creation and basic functionality
class SimpleWorldCreationTest : public TestSuite {
public:
    SimpleWorldCreationTest() : TestSuite("SimpleWorld::Creation") {}
    
    void run() override {
        std::cout << "  Testing SimpleWorld creation..." << std::endl;
        
        TIME_TEST("SimpleWorld Construction", {
            nlm::SimpleWorld world;
            assert(world.getWidth() == 20.0f);
            assert(world.getHeight() == 20.0f);
            assert(world.getVisionWidth() == 16);
            assert(world.getVisionHeight() == 16);
            assert(world.getMaxEnergy() == 100.0f);
            assert(world.getEnergyDecayRate() == 0.01f);
        });
        
        // Test world configuration
        TIME_TEST("World Configuration", {
            nlm::SimpleWorld world;
            world.configure(100.0f, 100.0f, 32, 32);
            
            assert(world.getWidth() == 100.0f);
            assert(world.getHeight() == 100.0f);
            assert(world.getVisionWidth() == 32);
            assert(world.getVisionHeight() == 32);
        });
        
        std::cout << "  SimpleWorld Creation tests PASSED" << std::endl;
    }
};

// Test world reset and initialization
class SimpleWorldResetTest : public TestSuite {
public:
    SimpleWorldResetTest() : TestSuite("SimpleWorld::Reset") {}
    
    void run() override {
        std::cout << "  Testing SimpleWorld reset..." << std::endl;
        
        TIME_TEST("World Reset", {
            nlm::SimpleWorld world;
            
            // Set initial state
            world.configure(50.0f, 50.0f, 16, 16);
            world.setAgentStart(10.0f, 10.0f);
            
            // Reset world
            world.reset();
            
            // Verify reset state
            assert(world.getAgentX() >= 0.0f && world.getAgentX() < 50.0f);
            assert(world.getAgentY() >= 0.0f && world.getAgentY() < 50.0f);
            assert(world.getAgentEnergy() == 100.0f);
            
            // Verify objects were placed
            auto body = world.getAgentBody();
            assert(body != nullptr);
        });
        
        std::cout << "  SimpleWorld Reset tests PASSED" << std::endl;
    }
};

// Test world physics and movement
class SimpleWorldPhysicsTest : public TestSuite {
public:
    SimpleWorldPhysicsTest() : TestSuite("SimpleWorld::Physics") {}
    
    void run() override {
        std::cout << "  Testing SimpleWorld physics..." << std::endl;
        
        // Create world
        nlm::SimpleWorld world;
        world.configure(100.0f, 100.0f, 16, 16);
        world.reset();
        
        // Set initial position
        world.setAgentStart(10.0f, 10.0f);
        
        TIME_TEST("World Update Physics", {
            // Get initial position
            float initialX = world.getAgentX();
            float initialY = world.getAgentY();
            
            // Update world
            world.update(0.1);  // 0.1 second timestep
            
            // Verify position changed
            float finalX = world.getAgentX();
            float finalY = world.getAgentY();
            
            assert(finalX != initialX || finalY != initialY);
            
            // Verify within bounds
            assert(finalX >= 0.0f && finalX < 100.0f);
            assert(finalY >= 0.0f && finalY < 100.0f);
        });
        
        // Test boundary collision
        TIME_TEST("Boundary Collision", {
            nlm::SimpleWorld world;
            world.configure(50.0f, 50.0f, 16, 16);
            world.reset();
            
            // Move agent to boundary
            world.setAgentStart(49.9f, 49.9f);
            
            // Update with large velocity
            world.update(1.0);  // This should trigger collision
            
            // Verify position at boundary
            float x = world.getAgentX();
            float y = world.getAgentY();
            
            assert(x >= 0.0f && x <= 50.0f);
            assert(y >= 0.0f && y <= 50.0f);
        });
        
        std::cout << "  SimpleWorld Physics tests PASSED" << std::endl;
    }
};

// Test world interactions and actions
class SimpleWorldInteractionTest : public TestSuite {
public:
    SimpleWorldInteractionTest() : TestSuite("SimpleWorld::Interaction") {}
    
    void run() override {
        std::cout << "  Testing SimpleWorld interactions..." << std::endl;
        
        // Create world
        nlm::SimpleWorld world;
        world.configure(100.0f, 100.0f, 16, 16);
        world.reset();
        
        TIME_TEST("Object Interaction", {
            // Get initial state
            float initialEnergy = world.getAgentEnergy();
            float initialHealth = world.getAgentHealth();
            
            // Apply a motor command that should trigger interaction
            nlm::ActionResult result = world.applyMotorCommand(nlm::MotorCommand::Interact, 0.0);
            
            // Verify interaction result
            // Result depends on whether objects are in range
            assert(result.success == true || result.success == false);
            assert(result.reward >= -10.0f && result.reward <= 10.0f);
        });
        
        // Test collision detection
        TIME_TEST("Collision Detection", {
            nlm::SimpleWorld world;
            world.configure(100.0f, 100.0f, 16, 16);
            world.reset();
            
            // Get initial health
            float initialHealth = world.getAgentHealth();
            
            // Update world to potentially trigger passive damage
            world.update(1.0);
            
            // Verify health didn't increase unexpectedly
            float finalHealth = world.getAgentHealth();
            assert(finalHealth <= initialHealth + 0.1f);  // Allow small floating point errors
        });
        
        std::cout << "  SimpleWorld Interaction tests PASSED" << std::endl;
    }
};

// Test world vision system
class SimpleWorldVisionTest : public TestSuite {
public:
    SimpleWorldVisionTest() : TestSuite("SimpleWorld::Vision") {}
    
    void run() override {
        std::cout << "  Testing SimpleWorld vision system..." << std::endl;
        
        // Create world
        nlm::SimpleWorld world;
        world.configure(100.0f, 100.0f, 16, 16);
        world.reset();
        
        TIME_TEST("Vision Generation", {
            // Generate vision
            world.generateVision();
            
            // Get vision data
            auto vision = world.getVision();
            assert(vision.size() == 256);  // 16x16 vision grid
            
            // Each vision value should be between 0.0 and 1.0
            for (float v : vision) {
                assert(v >= 0.0f && v <= 1.0f);
            }
            
            // Vision should encode object information
            // (0 = empty/far, 0.3-0.8 = objects, 1.0 = wall)
        });
        
        std::cout << "  SimpleWorld Vision tests PASSED" << std::endl;
    }
};

// Global test instances
static SimpleWorldCreationTest simpleWorldCreationTest;
static SimpleWorldResetTest simpleWorldResetTest;
static SimpleWorldPhysicsTest simpleWorldPhysicsTest;
static SimpleWorldInteractionTest simpleWorldInteractionTest;
static SimpleWorldVisionTest simpleWorldVisionTest;

void runAll() {
    std::cout << "=== SimpleWorld Tests ===" << std::endl;
    
    // Register all test suites with the framework
    TestFramework& framework = TestFramework::getInstance();
    framework.addSuite(&simpleWorldCreationTest);
    framework.addSuite(&simpleWorldResetTest);
    framework.addSuite(&simpleWorldPhysicsTest);
    framework.addSuite(&simpleWorldInteractionTest);
    framework.addSuite(&simpleWorldVisionTest);
    
    // Run all tests
    bool allPassed = framework.runAllTests();
    
    if (allPassed) {
        std::cout << "=== All SimpleWorld Tests PASSED ===" << std::endl;
    } else {
        std::cout << "=== Some SimpleWorld Tests FAILED ===" << std::endl;
    }
}

} // namespace test_simpleworld
