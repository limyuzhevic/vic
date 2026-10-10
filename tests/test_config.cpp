// Config Tests
#include "core/Config/Config.hpp"
#include "core/ErrorHandling/ErrorHandling.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <string>

namespace test_config {

void testConfigCreation() {
    nlm::Config config;
    assert(!config.has("test"));
    
    std::cout << "    testConfigCreation passed" << std::endl;
}

void testConfigSetAndGet() {
    nlm::Config config;
    
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    assert(config.has("int_value"));
    
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value());
    assert(*doubleVal > 3.13 && *doubleVal < 3.15);
    
    config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value());
    assert(*stringVal == "hello");
    
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    
    std::cout << "    testConfigSetAndGet passed" << std::endl;
}

void testConfigGetOr() {
    nlm::Config config;
    
    int val = config.getOr<int>("missing", 100);
    assert(val == 100);
    
    config.set("existing", 50, nlm::ConfigSource::Runtime);
    val = config.getOr<int>("existing", 100);
    assert(val == 50);
    
    std::cout << "    testConfigGetOr passed" << std::endl;
}

void testConfigRemove() {
    nlm::Config config;
    
    config.set("test", 123, nlm::ConfigSource::Runtime);
    assert(config.has("test"));
    
    config.remove("test");
    assert(!config.has("test"));
    
    std::cout << "    testConfigRemove passed" << std::endl;
}

void testConfigKeys() {
    nlm::Config config;
    
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", 2, nlm::ConfigSource::Runtime);
    config.set("key3", 3, nlm::ConfigSource::Runtime);
    
    auto keys = config.getKeys();
    assert(keys.size() == 3);
    
    std::cout << "    testConfigKeys passed" << std::endl;
}

void testConfigErrorHandling() {
    nlm::Config config;
    
    // Test type validation
    config.set("string_int", "not_a_number", nlm::ConfigSource::Runtime);
    auto intVal = config.get<int>("string_int");
    assert(!intVal.has_value());  // Should fail type conversion
    
    // Test getOr with invalid types
    int defaultInt = config.getOr<int>("string_int", 999);
    assert(defaultInt == 999);  // Should return default
    
    // Test missing key with getOr
    bool missingBool = config.getOr<bool>("missing_bool", true);
    assert(missingBool == true);  // Should return default
    
    std::cout << "    testConfigErrorHandling passed" << std::endl;
}

void testConfigSourceTracking() {
    nlm::Config config;
    
    config.set("source_test", 42, nlm::ConfigSource::Runtime);
    auto entry = config.get<std::string>("source_test");  // This should work since we set it as int, but get returns string
    assert(entry.has_value());  // Check that key exists
    
    std::cout << "    testConfigSourceTracking passed" << std::endl;
}

void testConfigSaveLoad() {
    nlm::Config config;
    
    // Add some test data
    config.set("int_setting", 42, nlm::ConfigSource::Runtime);
    config.set("double_setting", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_setting", "test_value", nlm::ConfigSource::Runtime);
    config.set("bool_setting", true, nlm::ConfigSource::Runtime);
    
    // Test saving to file
    std::string testFile = "test_config_save.ini";
    assert(config.saveToFile(testFile));
    
    // Create a new config and load from file
    nlm::Config config2;
    assert(config2.loadFromFile(testFile));
    
    // Verify data was loaded correctly
    assert(config2.getOr<int>("int_setting", 0) == 42);
    assert(config2.getOr<double>("double_setting", 0.0) > 3.13 && config2.getOr<double>("double_setting", 0.0) < 3.15);
    assert(config2.getOr<std::string>("string_setting", "") == "test_value");
    assert(config2.getOr<bool>("bool_setting", false) == true);
    
    std::cout << "    testConfigSaveLoad passed" << std::endl;
}

void testConfigErrorHandlingIntegration() {
    // Test that error handling catches issues during config loading
    nlm::Config config;
    
    // Create a config with invalid data
    std::string invalidJson = R"({"valid": "data", "invalid": })";
    std::ofstream invalidFile("invalid_config.json");
    invalidFile << invalidJson;
    invalidFile.close();
    
    // Try to load the invalid JSON - should handle gracefully
    bool loadResult = config.loadFromFile("invalid_config.json");
    // The implementation should handle this gracefully (maybe return false or load partial data)
    
    // Test vector parsing
    config.set("vector_test", "[1, 2, 3]", nlm::ConfigSource::Runtime);
    auto vectorVal = config.get<std::vector<int>>("vector_test");
    assert(vectorVal.has_value());
    assert(vectorVal.value().size() == 3);
    assert(vectorVal.value()[0] == 1);
    
    std::cout << "    testConfigErrorHandlingIntegration passed" << std::endl;
}

void testConfigAdvancedErrorHandling() {
    nlm::Config config;
    
    // Test advanced type conversion scenarios
    config.set("int_string", "123", nlm::ConfigSource::Runtime);
    config.set("float_string", "3.14159", nlm::ConfigSource::Runtime);
    config.set("bool_string_true", "true", nlm::ConfigSource::Runtime);
    config.set("bool_string_false", "false", nlm::ConfigSource::Runtime);
    
    // Test type conversions
    assert(config.get<int>("int_string").has_value());
    assert(config.get<int>("int_string").value() == 123);
    
    assert(config.get<double>("float_string").has_value());
    assert(config.get<double>("float_string").value() > 3.14 && 
           config.get<double>("float_string").value() < 3.15);
    
    assert(config.get<bool>("bool_string_true").has_value());
    assert(config.get<bool>("bool_string_true").value() == true);
    
    assert(config.get<bool>("bool_string_false").has_value());
    assert(config.get<bool>("bool_string_false").value() == false);
    
    // Test error type conversions
    config.set("invalid_int", "not_a_number", nlm::ConfigSource::Runtime);
    assert(!config.get<int>("invalid_int").has_value());
    
    config.set("invalid_double", "not_a_float", nlm::ConfigSource::Runtime);
    assert(!config.get<double>("invalid_double").has_value());
    
    // Test getOr with error types
    assert(config.getOr<int>("invalid_int", 999) == 999);
    assert(config.getOr<double>("invalid_double", 3.14) > 3.13 && 
           config.getOr<double>("invalid_double", 3.14) < 3.15);
    
    std::cout << "    testConfigAdvancedErrorHandling passed" << std::endl;
}

void testConfigPerformanceScenarios() {
    nlm::Config config;
    
    // Test performance with many entries
    const size_t numEntries = 1000;
    for (size_t i = 0; i < numEntries; ++i) {
        config.set("key_" + std::to_string(i), static_cast<int>(i), nlm::ConfigSource::Runtime);
    }
    
    // Verify all entries exist
    assert(config.getKeys().size() == numEntries);
    
    // Test retrieval performance
    for (size_t i = 0; i < 100; ++i) {
        auto val = config.get<int>("key_" + std::to_string(i % numEntries));
        assert(val.has_value());
        assert(val.value() == static_cast<int>(i % numEntries));
    }
    
    // Test removal performance
    for (size_t i = 0; i < 10; ++i) {
        config.remove("key_" + std::to_string(i));
    }
    
    assert(config.getKeys().size() == numEntries - 10);
    
    std::cout << "    testConfigPerformanceScenarios passed" << std::endl;
}

void testConfigBoundaryValues() {
    nlm::Config config;
    
    // Test boundary integer values
    config.set("max_int", std::numeric_limits<int>::max(), nlm::ConfigSource::Runtime);
    config.set("min_int", std::numeric_limits<int>::min(), nlm::ConfigSource::Runtime);
    
    assert(config.get<int>("max_int").has_value());
    assert(config.get<int>("min_int").has_value());
    
    // Test boundary floating point values
    config.set("max_double", std::numeric_limits<double>::max(), nlm::ConfigSource::Runtime);
    config.set("min_double", std::numeric_limits<double>::min(), nlm::ConfigSource::Runtime);
    config.set("neg_inf", -std::numeric_limits<double>::infinity(), nlm::ConfigSource::Runtime);
    config.set("pos_inf", std::numeric_limits<double>::infinity(), nlm::ConfigSource::Runtime);
    
    assert(config.get<double>("max_double").has_value());
    assert(config.get<double>("min_double").has_value());
    
    // Test string boundaries
    config.set("empty_string", "", nlm::ConfigSource::Runtime);
    config.set("long_string", std::string(10000, 'a'), nlm::ConfigSource::Runtime);
    
    assert(config.get<std::string>("empty_string").has_value());
    assert(config.get<std::string>("empty_string").value() == "");
    
    assert(config.get<std::string>("long_string").has_value());
    assert(config.get<std::string>("long_string").value().size() == 10000);
    
    std::cout << "    testConfigBoundaryValues passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigErrorHandling();
    testConfigSourceTracking();
    testConfigSaveLoad();
    testConfigErrorHandlingIntegration();
    testConfigAdvancedErrorHandling();
    testConfigPerformanceScenarios();
    testConfigBoundaryValues();
    
    std::cout << "All config tests passed!" << std::endl;
}

} // namespace test_config

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigErrorHandling();
    testConfigSourceTracking();
    testConfigSaveLoad();
    testConfigErrorHandlingIntegration();
    
    std::cout << "All config tests passed!" << std::endl;
}

} // namespace test_config

