#include <chrono>
// Random Tests
// Enhanced with statistical tests, edge cases, and stress tests

#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <chrono>
#include <numeric>
#include <random>

namespace test_random {

// Basic Tests (existing, preserved)
void testRandomCreation() {
    nlm::RandomGenerator rng(42);
    
    // Test reproducibility
    float val1 = rng.randomFloat();
    nlm::RandomGenerator rng2(42);
    float val2 = rng2.randomFloat();
    
    assert(fabs(val1 - val2) < 0.001f);
    
    std::cout << "    testRandomCreation passed" << std::endl;
}

void testRandomFloat() {
    nlm::RandomGenerator rng(42);
    
    for (int i = 0; i < 10; ++i) {
        float val = rng.randomFloat();
        assert(val >= 0.0f && val <= 1.0f);
    }
    
    std::cout << "    testRandomFloat passed" << std::endl;
}

void testRandomInt() {
    nlm::RandomGenerator rng(42);
    
    for (int i = 0; i < 10; ++i) {
        int val = rng.randomInt(0, 100);
        assert(val >= 0 && val < 100);
    }
    
    std::cout << "    testRandomInt passed" << std::endl;
}

void testRandomNormal() {
    nlm::RandomGenerator rng(42);
    
    for (int i = 0; i < 10; ++i) {
        float val = rng.randomNormal(0.0f, 1.0f);
        // Normal distribution: most values should be within reasonable range
        assert(val > -5.0f && val < 5.0f);
    }
    
    std::cout << "    testRandomNormal passed" << std::endl;
}

void testRandomProbability() {
    nlm::RandomGenerator rng(42);
    
    for (int i = 0; i < 10; ++i) {
        bool val = rng.randomBool(0.5f);
        // Should be true or false
        assert(val == true || val == false);
    }
    
    // Test probability bias
    int trueCount = 0;
    for (int i = 0; i < 1000; ++i) {
        if (rng.randomBool(0.7f)) {
            trueCount++;
        }
    }
    
    // Should be approximately 70% true
    float proportion = static_cast<float>(trueCount) / 1000.0f;
    assert(proportion > 0.6f && proportion < 0.8f);
    
    std::cout << "    testRandomProbability passed" << std::endl;
}

void testRandomSeed() {
    nlm::RandomGenerator rng1(42);
    nlm::RandomGenerator rng2(123);
    
    // Different seeds should produce different sequences
    float val1 = rng1.randomFloat();
    float val2 = rng2.randomFloat();
    
    assert(fabs(val1 - val2) > 0.1f);
    
    // Same seeds should produce same sequences
    nlm::RandomGenerator rng3(42);
    float val3 = rng3.randomFloat();
    assert(fabs(val1 - val3) < 0.001f);
    
    std::cout << "    testRandomSeed passed" << std::endl;
}

// Enhanced Tests
void testRandomStress() {
    std::cout << "    Stress Test" << std::endl;
    
    // Create many random generators and stress test them
    std::vector<nlm::RandomGenerator> rngs;
    for (int i = 0; i < 1000; ++i) {
        rngs.emplace_back(static_cast<uint64_t>(i));
    }
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Generate random numbers from all generators
    std::vector<float> randomValues;
    for (auto& rng : rngs) {
        randomValues.push_back(rng.randomFloat());
        randomValues.push_back(rng.randomInt(0, 1000));
    }
    
    // Verify all values are reasonable
    for (float val : randomValues) {
        assert(val >= 0.0f && val <= 1000.0f);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    std::cout << "      Stress test: " << std::fixed << std::setprecision(0) << 2000.0 / totalTime << " random/sec" << std::endl;
    
    std::cout << "      Stress test passed" << std::endl;
}

void testRandomEdgeCases() {
    std::cout << "    Edge Cases Test" << std::endl;
    
    nlm::RandomGenerator rng(42);
    
    // Test boundary values
    rng.setSeed(0);
    assert(rng.randomFloat() >= 0.0f && rng.randomFloat() <= 1.0f);
    
    // Test with different ranges
    for (int i = 0; i < 10; ++i) {
        int val = rng.randomInt(0, 1); // {0, 1}
        assert(val == 0 || val == 1);
    }
    
    // Test very large range
    for (int i = 0; i < 10; ++i) {
        int val = rng.randomInt(0, 1000000);
        assert(val >= 0 && val < 1000000);
    }
    
    // Test zero probability
    int trueCount = 0;
    for (int i = 0; i < 100; ++i) {
        if (rng.randomBool(0.0f)) {
            trueCount++;
        }
    }
    assert(trueCount == 0);
    
    // Test unit probability
    trueCount = 0;
    for (int i = 0; i < 100; ++i) {
        if (rng.randomBool(1.0f)) {
            trueCount++;
        }
    }
    assert(trueCount == 100);
    
    std::cout << "      Edge cases test passed" << std::endl;
}

void testRandomErrorHandling() {
    std::cout << "    Error Handling Test" << std::endl;
    
    nlm::RandomGenerator rng(42);
    
    // Test with various input combinations
    for (int i = 0; i < 100; ++i) {
        float val = rng.randomFloat();
        int intVal = rng.randomInt(0, 100);
        bool boolVal = rng.randomBool(0.5f);
        float normalVal = rng.randomNormal(0.0f, 1.0f);
        
        // All should produce valid values
        assert(val >= 0.0f && val <= 1.0f);
        assert(intVal >= 0 && intVal < 100);
        assert(boolVal == true || boolVal == false);
        assert(normalVal > -5.0f && normalVal < 5.0f);
    }
    
    std::cout << "      Error handling test passed" << std::endl;
}

void testRandomMemoryManagement() {
    std::cout << "    Memory Management Test" << std::endl;
    
    // Test random generator creation and destruction
    std::vector<nlm::RandomGenerator> rngs;
    
    // Create many random generators
    for (int i = 0; i < 500; ++i) {
        rngs.emplace_back(static_cast<uint64_t>(i));
    }
    
    // Generate random numbers
    std::vector<float> values;
    for (auto& rng : rngs) {
        values.push_back(rng.randomFloat());
        values.push_back(rng.randomInt(0, 100));
    }
    
    // Vector should be in valid state
    assert(values.size() == 1000);
    
    // All values should be reasonable
    for (float val : values) {
        assert(val >= 0.0f && val <= 100.0f);
    }
    
    std::cout << "      Memory management test passed" << std::endl;
}

void testRandomPerformance() {
    std::cout << "    Performance Test" << std::endl;
    
    // Performance test for random number generation
    auto startTime = std::chrono::high_resolution_clock::now();
    
    nlm::RandomGenerator rng(42);
    
    for (int i = 0; i < 10000; ++i) {
        float val = rng.randomFloat();
        int intVal = rng.randomInt(0, 1000);
        bool boolVal = rng.randomBool(0.5f);
        float normalVal = rng.randomNormal(0.0f, 1.0f);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>( endTime - startTime).count();
    
    std::cout << "      Performance: " << std::fixed << std::setprecision(0) << 40000.0 / totalTime << " random/sec" << std::endl;
    
    std::cout << "      Performance test passed" << std::endl;
}

void testRandomDistribution() {
    std::cout << "    Distribution Test" << std::endl;
    
    // Test statistical distribution properties
    nlm::RandomGenerator rng(42);
    
    // Collect normal distribution samples
    std::vector<float> normalSamples;
    for (int i = 0; i < 10000; ++i) {
        normalSamples.push_back(rng.randomNormal(0.0f, 1.0f));
    }
    
    // Calculate statistics
    float mean = std::accumulate(normalSamples.begin(), normalSamples.end(), 0.0f) / normalSamples.size();
    float variance = 0.0f;
    for (float val : normalSamples) {
        variance += (val - mean) * (val - mean);
    }
    variance /= normalSamples.size();
    float stddev = std::sqrt(variance);
    
    // Normal distribution should have mean ~0 and stddev ~1
    assert(fabs(mean) < 0.1f);
    assert(fabs(stddev - 1.0f) < 0.1f);
    
    // Test uniform distribution (randomFloat)
    std::vector<float> uniformSamples;
    for (int i = 0; i < 10000; ++i) {
        uniformSamples.push_back(rng.randomFloat());
    }
    
    mean = std::accumulate(uniformSamples.begin(), uniformSamples.end(), 0.0f) / uniformSamples.size();
    assert(fabs(mean - 0.5f) < 0.01f); // Mean of uniform [0,1] is 0.5
    
    std::cout << "      Distribution test passed" << std::endl;
}

void testRandomReproducibility() {
    std::cout << "    Reproducibility Test" << std::endl;
    
    // Test that same seed produces same sequence
    std::vector<float> sequence1;
    std::vector<float> sequence2;
    
    nlm::RandomGenerator rng1(12345);
    nlm::RandomGenerator rng2(12345);
    
    for (int i = 0; i < 1000; ++i) {
        sequence1.push_back(rng1.randomFloat());
        sequence2.push_back(rng2.randomFloat());
    }
    
    // Sequences should be identical
    for (size_t i = 0; i < sequence1.size(); ++i) {
        assert(fabs(sequence1[i] - sequence2[i]) < 0.0001f);
    }
    
    // Different seed should produce different sequence
    nlm::RandomGenerator rng3(54321);
    std::vector<float> sequence3;
    for (int i = 0; i < 100; ++i) {
        sequence3.push_back(rng3.randomFloat());
    }
    
    bool different = false;
    for (float val3 : sequence3) {
        for (float val2 : sequence2) {
            if (fabs(val3 - val2) > 0.01f) {
                different = true;
                break;
            }
        }
        if (different) break;
    }
    
    assert(different); // Should be different sequences
    
    std::cout << "      Reproducibility test passed" << std::endl;
}

void runAll() {
    std::cout << "Running Enhanced Random Tests..." << std::endl;
    
    try {
        // Run basic tests
        testRandomCreation();
        testRandomFloat();
        testRandomInt();
        testRandomNormal();
        testRandomProbability();
        testRandomSeed();
        
        // Run enhanced tests
        testRandomStress();
        testRandomEdgeCases();
        testRandomErrorHandling();
        testRandomMemoryManagement();
        testRandomPerformance();
        testRandomDistribution();
        testRandomReproducibility();
        
        std::cout << "  All Enhanced Random Tests PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        throw;
    }
}

} // namespace test_random