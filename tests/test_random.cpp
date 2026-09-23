// Random Number Generator Tests
// Tests for NLM's random number generation functionality
// #include "core/Random/Random.hpp"
// #include <cassert>
// #include <iostream>
// #include <cmath>

namespace test_random {

void testRandomGeneratorCreation() {
    nlm::RandomGenerator rng(42);
    
    // Test that different seeds produce different sequences
    nlm::RandomGenerator rng2(43);
    
    float val1 = rng.uniformReal(0.0f, 1.0f);
    float val2 = rng2.uniformReal(0.0f, 1.0f);
    
    // Most likely different
    assert(val1 != val2 || std::abs(val1 - val2) > 0.1f);
    
    std::cout << "    testRandomGeneratorCreation passed" << std::endl;
}

void testUniformReal() {
    nlm::RandomGenerator rng(12345);
    
    // Test multiple calls produce different values
    float val1 = rng.uniformReal(0.0f, 1.0f);
    float val2 = rng.uniformReal(0.0f, 1.0f);
    float val3 = rng.uniformReal(0.0f, 1.0f);
    
    // Values should be in range [0, 1]
    assert(val1 >= 0.0f && val1 <= 1.0f);
    assert(val2 >= 0.0f && val2 <= 1.0f);
    assert(val3 >= 0.0f && val3 <= 1.0f);
    
    // Test specific seed produces reproducible sequence
    nlm::RandomGenerator rng_repro(12345);
    float val1_repro = rng_repro.uniformReal(0.0f, 1.0f);
    float val2_repro = rng_repro.uniformReal(0.0f, 1.0f);
    
    assert(val1_repro == val1);
    assert(val2_repro == val2);
    
    std::cout << "    testUniformReal passed" << std::endl;
}

void testUniformInt() {
    nlm::RandomGenerator rng(12345);
    
    // Test uniform integer distribution
    int val1 = rng.uniformInt(0, 100);
    int val2 = rng.uniformInt(0, 100);
    
    assert(val1 >= 0 && val1 <= 100);
    assert(val2 >= 0 && val2 <= 100);
    
    // Test that min and max work
    int val3 = rng.uniformInt(10, 20);
    assert(val3 >= 10 && val3 <= 20);
    
    // Test reproducible sequence
    nlm::RandomGenerator rng_repro(12345);
    int val1_repro = rng_repro.uniformInt(0, 100);
    int val2_repro = rng_repro.uniformInt(0, 100);
    
    assert(val1_repro == val1);
    assert(val2_repro == val2);
    
    std::cout << "    testUniformInt passed" << std::endl;
}

void testRandomSeed() {
    // Test that zero seed produces valid sequence
    nlm::RandomGenerator rng_zero(0);
    float val_zero = rng_zero.uniformReal(0.0f, 1.0f);
    assert(val_zero >= 0.0f && val_zero <= 1.0f);
    
    // Test large seed
    nlm::RandomGenerator rng_large(2147483647);
    float val_large = rng_large.uniformReal(0.0f, 1.0f);
    assert(val_large >= 0.0f && val_large <= 1.0f);
    
    // Test negative seed (should still work)
    nlm::RandomGenerator rng_neg(-1);
    float val_neg = rng_neg.uniformReal(0.0f, 1.0f);
    assert(val_neg >= 0.0f && val_neg <= 1.0f);
    
    std::cout << "    testRandomSeed passed" << std::endl;
}

void testRandomDistribution() {
    nlm::RandomGenerator rng(12345);
    
    // Generate many samples and test distribution
    const int num_samples = 1000;
    float sum = 0.0f;
    float sum_sq = 0.0f;
    
    for (int i = 0; i < num_samples; ++i) {
        float val = rng.uniformReal(0.0f, 1.0f);
        sum += val;
        sum_sq += val * val;
    }
    
    float mean = sum / num_samples;
    float variance = sum_sq / num_samples - mean * mean;
    
    // Mean should be approximately 0.5
    assert(std::abs(mean - 0.5f) < 0.02f);
    
    // Variance should be approximately 1/12 = 0.0833 for uniform [0,1]
    assert(std::abs(variance - 0.0833f) < 0.02f);
    
    std::cout << "    testRandomDistribution passed" << std::endl;
}

void runAll() {
    testRandomGeneratorCreation();
    testUniformReal();
    testUniformInt();
    testRandomSeed();
    testRandomDistribution();
    
    std::cout << std::endl;
    std::cout << "=== Random Tests ===" << std::endl;
    std::cout << "All random tests passed!" << std::endl;
}

} // namespace test_random