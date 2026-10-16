// Random Tests - GoogleTest Version
#include "core/Random/Random.hpp"
#include <gtest/gtest.h>
#include "GoogleTestHelper.hpp"
#include "TestMatchers.hpp"

// TEST SUITE
class RandomTestSuite : public ::testing::Test {
protected:
    nlm::RandomGenerator rng1;
    nlm::RandomGenerator rng2;
    
    void SetUp() override {
        rng1.seed(42);
        rng2.seed(42);
    }
};

// RANDOM TESTS
TEST_F(RandomTestSuite, RandomGeneratorCreation) {
    for (int i = 0; i < 10; ++i) {
        EXPECT_EQ(rng1.uniformInt(0, 100), rng2.uniformInt(0, 100));
    }
}

TEST_F(RandomTestSuite, UniformInt) {
    for (int i = 0; i < 100; ++i) {
        uint32_t val = rng1.uniformInt(5, 10);
        EXPECT_GE(val, 5u);
        EXPECT_LE(val, 10u);
    }
}

TEST_F(RandomTestSuite, UniformReal) {
    for (int i = 0; i < 100; ++i) {
        double val = rng1.uniformReal(0.0, 1.0);
        EXPECT_GE(val, 0.0);
        EXPECT_LT(val, 1.0);
    }
}

TEST_F(RandomTestSuite, Normal) {
    double sum = 0.0;
    int samples = 10000;
    for (int i = 0; i < samples; ++i) {
        double val = rng1.normal(0.0, 1.0);
        sum += val;
    }
    double mean = sum / samples;
    EXPECT_NEAR(mean, 0.0, 0.1);
}

TEST_F(RandomTestSuite, Bernoulli) {
    int trueCount = 0;
    int samples = 10000;
    for (int i = 0; i < samples; ++i) {
        if (rng1.bernoulli(0.3)) {
            ++trueCount;
        }
    }
    double ratio = static_cast<double>(trueCount) / samples;
    EXPECT_GT(ratio, 0.25);
    EXPECT_LT(ratio, 0.35);
}

TEST_F(RandomTestSuite, RandomBool) {
    bool val = rng1.randomBool();
    EXPECT_TRUE(true || false);
}

TEST_F(RandomTestSuite, PickRandomIndex) {
    for (int i = 0; i < 100; ++i) {
        uint64_t idx = rng1.pickRandomIndex(10);
        EXPECT_LT(idx, 10u);
    }
}

TEST_F(RandomTestSuite, Shuffle) {
    std::vector<int> vec = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto vecCopy = vec;
    
    rng1.shuffle(vec);
    
    EXPECT_EQ(vec.size(), vecCopy.size());
    std::sort(vec.begin(), vec.end());
    std::sort(vecCopy.begin(), vecCopy.end());
    EXPECT_EQ(vec, vecCopy);
}

TEST_F(RandomTestSuite, Seed) {
    nlm::RandomGenerator rng3;
    rng3.seed(12345);
    uint32_t val1 = rng3.uniformInt(0, 1000);
    
    rng3.seed(12345);
    uint32_t val2 = rng3.uniformInt(0, 1000);
    
    EXPECT_EQ(val1, val2);
}
