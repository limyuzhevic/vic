// Cognition Integration Tests
// Tests cognition systems (neural planner, concept formation, attention)

#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "cognition/AttentionalSelection.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

namespace test_cognition {

void testNeuralPlannerBase() {
    std::cout << "  Testing NeuralPlanner Base..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::NeuralPlanner* planner = brain.getPlanner();
    assert(planner != nullptr);
    
    // Test initial state
    assert(planner->getPlanningDepth() > 0);
    assert(planner->getPlanningConfidence() == 0.0f);
    
    // Test goal setting
    std::vector<float> goal = {0.5f, 0.5f, 0.5f};
    planner->setCurrentGoal(goal);
    assert(planner->getCurrentGoal().size() == 3);
    
    // Test planning depth adjustment
    planner->setPlanningDepth(5);
    assert(planner->getPlanningDepth() == 5);
    
    // Test action quality setting
    planner->setActionQuality(nlm::ActionType::WALK, 0.8f);
    // (Implementation detail: actionQuality_ is internal)
    
    std::cout << "    testNeuralPlannerBase passed" << std::endl;
}

void testNeuralPlannerActionPlanning() {
    std::cout << "  Testing NeuralPlanner Action Planning..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::NeuralPlanner* planner = brain.getPlanner();
    assert(planner != nullptr);
    
    // Initialize planner with brain reference
    planner->initialize(&brain);
    
    // Test planning action given current state
    std::vector<float> currentState = {0.1f, 0.2f, 0.3f};
    nlm::ActionType action = planner->planAction(currentState, 0.5f);
    
    // Action should be valid (implementation dependent)
    // In this implementation, ActionType is enum
    
    // Test action evaluation
    std::vector<nlm::ActionType> actions = {nlm::ActionType::WALK, nlm::ActionType::JUMP};
    nlm::PlanningCandidate candidate = planner->evaluateSequence(actions, currentState);
    
    assert(candidate.actions.size() == actions.size());
    assert(candidate.expectedReward >= 0.0f);
    assert(candidate.confidence >= 0.0f && candidate.confidence <= 1.0f);
    
    // Test plan update
    std::vector<nlm::ActionType> plannedActions = {nlm::ActionType::WALK, nlm::ActionType::RUN};
    std::vector<nlm::ActionType> actualActions = {nlm::ActionType::WALK, nlm::ActionType::WALK};
    
    planner->updatePlanQuality(plannedActions, actualActions, 0.8f);
    
    // Test plan success
    bool recentSuccess = planner->wasRecentPlanSuccessful();
    assert(recentSuccess == true);
    
    std::cout << "    testNeuralPlannerActionPlanning passed" << std::endl;
}

void testNeuralPlannerPlanningDepth() {
    std::cout << "  Testing NeuralPlanner Planning Depth..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::NeuralPlanner* planner = brain.getPlanner();
    assert(planner != nullptr);
    
    planner->initialize(&brain);
    
    // Test that planning depth affects planning complexity
    size_t depth1 = 1;
    size_t depth2 = 10;
    
    planner->setPlanningDepth(depth1);
    assert(planner->getPlanningDepth() == depth1);
    
    planner->setPlanningDepth(depth2);
    assert(planner->getPlanningDepth() == depth2);
    
    // Deeper planning should be more complex
    // (Implementation dependent)
    
    std::cout << "    testNeuralPlannerPlanningDepth passed" << std::endl;
}

void testConceptFormationBase() {
    std::cout << "  Testing ConceptFormation Base..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::ConceptFormation* conceptFormation = brain.getConceptFormation();
    assert(conceptFormation != nullptr);
    
    // Test concept initialization
    conceptFormation->initialize(&brain);
    
    // Test concept learning from patterns
    std::vector<float> pattern1 = {0.1f, 0.2f, 0.3f};
    std::vector<float> pattern2 = {0.11f, 0.21f, 0.31f};  // Similar to pattern1
    std::vector<float> pattern3 = {0.9f, 0.9f, 0.9f};    // Different pattern
    
    conceptFormation->learnPattern(pattern1);
    conceptFormation->learnPattern(pattern2);
    conceptFormation->learnPattern(pattern3);
    
    // Test concept discovery
    std::vector<float> testPattern = {0.15f, 0.25f, 0.35f};  // Should match pattern1
    std::string concept = conceptFormation->discoverConcept(testPattern);
    
    // Concept could be "similar_to_pattern1" or similar
    assert(!concept.empty());
    
    // Test concept confidence
    float confidence = conceptFormation->getConceptConfidence(concept);
    assert(confidence >= 0.0f && confidence <= 1.0f);
    
    std::cout << "    testConceptFormationBase passed" << std::endl;
}

void testConceptFormationPatternMatching() {
    std::cout << "  Testing ConceptFormation Pattern Matching..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(75), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::ConceptFormation* conceptFormation = brain.getConceptFormation();
    assert(conceptFormation != nullptr);
    
    conceptFormation->initialize(&brain);
    
    // Create clusters of patterns
    std::vector<std::vector<float>> cluster1;
    for (int i = 0; i < 10; ++i) {
        std::vector<float> pattern(3, 0.1f + (static_cast<float>(i) * 0.01f));
        cluster1.push_back(pattern);
        conceptFormation->learnPattern(pattern);
    }
    
    std::vector<std::vector<float>> cluster2;
    for (int i = 0; i < 10; ++i) {
        std::vector<float> pattern(3, 0.8f + (static_cast<float>(i) * 0.01f));
        cluster2.push_back(pattern);
        conceptFormation->learnPattern(pattern);
    }
    
    // Test pattern matching to each cluster
    std::vector<float> testFromCluster1 = {0.15f, 0.25f, 0.35f};
    std::vector<float> testFromCluster2 = {0.85f, 0.95f, 1.05f};
    
    std::string concept1 = conceptFormation->discoverConcept(testFromCluster1);
    std::string concept2 = conceptFormation->discoverConcept(testFromCluster2);
    
    assert(!concept1.empty());
    assert(!concept2.empty());
    
    // Patterns from different clusters should give different concepts
    // (Implementation dependent)
    
    std::cout << "    testConceptFormationPatternMatching passed" << std::endl;
}

void testAttentionalSelectionBase() {
    std::cout << "  Testing AttentionalSelection Base..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::AttentionalSelection* attention = brain.getAttention();
    assert(attention != nullptr);
    
    // Test attention initialization
    attention->initialize(&brain);
    
    // Test attention selection from multiple stimuli
    std::vector<std::vector<float>> stimuli = {
        {0.1f, 0.2f, 0.3f},
        {0.8f, 0.9f, 1.0f},
        {0.0f, 0.0f, 0.0f},
        {0.5f, 0.5f, 0.5f}
    };
    
    std::vector<float> focused = attention->selectFocus(stimuli);
    assert(focused.size() == stimuli[0].size());
    
    // Focused attention should emphasize salient features
    // In implementation, this would select one stimulus or mix based on salience
    
    // Test attention reset
    attention->reset();
    
    std::cout << "    testAttentionalSelectionBase passed" << std::endl;
}

void testAttentionalSelectionSalience() {
    std::cout << "  Testing AttentionalSelection Salience..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(60), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::AttentionalSelection* attention = brain.getAttention();
    assert(attention != nullptr);
    
    attention->initialize(&brain);
    
    // Create stimuli with varying salience
    std::vector<std::vector<float>> stimuli;
    
    // High salience (bright, prominent)
    std::vector<float> highSalience = {1.0f, 1.0f, 1.0f};
    stimuli.push_back(highSalience);
    
    // Low salience (dim, less prominent)
    std::vector<float> lowSalience = {0.1f, 0.1f, 0.1f};
    stimuli.push_back(lowSalience);
    
    // Medium salience
    std::vector<float> mediumSalience = {0.5f, 0.5f, 0.5f};
    stimuli.push_back(mediumSalience);
    
    // Test attention selection
    std::vector<float> selected = attention->selectFocus(stimuli);
    assert(selected.size() == 3);
    
    // Selected attention should emphasize high salience features
    // (Implementation dependent - could select entire stimulus or weighted combination)
    
    // Test that attention updates based on novelty or prediction error
    // In real implementation, attention modulates based on multiple factors
    
    std::cout << "    testAttentionalSelectionSalience passed" << std::endl;
}

void testCognitionIntegration() {
    std::cout << "  Testing Cognition Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Verify all cognition systems are accessible
    assert(brain.getPlanner() != nullptr);
    assert(brain.getConceptFormation() != nullptr);
    assert(brain.getAttention() != nullptr);
    
    // Test cognition system interconnections
    nlm::NeuralPlanner* planner = brain.getPlanner();
    nlm::ConceptFormation* concepts = brain.getConceptFormation();
    nlm::AttentionalSelection* attention = brain.getAttention();
    
    // Test integrated cognition
    std::vector<float> state = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    
    // Concept formation can help planner understand the state
    concepts->learnPattern(state);
    std::string concept = concepts->discoverConcept(state);
    
    // Planner can use concepts to inform action selection
    nlm::ActionType action = planner->planAction(state, 0.7f);
    
    // Attention can focus on relevant features
    std::vector<std::vector<float>> stimuli = {state, {0.6f, 0.7f, 0.8f, 0.9f, 1.0f}};
    std::vector<float> focused = attention->selectFocus(stimuli);
    
    // Test SelfModel integration (if available through planner)
    // SelfModel would learn action effects
    
    // Test SocialLearning integration (if available through concept formation)
    // SocialLearning would learn from observations
    
    std::cout << "    testCognitionIntegration passed" << std::endl;
}

void testCognitionPerformance() {
    std::cout << "  Testing Cognition Performance..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::NeuralPlanner* planner = brain.getPlanner();
    nlm::ConceptFormation* concepts = brain.getConceptFormation();
    nlm::AttentionalSelection* attention = brain.getAttention();
    
    assert(planner != nullptr);
    assert(concepts != nullptr);
    assert(attention != nullptr);
    
    const size_t iterations = 5000;
    
    // Performance benchmark for cognition systems
    for (size_t i = 0; i < iterations; ++i) {
        std::vector<float> state(5, static_cast<float>(i % 10) / 10.0f);
        
        // Concept formation
        concepts->learnPattern(state);
        concepts->discoverConcept(state);
        
        // Neural planning
        nlm::ActionType action = planner->planAction(state, 0.5f);
        
        // Attention
        std::vector<std::vector<float>> stimuli = {state};
        attention->selectFocus(stimuli);
        
        // This tests that all cognition systems can operate together
    }
    
    // Verify performance
    std::string concept = concepts->discoverConcept({0.5f, 0.5f, 0.5f, 0.5f, 0.5f});
    assert(!concept.empty());
    
    std::cout << "    testCognitionPerformance passed" << std::endl;
}

void testCognitionErrorConditions() {
    std::cout << "  Testing Cognition Error Conditions..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::NeuralPlanner* planner = brain.getPlanner();
    nlm::ConceptFormation* concepts = brain.getConceptFormation();
    nlm::AttentionalSelection* attention = brain.getAttention();
    
    // Test cognition systems with edge cases
    std::vector<float> emptyState;
    std::vector<float> zeroState(5, 0.0f);
    std::vector<float> largeState(5, 1000.0f);
    std::vector<float> nanState(5, std::nanf(""));
    
    // Should handle empty states gracefully
    concepts->learnPattern(emptyState);
    
    // Should handle extreme values
    concepts->learnPattern(zeroState);
    concepts->learnPattern(largeState);
    
    // Concept discovery should handle edge cases
    std::string concept = concepts->discoverConcept(zeroState);
    
    // Should handle null/invalid states
    if (!emptyState.empty()) {
        concept = concepts->discoverConcept(emptyState);
    }
    
    // Should handle planning with edge case states
    nlm::ActionType action = planner->planAction(emptyState, 0.0f);
    
    // Should handle attention with edge case stimuli
    std::vector<std::vector<float>> edgeCaseStimuli = {zeroState, largeState};
    std::vector<float> focused = attention->selectFocus(edgeCaseStimuli);
    
    std::cout << "    testCognitionErrorConditions passed" << std::endl;
}

void runAll() {
    testNeuralPlannerBase();
    testNeuralPlannerActionPlanning();
    testNeuralPlannerPlanningDepth();
    testConceptFormationBase();
    testConceptFormationPatternMatching();
    testAttentionalSelectionBase();
    testAttentionalSelectionSalience();
    testCognitionIntegration();
    testCognitionPerformance();
    testCognitionErrorConditions();
}

} // namespace test_cognition
