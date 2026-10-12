// Neuromodulation system integration tester
// 
// Comprehensive tests for neuromodulation system integration across the NLM architecture.
// Tests the integration of dopamine, curiosity, novelty, and prediction error systems
// with memory, prediction, and action selection systems.

#pragma once

#include "IntegrationTest.hpp"
#include "../neuromodulation/Reward.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../development/DevelopmentSystem.hpp"
#include <gtest/gtest.h>

namespace nlm {
namespace test {

// Neuromodulation system integration tester
class NeuromodulationIntegrationTester : public IntegrationTest {
protected:
    NeuromodulationIntegrationTester() = default;
    virtual ~NeuromodulationIntegrationTester() = default;
    
    // Test setup
    void SetUp() override;
    void TearDown() override;
    
    // Neuromodulation system access helpers
    Reward* getRewardSystem(Brain* brain) const;
    PredictionError* getPredictionErrorSystem(Brain* brain) const;
    Curiosity* getCuriositySystem(Brain* brain) const;
    Novelty* getNoveltySystem(Brain* brain) const;
    Dopamine* getDopamineSystem(Brain* brain) const;
    
    // Integration test cases
    void testDopamineIntegrationWithWorkingMemory();
    void testCuriosityIntegrationWithEpisodicMemory();
    void testNoveltyIntegrationWithPredictionSystems();
    void testPredictionErrorIntegrationWithActionSelection();
    void testNeuromodulationWithMemoryConsolidation();
    void testNeuromodulationWithPredictionLearning();
    void testNeuromodulationWithDevelopment();
    void testNeuromodulationWithCognition();
    
    // Test utilities
    void verifyNeuromodulationState(Reward* reward, PredictionError* predError,
                                   Curiosity* curiosity, Novelty* novelty);
    float calculateNeuromodulationImpact(Reward* reward, PredictionError* predError);
}; // class NeuromodulationIntegrationTester

} // namespace test
} // namespace nlm