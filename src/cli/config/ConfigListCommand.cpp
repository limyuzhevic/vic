#include "ConfigListCommand.hpp"
#include "core/Config/Config.hpp"
#include <iostream>

bool ConfigListCommand::execute(const std::vector<std::string>& args) {
    auto config = std::make_shared<Config>();
    
    std::string format = "list";
    if (!args.empty()) {
        format = args[0];
    }
    
    auto keys = config->getKeys();
    
    if (format == "detailed") {
        std::cout << "Detailed Configuration:" << std::endl;
        std::cout << std::left << std::setw(20) << "Key" 
                  << std::setw(15) << "Value" 
                  << std::setw(10) << "Source" 
                  << "Description" << std::endl;
        std::cout << std::string(70, '-') << std::endl;
        
        for (const auto& key : keys) {
            auto value = config->get<std::string>(key);
            std::cout << std::left << std::setw(20) << key
                      << std::setw(15) << (value ? *value : "<empty>")
                      << "Config" << std::setw(10) << ""
                      << "(config entry)" << std::endl;
        }
    } else {
        std::cout << "Configuration Options:" << std::endl;
        for (const auto& key : keys) {
            auto value = config->get<std::string>(key);
            std::cout << "  " << key << " = " << (value ? *value : "<empty>") << std::endl;
        }
    }
    
    return true;
}