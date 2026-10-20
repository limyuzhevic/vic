// Configuration Validation Example
#include "core/Config/Config.hpp"
#include <iostream>
#include <string>

namespace nlm {

void demonstrateValidationFeatures() {
    std::cout << "=== Configuration Validation Demonstration ===" << std::endl;
    
    // Create a configuration
    auto config = std::make_shared<Config>();
    
    // Set up configuration with validation
    std::cout << "\n1. Setting up basic configuration..." << std::endl;
    config->set("dopamine_baseline", 0.5, ConfigSource::Default);
    config->set("reward_discount_factor", 0.99, ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("max_simulation_steps", 10000, ConfigSource::Default);
    config->set("synaptogenesis_rate", 0.0001, ConfigSource::Default);
    config->set("pruning_rate", 0.00001, ConfigSource::Default);
    config->set("width", 10, ConfigSource::Default);
    config->set("height", 10, ConfigSource::Default);
    
    // Enable strict validation
    config->setValidationMode(ValidationMode::Strict);
    
    // Validate configuration
    std::cout << "\n2. Validating configuration (strict mode)..." << std::endl;
    bool isValid = config->validate();
    std::cout << "   Configuration is valid: " << (isValid ? "YES" : "NO") << std::endl;
    
    if (!isValid) {
        std::cout << "   Validation errors:" << std::endl;
        auto errors = config->getValidationErrors();
        for (const auto& error : errors) {
            std::cout << "     - " << error.field << ": " << error.message << std::endl;
        }
    }
    
    // Add some invalid values
    std::cout << "\n3. Adding invalid values..." << std::endl;
    config->set("dopamine_baseline", 1.5, ConfigSource::Runtime); // Invalid: should be 0.0-1.0
    config->set("simulation_timestep", -0.001, ConfigSource::Runtime); // Invalid: should be positive
    config->set("width", -5, ConfigSource::Runtime); // Invalid: should be positive
    
    // Validate again
    std::cout << "\n4. Validating again with invalid values..." << std::endl;
    isValid = config->validate();
    std::cout << "   Configuration is valid: " << (isValid ? "YES" : "NO") << std::endl;
    
    std::cout << "   Validation errors:" << std::endl;
    auto errors = config->getValidationErrors();
    for (const auto& error : errors) {
        std::cout << "     - " << error.field << ": " << error.message << std::endl;
    }
    
    // Demonstrate lenient validation
    std::cout << "\n5. Switching to lenient validation mode..." << std::endl;
    config->setValidationMode(ValidationMode::Lenient);
    isValid = config->validate();
    std::cout << "   Configuration is valid (lenient): " << (isValid ? "YES" : "NO") << std::endl;
    
    std::cout << "\n6. Category detection examples..." << std::endl;
    auto categories = std::vector<std::string>{
        "dopamine_baseline", "simulation_timestep", "synaptogenesis_rate", "width",
        "height", "log_level", "visualization_enabled", "working_memory_capacity"
    };
    
    for (const auto& key : categories) {
        auto category = Config::detectCategory(key);
        std::cout << "   - " << key << " -> " << Config::categoryToString(category) << std::endl;
    }
    
    std::cout << "\n=== Validation Demonstration Complete ===" << std::endl;
}

} // namespace nlm