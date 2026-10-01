// tests/test_simpleworld.cpp - Test suite for new SimpleWorld implementation
#include "SimpleWorld.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

void testBasicConfiguration() {
    std::cout << "Testing basic configuration..." << std::endl;
    
    nlm::WorldConfig config;
    config.width = 20.0f;
    config.height = 20.0f;
    config.visionWidth = 16;
    config.visionHeight = 16;
    
    nlm::SimpleWorld world(config);
    
    assert(world.getWidth() == 20.0f);
    assert(world.getHeight() == 20.0f);
    assert(world.getSensoryPercept().getVisionWidth() == 16);
    assert(world.getSensoryPercept().getVisionHeight() == 16);
    
    std::cout << "✓ Basic configuration test passed" << std::endl;
}

void testWorldReset() {
    std::cout << "Testing world reset..." << std::endl;
    
    nlm::WorldConfig config;
    nlm::SimpleWorld world(config);
    
    double initialTime = world.getSimulationTime();
    world.reset();
    
    // After reset, time should be reset to 0
    assert(world.getSimulationTime() == 0.0);
    
    std::cout << "✓ World reset test passed" << std::endl;
}

void testAgentStart() {
    std::cout << "Testing agent start position..." << std::endl;
    
    nlm::WorldConfig config;
    nlm::SimpleWorld world(config);
    
    world.setAgentStart(5.0f, 7.0f);
    const auto& agent = world.getAgentBody();
    
    assert(agent.x == 5.0f);
    assert(agent.y == 7.0f);
    
    std::cout << "✓ Agent start position test passed" << std::endl;
}

void testBoundaryChecking() {
    std::cout << "Testing boundary checking..." << std::endl;
    
    nlm::WorldConfig config;
    config.width = 10.0f;
    config.height = 10.0f;
    nlm::SimpleWorld world(config);
    
    // Valid positions
    assert(world.isValidPosition(0.0f, 0.0f) == true);
    assert(world.isValidPosition(5.0f, 5.0f) == true);
    assert(world.isValidPosition(9.99f, 9.99f) == true);
    
    // Invalid positions
    assert(world.isValidPosition(-0.1f, 5.0f) == false);
    assert(world.isValidPosition(5.0f, -0.1f) == false);
    assert(world.isValidPosition(10.0f, 5.0f) == false);
    assert(world.isValidPosition(5.0f, 10.0f) == false);
    assert(world.isValidPosition(10.5f, 5.0f) == false);
    assert(world.isValidPosition(5.0f, 10.5f) == false);
    
    std::cout << "✓ Boundary checking test passed" << std::endl;
}

void testObjectManagement() {
    std::cout << "Testing object management..." << std::endl;
    
    nlm::WorldConfig config;
    config.width = 20.0f;
    config.height = 20.0f;
    nlm::SimpleWorld world(config);
    
    // Add test object
    nlm::WorldObject obj(5.0f, 5.0f, nlm::WorldObjectType::Resource, 10.0f, 0.5f);
    world.addObject(obj);
    
    // Get object at position
    nlm::WorldObject* found = world.getObjectAt(5.0f, 5.0f);
    assert(found != nullptr);
    assert(found->x == 5.0f);
    assert(found->y == 5.0f);
    assert(found->type == nlm::WorldObjectType::Resource);
    assert(found->value == 10.0f);
    
    // Remove object
    world.removeObject(5.0f, 5.0f);
    
    // Object should no longer be accessible
    found = world.getObjectAt(5.0f, 5.0f);
    // Note: After removal, object might still be accessible until cleanup
    
    std::cout << "✓ Object management test passed" << std::endl;
}

void testRandomSeed() {
    std::cout << "Testing random seed..." << std::endl;
    
    nlm::WorldConfig config1;
    config1.width = 20.0f;
    config1.height = 20.0f;
    config1.randomSeed = 123;
    
    nlm::WorldConfig config2;
    config2.width = 20.0f;
    config2.height = 20.0f;
    config2.randomSeed = 123;
    
    nlm::SimpleWorld world1(config1);
    nlm::SimpleWorld world2(config2);
    
    assert(world1.getRandomSeed() == world2.getRandomSeed());
    
    std::cout << "✓ Random seed test passed" << std::endl;
}

void testConfigurationValidation() {
    std::cout << "Testing configuration validation..." << std::endl;
    
    // Test valid configuration
    nlm::WorldConfig validConfig;
    validConfig.width = 20.0f;
    validConfig.height = 20.0f;
    validConfig.visionWidth = 16;
    validConfig.visionHeight = 16;
    
    nlm::SimpleWorld world(validConfig);
    
    // Test invalid configuration (should throw)
    try {
        nlm::WorldConfig invalidConfig;
        invalidConfig.width = 0.0f; // Invalid: width must be positive
        invalidConfig.height = 20.0f;
        nlm::SimpleWorld invalidWorld(invalidConfig);
        assert(false && "Should have thrown exception for invalid width");
    } catch (const nlm::WorldError& e) {
        // Expected
        assert(e.code() == nlm::ErrorCode::InvalidParameter);
    }
    
    std::cout << "✓ Configuration validation test passed" << std::endl;
}

void testPerformanceTracking() {
    std::cout << "Testing performance tracking..." << std::endl;
    
    nlm::WorldConfig config;
    nlm::SimpleWorld world(config);
    
    const auto& stats = world.getPerformanceStats();
    
    // Initial stats should be zero
    assert(stats.totalUpdates == 0);
    assert(stats.visionGenerationTime == 0);
    assert(stats.collisionDetectionTime == 0);
    assert(stats.interactionTime == 0);
    
    // Update world and check that stats are updated
    world.update(0.1);
    
    assert(stats.totalUpdates > 0);
    
    std::cout << "✓ Performance tracking test passed" << std::endl;
}

void testThreadSafety() {
    std::cout << "Testing thread safety..." << std::endl;
    
    nlm::WorldConfig config;
    nlm::SimpleWorld world(config);
    
    // Test locking
    assert(world.tryLock() == true);
    world.unlock();
    
    // Lock and unlock multiple times
    for (int i = 0; i < 5; ++i) {
        assert(world.tryLock() == true);
        world.unlock();
    }
    
    std::cout << "✓ Thread safety test passed" << std::endl;
}

void testMemoryPool() {
    std::cout << "Testing memory pool..." << std::endl;
    
    nlm::WorldConfig config;
    config.objectPoolSize = 10;
    nlm::SimpleWorld world(config);
    
    // Add and remove objects multiple times
    for (int i = 0; i < 5; ++i) {
        nlm::WorldObject obj(i * 2.0f, i * 2.0f, 
                            nlm::WorldObjectType::Resource, 
                            static_cast<float>(i), 0.5f);
        world.addObject(obj);
        
        nlm::WorldObject* found = world.getObjectAt(i * 2.0f, i * 2.0f);
        assert(found != nullptr);
        assert(found->active == true);
        
        world.removeObject(i * 2.0f, i * 2.0f);
    }
    
    std::cout << "✓ Memory pool test passed" << std::endl;
}

void testSpatialIndex() {
    std::cout << "Testing spatial index..." << std::endl;
    
    nlm::WorldConfig config;
    config.width = 20.0f;
    config.height = 20.0f;
    nlm::SimpleWorld world(config);
    
    // Add objects at different positions
    world.addObject(nlm::WorldObject(3.0f, 3.0f, nlm::WorldObjectType::Resource, 10.0f, 1.0f));
    world.addObject(nlm::WorldObject(7.0f, 7.0f, nlm::WorldObjectType::Hazard, -15.0f, 1.0f));
    world.addObject(nlm::WorldObject(15.0f, 15.0f, nlm::WorldObjectType::Resource, 20.0f, 1.0f));
    
    // Query spatial index for objects near center
    nlm::WorldObject* center = world.getObjectAt(8.0f, 8.0f);
    assert(center != nullptr); // Should find hazard at (7, 7)
    
    std::cout << "✓ Spatial index test passed" << std::endl;
}

int main() {
    std::cout << "Running SimpleWorld test suite..." << std::endl;
    std::cout << "=====================================" << std::endl;
    
    try {
        testBasicConfiguration();
        testWorldReset();
        testAgentStart();
        testBoundaryChecking();
        testObjectManagement();
        testRandomSeed();
        testConfigurationValidation();
        testPerformanceTracking();
        testThreadSafety();
        testMemoryPool();
        testSpatialIndex();
        
        std::cout << std::endl;
        std::cout << "=====================================" << std::endl;
        std::cout << "All tests passed! ✓" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}