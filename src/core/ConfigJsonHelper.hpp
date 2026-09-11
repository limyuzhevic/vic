// ConfigJsonHelper.hpp - Helper class for JSON-based configuration
#pragma once

#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include "core/Config/Config.hpp"

namespace nlm {

/**
 * ConfigJsonHelper - Helper class for processing JSON configuration files
 * 
 * Provides functionality to:
 * - Parse JSON files
 * - Flatten nested JSON objects
 * - Convert between JSON values and ConfigValue types
 * - Support for both JSON and key=value file formats
 */
class ConfigJsonHelper {
public:
    /**
     * Check if a file is in JSON format by extension and content
     */
    static bool isJsonFile(const std::string& filepath);
    
    /**
     * Check if a file is in key=value format
     */
    static bool isKeyValueFile(const std::string& filepath);
    
    /**
     * Convert a JSON value to a string representation
     * Handles all JSON types including arrays
     */
    static std::string jsonToString(const nlohmann::json& value);
    
    /**
     * Convert a string to ConfigValue type
     * Tries to parse as int, int64_t, double, bool, or string
     */
    static ConfigValue convertStringToConfigValue(const std::string& str);
    
    /**
     * Recursively flatten a nested JSON object to key=value pairs
     * Handles nested objects and arrays
     */
    static void flattenJson(const nlohmann::json& json, std::string prefix,
                           std::unordered_map<std::string, std::string>& result);
    
    /**
     * Get file extension from path
     */
    static std::string getFileExtension(const std::string& filepath);
    
    /**
     * Trim whitespace from a string
     */
    static std::string trim(const std::string& str);
    
    /**
     * Check if a string represents a boolean
     */
    static bool isBooleanString(const std::string& str);
    
    /**
     * Check if a string represents an integer
     */
    static bool isIntegerString(const std::string& str);
    
    /**
     * Check if a string represents a floating-point number
     */
    static bool isDoubleString(const std::string& str);
    
private:
    // JSON type strings
    static const std::string JSON_EXT;
    static const std::string KV_EXT;
};
