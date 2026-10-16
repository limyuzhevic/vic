#include "core/Config/Config.hpp"
#include "core/Config/ConfigValidator.hpp"
#include <iostream>

int main() {
    std::cout << "Testing NLM Configuration Validation System" << std::endl;
    
    // Create a Config object
    nlm::Config config;
    
    // Create a validator with strict mode
    auto validator = std::make_unique<nlm::NLMConfigValidator>(nlm::ValidationMode::Strict);
    
    // Set the validator
    config.setValidator(std::move(validator));
    
    // Set some valid configuration
    config.set("neuron_count", static_cast<int64_t>(1000));
    config.set("simulation_timestep", 0.001);
    
    std::cout << "Valid configuration set successfully" << std::endl;
    
    // Try to set invalid configuration (should throw)
    try {
        config.set("neuron_count", static_cast<int64_t>(0)); // Invalid: below minimum
        std::cout << "ERROR: Should have thrown ValidationException!" << std::endl;
        return 1;
    } catch (const nlm::ValidationException& e) {
        std::cout << "Correctly caught validation error: " << e.what() << std::endl;
    }
    
    // Test warning mode
    auto warningValidator = std::make_unique<nlm::NLMConfigValidator>(nlm::ValidationMode::Warning);
    config.setValidator(std::move(warningValidator));
    
    // This should log a warning but not throw
    config.set("neuron_count", static_cast<int64_t>(500000)); // Valid but near max
    
    std::cout << "Warning mode test passed" << std::endl;
    
    // Test validation results
    nlm::ValidationResult result = config.validate();
    std::cout << "Validation result isValid: " << (result.isValid ? "true" : "false") << std::endl;
    
    if (!result.isValid) {
        std::cout << "Validation errors:" << std::endl;
        for (const auto& error : result.errors) {
            std::cout << "  - " << error->what() << std::endl;
        }
    }
    
    std::cout << "All tests passed!" << std::endl;
    return 0;
}
