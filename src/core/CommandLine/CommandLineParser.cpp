// CommandLineParser implementation

#include "CommandLineParser.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

namespace nlm {

void CommandLineParser::processArgument(const std::string& key, const std::string& value) {
    auto it = options.find(key);
    if (it != options.end()) {
        const auto& option = it->second;
        if (option.action) {
            option.action(value);
        }
        parsedArgs[key] = value;
        
        // Handle special arguments
        if (key == "debug") {
            try {
                debugLevel = std::stoi(value);
            } catch (const std::exception& e) {
                std::cerr << "Error parsing debug level: " << e.what() << std::endl;
                debugLevel = 0;
            }
        }
    } else {
        std::cerr << "Warning: Unknown argument --" << key << std::endl;
    }
}

bool ConfigFileLoader::loadConfig(const std::string& filepath, std::map<std::string, std::string>& config) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
        // Skip comments and empty lines
        size_t commentPos = line.find('#');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }
        
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);
        
        if (line.empty()) {
            continue;
        }
        
        size_t equalsPos = line.find('=');
        if (equalsPos == std::string::npos) {
            std::cerr << "Warning: Invalid config line " << lineNumber << ": " << line << std::endl;
            continue;
        }
        
        std::string key = line.substr(0, equalsPos);
        std::string value = line.substr(equalsPos + 1);
        
        // Trim whitespace
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);
        
        config[key] = value;
    }
    
    return true;
}

bool ConfigFileLoader::saveConfig(const std::string& filepath, const std::map<std::string, std::string>& config) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& pair : config) {
        file << pair.first << " = " << pair.second << std::endl;
    }
    
    return true;
}

bool ConfigFileLoader::validateConfig(const std::map<std::string, std::string>& config) {
    // Basic validation - can be extended with more specific validation rules
    for (const auto& pair : config) {
        if (pair.first.empty()) {
            std::cerr << "Warning: Empty key in configuration" << std::endl;
            return false;
        }
    }
    
    return true;
}

void common_options::setupOptions(CommandLineParser& parser) {
    // Setup common command-line options for NLM applications
    parser.addOption("debug", ArgumentType::Int, 
                     "Enable debug output (0-3 levels)", "0",
                     nullptr, false);
    
    parser.addOption("verbose", ArgumentType::Bool,
                     "Enable verbose logging", "false",
                     nullptr, false);
    
    parser.addOption("quiet", ArgumentType::Bool,
                     "Suppress output messages", "false",
                     nullptr, false);
    
    parser.addOption("force", ArgumentType::Bool,
                     "Force operation (override safety checks)", "false",
                     nullptr, true);
    
    parser.addOption("config", ArgumentType::File,
                     "Load configuration from file", "",
                     nullptr, false);
    
    parser.addOption("config-set", ArgumentType::String,
                     "Set configuration value", "",
                     nullptr, true);
    
    parser.addOption("viz-mode", ArgumentType::String,
                     "Set visualization mode (network, activity, spikes, weights, all)", "network",
                     nullptr, true);
    
    parser.addOption("viz-update-rate", ArgumentType::Double,
                     "Set visualization update rate (Hz)", "30.0",
                     nullptr, true);
    
    parser.addOption("viz-export", ArgumentType::String,
                     "Export visualization (svg, png, json, csv)", "",
                     nullptr, true);
    
    parser.addOption("perf-profile", ArgumentType::Bool,
                     "Enable performance profiling", "false",
                     nullptr, true);
    
    parser.addOption("metrics-file", ArgumentType::File,
                     "Save metrics to file", "",
                     nullptr, true);
    
    parser.addOption("info", ArgumentType::Bool,
                     "Display system information", "false",
                     nullptr, false);
    
    parser.addOption("version", ArgumentType::Bool,
                     "Display version information", "false",
                     nullptr, false);
}

} // namespace nlm
