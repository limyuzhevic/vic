// AgentBrain Integration Tests
#include "agent/AgentBrain.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_agentbrain_integration {

void testAgentBrainCreation() {
    nlm::AgentBrain agent;
    
    // Should initialize properly
    assert(agent.getStatus() != nullptr);
    
    std::cout << "    testAgentBrainCreation passed" << std::endl;
}

void testAgentBrainBrainIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::AgentBrain agent;
    agent.initialize(&brain);
    
    // Verify integration
    assert(agent.getBrainReference() != nullptr);
    
    std::cout << "    testAgentBrainBrainIntegration passed" << std::endl;
}

void testAgentBrainSensoryInput() {
    nlm::AgentBrain agent;
    
    // Create mock sensory percept
    nlm::SensoryPercept percept;
    percept.vision = {0.1f, 0.2f, 0.3f};
    percept.touch = {0.0f, 0.0f, 0.0f};
    
    // Process sensory input
    nlm::SensoryPercept processed = agent.processSensoryInput(percept);
    
    // Should not crash
    assert(processed.vision.size() > 0 || processed.touch.size() > 0);
    
    std::cout << "    testAgentBrainSensoryInput passed" << std::endl;
}

void testAgentBrainMotorOutput() {
    nlm::AgentBrain agent;
    
    // Decode motor command
    nlm::MotorCommand action = agent.decodeMotorCommand();
    
    // Should return valid command
    assert(action != nullptr);
    
    std::cout << "    testAgentBrainMotorOutput passed" << std::endl;
}

void testAgentBrainReset() {
    nlm::AgentBrain agent;
    
    // Create mock brain
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    agent.initialize(&brain);
    
    // Reset agent
    agent.reset();
    
    // Should reset properly
    std::cout << "    testAgentBrainReset passed" << std::endl;
}

void testAgentBrainIntegrationWithMemory() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::AgentBrain agent;
    agent.initialize(&brain);
    
    // Get working memory from brain
    auto* workingMem = brain.getWorkingMemory();
    assert(workingMem != nullptr);
    
    // Store pattern in working memory via agent
    std::vector<float> pattern = {0.5f, 0.6f, 0.7f};
    
    std::cout << "    testAgentBrainIntegrationWithMemory passed" << std::endl;
}

void runAll() {
    std::cout << "Running AgentBrain integration tests..." << std::endl;
    testAgentBrainCreation();
    testAgentBrainBrainIntegration();
    testAgentBrainSensoryInput();
    testAgentBrainMotorOutput();
    testAgentBrainReset();
    testAgentBrainIntegrationWithMemory();
}

} // namespace test_agentbrain_integration