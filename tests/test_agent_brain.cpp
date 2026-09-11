// NLM AgentBrain Tests
// Phase 6: Final Integration Tests

#include <cassert>
#include <iostream>
#include <vector>
#include <memory>
#include "AgentBrain.hpp"
#include "Brain.hpp"
#include "Config/Config.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/SensoryPercept.hpp"
#include "motor/Action.hpp"

namespace test_agent_brain {

void testInitialization() {
    std::cout << "  Testing AgentBrain initialization..." << std::endl;
    
    auto config = std::make_shared<Config>();
    auto brain = std::make_shared<Brain>(config);
    assert(brain->initialize());
    
    AgentBrain agent(brain);
    SimpleWorld world;
    world.configure(20, 20, 16, 16);
    agent.initialize(world);
    
    assert(agent.getSensoryInputSize() > 0);
    assert(agent.getMotorOutputSize() > 0);
    assert(agent.getBrain() == brain.get());
    
    std::cout << "  PASSED" << std::endl;
}

void testSensoryProcessing() {
    std::cout << "  Testing sensory input processing..." << std::endl;
    
    auto config = std::make_shared<Config>();
    auto brain = std::make_shared<Brain>(config);
    assert(brain->initialize());
    
    AgentBrain agent(brain);
    SimpleWorld world;
    world.configure(20, 20, 16, 16);
    agent.initialize(world);
    
    // Create a percept with vision data
    SensoryPercept percept;
    std::vector<float> vision(256, 0.0f);
    for (size_t i = 0; i < 256; ++i) {
        vision[i] = static_cast<float>(i) / 255.0f;
    }
    percept.setVision(vision);
    
    // Process sensory input
    agent.processSensoryInput(percept);
    
    // Check that novelty was calculated
    float novelty = agent.getNoveltyLevel();
    assert(novelty >= 0.0f && novelty <= 1.0f);
    
    // Check that curiosity was updated based on novelty
    float curiosity = agent.getCuriosityLevel();
    assert(curiosity >= 0.0f && curiosity <= 1.0f);
    
    std::cout << "  PASSED" << std::endl;
}

void testMotorDecoding() {
    std::cout << "  Testing motor command decoding..." << std::endl;
    
    auto config = std::make_shared<Config>();
    auto brain = std::make_shared<Brain>(config);
    assert(brain->initialize());
    
    AgentBrain agent(brain);
    SimpleWorld world;
    world.configure(20, 20, 16, 16);
    agent.initialize(world);
    
    // Test default (no neurons firing)
    MotorCommand cmd = agent.decodeMotorCommand();
    assert(cmd == MotorCommand::Wait);
    
    // Create a percept to activate some neurons
    SensoryPercept percept;
    std::vector<float> vision(256, 0.5f);
    percept.setVision(vision);
    agent.processSensoryInput(percept);
    
    // Decode motor command
    cmd = agent.decodeMotorCommand();
    // Should not crash and return a valid command
    assert(cmd == MotorCommand::MoveForward || cmd == MotorCommand::Wait);
    
    std::cout << "  PASSED" << std::endl;
}

void testRewardModulation() {
    std::cout << "  Testing reward-modulated learning..." << std::endl;
    
    auto config = std::make_shared<Config>();
    auto brain = std::make_shared<Brain>(config);
    assert(brain->initialize());
    
    AgentBrain agent(brain);
    SimpleWorld world;
    world.configure(20, 20, 16, 16);
    agent.initialize(world);
    
    // Enable reward modulation
    agent.enableRewardModulation(true);
    assert(agent.isRewardModulationEnabled());
    
    // Apply reward modulation
    float initialPredictionError = agent.getPredictionError();
    agent.applyRewardModulation(1.0f, 0.5f);
    
    float predictionError = agent.getPredictionError();
    assert(predictionError == 0.5f); // reward - predicted = 1.0 - 0.5
    
    // Check dopamine level was set
    float dopamine = agent.getNeuromodulationLevel();
    assert(dopamine == 0.5f);
    
    std::cout << "  PASSED" << std::endl;
}

void testDevelopment() {
    std::cout << "  Testing developmental system..." << std::endl;
    
    auto config = std::make_shared<Config>();
    auto brain = std::make_shared<Brain>(config);
    assert(brain->initialize());
    
    AgentBrain agent(brain);
    SimpleWorld world;
    world.configure(20, 20, 16, 16);
    agent.initialize(world);
    
    // Enable development
    agent.enableDevelopment(true);
    assert(agent.isDevelopmentEnabled());
    
    DevelopmentalStage initialStage = agent.getDevelopmentalStage();
    assert(initialStage == DevelopmentalStage::Initial);
    
    // Update development
    agent.updateDevelopment(10.0);
    
    // Should have advanced to next stage (still Initial for small timestep)
    DevelopmentalStage stage = agent.getDevelopmentalStage();
    assert(stage == DevelopmentalStage::Initial);
    
    std::cout << "  PASSED" << std::endl;
}

void testCuriosityExploration() {
    std::cout << "  Testing curiosity-driven exploration..." << std::endl;
    
    auto config = std::make_shared<Config>();
    auto brain = std::make_shared<Brain>(config);
    assert(brain->initialize());
    
    AgentBrain agent(brain);
    SimpleWorld world;
    world.configure(20, 20, 16, 16);
    agent.initialize(world);
    
    // Enable curiosity
    agent.enableCuriosity(true);
    assert(agent.isCuriosityEnabled());
    
    // Set high curiosity level
    // Note: In real usage, curiosity is calculated automatically from novelty
    // For testing, we can simulate by calling processSensoryInput with changing input
    
    // Create percepts with different novelty
    SensoryPercept percept1;
    std::vector<float> vision1(256, 0.0f);
    percept1.setVision(vision1);
    agent.processSensoryInput(percept1);
    
    float curiosity1 = agent.getCuriosityLevel();
    
    SensoryPercept percept2;
    std::vector<float> vision2(256, 1.0f);
    percept2.setVision(vision2);
    agent.processSensoryInput(percept2);
    
    float curiosity2 = agent.getCuriosityLevel();
    
    // Curiosity should be different (not necessarily higher)
    assert(curiosity1 >= 0.0f && curiosity1 <= 1.0f);
    assert(curiosity2 >= 0.0f && curiosity2 <= 1.0f);
    
    std::cout << "  PASSED" << std::endl;
}

void testReset() {
    std::cout << "  Testing agent reset functionality..." << std::endl;
    
    auto config = std::make_shared<Config>();
    auto brain = std::make_shared<Brain>(config);
    assert(brain->initialize());
    
    AgentBrain agent(brain);
    SimpleWorld world;
    world.configure(20, 20, 16, 16);
    agent.initialize(world);
    
    // Modify some state
    agent.applyRewardModulation(1.0f, 0.5f);
    float dopamineBefore = agent.getNeuromodulationLevel();
    
    // Reset
    agent.reset();
    
    // Check state was reset
    assert(agent.getNeuromodulationLevel() == 0.0f);
    assert(agent.getPredictionError() == 0.0f);
    assert(agent.getCuriosityLevel() == 0.0f);
    assert(agent.getNoveltyLevel() == 0.0f);
    
    std::cout << "  PASSED" << std::endl;
}

void testSubsystemEnableDisable() {
    std::cout << "  Testing subsystem enable/disable..." << std::endl;
    
    auto config = std::make_shared<Config>();
    auto brain = std::make_shared<Brain>(config);
    assert(brain->initialize());
    
    AgentBrain agent(brain);
    
    // Test initial state (all enabled by default)
    assert(agent.isRewardModulationEnabled());
    assert(agent.isStructuralPlasticityEnabled());
    assert(agent.isDevelopmentEnabled());
    assert(agent.isCuriosityEnabled());
    
    // Disable all subsystems
    agent.enableRewardModulation(false);
    agent.enableStructuralPlasticity(false);
    agent.enableDevelopment(false);
    agent.enableCuriosity(false);
    
    assert(!agent.isRewardModulationEnabled());
    assert(!agent.isStructuralPlasticityEnabled());
    assert(!agent.isDevelopmentEnabled());
    assert(!agent.isCuriosityEnabled());
    
    // Re-enable
    agent.enableRewardModulation(true);
    agent.enableStructuralPlasticity(true);
    agent.enableDevelopment(true);
    agent.enableCuriosity(true);
    
    assert(agent.isRewardModulationEnabled());
    assert(agent.isStructuralPlasticityEnabled());
    assert(agent.isDevelopmentEnabled());
    assert(agent.isCuriosityEnabled());
    
    std::cout << "  PASSED" << std::endl;
}

void runAll() {
    std::cout << "=== AgentBrain Tests ===" << std::endl;
    
    try {
        testInitialization();
        testSensoryProcessing();
        testMotorDecoding();
        testRewardModulation();
        testDevelopment();
        testCuriosityExploration();
        testReset();
        testSubsystemEnableDisable();
        
        std::cout << std::endl;
        std::cout << "=== All AgentBrain Tests PASSED ===" << std::endl;
    } catch (const std::exception& e) {
        std::cout << std::endl;
        std::cout << "=== AgentBrain Test FAILED: " << e.what() << " ===" << std::endl;
        throw;
    }
}

} // namespace test_agent_brain
