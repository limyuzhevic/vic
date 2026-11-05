#include "ConfigExportCommand.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <fstream>

bool ConfigExportCommand::execute(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cerr << "Error: Missing format argument" << std::endl;
        std::cerr << "Usage: nlm config export <format> [path]" << std::endl;
        return false;
    }
    
    std::string format = args[0];
    std::string filepath = "config.json"; // Default
    if (args.size() >= 2) {
        filepath = args[1];
    }
    
    std::cout << "Exporting configuration to " << filepath 
              << " in " << format << " format..." << std::endl;
    
    auto config = std::make_shared<Config>();
    
    // For demonstration, create a simple JSON export
    std::ofstream file(filepath);
    if (!file) {
        std::cerr << "Error: Cannot write to file: " << filepath << std::endl;
        return false;
    }
    
    file << "{\n";
    auto keys = config->getKeys();
    
    for (size_t i = 0; i < keys.size(); ++i) {
        file << "  \"" << keys[i] << "\": \"[config value]\"";
        if (i < keys.size() - 1) {
            file << ",";
        }
        file << "\n";
    }
    
    file << "}\n";
    file.close();
    
    std::cout << "Configuration exported to " << filepath << std::endl;
    return true;
}