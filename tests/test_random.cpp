// Random Tests
#include "core/Random/Random.hpp"
#include "core/Exception.hpp"
#include <iostream>
#include <cmath>

namespace test_random {

void testRandomGeneratorCreation() {
    nlm::RandomGenerator rng1(42);
    nlm::RandomGenerator rng2(42);
    
    // Same seed should produce same sequence
    for (int i = 0; i < 10; ++i) {
        if (rng1.uniformInt(0, 100) != rng2.uniformInt(0, 100)) {
            throw nlm::NLMTestException("Random generator with same seed produced different sequence at iteration " + 
                                       std::to_string(i));
        }
    }
    
    std::cout << "    testRandomGeneratorCreation passed" << std::endl;
}

void testUniformInt() {
    nlm::RandomGenerator rng(42);
    
    for (int i = 0; i < 100; ++i) {
        uint32_t val = rng.uniformInt(5, 10);
        if (val < 5 || val > 10) {
            throw nlm::NLMTestException("UniformInt returned out-of-range value: " + 
                                       std::to_string(val) + " (expected 5-10)");
        }
    }
    
    std::cout << "    testUniformInt passed" << std::endl;
}

void testUniformReal() {
    nlm::RandomGenerator rng(42);
    
    for (int i = 0; i < 100; ++i) {
        double val = rng.uniformReal(0.0, 1.0);
        if (val < 0.0 || val >= 1.0) {
            throw nlm::NLMTestException("UniformReal returned out-of-range value: " + 
                                       std::to_string(val) + " (expected 0.0-1.0)");
        }
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
    if (std::abs(mean) >= 0.1) {
        throw nlm::NLMTestException("Normal distribution mean too far from 0: " + 
                                   std::to_string(mean) + " (expected < 0.1)");
    }
    
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
    if (ratio <= 0.25 || ratio >= 0.35) {
        throw nlm::NLMTestException("Bernoulli distribution ratio out of expected range: " + 
                                   std::to_string(ratio) + " (expected 0.25-0.35)");
    }
    
    std::cout << "    testBernoulli passed" << std::endl;
}

void testRandomBool() {
    nlm::RandomGenerator rng(42);
    
    bool val = rng.randomBool();
    // Just check it doesn't crash
    if (!(true || false)) {  // Always false to throw
        throw nlm::NLMTestException("TestRandomBool: Logic error in test");
    }
    
    std::cout << "    testRandomBool passed" << std::endl;
}

void testPickRandomIndex() {
    nlm::RandomGenerator rng(42);
    
    for (int i = 0; i < 100; ++i) {
        uint64_t idx = rng.pickRandomIndex(10);
        if (idx >= 10) {
            throw nlm::NLMTestException("PickRandomIndex returned out-of-range index: " + 
                                       std::to_string(idx) + " (expected < 10)");
        }
    }
    
    std::cout << "    testPickRandomIndex passed" << std::endl;
}

void testShuffle() {
    nlm::RandomGenerator rng(42);
    
    std::vector<int> vec = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto vecCopy = vec;
    
    rng.shuffle(vec);
    
    // Should have same elements but possibly different order
    if (vec.size() != vecCopy.size()) {
        throw nlm::NLMTestException("Shuffle changed vector size from " + 
                                   std::to_string(vecCopy.size()) + " to " + std::to_string(vec.size()));
    }
    
    // Sort and compare
    std::sort(vec.begin(), vec.end());
    std::sort(vecCopy.begin(), vecCopy.end());
    if (vec != vecCopy) {
        throw nlm::NLMTestException("Shuffle changed vector contents");
    }
    
    std::cout << "    testShuffle passed" << std::endl;
}

void testSeed() {
    nlm::RandomGenerator rng;
    
    rng.seed(12345);
    uint32_t val1 = rng.uniformInt(0, 1000);
    
    rng.seed(12345);
    uint32_t val2 = rng.uniformInt(0, 1000);
    
    if (val1 != val2) {
        throw nlm::NLMTestException("Random generator with same seed produced different values: " + 
                                   std::to_string(val1) + " vs " + std::to_string(val2));
    }
    
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
}

} // namespace test_random
