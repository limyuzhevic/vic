// test_phase4_planner.cpp
// Tests NeuralPlanner functionality

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include <memory>

namespace nlm {

TEST_CASE("Phase 4 Neural Planner Tests", "[planner]") {
    
    SECTION("Test NeuralPlanner initialization") {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100);
        
        auto brain = std::make_shared<Brain>(cfg);
        brain->initialize();
        
        NeuralPlanner planner;
        planner.initialize(brain.get());
        
        // Test default parameters
        REQUIRE(planner.getPlanningDepth() == 3);
        REQUIRE(planner.getPlanningConfidence() == 0.5f);
        
        // Test parameter setting
        planner.setPlanningDepth(5);
        REQUIRE(planner.getPlanningDepth() == 5);
        
        planner.setCurrentGoal(std::vector<float>(50, 0.5f));
        REQUIRE(planner.getCurrentGoal().size() == 50);
    }
    
    SECTION("Test NeuralPlanner action planning") {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100);
        
        auto brain = std::make_shared<Brain>(cfg);
        brain->initialize();
        
        NeuralPlanner planner;
        planner.initialize(brain.get());
        
        // Set up action quality
        planner.setActionQuality(ActionType::MoveForward, 0.7f);
        planner.setActionQuality(ActionType::Interact, 0.8f);
        planner.setActionQuality(ActionType::Wait, 0.0f);
        
        // Test planning with state
        std::vector<float> currentState(50, 0.5f);
        ActionType action = planner.planAction(currentState, 0.5f);
        
        // Should return a valid action
        REQUIRE(action == ActionType::MoveForward || 
                action == ActionType::Interact || 
                action == ActionType::Wait);
        
        // Test goal-based planning
        std::vector<float> goal(50, 0.9f);
        planner.setCurrentGoal(goal);
        
        ActionType goalAction = planner.planAction(currentState, 0.5f);
        REQUIRE(goalAction != ActionType::Wait || planner.getPlanningConfidence() < 0.5f);
    }
    
    SECTION("Test NeuralPlanner sequence evaluation") {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100);
        
        auto brain = std::make_shared<Brain>(cfg);
        brain->initialize();
        
        NeuralPlanner planner;
        planner.initialize(brain.get());
        
        // Set up action quality
        std::vector<ActionType> testActions = {ActionType::MoveForward, ActionType::Interact};
        
        // Test sequence evaluation
        std::vector<ActionType> sequence = {ActionType::MoveForward, ActionType::Interact};
        std::vector<float> startState(50, 0.5f);
        
        PlanningCandidate candidate = planner.evaluateSequence(sequence, startState);
        
        // Should evaluate sequence
        REQUIRE(candidate.depth == sequence.size());
        REQUIRE(candidate.expectedReward >= -1000.0f);  // Reasonable range
        REQUIRE(candidate.confidence >= 0.0f && candidate.confidence <= 1.0f);
    }
    
    SECTION("Test NeuralPlanner update mechanisms") {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100);
        
        auto brain = std::make_shared<Brain>(cfg);
        brain->initialize();
        
        NeuralPlanner planner;
        planner.initialize(brain.get());
        
        // Set up initial quality
        planner.setActionQuality(ActionType::MoveForward, 0.5f);
        
        // Test plan quality updates
        std::vector<ActionType> plannedActions = {ActionType::MoveForward};
        std::vector<ActionType> actualActions = {ActionType::MoveForward};
        
        planner.updatePlanQuality(plannedActions, actualActions, 1.0f);
        
        // Quality should be updated
        float quality = planner.getPlanningConfidence();
        REQUIRE(quality >= 0.0f);
        
        // Test plan success tracking
        bool recentSuccess = planner.wasRecentPlanSuccessful();
        REQUIRE(recentSuccess == true);  // No failures recorded yet
    }
}

} // namespace nlm
