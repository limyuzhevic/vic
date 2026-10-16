#include "core/Config/Config.hpp"
#include "core/Config/ConfigSchema.hpp"
#include <iostream>

int main() {
    std::cout << "=== NLM Configuration System with Validation ===\n" << std::endl;
    
    // Create a configuration
    nlm::Config config;
    
    // Load from a configuration file
    if (config.loadFromFile("configs/default.cfg")) {
        std::cout << "Loaded configuration from configs/default.cfg\n" << std::endl;
    }
    
    // Show the configuration
    std::cout << "Configuration:\n" << config.summary() << std::endl;
    
    // Create a schema with validation rules
    nlm::ConfigSchema schema;
    
    // Add range validation for temperature
    schema.addRangeRule("temperature", -50.0, 150.0, nlm::ValidationSeverity::Error, "Temperature must be between -50 and 150 degrees Celsius");
    
    // Add enum validation for environment type
    schema.addEnumRule("environment_type", {"indoor", "outdoor", "marine", "aerospace"}, nlm::ValidationSeverity::Warning, "Recommended environment types");
    
    // Add required validation for experiment name
    schema.addCustomRule("experiment_name", [](const nlm::ConfigValue& val, const std::string& key, const nlm::Config& cfg) {
        auto nameVal = cfg.get<std::string>(key);
        return nameVal.has_value() && !nameVal.value().empty();
    }, nlm::ValidationSeverity::Error, "Experiment name is required and must not be empty");
    
    // Set some configuration values
    config.set("temperature", 25.5, nlm::ConfigSource::Runtime);
    config.set("environment_type", std::string("indoor"), nlm::ConfigSource::Runtime);
    config.set("experiment_name", std::string("Neural Plasticity Experiment"), nlm::ConfigSource::Runtime);
    
    // Validate the configuration
    std::cout << "Validating configuration against schema...\n" << std::endl;
    
    if (config.validate(schema)) {
        std::cout << "✓ Configuration is valid!\n" << std::endl;
    } else {
        std::cout << "✗ Configuration is invalid!\n" << std::endl;
        
        // Get validation errors
        auto errors = config.getValidationErrors(schema);
        std::cout << "Validation errors:\n";
        for (const auto& error : errors) {
            std::cout << "  - [" << (error.severity == nlm::ValidationSeverity::Critical ? "CRITICAL" : 
                                error.severity == nlm::ValidationSeverity::Error ? "ERROR" : "WARNING") 
                      << "] " << error.message << " (field: " << error.fieldName << ")\n";
        }
    }
    
    // Test validation modes
    std::cout << "\n=== Testing Different Validation Modes ===\n" << std::endl;
    
    // Set an invalid value
    config.set("temperature", 200.0, nlm::ConfigSource::Runtime);
    
    std::cout << "Temperature: 200.0 (invalid - out of range -50 to 150)\n" << std::endl;
    
    // Test strict mode
    config.setValidationMode(nlm::Config::ValidationMode::Strict);
    std::cout << "Strict mode: " << (config.validate(schema) ? "VALID" : "INVALID") << std::endl;
    
    // Test lenient mode (only critical errors)
    config.setValidationMode(nlm::Config::ValidationMode::Lenient);
    std::cout << "Lenient mode: " << (config.validate(schema) ? "VALID" : "INVALID") << std::endl;
    
    // Test warnings mode (only warnings)
    config.setValidationMode(nlm::Config::ValidationMode::Warnings);
    std::cout << "Warnings mode: " << (config.validate(schema) ? "VALID" : "INVALID") << std::endl;
    
    // Demonstrate schema merging
    std::cout << "\n=== Demonstrating Schema Merging ===\n" << std::endl;
    
    nlm::ConfigSchema additionalSchema;
    additionalSchema.addRangeRule("pressure", 0.0, 100.0, nlm::ValidationSeverity::Error, "Pressure must be between 0 and 100 psi");
    
    schema.merge(additionalSchema);
    std::cout << "Merged schema with additional pressure validation\n" << std::endl;
    
    config.set("pressure", 50.0, nlm::ConfigSource::Runtime);
    std::cout << "Added pressure: 50.0\n" << std::endl;
    
    if (config.validate(schema)) {
        std::cout << "✓ Configuration with merged schema is valid!\n" << std::endl;
    } else {
        std::cout << "✗ Configuration with merged schema is invalid!\n" << std::endl;
    }
    
    std::cout << "\n=== Configuration Management Demo Complete ===\n" << std::endl;
    
    return 0;
}