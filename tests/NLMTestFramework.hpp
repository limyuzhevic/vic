// Modern test infrastructure for NLM
// Comprehensive testing framework with Google Test integration

#pragma once

#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <random>
#include <chrono>
#include "src/core/Config/Config.hpp"
#include "src/brain/Brain.hpp"
#include "src/brain/Neuron.hpp"
#include "src/brain/Synapse.hpp"
#include "src/plasticity/STDP.hpp"
#include "src/plasticity/Hebbian.hpp"
#include "src/core/Random/Random.hpp"
#include "src/core/SimulationClock/SimulationClock.hpp"

namespace nlm {
namespace test {

// Test utilities and macros
#define EXPECT_FLOAT_EQ_APPROX(val1, val2, epsilon) \
    EXPECT_NEAR(val1, val2, epsilon)

#define ASSERT_FLOAT_EQ_APPROX(val1, val2, epsilon) \
    ASSERT_NEAR(val1, val2, epsilon)

#define EXPECT_VECTOR_SIZE(container, expected_size) \
    EXPECT_EQ((container).size(), expected_size)

#define ASSERT_VECTOR_SIZE(container, expected_size) \
    ASSERT_EQ((container).size(), expected_size)

#define EXPECT_GREATER_THAN(val1, val2) \
    EXPECT_GT(val1, val2)

#define ASSERT_GREATER_THAN(val1, val2) \
    ASSERT_GT(val1, val2)

#define EXPECT_LESS_THAN(val1, val2) \
    EXPECT_LT(val1, val2)

#define ASSERT_LESS_THAN(val1, val2) \
    ASSERT_LT(val1, val2)

// Custom matcher for float comparison with tolerance
template <typename T>
class FloatNearMatcher {
public:
    FloatNearMatcher(T expected, T epsilon) : expected_(expected), epsilon_(epsilon) {}

    template <typename Actual>
    bool MatchAndExplain(Actual actual, ::testing::MatchResultListener* listener) const {
        bool match = std::abs(actual - expected_) <= epsilon_;
        if (!match) {
            *listener << "Expected value " << expected_ 
                     << " within " << epsilon_ 
                     << " of " << actual;
        }
        return match;
    }

private:
    T expected_;
    T epsilon_;
};

inline FloatNearMatcher<float> FloatNear(float expected, float epsilon = 1e-6f) {
    return FloatNearMatcher<float>(expected, epsilon);
}

// Test fixture base class with common setup/teardown
class NLMTestBase : public ::testing::Test {
protected:
    std::shared_ptr<Config> config;
    std::unique_ptr<RandomGenerator> rng;
    
    NLMTestBase() : rng(std::make_unique<RandomGenerator>(42)) {
        // Create default config for testing
        config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
        config->set("connection_probability", 0.1, ConfigSource::Default);
    }
    
    ~NLMTestBase() override {}
    
    virtual void SetUp() override {
        // Additional setup can be added by derived classes
    }
    
    virtual void TearDown() override {
        // Cleanup resources
    }
    
    // Common test utilities
    Neuron CreateTestNeuron(NeuronId id) {
        return Neuron(id);
    }
    
    Synapse CreateTestSynapse(SynapseId id, NeuronId source, NeuronId dest) {
        return Synapse(id, source, dest);
    }
    
    void SeedRandom(uint32_t seed) {
        rng->seed(seed);
    }
    
    float RandomFloat(float min = 0.0f, float max = 1.0f) {
        return rng->uniformReal(min, max);
    }
    
    int RandomInt(int min, int max) {
        return rng->uniformInt(min, max);
    }
};

// Brain system test fixture
class BrainTestFixture : public NLMTestBase {
protected:
    std::unique_ptr<Brain> brain;
    
    void SetUp() override {
        NLMTestBase::SetUp();
        brain = std::make_unique<Brain>(config);
        ASSERT_TRUE(brain->initialize());
    }
    
    void TearDown() override {
        brain.reset();
        NLMTestBase::TearDown();
    }
};

// Neuron test fixture
class NeuronTestFixture : public NLMTestBase {
protected:
    std::vector<Neuron> neurons;
    
    void SetUp() override {
        NLMTestBase::SetUp();
        // Create test neurons
        for (int i = 0; i < 10; ++i) {
            neurons.push_back(CreateTestNeuron(NeuronId(i + 1)));
        }
    }
    
    void TearDown() override {
        neurons.clear();
        NLMTestBase::TearDown();
    }
}; 

// Synapse test fixture
class SynapseTestFixture : public NLMTestBase {
protected:
    std::vector<Synapse> synapses;
    
    void SetUp() override {
        NLMTestBase::SetUp();
        // Create test synapses
        for (int i = 0; i < 5; ++i) {
            synapses.push_back(CreateTestSynapse(
                SynapseId(i + 1),
                NeuronId(i + 1),
                NeuronId(i + 2)
            ));
        }
    }
    
    void TearDown() override {
        synapses.clear();
        NLMTestBase::TearDown();
    }
};

// STDP test fixture
class STDPTestFixture : public NLMTestBase {
protected:
    std::unique_ptr<STDP> stdp;
    
    void SetUp() override {
        NLMTestBase::SetUp();
        stdp = std::make_unique<STDP>();
    }
    
    void TearDown() override {
        stdp.reset();
        NLMTestBase::TearDown();
    }
};

// Performance testing utilities
class PerformanceTest {
public:
    static double MeasureExecutionTime(std::function<void()> func, int iterations = 100) {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i) {
            func();
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        return duration.count() / iterations; // microseconds per iteration
    }
    
    static void StressTest(std::function<void()> func, 
                          int threadCount = 4, 
                          int iterationsPerThread = 100) {
        // Implementation for stress testing would go here
        // This is a placeholder for future implementation
    }
};

// Benchmark macros for performance testing
#define TIME_FUNCTION(func, iterations) \
    PerformanceTest::MeasureExecutionTime(func, iterations)

} // namespace test
} // namespace nlm
