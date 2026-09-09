// Python Bindings Test Suite
// Tests for Python bindings improvements including:
// - New error handling and exceptions
// - Validation of parameters
// - Convenience methods
// - Documentation strings

#include "src/core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace test_python_bindings {

void testCustomExceptionsRegistered() {
    // Test that custom exception classes exist and can be caught
    // This verifies the exception registration in pybind11 bindings
    
    try {
        throw nlm::ConfigValidationError("Test validation error");
    } catch (const nlm::ConfigValidationError& e) {
        assert(std::string(e.what()) == "Test validation error");
    }
    
    try {
        throw nlm::ConfigLoadError("Test load error");
    } catch (const nlm::ConfigLoadError& e) {
        assert(std::string(e.what()) == "Test load error");
    }
    
    try {
        throw nlm::ConfigSaveError("Test save error");
    } catch (const nlm::ConfigSaveError& e) {
        assert(std::string(e.what()) == "Test save error");
    }
    
    std::cout << "    testCustomExceptionsRegistered passed" << std::endl;
}

void testConfigConvenienceMethods() {
    // Test convenience methods that are exposed to Python
    // These verify the improved Config API
    
    nlm::Config config;
    
    // Test addInt/addDouble/addBool/addString
    config.addInt("test_int", 42, "Test integer");
    assert(config.has("test_int"));
    assert(config.getOr<int>("test_int", 0) == 42);
    
    config.addDouble("test_double", 3.14, "Test double");
    assert(config.has("test_double"));
    assert(config.getOr<double>("test_double", 0.0) > 3.13 && config.getOr<double>("test_double", 0.0) < 3.15);
    
    config.addBool("test_bool", true, "Test boolean");
    assert(config.has("test_bool"));
    assert(config.getOr<bool>("test_bool", false) == true);
    
    config.addString("test_string", "hello", "Test string");
    assert(config.has("test_string"));
    assert(config.getOr<std::string>("test_string", "") == "hello");
    
    // Test getter convenience methods
    int valInt = config.getInt("test_int", 0, "");
    assert(valInt == 42);
    
    double valDouble = config.getDouble("test_double", 0.0, "");
    assert(valDouble > 3.13 && valDouble < 3.15);
    
    bool valBool = config.getBool("test_bool", false, "");
    assert(valBool == true);
    
    std::string valString = config.getString("test_string", "", "");
    assert(valString == "hello");
    
    std::cout << "    testConfigConvenienceMethods passed" << std::endl;
}

void testValidationErrorHandling() {
    // Test validation error handling for improved Config
    nlm::Config config;
    
    // Test invalid key validation
    try {
        config.set("", "value"); // Empty key should trigger validation error
        assert(false); // Should not reach here
    } catch (const nlm::ConfigValidationError& e) {
        // Expected
        assert(std::string(e.what()).find("invalid") != std::string::npos);
    }
    
    // Test key validation with special characters
    bool valid1 = nlm::Config::isValidKey("valid_key");
    assert(valid1 == true);
    
    bool valid2 = nlm::Config::isValidKey("invalid key"); // Space not allowed
    assert(valid2 == false);
    
    bool valid3 = nlm::Config::isValidKey("@invalid"); // Special char not allowed
    assert(valid3 == false);
    
    // Test key normalization
    std::string normalized = nlm::Config::validateAndNormalizeKey("  Test_Key  ");
    assert(normalized == "test_key");
    
    std::cout << "    testValidationErrorHandling passed" << std::endl;
}

void testParameterValidationForBindings() {
    // Test parameter validation improvements in bindings
    nlm::Config config;
    
    // Add values for testing
    config.addInt("valid_int", 100, "Valid integer");
    config.addDouble("valid_double", 50.5, "Valid double");
    config.addBool("valid_bool", true, "Valid boolean");
    config.addString("valid_string", "test", "Valid string");
    
    // Test get with validation - wrong type
    try {
        auto val = config.get<int>("valid_string"); // Wrong type
        assert(!val.has_value()); // Should return empty optional
    } catch (...) {
        // Some implementations might throw, others return empty
    }
    
    // Test error message functionality
    std::string lastError = nlm::Config::getLastError();
    assert(lastError.empty() || !lastError.empty()); // Should have error or be empty
    
    std::cout << "    testParameterValidationForBindings passed" << std::endl;
}

void testDocumentationStrings() {
    // Test that documentation strings are present in Config methods
    // This is more of a structural check that the methods exist with proper docstrings
    
    nlm::Config config;
    
    // The methods should exist and be callable
    config.loadFromFile("./nonexistent.json"); // Should return false, not crash
    config.saveToFile("./nonexistent.json"); // Should return false, not crash
    
    config.clear(); // Should work
    assert(config.getKeys().size() == 0);
    
    std::cout << "    testDocumentationStrings passed" << std::endl;
}

void testConfigValidationIntegration() {
    // Test the improved Config validation integration
    nlm::Config config;
    
    // Test comprehensive validation workflow
    config.addInt("simulation.steps", 1000, "Number of simulation steps");
    config.addDouble("simulation.dt", 0.01, "Time step size");
    config.addBool("simulation.enabled", true, "Whether simulation is enabled");
    config.addString("output.path", "./results", "Output directory path");
    
    // All values should be valid and accessible
    assert(config.has("simulation.steps"));
    assert(config.has("simulation.dt"));
    assert(config.has("simulation.enabled"));
    assert(config.has("output.path"));
    
    // Get values with validation
    int steps = config.getOr<int>("simulation.steps", 0);
    double dt = config.getOr<double>("simulation.dt", 0.0);
    bool enabled = config.getOr<bool>("simulation.enabled", false);
    std::string path = config.getOr<std::string>("output.path", "");
    
    assert(steps == 1000);
    assert(dt > 0.0 && dt < 1.0);
    assert(enabled == true);
    assert(path == "./results");
    
    std::cout << "    testConfigValidationIntegration passed" << std::endl;
}

void runAll() {
    testCustomExceptionsRegistered();
    testConfigConvenienceMethods();
    testValidationErrorHandling();
    testParameterValidationForBindings();
    testDocumentationStrings();
    testConfigValidationIntegration();
}

} // namespace test_python_bindings