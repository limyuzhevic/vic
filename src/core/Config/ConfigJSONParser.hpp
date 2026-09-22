#pragma once

#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include <nlohmann/json.hpp>

namespace nlm {

// Simple JSON parser for Config class
class ConfigJSONParser {
public:
    // Parse JSON string into config entries
    static std::unordered_map<std::string, std::string> parseJSON(const std::string& jsonStr);
    
    // Parse JSON file into config entries
    static std::unordered_map<std::string, std::string> parseJSONFile(const std::string& filepath);
    
    // Convert value to string representation for config
    template<typename T>
    static std::string valueToString(const T& value);
    
    // Convert string to specific type
    template<typename T>
    static T stringToValue(const std::string& str);
};

} // namespace nlm