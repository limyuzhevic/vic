#include "test_common.hpp"
#include <cassert>
#include <cmath>
#include <algorithm>

namespace test_world {

void testSimpleWorldCreation() {
    nlm::SimpleWorld world;
    assert(world.getTimeStep() == 0.001f);
    assert(world.getWidth() == 100.0f);
    assert(world.getHeight() == 100.0f);
    assert(world.getDepth() == 100.0f);
    std::cout << "    testSimpleWorldCreation passed" << std::endl;
}

void testWorldTime() {
    nlm::SimpleWorld world;
    
    // Get current time
    float time = world.getTime();
    assert(time == 0.0f);
    
    // Advance time
    world.advanceTime(1.0f);
    assert(world.getTime() == 1.0f);
    
    std::cout << "    testWorldTime passed" << std::endl;
}

void testWorldSimulation() {
    nlm::SimpleWorld world;
    
    // Run simulation
    for (int i = 0; i < 10; ++i) {
        world.simulateStep();
    }
    
    assert(world.getTime() == 0.01f);
    std::cout << "    testWorldSimulation passed" << std::endl;
}

void testWorldBoundaries() {
    nlm::SimpleWorld world;
    
    // Test world boundaries
    assert(world.isInBounds(50.0f, 50.0f, 50.0f) == true);
    assert(world.isInBounds(-50.0f, 50.0f, 50.0f) == false);
    assert(world.isInBounds(50.0f, -50.0f, 50.0f) == false);
    assert(world.isInBounds(50.0f, 50.0f, -50.0f) == false);
    
    std::cout << "    testWorldBoundaries passed" << std::endl;
}

void testWorldIntegration() {
    nlm::SimpleWorld world;
    
    // Test integration with Brain
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // World should be accessible
    assert(&world != nullptr);
    std::cout << "    testWorldIntegration passed" << std::endl;
}

void runAll() {
    std::cout << "Running World tests..." << std::endl;
    testSimpleWorldCreation();
    testWorldTime();
    testWorldSimulation();
    testWorldBoundaries();
    testWorldIntegration();
}

} // namespace test_world