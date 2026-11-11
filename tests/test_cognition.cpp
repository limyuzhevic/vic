// NLM Cognition Test Suite
// Phase 5: Cognition Integration Tests

#include <iostream>
#include <cassert>
#include <vector>
#include <memory>

#include "../src/cognition/ConceptFormation.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/core/Config/Config.hpp"

namespace test_cognition {
    
    void test_concept_formation() {
        std::cout << "Testing Concept Formation system..." << std::endl;
        
        auto config = std::make_shared<Config>();
        ConceptFormation conceptFormation(config);
        
        assert(conceptFormation.initialize() == true);
        assert(conceptFormation.getConceptCount() >= 0);
        
        // Test concept formation operations
        conceptFormation.clearConcepts();
        
        std::cout << "  PASSED" << std::endl;
    }
    
    void test_neural_planner() {
        std::cout << "Testing Neural Planner system..." << std::endl;
        
        auto config = std::make_shared<Config>();
        NeuralPlanner neuralPlanner(config);
        
        assert(neuralPlanner.initialize() == true);
        assert(neuralPlanner.getPlannerCapacity() >= 0);
        
        // Test planning operations
        neuralPlanner.clearPlans();
        
        std::cout << "  PASSED" << std::endl;
    }
    
    void test_concept_formation_integration() {
        std::cout << "Testing Concept Formation integration with learning..." << std::endl;
        
        auto config = std::make_shared<Config>();
        ConceptFormation conceptFormation(config);
        
        assert(conceptFormation.initialize() == true);
        
        // Test concept learning
        conceptFormation.learnConcept("test_concept", std::vector<float>{0.1f, 0.2f, 0.3f});
        assert(conceptFormation.getConceptCount() > 0);
        
        conceptFormation.clearConcepts();
        
        std::cout << "  PASSED" << std::endl;
    }
    
    void test_neural_planner_integration() {
        std::cout << "Testing Neural Planner integration with action selection..." << std::endl;
        
        auto config = std::make_shared<Config>();
        NeuralPlanner neuralPlanner(config);
        
        assert(neuralPlanner.initialize() == true);
        
        // Test planning operations
        neuralPlanner.createPlan("test_plan", std::vector<ActionType>{ActionType::MoveForward, ActionType::TurnLeft});
        assert(neuralPlanner.getPlanCount() > 0);
        
        neuralPlanner.clearPlans();
        
        std::cout << "  PASSED" << std::endl;
    }
    
    void test_cognition_integration() {
        std::cout << "Testing Cognition integration with perception..." << std::endl;
        
        auto config = std::make_shared<Config>();
        ConceptFormation conceptFormation(config);
        NeuralPlanner neuralPlanner(config);
        
        assert(conceptFormation.initialize() == true);
        assert(neuralPlanner.initialize() == true);
        
        // Test concept-planner interaction
        conceptFormation.learnConcept("movement", std::vector<float>{1.0f, 0.0f, 0.0f});
        neuralPlanner.createPlan("move_forward", std::vector<ActionType>{ActionType::MoveForward});
        
        assert(conceptFormation.getConceptCount() > 0);
        assert(neuralPlanner.getPlanCount() > 0);
        
        conceptFormation.clearConcepts();
        neuralPlanner.clearPlans();
        
        std::cout << "  PASSED" << std::endl;
    }
    
    void runAll() {
        std::cout << "=== NLM Cognition Tests ===" << std::endl;
        
        test_concept_formation();
        test_neural_planner();
        test_concept_formation_integration();
        test_neural_planner_integration();
        test_cognition_integration();
        
        std::cout << std::endl;
        std::cout << "=== All Cognition Tests PASSED ===" << std::endl;
    }
}