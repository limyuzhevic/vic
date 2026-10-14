#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <nlohmann/json.hpp>
#include "core/Config/Config.hpp"
#include "core/Config/Validator.hpp"

namespace nlm {

class JSONParser {
public:
    JSONParser();
    ~JSONParser();
    
    // Parse JSON file to Config
    bool parseToConfig(const std::string& filepath, std::shared_ptr<Config> config);
    
    // Parse JSON string to Config
    bool parseToConfigFromString(const std::string& jsonString, std::shared_ptr<Config> config);
    
    // Convert Config to JSON
    nlohmann::json configToJSON(const Config& config) const;
    
    // Validate JSON structure
    std::vector<std::string> validateJSON(const nlohmann::json& json) const;
    
    // Convert dot notation to JSON path
    std::vector<std::string> dotNotationToPath(const std::string& key) const;
    
    // Convert JSON path to dot notation
    std::string pathToDotNotation(const std::vector<std::string>& path) const;
    
    // Get supported JSON version
    std::string getSupportedVersion() const { return "2.0"; }
    
    // Check if filepath is JSON format
    bool isJSONFile(const std::string& filepath) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper functions
    bool parseValue(const nlohmann::json& jsonValue, ConfigValue& configValue, 
                   const std::string& key) const;
    bool parseNestedObject(const nlohmann::json& jsonObject, std::shared_ptr<Config> config,
                         const std::string& prefix = "") const;
    std::string jsonValueToString(const nlohmann::json& jsonValue) const;
};

} // namespace nlm
