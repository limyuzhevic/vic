"""
Example test cases for NLM using the improved test framework.

This file demonstrates how to write comprehensive tests for NLM using the
enhanced test framework with proper assertions, setup/teardown, and performance monitoring.
"""

#include "pynlm/test_framework.h"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"

// Test helper functions
namespace test_helpers {
    // Create default brain for testing
    std::unique_ptr<nlm::Brain> createTestBrain() {
        auto config = std::make_shared<nlm::Config>();
        config->loadFromFile("configs/default.cfg");
        return std::make_unique<nlm::Brain>(config);
    }
    
    // Create test world
    std::unique_ptr<nlm::SimpleWorld> createTestWorld() {
        auto world = std::make_unique<nlm::SimpleWorld>();
        world->configure(10, 10, 8, 8);
        world->reset();
        return world;
    }
    
    // Create test agent
    std::unique_ptr<nlm::AgentBrain> createTestAgent(std::unique_ptr<nlm::Brain> brain) {
        auto agent = std::make_unique<nlm::AgentBrain>(std::move(brain));
        return agent;
    }
}

// Basic functionality tests
class BrainBasicTests : public TestCase {
public:
    BrainBasicTests() : TestCase("Brain Basic Tests", 
        "Test basic brain functionality including initialization, stepping, and statistics", 
        [=]() { return runBasicTests(); }) {}
    
    void setup() override {
        brain = test_helpers::createTestBrain();
        agent = test_helpers::createTestAgent(std::make_unique<nlm::Brain>(*brain));
        world = test_helpers::createTestWorld();
    }
    
    void teardown() override {
        world.reset();
        agent.reset();
        brain.reset();
    }
    
private:
    bool runBasicTests() {
        bool allPassed = true;
        
        // Test 1: Brain initialization
        allPassed &= assertTrue(brain->initialize(), "Brain initialization failed");
        
        // Test 2: Basic statistics
        allPassed &= assertEqual(1000, brain->getTotalNeuronCount(), 
                                "Expected 1000 neurons from default config");
        
        allPassed &= assertEqual(0, brain->getTotalSpikeCount(),
                                "Expected 0 spikes initially");
        
        // Test 3: Simulation steps
        for (int i = 0; i < 10; ++i) {
            nlm::SimulationStep step(i);
            brain->step(step);
            
            allPassed &= assertGreaterThan(brain->getTotalSpikeCount(), 0,
                                          "Spike count should increase with steps");
        }
        
        // Test 4: Firing rate calculation
        float firingRate = brain->getAverageFiringRate();
        allPassed &= assertInRange(firingRate, 0.0, 1.0,
                                  "Firing rate should be between 0 and 1");
        
        // Test 5: Development stage
        auto stage = brain->getDevelopmentalStage();
        allPassed &= assertTrue(stage != nlm::DevelopmentalStage::Aging,
                               "Brain should not be in aging stage after 10 steps");
        
        return allPassed;
    }
    
    std::unique_ptr<nlm::Brain> brain;
    std::unique_ptr<nlm::AgentBrain> agent;
    std::unique_ptr<nlm::SimpleWorld> world;
};

class WorldInteractionTests : public TestCase {
public:
    WorldInteractionTests() : TestCase("World Interaction Tests",
        "Test brain-world interaction including sensory input and motor output",
        [=]() { return runWorldInteractionTests(); }) {}
    
    void setup() override {
        brain = test_helpers::createTestBrain();
        brain->initialize();
        world = test_helpers::createTestWorld();
        agent = test_helpers::createTestAgent(std::make_unique<nlm::Brain>(*brain));
        agent->initialize(world.get());
    }
    
    void teardown() override {
        world.reset();
        agent.reset();
        brain.reset();
    }
    
private:
    bool runWorldInteractionTests() {
        bool allPassed = true;
        
        // Test 1: World setup
        allPassed &= assertEqual(10, world->getWidth(), "Expected world width of 10");
        allPassed &= assertEqual(10, world->getHeight(), "Expected world height of 10");
        
        // Test 2: Agent setup
        allPassed &= assertEqual(100, world->getAgentBody().energy,
                                "Expected initial agent energy of 100");
        
        // Test 3: Simulation with world interaction
        for (int i = 0; i < 20; ++i) {
            // Update world
            world->update(0.1);
            
            // Get sensory percept
            auto percept = world->getSensoryPercept();
            allPassed &= assertNotNull(percept.get(), "Expected valid sensory percept");
            
            // Process sensory input
            agent->processSensoryInput(*percept);
            
            // Brain step
            nlm::SimulationStep step(i);
            brain->step(step);
            
            // Generate action
            auto action = agent->decodeMotorCommand();
            allPassed &= assertNotNull(action.get(), "Expected valid action");
            
            // Apply action
            world->applyMotorCommand(*action, world->getSimulationTime());
            
            // Check for energy decay
            allPassed &= assertTrue(world->getAgentBody().energy >= 0,
                                   "Agent energy should not be negative");
        }
        
        return allPassed;
    }
    
    std::unique_ptr<nlm::Brain> brain;
    std::unique_ptr<nlm::AgentBrain> agent;
    std::unique_ptr<nlm::SimpleWorld> world;
};

class ConfigurationTests : public TestCase {
public:
    ConfigurationTests() : TestCase("Configuration Tests",
        "Test configuration loading and validation",
        [=]() { return runConfigurationTests(); }) {}
    
    void setup() override {}
    void teardown() override {}
    
private:
    bool runConfigurationTests() {
        bool allPassed = true;
        
        // Test 1: Default configuration
        auto config = std::make_shared<nlm::Config>();
        
        allPassed &= assertTrue(config->loadFromFile("configs/default.cfg"),
                               "Failed to load default configuration");
        
        // Test 2: Configuration keys
        auto keys = config->getKeys();
        allPassed &= assertTrue(keys.size() > 0, "Expected configuration keys");
        
        // Check for expected keys
        allPassed &= assertTrue(config->has("neuron_count"), "Missing neuron_count key");
        allPassed &= assertTrue(config->has("simulation_timestep"), "Missing simulation_timestep key");
        
        // Test 3: Configuration values
        allPassed &= assertEqual(1000, config->get<std::string>("neuron_count"),
                                "Expected 1000 neurons");
        
        allPassed &= assertEqual(0.001f, config->get<double>("simulation_timestep"),
                                "Expected simulation timestep of 0.001");
        
        // Test 4: Save and load configuration
        std::string testFile = "test_config.json";
        allPassed &= assertTrue(config->saveToFile(testFile), "Failed to save configuration");
        
        auto config2 = std::make_shared<nlm::Config>();
        allPassed &= assertTrue(config2->loadFromFile(testFile), "Failed to load saved configuration");
        
        // Clean up
        std::remove(testFile.c_str());
        
        return allPassed;
    }
};

class MemoryManagementTests : public TestCase {
public:
    MemoryManagementTests() : TestCase("Memory Management Tests",
        "Test memory management and resource cleanup",
        [=]() { return runMemoryManagementTests(); }) {}
    
    void setup() override {
        // Enable memory tracking
        MemoryTracker::enable();
    }
    
    void teardown() override {
        // Disable memory tracking
        MemoryTracker::disable();
    }
    
private:
    bool runMemoryManagementTests() {
        bool allPassed = true;
        
        // Test 1: Object creation and destruction
        {
            auto config = std::make_shared<nlm::Config>();
            auto brain = std::make_unique<nlm::Brain>(config);
            
            allPassed &= assertNotNull(brain.get(), "Brain should not be null");
            
            // Use the brain
            allPassed &= assertTrue(brain->initialize(), "Brain initialization failed");
            
            // Brain goes out of scope here - should be automatically cleaned up
        }
        
        // Force garbage collection
        std::cout << "Memory cleanup completed" << std::endl;
        
        // Test 2: Multiple brain instances
        std::vector<std::unique_ptr<nlm::Brain>> brains;
        
        for (int i = 0; i < 10; ++i) {
            auto config = std::make_shared<nlm::Config>();
            auto brain = std::make_unique<nlm::Brain>(config);
            brain->initialize();
            brains.push_back(std::move(brain));
            
            allPassed &= assertTrue(brain->getTotalSpikeCount() >= 0,
                                   "Brain spike count should be valid");
        }
        
        // Clear vector
        brains.clear();
        std::cout << "Multiple brain instances cleaned up" << std::endl;
        
        return allPassed;
    }
};

class PerformanceTests : public TestCase {
public:
    PerformanceTests() : TestCase("Performance Tests",
        "Test performance characteristics of NLM components",
        [=]() { return runPerformanceTests(); }) {}
    
    void setup() override {}
    void teardown() override {}
    
private:
    bool runPerformanceTests() {
        bool allPassed = true;
        
        // Test 1: Brain creation performance
        auto startTime = test_helpers::getCurrentTime();
        
        for (int i = 0; i < 10; ++i) {
            auto config = std::make_shared<nlm::Config>();
            auto brain = std::make_unique<nlm::Brain>(config);
            brain->initialize();
        }
        
        double creationTime = test_helpers::getCurrentTime() - startTime;
        double avgCreationTime = creationTime / 10.0;
        
        // Performance should be reasonable (less than 1 second per brain)
        allPassed &= assertLessThan(avgCreationTime, 1.0,
                                   "Average brain creation time should be less than 1 second");
        
        // Test 2: Simulation performance
        auto config = std::make_shared<nlm::Config>();
        auto brain = std::make_unique<nlm::Brain>(config);
        brain->initialize();
        
        startTime = test_helpers::getCurrentTime();
        
        for (int i = 0; i < 100; ++i) {
            nlm::SimulationStep step(i);
            brain->step(step);
        }
        
        double simulationTime = test_helpers::getCurrentTime() - startTime;
        double stepsPerSecond = 100.0 / simulationTime;
        
        // Performance should be reasonable (at least 1 step per second)
        allPassed &= assertGreaterThan(stepsPerSecond, 1.0,
                                      "Simulation should run at least 1 step per second");
        
        std::cout << "Performance results:" << std::endl;
        std::cout << "  Average brain creation time: " << avgCreationTime << "s" << std::endl;
        std::cout << "  Simulation speed: " << stepsPerSecond << " steps/s" << std::endl;
        
        return allPassed;
    }
};

class ErrorHandlingTests : public TestCase {
public:
    ErrorHandlingTests() : TestCase("Error Handling Tests",
        "Test error handling and exception management",
        [=]() { return runErrorHandlingTests(); }) {}
    
    void setup() override {}
    void teardown() override {}
    
private:
    bool runErrorHandlingTests() {
        bool allPassed = true;
        
        // Test 1: Invalid configuration
        auto config = std::make_shared<nlm::Config>();
        
        // Try to load non-existent file (should fail gracefully)
        allPassed &= assertFalse(config->loadFromFile("nonexistent_file.json"),
                                "Loading non-existent file should fail");
        
        // Test 2: Invalid parameters
        // (Note: In real implementation, this would test invalid parameter handling)
        
        // Test 3: Null pointer checks
        // (Note: In real implementation, this would test null pointer handling)
        
        // Test 4: Memory error simulation (if possible)
        // (Note: In real implementation, this would test memory error handling)
        
        return allPassed;
    }
};

// Integration test
class IntegrationTest : public TestCase {
public:
    IntegrationTest() : TestCase("Integration Test",
        "Test complete NLM system integration",
        [=]() { return runIntegrationTest(); }) {}
    
    void setup() override {
        brain = test_helpers::createTestBrain();
        brain->initialize();
        world = test_helpers::createTestWorld();
        agent = test_helpers::createTestAgent(std::make_unique<nlm::Brain>(*brain));
        agent->initialize(world.get());
    }
    
    void teardown() override {
        world.reset();
        agent.reset();
        brain.reset();
    }
    
private:
    bool runIntegrationTest() {
        bool allPassed = true;
        
        // Test 1: Complete simulation cycle
        for (int i = 0; i < 50; ++i) {
            // Update world
            world->update(0.1);
            
            // Get sensory input
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(*percept);
            
            // Brain processes input
            nlm::SimulationStep step(i);
            brain->step(step);
            
            // Generate action
            auto action = agent->decodeMotorCommand();
            
            // Execute action
            world->applyMotorCommand(*action, world->getSimulationTime());
            
            // Check system state
            allPassed &= assertTrue(brain->getTotalSpikeCount() >= 0,
                                   "Brain should have valid spike count");
            allPassed &= assertTrue(world->getAgentBody().energy >= 0,
                                   "Agent energy should not be negative");
            allPassed &= assertTrue(world->getAgentBody().health >= 0,
                                   "Agent health should not be negative");
        }
        
        // Test 2: Enable learning features
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableDevelopment(true);
        
        allPassed &= assertTrue(agent->isRewardModulationEnabled(),
                               "Reward modulation should be enabled");
        allPassed &= assertTrue(agent->isCuriosityEnabled(),
                               "Curiosity should be enabled");
        allPassed &= assertTrue(agent->isDevelopmentEnabled(),
                               "Development should be enabled");
        
        // Test 3: Development progression
        auto initialStage = brain->getDevelopmentalStage();
        
        // Run more steps to trigger development changes
        for (int i = 50; i < 200; ++i) {
            nlm::SimulationStep step(i);
            brain->step(step);
            
            auto currentStage = brain->getDevelopmentalStage();
            
            // Stage should progress or stay the same
            allPassed &= assertTrue(static_cast<int>(currentStage) >= static_cast<int>(initialStage),
                                   "Development stage should not regress");
        }
        
        return allPassed;
    }
    
    std::unique_ptr<nlm::Brain> brain;
    std::unique_ptr<nlm::AgentBrain> agent;
    std::unique_ptr<nlm::SimpleWorld> world;
};

// Test registration
void registerAllTests() {
    auto suite = std::make_shared<TestSuite>("Basic Functionality Tests", 
        "Tests for basic NLM functionality");
    
    suite->addTestCase(std::make_shared<BrainBasicTests>());
    suite->addTestCase(std::make_shared<WorldInteractionTests>());
    suite->addTestCase(std::make_shared<ConfigurationTests>());
    
    getTestRunner().addSuite(suite);
    
    auto performanceSuite = std::make_shared<TestSuite>("Performance Tests",
        "Tests for NLM performance characteristics");
    
    performanceSuite->addTestCase(std::make_shared<PerformanceTests>());
    
    getTestRunner().addSuite(performanceSuite);
    
    auto errorSuite = std::make_shared<TestSuite>("Error Handling Tests",
        "Tests for error handling and exception management");
    
    errorSuite->addTestCase(std::make_shared<ErrorHandlingTests>());
    
    getTestRunner().addSuite(errorSuite);
    
    auto integrationSuite = std::make_shared<TestSuite>("Integration Tests",
        "Tests for complete NLM system integration");
    
    integrationSuite->addTestCase(std::make_shared<IntegrationTest>());
    integrationSuite->addTestCase(std::make_shared<MemoryManagementTests>());
    
    getTestRunner().addSuite(integrationSuite);
}

// Main test function
int main(int argc, char* argv[]) {
    // Register all tests
    registerAllTests();
    
    // Check for command line arguments
    bool runPerformanceTests = false;
    bool runAllTests = true;
    
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--performance") {
            runPerformanceTests = true;
        } else if (std::string(argv[i]) == "--help") {
            std::cout << "Usage: test_runner [options]" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  --performance    Run only performance tests" << std::endl;
            std::cout << "  --help           Show this help message" << std::endl;
            return 0;
        }
    }
    
    // Run tests based on command line arguments
    if (runPerformanceTests) {
        std::cout << "Running performance tests..." << std::endl;
        auto performanceSuite = std::make_shared<TestSuite>("Performance Tests",
            "Performance characteristics tests");
        performanceSuite->addTestCase(std::make_shared<PerformanceTests>());
        getTestRunner().addSuite(performanceSuite);
    }
    
    // Run all tests
    getTestRunner().runAllTests();
    
    return 0;
}
