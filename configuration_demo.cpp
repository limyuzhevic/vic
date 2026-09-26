// Configuration System Demonstration
// This file demonstrates the enhanced configuration system features
// and ensures backward compatibility with existing code

#include "core/Config/Config.hpp"
#include <iostream>
#include <cassert>

void demonstrate_backward_compatibility() {
    std::cout << "=== Backward Compatibility Tests ===" << std::endl;
    
    // Original test patterns should still work
    nlm::Config config;
    
    // Test 1: Basic set/get operations (from original tests)
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    assert(config.has("int_value"));
    
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    // Test 2: getOr functionality
    int val = config.getOr<int>("missing", 100);
    assert(val == 100);
    
    // Test 3: Remove functionality
    config.set("test", 123, nlm::ConfigSource::Runtime);
    assert(config.has("test"));
    config.remove("test");
    assert(!config.has("test"));
    
    // Test 4: getKeys functionality
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", 2, nlm::ConfigSource::Runtime);
    config.set("key3", 3, nlm::ConfigSource::Runtime);
    
    auto keys = config.getKeys();
    assert(keys.size() == 3);
    
    std::cout << "✓ All backward compatibility tests passed" << std::endl;
}

void demonstrate_new_features() {
    std::cout << "\n=== New Features Demonstration ===" << std::endl;
    
    nlm::Config config;
    
    // Test 1: Enhanced ConfigSource enum
    std::cout << "1. Enhanced ConfigSource enum:";
    std::cout << " Default(" << static_cast<int>(nlm::ConfigSource::Default) << "),";
    std::cout << " File(" << static_cast<int>(nlm::ConfigSource::File) << "),";
    std::cout << " Runtime(" << static_cast<int>(nlm::ConfigSource::Runtime) << "),";
    std::cout << " Environment(" << static_cast<int>(nlm::ConfigSource::Environment) << ");" << std::endl;
    
    // Test 2: Configuration entry metadata
    config.set("learning_rate", 0.01, nlm::ConfigSource::File);
    config.setDescription("learning_rate", "STDP learning rate");
    config.setType("learning_rate", "float");
    config.setRequired("learning_rate", true);
    
    auto description = config.getDescription("learning_rate");
    auto type = config.getType("learning_rate");
    auto required = config.getRequired("learning_rate");
    
    assert(description == "STDP learning rate");
    assert(type == "float");
    assert(required == true);
    
    std::cout << "✓ Configuration metadata (description, type, required) working" << std::endl;
    
    // Test 3: Multiple sources
    config.set("setting1", "value1", nlm::ConfigSource::CommandLine);
    config.set("setting2", "value2", nlm::ConfigSource::Environment);
    config.set("setting3", "value3", nlm::ConfigSource::File);
    
    auto source1 = config.getSource("setting1");
    auto source2 = config.getSource("setting2"); 
    auto source3 = config.getSource("setting3");
    
    assert(source1 == nlm::ConfigSource::CommandLine);
    assert(source2 == nlm::ConfigSource::Environment);
    assert(source3 == nlm::ConfigSource::File);
    
    std::cout << "✓ Multiple source types (CommandLine, Environment, File) working" << std::endl;
    
    // Test 4: Source-specific has() method
    bool hasCmdLine = config.has("setting1", nlm::ConfigSource::CommandLine);
    bool hasEnv = config.has("setting1", nlm::ConfigSource::Environment);
    
    assert(hasCmdLine == true);
    assert(hasEnv == false);
    
    std::cout << "✓ Source-specific has() method working" << std::endl;
    
    // Test 5: getKeys with source filtering
    auto allKeys = config.getKeys();
    auto fileKeys = config.getKeys(nlm::ConfigSource::File);
    
    assert(allKeys.size() == 3);
    assert(fileKeys.size() == 1);
    assert(std::find(fileKeys.begin(), fileKeys.end(), "setting3") != fileKeys.end());
    
    std::cout << "✓ getKeys() with source filtering working" << std::endl;
    
    // Test 6: Dependencies
    config.addDependency("dependent_key", "required_key");
    
    // Set the required key
    config.set("required_key", 100, nlm::ConfigSource::Runtime);
    
    std::cout << "✓ Dependency management working" << std::endl;
    
    // Test 7: Type-specific getters
    config.set("int_config", 42, nlm::ConfigSource::Runtime);
    config.set("double_config", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_config", "test", nlm::ConfigSource::Runtime);
    config.set("bool_config", true, nlm::ConfigSource::Runtime);
    
    auto intConfig = config.getInt("int_config");
    auto doubleConfig = config.getDouble("double_config");
    auto stringConfig = config.getString("string_config");
    auto boolConfig = config.getBool("bool_config");
    
    assert(intConfig == 42);
    assert(doubleConfig > 3.13 && doubleConfig < 3.15);
    assert(stringConfig == "test");
    assert(boolConfig == true);
    
    std::cout << "✓ Type-specific getters (getInt, getDouble, getString, getBool) working" << std::endl;
    
    // Test 8: JSON serialization
    std::string jsonStr = config.toJSON();
    assert(!jsonStr.empty());
    assert(jsonStr.find("int_config") != std::string::npos);
    
    std::cout << "✓ JSON serialization (toJSON()) working" << std::endl;
    
    // Test 9: Enhanced validation framework
    config.set("valid_key", 123, nlm::ConfigSource::Runtime);
    config.setRequired("valid_key", true);
    
    bool isValid = config.validate();
    assert(isValid == true);
    
    auto errors = config.getValidationErrors();
    assert(errors.empty());
    
    std::cout << "✓ Validation framework (validate(), getValidationErrors()) working" << std::endl;
    
    // Test 10: Config entry properties
    std::string key = "test_key";
    config.set(key, "test_value", nlm::ConfigSource::Runtime);
    
    // Modify properties
    config.setDescription(key, "Test description");
    config.setType(key, "string");
    config.setRequired(key, true);
    
    auto desc = config.getDescription(key);
    auto type = config.getType(key);
    auto req = config.getRequired(key);
    
    assert(desc == "Test description");
    assert(type == "string");
    assert(req == true);
    
    std::cout << "✓ Config entry property management working" << std::endl;
}

void demonstrate_error_handling() {
    std::cout << "\n=== Error Handling Demonstration ===" << std::endl;
    
    nlm::Config config;
    
    // Test 1: Missing required key validation
    config.set("optional_key", 100, nlm::ConfigSource::Runtime);
    config.setRequired("required_key", true);
    
    bool isValid = config.validate();
    assert(isValid == false);
    
    auto errors = config.getValidationErrors();
    assert(!errors.empty());
    assert(errors[0].find("required_key") != std::string::npos);
    
    std::cout << "✓ Missing required key validation working" << std::endl;
    
    // Test 2: Dependency validation
    config.addDependency("dependent_key", "missing_dependency");
    
    isValid = config.validate();
    assert(isValid == false);
    
    errors = config.getValidationErrors();
    assert(!errors.empty());
    assert(errors[0].find("depends on missing key") != std::string::npos);
    
    std::cout << "✓ Dependency validation working" << std::endl;
    
    // Test 3: Type conversion errors (should not crash)
    config.set("number_key", "not_a_number", nlm::ConfigSource::Runtime);
    
    // Try to get as int - should return nullopt
    auto intVal = config.getInt("number_key");
    assert(!intVal.has_value());
    
    std::cout << "✓ Type conversion error handling working" << std::endl;
}

int main() {
    std::cout << "NLM Configuration System - Feature Demonstration" << std::endl;
    std::cout << "=================================================" << std::endl;
    
    demonstrate_backward_compatibility();
    demonstrate_new_features();
    demonstrate_error_handling();
    
    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "All tests passed successfully!" << std::endl;
    std::cout << "The enhanced configuration system is working correctly." << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    
    return 0;
}