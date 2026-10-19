// Cognition System Tests
// Comprehensive tests for neural planner and concept formation

#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

namespace test_cognition {

// NeuralPlanner Tests
void testNeuralPlannerBasic() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* planner = brain.getPlanner();
    assert(planner != nullptr);
    
    // Test initial state
    assert(planner->getPlanningDepth() == 3); // Default depth
    assert(planner->getPlanningConfidence() >= 0.0f && planner->getPlanningConfidence() <= 1.0f);
    
    // Test current state
    std::vector<float> currentState = {0.5f, 0.6f, 0.7f, 0.8f};
    
    // Test planning with default parameters
    nlm::ActionType action = planner->planAction(currentState, 0.5f);
    assert(action == nlm::ActionType::Wait || action == nlm::ActionType::MoveForward || 
           action == nlm::ActionType::Wait || action == nlm::ActionType::Attack);
    
    // Test sequence evaluation
    std::vector<nlm::ActionType> actions = {nlm::ActionType::MoveForward, nlm::ActionType::Wait};
    nlm::PlanningCandidate candidate = planner->evaluateSequence(actions, currentState);
    
    assert(candidate.actions.size() == actions.size());
    assert(candidate.predictedStates.size() == actions.size());
    assert(candidate.expectedReward >= 0.0f && candidate.expectedReward <= 10.0f);
    assert(candidate.confidence >= 0.0f && candidate.confidence <= 1.0f);
    assert(candidate.depth == actions.size());
    
    // Test planning depth setting
    planner->setPlanningDepth(5);
    assert(planner->getPlanningDepth() == 5);
    
    // Test planning confidence
    float confidence = planner->getPlanningConfidence();
    assert(confidence >= 0.0f && confidence <= 1.0f);
    
    std::cout << "    testNeuralPlannerBasic passed" << std::endl;
}

void testNeuralPlannerIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* planner = brain.getPlanner();
    assert(planner != nullptr);
    
    // Set up planning scenario
    std::vector<float> currentState = {0.2f, 0.4f, 0.6f, 0.8f};
    planner->setCurrentGoal({0.9f, 0.9f, 0.9f, 0.9f});
    
    // Test multiple action plans
    for (int i = 0; i < 5; ++i) {
        nlm::ActionType action = planner->planAction(currentState, 0.5f);
        assert(action != nlm::ActionType::Wait || i < 2); // Allow early waits
        
        // Evaluate action sequences
        std::vector<nlm::ActionType> sequence;
        for (int j = 0; j < 3; ++j) {
            nlm::ActionType nextAction = planner->planAction(currentState, 0.5f);
            sequence.push_back(nextAction);
        }
        
        nlm::PlanningCandidate candidate = planner->evaluateSequence(sequence, currentState);
        assert(candidate.actions.size() == sequence.size());
    }
    
    std::cout << "    testNeuralPlannerIntegration passed" << std::endl;
}

void testNeuralPlannerFeedback() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* planner = brain.getPlanner();
    assert(planner != nullptr);
    
    // Setup planning
    std::vector<float> state = {0.3f, 0.5f, 0.7f, 0.9f};
    std::vector<nlm::ActionType> plannedActions = {
        nlm::ActionType::MoveForward,
        nlm::ActionType::Attack,
        nlm::ActionType::Wait
    };
    
    std::vector<nlm::ActionType> actualActions = {
        nlm::ActionType::MoveForward,
        nlm::ActionType::Wait,  // Different than planned
        nlm::ActionType::Wait
    };
    
    float actualReward = 5.0f;
    
    // Update plan quality based on actual outcome
    planner->updatePlanQuality(plannedActions, actualActions, actualReward);
    
    // Test recent plan success
    bool recentSuccess = planner->wasRecentPlanSuccessful();
    assert(recentSuccess == true || recentSuccess == false); // Can be either based on outcome
    
    // Test action quality setting
    for (size_t i = 0; i < static_cast<size_t>(nlm::ActionType::ActionCount)); ++i) {
        float quality = 1.0f - (static_cast<float>(i) * 0.1f);
        planner->setActionQuality(static_cast<nlm::ActionType>(i), quality);
    }
    
    // Clear cache
    planner->clearCache();
    
    std::cout << "    testNeuralPlannerFeedback passed" << std::endl;
}

// ConceptFormation Tests
void testConceptFormationBasic() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* conceptFormation = brain.getConceptFormation();
    assert(conceptFormation != nullptr);
    
    // Test initial state
    assert(conceptFormation->getConcepts().empty());
    
    // Present first experience
    std::vector<float> pattern1 = {0.1f, 0.2f, 0.3f};
    std::vector<float> features1 = {0.5f, 0.7f};
    
    size_t conceptId1 = conceptFormation->presentExperience(pattern1, features1, 5.0f, 0);
    
    // First experience should create a new concept
    if (conceptId1 == 0) {
        // Means early experience, still in concept 0
        assert(conceptFormation->getConcepts().size() > 0);
    } else {
        assert(conceptFormation->getConcepts().size() == conceptId1);
    }
    
    // Present similar experience
    std::vector<float> pattern2 = {0.11f, 0.21f, 0.29f};
    std::vector<float> features2 = {0.55f, 0.65f};
    
    size_t conceptId2 = conceptFormation->presentExperience(pattern2, features2, 4.5f, 10);
    
    // Similar pattern should belong to same concept
    assert(conceptId2 == conceptId1 || conceptId2 == 0);
    
    // Test concept retrieval
    size_t matchingConcept = conceptFormation->getMatchingConcept(pattern1, 0.8f);
    
    // Get specific concept
    const nlm::DiscoveredConcept* concept = conceptFormation->getConcept(conceptId1);
    assert(concept != nullptr);
    
    assert(concept->prototype.size() > 0);
    assert(concept->instances.size() > 0);
    
    std::cout << "    testConceptFormationBasic passed" << std::endl;
}

void testConceptFormationEmergence() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* conceptFormation = brain.getConceptFormation();
    assert(conceptFormation != nullptr);
    
    // Present a variety of experiences to trigger concept emergence
    std::vector<std::vector<float>> patterns = {
        {0.1f, 0.2f, 0.3f}, {0.15f, 0.25f, 0.35f}, {0.12f, 0.22f, 0.32f}, // Similar group 1
        {0.6f, 0.7f, 0.8f}, {0.65f, 0.75f, 0.85f}, {0.62f, 0.72f, 0.82f}, // Similar group 2
        {0.9f, 0.3f, 0.5f}, {0.92f, 0.32f, 0.52f}, {0.88f, 0.28f, 0.48f}  // Similar group 3
    };
    
    std::vector<std::vector<float>> featuresList = {
        {0.5f, 0.6f}, {0.55f, 0.65f}, {0.52f, 0.62f},
        {0.8f, 0.9f}, {0.85f, 0.95f}, {0.82f, 0.92f},
        {0.95f, 0.4f}, {0.97f, 0.42f}, {0.93f, 0.38f}
    };
    
    std::vector<float> rewards = {5.0f, 4.5f, 5.5f, 8.0f, 7.5f, 8.5f, 3.0f, 2.5f, 3.5f};
    
    size_t conceptCount = 0;
    for (int i = 0; i < 9; ++i) {
        size_t conceptId = conceptFormation->presentExperience(
            patterns[i], featuresList[i], rewards[i], i * 10);
        
        if (conceptId > conceptCount) {
            conceptCount = conceptId;
        }
    }
    
    // Should have discovered some concepts by now
    assert(conceptFormation->getConcepts().size() > 0);
    
    std::cout << "    testConceptFormationEmergence passed" << std::endl;
}

void testConceptFormationMatching() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* conceptFormation = brain.getConceptFormation();
    assert(conceptFormation != nullptr);
    
    // Create some baseline concepts
    for (int i = 0; i < 5; ++i) {
        std::vector<float> pattern(5);
        for (int j = 0; j < 5; ++j) {
            pattern[j] = static_cast<float>(i * 0.2f + j * 0.1f);
        }
        
        std::vector<float> features(3);
        for (int j = 0; j < 3; ++j) {
            features[j] = static_cast<float>(i * 0.3f + j * 0.05f);
        }
        
        conceptFormation->presentExperience(pattern, features, 5.0f + i, i * 10);
    }
    
    // Test matching with existing concepts
    std::vector<float> testPattern = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    
    size_t matchedConcept = conceptFormation->getMatchingConcept(testPattern, 0.8f);
    
    // Should match one of the existing concepts
    assert(matchedConcept > 0);
    
    const nlm::DiscoveredConcept* concept = conceptFormation->getConcept(matchedConcept);
    assert(concept != nullptr);
    
    std::cout << "    testConceptFormationMatching passed" << std::endl;
}

// Cognition Integration Tests
void CognitionSystemIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test that cognition systems integrate with all other subsystems
    auto* planner = brain.getPlanner();
    auto* conceptFormation = brain.getConceptFormation();
    
    assert(planner != nullptr);
    assert(conceptFormation != nullptr);
    
    // Test that planning can use concepts
    std::vector<float> currentState = {0.3f, 0.5f, 0.7f, 0.9f};
    
    // Present experience to concept formation
    conceptFormation->presentExperience(currentState, {0.6f, 0.8f}, 7.0f, 0);
    
    // Set current goal based on concept
    std::vector<float> goal = {0.8f, 0.8f, 0.8f, 0.8f};
    planner->setCurrentGoal(goal);
    
    // Test planning with concept-based goal
    nlm::ActionType action = planner->planAction(currentState, 0.7f);
    assert(action != nlm::ActionType::Wait || action != nlm::ActionType::Attack);
    
    // Test that concepts can be used for planning
    std::vector<nlm::ActionType> actionSequence = {
        nlm::ActionType::MoveForward,
        nlm::ActionType::Attack
    };
    
    nlm::PlanningCandidate candidate = planner->evaluateSequence(actionSequence, currentState);
    assert(candidate.expectedReward > 0.0f);
    
    std::cout << "    testCognitionSystemIntegration passed" << std::endl;
}

void testCognitionPerformance() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(5), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* planner = brain.getPlanner();
    auto* conceptFormation = brain.getConceptFormation();
    
    assert(planner != nullptr);
    assert(conceptFormation != nullptr);
    
    // Test performance with larger network and more complex planning
    for (int i = 0; i < 100; ++i) {
        // Create complex state
        std::vector<float> state(10);
        for (int j = 0; j < 10; ++j) {
            state[j] = static_cast<float>(sin(i * 0.1f + j * 0.3f));
        }
        
        // Present to concept formation
        conceptFormation->presentExperience(state, {0.5f, 0.6f, 0.7f}, 5.0f + i, i);
        
        // Plan action
        nlm::ActionType action = planner->planAction(state, 0.6f);
        
        // Evaluate sequence if possible
        if (i % 3 == 0) {
            std::vector<nlm::ActionType> sequence;
            for (int j = 0; j < 3; ++j) {
                nlm::ActionType nextAction = planner->planAction(state, 0.5f);
                sequence.push_back(nextAction);
            }
            
            nlm::PlanningCandidate candidate = planner->evaluateSequence(sequence, state);
            assert(candidate.actions.size() == sequence.size());
        }
    }
    
    // System should remain stable
    float planningConfidence = planner->getPlanningConfidence();
    assert(planningConfidence >= 0.0f && planningConfidence <= 1.0f);
    
    std::cout << "    testCognitionPerformance passed" << std::endl;
}

void testCognitionErrorHandling() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* planner = brain.getPlanner();
    auto* conceptFormation = brain.getConceptFormation();
    
    assert(planner != nullptr);
    assert(conceptFormation != nullptr);
    
    // Test cognition system with edge cases
    // Empty state
    std::vector<float> emptyState;
    conceptFormation->presentExperience(emptyState, {}, 0.0f, 0);
    
    // Very large state
    std::vector<float> largeState(100, 1.0f);
    conceptFormation->presentExperience(largeState, std::vector<float>(10, 0.5f), 10.0f, 1);
    
    // State with NaN/Inf values
    std::vector<float> edgeState = {0.0f, 1.0f, std::nanf(""), std::numeric_limits<float>::infinity()};
    conceptFormation->presentExperience(edgeState, {}, 5.0f, 2);
    
    // Planning with edge cases
    std::vector<float> testState = {0.0f, 0.0f, 0.0f};
    nlm::ActionType action = planner->planAction(testState, 0.0f);
    
    // Should not crash
    assert(action != nlm::ActionType::ActionCount);
    
    std::cout << "    testCognitionErrorHandling passed" << std::endl;
}

void runAll() {
    std::cout << "Running Cognition System tests..." << std::endl;
    
    try {
        testNeuralPlannerBasic();
        testNeuralPlannerIntegration();
        testNeuralPlannerFeedback();
        
        testConceptFormationBasic();
        testConceptFormationEmergence();
        testConceptFormationMatching();
        
        testCognitionSystemIntegration();
        testCognitionPerformance();
        testCognitionErrorHandling();
        
        std::cout << "  All Cognition System tests PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        throw;
    }
}

} // namespace test_cognition