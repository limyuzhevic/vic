// Enhanced AgentBrain Tests
// These tests cover the refactored AgentBrain with improved modularity and separation of concerns
#include "agent/AgentBrain.hpp"
#include "agent/SensoryPercept.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <cassert>
#include <iostream>
#include <memory>

namespace test_agentbrain {

void testAgentBrainCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agent(brain);
    
    // Verify basic properties
    assert(agent.getSensoryInputSize() > 0);
    assert(agent.getMotorOutputSize() > 0);
    
    std::cout << "    testAgentBrainCreation passed" << std::endl;
}

void testAgentBrainInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agent(brain);
    
    // Create a mock world for initialization (SimpleWorld)
    auto world = std::make_shared<nlm::SimpleWorld>();
    world->initialize(16, 16);
    
    // Initialize agent with world
    agent.initialize(*world);
    
    // Verify initialization worked
    assert(agent.getCuriosityLevel() >= 0.0f);
    assert(agent.getNoveltyLevel() >= 0.0f);
    assert(agent.getNeuromodulationLevel() >= -1.0f && 
           agent.getNeuromodulationLevel() <= 1.0f);
    
    std::cout << "    testAgentBrainInitialization passed" << std::endl;
}

void testAgentBrainSensoryProcessing() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agent(brain);
    
    // Create mock sensory percept
    nlm::SensoryPercept percept;
    
    // Set up some vision data
    std::vector<float> vision(16 * 16, 0.5f);
    for (size_t i = 0; i < std::min<size_t>(10, vision.size()); ++i) {
        vision[i] = 0.8f;
    }
    
    // Process sensory input (this should not crash)
    agent.processSensoryInput(percept);
    
    // Verify processing worked
    assert(agent.getNoveltyLevel() >= 0.0f);
    assert(agent.getCuriosityLevel() >= 0.0f);
    
    std::cout << "    testAgentBrainSensoryProcessing passed" << std::endl;
}

void testAgentBrainMotorDecoding() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agent(brain);
    
    // Get motor command (should return a valid command)
    auto motorCmd = agent.decodeMotorCommand();
    
    // Verify we got a valid motor command
    assert(motorCmd == nlm::MotorCommand::Wait || 
           motorCmd == nlm::MotorCommand::MoveForward ||
           motorCmd == nlm::MotorCommand::MoveBackward ||
           motorCmd == nlm::MotorCommand::TurnLeft ||
           motorCmd == nlm::MotorCommand::TurnRight ||
           motorCmd == nlm::MotorCommand::Interact ||
           motorCmd == nlm::MotorCommand::LookLeft ||
           motorCmd == nlm::MotorCommand::LookRight);
    
    std::cout << "    testAgentBrainMotorDecoding passed" << std::endl;
}

void testAgentBrainNeuromodulation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agent(brain);
    
    // Apply reward modulation
    agent.applyRewardModulation(1.0f, 0.8f);
    
    // Verify neuromodulation worked
    assert(agent.getPredictionError() >= -1.0f && 
           agent.getPredictionError() <= 1.0f);
    assert(agent.getNeuromodulationLevel() >= -1.0f && 
           agent.getNeuromodulationLevel() <= 1.0f);
    assert(agent.getExpectedReward() >= 0.0f);
    
    std::cout << "    testAgentBrainNeuromodulation passed" << std::endl;
}

void testAgentBrainDevelopment() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agent(brain);
    
    // Update development
    agent.updateDevelopment(1.0);
    
    // Verify development worked
    assert(agent.getDevelopmentalStage() == nlm::DevelopmentalStage::Initial ||
           agent.getDevelopmentalStage() == nlm::DevelopmentalStage::CriticalPeriod ||
           agent.getDevelopmentalStage() == nlm::DevelopmentalStage::Maturation ||
           agent.getDevelopmentalStage() == nlm::DevelopmentalStage::Adult ||
           agent.getDevelopmentalStage() == nlm::DevelopmentalStage::Aging);
    
    std::cout << "    testAgentBrainDevelopment passed" << std::endl;
}

void testAgentBrainReset() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agent(brain);
    
    // Apply some modifications
    agent.applyRewardModulation(1.0f, 0.8f);
    agent.updateDevelopment(1.0);
    
    // Reset agent
    agent.reset();
    
    // Verify reset worked
    assert(agent.getNeuromodulationLevel() == 0.0f);
    assert(agent.getNoveltyLevel() == 0.0f);
    assert(agent.getCuriosityLevel() == 0.0f);
    assert(agent.getPredictionError() == 0.0f);
    assert(agent.getExpectedReward() == 0.0f);
    assert(agent.getDevelopmentalStage() == nlm::DevelopmentalStage::Initial);
    
    std::cout << "    testAgentBrainReset passed" << std::endl;
}

void testAgentBrainSubsystems() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agent(brain);
    
    // Test that subsystems are accessible
    auto* sensoryProcessor = agent.getSensoryProcessor();
    auto* motorDecoder = agent.getMotorDecoder();
    auto* neuromodulationSystem = agent.getNeuromodulationSystem();
    auto* developmentSystem = agent.getDevelopmentSystem();
    auto* curiositySystem = agent.getCuriositySystem();
    
    assert(sensoryProcessor != nullptr);
    assert(motorDecoder != nullptr);
    assert(neuromodulationSystem != nullptr);
    assert(developmentSystem != nullptr);
    assert(curiositySystem != nullptr);
    
    // Test subsystem configuration
    nlm::SensoryProcessor::SensoryConfig sensoryConfig;
    sensoryConfig.visionScaleFactor = 2.0f;
    agent.setSensoryProcessorConfig(sensoryConfig);
    
    nlm::MotorDecoder::MotorDecodingConfig motorConfig;
    motorConfig.activityThreshold = 0.7f;
    agent.setMotorDecoderConfig(motorConfig);
    
    std::cout << "    testAgentBrainSubsystems passed" << std::endl;
}

void testAgentBrainConfiguration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agent(brain);
    
    // Test that agent can be configured through subsystems
    nlm::SensoryProcessor::SensoryConfig sensoryConfig;
    sensoryConfig.visionScaleFactor = 3.0f;
    sensoryConfig.touchScaleFactor = 4.0f;
    agent.setSensoryProcessorConfig(sensoryConfig);
    
    nlm::MotorDecoder::MotorDecodingConfig motorConfig;
    motorConfig.activityScale = 2.0f;
    motorConfig.enableCuriosityBasedExploration = false;
    agent.setMotorDecoderConfig(motorConfig);
    
    nlm::NeuromodulationSystem::NeuromodulationConfig neuromodConfig;
    neuromodConfig.predictionUpdateAlpha = 0.1f;
    agent.setNeuromodulationSystemConfig(neuromConfig);
    
    nlm::DevelopmentSystem::DevelopmentConfig devConfig;
    devConfig.plasticityInitial = 1.5f;
    agent.setDevelopmentSystemConfig(devConfig);
    
    nlm::CuriositySystem::CuriosityConfig curiosityConfig;
    curiosityConfig.noveltyWeight = 3.0f;
    agent.setCuriositySystemConfig(curiosityConfig);
    
    std::cout << "    testAgentBrainConfiguration passed" << std::endl;
}

void testAgentBrainErrorHandling() {
    // Test with null brain (should throw exception)
    try {
        nlm::AgentBrain agent(nullptr);
        assert(false && "Should have thrown exception for null brain");
    } catch (const std::exception& e) {
        assert(std::string(e.what()).find("cannot be null") != std::string::npos);
    }
    
    // Test with valid brain
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agent(brain);
    
    // Create mock world
    auto world = std::make_shared<nlm::SimpleWorld>();
    world->initialize(16, 16);
    
    // Initialize with world (should work)
    agent.initialize(*world);
    
    std::cout << "    testAgentBrainErrorHandling passed" << std::endl;
}

void runAll() {
    testAgentBrainCreation();
    testAgentBrainInitialization();
    testAgentBrainSensoryProcessing();
    testAgentBrainMotorDecoding();
    testAgentBrainNeuromodulation();
    testAgentBrainDevelopment();
    testAgentBrainReset();
    testAgentBrainSubsystems();
    testAgentBrainConfiguration();
    testAgentBrainErrorHandling();
    
    std::cout << "=== All AgentBrain Tests Passed ===" << std::endl;
}

} // namespace test_agentbrain
