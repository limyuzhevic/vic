#ifndef NLOPT_CONFIG_JSON_HELPER_HPP
#define NLOPT_CONFIG_JSON_HELPER_HPP

#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <unordered_map>
#include <variant>
#include <optional>

namespace nlm {

// JSON configuration helper to support nested structures
class ConfigJsonHelper {
public:
    // Parse JSON and convert to ConfigValue compatible structure
    static std::unordered_map<std::string, nlohmann::json> parseJson(const std::string& jsonStr);
    
    // Flatten nested JSON to key=value format
    static std::unordered_map<std::string, std::string> flattenJson(const nlohmann::json& json);
    
    // Convert string value to appropriate ConfigValue type
    static ConfigValue convertStringToConfigValue(const std::string& valueStr);
    
    // Convert ConfigValue to string for save
    static std::string convertConfigValueToString(const ConfigValue& value);
    
    // Detect file type by extension and content
    static bool isJsonFile(const std::string& filepath);
};

} // namespace nlm

#endif // NLOPT_CONFIG_JSON_HELPER_HPP