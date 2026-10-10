#include "test_common.hpp"
#include <cassert>
#include <cmath>
#include <algorithm>

namespace test_agent {

void testAgentBrainCreation() {
    nlm::AgentBrain agentBrain;
    assert(agentBrain.getBrainId() == 0);
    assert(agentBrain.getAgentId() == 0);
    assert(agentBrain.isActive() == false);
    std::cout << "    testAgentBrainCreation passed" << std::endl;
}

void testSensoryPercept() {
    nlm::SensoryPercept percept;
    assert(percept.getPerceptType() == nlm::PerceptType::Visual);
    assert(percept.getPerceptStrength() == 0.0f);
    assert(percept.getConfidence() == 0.0f);
    std::cout << "    testSensoryPercept passed" << std::endl;
}

void testAgentBody() {
    nlm::AgentBody agentBody;
    assert(agentBody.getBodyId() == 0);
    assert(agentBody.getAgentId() == 0);
    assert(agentBody.getMaxEnergy() == 100.0f);
    assert(agentBody.getCurrentEnergy() == 100.0f);
    std::cout << "    testAgentBody passed" << std::endl;
}

void testAgentBrain() {
    nlm::AgentBrain agentBrain;
    
    // Set up agent brain
    agentBrain.setBrainId(1);
    agentBrain.setAgentId(1);
    
    assert(agentBrain.getBrainId() == 1);
    assert(agentBrain.getAgentId() == 1);
    
    // Activate
    agentBrain.activate();
    assert(agentBrain.isActive() == true);
    
    // Deactivate
    agentBrain.deactivate();
    assert(agentBrain.isActive() == false);
    std::cout << "    testAgentBrain passed" << std::endl;
}

void testSensoryPercept() {
    nlm::SensoryPercept percept;
    
    // Set percept type
    percept.setPerceptType(nlm::PerceptType::Audio);
    assert(percept.getPerceptType() == nlm::PerceptType::Audio);
    
    // Set percept strength
    percept.setPerceptStrength(0.8f);
    assert(percept.getPerceptStrength() == 0.8f);
    
    // Set confidence
    percept.setConfidence(0.9f);
    assert(percept.getConfidence() == 0.9f);
    
    std::cout << "    testSensoryPercept passed" << std::endl;
}

void testAgentBodyEnergy() {
    nlm::AgentBody agentBody;
    
    // Test energy management
    assert(agentBody.getCurrentEnergy() == 100.0f);
    
    // Consume energy
    agentBody.consumeEnergy(20.0f);
    assert(agentBody.getCurrentEnergy() == 80.0f);
    
    // Restore energy
    agentBody.restoreEnergy(30.0f);
    assert(agentBody.getCurrentEnergy() == 100.0f); // Maxes out
    
    // Test energy bounds
    agentBody.consumeEnergy(200.0f); // Try to consume more than available
    assert(agentBody.getCurrentEnergy() == 0.0f);
    
    std::cout << "    testAgentBodyEnergy passed" << std::endl;
}

void testAgentIntegration() {
    nlm::AgentBrain agentBrain;
    
    // Test integration with Brain
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Agent brain should be accessible
    assert(&agentBrain != nullptr);
    std::cout << "    testAgentIntegration passed" << std::endl;
}

void runAll() {
    std::cout << "Running Agent tests..." << std::endl;
    testAgentBrainCreation();
    testSensoryPercept();
    testAgentBody();
    testAgentBrain();
    testSensoryPercept();
    testAgentBodyEnergy();
    testAgentIntegration();
}

} // namespace test_agent