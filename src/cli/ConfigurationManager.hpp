// src/cli/ConfigurationManager.hpp
#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <unordered_set>

namespace nlm {

/**
 * Configuration management system for NLM
 * 
 * Provides comprehensive configuration file support including JSON/YAML loading,
 * validation, template management, and comparison features.
 */
class ConfigurationManager {
public:
    ConfigurationManager();
    ~ConfigurationManager();
    
    /**
     * Load configuration from file
     * 
     * @param filePath Path to configuration file (JSON or YAML)
     * @param config Shared pointer to configuration object to populate
     * @return true if configuration loaded successfully
     */
    bool loadConfiguration(const std::string& filePath, std::shared_ptr<Config> config);
    
    /**
     * Save configuration to file
     * 
     * @param config Shared pointer to configuration object
     * @param filePath Output file path
     * @param format File format ("json" or "yaml")
     * @return true if configuration saved successfully
     */
    bool saveConfiguration(std::shared_ptr<Config> config, const std::string& filePath, 
                          const std::string& format = "json");
    
    /**
     * Validate configuration file
     * 
     * @param filePath Path to configuration file
     * @return true if configuration is valid
     */
    bool validateConfiguration(const std::string& filePath);
    
    /**
     * Get configuration file extensions supported
     * 
     * @return Vector of supported file extensions
     */
    std::vector<std::string> getSupportedExtensions() const;
    
    /**
     * Get configuration templates
     * 
     * @return Map of template names to template contents
     */
    std::map<std::string, std::string> getConfigurationTemplates() const;
    
    /**
     * Compare two configuration files
     * 
     * @param filePath1 First configuration file
     * @param filePath2 Second configuration file
     * @return Vector of differences found
     */
    std::vector<std::string> compareConfigurations(const std::string& filePath1, 
                                                  const std::string& filePath2);
    
    /**
     * Get configuration summary
     * 
     * @param config Shared pointer to configuration object
     * @return Human-readable configuration summary
     */
    std::string getConfigurationSummary(std::shared_ptr<Config> config) const;
    
    /**
     * Check if file is configuration file
     * 
     * @param filePath Path to file to check
     * @return true if file appears to be a configuration file
     */
    bool isConfigurationFile(const std::string& filePath) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm