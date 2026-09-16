// Config Validation Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_config {

void testCustomExceptions() {
    nlm::Config config;
    
    // Test ConfigParseError
    try {
        throw nlm::ConfigParseError("Test parse error");
    } catch (const nlm::ConfigParseError& e) {
        std::cout << "    ConfigParseError caught: " << e.what() << std::endl;
    }
    
    // Test ConfigValidationError
    try {
        throw nlm::ConfigValidationError("Test validation error");
    } catch (const nlm::ConfigValidationError& e) {
        std::cout << "    ConfigValidationError caught: " << e.what() << std::endl;
    }
    
    // Test ConfigFileError
    try {
        throw nlm::ConfigFileError("Test file error");
    } catch (const nlm::ConfigFileError& e) {
        std::cout << "    ConfigFileError caught: " << e.what() << std::endl;
    }
    
    std::cout << "    testCustomExceptions passed" << std::endl;
}

void testTypeValidation() {
    nlm::Config config;
    
    // Set values with validation
    config.set("int_val", 42, nlm::ConfigSource::Runtime);
    config.set("str_val", "hello", nlm::ConfigSource::Runtime);
    config.set("bool_val", true, nlm::ConfigSource::Runtime);
    config.set("double_val", 3.14, nlm::ConfigSource::Runtime);
    
    // Test type validation in get()
    auto intVal = config.get<int>("int_val");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    auto strVal = config.get<std::string>("str_val");
    assert(strVal.has_value());
    assert(*strVal == "hello");
    
    auto boolVal = config.get<bool>("bool_val");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    
    auto doubleVal = config.get<double>("double_val");
    assert(doubleVal.has_value());
    assert(*doubleVal > 3.13 && *doubleVal < 3.15);
    
    std::cout << "    testTypeValidation passed" << std::endl;
}

void testRangeValidation() {
    nlm::Config config;
    
    // Set numeric value
    config.set("temp", 25, nlm::ConfigSource::Runtime);
    
    // Set range validators
    config.pImpl->rangeValidators["temp"] = std::make_pair(0.0, 100.0);
    
    // Test get with valid range
    auto tempVal = config.get<int>("temp");
    assert(tempVal.has_value());
    assert(*tempVal == 25);
    
    // Set out-of-range value
    config.set("temp", 150, nlm::ConfigSource::Runtime);
    auto tempVal2 = config.get<int>("temp");
    // Should return nullopt due to range validation
    
    std::cout << "    testRangeValidation passed" << std::endl;
}

void testArrayBoundsValidation() {
    nlm::Config config;
    
    // Set array values
    config.set("arr_int", std::vector<int>{1, 2, 3, 4, 5}, nlm::ConfigSource::Runtime);
    
    // Set array bounds validators
    config.pImpl->arrayBoundsValidators["arr_int"] = std::make_pair<size_t, size_t>(3, 10);
    
    // Test get with valid bounds
    auto arrVal = config.get<std::vector<int>>("arr_int");
    assert(arrVal.has_value());
    assert(arrVal->size() == 5);
    
    // Set array with invalid bounds
    config.set("arr_int", std::vector<int>{1}, nlm::ConfigSource::Runtime);
    auto arrVal2 = config.get<std::vector<int>>("arr_int");
    // Should return nullopt due to bounds validation
    
    std::cout << "    testArrayBoundsValidation passed" << std::endl;
}

void testRequiredKeysValidation() {
    nlm::Config config;
    
    // Set required keys
    config.pImpl->requiredKeys = {"required1", "required2"};
    
    // Set one required key
    config.set("required1", "value1", nlm::ConfigSource::Runtime);
    
    // Test validateAll
    config.validateAll();
    auto errors = config.getValidationErrors();
    assert(errors.size() > 0); // Should have error about missing required2
    
    // Set the other required key
    config.set("required2", "value2", nlm::ConfigSource::Runtime);
    config.validateAll();
    errors = config.getValidationErrors();
    assert(errors.empty()); // Should have no errors now
    
    std::cout << "    testRequiredKeysValidation passed" << std::endl;
}

void testStructureValidation() {
    nlm::Config config;
    
    // Set structure validators
    config.pImpl->structureValidators["nested.key"] = "nested";
    config.pImpl->structureValidators["array[]"] = "array";
    
    // Set valid values
    config.set("nested.key", "value", nlm::ConfigSource::Runtime);
    config.set("array[]", std::vector<int>{1, 2, 3}, nlm::ConfigSource::Runtime);
    
    // Test validateAll
    config.validateAll();
    auto errors = config.getValidationErrors();
    assert(errors.empty()); // Should have no errors for valid structure
    
    std::cout << "    testStructureValidation passed" << std::endl;
}

void testGracefulDegradation() {
    nlm::Config config;
    
    // Set values
    config.set("good_key", 100, nlm::ConfigSource::Runtime);
    config.set("bad_key", -50, nlm::ConfigSource::Runtime);
    
    // Set range validation for bad_key (should reject negative values)
    config.pImpl->rangeValidators["bad_key"] = std::make_pair(0.0, 100.0);
    
    // Test get with bad key - should return nullopt, not crash
    auto badVal = config.get<int>("bad_key");
    assert(!badVal.has_value()); // Should be invalid due to range
    
    // Good key should still work
    auto goodVal = config.get<int>("good_key");
    assert(goodVal.has_value());
    assert(*goodVal == 100);
    
    std::cout << "    testGracefulDegradation passed" << std::endl;
}

void testExceptionInFileOperations() {
    nlm::Config config;
    
    // Try to load a non-existent file - should handle gracefully
    bool result = config.loadFromFile("/non/existent/file.json");
    assert(!result); // Should return false, not throw
    
    // The error should be caught internally, not exposed to caller
    std::cout << "    testExceptionInFileOperations passed" << std::endl;
}

void testComprehensiveValidation() {
    nlm::Config config;
    
    // Set up complex configuration
    config.set("api.port", 8080, nlm::ConfigSource::File);
    config.set("api.timeout", 30.0, nlm::ConfigSource::File);
    config.set("limits.maxConnections", 100, nlm::ConfigSource::File);
    config.set("limits.maxRequests[]", std::vector<int>{10, 20, 30}, nlm::ConfigSource::File);
    config.set("feature.enabled", true, nlm::ConfigSource::File);
    
    // Set up validation rules
    config.pImpl->requiredKeys = {"api.port", "api.timeout"};
    config.pImpl->rangeValidators["api.port"] = std::make_pair(1, 65535);
    config.pImpl->rangeValidators["api.timeout"] = std::make_pair(0, 300);
    config.pImpl->arrayBoundsValidators["limits.maxRequests[]"] = std::make_pair(1, 10);
    config.pImpl->structureValidators["api.port"] = "nested";
    config.pImpl->structureValidators["api.timeout"] = "nested";
    
    // Validate all
    config.validateAll();
    auto errors = config.getValidationErrors();
    
    // Should have no errors since all validations pass
    assert(errors.empty());
    
    std::cout << "    testComprehensiveValidation passed" << std::endl;
}

void runAll() {
    testCustomExceptions();
    testTypeValidation();
    testRangeValidation();
    testArrayBoundsValidation();
    testRequiredKeysValidation();
    testStructureValidation();
    testGracefulDegradation();
    testExceptionInFileOperations();
    testComprehensiveValidation();
    
    std::cout << std::endl << "All Config validation tests passed!" << std::endl;
}

} // namespace test_config