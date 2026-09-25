#pragma once

#include <string>
#include <memory>
#include <map>
#include <variant>
#include <vector>
#include <functional>
#include <nlohmann/json.hpp>
#include "Config.hpp"
#include "ConfigValidator.hpp"

namespace nlm {

class ConfigLoader {
public:
    // JSON parser callback type
    using JsonParser = std::function<nlohmann::json(const std::string&)>;
    
    ConfigLoader();
    ~ConfigLoader() = default;
    
    // Load configuration from JSON string
    bool loadFromJsonString(const std::string& jsonString, ConfigSource source = ConfigSource::File);
    
    // Load configuration from JSON file
    bool loadFromJsonFile(const std::string& filepath, ConfigSource source = ConfigSource::File);
    
    // Load configuration from YAML string
    bool loadFromYamlString(const std::string& yamlString, ConfigSource source = ConfigSource::File);
    
    // Load configuration from YAML file
    bool loadFromYamlFile(const std::string& filepath, ConfigSource source = ConfigSource::File);
    
    // Print help message (static method for use in main.cpp)
    static void printHelp();
    
    // Parse command-line arguments into configuration
    bool parseCommandLine(int argc, char** argv, Config& config, ConfigSource source = ConfigSource::CommandLine);
    
    // Add custom JSON parser for other formats
    void registerJsonParser(const std::string& extension, JsonParser parser);
    
    // Get loaded configuration
    const Config& getConfig() const { return config_; }
    
    // Clear current configuration
    void clear() { config_.clear(); }
    
    // Set validation reference
    void setValidator(const std::shared_ptr<ConfigValidator>& validator) { validator_ = validator; }
    
    // Validate loaded configuration
    std::vector<ConfigValidator::ValidationResult> validate() const;
    
    // Get all loaded configuration keys
    std::vector<std::string> getKeys() const;
    
private:
    Config config_;
    std::shared_ptr<ConfigValidator> validator_;
    std::map<std::string, JsonParser> jsonParsers_;
    
    // Helper methods
    bool parseKeyValuePair(const std::string& line, ConfigSource source);
    bool parseJsonObject(const nlohmann::json& json, ConfigSource source);
    std::string trim(const std::string& str) const;
    bool parseSimpleType(const std::string& value, ConfigValue& out) const;
    std::string toLower(const std::string& str) const;
};

} // namespace nlm