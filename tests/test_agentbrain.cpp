// AgentBrain Tests
// Tests for the agent interface connecting brain to world

#include "agent/AgentBrain.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "world/SimpleWorld.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_agentbrain {

void testAgentBrainCreation() {
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    auto agentBrain = std::make_shared<nlm::AgentBrain>(brain);
    
    assert(agentBrain != nullptr);
    assert(agentBrain->getBrain() == brain.get());
    
    std::cout << "    testAgentBrainCreation passed" << std::endl;
}

void testAgentBrainInitialize() {
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    auto agentBrain = std::make_shared<nlm::AgentBrain>(brain);
    
    nlm::SimpleWorld world;
    world.configure(10, 10, 8, 8);
    
    agentBrain->initialize(world);
    
    // Should have set up sensory neuron groups
    assert(agentBrain->getSensoryInputSize() == 256 + 8 + 4 + 6);
    assert(agentBrain->getMotorOutputSize() == 6);
    
    std::cout << "    testAgentBrainInitialize passed" << std::endl;
}

void testAgentBrainReset() {
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    auto agentBrain = std::make_shared<nlm::AgentBrain>(brain);
    
    // Set some initial state
    agentBrain->enableRewardModulation(true);
    agentBrain->enableCuriosity(true);
    
    // Reset should clear state
    agentBrain->reset();
    
    assert(!agentBrain->isRewardModulationEnabled());  // Should reset to defaults
    assert(agentBrain->isCuriosityEnabled());  // This is always enabled by default
    
    std::cout << "    testAgentBrainReset passed" << std::endl;
}

void testAgentBrainSensoryProcessing() {
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    auto agentBrain = std::make_shared<nlm::AgentBrain>(brain);
    
    nlm::SimpleWorld world;
    world.configure(10, 10, 8, 8);
    agentBrain->initialize(world);
    
    // Create a simple vision percept
    nlm::Vision vision(8, 8, 3);
    for (size_t i = 0; i < 8 * 8 * 3; ++i) {
        vision.setData(i, 0.5f);
    }
    
    nlm::SensoryPercept percept;
    percept.setVision(vision);
    
    // This should process without crashing
    agentBrain->processSensoryInput(percept);
    
    std::cout << "    testAgentBrainSensoryProcessing passed" << std::endl;
}

void testAgentBrainRewardModulation() {
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    auto agentBrain = std::make_shared<nlm::AgentBrain>(brain);
    
    // Test basic reward modulation
    agentBrain->applyRewardModulation(1.0f, 0.5f);  // Positive prediction error
    assert(agentBrain->getNeuromodulationLevel() > 0.0f);
    
    agentBrain->applyRewardModulation(-1.0f, 0.5f);  // Negative prediction error
    assert(agentBrain->getNeuromodulationLevel() < 0.0f);
    
    std::cout << "    testAgentBrainRewardModulation passed" << std::endl;
}

void testAgentBrainConfiguration() {
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    auto agentBrain = std::make_shared<nlm::AgentBrain>(brain);
    
    // Test configuration flags
    agentBrain->enableRewardModulation(true);
    agentBrain->enableStructuralPlasticity(true);
    agentBrain->enableDevelopment(true);
    agentBrain->enableCuriosity(true);
    
    assert(agentBrain->isRewardModulationEnabled());
    assert(agentBrain->isStructuralPlasticityEnabled());
    assert(agentBrain->isDevelopmentEnabled());
    assert(agentBrain->isCuriosityEnabled());
    
    std::cout << "    testAgentBrainConfiguration passed" << std::endl;
}

void testAgentBrainDevelopment() {
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    auto agentBrain = std::make_shared<nlm::AgentBrain>(brain);
    
    nlm::SimpleWorld world;
    world.configure(10, 10, 8, 8);
    agentBrain->initialize(world);
    
    // Test development state
    nlm::DevelopmentalStage stage = agentBrain->getDevelopmentalStage();
    assert(stage == nlm::DevelopmentalStage::Initial);
    
    // Update development
    agentBrain->updateDevelopment(0.1);  // 100ms
    
    stage = agentBrain->getDevelopmentalStage();
    // Should now be in critical period or maturation
    assert(stage != nlm::DevelopmentalStage::Initial);
    
    std::cout << "    testAgentBrainDevelopment passed" << std::endl;
}

void runAll() {
    std::cout << "Running AgentBrain tests..." << std::endl;
    testAgentBrainCreation();
    testAgentBrainInitialize();
    testAgentBrainReset();
    testAgentBrainSensoryProcessing();
    testAgentBrainRewardModulation();
    testAgentBrainConfiguration();
    testAgentBrainDevelopment();
}

} // namespace test_agentbrain
