// Neuron Unit Tests
// Comprehensive test suite for Neuron class using Google Test framework

#include "tests/common/test_base.hpp"
#include <gtest/gtest.h>
#include <cmath>

namespace nlm {
namespace test {

// Test fixture for Neuron tests
class NeuronTest : public TestBase {
protected:
    void SetUp() override {
        TestBase::SetUp();
        
        // Create test neurons
        neuron1 = createTestNeuron(NeuronId(1), NeuronType::Excitatory);
        neuron2 = createTestNeuron(NeuronId(2), NeuronType::Inhibitory);
        neuron3 = createTestNeuron(NeuronId(3), NeuronType::Internal);
    }
    
    void TearDown() override {
        TestBase::TearDown();
    }
    
protected:
    nlm::Neuron neuron1;
    nlm::Neuron neuron2;
    nlm::Neuron neuron3;
};

// Test basic neuron creation and identity
TEST_F(NeuronTest, Creation) {
    EXPECT_TRUE(neuron1.getId().isValid());
    EXPECT_EQ(neuron1.getType(), NeuronType::Excitatory);
    EXPECT_MEMB_POTENTIAL_APPROX(neuron1, -70.0f);
    EXPECT_EQ(neuron1.getThreshold(), -55.0f);
}

// Test neuron type setting
TEST_F(NeuronTest, TypeSetting) {
    neuron1.setType(NeuronType::Inhibitory);
    EXPECT_EQ(neuron1.getType(), NeuronType::Inhibitory);
    
    neuron2.setType(NeuronType::Excitatory);
    EXPECT_EQ(neuron2.getType(), NeuronType::Excitatory);
    
    neuron3.setType(NeuronType::Internal);
    EXPECT_EQ(neuron3.getType(), NeuronType::Internal);
}

// Test membrane potential manipulation
TEST_F(NeuronTest, MembranePotential) {
    // Default potential
    EXPECT_MEMB_POTENTIAL_APPROX(neuron1, -70.0f);
    
    // Set specific potential
    neuron1.setMembranePotential(-65.0f);
    EXPECT_MEMB_POTENTIAL_APPROX(neuron1, -65.0f);
    
    // Add to potential
    neuron1.addToMembranePotential(10.0f);
    EXPECT_MEMB_POTENTIAL_APPROX(neuron1, -55.0f);
    
    // Add negative potential
    neuron2.addToMembranePotential(-20.0f);
    EXPECT_MEMB_POTENTIAL_APPROX(neuron2, -90.0f);
}

// Test threshold setting
TEST_F(NeuronTest, Threshold) {
    EXPECT_EQ(neuron1.getThreshold(), -55.0f);
    
    neuron1.setThreshold(-50.0f);
    EXPECT_EQ(neuron1.getThreshold(), -50.0f);
    
    neuron1.setThreshold(-40.0f);
    EXPECT_EQ(neuron1.getThreshold(), -40.0f);
}

// Test firing state
TEST_F(NeuronTest, FiringState) {
    EXPECT_FALSE(neuron1.isFiring());
    EXPECT_FALSE(neuron1.isRefractory());
    
    neuron1.setFiringState(FiringState::Active);
    EXPECT_TRUE(neuron1.isFiring());
    EXPECT_FALSE(neuron1.isRefractory());
    
    neuron2.setRefractoryPeriod(10);
    EXPECT_TRUE(neuron2.isRefractory());
    EXPECT_FALSE(neuron2.isFiring());
}

// Test refractory period management
TEST_F(NeuronTest, RefractoryPeriod) {
    neuron1.setRefractoryPeriod(5);
    EXPECT_TRUE(neuron1.isRefractory());
    EXPECT_FALSE(neuron1.isFiring());
    
    for (int i = 0; i < 4; ++i) {
        neuron1.decrementRefractory();
        EXPECT_TRUE(neuron1.isRefractory());
    }
    
    neuron1.decrementRefractory();
    EXPECT_FALSE(neuron1.isRefractory());
    EXPECT_EQ(neuron1.getState().firingState, FiringState::Resting);
}

// Test firing rate
TEST_F(NeuronTest, FiringRate) {
    EXPECT_MEMB_POTENTIAL_APPROX(neuron1, -70.0f);
    
    neuron1.setFiringRate(10.0f);
    EXPECT_EQ(neuron1.getFiringRate(), 10.0f);
    
    neuron1.setFiringRate(0.0f);
    EXPECT_EQ(neuron1.getFiringRate(), 0.0f);
}

// Test LIF neuron parameters
TEST_F(NeuronTest, LIFParameters) {
    EXPECT_EQ(neuron1.getLeakConductance(), 10.0f);
    
    neuron1.setLeakConductance(15.0f);
    EXPECT_EQ(neuron1.getLeakConductance(), 15.0f);
    
    neuron1.setRestingPotential(-65.0f);
    EXPECT_MEMB_POTENTIAL_APPROX(neuron1, -65.0f);
    
    neuron1.setResetPotential(-60.0f);
    EXPECT_MEMB_POTENTIAL_APPROX(neuron1, -65.0f); // Should still be resting potential
    
    neuron1.setRefractoryPeriod(20);
    EXPECT_EQ(neuron1.getRefractoryPeriod(), 20);
}

// Test spike detection
TEST_F(NeuronTest, SpikeDetection) {
    // Set potential below threshold
    neuron1.setMembranePotential(-60.0f);
    EXPECT_FALSE(neuron1.checkThreshold());
    
    // Set potential at threshold
    neuron1.setMembranePotential(-55.0f);
    EXPECT_TRUE(neuron1.checkThreshold());
    
    // Set potential above threshold
    neuron1.setMembranePotential(-50.0f);
    EXPECT_TRUE(neuron1.checkThreshold());
}

// Test last spike time
TEST_F(NeuronTest, LastSpikeTime) {
    EXPECT_EQ(neuron1.getLastSpikeTime(), -1.0f);
    
    // Note: Neuron doesn't have a direct spike recording method in the current API
    // This test validates the current state
}

// Test neuron state integrity
TEST_F(NeuronTest, StateIntegrity) {
    const auto& state1 = neuron1.getState();
    const auto& state2 = neuron2.getState();
    
    EXPECT_TRUE(TestValidator::validateNeuron(neuron1));
    EXPECT_TRUE(TestValidator::validateNeuron(neuron2));
    
    // Test state consistency
    EXPECT_EQ(state1.membranePotential, neuron1.getMembranePotential());
    EXPECT_EQ(state1.threshold, neuron1.getThreshold());
    EXPECT_EQ(state1.firingState, neuron2.getState().firingState);
}

// Parameterized test for different neuron configurations
class ParameterizedNeuronTest : public ParameterizedTestBase, public ::testing::WithParamInterface<TestParameters> {
public:
    static std::vector<TestParameters> GetTestParameters() {
        return {
            TestParameters(10, 0.1f, 0.01f, "Small network"),
            TestParameters(50, 0.2f, 0.05f, "Medium network"),
            TestParameters(200, 0.15f, 0.1f, "Large network")
        };
    }
};

TEST_P(ParameterizedNeuronTest, CreationWithDifferentParameters) {
    TestParameters params = GetParam();
    
    std::vector<nlm::Neuron> neurons;
    for (int i = 0; i < params.neuronCount; ++i) {
        neurons.push_back(createTestNeuron(NeuronId(i), 
            static_cast<NeuronType>(i % 3)));
    }
    
    EXPECT_EQ(neurons.size(), static_cast<size_t>(params.neuronCount));
    EXPECT_TRUE(TestValidator::validateNeuron(neurons[0]));
}

INSTANTIATE_TEST_SUITE_P(
    NeuronParameterizedTests,
    ParameterizedNeuronTest,
    ::testing::ValuesIn(ParameterizedNeuronTest::GetTestParameters())
);

// Stress test for neuron operations
TEST(NeuronStressTest, RapidOperations) {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    for (int i = 0; i < 10000; ++i) {
        neuron.setMembranePotential(static_cast<MembranePotential>(i % 200 - 100));
        neuron.setFiringState(static_cast<FiringState>(i % 3));
        neuron.setRefractoryPeriod(i % 20);
        neuron.decrementRefractory();
    }
    
    EXPECT_TRUE(TestValidator::validateNeuron(neuron));
}

// Performance benchmark for neuron operations
TEST(NeuronPerformanceTest, CreationBenchmark) {
    TestBase testBase;
    auto result = testBase.runBenchmark([]() {
        nlm::Neuron neuron(nlm::NeuronId(1));
    }, 100000);
    
    std::cout << "Neuron creation performance: " << result.timeMs << " ms for " 
              << result.iterations << " iterations (" << result.opsPerMs << " ops/ms)" << std::endl;
    
    // Should be able to create at least 10,000 neurons per second
    EXPECT_GT(result.opsPerMs, 10000.0);
}

// Integration test with random generator
TEST_F(NeuronTest, RandomInitialization) {
    nlm::RandomGenerator rng(42);
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Initialize with random parameters
    neuron.setType(static_cast<NeuronType>(rng.uniformInt(0, 2)));
    neuron.setMembranePotential(rng.normal(-70.0, 5.0));
    neuron.setThreshold(rng.normal(-55.0, 5.0));
    neuron.setLeakConductance(std::abs(rng.normal(10.0, 2.0)));
    
    EXPECT_TRUE(TestValidator::validateNeuron(neuron));
}

// Edge case tests
TEST_F(NeuronTest, ExtremeValues) {
    // Test extreme membrane potential
    neuron1.setMembranePotential(100.0f);
    EXPECT_MEMB_POTENTIAL_APPROX(neuron1, 100.0f);
    
    neuron1.setMembranePotential(-100.0f);
    EXPECT_MEMB_POTENTIAL_APPROX(neuron1, -100.0f);
    
    // Test extreme refractory period
    neuron1.setRefractoryPeriod(1000);
    EXPECT_TRUE(neuron1.isRefractory());
    
    neuron1.setRefractoryPeriod(0);
    EXPECT_FALSE(neuron1.isRefractory());
}

// Test helper functions
TEST(TestHelperFunctions, MathematicalHelpers) {
    TestBase testBase;
    
    EXPECT_FLOAT_EQ(testBase.normalize(0.5f, 0.0f, 1.0f), 0.5f);
    EXPECT_FLOAT_EQ(testBase.normalize(0.0f, 0.0f, 1.0f), 0.0f);
    EXPECT_FLOAT_EQ(testBase.normalize(1.0f, 0.0f, 1.0f), 1.0f);
    
    EXPECT_FLOAT_EQ(testBase.clamp(5.0f, 0.0f, 10.0f), 5.0f);
    EXPECT_FLOAT_EQ(testBase.clamp(-5.0f, 0.0f, 10.0f), 0.0f);
    EXPECT_FLOAT_EQ(testBase.clamp(15.0f, 0.0f, 10.0f), 10.0f);
    
    EXPECT_FLOAT_EQ(testBase.lerp(0.0f, 10.0f, 0.5f), 5.0f);
    EXPECT_FLOAT_EQ(testBase.lerp(0.0f, 10.0f, 0.0f), 0.0f);
    EXPECT_FLOAT_EQ(testBase.lerp(0.0f, 10.0f, 1.0f), 10.0f);
}

} // namespace test
} // namespace nlm
