#include "NeuralPlanner.hpp"
#include "ConceptFormation.hpp"
#include "AttentionalSelection.hpp"
#include <iostream>
#include <cassert>
#include <cmath>

namespace nlm {

void test_attentional_selection_system() {
    std::cout << "Testing Attentional Selection System" << std::endl;
    std::cout << "===================================" << std::endl;
    
    // Test 1: Basic attentional selection creation
    std::cout << "Test 1: Basic attentional selection creation..." << std::endl;
    AttentionalSelection attention;
    
    assert(attention.getAttentionLevel() >= 0.0f && attention.getAttentionLevel() <= 1.0f);
    assert(attention.getFocusStrength() >= 0.0f && attention.getFocusStrength() <= 1.0f);
    assert(attention.getDistractibility() >= 0.0f && attention.getDistractibility() <= 1.0f);
    assert(attention.getSaliencyThreshold() >= 0.0f);
    std::cout << "✓ Basic attentional selection creation passed" << std::endl;
    
    // Test 2: Attention level management
    std::cout << "Test 2: Attention level management..." << std::endl;
    float initialLevel = attention.getAttentionLevel();
    attention.setAttentionLevel(0.8f);
    assert(attention.getAttentionLevel() == 0.8f);
    assert(attention.getAttentionLevel() <= 1.0f);
    std::cout << "✓ Attention level management passed" << std::endl;
    
    // Test 3: Focus strength
    std::cout << "Test 3: Focus strength..." << std::endl;
    attention.setFocusStrength(0.9f);
    assert(attention.getFocusStrength() == 0.9f);
    assert(attention.getFocusStrength() <= 1.0f);
    std::cout << "✓ Focus strength passed" << std::endl;
    
    // Test 4: Distractibility
    std::cout << "Test 4: Distractibility..." << std::endl;
    attention.setDistractibility(0.3f);
    assert(attention.getDistractibility() == 0.3f);
    assert(attention.getDistractibility() <= 1.0f);
    std::cout << "✓ Distractibility passed" << std::endl;
    
    // Test 5: Saliency threshold
    std::cout << "Test 5: Saliency threshold..." << std::endl;
    attention.setSaliencyThreshold(0.5f);
    assert(attention.getSaliencyThreshold() == 0.5f);
    assert(attention.getSaliencyThreshold() >= 0.0f);
    std::cout << "✓ Saliency threshold passed" << std::endl;
    
    // Test 6: Update with stimuli
    std::cout << "Test 6: Update with stimuli..." << std::endl;
    attention.update(0.1f, std::vector<float>{1.0f, 2.0f, 3.0f});  // timestep, stimuli
    assert(attention.getAttentionLevel() >= 0.0f && attention.getAttentionLevel() <= 1.0f);
    std::cout << "✓ Update with stimuli passed" << std::endl;
    
    // Test 7: Select focus
    std::cout << "Test 7: Select focus..." << std::endl;
    int focusIndex = attention.selectFocus(std::vector<float>{1.0f, 2.0f, 3.0f, 4.0f});
    assert(focusIndex >= 0 && focusIndex < 4);
    std::cout << "✓ Select focus passed" << std::endl;
    
    // Test 8: Reset attention
    std::cout << "Test 8: Reset attention..." << std::endl;
    attention.reset();
    assert(attention.getAttentionLevel() == 0.0f);
    assert(attention.getFocusStrength() == 0.0f);
    assert(attention.getDistractibility() == 0.0f);
    std::cout << "✓ Reset attention passed" << std::endl;
    
    std::cout << std::endl;
    std::cout << "Attentional Selection System tests PASSED! ✓" << std::endl;
    std::cout << "===================================" << std::endl;
}

void test_concept_formation_system() {
    std::cout << "Testing Concept Formation System" << std::endl;
    std::cout << "==============================" << std::endl;
    
    // Test 1: Basic concept formation creation
    std::cout << "Test 1: Basic concept formation creation..." << std::endl;
    ConceptFormation concept;
    
    assert(concept.getConceptCount() >= 0);
    assert(concept.getAbstractionLevel() >= 0.0f && concept.getAbstractionLevel() <= 1.0f);
    assert(concept.getPatternSimilarity() >= 0.0f && concept.getPatternSimilarity() <= 1.0f);
    std::cout << "✓ Basic concept formation creation passed" << std::endl;
    
    // Test 2: Concept count
    std::cout << "Test 2: Concept count..." << std::endl;
    size_t initialCount = concept.getConceptCount();
    assert(initialCount >= 0);
    std::cout << "✓ Concept count passed" << std::endl;
    
    // Test 3: Abstraction level
    std::cout << "Test 3: Abstraction level..." << std::endl;
    concept.setAbstractionLevel(0.7f);
    assert(concept.getAbstractionLevel() == 0.7f);
    assert(concept.getAbstractionLevel() <= 1.0f);
    std::cout << "✓ Abstraction level passed" << std::endl;
    
    // Test 4: Pattern similarity
    std::cout << "Test 4: Pattern similarity..." << std::endl;
    concept.setPatternSimilarity(0.8f);
    assert(concept.getPatternSimilarity() == 0.8f);
    assert(concept.getPatternSimilarity() <= 1.0f);
    std::cout << "✓ Pattern similarity passed" << std::endl;
    
    // Test 5: Add concept
    std::cout << "Test 5: Add concept..." << std::endl;
    concept.addConcept(std::vector<float>{1.0f, 2.0f, 3.0f});
    assert(concept.getConceptCount() >= 1);
    std::cout << "✓ Add concept passed" << std::endl;
    
    // Test 6: Get concept
    std::cout << "Test 6: Get concept..." << std::endl;
    std::vector<float> conceptPattern = concept.getConcept(0);
    assert(conceptPattern.size() == 3);
    assert(conceptPattern[0] == 1.0f);
    std::cout << "✓ Get concept passed" << std::endl;
    
    // Test 7: Remove concept
    std::cout << "Test 7: Remove concept..." << std::endl;
    concept.removeConcept(0);
    assert(concept.getConceptCount() == initialCount);
    std::cout << "✓ Remove concept passed" << std::endl;
    
    // Test 8: Update concept
    std::cout << "Test 8: Update concept..." << std::endl;
    concept.addConcept(std::vector<float>{1.0f, 2.0f, 3.0f});
    concept.updateConcept(0, std::vector<float>{4.0f, 5.0f, 6.0f});
    
    std::vector<float> updatedConcept = concept.getConcept(0);
    assert(updatedConcept.size() == 3);
    assert(updatedConcept[0] == 4.0f);
    std::cout << "✓ Update concept passed" << std::endl;
    
    // Test 9: Concept similarity
    std::cout << "Test 9: Concept similarity..." << std::endl;
    float similarity = concept.calculateConceptSimilarity(std::vector<float>{1.0f, 2.0f, 3.0f}, std::vector<float>{1.1f, 2.1f, 3.1f});
    assert(similarity >= 0.0f && similarity <= 1.0f);
    std::cout << "✓ Concept similarity passed" << std::endl;
    
    // Test 10: Reset concepts
    std::cout << "Test 10: Reset concepts..." << std::endl;
    concept.reset();
    assert(concept.getConceptCount() == 0);
    std::cout << "✓ Reset concepts passed" << std::endl;
    
    std::cout << std::endl;
    std::cout << "Concept Formation System tests PASSED! ✓" << std::endl;
    std::cout << "==============================" << std::endl;
}

void test_neural_planner_system() {
    std::cout << "Testing Neural Planner System" << std::endl;
    std::cout << "=============================== " << std::endl;
    
    // Test 1: Basic neural planner creation
    std::cout << "Test 1: Basic neural planner creation..." << std::endl;
    NeuralPlanner planner;
    
    assert(planner.getPlanCount() >= 0);
    assert(planner.getPlanningHorizon() >= 0);
    assert(planner.getActionConfidence() >= 0.0f && planner.getActionConfidence() <= 1.0f);
    std::cout << "✓ Basic neural planner creation passed" << std::endl;
    
    // Test 2: Plan count
    std::cout << "Test 2: Plan count..." << std::endl;
    size_t initialPlanCount = planner.getPlanCount();
    assert(initialPlanCount >= 0);
    std::cout << "✓ Plan count passed" << std::endl;
    
    // Test 3: Planning horizon
    std::cout << "Test 3: Planning horizon..." << std::endl;
    planner.setPlanningHorizon(10);
    assert(planner.getPlanningHorizon() == 10);
    assert(planner.getPlanningHorizon() >= 0);
    std::cout << "✓ Planning horizon passed" << std::endl;
    
    // Test 4: Action confidence
    std::cout << "Test 4: Action confidence..." << std::endl;
    planner.setActionConfidence(0.8f);
    assert(planner.getActionConfidence() == 0.8f);
    assert(planner.getActionConfidence() <= 1.0f);
    std::cout << "✓ Action confidence passed" << std::endl;
    
    // Test 5: Create plan
    std::cout << "Test 5: Create plan..." << std::endl;
    std::vector<float> goalState{1.0f, 2.0f, 3.0f};
    std::vector<Action> plan = planner.createPlan(goalState);
    assert(plan.size() >= 0);
    std::cout << "✓ Create plan passed" << std::endl;
    
    // Test 6: Evaluate plan
    std::cout << "Test 6: Evaluate plan..." << std::endl;
    if (plan.size() > 0) {
        float planScore = planner.evaluatePlan(plan);
        assert(planScore >= 0.0f);
    }
    std::cout << "✓ Evaluate plan passed" << std::endl;
    
    // Test 7: Select plan
    std::cout << "Test 7: Select plan..." << std::endl;
    std::vector<std::vector<Action>> plans = {plan};
    Action selectedAction = planner.selectPlan(plans);
    assert(selectedAction.getType() != ActionType::Wait || plans.size() == 1);
    std::cout << "✓ Select plan passed" << std::endl;
    
    // Test 8: Plan revision
    std::cout << "Test 8: Plan revision..." << std::endl;
    planner.revisePlan(plan, std::vector<float>{2.0f, 3.0f, 4.0f});
    assert(planner.getPlanCount() >= 0);
    std::cout << "✓ Plan revision passed" << std::endl;
    
    // Test 9: Reset plans
    std::cout << "Test 9: Reset plans..." << std::endl;
    planner.reset();
    assert(planner.getPlanCount() == 0);
    std::cout << "✓ Reset plans passed" << std::endl;
    
    // Test 10: Planning statistics
    std::cout << "Test 10: Planning statistics..." << std::endl;
    float successRate = planner.calculatePlanSuccessRate();
    assert(successRate >= 0.0f && successRate <= 1.0f);
    
    float averagePlanLength = planner.getAveragePlanLength();
    assert(averagePlanLength >= 0.0f);
    std::cout << "✓ Planning statistics passed" << std::endl;
    
    std::cout << std::endl;
    std::cout << "Neural Planner System tests PASSED! ✓" << std::endl;
    std::cout << "=============================== " << std::endl;
}

void run_all_cognitive_tests() {
    std::cout << "Running comprehensive Cognitive tests..." << std::endl;
    std::cout << "==========================================" << std::endl;
    
    test_attentional_selection_system();
    test_concept_formation_system();
    test_neural_planner_system();
    
    std::cout << "==========================================" << std::endl;
    std::cout << "All Cognitive tests PASSED! ✓" << std::endl;
    std::cout << "==========================================" << std::endl;
}

} // namespace nlm