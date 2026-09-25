// Plasticity Unit Tests
// Comprehensive test suite for plasticity rules (STDP, Hebbian) using Google Test framework

#include "tests/common/test_base.hpp"
#include <gtest/gtest.h>
#include <cmath>
#include <vector>
#include <algorithm>

namespace nlm {
namespace test {

// Test fixture for plasticity tests
class PlasticityTest : public TestBase {
protected:
    void SetUp() override {
        TestBase::SetUp();
        
        // Create test synapses for plasticity testing
        synapse1 = createTestSynapse(SynapseId(1), NeuronId(1), NeuronId(2));
        synapse2 = createTestSynapse(SynapseId(2), NeuronId(3), NeuronId(4));
        synapse3 = createTestSynapse(SynapseId(3), NeuronId(5), NeuronId(6));
    }
    
    void TearDown() override {
        TestBase::TearDown();
    }
    
protected:
    nlm::Synapse synapse1;
    nlm::Synapse synapse2;
    nlm::Synapse synapse3;
};

// Test STDP configuration
TEST_F(PlasticityTest, STDPConfiguration) {
    nlm::STDP stdp;
    
    EXPECT_FLOAT_EQ(stdp.getLTPWeight(), 0.01f);
    EXPECT_FLOAT_EQ(stdp.getLTDWeight(), 0.012f);
    EXPECT_FLOAT_EQ(stdp.getTimeConstant(), 20.0f);
}

// Test STDP parameter configuration
TEST_F(PlasticityTest, STDPParameterSetting) {
    nlm::STDP stdp;
    
    stdp.setLTPWeight(0.05f);
    stdp.setLTDWeight(0.06f);
    stdp.setTimeConstant(30.0f);
    
    EXPECT_FLOAT_EQ(stdp.getLTPWeight(), 0.05f);
    EXPECT_FLOAT_EQ(stdp.getLTDWeight(), 0.06f);
    EXPECT_FLOAT_EQ(stdp.getTimeConstant(), 30.0f);
}

// Test STDP configure method
TEST_F(PlasticityTest, STDPConfigure) {
    nlm::STDP stdp;
    
    stdp.configure(0.03f, 0.035f, 25.0f);
    
    EXPECT_FLOAT_EQ(stdp.getLTPWeight(), 0.03f);
    EXPECT_FLOAT_EQ(stdp.getLTDWeight(), 0.035f);
    EXPECT_FLOAT_EQ(stdp.getTimeConstant(), 25.0f);
}

// Test STDP name
TEST_F(PlasticityTest, STDPName) {
    nlm::STDP stdp;
    
    EXPECT_NE(stdp.getName(), nullptr);
    EXPECT_EQ(std::string(stdp.getName()), "STDP");
}

// Test Hebbian configuration
TEST_F(PlasticityTest, HebbianConfiguration) {
    nlm::Hebbian hebbian;
    
    EXPECT_FLOAT_EQ(hebbian.getLearningRate(), 0.01f);
    EXPECT_FLOAT_EQ(hebbian.getTimeConstant(), 20.0f);
    EXPECT_FLOAT_EQ(hebbian.getWeightIncrement(), 0.1f);
}

// Test Hebbian parameter setting
TEST_F(PlasticityTest, HebbianParameterSetting) {
    nlm::Hebbian hebbian;
    
    hebbian.setLearningRate(0.05f);
    hebbian.setTimeConstant(30.0f);
    hebbian.setWeightIncrement(0.2f);
    
    EXPECT_FLOAT_EQ(hebbian.getLearningRate(), 0.05f);
    EXPECT_FLOAT_EQ(hebbian.getTimeConstant(), 30.0f);
    EXPECT_FLOAT_EQ(hebbian.getWeightIncrement(), 0.2f);
}

// Test Hebbian name
TEST_F(PlasticityTest, HebbianName) {
    nlm::Hebbian hebbian;
    
    EXPECT_NE(hebbian.getName(), nullptr);
    EXPECT_EQ(std::string(hebbian.getName()), "Hebbian");
}

// Test STDP potentiation (pre before post)
TEST_F(PlasticityTest, STDPPotentiation) {
    nlm::STDP stdp;
    nlm::RandomGenerator rng(42);
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Pre fires before post (positive dt) - should potentiate
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0};  // ms
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};  // ms (10ms after pre)
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should have increased due to pre-before-post
    EXPECT_GT(finalWeight, initialWeight);
    
    std::cout << "    STDP potentiation: Δ=" << (finalWeight - initialWeight) << std::endl;
}

// Test STDP depression (post before pre)
TEST_F(PlasticityTest, STDPDepression) {
    nlm::STDP stdp;
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Post fires before pre (negative dt) - should depress
    std::vector<nlm::Timestamp> preSpikes = {10.0, 20.0, 30.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};  // 5ms before pre
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should have decreased due to post-before-pre
    EXPECT_LT(finalWeight, initialWeight);
    
    std::cout << "    STDP depression: Δ=" << (finalWeight - initialWeight) << std::endl;
}

// Test STDP with empty spikes
TEST_F(PlasticityTest, STDPEmptySpikes) {
    nlm::STDP stdp;
    
    nlm::Synapse synapse(nlm::SynapseId(3), nlm::NeuronId(5), nlm::NeuronId(6));
    float initialWeight = synapse.getWeight();
    
    // Empty pre spikes - should not change
    std::vector<nlm::Timestamp> preSpikes;
    std::vector<nlm::Timestamp> postSpikes = {1.0, 2.0};
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    EXPECT_EQ(synapse.getWeight(), initialWeight);
    
    // Empty post spikes - should not change
    preSpikes = {1.0, 2.0};
    postSpikes = {};
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    EXPECT_EQ(synapse.getWeight(), initialWeight);
    
    // Both empty - should not change
    preSpikes = {};
    postSpikes = {};
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    EXPECT_EQ(synapse.getWeight(), initialWeight);
}

// Test STDP weight bounds
TEST_F(PlasticityTest, STDPWeightBounds) {
    nlm::STDP stdp;
    
    nlm::Synapse synapse(nlm::SynapseId(4), nlm::NeuronId(7), nlm::NeuronId(8));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.99f);  // Near max
    
    // Very strong potentiation
    std::vector<nlm::Timestamp> preSpikes = {0.0, 1.0, 2.0, 3.0, 4.0};
    std::vector<nlm::Timestamp> postSpikes = {1.0, 2.0, 3.0, 4.0, 5.0};
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should be clamped to max
    EXPECT_LE(synapse.getWeight(), 1.0f);
}

// Test Hebbian learning
TEST_F(PlasticityTest, HebbianLearning) {
    nlm::Hebbian hebbian;
    
    nlm::Synapse synapse(nlm::SynapseId(5), nlm::NeuronId(9), nlm::NeuronId(10));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.3f);
    
    float initialWeight = synapse.getWeight();
    
    // Simulate co-activation (both neurons fire)
    hebbian.update(&synapse, std::vector<nlm::Timestamp>{0.0}, std::vector<nlm::Timestamp>{0.0}, 0.001);
    
    // Weight should have increased due to Hebbian learning
    EXPECT_GT(synapse.getWeight(), initialWeight);
}

// Test plasticity rule name
TEST_F(PlasticityTest, PlasticityRuleName) {
    nlm::STDP stdp;
    nlm::Hebbian hebbian;
    
    EXPECT_NE(stdp.getName(), nullptr);
    EXPECT_NE(hebbian.getName(), nullptr);
    EXPECT_EQ(std::string(stdp.getName()), "STDP");
    EXPECT_EQ(std::string(hebbian.getName()), "Hebbian");
}

// Test plasticity rule application
TEST_F(PlasticityTest, PlasticityRuleApplication) {
    nlm::STDP stdp;
    nlm::Hebbian hebbian;
    
    nlm::Synapse synapse1(nlm::SynapseId(6), nlm::NeuronId(11), nlm::NeuronId(12));
    nlm::Synapse synapse2(nlm::SynapseId(7), nlm::NeuronId(13), nlm::NeuronId(14));
    
    // Test STDP weight change
    float weight1Before = synapse1.getWeight();
    stdp.applyWeightChange(&synapse1, 0.1f);
    float weight1After = synapse1.getWeight();
    EXPECT_GT(weight1After, weight1Before);
    
    // Test Hebbian weight change
    float weight2Before = synapse2.getWeight();
    hebbian.applyWeightChange(&synapse2, 0.05f);
    float weight2After = synapse2.getWeight();
    EXPECT_GT(weight2After, weight2Before);
}

// Test plasticity with random parameters
TEST_F(PlasticityTest, RandomPlasticityParameters) {
    nlm::RandomGenerator rng(42);
    
    // Test STDP with random parameters
    nlm::STDP stdp;
    stdp.setLTPWeight(rng.uniformReal(0.001, 0.1));
    stdp.setLTDWeight(rng.uniformReal(0.001, 0.1));
    stdp.setTimeConstant(rng.normal(20.0, 5.0));
    
    EXPECT_GT(stdp.getLTPWeight(), 0.0f);
    EXPECT_GT(stdp.getLTDWeight(), 0.0f);
    EXPECT_GT(stdp.getTimeConstant(), 0.0f);
    
    // Test Hebbian with random parameters
    nlm::Hebbian hebbian;
    hebbian.setLearningRate(rng.uniformReal(0.001, 0.1));
    hebbian.setTimeConstant(rng.normal(20.0, 5.0));
    hebbian.setWeightIncrement(rng.uniformReal(0.01, 0.5));
    
    EXPECT_GT(hebbian.getLearningRate(), 0.0f);
    EXPECT_GT(hebbian.getTimeConstant(), 0.0f);
    EXPECT_GT(hebbian.getWeightIncrement(), 0.0f);
}

// Test plasticity with spike history
TEST_F(PlasticityTest, PlasticityWithSpikeHistory) {
    nlm::STDP stdp;
    
    nlm::Synapse synapse(nlm::SynapseId(8), nlm::NeuronId(15), nlm::NeuronId(16));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    // Record spike history
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0, 30.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0, 35.0};
    
    for (float t : preSpikes) {
        synapse.recordPreSpike(t);
    }
    for (float t : postSpikes) {
        synapse.recordPostSpike(t);
    }
    
    EXPECT_EQ(synapse.getPreSpikeHistory().size(), 4);
    EXPECT_EQ(synapse.getPostSpikeHistory().size(), 4);
    
    // Apply STDP
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    EXPECT_TRUE(TestValidator::validateSynapse(synapse));
}

// Test plasticity reset
TEST_F(PlasticityTest, PlasticityReset) {
    nlm::STDP stdp;
    nlm::Hebbian hebbian;
    
    nlm::Synapse synapse(nlm::SynapseId(9), nlm::NeuronId(17), nlm::NeuronId(18));
    synapse.setWeight(0.8f);
    synapse.recordPreSpike(1.0);
    synapse.recordPostSpike(2.0);
    
    // Reset synapse (should reset weight)
    synapse.reset();
    
    EXPECT_WEIGHT_APPROX(synapse, 0.0f);
    EXPECT_TRUE(synapse.getPreSpikeHistory().empty());
    EXPECT_TRUE(synapse.getPostSpikeHistory().empty());
}

// Parameterized test for plasticity with different configurations
class ParameterizedPlasticityTest : public ParameterizedTestBase, public ::testing::WithParamInterface<TestParameters> {
public:
    static std::vector<TestParameters> GetTestParameters() {
        return {
            TestParameters(100, 0.1f, 0.01f, "STDP with small learning rate"),
            TestParameters(500, 0.05f, 0.001f, "STDP with very small learning rate"),
            TestParameters(1000, 0.02f, 0.0001f, "STDP with tiny learning rate")
        };
    }
};

TEST_P(ParameterizedPlasticityTest, STDPWithDifferentLearningRates) {
    TestParameters params = GetParam();
    
    nlm::STDP stdp;
    stdp.configure(params.learningRate, params.learningRate * 1.2f, 20.0f);
    
    nlm::Synapse synapse(nlm::SynapseId(10), nlm::NeuronId(19), nlm::NeuronId(20));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    // Apply STDP
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0};
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    EXPECT_TRUE(TestValidator::validateSynapse(synapse));
}

INSTANTIATE_TEST_SUITE_P(
    PlasticityParameterizedTests,
    ParameterizedPlasticityTest,
    ::testing::ValuesIn(ParameterizedPlasticityTest::GetTestParameters())
);

// Performance benchmark for STDP
TEST(PlasticityPerformanceTest, STDPPeformance) {
    TestBase testBase;
    
    nlm::STDP stdp;
    nlm::Synapse synapse(nlm::SynapseId(11), nlm::NeuronId(21), nlm::NeuronId(22));
    
    auto func = [&]() {
        std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0};
        std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};
        stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    };
    
    auto result = testBase.runBenchmark(func, 10000);
    
    std::cout << "STDP performance: " << result.timeMs << " ms for " 
              << result.iterations << " iterations (" << result.opsPerMs << " ops/ms)" << std::endl;
    
    // Should be able to perform at least 50,000 STDP updates per second
    EXPECT_GT(result.opsPerMs, 50000.0);
}

// Performance benchmark for Hebbian learning
TEST(PlasticityPerformanceTest, HebbianPerformance) {
    TestBase testBase;
    
    nlm::Hebbian hebbian;
    nlm::Synapse synapse(nlm::SynapseId(12), nlm::NeuronId(23), nlm::NeuronId(24));
    
    auto func = [&]() {
        hebbian.update(&synapse, std::vector<nlm::Timestamp>{0.0}, std::vector<nlm::Timestamp>{0.0}, 0.001);
    };
    
    auto result = testBase.runBenchmark(func, 10000);
    
    std::cout << "Hebbian performance: " << result.timeMs << " ms for " 
              << result.iterations << " iterations (" << result.opsPerMs << " ops/ms)" << std::endl;
}

// Edge case tests for plasticity
TEST_F(PlasticityTest, PlasticityEdgeCases) {
    nlm::STDP stdp;
    
    // Test with very large time constant
    stdp.setTimeConstant(1000.0f);
    EXPECT_FLOAT_EQ(stdp.getTimeConstant(), 1000.0f);
    
    // Test with very small weights
    stdp.setLTPWeight(0.0001f);
    stdp.setLTDWeight(0.0001f);
    EXPECT_FLOAT_EQ(stdp.getLTPWeight(), 0.0001f);
    EXPECT_FLOAT_EQ(stdp.getLTDWeight(), 0.0001f);
    
    // Test with negative time constant (should be clamped)
    stdp.setTimeConstant(-10.0f);
    EXPECT_GE(stdp.getTimeConstant(), 0.0f); // Should be clamped to non-negative
}

// Test plasticity with extreme spike patterns
TEST_F(PlasticityTest, ExtremeSpikePatterns) {
    nlm::STDP stdp;
    
    nlm::Synapse synapse(nlm::SynapseId(13), nlm::NeuronId(25), nlm::NeuronId(26));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    // Test with very closely spaced spikes
    std::vector<nlm::Timestamp> preSpikes = {0.0, 0.001, 0.002, 0.003};
    std::vector<nlm::Timestamp> postSpikes = {0.0005, 0.0015, 0.0025, 0.0035};
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    EXPECT_TRUE(TestValidator::validateSynapse(synapse));
}

} // namespace test
} // namespace nlm
