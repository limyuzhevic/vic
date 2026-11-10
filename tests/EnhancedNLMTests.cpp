// Comprehensive test suite for enhanced NLM components

#include "tests/TestSuite.hpp"
#include "src/core/ValidatedConfig.hpp"
#include "src/performance/MemoryPool.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <memory>

namespace test {

// Memory Pool Tests
class MemoryPoolTests {
public:
    void run() {
        testMemoryPoolCreation();
        testMemoryPoolAllocation();
        testMemoryPoolDeallocation();
        testMemoryPoolStatistics();
    }
    
private:
    void testMemoryPoolCreation() {
        std::cout << "Running Memory Pool Creation Test..." << std::endl;
        
        // Create a memory pool
        MemoryPool<int> pool(1024, 4);
        
        // Verify pool is created successfully
        assert(!pool.empty());
        assert(pool.availableCount() >= 0);
        
        std::cout << "  Memory pool created successfully." << std::endl;
    }
    
    void testMemoryPoolAllocation() {
        std::cout << "Running Memory Pool Allocation Test..." << std::endl;
        
        MemoryPool<int> pool(10, 2);
        
        // Allocate objects
        std::vector<int*> allocated;
        for (int i = 0; i < 15; ++i) {
            int* obj = pool.allocate();
            if (obj) {
                allocated.push_back(obj);
                *obj = i * 10;
            }
        }
        
        // Verify allocations
        assert(allocated.size() == 15);
        
        // Check that objects were initialized correctly
        for (size_t i = 0; i < allocated.size() && i < 10; ++i) {
            assert(*allocated[i] == static_cast<int>(i * 10));
        }
        
        // Deallocate some objects
        for (size_t i = 0; i < allocated.size(); i += 2) {
            pool.deallocate(allocated[i]);
        }
        
        std::cout << "  Allocation and deallocation test passed." << std::endl;
    }
    
    void testMemoryPoolDeallocation() {
        std::cout << "Running Memory Pool Deallocation Test..." << std::endl;
        
        MemoryPool<float> pool(10, 2);
        
        // Allocate and deallocate in various patterns
        std::vector<float*> objects;
        for (int i = 0; i < 20; ++i) {
            float* obj = pool.allocate();
            if (obj) {
                objects.push_back(obj);
                *obj = i * 0.5f;
            }
        }
        
        // Deallocate all objects
        for (float* obj : objects) {
            pool.deallocate(obj);
        }
        
        // Verify pool state
        assert(objects.size() == 20);
        assert(pool.allocatedCount() == 0);
        
        std::cout << "  Deallocation test passed." << std::endl;
    }
    
    void testMemoryPoolStatistics() {
        std::cout << "Running Memory Pool Statistics Test..." << std::endl;
        
        MemoryPool<double> pool(100, 5);
        
        size_t initialAllocated = pool.allocatedCount();
        size_t initialAvailable = pool.availableCount();
        
        // Allocate some objects
        std::vector<double*> objects;
        for (int i = 0; i < 50; ++i) {
            double* obj = pool.allocate();
            if (obj) {
                objects.push_back(obj);
                *obj = i * 1.5;
            }
        }
        
        // Check statistics
        assert(pool.allocatedCount() > initialAllocated);
        assert(pool.availableCount() < initialAvailable);
        
        // Deallocate all
        for (double* obj : objects) {
            pool.deallocate(obj);
        }
        
        // Statistics should be back to initial
        assert(pool.allocatedCount() == initialAllocated);
        
        std::cout << "  Statistics test passed." << std::endl;
    }
};

// Configuration Validation Tests
class ConfigValidationTests {
public:
    void run() {
        testConfigValidationSetup();
        testSchemaCreation();
        testValidationRules();
        testValidationResults();
        testRequiredKeys();
    }
    
private:
    void testConfigValidationSetup() {
        std::cout << "Running Configuration Validation Setup Test..." << std::endl;
        
        ConfigSchema schema;
        
        // Verify schema is created successfully
        assert(!schema.isKnownKey("test.key"));
        
        // Add a test rule
        schema.addRule("test.key", 
                      [](const ConfigValue& value) { return !value.empty(); },
                      "Value must not be empty");
        
        std::cout << "  Schema setup test passed." << std::endl;
    }
    
    void testSchemaCreation() {
        std::cout << "Running Schema Creation Test..." << std::endl;
        
        ConfigSchema schema;
        
        // Add various types of validation rules
        schema.addIntRule("brain.neuron_count", {1, 100000}, true);
        schema.addFloatRule("brain.synapse_density", {0.0, 1.0});
        schema.addStringRule("test.mode", false, {"training", "inference", "validation"});
        schema.addBoolRule("enable_debug", false);
        
        // Verify all rules are added
        std::vector<std::string> requiredKeys = schema.getRequiredKeys();
        assert(std::find(requiredKeys.begin(), requiredKeys.end(), "brain.neuron_count") != requiredKeys.end());
        
        std::cout << "  Schema creation test passed." << std::endl;
    }
    
    void testValidationRules() {
        std::cout << "Running Validation Rules Test..." << std::endl;
        
        ConfigSchema schema;
        
        // Test integer validation
        schema.addIntRule("test.int_value", {-100, 100});
        
        ConfigEntry intEntry("test.int_value", ConfigValue(50), ConfigSource::Runtime);
        std::vector<ConfigValidationError> intErrors = schema.validate(intEntry);
        assert(intErrors.empty()); // 50 is within range [-100, 100]
        
        // Test out-of-range error
        ConfigEntry intEntry2("test.int_value", ConfigValue(200), ConfigSource::Runtime);
        std::vector<ConfigValidationError> intErrors2 = schema.validate(intEntry2);
        assert(!intErrors2.empty()); // 200 is out of range
        
        // Test float validation
        schema.addFloatRule("test.float_value", {-1.0, 1.0});
        
        ConfigEntry floatEntry("test.float_value", ConfigValue(0.5f), ConfigSource::Runtime);
        std::vector<ConfigValidationError> floatErrors = schema.validate(floatEntry);
        assert(floatErrors.empty()); // 0.5 is within range [-1.0, 1.0]
        
        std::cout << "  Validation rules test passed." << std::endl;
    }
    
    void testValidationResults() {
        std::cout << "Running Validation Results Test..." << std::endl;
        
        ConfigSchema schema;
        schema.addIntRule("required.int", {0, 100}, true);
        schema.addStringRule("optional.string", false);
        
        // Create a config for testing
        auto config = std::make_shared<Config>();
        config->set("required.int", 50);
        config->set("optional.string", "test_value");
        
        // Validate the config
        std::vector<std::pair<ConfigValidationError, std::string>> validationResults = schema.validate(*config);
        
        // Should have no errors
        assert(validationResults.empty());
        
        // Test missing required key
        auto config2 = std::make_shared<Config>();
        config2->set("optional.string", "test_value");
        
        std::vector<std::pair<ConfigValidationError, std::string>> validationResults2 = schema.validate(*config2);
        
        // Should have missing required key error
        bool hasMissingRequired = false;
        for (const auto& result : validationResults2) {
            if (result.first == ConfigValidationError::MissingRequired) {
                hasMissingRequired = true;
                break;
            }
        }
        assert(hasMissingRequired);
        
        std::cout << "  Validation results test passed." << std::endl;
    }
    
    void testRequiredKeys() {
        std::cout << "Running Required Keys Test..." << std::endl;
        
        ConfigSchema schema;
        schema.addIntRule("key1", {0, 10}, true);
        schema.addFloatRule("key2", {-1.0, 1.0}, true);
        schema.addStringRule("key3", false);
        
        std::vector<std::string> requiredKeys = schema.getRequiredKeys();
        
        assert(requiredKeys.size() == 2);
        assert(std::find(requiredKeys.begin(), requiredKeys.end(), "key1") != requiredKeys.end());
        assert(std::find(requiredKeys.begin(), requiredKeys.end(), "key2") != requiredKeys.end());
        assert(std::find(requiredKeys.begin(), requiredKeys.end(), "key3") == requiredKeys.end());
        
        std::cout << "  Required keys test passed." << std::endl;
    }
};

// Python Bindings Tests
class PythonBindingsTests {
public:
    void run() {
        testPythonConfigCreation();
        testPythonConfigValidation();
        testPythonErrorHandling();
        testPythonTypeChecking();
    }
    
private:
    void testPythonConfigCreation() {
        std::cout << "Running Python Config Creation Test..." << std::endl;
        
        // Test creating enhanced Python config
        PythonConfig config;
        
        // Set values
        config.set("brain.neuron_count", 1000);
        config.set("brain.synapse_density", 0.1f);
        config.set("enable_debug", true);
        
        // Verify values
        assert(config.has("brain.neuron_count"));
        assert(config.has("brain.synapse_density"));
        assert(config.has("enable_debug"));
        
        assert(config.getInt("brain.neuron_count") == 1000);
        assert(config.getFloat("brain.synapse_density") == 0.1f);
        assert(config.getBool("enable_debug") == true);
        
        std::cout << "  Python config creation test passed." << std::endl;
    }
    
    void testPythonConfigValidation() {
        std::cout << "Running Python Config Validation Test..." << std::endl;
        
        PythonConfig config;
        
        // Add validation schema
        config.addIntValidation("brain.neuron_count", {100, 10000});
        config.addFloatValidation("brain.synapse_density", {0.0, 1.0});
        config.addStringValidation("test.mode", false, {"training", "inference"});
        
        // Set valid values
        config.set("brain.neuron_count", 5000);
        config.set("brain.synapse_density", 0.05f);
        config.set("test.mode", "training");
        
        // Should validate without errors
        assert(config.validate());
        
        // Try invalid value
        try {
            config.set("brain.neuron_count", 50000); // Out of range
            assert(false); // Should have thrown exception
        } catch (const ValidationError& e) {
            // Expected
        }
        
        std::cout << "  Python config validation test passed." << std::endl;
    }
    
    void testPythonErrorHandling() {
        std::cout << "Running Python Error Handling Test..." << std::endl;
        
        PythonConfig config;
        
        // Add schema
        config.addIntValidation("test.key", {0, 10});
        
        // Test that valid values don't throw
        config.set("test.key", 5);
        assert(config.getInt("test.key") == 5);
        
        // Test that invalid values throw ValidationError
        bool validationErrorThrown = false;
        try {
            config.set("test.key", 15); // Out of range
        } catch (const ValidationError& e) {
            validationErrorThrown = true;
        }
        
        assert(validationErrorThrown);
        
        // Test ConfigError for missing key
        bool configErrorThrown = false;
        try {
            config.getInt("nonexistent.key");
        } catch (const ConfigError& e) {
            configErrorThrown = true;
        }
        
        assert(configErrorThrown);
        
        std::cout << "  Python error handling test passed." << std::endl;
    }
    
    void testPythonTypeChecking() {
        std::cout << "Running Python Type Checking Test..." << std::endl;
        
        PythonConfig config;
        
        // Test type-specific get methods
        config.set("int.key", 42);
        config.set("float.key", 3.14f);
        config.set("string.key", "hello");
        config.set("bool.key", true);
        
        // Type checking should work correctly
        assert(config.getInt("int.key") == 42);
        assert(config.getFloat("float.key") == 3.14f);
        assert(config.getString("string.key") == "hello");
        assert(config.getBool("bool.key") == true);
        
        // Type mismatch should throw ConfigError
        bool typeErrorThrown = false;
        try {
            config.getInt("float.key"); // Float value, expecting int
        } catch (const ConfigError& e) {
            typeErrorThrown = true;
        }
        
        assert(typeErrorThrown);
        
        std::cout << "  Python type checking test passed." << std::endl;
    }
};

// Main test runner
class NLMTestSuite {
public:
    void runAllTests() {
        std::cout << "========================================" << std::endl;
        std::cout << "Running Enhanced NLM Component Tests" << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << std::endl;
        
        // Run all test suites
        MemoryPoolTests memoryPoolTests;
        memoryPoolTests.run();
        
        std::cout << std::endl;
        
        ConfigValidationTests configValidationTests;
        configValidationTests.run();
        
        std::cout << std::endl;
        
        PythonBindingsTests pythonBindingsTests;
        pythonBindingsTests.run();
        
        std::cout << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << "All tests completed successfully!" << std::endl;
        std::cout << "========================================" << std::endl;
    }
};

} // namespace test

// Test main function
int main() {
    try {
        test::NLMTestSuite testSuite;
        testSuite.runAllTests();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}