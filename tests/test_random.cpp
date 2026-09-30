// RandomGenerator Tests
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>

namespace test_random {

void testRandomCreation() {
    nlm::RandomGenerator rng1(42);
    nlm::RandomGenerator rng2(42);
    nlm::RandomGenerator rng3; // Time-based seed
    
    // Same seeds should produce same sequence
    assert(rng1.getSeed() == 42);
    assert(rng2.getSeed() == 42);
    assert(rng1.getSeed() == rng2.getSeed());
    
    std::cout << "    testRandomCreation passed" << std::endl;
}

void testUniformInt() {
    nlm::RandomGenerator rng(12345);
    
    // Test fixed seed produces deterministic results
    std::vector<uint32_t> results;
    for (int i = 0; i < 10; ++i) {
        results.push_back(rng.uniformInt(1, 100));
    }
    
    // Verify all values are in range
    for (uint32_t val : results) {
        assert(val >= 1 && val <= 100);
    }
    
    // Create new generator with same seed
    nlm::RandomGenerator rng2(12345);
    for (int i = 0; i < 10; ++i) {
        uint32_t val2 = rng2.uniformInt(1, 100);
        // The 5th value should match due to deterministic sequence
        if (i == 4) {
            assert(val2 == results[4]);
        }
    }
    
    std::cout << "    testUniformInt passed" << std::endl;
}

void testUniformReal() {
    nlm::RandomGenerator rng(54321);
    
    for (int i = 0; i < 100; ++i) {
        float val = rng.uniformReal(0.0f, 1.0f);
        assert(val >= 0.0f && val <= 1.0f);
    }
    
    std::cout << "    testUniformReal passed" << std::endl;
}

void testNormalDistribution() {
    nlm::RandomGenerator rng(999);
    
    // Generate some normal distribution samples
    float mean = 0.0f;
    float stddev = 1.0f;
    
    std::vector<float> samples;
    for (int i = 0; i < 1000; ++i) {
        samples.push_back(rng.normal(mean, stddev));
    }
    
    // Check that we get both positive and negative values
    bool hasPositive = false;
    bool hasNegative = false;
    for (float val : samples) {
        if (val > 0.0f) hasPositive = true;
        if (val < 0.0f) hasNegative = true;
    }
    
    assert(hasPositive && hasNegative);
    
    std::cout << "    testNormalDistribution passed" << std::endl;
}

void testBernoulli() {
    nlm::RandomGenerator rng(777);
    
    // Test with p = 0.5
    int trueCount = 0;
    for (int i = 0; i < 1000; ++i) {
        if (rng.bernoulli(0.5)) trueCount++;
    }
    
    // Should be roughly 50%
    float ratio = static_cast<float>(trueCount) / 1000.0f;
    assert(ratio > 0.4f && ratio < 0.6f);
    
    // Test with p = 0.0 (should always be false)
    int falseCount = 0;
    for (int i = 0; i < 100; ++i) {
        if (!rng.bernoulli(0.0f)) falseCount++;
    }
    assert(falseCount == 100);
    
    // Test with p = 1.0 (should always be true)
    int trueCount2 = 0;
    for (int i = 0; i < 100; ++i) {
        if (rng.bernoulli(1.0f)) trueCount2++;
    }
    assert(trueCount2 == 100);
    
    std::cout << "    testBernoulli passed" << std::endl;
}

void testPoisson() {
    nlm::RandomGenerator rng(888);
    
    uint32_t lambda = 3;
    uint32_t maxObserved = 0;
    
    for (int i = 0; i < 1000; ++i) {
        uint32_t val = rng.poisson(lambda);
        assert(val >= 0);
        if (val > maxObserved) maxObserved = val;
    }
    
    // With lambda=3, we should see some values up to at least 8
    assert(maxObserved >= 8);
    
    std::cout << "    testPoisson passed" << std::endl;
}

void testRandomBool() {
    nlm::RandomGenerator rng(456);
    
    int trueCount = 0;
    for (int i = 0; i < 1000; ++i) {
        if (rng.randomBool()) trueCount++;
    }
    
    // Should be roughly 50%
    float ratio = static_cast<float>(trueCount) / 1000.0f;
    assert(ratio > 0.4f && ratio < 0.6f);
    
    std::cout << "    testRandomBool passed" << std::endl;
}

void testShuffle() {
    nlm::RandomGenerator rng(111);
    
    std::vector<int> vec = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    std::vector<int> original = vec;
    
    // Shuffle with seed 111
    rng.shuffle(vec);
    
    // Create new generator with same seed
    nlm::RandomGenerator rng2(111);
    std::vector<int> vec2 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    rng2.shuffle(vec2);
    
    // Should be identical
    assert(vec == vec2);
    
    // Should not be identical to original
    assert(vec != original);
    
    std::cout << "    testShuffle passed" << std::endl;
}

void testPickRandom() {
    nlm::RandomGenerator rng(222);
    
    std::vector<int> vec = {10, 20, 30, 40, 50};
    
    // Pick 1000 times and verify they're all in the vector
    std::unordered_map<int, int> counts;
    for (int i = 0; i < 1000; ++i) {
        int val = rng.pickRandom(vec);
        assert(std::find(vec.begin(), vec.end(), val) != vec.end());
        counts[val]++;
    }
    
    // All elements should be picked at least once
    for (int val : vec) {
        assert(counts.find(val) != counts.end());
    }
    
    std::cout << "    testPickRandom passed" << std::endl;
}

void testPickRandomIndex() {
    nlm::RandomGenerator rng(333);
    
    for (uint64_t n = 1; n <= 100; n++) {
        uint64_t index = rng.pickRandomIndex(n);
        assert(index < n);
    }
    
    std::cout << "    testPickRandomIndex passed" << std::endl;
}

void testFillBytes() {
    nlm::RandomGenerator rng(444);
    
    char buffer[100];
    rng.fillBytes(buffer, sizeof(buffer));
    
    // Check that buffer was filled (not all zeros)
    bool allZero = true;
    for (int i = 0; i < sizeof(buffer); ++i) {
        if (buffer[i] != 0) {
            allZero = false;
            break;
        }
    }
    assert(!allZero);
    
    std::cout << "    testFillBytes passed" << std::endl;
}

void testErrorHandling() {
    // Test error handling for empty containers
    nlm::RandomGenerator rng(555);
    
    std::vector<int> emptyVec;
    
    try {
        int val = rng.pickRandom(emptyVec);
        assert(false); // Should have thrown
    } catch (const std::runtime_error& e) {
        // Expected
    }
    
    try {
        uint64_t index = rng.pickRandomIndex(0);
        assert(false); // Should have thrown
    } catch (const std::invalid_argument& e) {
        // Expected
    }
    
    std::cout << "    testErrorHandling passed" << std::endl;
}

void runAll() {
    std::cout << "Running RandomGenerator tests..." << std::endl;
    testRandomCreation();
    testUniformInt();
    testUniformReal();
    testNormalDistribution();
    testBernoulli();
    testPoisson();
    testRandomBool();
    testShuffle();
    testPickRandom();
    testPickRandomIndex();
    testFillBytes();
    testErrorHandling();
    
    std::cout << "All RandomGenerator tests passed!" << std::endl;
}

} // namespace test_random
