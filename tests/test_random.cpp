// Random Tests - GoogleTest Version
#include "core/Random/Random.hpp"
#include <gtest/gtest.h>
#include <cmath>

namespace {

TEST(RandomTest, RandomGeneratorCreation) {
    nlm::RandomGenerator rng1(42);
    nlm::RandomGenerator rng2(42);
    
    // Same seed should produce same sequence
    for (int i = 0; i < 10; ++i) {
        EXPECT_EQ(rng1.uniformInt(0, 100), rng2.uniformInt(0, 100));
    }
}

TEST(RandomTest, UniformInt) {
    nlm::RandomGenerator rng(42);
    
    for (int i = 0; i < 100; ++i) {
        uint32_t val = rng.uniformInt(5, 10);
        EXPECT_GE(val, 5);
        EXPECT_LE(val, 10);
    }
}

TEST(RandomTest, UniformReal) {
    nlm::RandomGenerator rng(42);
    
    for (int i = 0; i < 100; ++i) {
        double val = rng.uniformReal(0.0, 1.0);
        EXPECT_GE(val, 0.0);
        EXPECT_LT(val, 1.0);
    }
}

TEST(RandomTest, Normal) {
    nlm::RandomGenerator rng(42);
    
    double sum = 0.0;
    int samples = 10000;
    for (int i = 0; i < samples; ++i) {
        double val = rng.normal(0.0, 1.0);
        sum += val;
    }
    double mean = sum / samples;
    
    EXPECT_LT(std::abs(mean), 0.1);
}

TEST(RandomTest, Bernoulli) {
    nlm::RandomGenerator rng(42);
    
    int trueCount = 0;
    int samples = 10000;
    for (int i = 0; i < samples; ++i) {
        if (rng.bernoulli(0.3)) {
            ++trueCount;
        }
    }
    
    double ratio = static_cast<double>(trueCount) / samples;
    EXPECT_GT(ratio, 0.25);
    EXPECT_LT(ratio, 0.35);
}

TEST(RandomTest, RandomBool) {
    nlm::RandomGenerator rng(42);
    
    bool val = rng.randomBool();
    EXPECT_TRUE(val || !val);  // Just check it doesn't crash
}

TEST(RandomTest, PickRandomIndex) {
    nlm::RandomGenerator rng(42);
    
    for (int i = 0; i < 100; ++i) {
        uint64_t idx = rng.pickRandomIndex(10);
        EXPECT_LT(idx, 10);
    }
}

TEST(RandomTest, Shuffle) {
    nlm::RandomGenerator rng(42);
    
    std::vector<int> vec = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto vecCopy = vec;
    
    rng.shuffle(vec);
    
    ASSERT_EQ(vec.size(), vecCopy.size());
    
    std::sort(vec.begin(), vec.end());
    std::sort(vecCopy.begin(), vecCopy.end());
    EXPECT_EQ(vec, vecCopy);
}

TEST(RandomTest, Seed) {
    nlm::RandomGenerator rng1;
    nlm::RandomGenerator rng2;
    
    rng1.seed(12345);
    uint32_t val1 = rng1.uniformInt(0, 1000);
    
    rng2.seed(12345);
    uint32_t val2 = rng2.uniformInt(0, 1000);
    
    EXPECT_EQ(val1, val2);
}

TEST(RandomTest, RandomProperties) {
    nlm::RandomGenerator rng(42);
    
    // Test that we can generate various types
    uint32_t uintVal = rng.uniformInt(0, UINT32_MAX);
    EXPECT_LE(uintVal, UINT32_MAX);
    
    double doubleVal = rng.uniformReal(-1.0, 1.0);
    EXPECT_GE(doubleVal, -1.0);
    EXPECT_LE(doubleVal, 1.0);
    
    bool boolVal = rng.bernoulli(0.5);
    EXPECT_TRUE(boolVal || !boolVal);
}

TEST(RandomTest, RandomRangeBoundary) {
    nlm::RandomGenerator rng(42);
    
    // Test edge cases for range
    uint32_t val1 = rng.uniformInt(0, 0);
    EXPECT_EQ(val1, 0);
    
    double val2 = rng.uniformReal(1.0, 1.0);
    EXPECT_DOUBLE_EQ(val2, 1.0);
}

TEST(RandomTest, RandomDeterminism) {
    nlm::RandomGenerator rng1(999);
    nlm::RandomGenerator rng2(999);
    
    // Multiple consecutive calls should produce same sequence
    for (int i = 0; i < 20; ++i) {
        EXPECT_EQ(rng1.uniformInt(0, 1000), rng2.uniformInt(0, 1000));
    }
}

} // namespace

// Backward compatibility - keep the runAll function for existing test_main.cpp
namespace test_random {
    void runAll();
}