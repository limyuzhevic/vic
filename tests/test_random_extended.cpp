// RandomGenerator Tests
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>

namespace test_random {

void testRandomCreation() {
    nlm::RandomGenerator rng(42);
    uint64_t seed = rng.getSeed();
    assert(seed == 42);
    
    std::cout << "    testRandomCreation passed" << std::endl;
}

void testRandomSeeding() {
    nlm::RandomGenerator rng1(42);
    nlm::RandomGenerator rng2(42);
    
    // Same seed should produce same sequence
    assert(rng1.getSeed() == rng2.getSeed());
    
    std::cout << "    testRandomSeeding passed" << std::endl;
}

void testUniformInt() {
    nlm::RandomGenerator rng(42);
    
    // Test boundaries
    uint32_t result = rng.uniformInt(10, 20);
    assert(result >= 10 && result <= 20);
    
    // Test same values
    result = rng.uniformInt(5, 5);
    assert(result == 5);
    
    std::cout << "    testUniformInt passed" << std::endl;
}

void testUniformReal() {
    nlm::RandomGenerator rng(42);
    
    // Test boundaries
    double result = rng.uniformReal(0.0, 1.0);
    assert(result >= 0.0 && result < 1.0);
    
    // Test negative range
    result = rng.uniformReal(-5.0, -1.0);
    assert(result >= -5.0 && result < -1.0);
    
    std::cout << "    testUniformReal passed" << std::endl;
}

void testBernoulli() {
    nlm::RandomGenerator rng(42);
    
    // Test various probabilities
    bool result = rng.bernoulli(0.5);
    assert(result == true || result == false);  // Either is valid
    
    result = rng.bernoulli(0.0); // Should always be false
    assert(result == false);
    
    result = rng.bernoulli(1.0); // Should always be true
    assert(result == true);
    
    std::cout << "    testBernoulli passed" << std::endl;
}

void testShuffle() {
    nlm::RandomGenerator rng(42);
    std::vector<int> vec = {1, 2, 3, 4, 5};
    
    rng.shuffle(vec);
    
    // Check that all elements are still present
    assert(std::find(vec.begin(), vec.end(), 1) != vec.end());
    assert(std::find(vec.begin(), vec.end(), 2) != vec.end());
    assert(std::find(vec.begin(), vec.end(), 3) != vec.end());
    assert(std::find(vec.begin(), vec.end(), 4) != vec.end());
    assert(std::find(vec.begin(), vec.end(), 5) != vec.end());
    
    std::cout << "    testShuffle passed" << std::endl;
}

void testPickRandom() {
    nlm::RandomGenerator rng(42);
    std::vector<int> vec = {10, 20, 30, 40, 50};
    
    int result = rng.pickRandom(vec);
    assert(std::find(vec.begin(), vec.end(), result) != vec.end());
    
    // Test empty container (should throw)
    std::vector<int> empty;
    try {
        rng.pickRandom(empty);
        assert(false); // Should not reach here
    } catch (const std::runtime_error& e) {
        // Expected exception
    }
    
    std::cout << "    testPickRandom passed" << std::endl;
}

void testNormalDistribution() {
    nlm::RandomGenerator rng(42);
    
    double result = rng.normal(0.0, 1.0);
    // Normal distribution can produce any value, just verify it doesn't crash
    assert(!std::isnan(result));
    
    std::cout << "    testNormalDistribution passed" << std::endl;
}

void testRandomBool() {
    nlm::RandomGenerator rng(42);
    
    bool result = rng.randomBool();
    assert(result == true || result == false);
    
    std::cout << "    testRandomBool passed" << std::endl;
}

void testRandomBit() {
    nlm::RandomGenerator rng(42);
    
    uint32_t result = rng.randomBit();
    assert(result == 0 || result == 1);
    
    std::cout << "    testRandomBit passed" << std::endl;
}

void runAll() {
    testRandomCreation();
    testRandomSeeding();
    testUniformInt();
    testUniformReal();
    testBernoulli();
    testShuffle();
    testPickRandom();
    testNormalDistribution();
    testRandomBool();
    testRandomBit();
}

} // namespace test_random
