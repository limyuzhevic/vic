// Simple JSON parser implementation for configuration files
// Supports: strings (quoted), numbers (int/double), booleans, arrays
// Also supports simple key=value format for backward compatibility

#include <nlohmann/json.hpp>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <stdexcept>

// Include Config header for ConfigEntry and ConfigSource
#include "Config.hpp"

using json = nlohmann::json;

namespace nlm {

class JsonConfigParser {
public:
    // Parse JSON file
    static bool parseJsonFile(const std::string& filepath, std::vector<ConfigEntry>& entries);
    
    // Parse JSON string
    static bool parseJsonString(const std::string& jsonStr, std::vector<ConfigEntry>& entries);
    
    // Parse JSON object (from JSON)
    static bool parseJsonObject(const json& obj, std::vector<ConfigEntry>& entries);
    
    // Convert ConfigEntry to JSON value
    static json configEntryToJson(const ConfigEntry& entry);
    
    // Convert JSON value to ConfigEntry
    static bool jsonToConfigEntry(const json& value, ConfigEntry& entry);
    
    // Convert string to ConfigValue
    static bool stringToConfigValue(const std::string& str, ConfigValue& value);
    
    // Convert ConfigValue to string
    static std::string configValueToString(const ConfigValue& value);
};

// Legacy parser for backward compatibility
class LegacyConfigParser {
public:
    static bool parseLegacyFile(const std::string& filepath, std::vector<ConfigEntry>& entries);
    static std::string legacyConfigToString(const ConfigEntry& entry);
};