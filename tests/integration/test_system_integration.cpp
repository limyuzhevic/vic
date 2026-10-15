// Integration Tests for System Integration
// Comprehensive test suite for system-level integration

#include "tests/utils/TestUtils.hpp"
#include "src/brain/Brain.hpp"
#include "src/agent/AgentBrain.hpp"
#include "src/environment/Environment.hpp"
#include "src/agent/AgentBody.hpp"
#include "src/experiments/ExperimentManager.hpp"
#include <cassert>
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <algorithm>

namespace test_system_integration {

void testSystemIntegration() {
    TestUtils::TestSession session("testSystemIntegration");
    
    // Create core system components
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_unique<nlm::Brain>(config);
    auto agentBrain = std::make_unique<nlm::AgentBrain>();
    auto environment = std::make_unique<nlm::Environment>();
    auto agentBody = std::make_unique<nlm::AgentBody>();
    
    // Configure components
    brain->initialize();
    agentBrain->initialize();
    environment->initialize();
    
    // Create integration
    agentBrain->setBrain(std::move(brain));
    agentBrain->setEnvironment(std::move(environment));
    
    agentBody->setAgentBrain(std::move(agentBrain));
    
    // Simulate system integration
    double startTime = TestUtils::now().time_since_epoch().count();
    
    for (int step = 0; step < 100; ++step) {
        // System step
        double time = step * 0.1;
        
        // Agent brain processes sensory input
        agentBody->update(time, time * 0.1);
        
        // Environment updates
        auto state = agentBody->getCurrentState();
        agentBody->updateEnvironment(state, time);
    }
    
    double endTime = TestUtils::now().time_since_epoch().count();
    double durationMs = (endTime - startTime) * 0.001;
    
    // Integration should be stable
    TestAssert::true(durationMs < 100.0, "System integration should be efficient");
    
    std::cout << "    testSystemIntegration passed" << std::endl;
    session.setSuccess(true);
}

void testBrainEnvironmentIntegration() {
    TestUtils::TestSession session("testBrainEnvironmentIntegration");
    
    // Create brain and environment
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_unique<nlm::Brain>(config);
    auto environment = std::make_unique<nlm::Environment>();
    
    // Initialize components
    brain->initialize();
    environment->initialize();
    
    // Create integration
    brain->setEnvironment(std::move(environment));
    
    // Simulate interaction
    double startTime = TestUtils::now().time_since_epoch().count();
    
    for (int step = 0; step < 50; ++step) {
        double time = step * 0.1;
        
        // Brain processes inputs
        auto observation = brain->getObservation(time);
        
        // Environment provides feedback
        double reward = brain->getReward();
        bool done = brain->isDone();
        
        // Brain updates
        brain->step(time, reward);
        
        if (done) break;
    }
    
    double endTime = TestUtils::now().time_since_epoch().count();
    double durationMs = (endTime - startTime) * 0.001;
    
    // Integration should be stable
    TestAssert::true(durationMs < 50.0, "Brain-environment integration should be efficient");
    
    std::cout << "    testBrainEnvironmentIntegration passed" << std::endl;
    session.setSuccess(true);
}

void testAgentSystemIntegration() {
    TestUtils::TestSession session("testAgentSystemIntegration");
    
    // Create agent system components
    auto agentBrain = std::make_unique<nlm::AgentBrain>();
    auto agentBody = std::make_unique<nlm::AgentBody>();
    auto environment = std::make_unique<nlm::Environment>();
    
    // Initialize components
    agentBrain->initialize();
    agentBody->initialize();
    environment->initialize();
    
    // Create integration
    agentBrain->setEnvironment(std::move(environment));
    agentBody->setAgentBrain(std::move(agentBrain));
    
    // Simulate agent system integration
    double startTime = TestUtils::now().time_since_epoch().count();
    
    for (int step = 0; step < 100; ++step) {
        double time = step * 0.1;
        
        // Agent brain processes sensory input
        auto state = agentBody->getCurrentState();
        
        // Agent body processes output
        auto action = agentBody->getAction(time);
        
        // Environment responds
        auto nextState = agentBody->updateEnvironment(state, time);
        
        // Agent brain learns
        agentBody->update(time, action.magnitude);
    }
    
    double endTime = TestUtils::now().time_since_epoch().count();
    double durationMs = (endTime - startTime) * 0.001;
    
    // Integration should be stable
    TestAssert::true(durationMs < 100.0, "Agent system integration should be efficient");
    
    std::cout << "    testAgentSystemIntegration passed" << std::endl;
    session.setSuccess(true);
}

void testExperimentSystemIntegration() {
    TestUtils::TestSession session("testExperimentSystemIntegration");
    
    // Create experiment manager
    auto experimentManager = std::make_unique<nlm::ExperimentManager>();
    
    // Create brain and environment for experiments
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_unique<nlm::Brain>(config);
    auto environment = std::make_unique<nlm::Environment>();
    
    // Initialize components
    brain->initialize();
    environment->initialize();
    
    // Create experiment configuration
    nlm::ExperimentManager::ExperimentConfig experimentConfig;
    experimentConfig.name = "integration_test_exp";
    experimentConfig.description = "Test system integration with experiment management";
    experimentConfig.parameters = {
        {"learning_rate", "0.01"},
        {"population_size", "50"},
        {"max_steps", "100"}
    };
    experimentConfig.seed = 42;
    experimentConfig.maxSteps = 100;
    experimentConfig.enabled = true;
    
    // Create experiment
    bool createSuccess = experimentManager->createExperiment("integration_test_exp", experimentConfig);
    TestAssert::true(createSuccess, "Should create experiment for integration test");
    
    // Run experiment (simplified)
    bool runSuccess = experimentManager->runExperiment("integration_test_exp", std::move(brain), std::move(environment), 100);
    TestAssert::true(runSuccess, "Should run experiment for integration test");
    
    std::cout << "    testExperimentSystemIntegration passed" << std::endl;
    session.setSuccess(true);
}

void testComponentCommunication() {
    TestUtils::TestSession session("testComponentCommunication");
    
    // Create multiple components
    auto agentBrain = std::make_unique<nlm::AgentBrain>();
    auto agentBody = std::make_unique<nlm::AgentBody>();
    auto config = std::make_shared<nlm::Config>();
    
    // Initialize components
    agentBrain->initialize();
    agentBody->initialize();
    
    // Set up communication
    agentBrain->setAgentBody(std::move(agentBody));
    agentBrain->setConfig(config);
    
    // Simulate communication
    double startTime = TestUtils::now().time_since_epoch().count();
    
    for (int step = 0; step < 50; ++step) {
        double time = step * 0.1;
        
        // Components communicate
        auto sensoryInput = agentBrain->getSensoryInput();
        agentBrain->processSensoryInput(sensoryInput);
        
        auto motorOutput = agentBrain->getMotorOutput();
        agentBrain->processMotorOutput(motorOutput);
        
        // State updates
        agentBrain->update(time, time * 0.1);
    }
    
    double endTime = TestUtils::now().time_since_epoch().count();
    double durationMs = (endTime - startTime) * 0.001;
    
    // Communication should be reliable
    TestAssert::true(durationMs < 50.0, "Component communication should be efficient");
    
    std::cout << "    testComponentCommunication passed" << std::endl;
    session.setSuccess(true);
}

void testMemoryManagementIntegration() {
    TestUtils::TestSession session("testMemoryManagementIntegration");
    
    // Create components with memory management
    auto agentBrain = std::make_unique<nlm::AgentBrain>();
    auto config = std::make_shared<nlm::Config>();
    
    // Initialize components
    agentBrain->initialize();
    
    // Set up memory management
    agentBrain->setConfig(config);
    
    // Simulate memory operations
    double startTime = TestUtils::now().time_since_epoch().count();
    
    for (int step = 0; step < 100; ++step) {
        double time = step * 0.1;
        
        // Store episodic memory
        nlm::EpisodicMemoryEntry episodicEntry;
        episodicEntry.id = "episodic_" + std::to_string(step);
        episodicEntry.content = "Test episodic memory entry " + std::to_string(step);
        episodicEntry.timestamp = time;
        episodicEntry.importance = 0.5 + (TestUtils::randomDouble() * 0.5);
        
        agentBrain->storeEpisodicMemory(episodicEntry);
        
        // Store semantic memory
        nlm::SemanticMemoryEntry semanticEntry;
        semanticEntry.concept = "concept_" + std::to_string(step % 10);
        semanticEntry.definition = "Definition for concept " + std::to_string(step % 10);
        semanticEntry.saliency = 0.3 + (TestUtils::randomDouble() * 0.7);
        
        agentBrain->storeSemanticMemory(semanticEntry);
        
        // Retrieve memory
        auto episodicMemory = agentBrain->getEpisodicMemory();
        auto semanticMemory = agentBrain->getSemanticMemory();
        
        // Update agent state
        agentBrain->update(time, time * 0.1);
    }
    
    double endTime = TestUtils::now().time_since_epoch().count();
    double durationMs = (endTime - startTime) * 0.001;
    
    // Memory management should be efficient
    TestAssert::true(durationMs < 100.0, "Memory management integration should be efficient");
    
    std::cout << "    testMemoryManagementIntegration passed" << std::endl;
    session.setSuccess(true);
}

void testStatePersistenceIntegration() {
    TestUtils::TestSession session("testStatePersistenceIntegration");
    
    // Create agent brain
    auto agentBrain = std::make_unique<nlm::AgentBrain>();
    
    // Initialize and configure
    agentBrain->initialize();
    
    // Test state checkpointing
    agentBrain->enableStateCheckpointing(true);
    
    // Create checkpoint
    bool checkpointSuccess = agentBrain->saveStateCheckpoint("integration_checkpoint");
    TestAssert::true(checkpointSuccess, "Should save state checkpoint");
    
    // Modify state
    double time = 10.0;
    agentBrain->update(time, time * 0.1);
    
    // Load checkpoint
    bool loadSuccess = agentBrain->loadStateCheckpoint("integration_checkpoint");
    TestAssert::true(loadSuccess, "Should load state checkpoint");
    
    // Verify state was restored
    auto episodeInfo = agentBrain->getCurrentEpisodeInfo();
    TestAssert::true(episodeInfo.episodeId > 0, "Episode should have ID after checkpoint load");
    
    std::cout << "    testStatePersistenceIntegration passed" << std::endl;
    session.setSuccess(true);
}

void testRealTimeIntegration() {
    TestUtils::TestSession session("testRealTimeIntegration");
    
    // Create agent system
    auto agentBrain = std::make_unique<nlm::AgentBrain>();
    auto agentBody = std::make_unique<nlm::AgentBody>();
    
    // Initialize components
    agentBrain->initialize();
    agentBody->initialize();
    
    // Set up integration
    agentBrain->setAgentBody(std::move(agentBody));
    
    // Simulate real-time integration
    double startTime = TestUtils::now().time_since_epoch().count();
    
    for (int step = 0; step < 200; ++step) {
        double time = step * 0.05; // 50ms steps
        
        // Real-time processing
        auto action = agentBrain->getAction(time);
        agentBrain->update(time, action.magnitude);
        
        // Simulate variable processing time
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    double endTime = TestUtils::now().time_since_epoch().count();
    double durationMs = (endTime - startTime) * 0.001;
    
    // Real-time integration should be stable
    TestAssert::true(durationMs < 200.0, "Real-time integration should be stable");
    
    std::cout << "    testRealTimeIntegration passed" << std::endl;
    session.setSuccess(true);
}

void runAll() {
    std::cout << "=== System Integration Tests ===" << std::endl;
    
    testSystemIntegration();
    testBrainEnvironmentIntegration();
    testAgentSystemIntegration();
    testExperimentSystemIntegration();
    testComponentCommunication();
    testMemoryManagementIntegration();
    testStatePersistenceIntegration();
    testRealTimeIntegration();
    
    std::cout << std::endl;
    std::cout << "=== All System Integration Tests PASSED ===" << std::endl;
}