// Configuration manager for CLI
// Handles loading, saving, and manipulation of configuration files and parameters

#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <ctime>
#include "../core/Config/Config.hpp"

namespace nlm {

// Configuration manager for CLI operations
class ConfigManager {
public:
    ConfigManager(std::shared_ptr<Config> config);
    ~ConfigManager();
    
    // Load configuration from file
    bool loadFromFile(const std::string& filepath);
    
    // Save configuration to file
    bool saveToFile(const std::string& filepath) const;
    
    // Set configuration parameter
    bool setParameter(const std::string& key, const std::string& value);
    
    // Show current configuration
    void showConfig() const;
    
    // Get configuration value
    std::string getParameter(const std::string& key) const;
    
    // Check if parameter exists
    bool hasParameter(const std::string& key) const;
    
    // Get all parameter keys
    std::vector<std::string> getAllKeys() const;
    
    // Get underlying config
    std::shared_ptr<Config> getConfig() const { return config_; }
    
private:
    std::shared_ptr<Config> config_;
    std::string currentConfigFile_;
    
    // Format configuration value for display
    std::string formatValue(const ConfigValue& value) const;
    
    // Parse configuration file
    bool parseConfigFile(const std::string& filepath);
    
    // Parse configuration line
    bool parseConfigLine(const std::string& line);
    
    // Save configuration to JSON file
    bool saveAsJSON(const std::string& filepath) const;
    
    // Load configuration from JSON file
    bool loadFromJSON(const std::string& filepath);
};

} // namespace nlm