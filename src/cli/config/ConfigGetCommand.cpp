#include "ConfigGetCommand.hpp"
#include "core/Config/Config.hpp"
#include <iostream>

bool ConfigGetCommand::execute(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cerr << "Error: Missing key argument" << std::endl;
        return false;
    }
    
    std::string key = args[0];
    auto config = std::make_shared<Config>();
    
    auto value = config->get<std::string>(key);
    if (value) {
        std::cout << key << " = " << *value << std::endl;
        return true;
    } else {
        std::cerr << "Error: Key not found: " << key << std::endl;
        return false;
    }
}