// Prediction system integration tester
// 
// Comprehensive tests for prediction system integration across the NLM architecture.
// Tests the integration of forward prediction, value prediction, behavioral prediction,
// and meta-prediction systems with sensory processing, neuromodulation, and motor systems.

#pragma once

#include "IntegrationTest.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../prediction/NeuralPrediction.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../neuromodulation/Reward.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../motor/MotorSystem.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include <gtest/gtest.h>

namespace nlm {
namespace test {

// Prediction system integration tester
class PredictionIntegrationTester : public IntegrationTest {
protected:
    PredictionIntegrationTester() = default;
    virtual ~PredictionIntegrationTester() = default;
    
    // Test setup
    void SetUp() override;
    void TearDown() override;
    
    // Prediction system access helpers
    PredictionSystem* getPredictionSystem(Brain* brain) const;
    NeuralPrediction* getNeuralPredictionSystem(Brain* brain) const;
    
    // Integration test cases
    void testForwardPredictionWithSensoryProcessing();
    void testValuePredictionWithRewardSystems();
    void testBehavioralPredictionWithMotorSystems();
    void testMetaPredictionWithCognitiveSystems();
    void testPredictionWithNeuromodulation();
    void testPredictionWithMemorySystems();
    void testPredictionWithDevelopment();
    
    // Test utilities
    void verifyPredictionAccuracy(PredictionSystem* predictionSystem);
    float calculatePredictionConfidence(PredictionSystem* predictionSystem);
}; // class PredictionIntegrationTester

} // namespace test
} // namespace nlm