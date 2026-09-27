#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <variant>
#include <optional>
#include <memory>
#include <stdexcept>
#include <map>

// Include JSON and YAML parsers
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

namespace nlm {

// Forward declarations
class ConfigParser;

// JSON/YAML Parser class
class ConfigParser {
public:
    ConfigParser() = default;
    ~ConfigParser() = default;
    
    // Parse JSON from string or file
    static std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>>>
    parseJSON(const std::string& jsonStr);
    
    static bool parseJSONFromFile(const std::string& filepath, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>>>& config);
    
    // Parse YAML from string or file
    static std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>>>
    parseYAML(const std::string& yamlStr);
    
    static bool parseYAMLFromFile(const std::string& filepath, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>>>& config);
    
    // Convert nested map to flat key-value pairs (for backward compatibility)
    static void flattenConfig(const std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>>>& nestedConfig, 
                              std::vector<std::pair<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>>>& flatConfig,
                              const std::string& prefix = "");
    
    // Validate configuration values
    static bool validateConfig(const std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>>>& config);
    
    // Convert ConfigEntry to JSON
    static nlohmann::json configEntryToJSON(const ConfigEntry& entry);
    
    // Convert JSON to ConfigEntry
    static ConfigEntry JSONToConfigEntry(const nlohmann::json& json, ConfigSource source);
    
    // Convert ConfigEntry to YAML
    static YAML::Node configEntryToYAML(const ConfigEntry& entry);
    
    // Convert YAML to ConfigEntry
    static ConfigEntry YAMLToConfigEntry(const YAML::Node& node, ConfigSource source);
    
    // Check if a file is JSON or YAML based on extension and content
    static bool isJSONFile(const std::string& filepath);
    static bool isYAMLFile(const std::string& filepath);
    
    // Error classes
    class ParseError : public std::runtime_error {
    public:
        ParseError(const std::string& msg) : std::runtime_error(msg) {}
    };
    
    class ValidationError : public std::runtime_error {
    public:
        ValidationError(const std::string& msg) : std::runtime_error(msg) {}
    };
};

} // namespace nlm