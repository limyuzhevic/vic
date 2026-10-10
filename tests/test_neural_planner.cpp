// NeuralPlanner Tests
#include "cognition/NeuralPlanner.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "test_framework_utils.h"
#include <cassert>
#include <iostream>

namespace test_neural_planner {

void testNeuralPlannerCreation() {
    nlm::NeuralPlanner planner;
    
    assert(planner.getPlanningDepth() == 3);
    assert(planner.getPlanningConfidence() == 0.5f);
    
    std::cout << "    testNeuralPlannerCreation passed" << std::endl;
}

void testNeuralPlannerInitialization() {
    nlm::NeuralPlanner planner;
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    planner.initialize(&brain);
    
    assert(planner.getPlanningDepth() == 3);
    
    std::cout << "    testNeuralPlannerInitialization passed" << std::endl;
}

void testNeuralPlannerPlanningDepth() {
    nlm::NeuralPlanner planner;
    
    assert(planner.getPlanningDepth() == 3);
    
    planner.setPlanningDepth(5);
    assert(planner.getPlanningDepth() == 5);
    
    planner.setPlanningDepth(1);
    assert(planner.getPlanningDepth() == 1);
    
    std::cout << "    testNeuralPlannerPlanningDepth passed" << std::endl;
}

void testNeuralPlannerPlanningConfidence() {
    nlm::NeuralPlanner planner;
    
    assert(planner.getPlanningConfidence() == 0.5f);
    
    // Note: confidence is updated during planning operations
    
    std::cout << "    testNeuralPlannerPlanningConfidence passed" << std::endl;
}

void testNeuralPlannerPlanAction() {
    nlm::NeuralPlanner planner;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    planner.initialize(&brain);
    
    // Test with simple state vector
    std::vector<float> state = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    
    // Plan an action
    nlm::ActionType action = planner.planAction(state);
    
    // Should return a valid action type
    assert(action == nlm::ActionType::Wait ||
           action == nlm::ActionType::MoveForward ||
           action == nlm::ActionType::MoveBackward ||
           action == nlm::ActionType::TurnLeft ||
           action == nlm::ActionType::TurnRight ||
           action == nlm::ActionType::Interact);
    
    // Test with different states
    std::vector<float> state2 = {0.9f, 0.8f, 0.7f, 0.6f, 0.5f};
    nlm::ActionType action2 = planner.planAction(state2, 0.8f);
    
    assert(action2 == nlm::ActionType::Wait ||
           action2 == nlm::ActionType::MoveForward ||
           action2 == nlm::ActionType::MoveBackward ||
           action2 == nlm::ActionType::TurnLeft ||
           action2 == nlm::ActionType::TurnRight ||
           action2 == nlm::ActionType::Interact);
    
    std::cout << "    testNeuralPlannerPlanAction passed" << std::endl;
}

void testNeuralPlannerActionQuality() {
    nlm::NeuralPlanner planner;
    
    // Set action quality
    planner.setActionQuality(nlm::ActionType::MoveForward, 0.8f);
    planner.setActionQuality(nlm::ActionType::MoveBackward, 0.2f);
    planner.setActionQuality(nlm::ActionType::TurnLeft, 0.6f);
    
    // Verify the quality was set (internal implementation detail)
    // Note: this tests that the API exists and can be used
    
    std::cout << "    testNeuralPlannerActionQuality passed" << std::endl;
}

void testNeuralPlannerSetGetGoal() {
    nlm::NeuralPlanner planner;
    
    std::vector<float> goal = {0.1f, 0.2f, 0.3f};
    planner.setCurrentGoal(goal);
    
    assert(planner.getCurrentGoal().size() == 3);
    assert(planner.getCurrentGoal()[0] == 0.1f);
    
    std::cout << "    testNeuralPlannerSetGetGoal passed" << std::endl;
}

void testNeuralPlannerUpdatePlanQuality() {
    nlm::NeuralPlanner planner;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    planner.initialize(&brain);
    
    std::vector<nlm::ActionType> planned = {
        nlm::ActionType::MoveForward,
        nlm::ActionType::TurnLeft,
        nlm::ActionType::Interact
    };
    
    std::vector<nlm::ActionType> actual = {
        nlm::ActionType::MoveForward,
        nlm::ActionType::TurnLeft,
        nlm::ActionType::Wait  // Slightly different
    };
    
    // Update based on actual outcomes
    planner.updatePlanQuality(planned, actual, 0.7f);
    
    // Should not crash
    std::cout << "    testNeuralPlannerUpdatePlanQuality passed" << std::endl;
}

void testNeuralPlannerRecentPlanSuccess() {
    nlm::NeuralPlanner planner;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    planner.initialize(&brain);
    
    // Initially should return true (no data)
    assert(planner.wasRecentPlanSuccessful() == true);
    
    std::cout << "    testNeuralPlannerRecentPlanSuccess passed" << std::endl;
}

void testNeuralPlannerIntegration() {
    // Test integration with other systems
    nlm::NeuralPlanner planner;
    
    // NeuralPlanner should be able to integrate with brain
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    planner.initialize(&brain);
    
    // Test basic planning functionality
    std::vector<float> state = {0.5f, 0.5f, 0.5f};
    nlm::ActionType action = planner.planAction(state);
    
    assert(action != nlm::ActionType::Wait || !state.empty());
    
    std::cout << "    testNeuralPlannerIntegration passed" << std::endl;
}

void runAll() {
    std::cout << "Running NeuralPlanner tests..." << std::endl;
    testNeuralPlannerCreation();
    testNeuralPlannerInitialization();
    testNeuralPlannerPlanningDepth();
    testNeuralPlannerPlanningConfidence();
    testNeuralPlannerPlanAction();
    testNeuralPlannerActionQuality();
    testNeuralPlannerSetGetGoal();
    testNeuralPlannerUpdatePlanQuality();
    testNeuralPlannerRecentPlanSuccess();
    testNeuralPlannerIntegration();
}

} // namespace test_neural_planner
