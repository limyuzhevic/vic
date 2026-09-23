// AgentBrain Tests
#include "agent/AgentBrain.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "sensory/SensoryInput.hpp"
#include "agent/SensoryPercept.hpp"
#include "world/SimpleWorld.hpp"
#include <cassert>
#include <iostream>
#include <memory>

namespace test_agent_brain {

void testAgentBrainCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agentBrain(brain);
    
    assert(agentBrain.getBrain() == brain.get());
    assert(agentBrain.getSensoryInputSize() > 0);
    assert(agentBrain.getMotorOutputSize() > 0);
    
    std::cout << "    testAgentBrainCreation passed" << std::endl;
}

void testAgentBrainFactoryMethods() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    // Test factory methods
    auto agent1 = nlm::AgentBrain::createWithBrain(std::make_shared<nlm::Brain>(config));
    assert(agent1 != nullptr);
    
    auto agent2 = nlm::AgentBrain::createWithConfig(config);
    assert(agent2 != nullptr);
    
    auto agent3 = nlm::AgentBrain::createDefault();
    assert(agent3 != nullptr);
    
    // Test builder pattern
    auto agent4 = nlm::AgentBrain::createWithBrain(std::make_shared<nlm::Brain>(config));
    agent4->enableAllFeatures();
    assert(agent4->isRewardModulationEnabled());
    assert(agent4->isDevelopmentEnabled());
    assert(agent4->isCuriosityEnabled());
    
    std::cout << "    testAgentBrainFactoryMethods passed" << std::endl;
}

void testSensoryProcessing() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agentBrain(brain);
    
    // Create a simple world for testing
    nlm::SimpleWorld world;
    world.configure(10, 10, 16, 16); // width, height, visionWidth, visionHeight
    agentBrain.initialize(world);
    
    // Create sensory percept
    nlm::SensoryPercept percept;
    std::vector<float> vision(256, 0.5f); // 16x16 vision grid
    percept.setVision(vision);
    
    // Process sensory input
    agentBrain.processSensoryInput(percept);
    
    // Verify novelty was computed
    assert(agentBrain.getNoveltyLevel() >= 0.0f);
    assert(agentBrain.getNoveltyLevel() <= 1.0f);
    
    // Test curiosity update
    assert(agentBrain.getCuriosityLevel() >= 0.0f);
    assert(agentBrain.getCuriosityLevel() <= 1.0f);
    
    std::cout << "    testSensoryProcessing passed" << std::endl;
}

void testMotorDecoding() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agentBrain(brain);
    
    // Test motor command decoding
    nlm::MotorCommand cmd = agentBrain.decodeMotorCommand();
    assert(cmd == nlm::MotorCommand::Wait || cmd == nlm::MotorCommand::MoveForward || 
           cmd == nlm::MotorCommand::MoveBackward || cmd == nlm::MotorCommand::TurnLeft ||
           cmd == nlm::MotorCommand::TurnRight || cmd == nlm::MotorCommand::Interact);
    
    std::cout << "    testMotorDecoding passed" << std::endl;
}

void testNeuromodulation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agentBrain(brain);
    
    // Test reward modulation
    float initialNeuromodulation = agentBrain.getNeuromodulationLevel();
    float initialPredictionError = agentBrain.getPredictionError();
    
    agentBrain.applyRewardModulation(1.0f, 0.5f); // reward=1.0, predicted=0.5
    
    assert(agentBrain.getPredictionError() == 0.5f); // 1.0 - 0.5 = 0.5
    assert(agentBrain.getNeuromodulationLevel() == 0.5f); // prediction error = 0.5
    
    // Test with negative reward (punishment)
    agentBrain.applyRewardModulation(-1.0f, -0.5f); // reward=-1.0, predicted=-0.5
    
    assert(agentBrain.getPredictionError() == -0.5f); // -1.0 - (-0.5) = -0.5
    assert(agentBrain.getNeuromodulationLevel() == -0.5f); // prediction error = -0.5
    
    std::cout << "    testNeuromodulation passed" << std::endl;
}

void testDevelopment() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agentBrain(brain);
    
    // Test development stages
    DevelopmentalStage initialStage = agentBrain.getDevelopmentalStage();
    assert(initialStage == DevelopmentalStage::Initial || initialStage == DevelopmentalStage::CriticalPeriod ||
           initialStage == DevelopmentalStage::Maturation || initialStage == DevelopmentalStage::Adult);
    
    // Update development
    agentBrain.updateDevelopment(60.0); // 60 time units
    
    DevelopmentalStage stageAfterUpdate = agentBrain.getDevelopmentalStage();
    assert(stageAfterUpdate != initialStage); // Should have changed stage
    
    std::cout << "    testDevelopment passed" << std::endl;
}

void testConfiguration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agentBrain(brain);
    
    // Test configuration methods
    agentBrain.enableRewardModulation(false);
    assert(!agentBrain.isRewardModulationEnabled());
    
    agentBrain.enableDevelopment(false);
    assert(!agentBrain.isDevelopmentEnabled());
    
    agentBrain.enableCuriosity(false);
    assert(!agentBrain.isCuriosityEnabled());
    
    agentBrain.enableStructuralPlasticity(false);
    assert(!agentBrain.isStructuralPlasticityEnabled());
    
    // Test enabling all features
    agentBrain.enableAllFeatures();
    assert(agentBrain.isRewardModulationEnabled());
    assert(agentBrain.isDevelopmentEnabled());
    assert(agentBrain.isCuriosityEnabled());
    assert(agentBrain.isStructuralPlasticityEnabled());
    
    std::cout << "    testConfiguration passed" << std::endl;
}

void testContextManager() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    auto agentBrain = std::make_shared<nlm::AgentBrain>(brain);
    
    // Test context manager (should reset agent)
    {
        nlm::AgentBrain& agent = *agentBrain;
        agent.applyRewardModulation(1.0f, 0.0f);
        // State should be modified within the context
        float neuromodulation = agent.getNeuromodulationLevel();
        assert(neuromodulation > 0.0f);
    }  // Context exit - should reset
    
    // After context exit, agent should be reset
    assert(agentBrain->getNeuromodulationLevel() == 0.0f);
    assert(agentBrain->getCuriosityLevel() == 0.0f);
    assert(agentBrain->getPredictionError() == 0.0f);
    
    std::cout << "    testContextManager passed" << std::endl;
}

void testReset() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agentBrain(brain);
    
    // Modify state
    agentBrain.applyRewardModulation(1.0f, 0.5f);
    agentBrain.updateDevelopment(60.0);
    
    // Reset should clear all state
    agentBrain.reset();
    
    assert(agentBrain.getNeuromodulationLevel() == 0.0f);
    assert(agentBrain.getCuriosityLevel() == 0.0f);
    assert(agentBrain.getNoveltyLevel() == 0.0f);
    assert(agentBrain.getPredictionError() == 0.0f);
    assert(agentBrain.getExpectedReward() == 0.0f);
    assert(agentBrain.getDevelopmentalStage() == DevelopmentalStage::Initial);
    
    std::cout << "    testReset passed" << std::endl;
}

void testErrorHandling() {
    // Test with nullptr brain (should not crash)
    nlm::AgentBrain agentBrain(nullptr);
    
    // These methods should not crash when brain_ is nullptr
    agentBrain.processSensoryInput(nlm::SensoryPercept());
    nlm::MotorCommand cmd = agentBrain.decodeMotorCommand();
    agentBrain.applyRewardModulation(1.0f, 0.0f);
    agentBrain.updateDevelopment(1.0);
    
    assert(cmd == nlm::MotorCommand::Wait); // Default when brain is nullptr
    
    std::cout << "    testErrorHandling passed" << std::endl;
}

void runAll() {
    testAgentBrainCreation();
    testAgentBrainFactoryMethods();
    testSensoryProcessing();
    testMotorDecoding();
    testNeuromodulation();
    testDevelopment();
    testConfiguration();
    testContextManager();
    testReset();
    testErrorHandling();
}

} // namespace test_agent_brain