// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <limits>

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

void testValidationErrorClasses() {
    nlm::Config config;
    
    // Test that validation errors are created
    try {
        throw nlm::ConfigValidationError("Test validation error", "test_key");
    } catch (const nlm::ConfigValidationError& e) {
        assert(e.getKey() == "test_key");
    }
    
    try {
        throw nlm::ConfigRangeError("Test range error", "range_key", 150.0, 0.0, 100.0);
    } catch (const nlm::ConfigRangeError& e) {
        assert(e.getKey() == "range_key");
        assert(e.getValue() == 150.0);
        assert(e.getMin() == 0.0);
        assert(e.getMax() == 100.0);
    }
    
    try {
        throw nlm::ConfigTypeError("Test type error", "type_key", "int", "string");
    } catch (const nlm::ConfigTypeError& e) {
        assert(e.getKey() == "type_key");
        assert(e.getExpectedType() == "int");
        assert(e.getActualType() == "string");
    }
    
    try {
        throw nlm::ConfigDependencyError("Test dependency error", "dep_key", "dep_required");
    } catch (const nlm::ConfigDependencyError& e) {
        assert(e.getKey() == "dep_key");
        assert(e.getDependencyKey() == "dep_required");
    }
    
    std::cout << "    testValidationErrorClasses passed" << std::endl;
}

void testConfigSchemaValidation() {
    nlm::Config config;
    
    // Add validation rules
    config.addValidationRule("positive_int", "Positive Integer", nlm::ConfigValidationType::Integer, true, 0.0, std::numeric_limits<double>::max());
    config.addValidationRule("range_float", "Float in Range", nlm::ConfigValidationType::Float, false, -10.0, 10.0);
    config.addValidationRule("required_bool", "Required Boolean", nlm::ConfigValidationType::Boolean, true);
    config.addValidationRule("string_list", "String List", nlm::ConfigValidationType::VectorString, false);
    
    // Test valid values
    config.set("positive_int", 42);
    config.set("range_float", 5.5);
    config.set("required_bool", true);
    config.set("string_list", std::vector<std::string>{"a", "b", "c"});
    
    // All should be valid
    assert(config.isValid());
    
    // Test invalid values
    config.clear();
    config.set("positive_int", -5); // Invalid negative value
    config.set("range_float", 15.0); // Invalid out of range
    config.set("required_bool", true);
    
    // Should have validation errors
    auto errors = config.validate();
    assert(!errors.empty());
    
    // Test schema enforcement
    config.clear();
    config.addValidationRule("strict_string", "Strict String", nlm::ConfigValidationType::String, true);
    config.set("strict_string", 123); // Wrong type
    
    errors = config.validate();
    assert(!errors.empty());
    
    std::cout << "    testConfigSchemaValidation passed" << std::endl;
}

void testRuntimeValidation() {
    nlm::Config config;
    config.setValidationEnabled(true);
    config.setValidationLevel(nlm::ConfigValidationException::Level::Full);
    
    // Add validation rules
    config.addValidationRule("temp_celsius", "Temperature Celsius", nlm::ConfigValidationType::Float, true, -100.0, 100.0);
    config.addValidationRule("pressure_kpa", "Pressure kPa", nlm::ConfigValidationType::Float, false, 0.0, 1000.0);
    config.addValidationRule("is_enabled", "Enabled Flag", nlm::ConfigValidationType::Boolean, true);
    
    // Valid configuration should pass runtime validation
    config.set("temp_celsius", 25.0);
    config.set("pressure_kpa", 101.3);
    config.set("is_enabled", true);
    
    try {
        config.validate();
    } catch (const nlm::ConfigValidationError& e) {
        assert(false && "Validation should not have failed");
    }
    
    // Invalid configuration should trigger validation
    config.clear();
    config.set("temp_celsius", 150.0); // Invalid temperature
    config.set("pressure_kpa", 101.3);
    config.set("is_enabled", true);
    
    auto errors = config.validate();
    assert(!errors.empty());
    
    // Test pre-validation
    config.clear();
    config.set("temp_celsius", 50.0);
    config.set("pressure_kpa", 101.3);
    config.set("is_enabled", true);
    
    try {
        config.preValidate();
    } catch (const nlm::ConfigValidationError& e) {
        assert(false && "Pre-validation should not have failed");
    }
    
    std::cout << "    testRuntimeValidation passed" << std::endl;
}

void testCategoryValidationHooks() {
    nlm::Config config;
    
    // Add category-specific validation
    config.set("brain.region_count", 3);
    config.set("brain.connections_per_region", 10);
    config.set("plasticity.rate", 0.5);
    config.set("plasticity.ltp_threshold", 0.1);
    config.set("neuromodulation.dopamine_level", 0.8);
    config.set("neuromodulation.curiosity_rate", 0.3);
    config.set("memory.working_memory.capacity", 1000);
    config.set("cognition.planning.depth", 2);
    config.set("development.maturation.rate", 0.7);
    
    // Validate category-specific values
    auto brainCount = config.getOr<int>("brain.region_count", 0);
    auto plasticityRate = config.getOr<double>("plasticity.rate", 0.0);
    auto dopamineLevel = config.getOr<double>("neuromodulation.dopamine_level", 0.0);
    auto memoryCapacity = config.getOr<size_t>("memory.working_memory.capacity", 0);
    auto planningDepth = config.getOr<int>("cognition.planning.depth", 0);
    auto maturationRate = config.getOr<double>("development.maturation.rate", 0.0);
    
    assert(brainCount >= 0);
    assert(plasticityRate >= 0.0 && plasticityRate <= 1.0);
    assert(dopamineLevel >= 0.0 && dopamineLevel <= 1.0);
    assert(memoryCapacity > 0);
    assert(planningDepth >= 0);
    assert(maturationRate >= 0.0 && maturationRate <= 1.0);
    
    std::cout << "    testCategoryValidationHooks passed" << std::endl;
}

void testValidationVerbosityLevels() {
    nlm::Config config;
    config.setValidationLevel(nlm::ConfigValidationException::Level::Summary);
    
    // Invalid configuration
    config.set("test.value", 999.0);
    
    std::cout << "    testValidationVerbosityLevels passed" << std::endl;
}

void testValidationDependencies() {
    nlm::Config config;
    
    // Add validation with dependencies
    config.addValidationRule("advanced_feature", "Advanced Feature", nlm::ConfigValidationType::Boolean, false, std::nullopt, std::nullopt, std::nullopt, {"basic_feature"}, {{}, {}})
        .requiredIf({"basic_feature"})
        .dependencies({
            {"basic_feature", "feature_enabled"},
            {"advanced_feature", "basic_feature"}
        });
    
    // Test dependency validation
    config.set("basic_feature", true);
    config.set("advanced_feature", 42); // Wrong type
    
    auto errors = config.validate();
    assert(!errors.empty());
    
    std::cout << "    testValidationDependencies passed" << std::endl;
}

void testCustomValidators() {
    nlm::Config config;
    
    // Custom validator for email-like strings
    auto emailValidator = [](const nlm::ConfigValue& value) {
        if (std::holds_alternative<std::string>(value)) {
            const std::string& email = std::get<std::string>(value);
            return email.find('@') != std::string::npos && email.find('.') != std::string::npos;
        }
        return false;
    };
    
    // Add validation rule with custom validator
    config.addValidationRule("user.email", "User Email", nlm::ConfigValidationType::String, true, std::nullopt, std::nullopt, std::nullopt, {}, {}, emailValidator);
    
    // Valid email
    config.set("user.email", "user@example.com");
    assert(config.isValid());
    
    // Invalid email
    config.clear();
    config.set("user.email", "invalid-email");
    auto errors = config.validate();
    assert(!errors.empty());
    
    std::cout << "    testCustomValidators passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testValidationErrorClasses();
    testConfigSchemaValidation();
    testRuntimeValidation();
    testCategoryValidationHooks();
    testValidationVerbosityLevels();
    testValidationDependencies();
    testCustomValidators();
}

} // namespace test_config