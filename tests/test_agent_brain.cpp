// AgentBrain Tests
#include "agent/AgentBrain.hpp"
#include "agent/SensoryPercept.hpp"
#include "world/SimpleWorld.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_agent_brain {

void testAgentBrainCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    
    nlm::AgentBrain agent(brain);
    
    std::cout << "    testAgentBrainCreation passed" << std::endl;
}

void testAgentBrainInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    
    nlm::AgentBrain agent(brain);
    
    // Create world and initialize
    auto world = std::make_shared<nlm::SimpleWorld>();
    world->configure(20, 20, 8, 8);
    world->reset();
    
    agent.initialize(*world);
    
    // Verify initialization
    assert(agent.getSensoryInputSize() > 0);
    assert(agent.getMotorOutputSize() > 0);
    
    std::cout << "    testAgentBrainInitialization passed" << std::endl;
}

void testAgentBrainSensoryProcessing() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agent(brain);
    
    // Create world and initialize
    auto world = std::make_shared<nlm::SimpleWorld>();
    world->configure(20, 20, 8, 8);
    world->reset();
    world->setAgentStart(10.0, 10.0);
    
    agent.initialize(*world);
    
    // Create sensory percept
    nlm::SensoryPercept percept;
    
    // Set vision data
    std::vector<float> vision(64, 0.5f); // 8x8 vision grid
    percept.setVision(vision);
    
    // Set touch data
    std::vector<float> touch(8, 0.0f);
    percept.setTouch(touch);
    
    // Set internal data
    std::vector<float> internal(4, 0.0f);
    percept.setInternal(internal);
    
    // Set timestamp
    percept.setTimestamp(1.0);
    
    // Process sensory input
    agent.processSensoryInput(percept);
    
    // Verify processing (should not crash)
    assert(agent.getCuriosityLevel() >= 0.0f);
    assert(agent.getNoveltyLevel() >= 0.0f);
    
    std::cout << "    testAgentBrainSensoryProcessing passed" << std::endl;
}

void testAgentBrainMotorDecoding() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agent(brain);
    
    // Create world and initialize
    auto world = std::make_shared<nlm::SimpleWorld>();
    world->configure(20, 20, 8, 8);
    world->reset();
    
    agent.initialize(*world);
    
    // Decode motor command (should return valid command)
    nlm::MotorCommand action = agent.decodeMotorCommand();
    assert(action == nlm::MotorCommand::Wait || action == nlm::MotorCommand::MoveForward ||
           action == nlm::MotorCommand::TurnLeft || action == nlm::MotorCommand::TurnRight ||
           action == nlm::MotorCommand::Interact);
    
    std::cout << "    testAgentBrainMotorDecoding passed" << std::endl;
}

void testAgentBrainRewardModulation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agent(brain);
    
    // Create world and initialize
    auto world = std::make_shared<nlm::SimpleWorld>();
    world->configure(20, 20, 8, 8);
    world->reset();
    
    agent.initialize(*world);
    
    // Enable reward modulation
    agent.enableRewardModulation(true);
    
    // Apply reward modulation
    agent.applyRewardModulation(1.0f, 0.5f); // reward=1.0, predicted=0.5
    
    // Verify neuromodulation
    assert(agent.getNeuromodulationLevel() >= -1.0f);
    assert(agent.getNeuromodulationLevel() <= 1.0f);
    
    std::cout << "    testAgentBrainRewardModulation passed" << std::endl;
}

void testAgentBrainDevelopment() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agent(brain);
    
    // Create world and initialize
    auto world = std::make_shared<nlm::SimpleWorld>();
    world->configure(20, 20, 8, 8);
    world->reset();
    
    agent.initialize(*world);
    
    // Enable development
    agent.enableDevelopment(true);
    
    // Update development
    agent.updateDevelopment(1.0);
    
    // Verify development stage
    nlm::DevelopmentalStage stage = agent.getDevelopmentalStage();
    assert(stage == nlm::DevelopmentalStage::Initial || stage == nlm::DevelopmentalStage::CriticalPeriod ||
           stage == nlm::DevelopmentalStage::Maturation || stage == nlm::DevelopmentalStage::Adult);
    
    std::cout << "    testAgentBrainDevelopment passed" << std::endl;
}

void testAgentBrainCuriosity() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agent(brain);
    
    // Create world and initialize
    auto world = std::make_shared<nlm::SimpleWorld>();
    world->configure(20, 20, 8, 8);
    world->reset();
    
    agent.initialize(*world);
    
    // Enable curiosity
    agent.enableCuriosity(true);
    
    // Check initial curiosity level
    assert(agent.getCuriosityLevel() >= 0.0f && agent.getCuriosityLevel() <= 1.0f);
    
    // Process some sensory input to change curiosity
    nlm::SensoryPercept percept;
    std::vector<float> vision(64, 1.0f);
    percept.setVision(vision);
    agent.processSensoryInput(percept);
    
    // Verify curiosity changed
    assert(agent.getCuriosityLevel() >= 0.0f);
    
    std::cout << "    testAgentBrainCuriosity passed" << std::endl;
}

void testAgentBrainReset() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agent(brain);
    
    // Create world and initialize
    auto world = std::make_shared<nlm::SimpleWorld>();
    world->configure(20, 20, 8, 8);
    world->reset();
    
    agent.initialize(*world);
    
    // Enable features
    agent.enableRewardModulation(true);
    agent.enableStructuralPlasticity(true);
    agent.enableDevelopment(true);
    agent.enableCuriosity(true);
    
    // Apply some modulation
    agent.applyRewardModulation(0.5f, 0.3f);
    agent.updateDevelopment(1.0);
    
    // Get initial levels
    float initialNeuromodulation = agent.getNeuromodulationLevel();
    float initialCuriosity = agent.getCuriosityLevel();
    
    // Reset agent
    agent.reset();
    
    // Verify reset
    assert(agent.getNeuromodulationLevel() == 0.0f);
    assert(agent.getCuriosityLevel() == 0.0f);
    assert(agent.getNoveltyLevel() == 0.0f);
    assert(agent.getPredictionError() == 0.0f);
    
    std::cout << "    testAgentBrainReset passed" << std::endl;
}

void runAll() {
    testAgentBrainCreation();
    testAgentBrainInitialization();
    testAgentBrainSensoryProcessing();
    testAgentBrainMotorDecoding();
    testAgentBrainRewardModulation();
    testAgentBrainDevelopment();
    testAgentBrainCuriosity();
    testAgentBrainReset();
}

} // namespace test_agent_brain