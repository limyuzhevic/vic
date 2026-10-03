// Cognitive System Tests
// This file tests the cognitive components of the NLM framework
// Covers neural planning, concept formation, and attentional selection

#include "cognition/NeuralPlanner/NeuralPlanner.hpp"
#include "cognition/ConceptFormation/ConceptFormation.hpp"
#include "cognition/AttentionalSelection/AttentionalSelection.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>

namespace test_cognition {

void testNeuralPlannerActionSelection() {
    auto config = std::make_shared<nlm::Config>();
    
    config->set("cognition.planner.action_count", 5, nlm::ConfigSource::Default);
    config->set("cognition.planner.planning_horizon", 10, nlm::ConfigSource::Default);
    config->set("cognition.planner.exploration_bonus", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto neuralPlanner = brain.getNeuralPlanner();
    assert(neuralPlanner != nullptr);
    
    // Test action sequence evaluation
    std::vector<nlm::Action> actionSequence;
    for (int i = 0; i < 3; ++i) {
        nlm::Action action;
        action.type = nlm::ActionType::Move;
        action.parameters = {static_cast<double>(i), 0.0};
        actionSequence.push_back(action);
    }
    
    double actionValue = neuralPlanner->evaluateActionSequence(actionSequence);
    assert(actionValue >= -1.0 && actionValue <= 1.0); // Value should be in reasonable range
    
    // Test action selection
    nlm::Action selectedAction = neuralPlanner->selectAction();
    assert(selectedAction.type != nlm::ActionType::Invalid);
    
    std::cout << "    testNeuralPlannerActionSelection passed" << std::endl;
}

void testNeuralPlannerActionValueLearning() {
    auto config = std::make_shared<nlm::Config>();
    
    config->set("cognition.planner.learning_rate", 0.05, nlm::ConfigSource::Default);
    config->set("cognition.planner.discount_factor", 0.9, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto neuralPlanner = brain.getNeuralPlanner();
    
    // Test updating action values based on rewards
    nlm::Action action1;
    action1.type = nlm::ActionType::Move;
    action1.parameters = {1.0, 0.0};
    
    nlm::Action action2;
    action2.type = nlm::ActionType::Rotate;
    action2.parameters = {0.0, 1.0};
    
    std::vector<nlm::Action> actionSequence1 = {action1, action2};
    std::vector<nlm::Action> actionSequence2 = {action2, action1};
    
    // Initial values should be equal or close
    double value1 = neuralPlanner->evaluateActionSequence(actionSequence1);
    double value2 = neuralPlanner->evaluateActionSequence(actionSequence2);
    assert(std::abs(value1 - value2) < 0.1);
    
    // Update values based on rewards
    neuralPlanner->updateActionValue(action1, 0.8); // High reward for action1
    neuralPlanner->updateActionValue(action2, 0.2); // Low reward for action2
    
    // Values should now be different
    double value1After = neuralPlanner->evaluateActionSequence(actionSequence1);
    double value2After = neuralPlanner->evaluateActionSequence(actionSequence2);
    
    // Action1 should now have higher value
    assert(value1After > value2After || std::abs(value1After - value2After) < 0.01);
    
    std::cout << "    testNeuralPlannerActionValueLearning passed" << std::endl;
}

void testConceptFormationPatternAbstraction() {
    auto config = std::make_shared<nlm::Config>();
    
    config->set("cognition.concept.formation_threshold", 0.7, nlm::ConfigSource::Default);
    config->set("cognition.concept.similarity_metric", 0.8, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto conceptFormation = brain.getConceptFormation();
    assert(conceptFormation != nullptr);
    
    // Test storing patterns and forming concepts
    std::vector<std::vector<double>> patternSet = {
        {1.0, 2.0, 3.0, 4.0},
        {1.1, 2.1, 3.1, 4.1},  // Similar to pattern 1
        {5.0, 6.0, 7.0, 8.0},
        {5.1, 6.1, 7.1, 8.1},  // Similar to pattern 3
        {9.0, 10.0, 11.0, 12.0}
    };
    
    for (const auto& pattern : patternSet) {
        conceptFormation->storePattern(pattern);
    }
    
    // Test concept retrieval
    std::vector<double> queryPattern{1.05, 2.05, 3.05, 4.05};
    auto concepts = conceptFormation->getConceptsForPattern(queryPattern);
    
    // Should find at least one concept matching the query
    assert(concepts.size() > 0);
    
    // Test concept abstraction
    auto abstractedConcept = conceptFormation->abstractConcept(queryPattern);
    assert(abs abstractedConcept.size() > 0);
    
    std::cout << "    testConceptFormationPatternAbstraction passed" << std::endl;
}

void testConceptFormationSimilarityMatching() {
    auto config = std::make_shared<nlm::Config>();
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto conceptFormation = brain.getConceptFormation();
    
    // Store some reference patterns
    std::vector<double> referencePattern1{1.0, 2.0, 3.0, 4.0};
    std::vector<double> referencePattern2{5.0, 6.0, 7.0, 8.0};
    
    conceptFormation->storePattern(referencePattern1);
    conceptFormation->storePattern(referencePattern2);
    
    // Test similarity matching with a query pattern
    std::vector<double> queryPattern{1.05, 2.05, 3.05, 4.05}; // Very similar to pattern1
    std::vector<double> dissimilarPattern{9.0, 10.0, 11.0, 12.0}; // Very different
    
    double similarityToPattern1 = conceptFormation->calculateSimilarity(queryPattern, referencePattern1);
    double similarityToPattern2 = conceptFormation->calculateSimilarity(queryPattern, referencePattern2);
    double similarityToDissimilar = conceptFormation->calculateSimilarity(dissimilarPattern, referencePattern1);
    
    // Query should be more similar to pattern1 than pattern2
    assert(similarityToPattern1 > similarityToPattern2);
    
    // Dissimilar pattern should have low similarity
    assert(similarityToDissimilar < similarityToPattern1);
    
    std::cout << "    testConceptFormationSimilarityMatching passed" << std::endl;
}

void testAttentionalSelectionSelectiveFocus() {
    auto config = std::make_shared<nlm::Config>();
    
    config->set("cognition.attention.focus_strength", 0.8, nlm::ConfigSource::Default);
    config->set("cognition.attention.competition_strength", 0.3, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto attentionalSelection = brain.getAttentionalSelection();
    assert(attentionalSelection != nullptr);
    
    // Test stimulus presentation and attentional selection
    std::vector<std::vector<double>> stimuli = {
        {0.1, 0.9, 0.2, 0.8, 0.3},  // High attention stimulus
        {0.7, 0.3, 0.6, 0.4, 0.5},  // Medium attention stimulus
        {0.4, 0.5, 0.6, 0.7, 0.8},  // Low attention stimulus
    };
    
    for (size_t i = 0; i < stimuli.size(); ++i) {
        attentionalSelection->processStimulus(stimuli[i], static_cast<int>(i));
    }
    
    // Test attentional focus selection
    nlm::Stimulus focusedStimulus = attentionalSelection->getFocusedStimulus();
    assert(focusedStimulus.value.size() > 0);
    
    // Test attentional weight adjustment
    attentionalSelection->adjustWeights();
    
    std::cout << "    testAttentionalSelectionSelectiveFocus passed" << std::endl;
}

void testAttentionalSelectionCompetition() {
    auto config = std::make_shared<nlm::Config>();
    
    config->set("cognition.attention.competition_strength", 0.5, nlm::ConfigSource::Default);
    config->set("cognition.attention.focus_strength", 0.5, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto attentionalSelection = brain.getAttentionalSelection();
    
    // Test competitive attentional selection
    std::vector<std::vector<double>> competingStimuli;
    for (int i = 0; i < 5; ++i) {
        std::vector<double> stimulus(10, 0.5); // All stimuli have same baseline
        stimulus[i] = 0.9; // Make one stimulus more salient
        competingStimuli.push_back(stimulus);
    }
    
    // Process all stimuli
    for (size_t i = 0; i < competingStimuli.size(); ++i) {
        attentionalSelection->processStimulus(competingStimuli[i], static_cast<int>(i));
    }
    
    // Get focused stimulus (should be the most salient one)
    nlm::Stimulus focusedStimulus = attentionalSelection->getFocusedStimulus();
    
    // The focused stimulus should have some distinctive feature
    assert(focusedStimulus.value.size() > 0);
    
    std::cout << "    testAttentionalSelectionCompetition passed" << std::endl;
}

void testCognitionIntegration() {
    auto config = std::make_shared<nlm::Config>();
    
    config->set("cognition.planner.action_count", 3, nlm::ConfigSource::Default);
    config->set("cognition.concept.formation_threshold", 0.5, nlm::ConfigSource::Default);
    config->set("cognition.attention.focus_strength", 0.7, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test that all cognitive systems are present and functional
    assert(brain.getNeuralPlanner() != nullptr);
    assert(brain.getConceptFormation() != nullptr);
    assert(brain.getAttentionalSelection() != nullptr);
    
    // Test integration by running cognitive operations
    std::vector<double> inputPattern{1.0, 2.0, 3.0, 4.0};
    
    // Neural planning
    nlm::Action plannedAction = brain.getNeuralPlanner()->selectAction();
    
    // Concept formation
    auto concepts = brain.getConceptFormation()->getConceptsForPattern(inputPattern);
    
    // Attentional selection
    attentionalSelection->processStimulus(inputPattern, 0);
    
    std::cout << "    testCognitionIntegration passed" << std::endl;
}

void runAll() {
    testNeuralPlannerActionSelection();
    testNeuralPlannerActionValueLearning();
    testConceptFormationPatternAbstraction();
    testConceptFormationSimilarityMatching();
    testAttentionalSelectionSelectiveFocus();
    testAttentionalSelectionCompetition();
    testCognitionIntegration();
    
    std::cout << "All cognition tests passed!" << std::endl;
}

} // namespace test_cognition
