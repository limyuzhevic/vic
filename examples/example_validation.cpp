// NLM Configuration Examples
// Demonstrates configuration validation capabilities

#include "src/core/Config/Config.hpp"
#include <iostream>

int main() {
    nlm::Config config;
    
    // Add validation rules
    config.addValidationRule({
        "neuron_count",
        [](const nlm::ConfigValue& val) {
            if (std::holds_alternative<int>(val)) {
                return std::get<int>(val) > 0 && std::get<int>(val) <= 10000;
            }
            return false;
        },
        "Neuron count must be between 1 and 10000"
    });
    
    config.addValidationRule({
        "simulation_timestep",
        [](const nlm::ConfigValue& val) {
            if (std::holds_alternative<double>(val)) {
                return std::get<double>(val) > 0.0 && std::get<double>(val) <= 1.0;
            }
            return false;
        },
        "Simulation timestep must be between 0.0001 and 1.0"
    });
    
    // Test configuration
    config.set("neuron_count", 1000);
    config.set("simulation_timestep", 0.001);
    
    if (!config.isValid()) {
        std::cout << "Configuration is invalid:" << std::endl;
        auto errors = config.getValidationErrors();
        for (const auto& error : errors) {
            std::cout << "  - " << error << std::endl;
        }
    } else {
        std::cout << "Configuration is valid!" << std::endl;
    }
    
    return 0;
}
