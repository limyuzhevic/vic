// Cognition Tests
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include <catch2/catch_test_macros.hpp>

namespace test_cognition {

TEST_CASE("NeuralPlanner creation and initialization", "[cognition][planner][creation]") {
    nlm::NeuralPlanner planner;
    
    REQUIRE(planner.getPlanCount() == 0);
    REQUIRE(planner.getActionSequence().empty());
}

TEST_CASE("NeuralPlanner action planning", "[cognition][planner][planning]") {
    nlm::NeuralPlanner planner;
    
    std::vector<nlm::Action> actions;
    actions.push_back(nlm::Action{nlm::ActionId(1), "move", "forward", 1.0f});
    actions.push_back(nlm::Action{nlm::ActionId(2), "turn", "left", 0.5f});
    
    planner.setAvailableActions(actions);
    
    auto plan = planner.planAction();
    REQUIRE(plan != nullptr);
    REQUIRE(plan->action_type != nullptr);
}

TEST_CASE("NeuralPlanner plan evaluation", "[cognition][planner][evaluation]") {
    nlm::NeuralPlanner planner;
    
    std::vector<nlm::Action> actions;
    actions.push_back(nlm::Action{nlm::ActionId(3), "action1", "description1", 1.0f});
    
    planner.setAvailableActions(actions);
    
    auto plan = planner.planAction();
    float score = planner.evaluatePlan(plan.get());
    
    REQUIRE(score >= 0.0f);
}

TEST_CASE("ConceptFormation creation and learning", "[cognition][concept][creation]") {
    nlm::ConceptFormation conceptFormation;
    
    REQUIRE(conceptFormation.getConceptCount() == 0);
    REQUIRE(conceptFormation.getSimilarityThreshold() > 0.0f);
}

TEST_CASE("ConceptFormation pattern learning", "[cognition][concept][learning]") {
    nlm::ConceptFormation conceptFormation;
    
    nlm::Pattern pattern;
    pattern.features = {0.1f, 0.2f, 0.3f};
    pattern.labels = {"label1", "label2"};
    
    conceptFormation.learnPattern(pattern);
    REQUIRE(conceptFormation.getConceptCount() > 0);
}

TEST_CASE("ConceptFormation pattern matching", "[cognition][concept][matching]") {
    nlm::ConceptFormation conceptFormation;
    
    nlm::Pattern pattern;
    pattern.features = {0.1f, 0.2f, 0.3f};
    pattern.labels = {"test_label"};
    
    conceptFormation.learnPattern(pattern);
    
    nlm::Pattern queryPattern;
    queryPattern.features = {0.15f, 0.25f, 0.35f};
    
    auto result = conceptFormation.matchPattern(queryPattern);
    REQUIRE(result.has_value());
}

TEST_CASE("NeuralPlanner with concept formation integration", "[cognition][integration][planner]") {
    nlm::NeuralPlanner planner;
    nlm::ConceptFormation conceptFormation;
    
    std::vector<nlm::Action> actions;
    actions.push_back(nlm::Action{nlm::ActionId(4), "explore", "search environment", 1.5f});
    
    planner.setAvailableActions(actions);
    
    nlm::Pattern pattern;
    pattern.features = {0.5f, 0.6f, 0.7f};
    pattern.labels = {"exploration"};
    
    conceptFormation.learnPattern(pattern);
    
    auto plan = planner.planAction();
    REQUIRE(plan != nullptr);
}

TEST_CASE("Cognition system performance evaluation", "[cognition][performance][evaluation]") {
    nlm::NeuralPlanner planner;
    nlm::ConceptFormation conceptFormation;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Simulate planning and concept formation
    std::vector<nlm::Action> actions;
    actions.push_back(nlm::Action{nlm::ActionId(5), "process", "data", 2.0f});
    
    planner.setAvailableActions(actions);
    
    nlm::Pattern pattern;
    pattern.features = {0.8f, 0.9f, 1.0f};
    pattern.labels = {"processing"};
    
    conceptFormation.learnPattern(pattern);
    
    auto plan = planner.planAction();
    conceptFormation.matchPattern(pattern);
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    // Should complete within reasonable time
    REQUIRE(duration.count() < 1000000);  // Less than 1 second
}

TEST_CASE("Cognitive state management", "[cognition][state][management]") {
    nlm::NeuralPlanner planner;
    
    // Set up initial state
    std::vector<nlm::Action> actions;
    actions.push_back(nlm::Action{nlm::ActionId(6), "initial_action", "start", 1.0f});
    
    planner.setAvailableActions(actions);
    
    // Get current state
    auto plan = planner.planAction();
    REQUIRE(plan != nullptr);
    
    // Verify planner state is consistent
    auto plannedActions = planner.getPlannedActions();
    REQUIRE(!plannedActions.empty());
}

} // namespace test_cognition
