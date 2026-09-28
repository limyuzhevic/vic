#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <stdexcept>
#include "core/Config/Config.hpp"

namespace nlm {

// JSON Configuration Exception
class JSONConfigException : public ConfigException {
public:
    enum class JSONErrorCode {
        InvalidJSON,
        SchemaMismatch,
        ConversionError,
        MissingRequiredField,
        TypeMismatch,
        NestedObjectError
    };
    
    JSONConfigException(JSONErrorCode code, const std::string& message)
        : ConfigException(static_cast<ConfigException::ErrorCode>(code), message) {}
    
    JSONErrorCode getJSONErrorCode() const {
        return static_cast<JSONErrorCode>(ConfigException::getErrorCode());
    }
};

// JSON Configuration Parser
class JSONConfig {
public:
    // Parse JSON string to ConfigValue map
    static std::unordered_map<std::string, ConfigValue> parseJSON(const std::string& jsonString);
    
    // Parse JSON file to ConfigValue map
    static std::unordered_map<std::string, ConfigValue> parseJSONFile(const std::string& filepath);
    
    // Convert ConfigValue map to JSON string
    static std::string toJSON(const std::unordered_map<std::string, ConfigValue>& configMap);
    
    // Convert ConfigValue map to formatted JSON string
    static std::string toFormattedJSON(const std::unordered_map<std::string, ConfigValue>& configMap, int indent = 2);
    
    // Convert single ConfigValue to JSON
    static nlohmann::json configValueToJSON(const ConfigValue& value);
    
    // Convert JSON to ConfigValue
    static ConfigValue jsonToConfigValue(const nlohmann::json& value);
    
    // Validate JSON against schema
    static bool validateJSON(const nlohmann::json& jsonData, const ConfigSchema& schema);
    
    // Validate JSON against schema and return detailed errors
    static std::vector<std::string> validateJSON(const nlohmann::json& jsonData, const ConfigSchema& schema, bool throwOnError = false);
    
    // Flatten nested JSON objects into flat key=value pairs
    static std::unordered_map<std::string, ConfigValue> flattenJSON(const nlohmann::json& jsonData, const std::string& prefix = "");
    
    // Handle special JSON types
    static std::vector<ConfigValue> jsonArrayToConfigVector(const nlohmann::json& array);
    static nlohmann::json configVectorToJSONArray(const std::vector<ConfigValue>& vector);
    
    // String conversion utilities
    static std::string configValueToString(const ConfigValue& value);
    static std::string escapeJSONString(const std::string& str);
    
    // Type checking utilities
    static bool isNumeric(const nlohmann::json& value);
    static bool isBoolean(const nlohmann::json& value);
    static bool isString(const nlohmann::json& value);
    static bool isArray(const nlohmann::json& value);
    static bool isObject(const nlohmann::json& value);
    
    // Schema validation helpers
    static std::string jsonTypeToConfigType(const nlohmann::json& value);
    static bool convertJSONValueToType(const nlohmann::json& jsonValue, const std::string& targetType, ConfigValue& outValue);
    
    // Error handling
    static std::string getJSONErrorMessage(const std::exception& e);
};

} // namespace nlm