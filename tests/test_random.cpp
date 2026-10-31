// Random Tests
#include "nlm.h"
#include <cassert>
#include <iostream>

namespace test_random {

void testRandomGenerator() {
    nlm::RandomGenerator rng1(42);
    nlm::RandomGenerator rng2(42);
    nlm::RandomGenerator rng3(123);
    
    // Same seed should produce same sequence
    assert(rng1.getSeed() == 42);
    assert(rng2.getSeed() == 42);
    
    // Different seeds should produce different sequences
    assert(rng1.getSeed() != rng3.getSeed());
    
    std::cout << "    testRandomGenerator passed" << std::endl;
}

void testRandomUniformInt() {
    nlm::RandomGenerator rng(42);
    
    for (int i = 0; i < 10; ++i) {
        int val = rng.uniformInt(1, 100);
        assert(val >= 1 && val <= 100);
    }
    
    std::cout << "    testRandomUniformInt passed" << std::endl;
}

void testRandomUniformFloat() {
    nlm::RandomGenerator rng(42);
    
    for (int i = 0; i < 10; ++i) {
        float val = rng.uniformReal(0.0f, 1.0f);
        assert(val >= 0.0f && val <= 1.0f);
    }
    
    std::cout << "    testRandomUniformFloat passed" << std::endl;
}

void testRandomBool() {
    nlm::RandomGenerator rng(42);
    
    for (int i = 0; i < 10; ++i) {
        bool val = rng.randomBool();
        assert(val == true || val == false);
    }
    
    std::cout << "    testRandomBool passed" << std::endl;
}

void testRandomSeed() {
    nlm::RandomGenerator rng1(123);
    nlm::RandomGenerator rng2(123);
    
    // Same seed should be equal
    assert(rng1.getSeed() == rng2.getSeed());
    
    std::cout << "    testRandomSeed passed" << std::endl;
}

void runAll() {
    testRandomGenerator();
    testRandomUniformInt();
    testRandomUniformFloat();
    testRandomBool();
    testRandomSeed();
}

} // namespace test_random
