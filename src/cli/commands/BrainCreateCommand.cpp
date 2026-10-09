#include "BrainCreateCommand.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <map>
#include <sstream>

namespace nlm {

int BrainCreateCommand::execute(const std::vector<std::string>& args) {
    // Parse arguments
    if (args.empty()) {
        std::cerr << "Error: Brain name required" << std::endl;
        std::cerr << "Usage: brain create <name> [--neurons N] [--regions R] [--config FILE]" << std::endl;
        return 1;
    }
    
    std::string brainName = args[0];
    
    // Parse optional arguments
    std::map<std::string, std::string> options;
    std::vector<std::string> configKeys;
    
    for (size_t i = 1; i < args.size(); ++i) {
        std::string arg = args[i];
        if (arg.substr(0, 2) == "--") {
            // Option with value
            if (i + 1 < args.size() && args[i + 1].substr(0, 2) != "--") {
                options[arg] = args[++i];
            } else {
                options[arg] = "";
            }
        } else {
            // Config value
            configKeys.push_back(arg);
        }
    }
    
    // Create configuration
    auto config = std::make_shared<Config>();
    
    // Set default values from config file
    std::string defaultConfig = "configs/default.cfg";
    if (options.find("--config") != options.end() && !options["--config"].empty()) {
        defaultConfig = options["--config"];
    }
    
    // Load default config
    if (!config->loadFromFile(defaultConfig)) {
        std::cout << "Warning: Could not load config file " << defaultConfig << ". Using defaults." << std::endl;
    }
    
    // Override with command line options
    auto setConfigValue = [&](const std::string& key, const std::string& valueStr) {
        if (valueStr.empty()) return;
        
        // Try to parse as different types
        try {
            // Check if it's a boolean
            if (valueStr == "true" || valueStr == "false") {
                bool boolVal = (valueStr == "true");
                config->set(key, boolVal, ConfigSource::CommandLine);
            }
            // Check if it's an integer
            else {
                size_t pos;
                int intVal = std::stoi(valueStr, &pos);
                if (pos == valueStr.length()) {
                    config->set(key, intVal, ConfigSource::CommandLine);
                } else {
                    // Try as double
                    double doubleVal = std::stod(valueStr, &pos);
                    if (pos == valueStr.length()) {
                        config->set(key, doubleVal, ConfigSource::CommandLine);
                    } else {
                        // Treat as string
                        config->set(key, valueStr, ConfigSource::CommandLine);
                    }
                }
            }
        } catch (const std::exception&) {
            // Fall back to string
            config->set(key, valueStr, ConfigSource::CommandLine);
        }
    };
    
    // Process options
    for (const auto& option : options) {
        std::string key = option.first.substr(2); // Remove "--"
        setConfigValue(key, option.second);
    }
    
    // Process config keys
    for (const auto& key : configKeys) {
        if (key.find('=') != std::string::npos) {
            size_t equalsPos = key.find('=');
            std::string keyName = key.substr(0, equalsPos);
            std::string value = key.substr(equalsPos + 1);
            setConfigValue(keyName, value);
        }
    }
    
    // Set brain-specific name
    config->set("brain_name", brainName, ConfigSource::Runtime);
    
    // Create brain with configuration
    try {
        brain = std::make_shared<Brain>(config);
        
        // Initialize the brain
        if (!brain->initialize()) {
            std::cerr << "Error: Failed to initialize brain" << std::endl;
            return 1;
        }
        
        // Store brain in a global registry (in a real implementation)
        std::cout << "✓ Created and initialized brain: " << brainName << std::endl;
        std::cout << "  Configuration summary:" << std::endl;
        
        // Show key configuration values
        if (config->has("neuron_count")) {
            std::cout << "  Neurons: " << config->getOr<int64_t>("neuron_count", 0) << std::endl;
        }
        if (config->has("region_count")) {
            std::cout << "  Regions: " << config->getOr<int64_t>("region_count", 0) << std::endl;
        }
        if (config->has("simulation_timestep")) {
            std::cout << "  Timestep: " << config->getOr<double>("simulation_timestep", 0.0) << "s" << std::endl;
        }
        if (config->has("random_seed")) {
            std::cout << "  Random seed: " << config->getOr<int64_t>("random_seed", 0) << std::endl;
        }
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: Failed to create brain: " << e.what() << std::endl;
        return 1;
    }
}

} // namespace nlm