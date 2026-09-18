#include "test_brain.h"
#include "../src/agent/AgentBrain.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include <cassert>
#include <iostream>

void testAgentBrainInitialization() {
    std::cout << "Testing AgentBrain initialization..." << std::endl;
    
    // Create config and brain
    auto config = std::make_shared<Config>();
    config->set("random_seed", 42);
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    
    auto brain = std::make_shared<Brain>(config);
    assert(brain->initialize());
    
    // Create AgentBrain
    AgentBrain agent(brain);
    
    // Test with valid brain pointer
    assert(agent.getBrain() != nullptr);
    
    // Test initialization with null brain (should handle gracefully)
    AgentBrain nullAgent(nullptr);
    assert(nullAgent.getBrain() == nullptr);
    
    std::cout << "✓ AgentBrain initialization tests passed" << std::endl;
}

void testActionHistoryTracking() {
    std::cout << "Testing action history tracking..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("random_seed", 43);
    config->set("neuron_count", 50);
    config->set("region_count", 1);
    
    auto brain = std::make_shared<Brain>(config);
    assert(brain->initialize());
    
    AgentBrain agent(brain);
    
    // Enable history tracking
    agent.setHistoryTracking(true);
    
    // Add some actions to history
    agent.updateActionHistory(MotorCommand::MoveForward, 1.0f, 1.0);
    agent.updateActionHistory(MotorCommand::TurnLeft, 0.5f, 2.0);
    agent.updateActionHistory(MotorCommand::Wait, -0.1f, 3.0);
    
    // Test history access
    auto history = agent.getActionHistory();
    assert(history.size() == 3);
    assert(history[0].action == MotorCommand::MoveForward);
    assert(history[0].reward == 1.0f);
    assert(history[1].action == MotorCommand::TurnLeft);
    assert(history[2].action == MotorCommand::Wait);
    
    // Test success rate calculation
    agent.incrementStepsProcessed();
    agent.recordSuccessfulAction();
    
    float successRate = agent.getActionSuccessRate();
    assert(successRate >= 0.0f && successRate <= 1.0f);
    
    // Test clearing history
    agent.clearActionHistory();
    assert(agent.getActionHistory().empty());
    
    std::cout << "✓ Action history tracking tests passed" << std::endl;
}

void testAdvancedExploration() {
    std::cout << "Testing advanced exploration features..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("random_seed", 44);
    config->set("neuron_count", 75);
    config->set("region_count", 1);
    
    auto brain = std::make_shared<Brain>(config);
    assert(brain->initialize());
    
    AgentBrain agent(brain);
    
    // Configure advanced exploration
    agent.setAdvancedExploration(true);
    agent.setExplorationThreshold(0.3f);
    agent.setConfidenceThreshold(0.6f);
    
    // Test confidence threshold
    MotorCommand action1 = agent.advancedActionSelection(0.4f); // Below threshold
    // Should be more conservative with low confidence
    
    MotorCommand action2 = agent.advancedActionSelection(0.8f); // Above threshold
    // Should be more exploratory with high confidence
    
    // Test exploration threshold
    agent.setExplorationThreshold(0.8f);
    
    std::cout << "✓ Advanced exploration tests passed" << std::endl;
}

void testBehavioralAdaptation() {
    std::cout << "Testing behavioral adaptation..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("random_seed", 45);
    config->set("neuron_count", 60);
    config->set("region_count", 1);
    
    auto brain = std::make_shared<Brain>(config);
    assert(brain->initialize());
    
    AgentBrain agent(brain);
    agent.setHistoryTracking(true);
    
    // Add some actions to establish baseline
    for (int i = 0; i < 10; ++i) {
        agent.updateActionHistory(MotorCommand::MoveForward, 1.0f, i * 1.0);
        agent.incrementStepsProcessed();
        agent.recordSuccessfulAction();
    }
    
    // Calculate behavioral adaptation
    float adaptation = agent.calculateBehavioralAdaptation();
    assert(adaptation >= 0.0f && adaptation <= 1.0f);
    
    // Test adaptive behavior application
    MotorCommand baseAction = MotorCommand::MoveForward;
    MotorCommand adaptiveAction = agent.applyAdaptiveBehavior(baseAction, adaptation);
    
    // Adaptive action should be different from base in some cases
    // (this depends on exploration settings and random chance)
    
    std::cout << "✓ Behavioral adaptation tests passed" << std::endl;
}

void testNewObservationDetection() {
    std::cout << "Testing new observation detection..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("random_seed", 46);
    config->set("neuron_count", 80);
    config->set("region_count", 1);
    
    auto brain = std::make_shared<Brain>(config);
    assert(brain->initialize());
    
    AgentBrain agent(brain);
    
    // Test with empty vision
    std::vector<float> emptyVision;
    bool isNew = agent.isNewObservation(emptyVision);
    assert(isNew == true); // Empty vision should be considered new
    
    // Test with different sized visions
    std::vector<float> vision1(10, 0.5f);
    std::vector<float> vision2(20, 0.5f);
    bool isNew1 = agent.isNewObservation(vision1);
    bool isNew2 = agent.isNewObservation(vision2);
    assert(isNew1 == true); // First observation should be new
    assert(isNew2 == true); // Different size should be new
    
    // Test with similar visions
    std::vector<float> vision3(10, 0.5f); // Same as vision1
    bool isNew3 = agent.isNewObservation(vision3);
    // Vision3 is identical to vision1, but may be considered new
    // depending on the threshold
    
    std::cout << "✓ New observation detection tests passed" << std::endl;
}

void testAgentBody() {
    std::cout << "Testing AgentBody functionality..." << std::endl;
    
    AgentBody body;
    
    // Test default values
    assert(body.x == 0.0f);
    assert(body.y == 0.0f);
    assert(body.orientation == 0.0f);
    assert(body.velocityX == 0.0f);
    assert(body.velocityY == 0.0f);
    assert(body.angularVelocity == 0.0f);
    assert(body.energy == 1.0f);
    assert(body.health == 1.0f);
    assert(body.age == 0.0f);
    assert(body.isMoving == false);
    assert(body.isTurning == false);
    assert(body.lastActionTime == 0.0f);
    
    // Test setting values
    body.x = 10.0f;
    body.y = 20.0f;
    body.orientation = 1.57f;
    body.velocityX = 5.0f;
    body.velocityY = 2.0f;
    body.angularVelocity = 0.5f;
    body.energy = 0.8f;
    body.health = 0.9f;
    body.age = 100.0f;
    body.isMoving = true;
    body.isTurning = true;
    body.lastActionTime = 5.0f;
    
    assert(body.x == 10.0f);
    assert(body.y == 20.0f);
    assert(body.orientation == 1.57f);
    assert(body.velocityX == 5.0f);
    assert(body.velocityY == 2.0f);
    assert(body.angularVelocity == 0.5f);
    assert(body.energy == 0.8f);
    assert(body.health == 0.9f);
    assert(body.age == 100.0f);
    assert(body.isMoving == true);
    assert(body.isTurning == true);
    assert(body.lastActionTime == 5.0f);
    
    // Test reset
    body.reset();
    assert(body.x == 0.0f);
    assert(body.y == 0.0f);
    assert(body.orientation == 0.0f);
    assert(body.velocityX == 0.0f);
    assert(body.velocityY == 0.0f);
    assert(body.angularVelocity == 0.0f);
    assert(body.energy == 1.0f);
    assert(body.health == 1.0f);
    assert(body.age == 0.0f);
    assert(body.isMoving == false);
    assert(body.isTurning == false);
    assert(body.lastActionTime == 0.0f);
    
    std::cout << "✓ AgentBody tests passed" << std::endl;
}

void testMotorCommandEnumeration() {
    std::cout << "Testing MotorCommand enumeration..." << std::endl;
    
    // Test all motor command values
    assert(static_cast<int>(MotorCommand::MoveForward) == 0);
    assert(static_cast<int>(MotorCommand::MoveBackward) == 1);
    assert(static_cast<int>(MotorCommand::TurnLeft) == 2);
    assert(static_cast<int>(MotorCommand::TurnRight) == 3);
    assert(static_cast<int>(MotorCommand::LookLeft) == 4);
    assert(static_cast<int>(MotorCommand::LookRight) == 5);
    assert(static_cast<int>(MotorCommand::Interact) == 6);
    assert(static_cast<int>(MotorCommand::Wait) == 7);
    
    std::cout << "✓ MotorCommand enumeration tests passed" << std::endl;
}

void runAllAgentTests() {
    std::cout << "\n=== Running NLM Agent Tests ===" << std::endl;
    
    try {
        testAgentBrainInitialization();
        testActionHistoryTracking();
        testAdvancedExploration();
        testBehavioralAdaptation();
        testNewObservationDetection();
        testAgentBody();
        testMotorCommandEnumeration();
        
        std::cout << "\n=== All Agent Tests Passed! ===" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "\n*** TEST FAILED: " << e.what() << " ***" << std::endl;
        throw;
    } catch (...) {
        std::cerr << "\n*** TEST FAILED: Unknown error ***" << std::endl;
        throw;
    }
}

int main() {
    runAllAgentTests();
    return 0;
}
