// Cognition system integration tester
// 
// Comprehensive tests for cognition system integration across the NLM architecture.
// Tests the integration of concept formation, attention selection, planning, and self-model
// systems with episodic memory, prediction systems, and action selection.

#pragma once

#include "IntegrationTest.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include <gtest/gtest.h>

namespace nlm {
namespace test {

// Cognition system integration tester
class CognitionIntegrationTester : public IntegrationTest {
protected:
    CognitionIntegrationTester() = default;
    virtual ~CognitionIntegrationTester() = default;
    
    // Test setup
    void SetUp() override;
    void TearDown() override;
    
    // Cognition system access helpers
    ConceptFormation* getConceptFormationSystem(Brain* brain) const;
    NeuralPlanner* getPlannerSystem(Brain* brain) const;
    AttentionalSelection* getAttentionSystem(Brain* brain) const;
    SelfModel* getSelfModelSystem(Brain* brain) const;
    SocialLearning* getSocialLearningSystem(Brain* brain) const;
    
    // Integration test cases
    void testConceptFormationWithEpisodicMemory();
    void testAttentionSelectionWithPredictionSystems();
    void testPlanningWithActionSelection();
    void testSelfModelWithSocialLearning();
    void testCognitionWithNeuromodulation();
    void testCognitionWithMemorySystems();
    void testCognitionWithPrediction();
    void testCognitionWithDevelopment();
    
    // Test utilities
    void verifyConceptFormationIntegrity(ConceptFormation* conceptFormation);
    void verifyPlanningIntegrity(NeuralPlanner* planner);
    float calculateCognitivePerformance(ConceptFormation* conceptFormation, NeuralPlanner* planner);
}; // class CognitionIntegrationTester

} // namespace test
} // namespace nlm