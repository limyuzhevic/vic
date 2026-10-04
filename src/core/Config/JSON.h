#ifndef JSON_H
#define JSON_H

#include <string>
#include <unordered_map>
#include <vector>
#include <variant>
#include <optional>
#include <stdexcept>
#include <fstream>
#include <sstream>

namespace nlm {

// Simple JSON parser for configuration files
class JSONParser {
public:
    // Parse JSON string and return vector of key-value pairs
    static std::vector<std::pair<std::string, std::string>> parseJSON(const std::string& json);
    
    // Parse JSON from file
    static std::vector<std::pair<std::string, std::string>> parseJSONFile(const std::string& filepath);
    
    // Parse JSON from string with error handling
    static std::vector<std::pair<std::string, std::string>> parseJSONString(const std::string& json);
    
    // Parse YAML-like simple format (key: value)
    static std::vector<std::pair<std::string, std::string>> parseYAML(const std::string& yaml);
    
    // Parse YAML from file
    static std::vector<std::pair<std::string, std::string>> parseYAMLFile(const std::string& filepath);
    
    // Convert value string to appropriate type based on content
    static std::string inferType(const std::string& value);
    
    // Validate JSON syntax
    static bool validateJSON(const std::string& json);
    
    // Validate YAML syntax
    static bool validateYAML(const std::string& yaml);
};

// JSON serialization for configuration
class JSONSerializer {
public:
    // Convert configuration entries to JSON format
    static std::string toJSON(const std::vector<std::pair<std::string, std::string>>& entries);
    
    // Convert configuration entries to YAML format
    static std::string toYAML(const std::vector<std::pair<std::string, std::string>>& entries);
    
    // Write configuration to JSON file
    static bool toJSONFile(const std::string& filepath, const std::vector<std::pair<std::string, std::string>>& entries);
    
    // Write configuration to YAML file
    static bool toYAMLFile(const std::string& filepath, const std::vector<std::pair<std::string, std::string>>& entries);
};

} // namespace nlm

#endif // JSON_H
