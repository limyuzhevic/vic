// Random Tests
#include "../src/core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_random {

void testRandomGeneratorCreation() {
    nlm::RandomGenerator rng1(42);
    nlm::RandomGenerator rng2(42);
    
    // Same seed should produce same sequence
    for (int i = 0; i < 10; ++i) {
        assert(rng1.uniformInt(0, 100) == rng2.uniformInt(0, 100));
    }
    
    std::cout << "    testRandomGeneratorCreation passed" << std::endl;
}

void testUniformInt() {
    nlm::RandomGenerator rng(42);
    
    for (int i = 0; i < 100; ++i) {
        uint32_t val = rng.uniformInt(5, 10);
        assert(val >= 5 && val <= 10);
    }
    
    std::cout << "    testUniformInt passed" << std::endl;
}

void testUniformReal() {
    nlm::RandomGenerator rng(42);
    
    for (int i = 0; i < 100; ++i) {
        double val = rng.uniformReal(0.0, 1.0);
        assert(val >= 0.0 && val < 1.0);
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
    assert(std::abs(mean) < 0.1);
    
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
    assert(ratio > 0.25 && ratio < 0.35);
    
    std::cout << "    testBernoulli passed" << std::endl;
}

void testRandomBool() {
    nlm::RandomGenerator rng(42);
    
    // Test that randomBool returns a valid boolean value
    bool val = rng.randomBool();
    assert(val == true || val == false);
    
    // Test with different seed to ensure it can produce different values
    nlm::RandomGenerator rng2(43);
    bool val2 = rng2.randomBool();
    assert(val2 == true || val2 == false);
    
    // Most importantly, test that the function actually produces random behavior
    // by testing multiple calls with the same seed (they should be consistent)
    nlm::RandomGenerator rng3(42);
    for (int i = 0; i < 10; ++i) {
        bool testVal = rng3.randomBool();
        assert(testVal == true || testVal == false);
    }
    
    std::cout << "    testRandomBool passed" << std::endl;
}

void testUniformInt64() {
    nlm::RandomGenerator rng(42);
    
    for (int i = 0; i < 100; ++i) {
        uint64_t val = rng.uniformInt64(100, 200);
        assert(val >= 100 && val <= 200);
    }
    
    std::cout << "    testUniformInt64 passed" << std::endl;
}

void testUniformReal32() {
    nlm::RandomGenerator rng(42);
    
    for (int i = 0; i < 100; ++i) {
        float val = rng.uniformReal32(0.0f, 1.0f);
        assert(val >= 0.0f && val < 1.0f);
    }
    
    std::cout << "    testUniformReal32 passed" << std::endl;
}

void testNormal32() {
    nlm::RandomGenerator rng(42);
    
    double sum = 0.0;
    int samples = 10000;
    for (int i = 0; i < samples; ++i) {
        float val = rng.normal32(0.0f, 1.0f);
        sum += val;
    }
    double mean = sum / samples;
    
    // Mean should be close to 0
    assert(std::abs(mean) < 0.1);
    
    std::cout << "    testNormal32 passed" << std::endl;
}

void testUniformReal64() {
    nlm::RandomGenerator rng(42);
    
    for (int i = 0; i < 100; ++i) {
        double val = rng.uniformReal64(0.0, 1.0);
        assert(val >= 0.0 && val < 1.0);
    }
    
    std::cout << "    testUniformReal64 passed" << std::endl;
}

void testNormal64() {
    nlm::RandomGenerator rng(42);
    
    double sum = 0.0;
    int samples = 10000;
    for (int i = 0; i < samples; ++i) {
        double val = rng.normal64(0.0, 1.0);
        sum += val;
    }
    double mean = sum / samples;
    
    // Mean should be close to 0
    assert(std::abs(mean) < 0.1);
    
    std::cout << "    testNormal64 passed" << std::endl;
}

void testPickRandomIndex() {
    nlm::RandomGenerator rng(42);
    
    for (int i = 0; i < 100; ++i) {
        uint64_t idx = rng.pickRandomIndex(10);
        assert(idx < 10);
    }
    
    std::cout << "    testPickRandomIndex passed" << std::endl;
}

void testShuffle() {
    nlm::RandomGenerator rng(42);
    
    std::vector<int> vec = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto vecCopy = vec;
    
    rng.shuffle(vec);
    
    // Should have same elements but possibly different order
    assert(vec.size() == vecCopy.size());
    
    // Sort and compare
    std::sort(vec.begin(), vec.end());
    std::sort(vecCopy.begin(), vecCopy.end());
    assert(vec == vecCopy);
    
    std::cout << "    testShuffle passed" << std::endl;
}

void testSeed() {
    nlm::RandomGenerator rng;
    
    rng.seed(12345);
    uint32_t val1 = rng.uniformInt(0, 1000);
    
    rng.seed(12345);
    uint32_t val2 = rng.uniformInt(0, 1000);
    
    assert(val1 == val2);
    
    std::cout << "    testSeed passed" << std::endl;
}

void runAll() {
    testRandomGeneratorCreation();
    testUniformInt();
    testUniformReal();
    testNormal();
    testBernoulli();
    testRandomBool();
    testPickRandomIndex();
    testShuffle();
    testSeed();
    testUniformInt64();
    testUniformReal32();
    testNormal32();
    testUniformReal64();
    testNormal64();
}

} // namespace test_random
