// Advanced Configuration Tests
// Tests advanced configuration parsing and validation

#include "core/Config/AdvancedConfig.hpp"
#include "core/Config/TemplateConfig.hpp"
#include "core/Config/ConfigValidator.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <string>

namespace test_advanced_config {

void testAdvancedConfigValidation() {
    nlm::AdvancedConfig config;
    
    // Test type validation
    assert(config.validate("neuron_count", 100, "int"));
    assert(config.validate("connection_probability", 0.15f, "float"));
    assert(config.validate("model_name", std::string("resnet"), "string"));
    assert(config.validate("enabled_features", std::vector<std::string>{"attention", "prediction"}, "array"));
    assert(config.validate("config_enabled", true, "bool"));
    
    // Test range validation
    assert(config.validateRange("neuron_count", 100, 1000, 10000));
    assert(config.validateRange("learning_rate", 0.001f, 0.1f, 0.01f));
    assert(config.validateRange("temperature", 0.1f, 2.0f, 1.0f));
    
    // Test constraint validation
    assert(config.validateConstraint("max_memory_mb", 1024, "min(512)"));
    assert(config.validateConstraint("max_memory_mb", 1024, "max(2048)"));
    assert(config.validateConstraint("sparsity_level", 0.95f, "range(0.8, 0.99)"));
    
    // Test dependency validation
    assert(config.validateDependencies({
        {"feature_attention", true},
        {"feature_prediction", true}
    }, {"feature_working_memory", true})); // Should pass: dependencies met
    
    assert(!config.validateDependencies({
        {"feature_attention", true},
        {"feature_prediction", true}
    }, {"feature_working_memory", false})); // Should fail: dependency not met
    
    std::cout << "    testAdvancedConfigValidation passed" << std::endl;
}

void testTemplateConfiguration() {
    nlm::TemplateConfigManager manager;
    
    // Create templates
    nlm::Template phase3Template = manager.createTemplate("phase3_learning", {
        {"brain.neuron_count", "100"},
        {"brain.region_count", "2"},
        {"brain.learning_rate", "0.01"},
        {"environment.width", "20.0"},
        {"environment.height", "20.0"}
    });
    
    nlm::Template phase4Template = manager.createTemplate("phase4_cognitive", {
        {"brain.neuron_count", "200"},
        {"brain.region_count", "4"},
        {"brain.learning_rate", "0.005"},
        {"brain.use_attention", "true"},
        {"brain.use_prediction", "true"}
    });
    
    // Store templates
    assert(manager.storeTemplate(phase3Template));
    assert(manager.storeTemplate(phase4Template));
    
    // Retrieve templates
    auto retrievedPhase3 = manager.getTemplate("phase3_learning");
    auto retrievedPhase4 = manager.getTemplate("phase4_cognitive");
    
    assert(retrievedPhase3 != nullptr);
    assert(retrievedPhase4 != nullptr);
    assert(retrievedPhase3->name == "phase3_learning");
    
    // List all templates
    auto templates = manager.listTemplates();
    assert(templates.size() >= 2);
    
    std::cout << "    testTemplateConfiguration passed" << std::endl;
}

void testTemplateExpansion() {
    nlm::TemplateConfigManager manager;
    
    // Create base template
    nlm::Template baseTemplate = manager.createTemplate("base", {
        {"brain.neuron_count", "100"},
        {"brain.learning_rate", "0.01"},
        {"environment.width", "10.0"}
    });
    
    // Create child template with inheritance
    nlm::Template childTemplate = manager.createTemplate("phase3", {
        {"brain.region_count", "2"},
        {"brain.use_attention", "true"}
    });
    
    // Set inheritance relationship
    assert(manager.setTemplateInheritance("phase3", "base"));
    
    // Expand child template (should inherit from base)
    auto expanded = manager.expandTemplate("phase3", {
        {"brain.neuron_count", "150"} // Override base value
    });
    
    assert(expanded.getInt("brain.neuron_count") == 150); // Overridden
    assert(expanded.getInt("brain.region_count") == 2);    // Child value
    assert(std::abs(expanded.getFloat("environment.width") - 10.0f) < 0.001f); // Inherited
    assert(expanded.getBool("brain.use_attention") == true);   // Child value
    
    std::cout << "    testTemplateExpansion passed" << std::endl;
}

void testConfigSecurity() {
    nlm::ConfigurationSecurityManager security;
    
    // Test sensitive data masking
    nlm::ConfigSection section;
    section.addValue("api_key", "secret_key_123");
    section.addValue("database_password", "db_password_456");
    section.addValue("public_setting", "public_value");
    
    auto masked = security.maskSensitiveData(section);
    assert(masked.getValue("api_key") != "secret_key_123");
    assert(masked.getValue("database_password") != "db_password_456");
    assert(masked.getValue("public_setting") == "public_value");
    
    // Test configuration integrity
    std::string configHash = security.computeConfigHash(section);
    assert(!configHash.empty());
    
    // Test configuration tampering detection
    nlm::ConfigSection modified = section;
    modified.addValue("tampered_field", "should_be_detected");
    assert(security.detectTampering(section, modified));
    
    std::cout << "    testConfigSecurity passed" << std::endl;
}

void testConfigValidationEdgeCases() {
    nlm::AdvancedConfigValidator validator;
    
    // Test extreme values
    validator.testExtremeValueValidation(1e10f, 1e-10f); // Very large and very small
    validator.testExtremeIntValidation(std::numeric_limits<int>::max(), 
                                         std::numeric_limits<int>::min());
    
    // Test boundary values
    validator.testBoundaryValueValidation(0.0f, 1.0f); // Valid boundaries
    validator.testBoundaryValueValidation(-1.0f, 2.0f); // Invalid boundaries
    
    // Test malformed configurations
    validator.testMalformedConfigValidation("<invalid_xml>");
    validator.testMalformedConfigValidation("{\"missing_comma\":\"value1\"\"value2\"}");
    
    std::cout << "    testConfigValidationEdgeCases passed" << std::endl;
}

void runAll() {
    testAdvancedConfigValidation();
    testTemplateConfiguration();
    testTemplateExpansion();
    testConfigSecurity();
    testConfigValidationEdgeCases();
}

} // namespace test_advanced_config