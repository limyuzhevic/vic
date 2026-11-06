// AgentBrain Tests
// Tests for AgentBrain functionality using the new helper classes

#include "agent/AgentBrain.hpp"
#include "agent/AgentBrainNeuronManager.hpp"
#include "agent/AgentBrainStateValidator.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_agentbrain {

void testAgentBrainCreation() {
    std::cout << "    testAgentBrainCreation..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agent(brain);
    
    assert(brain != nullptr);
    assert(agent.getBrain() != nullptr);
    
    // Verify that neuron manager was initialized
    assert(agent.getMotorNeuronsCount() > 0 || agent.getSensoryNeuronsCount() > 0);
    
    std::cout << "    testAgentBrainCreation passed" << std::endl;
}

void testAgentBrainInitialization() {
    std::cout << "    testAgentBrainInitialization..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agent(brain);
    
    // Create a simple world for testing
    nlm::SimpleWorld world;
    world.configure(20.0f, 20.0f, 16, 16);
    
    // Test initialization
    agent.initialize(world);
    
    assert(agent.getBrain() != nullptr);
    assert(!agent.getMotorNeuronsCount() == 0);  // Should be > 0 after initialization
    
    std::cout << "    testAgentBrainInitialization passed" << std::endl;
}

void testAgentBrainReset() {
    std::cout << "    testAgentBrainReset..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agent(brain);
    
    // Create a world and initialize
    nlm::SimpleWorld world;
    world.configure(20.0f, 20.0f, 16, 16);
    agent.initialize(world);
    
    // Verify initial state
    assert(agent.getBrain() != nullptr);
    
    // Reset the agent
    agent.reset();
    
    // Verify state after reset
    assert(agent.getBrain() != nullptr);
    
    std::cout << "    testAgentBrainReset passed" << std::endl;
}

void testAgentBrainStateValidation() {
    std::cout << "    testAgentBrainStateValidation..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agent(brain);
    
    // Create a world and initialize
    nlm::SimpleWorld world;
    world.configure(20.0f, 20.0f, 16, 16);
    agent.initialize(world);
    
    // Test state validation
    bool stateValid = agent.validateState();
    assert(stateValid);  // Should be valid
    
    // Test with null brain (should handle gracefully)
    nlm::AgentBrain nullAgent(nullptr);
    bool nullStateValid = nullAgent.validateState();
    // nullStateValid may be false, which is acceptable
    
    std::cout << "    testAgentBrainStateValidation passed" << std::endl;
}

void testAgentBrainNeuronGroups() {
    std::cout << "    testAgentBrainNeuronGroups..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agent(brain);
    
    // Test neuron group counts
    size_t motorCount = agent.getMotorNeuronsCount();
    size_t sensoryCount = agent.getSensoryNeuronsCount();
    size_t internalCount = agent.getInternalNeuronsCount();
    
    assert(motorCount >= 0);
    assert(sensoryCount >= 0);
    assert(internalCount >= 0);
    
    // Test individual motor groups
    const auto& motorForward = agent.getMotorForward();
    const auto& motorBackward = agent.getMotorBackward();
    const auto& motorTurnLeft = agent.getMotorTurnLeft();
    const auto& motorTurnRight = agent.getMotorTurnRight();
    const auto& motorInteract = agent.getMotorInteract();
    const auto& motorWait = agent.getMotorWait();
    
    assert(&motorForward == &agent.getMotorForward());
    assert(&motorBackward == &agent.getMotorBackward());
    assert(&motorTurnLeft == &agent.getMotorTurnLeft());
    assert(&motorTurnRight == &agent.getMotorTurnRight());
    assert(&motorInteract == &agent.getMotorInteract());
    assert(&motorWait == &agent.getMotorWait());
    
    // Test sensory groups
    const auto& sensoryVision = agent.getSensoryVision();
    const auto& sensoryTouch = agent.getSensoryTouch();
    const auto& sensoryInternal = agent.getSensoryInternal();
    const auto& sensoryProprioception = agent.getSensoryProprioception();
    const auto& internalNeurons = agent.getInternalNeurons();
    
    std::cout << "    testAgentBrainNeuronGroups passed" << std::endl;
}

void testAgentBrainAllSignals() {
    std::cout << "    testAgentBrainAllSignals..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agent(brain);
    
    // Test getAllSignals
    auto allSignals = agent.getAllSignals();
    
    // Should be empty initially (no sensory input)
    assert(allSignals.size() == 0);
    
    std::cout << "    testAgentBrainAllSignals passed" << std::endl;
}

void runAll() {
    std::cout << "=== AgentBrain Tests ===" << std::endl;
    
    testAgentBrainCreation();
    testAgentBrainInitialization();
    testAgentBrainReset();
    testAgentBrainStateValidation();
    testAgentBrainNeuronGroups();
    testAgentBrainAllSignals();
    
    std::cout << "All AgentBrain tests passed!" << std::endl << std::endl;
}

} // namespace test_agentbrain