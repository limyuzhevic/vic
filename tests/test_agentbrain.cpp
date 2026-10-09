// AgentBrain Tests
// Comprehensive unit tests for AgentBrain functionality

#include "agent/AgentBrain.hpp"
#include "agent/SensoryPercept.hpp"
#include "agent/AgentBody.hpp"
#include "world/SimpleWorld.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <algorithm>

namespace test_agentbrain {

void testAgentBrainCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agentBrain(brain);
    
    // Verify brain pointer
    assert(agentBrain.getBrain() == brain.get());
    
    // Verify configuration flags
    assert(agentBrain.isRewardModulationEnabled() == true);
    assert(agentBrain.isStructuralPlasticityEnabled() == true);
    assert(agentBrain.isDevelopmentEnabled() == true);
    assert(agentBrain.isCuriosityEnabled() == true);
    
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
    world.initialize(100.0f, 100.0f);
    
    // Initialize agent brain
    agentBrain.initialize(world);
    
    // Verify initialization worked
    assert(agentBrain.getSensoryInputSize() > 0);
    assert(agentBrain.getMotorOutputSize() > 0);
    
    // Verify development state
    assert(agentBrain.getDevelopmentalStage() == nlm::DevelopmentalStage::Initial);
    assert(agentBrain.getCuriosityLevel() >= 0.0f);
    assert(agentBrain.getNoveltyLevel() >= 0.0f);
    
    std::cout << "    testAgentBrainInitialization passed" << std::endl;
}

void testAgentBrainConfigurationToggle() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agentBrain(brain);
    
    // Test reward modulation toggle
    agentBrain.enableRewardModulation(false);
    assert(agentBrain.isRewardModulationEnabled() == false);
    
    agentBrain.enableRewardModulation(true);
    assert(agentBrain.isRewardModulationEnabled() == true);
    
    // Test structural plasticity toggle
    agentBrain.enableStructuralPlasticity(false);
    assert(agentBrain.isStructuralPlasticityEnabled() == false);
    
    agentBrain.enableStructuralPlasticity(true);
    assert(agentBrain.isStructuralPlasticityEnabled() == true);
    
    // Test development toggle
    agentBrain.enableDevelopment(false);
    assert(agentBrain.isDevelopmentEnabled() == false);
    
    agentBrain.enableDevelopment(true);
    assert(agentBrain.isDevelopmentEnabled() == true);
    
    // Test curiosity toggle
    agentBrain.enableCuriosity(false);
    assert(agentBrain.isCuriosityEnabled() == false);
    
    agentBrain.enableCuriosity(true);
    assert(agentBrain.isCuriosityEnabled() == true);
    
    std::cout << "    testAgentBrainConfigurationToggle passed" << std::endl;
}

void testAgentBrainSensoryInputProcessing() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agentBrain(brain);
    
    nlm::SimpleWorld world;
    world.initialize(100.0f, 100.0f);
    agentBrain.initialize(world);
    
    // Create sensory percept with various inputs
    nlm::SensoryPercept percept;
    
    // Vision: 16x16 grid (256 values)
    std::vector<float> vision(256, 0.5f);
    for (size_t i = 0; i < std::min<size_t>(256, vision.size()); ++i) {
        vision[i] = static_cast<float>(i % 10) / 10.0f;
    }
    percept.setVision(vision);
    
    // Touch: 8 values
    std::vector<float> touch(8, 0.0f);
    touch[0] = 0.8f;
    touch[1] = 0.3f;
    percept.setTouch(touch);
    
    // Internal: 4 values (-1 to 1 range)
    std::vector<float> internal(4, 0.0f);
    internal[0] = 0.5f;
    internal[1] = -0.2f;
    percept.setInternal(internal);
    
    // Proprioception: 6 values
    std::vector<float> proprioception(6, 0.0f);
    proprioception[0] = 1.0f;
    proprioception[3] = -0.5f;
    percept.setProprioception(proprioception);
    
    // Process sensory input
    agentBrain.processSensoryInput(percept);
    
    // Verify novelty was calculated
    assert(agentBrain.getNoveltyLevel() >= 0.0f);
    assert(agentBrain.getNoveltyLevel() < 2.0f); // Should be normalized
    
    std::cout << "    testAgentBrainSensoryInputProcessing passed" << std::endl;
}

void testAgentBrainMotorDecoding() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agentBrain(brain);
    
    nlm::SimpleWorld world;
    world.initialize(100.0f, 100.0f);
    agentBrain.initialize(world);
    
    // Verify default motor command
    nlm::MotorCommand cmd = agentBrain.decodeMotorCommand();
    assert(static_cast<int>(cmd) >= 0 && static_cast<int>(cmd) <= 7);
    
    std::cout << "    testAgentBrainMotorDecoding passed" << std::endl;
}

void testAgentBrainRewardModulation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agentBrain(brain);
    
    // Apply reward modulation with positive reward
    agentBrain.applyRewardModulation(1.0f, 0.5f);
    
    // Verify neuromodulation state
    assert(agentBrain.getPredictionError() == 0.5f); // 1.0 - 0.5
    assert(agentBrain.getNeuromodulationLevel() == 0.5f); // Should be clamped
    assert(agentBrain.getNoveltyLevel() >= 0.0f);
    
    // Apply reward modulation with negative reward (prediction error < 0)
    agentBrain.applyRewardModulation(-1.0f, 0.0f);
    
    // Verify prediction error is negative
    assert(agentBrain.getPredictionError() == -1.0f);
    assert(agentBrain.getNeuromodulationLevel() == -1.0f); // Should be clamped
    
    std::cout << "    testAgentBrainRewardModulation passed" << std::endl;
}

void testAgentBrainDevelopmentSystem() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agentBrain(brain);
    
    nlm::SimpleWorld world;
    world.initialize(100.0f, 100.0f);
    agentBrain.initialize(world);
    
    // Verify initial development stage
    assert(agentBrain.getDevelopmentalStage() == nlm::DevelopmentalStage::Initial);
    
    // Update development
    agentBrain.updateDevelopment(10.0); // 10 seconds
    
    // Should still be in initial stage (development < 60s)
    assert(agentBrain.getDevelopmentalStage() == nlm::DevelopmentalStage::Initial);
    
    // Test development across stages
    agentBrain.updateDevelopment(50.0); // Move to critical period
    assert(agentBrain.getDevelopmentalStage() == nlm::DevelopmentalStage::CriticalPeriod);
    
    agentBrain.updateDevelopment(100.0); // Move to maturation
    assert(agentBrain.getDevelopmentalStage() == nlm::DevelopmentalStage::Maturation);
    
    agentBrain.updateDevelopment(500.0); // Move to adult
    assert(agentBrain.getDevelopmentalStage() == nlm::DevelopmentalStage::Adult);
    
    std::cout << "    testAgentBrainDevelopmentSystem passed" << std::endl;
}

void testAgentBrainStateReset() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agentBrain(brain);
    
    nlm::SimpleWorld world;
    world.initialize(100.0f, 100.0f);
    agentBrain.initialize(world);
    
    // Apply some changes to state
    agentBrain.applyRewardModulation(1.0f, 0.5f);
    agentBrain.updateDevelopment(30.0);
    
    // Reset
    agentBrain.reset();
    
    // Verify state was reset
    assert(agentBrain.getPredictionError() == 0.0f);
    assert(agentBrain.getNeuromodulationLevel() == 0.0f);
    assert(agentBrain.getCuriosityLevel() == 0.0f);
    assert(agentBrain.getNoveltyLevel() == 0.0f);
    assert(agentBrain.getDevelopmentalStage() == nlm::DevelopmentalStage::Initial);
    
    std::cout << "    testAgentBrainStateReset passed" << std::endl;
}

void testAgentBrainIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    bool brainInit = brain->initialize();
    assert(brainInit);
    
    nlm::AgentBrain agentBrain(brain);
    
    // Create world with visual targets
    nlm::SimpleWorld world;
    world.initialize(100.0f, 100.0f);
    
    // Create agent body
    nlm::AgentBody agentBody;
    agentBody.x = 50.0f;
    agentBody.y = 50.0f;
    
    // Initialize agent brain with world
    agentBrain.initialize(world);
    
    // Create sensory percept with visual input
    nlm::SensoryPercept percept;
    
    // Vision: Simple pattern - center bright, edges dark
    std::vector<float> vision(256, 0.0f);
    for (int y = 0; y < 16; ++y) {
        for (int x = 0; x < 16; ++x) {
            int idx = y * 16 + x;
            float dist = std::sqrt(std::pow(x - 8.0f, 2) + std::pow(y - 8.0f, 2));
            vision[idx] = std::max(0.0f, 1.0f - dist / 8.0f);
        }
    }
    percept.setVision(vision);
    
    // Touch: Some contact
    std::vector<float> touch(8, 0.0f);
    touch[0] = 0.7f;
    touch[1] = 0.2f;
    percept.setTouch(touch);
    
    // Internal: Energy state
    std::vector<float> internal(4, 0.0f);
    internal[0] = 0.8f; // High energy
    internal[1] = 0.5f; // Good health
    percept.setInternal(internal);
    
    // Proprioception: Movement
    std::vector<float> proprioception(6, 0.0f);
    proprioception[0] = 1.0f; // Moving forward
    percept.setProprioception(proprioception);
    
    // Process sensory input
    agentBrain.processSensoryInput(percept);
    
    // Decode motor command
    nlm::MotorCommand command = agentBrain.decodeMotorCommand();
    assert(static_cast<int>(command) >= 0 && static_cast<int>(command) <= 7);
    
    // Apply reward based on action result
    agentBrain.applyRewardModulation(0.5f, 0.3f);
    
    // Update development
    agentBrain.updateDevelopment(1.0);
    
    // Verify integration
    assert(agentBrain.getBrain() != nullptr);
    assert(agentBrain.getSensoryInputSize() > 0);
    assert(agentBrain.getMotorOutputSize() > 0);
    
    std::cout << "    testAgentBrainIntegration passed" << std::endl;
}

void testAgentBrainCuriosityDrivenExploration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agentBrain(brain);
    
    nlm::SimpleWorld world;
    world.initialize(100.0f, 100.0f);
    agentBrain.initialize(world);
    
    // Create percept that generates high novelty
    nlm::SensoryPercept percept;
    std::vector<float> vision(256, 0.0f);
    for (size_t i = 0; i < 256; ++i) {
        vision[i] = static_cast<float>(i) / 255.0f; // High variation
    }
    percept.setVision(vision);
    
    // Process input to generate curiosity
    agentBrain.processSensoryInput(percept);
    
    // Curiosity should be elevated due to high novelty
    assert(agentBrain.getCuriosityLevel() > 0.0f);
    
    // Decode motor command with high curiosity
    nlm::MotorCommand cmd = agentBrain.decodeMotorCommand();
    
    // Enable curiosity
    agentBrain.enableCuriosity(true);
    
    std::cout << "    testAgentBrainCuriosityDrivenExploration passed" << std::endl;
}

void testAgentBrainEdgeCases() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agentBrain(brain);
    
    // Test with null brain (edge case - constructor doesn't check)
    nlm::AgentBrain nullAgentBrain(nullptr);
    assert(nullAgentBrain.getBrain() == nullptr);
    
    // Test with zero world dimensions
    nlm::SimpleWorld world;
    world.initialize(0.0f, 0.0f);
    agentBrain.initialize(world);
    
    // Should handle gracefully
    assert(agentBrain.getSensoryInputSize() > 0);
    
    // Test with negative reward values
    agentBrain.applyRewardModulation(-2.0f, 1.0f);
    assert(agentBrain.getPredictionError() == -2.0f - 1.0f); // -3.0
    
    std::cout << "    testAgentBrainEdgeCases passed" << std::endl;
}

void runAll() {
    std::cout << "Running AgentBrain tests..." << std::endl;
    testAgentBrainCreation();
    testAgentBrainInitialization();
    testAgentBrainConfigurationToggle();
    testAgentBrainSensoryInputProcessing();
    testAgentBrainMotorDecoding();
    testAgentBrainRewardModulation();
    testAgentBrainDevelopmentSystem();
    testAgentBrainStateReset();
    testAgentBrainIntegration();
    testAgentBrainCuriosityDrivenExploration();
    testAgentBrainEdgeCases();
}

} // namespace test_agentbrain
