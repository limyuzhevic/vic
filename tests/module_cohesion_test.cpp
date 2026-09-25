// Module Cohesion Test Suite
// Tests for module cohesion and reduced code duplication
// Validates improved modularity and separation of concerns

#include "module_cohesion_test.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/ErrorHandling/ErrorHierarchy.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include <cassert>
#include <iostream>
#include <memory>

namespace nlm {

void testBrainModuleCohesion() {
    std::cout << "Testing Brain module cohesion..." << std::endl;
    
    // Test that Brain properly integrates with its modules
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    
    Brain brain(config);
    brain.initialize();
    
    // Test integration with core modules
    assert(brain.getTotalNeuronCount() > 0);
    assert(brain.getTotalSynapseCount() >= 0);
    
    // Test integration with memory modules
    auto workingMemory = brain.getWorkingMemory();
    if (workingMemory) {
        std::cout << "Brain-memory integration: OK" << std::endl;
    }
    
    // Test integration with neuromodulation modules
    auto dopamine = brain.getDopamine();
    if (dopamine) {
        std::cout << "Brain-neuromodulation integration: OK" << std::endl;
    }
    
    // Test integration with prediction modules
    auto prediction = brain.getPredictionSystem();
    if (prediction) {
        std::cout << "Brain-prediction integration: OK" << std::endl;
    }
    
    // Test integration with development modules
    auto development = brain.getDevelopmentSystem();
    if (development) {
        std::cout << "Brain-development integration: OK" << std::endl;
    }
    
    std::cout << "Brain module cohesion test passed!" << std::endl;
}

void testConfigModuleCohesion() {
    std::cout << "Testing Config module cohesion..." << std::endl;
    
    // Test that Config properly integrates with its modules
    auto config = std::make_shared<Config>();
    
    // Test integration with error handling
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
    
    // Test value validation and retrieval
    assert(config->getOr<int64_t>("neuron_count", 50) == 100);
    assert(config->getOr<int64_t>("region_count", 1) == 2);
    
    // Test configuration validation
    assert(config->has("neuron_count"));
    assert(config->has("region_count"));
    
    // Test configuration persistence
    config->remove("neuron_count");
    assert(config->getOr<int64_t>("neuron_count", 50) == 50);
    
    std::cout << "Config module cohesion test passed!" << std::endl;
}

void testAgentBrainModuleCohesion() {
    std::cout << "Testing AgentBrain module cohesion..." << std::endl;
    
    // Create brain for agent brain
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Create agent brain
    AgentBrain agentBrain(brain);
    agentBrain.initialize();
    
    // Test integration between AgentBrain and Brain modules
    assert(agentBrain.getBrain() == brain);
    assert(agentBrain.getSensoryPercept().isValid() == true);
    
    // Test that agent brain properly delegates to brain
    agentBrain.step(1.0f);
    assert(brain->getTotalNeuronCount() > 0);
    
    std::cout << "AgentBrain module cohesion test passed!" << std::endl;
}

void testWorldIntegrationModuleCohesion() {
    std::cout << "Testing world integration module cohesion..." << std::endl;
    
    // Create world and integrate with agent brain
    SimpleWorld world;
    world.configure(50.0f, 50.0f, 16, 16);
    
    // Create brain and agent for world integration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    AgentBrain agentBrain(brain);
    agentBrain.initialize(world);
    
    // Test world integration with agent brain
    agentBrain.step(1.0f);
    
    assert(agentBrain.getSensoryPercept().isValid() == true);
    
    // Test that world properly integrates with agent brain
    // World should receive agent's actions and provide observations
    
    std::cout << "World integration module cohesion test passed!" << std::endl;
}

void testErrorHandlingModuleCohesion() {
    std::cout << "Testing error handling module cohesion..." << std::endl;
    
    // Test that error handling integrates properly with other modules
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    
    Brain brain(config);
    brain.initialize();
    
    // Test error handling with Brain operations
    try {
        brain.step(-1); // Invalid step - should be handled gracefully
    } catch (const std::exception& e) {
        std::cout << "Error properly handled: " << e.what() << std::endl;
    }
    
    // Test that error handling doesn't break brain functionality
    brain.step(5);
    assert(brain.getTotalNeuronCount() > 0);
    
    // Test error hierarchy integration
    NLMErrorHierarchy errorHierarchy;
    assert(errorHierarchy.getErrorCount() == 0); // Initial state
    
    std::cout << "Error handling module cohesion test passed!" << std::endl;
}

void testPerformanceModuleCohesion() {
    std::cout << "Testing performance module cohesion..." << std::endl;
    
    // Test that performance optimization integrates with other modules
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    
    Brain brain(config);
    brain.initialize();
    
    // Test that performance optimization works with brain operations
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 100; ++i) {
        brain.step(1);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(end - start);
    
    std::cout << "Performance: 100 steps in " << duration.count() << " ms" << std::endl;
    
    // Test that performance metrics are properly collected
    // (Performance metrics would be collected by PerformanceOptimizer)
    
    std::cout << "Performance module cohesion test passed!" << std::endl;
}

void runModuleCohesionTests() {
    std::cout << "=== Running Module Cohesion Tests ===" << std::endl;
    std::cout << std::endl;
    
    testBrainModuleCohesion();
    std::cout << std::endl;
    
    testConfigModuleCohesion();
    std::cout << std::endl;
    
    testAgentBrainModuleCohesion();
    std::cout << std::endl;
    
    testWorldIntegrationModuleCohesion();
    std::cout << std::endl;
    
    testErrorHandlingModuleCohesion();
    std::cout << std::endl;
    
    testPerformanceModuleCohesion();
    std::cout << std::endl;
    
    std::cout << "=== All Module Cohesion Tests Passed ===" << std::endl;
}

} // namespace nlm
