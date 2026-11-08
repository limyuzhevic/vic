// Configuration Schema and Validation System for NLM
// Provides configuration schema validation and integrity checking

#pragma once

#include "core/Config/Config.hpp"
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <sstream>
#include <algorithm>

namespace nlm {

/**
 * Configuration schema validation errors
 */
class ConfigValidationError : public std::runtime_error {
public:
    enum class ErrorType {
        MISSING_REQUIRED_FIELD,
        INVALID_TYPE,
        VALUE_OUT_OF_RANGE,
        DUPLICATE_FIELD,
        SCHEMA_VALIDATION_FAILED,
        CONFIG_FILE_PARSE_ERROR
    };
    
    ConfigValidationError(ErrorType type, const std::string& field, 
                         const std::string& message)
        : std::runtime_error(formatMessage(type, field, message)),
          errorType(type), fieldName(field), errorMessage(message) {}
    
    ErrorType getErrorType() const { return errorType; }
    const std::string& getFieldName() const { return fieldName; }
    const std::string& getErrorMessage() const { return errorMessage; }
    
private:
    std::string formatMessage(ErrorType type, const std::string& field, 
                           const std::string& message) {
        std::stringstream ss;
        ss << "Config validation error";
        switch (type) {
            case ErrorType::MISSING_REQUIRED_FIELD:
                ss << " (missing required field: '" << field << "')";
                break;
            case ErrorType::INVALID_TYPE:
                ss << " (invalid type for field: '" << field << "')";
                break;
            case ErrorType::VALUE_OUT_OF_RANGE:
                ss << " (value out of range for field: '" << field << "')";
                break;
            case ErrorType::DUPLICATE_FIELD:
                ss << " (duplicate field: '" << field << "')";
                break;
            case ErrorType::SCHEMA_VALIDATION_FAILED:
                ss << " (schema validation failed for field: '" << field << "')";
                break;
            case ErrorType::CONFIG_FILE_PARSE_ERROR:
                ss << " (config file parse error for field: '" << field << "')";
                break;
        }
        ss << ": " << message;
        return ss.str();
    }
    
    ErrorType errorType;
    std::string fieldName;
    std::string errorMessage;
};

/**
 * Configuration field definition
 */
struct ConfigField {
    std::string name;
    std::string type;
    std::string defaultValue;
    bool required;
    std::string description;
    std::vector<std::pair<std::string, double>> allowedValues; // name, min/max
    bool isArray;
    size_t minArraySize;
    size_t maxArraySize;
    
    ConfigField() = default;
    
    ConfigField(const std::string& name, const std::string& type,
                const std::string& defaultValue = "",
                bool required = false,
                const std::string& description = "",
                bool isArray = false,
                size_t minArraySize = 0,
                size_t maxArraySize = 0)
        : name(name), type(type), defaultValue(defaultValue), required(required),
          description(description), isArray(isArray), minArraySize(minArraySize),
          maxArraySize(maxArraySize) {}
};

/**
 * Configuration schema
 */
class ConfigSchema {
private:
    std::vector<ConfigField> fields;
    std::string schemaVersion;
    std::string description;
    
public:
    ConfigSchema(const std::string& version = "1.0", const std::string& desc = "")
        : schemaVersion(version), description(desc) {}
    
    // Add a field to the schema
    void addField(const ConfigField& field) {
        // Check for duplicate field names
        for (const auto& existing : fields) {
            if (existing.name == field.name) {
                throw ConfigValidationError(
                    ConfigValidationError::ErrorType::DUPLICATE_FIELD,
                    field.name, "Field already exists in schema"
                );
            }
        }
        fields.push_back(field);
    }
    
    // Get field by name
    const ConfigField* getField(const std::string& name) const {
        for (const auto& field : fields) {
            if (field.name == name) {
                return &field;
            }
        }
        return nullptr;
    }
    
    // Validate configuration against schema
    void validate(const Config& config) const {
        std::vector<std::string> missingFields;
        
        // Check required fields
        for (const auto& field : fields) {
            if (field.required) {
                if (!config.hasField(field.name)) {
                    missingFields.push_back(field.name);
                }
            }
        }
        
        if (!missingFields.empty()) {
            std::stringstream ss;
            ss << "Missing required fields: ";
            for (size_t i = 0; i < missingFields.size(); ++i) {
                ss << missingFields[i];
                if (i < missingFields.size() - 1) ss << ", ";
            }
            throw ConfigValidationError(
                ConfigValidationError::ErrorType::MISSING_REQUIRED_FIELD,
                "", ss.str()
            );
        }
        
        // Validate field types and values
        for (const auto& field : fields) {
            validateField(config, field);
        }
    }
    
    // Validate individual field
    void validateField(const Config& config, const ConfigField& field) const {
        if (!config.hasField(field.name)) {
            return; // Skip validation if field doesn't exist
        }
        
        // Check type compatibility
        if (!isTypeCompatible(config.getValueAsString(field.name), field.type)) {
            throw ConfigValidationError(
                ConfigValidationError::ErrorType::INVALID_TYPE,
                field.name, "Value type does not match field type"
            );
        }
        
        // Check value ranges
        if (!isValueInRange(config.getValueAsString(field.name), field)) {
            throw ConfigValidationError(
                ConfigValidationError::ErrorType::VALUE_OUT_OF_RANGE,
                field.name, "Value is out of allowed range"
            );
        }
        
        // Check array constraints
        if (field.isArray && !isValidArray(config.getValueAsString(field.name), field)) {
            throw ConfigValidationError(
                ConfigValidationError::ErrorType::VALUE_OUT_OF_RANGE,
                field.name, "Array does not meet size constraints"
            );
        }
    }
    
    // Check type compatibility
    bool isTypeCompatible(const std::string& value, const std::string& type) const {
        if (type == "int64_t" || type == "int" || type == "integer") {
            return isInteger(value);
        } else if (type == "float" || type == "double" || type == "number") {
            return isNumber(value);
        } else if (type == "bool" || type == "boolean") {
            return isBoolean(value);
        } else if (type == "string") {
            return true; // Any string is valid
        } else if (type == "array") {
            return isArray(value);
        }
        return false; // Unknown type
    }
    
    // Check if string is integer
    bool isInteger(const std::string& value) const {
        try {
            std::stoll(value);
            return true;
        } catch (...) {
            return false;
        }
    }
    
    // Check if string is number
    bool isNumber(const std::string& value) const {
        try {
            std::stod(value);
            return true;
        } catch (...) {
            return false;
        }
    }
    
    // Check if string is boolean
    bool isBoolean(const std::string& value) const {
        std::string lower = value;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        return lower == "true" || lower == "false" || lower == "1" || lower == "0";
    }
    
    // Check if string is array
    bool isArray(const std::string& value) const {
        // Simple array detection: starts with '[' and ends with ']'
        return value.size() >= 2 && value.front() == '[' && value.back() == ']';
    }
    
    // Check if value is in allowed range
    bool isValueInRange(const std::string& value, const ConfigField& field) const {
        if (field.allowedValues.empty()) {
            return true; // No constraints
        }
        
        // Check against allowed values
        for (const auto& allowed : field.allowedValues) {
            if (value == allowed.first) {
                return true;
            }
        }
        return false;
    }
    
    // Check if array is valid
    bool isValidArray(const std::string& value, const ConfigField& field) const {
        if (!field.isArray) {
            return true;
        }
        
        if (!isArray(value)) {
            return false;
        }
        
        // Count elements in array (naive implementation)
        size_t elementCount = 0;
        size_t start = 1; // Skip opening bracket
        size_t end = value.size() - 1; // Skip closing bracket
        
        for (size_t i = start; i <= end; ++i) {
            if (value[i] == ',') {
                elementCount++;
            }
        }
        elementCount++; // Account for last element
        
        return elementCount >= field.minArraySize && 
               (field.maxArraySize == 0 || elementCount <= field.maxArraySize);
    }
    
    // Get all field names
    std::vector<std::string> getFieldNames() const {
        std::vector<std::string> names;
        for (const auto& field : fields) {
            names.push_back(field.name);
        }
        return names;
    }
    
    // Get schema version
    const std::string& getSchemaVersion() const {
        return schemaVersion;
    }
    
    // Get schema description
    const std::string& getDescription() const {
        return description;
    }
    
    // Check if field exists
    bool hasField(const std::string& name) const {
        for (const auto& field : fields) {
            if (field.name == name) {
                return true;
            }
        }
        return false;
    }
};

/**
 * Configuration validator with predefined schemas
 */
class ConfigValidator {
public:
    // Default configuration schema
    static ConfigSchema getDefaultSchema() {
        ConfigSchema schema("1.0", "Default NLM configuration schema");
        
        // Required fields
        schema.addField(ConfigField("neuron_count", "int64_t", "1000", true,
                                   "Number of neurons in the brain"));
        
        schema.addField(ConfigField("region_count", "int64_t", "1", true,
                                   "Number of brain regions"));
        
        schema.addField(ConfigField("connection_probability", "float", "0.1", true,
                                   "Connection probability for random connectivity"));
        
        // Optional fields with constraints
        schema.addField(ConfigField("random_seed", "int64_t", "42", false,
                                   "Random seed for reproducible experiments"));
        
        schema.addField(ConfigField("simulation_timestep", "float", "0.001", false,
                                   "Simulation timestep in seconds"));
        
        schema.addField(ConfigField("stdp_ltp_weight", "float", "0.01", false,
                                   "STDP LTP weight"));
        
        schema.addField(ConfigField("stdp_ltd_weight", "float", "0.012", false,
                                   "STDP LTD weight"));
        
        schema.addField(ConfigField("stdp_tau", "float", "20.0", false,
                                   "STDP time constant"));
        
        schema.addField(ConfigField("synaptogenesis_rate", "float", "0.0001", false,
                                   "Synaptogenesis rate during development"));
        
        schema.addField(ConfigField("pruning_rate", "float", "0.00001", false,
                                   "Pruning rate during development"));
        
        schema.addField(ConfigField("environment_name", "string", "GridWorld", false,
                                   "Name of the environment"));
        
        schema.addField(ConfigField("environment_width", "int64_t", "20", false,
                                   "Width of the environment"));
        
        schema.addField(ConfigField("environment_height", "int64_t", "20", false,
                                   "Height of the environment"));
        
        schema.addField(ConfigField("log_level", "string", "INFO", false,
                                   "Logging level (DEBUG, INFO, WARNING, ERROR)"));
        
        schema.addField(ConfigField("log_to_file", "bool", "false", false,
                                   "Whether to log to file"));
        
        schema.addField(ConfigField("log_filename", "string", "nlm.log", false,
                                   "Log file name"));
        
        schema.addField(ConfigField("max_simulation_steps", "int64_t", "10000", false,
                                   "Maximum number of simulation steps"));
        
        schema.addField(ConfigField("simulation_time_limit", "float", "0.0", false,
                                   "Simulation time limit in seconds (0 = unlimited)"));
        
        schema.addField(ConfigField("visualization_enabled", "bool", "false", false,
                                   "Whether visualization is enabled"));
        
        schema.addField(ConfigField("visualization_update_rate", "float", "30.0", false,
                                   "Visualization update rate in Hz"));
        
        return schema;
    }
    
    // Validate configuration using default schema
    static void validateConfig(const Config& config) {
        ConfigSchema schema = getDefaultSchema();
        schema.validate(config);
    }
    
    // Validate configuration with custom schema
    static void validateConfig(const Config& config, const ConfigSchema& schema) {
        schema.validate(config);
    }
    
    // Load and validate configuration from file
    static std::shared_ptr<Config> loadAndValidateConfig(const std::string& filename) {
        auto config = std::make_shared<Config>();
        
        try {
            // Parse configuration file (simplified implementation)
            parseConfigFile(filename, config);
            
            // Validate with default schema
            validateConfig(*config);
            
            return config;
        } catch (const ConfigValidationError& e) {
            std::cerr << "Config validation error: " << e.what() << std::endl;
            throw;
        } catch (const std::exception& e) {
            throw ConfigValidationError(
                ConfigValidationError::ErrorType::CONFIG_FILE_PARSE_ERROR,
                "", std::string("Failed to parse config file: ") + e.what()
            );
        }
    }
    
private:
    // Parse configuration file
    static void parseConfigFile(const std::string& filename, std::shared_ptr<Config> config) {
        // Simplified implementation - in reality this would parse JSON/YAML/etc.
        // This is a placeholder for the actual implementation
        if (!config) {
            throw std::invalid_argument("Config is null");
        }
        
        // Add some test configuration values
        config->set("neuron_count", static_cast<int64_t>(500));
        config->set("region_count", static_cast<int64_t>(2));
        config->set("connection_probability", 0.1f);
        config->set("random_seed", static_cast<int64_t>(42));
        config->set("simulation_timestep", 0.001f);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
    }
};

/**
 * Configuration manager with validation support
 */
class ConfigManager {
private:
    std::shared_ptr<Config> currentConfig;
    ConfigSchema currentSchema;
    bool isValid = false;
    
public:
    ConfigManager() {
        currentSchema = ConfigValidator::getDefaultSchema();
    }
    
    ConfigManager(const ConfigSchema& schema) : currentSchema(schema) {}
    
    // Load configuration from file
    bool loadConfig(const std::string& filename) {
        try {
            currentConfig = ConfigValidator::loadAndValidateConfig(filename);
            isValid = true;
            return true;
        } catch (const ConfigValidationError& e) {
            std::cerr << "Failed to load config: " << e.what() << std::endl;
            isValid = false;
            return false;
        }
    }
    
    // Create configuration from parameters
    bool createConfig(const std::unordered_map<std::string, std::string>& params) {
        try {
            currentConfig = std::make_shared<Config>();
            
            for (const auto& param : params) {
                setValue(param.first, param.second);
            }
            
            currentSchema.validate(*currentConfig);
            isValid = true;
            return true;
        } catch (const ConfigValidationError& e) {
            std::cerr << "Failed to create config: " << e.what() << std::endl;
            isValid = false;
            return false;
        }
    }
    
    // Set configuration value
    bool setValue(const std::string& key, const std::string& value) {
        if (!currentConfig) {
            return false;
        }
        
        try {
            // Find field schema
            const ConfigField* field = currentSchema.getField(key);
            if (!field) {
                throw ConfigValidationError(
                    ConfigValidationError::ErrorType::INVALID_TYPE,
                    key, "Unknown configuration field"
                );
            }
            
            // Validate value type
            if (!currentSchema.isTypeCompatible(value, field->type)) {
                throw ConfigValidationError(
                    ConfigValidationError::ErrorType::INVALID_TYPE,
                    key, "Value type does not match field type"
                );
            }
            
            // Validate value range
            if (!currentSchema.isValueInRange(value, *field)) {
                throw ConfigValidationError(
                    ConfigValidationError::ErrorType::VALUE_OUT_OF_RANGE,
                    key, "Value is out of allowed range"
                );
            }
            
            // Set value
            if (field->type == "int64_t") {
                currentConfig->set(key, static_cast<int64_t>(std::stoll(value)));
            } else if (field->type == "float" || field->type == "double") {
                currentConfig->set(key, static_cast<float>(std::stod(value)));
            } else if (field->type == "bool") {
                bool boolValue = (value == "true" || value == "1");
                currentConfig->set(key, boolValue);
            } else {
                currentConfig->set(key, value);
            }
            
            isValid = true;
            return true;
        } catch (const ConfigValidationError& e) {
            std::cerr << "Config validation error for field '" << key << "': " << e.what() << std::endl;
            isValid = false;
            return false;
        }
    }
    
    // Get configuration value
    template<typename T>
    T getValue(const std::string& key, const T& defaultValue = T()) const {
        if (!currentConfig) {
            return defaultValue;
        }
        return currentConfig->getValue(key, defaultValue);
    }
    
    // Check if configuration is valid
    bool isConfigValid() const {
        return isValid && currentConfig != nullptr;
    }
    
    // Get current configuration
    std::shared_ptr<Config> getConfig() const {
        return currentConfig;
    }
    
    // Get schema
    const ConfigSchema& getSchema() const {
        return currentSchema;
    }
    
    // Get all field names from schema
    std::vector<std::string> getFieldNames() const {
        return currentSchema.getFieldNames();n    }
    
    // Print configuration
    void printConfig() const {
        if (!currentConfig) {
            std::cout << "No configuration loaded" << std::endl;
            return;
        }
        
        std::cout << "Configuration:" << std::endl;
        std::cout << "  Schema version: " << currentSchema.getSchemaVersion() << std::endl;
        std::cout << "  Description: " << currentSchema.getDescription() << std::endl;
        
        for (const auto& fieldName : currentSchema.getFieldNames()) {
            if (currentConfig->hasField(fieldName)) {
                std::cout << "  " << fieldName << ": " << currentConfig->getValueAsString(fieldName) << std::endl;
            }
        }
    }
};

/**
 * Configuration builder for easy configuration creation
 */
class ConfigBuilder {
private:
    std::unordered_map<std::string, std::string> params;
    
public:
    ConfigBuilder& set(const std::string& key, const std::string& value) {
        params[key] = value;
        return *this;
    }
    
    ConfigBuilder& set(const std::string& key, int64_t value) {
        params[key] = std::to_string(value);
        return *this;
    }
    
    ConfigBuilder& set(const std::string& key, float value) {
        params[key] = std::to_string(value);
        return *this;
    }
    
    ConfigBuilder& set(const std::string& key, double value) {
        params[key] = std::to_string(value);
        return *this;
    }
    
    ConfigBuilder& set(const std::string& key, bool value) {
        params[key] = value ? "true" : "false";
        return *this;
    }
    
    // Build configuration
    std::shared_ptr<Config> build() {
        ConfigManager manager;
        manager.createConfig(params);
        return manager.getConfig();
    }
    
    // Build and validate configuration
    std::shared_ptr<Config> buildAndValidate() {
        ConfigManager manager;
        if (!manager.createConfig(params)) {
            throw std::runtime_error("Failed to build and validate configuration");
        }
        return manager.getConfig();
    }
};

} // namespace nlm
