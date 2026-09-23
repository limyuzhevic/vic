// NeuralPlanner Tests
#include "brain/NeuralPlanner.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <vector>

namespace test_neuralplanner {

void testNeuralPlannerCreation() {
    nlm::NeuralPlanner planner;
    
    // Should be able to access via Brain interface
    std::cout << "    testNeuralPlannerCreation passed" << std::endl;
}

void testNeuralPlannerConfiguration() {
    nlm::NeuralPlanner planner;
    
    // Set planning parameters
    planner.setPlanningHorizon(20);
    planner.setPlanningSpeed(5.0f);
    planner.setExplorationWeight(0.3f);
    planner.setExploitationWeight(0.7f);
    
    // Get planning parameters
    size_t horizon = planner.getPlanningHorizon();
    assert(horizon > 0);
    
    float speed = planner.getPlanningSpeed();
    assert(speed > 0.0f);
    
    std::cout << "    testNeuralPlannerConfiguration passed" << std::endl;
}

void testNeuralPlannerPlan() {
    nlm::NeuralPlanner planner;
    
    // Create a simple goal
    nlm::NeuralPlanner::Goal goal;
    goal.target = {10.0f, 20.0f};
    goal.objective = "reach_target";
    goal.priority = 1.0f;
    goal.constraints = {"max_steps", "max_energy"};
    
    // Create another goal for comparison
    nlm::NeuralPlanner::Goal goal2;
    goal2.target = {15.0f, 25.0f};
    goal2.objective = "collect_item";
    goal2.priority = 0.8f;
    
    // Add goals
    planner.addGoal(goal);
    planner.addGoal(goal2);
    
    // Get planned action
    nlm::MotorCommand action = planner.planAction();
    
    // Should not crash
    assert(action != nullptr);
    
    std::cout << "    testNeuralPlannerPlan passed" << std::endl;
}

void testNeuralPlannerLearning() {
    nlm::NeuralPlanner planner;
    
    // Create a goal
    nlm::NeuralPlanner::Goal goal;
    goal.target = {5.0f, 5.0f};
    goal.objective = "test";
    goal.priority = 1.0f;
    
    // Add goal
    planner.addGoal(goal);
    
    // Learn from outcomes
    std::vector<double> rewards = {0.5, 0.8, 0.3, 0.9};
    planner.learnFromOutcomes(rewards);
    
    // Get updated action
    nlm::MotorCommand action = planner.planAction();
    
    // Should not crash
    assert(action != nullptr);
    
    std::cout << "    testNeuralPlannerLearning passed" << std::endl;
}

void testNeuralPlannerEmergency() {
    nlm::NeuralPlanner planner;
    
    // Set up emergency override
    planner.setEmergencyMode(true);
    
    // Create a goal
    nlm::NeuralPlanner::Goal goal;
    goal.target = {10.0f, 10.0f};
    goal.objective = "normal_planning";
    goal.priority = 0.5f;
    
    // Add goal
    planner.addGoal(goal);
    
    // Get action (should use emergency planning)
    nlm::MotorCommand action = planner.planAction();
    
    // Should not crash
    assert(action != nullptr);
    
    std::cout << "    testNeuralPlannerEmergency passed" << std::endl;
}

void runAll() {
    std::cout << "Running NeuralPlanner tests..." << std::endl;
    testNeuralPlannerCreation();
    testNeuralPlannerConfiguration();
    testNeuralPlannerPlan();
    testNeuralPlannerLearning();
    testNeuralPlannerEmergency();
}

} // namespace test_neuralplanner