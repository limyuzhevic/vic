// NLM AgentBrain Tests
// Tests for AgentBrain functionality and integration

#include <iostream>
#include <cassert>
#include <memory>
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include "agent/SensoryPercept.hpp"

namespace test_agent_brain {

void testAgentBrainInitialization() {
    std::cout << "  Testing AgentBrain initialization..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    SimpleWorld world;
    world.initialize(16, 16);
    
    AgentBrain agent(brain);
    agent.initialize(world);
    
    assert(agent.getBrain() != nullptr);
    assert(agent.getBrain() == brain.get());
    assert(agent.getSensoryInputSize() > 0);
    assert(agent.getMotorOutputSize() > 0);
    
    std::cout << "    PASSED: AgentBrain initialization test" << std::endl;
}

void testAgentBrainSensoryProcessing() {
    std::cout << "  Testing AgentBrain sensory processing..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    SimpleWorld world;
    world.initialize(16, 16);
    world.reset();
    
    AgentBrain agent(brain);
    agent.initialize(world);
    
    // Create a test sensory percept
    SensoryPercept percept;
    
    // Set up some test vision data
    std::vector<float> vision(16 * 16, 0.5f);  // Medium intensity
    percept.setVision(vision);
    
    // Set up some test touch data
    std::vector<float> touch(8, 0.0f);  // No touch
    percept.setTouch(touch);
    
    // Set up some test internal data
    std::vector<float> internal(4, 0.0f);  // No internal signals
    percept.setInternal(internal);
    
    // Set up some test proprioception data
    std::vector<float> proprioception(6, 0.0f);  // No proprioception signals
    percept.setProprioception(proprioception);
    
    // Process the sensory input
    agent.processSensoryInput(percept);
    
    std::cout << "    PASSED: AgentBrain sensory processing test" << std::endl;
}

void testAgentBrainMotorDecoding() {
    std::cout << "  Testing AgentBrain motor decoding..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    SimpleWorld world;
    world.initialize(16, 16);
    
    AgentBrain agent(brain);
    agent.initialize(world);
    
    // Test motor command decoding
    MotorCommand command = agent.decodeMotorCommand();
    
    // The decoded command should be a valid MotorCommand enum value
    assert(static_cast<int>(command) >= 0 && static_cast<int>(command) < 10); // MotorCommand has several possible values
    
    std::cout << "    PASSED: AgentBrain motor decoding test" << std::endl;
}

void testAgentBrainNeuromodulation() {
    std::cout << "  Testing AgentBrain neuromodulation..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    SimpleWorld world;
    world.initialize(16, 16);
    
    AgentBrain agent(brain);
    agent.initialize(world);
    
    // Test neuromodulation levels
    float dopamineLevel = agent.getNeuromodulationLevel();
    float curiosityLevel = agent.getCuriosityLevel();
    float noveltyLevel = agent.getNoveltyLevel();
    
    // These should return valid float values
    assert(dopamineLevel >= -1.0f && dopamineLevel <= 1.0f);
    assert(curiosityLevel >= 0.0f && curiosityLevel <= 1.0f);
    assert(noveltyLevel >= 0.0f && noveltyLevel <= 1.0f);
    
    // Test reward modulation
    agent.applyRewardModulation(1.0f, 0.5f);  // Reward = 1.0, Predicted = 0.5
    
    std::cout << "    PASSED: AgentBrain neuromodulation test" << std::endl;
}

void testAgentBrainDevelopment() {
    std::cout << "  Testing AgentBrain development..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    SimpleWorld world;
    world.initialize(16, 16);
    
    AgentBrain agent(brain);
    agent.initialize(world);
    
    // Test developmental stage
    DevelopmentalStage stage = agent.getDevelopmentalStage();
    
    // Should return a valid developmental stage
    assert(static_cast<int>(stage) >= 0 && static_cast<int>(stage) < 6); // 6 developmental stages
    
    // Test development update
    agent.updateDevelopment(0.1);  // Update with 0.1 seconds
    
    std::cout << "    PASSED: AgentBrain development test" << std::endl;
}

void testAgentBrainSubsystemControls() {
    std::cout << "  Testing AgentBrain subsystem controls..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    SimpleWorld world;
    world.initialize(16, 16);
    
    AgentBrain agent(brain);
    agent.initialize(world);
    
    // Test subsystem enable/disable controls
    agent.enableRewardModulation(true);
    agent.enableStructuralPlasticity(true);
    agent.enableDevelopment(true);
    agent.enableCuriosity(true);
    
    // Verify that the subsystems can be enabled
    assert(agent.isRewardModulationEnabled() == true);
    assert(agent.isStructuralPlasticityEnabled() == true);
    assert(agent.isDevelopmentEnabled() == true);
    assert(agent.isCuriosityEnabled() == true);
    
    // Test subsystem status getters
    bool rewardModEnabled = agent.isRewardModulationEnabled();
    bool structuralPlasticityEnabled = agent.isStructuralPlasticityEnabled();
    bool developmentEnabled = agent.isDevelopmentEnabled();
    bool curiosityEnabled = agent.isCuriosityEnabled();
    
    assert(rewardModEnabled);
    assert(structuralPlasticityEnabled);
    assert(developmentEnabled);
    assert(curiosityEnabled);
    
    std::cout << "    PASSED: AgentBrain subsystem controls test" << std::endl;
}

void runAll() {
    std::cout << "AgentBrain Tests:" << std::endl;
    std::cout << "  Testing AgentBrain functionality and integration..." << std::endl;
    std::cout << std::endl;
    
    testAgentBrainInitialization();
    testAgentBrainSensoryProcessing();
    testAgentBrainMotorDecoding();
    testAgentBrainNeuromodulation();
    testAgentBrainDevelopment();
    testAgentBrainSubsystemControls();
    
    std::cout << std::endl;
    std::cout << "AgentBrain Tests: All tests completed!" << std::endl;
}

} // namespace test_agent_brain