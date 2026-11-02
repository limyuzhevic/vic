#pragma once

#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <iostream>
#include <fstream>

namespace nlm {

class CLIConfigurator {
public:
    // Parse command-line arguments and create configuration
    static std::shared_ptr<Config> createConfigFromArgs(int argc, char* argv[]);
    
    // Load configuration from file
    static std::shared_ptr<Config> loadConfigFromFile(const std::string& filename);
    
    // Save configuration to file
    static bool saveConfigToFile(const std::shared_ptr<const Config>& config, const std::string& filename);
    
    // Print configuration to stdout
    static void printConfig(const std::shared_ptr<const Config>& config);
    
    // Print help information
    static void printHelp(const std::string& programName);
    
    // Get default configuration
    static std::shared_ptr<Config> getDefaultConfig();
    
    // Validate configuration
    static bool validateConfig(const std::shared_ptr<const Config>& config, std::string& errorMessage);
    
private:
    // Parse key-value pairs from command line
    static std::map<std::string, std::string> parseKeyValueArgs(int argc, char* argv[]);
    
    // Parse file-based configuration
    static std::map<std::string, std::string> parseFileConfig(const std::string& filename);
    
    // Convert string value to appropriate type
    static bool convertValue(const std::string& value, std::string& result);
    static bool convertValue(const std::string& value, int64_t& result);
    static bool convertValue(const std::string& value, float& result);
    static bool convertValue(const std::string& value, double& result);
    static bool convertValue(const std::string& value, bool& result);
    
    // Get environment variable
    static std::string getEnvVar(const std::string& name, const std::string& defaultValue = "");
};

} // namespace nlm
