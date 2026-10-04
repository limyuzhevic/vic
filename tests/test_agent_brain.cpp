#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "agent/AgentBrain.hpp"
#include "agent/SensoryPercept.hpp"
#include "world/SimpleWorld.hpp"
#include "core/Logger/Logger.hpp"
#include <cassert>
#include <iostream>

namespace test_agent_brain {

void testAgentBrainCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    std::cout << "    testAgentBrainCreation passed" << std::endl;
}

void testAgentBrainSensoryInputSize() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    size_t inputSize = agentBrain.getSensoryInputSize();
    // Should be vision (16x16=256) + touch (8) + internal (4) + proprioception (6) = 274
    assert(inputSize == 274);
    
    std::cout << "    testAgentBrainSensoryInputSize passed" << std::endl;
}

void testAgentBrainMotorOutputSize() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    size_t outputSize = agentBrain.getMotorOutputSize();
    // Should be 6 motor commands
    assert(outputSize == 6);
    
    std::cout << "    testAgentBrainMotorOutputSize passed" << std::endl;
}

void testAgentBrainInitialize() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    SimpleWorld world;
    world.configure(10, 10, 8, 8);
    world.reset();
    
    agentBrain.initialize(world);
    
    // Should not crash and should log initialization
    assert(agentBrain.getBrain() == &brain);
    
    std::cout << "    testAgentBrainInitialize passed" << std::endl;
}

void testAgentBrainProcessSensoryInput() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    SimpleWorld world;
    world.configure(10, 10, 8, 8);
    world.reset();
    
    agentBrain.initialize(world);
    
    // Create a perceptual input with typical sizes
    nlm::SensoryPercept percept;
    percept.setVision(std::vector<float>(256, 0.5f));  // Vision (16x16)
    percept.setTouch(std::vector<float>(8, 0.0f));     // Touch (8)
    percept.setInternal(std::vector<float>(4, 0.0f));   // Internal (4)
    percept.setProprioception(std::vector<float>(6, 0.0f)); // Proprioception (6)
    
    // Should process without crashing
    agentBrain.processSensoryInput(percept);
    
    // Check that agent has processed the input
    assert(agentBrain.getCuriosityLevel() == 0.0f); // No novelty expected with uniform input
    
    std::cout << "    testAgentBrainProcessSensoryInput passed" << std::endl;
}

void testAgentBrainDecodeMotorCommand() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    SimpleWorld world;
    world.configure(10, 10, 8, 8);
    world.reset();
    
    agentBrain.initialize(world);
    
    // Without any neural activity, should default to Wait
    nlm::MotorCommand cmd = agentBrain.decodeMotorCommand();
    assert(cmd == nlm::MotorCommand::Wait);
    
    std::cout << "    testAgentBrainDecodeMotorCommand passed" << std::endl;
}

void testAgentBrainApplyRewardModulation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    SimpleWorld world;
    world.configure(10, 10, 8, 8);
    world.reset();
    
    agentBrain.initialize(world);
    
    // Apply reward modulation
    agentBrain.applyRewardModulation(1.0f, 0.5f);  // Positive reward
    
    // Should have computed prediction error and updated state
    assert(agentBrain.getNeuromodulationLevel() > 0.0f); // Positive prediction error
    
    std::cout << "    testAgentBrainApplyRewardModulation passed" << std::endl;
}

void testAgentBrainUpdateDevelopment() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    SimpleWorld world;
    world.configure(10, 10, 8, 8);
    world.reset();
    
    agentBrain.initialize(world);
    
    // Update development
    agentBrain.updateDevelopment(1.0);
    
    // Should have updated developmental age
    assert(agentBrain.getDevelopmentalStage() == nlm::DevelopmentalStage::Initial);
    
    // Update more to reach critical period
    agentBrain.updateDevelopment(100.0);
    assert(agentBrain.getDevelopmentalStage() == nlm::DevelopmentalStage::CriticalPeriod);
    
    std::cout << "    testAgentBrainUpdateDevelopment passed" << std::endl;
}

void testAgentBrainGetAllStats() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    SimpleWorld world;
    world.configure(10, 10, 8, 8);
    world.reset();
    
    agentBrain.initialize(world);
    
    // Test all getter methods
    assert(agentBrain.getNeuromodulationLevel() == 0.0f); // Initial state
    assert(agentBrain.getCuriosityLevel() == 0.0f);
    assert(agentBrain.getNoveltyLevel() == 0.0f);
    assert(agentBrain.getPredictionError() == 0.0f);
    assert(agentBrain.getDevelopmentalStage() == nlm::DevelopmentalStage::Initial);
    
    std::cout << "    testAgentBrainGetAllStats passed" << std::endl;
}

void testAgentBrainConfigurationOptions() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    SimpleWorld world;
    world.configure(10, 10, 8, 8);
    world.reset();
    
    agentBrain.initialize(world);
    
    // Enable/disable features
    agentBrain.enableRewardModulation(true);
    agentBrain.enableStructuralPlasticity(true);
    agentBrain.enableDevelopment(true);
    agentBrain.enableCuriosity(true);
    
    assert(agentBrain.isRewardModulationEnabled());
    assert(agentBrain.isStructuralPlasticityEnabled());
    assert(agentBrain.isDevelopmentEnabled());
    assert(agentBrain.isCuriosityEnabled());
    
    std::cout << "    testAgentBrainConfigurationOptions passed" << std::endl;
}

void testAgentBrainReset() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    SimpleWorld world;
    world.configure(10, 10, 8, 8);
    world.reset();
    
    agentBrain.initialize(world);
    
    // Modify state
    agentBrain.applyRewardModulation(0.5f, 0.2f);
    agentBrain.updateDevelopment(10.0);
    
    // Reset agent
    agentBrain.reset();
    
    // State should be reset
    assert(agentBrain.getNeuromodulationLevel() == 0.0f);
    assert(agentBrain.getCuriosityLevel() == 0.0f);
    assert(agentBrain.getPredictionError() == 0.0f);
    assert(agentBrain.getDevelopmentalStage() == nlm::DevelopmentalStage::Initial);
    
    std::cout << "    testAgentBrainReset passed" << std::endl;
}

void testAgentBrainSensoryPerceptIntegration() {
    // Test integration with configurable SensoryPercept
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    SimpleWorld world;
    world.configure(10, 10, 8, 8);
    world.reset();
    
    agentBrain.initialize(world);
    
    // Test with configured sensory percept (non-default sizes)
    nlm::SensoryPercept percept(32, 32, 16, 8, 12, 4); // Larger dimensions
    
    // Fill with test data
    std::vector<float> vision(32 * 32, 0.3f);
    std::vector<float> touch(16, 0.1f);
    std::vector<float> internal(8, 0.5f);
    std::vector<float> proprioception(12, 0.2f);
    std::vector<float> audio(4, 0.8f);
    
    percept.setVision(vision);
    percept.setTouch(touch);
    percept.setInternal(internal);
    percept.setProprioception(proprioception);
    percept.setAudio(audio);
    
    // Process should work with custom dimensions
    agentBrain.processSensoryInput(percept);
    
    // Validate the percept
    assert(percept.validate());
    assert(percept.getVisionWidth() == 32);
    assert(percept.getVisionHeight() == 32);
    assert(percept.getTouchSize() == 16);
    assert(percept.getInternalSize() == 8);
    assert(percept.getProprioceptionSize() == 12);
    assert(percept.getAudioSize() == 4);
    
    // Total input size should match
    size_t totalInput = percept.getTotalSensoryInputSize();
    assert(totalInput == 32*32 + 16 + 8 + 12 + 4);
    
    std::cout << "    testAgentBrainSensoryPerceptIntegration passed" << std::endl;
}

void runAll() {
    testAgentBrainCreation();
    testAgentBrainSensoryInputSize();
    testAgentBrainMotorOutputSize();
    testAgentBrainInitialize();
    testAgentBrainProcessSensoryInput();
    testAgentBrainDecodeMotorCommand();
    testAgentBrainApplyRewardModulation();
    testAgentBrainUpdateDevelopment();
    testAgentBrainGetAllStats();
    testAgentBrainConfigurationOptions();
    testAgentBrainReset();
    testAgentBrainSensoryPerceptIntegration();
}

} // namespace test_agent_brain
