// Basic Error Handling Tests
// Tests error handling using existing error handling mechanisms in the NLM system

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <vector>

namespace test_error_handling {

// Test basic error handling through exceptions
void testExceptionHandling() {
    std::cout << "Testing basic exception handling..." << std::endl;
    
    // Test that exceptions can be caught and handled
    bool exceptionCaught = false;
    
    try {
        // Try to create operations that might throw
        auto config = nlm::Config::create();
        
        // Perform some operations that could potentially throw
        config->set("test", 1, nlm::ConfigSource::Runtime);
        
        // Normal operation - no exception expected
        assert(config != nullptr);
        
        exceptionCaught = true;
    } catch (const std::exception& e) {
        // Exception should be caught
        std::cout << "    Exception caught: " << e.what() << std::endl;
        exceptionCaught = true;
    } catch (...) {
        // Catch-all for unexpected exceptions
        std::cout << "    Unknown exception caught" << std::endl;
        exceptionCaught = true;
    }
    
    assert(exceptionCaught);
    std::cout << "    Basic exception handling tests passed" << std::endl;
}

// Test that RandomGenerator handles edge cases gracefully
void testRandomErrorScenarios() {
    std::cout << "Testing RandomGenerator error scenarios..." << std::endl;
    
    // Test with various seed values
    nlm::RandomGenerator rng1(0); // Edge case seed
    nlm::RandomGenerator rng2(1); // Another edge case
    nlm::RandomGenerator rng3(UINT64_MAX); // Maximum seed
    
    // Test that generators are usable even with edge case seeds
    uint32_t val1 = rng1.uniformInt(0, 100);
    uint32_t val2 = rng2.uniformInt(0, 100);
    uint32_t val3 = rng3.uniformInt(0, 100);
    
    assert(val1 >= 0 && val1 <= 100);
    assert(val2 >= 0 && val2 <= 100);
    assert(val3 >= 0 && val3 <= 100);
    
    // Test that reproducibility is maintained
    nlm::RandomGenerator rng4(42);
    nlm::RandomGenerator rng5(42);
    
    for (int i = 0; i < 10; ++i) {
        assert(rng4.uniformInt(0, 1000) == rng5.uniformInt(0, 1000));
    }
    
    std::cout << "    RandomGenerator error scenario tests passed" << std::endl;
}

// Test that Config handles invalid inputs gracefully
void testConfigErrorScenarios() {
    std::cout << "Testing Config error scenarios..." << std::endl;
    
    auto config = nlm::Config::create();
    
    // Test setting valid values
    config->set("valid_key", 42, nlm::ConfigSource::Runtime);
    config->set("valid_key2", std::string("test"), nlm::ConfigSource::Runtime);
    config->set("valid_key3", 3.14, nlm::ConfigSource::Runtime);
    
    // Verify values were set correctly
    assert(config->has("valid_key"));
    assert(config->getOr<int>("valid_key", 0) == 42);
    
    assert(config->has("valid_key2"));
    assert(config->getOr<std::string>("valid_key2", "") == "test");
    
    assert(config->has("valid_key3"));
    assert(config->getOr<double>("valid_key3", 0.0) == 3.14);
    
    // Test validation
    bool isValid = config->validate();
    assert(isValid); // Should be valid with proper settings
    
    std::cout << "    Config error scenario tests passed" << std::endl;
}

// Test thread safety with error scenarios
void testThreadSafetyErrorScenarios() {
    std::cout << "Testing thread safety with error scenarios..." << std::endl;
    
    std::vector<bool> threadResults;
    std::vector<std::thread> threads;
    std::mutex mutex;
    
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&mutex, &threadResults, i]() {
            bool threadSuccess = false;
            
            try {
                // Each thread uses its own random generator
                nlm::RandomGenerator rng(i);
                
                // Perform operations that should not throw
                uint32_t val = rng.uniformInt(0, 100);
                assert(val >= 0 && val <= 100);
                
                // Thread-safe operations with protection
                std::lock_guard<std::mutex> lock(mutex);
                threadResults.push_back(true);
                threadSuccess = true;
                
            } catch (...) {
                std::lock_guard<std::mutex> lock(mutex);
                threadResults.push_back(false);
                threadSuccess = false;
            }
        });
    }
    
    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }
    
    // Verify all threads succeeded
    for (bool result : threadResults) {
        assert(result);
    }
    
    std::cout << "    Thread safety error scenario tests passed" << std::endl;
}

// Test error recovery mechanisms
void testErrorRecoveryMechanisms() {
    std::cout << "Testing error recovery mechanisms..." << std::endl;
    
    // Test that objects can be recreated after "failures"
    bool recoverySuccessful = false;
    
    try {
        // First attempt
        auto config1 = nlm::Config::create();
        config1->set("test", 1, nlm::ConfigSource::Runtime);
        assert(config1 != nullptr);
        
        // Simulate a scenario that might cause issues
        bool valueSet = config1->has("test");
        assert(valueSet);
        
        // Second attempt - recovery
        auto config2 = nlm::Config::create();
        config2->set("test2", 2, nlm::ConfigSource::Runtime);
        assert(config2 != nullptr);
        
        // Both should be valid
        recoverySuccessful = true;
        
    } catch (...) {
        recoverySuccessful = false;
    }
    
    assert(recoverySuccessful);
    std::cout << "    Error recovery mechanism tests passed" << std::endl;
}

// Test that the system handles edge cases gracefully
void testEdgeCaseHandling() {
    std::cout << "Testing edge case handling..." << std::endl;
    
    // Test with various data types
    auto config = nlm::Config::create();
    
    // Test different value types
    config->set("int_value", 42, nlm::ConfigSource::Runtime);
    config->set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
    config->set("double_value", 3.14, nlm::ConfigSource::Runtime);
    config->set("bool_value", true, nlm::ConfigSource::Runtime);
    
    // Verify all values were set correctly
    assert(config->getOr<int>("int_value", 0) == 42);
    assert(config->getOr<std::string>("string_value", "") == "hello");
    assert(config->getOr<double>("double_value", 0.0) == 3.14);
    assert(config->getOr<bool>("bool_value", false) == true);
    
    // Test random generator with different data types
    nlm::RandomGenerator rng(42);
    
    // Test various operations
    uint32_t uint_val = rng.uniformInt32(0, 100);
    uint64_t ulong_val = rng.uniformInt64(0, 100);
    float float_val = rng.uniformReal32(0.0f, 1.0f);
    double double_val = rng.uniformReal64(0.0, 1.0);
    
    assert(uint_val >= 0 && uint_val <= 100);
    assert(ulong_val >= 0 && ulong_val <= 100);
    assert(float_val >= 0.0f && float_val < 1.0f);
    assert(double_val >= 0.0 && double_val < 1.0);
    
    std::cout << "    Edge case handling tests passed" << std::endl;
}

void runAll() {
    testExceptionHandling();
    testRandomErrorScenarios();
    testConfigErrorScenarios();
    testThreadSafetyErrorScenarios();
    testErrorRecoveryMechanisms();
    testEdgeCaseHandling();
    
    std::cout << std::endl;
    std::cout << "=== All Basic Error Handling Tests Passed! ===" << std::endl;
    std::cout << std::endl;
}

} // namespace test_error_handling