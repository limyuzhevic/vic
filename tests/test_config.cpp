// Enhanced Config Tests for ConfigEnhanced.hpp
// Comprehensive testing of the new enhanced configuration system

#include "core/Config/ConfigEnhanced.hpp"
#include <cassert>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace test_config {

void testEnhancedConfigCreation() {
    nlm::Config config;
    assert(!config.has("test"));
    assert(config.getEntryCount() == 0);
    
    std::cout << "    testEnhancedConfigCreation passed" << std::endl;
}

void testEnhancedConfigSetAndGet() {
    nlm::Config config;
    
    // Test basic types
    config.setInt("int_value", 42);
    assert(config.has("int_value"));
    
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    config.setDouble("double_value", 3.14);
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value());
    assert(*doubleVal > 3.13 && *doubleVal < 3.15);
    
    config.setString("string_value", std::string("hello"));
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value());
    assert(*stringVal == "hello");
    
    config.setBool("bool_value", true);
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    
    // Test enhanced set methods
    config.setList("string_list", std::vector<std::string>{"a", "b", "c"});
    
    std::cout << "    testEnhancedConfigSetAndGet passed" << std::endl;
}

void testEnhancedConfigGetOr() {
    nlm::Config config;
    
    int val = config.getOr<int>("missing", 100);
    assert(val == 100);
    
    config.setInt("existing", 50);
    val = config.getOr<int>("existing", 100);
    assert(val == 50);
    
    std::cout << "    testEnhancedConfigGetOr passed" << std::endl;
}

void testEnhancedConfigSchema() {
    nlm::Config config;
    
    // Add schemas
    std::vector<nlm::ConfigSchema> schemas = {
        nlm::ConfigSchema("required_int", 0, "Required integer"),
        nlm::ConfigSchema("optional_string", std::string("default"), "Optional string"),
        nlm::ConfigSchema("test_range", 0.0, "Test range"),
    };
    
    config.addDefaultSchema(schemas);
    
    // Test validation - should pass for required
    config.setInt("required_int", 42);
    assert(config.validateSchema());
    
    // Test validation - should fail for missing required
    nlm::Config config2;
    config2.addDefaultSchema(schemas);
    assert(!config2.validateSchema()); // Missing required_int
    
    // Add all required values
    config2.setInt("required_int", 42);
    config2.setString("optional_string", "custom");
    assert(config2.validateSchema());
    
    std::cout << "    testEnhancedConfigSchema passed" << std::endl;
}

void testEnhancedConfigJSON() {
    nlm::Config config;
    
    // Set some values
    config.setInt("random_seed", 12345);
    config.setDouble("simulation_timestep", 0.001);
    config.setString("mode", "phase6");
    
    // Export to JSON
    std::string jsonStr = config.toJSON();
    
    // Parse back
    json j = json::parse(jsonStr);
    
    assert(j.contains("random_seed"));
    assert(j["random_seed"] == 12345);
    assert(j.contains("simulation_timestep"));
    assert(j["simulation_timestep"] == 0.001);
    assert(j.contains("mode"));
    assert(j["mode"] == "phase6");
    
    // Import from JSON
    nlm::Config config2;
    assert(config2.importFromJSON(j, nlm::ConfigSource::File));
    assert(config2.getOr<int>("random_seed", 0) == 12345);
    
    std::cout << "    testEnhancedConfigJSON passed" << std::endl;
}

void testEnhancedConfigEnvironment() {
    // Set environment variable
    setenv("NLM_TEST_ENV_VAR", "env_value", 1);
    
    nlm::Config config;
    
    // Load from environment
    bool loaded = config.loadFromEnvironment("NLM_");
    assert(loaded);
    
    // Check if our environment variable was loaded
    bool found = false;
    auto keys = config.getKeys();
    for (const auto& key : keys) {
        if (key == "test_env_var") {
            found = true;
            auto value = config.get<std::string>(key);
            assert(value.has_value());
            assert(*value == "env_value");
            break;
        }
    }
    assert(found);
    
    // Clean up
    unsetenv("NLM_TEST_ENV_VAR");
    
    std::cout << "    testEnhancedConfigEnvironment passed" << std::endl;
}

void testEnhancedConfigImportExport() {
    nlm::Config config;
    
    // Set various types of values
    config.setInt("int_val", 100);
    config.setDouble("double_val", 3.14159);
    config.setString("string_val", "test");
    config.setBool("bool_val", true);
    config.setList("string_list", std::vector<std::string>{"a", "b", "c"});
    
    // Test save to file
    bool saved = config.saveToFile("test_config.json", true);
    assert(saved);
    
    // Load from file
    nlm::Config config2;
    bool loaded = config2.loadFromFile("test_config.json");
    assert(loaded);
    
    // Verify values match
    assert(config2.getOr<int>("int_val", 0) == 100);
    assert(config2.getOr<double>("double_val", 0.0) == 3.14159);
    assert(config2.getOr<std::string>("string_val", "") == "test");
    assert(config2.getOr<bool>("bool_val", false) == true);
    
    // Clean up
    std::remove("test_config.json");
    
    std::cout << "    testEnhancedConfigImportExport passed" << std::endl;
}

void testEnhancedConfigMerge() {
    nlm::Config config1;
    config1.setInt("key1", 100);
    config1.setString("key2", "value1");
    
    nlm::Config config2;
    config2.setInt("key2", 200);  // Overwrite
    config2.setString("key3", "value3");
    
    // Merge config2 into config1
    config1.merge(config2, nlm::ConfigSource::File);
    
    // Verify merged values
    assert(config1.getOr<int>("key1", 0) == 100);  // Original preserved
    assert(config1.getOr<int>("key2", 0) == 200);  // Updated by merge
    assert(config1.getOr<std::string>("key3", "") == "value3");  // Added by merge
    
    std::cout << "    testEnhancedConfigMerge passed" << std::endl;
}

void testEnhancedConfigValidationErrors() {
    nlm::Config config;
    
    // Add a schema with validation
    std::vector<nlm::ConfigSchema> schemas = {
        nlm::ConfigSchema("positive_int", 0, "Must be positive"),
        nlm::ConfigSchema("allowed_value", std::string(""), "Must be a or b"),
        nlm::ConfigSchema("range_test", 0.0, "Value must be between 1 and 10"),
    };
    
    // Custom validator for range
    auto rangeValidator = [](const nlm::ConfigValue& value) -> nlm::ValidationResult {
        double val;
        if (std::holds_alternative<double>(value)) {
            val = std::get<double>(value);
        } else if (std::holds_alternative<int>(value)) {
            val = static_cast<double>(std::get<int>(value));
        } else {
            return nlm::ValidationResult::error("Invalid type for range test");
        }
        
        if (val >= 1.0 && val <= 10.0) {
            return nlm::ValidationResult::success();
        }
        return nlm::ValidationResult::error("Value must be between 1 and 10");
    };
    
    // Add validator to the range_test schema
    for (auto& schema : schemas) {
        if (schema.key == "range_test") {
            schema.validator = rangeValidator;
        }
    }
    
    config.addDefaultSchema(schemas);
    
    // Test various validation scenarios
    std::vector<nlm::ValidationResult> results;
    
    // Missing required
    results = config.validate();
    bool hasMissingError = false;
    for (const auto& result : results) {
        if (!result.valid && result.errorMessage.find("Required") != std::string::npos) {
            hasMissingError = true;
            break;
        }
    }
    assert(hasMissingError);
    
    // Valid values
    config.setInt("positive_int", 5);
    config.setString("allowed_value", "a");
    config.setDouble("range_test", 5.0);
    results = config.validate();
    
    // All valid
    bool allValid = true;
    for (const auto& result : results) {
        if (!result.valid) {
            allValid = false;
            break;
        }
    }
    assert(allValid);
    
    // Invalid range
    config.remove("positive_int");
    config.remove("allowed_value");
    config.remove("range_test");
    
    // Test specific validation
    config.setInt("positive_int", -5);  // Invalid range
    auto valResult = config.validateValue(schemas[0], nlm::ConfigValue(-5), "positive_int");
    assert(!valResult.valid);
    
    std::cout << "    testEnhancedConfigValidationErrors passed" << std::endl;
}

void testEnhancedConfigPerformanceMonitor() {
    nlm::Config config;
    
    // Add performance monitoring counters
    config.addCounter("config_load_time");
    config.addCounter("config_save_time");
    
    // Simulate some operations with timing
    auto start = std::chrono::high_resolution_clock::now();
    config.setInt("test_key", 42);
    auto end = std::chrono::high_resolution_clock::now();
    
    double elapsed = std::chrono::duration<double>(end - start).count();
    
    // Update custom metrics
    config.updateMetric("config_load_time", elapsed);
    
    // Get metrics
    auto metrics = config.getMetrics();
    assert(metrics.totalTime > 0.0);
    
    std::cout << "    testEnhancedConfigPerformanceMonitor passed" << std::endl;
}

void runAllEnhanced() {
    testEnhancedConfigCreation();
    testEnhancedConfigSetAndGet();
    testEnhancedConfigGetOr();
    testEnhancedConfigSchema();
    testEnhancedConfigJSON();
    testEnhancedConfigEnvironment();
    testEnhancedConfigImportExport();
    testEnhancedConfigMerge();
    testEnhancedConfigValidationErrors();
    testEnhancedConfigPerformanceMonitor();
    
    std::cout << std::endl;
    std::cout << "=== All Enhanced Config Tests Passed ===" << std::endl;
}

} // namespace test_config