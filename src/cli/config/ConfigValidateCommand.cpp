#include "ConfigValidateCommand.hpp"
#include "core/Config/Config.hpp"
#include <iostream>

bool ConfigValidateCommand::execute(const std::vector<std::string>& args) {
    auto config = std::make_shared<Config>();
    
    std::cout << "Validating configuration..." << std::endl;
    
    // For demonstration, just check if we can access the configuration
    auto keys = config->getKeys();
    
    if (keys.empty()) {
        std::cout << "Warning: No configuration keys found" << std::endl;
    }
    
    // Check some example required configurations
    std::vector<std::string> requiredKeys = {"random_seed", "simulation_timestep"};
    bool allValid = true;
    
    for (const auto& key : requiredKeys) {
        if (config->get<std::string>(key)) {
            std::cout << "✓ " << key << " is set" << std::endl;
        } else {
            std::cout << "! " << key << " is not set (using default)" << std::endl;
            allValid = false;
        }
    }
    
    if (allValid) {
        std::cout << "Configuration validation passed" << std::endl;
    } else {
        std::cout << "Configuration validation completed with warnings" << std::endl;
    }
    
    return true;
}