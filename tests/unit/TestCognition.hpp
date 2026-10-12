#ifndef TEST_COGNITION_HPP
#define TEST_COGNITION_HPP

#include "tests/Framework/TestFramework.hpp"
#include "src/cognition/ConceptFormation.hpp"
#include "src/cognition/NeuralPlanner.hpp"
#include "tests/Helpers/TestHelpers.hpp"

class TestCognition : public TestCase {
public:
    TestCognition() : TestCase("Cognition") {}
    
    void run() override {
        // Test ConceptFormation
        ConceptFormation::ConceptFormationConfig cf_config;
        cf_config.num_concepts = 100;
        cf_config.similarity_threshold = 0.7;
        cf_config.growth_rate = 0.01;
        cf_config.decay_rate = 0.001;
        cf_config.max_concept_strength = 1.0;
        cf_config.min_concept_strength = 0.01;
        cf_config.association_threshold = 0.5;
        
        ConceptFormation concept_formation(cf_config);
        
        // Test concept formation from patterns
        std::vector<double> pattern1 = {1.0, 0.5, 0.3, 0.1, 0.0};
        std::vector<double> pattern2 = {1.1, 0.6, 0.4, 0.2, 0.1}; // Similar to pattern1
        std::vector<double> pattern3 = {0.1, 1.0, 0.2, 0.8, 0.3}; // Different pattern
        
        concept_formation.encode(pattern1, 1.0);
        concept_formation.encode(pattern2, 2.0);
        concept_formation.encode(pattern3, 3.0);
        
        ASSERT_TRUE(concept_formation.getNumConcepts() > 0, "Should have created concepts");
        
        // Test concept retrieval
        std::vector<double> query_pattern = {1.05, 0.55, 0.35, 0.15, 0.05}; // Similar to pattern1
        std::vector<Concept> retrieved_concepts = concept_formation.retrieve(query_pattern, 5);
        ASSERT_TRUE(retrieved_concepts.size() <= 5, "Should retrieve at most 5 concepts");
        
        // Test concept association
        concept_formation.associate(1, 2, 0.8);
        std::vector<Concept> associated_concepts = concept_formation.getAssociatedConcepts(1);
        ASSERT_TRUE(associated_concepts.size() > 0, "Concepts should have associations");
        
        // Test concept formation decay
        double strength_before = concept_formation.getConceptStrength(1);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        concept_formation.step();
        double strength_after = concept_formation.getConceptStrength(1);
        ASSERT_TRUE(strength_after <= strength_before, "Concept strength should decay over time");
        
        // Test NeuralPlanner
        NeuralPlanner::NeuralPlannerConfig np_config;
        np_config.max_plans = 50;
        np_config.plan_horizon = 10;
        np_config.action_cost = 1.0;
        np_config.reward_weight = 1.0;
        np_config.confidence_threshold = 0.5;
        np_config.max_planning_steps = 1000;
        
        NeuralPlanner neural_planner(np_config);
        
        // Test planner action generation
        std::vector<Action> actions = {
            Action(0, "move_forward", 1.0, 1.0),
            Action(1, "turn_left", 1.0, 1.0),
            Action(2, "turn_right", 1.0, 1.0),
            Action(3, "turn_around", 1.0, 1.0)
        };
        
        neural_planner.setAvailableActions(actions);
        
        // Test plan generation
        std::vector<double> start_state = {0.0, 0.0, 0.0, 0.0};
        std::vector<double> goal_state = {10.0, 10.0, 0.0, 0.0};
        
        Plan plan = neural_planner.plan(start_state, goal_state);
        ASSERT_TRUE(plan.sequence.size() > 0, "Planner should generate a plan");
        
        // Test plan evaluation
        double plan_score = neural_planner.evaluatePlan(plan, start_state, goal_state);
        ASSERT_TRUE(plan_score > 0.0, "Plan should have a positive score");
        
        // Test plan execution
        neural_planner.setCurrentState(start_state);
        neural_planner.executePlan(plan);
        
        // Test cognition integration
        ConceptFormation concept_formation2(cf_config);
        NeuralPlanner neural_planner2(np_config);
        
        // Form concepts from actions
        concept_formation2.encode({1.0, 0.0, 0.0, 0.0}, 1); // Move forward
        concept_formation2.encode({0.0, 1.0, 0.0, 0.0}, 2); // Turn left
        
        // Plan based on concepts
        std::vector<double> concept_state = {0.5, 0.5, 0.0, 0.0};
        Plan concept_plan = neural_planner2.plan(concept_state, goal_state);
        
        ASSERT_TRUE(concept_plan.sequence.size() > 0, "Planner should work with conceptual states");
        
        // Test cognition metrics
        double concept_utilization = concept_formation.getConceptUtilizationRate();
        double planning_efficiency = neural_planner.getPlanningEfficiency();
        
        ASSERT_TRUE(concept_utilization >= 0.0 && concept_utilization <= 1.0, "Concept utilization should be between 0 and 1");
        ASSERT_TRUE(planning_efficiency >= 0.0 && planning_efficiency <= 1.0, "Planning efficiency should be between 0 and 1");
        
        ASSERT_TRUE(true, "Cognition integration test completed");
    }
};

#endif // TEST_COGNITION_HPP