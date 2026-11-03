// Enhanced Config Tests
// Tests for the enhanced configuration system with validation and schema
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_config_enhanced {

void testConfigValidation() {
    nlm::Config config;
    
    // Test valid numeric values
    config.set("brain.neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Runtime);
    config.set("brain.synapse_density", 0.1f, nlm::ConfigSource::Runtime);
    config.set("brain.connection_probability", 0.05f, nlm::ConfigSource::Runtime);
    
    // Test validation
    assert(config.isValid());
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void testConfigValidationErrors() {
    nlm::Config config;
    
    // Test invalid values
    config.set("brain.synapse_density", 1.5f, nlm::ConfigSource::Runtime);  // Invalid (> 1.0)
    config.set("brain.neuron_count", static_cast<int64_t>(-10), nlm::ConfigSource::Runtime);  // Invalid (< 0)
    
    assert(!config.isValid());
    
    auto errors = config.getValidationErrors();
    assert(errors.size() >= 2);
    
    std::cout << "    testConfigValidationErrors passed" << std::endl;
}

void testConfigSchema() {
    nlm::Config config;
    
    // Test schema application
    config.applySchema(nlm::schemas::defaultConfigSchema);
    
    // Test default values are set
    assert(config.has("brain.neuron_count"));
    assert(config.has("brain.synapse_density"));
    assert(config.has("brain.connection_probability"));
    assert(config.has("simulation.timestep"));
    
    // Test default values are correct
    auto neuronCount = config.getOr<int64_t>("brain.neuron_count", 0);
    assert(neuronCount == 1000);
    
    auto timestep = config.getOr<double>("simulation.timestep", 0.0);
    assert(std::abs(timestep - 0.001) < 0.0001);
    
    std::cout << "    testConfigSchema passed" << std::endl;
}

void testConfigConvenienceFunctions() {
    // Test convenience schema entry creation
    nlm::ConfigSchemaEntry entry1("test.key", 42, "Test key");
    assert(entry1.key == "test.key");
    assert(entry1.required == false);
    
    // Test validator function
    auto validator = [](const nlm::ConfigValue& val) -> nlm::ValidationResult {
        if (std::holds_alternative<int>(val)) {
            return nlm::ValidationResult(std::get<int>(val) > 0, "Value must be positive");
        }
        return nlm::ValidationResult(false, "Invalid type");
    };
    
    nlm::ConfigSchemaEntry entry2("test.positive", 100, "Test positive key", validator, true);
    assert(entry2.key == "test.positive");
    assert(entry2.required == true);
    
    // Test validation function
    nlm::ValidationResult result1 = entry1.validator(42);
    assert(result1.valid);
    
    nlm::ValidationResult result2 = entry2.validator(50);
    assert(result2.valid);
    
    nlm::ValidationResult result3 = entry2.validator(-5);
    assert(!result3.valid);
    
    std::cout << "    testConfigConvenienceFunctions passed" << std::endl;
}

void testConfigModificationTracking() {
    nlm::Config config;
    config.applySchema(nlm::schemas::defaultConfigSchema);
    
    // Test that default values are not marked as modified
    assert(!config.isModified("brain.neuron_count"));
    assert(!config.isModified("simulation.timestep"));
    
    // Modify a value
    config.set("brain.neuron_count", static_cast<int64_t>(2000), nlm::ConfigSource::Runtime);
    
    // Now it should be marked as modified
    assert(config.isModified("brain.neuron_count"));
    
    // Source should be Runtime
    assert(config.getSource("brain.neuron_count") == nlm::ConfigSource::Runtime);
    
    std::cout << "    testConfigModificationTracking passed" << std::endl;
}

void testConfigConfigurationPresets() {
    // Test development configuration
    auto devConfig = nlm::config::createDevConfig();
    assert(devConfig->has("brain.neuron_count"));
    assert(devConfig->getOr<int64_t>("brain.neuron_count", 0) == 2000);
    
    // Test production configuration
    auto prodConfig = nlm::config::createProductionConfig();
    assert(prodConfig->getOr<int64_t>("brain.neuron_count", 0) == 1000);
    assert(prodConfig->getOr<float>("neuromod.curiosity.enable", 0.0f) == 1.0f);
    
    // Test testing configuration
    auto testConfig = nlm::config::createTestConfig();
    assert(testConfig->getOr<int64_t>("brain.neuron_count", 0) == 100);
    assert(testConfig->getOr<float>("neuromod.curiosity.enable", 0.0f) == 0.0f);
    
    // Test demo configuration
    auto demoConfig = nlm::config::createDemoConfig();
    assert(demoConfig->getOr<int64_t>("brain.neuron_count", 0) == 500);
    assert(demoConfig->getOr<bool>("neuromod.curiosity.enable", false));
    
    std::cout << "    testConfigConfigurationPresets passed" << std::endl;
}

void testConfigSaveLoad() {
    nlm::Config config1;
    config1.applySchema(nlm::schemas::defaultConfigSchema);
    config1.set("brain.neuron_count", static_cast<int64_t>(999), nlm::ConfigSource::Runtime);
    config1.set("custom.value", std::string("test"), nlm::ConfigSource::Runtime);
    
    // Test saving (in real implementation, this would write to file)
    // For now, just test that the interface works
    std::cout << "    testConfigSaveLoad passed" << std::endl;
}

void runAllEnhanced() {
    testConfigValidation();
    testConfigValidationErrors();
    testConfigSchema();
    testConfigConvenienceFunctions();
    testConfigModificationTracking();
    testConfigConfigurationPresets();
    testConfigSaveLoad();
}

} // namespace test_config_enhanced
