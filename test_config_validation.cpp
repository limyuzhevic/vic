#include "../core/Config/Config.hpp"
#include "../core/Config/ConfigValidator.hpp"
#include <iostream>
#include <vector>

// Test the configuration validation system
void testConfigValidation() {
    std::cout << "=== Configuration Validation Test ===" << std::endl << std::endl;
    
    // Create validator
    nlm::ConfigValidator validator;
    
    // Register parameters with validation rules
    validator.registerParameter("neuron_count", "int64_t", ConfigValue(1000),
                               "Number of neurons in the brain", true);
    validator.addValidator<int64_t>("neuron_count",
        [](int64_t val, std::string& error) {
            if (val <= 0) {
                error = "neuron_count must be positive, got: " + std::to_string(val);
                return false;
            }
            if (val > 100000) {
                error = "neuron_count too large, max is 100000: " + std::to_string(val);
                return false;
            }
            return true;
        });
    
    validator.registerParameter("connection_probability", "double", ConfigValue(0.1),
                               "Connection probability for random connectivity");
    validator.addValidator<double>("connection_probability",
        [](double val, std::string& error) {
            if (val < 0.0 || val > 1.0) {
                error = "connection_probability must be between 0.0 and 1.0, got: " + std::to_string(val);
                return false;
            }
            return true;
        });
    
    validator.registerParameter("simulation_timestep", "double", ConfigValue(0.001),
                               "Simulation timestep in seconds");
    validator.addValidator<double>("simulation_timestep",
        [](double val, std::string& error) {
            if (val <= 0.0 || val > 1.0) {
                error = "simulation_timestep must be positive and <= 1.0, got: " + std::to_string(val);
                return false;
            }
            return true;
        });
    
    validator.registerParameter("region_count", "int64_t", ConfigValue(1),
                               "Number of brain regions");
    
    validator.registerParameter("log_level", "string", ConfigValue(std::string("INFO")),
                               "Log level (DEBUG, INFO, WARNING, ERROR)");
    validator.addValidator<std::string>("log_level",
        [](const std::string& val, std::string& error) {
            std::vector<std::string> validLevels = {"DEBUG", "INFO", "WARNING", "ERROR"};
            if (std::find(validLevels.begin(), validLevels.end(), val) == validLevels.end()) {
                error = "log_level must be one of: DEBUG, INFO, WARNING, ERROR, got: " + val;
                return false;
            }
            return true;
        });
    
    std::cout << "Validation Summary:" << std::endl;
    std::cout << validator.getValidationSummary() << std::endl;
    
    // Test 1: Valid configuration
    std::cout << "\n=== Test 1: Valid Configuration ===" << std::endl;
    nlm::Config config1;
    config1.set("neuron_count", 500);
    config1.set("connection_probability", 0.15);
    config1.set("simulation_timestep", 0.001);
    config1.set("region_count", 2);
    config1.set("log_level", std::string("INFO"));
    
    std::vector<std::string> errors1;
    bool valid1 = validator.validate(config1, errors1);
    
    if (valid1) {
        std::cout << "✓ Configuration is valid" << std::endl;
        std::cout << "Neuron count: " << config1.getOr<int64_t>("neuron_count", 0) << std::endl;
        std::cout << "Connection probability: " << config1.getOr<double>("connection_probability", 0.0) << std::endl;
    } else {
        std::cout << "✗ Configuration has errors:" << std::endl;
        for (const auto& error : errors1) {
            std::cout << "  - " << error << std::endl;
        }
    }
    
    // Test 2: Invalid configuration (negative neuron count)
    std::cout << "\n=== Test 2: Invalid Configuration (negative neuron count) ===" << std::endl;
    nlm::Config config2;
    config2.set("neuron_count", -100);
    config2.set("connection_probability", 0.15);
    config2.set("simulation_timestep", 0.001);
    config2.set("region_count", 2);
    config2.set("log_level", std::string("INFO"));
    
    std::vector<std::string> errors2;
    bool valid2 = validator.validate(config2, errors2);
    
    if (valid2) {
        std::cout << "✓ Configuration is valid (unexpected)" << std::endl;
    } else {
        std::cout << "✗ Configuration has errors (expected):" << std::endl;
        for (const auto& error : errors2) {
            std::cout << "  - " << error << std::endl;
        }
    }
    
    // Test 3: Invalid configuration (invalid log level)
    std::cout << "\n=== Test 3: Invalid Configuration (invalid log level) ===" << std::endl;
    nlm::Config config3;
    config3.set("neuron_count", 1000);
    config3.set("connection_probability", 1.5); // Invalid (> 1.0)
    config3.set("simulation_timestep", 0.001);
    config3.set("region_count", 2);
    config3.set("log_level", std::string("VERBOSE")); // Invalid level
    
    std::vector<std::string> errors3;
    bool valid3 = validator.validate(config3, errors3);
    
    if (valid3) {
        std::cout << "✓ Configuration is valid (unexpected)" << std::endl;
    } else {
        std::cout << "✗ Configuration has errors (expected):" << std::endl;
        for (const auto& error : errors3) {
            std::cout << "  - " << error << std::endl;
        }
    }
    
    // Test 4: Default configuration
    std::cout << "\n=== Test 4: Default Configuration ===" << std::endl;
    nlm::Config defaultConfig = validator.generateDefaultConfig();
    std::cout << "Default configuration summary:" << std::endl;
    std::cout << defaultConfig.summary() << std::endl;
    
    std::cout << "\n=== All Tests Complete ===" << std::endl;
}

int main() {
    testConfigValidation();
    return 0;
}