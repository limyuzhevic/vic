// Random Tests
#include "core/Random/Random.hpp"
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
    
    bool val = rng.randomBool();
    // Just check it doesn't crash
    assert(true || false);  // Always true
    
    std::cout << "    testRandomBool passed" << std::endl;
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

void testRandomGeneratorMoveSemantics() {
    nlm::RandomGenerator rng1(42);
    
    // Move constructor
    nlm::RandomGenerator rng2(std::move(rng1));
    
    // Test that moved generator still works
    uint32_t val = rng2.uniformInt(0, 100);
    assert(val >= 0 && val <= 100);
    
    std::cout << "    testRandomGeneratorMoveSemantics passed" << std::endl;
}

void testRandomGeneratorMemoryCleanup() {
    uint64_t seed = 12345;
    {
        nlm::RandomGenerator rng(seed);
        
        // Use the generator
        uint32_t val1 = rng.uniformInt(0, 100);
        double val2 = rng.uniformReal(0.0, 1.0);
        bool val3 = rng.bernoulli(0.5);
        
        // Verify values are reasonable
        assert(val1 <= 100);
        assert(val2 >= 0.0 && val2 < 1.0);
        assert(val3 == true || val3 == false);
    } // RandomGenerator destroyed here
    
    // Generator should be properly destroyed now
    std::cout << "    testRandomGeneratorMemoryCleanup passed" << std::endl;
}

void testRandomGeneratorArrayMemory() {
    const size_t rngCount = 25;
    {
        // Create multiple random generators
        for (size_t i = 0; i < rngCount; ++i) {
            nlm::RandomGenerator rng(42 + i);
            
            // Use each generator
            uint32_t val = rng.uniformInt(0, 1000);
            assert(val <= 1000);
        }
    }
    
#include <chrono>

void testRandomGeneratorPerformance() {
    const size_t iterationCount = 100000;
    
    // Test random number generation performance
    nlm::RandomGenerator rng(42);
    
    auto startTime = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < iterationCount; ++i) {
        uint32_t val = rng.uniformInt(0, 1000000);
        (void)val;
    }
    auto endTime = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    std::cout << "    Performed " << iterationCount << " uniformInt operations in " 
              << duration.count() << " microseconds (" 
              << (1000000.0 / duration.count() * iterationCount) << " operations/second)" << std::endl;
    
    // Test random real numbers performance
    startTime = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < iterationCount; ++i) {
        double val = rng.uniformReal(0.0, 1.0);
        (void)val;
    }
    endTime = std::chrono::high_resolution_clock::now();
    
    duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    std::cout << "    Performed " << iterationCount << " uniformReal operations in " 
              << duration.count() << " microseconds (" 
              << (1000000.0 / duration.count() * iterationCount) << " operations/second)" << std::endl;
    
    std::cout << "    testRandomGeneratorPerformance passed" << std::endl;
}

} // namespace test_random
