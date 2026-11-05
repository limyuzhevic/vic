#include "ConfigSetCommand.hpp"
#include "core/Config/Config.hpp"
#include <iostream>

bool ConfigSetCommand::execute(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cerr << "Error: Missing key or value arguments" << std::endl;
        std::cerr << "Usage: nlm config set <key> <value>" << std::endl;
        return false;
    }
    
    std::string key = args[0];
    std::string value = args[1];
    auto config = std::make_shared<Config>();
    
    // Try to determine value type
    try {
        // Try integer
        size_t pos;
        int intValue = std::stoi(value, &pos);
        if (pos == value.size()) {
            config->set(key, intValue);
            std::cout << "Set " << key << " = " << intValue << std::endl;
            return true;
        }
    } catch (...) {}
    
    try {
        // Try double
        size_t pos;
        double doubleValue = std::stod(value, &pos);
        if (pos == value.size()) {
            config->set(key, doubleValue);
            std::cout << "Set " << key << " = " << doubleValue << std::endl;
            return true;
        }
    } catch (...) {}
    
    try {
        // Try boolean
        if (value == "true" || value == "false") {
            bool boolValue = (value == "true");
            config->set(key, boolValue);
            std::cout << "Set " << key << " = " << (boolValue ? "true" : "false") << std::endl;
            return true;
        }
    } catch (...) {}
    
    // Default to string
    config->set(key, value);
    std::cout << "Set " << key << " = " << value << std::endl;
    return true;
}