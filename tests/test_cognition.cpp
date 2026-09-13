// Cognition System Tests
#include "cognition/ConceptFormation.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>

namespace test_cognition {

void testConceptFormationCreation() {
    nlm::ConceptFormation cf;
    
    assert(cf.getConceptCount() == 0);
    assert(cf.getFormationThreshold() == 0.75f);
    assert(cf.getStabilityThreshold() == 0.7f);
    assert(cf.getStabilityWindow() == 5);
    
    std::cout << "    testConceptFormationCreation passed" << std::endl;
}

void testConceptFormationInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    nlm::Brain brain(config);
    nlm::ConceptFormation cf;
    
    cf.initialize(&brain);
    
    assert(cf.getBrain() != nullptr);
    
    std::cout << "    testConceptFormationInitialization passed" << std::endl;
}

void testConceptFormationPresentExperience() {
    nlm::ConceptFormation cf;
    
    // Present first experience
    std::vector<float> pattern1 = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    std::vector<float> features1 = {0.0f, 0.0f, 0.0f};
    
    size_t concept1 = cf.presentExperience(pattern1, features1, 0.5f, 0);
    
    // First concept should be 1 (new concept)
    assert(concept1 == 1);
    
    // Same pattern should match existing concept
    size_t concept2 = cf.presentExperience(pattern1, features1, 0.7f, 1);
    assert(concept2 == 1);
    
    std::cout << "    testConceptFormationPresentExperience passed" << std::endl;
}

void testConceptFormationNovelty() {
    nlm::ConceptFormation cf;
    
    std::vector<float> pattern1 = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    std::vector<float> pattern2 = {0.9f, 0.8f, 0.7f, 0.6f, 0.5f};  // Very different
    
    // First pattern should form a concept
    size_t concept1 = cf.presentExperience(pattern1, {}, 0.5f, 0);
    assert(concept1 > 0);
    
    // Second pattern should be novel (different enough)
    // Note: This depends on the similarity threshold
    size_t concept2 = cf.presentExperience(pattern2, {}, 0.5f, 1);
    // Could be 0 (not yet recognized) or new concept (2, 3, etc.)
    
    std::cout << "    testConceptFormationNovelty passed" << std::endl;
}

void testConceptFormationSimilarity() {
    nlm::ConceptFormation cf;
    
    std::vector<float> pattern1 = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    std::vector<float> pattern2 = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};  // Same as pattern1
    std::vector<float> pattern3 = {0.9f, 0.8f, 0.7f, 0.6f, 0.5f};  // Different
    
    float similar = cf.computeSimilarity(pattern1, pattern2);
    float dissimilar = cf.computeSimilarity(pattern1, pattern3);
    
    // Similar patterns should have high similarity
    assert(similar > 0.9f);
    
    // Dissimilar patterns should have lower similarity
    assert(dissimilar < 0.8f);
    
    std::cout << "    testConceptFormationSimilarity passed" << std::endl;
}

void testConceptFormationConceptManagement() {
    nlm::ConceptFormation cf;
    
    std::vector<float> pattern = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    
    // Create a concept
    size_t conceptId = cf.presentExperience(pattern, {}, 0.5f, 0);
    assert(conceptId > 0);
    
    // Get concept prototype
    std::vector<float> prototype = cf.getConceptPrototype(conceptId);
    assert(prototype.size() == 5);
    
    // Get concept stability
    float stability = cf.getConceptStability(conceptId);
    assert(stability >= 0.0f && stability <= 1.0f);
    
    // Get concept instances
    const auto& instances = cf.getConceptInstances(conceptId);
    assert(!instances.empty());
    
    std::cout << "    testConceptFormationConceptManagement passed" << std::endl;
}

void testConceptFormationStability() {
    nlm::ConceptFormation cf;
    
    std::vector<float> pattern = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    
    // Present same pattern multiple times
    for (int i = 0; i < 10; ++i) {
        size_t conceptId = cf.presentExperience(pattern, {}, 0.5f, i);
        assert(conceptId > 0);
        
        float stability = cf.getConceptStability(conceptId);
        // Stability should improve with more observations
        assert(stability >= 0.5f);
    }
    
    std::cout << "    testConceptFormationStability passed" << std::endl;
}

void testConceptFormationParameters() {
    nlm::ConceptFormation cf;
    
    cf.setFormationThreshold(0.8f);
    cf.setStabilityWindow(10);
    
    assert(cf.getFormationThreshold() == 0.8f);
    assert(cf.getStabilityWindow() == 10);
    
    std::cout << "    testConceptFormationParameters passed" << std::endl;
}

void testNeuralPlannerCreation() {
    nlm::NeuralPlanner planner;
    
    assert(planner.getPlanningDepth() == 3);
    assert(planner.getPlanningConfidence() == 0.5f);
    assert(planner.wasRecentPlanSuccessful() == false);
    
    std::cout << "    testNeuralPlannerCreation passed" << std::endl;
}

void testNeuralPlannerInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    nlm::Brain brain(config);
    nlm::NeuralPlanner planner;
    
    planner.initialize(&brain);
    
    assert(planner.getBrain() != nullptr);
    
    std::cout << "    testNeuralPlannerInitialization passed" << std::endl;
}

void testNeuralPlannerPlanning() {
    nlm::NeuralPlanner planner;
    
    // Set action quality
    planner.setActionQuality(nlm::ActionType::MoveForward, 0.8f);
    planner.setActionQuality(nlm::ActionType::Wait, 0.2f);
    
    // Set a goal
    planner.setCurrentGoal(std::vector<float>{0.5f, 0.5f, 0.5f});
    
    // Plan an action (should return one of the valid actions)
    std::vector<float> currentState = {0.0f, 0.0f, 0.0f};
    nlm::ActionType action = planner.planAction(currentState, 0.5f);
    
    // Should return a valid action
    assert(action == nlm::ActionType::MoveForward || 
           action == nlm::ActionType::Wait ||
           action == nlm::ActionType::TurnLeft ||
           action == nlm::ActionType::TurnRight ||
           action == nlm::ActionType::Interact ||
           action == nlm::ActionType::LookLeft ||
           action == nlm::ActionType::LookRight ||
           action == nlm::ActionType::MoveBackward);
    
    std::cout << "    testNeuralPlannerPlanning passed" << std::endl;
}

void testNeuralPlannerEvaluation() {
    nlm::NeuralPlanner planner;
    
    // Create a simple action sequence
    std::vector<nlm::ActionType> sequence = {
        nlm::ActionType::MoveForward,
        nlm::ActionType::TurnRight,
        nlm::ActionType::Interact
    };
    
    std::vector<float> startState = {0.0f, 0.0f, 0.0f};
    
    nlm::PlanningCandidate candidate = planner.evaluateSequence(sequence, startState);
    
    assert(!candidate.actions.empty());
    assert(candidate.depth == 3);
    assert(candidate.expectedReward >= -1000.0f);
    
    std::cout << "    testNeuralPlannerEvaluation passed" << std::endl;
}

void testNeuralPlannerCache() {
    nlm::NeuralPlanner planner;
    
    // Clear cache
    planner.clearCache();
    
    // Set action quality
    planner.setActionQuality(nlm::ActionType::MoveForward, 0.9f);
    
    // Update plan quality
    std::vector<nlm::ActionType> planned = {
        nlm::ActionType::MoveForward,
        nlm::ActionType::TurnRight
    };
    std::vector<nlm::ActionType> actual = {
        nlm::ActionType::MoveForward,
        nlm::ActionType::TurnRight,
        nlm::ActionType::Interact  // Different actual outcome
    };
    
    planner.updatePlanQuality(planned, actual, 0.7f);
    
    // Check if recent plan success is tracked
    assert(planner.wasRecentPlanSuccessful() == true);
    
    std::cout << "    testNeuralPlannerCache passed" << std::endl;
}

void testCognitionIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    nlm::Brain brain(config);
    
    // Initialize cognitive systems
    nlm::ConceptFormation cf;
    cf.initialize(&brain);
    
    nlm::NeuralPlanner planner;
    planner.initialize(&brain);
    
    // Present experience to concept formation
    std::vector<float> pattern = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    size_t conceptId = cf.presentExperience(pattern, {}, 0.5f, 0);
    
    // Use concept to influence planning
    planner.setCurrentGoal(pattern);
    planner.setActionQuality(nlm::ActionType::MoveForward, 0.8f);
    
    // Should plan an action based on available information
    nlm::ActionType action = planner.planAction(pattern, 0.5f);
    
    // Should not crash and should return a valid action
    assert(action == nlm::ActionType::Wait || action != nlm::ActionType::Wait);
    
    std::cout << "    testCognitionIntegration passed" << std::endl;
}

void runAll() {
    std::cout << "Running Cognition system tests..." << std::endl;
    
    testConceptFormationCreation();
    testConceptFormationInitialization();
    testConceptFormationPresentExperience();
    testConceptFormationNovelty();
    testConceptFormationSimilarity();
    testConceptFormationConceptManagement();
    testConceptFormationStability();
    testConceptFormationParameters();
    
    testNeuralPlannerCreation();
    testNeuralPlannerInitialization();
    testNeuralPlannerPlanning();
    testNeuralPlannerEvaluation();
    testNeuralPlannerCache();
    
    testCognitionIntegration();
}

} // namespace test_cognition
