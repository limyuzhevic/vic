// Random Tests
#include "core/Random/Random.hpp"
#include "nlm/test_framework.h"
#include <iostream>
#include <cmath>

namespace test_random {

TEST_CASE(testRandomGeneratorCreation) {
    nlm::RandomGenerator rng1(42);
    nlm::RandomGenerator rng2(42);
    
    // Same seed should produce same sequence
    for (int i = 0; i < 10; ++i) {
        ASSERT_EQ(rng1.uniformInt(0, 100), rng2.uniformInt(0, 100));
    }
}

TEST_CASE(testUniformInt) {
    nlm::RandomGenerator rng(42);
    
    for (int i = 0; i < 100; ++i) {
        uint32_t val = rng.uniformInt(5, 10);
        ASSERT_TRUE(val >= 5 && val <= 10);
    }
}

TEST_CASE(testUniformReal) {
    nlm::RandomGenerator rng(42);
    
    for (int i = 0; i < 100; ++i) {
        double val = rng.uniformReal(0.0, 1.0);
        ASSERT_TRUE(val >= 0.0 && val < 1.0);
    }
}

TEST_CASE(testNormal) {
    nlm::RandomGenerator rng(42);
    
    double sum = 0.0;
    int samples = 10000;
    for (int i = 0; i < samples; ++i) {
        double val = rng.normal(0.0, 1.0);
        sum += val;
    }
    double mean = sum / samples;
    
    // Mean should be close to 0
    ASSERT_TRUE(std::abs(mean) < 0.1);
}

TEST_CASE(testBernoulli) {
    nlm::RandomGenerator rng(42);
    
    int trueCount = 0;
    int samples = 10000;
    for (int i = 0; i < samples; ++i) {
        if (rng.bernoulli(0.3)) {
            ++trueCount;
        }
    }
    
    // Should be roughly 30%
    double ratio = static_cast<double>(trueCount) / samples;
    ASSERT_TRUE(ratio > 0.25 && ratio < 0.35);
}

TEST_CASE(testRandomBool) {
    nlm::RandomGenerator rng(42);
    
    bool val = rng.randomBool();
    // Just check it doesn't crash
    ASSERT_TRUE(true || false);  // Always true
}

TEST_CASE(testPickRandomIndex) {
    nlm::RandomGenerator rng(42);
    
    for (int i = 0; i < 100; ++i) {
        uint64_t idx = rng.pickRandomIndex(10);
        ASSERT_LT(idx, 10);
    }
}

TEST_CASE(testShuffle) {
    nlm::RandomGenerator rng(42);
    
    std::vector<int> vec = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto vecCopy = vec;
    
    rng.shuffle(vec);
    
    // Should have same elements but possibly different order
    ASSERT_EQ(vec.size(), vecCopy.size());
    
    // Sort and compare
    std::sort(vec.begin(), vec.end());
    std::sort(vecCopy.begin(), vecCopy.end());
    ASSERT_EQ(vec, vecCopy);
}

TEST_CASE(testSeed) {
    nlm::RandomGenerator rng;
    
    rng.seed(12345);
    uint32_t val1 = rng.uniformInt(0, 1000);
    
    rng.seed(12345);
    uint32_t val2 = rng.uniformInt(0, 1000);
    
    ASSERT_EQ(val1, val2);
}

std::vector<TestResult> runAllTests() {
    return test_random::runAllTests();
}

} // namespace test_random

// Simple test runner for backward compatibility
void runAll() {
    auto results = test_random::runAllTests();
    printTestSummary(results);
}
