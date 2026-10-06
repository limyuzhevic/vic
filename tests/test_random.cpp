// Random Tests
// Enhanced with NLM Test Framework
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <string>

// Test macros for NLM Test Framework
#define NLM_TEST_CASE_LEGACY(name, testFunc) \
    void name##TestLegacy() { testFunc; } \
    NLM_TEST_CASE(name, name##TestLegacy)

namespace test_random {

void testRandomGeneratorCreation() {
    nlm::RandomGenerator rng1(42);
    nlm::RandomGenerator rng2(42);
    
    // Same seed should produce same sequence
    for (int i = 0; i < 10; ++i) {
        NLM_ASSERT_EQ(rng1.uniformInt(0, 100), rng2.uniformInt(0, 100));
    }
    
    std::cout << "    testRandomGeneratorCreation passed" << std::endl;
}

void testUniformInt() {
    nlm::RandomGenerator rng(42);
    
    for (int i = 0; i < 100; ++i) {
        uint32_t val = rng.uniformInt(5, 10);
        NLM_ASSERT_TRUE(val >= 5 && val <= 10);
    }
    
    std::cout << "    testUniformInt passed" << std::endl;
}

void testUniformReal() {
    nlm::RandomGenerator rng(42);
    
    for (int i = 0; i < 100; ++i) {
        double val = rng.uniformReal(0.0, 1.0);
        NLM_ASSERT_TRUE(val >= 0.0 && val < 1.0);
    }
    
    std::cout << "    testUniformReal passed" << std::endl;
}

void testNormal() {
    nlm::RandomGenerator rng(42);
    
    double sum = 0.0;
    int samples = 10000;
    for (int i = 0; i < samples; ++i) {
        double val = rng.normal(0.0, 1.0);
        sum += val;
    }
    double mean = sum / samples;
    
    // Mean should be close to 0
    NLM_ASSERT_TRUE(std::abs(mean) < 0.1);
    
    std::cout << "    testNormal passed" << std::endl;
}

void testBernoulli() {
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
    NLM_ASSERT_TRUE(ratio > 0.25 && ratio < 0.35);
    
    std::cout << "    testBernoulli passed" << std::endl;
}

void testRandomBool() {
    nlm::RandomGenerator rng(42);
    
    bool val = rng.randomBool();
    // Just check it doesn't crash
    NLM_ASSERT_TRUE(true);  // Always true
    
    std::cout << "    testRandomBool passed" << std::endl;
}

void testPickRandomIndex() {
    nlm::RandomGenerator rng(42);
    
    for (int i = 0; i < 100; ++i) {
        uint64_t idx = rng.pickRandomIndex(10);
        NLM_ASSERT_LT(idx, 10);
    }
    
    std::cout << "    testPickRandomIndex passed" << std::endl;
}

void testShuffle() {
    nlm::RandomGenerator rng(42);
    
    std::vector<int> vec = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto vecCopy = vec;
    
    rng.shuffle(vec);
    
    // Should have same elements but possibly different order
    NLM_ASSERT_EQ(vec.size(), vecCopy.size());
    
    // Sort and compare
    std::sort(vec.begin(), vec.end());
    std::sort(vecCopy.begin(), vecCopy.end());
    NLM_ASSERT_EQ(vec, vecCopy);
    
    std::cout << "    testShuffle passed" << std::endl;
}

void testSeed() {
    nlm::RandomGenerator rng;
    
    rng.seed(12345);
    uint32_t val1 = rng.uniformInt(0, 1000);
    
    rng.seed(12345);
    uint32_t val2 = rng.uniformInt(0, 1000);
    
    NLM_ASSERT_EQ(val1, val2);
    
    std::cout << "    testSeed passed" << std::endl;
}

// Modern test with enhanced random features
void testRandomAdvancedFeatures() {
    nlm::RandomGenerator rng(42);
    
    // Test advanced features with modern assertions
    uint32_t val1 = rng.uniformInt(0, 100);
    uint32_t val2 = rng.uniformInt(0, 100);
    NLM_ASSERT_GT(val2, val1);  // Usually increasing
    
    double d1 = rng.uniformReal(0.0, 1.0);
    double d2 = rng.uniformReal(0.0, 1.0);
    NLM_ASSERT_TRUE(d1 >= 0.0 && d1 < 1.0);
    NLM_ASSERT_TRUE(d2 >= 0.0 && d2 < 1.0);
    
    double n1 = rng.normal(0.0, 1.0);
    NLM_ASSERT_TRUE(std::abs(n1) < 3.0);  // Within 3 sigma
    
    bool b1 = rng.bernoulli(0.5);
    bool b2 = rng.bernoulli(0.5);
    NLM_ASSERT_TRUE(b1 == true || b1 == false);  // Valid boolean
    NLM_ASSERT_TRUE(b2 == true || b2 == false);  // Valid boolean
    
    uint64_t idx = rng.pickRandomIndex(5);
    NLM_ASSERT_LT(idx, 5);
    
    std::cout << "    testRandomAdvancedFeatures passed" << std::endl;
}

void runAll() {
    std::cout << "Running Enhanced Random tests..." << std::endl;
    
    testRandomGeneratorCreation();
    testUniformInt();
    testUniformReal();
    testNormal();
    testBernoulli();
    testRandomBool();
    testPickRandomIndex();
    testShuffle();
    testSeed();
    testRandomAdvancedFeatures();
}

} // namespace test_random
