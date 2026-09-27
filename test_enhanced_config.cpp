// Simple test for enhanced configuration system
#include <iostream>
#include <nlm/Config.hpp>

int main() {
    std::cout << "Testing enhanced configuration system..." << std::endl;
    
    // Test basic configuration
    nlm::Config config;
    
    // Set some values
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    
    // Test getting values
    auto intVal = config.get<int>("int_value");
    if (intVal.has_value() && *intVal == 42) {
        std::cout << "✓ int_value test passed" << std::endl;
    } else {
        std::cout << "✗ int_value test failed" << std::endl;
        return 1;
    }
    
    auto stringVal = config.get<std::string>("string_value");
    if (stringVal.has_value() && *stringVal == "hello") {
        std::cout << "✓ string_value test passed" << std::endl;
    } else {
        std::cout << "✗ string_value test failed" << std::endl;
        return 1;
    }
    
    // Test validation rules
    config.addValidationRule(std::make_unique<nlm::TypeValidationRule>(nlm::TypeValidationRule::ValueType::String, "*"));
    config.addValidationRule(std::make_unique<nlm::RangeValidationRule>("port*", 1, 65535, "port"));
    
    config.set("port", 8080, nlm::ConfigSource::Runtime);
    
    if (config.isValid()) {
        std::cout << "✓ Validation test passed" << std::endl;
    } else {
        std::cout << "✗ Validation test failed" << std::endl;
        return 1;
    }
    
    // Test template system
    nlm::ConfigTemplate template;
    template.setTemplateName("default");
    template.addDefaultValue("timeout", 30.0, "Default timeout");
    template.addDefaultValue("retries", 3, "Default retries");
    
    config.addTemplate(std::make_unique<nlm::ConfigTemplate>(template));
    config.applyTemplate("default");
    
    if (config.has("timeout") && config.has("retries")) {
        std::cout << "✓ Template system test passed" << std::endl;
    } else {
        std::cout << "✗ Template system test failed" << std::endl;
        return 1;
    }
    
    // Test inheritance
    nlm::ParentConfig parent;
    parent.setParentPath("/tmp/test_config.json"); // This won't actually exist, but tests the mechanism
    config.addParentConfig(parent);
    
    if (config.hasInheritance()) {
        std::cout << "✓ Inheritance test passed" << std::endl;
    } else {
        std::cout << "✗ Inheritance test failed" << std::endl;
        return 1;
    }
    
    // Test environment variable prefix
    config.setEnvironmentVariablePrefix("TEST_");
    if (config.getMetadata().find("Environment Prefix: TEST_") != std::string::npos) {
        std::cout << "✓ Environment variable prefix test passed" << std::endl;
    } else {
        std::cout << "✗ Environment variable prefix test failed" << std::endl;
        return 1;
    }
    
    std::cout << "\n✓ All enhanced configuration tests passed!" << std::endl;
    return 0;
}