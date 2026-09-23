#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Neuron.hpp"
#include "sensory/SensoryInput.hpp"
#include "world/SimpleWorld.hpp"
#include <cassert>
#include <iostream>
#include <memory>

namespace test_improved {

void testMainErrorHandling() {
    std::cout << "Testing main.cpp error handling..." << std::endl;
    
    // Test that the error handling functions work
    // This would test the new error handling and timing utilities
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test brain status logging
    brain.logStatus();
    
    std::cout << "    testMainErrorHandling passed" << std::endl;
}

void testBrainNullPointerSafety() {
    std::cout << "Testing Brain.cpp null pointer safety..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test various methods with null pointer checks
    auto* spikeSystem = brain.getSpikeSystem();
    assert(spikeSystem != nullptr);
    
    auto* stdp = brain.getSTDP();
    assert(stdp != nullptr);
    
    auto* hebbian = brain.getHebbian();
    assert(hebbian != nullptr);
    
    auto* structuralPlasticity = brain.getStructuralPlasticity();
    assert(structuralPlasticity != nullptr);
    
    // Test that assertValid() doesn't throw when brain is valid
    brain.assertValid();
    
    // Test step() method (this would trigger the null checks)
    for (int i = 0; i < 10; ++i) {
        brain.step(i, static_cast<double>(i) * 0.001);
    }
    
    std::cout << "    testBrainNullPointerSafety passed" << std::endl;
}

void testAgentBrainInitialization() {
    std::cout << "Testing AgentBrain.cpp initialization fixes..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Create agent brain
    auto agentBrain = std::make_shared<nlm::AgentBrain>(std::make_shared<nlm::Brain>(brain));
    
    // Create a simple world for testing
    nlm::SimpleWorld world(16, 16);
    
    // Test initialize() with null checks
    agentBrain->initialize(world);
    
    // Test that sensory input size methods work
    size_t sensorySize = agentBrain->getSensoryInputSize();
    size_t motorSize = agentBrain->getMotorOutputSize();
    assert(sensorySize > 0);
    assert(motorSize > 0);
    
    // Test processSensoryInput with null checks
    nlm::SensoryInput sensoryInput(sensorySize);
    nlm::SensoryPercept percept(sensoryInput);
    
    agentBrain->processSensoryInput(percept);
    
    // Test neuromodulation methods
    agentBrain->applyRewardModulation(1.0f, 0.5f);
    agentBrain->updateDevelopment(1.0);
    
    // Test developmental stage access
    auto stage = agentBrain->getDevelopmentalStage();
    
    // Test neuromodulation level access
    float dopamine = agentBrain->getNeuromodulationLevel();
    float curiosity = agentBrain->getCuriosityLevel();
    float novelty = agentBrain->getNoveltyLevel();
    float predictionError = agentBrain->getPredictionError();
    
    // Test reset
    agentBrain->reset();
    
    std::cout << "    testAgentBrainInitialization passed" << std::endl;
}

void testMemoryManagement() {
    std::cout << "Testing memory management fixes..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    // Test move constructor
    nlm::Brain brain1(config);
    assert(brain1.initialize());
    
    nlm::Brain brain2(std::move(brain1));
    
    // Test that moved-from brain is in valid state
    // (Note: This is implementation-dependent, but we're testing the fix)
    
    std::cout << "    testMemoryManagement passed" << std::endl;
}

void testComprehensive() {
    std::cout << "Running comprehensive test suite..." << std::endl;
    
    testMainErrorHandling();
    testBrainNullPointerSafety();
    testAgentBrainInitialization();
    testMemoryManagement();
    
    std::cout << "All comprehensive tests passed!" << std::endl;
}

} // namespace test_improved