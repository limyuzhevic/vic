// AgentBrain Tests
#include "agent/AgentBrain.hpp"
#include "agent/SensoryPercept.hpp"
#include "brain/Brain.hpp"
#include "world/SimpleWorld.hpp"
#include "core/Config/Config.hpp"
#include "test_framework_utils.h"
#include <cassert>
#include <iostream>
#include <algorithm>

namespace test_agent_brain {

void testAgentBrainCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::AgentBrain agentBrain(&brain);
    
    assert(agentBrain.getBrain() != nullptr);
    assert(agentBrain.getSensoryInputSize() > 0);
    assert(agentBrain.getMotorOutputSize() > 0);
    
    std::cout << "    testAgentBrainCreation passed" << std::endl;
}

void testAgentBrainInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::AgentBrain agentBrain(&brain);
    
    // Create a simple world for initialization
    nlm::SimpleWorld world;
    
    agentBrain.initialize(world);
    
    assert(agentBrain.getSensoryInputSize() > 0);
    assert(agentBrain.getMotorOutputSize() > 0);
    
    std::cout << "    testAgentBrainInitialization passed" << std::endl;
}

void testAgentBrainProcessSensoryInput() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::AgentBrain agentBrain(&brain);
    
    nlm::SensoryPercept percept;
    
    // Set up vision input
    std::vector<float> vision(256, 0.5f); // 16x16 vision
    percept.setVision(vision);
    
    // Set up touch input
    std::vector<float> touch(8, 0.0f);
    percept.setTouch(touch);
    
    // Set up internal signals
    std::vector<float> internal(4, 0.0f);
    percept.setInternal(internal);
    
    // Set up proprioception
    std::vector<float> proprioception(6, 0.0f);
    percept.setProprioception(proprioception);
    
    // Process sensory input
    agentBrain.processSensoryInput(percept);
    
    // Should not crash and should process input
    assert(agentBrain.getBrain() != nullptr);
    
    std::cout << "    testAgentBrainProcessSensoryInput passed" << std::endl;
}

void testAgentBrainDecodeMotorCommand() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::AgentBrain agentBrain(&brain);
    
    // Decode motor command
    nlm::MotorCommand command = agentBrain.decodeMotorCommand();
    
    // Should return a valid motor command
    assert(command == nlm::MotorCommand::Wait ||
           command == nlm::MotorCommand::MoveForward ||
           command == nlm::MotorCommand::MoveBackward ||
           command == nlm::MotorCommand::TurnLeft ||
           command == nlm::MotorCommand::TurnRight ||
           command == nlm::MotorCommand::Interact);
    
    std::cout << "    testAgentBrainDecodeMotorCommand passed" << std::endl;
}

void testAgentBrainApplyRewardModulation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::AgentBrain agentBrain(&brain);
    
    // Apply reward modulation
    agentBrain.applyRewardModulation(1.0f, 0.5f); // reward, predicted_reward
    
    // Get neuromodulation level
    float modLevel = agentBrain.getNeuromodulationLevel();
    assert(modLevel >= -1.0f && modLevel <= 1.0f);
    
    std::cout << "    testAgentBrainApplyRewardModulation passed" << std::endl;
}

void testAgentBrainUpdateDevelopment() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::AgentBrain agentBrain(&brain);
    
    // Update development
    agentBrain.updateDevelopment(1.0);
    
    // Get developmental stage
    nlm::DevelopmentalStage stage = agentBrain.getDevelopmentalStage();
    assert(stage == nlm::DevelopmentalStage::Initial ||
           stage == nlm::DevelopmentalStage::CriticalPeriod ||
           stage == nlm::DevelopmentalStage::Maturation ||
           stage == nlm::DevelopmentalStage::Adult ||
           stage == nlm::DevelopmentalStage::Aging);
    
    std::cout << "    testAgentBrainUpdateDevelopment passed" << std::endl;
}

void testAgentBrainNeuromodulation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::AgentBrain agentBrain(&brain);
    
    // Test neuromodulation levels
    assert(agentBrain.getNeuromodulationLevel() == 0.0f);
    assert(agentBrain.getCuriosityLevel() == 0.0f);
    assert(agentBrain.getNoveltyLevel() == 0.0f);
    assert(agentBrain.getPredictionError() == 0.0f);
    
    std::cout << "    testAgentBrainNeuromodulation passed" << std::endl;
}

void testAgentBrainReset() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::AgentBrain agentBrain(&brain);
    
    // Set some neuromodulation levels
    // Note: In real implementation, these would be set by processing
    assert(agentBrain.getNeuromodulationLevel() == 0.0f);
    assert(agentBrain.getCuriosityLevel() == 0.0f);
    
    // Reset agent
    agentBrain.reset();
    
    // All levels should be reset to 0
    assert(agentBrain.getNeuromodulationLevel() == 0.0f);
    assert(agentBrain.getCuriosityLevel() == 0.0f);
    assert(agentBrain.getNoveltyLevel() == 0.0f);
    assert(agentBrain.getPredictionError() == 0.0f);
    
    std::cout << "    testAgentBrainReset passed" << std::endl;
}

void testAgentBrainConfiguration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::AgentBrain agentBrain(&brain);
    
    // Test configuration flags
    agentBrain.enableRewardModulation(true);
    agentBrain.enableStructuralPlasticity(true);
    agentBrain.enableDevelopment(true);
    agentBrain.enableCuriosity(true);
    
    assert(agentBrain.isRewardModulationEnabled() == true);
    assert(agentBrain.isStructuralPlasticityEnabled() == true);
    assert(agentBrain.isDevelopmentEnabled() == true);
    assert(agentBrain.isCuriosityEnabled() == true);
    
    std::cout << "    testAgentBrainConfiguration passed" << std::endl;
}

void runAll() {
    std::cout << "Running AgentBrain tests..." << std::endl;
    testAgentBrainCreation();
    testAgentBrainInitialization();
    testAgentBrainProcessSensoryInput();
    testAgentBrainDecodeMotorCommand();
    testAgentBrainApplyRewardModulation();
    testAgentBrainUpdateDevelopment();
    testAgentBrainNeuromodulation();
    testAgentBrainReset();
    testAgentBrainConfiguration();
}

} // namespace test_agent_brain
