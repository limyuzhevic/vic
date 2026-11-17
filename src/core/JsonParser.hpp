#pragma once

#include <string>
#include <vector>
#include <variant>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace nlm {

class JsonParser {
public:
    // Parse JSON string to ConfigValue
    static ConfigValue parseJsonToConfigValue(const std::string& jsonStr);
    
    // Parse JSON file to ConfigValue
    static ConfigValue parseJsonFileToConfigValue(const std::string& filepath);
    
    // Convert ConfigValue to JSON string
    static std::string configValueToJson(const ConfigValue& value);
    
    // Validate JSON syntax
    static bool validateJson(const std::string& jsonStr);
    
    // Get JSON schema description (for documentation)
    static std::string getJsonSchema();

private:
    // Helper functions for JSON parsing
    static ConfigValue parseJsonValue(const std::string& jsonStr);
    static std::string configValueToJsonValue(const ConfigValue& value);
    static std::string escapeString(const std::string& str);
    static std::string unescapeString(const std::string& str);
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string extractString(const std::string& jsonStr, size_t& pos);
    static std::string extractNumber(const std::string& jsonStr, size_t& pos);
    static std::string extractValue(const std::string& jsonStr, size_t& pos);
};

} // namespace nlm
