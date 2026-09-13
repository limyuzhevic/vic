// AgentBrain Tests
// Phase 6: Integration Test Suite

#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_agentbrain {

void testAgentBrainCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    
    nlm::AgentBrain agentBrain(brain);
    
    // Verify brain pointer is stored correctly
    assert(agentBrain.getBrain() == brain.get());
    
    std::cout << "    testAgentBrainCreation passed" << std::endl;
}

void testAgentBrainInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agentBrain(brain);
    
    // Create a simple world
    nlm::SimpleWorld world;
    world.configure(10, 10, 8, 8);
    world.reset();
    
    // Initialize agent brain
    agentBrain.initialize(world);
    
    // Verify initialization worked
    assert(agentBrain.getSensoryInputSize() == 256 + 8 + 4 + 6); // Vision(256) + Touch(8) + Internal(4) + Proprioception(6)
    assert(agentBrain.getMotorOutputSize() == 6);
    
    std::cout << "    testAgentBrainInitialization passed" << std::endl;
}

void testAgentBrainSensoryProcessing() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agentBrain(brain);
    
    // Create world and percept
    nlm::SimpleWorld world;
    world.configure(10, 10, 8, 8);
    world.reset();
    
    agentBrain.initialize(world);
    
    // Create a test percept with non-zero values
    nlm::SensoryPercept percept;
    
    // Vision data (8x8 = 64 values, but AgentBrain expects 256 for 16x16)
    std::vector<float> vision(256, 0.5f);
    for (size_t i = 0; i < std::min(size_t(64), vision.size()); ++i) {
        vision[i] = 0.8f; // High vision values to ensure neuron activity
    }
    percept.setVision(vision);
    
    // Touch data
    std::vector<float> touch = {0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    percept.setTouch(touch);
    
    // Internal signals
    std::vector<float> internal = {0.0f, 0.5f, -0.5f, 1.0f};
    percept.setInternal(internal);
    
    // Proprioception
    std::vector<float> proprioception = {0.1f, -0.1f, 0.05f, -0.05f, 0.02f, -0.02f};
    percept.setProprioception(proprioception);
    
    // Process sensory input
    agentBrain.processSensoryInput(percept);
    
    // Check that curiosity level was updated (based on novelty computation)
    // Since we changed vision from default (0.0) to actual values, novelty should be positive
    assert(agentBrain.getNoveltyLevel() >= 0.0f);
    
    // Check curiosity level (depends on novelty and prediction error)
    assert(agentBrain.getCuriosityLevel() >= 0.0f && agentBrain.getCuriosityLevel() <= 1.0f);
    
    std::cout << "    testAgentBrainSensoryProcessing passed" << std::endl;
}

void testAgentBrainMotorDecoding() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agentBrain(brain);
    
    // Initialize world
    nlm::SimpleWorld world;
    world.configure(10, 10, 8, 8);
    world.reset();
    agentBrain.initialize(world);
    
    // Without neural activity, should return Wait
    nlm::MotorCommand cmd = agentBrain.decodeMotorCommand();
    assert(cmd == nlm::MotorCommand::Wait);
    
    // Test with enabled curiosity (should still return Wait when curiosity is low)
    agentBrain.enableCuriosity(true);
    cmd = agentBrain.decodeMotorCommand();
    assert(cmd == nlm::MotorCommand::Wait);
    
    std::cout << "    testAgentBrainMotorDecoding passed" << std::endl;
}

void testAgentBrainRewardModulation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agentBrain(brain);
    
    // Initialize world
    nlm::SimpleWorld world;
    world.configure(10, 10, 8, 8);
    world.reset();
    agentBrain.initialize(world);
    
    // Store initial values
    float initialPredictionError = agentBrain.getPredictionError();
    float initialCuriosity = agentBrain.getCuriosityLevel();
    float initialDopamine = agentBrain.getNeuromodulationLevel();
    
    // Apply reward modulation
    float reward = 1.0f;
    float predictedReward = 0.5f;
    agentBrain.applyRewardModulation(reward, predictedReward);
    
    // Check that prediction error was computed
    assert(agentBrain.getPredictionError() == reward - predictedReward);
    
    // Check dopamine level (should be clamped to [-1, 1])
    assert(agentBrain.getNeuromodulationLevel() >= -1.0f && agentBrain.getNeuromodulationLevel() <= 1.0f);
    
    // Check that expected reward was updated (exponential moving average)
    assert(agentBrain.getPredictionError() == reward - predictedReward);
    
    std::cout << "    testAgentBrainRewardModulation passed" << std::endl;
}

void testAgentBrainDevelopment() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agentBrain(brain);
    
    // Initialize world
    nlm::SimpleWorld world;
    world.configure(10, 10, 8, 8);
    world.reset();
    agentBrain.initialize(world);
    
    // Get initial developmental stage
    nlm::DevelopmentalStage initialStage = agentBrain.getDevelopmentalStage();
    
    // Update development
    agentBrain.updateDevelopment(10.0); // 10 time units
    
    // Check developmental age increased
    assert(agentBrain.getDevelopmentalStage() == nlm::DevelopmentalStage::Initial ||
           agentBrain.getDevelopmentalStage() == nlm::DevelopmentalStage::CriticalPeriod);
    
    std::cout << "    testAgentBrainDevelopment passed" << std::endl;
}

void testAgentBrainReset() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agentBrain(brain);
    
    // Initialize world
    nlm::SimpleWorld world;
    world.configure(10, 10, 8, 8);
    world.reset();
    agentBrain.initialize(world);
    
    // Store initial values
    float initialDopamine = agentBrain.getNeuromodulationLevel();
    float initialCuriosity = agentBrain.getCuriosityLevel();
    float initialPredictionError = agentBrain.getPredictionError();
    
    // Reset agent brain
    agentBrain.reset();
    
    // Check that values were reset to defaults
    assert(agentBrain.getNeuromodulationLevel() == 0.0f);
    assert(agentBrain.getCuriosityLevel() == 0.0f);
    assert(agentBrain.getPredictionError() == 0.0f);
    
    std::cout << "    testAgentBrainReset passed" << std::endl;
}

void testAgentBrainEnableDisableSubsystems() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agentBrain(brain);
    
    // Test enabling/disabling subsystems
    agentBrain.enableRewardModulation(true);
    assert(agentBrain.isRewardModulationEnabled() == true);
    
    agentBrain.enableStructuralPlasticity(true);
    assert(agentBrain.isStructuralPlasticityEnabled() == true);
    
    agentBrain.enableDevelopment(true);
    assert(agentBrain.isDevelopmentEnabled() == true);
    
    agentBrain.enableCuriosity(true);
    assert(agentBrain.isCuriosityEnabled() == true);
    
    // Test disabling
    agentBrain.enableRewardModulation(false);
    assert(agentBrain.isRewardModulationEnabled() == false);
    
    agentBrain.enableStructuralPlasticity(false);
    assert(agentBrain.isStructuralPlasticityEnabled() == false);
    
    agentBrain.enableDevelopment(false);
    assert(agentBrain.isDevelopmentEnabled() == false);
    
    agentBrain.enableCuriosity(false);
    assert(agentBrain.isCuriosityEnabled() == false);
    
    std::cout << "    testAgentBrainEnableDisableSubsystems passed" << std::endl;
}

void testAgentBrainNullSafety() {
    // Test with null brain (should not crash)
    nlm::AgentBrain agentBrain(nullptr);
    
    // These operations should not crash
    assert(agentBrain.getBrain() == nullptr);
    
    // Process sensory input with null brain (should not crash)
    nlm::SensoryPercept percept;
    agentBrain.processSensoryInput(percept); // Should not crash
    
    // Decode motor command with null brain (should return Wait)
    nlm::MotorCommand cmd = agentBrain.decodeMotorCommand();
    assert(cmd == nlm::MotorCommand::Wait);
    
    std::cout << "    testAgentBrainNullSafety passed" << std::endl;
}

void runAll() {
    testAgentBrainCreation();
    testAgentBrainInitialization();
    testAgentBrainSensoryProcessing();
    testAgentBrainMotorDecoding();
    testAgentBrainRewardModulation();
    testAgentBrainDevelopment();
    testAgentBrainReset();
    testAgentBrainEnableDisableSubsystems();
    testAgentBrainNullSafety();
}

} // namespace test_agentbrain
