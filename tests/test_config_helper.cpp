#include "core/TestFramework/TestFramework.hpp"
#include "core/Config/ConfigHelper.hpp"
#include "core/ErrorHandling/ErrorHandling.hpp"
#include <cassert>
#include <iostream>
#include <string>
#include <vector>

namespace test_config_helper {

class ConfigHelperTestSuite : public TestSuite {
public:
    std::string getName() const override {
        return "ConfigHelper";
    }
    
    std::vector<TestResult> runTests() override {
        std::vector<TestResult> results;
        
        auto start = std::chrono::high_resolution_clock::now();
        
        try {
            // Test 1: Default configuration creation
            testDefaultConfig();
            results.emplace_back("DefaultConfig", true, "Default configuration created", getDuration(start));
        } catch (const std::exception& e) {
            results.emplace_back("DefaultConfig", false, std::string("Exception: ") + e.what(), getDuration(start), "std::exception", 0);
        }
        
        start = std::chrono::high_resolution_clock::now();
        try {
            // Test 2: Loading from JSON
            testLoadFromJSON();
            results.emplace_back("LoadFromJSON", true, "JSON loading successful", getDuration(start));
        } catch (const std::exception& e) {
            results.emplace_back("LoadFromJSON", false, std::string("Exception: ") + e.what(), getDuration(start), "std::exception", 0);
        }
        
        start = std::chrono::high_resolution_clock::now();
        try {
            // Test 3: Setting and getting values
            testSetAndGet();
            results.emplace_back("SetAndGet", true, "Set and get operations successful", getDuration(start));
        } catch (const std::exception& e) {
            results.emplace_back("SetAndGet", false, std::string("Exception: ") + e.what(), getDuration(start), "std::exception", 0);
        }
        
        start = std::chrono::high_resolution_clock::now();
        try {
            // Test 4: Key existence
            testKeyExistence();
            results.emplace_back("KeyExistence", true, "Key existence checks passed", getDuration(start));
        } catch (const std::exception& e) {
            results.emplace_back("KeyExistence", false, std::string("Exception: ") + e.what(), getDuration(start), "std::exception", 0);
        }
        
        start = std::chrono::high_resolution_clock::now();
        try {
            // Test 5: Configuration to JSON
            testToJSON();
            results.emplace_back("ToJSON", true, "JSON conversion successful", getDuration(start));
        } catch (const std::exception& e) {
            results.emplace_back("ToJSON", false, std::string("Exception: ") + e.what(), getDuration(start), "std::exception", 0);
        }
        
        return results;
    }
    
private:
    double getDuration(std::chrono::high_resolution_clock::time_point start) {
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double>(end - start).count();
    }
    
    void testDefaultConfig() {
        nlm::ConfigHelper config = nlm::ConfigHelper::createDefaultConfig();
        ASSERT_TRUE(config.has("brain.neuron.count"));
        ASSERT_TRUE(config.has("brain.simulation.dt"));
        ASSERT_TRUE(config.has("memory.working.capacity"));
        
        auto neuronCount = config.get<int>("brain.neuron.count");
        ASSERT_TRUE(neuronCount.has_value());
        ASSERT_EQ(*neuronCount, 10000);
    }
    
    void testLoadFromJSON() {
        std::string json = "{\"brain.neuron.count\": 15000, \"debug.enabled\": true}";
        nlm::ConfigHelper config;
        
        bool result = config.loadFromString(json);
        ASSERT_TRUE(result);
        
        auto neuronCount = config.get<int>("brain.neuron.count");
        ASSERT_TRUE(neuronCount.has_value());
        ASSERT_EQ(*neuronCount, 15000);
        
        auto debugEnabled = config.get<bool>("debug.enabled");
        ASSERT_TRUE(debugEnabled.has_value());
        ASSERT_TRUE(*debugEnabled);
    }
    
    void testSetAndGet() {
        nlm::ConfigHelper config;
        
        config.set("test.int", 42);
        auto intVal = config.get<int>("test.int");
        ASSERT_TRUE(intVal.has_value());
        ASSERT_EQ(*intVal, 42);
        
        config.set("test.double", 3.14159);
        auto doubleVal = config.get<double>("test.double");
        ASSERT_TRUE(doubleVal.has_value());
        ASSERT_TRUE(std::abs(*doubleVal - 3.14159) < 0.0001);
        
        config.set("test.string", std::string("hello"));
        auto stringVal = config.get<std::string>("test.string");
        ASSERT_TRUE(stringVal.has_value());
        ASSERT_EQ(*stringVal, "hello");
        
        config.set("test.bool", true);
        auto boolVal = config.get<bool>("test.bool");
        ASSERT_TRUE(boolVal.has_value());
        ASSERT_TRUE(*boolVal);
    }
    
    void testKeyExistence() {
        nlm::ConfigHelper config;
        
        // Test non-existent key
        ASSERT_FALSE(config.has("nonexistent.key"));
        
        // Test existing key
        config.set("existent.key", "value");
        ASSERT_TRUE(config.has("existent.key"));
        
        // Test with default config
        nlm::ConfigHelper defaultConfig = nlm::ConfigHelper::createDefaultConfig();
        ASSERT_TRUE(defaultConfig.has("brain.neuron.count"));
        ASSERT_FALSE(defaultConfig.has("nonexistent.key"));
    }
    
    void testToJSON() {
        nlm::ConfigHelper config;
        config.set("test.value", 123);
        config.set("test.name", std::string("test"));
        
        std::string json = config.toJSON(true); // Pretty print
        ASSERT_FALSE(json.empty());
        
        // Verify JSON contains expected content
        ASSERT_TRUE(json.find("test.value") != std::string::npos);
        ASSERT_TRUE(json.find("123") != std::string::npos);
        ASSERT_TRUE(json.find("test.name") != std::string::npos);
    }
};

class ErrorHandlingTestSuite : public TestSuite {
public:
    std::string getName() const override {
        return "ErrorHandling";
    }
    
    std::vector<TestResult> runTests() override {
        std::vector<TestResult> results;
        
        auto start = std::chrono::high_resolution_clock::now();
        
        try {
            // Test 1: Exception throwing
            testExceptionThrowing();
            results.emplace_back("ExceptionThrowing", true, "Exception handling test passed", getDuration(start));
        } catch (const std::exception& e) {
            results.emplace_back("ExceptionThrowing", false, std::string("Exception: ") + e.what(), getDuration(start), "std::exception", 0);
        }
        
        start = std::chrono::high_resolution_clock::now();
        try {
            // Test 2: Error codes
            testErrorCodes();
            results.emplace_back("ErrorCodes", true, "Error code handling test passed", getDuration(start));
        } catch (const std::exception& e) {
            results.emplace_back("ErrorCodes", false, std::string("Exception: ") + e.what(), getDuration(start), "std::exception", 0);
        }
        
        return results;
    }
    
private:
    double getDuration(std::chrono::high_resolution_clock::time_point start) {
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double>(end - start).count();
    }
    
    void testExceptionThrowing() {
        // Test that specific exceptions are thrown for specific error codes
        try {
            NLM_THROW_ERROR(nlm::NLMErrors::ConfigFileNotFound, "Test error");
            ASSERT_TRUE(false); // Should not reach here
        } catch (const nlm::NLMException& e) {
            ASSERT_TRUE(e.getErrorCode() == nlm::NLMErrors::ConfigFileNotFound);
        }
        
        try {
            NLM_THROW_ERROR(nlm::NLMErrors::MemoryAllocationFailed, "Test memory error");
            ASSERT_TRUE(false); // Should not reach here
        } catch (const nlm::NLMException& e) {
            ASSERT_TRUE(e.getErrorCode() == nlm::NLMErrors::MemoryAllocationFailed);
        }
    }
    
    void testErrorCodes() {
        // Test error code to string conversion
        ASSERT_TRUE(static_cast<int>(nlm::NLMErrors::ConfigFileNotFound) == 1);
        ASSERT_TRUE(static_cast<int>(nlm::NLMErrors::MemoryAllocationFailed) == 4);
        ASSERT_TRUE(static_cast<int>(nlm::NLMErrors::UnknownError) == 999);
    }
};

class AssertionsTestSuite : public TestSuite {
public:
    std::string getName() const override {
        return "Assertions";
    }
    
    std::vector<TestResult> runTests() override {
        std::vector<TestResult> results;
        
        auto start = std::chrono::high_resolution_clock::now();
        
        try {
            // Test 1: Basic assertions
            testBasicAssertions();
            results.emplace_back("BasicAssertions", true, "Basic assertion tests passed", getDuration(start));
        } catch (const std::exception& e) {
            results.emplace_back("BasicAssertions", false, std::string("Exception: ") + e.what(), getDuration(start), "std::exception", 0);
        }
        
        return results;
    }
    
private:
    double getDuration(std::chrono::high_resolution_clock::time_point start) {
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double>(end - start).count();
    }
    
    void testBasicAssertions() {
        // Test various assertion functions
        int a = 5;
        int b = 10;
        
        // Test equals
        ASSERT_EQ(a, 5);
        ASSERT_NE(a, b);
        
        // Test comparison
        ASSERT_TRUE(a < b);
        ASSERT_TRUE(a <= 5);
        ASSERT_TRUE(a > 0);
        ASSERT_TRUE(a >= 5);
        
        // Test boolean assertions
        bool flag = true;
        ASSERT_TRUE(flag);
        
        flag = false;
        ASSERT_FALSE(flag);
        
        // Test range
        double value = 7.5;
        ASSERT_RANGE(value, 0.0, 10.0);
        
        // Test container
        std::vector<int> vec = {1, 2, 3};
        ASSERT_NOT_EMPTY(vec);
        ASSERT_SIZE(vec, 3);
        
        // Test pointer
        int* ptr = new int(42);
        ASSERT_NOT_NULL(ptr);
        delete ptr;
        
        ASSERT_NULL(nullptr);
    }
};

} // namespace test_config_helper

// Test runner
int main() {
    nlm::TestRunner runner;
    
    // Add test suites
    runner.addSuite(std::make_unique<test_config_helper::ConfigHelperTestSuite>());
    runner.addSuite(std::make_unique<test_config_helper::ErrorHandlingTestSuite>());
    runner.addSuite(std::make_unique<test_config_helper::AssertionsTestSuite>());
    
    // Run all tests
    auto results = runner.runAllTests();
    
    // Print results
    runner.printResults(results);
    
    // Exit with error code if any tests failed
    if (runner.getFailedTests() > 0) {
        return 1;
    }
    
    return 0;
}
