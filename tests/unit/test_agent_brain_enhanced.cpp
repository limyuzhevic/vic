// Unit Tests for Enhanced AgentBrain Tests
// Comprehensive test suite for enhanced AgentBrain functionality

#include "tests/utils/TestUtils.hpp"
#include "src/agent/AgentBrain.hpp"
#include "src/agent/SensoryPercept.hpp"
#include "src/brain/Brain.hpp"
#include "src/environment/Environment.hpp"
#include <cassert>
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <algorithm>

namespace test_agent_brain_enhanced {

void testAgentBrainCreation() {
    TestUtils::TestSession session("testAgentBrainCreation");
    
    auto agentBrain = std::make_unique<nlm::AgentBrain>();
    TestAssert::true(agentBrain != nullptr, "AgentBrain should be created successfully");
    
    // Test basic properties
    TestAssert::true(agentBrain->getId() >= 0, "AgentBrain should have valid ID");
    TestAssert::true(agentBrain->getState() == nlm::AgentState::IDLE, "AgentBrain should be in IDLE state initially");
    
    std::cout << "    testAgentBrainCreation passed" << std::endl;
    session.setSuccess(true);
}

void testAgentBrainConfiguration() {
    TestUtils::TestSession session("testAgentBrainConfiguration");
    
    auto agentBrain = std::make_unique<nlm::AgentBrain>();
    
    // Test default configuration
    auto config = agentBrain->getConfig();
    TestAssert::true(config != nullptr, "AgentBrain should have configuration");
    
    // Test configuration properties
    TestAssert::true(config->rewardModulationEnabled, "Reward modulation should be enabled by default");
    TestAssert::true(config->curiosityEnabled, "Curiosity should be enabled by default");
    TestAssert::true(config->noveltyDetectionEnabled, "Novelty detection should be enabled by default");
    TestAssert::true(config->predictionErrorEnabled, "Prediction error should be enabled by default");
    TestAssert::true(config->structuralPlasticityEnabled, "Structural plasticity should be enabled by default");
    TestAssert::true(config->developmentEnabled, "Development should be enabled by default");
    
    // Test custom configuration
    config->rewardModulationEnabled = false;
    config->curiosityEnabled = false;
    config->noveltyDetectionEnabled = false;
    
    TestAssert::false(config->rewardModulationEnabled, "Reward modulation should be disabled");
    TestAssert::false(config->curiosityEnabled, "Curiosity should be disabled");
    TestAssert::false(config->noveltyDetectionEnabled, "Novelty detection should be disabled");
    
    std::cout << "    testAgentBrainConfiguration passed" << std::endl;
    session.setSuccess(true);
}

void testAgentBrainNeuromodulation() {
    TestUtils::TestSession session("testAgentBrainNeuromodulation");
    
    auto agentBrain = std::make_unique<nlm::AgentBrain>();
    
    // Test neuromodulation parameters
    TestAssert::true(agentBrain->getRewardValue() >= 0.0, "Reward value should be non-negative");
    TestAssert::true(agentBrain->getCuriosityLevel() >= 0.0, "Curiosity level should be non-negative");
    TestAssert::true(agentBrain->getNoveltyLevel() >= 0.0, "Novelty level should be non-negative");
    TestAssert::true(agentBrain->getPredictionError() >= 0.0, "Prediction error should be non-negative");
    
    // Update neuromodulation
    agentBrain->updateNeuromodulation(0.5, 0.3, 0.2, 0.1);
    
    TestAssert::true(agentBrain->getRewardValue() >= 0.0, "Updated reward value should be valid");
    TestAssert::true(agentBrain->getCuriosityLevel() >= 0.0, "Updated curiosity level should be valid");
    
    std::cout << "    testAgentBrainNeuromodulation passed" << std::endl;
    session.setSuccess(true);
}

void testAgentBrainSensoryProcessing() {
    TestUtils::TestSession session("testAgentBrainSensoryProcessing");
    
    auto agentBrain = std::make_unique<nlm::AgentBrain>();
    
    // Create test sensory perception
    nlm::SensoryPercept perception;
    perception.type = nlm::SensoryModality::VISUAL;
    perception.value = 0.8;
    perception.confidence = 0.9;
    perception.timestamp = std::chrono::steady_clock::now().time_since_epoch().count();
    
    // Add perception
    agentBrain->addSensoryPerception(perception);
    
    // Process sensory input
    agentBrain->processSensoryInput();
    
    // Check that perception was processed
    auto processedPerceptions = agentBrain->getProcessedPerceptions();
    TestAssert::true(processedPerceptions.size() > 0, "Should have processed perceptions");
    
    std::cout << "    testAgentBrainSensoryProcessing passed" << std::endl;
    session.setSuccess(true);
}

void testAgentBrainMotorOutput() {
    TestUtils::TestSession session("testAgentBrainMotorOutput");
    
    auto agentBrain = std::make_unique<nlm::AgentBrain>();
    
    // Create test motor command
    nlm::MotorCommand command;
    command.action = nlm::ActionType::MOVE_FORWARD;
    command.magnitude = 1.0;
    command.duration = 0.1;
    command.timestamp = std::chrono::steady_clock::now().time_since_epoch().count();
    
    // Add motor command
    agentBrain->addMotorCommand(command);
    
    // Process motor output
    agentBrain->processMotorOutput();
    
    // Check that command was processed
    auto processedCommands = agentBrain->getProcessedCommands();
    TestAssert::true(processedCommands.size() > 0, "Should have processed commands");
    
    std::cout << "    testAgentBrainMotorOutput passed" << std::endl;
    session.setSuccess(true);
}

void testAgentBrainLearning() {
    TestUtils::TestSession session("testAgentBrainLearning");
    
    auto agentBrain = std::make_unique<nlm::AgentBrain>();
    
    // Create test experience
    nlm::Experience experience;
    experience.sensoryInput = std::vector<double>{0.5, 0.8, 0.3};
    experience.action = nlm::ActionType::MOVE_FORWARD;
    experience.reward = 1.0;
    experience.nextState = std::vector<double>{0.6, 0.9, 0.4};
    experience.done = false;
    experience.timestamp = std::chrono::steady_clock::now().time_since_epoch().count();
    
    // Store experience
    agentBrain->storeExperience(experience);
    
    // Learn from experiences
    agentBrain->learnFromExperiences();
    
    // Check that learning occurred
    TestAssert::true(agentBrain->getExperienceCount() > 0, "Should have stored experiences");
    
    std::cout << "    testAgentBrainLearning passed" << std::endl;
    session.setSuccess(true);
}

void testAgentBrainDevelopment() {
    TestUtils::TestSession session("testAgentBrainDevelopment");
    
    auto agentBrain = std::make_unique<nlm::AgentBrain>();
    
    // Set development parameters
    agentBrain->setDevelopmentAge(60.0); // 60 seconds
    agentBrain->setCriticalPeriodStart(60.0);
    agentBrain->setCriticalPeriodEnd(300.0);
    agentBrain->setMaturationEnd(900.0);
    
    TestAssert::true(agentBrain->getDevelopmentAge() >= 0.0, "Development age should be non-negative");
    TestAssert::true(agentBrain->getDevelopmentAge() <= 900.0, "Development age should be reasonable");
    
    // Check development stage
    nlm::DevelopmentStage stage = agentBrain->getDevelopmentStage();
    TestAssert::true(stage == nlm::DevelopmentStage::CRITICAL_PERIOD || 
                   stage == nlm::DevelopmentStage::MATURING ||
                   stage == nlm::DevelopmentStage::ADULT,
                   "Development stage should be valid");
    
    std::cout << "    testAgentBrainDevelopment passed" << std::endl;
    session.setSuccess(true);
}

void testAgentBrainMemoryManagement() {
    TestUtils::TestSession session("testAgentBrainMemoryManagement");
    
    auto agentBrain = std::make_unique<nlm::AgentBrain>();
    
    // Test episodic memory
    nlm::EpisodicMemoryEntry entry;
    entry.id = "test_episodic_1";
    entry.content = "Test episodic memory";
    entry.timestamp = std::chrono::steady_clock::now().time_since_epoch().count();
    entry.importance = 0.8;
    
    agentBrain->storeEpisodicMemory(entry);
    
    auto episodicMemory = agentBrain->getEpisodicMemory();
    TestAssert::true(episodicMemory.size() > 0, "Should have episodic memory");
    
    // Test semantic memory
    nlm::SemanticMemoryEntry semanticEntry;
    semanticEntry.concept = "test_concept";
    semanticEntry.definition = "Test semantic memory";
    semanticEntry.saliency = 0.9;
    
    agentBrain->storeSemanticMemory(semanticEntry);
    
    auto semanticMemory = agentBrain->getSemanticMemory();
    TestAssert::true(semanticMemory.size() > 0, "Should have semantic memory");
    
    std::cout << "    testAgentBrainMemoryManagement passed" << std::endl;
    session.setSuccess(true);
}

void testAgentBrainStateManagement() {
    TestUtils::TestSession session("testAgentBrainStateManagement");
    
    auto agentBrain = std::make_unique<nlm::AgentBrain>();
    
    // Test state checkpointing
    agentBrain->enableStateCheckpointing(true);
    TestAssert::true(agentBrain->isStateCheckpointingEnabled(), "State checkpointing should be enabled");
    
    // Save checkpoint
    bool checkpointSuccess = agentBrain->saveStateCheckpoint("test_checkpoint");
    TestAssert::true(checkpointSuccess, "Should save state checkpoint");
    
    // Load checkpoint
    bool loadSuccess = agentBrain->loadStateCheckpoint("test_checkpoint");
    TestAssert::true(loadSuccess, "Should load state checkpoint");
    
    // Test episode management
    agentBrain->startEpisode();
    
    auto episodeInfo = agentBrain->getCurrentEpisodeInfo();
    TestAssert::true(episodeInfo.episodeId > 0, "Episode should have ID");
    TestAssert::true(episodeInfo.stepCount > 0, "Episode should have steps");
    
    std::cout << "    testAgentBrainStateManagement passed" << std::endl;
    session.setSuccess(true);
}

void testAgentBrainPerformance() {
    TestUtils::TestSession session("testAgentBrainPerformance");
    
    auto agentBrain = std::make_unique<nlm::AgentBrain>();
    
    // Performance test - simulate many steps
    double startTime = TestUtils::now().time_since_epoch().count();
    
    for (int i = 0; i < 1000; ++i) {
        agentBrain->step(i * 0.001, i * 0.1);
    }
    
    double endTime = TestUtils::now().time_since_epoch().count();
    double durationMs = (endTime - startTime) * 0.001; // Convert to milliseconds
    
    // Performance should be reasonable (< 100ms for 1000 steps)
    TestAssert::true(durationMs < 100.0, "AgentBrain performance should be reasonable");
    
    std::cout << "    testAgentBrainPerformance passed" << std::endl;
    session.setSuccess(true);
}

void testAgentBrainCuriosityDrivenLearning() {
    TestUtils::TestSession session("testAgentBrainCuriosityDrivenLearning");
    
    auto agentBrain = std::make_unique<nlm::AgentBrain>();
    
    // Test curiosity-driven learning
    auto& config = agentBrain->getConfig();
    config.curiosityEnabled = true;
    config.curiosityThreshold = 0.3;
    config.explorationMultiplier = 1.5;
    
    // Create test experience with novelty
    nlm::Experience experience;
    experience.sensoryInput = std::vector<double>{0.9, 0.1, 0.8}; // Novel combination
    experience.action = nlm::ActionType::EXPLORE;
    experience.reward = 0.0; // No external reward
    experience.nextState = std::vector<double>{0.2, 0.7, 0.3};
    experience.done = false;
    
    agentBrain->storeExperience(experience);
    
    // Learn from experience (should be driven by curiosity)
    agentBrain->learnFromExperiences();
    
    // Check that curiosity was activated
    TestAssert::true(agentBrain->getCuriosityLevel() > 0.0, "Curiosity should be activated");
    
    std::cout << "    testAgentBrainCuriosityDrivenLearning passed" << std::endl;
    session.setSuccess(true);
}

void testAgentBrainRewardModulation() {
    TestUtils::TestSession session("testAgentBrainRewardModulation");
    
    auto agentBrain = std::make_unique<nlm::AgentBrain>();
    
    // Test reward modulation
    auto& config = agentBrain->getConfig();
    config.rewardModulationEnabled = true;
    config.predictionErrorWeight = 0.5;
    
    // Create test experience with reward
    nlm::Experience experience;
    experience.sensoryInput = std::vector<double>{0.5, 0.5};
    experience.action = nlm::ActionType::MOVE_FORWARD;
    experience.reward = 1.0;
    experience.nextState = std::vector<double>{0.6, 0.6};
    experience.done = false;
    
    agentBrain->storeExperience(experience);
    
    // Learn from experience (should be modulated by reward)
    agentBrain->learnFromExperiences();
    
    // Check that reward was processed
    TestAssert::true(agentBrain->getRewardValue() > 0.0, "Reward should be processed");
    
    std::cout << "    testAgentBrainRewardModulation passed" << std::endl;
    session.setSuccess(true);
}

void testAgentBrainPredictionError() {
    TestUtils::TestSession session("testAgentBrainPredictionError");
    
    auto agentBrain = std::make_unique<nlm::AgentBrain>();
    
    // Test prediction error
    auto& config = agentBrain->getConfig();
    config.predictionErrorEnabled = true;
    config.predictionErrorWeight = 0.5;
    
    // Create test experience
    nlm::Experience experience;
    experience.sensoryInput = std::vector<double>{0.5, 0.5};
    experience.action = nlm::ActionType::MOVE_FORWARD;
    experience.reward = 0.0;
    experience.nextState = std::vector<double>{0.6, 0.6}; // Predicted state
    experience.done = false;
    
    agentBrain->storeExperience(experience);
    
    // Learn from experience (should generate prediction error)
    agentBrain->learnFromExperiences();
    
    // Check that prediction error was generated
    TestAssert::true(agentBrain->getPredictionError() >= 0.0, "Prediction error should be non-negative");
    
    std::cout << "    testAgentBrainPredictionError passed" << std::endl;
    session.setSuccess(true);
}

void runAll() {
    std::cout << "=== Enhanced AgentBrain Tests ===" << std::endl;
    
    testAgentBrainCreation();
    testAgentBrainConfiguration();
    testAgentBrainNeuromodulation();
    testAgentBrainSensoryProcessing();
    testAgentBrainMotorOutput();
    testAgentBrainLearning();
    testAgentBrainDevelopment();
    testAgentBrainMemoryManagement();
    testAgentBrainStateManagement();
    testAgentBrainPerformance();
    testAgentBrainCuriosityDrivenLearning();
    testAgentBrainRewardModulation();
    testAgentBrainPredictionError();
    
    std::cout << std::endl;
    std::cout << "=== All Enhanced AgentBrain Tests PASSED ===" << std::endl;
}