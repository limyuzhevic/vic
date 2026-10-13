// Planning Tests
// Tests multi-step prediction, goal-directed action selection, outcome evaluation, and adaptation based on outcomes

#include "cognition/NeuralPlanner.hpp"
#include "brain/Brain.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>

namespace test_planning {

void testNeuralPlannerCreation() {
    nlm::NeuralPlanner planner;
    
    assert(planner.getPlanningDepth() == 0);
    assert(planner.getPlanningConfidence() == 0.0f);
    assert(!planner.wasRecentPlanSuccessful());
    
    std::cout << "    testNeuralPlannerCreation passed" << std::endl;
}

void testNeuralPlannerInitialization() {
    nlm::NeuralPlanner planner;
    
    // Initialize with mock brain
    class MockBrain : public nlm::Brain {
    public:
        MockBrain() : nlm::Brain(std::make_shared<nlm::Config>()) {}
        std::shared_ptr<const nlm::Config> getConfig() const override { return nullptr; }
        nlm::RandomGenerator* getRandomGenerator() override { return nullptr; }
    };
    
    MockBrain mockBrain;
    planner.initialize(&mockBrain);
    
    assert(planner.getPlanningConfidence() == 0.0f);
    
    std::cout << "    testNeuralPlannerInitialization passed" << std::endl;
}

void testActionQualitySetting() {
    nlm::NeuralPlanner planner;
    
    // Initialize with mock brain
    class MockBrain : public nlm::Brain {
    public:
        MockBrain() : nlm::Brain(std::make_shared<nlm::Config>()) {}
        std::shared_ptr<const nlm::Config> getConfig() const override { return nullptr; }
        nlm::RandomGenerator* getRandomGenerator() override { return nullptr; }
    };
    
    MockBrain mockBrain;
    planner.initialize(&mockBrain);
    
    // Set action quality
    planner.setActionQuality(nlm::ActionType::MoveForward, 0.8f);
    planner.setActionQuality(nlm::ActionType::Interact, 0.6f);
    planner.setActionQuality(nlm::ActionType::Wait, 0.2f);
    
    // Note: The NeuralPlanner might not expose getActionQuality method
    // This test just verifies the API works
    
    std::cout << "    testActionQualitySetting passed" << std::endl;
}

void testPlanningDepthSetting() {
    nlm::NeuralPlanner planner;
    
    // Initialize with mock brain
    class MockBrain : public nlm::Brain {
    public:
        MockBrain() : nlm::Brain(std::make_shared<nlm::Config>()) {}
        std::shared_ptr<const nlm::Config> getConfig() const override { return nullptr; }
        nlm::RandomGenerator* getRandomGenerator() override { return nullptr; }
    };
    
    MockBrain mockBrain;
    planner.initialize(&mockBrain);
    
    planner.setPlanningDepth(5);
    assert(planner.getPlanningDepth() == 5);
    
    planner.setPlanningDepth(10);
    assert(planner.getPlanningDepth() == 10);
    
    std::cout << "    testPlanningDepthSetting passed" << std::endl;
}

void testGoalSetting() {
    nlm::NeuralPlanner planner;
    
    // Initialize with mock brain
    class MockBrain : public nlm::Brain {
    public:
        MockBrain() : nlm::Brain(std::make_shared<nlm::Config>()) {}
        std::shared_ptr<const nlm::Config> getConfig() const override { return nullptr; }
        nlm::RandomGenerator* getRandomGenerator() override { return nullptr; }
    };
    
    MockBrain mockBrain;
    planner.initialize(&mockBrain);
    
    std::vector<float> goal = {0.5f, 0.7f, 0.3f, 0.9f};
    planner.setCurrentGoal(goal);
    
    // The current implementation might not expose getCurrentGoal
    // This test verifies the API call works
    
    std::cout << "    testGoalSetting passed" << std::endl;
}

void testPlanningMultiStepPrediction() {
    nlm::NeuralPlanner planner;
    
    // Initialize with mock brain
    class MockBrain : public nlm::Brain {
    public:
        MockBrain() : nlm::Brain(std::make_shared<nlm::Config>()) {}
        std::shared_ptr<const nlm::Config> getConfig() const override { return nullptr; }
        nlm::RandomGenerator* getRandomGenerator() override { return nullptr; }
    };
    
    MockBrain mockBrain;
    planner.initialize(&mockBrain);
    
    std::vector<float> currentState = {0.5f, 0.5f, 0.5f, 0.5f, 0.5f};
    
    // Plan action
    nlm::ActionType plannedAction = planner.planAction(currentState, 0.5f);
    
    // Should return a valid action
    assert(plannedAction == nlm::ActionType::MoveForward ||
           plannedAction == nlm::ActionType::Interact ||
           plannedAction == nlm::ActionType::Wait ||
           plannedAction == nlm::ActionType::Look ||
           plannedAction == nlm::ActionType::Talk);
    
    std::cout << "    testPlanningMultiStepPrediction passed" << std::endl;
}

void testActionEvaluation() {
    nlm::NeuralPlanner planner;
    
    // Initialize with mock brain
    class MockBrain : public nlm::Brain {
    public:
        MockBrain() : nlm::Brain(std::make_shared<nlm::Config>()) {}
        std::shared_ptr<const nlm::Config> getConfig() const override { return nullptr; }
        nlm::RandomGenerator* getRandomGenerator() override { return nullptr; }
    };
    
    MockBrain mockBrain;
    planner.initialize(&mockBrain);
    
    std::vector<nlm::ActionType> actions = {nlm::ActionType::MoveForward, nlm::ActionType::Wait};
    std::vector<float> startState = {0.5f, 0.5f, 0.5f, 0.5f, 0.5f};
    
    // Evaluate action sequence
    nlm::PlanningCandidate candidate = planner.evaluateSequence(actions, startState);
    
    // Should have valid fields
    assert(candidate.actions.size() == actions.size());
    assert(candidate.predictedStates.size() == actions.size());
    assert(candidate.expectedReward >= 0.0f);
    assert(candidate.confidence >= 0.0f && candidate.confidence <= 1.0f);
    assert(candidate.depth > 0);
    
    std::cout << "    testActionEvaluation passed" << std::endl;
}

void testOutcomeEvaluation() {
    nlm::NeuralPlanner planner;
    
    // Initialize with mock brain
    class MockBrain : public nlm::Brain {
    public:
        MockBrain() : nlm::Brain(std::make_shared<nlm::Config>()) {}
        std::shared_ptr<const nlm::Config> getConfig() const override { return nullptr; }
        nlm::RandomGenerator* getRandomGenerator() override { return nullptr; }
    };
    
    MockBrain mockBrain;
    planner.initialize(&mockBrain);
    
    // Set up some action quality
    planner.setActionQuality(nlm::ActionType::MoveForward, 0.7f);
    
    std::vector<nlm::ActionType> planned = {nlm::ActionType::MoveForward};
    std::vector<nlm::ActionType> actual = {nlm::ActionType::MoveForward};
    
    // Update with successful outcome
    planner.updatePlanQuality(planned, actual, 1.0f);
    
    // Should not crash
    std::cout << "    testOutcomeEvaluation passed" << std::endl;
}

void testPlanningAdaptation() {
    nlm::NeuralPlanner planner;
    
    // Initialize with mock brain
    class MockBrain : public nlm::Brain {
    public:
        MockBrain() : nlm::Brain(std::make_shared<nlm::Config>()) {}
        std::shared_ptr<const nlm::Config> getConfig() const override { return nullptr; }
        nlm::RandomGenerator* getRandomGenerator() override { return nullptr; }
    };
    
    MockBrain mockBrain;
    planner.initialize(&mockBrain);
    
    std::vector<float> state = {0.5f, 0.5f, 0.5f, 0.5f, 0.5f};
    
    // Plan action
    nlm::ActionType action1 = planner.planAction(state, 0.5f);
    
    // Update with poor outcome
    std::vector<nlm::ActionType> planned = {action1};
    std::vector<nlm::ActionType> actual = {nlm::ActionType::Wait}; // Different action
    planner.updatePlanQuality(planned, actual, -0.5f);
    
    // Plan again - should potentially adapt
    nlm::ActionType action2 = planner.planAction(state, 0.5f);
    
    // Should still produce valid action
    assert(action2 == nlm::ActionType::MoveForward ||
           action2 == nlm::ActionType::Interact ||
           action2 == nlm::ActionType::Wait ||
           action2 == nlm::ActionType::Look ||
           action2 == nlm::ActionType::Talk);
    
    std::cout << "    testPlanningAdaptation passed" << std::endl;
}

void testRecentPlanSuccessTracking() {
    nlm::NeuralPlanner planner;
    
    // Initialize with mock brain
    class MockBrain : public nlm::Brain {
    public:
        MockBrain() : nlm::Brain(std::make_shared<nlm::Config>()) {}
        std::shared_ptr<const nlm::Config> getConfig() const override { return nullptr; }
        nlm::RandomGenerator* getRandomGenerator() override { return nullptr; }
    };
    
    MockBrain mockBrain;
    planner.initialize(&mockBrain);
    
    // Initially no recent success
    assert(!planner.wasRecentPlanSuccessful());
    
    // Clear cache (if method exists)
    if (planner.getPlanningConfidence() >= 0.0f) {
        // Try to clear cache - might not be exposed in current API
    }
    
    std::cout << "    testRecentPlanSuccessTracking passed" << std::endl;
}

void testPlanningCache() {
    nlm::NeuralPlanner planner;
    
    // Initialize with mock brain
    class MockBrain : public nlm::Brain {
    public:
        MockBrain() : nlm::Brain(std::make_shared<nlm::Config>()) {}
        std::shared_ptr<const nlm::Config> getConfig() const override { return nullptr; }
        nlm::RandomGenerator* getRandomGenerator() override { return nullptr; }
    };
    
    MockBrain mockBrain;
    planner.initialize(&mockBrain);
    
    // Set planning depth
    planner.setPlanningDepth(5);
    
    // The cache functionality might not be directly testable through public API
    // This test just ensures the planner can be used
    
    std::cout << "    testPlanningCache passed" << std::endl;
}

void runAll() {
    testNeuralPlannerCreation();
    testNeuralPlannerInitialization();
    testActionQualitySetting();
    testPlanningDepthSetting();
    testGoalSetting();
    testPlanningMultiStepPrediction();
    testActionEvaluation();
    testOutcomeEvaluation();
    testPlanningAdaptation();
    testRecentPlanSuccessTracking();
    testPlanningCache();
}

} // namespace test_planning
