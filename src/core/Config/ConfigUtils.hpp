// ConfigUtils.hpp - Utility functions for configuration management
// Provides additional configuration functionality beyond basic get/set operations

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include "core/Config/Config.hpp"

namespace nlm {

// Configuration validation utilities
class ConfigUtils {
public:
    // Validate configuration value based on key
    static bool validate(const std::string& key, const ConfigValue& value);
    
    // Get configuration value type as string
    static std::string getValueType(const ConfigValue& value);
    
    // Check if configuration has required keys
    static bool hasRequiredKeys(const Config& config, 
                                const std::vector<std::string>& requiredKeys);
    
    // Get all configuration values as string
    static std::unordered_map<std::string, std::string> getAllStringValues(const Config& config);
    
    // Create configuration from a string
    static std::shared_ptr<Config> parseFromString(const std::string& configString);
    
    // Merge two configurations (runtime overrides)
    static void merge(Config& target, const Config& source);
    
    // Apply configuration profile
    static void applyProfile(Config& config, const std::string& profileName);
    
    // Get configuration profile names available
    static std::vector<std::string> getAvailableProfiles();
    
    // Create default configuration
    static std::shared_ptr<Config> createDefaultConfig();
    
    // Create minimal test configuration
    static std::shared_ptr<Config> createTestConfig();
};

// Global configuration helpers
namespace ConfigHelper {
    // Get global configuration instance (will be implemented in .cpp)
    std::shared_ptr<Config> getGlobalConfig();
    
    // Set global configuration
    void setGlobalConfig(std::shared_ptr<Config> config);
    
    // Load configuration from file
    bool loadConfig(const std::string& filepath);
    
    // Load configuration from command line
    bool loadConfigFromArgs(int argc, char** argv);
}

} // namespace nlm
