// Simple World Tests
// Tests for SimpleWorld functionality including environment simulation, observation, and action application

#include "world/SimpleWorld.hpp"
#include "environment/Environment.hpp"
#include "environment/Observation.hpp"
#include "environment/Action.hpp"
#include "sensory/Vision.hpp"
#include "TestInfrastructure.hpp"
#include "TestUtilities.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <memory>

namespace test_simple_world {

void testSimpleWorldCreation() {
    nlm::SimpleWorld simpleWorld;
    
    // Test basic properties
    auto config = simpleWorld.getConfig();
    assert(config != nullptr);
    
    std::cout << "    testSimpleWorldCreation passed" << std::endl;
}

void testSimpleWorldStep() {
    nlm::SimpleWorld simpleWorld;
    
    // Test world stepping
    nlm::SimulationStep step = 100;
    
    // This should not crash
    simpleWorld.step(step);
    
    // Can check time advancement
    nlm::Timestamp time = simpleWorld.getTime();
    assert(time >= 0.0);
    
    std::cout << "    testSimpleWorldStep passed" << std::endl;
}

void testSimpleWorldEnvironmentIntegration() {
    nlm::SimpleWorld simpleWorld;
    
    // Test environment integration
    auto* environment = simpleWorld.getEnvironment();
    assert(environment != nullptr);
    
    // Get world environment
    nlm::Environment* worldEnv = simpleWorld.getEnvironment();
    assert(worldEnv != nullptr);
    
    std::cout << "    testSimpleWorldEnvironmentIntegration passed" << std::endl;
}

void testSimpleWorldObservation() {
    nlm::SimpleWorld simpleWorld;
    
    // Test observation generation
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Generate observation through world
    auto observation = simpleWorld.generateObservation(brain);
    assert(observation != nullptr);
    
    // Observation should have neural activity
    assert(!observation->neuralActivity.empty());
    
    std::cout << "    testSimpleWorldObservation passed" << std::endl;
}

void testSimpleWorldAction() {
    nlm::SimpleWorld simpleWorld;
    
    // Test action application
    nlm::Action action;
    
    // Create a simple action
    action.type = "move";
    action.parameters = "10, 5, 0";
    action.timestamp = 100;
    action.source = "agent";
    
    // Apply action to world
    simpleWorld.applyAction(action);
    
    // World state should be updated
    nlm::SimulationStep currentStep = simpleWorld.getCurrentStep();
    assert(currentStep >= 100);
    
    std::cout << "    testSimpleWorldAction passed" << std::endl;
}

void testSimpleWorldVisionIntegration() {
    nlm::SimpleWorld simpleWorld;
    
    // Test vision system integration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Get vision system through brain
    auto* vision = brain.getVision();
    assert(vision != nullptr);
    
    // Use vision to generate observations
    auto sensoryInput = vision->processEnvironment(simpleWorld);
    assert(sensoryInput != nullptr);
    
    std::cout << "    testSimpleWorldVisionIntegration passed" << std::endl;
}

void testSimpleWorldFullCycle() {
    nlm::SimpleWorld simpleWorld;
    
    // Create brain for integration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Full simulation cycle
    for (nlm::SimulationStep step = 0; step < 20; ++step) {
        // Generate observation
        auto observation = simpleWorld.generateObservation(brain);
        
        // Brain processes observation
        if (observation) {
            brain.receiveSensoryInput(*observation);
        }
        
        // Brain steps
        brain.step(step);
        
        // Apply action
        auto action = brain.produceAction();
        if (action) {
            simpleWorld.applyAction(*action);
        }
        
        // World steps
        simpleWorld.step(step);
        
        // Verify world state is valid
        assert(simpleWorld.getCurrentStep() >= step);
    }
    
    std::cout << "    testSimpleWorldFullCycle passed" << std::endl;
}

void testSimpleWorldStress() {
    nlm::SimpleWorld simpleWorld;
    
    // Stress test with rapid operations
    for (int i = 0; i < 100; ++i) {
        // Create brain
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Generate and process observations
        auto observation = simpleWorld.generateObservation(brain);
        
        // Step world
        simpleWorld.step(i);
        
        // Create and apply actions
        nlm::Action action;
        action.type = "test_action";
        action.parameters = std::to_string(i);
        action.timestamp = i;
        action.source = "test_system";
        
        simpleWorld.applyAction(action);
        
        // Verify world is still functional
        assert(simpleWorld.getCurrentStep() >= i);
    }
    
    std::cout << "    testSimpleWorldStress passed" << std::endl;
}

void testSimpleWorldConfig() {
    nlm::SimpleWorld simpleWorld;
    
    // Get and verify configuration
    auto config = simpleWorld.getConfig();
    assert(config != nullptr);
    
    // Config should have expected settings
    assert(config->has("simulation_step"));
    assert(config->has("world_width"));
    assert(config->has("world_height"));
    
    // Retrieve values
    auto step = config->getOr<int64_t>("simulation_step", 1000);
    auto width = config->getOr<int64_t>("world_width", 100);
    auto height = config->getOr<int64_t>("world_height", 100);
    
    assert(step > 0);
    assert(width > 0);
    assert(height > 0);
    
    std::cout << "    testSimpleWorldConfig passed" << std::endl;
}

void testSimpleWorldReset() {
    nlm::SimpleWorld simpleWorld;
    
    // Advance world state
    for (int i = 0; i < 10; ++i) {
        simpleWorld.step(i);
    }
    
    // Get current state
    nlm::Timestamp time = simpleWorld.getTime();
    nlm::SimulationStep step = simpleWorld.getCurrentStep();
    
    // Reset world
    simpleWorld.reset();
    
    // Verify reset worked
    assert(simpleWorld.getTime() == 0.0);
    assert(simpleWorld.getCurrentStep() == 0);
    
    std::cout << "    testSimpleWorldReset passed" << std::endl;
}

void runAll() {
    nlm_test::TestRunner runner(true);
    
    runner.runTest("Simple World Creation", testSimpleWorldCreation);
    runner.runTest("Simple World Step", testSimpleWorldStep);
    runner.runTest("Simple World Environment Integration", testSimpleWorldEnvironmentIntegration);
    runner.runTest("Simple World Observation", testSimpleWorldObservation);
    runner.runTest("Simple World Action", testSimpleWorldAction);
    runner.runTest("Simple World Vision Integration", testSimpleWorldVisionIntegration);
    runner.runTest("Simple World Full Cycle", testSimpleWorldFullCycle);
    runner.runTest("Simple World Stress Test", testSimpleWorldStress);
    runner.runTest("Simple World Config", testSimpleWorldConfig);
    runner.runTest("Simple World Reset", testSimpleWorldReset);
    
    runner.runAll();
}

} // namespace test_simple_world
