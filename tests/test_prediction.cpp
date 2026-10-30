// Prediction System Tests
// Tests for prediction and planning systems

#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_prediction {

void testPredictionSystem() {
    std::cout << "    Testing Prediction System..." << std::endl;
    
    nlm::Config config;
    config.set("neuron_count", 1000);
    config.set("region_count", 2);
    
    auto brain = std::make_shared<nlm::Brain>(std::make_shared<nlm::Config>(config));
    
    if (!brain->initialize()) {
        throw std::runtime_error("Failed to initialize brain");
    }
    
    auto prediction_system = brain->getPredictionSystem();
    assert(prediction_system != nullptr);
    
    // Test prediction system functionality
    // Create test input
    nlm::Observation observation;
    std::vector<float> sensory_data = {0.1f, 0.5f, 0.3f, 0.8f, 0.2f};
    observation.sensory_data = sensory_data;
    
    // Generate prediction
    auto prediction = prediction_system->predict(observation);
    assert(prediction.has_value());
    
    // Test prediction accuracy
    float prediction_accuracy = prediction_system->getAccuracy();
    assert(prediction_accuracy >= 0.0f && prediction_accuracy <= 1.0f);
    
    std::cout << "      PASSED" << std::endl;
}

void testNeuralPlanner() {
    std::cout << "    Testing Neural Planner..." << std::endl;
    
    nlm::Config config;
    config.set("neuron_count", 1000);
    
    auto brain = std::make_shared<nlm::Brain>(std::make_shared<nlm::Config>(config));
    
    if (!brain->initialize()) {
        throw std::runtime_error("Failed to initialize brain");
    }
    
    auto planner = brain->getPlanner();
    assert(planner != nullptr);
    
    // Test planning functionality
    // Create goal
    nlm::Goal goal;
    goal.id = 1;
    goal.priority = 1.0f;
    goal.value = 100.0f;
    std::vector<float> goal_vector = {1.0f, 0.5f, 0.0f, -0.5f};
    goal.vector = goal_vector;
    
    // Plan action sequence
    std::vector<nlm::Action> action_sequence;
    planner->plan(goal, action_sequence);
    
    // Evaluate plan
    float plan_quality = planner->evaluatePlan(action_sequence);
    assert(plan_quality >= 0.0f && plan_quality <= 1.0f);
    
    std::cout << "      PASSED" << std::endl;
}

void testConceptFormation() {
    std::cout << "    Testing Concept Formation..." << std::endl;
    
    nlm::Config config;
    config.set("neuron_count", 1000);
    
    auto brain = std::make_shared<nlm::Brain>(std::make_shared<nlm::Config>(config));
    
    if (!brain->initialize()) {
        throw std::runtime_error("Failed to initialize brain");
    }
    
    auto concept_formation = brain->getConceptFormation();
    assert(concept_formation != nullptr);
    
    // Test concept formation
    std::vector<float> input_pattern = {0.1f, 0.5f, 0.3f, 0.8f, 0.2f};
    
    concept_formation->processInput(input_pattern);
    
    // Get formed concepts
    auto concepts = concept_formation->getConcepts();
    assert(!concepts.empty());
    
    // Test concept activation
    float concept_activation = concept_formation->getActivation(0);
    assert(concept_activation >= 0.0f && concept_activation <= 1.0f);
    
    std::cout << "      PASSED" << std::endl;
}

void runAll() {
    testPredictionSystem();
    testNeuralPlanner();
    testConceptFormation();
}

} // namespace test_prediction
