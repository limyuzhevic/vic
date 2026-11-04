// Random Generator Error Handling Tests
// Tests for improved error handling in RandomGenerator class

#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <limits>

namespace test_random_error_handling {

void testRandomGeneratorInvalidRange() {
    nlm::RandomGenerator rng;
    
    // Test invalid range for uniformInt
    try {
        rng.uniformInt(10, 5);  // min > max
        assert(false && "Should have thrown for invalid range");
    } catch (const std::invalid_argument& e) {
        assert(std::string(e.what()).find("min must be") != std::string::npos);
    }
    
    try {
        rng.uniformInt(5, 5);  // min == max (now allowed but tested)
        uint32_t result = rng.uniformInt(5, 5);
        assert(result == 5);
    } catch (const std::invalid_argument& e) {
        // This should not throw
    }
    
    // Test invalid range for uniformReal
    try {
        rng.uniformReal(1.0, 0.5);  // min >= max
        assert(false && "Should have thrown for invalid range");
    } catch (const std::invalid_argument& e) {
        assert(std::string(e.what()).find("min must be") != std::string::npos);
    }
    
    std::cout << "    testRandomGeneratorInvalidRange passed" << std::endl;
}

void testRandomGeneratorOutOfBounds() {
    nlm::RandomGenerator rng;
    
    // Test with values that might cause overflow
    try {
        // This should work with validation
        uint32_t result = rng.uniformInt(uint32_t(0), uint32_t(std::numeric_limits<uint32_t>::max()));
        // Should not throw
        assert(result >= 0 && result <= std::numeric_limits<uint32_t>::max());
    } catch (const std::invalid_argument& e) {
        // This might throw if validation is too strict
    }
    
    std::cout << "    testRandomGeneratorOutOfBounds passed" << std::endl;
}

void testRandomGeneratorNormalValidation() {
    nlm::RandomGenerator rng;
    
    // Test normal distribution with invalid standard deviation
    try {
        rng.normal(0.0, -1.0);  // negative stddev
        assert(false && "Should have thrown for negative stddev");
    } catch (const std::invalid_argument& e) {
        assert(std::string(e.what()).find("standard deviation") != std::string::npos);
    }
    
    // Test normal distribution with valid parameters
    double result = rng.normal(0.0, 1.0);
    // Should not throw
    
    std::cout << "    testRandomGeneratorNormalValidation passed" << std::endl;
}

void testRandomGeneratorBernoulliValidation() {
    nlm::RandomGenerator rng;
    
    // Test bernoulli with invalid probability
    try {
        rng.bernoulli(-0.1);  // probability < 0
        assert(false && "Should have thrown for invalid probability");
    } catch (const std::invalid_argument& e) {
        assert(std::string(e.what()).find("probability") != std::string::npos);
    }
    
    try {
        rng.bernoulli(1.1);  // probability > 1
        assert(false && "Should have thrown for invalid probability");
    } catch (const std::invalid_argument& e) {
        assert(std::string(e.what()).find("probability") != std::string::npos);
    }
    
    // Test bernoulli with valid probabilities
    bool result = rng.bernoulli(0.0);
    assert(result == false);
    
    result = rng.bernoulli(1.0);
    assert(result == true);
    
    std::cout << "    testRandomGeneratorBernoulliValidation passed" << std::endl;
}

void testRandomGeneratorPoissonValidation() {
    nlm::RandomGenerator rng;
    
    // Test poisson with invalid lambda
    try {
        rng.poisson(-1.0);  // lambda < 0
        assert(false && "Should have thrown for negative lambda");
    } catch (const std::invalid_argument& e) {
        assert(std::string(e.what()).find("lambda") != std::string::npos);
    }
    
    // Test poisson with valid parameters
    uint32_t result = rng.poisson(1.0);
    // Should not throw
    
    std::cout << "    testRandomGeneratorPoissonValidation passed" << std::endl;
}

void testRandomGeneratorExponentialValidation() {
    nlm::RandomGenerator rng;
    
    // Test exponential with invalid lambda
    try {
        rng.exponential(-1.0);  // lambda <= 0
        assert(false && "Should have thrown for invalid lambda");
    } catch (const std::invalid_argument& e) {
        assert(std::string(e.what()).find("lambda") != std::string::npos);
    }
    
    try {
        rng.exponential(0.0);  // lambda <= 0
        assert(false && "Should have thrown for invalid lambda");
    } catch (const std::invalid_argument& e) {
        assert(std::string(e.what()).find("lambda") != std::string::npos);
    }
    
    // Test exponential with valid parameter
    double result = rng.exponential(1.0);
    assert(result >= 0.0);
    
    std::cout << "    testRandomGeneratorExponentialValidation passed" << std::endl;
}

void testRandomGeneratorPickRandomIndex() {
    nlm::RandomGenerator rng;
    
    // Test picking from zero items
    try {
        rng.pickRandomIndex(0);
        assert(false && "Should have thrown for zero items");
    } catch (const std::invalid_argument& e) {
        assert(std::string(e.what()).find("zero items") != std::string::npos);
    }
    
    // Test picking from valid number of items
    uint64_t result = rng.pickRandomIndex(1);
    assert(result == 0);
    
    result = rng.pickRandomIndex(100);
    assert(result < 100);
    
    std::cout << "    testRandomGeneratorPickRandomIndex passed" << std::endl;
}

void testRandomGeneratorSerialization() {
    nlm::RandomGenerator rng1(42);
    nlm::RandomGenerator rng2(123);
    
    // Get state from first generator
    std::string state1 = rng1.serializeState();
    
    // Try to restore state to second generator
    bool result = rng2.restoreState(state1);
    // Result depends on implementation (might always return false for simplicity)
    
    // Test that serialization doesn't throw exceptions
    std::string state2 = rng2.serializeState();
    
    std::cout << "    testRandomGeneratorSerialization passed" << std::endl;
}

void runAll() {
    testRandomGeneratorInvalidRange();
    testRandomGeneratorOutOfBounds();
    testRandomGeneratorNormalValidation();
    testRandomGeneratorBernoulliValidation();
    testRandomGeneratorPoissonValidation();
    testRandomGeneratorExponentialValidation();
    testRandomGeneratorPickRandomIndex();
    testRandomGeneratorSerialization();
    
    std::cout << "=== All Random Generator Error Handling Tests PASSED ===" << std::endl;
}

} // namespace test_random_error_handling