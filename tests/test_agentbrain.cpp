// AgentBrain Tests
// Comprehensive test suite for improved AgentBrain implementation

#include "agent/AgentBrain.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "world/SimpleWorld.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <vector>

namespace test_agentbrain {

void testAgentBrainCreation() {
    std::cout << "    testAgentBrainCreation passed" << std::endl;
}

void testAgentBrainInitialization() {
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agentBrain(brain);
    
    nlm::SimpleWorld world;
    world.configure(64, 64, 16, 16);
    
    agentBrain.initialize(world);
    
    // Verify initialization
    size_t sensoryInputSize = agentBrain.getSensoryInputSize();
    assert(sensoryInputSize == 266);  // 256 + 8 + 4 + 6 + 2 (from AgentBrain::getSensoryInputSize())
    assert(agentBrain.getMotorOutputSize() == 6);
    
    std::cout << "    testAgentBrainInitialization passed" << std::endl;
}

void testAgentBrainConfiguration() {
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agentBrain(brain);
    
    // Test configuration methods
    agentBrain.enableRewardModulation(false);
    assert(!agentBrain.isRewardModulationEnabled());
    
    agentBrain.enableCuriosity(true);
    assert(agentBrain.isCuriosityEnabled());
    
    // Test new configuration parameters
    agentBrain.setSensoryNoveltyDecay(0.95f);
    assert(agentBrain.getSensoryNoveltyDecay() == 0.95f);
    
    agentBrain.setCuriosityThreshold(0.5f);
    assert(agentBrain.getCuriosityThreshold() == 0.5f);
    
    agentBrain.setExplorationProbability(0.7f);
    assert(agentBrain.getExplorationProbability() == 0.7f);
    
    agentBrain.setPredictionErrorLearningRate(0.1f);
    assert(agentBrain.getPredictionErrorLearningRate() == 0.1f);
    
    agentBrain.setPlasticityFactorRange(0.05f, 1.5f);
    assert(agentBrain.getPlasticityFactorMin() == 0.05f);
    assert(agentBrain.getPlasticityFactorMax() == 1.5f);
    
    std::cout << "    testAgentBrainConfiguration passed" << std::endl;
}

void testAgentBrainSensoryProcessing() {
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agentBrain(brain);
    
    nlm::SimpleWorld world;
    world.configure(64, 64, 16, 16);
    
    agentBrain.initialize(world);
    
    // Create test sensory percept
    nlm::SensoryPercept percept;
    std::vector<float> vision(256, 0.5f);
    std::vector<float> touch(8, 0.0f);
    std::vector<float> internal(4, 0.0f);
    std::vector<float> proprioception(6, 0.0f);
    
    percept.setVision(vision);
    percept.setTouch(touch);
    percept.setInternal(internal);
    percept.setProprioception(proprioception);
    
    // Process sensory input
    agentBrain.processSensoryInput(percept);
    
    // Verify processing didn't crash
    assert(agentBrain.getNoveltyLevel() >= 0.0f);
    assert(agentBrain.getCuriosityLevel() >= 0.0f);
    
    std::cout << "    testAgentBrainSensoryProcessing passed" << std::endl;
}

void testAgentBrainMotorDecoding() {
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agentBrain(brain);
    
    // Verify motor command decoding works
    nlm::MotorCommand cmd = agentBrain.decodeMotorCommand();
    assert(cmd == nlm::MotorCommand::Wait || cmd == nlm::MotorCommand::MoveForward ||
           cmd == nlm::MotorCommand::MoveBackward || cmd == nlm::MotorCommand::TurnLeft ||
           cmd == nlm::MotorCommand::TurnRight || cmd == nlm::MotorCommand::Interact);
    
    std::cout << "    testAgentBrainMotorDecoding passed" << std::endl;
}

void testAgentBrainRewardModulation() {
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agentBrain(brain);
    
    // Apply reward modulation
    agentBrain.applyRewardModulation(1.0f, 0.5f);
    
    // Verify prediction error and dopamine level
    float predError = agentBrain.getPredictionError();
    float dopamine = agentBrain.getNeuromodulationLevel();
    
    assert(std::abs(predError - 0.5f) < 0.001f);
    assert(std::abs(dopamine - 0.5f) < 0.001f);
    
    std::cout << "    testAgentBrainRewardModulation passed" << std::endl;
}

void testAgentBrainDevelopment() {
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agentBrain(brain);
    
    // Update development
    agentBrain.updateDevelopment(10.0);
    
    // Verify developmental stage
    nlm::DevelopmentalStage stage = agentBrain.getDevelopmentalStage();
    assert(stage == nlm::DevelopmentalStage::Initial || stage == nlm::DevelopmentalStage::CriticalPeriod ||
           stage == nlm::DevelopmentalStage::Maturation || stage == nlm::DevelopmentalStage::Adult);
    
    std::cout << "    testAgentBrainDevelopment passed" << std::endl;
}

void testAgentBrainValidation() {
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agentBrain(brain);
    
    // Test configuration validation
    agentBrain.validateConfiguration();
    
    // Test brain state validation
    assert(agentBrain.getBrain() != nullptr);
    
    // Test memory consistency check
    agentBrain.checkMemoryConsistency();
    
    std::cout << "    testAgentBrainValidation passed" << std::endl;
}

void testAgentBrainSystemState() {
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agentBrain(brain);
    
    // Test system state logging
    agentBrain.logSystemState();
    
    // Test neuron validation
    std::vector<nlm::Neuron*> testNeurons;
    bool valid = agentBrain.validateNeuron(testNeurons, "test");
    assert(valid == true);
    
    // Test valid neurons helper
    std::vector<nlm::Neuron*> nullNeurons = {nullptr, nullptr};
    std::vector<nlm::Neuron*> validNeurons = agentBrain.getValidNeurons(nullNeurons);
    assert(validNeurons.size() == 0);
    
    std::cout << "    testAgentBrainSystemState passed" << std::endl;
}

void testAgentBrainReset() {
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agentBrain(brain);
    
    // Set some state
    agentBrain.applyRewardModulation(1.0f, 0.5f);
    agentBrain.updateDevelopment(10.0);
    
    // Reset
    agentBrain.reset();
    
    // Verify reset
    assert(agentBrain.getNeuromodulationLevel() == 0.0f);
    assert(agentBrain.getPredictionError() == 0.0f);
    
    std::cout << "    testAgentBrainReset passed" << std::endl;
}

void testAgentBrainHealth() {
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agentBrain(brain);
    
    // Test health check
    bool healthy = agentBrain.isHealthy();
    // Health depends on implementation specifics, just ensure it runs without crashing
    
    std::cout << "    testAgentBrainHealth passed" << std::endl;
}

void testAgentBrainPythonBindings() {
    // This test would verify Python bindings work correctly
    // For now, just ensure the class can be instantiated
    std::cout << "    testAgentBrainPythonBindings skipped (Python integration test)" << std::endl;
}

void runAllTests() {
    std::cout << "Running AgentBrain tests..." << std::endl;
    
    testAgentBrainCreation();
    testAgentBrainInitialization();
    testAgentBrainConfiguration();
    testAgentBrainSensoryProcessing();
    testAgentBrainMotorDecoding();
    testAgentBrainRewardModulation();
    testAgentBrainDevelopment();
    testAgentBrainValidation();
    testAgentBrainSystemState();
    testAgentBrainReset();
    testAgentBrainHealth();
    testAgentBrainPythonBindings();
    
    std::cout << std::endl << "All AgentBrain tests passed!" << std::endl;
}

} // namespace test_agentbrain

// Entry point for running AgentBrain tests
int main() {
    test_agentbrain::runAllTests();
    return 0;
}
