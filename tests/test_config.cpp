// Configuration Tests - Comprehensive validation and schema testing
#include "core/Config/Config.hpp"
#include "TestFramework.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <regex>

namespace test_config {

// Configuration schema validation test
class ConfigSchemaValidationTest : public test_framework::TestBase {
public:
    ConfigSchemaValidationTest(const std::string& name)
        : TestBase(name, test_framework::TestCategory::Configuration) {}
    
    void run() override {
        nlm::ConfigSchema schema;
        
        // Add validation rules
        schema.addRequired("required_key", "This key is required!")
              .addEnum("status", ["active", "inactive", "pending"], "Invalid status value")
              .addRegex("email", r"^[^@]+@[^@]+\\.[^@]+$", "Invalid email format")
              .addRange("port", 1.0, 65535.0, "Port must be between 1-65535")
              .addDependency("api_key", "service_url", "API key requires service URL");
        
        nlm::Config config;
        config.setSchema(std::make_shared<nlm::ConfigSchema>(schema));
        
        // Test with valid configuration
        config.set("required_key", "value1");
        config.set("status", "active");
        config.set("email", "user@example.com");
        config.set("port", 8080);
        config.set("service_url", "http://example.com");
        
        std::vector<nlm::ConfigError> errors = config.getSchema()->validate(config);
        assert(errors.empty());  // Should pass validation
        
        // Test with invalid configuration (missing required key)
        nlm::Config config2;
        config2.setSchema(std::make_shared<nlm::ConfigSchema>(schema));
        config2.set("status", "active");
        config2.set("email", "user@example.com");
        
        errors = config2.getSchema()->validate(config2);
        assert(!errors.empty());
        assert(errors.size() >= 1);
        
        // Test with invalid enum value
        nlm::Config config3;
        config3.setSchema(std::make_shared<nlm::ConfigSchema>(schema));
        config3.set("required_key", "value1");
        config3.set("status", "invalid_status");
        
        errors = config3.getSchema()->validate(config3);
        assert(!errors.empty());
        bool foundEnumError = false;
        for (const auto& error : errors) {
            if (error.getCategory() == nlm::ConfigErrorCategory::ConstraintViolation) {
                foundEnumError = true;
                break;
            }
        }
        assert(foundEnumError);
        
        // Test with invalid regex
        nlm::Config config4;
        config4.setSchema(std::make_shared<nlm::ConfigSchema>(schema));
        config4.set("required_key", "value1");
        config4.set("email", "invalid_email");
        
        errors = config4.getSchema()->validate(config4);
        assert(!errors.empty());
        
        // Test with out of range value
        nlm::Config config5;
        config5.setSchema(std::make_shared<nlm::ConfigSchema>(schema));
        config5.set("required_key", "value1");
        config5.set("port", 70000);
        
        errors = config5.getSchema()->validate(config5);
        assert(!errors.empty());
        
        std::cout << "    Config schema validation test for " << getName() << " passed" << std::endl;
    }
};

// Typed configuration validation test
class TypedConfigValidationTest : public test_framework::TestBase {
public:
    TypedConfigValidationTest(const std::string& name)
        : TestBase(name, test_framework::TestCategory::Configuration) {}
    
    void run() override {
        nlm::Config config;
        
        // Test type-safe configuration
        config.set("int_value", 42);
        config.set("float_value", 3.14159);
        config.set("string_value", "hello");
        config.set("bool_value", true);
        config.set("list_value", std::vector<int>{1, 2, 3, 4, 5});
        
        // Test type-safe retrieval
        auto intVal = config.getTyped<int>("int_value");
        assert(intVal.has_value());
        assert(intVal->getValue() == 42);
        assert(intVal->getKey() == "int_value");
        
        auto floatVal = config.getTyped<double>("float_value");
        assert(floatVal.has_value());
        assert(floatVal->getValue() == 3.14159);
        
        auto stringVal = config.getTyped<std::string>("string_value");
        assert(stringVal.has_value());
        assert(stringVal->getValue() == "hello");
        
        auto boolVal = config.getTyped<bool>("bool_value");
        assert(boolVal.has_value());
        assert(boolVal->getValue() == true);
        
        auto listVal = config.getTyped<std::vector<int>>("list_value");
        assert(listVal.has_value());
        assert(listVal->getValue().size() == 5);
        
        // Test type conversion utilities
        std::string intToString = nlm::Config::valueToString(nlm::Config::typeToValue(123));
        assert(intToString.find("123") != std::string::npos);
        
        assert(nlm::Config::canConvertToType<int>(nlm::Config::typeToValue(123)));
        assert(nlm::Config::canConvertToType<std::string>(nlm::Config::typeToValue("test")));
        
        // Test type conversion with invalid types
        assert(!nlm::Config::canConvertToType<int>(nlm::Config::typeToValue("not_an_int")));
        assert(!nlm::Config::canConvertToType<double>(nlm::Config::typeToValue("not_a_number")));
        
        std::cout << "    Typed config validation test for " << getName() << " passed" << std::endl;
    }
};

// Configuration querying validation test
class ConfigQueryingValidationTest : public test_framework::TestBase {
public:
    ConfigQueryingValidationTest(const std::string& name)
        : TestBase(name, test_framework::TestCategory::Configuration) {}
    
    void run() override {
        nlm::Config config;
        
        // Add test data
        config.set("server.host", "localhost");
        config.set("server.port", 8080);
        config.set("server.enabled", true);
        config.set("logging.level", "INFO");
        config.set("logging.format", "json");
        config.set("database.url", "postgres://localhost:5432/mydb");
        
        // Test pattern querying
        auto patternResults = config.queryByPattern("host|port");
        assert(patternResults.size() == 2);
        assert(std::find(patternResults.begin(), patternResults.end(), "server.host") != patternResults.end());
        assert(std::find(patternResults.begin(), patternResults.end(), "server.port") != patternResults.end());
        
        // Test source querying
        config.set("temp_key", "temp_value", nlm::ConfigSource::Runtime);
        auto runtimeResults = config.queryBySource(nlm::ConfigSource::Runtime);
        assert(std::find(runtimeResults.begin(), runtimeResults.end(), "temp_key") != runtimeResults.end());
        
        // Test tree navigation
        std::string treeResult = config.configTreeNavigate("-");
        assert(treeResult.find("server") != std::string::npos);
        assert(treeResult.find("logging") != std::string::npos);
        
        // Test hierarchy
        auto hierarchy = config.getConfigHierarchy({"server"});
        assert(hierarchy.find("server.host") != hierarchy.end());
        assert(hierarchy.find("server.port") != hierarchy.end());
        assert(hierarchy.find("server.enabled") != hierarchy.end());
        
        // Test similar configurations
        nlm::Config config2;
        config2.set("server.host", "localhost");
        config2.set("server.port", 8080);
        config2.set("server.enabled", true);
        
        auto similar = config2.findSimilarConfigurations(0.9, {"logging.level"});
        // Should find config2 similar to itself (threshold met for shared keys)
        
        // Test configuration statistics
        std::string stats = config.getConfigStatistics();
        assert(stats.find("Total keys") != std::string::npos);
        
        std::cout << "    Config querying validation test for " << getName() << " passed" << std::endl;
    }
};

// Batch operations validation test
class ConfigBatchOperationsValidationTest : public test_framework::TestBase {
public:
    ConfigBatchOperationsValidationTest(const std::string& name)
        : TestBase(name, test_framework::TestCategory::Configuration) {}
    
    void run() override {
        nlm::Config config;
        
        // Test batch set
        std::map<std::string, nlm::ConfigValue> batchConfig = {
            {"server.host", "192.168.1.1"},
            {"server.port", 9090},
            {"server.enabled", true},
            {"server.timeout", 60}
        };
        
        config.batchSet(batchConfig, nlm::ConfigSource::File);
        
        // Verify batch set worked
        assert(config.getOr<std::string>("server.host", "") == "192.168.1.1");
        assert(config.getOr<int>("server.port", -1) == 9090);
        assert(config.getOr<bool>("server.enabled", false) == true);
        
        // Test batch update
        std::vector<std::pair<std::string, nlm::ConfigValue>> updates = {
            {"server.port", 8080},
            {"server.timeout", 120}
        };
        
        config.batchUpdate(updates);
        assert(config.getOr<int>("server.port", -1) == 8080);
        assert(config.getOr<int>("server.timeout", -1) == 120);
        
        // Test batch remove
        config.batchRemove({"server.timeout", "nonexistent_key"});
        assert(!config.has("server.timeout"));
        assert(config.has("server.host")); // Other keys should remain
        
        // Test batch import
        std::string importJson = "{\"imported_key\": \"imported_value\", \"imported_number\": 42, \"imported_boolean\": false}";
        bool importSuccess = config.batchImport(importJson, "merge");
        assert(importSuccess);
        
        // Test batch export
        std::string exportResult = config.batchExport(include_sensitive=false);
        assert(exportResult.find("imported_key") != std::string::npos);
        
        std::cout << "    Config batch operations validation test for " << getName() << " passed" << std::endl;
    }
};

// Configuration preset validation test
class ConfigPresetValidationTest : public test_framework::TestBase {
public:
    ConfigPresetValidationTest(const std::string& name)
        : TestBase(name, test_framework::TestCategory::Configuration) {}
    
    void run() override {
        nlm::Config config;
        
        // Test applying configuration presets
        assert(config.applyConfigurationPreset("development"));
        assert(config.getOr<bool>("debug", false));
        
        assert(config.applyConfigurationPreset("production"));
        assert(config.getOr<bool>("debug", true) == false);  // Production should have debug=false
        
        assert(config.applyConfigurationPreset("testing"));
        assert(config.getOr<bool>("debug", true));  // Testing should have debug=true
        
        std::cout << "    Config preset validation test for " << getName() << " passed" << std::endl;
    }
};

// Unit Tests (Original config tests)
void testConfigLoadSave() {
    nlm::Config config;
    
    config.set("test_key", "test_value");
    config.set("test_number", 42);
    config.set("test_flag", true);
    
    // Test that we can retrieve the values
    assert(config.getOr<std::string>("test_key", "") == "test_value");
    assert(config.getOr<int>("test_number", 0) == 42);
    assert(config.getOr<bool>("test_flag", false) == true);
    
    // Test has method
    assert(config.has("test_key"));
    assert(!config.has("nonexistent_key"));
    
    std::cout << "    testConfigLoadSave passed" << std::endl;
}

void testConfigGetOr() {
    nlm::Config config;
    
    config.set("existing_key", "existing_value");
    
    // Test getOr with existing key
    assert(config.getOr<std::string>("existing_key", "default") == "existing_value");
    
    // Test getOr with nonexistent key (should return default)
    assert(config.getOr<std::string>("nonexistent_key", "default") == "default");
    
    std::cout << "    testConfigGetOr passed" << std::endl;
}

void testConfigTyped() {
    nlm::Config config;
    
    config.set("int_key", 123);
    config.set("string_key", "test");
    config.set("bool_key", true);
    
    // Test type-safe getTyped
    auto typed = config.getTyped<int>("int_key");
    assert(typed.has_value());
    assert(typed->getValue() == 123);
    
    auto typedString = config.getTyped<std::string>("string_key");
    assert(typedString.has_value());
    assert(typedString->getValue() == "test");
    
    auto typedBool = config.getTyped<bool>("bool_key");
    assert(typedBool.has_value());
    assert(typedBool->getValue() == true);
    
    std::cout << "    testConfigTyped passed" << std::endl;
}

void testConfigQuery() {
    nlm::Config config;
    
    config.set("key1", "value1");
    config.set("key2", "value2");
    config.set("key3", "value3");
    
    // Test getting all keys
    auto keys = config.getKeys();
    assert(keys.size() == 3);
    assert(std::find(keys.begin(), keys.end(), "key1") != keys.end());
    assert(std::find(keys.begin(), keys.end(), "key2") != keys.end());
    assert(std::find(keys.begin(), keys.end(), "key3") != keys.end());
    
    // Test removing key
    config.remove("key2");
    keys = config.getKeys();
    assert(keys.size() == 2);
    assert(std::find(keys.begin(), keys.end(), "key2") == keys.end());
    
    std::cout << "    testConfigQuery passed" << std::endl;
}

void testConfigSummary() {
    nlm::Config config;
    
    config.set("key1", "value1");
    config.set("key2", 123);
    config.set("key3", true);
    
    std::string summary = config.summary();
    assert(summary.find("key1") != std::string::npos);
    assert(summary.find("key2") != std::string::npos);
    assert(summary.find("key3") != std::string::npos);
    
    std::cout << "    testConfigSummary passed" << std::endl;
}

void testConfigClear() {
    nlm::Config config;
    
    config.set("key1", "value1");
    config.set("key2", "value2");
    
    assert(config.getKeys().size() == 2);
    
    config.clear();
    
    assert(config.getKeys().empty());
    
    std::cout << "    testConfigClear passed" << std::endl;
}

void runAll() {
    std::cout << "=== Running Enhanced Configuration Tests ===" << std::endl;
    
    // Run original unit tests
    testConfigLoadSave();
    testConfigGetOr();
    testConfigTyped();
    testConfigQuery();
    testConfigSummary();
    testConfigClear();
    
    std::cout << "\n=== Running Configuration Validation Tests ===" << std::endl;
    
    // Run comprehensive validation tests
    ConfigSchemaValidationTest schemaTest("config_schema_validation");
    schemaTest.run();
    
    TypedConfigValidationTest typedTest("typed_config_validation");
    typedTest.run();
    
    ConfigQueryingValidationTest queryTest("config_querying_validation");
    queryTest.run();
    
    ConfigBatchOperationsValidationTest batchTest("config_batch_operations_validation");
    batchTest.run();
    
    ConfigPresetValidationTest presetTest("config_preset_validation");
    presetTest.run();
    
    std::cout << "\n=== All Configuration Tests Completed ===" << std::endl;
}

} // namespace test_config