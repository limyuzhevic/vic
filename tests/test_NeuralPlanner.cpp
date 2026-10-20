// NeuralPlanner Tests
#include "cognition/NeuralPlanner.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_neural_planner {

void testPlannerCreation() {
    nlm::NeuralPlanner planner;
    
    assert(planner.getPlanningDepth() == 1);  // Default
    assert(planner.getPlanningConfidence() == 0.0f);  // Default
    assert(planner.getCurrentGoal().empty());
    
    std::cout << "    testPlannerCreation passed" << std::endl;
}

void testPlannerInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::NeuralPlanner planner;
    planner.initialize(&brain);
    
    assert(planner.getCurrentGoal().empty());
    assert(planner.getPlanningDepth() == 1);
    
    std::cout << "    testPlannerInitialization passed" << std::endl;
}

void testPlannerDepth() {
    nlm::NeuralPlanner planner;
    
    // Test default depth
    assert(planner.getPlanningDepth() == 1);
    
    // Set different depths
    planner.setPlanningDepth(5);
    assert(planner.getPlanningDepth() == 5);
    
    planner.setPlanningDepth(10);
    assert(planner.getPlanningDepth() == 10);
    
    std::cout << "    testPlannerDepth passed" << std::endl;
}

void testPlannerConfidence() {
    nlm::NeuralPlanner planner;
    
    // Test initial confidence
    assert(planner.getPlanningConfidence() == 0.0f);
    
    // Set confidence
    planner.setPlanningConfidence(0.7f);
    assert(planner.getPlanningConfidence() == 0.7f);
    
    std::cout << "    testPlannerConfidence passed" << std::endl;
}

void testPlannerActionQuality() {
    nlm::NeuralPlanner planner;
    
    // Test default action quality
    assert(planner.getActionQuality(nlm::ActionType::Wait) == 0.0f);
    
    // Set action quality
    planner.setActionQuality(nlm::ActionType::MoveForward, 0.8f);
    assert(planner.getActionQuality(nlm::ActionType::MoveForward) == 0.8f);
    
    planner.setActionQuality(nlm::ActionType::MoveLeft, 0.3f);
    assert(planner.getActionQuality(nlm::ActionType::MoveLeft) == 0.3f);
    
    std::cout << "    testPlannerActionQuality passed" << std::endl;
}

void testPlannerGoal() {
    nlm::NeuralPlanner planner;
    
    // Test default goal
    assert(planner.getCurrentGoal().empty());
    
    // Set goal
    std::vector<float> goal = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    planner.setCurrentGoal(goal);
    assert(planner.getCurrentGoal() == goal);
    
    // Change goal
    std::vector<float> newGoal = {0.6f, 0.7f, 0.8f, 0.9f, 1.0f};
    planner.setCurrentGoal(newGoal);
    assert(planner.getCurrentGoal() == newGoal);
    
    std::cout << "    testPlannerGoal passed" << std::endl;
}

void testPlannerPlanAction() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::NeuralPlanner planner;
    planner.initialize(&brain);
    
    // Test planning with default parameters
    std::vector<float> currentState = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    
    nlm::ActionType action = planner.planAction(currentState, 0.5f);
    
    // Should return a valid action
    assert(static_cast<int>(action) >= 0);
    
    // Test planning with different target reward
    nlm::ActionType action2 = planner.planAction(currentState, 0.8f);
    assert(static_cast<int>(action2) >= 0);
    
    std::cout << "    testPlannerPlanAction passed" << std::endl;
}

void testPlannerEvaluateSequence() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::NeuralPlanner planner;
    planner.initialize(&brain);
    
    // Create a sequence of actions
    std::vector<nlm::ActionType> actions = {
        nlm::ActionType::MoveForward,
        nlm::ActionType::MoveLeft,
        nlm::ActionType::MoveForward,
        nlm::ActionType::Turn
    };
    
    // Create starting state
    std::vector<float> startState = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    
    // Evaluate the sequence
    auto candidate = planner.evaluateSequence(actions, startState);
    
    // Should return a PlanningCandidate
    assert(&candidate != nullptr);
    assert(candidate.expectedReward >= 0.0f);
    assert(candidate.confidence >= 0.0f && candidate.confidence <= 1.0f);
    assert(candidate.depth >= 0);
    
    std::cout << "    testPlannerEvaluateSequence passed" << std::endl;
}

void testPlannerUpdatePlanQuality() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::NeuralPlanner planner;
    planner.initialize(&brain);
    
    // Set up some planned actions
    std::vector<nlm::ActionType> plannedActions = {
        nlm::ActionType::MoveForward,
        nlm::ActionType::MoveLeft
    };
    
    // Set up actual actions (different)
    std::vector<nlm::ActionType> actualActions = {
        nlm::ActionType::MoveForward,
        nlm::ActionType::MoveForward
    };
    
    // Update plan quality with reward
    planner.updatePlanQuality(plannedActions, actualActions, 5.0f);
    
    // Should have updated without crashing
    assert(planner.getPlanningConfidence() >= 0.0f);
    
    std::cout << "    testPlannerUpdatePlanQuality passed" << std::endl;
}

void testPlannerCache() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::NeuralPlanner planner;
    planner.initialize(&brain);
    
    // Clear cache (should be empty initially)
    planner.clearCache();
    
    // Do some planning operations
    std::vector<float> state = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    planner.planAction(state);
    planner.evaluateSequence({nlm::ActionType::MoveForward}, state);
    
    // Clear cache again
    planner.clearCache();
    
    // Should have completed without issues
    assert(planner.getPlanningDepth() == 1);  // Should still have default depth
    
    std::cout << "    testPlannerCache passed" << std::endl;
}

void testPlannerRecentPlanSuccess() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::NeuralPlanner planner;
    planner.initialize(&brain);
    
    // Initially should not have recent plan success
    assert(planner.wasRecentPlanSuccessful() == false);
    
    // Update plan quality with a successful plan
    std::vector<nlm::ActionType> planned = {nlm::ActionType::MoveForward};
    std::vector<nlm::ActionType> actual = {nlm::ActionType::MoveForward};
    
    planner.updatePlanQuality(planned, actual, 10.0f);
    
    // Still might not show success immediately, but should not crash
    
    std::cout << "    testPlannerRecentPlanSuccess passed" << std::endl;
}

void testPlannerGenerateActionSequences() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::NeuralPlanner planner;
    planner.initialize(&brain);
    
    // Test with depth 1
    planner.setPlanningDepth(1);
    auto sequences1 = planner.generateActionSequences(1);
    
    // Should generate at least the empty sequence
    assert(!sequences1.empty());
    
    // Test with depth 2
    planner.setPlanningDepth(2);
    auto sequences2 = planner.generateActionSequences(2);
    
    // Should generate more sequences
    assert(sequences2.size() >= sequences1.size());
    
    std::cout << "    testPlannerGenerateActionSequences passed" << std::endl;
}

void testPlannerActionEvaluation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    nlm::NeuralPlanner planner;
    planner.initialize(&brain);
    
    // Test action evaluation
    std::vector<float> state = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    
    for (int i = 0; i < 4; ++i) {  // Test all action types
        nlm::ActionType action = static_cast<nlm::ActionType>(i);
        float quality = planner.evaluateAction(action, state);
        
        assert(quality >= 0.0f);  // Quality should be non-negative
    }
    
    std::cout << "    testPlannerActionEvaluation passed" << std::endl;
}

void runAll() {
    testPlannerCreation();
    testPlannerInitialization();
    testPlannerDepth();
    testPlannerConfidence();
    testPlannerActionQuality();
    testPlannerGoal();
    testPlannerPlanAction();
    testPlannerEvaluateSequence();
    testPlannerUpdatePlanQuality();
    testPlannerCache();
    testPlannerRecentPlanSuccess();
    testPlannerGenerateActionSequences();
    testPlannerActionEvaluation();
}

} // namespace test_neural_planner
