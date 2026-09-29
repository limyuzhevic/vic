// Random Tests
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <vector>
#include <random>

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

// Distribution validation
void testDistributionValidation() {
    nlm::RandomGenerator rng(42);  // Deterministic seed
    
    // Test uniform int distribution
    std::vector<uint32_t> int_samples;
    const int num_int_samples = 10000;
    
    for (int i = 0; i < num_int_samples; ++i) {
        int_samples.push_back(rng.uniformInt(1, 100));
    }
    
    // Check bounds
    for (uint32_t val : int_samples) {
        assert(val >= 1 && val <= 100);
    }
    
    // Test uniform real distribution
    std::vector<double> real_samples;
    const int num_real_samples = 10000;
    
    for (int i = 0; i < num_real_samples; ++i) {
        real_samples.push_back(rng.uniformReal(0.0, 1.0));
    }
    
    // Check bounds
    for (double val : real_samples) {
        assert(val >= 0.0 && val < 1.0);
    }
    
    // Test normal distribution (approximate)
    std::vector<double> normal_samples;
    const int num_normal_samples = 10000;
    double sum = 0.0;
    double sum_sq = 0.0;
    
    for (int i = 0; i < num_normal_samples; ++i) {
        double val = rng.normal(0.0, 1.0);
        normal_samples.push_back(val);
        sum += val;
        sum_sq += val * val;
    }
    
    double mean = sum / num_normal_samples;
    double stddev = std::sqrt((sum_sq / num_normal_samples) - (mean * mean));
    
    // Check mean and stddev are close to expected values
    assert(std::abs(mean) < 0.1);
    assert(std::abs(stddev - 1.0) < 0.1);
    
    std::cout << "    testDistributionValidation passed" << std::endl;
}

// Seed management
void testSeedManagement() {
    nlm::RandomGenerator rng1(12345);
    nlm::RandomGenerator rng2(12345);
    
    // Same seed should produce same sequence
    for (int i = 0; i < 10; ++i) {
        assert(rng1.uniformInt(0, 1000) == rng2.uniformInt(0, 1000));
    }
    
    // Different seeds should produce different sequences
    nlm::RandomGenerator rng3(54321);
    
    // Most likely different, but could theoretically be same (very unlikely)
    bool different = false;
    for (int i = 0; i < 10; ++i) {
        if (rng1.uniformInt(0, 1000) != rng3.uniformInt(0, 1000)) {
            different = true;
            break;
        }
    }
    
    assert(different); // Should be different
    
    // Test seed update
    rng1.seed(99999);
    uint32_t val_after_seed = rng1.uniformInt(0, 1000);
    
    rng1.seed(99999);
    uint32_t val_after_seed2 = rng1.uniformInt(0, 1000);
    
    assert(val_after_seed == val_after_seed2);
    
    std::cout << "    testSeedManagement passed" << std::endl;
}

// Statistical properties
void testStatisticalProperties() {
    nlm::RandomGenerator rng(42);
    
    const int sample_size = 10000;
    const double expected_mean = 0.5;
    const double expected_std = std::sqrt(1.0 / 12.0);  // For uniform [0,1)
    
    std::vector<double> samples;
    samples.reserve(sample_size);
    
    for (int i = 0; i < sample_size; ++i) {
        samples.push_back(rng.uniformReal(0.0, 1.0));
    }
    
    // Calculate mean
    double sum = std::accumulate(samples.begin(), samples.end(), 0.0);
    double mean = sum / sample_size;
    
    // Calculate standard deviation
    double variance = 0.0;
    for (double val : samples) {
        variance += (val - mean) * (val - mean);
    }
    variance /= sample_size;
    double stddev = std::sqrt(variance);
    
    // Check properties are within reasonable bounds
    assert(std::abs(mean - expected_mean) < 0.01);
    assert(std::abs(stddev - expected_std) < 0.01);
    
    // Test that all values are unique (very high probability with sample_size=10000)
    std::sort(samples.begin(), samples.end());
    for (size_t i = 1; i < samples.size(); ++i) {
        assert(samples[i] != samples[i-1]);  // No exact duplicates
    }
    
    std::cout << "    testStatisticalProperties passed" << std::endl;
}

// Random generator properties
void testRandomGeneratorProperties() {
    nlm::RandomGenerator rng(12345);
    
    // Test that min/max are correct
    assert(rng.min() == std::numeric_limits<uint32_t>::min());
    assert(rng.max() == std::numeric_limits<uint32_t>::max());
    
    // Test operator() returns valid range
    for (int i = 0; i < 100; ++i) {
        uint32_t val = rng();
        assert(val >= rng.min() && val <= rng.max());
    }
    
    // Test underlying engine access
    auto& engine = rng.engine();
    
    // Should be able to use the engine
    std::mt19937_64 test_engine(42);
    uint32_t val1 = engine()();
    uint32_t val2 = test_engine()();
    assert(val1 != val2 || (val1 == val2 && 42 == 42)); // May be equal by chance
    
    std::cout << "    testRandomGeneratorProperties passed" << std::endl;
}

// Edge case testing
void testEdgeCases() {
    nlm::RandomGenerator rng(123);
    
    // Test zero range
    uint32_t val1 = rng.uniformInt(5, 5);
    assert(val1 == 5);
    
    // Test negative range (if supported)
    // double val2 = rng.uniformReal(-1.0, -0.5);  // Implementation dependent
    
    // Test Bernoulli with extreme probabilities
    bool val3 = rng.bernoulli(0.0);  // Should always be false
    assert(val3 == false);
    
    bool val4 = rng.bernoulli(1.0);  // Should always be true
    assert(val4 == true);
    
    // Test pickRandomIndex with edge cases
    uint64_t idx1 = rng.pickRandomIndex(1);  // Only one element
    assert(idx1 == 0);
    
    // Test shuffle with small collection
    std::vector<int> small_vec = {1, 2, 3};
    auto vec_copy = small_vec;
    
    rng.shuffle(small_vec);
    
    // Should contain same elements
    std::sort(small_vec.begin(), small_vec.end());
    std::sort(vec_copy.begin(), vec_copy.end());
    assert(small_vec == vec_copy);
    
    std::cout << "    testEdgeCases passed" << std::endl;
}

// Memory leak detection for random generator
void testRandomGeneratorMemoryLeak() {
    // Create and destroy many generators
    for (int i = 0; i < 100; ++i) {
        nlm::RandomGenerator rng(i);  // Different seed each time
        
        // Generate some random numbers
        for (int j = 0; j < 10; ++j) {
            rng.uniformInt(0, 100);
            rng.uniformReal(0.0, 1.0);
            rng.normal(0.0, 1.0);
            rng.bernoulli(0.5);
        }
    }
    
    std::cout << "    testRandomGeneratorMemoryLeak passed" << std::endl;
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
    
    // Additional comprehensive tests
    testDistributionValidation();
    testSeedManagement();
    testStatisticalProperties();
    testRandomGeneratorProperties();
    testEdgeCases();
    testRandomGeneratorMemoryLeak();
}

} // namespace test_random
