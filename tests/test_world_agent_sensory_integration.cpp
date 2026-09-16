// World-Agent Sensory Integration Tests
// Tests sensory input processing and agent-world interaction

#include "agent/AgentBrain.hpp"
#include "agent/AgentBody.hpp"
#include "sensory/SensoryInput.hpp"
#include "world/SimpleWorld.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_world_agent_sensory_integration {

void testSensoryPerception() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    // Create simple world
    nlm::SimpleWorld world(20, 20);
    agentBrain.initialize(world);
    
    // Create sensory percept
    nlm::SensoryPercept percept;
    
    // Add vision input (16x16 grid)
    std::vector<float> vision(256);
    for (int i = 0; i < 256; ++i) {
        vision[i] = static_cast<float>(i % 10) / 10.0f;
    }
    percept.setVision(vision);
    
    // Add touch input
    std::vector<float> touch{0.8f, 0.3f, 0.5f};
    percept.setTouch(touch);
    
    // Add internal signals
    std::vector<float> internal{0.1f, -0.2f, 0.4f};
    percept.setInternal(internal);
    
    // Add proprioception
    std::vector<float> proprioception{0.0f, 0.1f, -0.1f, 0.2f};
    percept.setProprioception(proprioception);
    
    // Process sensory input
    agentBrain.processSensoryInput(percept);
    
    // Verify sensory input size
    size_t inputSize = agentBrain.getSensoryInputSize();
    assert(inputSize == 256 + 8 + 4 + 6); // Vision + touch + internal + proprioception
    
    std::cout << "    testSensoryPerception passed" << std::endl;
}

void testMotorDecoding() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    // Create simple world
    nlm::SimpleWorld world(20, 20);
    agentBrain.initialize(world);
    
    // Decode motor command
    nlm::MotorCommand command = agentBrain.decodeMotorCommand();
    
    // Verify command is valid
    assert(command != nlm::MotorCommand::LookLeft && command != nlm::MotorCommand::LookRight);
    
    // Test all possible motor commands
    std::vector<nlm::MotorCommand> validCommands = {
        nlm::MotorCommand::Wait,
        nlm::MotorCommand::MoveForward,
        nlm::MotorCommand::MoveBackward,
        nlm::MotorCommand::TurnLeft,
        nlm::MotorCommand::TurnRight,
        nlm::MotorCommand::Interact
    };
    
    assert(std::find(validCommands.begin(), validCommands.end(), command) != validCommands.end());
    
    std::cout << "    testMotorDecoding passed" << std::endl;
}

void testNeuromodulationIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(250), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("use_reward_modulation", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    // Create simple world
    nlm::SimpleWorld world(20, 20);
    agentBrain.initialize(world);
    
    // Apply reward modulation
    float reward = 1.0f;
    float predictedReward = 0.7f;
    agentBrain.applyRewardModulation(reward, predictedReward);
    
    // Verify neuromodulation state
    float neuromodulationLevel = agentBrain.getNeuromodulationLevel();
    assert(neuromodulationLevel >= -1.0f && neuromodulationLevel <= 1.0f);
    
    // Verify curiosity level is updated
    float curiosityLevel = agentBrain.getCuriosityLevel();
    assert(curiosityLevel >= 0.0f && curiosityLevel <= 1.0f);
    
    // Verify prediction error
    float predictionError = agentBrain.getPredictionError();
    assert(predictionError >= -1.0f && predictionError <= 1.0f);
    
    std::cout << "    testNeuromodulationIntegration passed" << std::endl;
}

void testAgentWorldLoop() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(400), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    // Create simple world
    nlm::SimpleWorld world(30, 30);
    agentBrain.initialize(world);
    
    // Simulate agent-world interaction loop
    for (int step = 0; step < 50; ++step) {
        // Get sensory input
        auto percept = world.getSensoryInput();
        
        // Process sensory input
        agentBrain.processSensoryInput(percept);
        
        // Generate motor command
        auto action = agentBrain.decodeMotorCommand();
        
        // Apply action to world
        world.applyAction(action);
        
        // Verify agent state
        assert(agentBrain.getBrain() != nullptr);
        assert(agentBrain.getBrain()->getTotalNeuronCount() == 400);
    }
    
    std::cout << "    testAgentWorldLoop passed" << std::endl;
}

void testDevelopmentIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(350), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("enable_development", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    // Create simple world
    nlm::SimpleWorld world(20, 20);
    agentBrain.initialize(world);
    
    // Update development system
    agentBrain.updateDevelopment(0.1f); // Update by 0.1 seconds
    
    // Verify developmental stage
    nlm::DevelopmentalStage stage = agentBrain.getDevelopmentalStage();
    assert(stage != nlm::DevelopmentalStage::Invalid);
    
    // Verify developmental age increases
    double age = agentBrain.getDevelopmentalStage() == nlm::DevelopmentalStage::Initial ? 0.0 : 1.0;
    assert(age >= 0.0);
    
    // Test reset functionality
    agentBrain.reset();
    
    std::cout << "    testDevelopmentIntegration passed" << std::endl;
}

void runAll() {
    testSensoryPerception();
    testMotorDecoding();
    testNeuromodulationIntegration();
    testAgentWorldLoop();
    testDevelopmentIntegration();
}

} // namespace test_world_agent_sensory_integration