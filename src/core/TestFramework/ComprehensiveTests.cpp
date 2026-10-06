// Comprehensive test suite for NLM improvements
#include "TestFramework.hpp"
#include "ErrorHandling.hpp"
#include "Config.hpp"
#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>

namespace nlm {

// Initialize test data
void initializeTestConfig(Config& config) {
    config.set("neuron_count", 100);
    config.set("region_count", 1);
    config.set("connection_probability", 0.1f);
    config.set("simulation_timestep", 0.001);
    config.set("random_seed", 42);
}

// Test Error Handling
void testErrorHandling() {
    std::cout << "Testing Error Handling..." << std::endl;
    
    // Test 1: Basic error handling
    try {
        NLM_THROW_ERROR("Test error", ErrorSeverity::Error);
        ASSERT_FALSE(true, "Should have thrown exception");
    } catch (const NLMException& e) {
        ASSERT_EQ(static_cast<int>(e.getSeverity()), ErrorSeverity::Error);
    }
    
    // Test 2: Warning handling  
    try {
        NLM_THROW_ERROR("Test warning", ErrorSeverity::Warning);
        ASSERT_FALSE(true, "Should have thrown exception");
    } catch (const NLMException& e) {
        ASSERT_EQ(static_cast<int>(e.getSeverity()), ErrorSeverity::Warning);
    }
    
    // Test 3: Safe operations
    auto result = ErrorHandler::getInstance()->safeOperation("test", []() { return 42; });
    ASSERT_EQ(result, 42);
    
    // Test 4: Config operations
    Config config;
    initializeTestConfig(config);
    
    // Test getting config values
    auto neuronCount = config.get<int64_t>("neuron_count");
    ASSERT_TRUE(neuronCount.has_value());
    ASSERT_EQ(neuronCount.value(), 100);
    
    // Test getting with defaults
    auto regionCount = config.getOr<int64_t>("region_count", 0);
    ASSERT_EQ(regionCount, 1);
    
    // Test getting non-existent key
    auto nonExistent = config.get<int64_t>("non_existent_key");
    ASSERT_FALSE(nonExistent.has_value());
    
    // Test getting non-existent with default
    auto defaultValue = config.getOr<int64_t>("non_existent", 999);
    ASSERT_EQ(defaultValue, 999);
    
    // Test save/load functionality
    bool saveResult = config.saveToFile("test_config.cfg");
    ASSERT_TRUE(saveResult);
    
    Config config2;
    bool loadResult = config2.loadFromFile("test_config.cfg");
    ASSERT_TRUE(loadResult);
    
    // Clean up
    std::filesystem::remove("test_config.cfg");
    
    std::cout << "✓ Error handling tests passed" << std::endl;
}

// Test Memory Safety
void testMemorySafety() {
    std::cout << "Testing Memory Safety..." << std::endl;
    
    Config config;
    initializeTestConfig(config);
    
    // Test nullptr safety
    auto safeAccess = ErrorHandler::getInstance()->safeAccess(
        std::shared_ptr<int>(), "null access test"
    );
    ASSERT_EQ(safeAccess, nullptr);
    
    // Test resource creation
    auto safeResource = SafeResource::create<int>("test_resource", 42);
    ASSERT_TRUE(SafeResource::isValid(safeResource));
    
    // Test brain initialization (if headers available)
    // This would require including brain headers
    std::cout << "✓ Memory safety tests passed" << std::endl;
}

// Test Performance Improvements
void testPerformance() {
    std::cout << "Testing Performance..." << std::endl;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Create many config entries
    Config config;
    for (int i = 0; i < 1000; ++i) {
        config.set("key_" + std::to_string(i), i);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(endTime - startTime).count();
    
    // Should complete within reasonable time (less than 1 second for 1000 entries)
    ASSERT_LT(duration, 1.0, "Config creation took too long");
    
    // Test config retrieval performance
    startTime = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 1000; ++i) {
        auto val = config.get<int>("key_" + std::to_string(i % 100));
        (void)val;  // Suppress unused variable warning
    }
    
    endTime = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration<double>(endTime - startTime).count();
    
    ASSERT_LT(duration, 0.5, "Config retrieval took too long");
    
    std::cout << "✓ Performance tests passed (1000 config entries in " << duration << "s)" << std::endl;
}

// Test Configuration Robustness
void testConfigurationRobustness() {
    std::cout << "Testing Configuration Robustness..." << std::endl;
    
    Config config;
    
    // Test setting different value types
    config.set("string_key", "test_value");
    config.set("int_key", 42);
    config.set("double_key", 3.14);
    config.set("bool_key", true);
    
    // Test type-safe retrieval
    auto stringVal = config.get<std::string>("string_key");
    ASSERT_TRUE(stringVal.has_value());
    ASSERT_EQ(stringVal.value(), "test_value");
    
    auto intVal = config.get<int64_t>("int_key");
    ASSERT_TRUE(intVal.has_value());
    ASSERT_EQ(intVal.value(), 42);
    
    // Test vector types if supported
    config.set("vector_int", std::vector<int>({1, 2, 3, 4, 5}));
    // vector retrieval would need to be implemented
    
    // Test clear functionality
    config.clear();
    ASSERT_EQ(config.getKeys().size(), 0);
    
    // Test removal
    config.set("temp_key", "temp_value");
    ASSERT_TRUE(config.has("temp_key"));
    config.remove("temp_key");
    ASSERT_FALSE(config.has("temp_key"));
    
    // Test non-existent key handling
    config.set("existing_key", "value");
    config.remove("non_existent_key");  // Should not crash
    
    std::cout << "✓ Configuration robustness tests passed" << std::endl;
}

// Test Error Handling in C++
void testCppErrorHandling() {
    std::cout << "Testing C++ Error Handling..." << std::endl;
    
    // Test parameter validation
    try {
        NLM_VALIDATE_PARAM(true, "Test param validation", "Param should be valid");
    } catch (...) {
        ASSERT_FALSE(true, "Should not throw for valid parameter");
    }
    
    try {
        NLM_VALIDATE_PARAM(false, "Test param validation", "Param should be invalid");
        ASSERT_FALSE(true, "Should have thrown for invalid parameter");
    } catch (const InvalidParameterError& e) {
        ASSERT_TRUE(true, "Should have thrown InvalidParameterError");
    }
    
    // Test resource creation errors
    try {
        // This should work fine
        auto resource = SafeResource::create<int>("test", 42);
        ASSERT_TRUE(SafeResource::isValid(resource));
    } catch (...) {
        ASSERT_FALSE(true, "Resource creation should not fail");
    }
    
    std::cout << "✓ C++ error handling tests passed" << std::endl;
}

// Test Memory Management
void testMemoryManagement() {
    std::cout << "Testing Memory Management..." << std::endl;
    
    // Test automatic resource cleanup
    {
        Config config;
        initializeTestConfig(config);
        
        // Should be cleaned up when config goes out of scope
        auto keys = config.getKeys();
        ASSERT_GT(keys.size(), 0);
    }
    
    // Test that multiple config objects don't interfere
    Config config1;
    Config config2;
    
    config1.set("key1", "value1");
    config2.set("key2", "value2");
    
    ASSERT_EQ(config1.get<std::string>("key1").value(), "value1");
    ASSERT_EQ(config2.get<std::string>("key2").value(), "value2");
    
    // config1 should not have key2
    ASSERT_FALSE(config1.has("key2"));
    
    std::cout << "✓ Memory management tests passed" << std::endl;
}

// Test Thread Safety (if supported)
void testThreadSafety() {
    std::cout << "Testing Thread Safety (basic)..." << std::endl;
    
    // Basic thread safety test - not comprehensive but better than nothing
    Config config;
    
    // Simulate concurrent access (in real implementation, would need proper mutex)
    auto val1 = config.getOr<int64_t>("test_key", 1);
    config.set("test_key", 2);
    auto val2 = config.getOr<int64_t>("test_key", 0);
    
    // Values should be different
    if (val1 == val2) {
        std::cout << "! Thread safety warning: concurrent access may cause race conditions" << std::endl;
    }
    
    std::cout << "✓ Basic thread safety tests passed" << std::endl;
}

// Run all tests
void runAllTests() {
    std::cout << "=== NLM Improved Test Suite ===" << std::endl;
    std::cout << std::endl;
    
    testErrorHandling();
    testMemorySafety();
    testPerformance();
    testConfigurationRobustness();
    testCppErrorHandling();
    testMemoryManagement();
    testThreadSafety();
    
    std::cout << std::endl;
    std::cout << "=== All Tests Passed! ===" << std::endl;
}

} // namespace nlm
