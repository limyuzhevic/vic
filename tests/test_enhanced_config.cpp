// Enhanced Configuration Tests
// This test file demonstrates the enhanced configuration testing capabilities

#include "core/Config/ConfigEnhanced.hpp"
#include <iostream>
#include <cassert>

void testEnhancedConfigBasics() {
    std::cout << "Running enhanced configuration tests..." << std::endl;
    
    nlm::Config config;
    
    // Test basic operations
    config.setInt("test_int", 42);
    assert(config.getOr<int>("test_int", 0) == 42);
    
    config.setString("test_string", "hello world");
    assert(config.getOr<std::string>("test_string", "") == "hello world");
    
    config.setDouble("test_double", 3.14159);
    assert(config.getOr<double>("test_double", 0.0) == 3.14159);
    
    std::cout << "Enhanced configuration tests passed!" << std::endl;
}

void testSchemaValidation() {
    std::cout << "Running schema validation tests..." << std::endl;
    
    nlm::Config config;
    
    // Add schemas
    std::vector<nlm::ConfigSchema> schemas = {
        nlm::ConfigSchema("required_field", std::string(""), "Required field"),
        nlm::ConfigSchema("numeric_field", 0.0, "Numeric field"),
    };
    
    config.addDefaultSchema(schemas);
    
    // Should fail validation (missing required field)
    assert(!config.validateSchema());
    
    // Set required field
    config.setString("required_field", "test_value");
    config.setDouble("numeric_field", 123.45);
    
    // Should pass validation now
    assert(config.validateSchema());
    
    std::cout << "Schema validation tests passed!" << std::endl;
}

void testJSONImportExport() {
    std::cout << "Running JSON import/export tests..." << std::endl;
    
    nlm::Config config;
    config.setString("name", "Test Config");
    config.setInt("version", 1);
    config.setBool("enabled", true);
    
    // Export to JSON
    std::string jsonStr = config.toJSON();
    std::cout << "Exported JSON: " << jsonStr << std::endl;
    
    // Import from JSON
    nlm::Config config2;
    bool loaded = config2.importFromJSON(json::parse(jsonStr), nlm::ConfigSource::File);
    assert(loaded);
    
    assert(config2.getOr<std::string>("name", "") == "Test Config");
    assert(config2.getOr<int>("version", 0) == 1);
    assert(config2.getOr<bool>("enabled", false) == true);
    
    std::cout << "JSON import/export tests passed!" << std::endl;
}

void testPerformanceMonitoring() {
    std::cout << "Running performance monitoring tests..." << std::endl;
    
    nlm::Config config;
    
    // Add performance counters
    config.addCounter("operation_time");
    
    // Simulate some work
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 1000; ++i) {
        config.setInt("temp", i);
    }
    auto end = std::chrono::high_resolution_clock::now();
    
    double elapsed = std::chrono::duration<double>(end - start).count();
    config.updateMetric("operation_time", elapsed);
    
    // Get metrics
    auto metrics = config.getMetrics();
    assert(metrics.totalTime > 0.0);
    
    std::cout << "Performance monitoring tests passed!" << std::endl;
}

void testEnvironmentVariables() {
    std::cout << "Running environment variable tests..." << std::endl;
    
    // Set test environment variable
    setenv("NLM_TEST_CONFIG_VALUE", "from_env", 1);
    
    nlm::Config config;
    bool loaded = config.loadFromEnvironment("NLM_");
    
    if (loaded) {
        assert(config.has("test_config_value"));
        auto value = config.get<std::string>("test_config_value");
        assert(value.has_value());
        assert(*value == "from_env");
    }
    
    // Clean up
    unsetenv("NLM_TEST_CONFIG_VALUE");
    
    std::cout << "Environment variable tests passed!" << std::endl;
}

void testConfigurationMerge() {
    std::cout << "Running configuration merge tests..." << std::endl;
    
    nlm::Config config1;
    config1.setString("common", "from_config1");
    config1.setInt("unique1", 100);
    
    nlm::Config config2;
    config2.setString("common", "from_config2");  // Should override
    config2.setInt("unique2", 200);
    
    // Merge config2 into config1
    config1.merge(config2, nlm::ConfigSource::File);
    
    // Verify merge results
    assert(config1.getOr<std::string>("common", "") == "from_config2");
    assert(config1.getOr<int>("unique1", 0) == 100);
    assert(config1.getOr<int>("unique2", 0) == 200);
    
    std::cout << "Configuration merge tests passed!" << std::endl;
}

void testValidationErrors() {
    std::cout << "Running validation error tests..." << std::endl;
    
    nlm::Config config;
    
    // Add schema with validation
    auto customValidator = [](const nlm::ConfigValue& value) -> nlm::ValidationResult {
        if (std::holds_alternative<int>(value) && std::get<int>(value) < 0) {
            return nlm::ValidationResult::error("Value must be non-negative");
        }
        return nlm::ValidationResult::success();
    };
    
    std::vector<nlm::ConfigSchema> schemas = {
        nlm::ConfigSchema("positive_int", 0, "Must be positive"),
        nlm::ConfigSchema("custom_range", 0, "Custom range"),
    };
    
    // Add custom validator
    for (auto& schema : schemas) {
        if (schema.key == "custom_range") {
            schema.validator = customValidator;
        }
    }
    
    config.addDefaultSchema(schemas);
    
    // Test validation
    config.setInt("positive_int", -5);  // Invalid
    auto validationResults = config.validate();
    
    // Should have validation errors
    bool hasErrors = false;
    for (const auto& result : validationResults) {
        if (!result.valid) {
            hasErrors = true;
            break;
        }
    }
    assert(hasErrors);
    
    // Fix validation
    config.remove("positive_int");
    config.setInt("positive_int", 5);  // Valid
    assert(config.validateSchema());  // Should pass now
    
    std::cout << "Validation error tests passed!" << std::endl;
}

int main() {
    std::cout << "=== Enhanced Configuration Tests ===" << std::endl << std::endl;
    
    testEnhancedConfigBasics();
    testSchemaValidation();
    testJSONImportExport();
    testPerformanceMonitoring();
    testEnvironmentVariables();
    testConfigurationMerge();
    testValidationErrors();
    
    std::cout << std::endl;
    std::cout << "=== All Enhanced Configuration Tests Passed Successfully ===" << std::endl;
    
    return 0;
}