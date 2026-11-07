// Common CLI utility implementation

#include "cli/CommonCLI.hpp"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstring>

namespace nlm {

// Helper function to trim whitespace from string
static std::string trim(const std::string& str) {
    size_t start = 0;
    size_t end = str.length();
    
    // Trim leading whitespace
    while (start < end && std::isspace(str[start])) {
        start++;
    }
    
    // Trim trailing whitespace
    while (end > start && std::isspace(str[end - 1])) {
        end--;
    }
    
    return str.substr(start, end - start);
}

// Helper function to split string by delimiter
static std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(trim(token));
    }
    
    return tokens;
}

// Parse command line arguments
bool CommonCLI::parseArguments(int argc, char* argv[], std::shared_ptr<Config> config) {
    if (!config) return false;
    
    std::unordered_map<std::string, std::string> args;
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        // Check for --key=value format
        size_t equalsPos = arg.find('=');
        if (equalsPos != std::string::npos && arg[0] == '-' && arg[1] == '-') {
            std::string key = trim(arg.substr(2, equalsPos - 2));
            std::string value = trim(arg.substr(equalsPos + 1));
            args[key] = value;
        }
        // Check for --key value format
        else if (arg[0] == '-' && arg[1] == '-') {
            std::string key = trim(arg.substr(2));
            
            // Check if next argument is a value (not starting with -)
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                args[key] = trim(argv[++i]);
            } else {
                args[key] = "true";  // Boolean flag
            }
        }
    }
    
    // Apply parsed arguments to configuration
    applyArgumentsToConfig(args, config);
    return true;
}

// Load configuration from file
bool CommonCLI::loadConfigFile(const std::string& filename, std::shared_ptr<Config> config) {
    if (!config) return false;
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open configuration file: " << filename << std::endl;
        return false;
    }
    
    // Parse configuration file format
    std::string line;
    while (std::getline(file, line)) {
        // Skip comments and empty lines
        if (line.empty() || line[0] == '#') continue;
        
        // Parse key=value pairs
        size_t equalsPos = line.find('=');
        if (equalsPos != std::string::npos) {
            std::string key = trim(line.substr(0, equalsPos));
            std::string value = trim(line.substr(equalsPos + 1));
            
            // Convert value based on key type (try to infer from key)
            if (key.find(".") != std::string::npos) {
                // Has dots (e.g., plasticity.stdp.learning_rate) - likely float or int
                config->set(key, std::stof(value));
            } else if (key == "enabled" || key == "enable" || key == "verbose" || key == "quiet") {
                // Boolean flags
                bool boolValue = (value == "true" || value == "1" || value == "yes" || value == "on");
                config->set(key, boolValue);
            } else {
                // String or integer (try int first, then string)
                try {
                    config->set(key, std::stoi(value));
                } catch (...) {
                    config->set(key, value);
                }
            }
        }
    }
    
    file.close();
    return true;
}

// Print usage information
void CommonCLI::printUsage(const std::string& programName) {
    std::cout << "Usage: " << programName << " [options]" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --config <file>              Load configuration from file" << std::endl;
    std::cout << "  --help                       Show this help message" << std::endl;
    std::cout << "  --version                    Show version information" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Brain Configuration Options:" << std::endl;
    for (const auto& param : brainConfigParameters) {
        std::cout << "  --" << std::setw(20) << param.key << " <value>" << std::endl;
    }
    std::cout << "" << std::endl;
    std::cout << "Experiment Configuration Options:" << std::endl;
    for (const auto& param : experimentConfigParameters) {
        std::cout << "  --" << std::setw(20) << param.key << " <value>" << std::endl;
    }
    std::cout << "" << std::endl;
    std::cout << "Debug Options:" << std::endl;
    for (const auto& param : debugConfigParameters) {
        std::cout << "  --" << std::setw(20) << param.key << " <value>" << std::endl;
    }
    std::cout << "" << std::endl;
    std::cout << "Examples:"" << std::endl;
    std::cout << "  " << programName << " --config my_config.cfg" << std::endl;
    std::cout << "  " << programName << " --neuron_count 2000 --enable_curiosity" << std::endl;
    std::cout << "  " << programName << " --steps_per_episode 500" << std::endl;
}

// Print version information
void CommonCLI::printVersion() {
    std::cout << "NLM (熙然) - Neural Learning Machine" << std::endl;
    std::cout << "Version 0.1.0" << std::endl;
    std::cout << "An experimental artificial developmental brain." << std::endl;
    std::cout << "" << std::endl;
    std::cout << "For more information, visit: https://github.com/nlm-project/nlm" << std::endl;
}

// Print configuration parameters for help
void CommonCLI::printConfigHelp() {
    std::cout << "Configuration Parameters:" << std::endl;
    std::cout << "" << std::endl;
    
    std::cout << "Brain Configuration:" << std::endl;
    for (const auto& param : brainConfigParameters) {
        std::cout << "  " << param.key << ": <value>" << std::endl;
        std::cout << "    " << param.help << "" << std::endl;
        if (!param.validValues.empty()) {
            std::cout << "    Valid values: " << param.validValues[0];
            for (size_t i = 1; i < param.validValues.size(); ++i) {
                std::cout << ", " << param.validValues[i];
            }
            std::cout << "" << std::endl;
        }
    }
    
    std::cout << "" << std::endl;
    std::cout << "Experiment Configuration:" << std::endl;
    for (const auto& param : experimentConfigParameters) {
        std::cout << "  " << param.key << ": <value>" << std::endl;
        std::cout << "    " << param.help << "" << std::endl;
        if (!param.validValues.empty()) {
            std::cout << "    Valid values: " << param.validValues[0];
            for (size_t i = 1; i < param.validValues.size(); ++i) {
                std::cout << ", " << param.validValues[i];
            }
            std::cout << "" << std::endl;
        }
    }
    
    std::cout << "" << std::endl;
    std::cout << "Debug Configuration:" << std::endl;
    for (const auto& param : debugConfigParameters) {
        std::cout << "  " << param.key << ": <value>" << std::endl;
        std::cout << "    " << param.help << "" << std::endl;
        if (!param.validValues.empty()) {
            std::cout << "    Valid values: " << param.validValues[0];
            for (size_t i = 1; i < param.validValues.size(); ++i) {
                std::cout << ", " << param.validValues[i];
            }
            std::cout << "" << std::endl;
        }
    }
}

// Get configuration parameter value as string
std::string CommonCLI::getStringArg(const std::unordered_map<std::string, std::string>& args, const std::string& key, const std::string& defaultValue) {
    auto it = args.find(key);
    if (it != args.end()) {
        return it->second;
    }
    return defaultValue;
}

// Get configuration parameter value as integer
int CommonCLI::getIntArg(const std::unordered_map<std::string, std::string>& args, const std::string& key, int defaultValue) {
    auto it = args.find(key);
    if (it != args.end()) {
        try {
            return std::stoi(it->second);
        } catch (...) {
            return defaultValue;
        }
    }
    return defaultValue;
}

// Get configuration parameter value as float
float CommonCLI::getFloatArg(const std::unordered_map<std::string, std::string>& args, const std::string& key, float defaultValue) {
    auto it = args.find(key);
    if (it != args.end()) {
        try {
            return std::stof(it->second);
        } catch (...) {
            return defaultValue;
        }
    }
    return defaultValue;
}

// Get configuration parameter value as boolean
bool CommonCLI::getBoolArg(const std::unordered_map<std::string, std::string>& args, const std::string& key, bool defaultValue) {
    auto it = args.find(key);
    if (it != args.end()) {
        std::string value = it->second;
        // Convert string to boolean
        if (value == "true" || value == "1" || value == "yes" || value == "on") {
            return true;
        } else if (value == "false" || value == "0" || value == "no" || value == "off") {
            return false;
        }
    }
    return defaultValue;
}

// Check if argument exists
bool CommonCLI::hasArg(const std::unordered_map<std::string, std::string>& args, const std::string& key) {
    return args.find(key) != args.end();
}

// Apply parsed arguments to configuration
void CommonCLI::applyArgumentsToConfig(const std::unordered_map<std::string, std::string>& args, std::shared_ptr<Config> config) {
    if (!config) return;
    
    // Apply brain configuration parameters
    for (const auto& param : brainConfigParameters) {
        if (hasArg(args, param.key)) {
            std::string value = getStringArg(args, param.key);
            
            if (param.type == "int") {
                config->set(param.key, getIntArg(args, param.key, std::stoi(param.defaultValue)));
            } else if (param.type == "float") {
                config->set(param.key, getFloatArg(args, param.key, std::stof(param.defaultValue)));
            } else if (param.type == "bool") {
                config->set(param.key, getBoolArg(args, param.key, param.defaultValue == "true" || param.defaultValue == "1"));
            } else {
                config->set(param.key, value);
            }
        }
    }
    
    // Apply experiment configuration parameters
    for (const auto& param : experimentConfigParameters) {
        if (hasArg(args, param.key)) {
            std::string value = getStringArg(args, param.key);
            
            if (param.type == "int") {
                config->set(param.key, getIntArg(args, param.key, std::stoi(param.defaultValue)));
            } else if (param.type == "float") {
                config->set(param.key, getFloatArg(args, param.key, std::stof(param.defaultValue)));
            } else if (param.type == "bool") {
                config->set(param.key, getBoolArg(args, param.key, param.defaultValue == "true" || param.defaultValue == "1"));
            } else {
                config->set(param.key, value);
            }
        }
    }
    
    // Apply debug configuration parameters
    for (const auto& param : debugConfigParameters) {
        if (hasArg(args, param.key)) {
            std::string value = getStringArg(args, param.key);
            
            if (param.type == "int") {
                config->set(param.key, getIntArg(args, param.key, std::stoi(param.defaultValue)));
            } else if (param.type == "float") {
                config->set(param.key, getFloatArg(args, param.key, std::stof(param.defaultValue)));
            } else if (param.type == "bool") {
                config->set(param.key, getBoolArg(args, param.key, param.defaultValue == "true" || param.defaultValue == "1"));
            } else if (param.type == "string") {
                config->set(param.key, value);
            }
        }
    }
}

// Merge configuration files
void CommonCLI::mergeConfigFiles(const std::vector<std::string>& configFiles, std::shared_ptr<Config> config) {
    if (!config) return;
    
    for (const auto& filename : configFiles) {
        if (filename.empty()) continue;
        loadConfigFile(filename, config);
    }
}

} // namespace nlm
