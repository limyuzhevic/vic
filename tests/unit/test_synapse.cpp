// Synapse Unit Tests
// Comprehensive test suite for Synapse class using Google Test framework

#include "tests/common/test_base.hpp"
#include <gtest/gtest.h>
#include <cmath>
#include <vector>
#include <algorithm>

namespace nlm {
namespace test {

// Test fixture for Synapse tests
class SynapseTest : public TestBase {
protected:
    void SetUp() override {
        TestBase::SetUp();
        
        // Create test synapses
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

// Test basic synapse creation and identity
TEST_F(SynapseTest, Creation) {
    EXPECT_TRUE(synapse1.getId().isValid());
    EXPECT_EQ(synapse1.getSourceNeuron(), NeuronId(1));
    EXPECT_EQ(synapse1.getDestinationNeuron(), NeuronId(2));
    EXPECT_WEIGHT_APPROX(synapse1, 0.0f);
    EXPECT_EQ(synapse1.getDelay(), 1);
    EXPECT_EQ(synapse1.getType(), SynapseType::Excitatory);
    EXPECT_TRUE(synapse1.isExcitatory());
    EXPECT_FALSE(synapse1.isInhibitory());
}

// Test synapse weight manipulation
TEST_F(SynapseTest, Weight) {
    EXPECT_WEIGHT_APPROX(synapse1, 0.0f);
    
    synapse1.setWeight(0.5f);
    EXPECT_WEIGHT_APPROX(synapse1, 0.5f);
    
    synapse1.addToWeight(0.2f);
    EXPECT_WEIGHT_APPROX(synapse1, 0.7f);
    
    synapse1.addToWeight(-0.3f);
    EXPECT_WEIGHT_APPROX(synapse1, 0.4f);
}

// Test synapse weight bounds/clamping
TEST_F(SynapseTest, WeightBounds) {
    // Test upper bound
    synapse1.setWeight(10.0f);
    EXPECT_LE(synapse1.getWeight(), 1.0f);
    
    // Test lower bound
    synapse1.setWeight(-10.0f);
    EXPECT_GE(synapse1.getWeight(), -1.0f);
    
    // Test positive weights
    synapse2.setWeight(1.5f);
    EXPECT_LE(synapse2.getWeight(), 1.0f);
    
    // Test negative weights
    synapse3.setWeight(-1.5f);
    EXPECT_GE(synapse3.getWeight(), -1.0f);
}

// Test synapse type
TEST_F(SynapseTest, Type) {
    EXPECT_EQ(synapse1.getType(), SynapseType::Excitatory);
    EXPECT_TRUE(synapse1.isExcitatory());
    EXPECT_FALSE(synapse1.isInhibitory());
    
    synapse1.setType(SynapseType::Inhibitory);
    EXPECT_EQ(synapse1.getType(), SynapseType::Inhibitory);
    EXPECT_FALSE(synapse1.isExcitatory());
    EXPECT_TRUE(synapse1.isInhibitory());
}

// Test synapse delay
TEST_F(SynapseTest, Delay) {
    EXPECT_EQ(synapse1.getDelay(), 1); // Default
    
    synapse1.setDelay(5);
    EXPECT_EQ(synapse1.getDelay(), 5);
    
    synapse1.setDelay(0);
    EXPECT_EQ(synapse1.getDelay(), 0);
    
    synapse1.setDelay(100);
    EXPECT_EQ(synapse1.getDelay(), 100);
}

// Test spike history
TEST_F(SynapseTest, SpikeHistory) {
    EXPECT_TRUE(synapse1.getPreSpikeHistory().empty());
    EXPECT_TRUE(synapse1.getPostSpikeHistory().empty());
    
    synapse1.recordPreSpike(0.1);
    synapse1.recordPreSpike(0.2);
    synapse1.recordPostSpike(0.15);
    
    EXPECT_EQ(synapse1.getPreSpikeHistory().size(), 2);
    EXPECT_EQ(synapse1.getPostSpikeHistory().size(), 1);
    
    EXPECT_EQ(synapse1.getPreSpikeHistory()[0], 0.1);
    EXPECT_EQ(synapse1.getPreSpikeHistory()[1], 0.2);
    EXPECT_EQ(synapse1.getPostSpikeHistory()[0], 0.15);
    
    synapse1.clearHistory();
    EXPECT_TRUE(synapse1.getPreSpikeHistory().empty());
    EXPECT_TRUE(synapse1.getPostSpikeHistory().empty());
}

// Test eligibility trace
TEST_F(SynapseTest, EligibilityTrace) {
    EXPECT_FLOAT_EQ(synapse1.getEligibilityTrace(), 0.0f);
    
    synapse1.setEligibilityTrace(0.5f);
    EXPECT_FLOAT_EQ(synapse1.getEligibilityTrace(), 0.5f);
    
    // Test decay
    synapse1.decayEligibilityTrace(0.1f);
    EXPECT_LT(synapse1.getEligibilityTrace(), 0.5f);
    EXPECT_GT(synapse1.getEligibilityTrace(), 0.0f);
    
    // Test decay to zero
    synapse1.setEligibilityTrace(0.1f);
    synapse1.decayEligibilityTrace(0.2f); // decay rate > current trace
    EXPECT_LE(synapse1.getEligibilityTrace(), 0.0f);
}

// Test synaptic efficacy
TEST_F(SynapseTest, Efficacy) {
    EXPECT_FLOAT_EQ(synapse1.getEfficacy(), 1.0f); // Default
    
    synapse1.setEfficacy(0.5f);
    EXPECT_FLOAT_EQ(synapse1.getEfficacy(), 0.5f);
    
    // Test bounds
    synapse1.setEfficacy(5.0f);
    EXPECT_LE(synapse1.getEfficacy(), 2.0f); // Should be clamped
    
    synapse1.setEfficacy(-5.0f);
    EXPECT_GE(synapse1.getEfficacy(), 0.0f); // Should be clamped to 0
}

// Test plasticity flags
TEST_F(SynapseTest, PlasticityFlags) {
    const auto& flags1 = synapse1.getPlasticityFlags();
    const auto& flags2 = synapse2.getPlasticityFlags();
    
    // Default flags should be valid
    EXPECT_TRUE(flags1.hebbian);
    EXPECT_TRUE(flags1.stdp);
    EXPECT_TRUE(flags1.rewardModulated);
    
    synapse1.getPlasticityFlags().hebbian = false;
    EXPECT_FALSE(synapse1.getPlasticityFlags().hebbian);
    
    synapse1.getPlasticityFlags().stdp = false;
    EXPECT_FALSE(synapse1.getPlasticityFlags().stdp);
    
    synapse1.getPlasticityFlags().rewardModulated = false;
    EXPECT_FALSE(synapse1.getPlasticityFlags().rewardModulated);
}

// Test plasticity enable/disable
TEST_F(SynapseTest, PlasticityControl) {
    synapse1.enablePlasticity(false, true, true);
    EXPECT_FALSE(synapse1.getPlasticityFlags().hebbian);
    EXPECT_TRUE(synapse1.getPlasticityFlags().stdp);
    EXPECT_TRUE(synapse1.getPlasticityFlags().rewardModulated);
    
    synapse1.enablePlasticity(true, false, false);
    EXPECT_TRUE(synapse1.getPlasticityFlags().hebbian);
    EXPECT_FALSE(synapse1.getPlasticityFlags().stdp);
    EXPECT_FALSE(synapse1.getPlasticityFlags().rewardModulated);
}

// Test random initialization
TEST_F(SynapseTest, RandomInitialization) {
    nlm::RandomGenerator rng(42);
    nlm::Synapse synapse(nlm::SynapseId(4), nlm::NeuronId(7), nlm::NeuronId(8));
    
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.initializeRandom(rng);
    
    // Weight should be positive for excitatory
    EXPECT_GT(synapse.getWeight(), 0.0f);
    
    // Delay should be in valid range
    EXPECT_GE(synapse.getDelay(), 1);
    EXPECT_LE(synapse.getDelay(), 5);
    
    // Efficacy should be reasonable
    EXPECT_GT(synapse.getEfficacy(), 0.0f);
}

// Test synapse reset
TEST_F(SynapseTest, Reset) {
    synapse1.setWeight(0.5f);
    synapse1.recordPreSpike(0.1);
    synapse1.recordPostSpike(0.2);
    synapse1.setEligibilityTrace(0.3f);
    synapse1.setEfficacy(2.0f);
    
    synapse1.reset();
    
    EXPECT_WEIGHT_APPROX(synapse1, 0.0f);
    EXPECT_TRUE(synapse1.getPreSpikeHistory().empty());
    EXPECT_TRUE(synapse1.getPostSpikeHistory().empty());
    EXPECT_FLOAT_EQ(synapse1.getEligibilityTrace(), 0.0f);
    EXPECT_FLOAT_EQ(synapse1.getEfficacy(), 1.0f);
}

// Test multiple synapses with different configurations
TEST(SynapseConfigurationTest, MultipleSynapses) {
    std::vector<nlm::Synapse> synapses;
    
    for (int i = 0; i < 10; ++i) {
        nlm::Synapse synapse(SynapseId(i), NeuronId(i), NeuronId(i + 10));
        
        // Alternate between excitatory and inhibitory
        if (i % 2 == 0) {
            synapse.setType(SynapseType::Excitatory);
        } else {
            synapse.setType(SynapseType::Inhibitory);
        }
        
        // Set random weight
        float weight = static_cast<float>(i % 10) / 10.0f;
        synapse.setWeight(weight);
        
        synapses.push_back(synapse);
    }
    
    // Validate all synapses
    for (size_t i = 0; i < synapses.size(); ++i) {
        const auto& syn = synapses[i];
        EXPECT_TRUE(TestValidator::validateSynapse(syn));
        
        // Check configuration
        if (i % 2 == 0) {
            EXPECT_TRUE(syn.isExcitatory());
            EXPECT_FALSE(syn.isInhibitory());
        } else {
            EXPECT_TRUE(syn.isInhibitory());
            EXPECT_FALSE(syn.isExcitatory());
        }
    }
}

// Test synapse step functionality
TEST_F(SynapseTest, Step) {
    // Record some spike history
    synapse1.recordPreSpike(0.0);
    synapse1.recordPostSpike(0.5);
    synapse1.setEligibilityTrace(0.3f);
    
    // Call step method (should not crash)
    synapse1.step(1.0);
    
    // Verify spike history is preserved
    EXPECT_EQ(synapse1.getPreSpikeHistory().size(), 1);
    EXPECT_EQ(synapse1.getPostSpikeHistory().size(), 1);
    EXPECT_FLOAT_EQ(synapse1.getEligibilityTrace(), 0.3f);
}

// Parameterized test for synapse configurations
class ParameterizedSynapseTest : public ParameterizedTestBase, public ::testing::WithParamInterface<TestParameters> {
public:
    static std::vector<TestParameters> GetTestParameters() {
        return {
            TestParameters(100, 0.1f, 0.01f, "Small network"),
            TestParameters(1000, 0.05f, 0.001f, "Medium network"),
            TestParameters(5000, 0.02f, 0.0001f, "Large network")
        };
    }
};

TEST_P(ParameterizedSynapseTest, ConfigurationWithParameters) {
    TestParameters params = GetParam();
    
    std::vector<nlm::Synapse> synapses;
    for (int i = 0; i < params.neuronCount; ++i) {
        nlm::Synapse synapse(SynapseId(i), NeuronId(i), NeuronId(i + params.neuronCount));
        synapse.setWeight(params.learningRate * (i % 10 + 1));
        synapses.push_back(synapse);
    }
    
    EXPECT_EQ(synapses.size(), static_cast<size_t>(params.neuronCount));
    
    // Validate synapse properties
    for (const auto& syn : synapses) {
        EXPECT_TRUE(TestValidator::validateSynapse(syn));
    }
}

INSTANTIATE_TEST_SUITE_P(
    SynapseParameterizedTests,
    ParameterizedSynapseTest,
    ::testing::ValuesIn(ParameterizedSynapseTest::GetTestParameters())
);

// Performance benchmark for synapse operations
TEST(SynapsePerformanceTest, CreationBenchmark) {
    TestBase testBase;
    auto result = testBase.runBenchmark([]() {
        nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    }, 100000);
    
    std::cout << "Synapse creation performance: " << result.timeMs << " ms for " 
              << result.iterations << " iterations (" << result.opsPerMs << " ops/ms)" << std::endl;
    
    // Should be able to create at least 20,000 synapses per second
    EXPECT_GT(result.opsPerMs, 20000.0);
}

// Performance benchmark for weight operations
TEST(SynapsePerformanceTest, WeightOperationsBenchmark) {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    auto func = [&]() {
        synapse.setWeight(0.5f);
        synapse.addToWeight(0.1f);
        synapse.setWeight(-0.2f);
    };
    
    TestBase testBase;
    auto result = testBase.runBenchmark(func, 100000);
    
    std::cout << "Synapse weight operations performance: " << result.timeMs << " ms for " 
              << result.iterations << " iterations (" << result.opsPerMs << " ops/ms)" << std::endl;
}

// Edge case tests
TEST_F(SynapseTest, EdgeCases) {
    // Test with very small delay
    synapse1.setDelay(0);
    EXPECT_EQ(synapse1.getDelay(), 0);
    
    // Test with very large delay
    synapse1.setDelay(10000);
    EXPECT_EQ(synapse1.getDelay(), 10000);
    
    // Test weight at boundaries
    synapse1.setWeight(1.0f);
    EXPECT_LE(synapse1.getWeight(), 1.0f);
    
    synapse2.setWeight(-1.0f);
    EXPECT_GE(synapse2.getWeight(), -1.0f);
    
    // Test efficacy boundaries
    synapse1.setEfficacy(100.0f);
    EXPECT_LE(synapse1.getEfficacy(), 2.0f);
    
    synapse2.setEfficacy(-100.0f);
    EXPECT_GE(synapse2.getEfficacy(), 0.0f);
}

// Stress test for synapse operations
TEST(SynapseStressTest, RapidOperations) {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    for (int i = 0; i < 10000; ++i) {
        synapse.setWeight(static_cast<float>(i % 20) / 10.0f - 1.0f);
        synapse.setDelay(i % 100);
        synapse.setEfficacy(static_cast<float>(i % 50) / 10.0f);
        synapse.setEligibilityTrace(static_cast<float>(i % 100) / 100.0f);
        synapse.decayEligibilityTrace(0.01f);
        
        // Record spike history
        if (i % 10 == 0) {
            synapse.recordPreSpike(static_cast<float>(i));
        }
    }
    
    EXPECT_TRUE(TestValidator::validateSynapse(synapse));
}

// Integration test with neuron and synapse
TEST(IntegrationTest, NeuronSynapseConnection) {
    nlm::Neuron neuron(nlm::NeuronId(1));
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Neuron should be able to send spike to synapse
    neuron.setFiringState(nlm::FiringState::Active);
    
    // Synapse should be able to record spike
    synapse.recordPreSpike(1.0);
    
    // Verify connection
    EXPECT_TRUE(synapse.getSourceNeuron() == neuron.getId());
    EXPECT_TRUE(neuron.getState().firingState == nlm::FiringState::Active);
    EXPECT_EQ(synapse.getPreSpikeHistory().size(), 1);
}

} // namespace test
} // namespace nlm
