#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <nlohmann/json.hpp>
#include "ErrorSystem.hpp"
#include "Validation/NeuralInputValidator.hpp"

using json = nlohmann::json;

namespace nlm {

// JSON Schema validation system for configuration
class JSONSchemaValidator {
public:
    struct Schema {
        std::string id;
        std::string title;
        std::string description;
        std::string type = "object";
        std::vector<std::string> required;
        std::unordered_map<std::string, json> properties;
        std::unordered_map<std::string, json> patternProperties;
        std::vector<json> items;
        bool additionalProperties = true;
        std::string pattern;
        double minimum = std::numeric_limits<double>::lowest();
        double maximum = std::numeric_limits<double>::max();
        std::vector<json> enum;
        json propertyNames;
        std::string format;
        std::string defaultValue;
        
        // Validation functions
        std::function<bool(const json&)> customValidator;
    };
    
    struct SchemaValidationResult {
        bool valid = true;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        std::vector<std::string> path;
        
        SchemaValidationResult() {}
        SchemaValidationResult(const std::vector<std::string>& p) : path(p) {}
        
        SchemaValidationResult& addError(const std::string& error) {
            errors.push_back(error);
            valid = false;
            return *this;
        }
        
        SchemaValidationResult& addWarning(const std::string& warning) {
            warnings.push_back(warning);
            return *this;
        }
        
        std::string getFullError() const {
            std::ostringstream oss;
            oss << "Schema validation failed at ";
            for (size_t i = 0; i < path.size(); ++i) {
                if (i > 0) oss << ".";
                oss << path[i];
            }
            oss << ": ";
            if (!errors.empty()) {
                oss << errors[0];
            }
            return oss.str();
        }
    };
    
    class ConfigSchema {
    public:
        std::string id;
        std::string version;
        std::string title;
        std::string description;
        Schema schema;
        
        // Predefined schema versions
        static ConfigSchema getDefaultSchema() {
            ConfigSchema schema;
            schema.id = "nlm-default-config-v1.0";
            schema.version = "1.0";
            schema.title = "NLM Default Configuration Schema";
            schema.description = "Default schema for NLM neural simulation configuration";
            
            // Define schema
            schema.schema.type = "object";
            schema.schema.required = {"random_seed", "simulation_timestep", "neuron_count", "region_count"};
            schema.schema.additionalProperties = false;
            
            // Random seed properties
            schema.schema.properties["random_seed"] = {
                {"type", "integer"},
                {"minimum", 0},
                {"maximum", 2147483647},
                {"description", "Random seed for neural network initialization"},
                {"default", 42}
            };
            
            // Simulation timestep
            schema.schema.properties["simulation_timestep"] = {
                {"type", "number"},
                {"minimum", 0.0},
                {"maximum", 1.0},
                {"description", "Simulation timestep in seconds"},
                {"default", 0.001}
            };
            
            // Neuron count
            schema.schema.properties["neuron_count"] = {
                {"type", "integer"},
                {"minimum", 1},
                {"maximum", 1000000},
                {"description", "Total number of neurons in the network"},
                {"default", 1000}
            };
            
            // Region count
            schema.schema.properties["region_count"] = {
                {"type", "integer"},
                {"minimum", 1},
                {"maximum", 1000},
                {"description", "Number of neural regions"},
                {"default", 1}
            };
            
            // Connection probability
            schema.schema.properties["connection_probability"] = {
                {"type", "number"},
                {"minimum", 0.0},
                {"maximum", 1.0},
                {"description", "Probability of connection between neurons"},
                {"default", 0.1}
            };
            
            return schema;
        }
        
        static ConfigSchema getDevelopmentSchema() {
            ConfigSchema schema = getDefaultSchema();
            schema.id = "nlm-development-config-v1.0";
            schema.title = "NLM Development Configuration Schema";
            
            // Add development-specific properties
            schema.schema.properties["checkpoint_dir"] = {
                {"type", "string"},
                {"pattern", "^[a-zA-Z0-9_/-]+$"},
                {"description", "Directory for saving checkpoints"},
                {"default", "./checkpoints"}
            };
            
            schema.schema.properties["checkpoint_interval"] = {
                {"type", "integer"},
                {"minimum", 1},
                {"description", "Interval between checkpoints"},
                {"default", 10000}
            };
            
            schema.schema.properties["log_level"] = {
                {"type", "string"},
                {"enum", {"debug", "info", "warning", "error", "critical"}},
                {"description", "Logging verbosity level"},
                {"default", "info"}
            };
            
            return schema;
        }
        
        static ConfigSchema getProductionSchema() {
            ConfigSchema schema = getDefaultSchema();
            schema.id = "nlm-production-config-v1.0";
            schema.title = "NLM Production Configuration Schema";
            
            // Add production-specific constraints and security
            schema.schema.properties["random_seed"] = {
                {"type", "integer"},
                {"minimum", 0},
                {"maximum", 2147483647},
                {"description", "Random seed for neural network initialization"},
                {"default", 42}
            };
            
            // Add security-related properties
            schema.schema.properties["encryption_key"] = {
                {"type", "string"},
                {"minLength", 32},
                {"description", "Encryption key for sensitive configuration data (production only)"}
            };
            
            schema.schema.properties["audit_logging"] = {
                {"type", "boolean"},
                {"description", "Enable comprehensive audit logging"},
                {"default", true}
            };
            
            return schema;
        }
        
        ValidationResult validate(const Config& config, const json& schema, 
                                  const std::string& profile = "development") {
            ValidationResult result;
            
            try {
                json configJson = configToJson(config);
                validateJson(configJson, schema, {}, profile, result);
            } catch (const std::exception& e) {
                result.addError("JSON schema validation error: " + std::string(e.what()));
            }
            
            return result;
        }
        
    private:
        json configToJson(const Config& config) {
            json j;
            auto keys = config.getKeys();
            
            for (const auto& key : keys) {
                auto valueOpt = config.get<std::string>(key);
                if (valueOpt) {
                    j[key] = *valueOpt;
                    continue;
                }
                
                auto intOpt = config.get<int64_t>(key);
                if (intOpt) {
                    j[key] = *intOpt;
                    continue;
                }
                
                auto doubleOpt = config.get<double>(key);
                if (doubleOpt) {
                    j[key] = *doubleOpt;
                    continue;
                }
                
                auto boolOpt = config.get<bool>(key);
                if (boolOpt) {
                    j[key] = *boolOpt;
                    continue;
                }
                
                auto vectorIntOpt = config.get<std::vector<int>>(key);
                if (vectorIntOpt) {
                    j[key] = *vectorIntOpt;
                    continue;
                }
                
                auto vectorDoubleOpt = config.get<std::vector<double>>(key);
                if (vectorDoubleOpt) {
                    j[key] = *vectorDoubleOpt;
                    continue;
                }
                
                auto vectorStringOpt = config.get<std::vector<std::string>>(key);
                if (vectorStringOpt) {
                    j[key] = *vectorStringOpt;
                }
            }
            
            return j;
        }
        
        void validateJson(const json& instance, const json& schema, 
                         const std::vector<std::string>& path, const std::string& profile,
                         ValidationResult& result) {
            if (schema.is_object()) {
                validateObject(instance, schema, path, profile, result);
            } else if (schema.is_array()) {
                validateArray(instance, schema, path, profile, result);
            } else {
                validateValue(instance, schema, path, profile, result);
            }
        }
        
        void validateObject(const json& instance, const json& schema, 
                           const std::vector<std::string>& path, const std::string& profile,
                           ValidationResult& result) {
            // Check required properties
            if (schema.contains("required") && schema["required"].is_array()) {
                for (const auto& requiredProp : schema["required"]) {
                    if (!instance.contains(requiredProp.get<std::string>()) && 
                        !schema.contains("$defs")) {
                        result.addError("Missing required property: " + requiredProp.get<std::string>());
                    }
                }
            }
            
            // Validate properties
            if (schema.contains("properties") && schema["properties"].is_object()) {
                for (const auto& item : schema["properties"].items()) {
                    std::string propName = item.key();
                    std::vector<std::string> newPath = path;
                    newPath.push_back(propName);
                    
                    if (instance.contains(propName)) {
                        validateJson(instance[propName], item.value(), newPath, profile, result);
                    } else if (item.value().contains("default") && profile != "production") {
                        // Missing property but has default (development/testing)
                        result.addWarning("Property missing, using default: " + propName);
                    }
                }
            }
            
            // Validate pattern properties
            if (schema.contains("patternProperties") && schema["patternProperties"].is_object()) {
                for (const auto& item : schema["patternProperties"].items()) {
                    std::string pattern = item.key();
                    std::regex regex(pattern);
                    
                    for (const auto& instanceItem : instance.items()) {
                        if (std::regex_match(instanceItem.key(), regex)) {
                            std::vector<std::string> newPath = path;
                            newPath.push_back(instanceItem.key());
                            validateJson(instanceItem.value(), item.value(), newPath, profile, result);
                        }
                    }
                }
            }
            
            // Check additional properties
            if (!schema.value("additionalProperties", true)) {
                for (const auto& instanceItem : instance.items()) {
                    std::string propName = instanceItem.key();
                    bool isAllowed = false;
                    
                    if (schema.contains("properties") && schema["properties"].is_object() && 
                        schema["properties"].contains(propName)) {
                        isAllowed = true;
                    }
                    
                    if (!isAllowed && schema.contains("patternProperties") && 
                        schema["patternProperties"].is_object()) {
                        for (const auto& patternItem : schema["patternProperties"].items()) {
                            std::regex regex(patternItem.key());
                            if (std::regex_match(propName, regex)) {
                                isAllowed = true;
                                break;
                            }
                        }
                    }
                    
                    if (!isAllowed) {
                        result.addError("Additional property not allowed: " + propName);
                    }
                }
            }
        }
        
        void validateArray(const json& instance, const json& schema,
                          const std::vector<std::string>& path, const std::string& profile,
                          ValidationResult& result) {
            if (!instance.is_array()) {
                result.addError("Expected array at " + joinPath(path));
                return;
            }
            
            if (schema.contains("items") && schema["items"].is_array() && !schema["items"].empty()) {
                for (size_t i = 0; i < instance.size(); ++i) {
                    std::vector<std::string> newPath = path;
                    newPath.push_back(std::to_string(i));
                    validateJson(instance[i], schema["items"][0], newPath, profile, result);
                }
            } else if (schema.contains("items") && schema["items"].is_object()) {
                for (size_t i = 0; i < instance.size(); ++i) {
                    std::vector<std::string> newPath = path;
                    newPath.push_back(std::to_string(i));
                    validateJson(instance[i], schema["items"], newPath, profile, result);
                }
            }
        }
        
        void validateValue(const json& instance, const json& schema,
                          const std::vector<std::string>& path, const std::string& profile,
                          ValidationResult& result) {
            if (schema.contains("type")) {
                std::string type = schema["type"];
                
                if (type == "string") {
                    validateString(instance, schema, path, result);
                } else if (type == "number" || type == "integer") {
                    validateNumber(instance, schema, path, result);
                } else if (type == "boolean") {
                    validateBoolean(instance, schema, path, result);
                } else if (type == "array") {
                    validateArray(instance, schema, path, profile, result);
                } else if (type == "object") {
                    validateObject(instance, schema, path, profile, result);
                }
            }
            
            // Check enum values
            if (schema.contains("enum") && schema["enum"].is_array()) {
                bool found = false;
                for (const auto& enumValue : schema["enum"]) {
                    if (instance == enumValue) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    result.addError("Value must be one of: " + 
                                  json(schema["enum"]).dump());
                }
            }
            
            // Check format
            if (schema.contains("format")) {
                std::string format = schema["format"];
                if (format == "uri" || format == "url") {
                    validateUrl(instance, schema, path, result);
                } else if (format == "email") {
                    validateEmail(instance, schema, path, result);
                } else if (format == "date-time") {
                    validateDateTime(instance, schema, path, result);
                }
            }
        }
        
        void validateString(const json& instance, const json& schema,
                           const std::vector<std::string>& path, ValidationResult& result) {
            if (!instance.is_string()) {
                result.addError("Expected string at " + joinPath(path));
                return;
            }
            
            std::string value = instance.get<std::string>();
            
            if (schema.contains("minLength")) {
                int minLen = schema["minLength"];
                if (value.size() < static_cast<size_t>(minLen)) {
                    result.addError("String length must be >= " + std::to_string(minLen) +
                                  " at " + joinPath(path));
                }
            }
            
            if (schema.contains("maxLength")) {
                int maxLen = schema["maxLength"];
                if (value.size() > static_cast<size_t>(maxLen)) {
                    result.addError("String length must be <= " + std::to_string(maxLen) +
                                  " at " + joinPath(path));
                }
            }
            
            if (schema.contains("pattern")) {
                std::string pattern = schema["pattern"];
                std::regex regex(pattern);
                if (!std::regex_match(value, regex)) {
                    result.addError("String does not match pattern " + pattern +
                                  " at " + joinPath(path));
                }
            }
        }
        
        void validateNumber(const json& instance, const json& schema,
                           const std::vector<std::string>& path, ValidationResult& result) {
            if (!instance.is_number()) {
                result.addError("Expected number at " + joinPath(path));
                return;
            }
            
            double value = instance.get<double>();
            
            if (schema.contains("minimum")) {
                double min = schema["minimum"];
                if (value < min) {
                    result.addError("Value must be >= " + std::to_string(min) +
                                  " at " + joinPath(path));
                }
            }
            
            if (schema.contains("maximum")) {
                double max = schema["maximum"];
                if (value > max) {
                    result.addError("Value must be <= " + std::to_string(max) +
                                  " at " + joinPath(path));
                }
            }
            
            if (schema.contains("exclusiveMinimum")) {
                double min = schema["exclusiveMinimum"];
                if (value <= min) {
                    result.addError("Value must be > " + std::to_string(min) +
                                  " at " + joinPath(path));
                }
            }
            
            if (schema.contains("exclusiveMaximum")) {
                double max = schema["exclusiveMaximum"];
                if (value >= max) {
                    result.addError("Value must be < " + std::to_string(max) +
                                  " at " + joinPath(path));
                }
            }
        }
        
        void validateBoolean(const json& instance, const json& schema,
                            const std::vector<std::string>& path, ValidationResult& result) {
            if (!instance.is_boolean()) {
                result.addError("Expected boolean at " + joinPath(path));
            }
        }
        
        void validateUrl(const json& instance, const json& schema,
                        const std::vector<std::string>& path, ValidationResult& result) {
            if (!instance.is_string()) return;
            
            std::string url = instance.get<std::string>();
            // Simple URL validation - in production, use a proper URL parser
            if (url.find("://") == std::string::npos) {
                result.addError("Invalid URL format: " + url + " at " + joinPath(path));
            }
        }
        
        void validateEmail(const json& instance, const json& schema,
                          const std::vector<std::string>& path, ValidationResult& result) {
            if (!instance.is_string()) return;
            
            std::string email = instance.get<std::string>();
            if (email.find("@") == std::string::npos) {
                result.addError("Invalid email format: " + email + " at " + joinPath(path));
            }
        }
        
        void validateDateTime(const json& instance, const json& schema,
                             const std::vector<std::string>& path, ValidationResult& result) {
            if (!instance.is_string()) return;
            
            // Simple ISO 8601 validation - in production, use a proper date parser
            std::string dateTime = instance.get<std::string>();
            if (dateTime.find("T") == std::string::npos || dateTime.size() < 10) {
                result.addError("Invalid date-time format: " + dateTime + " at " + joinPath(path));
            }
        }
        
        std::string joinPath(const std::vector<std::string>& path) {
            std::ostringstream oss;
            for (size_t i = 0; i < path.size(); ++i) {
                if (i > 0) oss << ".";
                oss << path[i];
            }
            return oss.str();
        }
    };
    
    // High-level API
    static ValidationResult validateConfig(const Config& config, 
                                         const std::string& schemaId = "default") {
        ConfigSchema schema;
        if (schemaId == "default") {
            schema = ConfigSchema::getDefaultSchema();
        } else if (schemaId == "development") {
            schema = ConfigSchema::getDevelopmentSchema();
        } else if (schemaId == "production") {
            schema = ConfigSchema::getProductionSchema();
        }
        
        return schema.validate(config, schema.schema);
    }
    
    static ValidationResult validateConfigWithProfile(const Config& config,
                                                     const std::string& profile,
                                                     const std::string& schemaId = "default") {
        ConfigSchema schema;
        if (schemaId == "default") {
            schema = ConfigSchema::getDefaultSchema();
        } else if (schemaId == "development") {
            schema = ConfigSchema::getDevelopmentSchema();
        } else if (schemaId == "production") {
            schema = ConfigSchema::getProductionSchema();
        }
        
        return schema.validate(config, schema.schema, profile);
    }
}; // namespace nlm