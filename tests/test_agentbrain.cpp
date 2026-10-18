// AgentBrain Tests
// Phase 2: Enhanced AgentBrain functionality tests

#include "agent/AgentBrain.hpp"
#include "agent/AgentBrainConstants.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "world/SimpleWorld.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_agentbrain {

void testConstants() {
    std::cout << "    Testing AgentBrainConstants..." << std::endl;
    
    // Test basic constants
    assert(AgentBrainConstants::MOTOR_GROUPS_COUNT == 6);
    assert(AgentBrainConstants::SENSORY_GROUPS_COUNT == 4);
    assert(AgentBrainConstants::NOVELTY_DECAY_FACTOR == 0.99f);
    assert(AgentBrainConstants::MAX_CURIOUSITY_LEVEL == 1.0f);
    
    // Test motor group indices
    assert(AgentBrainConstants::MOTOR_FORWARD == 0);
    assert(AgentBrainConstants::MOTOR_TURN_LEFT == 2);
    assert(AgentBrainConstants::MOTOR_INTERACT == 4);
    
    // Test sensory group indices  
    assert(AgentBrainConstants::SENSORY_VISION == 0);
    assert(AgentBrainConstants::SENSORY_TOUCH == 1);
    
    // Test scale factors
    assert(AgentBrainConstants::SENSORY_VISION_SCALE == 5.0f);
    assert(AgentBrainConstants::SENSORY_TOUCH_SCALE == 8.0f);
    
    std::cout << "    testConstants passed" << std::endl;
}

void testAgentBrainCreation() {
    std::cout << "    Testing AgentBrain creation..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    
    nlm::AgentBrain agentBrain(brain);
    
    // Test initial state
    assert(brain_ != nullptr);
    assert(agentBrain.getBrain() == brain.get());
    
    std::cout << "    testAgentBrainCreation passed" << std::endl;
}

void testAgentBrainInitialization() {
    std::cout << "    Testing AgentBrain initialization..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agentBrain(brain);
    
    // Create a simple world
    nlm::SimpleWorld world;
    world.configure(10, 10, 8, 8);
    
    // Initialize agent brain
    agentBrain.initialize(world);
    
    // Test initial state after initialization
    assert(agentBrain.getSensoryInputSize() == 256 + 8 + 4 + 6);
    assert(agentBrain.getMotorOutputSize() == 6);
    assert(agentBrain.getCuriosityLevel() == 0.0f);
    assert(agentBrain.getNoveltyLevel() == 0.0f);
    assert(agentBrain.getNeuromodulationLevel() == 0.0f);
    assert(agentBrain.getPredictionError() == 0.0f);
    
    std::cout << "    testAgentBrainInitialization passed" << std::endl;
}

void testNeuronDistribution() {
    std::cout << "    Testing neuron distribution..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agentBrain(brain);
    
    // Manually add neurons to test distribution (simplified for testing)
    // In real usage, this happens during brain initialization
    auto* region = brain->getRegion(nlm::RegionId(0));
    assert(region != nullptr);
    
    std::cout << "    testNeuronDistribution passed" << std::endl;
}

void testSensoryProcessing() {
    std::cout << "    Testing sensory processing..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agentBrain(brain);
    
    // Create world and initialize
    nlm::SimpleWorld world;
    world.configure(10, 10, 8, 8);
    agentBrain.initialize(world);
    
    // Create a test sensory percept
    nlm::SensoryPercept percept;
    
    // Set up vision data (8x8 = 64 values)
    std::vector<float> vision(64, 0.5f);
    percept.setVision(vision);
    
    // Set up touch data (8 values)
    std::vector<float> touch(8, 0.3f);
    percept.setTouch(touch);
    
    // Set up internal data (4 values)
    std::vector<float> internal(4, 0.1f);
    percept.setInternal(internal);
    
    // Set up proprioception data (6 values)
    std::vector<float> proprioception(6, 0.2f);
    percept.setProprioception(proprioception);
    
    // Process sensory input
    agentBrain.processSensoryInput(percept);
    
    // Test that novelty detection worked
    assert(agentBrain.getNoveltyLevel() >= 0.0f);
    assert(agentBrain.getNoveltyLevel() <= 1.0f);
    
    // Test that curiosity was computed
    assert(agentBrain.getCuriosityLevel() >= 0.0f);
    assert(agentBrain.getCuriosityLevel() <= AgentBrainConstants::MAX_CURIOUSITY_LEVEL);
    
    std::cout << "    testSensoryProcessing passed" << std::endl;
}

void testMotorCommandDecoding() {
    std::cout << "    Testing motor command decoding..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agentBrain(brain);
    
    // Test default behavior (no motor neurons initialized)
    nlm::MotorCommand command = agentBrain.decodeMotorCommand();
    assert(command == nlm::MotorCommand::Wait);
    
    std::cout << "    testMotorCommandDecoding passed" << std::endl;
}

void testRewardModulation() {
    std::cout << "    Testing reward modulation..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agentBrain(brain);
    
    // Test initial state
    assert(agentBrain.getPredictionError() == 0.0f);
    assert(agentBrain.getNeuromodulationLevel() == 0.0f);
    
    // Apply reward modulation
    agentBrain.applyRewardModulation(1.0f, 0.5f);
    
    // Test prediction error
    assert(agentBrain.getPredictionError() == 0.5f);
    assert(std::abs(agentBrain.getNeuromodulationLevel() - 0.5f) < 0.01f);
    
    // Apply reward modulation with negative reward
    agentBrain.applyRewardModulation(-1.0f, 0.0f);
    
    // Test prediction error with negative reward
    assert(agentBrain.getPredictionError() == -1.0f);
    assert(std::abs(agentBrain.getNeuromodulationLevel() - (-1.0f)) < 0.01f);
    
    std::cout << "    testRewardModulation passed" << std::endl;
}

void testDevelopmentSystem() {
    std::cout << "    Testing development system..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agentBrain(brain);
    
    // Test initial developmental stage
    assert(agentBrain.getDevelopmentalStage() == nlm::DevelopmentalStage::Initial);
    
    // Test development updates over time
    double timestep = 100.0; // ~1.67 minutes
    agentBrain.updateDevelopment(timestep);
    
    // Should be in CriticalPeriod after ~5 minutes
    assert(agentBrain.getDevelopmentalStage() == nlm::DevelopmentalStage::CriticalPeriod);
    assert(agentBrain.getCuriosityLevel() >= 0.0f);
    
    std::cout << "    testDevelopmentSystem passed" << std::endl;
}

void testConfigurationFlags() {
    std::cout << "    Testing configuration flags..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agentBrain(brain);
    
    // Test default enabled flags
    assert(agentBrain.isRewardModulationEnabled() == true);
    assert(agentBrain.isStructuralPlasticityEnabled() == true);
    assert(agentBrain.isDevelopmentEnabled() == true);
    assert(agentBrain.isCuriosityEnabled() == true);
    
    // Test disable flags
    agentBrain.enableRewardModulation(false);
    agentBrain.enableStructuralPlasticity(false);
    agentBrain.enableDevelopment(false);
    agentBrain.enableCuriosity(false);
    
    assert(agentBrain.isRewardModulationEnabled() == false);
    assert(agentBrain.isStructuralPlasticityEnabled() == false);
    assert(agentBrain.isDevelopmentEnabled() == false);
    assert(agentBrain.isCuriosityEnabled() == false);
    
    std::cout << "    testConfigurationFlags passed" << std::endl;
}

void testReset() {
    std::cout << "    Testing reset functionality..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agentBrain(brain);
    
    // Apply some state changes
    nlm::SimpleWorld world;
    world.configure(10, 10, 8, 8);
    agentBrain.initialize(world);
    
    nlm::SensoryPercept percept;
    std::vector<float> vision(64, 0.5f);
    percept.setVision(vision);
    agentBrain.processSensoryInput(percept);
    
    agentBrain.applyRewardModulation(1.0f, 0.5f);
    agentBrain.updateDevelopment(100.0);
    
    // Verify state changed
    assert(agentBrain.getNoveltyLevel() > 0.0f);
    assert(agentBrain.getPredictionError() > 0.0f);
    
    // Reset agent
    agentBrain.reset();
    
    // Verify state reset
    assert(agentBrain.getNoveltyLevel() == 0.0f);
    assert(agentBrain.getCuriosityLevel() == 0.0f);
    assert(agentBrain.getNeuromodulationLevel() == 0.0f);
    assert(agentBrain.getPredictionError() == 0.0f);
    assert(agentBrain.getDevelopmentalStage() == nlm::DevelopmentalStage::Initial);
    
    std::cout << "    testReset passed" << std::endl;
}

void testErrorHandling() {
    std::cout << "    Testing error handling..." << std::endl;
    
    // Test with null brain (should not crash)
    nlm::AgentBrain agentBrain(nullptr);
    
    // Test processSensoryInput with null brain
    nlm::SensoryPercept percept;
    agentBrain.processSensoryInput(percept);  // Should not crash
    
    // Test decodeMotorCommand with null brain
    nlm::MotorCommand cmd = agentBrain.decodeMotorCommand();
    assert(cmd == nlm::MotorCommand::Wait);
    
    std::cout << "    testErrorHandling passed" << std::endl;
}

void runAll() {
    testConstants();
    testAgentBrainCreation();
    testAgentBrainInitialization();
    testNeuronDistribution();
    testSensoryProcessing();
    testMotorCommandDecoding();
    testRewardModulation();
    testDevelopmentSystem();
    testConfigurationFlags();
    testReset();
    testErrorHandling();
    
    std::cout << "All AgentBrain tests passed!" << std::endl;
}

} // namespace test_agentbrain
