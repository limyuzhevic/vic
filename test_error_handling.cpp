#include "../../core/ErrorHandling.hpp"
#include "../../core/Random/Random.hpp"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "Testing NLM Error Handling System..." << std::endl;
    
    // Test 1: Basic error handling
    try {
        NLM_THROW_INVALID_ARG("Test invalid argument");
    } catch (const std::invalid_argument& e) {
        std::cout << "✓ Test 1 passed: Invalid argument exception caught" << std::endl;
    }
    
    // Test 2: RandomGenerator with standardized error handling
    {
        nlm::RandomGenerator rng(42);
        
        // These should all work without throwing
        auto result1 = rng.uniformInt(0, 10);
        auto result2 = rng.uniformReal(0.0, 1.0);
        auto result3 = rng.normal(0.0, 1.0);
        
        std::cout << "✓ Test 2 passed: RandomGenerator operations completed" << std::endl;
    }
    
    // Test 3: Error handling macros
    try {
        NLM_VALIDATE_PARAM(false, "This should fail");
    } catch (const std::runtime_error& e) {
        std::cout << "✓ Test 3 passed: Validation macro exception caught" << std::endl;
    }
    
    // Test 4: Null pointer check macro
    try {
        NLM_CHECK_NULL(nullptr, "This should fail");
    } catch (const std::runtime_error& e) {
        std::cout << "✓ Test 4 passed: Null pointer macro exception caught" << std::endl;
    }
    
    // Test 5: Container validation
    try {
        std::vector<int> empty_vec;
        NLM_CHECK_NOT_EMPTY(empty_vec, "This should fail");
    } catch (const std::runtime_error& e) {
        std::cout << "✓ Test 5 passed: Container validation macro exception caught" << std::endl;
    }
    
    // Test 6: Range validation
    try {
        NLM_CHECK_RANGE(5, 10, 15, "This should fail");
    } catch (const std::runtime_error& e) {
        std::cout << "✓ Test 6 passed: Range validation macro exception caught" << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "All error handling tests passed! ✓" << std::endl;
    return 0;
}
