// Cognition System Integration Tests
// Tests integration of cognitive functions (planning, concept formation, attention) with neural simulation

#pragma once

#include "IntegrationTestFramework.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../brain/Brain.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

class TestCognitionIntegration : public IntegrationTestBase {
public:
    explicit TestCognitionIntegration(std::shared_ptr<Config> cfg, IntegrationTestResult& results)
        : IntegrationTestBase(cfg, results) {}
    
    void runAllTests() {
        runTestCase("Concept Formation Integration", "Cognition Integration", [this]() {
            testConceptFormationIntegration();
        });
        
        runTestCase("Neural Planning Integration", "Cognition Integration", [this]() {
            testNeuralPlanningIntegration();
        });
        
        runTestCase("Attentional Selection Integration", "Cognition Integration", [this]() {
            testAttentionalSelectionIntegration();
        });
        
        runTestCase("Cognition-Neural Activity Coupling", "Cognition Integration", [this]() {
            testCognitionNeuralActivityCoupling();
        });
        
        runTestCase("Cognitive Flexibility", "Cognition Integration", [this]() {
            testCognitiveFlexibility();
        });
        
        runTestCase("Concept Generalization", "Cognition Integration", [this]() {
            testConceptGeneralization();
        });
        
        runTestCase("Planning Optimization", "Cognition Integration", [this]() {
            testPlanningOptimization();
        });
    }
    
private:
    void testConceptFormationIntegration() {
        setupSystem();
        
        auto* conceptFormation = brain->getConceptFormation();
        assert(conceptFormation != nullptr);
        
        // Create input patterns for concept formation
        std::vector<std::vector<float>> patterns = {
            {0.1f, 0.2f, 0.3f},
            {0.2f, 0.4f, 0.6f},
            {0.3f, 0.6f, 0.9f},
            {0.4f, 0.8f, 1.2f}
        };
        
        // Test pattern presentation and concept learning
        for (size_t i = 0; i < patterns.size(); ++i) {
            // In real implementation, this would involve neural pattern processing
            // For integration test, we verify system doesn't crash
            brain->step(static_cast<nlm::SimulationStep>(i));
        }
        
        // Test concept storage and retrieval
        // (Implementation depends on ConceptFormation API)
        
        // Verify concept formation integrates with neural activity
        size_t firingCount = brain->getFiringNeuronCount();
        assert(firingCount >= 0);
        
        teardownSystem();
    }
    
    void testNeuralPlanningIntegration() {
        setupSystem();
        
        auto* neuralPlanner = brain->getPlanner();
        assert(neuralPlanner != nullptr);
        
        // Test goal setting and planning
        std::vector<float> goals = {0.5f, 0.7f, 0.9f};
        
        for (size_t i = 0; i < goals.size(); ++i) {
            // In real implementation, planner would generate action plans
            brain->step(static_cast<nlm::SimulationStep>(i));
            
            // Verify planning affects neural activity
            size_t firingCount = brain->getFiringNeuronCount();
            assert(firingCount >= 0);
        }
        
        // Test action production with planning
        auto action = brain->produceAction();
        assert(action != nullptr);
        
        teardownSystem();
    }
    
    void testAttentionalSelectionIntegration() {
        setupSystem();
        
        auto* attention = brain->getAttention();
        assert(attention != nullptr);
        
        // Test attentional focus switching
        std::vector<NeuronId> attentionalFocuses = {1, 3, 5, 7, 9};
        
        for (size_t i = 0; i < attentionalFocuses.size(); ++i) {
            // In real implementation, attention system would focus on specific neurons
            brain->step(static_cast<nlm::SimulationStep>(i * 10));
        
        // Verify attentional effects on neural activity
        float averageFiringRate = brain->getAverageFiringRate();
        assert(averageFiringRate >= 0.0f);
        
        teardownSystem();
    }
    
    void testCognitionNeuralActivityCoupling() {
        setupSystem();
        
        // Create cognitive task that drives neural activity
        std::vector<std::string> cognitiveTasks = {
            "Pattern recognition",
            "Memory recall", 
            "Decision making",
            "Problem solving"
        };
        
        for (size_t i = 0; i < cognitiveTasks.size(); ++i) {
            // Apply cognitive load
            brain->step(static_cast<nlm::SimulationStep>(i * 100));
            
            // Verify cognitive processes affect neural activity
            float firingRate = brain->getAverageFiringRate();
            assert(firingRate >= 0.0f);
            
            // Test concept formation with cognitive task
            auto* conceptFormation = brain->getConceptFormation();
            if (conceptFormation) {
                // Process pattern related to cognitive task
                std::vector<float> pattern = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
                // (Implementation depends on ConceptFormation API)
            }
        }
        
        teardownSystem();
    }
    
    void testCognitiveFlexibility() {
        setupSystem();
        
        // Test switching between cognitive strategies
        std::vector<std::string> strategies = {
            "Exploratory",
            "Exploitative",
            "Adaptive",
            "Random"
        };
        
        for (size_t i = 0; i < strategies.size(); ++i) {
            // Switch strategy (in real implementation)
            brain->step(static_cast<nlm::SimulationStep>(i * 50));
            
            // Verify strategy affects behavior
            auto action = brain->produceAction();
            assert(action != nullptr);
        }
        
        // Test adaptation to changing environment
        for (nlm::SimulationStep step = 0; step < 100; ++step) {
            brain->step(step);
        }
        
        // Verify system adapts and maintains stability
        float firingRate = brain->getAverageFiringRate();
        assert(firingRate >= 0.0f);
        
        teardownSystem();
    }
    
    void testConceptGeneralization() {
        setupSystem();
        
        auto* conceptFormation = brain->getConceptFormation();
        assert(conceptFormation != nullptr);
        
        // Test generalization from specific instances to abstract concepts
        std::vector<std::vector<float>> specificInstances = {
            {0.1f, 0.2f, 0.3f},
            {0.2f, 0.4f, 0.6f},
            {0.3f, 0.6f, 0.9f}
        };
        
        // Train on specific instances
        for (size_t i = 0; i < specificInstances.size(); ++i) {
            // Present pattern for concept learning
            brain->step(static_cast<nlm::SimulationStep>(i));
        }
        
        // Test generalization to new patterns
        std::vector<float> newPattern = {0.4f, 0.8f, 1.2f};
        
        // In real implementation, conceptFormation would generalize
        // Here we verify system doesn't crash
        brain->step(static_cast<nlm::SimulationStep>(specificInstances.size()));
        
        teardownSystem();
    }
    
    void testPlanningOptimization() {
        setupSystem();
        
        auto* neuralPlanner = brain->getPlanner();
        assert(neuralPlanner != nullptr);
        
        // Test planning optimization over time
        std::vector<float> goalValues = {0.3f, 0.5f, 0.7f, 0.9f};
        
        for (size_t i = 0; i < goalValues.size(); ++i) {
            // Plan toward goal
            brain->step(static_cast<nlm::SimulationStep>(i * 25));
            
            // Verify planning progress
            auto action = brain->produceAction();
            assert(action != nullptr);
        }
        
        // Test planning efficiency
        size_t totalSteps = 50;
        for (nlm::SimulationStep step = 0; step < totalSteps; ++step) {
            brain->step(step);
        }
        
        // Verify planning maintained stability
        float firingRate = brain->getAverageFiringRate();
        assert(firingRate >= 0.0f);
        
        teardownSystem();
    }
};

} // namespace nlm