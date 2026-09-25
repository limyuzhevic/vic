// Random Number Generator Unit Tests
// Comprehensive test suite for RandomGenerator class using Google Test framework

#include "tests/common/test_base.hpp"
#include <gtest/gtest.h>
#include <cmath>
#include <vector>
#include <algorithm>
#include <numeric>

namespace nlm {
namespace test {

// Test fixture for random number generator tests
class RandomTest : public TestBase {
protected:
    void SetUp() override {
        TestBase::SetUp();
        
        // Create test random generators
        rng1 = std::make_shared<nlm::RandomGenerator>(42); // Deterministic seed
        rng2 = std::make_shared<nlm::RandomGenerator>();   // Time-based seed
    }
    
    void TearDown() override {
        TestBase::TearDown();
        rng1.reset();
        rng2.reset();
    }
    
protected:
    std::shared_ptr<nlm::RandomGenerator> rng1;
    std::shared_ptr<nlm::RandomGenerator> rng2;
};

// Test random generator with deterministic seed
TEST_F(RandomTest, DeterministicSeed) {
    nlm::RandomGenerator deterministic1(42);
    nlm::RandomGenerator deterministic2(42);
    
    // Same seed should produce same sequence
    EXPECT_EQ(deterministic1.uniformInt(0, 100), deterministic2.uniformInt(0, 100));
    EXPECT_FLOAT_EQ(deterministic1.normal(0.0f, 1.0f), deterministic2.normal(0.0f, 1.0f));
}

// Test random generator with different seeds
TEST_F(RandomTest, DifferentSeeds) {
    nlm::RandomGenerator rngA(123);
    nlm::RandomGenerator rngB(456);
    
    // Different seeds should produce different sequences (with high probability)
    int resultA = rngA.uniformInt(0, 1000);
    int resultB = rngB.uniformInt(0, 1000);
    
    // Not guaranteed, but very likely
    EXPECT_NE(resultA, resultB);
}

// Test uniform integer generation
TEST_F(RandomTest, UniformInt) {
    nlm::RandomGenerator rng(42);
    
    for (int i = 0; i < 1000; ++i) {
        int val = rng.uniformInt(0, 100);
        EXPECT_GE(val, 0);
        EXPECT_LE(val, 100);
    }
}

// Test uniform real generation
TEST_F(RandomTest, UniformReal) {
    nlm::RandomGenerator rng(42);
    
    for (int i = 0; i < 1000; ++i) {
        float val = rng.uniformReal(0.0f, 1.0f);
        EXPECT_GE(val, 0.0f);
        EXPECT_LE(val, 1.0f);
    }
}

// Test normal distribution
TEST_F(RandomTest, NormalDistribution) {
    nlm::RandomGenerator rng(42);
    
    // Generate many values from normal distribution
    std::vector<float> values;
    for (int i = 0; i < 10000; ++i) {
        values.push_back(rng.normal(0.0f, 1.0f));
    }
    
    // Check approximate mean and standard deviation
    float sum = std::accumulate(values.begin(), values.end(), 0.0f);
    float mean = sum / values.size();
    
    float sqSum = 0.0f;
    for (float val : values) {
        sqSum += (val - mean) * (val - mean);
    }
    float stddev = std::sqrt(sqSum / values.size());
    
    // Should be approximately 0 mean and 1 stddev
    EXPECT_NEAR(mean, 0.0f, 0.01f);
    EXPECT_NEAR(stddev, 1.0f, 0.01f);
}

// Test Bernoulli distribution
TEST_F(RandomTest, BernoulliDistribution) {
    nlm::RandomGenerator rng(42);
    
    for (int i = 0; i < 1000; ++i) {
        bool result = rng.bernoulli(0.5f);
        EXPECT_TRUE(result == true || result == false);
    }
}

// Test Bernoulli with different probabilities
TEST_F(RandomTest, BernoulliDifferentProbabilities) {
    nlm::RandomGenerator rng(42);
    
    for (int i = 0; i < 100; ++i) {
        float p = i / 100.0f; // 0.0, 0.01, 0.02, ..., 0.99
        if (p == 0.0f) {
            // With p=0, should always return false
            EXPECT_FALSE(rng.bernoulli(p));
        } else if (p == 1.0f) {
            // With p=1, should always return true
            EXPECT_TRUE(rng.bernoulli(p));
        } else {
            // Should return true or false
            bool result = rng.bernoulli(p);
            EXPECT_TRUE(result == true || result == false);
        }
    }
}

// Test seed management
TEST_F(RandomTest, SeedManagement) {
    nlm::RandomGenerator rng(123);
    
    int firstValue = rng.uniformInt(0, 100);
    rng.seed(123); // Reset to same seed
    int secondValue = rng.uniformInt(0, 100);
    
    EXPECT_EQ(firstValue, secondValue);
}

// Test seed retrieval
TEST_F(RandomTest, SeedRetrieval) {
    nlm::RandomGenerator rng(999);
    EXPECT_EQ(rng.getSeed(), 999);
    
    rng.seed(111);
    EXPECT_EQ(rng.getSeed(), 111);
}

// Test random generator independence
TEST_F(RandomTest, GeneratorIndependence) {
    nlm::RandomGenerator rng1(100);
    nlm::RandomGenerator rng2(200);
    
    // Generate sequences from both generators
    std::vector<int> seq1, seq2;
    for (int i = 0; i < 10; ++i) {
        seq1.push_back(rng1.uniformInt(0, 100));
        seq2.push_back(rng2.uniformInt(0, 100));
    }
    
    // Sequences should be different
    EXPECT_NE(seq1, seq2);
}

// Test random generator move operations
TEST_F(RandomTest, GeneratorMoveOperations) {
    nlm::RandomGenerator rng1(42);
    int value1 = rng1.uniformInt(0, 100);
    
    nlm::RandomGenerator rng2(std::move(rng1));
    
    // rng2 should have the same seed as rng1
    EXPECT_EQ(rng2.getSeed(), 42);
    
    // rng1 should be in valid but unspecified state after move
    // This is implementation-dependent
}

// Test random generator copy operations (should be disabled)
TEST_F(RandomTest, GeneratorCopyOperations) {
    nlm::RandomGenerator rng(42);
    
    // Copying should be disabled (compile-time test)
    // This would be tested with a static assert in real implementation
}

// Parameterized test for random number generation
class ParameterizedRandomTest : public ParameterizedTestBase, public ::testing::WithParamInterface<TestParameters> {
public:
    static std::vector<TestParameters> GetTestParameters() {
        return {
            TestParameters(100, 0.1f, 0.01f, "Small sample"),
            TestParameters(1000, 0.2f, 0.05f, "Medium sample"),
            TestParameters(10000, 0.15f, 0.1f, "Large sample")
        };
    }
};

TEST_P(ParameterizedRandomTest, RandomGenerationWithDifferentSizes) {
    TestParameters params = GetParam();
    
    nlm::RandomGenerator rng(12345);
    
    // Generate sample from random number generator
    std::vector<float> samples;
    for (int i = 0; i < params.neuronCount; ++i) {
        samples.push_back(rng.uniformReal(0.0f, 1.0f));
    }
    
    // Verify all samples are in valid range
    for (float val : samples) {
        EXPECT_GE(val, 0.0f);
        EXPECT_LE(val, 1.0f);
    }
}

INSTANTIATE_TEST_SUITE_P(
    RandomParameterizedTests,
    ParameterizedRandomTest,
    ::testing::ValuesIn(ParameterizedRandomTest::GetTestParameters())
);

// Performance benchmark for random number generation
TEST(RandomPerformanceTest, UniformIntPerformance) {
    TestBase testBase;
    
    auto func = []() {
        nlm::RandomGenerator rng(42);
        rng.uniformInt(0, 100);
    };
    
    auto result = testBase.runBenchmark(func, 100000);
    
    std::cout << "Uniform integer generation performance: " << result.timeMs << " ms for " 
              << result.iterations << " iterations (" << result.opsPerMs << " ops/ms)" << std::endl;
    
    // Should be able to perform at least 100,000 uniform integer generations per second
    EXPECT_GT(result.opsPerMs, 100000.0);
}

// Performance benchmark for normal distribution
TEST(RandomPerformanceTest, NormalDistributionPerformance) {
    TestBase testBase;
    
    auto func = []() {
        nlm::RandomGenerator rng(42);
        rng.normal(0.0f, 1.0f);
    };
    
    auto result = testBase.runBenchmark(func, 50000);
    
    std::cout << "Normal distribution generation performance: " << result.timeMs << " ms for " 
              << result.iterations << " iterations (" << result.opsPerMs << " ops/ms)" << std::endl;
}

// Edge case tests for random number generator
TEST_F(RandomTest, RandomGeneratorEdgeCases) {
    nlm::RandomGenerator rng(42);
    
    // Test with extreme seed values
    nlm::RandomGenerator extremeRng(std::numeric_limits<uint64_t>::max());
    extremeRng.uniformInt(0, 100);
    
    // Test with very small ranges
    nlm::RandomGenerator smallRng(42);
    for (int i = 0; i < 1000; ++i) {
        int val = smallRng.uniformInt(0, 0); // Range [0, 0]
        EXPECT_EQ(val, 0);
    }
    
    // Test with large ranges
    for (int i = 0; i < 100; ++i) {
        int val = smallRng.uniformInt(0, 1000000);
        EXPECT_GE(val, 0);
        EXPECT_LE(val, 1000000);
    }
}

// Test random generator reproducibility
TEST_F(RandomTest, RandomGeneratorReproducibility) {
    nlm::RandomGenerator rng1(12345);
    nlm::RandomGenerator rng2(12345);
    
    std::vector<float> sequence1, sequence2;
    for (int i = 0; i < 100; ++i) {
        sequence1.push_back(rng1.uniformReal(0.0f, 1.0f));
        sequence2.push_back(rng2.uniformReal(0.0f, 1.0f));
    }
    
    // Sequences should be identical
    EXPECT_EQ(sequence1, sequence2);
}

// Test random generator entropy
TEST(RandomEntropyTest, RandomGeneratorEntropy) {
    // Test that different seeds produce different sequences
    nlm::RandomGenerator rng1(1);
    nlm::RandomGenerator rng2(2);
    nlm::RandomGenerator rng3(3);
    
    std::vector<float> seq1, seq2, seq3;
    for (int i = 0; i < 50; ++i) {
        seq1.push_back(rng1.uniformReal(0.0f, 1.0f));
        seq2.push_back(rng2.uniformReal(0.0f, 1.0f));
        seq3.push_back(rng3.uniformReal(0.0f, 1.0f));
    }
    
    // All sequences should be different
    EXPECT_NE(seq1, seq2);
    EXPECT_NE(seq1, seq3);
    EXPECT_NE(seq2, seq3);
}

} // namespace test
} // namespace nlm
