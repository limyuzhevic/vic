#pragma once

#include "../core/Config/Config.hpp"
#include <memory>
#include <string>
#include <map>
#include <vector>
#include <variant>
#include <functional>
#include <nlohmann/json.hpp>

namespace nlm {

// Simple JSON/YAML configuration manager with validation
class ConfigManager {
public:
    // Configuration validation result
    struct ValidationResult {
        bool valid;
        std::string errorMessage;
        std::vector<std::string> warnings;
        
        ValidationResult() : valid(true) {}
        ValidationResult(bool v, const std::string& error) : valid(v), errorMessage(error) {}
    };
    
    // Configuration schema definition
    struct ConfigSchema {
        std::string name;
        std::map<std::string, std::function<ValidationResult(const ConfigValue&)>> validators;
        std::string description;
        bool required;
        
        ConfigSchema(const std::string& n, const std::string& desc = "") 
            : name(n), description(desc), required(false) {}
        
        void addValidator(const std::string& key, std::function<ValidationResult(const ConfigValue&)> validator) {
            validators[key] = std::move(validator);
        }
        
        void setRequired(bool req) { required = req; }
    };
    
    ConfigManager() = default;
    ~ConfigManager() = default;
    
    // Load configuration from file (supports JSON, YAML, and simple key=value)
    bool loadConfig(const std::string& filepath, std::shared_ptr<Config> config);
    
    // Load configuration from JSON string
    bool loadFromJSON(const std::string& jsonString, std::shared_ptr<Config> config);
    
    // Load configuration from YAML string (placeholder for future implementation)
    bool loadFromYAML(const std::string& yamlString, std::shared_ptr<Config> config);
    
    // Load configuration from command line arguments
    bool loadFromCommandLine(int argc, char** argv, std::shared_ptr<Config> config);
    
    // Validate configuration against schema
    ValidationResult validateConfig(const std::shared_ptr<Config>& config, const std::vector<ConfigSchema>& schema);
    
    // Get default configuration
    static std::shared_ptr<Config> getDefaultConfig();
    
    // Create configuration file template
    static bool createConfigTemplate(const std::string& filepath, const std::vector<ConfigSchema>& schema);
    
    // Configuration value converters
    static std::string toString(const ConfigValue& value);
    static double toDouble(const ConfigValue& value);
    static int64_t toInt64(const ConfigValue& value);
    static bool toBool(const ConfigValue& value);
    
private:
    // Helper functions
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool parseKeyValue(const std::string& line, std::string& key, std::string& value);
};

} // namespace nlm