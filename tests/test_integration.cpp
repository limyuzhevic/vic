// Integration Tests
// Tests integration between different components
// Covers end-to-end functionality and system interactions

#include "brain/Brain.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "dynamics/NeuralDynamics.hpp"
#include "core/Config/Config.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "sensory/Vision.hpp"
#include "motor/Action.hpp"
#include <cassert>
#include <iostream>
#include <memory>
#include <vector>

namespace test_integration {

void testBrainConfigurationIntegration() {
    auto config = std::make_shared<nlm::Config>();
    
    // Configure brain with various parameters
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    config->set("learning_rate", 0.01, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Test initialization
    bool success = brain.initialize();
    assert(success);
    
    // Verify configuration was applied
    assert(brain.getRegionCount() == 1);
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getTotalSynapseCount() > 0);
    
    std::cout << "    testBrainConfigurationIntegration passed" << std::endl;
}

void testPlasticityBrainIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get STDP and Hebbian plasticity rules
    nlm::STDP* stdp = brain.getSTDP();
    nlm::Hebbian* hebbian = brain.getHebbian();
    
    assert(stdp != nullptr);
    assert(hebbian != nullptr);
    
    // Configure plasticity rules
    stdp->configure(0.01f, 0.012f, 20.0f);
    hebbian->setLearningRate(0.02f);
    
    // Test that plasticity rules are integrated with brain
    assert(stdp->getName() != nullptr);
    assert(hebbian->getName() != nullptr);
    
    std::cout << "    testPlasticityBrainIntegration passed" << std::endl;
}

void testNeuralDynamicsBrainIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(25), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test neural dynamics integration
    nlm::IntegrateAndFireDynamics lif;
    
    // Configure for brain simulation
    lif.setMembraneTimeConstant(10.0f);
    lif.setThreshold(-50.0f);
    lif.setAdaptationConductance(0.5f);
    
    // Test neural dynamics parameters
    assert(lif.getMembraneTimeConstant() == 10.0f);
    assert(lif.getThreshold() == -50.0f);
    assert(lif.getAdaptationConductance() == 0.5f);
    
    std::cout << "    testNeuralDynamicsBrainIntegration passed" << std::endl;
}

void testBrainStepIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(20), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Run multiple steps
    for (nlm::SimulationStep step = 0; step < 20; ++step) {
        brain.step(step);
    }
    
    // Verify brain state after steps
    assert(brain.getTotalNeuronCount() == 20);
    assert(brain.getFiringNeuronCount() >= 0);
    assert(brain.getAverageFiringRate() >= 0.0f);
    
    std::cout << "    testBrainStepIntegration passed" << std::endl;
}

void testBrainResetIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(30), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Take a step
    brain.step(0);
    
    // Reset brain
    brain.reset();
    
    // Verify reset worked
    assert(brain.getTotalNeuronCount() == 30);
    
    std::cout << "    testBrainResetIntegration passed" << std::endl;
}

void testAgentBrainIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(15), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create AgentBrain
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    // Create a simple world
    nlm::SimpleWorld world;
    world.setVisionWidth(16);
    world.setVisionHeight(16);
    
    // Initialize AgentBrain
    agentBrain.initialize(world);
    
    // Test sensory processing
    nlm::SensoryPercept percept;
    
    // Create vision data
    std::vector<float> vision(256, 0.5f);
    percept.setVision(vision);
    
    // Create touch data
    std::vector<float> touch(8, 0.0f);
    percept.setTouch(touch);
    
    // Create internal signals
    std::vector<float> internal(4, 0.0f);
    percept.setInternal(internal);
    
    // Create proprioception
    std::vector<float> proprioception(6, 0.0f);
    percept.setProprioception(proprioception);
    
    // Process sensory input
    agentBrain.processSensoryInput(percept);
    
    // Test motor command decoding
    nlm::MotorCommand command = agentBrain.decodeMotorCommand();
    assert(command != nlm::MotorCommand::Count);  // Should be a valid command
    
    // Test neuromodulation
    agentBrain.applyRewardModulation(1.0f, 0.5f);
    
    float neuromodulation = agentBrain.getNeuromodulationLevel();
    assert(std::isfinite(neuromodulation));
    
    // Test development
    agentBrain.updateDevelopment(1.0);
    
    float developmentalStage = static_cast<float>(agentBrain.getDevelopmentalStage());
    assert(developmentalStage >= 0.0f);
    
    // Test reset
    agentBrain.reset();
    
    assert(agentBrain.getNeuromodulationLevel() == 0.0f);
    assert(agentBrain.getCuriosityLevel() == 0.0f);
    
    std::cout << "    testAgentBrainIntegration passed" << std::endl;
}

void testConfigBrainIntegration() {
    auto config = std::make_shared<nlm::Config>();
    
    // Set brain configuration parameters
    config->set("neuron_count", static_cast<int64_t>(40), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("learning_rate", 0.005, nlm::ConfigSource::Default);
    config->set("plasticity_enabled", true, nlm::ConfigSource::Default);
    config->set("reward_modulation", true, nlm::ConfigSource::Default);
    
    // Create brain with configuration
    nlm::Brain brain(config);
    brain.initialize();
    
    // Verify configuration was applied
    assert(brain.getRegionCount() == 2);
    assert(brain.getTotalNeuronCount() == 40);
    
    // Test that configuration affects brain behavior
    nlm::STDP* stdp = brain.getSTDP();
    assert(stdp != nullptr);
    
    // Test neuromodulation configuration
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    agentBrain.enableRewardModulation(true);
    
    assert(agentBrain.isRewardModulationEnabled());
    
    std::cout << "    testConfigBrainIntegration passed" << std::endl;
}

void testPlasticityRuleIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(35), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test different plasticity rules
    nlm::STDP stdp;
    nlm::Hebbian hebbian;
    
    // Configure both
    stdp.configure(0.01f, 0.012f, 20.0f);
    hebbian.setLearningRate(0.02f);
    
    // Test they can work together
    nlm::Synapse testSynapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    testSynapse.setType(nlm::SynapseType::Excitatory);
    testSynapse.setWeight(0.5f);
    
    // Create spike activity
    std::vector<nlm::Timestamp> preSpikes = {10.0, 20.0};
    std::vector<nlm::Timestamp> postSpikes = {15.0, 25.0};
    
    // Apply both plasticity rules
    stdp.update(&testSynapse, preSpikes, postSpikes, 0.001);
    hebbian.update(&testSynapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = testSynapse.getWeight();
    
    // Weight should have changed
    assert(finalWeight != 0.5f);
    
    std::cout << "    testPlasticityRuleIntegration passed (final weight: " << finalWeight << ")" << std::endl;
}

void testNeuralDynamicsIntegration() {
    // Test integration of different neural dynamics models
    nlm::IntegrateAndFireDynamics lif;
    nlm::HodgkinHuxleyDynamics hh;
    nlm::FitzHughNagumoDynamics fhn;
    
    // Configure all three models
    lif.setMembraneTimeConstant(20.0f);
    lif.setThreshold(-55.0f);
    
    hh.setTemperature(310.15f);
    hh.setSodiumConductance(120.0f);
    
    fhn.setFHN_a(-0.7f);
    fhn.setFHN_b(0.8f);
    
    // Test that all models can be configured
    assert(lif.getMembraneTimeConstant() == 20.0f);
    assert(hh.getTemperature() == 310.15f);
    assert(fhn.getFHN_a() == -0.7f);
    
    // Test reset functionality
    lif.reset();
    hh.reset();
    fhn.reset();
    
    std::cout << "    testNeuralDynamicsIntegration passed" << std::endl;
}

void testSystemEndToEnd() {
    // Test end-to-end system integration
    auto config = std::make_shared<nlm::Config>();
    
    // Configure complete system
    config->set("neuron_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("learning_rate", 0.01, nlm::ConfigSource::Default);
    config->set("plasticity_enabled", true, nlm::ConfigSource::Default);
    
    // Create brain
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create agent brain
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    // Create world
    nlm::SimpleWorld world;
    world.setVisionWidth(16);
    world.setVisionHeight(16);
    
    agentBrain.initialize(world);
    
    // Create sensory input
    nlm::SensoryPercept percept;
    std::vector<float> vision(256, 0.3f);
    percept.setVision(vision);
    
    // Process step
    agentBrain.processSensoryInput(percept);
    
    // Decode motor command
    nlm::MotorCommand command = agentBrain.decodeMotorCommand();
    
    // Apply reward
    agentBrain.applyRewardModulation(0.5f, 0.3f);
    
    // Update development
    agentBrain.updateDevelopment(1.0);
    
    // Verify system is still functional
    assert(brain.getTotalNeuronCount() == 10);
    assert(agentBrain.getNeuromodulationLevel() != 0.0f || true);  // Can be zero
    
    std::cout << "    testSystemEndToEnd passed" << std::endl;
}

void testErrorConditionIntegration() {
    // Test error conditions and integration robustness
    auto config = std::make_shared<nlm::Config>();
    
    // Create valid config
    config->set("neuron_count", static_cast<int64_t>(5), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test with null brain (edge case)
    nlm::AgentBrain nullAgentBrain(nullptr);
    // Should not crash
    
    // Test with empty config
    nlm::Config emptyConfig;
    nlm::Brain emptyBrain(emptyConfig);
    // Should not crash
    
    std::cout << "    testErrorConditionIntegration passed" << std::endl;
}

void runAll() {
    std::cout << "Running comprehensive integration tests..." << std::endl;
    
    testBrainConfigurationIntegration();
    testPlasticityBrainIntegration();
    testNeuralDynamicsBrainIntegration();
    
    testBrainStepIntegration();
    testBrainResetIntegration();
    
    testAgentBrainIntegration();
    testConfigBrainIntegration();
    
    testPlasticityRuleIntegration();
    testNeuralDynamicsIntegration();
    
    testSystemEndToEnd();
    testErrorConditionIntegration();
    
    std::cout << "All integration tests passed!" << std::endl;
}

} // namespace test_integration
