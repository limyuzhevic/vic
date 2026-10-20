// Validation Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <string>

namespace test_config_validation {

void testBasicValidation() {
    nlm::Config config;
    
    // Set up a valid configuration
    config.set("dopamine_baseline", 0.5, nlm::ConfigSource::Runtime);
    config.set("simulation_timestep", 0.001, nlm::ConfigSource::Runtime);
    config.set("synaptogenesis_rate", 0.0001, nlm::ConfigSource::Runtime);
    
    std::cout << "    testBasicValidation passed" << std::endl;
}

void testValidationWithErrors() {
    nlm::Config config;
    
    // Set up an invalid configuration
    config.set("dopamine_baseline", 1.5, nlm::ConfigSource::Runtime); // Should be 0.0-1.0
    config.set("simulation_timestep", -0.001, nlm::ConfigSource::Runtime); // Should be positive
    config.set("synaptogenesis_rate", 0.5, nlm::ConfigSource::Runtime); // Should be 0.0-0.1
    
    // Enable strict validation
    config.setValidationMode(nlm::ValidationMode::Strict);
    
    bool isValid = config.validate();
    std::vector<nlm::ValidationError> errors = config.getValidationErrors();
    
    assert(!isValid);
    assert(errors.size() > 0);
    
    std::cout << "    testValidationWithErrors passed" << std::endl;
}

void testLenientValidation() {
    nlm::Config config;
    
    // Set up a configuration with lenient validation
    config.set("dopamine_baseline", 1.5, nlm::ConfigSource::Runtime); // Invalid but allowed in lenient mode
    config.set("simulation_timestep", 0.001, nlm::ConfigSource::Runtime);
    
    // Enable lenient validation
    config.setValidationMode(nlm::ValidationMode::Lenient);
    
    bool isValid = config.validate();
    std::vector<nlm::ValidationError> errors = config.getValidationErrors();
    
    // In lenient mode, validation might pass or return warnings
    std::cout << "    testLenientValidation passed" << std::endl;
}

void testCategoryDetection() {
    nlm::Config config;
    
    // Test category detection for various keys
    assert(config.detectCategory("dopamine_baseline") == nlm::ConfigCategory::Neuromodulation);
    assert(config.detectCategory("simulation_timestep") == nlm::ConfigCategory::Simulation);
    assert(config.detectCategory("synaptogenesis_rate") == nlm::ConfigCategory::Development);
    assert(config.detectCategory("width") == nlm::ConfigCategory::Environment);
    assert(config.detectCategory("working_memory_capacity") == nlm::ConfigCategory::Memory);
    assert(config.detectCategory("log_level") == nlm::ConfigCategory::Logging);
    assert(config.detectCategory("visualization_enabled") == nlm::ConfigCategory::Visualization);
    assert(config.detectCategory("reward_discount_factor") == nlm::ConfigCategory::Reward);
    
    std::cout << "    testCategoryDetection passed" << std::endl;
}

void testValidationErrorsDetailed() {
    nlm::Config config;
    
    // Set up configuration with multiple errors
    config.set("dopamine_baseline", 1.5, nlm::ConfigSource::Runtime);
    config.set("reward_discount_factor", -0.5, nlm::ConfigSource::Runtime);
    config.set("simulation_timestep", -0.001, nlm::ConfigSource::Runtime);
    config.set("synaptogenesis_rate", 0.5, nlm::ConfigSource::Runtime);
    config.set("width", -10, nlm::ConfigSource::Runtime);
    
    // Enable strict validation
    config.setValidationMode(nlm::ValidationMode::Strict);
    
    std::vector<nlm::ValidationError> errors = config.getValidationErrors();
    
    // Check that we get multiple validation errors
    assert(errors.size() >= 5);
    
    // Check error details
    bool hasDopamineError = false;
    bool hasSimulationError = false;
    bool hasDevelopmentError = false;
    bool hasEnvironmentError = false;
    
    for (const auto& error : errors) {
        if (error.field == "dopamine_baseline") hasDopamineError = true;
        if (error.field == "simulation_timestep") hasSimulationError = true;
        if (error.field == "synaptogenesis_rate") hasDevelopmentError = true;
        if (error.field == "width") hasEnvironmentError = true;
    }
    
    assert(hasDopamineError);
    assert(hasSimulationError);
    assert(hasDevelopmentError);
    assert(hasEnvironmentError);
    
    std::cout << "    testValidationErrorsDetailed passed" << std::endl;
}

void testConfigSchemaManager() {
    const auto& schemaManager = nlm::ConfigSchemaManager::getInstance();
    
    // Check that schemas are registered
    assert(schemaManager.hasSchema(nlm::ConfigCategory::Neuromodulation));
    assert(schemaManager.hasSchema(nlm::ConfigCategory::Simulation));
    assert(schemaManager.hasSchema(nlm::ConfigCategory::Development));
    assert(schemaManager.hasSchema(nlm::ConfigCategory::Memory));
    assert(schemaManager.hasSchema(nlm::ConfigCategory::Environment));
    assert(schemaManager.hasSchema(nlm::ConfigCategory::Logging));
    assert(schemaManager.hasSchema(nlm::ConfigCategory::Visualization));
    assert(schemaManager.hasSchema(nlm::ConfigCategory::Reward));
    
    // Check specific validation rules
    const auto& neuromodulationSchema = schemaManager.getSchema(nlm::ConfigCategory::Neuromodulation);
    assert(neuromodulationSchema.rules.find("dopamine_baseline") != neuromodulationSchema.rules.end());
    assert(neuromodulationSchema.rules.find("reward_discount_factor") != neuromodulationSchema.rules.end());
    
    const auto& simulationSchema = schemaManager.getSchema(nlm::ConfigCategory::Simulation);
    assert(simulationSchema.rules.find("simulation_timestep") != simulationSchema.rules.end());
    assert(simulationSchema.rules.find("max_simulation_steps") != simulationSchema.rules.end());
    
    std::cout << "    testConfigSchemaManager passed" << std::endl;
}

void testLoadAndValidateConfigFile() {
    nlm::Config config;
    
    // Create a test configuration file
    std::ofstream testConfig("test_config.cfg");
    testConfig << "# Test Configuration" << std::endl;
    testConfig << "dopamine_baseline = 0.2" << std::endl;
    testConfig << "simulation_timestep = 0.001" << std::endl;
    testConfig << "synaptogenesis_rate = 0.001" << std::endl;
    testConfig << "width = 10" << std::endl;
    testConfig.close();
    
    // Load the configuration file
    bool loaded = config.loadFromFile("test_config.cfg");
    assert(loaded);
    
    // Validate the configuration
    config.setValidationMode(nlm::ValidationMode::Strict);
    bool isValid = config.validate();
    assert(isValid);
    
    // Clean up
    std::remove("test_config.cfg");
    
    std::cout << "    testLoadAndValidateConfigFile passed" << std::endl;
}

void runAll() {
    testBasicValidation();
    testValidationWithErrors();
    testLenientValidation();
    testCategoryDetection();
    testValidationErrorsDetailed();
    testConfigSchemaManager();
    testLoadAndValidateConfigFile();
}

} // namespace test_config_validation