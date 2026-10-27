// AgentBrain Tests
#include "agent/AgentBrain.hpp"
#include "agent/AgentBody.hpp"
#include "agent/SensoryPercept.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_agent_brain {

void testAgentBrainCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::AgentBrain agentBrain(std::shared_ptr<nlm::Brain>(brain.clone()));
    
    // Verify AgentBrain was created successfully
    assert(agentBrain.getSensoryInputSize() > 0);
    assert(agentBrain.getMotorOutputSize() > 0);
    
    std::cout << "    testAgentBrainCreation passed" << std::endl;
}

void testAgentBrainInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("vision_width", 16, nlm::ConfigSource::Default);
    config->set("vision_height", 16, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::AgentBrain agentBrain(std::shared_ptr<nlm::Brain>(brain.clone()));
    
    // Create a simple world for initialization
    nlm::SimpleWorld world(20, 20, 0.5f);
    
    agentBrain.initialize(world);
    
    // Verify initialization
    assert(agentBrain.getSensoryInputSize() > 0);
    assert(agentBrain.getMotorOutputSize() > 0);
    assert(agentBrain.getCuriosityLevel() >= 0.0f);
    assert(agentBrain.getNoveltyLevel() >= 0.0f);
    assert(agentBrain.getNeuromodulationLevel() >= -1.0f);
    
    std::cout << "    testAgentBrainInitialization passed" << std::endl;
}

void testAgentBrainSensoryProcessing() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::AgentBrain agentBrain(std::shared_ptr<nlm::Brain>(brain.clone()));
    
    // Create a simple world
    nlm::SimpleWorld world(20, 20, 0.5f);
    agentBrain.initialize(world);
    
    // Create sensory percept
    nlm::SensoryPercept percept;
    
    // Set some vision data
    std::vector<float> visionData(16 * 16, 0.5f);
    for (size_t i = 0; i < std::min(size_t(10), visionData.size()); ++i) {
        visionData[i] = 0.8f;
    }
    percept.setVision(visionData);
    
    // Set touch data
    std::vector<float> touchData(8, 0.0f);
    touchData[0] = 1.0f;
    percept.setTouch(touchData);
    
    // Set internal data
    std::vector<float> internalData(4, 0.0f);
    internalData[0] = 0.3f;
    percept.setInternal(internalData);
    
    // Set proprioception data
    std::vector<float> proprioceptionData(6, 0.0f);
    proprioceptionData[0] = 0.1f;
    proprioceptionData[1] = 0.2f;
    percept.setProprioception(proprioceptionData);
    
    // Set timestamp
    percept.setTimestamp(1.0);
    
    // Process sensory input
    agentBrain.processSensoryInput(percept);
    
    // Verify processing
    assert(agentBrain.getNoveltyLevel() >= 0.0f);
    assert(agentBrain.getCuriosityLevel() >= 0.0f);
    
    std::cout << "    testAgentBrainSensoryProcessing passed" << std::endl;
}

void testAgentBrainMotorDecoding() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::AgentBrain agentBrain(std::shared_ptr<nlm::Brain>(brain.clone()));
    
    // Initialize with world
    nlm::SimpleWorld world(20, 20, 0.5f);
    agentBrain.initialize(world);
    
    // Test motor command decoding (brain not fully initialized, but should not crash)
    nlm::MotorCommand command = agentBrain.decodeMotorCommand();
    
    // Verify command is valid
    assert(command == nlm::MotorCommand::Wait ||  // Wait is default if no activity
           command == nlm::MotorCommand::MoveForward ||
           command == nlm::MotorCommand::MoveBackward ||
           command == nlm::MotorCommand::TurnLeft ||
           command == nlm::MotorCommand::TurnRight ||
           command == nlm::MotorCommand::Interact);
    
    std::cout << "    testAgentBrainMotorDecoding passed" << std::endl;
}

void testAgentBrainNeuromodulation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::AgentBrain agentBrain(std::shared_ptr<nlm::Brain>(brain.clone()));
    
    // Initialize with world
    nlm::SimpleWorld world(20, 20, 0.5f);
    agentBrain.initialize(world);
    
    // Apply reward modulation
    agentBrain.applyRewardModulation(0.5f, 0.3f);
    
    // Verify neuromodulation effects
    assert(agentBrain.getPredictionError() >= -1.0f);  // -1 to 1 range
    assert(agentBrain.getNeuromodulationLevel() >= -1.0f);
    assert(agentBrain.getCuriosityLevel() >= 0.0f);
    
    std::cout << "    testAgentBrainNeuromodulation passed" << std::endl;
}

void testAgentBrainDevelopment() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::AgentBrain agentBrain(std::shared_ptr<nlm::Brain>(brain.clone()));
    
    // Initialize with world
    nlm::SimpleWorld world(20, 20, 0.5f);
    agentBrain.initialize(world);
    
    // Update development system
    double timestep = 1.0;
    agentBrain.updateDevelopment(timestep);
    
    // Verify developmental effects
    assert(agentBrain.getDevelopmentalStage() >= nlm::DevelopmentalStage::Initial);
    assert(agentBrain.getDevelopmentalStage() <= nlm::DevelopmentalStage::Adult);
    
    std::cout << "    testAgentBrainDevelopment passed" << std::endl;
}

void testAgentBrainReset() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::AgentBrain agentBrain(std::shared_ptr<nlm::Brain>(brain.clone()));
    
    // Initialize with world
    nlm::SimpleWorld world(20, 20, 0.5f);
    agentBrain.initialize(world);
    
    // Apply some neuromodulation
    agentBrain.applyRewardModulation(0.5f, 0.3f);
    
    // Reset agent
    agentBrain.reset();
    
    // Verify reset
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
    nlm::AgentBrain agentBrain(std::shared_ptr<nlm::Brain>(brain.clone()));
    
    // Test configuration setters
    agentBrain.enableRewardModulation(true);
    agentBrain.enableStructuralPlasticity(false);
    agentBrain.enableDevelopment(true);
    agentBrain.enableCuriosity(true);
    
    // Verify configuration
    assert(agentBrain.isRewardModulationEnabled() == true);
    assert(agentBrain.isStructuralPlasticityEnabled() == false);
    assert(agentBrain.isDevelopmentEnabled() == true);
    assert(agentBrain.isCuriosityEnabled() == true);
    
    std::cout << "    testAgentBrainConfiguration passed" << std::endl;
}

void runAll() {
    testAgentBrainCreation();
    testAgentBrainInitialization();
    testAgentBrainSensoryProcessing();
    testAgentBrainMotorDecoding();
    testAgentBrainNeuromodulation();
    testAgentBrainDevelopment();
    testAgentBrainReset();
    testAgentBrainConfiguration();
}

} // namespace test_agent_brain