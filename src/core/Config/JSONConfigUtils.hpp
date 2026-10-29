#pragma once

#include <string>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>
#include "core/Config/Config.hpp"

namespace nlm {

/**
 * @brief JSON configuration utilities
 * 
 * Provides JSON parsing and serialization for the NLM configuration system.
 * Supports both reading and writing of configuration files in JSON format.
 */
class JSONConfigUtils {
public:
    /**
     * @brief Load configuration from JSON file
     * 
     * @param config Config object to populate
     * @param filepath Path to JSON configuration file
     * @return true if successful, false on error
     */
    static bool loadJSONFromFile(std::shared_ptr<Config> config, const std::string& filepath);
    
    /**
     * @brief Save configuration to JSON file
     * 
     * @param config Config object to save
     * @param filepath Path to output JSON configuration file
     * @return true if successful, false on error
     */
    static bool saveJSONToFile(const std::shared_ptr<Config>& config, const std::string& filepath);
    
    /**
     * @brief Detect if file contains JSON format
     * 
     * @param filepath Path to file to check
     * @return true if file appears to be JSON format
     */
    static bool isJSONFile(const std::string& filepath);
    
    /**
     * @brief Convert configuration to JSON
     * 
     * @param config Config object to convert
     * @return nlohmann::json representation
     */
    static nlohmann::json toJSON(const std::shared_ptr<Config>& config);
    
    /**
     * @brief Load configuration from JSON object
     * 
     * @param config Config object to populate
     * @param json JSON object to load from
     */
    static void fromJSON(std::shared_ptr<Config> config, const nlohmann::json& json);
};

/**
 * @brief Configuration validation utilities
 * 
 * Provides validation of configuration values and parameter ranges.
 */
class ConfigValidation {
public:
    /**
     * @brief Validate configuration entry
     * 
     * @param key Configuration key
     * @param value Configuration value
     * @param source Source of configuration
     * @return true if valid, false if invalid
     */
    static bool validateEntry(const std::string& key, const ConfigValue& value, ConfigSource source);
    
    /**
     * @brief Validate all configuration entries
     * 
     * @param config Config object to validate
     * @return true if all entries valid, false if any invalid
     */
    static bool validateAll(const std::shared_ptr<Config>& config);
    
    /**
     * @brief Get validation error message
     * 
     * @param key Configuration key
     * @param value Configuration value
     * @param source Source of configuration
     * @return Error message or empty string if valid
     */
    static std::string getValidationError(const std::string& key, const ConfigValue& value, ConfigSource source);
    
    /**
     * @brief Get allowed configuration keys
     * 
     * @return Set of valid configuration keys
     */
    static std::vector<std::string> getValidKeys();
    
    /**
     * @brief Get configuration key description
     * 
     * @param key Configuration key
     * @return Description or empty string if unknown
     */
    static std::string getKeyDescription(const std::string& key);
};

} // namespace nlm