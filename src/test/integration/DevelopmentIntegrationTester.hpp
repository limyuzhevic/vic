// Development system integration tester
// 
// Comprehensive tests for development system integration across the NLM architecture.
// Tests the integration of developmental plasticity, critical periods, structural plasticity,
// and adaptive development across all brain systems.

#pragma once

#include "IntegrationTest.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../plasticity/PlasticityRule.hpp"
#include <gtest/gtest.h>

namespace nlm {
namespace test {

// Development system integration tester
class DevelopmentIntegrationTester : public IntegrationTest {
protected:
    DevelopmentIntegrationTester() = default;
    virtual ~DevelopmentIntegrationTester() = default;
    
    // Test setup
    void SetUp() override;
    void TearDown() override;
    
    // Development system access helpers
    DevelopmentSystem* getDevelopmentSystem(Brain* brain) const;
    Neuromodulator* getNeuromodulatorSystem(Brain* brain) const;
    
    // Integration test cases
    void testPlasticityMaturationWithMemorySystems();
    void testCriticalPeriodEffectsWithLearning();
    void testStructuralPlasticityWithNeuralProcessing();
    void testAdaptiveDevelopmentAcrossSystems();
    void testDevelopmentWithNeuromodulation();
    void testDevelopmentWithPrediction();
    void testDevelopmentWithCognition();
    void testDevelopmentWithActionSelection();
    
    // Test utilities
    void verifyDevelopmentIntegrity(DevelopmentSystem* developmentSystem);
    float calculatePlasticityImpact(DevelopmentSystem* developmentSystem);
}; // class DevelopmentIntegrationTester

} // namespace test
} // namespace nlm