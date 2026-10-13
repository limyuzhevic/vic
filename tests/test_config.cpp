// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

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

// Enhanced Config Tests with edge cases and validation
void testConfigTypeConversion();
void testConfigValidation();
void testConfigEquality();
void testConfigJSON();
void testConfigPresets();
void testConfigErrorHandling();

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigTypeConversion();
    testConfigValidation();
    testConfigEquality();
    testConfigJSON();
    testConfigPresets();
    testConfigErrorHandling();
}

void testConfigTypeConversion() {
    nlm::Config config;
    
    // Test type conversion edge cases
    config.setInt("int_key", 42);
    assert(config.getInt("int_key") == 42);
    
    // Test string to int conversion
    config.setString("str_int", "123");
    auto intFromStr = config.getInt("str_int");
    assert(intFromStr.has_value());
    assert(*intFromStr == 123);
    
    // Test float to int conversion
    config.setFloat("float_val", 3.14);
    auto floatVal = config.getFloat("float_val");
    assert(floatVal.has_value());
    assert(std::abs(*floatVal - 3.14) < 0.01);
    
    // Test bool conversions
    config.setBool("bool_true", true);
    config.setBool("bool_false", false);
    
    assert(config.getBool("bool_true") == true);
    assert(config.getBool("bool_false") == false);
    
    // Test list edge cases
    config.setList("int_list", std::vector<int64_t>{1, 2, 3});
    auto intList = config.getIntList("int_list");
    assert(intList.has_value());
    assert(intList->size() == 3);
    
    config.setList("string_list", std::vector<std::string>{"a", "b", "c"});
    auto stringList = config.getStringList("string_list");
    assert(stringList.has_value());
    assert(stringList->size() == 3);
    
    std::cout << "    testConfigTypeConversion passed" << std::endl;
}

void testConfigValidation() {
    nlm::Config config;
    
    // Test basic validation
    assert(config.validate().valid);
    
    // Test required field validation
    nlm::Config::ValidationRule requiredRule;
    requiredRule.key = "required_field";
    requiredRule.validator = [](const nlm::ConfigValue& val) {
        return std::holds_alternative<std::string>(val) && !std::get<std::string>(val).empty();
    };
    requiredRule.errorMessage = "Required field cannot be empty";
    
    config.addValidationRule(requiredRule);
    
    // Should fail validation when required field is missing
    auto validationResult = config.validate();
    assert(!validationResult.valid);
    
    // Set required field
    config.setString("required_field", "test");
    validationResult = config.validate();
    assert(validationResult.valid);
    
    // Test regex validation
    nlm::Config::ValidationRule regexRule;
    regexRule.key = "email_field";
    regexRule.validator = [](const nlm::ConfigValue& val) {
        if (!std::holds_alternative<std::string>(val)) return false;
        const auto& str = std::get<std::string>(val);
        return str.find('@') != std::string::npos && str.size() > 3;
    };
    regexRule.errorMessage = "Invalid email format";
    
    config.addValidationRule(regexRule);
    
    // Should fail validation for invalid email
    config.setString("email_field", "invalid");
    validationResult = config.validate();
    assert(!validationResult.valid);
    
    // Should pass validation for valid email
    config.setString("email_field", "valid@example.com");
    validationResult = config.validate();
    assert(validationResult.valid);
    
    // Test validation score
    float score = config.getValidationScore();
    assert(score > 0.5);
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void testConfigEquality() {
    nlm::Config config1;
    nlm::Config config2;
    
    // Initially equal
    assert(config1.equals(config2));
    assert(!config1.differs(config2));
    
    // After setting a value
    config1.setInt("test", 42);
    assert(!config1.equals(config2));
    assert(config1.differs(config2));
    
    // Config2 with same value
    config2.setInt("test", 42);
    assert(config1.equals(config2));
    assert(!config1.differs(config2));
    
    // Different value
    config1.setInt("test", 43);
    assert(!config1.equals(config2));
    assert(config1.differs(config2));
    
    // Clone test
    auto config3 = config1.clone();
    assert(config1.equals(*config3));
    
    // Modify clone
    config3->setInt("test", 42);
    assert(config1.equals(*config3));
    
    std::cout << "    testConfigEquality passed" << std::endl;
}

void testConfigJSON() {
    nlm::Config config;
    
    // Set some values
    config.setInt("int_value", 42);
    config.setFloat("float_value", 3.14);
    config.setBool("bool_value", true);
    config.setString("string_value", "hello");
    config.setList("int_list", std::vector<int64_t>{1, 2, 3});
    
    // Export to JSON
    std::string json = config.exportToJSON();
    
    // Verify JSON is valid
    assert(!json.empty());
    assert(json.find("int_value") != std::string::npos);
    assert(json.find("float_value") != std::string::npos);
    
    // Clear and import from JSON
    config.clear();
    assert(config.importFromJSON(json));
    
    // Verify imported values
    assert(config.getInt("int_value") == 42);
    assert(config.getFloat("float_value") > 3.13 && config.getFloat("float_value") < 3.15);
    assert(config.getBool("bool_value") == true);
    assert(config.getString("string_value") == "hello");
    
    // Test invalid JSON
    assert(!config.importFromJSON("invalid json"));
    assert(!config.importFromJSON("{ invalid json"));
    
    std::cout << "    testConfigJSON passed" << std::endl;
}

void testConfigPresets() {
    // Test preset availability
    auto presets = nlm::ConfigPresets::getPresetNames();
    assert(!presets.empty());
    
    // Test preset application
    nlm::Config config;
    assert(config.applyPreset("basic"));
    
    // Verify preset was applied
    auto keys = config.getKeys();
    assert(!keys.empty());
    
    // Test invalid preset
    assert(!config.applyPreset("nonexistent_preset"));
    
    std::cout << "    testConfigPresets passed" << std::endl;
}

void testConfigErrorHandling() {
    nlm::Config config;
    
    // Test error handling for invalid keys
    bool result = config.set("", 123);
    assert(!result);  // Empty key should fail
    
    result = config.set("valid key", 123);
    assert(!result);  // Key with space should fail
    
    // Test error handling for invalid types
    result = config.setInt("valid", 42);
    assert(result);
    
    // Test error handling for type mismatch
    auto intVal = config.getInt("valid");
    assert(intVal.has_value());
    
    // Test error handling for validation
    nlm::Config::ValidationRule typeRule;
    typeRule.key = "typed_field";
    typeRule.validator = [](const nlm::ConfigValue& val) {
        return std::holds_alternative<int>(val);
    };
    typeRule.errorMessage = "Must be an integer";
    
    config.addValidationRule(typeRule);
    
    // Should pass validation for integer
    config.setInt("typed_field", 123);
    assert(config.validate().valid);
    
    // Should fail validation for string
    config.setString("typed_field", "not an int");
    assert(!config.validate().valid);
    
    std::cout << "    testConfigErrorHandling passed" << std::endl;
}

} // namespace test_config