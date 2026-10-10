#pragma once

#include <string>
#include <map>
#include <vector>
#include <variant>
#include <optional>
#include <stdexcept>
#include <regex>
#include <iostream>

namespace nlm {
namespace config {

// Configuration schema validation
class ConfigSchema {
public:
    struct FieldInfo {
        std::string name;
        std::string type; // "int", "float", "bool", "string", "vec_int", "vec_float", "vec_string"
        std::variant<int, double, bool, std::string> defaultValue;
        bool required = false;
        std::string description;
        std::regex pattern; // For string validation
        std::vector<std::variant<int, double, bool, std::string>> enumValues;
        
        FieldInfo(const std::string& n, const std::string& t, bool req = false, 
                 const std::string& desc = "") : name(n), type(t), required(req), description(desc) {}
    };
    
    std::map<std::string, FieldInfo> fields;
    
    void addField(const std::string& name, const std::string& type, 
                  bool required = false, const std::string& description = "",
                  const std::vector<std::variant<int, double, bool, std::string>>& enumVals = {});
    
    void addStringField(const std::string& name, bool required = false, 
                       const std::string& description = "",
                       const std::string& pattern = "");
    
    void addIntField(const std::string& name, bool required = false,
                    const std::string& description = "",
                    int defaultValue = 0,
                    const std::vector<int>& enumVals = {});
    
    void addFloatField(const std::string& name, bool required = false,
                      const std::string& description = "",
                      double defaultValue = 0.0,
                      const std::vector<double>& enumVals = {});
    
    void addBoolField(const std::string& name, bool required = false,
                     const std::string& description = "",
                     bool defaultValue = false);
    
    bool validate(const std::map<std::string, std::variant<int, double, bool, std::string>>& config) const;
    
    std::string getFieldTypeError(const std::string& field, const std::variant<int, double, bool, std::string>& value) const;
    
    std::vector<std::string> getFieldNames() const;
    
    FieldInfo getFieldInfo(const std::string& fieldName) const;
};

// Environment variable support
class EnvironmentConfig {
public:
    static const std::string NLM_CONFIG_PREFIX;
    
    static std::map<std::string, std::variant<int, double, bool, std::string>> 
        loadFromEnvironment(const std::map<std::string, FieldInfo>& schema);
    
    static std::string boolToString(bool value);
    static std::string intToString(int value);
    static std::string floatToString(double value);
    
    static std::variant<int, double, bool, std::string> parseEnvValue(
        const std::string& str, const FieldInfo& fieldInfo);
};

// Dynamic configuration updater
class DynamicConfig {
private:
    std::shared_ptr<Config> baseConfig;
    std::map<std::string, std::variant<int, double, bool, std::string>> overrides;
    std::map<std::string, std::function<bool(const std::variant<int, double, bool, std::string>&)>> validators;
    
public:
    DynamicConfig(std::shared_ptr<Config> config) : baseConfig(config) {}
    
    void addValidator(const std::string& field, 
                      std::function<bool(const std::variant<int, double, bool, std::string>&)> validator);
    
    bool setField(const std::string& field, const std::variant<int, double, bool, std::string>& value);
    
    std::variant<int, double, bool, std::string> getField(const std::string& field) const;
    
    bool isValid() const;
    
    std::string getValidationError() const;
    
    std::shared_ptr<Config> applyToConfig() const;
    
    void clear();
};

// Configuration export/import utilities
class ConfigExporter {
public:
    static std::string toJSON(const std::map<std::string, std::variant<int, double, bool, std::string>>& config);
    static std::string toTOML(const std::map<std::string, std::variant<int, double, bool, std::string>>& config);
    static std::string toYAML(const std::map<std::string, std::variant<int, double, bool, std::string>>& config);
    
    static bool fromJSON(std::map<std::string, std::variant<int, double, bool, std::string>>& config,
                         const std::string& json);
    static bool fromTOML(std::map<std::string, std::variant<int, double, bool, std::string>>& config,
                         const std::string& toml);
    static bool fromYAML(std::map<std::string, std::variant<int, double, bool, std::string>>& config,
                         const std::string& yaml);
};

// Configuration migration utilities
class ConfigMigrator {
public:
    struct Migration {
        int version;
        std::function<std::map<std::string, std::variant<int, double, bool, std::string>>(
            const std::map<std::string, std::variant<int, double, bool, std::string>>&)> transform;
        std::string description;
    };
    
    std::vector<Migration> migrations;
    
    void addMigration(int fromVersion, int toVersion,
                     const std::function<std::map<std::string, std::variant<int, double, bool, std::string>>(
                         const std::map<std::string, std::variant<int, double, bool, std::string>>&)>& transform,
                     const std::string& description);
    
    std::map<std::string, std::variant<int, double, bool, std::string>> 
        migrate(const std::map<std::string, std::variant<int, double, bool, std::string>>& config,
                int targetVersion) const;
    
    int getCurrentVersion() const;
};

// Predefined configuration schemas for different NLM use cases
class PredefinedSchemas {
public:
    static ConfigSchema getResearchSchema();
    static ConfigSchema getProductionSchema();
    static ConfigSchema getDevelopmentSchema();
    static ConfigSchema getPerformanceSchema();
    static ConfigSchema getVisualizationSchema();
    
    static std::map<std::string, std::variant<int, double, bool, std::string>> 
        applySchema(const ConfigSchema& schema,
                   const std::map<std::string, std::variant<int, double, bool, std::string>>& config);
};

} // namespace config
} // namespace nlm
