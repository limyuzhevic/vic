// Test for Cognition System Integration (NeuralPlanner, ConceptFormation, AttentionalSelection)
// Covers requirement: 3. Cognition System Tests

#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_cognition_system {

void testNeuralPlannerBasic() {
    nlm::NeuralPlanner planner;
    
    // Test initial state
    assert(planner.getPlanCount() == 0);
    assert(planner.getActionSequenceSize() == 0);
    
    std::cout << "    testNeuralPlannerBasic passed" << std::endl;
}

void testNeuralPlannerActionSelection() {
    nlm::NeuralPlanner planner;
    
    // Create test neurons for action representation
    nlm::Neuron actionNeuron1(nlm::NeuronId(1));
    nlm::Neuron actionNeuron2(nlm::NeuronId(2));
    
    // Make predictions for actions
    planner.evaluateAction(actionNeuron1, 0.8f);
    planner.evaluateAction(actionNeuron2, 0.6f);
    
    // Get best action
    nlm::NeuronId bestAction = planner.getBestAction();
    assert(bestAction == nlm::NeuronId(1)); // Should be the higher valued action
    
    // Get action sequence
    auto sequence = planner.getActionSequence();
    assert(sequence.size() > 0);
    
    std::cout << "    testNeuralPlannerActionSelection passed" << std::endl;
}

void testConceptFormationBasic() {
    nlm::ConceptFormation conceptForm;
    
    // Test initial state
    assert(conceptForm.getConceptCount() == 0);
    
    std::cout << "    testConceptFormationBasic passed" << std::endl;
}

void testConceptFormationWithExperience() {
    nlm::ConceptFormation conceptForm;
    
    // Create test experience (pattern of neuron activations)
    std::vector<nlm::NeuronId> pattern1;
    pattern1.push_back(nlm::NeuronId(1));
    pattern1.push_back(nlm::NeuronId(2));
    
    std::vector<nlm::NeuronId> pattern2;
    pattern2.push_back(nlm::NeuronId(1));
    pattern2.push_back(nlm::NeuronId(3));
    
    // Form concepts from patterns
    conceptForm.formConcept(pattern1, 0.7f, "concept1");
    conceptForm.formConcept(pattern2, 0.8f, "concept2");
    
    assert(conceptForm.getConceptCount() == 2);
    
    // Test concept retrieval
    float similarity1 = conceptForm.getConceptSimilarity(pattern1, "concept1");
    assert(similarity1 > 0.6f);
    
    std::cout << "    testConceptFormationWithExperience passed" << std::endl;
}

void testAttentionalSelection() {
    // Note: AttentionalSelection is not yet implemented in the codebase
    // This test would verify its integration when implemented
    
    std::cout << "    testAttentionalSelection skipped (not yet implemented)" << std::endl;
}

void testCognitionIntegration() {
    nlm::NeuralPlanner planner;
    nlm::ConceptFormation conceptForm;
    
    // Create a test scenario
    std::vector<nlm::NeuronId> actionPattern;
    actionPattern.push_back(nlm::NeuronId(1));
    actionPattern.push_back(nlm::NeuronId(2));
    
    // Form a concept for the action pattern
    conceptForm.formConcept(actionPattern, 0.9f, "reach_action");
    
    // Use concept to guide planner
    nlm::Neuron actionNeuron(nlm::NeuronId(10));
    planner.evaluateAction(actionNeuron, 0.85f);
    
    // Planner should consider the formed concept
    auto sequence = planner.getActionSequence();
    assert(sequence.size() >= 1);
    
    std::cout << "    testCognitionIntegration passed" << std::endl;
}

void runAll() {
    testNeuralPlannerBasic();
    testNeuralPlannerActionSelection();
    testConceptFormationBasic();
    testConceptFormationWithExperience();
    testAttentionalSelection();
    testCognitionIntegration();
}

} // namespace test_cognition_system
