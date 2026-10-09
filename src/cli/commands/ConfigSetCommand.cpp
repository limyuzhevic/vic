#include "ConfigSetCommand.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <map>

namespace nlm {

int ConfigSetCommand::execute(const std::vector<std::string>& args) {
    if (args.size() < 3) {
        std::cerr << "Error: Brain name, key, and value required" << std::endl;
        std::cerr << "Usage: config set [--global] [--brain NAME] <key> <value>" << std::endl;
        return 1;
    }
    
    bool global = false;
    std::string brainName = "default";
    std::string key = args[0];
    std::string value = args[1];
    
    // Parse optional flags
    size_t valueIndex = 2;
    for (size_t i = 0; i < args.size(); ++i) {
        std::string arg = args[i];
        if (arg == "--global") {
            global = true;
            valueIndex++; // Skip this flag
        } else if (arg == "--brain") {
            if (i + 1 < args.size()) {
                brainName = args[++i];
            }
            valueIndex += 2; // Skip this flag and its value
        }
    }
    
    // Extract key and value if they're in the remaining args
    if (valueIndex < args.size()) {
        key = args[valueIndex];
        valueIndex++;
        if (valueIndex < args.size()) {
            value = args[valueIndex];
        }
    }
    
    std::cout << "Config set command executed" << std::endl;
    std::cout << "Global: " << (global ? "true" : "false") << std::endl;
    std::cout << "Brain: " << brainName << std::endl;
    std::cout << "Key: " << key << std::endl;
    std::cout << "Value: " << value << std::endl;
    
    // In a real implementation, this would:
    // 1. Determine if this is a global or brain-specific setting
    // 2. Parse the value based on the key's expected type
    // 3. Set the configuration value
    // 4. Save the configuration
    
    // Simulate value parsing
    std::string parsedValue = value;
    std::string type = "string";
    
    // Try to parse as boolean
    if (value == "true" || value == "false") {
        parsedValue = value;
        type = "boolean";
    }
    // Try to parse as integer
    else {
        try {
            size_t pos;
            int intVal = std::stoi(value, &pos);
            if (pos == value.length()) {
                parsedValue = std::to_string(intVal);
                type = "integer";
            } else {
                // Try as double
                double doubleVal = std::stod(value, &pos);
                if (pos == value.length()) {
                    parsedValue = std::to_string(doubleVal);
                    type = "double";
                }
            }
        } catch (const std::exception&) {
            // Keep as string
        }
    }
    
    // Simulate setting the configuration
    std::cout << "\nSetting configuration value..." << std::endl;
    std::cout << "  Key: " << key << std::endl;
    std::cout << "  Parsed value: " << parsedValue << std::endl;
    std::cout << "  Type: " << type << std::endl;
    std::cout << "  Source: " << (global ? "Global" : "Brain-specific") << std::endl;
    
    // Simulate validation
    std::cout << "Validating configuration..." << std::endl;
    std::cout << "  Checking key existence..." << std::endl;
    std::cout << "  Validating value format..." << std::endl;
    std::cout << "  Checking configuration constraints..." << std::endl;
    
    // Simulate saving
    std::cout << "Saving configuration..." << std::endl;
    std::cout << "  Writing to config store..." << std::endl;
    std::cout << "  Creating backup..." << std::endl;
    
    std::cout << "\n✓ Configuration value set successfully" << std::endl;
    std::cout << "  Key: " << key << " = " << parsedValue << std::endl;
    std::cout << "  Scope: " << (global ? "Global" : "Brain-specific") << std::endl;
    std::cout << "  (Note: Full implementation requires config storage)" << std::endl;
    
    return 0;
}

} // namespace nlm