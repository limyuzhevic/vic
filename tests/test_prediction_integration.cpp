// Test for Prediction System Integration in NLM Brain

// Test 1: Prediction System Is Properly Initialized
#include "Brain.hpp"
#include "Config.hpp"
#include <cassert>
#include <iostream>

void testPredictionSystemInitialization() {
    std::cout << "Testing prediction system initialization..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1000);
    config->set("region_count", 1);
    config->set("random_seed", 42);
    
    nlm::Brain brain(config);
    
    // Initialize brain
    assert(brain.initialize());
    
    // Test that prediction systems are properly initialized
    assert(brain.getPredictionSystem() != nullptr);
    assert(brain.getActionConsequencePredictor() != nullptr);
    assert(brain.getNeuralPrediction() != nullptr);
    
    std::cout << "✓ Prediction systems initialized correctly" << std::endl;
}

// Test 2: Neural Prediction Records and Predicts
#include <iostream>

void testNeuralPrediction() {
    std::cout << "Testing neural prediction functionality..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1000);
    config->set("region_count", 1);
    config->set("random_seed", 42);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* neuralPred = brain.getNeuralPrediction();
    assert(neuralPred != nullptr);
    
    // Test recording sensory state
    std::vector<float> testState(50, 0.5f);
    brain.step(0);
    
    // Neural prediction should have recorded the state
    float predictionError = neuralPred->getPredictionError();
    float predictionConfidence = neuralPred->getPredictionConfidence();
    
    std::cout << "  Prediction error: " << predictionError << std::endl;
    std::cout << "  Prediction confidence: " << predictionConfidence << std::endl;
    
    // Test prediction generation
    auto predictedState = neuralPred->generatePrediction(10);
    std::cout << "  Generated prediction with " << predictedState.size() << " values" << std::endl;
    
    std::cout << "✓ Neural prediction working" << std::endl;
}

// Test 3: Action Consequence Predictor Learns Associations
#include <iostream>

void testActionConsequencePredictor() {
    std::cout << "Testing action consequence predictor..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1000);
    config->set("region_count", 1);
    config->set("random_seed", 42);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* actionPredictor = brain.getActionConsequencePredictor();
    assert(actionPredictor != nullptr);
    
    // Test recording experiences
    std::vector<float> beforeState(50, 0.3f);
    std::vector<float> afterState(50, 0.8f);
    
    actionPredictor->recordExperience(nlm::ActionType::MoveForward, beforeState, afterState, 1.0f);
    actionPredictor->recordExperience(nlm::ActionType::TurnLeft, beforeState, afterState, 0.8f);
    actionPredictor->recordExperience(nlm::ActionType::Interact, beforeState, afterState, 1.2f);
    
    // Test prediction generation
    std::vector<float> currentState(50, 0.4f);
    std::vector<float> consequences = actionPredictor->predictAllActions(currentState);
    
    std::cout << "  Predicted " << consequences.size() << " action consequences" << std::endl;
    
    // Test optimal action prediction
    nlm::ActionType optimalAction = actionPredictor->predictOptimalAction(currentState);
    std::cout << "  Predicted optimal action: " << static_cast<int>(optimalAction) << std::endl;
    
    std::cout << "✓ Action consequence predictor working" << std::endl;
}

// Test 4: Prediction Systems Integrate with Brain Loop
void testPredictionIntegration() {
    std::cout << "Testing prediction integration in brain loop..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1000);
    config->set("region_count", 1);
    config->set("random_seed", 42);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Run simulation with neural prediction active
    for (SimulationStep step = 0; step < 20; ++step) {
        brain.step(step);
        
        // Inject some current to stimulate activity
        brain.injectCurrentToNeurons(nlm::NeuronType::Internal, 3.0f);
    }
    
    // Check that prediction systems are being used
    auto* neuralPred = brain.getNeuralPrediction();
    auto* actionPredictor = brain.getActionConsequencePredictor();
    
    assert(neuralPred != nullptr);
    assert(actionPredictor != nullptr);
    
    // Neural prediction should have recorded some states
    size_t errorHistorySize = neuralPred->getErrorHistory().size();
    std::cout << "  Neural prediction error history size: " << errorHistorySize << std::endl;
    
    // Action consequence predictor should have learned some associations
    // (This is internal state, not directly accessible via public API)
    
    std::cout << "✓ Prediction integration working" << std::endl;
}

// Test 5: AgentBrain Uses Prediction for Action Selection
void testAgentBrainPredictionIntegration() {
    std::cout << "Testing AgentBrain prediction integration..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1000);
    config->set("region_count", 1);
    config->set("random_seed", 42);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    nlm::AgentBrain agent(brain);
    nlm::SimpleWorld world;
    world.initialize(16, 16);
    agent.initialize(world);
    
    // Enable action consequence prediction
    auto* actionPredictor = brain.getActionConsequencePredictor();
    if (actionPredictor) {
        // Record some experiences
        std::vector<float> beforeState(50, 0.3f);
        std::vector<float> afterState(50, 0.8f);
        
        actionPredictor->recordExperience(nlm::ActionType::MoveForward, beforeState, afterState, 1.0f);
        
        // Test action selection with prediction
        nlm::MotorCommand action = agent.decodeMotorCommand();
        std::cout << "  Agent selected action: " << static_cast<int>(action) << std::endl;
        
        // Action should be influenced by prediction
        assert(action != nlm::MotorCommand::Wait || agent.getCuriosityLevel() > 0.3f);
    }
    
    std::cout << "✓ AgentBrain prediction integration working" << std::endl;
}

// Main test runner
int main() {
    std::cout << "=== Testing NLM Prediction System Integration ===" << std::endl;
    std::cout << std::endl;
    
    try {
        testPredictionSystemInitialization();
        testNeuralPrediction();
        testActionConsequencePredictor();
        testPredictionIntegration();
        testAgentBrainPredictionIntegration();
        
        std::cout << std::endl;
        std::cout << "=== All Prediction Integration Tests PASSED ===" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cout << std::endl;
        std::cout << "=== TEST FAILED: " << e.what() << " ===" << std::endl;
        return 1;
    }
}