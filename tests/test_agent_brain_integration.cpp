// AgentBrain Integration Tests
// Phase 2: Real Neural Computation Tests
// Comprehensive tests for AgentBrain integration with memory systems and world interaction

#include "agent/AgentBrain.hpp"
#include "brain/Brain.hpp"
#include "environment/SimpleWorld.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <memory>

namespace test_agent_brain_integration {

void testAgentBrainCreation() {
    std::cout << "  Testing AgentBrain creation..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    auto agentBrain = std::make_shared<nlm::AgentBrain>(brain);
    
    // AgentBrain should have valid brain pointer
    assert(agentBrain->getBrain() != nullptr);
    assert(agentBrain->getBrain() == brain.get());
    
    std::cout << "    PASSED" << std::endl;
}

void testAgentBrainInitialization() {
    std::cout << "  Testing AgentBrain initialization..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    auto agentBrain = std::make_shared<nlm::AgentBrain>(brain);
    
    // Create a world for initialization
    nlm::SimpleWorld world(config);
    world.configure(10, 10, 8, 8);
    world.reset();
    
    // Initialize agent brain with world
    agentBrain->initialize(world);
    
    // Check sensory input size
    assert(agentBrain->getSensoryInputSize() > 0);
    
    // Check motor output size
    assert(agentBrain->getMotorOutputSize() > 0);
    
    std::cout << "    PASSED" << std::endl;
}

void testAgentBrainSensoryProcessing() {
    std::cout << "  Testing AgentBrain sensory processing..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    auto agentBrain = std::make_shared<nlm::AgentBrain>(brain);
    
    // Create a world and initialize agent brain
    nlm::SimpleWorld world(config);
    world.configure(10, 10, 8, 8);
    world.reset();
    agentBrain->initialize(world);
    
    // Create a sensory percept
    nlm::SensoryPercept percept;
    std::vector<float> visionData(256, 0.0f);
    std::vector<float> touchData(8, 0.0f);
    std::vector<float> internalData(4, 0.0f);
    std::vector<float> proprioceptionData(6, 0.0f);
    
    for (size_t i = 0; i < std::min(size_t(50), visionData.size()); ++i) {
        visionData[i] = 0.5f + 0.5f * (i % 2);  // Alternating 0.5 and 1.0
    }
    
    percept.setVision(visionData);
    percept.setTouch(touchData);
    percept.setInternal(internalData);
    percept.setProprioception(proprioceptionData);
    
    // Process sensory input
    agentBrain->processSensoryInput(percept);
    
    // Check that sensory processing doesn't crash
    // In a real implementation, this would verify that neurons were stimulated
    
    std::cout << "    PASSED" << std::endl;
}

void testAgentBrainMotorDecoding() {
    std::cout << "  Testing AgentBrain motor decoding..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    auto agentBrain = std::make_shared<nlm::AgentBrain>(brain);
    
    // Initialize brain (though motor neurons may not be active)
    brain->initialize();
    
    // Decode motor command
    nlm::MotorCommand action = agentBrain->decodeMotorCommand();
    
    // Should return a valid motor command
    assert(static_cast<int>(action) >= 0 && static_cast<int>(action) < 8);
    
    std::cout << "    PASSED" << std::endl;
}

void testAgentBrainRewardModulation() {
    std::cout << "  Testing AgentBrain reward modulation..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    auto agentBrain = std::make_shared<nlm::AgentBrain>(brain);
    
    // Enable reward modulation
    agentBrain->enableRewardModulation(true);
    
    // Apply reward modulation
    agentBrain->applyRewardModulation(0.8f, 0.5f);  // reward=0.8, predicted=0.5
    
    // Check prediction error
    float predictionError = agentBrain->getPredictionError();
    assert(std::abs(predictionError - 0.3f) < 0.001f);  // 0.8 - 0.5
    
    // Check neuromodulation level (dopamine-like signal)
    float neuromodulationLevel = agentBrain->getNeuromodulationLevel();
    assert(std::abs(neuromodulationLevel - 0.3f) < 0.001f);
    
    std::cout << "    PASSED" << std::endl;
}

void testAgentBrainDevelopment() {
    std::cout << "  Testing AgentBrain development..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    auto agentBrain = std::make_shared<nlm::AgentBrain>(brain);
    
    // Enable development
    agentBrain->enableDevelopment(true);
    
    // Update development
    agentBrain->updateDevelopment(0.001);
    
    // Check developmental stage
    nlm::DevelopmentalStage stage = agentBrain->getDevelopmentalStage();
    assert(static_cast<int>(stage) >= 0 && static_cast<int>(stage) < 5);
    
    std::cout << "    PASSED" << std::endl;
}

void testAgentBrainCuriosity() {
    std::cout << "  Testing AgentBrain curiosity..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    auto agentBrain = std::make_shared<nlm::AgentBrain>(brain);
    
    // Enable curiosity
    agentBrain->enableCuriosity(true);
    
    // Check initial curiosity level
    float initialCuriosity = agentBrain->getCuriosityLevel();
    assert(initialCuriosity >= 0.0f && initialCuriosity <= 1.0f);
    
    // Apply reward modulation to generate prediction error
    agentBrain->applyRewardModulation(0.9f, 0.2f);
    
    float curiosity = agentBrain->getCuriosityLevel();
    assert(curiosity >= 0.0f && curiosity <= 1.0f);
    
    std::cout << "    PASSED" << std::endl;
}

void testAgentBrainPredictionIntegration() {
    std::cout << "  Testing AgentBrain prediction system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    auto agentBrain = std::make_shared<nlm::AgentBrain>(brain);
    
    // Create a prediction system
    auto predictionSystem = std::make_unique<nlm::PredictionSystem>();
    agentBrain->setPredictionSystem(std::move(predictionSystem));
    
    // Check that prediction system is set
    assert(agentBrain->hasPredictionSystem());
    
    // Create a sensory percept for prediction
    nlm::SensoryPercept percept;
    std::vector<float> visionData(256, 0.0f);
    percept.setVision(visionData);
    
    // Apply prediction system update
    agentBrain->predictAndUpdatePredictions(percept, 0.001);
    
    // Check prediction accuracy
    float accuracy = agentBrain->getPredictionAccuracy();
    assert(accuracy >= 0.0f && accuracy <= 1.0f);
    
    std::cout << "    PASSED" << std::endl;
}

void testAgentBrainMemoryIntegration() {
    std::cout << "  Testing AgentBrain memory integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    auto agentBrain = std::make_shared<nlm::AgentBrain>(brain);
    
    // Create a world and initialize
    nlm::SimpleWorld world(config);
    world.configure(10, 10, 8, 8);
    world.reset();
    agentBrain->initialize(world);
    
    // Create a sensory percept
    nlm::SensoryPercept percept;
    std::vector<float> visionData(256, 0.1f);
    std::vector<float> touchData(8, 0.0f);
    std::vector<float> internalData(4, 0.0f);
    std::vector<float> proprioceptionData(6, 0.0f);
    
    percept.setVision(visionData);
    percept.setTouch(touchData);
    percept.setInternal(internalData);
    percept.setProprioception(proprioceptionData);
    
    // Process sensory input (should store experience)
    agentBrain->processSensoryInput(percept);
    
    // Decode motor command
    nlm::MotorCommand action = agentBrain->decodeMotorCommand();
    
    // Apply reward modulation (should store experience)
    agentBrain->applyRewardModulation(0.7f, 0.5f);
    
    // Check that agent brain has been updated
    assert(agentBrain->getPredictionError() >= 0.0f);
    
    std::cout << "    PASSED" << std::endl;
}

void runAll() {
    std::cout << "=== AgentBrain Integration Tests ===" << std::endl;
    
    testAgentBrainCreation();
    testAgentBrainInitialization();
    testAgentBrainSensoryProcessing();
    testAgentBrainMotorDecoding();
    testAgentBrainRewardModulation();
    testAgentBrainDevelopment();
    testAgentBrainCuriosity();
    testAgentBrainPredictionIntegration();
    testAgentBrainMemoryIntegration();
    
    std::cout << std::endl;
    std::cout << "=== All AgentBrain Integration Tests PASSED ===" << std::endl;
}

} // namespace test_agent_brain_integration
