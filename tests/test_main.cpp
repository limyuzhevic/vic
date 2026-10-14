#include "core/Config/Config.hpp"
#include "core/Config/ConfigInitializer.hpp"
#include "core/Config/ConfigValidator.hpp"
#include <iostream>
#include <any>
#include <unordered_map>

int main() {
    std::cout << "=== NLM Configuration System Test ===\n" << std::endl;
    
    // Initialize configuration system
    std::cout << "1. Initializing configuration system..." << std::endl;
    nlm::ConfigInitializer::initializeAllParameters();
    
    // Show schema documentation
    std::cout << "\n2. Configuration Schema Documentation:" << std::endl;
    std::cout << nlm::ConfigInitializer::getParameterDescriptions();
    
    // Create and test basic configuration
    std::cout << "\n3. Testing basic configuration operations..." << std::endl;
    nlm::Config config;
    
    // Set some values
    config.set("random_seed", 42, nlm::ConfigSource::Runtime);
    config.set("simulation_timestep", 0.001, nlm::ConfigSource::Runtime);
    config.set("neuron_count", 1000LL, nlm::ConfigSource::Runtime);
    config.set("region_count", 2LL, nlm::ConfigSource::Runtime);
    config.set("connection_probability", 0.1f, nlm::ConfigSource::Runtime);
    
    // Show configuration summary
    std::cout << "Configuration Summary:" << std::endl;
    std::cout << config.summary() << std::endl;
    
    // Test default values
    std::cout << "\n4. Testing default values..." << std::endl;
    config.setPhase2Defaults();
    std::cout << "Default values set successfully" << std::endl;
    
    // Test configuration validation
    std::cout << "\n5. Testing configuration validation..." << std::endl;
    bool isValid = nlm::ConfigInitializer::validateConfiguration(config);
    std::cout << "Configuration valid: " << (isValid ? "YES" : "NO") << std::endl;
    
    // Show validation report
    std::cout << "\n6. Validation Report:" << std::endl;
    auto errors = nlm::ConfigInitializer::getValidationErrors();
    if (errors.empty()) {
        std::cout << "✓ No validation errors found" << std::endl;
    } else {
        for (const auto& error : errors) {
            std::cout << "  " << error.severity << ": " << error.parameter << " - " << error.message << std::endl;
        }
    }
    
    // Test parameter validation
    std::cout << "\n7. Testing parameter validation..." << std::endl;
    std::unordered_map<std::string, std::any> testConfig;
    testConfig["random_seed"] = std::any(42);
    testConfig["neuron_count"] = std::any(1000LL);
    testConfig["simulation_timestep"] = std::any(0.001);
    
    auto& validator = nlm::ConfigInitializer::getValidator();
    auto paramErrors = validator.validate(testConfig);
    std::cout << "Parameter validation errors: " << paramErrors.size() << std::endl;
    
    std::cout << "\n=== Configuration System Test Complete ===\n" << std::endl;
    
    return 0;
}