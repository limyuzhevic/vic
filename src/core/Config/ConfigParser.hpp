#ifndef NLM_CONFIG_PARSER_HPP
#define NLM_CONFIG_PARSER_HPP

#include <string>
#include <memory>
#include <unordered_map>
#include <variant>
#include <vector>
#include <optional>
#include <json.hpp> // nlohmann::json

// Forward declaration
namespace nlm {
    class Config;
}

// JSON configuration parser
class ConfigParser {
public:
    ConfigParser() = default;
    ~ConfigParser() = default;
    
    // Parse JSON configuration string
    static std::unordered_map<std::string, nlohmann::json> parseJSON(const std::string& jsonString);
    
    // Parse JSON configuration file
    static bool parseJSONFile(const std::string& filepath, std::unordered_map<std::string, nlohmann::json>& config);
    
    // Parse YAML configuration string (simplified implementation)
    static std::unordered_map<std::string, nlohmann::json> parseYAML(const std::string& yamlString);
    
    // Convert JSON value to ConfigValue
    static nlm::ConfigValue jsonToConfigValue(const nlohmann::json& jsonValue);
    
    // Convert ConfigValue to JSON value
    static nlohmann::json configValueToJSON(const nlm::ConfigValue& configValue);
    
    // Load configuration from file with automatic format detection
    static bool loadConfigFromFile(nlm::Config& config, const std::string& filepath);
    
    // Save configuration to file (always as JSON for now)
    static bool saveConfigToFile(const nlm::Config& config, const std::string& filepath);
    
    // Validate configuration
    static std::vector<std::string> validateConfig(const nlm::Config& config);
    
    // Merge configurations
    static void mergeConfig(nlm::Config& target, const nlm::Config& source);
    
private:
    // Helper functions
    static std::string toLower(const std::string& str);
    static bool isJSON(const std::string& content);
    static bool isYAML(const std::string& content);
};

#endif // NLM_CONFIG_PARSER_HPP
