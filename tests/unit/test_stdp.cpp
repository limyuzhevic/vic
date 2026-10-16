// STDP (Spike-Timing-Dependent Plasticity) Tests
// Comprehensive unit tests for STDP plastic mechanisms
// Tests synaptic weight updates based on spike timing

#include "fixtures/TestHelpers.hpp"
#include "plasticity/STDP.hpp"
#include "brain/Neuron.hpp"
#include <gtest/gtest.h>
#include <vector>
#include <cmath>

namespace test {

// Test 6.1: STDP Initialization
TEST_F(NLMTest, STDPInitialization) {
    nlm::STDP stdp;
    
    EXPECT_DOUBLE_EQ(stdp.getLTPThreshold(), 20.0);   // Default LTP threshold
    EXPECT_DOUBLE_EQ(stdp.getLTDThreshold(), 20.0);  // Default LTD threshold
    EXPECT_DOUBLE_EQ(stdp.getLTPRate(), 0.01);       // Default LTP rate
    EXPECT_DOUBLE_EQ(stdp.getLTDRate(), 0.01);       // Default LTD rate
    EXPECT_DOUBLE_EQ(stdp.getLearningRate(), 0.1);   // Default learning rate
    EXPECT_TRUE(stdp.isEnabled());                   // Default enabled
}

// Test 6.2: STDP Configuration
TEST_F(NLMTest, STDPConfiguration) {
    nlm::STDP stdp;
    
    stdp.setLTPThreshold(30.0);
    EXPECT_DOUBLE_EQ(stdp.getLTPThreshold(), 30.0);
    
    stdp.setLTDThreshold(10.0);
    EXPECT_DOUBLE_EQ(stdp.getLTDThreshold(), 10.0);
    
    stdp.setLTPRate(0.05);
    EXPECT_DOUBLE_EQ(stdp.getLTPRate(), 0.05);
    
    stdp.setLTDRate(0.02);
    EXPECT_DOUBLE_EQ(stdp.getLTDRate(), 0.02);
    
    stdp.setLearningRate(0.2);
    EXPECT_DOUBLE_EQ(stdp.getLearningRate(), 0.2);
}

// Test 6.3: STDP Enable/Disable
TEST_F(NLMTest, STDPEnableDisable) {
    nlm::STDP stdp;
    
    EXPECT_TRUE(stdp.isEnabled());
    
    stdp.setEnabled(false);
    EXPECT_FALSE(stdp.isEnabled());
    
    stdp.setEnabled(true);
    EXPECT_TRUE(stdp.isEnabled());
}

// Test 6.4: STDP Plasticity Window
TEST_F(NLMTest, STDPPlasticityWindow) {
    nlm::STDP stdp;
    
    // Test LTP window (pre before post)
    double dt = 10.0;  // Positive time difference
    double weightChange = stdp.calculateWeightChange(dt, true);
    EXPECT_GT(weightChange, 0.0);  // Should increase weight
    
    // Test LTD window (post before pre)
    dt = -10.0;  // Negative time difference
    weightChange = stdp.calculateWeightChange(dt, true);
    EXPECT_LT(weightChange, 0.0);  // Should decrease weight
    
    // Test zero time difference
    dt = 0.0;
    weightChange = stdp.calculateWeightChange(dt, true);
    EXPECT_DOUBLE_EQ(weightChange, 0.0);  // No change
}

// Test 6.5: STDP Weight Change Calculation
TEST_F(NLMTest, STDPWeightChangeCalculation) {
    nlm::STDP stdp;
    
    // Test various time differences
    std::vector<double> timeDiffs = {30.0, 20.0, 10.0, 5.0, 2.0, 0.0, -2.0, -5.0, -10.0, -20.0, -30.0};
    
    for (double dt : timeDiffs) {
        double weightChange = stdp.calculateWeightChange(dt, true);
        
        // Weight change should be in reasonable bounds
        EXPECT_GE(weightChange, -1.0);  // No more than 100% decrease
        EXPECT_LE(weightChange, 1.0);  // No more than 100% increase
    }
}

// Test 6.6: STDP Function Integration
TEST_F(NLMTest, STDPFunctionIntegration) {
    nlm::STDP stdp;
    
    double initialWeight = 0.5;
    double dt = 15.0;  // Within LTP window
    
    double newWeight = stdp.updateWeight(initialWeight, dt, true);
    
    EXPECT_GT(newWeight, initialWeight);  // Weight should increase
    EXPECT_LE(newWeight, 1.0);  // Should not exceed maximum
}

// Test 6.7: STDP with Different Neuron Types
TEST_F(NLMTest, STDPWithDifferentNeuronTypes) {
    nlm::STDP stdp;
    
    double initialWeight = 0.3;
    double dt = 25.0;
    
    // Test with pre-synaptic neuron
    double weight1 = stdp.updateWeight(initialWeight, dt, true);
    
    // Test with post-synaptic neuron
    double weight2 = stdp.updateWeight(initialWeight, dt, false);
    
    // Both should be similar for same dt
    EXPECT_DOUBLE_EQ(weight1, weight2);
}

// Test 6.8: STDP Accumulator Management
TEST_F(NLMTest, STDPAccumulatorManagement) {
    nlm::STDP stdp;
    
    double initialLTP = stdp.getLTPAccumulator();
    double initialLTD = stdp.getLTDAccumulator();
    
    // Simulate STDP events
    stdp.addLTPEvent(10.0);
    stdp.addLTDEvent(15.0);
    
    EXPECT_GT(stdp.getLTPAccumulator(), initialLTP);
    EXPECT_GT(stdp.getLTDAccumulator(), initialLTD);
    
    // Reset accumulators
    stdp.resetAccumulators();
    EXPECT_DOUBLE_EQ(stdp.getLTPAccumulator(), 0.0);
    EXPECT_DOUBLE_EQ(stdp.getLTDAccumulator(), 0.0);
}

// Test 6.9: STDP Learning Rate Scaling
TEST_F(NLMTest, STDPLearningRateScaling) {
    nlm::STDP stdp;
    
    double baseWeightChange = stdp.calculateWeightChange(20.0, true);
    
    stdp.setLearningRate(0.5);  // Double learning rate
    double scaledWeightChange = stdp.calculateWeightChange(20.0, true);
    
    EXPECT_DOUBLE_EQ(scaledWeightChange, baseWeightChange * 2.0);
}

// Test 6.10: STDP with Negative Weights
TEST_F(NLMTest, STDPWithNegativeWeights) {
    nlm::STDP stdp;
    
    double initialWeight = -0.3;  // Negative initial weight
    double dt = 15.0;
    
    double newWeight = stdp.updateWeight(initialWeight, dt, true);
    
    // Weight should become less negative (increase)
    EXPECT_GT(newWeight, initialWeight);
}

// Test 6.11: STDP Boundary Conditions
TEST_F(NLMTest, STDPBoundaryConditions) {
    nlm::STDP stdp;
    
    // Test extreme time differences
    double weightChange = stdp.calculateWeightChange(1000.0, true);  // Very large positive dt
    EXPECT_GT(weightChange, 0.0);
    
    weightChange = stdp.calculateWeightChange(-1000.0, true);  // Very large negative dt
    EXPECT_LT(weightChange, 0.0);
    
    // Test very small time differences
    weightChange = stdp.calculateWeightChange(0.001, true);
    EXPECT_NEAR(weightChange, 0.0, 1e-10);
}

// Test 6.12: STDP Error Handling
TEST_F(NLMTest, STDPErrorHandling) {
    nlm::STDP stdp;
    
    // STDP should handle invalid inputs gracefully
    double weightChange = stdp.calculateWeightChange(NAN, true);
    EXPECT_TRUE(std::isnan(weightChange) || weightChange == 0.0);
    
    weightChange = stdp.calculateWeightChange(INFINITY, true);
    EXPECT_TRUE(std::isinf(weightChange) || weightChange > 0.0);
}

// Parameterized test for STDP window variations
class STDPWindowTest : public NLMTest, public ::testing::WithParamInterface<double> {
};

TEST_P(STDPWindowTest, STDPWindowVariations) {
    double dt = GetParam();
    nlm::STDP stdp;
    
    double weightChange = stdp.calculateWeightChange(dt, true);
    
    // Weight change should be reasonable for any dt
    EXPECT_GE(weightChange, -1.0);
    EXPECT_LE(weightChange, 1.0);
}

INSTANTIATE_TEST_SUITE_P(
    STDPWindowVariations,
    STDPWindowTest,
    ::testing::Values(-50.0, -30.0, -20.0, -10.0, -5.0, -2.0, -1.0, -0.5, 0.0, 0.5, 1.0, 2.0, 5.0, 10.0, 20.0, 30.0, 50.0)
);

// Test STDP with different configurations
TEST_F(NLMTest, STDPConfigVariations) {
    // Test with different STDP parameters
    struct STDPConfig {
        double ltpThreshold;
        double ltdThreshold;
        double ltpRate;
        double ltdRate;
        double learningRate;
    };
    
    std::vector<STDPConfig> configs = {
        {20.0, 20.0, 0.01, 0.01, 0.1},
        {30.0, 10.0, 0.05, 0.02, 0.2},
        {15.0, 15.0, 0.005, 0.005, 0.05},
        {25.0, 25.0, 0.02, 0.02, 0.15}
    };
    
    for (const auto& config : configs) {
        nlm::STDP stdp;
        stdp.setLTPThreshold(config.ltpThreshold);
        stdp.setLTDThreshold(config.ltdThreshold);
        stdp.setLTPRate(config.ltpRate);
        stdp.setLTDRate(config.ltdRate);
        stdp.setLearningRate(config.learningRate);
        
        // Verify configuration was applied
        EXPECT_DOUBLE_EQ(stdp.getLTPThreshold(), config.ltpThreshold);
        EXPECT_DOUBLE_EQ(stdp.getLTDThreshold(), config.ltdThreshold);
        EXPECT_DOUBLE_EQ(stdp.getLTPRate(), config.ltpRate);
        EXPECT_DOUBLE_EQ(stdp.getLTDRate(), config.ltdRate);
        EXPECT_DOUBLE_EQ(stdp.getLearningRate(), config.learningRate);
    }
}

} // namespace test