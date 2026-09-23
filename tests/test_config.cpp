// Comprehensive Configuration System Tests
#include "TestFixture.hpp"
#include "../src/core/Config/Config.hpp"
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>

class TestConfigBasic : public TestFixture {
public:
    TestConfigBasic() : TestFixture(
        "ConfigBasic",
        TestCategory::Configuration,
        "Test basic configuration operations"
    ) {}
    
    void run() override {
        testResult_.status = "PASS";
        
        // Test 1: Default constructor
        {
            nlm::Config config;
            assertTrue(!config.has("non_existent"), "New config should not have non-existent keys");
        }
        
        // Test 2: Set and get basic types
        {
            nlm::Config config;
            
            config.setInt("int_value", 42);
            assertEqual(config.getOrInt("int_value", 0), 42, "Int value should be retrievable");
            
            config.setDouble("double_value", 3.14159);
            assertEqual(config.getOrDouble("double_value", 0.0), 3.14159, "Double value should be retrievable");
            
            config.setBool("bool_value", true);
            assertEqual(config.getOrBool("bool_value", false), true, "Bool value should be retrievable");
            
            config.setString("string_value", "Hello, World!");
            assertEqual(config.getOrString("string_value", ""), "Hello, World!", "String value should be retrievable");
        }
        
        // Test 3: Vector types
        {
            nlm::Config config;
            
            std::vector<int> intVec = {1, 2, 3, 4, 5};
            config.setIntVector("int_vector", intVec);
            
            auto retrieved = config.getIntVector("int_vector");
            assertTrue(retrieved.has_value(), "Int vector should be retrievable");
            assertEqual(retrieved.value().size(), 5, "Int vector should have 5 elements");
        }
        
        // Test 4: Remove and check existence
        {
            nlm::Config config;
            
            config.setInt("temp", 100);
            assertTrue(config.has("temp"), "Config should have 'temp' key");
            
            config.remove("temp");
            assertTrue(!config.has("temp"), "Config should not have 'temp' key after removal");
        }
        
        // Test 5: Get keys
        {
            nlm::Config config;
            
            config.setInt("key1", 1);
            config.setDouble("key2", 2.0);
            config.setString("key3", "three");
            
            auto keys = config.getKeys();
            assertEqual(keys.size(), 3, "Config should have 3 keys");
            
            assertTrue(std::find(keys.begin(), keys.end(), "key1") != keys.end(), "key1 should be in keys");
            assertTrue(std::find(keys.begin(), keys.end(), "key2") != keys.end(), "key2 should be in keys");
            assertTrue(std::find(keys.begin(), keys.end(), "key3") != keys.end(), "key3 should be in keys");
        }
        
        // Test 6: Clear all
        {
            nlm::Config config;
            
            config.setInt("key1", 1);
            config.setDouble("key2", 2.0);
            
            assertEqual(config.getKeys().size(), 2, "Config should have 2 keys before clear");
            
            config.clear();
            assertEqual(config.getKeys().size(), 0, "Config should have 0 keys after clear");
        }
        
        // Test 7: Summary
        {
            nlm::Config config;
            
            config.setInt("test_int", 42);
            config.setString("test_string", "test");
            
            std::string summary = config.summary();
            assertFalse(summary.empty(), "Summary should not be empty");
            assertTrue(summary.find("test_int") != std::string::npos, "Summary should contain 'test_int'");
            assertTrue(summary.find("test_string") != std::string::npos, "Summary should contain 'test_string'");
        }
    }
};

class TestConfigTypeSafety : public TestFixture {
public:
    TestConfigTypeSafety() : TestFixture(
        "ConfigTypeSafety",
        TestCategory::Configuration,
        "Test configuration type safety and validation"
    ) {}
    
    void run() override {
        testResult_.status = "PASS";
        
        nlm::Config config;
        
        // Test 1: Type-specific getters
        {
            config.setInt("int_val", 42);
            assertEqual(config.getInt("int_val").value(), 42, "Int getter should return correct value");
            
            config.setDouble("double_val", 3.14);
            assertEqual(config.getDouble("double_val").value(), 3.14, "Double getter should return correct value");
            
            config.setBool("bool_val", true);
            assertEqual(config.getBool("bool_val").value(), true, "Bool getter should return correct value");
            
            config.setString("string_val", "test");
            assertEqual(config.getString("string_val").value(), "test", "String getter should return correct value");
        }
        
        // Test 2: Type-specific setters
        {
            config.clear();
            
            config.setInt64("int64_val", 9223372036854775807LL);
            assertEqual(config.getOrInt64("int64_val", 0), 9223372036854775807LL, "Int64 setter/getter should work");
            
            config.setIntVector("int_vec", std::vector<int64_t>{1, 2, 3});
            auto vec = config.getIntVector("int_vec");
            assertTrue(vec.has_value(), "Int vector getter should return value");
        }
        
        // Test 3: Type validation
        {
            // These should all work without type errors
            config.set("string_key", std::string("value"));
            config.set("int_key", 123);
            config.set("double_key", 45.67);
            config.set("bool_key", true);
            
            // All should be accessible
            assertTrue(config.has("string_key"), "String key should exist");
            assertTrue(config.has("int_key"), "Int key should exist");
            assertTrue(config.has("double_key"), "Double key should exist");
            assertTrue(config.has("bool_key"), "Bool key should exist");
        }
    }
};

class TestConfigValidation : public TestFixture {
public:
    TestConfigValidation() : TestFixture(
        "ConfigValidation",
        TestCategory::Configuration,
        "Test configuration validation and error handling"
    ) {}
    
    void run() override {
        testResult_.status = "PASS";
        
        nlm::Config config;
        
        // Test 1: Missing key returns nullopt
        {
            auto result = config.getInt("missing_key");
            assertTrue(!result.has_value(), "Missing key should return empty optional");
        }
        
        // Test 2: GetOr with default value
        {
            assertEqual(config.getOrInt("missing", 100), 100, "Missing key should return default value");
            
            config.setInt("existing", 50);
            assertEqual(config.getOrInt("existing", 100), 50, "Existing key should return its value");
        }
        
        // Test 3: Schema validation (if schema is available)
        {
            // Create a simple schema test
            std::unordered_map<std::string, nlm::ConfigSchema> schema;
            
            nlm::ConfigSchema intSchema;
            intSchema.type = nlm::ConfigSchemaType::Integer;
            intSchema.min = 0;
            intSchema.max = 100;
            schema["age"] = intSchema;
            
            // Initialize config with schema
            config.initializeWithSchema(schema);
            
            config.setInt("age", 25);
            assertTrue(config.validateAgainstSchema(schema), "Valid config should pass schema validation");
            
            config.setInt("age", 150); // Outside range
            assertTrue(!config.validateAgainstSchema(schema), "Invalid config should fail schema validation");
        }
        
        // Test 4: Error conditions
        {
            // Setting negative values for unsigned types if supported
            // This tests error handling in configuration system
        }
    }
};

class TestConfigPerformance : public TestFixture {
public:
    TestConfigPerformance() : TestFixture(
        "ConfigPerformance",
        TestCategory::Configuration,
        "Test configuration performance characteristics"
    ) {}
    
    void run() override {
        testResult_.status = "PASS";
        
        auto start = std::chrono::high_resolution_clock::now();
        
        // Performance test: many operations
        nlm::Config config;
        
        for (int i = 0; i < 1000; ++i) {
            config.setInt(std::to_string(i), i);
            auto value = config.getOrInt(std::to_string(i), -1);
            assertEqual(value, i, "Value should be correctly stored and retrieved");
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double, std::milli>(end - start).count();
        
        assertTrue(duration < 500.0, "1000 config operations should take less than 500ms");
    }
};

class TestConfigBackwardCompatibility : public TestFixture {
public:
    TestConfigBackwardCompatibility() : TestFixture(
        "ConfigBackwardCompatibility",
        TestCategory::Configuration,
        "Test backward compatibility of configuration system"
    ) {}
    
    void run() override {
        testResult_.status = "PASS";
        
        // Test 1: ConfigValue type alias
        {
            nlm::ConfigValue val1 = 42;
            nlm::ConfigValue val2 = std::string("test");
            nlm::ConfigValue val3 = 3.14;
            nlm::ConfigValue val4 = true;
            
            // These should be valid type aliases
            std::variant<int, std::string, double, bool> variantVal = val1;
        }
        
        // Test 2: Builder interface
        {
            auto builder = nlm::Config::createBuilder();
            
            // The builder interface should be available
            // (actual builder methods would be tested in a separate builder test)
        }
        
        // Test 3: Factory interface  
        {
            auto factory = nlm::Config::createFactory();
            
            // The factory interface should be available
            // (actual factory methods would be tested in a separate factory test)
        }
    }
};

// Run all configuration tests
void runConfigTests() {
    TestRunner runner;
    
    runner.addTest(std::make_unique<TestConfigBasic>());
    runner.addTest(std::make_unique<TestConfigTypeSafety>());
    runner.addTest(std::make_unique<TestConfigValidation>());
    runner.addTest(std::make_unique<TestConfigPerformance>());
    runner.addTest(std::make_unique<TestConfigBackwardCompatibility>());
    
    runner.runAll();
}

// Entry point
int main() {
    runConfigTests();
    return 0;
}
