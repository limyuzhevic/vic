// AgentBrain Tests
#include "agent/AgentBrain.hpp"
#include "brain/Brain.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/SensoryPercept.hpp"
#include "agent/AgentBody.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_agentbrain {

void testAgentBrainCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    nlm::Brain brain(config);
    
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    std::cout << "    testAgentBrainCreation passed" << std::endl;
}

void testAgentBrainInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    nlm::Brain brain(config);
    
    nlm::SimpleWorld world;
    world.configure(100.0f, 100.0f, 16, 16);
    
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    agentBrain.initialize(world);
    
    assert(agentBrain.getBrain() != nullptr);
    assert(agentBrain.getSensoryInputSize() > 0);
    assert(agentBrain.getMotorOutputSize() > 0);
    
    std::cout << "    testAgentBrainInitialization passed" << std::endl;
}

void testAgentBrainSensoryProcessing() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    nlm::Brain brain(config);
    nlm::SimpleWorld world;
    world.configure(100.0f, 100.0f, 16, 16);
    
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    agentBrain.initialize(world);
    
    // Create a sensory percept
    nlm::SensoryPercept percept(16, 16);
    percept.setVision(std::vector<float>(256, 0.5f));
    percept.setTouch(std::vector<float>(8, 0.0f));
    percept.setInternal(std::vector<float>(4, 0.0f));
    percept.setProprioception(std::vector<float>(6, 0.0f));
    
    agentBrain.processSensoryInput(percept);
    
    // Should not crash and should process input
    assert(agentBrain.getNoveltyLevel() >= 0.0f);
    
    std::cout << "    testAgentBrainSensoryProcessing passed" << std::endl;
}

void testAgentBrainMotorDecoding() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    nlm::Brain brain(config);
    nlm::SimpleWorld world;
    world.configure(100.0f, 100.0f, 16, 16);
    
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    agentBrain.initialize(world);
    
    auto action = agentBrain.decodeMotorCommand();
    
    // Should return a valid action
    assert(action == nlm::MotorCommand::Wait || 
           action == nlm::MotorCommand::MoveForward ||
           action == nlm::MotorCommand::MoveBackward ||
           action == nlm::MotorCommand::TurnLeft ||
           action == nlm::MotorCommand::TurnRight ||
           action == nlm::MotorCommand::Interact);
    
    std::cout << "    testAgentBrainMotorDecoding passed" << std::endl;
}

void testAgentBrainNeuromodulation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    nlm::Brain brain(config);
    nlm::SimpleWorld world;
    world.configure(100.0f, 100.0f, 16, 16);
    
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    agentBrain.initialize(world);
    
    // Apply reward modulation
    agentBrain.applyRewardModulation(1.0f, 0.5f);
    
    assert(agentBrain.getPredictionError() == 0.5f);
    assert(agentBrain.getNeuromodulationLevel() == 0.5f);
    
    // Apply negative reward
    agentBrain.applyRewardModulation(-1.0f, 0.2f);
    
    assert(agentBrain.getPredictionError() == -1.2f);
    assert(agentBrain.getNeuromodulationLevel() == -1.2f);
    
    std::cout << "    testAgentBrainNeuromodulation passed" << std::endl;
}

void testAgentBrainDevelopment() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    nlm::Brain brain(config);
    nlm::SimpleWorld world;
    world.configure(100.0f, 100.0f, 16, 16);
    
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    agentBrain.initialize(world);
    
    DevelopmentalStage initialStage = agentBrain.getDevelopmentalStage();
    
    // Update development
    agentBrain.updateDevelopment(10.0);
    
    // Stage may have advanced or not, but should not crash
    assert(agentBrain.getDevelopmentalStage() == DevelopmentalStage::Initial || 
           agentBrain.getDevelopmentalStage() == DevelopmentalStage::CriticalPeriod);
    
    std::cout << "    testAgentBrainDevelopment passed" << std::endl;
}

void testAgentBrainReset() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    nlm::Brain brain(config);
    nlm::SimpleWorld world;
    world.configure(100.0f, 100.0f, 16, 16);
    
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    agentBrain.initialize(world);
    
    // Apply some neuromodulation
    agentBrain.applyRewardModulation(1.0f, 0.0f);
    agentBrain.updateDevelopment(10.0);
    
    // Reset should clear all state
    agentBrain.reset();
    
    assert(agentBrain.getNeuromodulationLevel() == 0.0f);
    assert(agentBrain.getCuriosityLevel() == 0.0f);
    assert(agentBrain.getNoveltyLevel() == 0.0f);
    assert(agentBrain.getPredictionError() == 0.0f);
    assert(agentBrain.getDevelopmentalStage() == DevelopmentalStage::Initial);
    
    std::cout << "    testAgentBrainReset passed" << std::endl;
}

void runAll() {
    std::cout << "Running AgentBrain tests..." << std::endl;
    testAgentBrainCreation();
    testAgentBrainInitialization();
    testAgentBrainSensoryProcessing();
    testAgentBrainMotorDecoding();
    testAgentBrainNeuromodulation();
    testAgentBrainDevelopment();
    testAgentBrainReset();
}

} // namespace test_agentbrain
