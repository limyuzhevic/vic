#pragma once

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <string>
#include <memory>

namespace nlm {

class ConfigManager {
public:
    ConfigManager();
    ~ConfigManager() = default();
    
    // Disable copying
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;
    
    // Load configuration from file and command line
    bool loadConfig(const std::string& configFile, int argc, char** argv);
    
    // Get configuration
    std::shared_ptr<Config> getConfig() const { return config; }
    
    // Log configuration summary
    void logConfigSummary() const;
    
    // Get configuration file path
    std::string getConfigFile() const { return configFile; }
    
private:
    std::shared_ptr<Config> config;
    std::string configFile;
    
    // Set default configuration values for Phase 2
    void setDefaultConfig();
    
    // Parse command line arguments
    void parseCommandLineArgs(int argc, char** argv);
};

} // namespace nlm