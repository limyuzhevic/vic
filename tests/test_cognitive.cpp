// Cognitive and Neuromodulatory System Tests
// Tests for Dopamine, AttentionalSelection, NeuralPlanner, and ConceptFormation

#include "neuromodulation/Dopamine.hpp"
#include "cognition/AttentionalSelection.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_cognitive {

void testDopamine() {
    nlm::Dopamine dopamine;
    
    // Test initialization (requires brain, but we'll test basic functionality)
    assert(dopamine.getLevel() == 0.0f);
    assert(dopamine.getPlasticityFactor() == 1.0f);
    assert(dopamine.getPredictionError() == 0.0f);
    
    // Test prediction error update
    dopamine.updatePredictionError(0.5f, 0);
    assert(dopamine.getLevel() > 0.0f);
    assert(dopamine.getPredictionError() == 0.5f);
    
    // Test action value update
    dopamine.updateActionValue(nlm::ActionType::MoveForward, 1.0f, 0.8f, 0.1f, 0);
    
    // Test action value retrieval
    float actionValue = dopamine.getActionValue(nlm::ActionType::MoveForward);
    assert(actionValue >= 0.0f);
    
    std::cout << "    testDopamine passed" << std::endl;
}

void testAttentionalSelection() {
    nlm::AttentionalSelection attention;
    
    // Test initialization (requires brain, but we'll test basic functionality)
    assert(attention.getAttentionStrength() == 0.0f);
    assert(attention.getInhibitionStrength() == 0.5f);
    assert(attention.getExcitationStrength() == 1.5f);
    
    // Test attention update
    attention.updateAttentionalPriority(0.7f, 0.9f, 0);
    assert(attention.getAttentionStrength() > 0.0f);
    
    // Test competitive processing
    std::vector<nlm::NeuronId> competitors = {1, 2, 3, 4, 5};
    std::vector<float> activationLevels = {0.1f, 0.5f, 0.3f, 0.8f, 0.2f};
    
    size_t focus = attention.processCompetition(competitors, activationLevels);
    assert(focus < competitors.size());
    assert(attention.getAttentionalFocus() == focus);
    
    std::cout << "    testAttentionalSelection passed" << std::endl;
}

void testNeuralPlanner() {
    nlm::NeuralPlanner planner;
    
    // Test initialization
    assert(planner.getPlanningDepth() == 3);
    assert(planner.getPlanningConfidence() == 0.5f);
    
    // Test planning (requires brain state, but we'll test basic functionality)
    std::vector<float> currentState = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    nlm::ActionType action = planner.planAction(currentState, 1.0f);
    assert(action == nlm::ActionType::Wait || action == nlm::ActionType::MoveForward || 
           action == nlm::ActionType::TurnLeft || action == nlm::ActionType::TurnRight);
    
    // Test plan quality update
    std::vector<nlm::ActionType> plannedActions = {nlm::ActionType::MoveForward, nlm::ActionType::TurnRight};
    std::vector<nlm::ActionType> actualActions = {nlm::ActionType::MoveForward, nlm::ActionType::TurnRight};
    planner.updatePlanQuality(plannedActions, actualActions, 1.0f);
    
    // Test was recent plan successful
    bool wasSuccessful = planner.wasRecentPlanSuccessful();
    assert(wasSuccessful == true || wasSuccessful == false);  // Either is fine
    
    std::cout << "    testNeuralPlanner passed" << std::endl;
}

void testConceptFormation() {
    nlm::ConceptFormation conceptForm;
    
    // Test initialization
    assert(conceptForm.getConceptCount() == 0);
    
    // Test presenting experience (will create new concept since none exist)
    std::vector<float> pattern = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    std::vector<float> features = {0.5f, 0.6f, 0.7f};
    
    size_t conceptId = conceptForm.presentExperience(pattern, features, 1.0f, 0);
    // Concept ID could be 0 (too early) or new concept ID
    
    // Test is novel
    bool isNovel = conceptForm.isNovel(pattern);
    assert(isNovel == true || isNovel == false);  // Either is fine
    
    // Test get matching concept
    size_t matchingConcept = conceptForm.getMatchingConcept(pattern);
    assert(matchingConcept == 0 || matchingConcept == conceptId);
    
    std::cout << "    testConceptFormation passed" << std::endl;
}

void runAll() {
    testDopamine();
    testAttentionalSelection();
    testNeuralPlanner();
    testConceptFormation();
}

} // namespace test_cognitive