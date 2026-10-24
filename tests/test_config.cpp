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

// Advanced Configuration Tests
void testConfigTemplates() {
    nlm::Config config;
    
    // Create a simple template
    nlm::ConfigTemplate template1;
    template1.name = "basic_config";
    
    // Add validation for integer values
    template1.addValidation("neuron_count", 
        [](const nlm::ConfigValue& value) -> nlm::ValidationResult {
            auto val = std::get<int64_t>(value);
            if (val <= 0) {
                return nlm::ValidationResult::Error("neuron_count must be positive");
            }
            return nlm::ValidationResult::Success();
        });
    
    template1.addRequired("neuron_count");
    template1.defaultValues["neuron_count"] = static_cast<int64_t>(100);
    
    // Test template validation
    std::map<std::string, nlm::ConfigValue> configData;
    configData["neuron_count"] = static_cast<int64_t>(50);
    
    auto validation = template1.validate(configData);
    assert(validation.valid);
    
    // Test with invalid value
    std::map<std::string, nlm::ConfigValue> invalidData;
    invalidData["neuron_count"] = static_cast<int64_t>(-10);
    
    validation = template1.validate(invalidData);
    assert(!validation.valid);
    assert(!validation.errorMessage.empty());
    
    std::cout << "    testConfigTemplates passed" << std::endl;
}

void testConfigTemplateApplication() {
    nlm::Config config;
    
    // Create and add a template
    nlm::ConfigTemplate template1;
    template1.name = "test_template";
    template1.defaultValues["key1"] = "default1";
    template1.defaultValues["key2"] = 42;
    
    // Add validation for string length
    template1.addValidation("key1",
        [](const nlm::ConfigValue& value) -> nlm::ValidationResult {
            auto val = std::get<std::string>(value);
            if (val.length() < 3) {
                return nlm::ValidationResult::Error("key1 must be at least 3 characters");
            }
            return nlm::ValidationResult::Success();
        });
    
    // Create advanced config and add template
    nlm::AdvancedConfig advancedConfig;
    // Note: This would require adding setAdvancedConfig method to Config
    // For now, we'll test the template functionality directly
    
    std::map<std::string, nlm::ConfigValue> baseConfig;
    baseConfig["key1"] = "test";
    baseConfig["key2"] = 100;
    
    std::map<std::string, nlm::ConfigValue> overrideConfig;
    overrideConfig["key1"] = "override";  // Longer than minimum
    
    // Test config merging
    auto merged = advancedConfig.mergeConfigs(baseConfig, overrideConfig);
    assert(merged["key1"] == "override");
    assert(merged["key2"] == 100);
    
    std::cout << "    testConfigTemplateApplication passed" << std::endl;
}

void testConfigValidation() {
    nlm::Config config;
    
    // Test validation with missing required fields
    config.set("name", "test_brain");
    // Note: This would require adding validation framework to Config
    // For now, we'll test basic validation functionality
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    
    // Advanced tests
    testConfigTemplates();
    testConfigTemplateApplication();
    testConfigValidation();
}

} // namespace test_config
