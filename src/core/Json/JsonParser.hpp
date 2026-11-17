#pragma once

#include <string>
#include <vector>
#include <map>
#include <variant>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <nlohmann/json.hpp>

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
    static ConfigValue parseJsonValue(const nlohmann::json& jsonValue);
    static nlohmann::json configValueToJsonValue(const ConfigValue& value);
    static std::string escapeString(const std::string& str);
    static std::string unescapeString(const std::string& str);
};

} // namespace nlm
