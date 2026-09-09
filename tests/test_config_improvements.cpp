// Config Class Test Suite
// Tests for Config class improvements including:
// - Input validation (keys, values)
// - Error handling
// - Helper methods
// - File I/O with validation
// - Exception classes

#include "src/core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>

namespace test_config_improvements {

void testConfigConstructorDestructor() {
    // Test Config constructor and destructor
    {
        nlm::Config config;
        assert(!config.has("test_key"));
        assert(config.getKeys().empty());
        assert(nlm::Config::getLastError().empty());
    }
    // Destructor should not crash
    std::cout << "    testConfigConstructorDestructor passed" << std::endl;
}

void testMoveOperations() {
    // Test move constructor and assignment operator
    nlm::Config config1;
    config1.addInt("key1", 100, "Test key 1");
    config1.addString("key2", "value2", "Test key 2");
    
    nlm::Config config2(std::move(config1));
    assert(config2.has("key1"));
    assert(config2.has("key2"));
    assert(config1.has("key1") == false); // Original should be moved from
    assert(config1.has("key2") == false);
    
    nlm::Config config3;
    config3 = std::move(config2);
    assert(config3.has("key1"));
    assert(config3.has("key2"));
    
    std::cout << "    testMoveOperations passed" << std::endl;
}

void testInputValidationKeys() {
    // Test key validation and normalization
    nlm::Config config;
    
    // Valid keys
    assert(nlm::Config::isValidKey("valid_key") == true);
    assert(nlm::Config::isValidKey("key123") == true);
    assert(nlm::Config::isValidKey("KEY_ABC") == true);
    assert(nlm::Config::isValidKey("test_key_456") == true);
    
    // Invalid keys
    assert(nlm::Config::isValidKey("") == false); // Empty
    assert(nlm::Config::isValidKey("invalid key") == false); // Space
    assert(nlm::Config::isValidKey("@invalid") == false); // Special char
    assert(nlm::Config::isValidKey("123start") == false); // Starts with digit
    
    // Key normalization
    assert(nlm::Config::validateAndNormalizeKey("  Test_Key  ") == "test_key");
    assert(nlm::Config::validateAndNormalizeKey("KEY_VALUE") == "key_value");
    assert(nlm::Config::validateAndNormalizeKey("aBc_Def") == "aBc_Def"); // Case preserved
    
    std::cout << "    testInputValidationKeys passed" << std::endl;
}

void testInputValidationValues() {
    // Test value validation for different types
    nlm::Config config;
    
    // Valid value types
    config.addInt("int_val", 42, "Valid integer");
    config.addDouble("double_val", 3.14, "Valid double");
    config.addBool("bool_val", true, "Valid boolean");
    config.addString("string_val", "hello", "Valid string");
    
    // Valid list types
    config.addInt("int_list", std::vector<int>{1, 2, 3}, "Valid int list");
    config.addDouble("double_list", std::vector<double>{1.5, 2.5}, "Valid double list");
    config.addString("string_list", std::vector<std::string>{"a", "b"}, "Valid string list");
    
    // All should be accessible with correct type
    assert(config.getOr<int>("int_val", 0) == 42);
    assert(config.getOr<double>("double_val", 0.0) > 3.13 && config.getOr<double>("double_val", 0.0) < 3.15);
    assert(config.getOr<bool>("bool_val", false) == true);
    assert(config.getOr<std::string>("string_val", "") == "hello");
    
    // Type-specific getters for lists
    auto intList = config.get<std::vector<int>>("int_list");
    assert(intList && intList->size() == 3 && (*intList)[0] == 1);
    
    std::cout << "    testInputValidationValues passed" << std::endl;
}

void testErrorHandling() {
    // Test error handling and reporting
    nlm::Config config;
    
    // Initial error should be empty
    assert(nlm::Config::getLastError().empty());
    
    // Set an invalid key (empty) and check error
    try {
        config.set("", "value"); // Empty key
    } catch (...) {
        // Error should be set
        std::string error = nlm::Config::getLastError();
        assert(!error.empty());
    }
    
    // Test validation error exception
    try {
        nlm::ConfigValidationError e("Test validation error");
        assert(std::string(e.what()) == "Test validation error");
    } catch (...) {
        // Should not throw
    }
    
    // Test load error exception
    try {
        nlm::ConfigLoadError e("Test load error");
        assert(std::string(e.what()) == "Test load error");
    } catch (...) {
        // Should not throw
    }
    
    // Test save error exception
    try {
        nlm::ConfigSaveError e("Test save error");
        assert(std::string(e.what()) == "Test save error");
    } catch (...) {
        // Should not throw
    }
    
    std::cout << "    testErrorHandling passed" << std::endl;
}

void testHelperMethods() {
    // Test helper methods like trim, toLower, etc.
    nlm::Config config;
    
    // Test string utilities through validation methods
    // Note: These are static methods, so we test them indirectly
    std::string trimmed = nlm::Config::trim("  test  ");
    assert(trimmed == "test");
    
    std::string lowered = nlm::Config::toLower("TEST");
    assert(lowered == "test");
    
    // Test other utility functions
    assert(nlm::Config::isWhitespace(' ') == true);
    assert(nlm::Config::isWhitespace('\t') == true);
    assert(nlm::Config::isWhitespace('a') == false);
    
    assert(nlm::Config::isAlphaNumericOrUnderscore('a') == true);
    assert(nlm::Config::isAlphaNumericOrUnderscore('A') == true);
    assert(nlm::Config::isAlphaNumericOrUnderscore('1') == true);
    assert(nlm::Config::isAlphaNumericOrUnderscore('_') == true);
    assert(nlm::Config::isAlphaNumericOrUnderscore(' ') == false);
    
    assert(nlm::Config::isDigit('5') == true);
    assert(nlm::Config::isDigit('a') == false);
    
    // Test integer conversion
    assert(nlm::Config::intToString(42) == "42");
    assert(nlm::Config::intToString(-10) == "-10");
    
    // Test double conversion
    assert(nlm::Config::doubleToString(3.14) == "3.14");
    
    std::cout << "    testHelperMethods passed" << std::endl;
}

void testFileIOWithValidation() {
    // Test file I/O with validation
    nlm::Config config;
    
    // Create test data
    config.addInt("steps", 1000, "Simulation steps");
    config.addDouble("learning_rate", 0.01, "Learning rate");
    config.addBool("enabled", true, "Enabled flag");
    config.addString("description", "Test config", "Description");
    
    // Save to file
    std::string testFile = "./test_config.json";
    bool saveResult = config.saveToFile(testFile);
    assert(saveResult == true);
    
    // Load from file
    nlm::Config config2;
    bool loadResult = config2.loadFromFile(testFile);
    assert(loadResult == true);
    
    // Verify loaded data
    assert(config2.getOr<int>("steps", 0) == 1000);
    assert(config2.getOr<double>("learning_rate", 0.0) > 0.005 && config2.getOr<double>("learning_rate", 0.0) < 0.02);
    assert(config2.getOr<bool>("enabled", false) == true);
    assert(config2.getOr<std::string>("description", "") == "Test config");
    
    // Test file validation error handling
    bool invalidLoadResult = config2.loadFromFile("./nonexistent_file.json");
    assert(invalidLoadResult == false); // Should fail gracefully
    
    // Clean up
    std::remove(testFile.c_str());
    
    std::cout << "    testFileIOWithValidation passed" << std::endl;
}

void testExceptionClasses() {
    // Test exception class hierarchy and behavior
    nlm::ConfigValidationError valError("Validation test");
    nlm::ConfigLoadError loadError("Load test");
    nlm::ConfigSaveError saveError("Save test");
    
    // Test exception message access
    assert(std::string(valError.what()) == "Validation test");
    assert(std::string(loadError.what()) == "Load test");
    assert(std::string(saveError.what()) == "Save test");
    
    // Test that they're derived from std::runtime_error
    std::runtime_error* valBase = &valError;
    std::runtime_error* loadBase = &loadError;
    std::runtime_error* saveBase = &saveError;
    
    assert(valBase != nullptr);
    assert(loadBase != nullptr);
    assert(saveBase != nullptr);
    
    std::cout << "    testExceptionClasses passed" << std::endl;
}

void testConfigValueTypes() {
    // Test all supported ConfigValue types
    nlm::Config config;
    
    // Test int
    config.set("int_val", static_cast<int>(42), nlm::ConfigSource::Runtime);
    auto intVal = config.get<int>("int_val");
    assert(intVal && *intVal == 42);
    
    // Test int64_t
    config.set("int64_val", static_cast<int64_t>(9223372036854775807), nlm::ConfigSource::Runtime);
    auto int64Val = config.get<int64_t>("int64_val");
    assert(int64Val && *int64Val == 9223372036854775807);
    
    // Test double
    config.set("double_val", 3.14159265359, nlm::ConfigSource::Runtime);
    auto doubleVal = config.get<double>("double_val");
    assert(doubleVal && *doubleVal > 3.14 && *doubleVal < 3.15);
    
    // Test bool
    config.set("bool_val", true, nlm::ConfigSource::Runtime);
    auto boolVal = config.get<bool>("bool_val");
    assert(boolVal && *boolVal == true);
    
    // Test string
    config.set("string_val", "test string", nlm::ConfigSource::Runtime);
    auto stringVal = config.get<std::string>("string_val");
    assert(stringVal && *stringVal == "test string");
    
    // Test vector<int>
    config.set("int_vec", std::vector<int>{1, 2, 3, 4, 5}, nlm::ConfigSource::Runtime);
    auto intVecVal = config.get<std::vector<int>>("int_vec");
    assert(intVecVal && intVecVal->size() == 5);
    
    // Test vector<double>
    config.set("double_vec", std::vector<double>{1.1, 2.2, 3.3}, nlm::ConfigSource::Runtime);
    auto doubleVecVal = config.get<std::vector<double>>("double_vec");
    assert(doubleVecVal && doubleVecVal->size() == 3);
    
    // Test vector<string>
    config.set("string_vec", std::vector<std::string>{"a", "b", "c"}, nlm::ConfigSource::Runtime);
    auto stringVecVal = config.get<std::vector<std::string>>("string_vec");
    assert(stringVecVal && stringVecVal->size() == 3);
    
    std::cout << "    testConfigValueTypes passed" << std::endl;
}

void testSourceTracking() {
    // Test ConfigSource tracking for values
    nlm::Config config;
    
    config.set("default_val", 100, nlm::ConfigSource::Default);
    config.set("file_val", 200, nlm::ConfigSource::File);
    config.set("cli_val", 300, nlm::ConfigSource::CommandLine);
    config.set("runtime_val", 400, nlm::ConfigSource::Runtime);
    
    // We can't directly test source tracking without accessing internal structure,
    // but we can verify that all values are accessible
    assert(config.getOr<int>("default_val", 0) == 100);
    assert(config.getOr<int>("file_val", 0) == 200);
    assert(config.getOr<int>("cli_val", 0) == 300);
    assert(config.getOr<int>("runtime_val", 0) == 400);
    
    std::cout << "    testSourceTracking passed" << std::endl;
}

void runAll() {
    testConfigConstructorDestructor();
    testMoveOperations();
    testInputValidationKeys();
    testInputValidationValues();
    testErrorHandling();
    testHelperMethods();
    testFileIOWithValidation();
    testExceptionClasses();
    testConfigValueTypes();
    testSourceTracking();
}

} // namespace test_config_improvements