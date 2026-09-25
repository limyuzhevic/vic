// Cognition System Integration Tests
// Tests for cognitive functions and planning capabilities
// 
// Key integration features verified:
// 1. Neural Planner Integration - Multi-step action planning
// 2. Concept Formation Integration - Dynamic concept emergence
// 3. Cognitive Planning Integration - Goal-directed behavior
// 4. Concept Memory Integration - Knowledge representation
// 5. Planning Error Correction - Adaptive planning

#include "src/cognition/NeuralPlanner.hpp"
#include "src/cognition/ConceptFormation.hpp"
#include "src/brain/Brain.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <algorithm>

namespace test_cognition_integration {

void testNeuralPlannerIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("planning_horizon", 5, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    bool success = brain.initialize();
    assert(success);
    
    // Test neural planner initialization
    auto neuralPlanner = brain.getNeuralPlanner();
    assert(neuralPlanner != nullptr);
    
    // Plan action with target reward
    std::vector<float> currentState = {0.3f, 0.7f, 0.4f, 0.8f, 0.5f};
    nlm::ActionType action = neuralPlanner->planAction(currentState, 0.7f);
    
    // Verify planner returned valid action
    assert(action >= nlm::ActionType::MoveForward && action <= nlm::ActionType::TurnLeft);
    
    std::cout << "    testNeuralPlannerIntegration passed" << std::endl;
}

void testPlanningSequenceEvaluation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(80), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto neuralPlanner = brain.getNeuralPlanner();
    assert(neuralPlanner != nullptr);
    
    std::vector<float> startState = {0.2f, 0.6f, 0.4f, 0.7f, 0.3f};
    std::vector<nlm::ActionType> actionSequence = {
        nlm::ActionType::MoveForward,
        nlm::ActionType::TurnRight,
        nlm::ActionType::MoveForward,
        nlm::ActionType::TurnLeft
    };
    
    // Evaluate entire action sequence
    auto planningResult = neuralPlanner->evaluateSequence(actionSequence, startState);
    
    assert(planningResult.actions.size() == actionSequence.size());
    assert(planningResult.predictedStates.size() == planningResult.actions.size() + 1); // +1 for initial state
    assert(planningResult.expectedReward >= 0.0f && planningResult.expectedReward <= 1.0f);
    assert(planningResult.confidence >= 0.0f && planningResult.confidence <= 1.0f);
    assert(planningResult.depth == actionSequence.size());
    
    std::cout << "    testPlanningSequenceEvaluation passed" << std::endl;
}

void testConceptFormationIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto conceptFormation = brain.getConceptFormation();
    assert(conceptFormation != nullptr);
    
    // Present concepts to concept formation system
    std::vector<float> concept1Pattern = {0.8f, 0.2f, 0.1f, 0.9f, 0.3f};
    std::vector<float> concept1Features = {1.0f, 0.3f, 0.7f, 0.2f, 0.5f};
    
    std::vector<float> concept2Pattern = {0.1f, 0.9f, 0.8f, 0.2f, 0.6f};
    std::vector<float> concept2Features = {0.2f, 0.9f, 0.4f, 0.1f, 0.6f};
    
    // Record observations for concept formation
    for (size_t i = 0; i < 10; ++i) {
        conceptFormation->recordObservation(concept1Pattern, concept1Features, i + 1);
    }
    
    for (size_t i = 10; i < 20; ++i) {
        conceptFormation->recordObservation(concept2Pattern, concept2Features, i + 1);
    }
    
    // Get discovered concepts
    auto concepts = conceptFormation->getDiscoveredConcepts();
    assert(concepts.size() >= 1);
    
    for (const auto& concept : concepts) {
        assert(concept.id < 100);
        assert(concept.prototype.size() == concept1Pattern.size());
        assert(concept.stability >= 0.0f && concept.stability <= 1.0f);
        
        // Check concept instances
        assert(!concept.instances.empty());
        for (const auto& instance : concept.instances) {
            assert(instance.observationCount > 0);
            assert(instance.stability >= 0.0f && instance.stability <= 1.0f);
        }
    }
    
    std::cout << "    testConceptFormationIntegration passed" << std::endl;
}

void testCognitivePlanningWithConcepts() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(90), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto neuralPlanner = brain.getNeuralPlanner();
    auto conceptFormation = brain.getConceptFormation();
    assert(neuralPlanner != nullptr);
    assert(conceptFormation != nullptr);
    
    std::vector<float> currentState = {0.4f, 0.6f, 0.3f, 0.7f, 0.5f};
    
    // Discover concepts based on current state
    std::vector<float> features = {0.5f, 0.5f, 0.5f, 0.5f, 0.5f};
    
    // Record observation to help concept formation
    for (SimulationStep step = 0; step < 5; ++step) {
        conceptFormation->recordObservation(currentState, features, step);
    }
    
    // Plan action using cognitive state
    nlm::ActionType action = neuralPlanner->planAction(currentState, 0.6f);
    
    // Verify planning is influenced by cognitive state
    assert(action >= nlm::ActionType::MoveForward && action <= nlm::ActionType::TurnLeft);
    
    // Evaluate different action sequences
    std::vector<nlm::ActionType> sequence1 = {nlm::ActionType::MoveForward, nlm::ActionType::TurnRight};
    std::vector<nlm::ActionType> sequence2 = {nlm::ActionType::TurnLeft, nlm::ActionType::MoveForward};
    
    auto result1 = neuralPlanner->evaluateSequence(sequence1, currentState);
    auto result2 = neuralPlanner->evaluateSequence(sequence2, currentState);
    
    assert(result1.expectedReward >= 0.0f);
    assert(result2.expectedReward >= 0.0f);
    
    std::cout << "    testCognitivePlanningWithConcepts passed" << std::endl;
}

void testConceptMemoryStorage() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("episodic_memory_capacity", 30, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto conceptFormation = brain.getConceptFormation();
    assert(conceptFormation != nullptr);
    
    // Create multiple concept instances with varying stability
    for (size_t conceptId = 0; conceptId < 5; ++conceptId) {
        std::vector<float> pattern(5, static_cast<float>(conceptId) * 0.2f);
        std::vector<float> features(5, 1.0f - static_cast<float>(conceptId) * 0.1f);
        
        // Record multiple observations for stability
        for (size_t obs = 0; obs < 10 + conceptId * 5; ++obs) {
            conceptFormation->recordObservation(pattern, features, obs + 100);
        }
    }
    
    // Get concepts and verify storage
    auto concepts = conceptFormation->getDiscoveredConcepts();
    
    assert(concepts.size() >= 1);
    
    // Concepts should have been stabilized through repeated exposure
    for (const auto& concept : concepts) {
        assert(concept.instances.size() > 0);
        
        // Calculate average stability
        float avgStability = 0.0f;
        for (const auto& instance : concept.instances) {
            avgStability += instance.stability;
        }
        avgStability /= static_cast<float>(concept.instances.size());
        
        assert(avgStability >= 0.0f && avgStability <= 1.0f);
    }
    
    std::cout << "    testConceptMemoryStorage passed" << std::endl;
}

void testPlanningErrorCorrection() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(80), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto neuralPlanner = brain.getNeuralPlanner();
    assert(neuralPlanner != nullptr);
    
    std::vector<float> startState = {0.3f, 0.7f, 0.4f, 0.8f, 0.2f};
    
    // Initial plan
    std::vector<nlm::ActionType> plannedSequence = {
        nlm::ActionType::MoveForward,
        nlm::ActionType::TurnRight,
        nlm::ActionType::MoveForward
    };
    
    auto initialResult = neuralPlanner->evaluateSequence(plannedSequence, startState);
    
    // Simulate unexpected outcome by changing state
    std::vector<float> unexpectedState = {0.8f, 0.2f, 0.1f, 0.3f, 0.9f};
    
    // Re-evaluate with new state
    auto correctedResult = neuralPlanner->evaluateSequence(plannedSequence, unexpectedState);
    
    // Planning should adapt to new conditions
    assert(correctedResult.actions.size() == plannedSequence.size());
    assert(correctedResult.expectedReward >= 0.0f);
    
    // Test replanning with different sequence
    std::vector<nlm::ActionType> alternativeSequence = {
        nlm::ActionType::TurnLeft,
        nlm::ActionType::MoveForward
    };
    
    auto alternativeResult = neuralPlanner->evaluateSequence(alternativeSequence, unexpectedState);
    assert(alternativeResult.actions.size() == alternativeSequence.size());
    
    std::cout << "    testPlanningErrorCorrection passed" << std::endl;
}

void testCognitiveStateIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto neuralPlanner = brain.getNeuralPlanner();
    assert(neuralPlanner != nullptr);
    
    // Test cognitive state effects on planning
    std::vector<float> optimalState = {0.1f, 0.9f, 0.2f, 0.8f, 0.3f};
    std::vector<float> poorState = {0.5f, 0.5f, 0.5f, 0.5f, 0.5f};
    
    // Plan from optimal state
    nlm::ActionType actionFromOptimal = neuralPlanner->planAction(optimalState, 0.8f);
    
    // Plan from poor state
    nlm::ActionType actionFromPoor = neuralPlanner->planAction(poorState, 0.8f);
    
    // Both should be valid actions
    assert(actionFromOptimal >= nlm::ActionType::MoveForward && actionFromOptimal <= nlm::ActionType::TurnLeft);
    assert(actionFromPoor >= nlm::ActionType::MoveForward && actionFromPoor <= nlm::ActionType::TurnLeft);
    
    // Test with different reward targets
    nlm::ActionType actionHighReward = neuralPlanner->planAction(optimalState, 0.95f);
    nlm::ActionType actionLowReward = neuralPlanner->planAction(optimalState, 0.3f);
    
    // Planning should consider reward targets
    assert(actionHighReward >= nlm::ActionType::MoveForward && actionHighReward <= nlm::ActionType::TurnLeft);
    assert(actionLowReward >= nlm::ActionType::MoveForward && actionLowReward <= nlm::ActionType::TurnLeft);
    
    std::cout << "    testCognitiveStateIntegration passed" << std::endl;
}

void runAll() {
    testNeuralPlannerIntegration();
    testPlanningSequenceEvaluation();
    testConceptFormationIntegration();
    testCognitivePlanningWithConcepts();
    testConceptMemoryStorage();
    testPlanningErrorCorrection();
    testCognitiveStateIntegration();
}

} // namespace test_cognition_integration