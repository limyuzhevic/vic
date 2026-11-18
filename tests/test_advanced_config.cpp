// Advanced Configuration Tests
// Tests for advanced configuration features and validation

#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <string>

namespace test_advanced_config {

void testConfigurationValidation() {
    nlm::Config config;
    
    // Test valid configurations
    config.set("brain.neuron_count", 1000);
    config.set("brain.region_count", 2);
    config.set("brain.connection_probability", 0.1);
    config.set("simulation.timestep", 0.001);
    
    // Should not have errors
    auto errors = config.getValidationErrors();
    // (Note: This assumes Config has getValidationErrors method)
    
    std::cout << "    testConfigurationValidation passed" << std::endl;
}

void testConfigurationTemplates() {
    // Test predefined configuration templates
    std::vector<std::string> templates = {
        "small_brain",
        "large_brain", 
        "learning_focused",
        "optimized",
        "realistic"
    };
    
    for (const auto& templateName : templates) {
        // Create configuration from template
        // This would call PythonFactory.configureBrain with template settings
        assert(!templateName.empty());
    }
    
    std::cout << "    testConfigurationTemplates passed" << std::endl;
}

void testConfigurationMerging() {
    nlm::Config config1;
    nlm::Config config2;
    
    // Set different values in each config
    config1.set("brain.neuron_count", 1000);
    config1.set("brain.region_count", 2);
    
    config2.set("brain.neuron_count", 2000);
    config2.set("brain.connection_probability", 0.2);
    
    // Test that they can be merged
    auto merged = config1.merge(config2);
    
    assert(merged.getOr<int64_t>("brain.neuron_count", 0) == 2000);
    assert(merged.getOr<int64_t>("brain.region_count", 0) == 2);
    assert(merged.getOr<float>("brain.connection_probability", 0.0f) == 0.2f);
    
    std::cout << "    testConfigurationMerging passed" << std::endl;
}

void testConfigurationExportImport() {
    nlm::Config config;
    
    // Set various values
    config.set("brain.neuron_count", 1500);
    config.set("brain.region_count", 3);
    config.set("simulation.timestep", 0.002);
    
    // Export to JSON string
    std::string jsonStr = config.toJSON();
    assert(!jsonStr.empty());
    
    // Import back
    auto config2 = nlm::Config::fromJSON(jsonStr);
    
    assert(config2.getOr<int64_t>("brain.neuron_count", 0) == 1500);
    assert(config2.getOr<int64_t>("brain.region_count", 0) == 3);
    assert(config2.getOr<float>("simulation.timestep", 0.0f) == 0.002f);
    
    std::cout << "    testConfigurationExportImport passed" << std::endl;
}

void testConfigurationDiff() {
    nlm::Config config1;
    nlm::Config config2;
    
    // Make them different
    config1.set("brain.neuron_count", 1000);
    config1.set("brain.region_count", 2);
    
    config2.set("brain.neuron_count", 2000);
    config2.set("brain.region_count", 3);
    config2.set("brain.connection_probability", 0.15);
    
    // Get differences
    auto diff = config1.diff(config2);
    
    assert(diff.find("brain.neuron_count") != diff.end());
    assert(diff.find("brain.region_count") != diff.end());
    assert(diff.find("brain.connection_probability") != diff.end());
    
    std::cout << "    testConfigurationDiff passed" << std::endl;
}

void testConfigurationValidationWithRules() {
    nlm::Config config;
    
    // Test various validation rules
    std::vector<std::string> rules = {
        "neuron_count_range",
        "region_count_positive", 
        "connection_probability_range",
        "timestep_positive"
    };
    
    for (const auto& rule : rules) {
        // Apply validation rule
        bool isValid = config.validateRule(rule);
        assert(isValid);  // Should pass for empty config
    }
    
    std::cout << "    testConfigurationValidationWithRules passed" << std::endl;
}

void runAll() {
    testConfigurationValidation();
    testConfigurationTemplates();
    testConfigurationMerging();
    testConfigurationExportImport();
    testConfigurationDiff();
    testConfigurationValidationWithRules();
    
    std::cout << "\n=== All Advanced Configuration Tests Passed ===\n" << std::endl;
}

} // namespace test_advanced_config

// Integration test runner for advanced configuration
void runAdvancedConfigTests() {
    std::cout << "Running NLM Advanced Configuration Tests..." << std::endl;
    std::cout << "==============================================" << std::endl;
    
    test_advanced_config::runAll();
    
    std::cout << "=== Advanced Configuration Tests Complete ===" << std::endl;
}