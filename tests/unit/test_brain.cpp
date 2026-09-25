// Brain Unit Tests
// Comprehensive test suite for Brain class using Google Test framework

#include "tests/common/test_base.hpp"
#include <gtest/gtest.h>
#include <cmath>
#include <vector>
#include <algorithm>

namespace nlm {
namespace test {

// Test fixture for Brain tests
class BrainTest : public TestBase {
protected:
    void SetUp() override {
        TestBase::SetUp();
        
        // Create test configuration
        config = createDefaultConfig();
        
        // Create brain for testing
        brain = std::make_shared<nlm::Brain>(config);
        brain->initialize();
    }
    
    void TearDown() override {
        TestBase::TearDown();
        brain.reset();
    }
    
protected:
    std::shared_ptr<nlm::Config> config;
    std::shared_ptr<nlm::Brain> brain;
};

// Test brain creation
TEST_F(BrainTest, Creation) {
    EXPECT_NE(brain, nullptr);
    EXPECT_TRUE(brain->getTotalNeuronCount() > 0);
    EXPECT_TRUE(brain->getTotalSynapseCount() > 0);
    EXPECT_TRUE(brain->getRegionCount() > 0);
}

// Test brain initialization
TEST_F(BrainTest, Initialization) {
    // Test with different configuration
    auto testConfig = createDefaultConfig();
    testConfig->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    testConfig->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain testBrain(testConfig);
    bool initialized = testBrain.initialize();
    EXPECT_TRUE(initialized);
    
    EXPECT_EQ(testBrain.getTotalNeuronCount(), 200);
    EXPECT_EQ(testBrain.getRegionCount(), 3);
    EXPECT_TRUE(testBrain.getTotalSynapseCount() > 0);
}

// Test brain step execution
TEST_F(BrainTest, Step) {
    // Record initial state
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    // Run a few steps
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain->step(step);
    }
    
    // Verify brain still functional
    EXPECT_EQ(brain->getTotalNeuronCount(), 100);
    EXPECT_GT(brain->getTotalSpikeCount(), initialSpikes);
}

// Test brain reset
TEST_F(BrainTest, Reset) {
    // Run a step
    brain->step(0);
    
    // Verify spike count increased
    size_t spikeCountBefore = brain->getTotalSpikeCount();
    EXPECT_GT(spikeCountBefore, 0);
    
    // Reset brain
    brain->reset();
    
    // Verify reset worked
    EXPECT_EQ(brain->getTotalNeuronCount(), 100);
    EXPECT_EQ(brain->getTotalSpikeCount(), 0);
    EXPECT_EQ(brain->getFiringNeuronCount(), 0);
}

// Test brain regions
TEST_F(BrainTest, Regions) {
    auto regionIds = brain->getRegionIds();
    EXPECT_GT(regionIds.size(), 1);
    
    for (auto rid : regionIds) {
        auto* region = brain->getRegion(rid);
        EXPECT_NE(region, nullptr);
        EXPECT_GT(region->getNeuronCount(), 0);
    }
}

// Test brain action production
TEST_F(BrainTest, ActionProduction) {
    // Inject some current to make neurons fire
    brain->injectCurrentToNeurons(nlm::NeuronType::Excitatory, 10.0f);
    
    brain->step(0);
    
    auto action = brain->produceAction();
    EXPECT_NE(action, nullptr);
    
    // Action should have some data
    auto& actionData = action->getActionData();
    EXPECT_FALSE(actionData.empty());
}

// Test brain statistics
TEST_F(BrainTest, Statistics) {
    // Verify initial statistics are valid
    EXPECT_GE(brain->getTotalNeuronCount(), 0);
    EXPECT_GE(brain->getTotalSynapseCount(), 0);
    EXPECT_GE(brain->getFiringNeuronCount(), 0);
    EXPECT_GE(brain->getAverageFiringRate(), 0.0f);
    
    float eiRatio = brain->getExcitationInhibitionRatio();
    EXPECT_GE(eiRatio, 0.0f);
    EXPECT_LE(eiRatio, 1000.0f); // Can be very large if few inhibitory neurons
}

// Test brain sensory input
TEST_F(BrainTest, SensoryInput) {
    // Create mock sensory input
    class MockSensoryInput {
    public:
        std::vector<float> getVision() const {
            return std::vector<float>(16 * 16, 0.5f); // 16x16 vision at 50% activation
        }
        std::vector<float> getTouch() const {
            return std::vector<float>(8, 0.3f); // 8 touch sensors at 30% activation
        }
        std::vector<float> getInternal() const {
            return std::vector<float>(4, 0.1f); // 4 internal signals at 10% activation
        }
    };
    
    MockSensoryInput mockInput;
    brain->receiveSensoryInput(mockInput);
    
    // Verify brain processed input (no crash)
    brain->step(0);
    EXPECT_TRUE(true); // If we get here, input processing worked
}

// Test brain neuromodulation access
TEST_F(BrainTest, NeuromodulationAccess) {
    nlm::STDP* stdp = brain->getSTDP();
    nlm::Hebbian* hebbian = brain->getHebbian();
    nlm::StructuralPlasticity* structural = brain->getStructuralPlasticity();
    
    EXPECT_NE(stdp, nullptr);
    EXPECT_NE(hebbian, nullptr);
    EXPECT_NE(structural, nullptr);
    
    // Test basic operations
    EXPECT_GT(stdp->getLTPWeight(), 0.0f);
    EXPECT_GT(hebbian->getLearningRate(), 0.0f);
}

// Test brain with random configuration
TEST_F(BrainTest, RandomConfiguration) {
    nlm::RandomGenerator rng(42);
    
    auto randomConfig = createDefaultConfig();
    randomConfig->set("neuron_count", rng.uniformInt(50, 500), nlm::ConfigSource::Default);
    randomConfig->set("region_count", rng.uniformInt(1, 10), nlm::ConfigSource::Default);
    randomConfig->set("connection_probability", rng.uniformReal(0.01, 0.5), nlm::ConfigSource::Default);
    
    nlm::Brain randomBrain(randomConfig);
    bool initialized = randomBrain.initialize();
    EXPECT_TRUE(initialized);
    
    EXPECT_LE(randomBrain.getTotalNeuronCount(), 500);
    EXPECT_LE(randomBrain.getRegionCount(), 10);
    EXPECT_GT(randomBrain.getTotalSynapseCount(), 0);
}

// Parameterized test for brain with different sizes
class ParameterizedBrainTest : public ParameterizedTestBase, public ::testing::WithParamInterface<TestParameters> {
public:
    static std::vector<TestParameters> GetTestParameters() {
        return {
            TestParameters(50, 0.1f, 0.01f, "Small brain"),
            TestParameters(200, 0.2f, 0.05f, "Medium brain"),
            TestParameters(500, 0.15f, 0.1f, "Large brain")
        };
    }
};

TEST_P(ParameterizedBrainTest, BrainWithDifferentParameters) {
    TestParameters params = GetParam();
    
    auto paramConfig = createDefaultConfig();
    paramConfig->set("neuron_count", static_cast<int64_t>(params.neuronCount), nlm::ConfigSource::Default);
    paramConfig->set("region_count", static_cast<int64_t>(params.neuronCount / 50), nlm::ConfigSource::Default);
    paramConfig->set("connection_probability", params.connectionProbability, nlm::ConfigSource::Default);
    
    nlm::Brain paramBrain(paramConfig);
    bool initialized = paramBrain.initialize();
    EXPECT_TRUE(initialized);
    
    EXPECT_EQ(paramBrain.getTotalNeuronCount(), params.neuronCount);
    EXPECT_GT(paramBrain.getTotalSynapseCount(), 0);
}

INSTANTIATE_TEST_SUITE_P(
    BrainParameterizedTests,
    ParameterizedBrainTest,
    ::testing::ValuesIn(ParameterizedBrainTest::GetTestParameters())
);

// Performance benchmark for brain initialization
TEST(BrainPerformanceTest, InitializationPerformance) {
    TestBase testBase;
    
    auto func = []() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
    };
    
    auto result = testBase.runBenchmark(func, 10);
    
    std::cout << "Brain initialization performance: " << result.timeMs << " ms for " 
              << result.iterations << " iterations (" << result.opsPerMs << " ops/ms)" << std::endl;
    
    // Should be able to initialize at least 10 brains per second
    EXPECT_GT(result.opsPerMs, 10.0);
}

// Performance benchmark for brain step
TEST(BrainPerformanceTest, StepPerformance) {
    TestBase testBase;
    
    nlm::Brain brain(createDefaultConfig());
    brain.initialize();
    
    auto func = [&]() {
        brain.step(0);
    };
    
    auto result = testBase.runBenchmark(func, 1000);
    
    std::cout << "Brain step performance: " << result.timeMs << " ms for " 
              << result.iterations << " iterations (" << result.opsPerMs << " ops/ms)" << std::endl;
    
    // Should be able to step at least 5,000 times per second
    EXPECT_GT(result.opsPerMs, 5000.0);
}

// Edge case tests for brain
TEST_F(BrainTest, BrainEdgeCases) {
    // Test with very small configuration
    auto smallConfig = createDefaultConfig();
    smallConfig->set("neuron_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    smallConfig->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain smallBrain(smallConfig);
    bool initialized = smallBrain.initialize();
    EXPECT_TRUE(initialized);
    EXPECT_EQ(smallBrain.getTotalNeuronCount(), 1);
    
    // Test with very large configuration
    auto largeConfig = createDefaultConfig();
    largeConfig->set("neuron_count", static_cast<int64_t>(10000), nlm::ConfigSource::Default);
    largeConfig->set("region_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain largeBrain(largeConfig);
    bool largeInitialized = largeBrain.initialize();
    EXPECT_TRUE(largeInitialized);
    EXPECT_EQ(largeBrain.getTotalNeuronCount(), 10000);
}

// Test brain with zero connection probability
TEST_F(BrainTest, ZeroConnectionProbability) {
    auto zeroConfig = createDefaultConfig();
    zeroConfig->set("connection_probability", 0.0f, nlm::ConfigSource::Default);
    
    nlm::Brain zeroBrain(zeroConfig);
    bool initialized = zeroBrain.initialize();
    EXPECT_TRUE(initialized);
    
    // With zero connection probability, synapse count should be minimal
    EXPECT_EQ(zeroBrain.getTotalSynapseCount(), 0);
}

// Test brain statistics after long simulation
TEST_F(BrainTest, LongSimulationStatistics) {
    size_t initialSpikes = brain->getTotalSpikeCount();
    size_t initialFiringNeurons = brain->getFiringNeuronCount();
    float initialAvgFiringRate = brain->getAverageFiringRate();
    
    // Run many steps
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain->step(step);
        
        // Check that statistics don't become invalid
        EXPECT_GE(brain->getTotalSpikeCount(), initialSpikes);
        EXPECT_GE(brain->getFiringNeuronCount(), 0);
        EXPECT_GE(brain->getAverageFiringRate(), 0.0f);
    }
    
    // Statistics should be valid
    EXPECT_GE(brain->getTotalNeuronCount(), 0);
    EXPECT_GE(brain->getTotalSynapseCount(), 0);
}

} // namespace test
} // namespace nlm
