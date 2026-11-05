#pragma once

#include "ConfigurationManager.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <unordered_set>
#include <regex>
#include <json-schema.hpp>
#include <yaml-cpp/yaml.h>

namespace nlm {

// Schema validation implementation
namespace SchemaValidation {

    // JSON schema for configuration validation
    const std::string JSON_SCHEMA = R"(
    {
        "$schema": "http://json-schema.org/draft-07/schema#",
        "title": "NLM Configuration Schema",
        "description": "Schema for validating NLM configuration files",
        "type": "object",
        "properties": {
            "nlm": {
                "type": "object",
                "properties": {
                    "random_seed": {
                        "type": "integer",
                        "minimum": 0,
                        "description": "Random seed for reproducible experiments"
                    },
                    "simulation_timestep": {
                        "type": "number",
                        "minimum": 0.0001,
                        "maximum": 1.0,
                        "description": "Simulation timestep in seconds"
                    },
                    "neuron_count": {
                        "type": "integer",
                        "minimum": 100,
                        "maximum": 100000,
                        "description": "Number of neurons in the brain"
                    },
                    "region_count": {
                        "type": "integer",
                        "minimum": 1,
                        "maximum": 10,
                        "description": "Number of brain regions"
                    },
                    "connection_probability": {
                        "type": "number",
                        "minimum": 0.0,
                        "maximum": 1.0,
                        "description": "Connection probability for random connectivity"
                    },
                    "plasticity": {
                        "type": "object",
                        "properties": {
                            "learning_rate": {
                                "type": "number",
                                "minimum": 0.0,
                                "maximum": 1.0,
                                "description": "Plasticity learning rate"
                            },
                            "stdp_ltp_weight": {
                                "type": "number",
                                "minimum": 0.0,
                                "maximum": 1.0,
                                "description": "STDP LTP weight"
                            },
                            "stdp_ltd_weight": {
                                "type": "number",
                                "minimum": 0.0,
                                "maximum": 1.0,
                                "description": "STDP LTD weight"
                            },
                            "stdp_time_constant": {
                                "type": "number",
                                "minimum": 1.0,
                                "description": "STDP time constant in milliseconds"
                            }
                        }
                    },
                    "development": {
                        "type": "object",
                        "properties": {
                            "synaptogenesis_rate": {
                                "type": "number",
                                "minimum": 0.0,
                                "maximum": 0.1,
                                "description": "Synaptogenesis rate during development"
                            },
                            "pruning_rate": {
                                "type": "number",
                                "minimum": 0.0,
                                "maximum": 0.01,
                                "description": "Pruning rate during development"
                            }
                        }
                    },
                    "neuromodulation": {
                        "type": "object",
                        "properties": {
                            "dopamine_baseline": {
                                "type": "number",
                                "minimum": 0.0,
                                "maximum": 1.0,
                                "description": "Baseline dopamine level"
                            }
                        }
                    },
                    "reward": {
                        "type": "object",
                        "properties": {
                            "discount_factor": {
                                "type": "number",
                                "minimum": 0.0,
                                "maximum": 1.0,
                                "description": "Reward discount factor"
                            }
                        }
                    },
                    "environment": {
                        "type": "object",
                        "properties": {
                            "name": {
                                "type": "string",
                                "description": "Environment name"
                            },
                            "width": {
                                "type": "integer",
                                "minimum": 1,
                                "description": "Environment width"
                            },
                            "height": {
                                "type": "integer",
                                "minimum": 1,
                                "description": "Environment height"
                            }
                        }
                    },
                    "logging": {
                        "type": "object",
                        "properties": {
                            "level": {
                                "type": "string",
                                "enum": ["DEBUG", "INFO", "WARN", "ERROR"],
                                "description": "Log level"
                            },
                            "to_file": {
                                "type": "boolean",
                                "description": "Whether to log to file"
                            },
                            "filename": {
                                "type": "string",
                                "description": "Log filename"
                            }
                        }
                    },
                    "simulation": {
                        "type": "object",
                        "properties": {
                            "max_steps": {
                                "type": "integer",
                                "minimum": 1,
                                "description": "Maximum simulation steps"
                            },
                            "time_limit": {
                                "type": "number",
                                "minimum": 0.0,
                                "description": "Time limit for simulation in seconds"
                            }
                        }
                    },
                    "visualization": {
                        "type": "object",
                        "properties": {
                            "enabled": {
                                "type": "boolean",
                                "description": "Whether visualization is enabled"
                            },
                            "update_rate": {
                                "type": "number",
                                "minimum": 1.0,
                                "description": "Visualization update rate in Hz"
                            }
                        }
                    }
                },
                "required": [
                    "random_seed",
                    "simulation_timestep",
                    "neuron_count",
                    "region_count",
                    "connection_probability"
                ]
            }
        },
        "required": ["nlm"]
    }
    )";

    // Validation error types
    enum class ValidationErrorType {
        MISSING_REQUIRED_FIELD,
        INVALID_TYPE,
        OUT_OF_RANGE,
        INVALID_VALUE,
        DUPLICATE_KEY,
        SCHEMA_VALIDATION_FAILED,
        FILE_FORMAT_NOT_SUPPORTED
    };

    struct ValidationError {
        ValidationErrorType type;
        std::string field;
        std::string message;
        std::string suggestion;

        ValidationError(ValidationErrorType t, const std::string& f, const std::string& m,
                       const std::string& s = "")
            : type(t), field(f), message(m), suggestion(s) {}
    };

    // Validate configuration against JSON schema
    std::vector<ValidationError> validateWithJSONSchema(const Config& config) {
        std::vector<ValidationError> errors;
        
        try {
            // Convert config to JSON
            nlohmann::json jsonConfig;
            jsonConfig["nlm"] = nlohmann::json::object();
            
            auto keys = config.getKeys();
            for (const auto& key : keys) {
                // Handle nested properties (e.g., plasticity.learning_rate)
                size_t dotPos = key.find('.');
                if (dotPos != std::string::npos) {
                    std::string parent = key.substr(0, dotPos);
                    std::string child = key.substr(dotPos + 1);
                    
                    if (!jsonConfig["nlm"].contains(parent)) {
                        jsonConfig["nlm"][parent] = nlohmann::json::object();
                    }
                    
                    auto value = config.getOptional<ConfigValue>(key);
                    if (value.has_value()) {
                        jsonConfig["nlm"][parent][child] = ConfigUtils::toString(*value);
                    }
                } else {
                    auto value = config.getOptional<ConfigValue>(key);
                    if (value.has_value()) {
                        jsonConfig["nlm"][key] = ConfigUtils::toString(*value);
                    }
                }
            }
            
            // Parse JSON schema
            auto schema = nlohmann::json::parse(JSON_SCHEMA);
            
            // Validate
            auto validator = nlohmann::json_schema::json_validator(schema);
            try {
                validator(jsonConfig);
            } catch (const nlohmann::json_schema::detail::validation_error& e) {
                errors.emplace_back(ValidationErrorType::SCHEMA_VALIDATION_FAILED,
                                  "nlm",
                                  std::string("Schema validation error: ") + e.what(),
                                  "Check configuration values against JSON schema");
            }
            
        } catch (const std::exception& e) {
            errors.emplace_back(ValidationErrorType::SCHEMA_VALIDATION_FAILED,
                              "configuration",
                              std::string("JSON schema validation exception: ") + e.what(),
                              "Check JSON schema file and configuration format");
        }
        
        return errors;
    }

    // Validate configuration with INI format rules
    std::vector<ValidationError> validateWithINIRules(const Config& config) {
        std::vector<ValidationError> errors;
        
        // Get all keys
        auto keys = config.getKeys();
        std::unordered_set<std::string> uniqueKeys(keys.begin(), keys.end());
        
        // Check for duplicates
        if (keys.size() != uniqueKeys.size()) {
            errors.emplace_back(ValidationErrorType::DUPLICATE_KEY,
                              "multiple",
                              "Duplicate configuration keys found",
                              "Remove duplicate keys from configuration");
        }
        
        // Validate specific key patterns and values
        for (const auto& key : keys) {
            auto value = config.getOptional<ConfigValue>(key);
            if (!value.has_value()) {
                continue;
            }
            
            // Special validation rules for known keys
            if (key == "random_seed") {
                auto seed = config.getOptional<int64_t>("random_seed");
                if (seed && (*seed < 0 || *seed > INT64_MAX)) {
                    errors.emplace_back(ValidationErrorType::OUT_OF_RANGE,
                                      "random_seed",
                                      "Random seed must be between 0 and 2^63-1",
                                      "Use a positive integer less than 9,223,372,036,854,775,807");
                }
            } else if (key == "neuron_count") {
                auto count = config.getOptional<size_t>("neuron_count");
                if (count && (*count < 100 || *count > 100000)) {
                    errors.emplace_back(ValidationErrorType::OUT_OF_RANGE,
                                      "neuron_count",
                                      "Neuron count must be between 100 and 100,000",
                                      "Adjust neuron_count to a value between 100 and 100,000");
                }
            } else if (key == "connection_probability") {
                auto prob = config.getOptional<double>("connection_probability");
                if (prob && (*prob < 0.0 || *prob > 1.0)) {
                    errors.emplace_back(ValidationErrorType::OUT_OF_RANGE,
                                      "connection_probability",
                                      "Connection probability must be between 0.0 and 1.0",
                                      "Set connection_probability to a value between 0.0 and 1.0");
                }
            } else if (key == "simulation_timestep") {
                auto timestep = config.getOptional<double>("simulation_timestep");
                if (timestep && (*timestep <= 0.0 || *timestep > 1.0)) {
                    errors.emplace_back(ValidationErrorType::OUT_OF_RANGE,
                                      "simulation_timestep",
                                      "Simulation timestep must be between 0.0001 and 1.0 seconds",
                                      "Set simulation_timestep to a small positive value (e.g., 0.001)");
                }
            } else if (key == "log_level") {
                auto level = config.getOptional<std::string>("log_level");
                if (level) {
                    std::string lowerLevel = ConfigUtils::toLower(*level);
                    if (lowerLevel != "debug" && lowerLevel != "info" && 
                        lowerLevel != "warn" && lowerLevel != "error") {
                        errors.emplace_back(ValidationErrorType::INVALID_VALUE,
                                          "log_level",
                                          std::string("Invalid log level: ") + *level,
                                          "Use one of: DEBUG, INFO, WARN, ERROR");
                    }
                }
            } else if (key == "region_count") {
                auto count = config.getOptional<size_t>("region_count");
                if (count && (*count < 1 || *count > 10)) {
                    errors.emplace_back(ValidationErrorType::OUT_OF_RANGE,
                                      "region_count",
                                      "Region count must be between 1 and 10",
                                      "Adjust region_count to a value between 1 and 10");
                }
            }
        }
        
        // Validate required dependencies
        if (config.has("log_level") && config.has("log_to_file")) {
            auto level = config.getOptional<std::string>("log_level");
            auto toFile = config.getOptional<bool>("log_to_file");
            if (level && toFile && *toFile && ConfigUtils::toLower(*level) == "debug") {
                NLM_LOG_WARN("Debug logging to file may produce large log files", 
                           "SchemaValidation::validateWithINIRules");
            }
        }
        
        return errors;
    }

    // Validate configuration with YAML format rules
    std::vector<ValidationError> validateWithYAMLRules(const Config& config) {
        // For now, use the same validation as INI rules
        // In a full implementation, this would validate YAML-specific rules
        return validateWithINIRules(config);
    }

    // Generate validation report
    std::string generateValidationReport(const std::vector<ValidationError>& errors) {
        std::stringstream report;
        
        if (errors.empty()) {
            report << "Configuration validation passed successfully.";
            return report.str();
        }
        
        report << "Configuration validation failed with " << errors.size() << " error(s):\n\n";
        
        for (size_t i = 0; i < errors.size(); ++i) {
            const auto& error = errors[i];
            
            report << (i + 1) << ". " << error.field << ": " << error.message;
            
            if (!error.suggestion.empty()) {
                report << "\n   Suggestion: " << error.suggestion;
            }
            
            report << "\n";
        }
        
        return report.str();
    }

    // Check if configuration is valid
    bool isConfigurationValid(const Config& config) {
        auto errors = validateWithINIRules(config);
        errors.insert(errors.end(), validateWithJSONSchema(config).begin(), 
                     validateWithJSONSchema(config).end());
        
        return errors.empty();
    }

    // Get all validation errors
    std::vector<ValidationError> getAllValidationErrors(const Config& config) {
        std::vector<ValidationError> errors;
        
        errors = validateWithINIRules(config);
        errors.insert(errors.end(), validateWithJSONSchema(config).begin(), 
                     validateWithJSONSchema(config).end());
        
        return errors;
    }

    // Validate configuration file format
    ValidationErrorType getFileFormat(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return ValidationErrorType::FILE_FORMAT_NOT_SUPPORTED;
        }
        
        std::string line;
        std::getline(file, line);
        file.close();
        
        // Check for INI format
        if (line.find('#') == 0 || line.find('[') != std::string::npos) {
            return ValidationErrorType::INVALID_TYPE; // INI format expected
        }
        
        // Check for JSON format
        if (line.find('{') == 0 || line.find('[') == 0) {
            try {
                nlohmann::json::parse(line);
                return ValidationErrorType::FILE_FORMAT_NOT_SUPPORTED; // JSON format
            } catch (...) {
                return ValidationErrorType::INVALID_TYPE;
            }
        }
        
        // Check for YAML format
        if (line.find(":") != std::string::npos || line.find("-") == 0) {
            try {
                YAML::Load(line);
                return ValidationErrorType::FILE_FORMAT_NOT_SUPPORTED; // YAML format
            } catch (...) {
                return ValidationErrorType::INVALID_TYPE;
            }
        }
        
        return ValidationErrorType::INVALID_TYPE;
    }

    // Validate configuration against specific schema
    std::vector<ValidationError> validateAgainstSchema(const Config& config, 
                                                      const std::string& schemaName) {
        std::vector<ValidationError> errors;
        
        if (schemaName == "neural_simulation") {
            errors = validateWithJSONSchema(config);
        } else if (schemaName == "basic") {
            errors = validateWithINIRules(config);
        } else if (schemaName == "yaml") {
            errors = validateWithYAMLRules(config);
        } else {
            errors.emplace_back(ValidationErrorType::INVALID_TYPE,
                              "schema",
                              std::string("Unknown schema: ") + schemaName,
                              "Use one of: neural_simulation, basic, yaml");
        }
        
        return errors;
    }

    // Export configuration to YAML format
    bool exportToYAML(const Config& config, const std::string& filepath) {
        try {
            YAML::Emitter emitter;
            emitter << YAML::BeginMap;
            
            auto keys = config.getKeys();
            for (const auto& key : keys) {
                emitter << YAML::Key << key;
                
                auto value = config.getOptional<ConfigValue>(key);
                if (value.has_value()) {
                    emitter << ConfigUtils::toString(*value);
                }
            }
            
            emitter << YAML::EndMap;
            
            std::ofstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            file << emitter.c_str();
            return true;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error exporting to YAML: ") + e.what(), 
                         "SchemaValidation::exportToYAML");
            return false;
        }
    }

    // Export configuration to JSON format
    bool exportToJSON(const Config& config, const std::string& filepath) {
        try {
            nlohmann::json jsonConfig;
            jsonConfig["nlm"] = nlohmann::json::object();
            
            auto keys = config.getKeys();
            for (const auto& key : keys) {
                // Handle nested properties (e.g., plasticity.learning_rate)
                size_t dotPos = key.find('.');
                if (dotPos != std::string::npos) {
                    std::string parent = key.substr(0, dotPos);
                    std::string child = key.substr(dotPos + 1);
                    
                    if (!jsonConfig["nlm"].contains(parent)) {
                        jsonConfig["nlm"][parent] = nlohmann::json::object();
                    }
                    
                    auto value = config.getOptional<ConfigValue>(key);
                    if (value.has_value()) {
                        jsonConfig["nlm"][parent][child] = ConfigUtils::toString(*value);
                    }
                } else {
                    auto value = config.getOptional<ConfigValue>(key);
                    if (value.has_value()) {
                        jsonConfig["nlm"][key] = ConfigUtils::toString(*value);
                    }
                }
            }
            
            std::ofstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            file << std::setw(2) << jsonConfig << std::endl;
            return true;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error exporting to JSON: ") + e.what(), 
                         "SchemaValidation::exportToJSON");
            return false;
        }
    }

    // Import configuration from YAML format
    bool importFromYAML(Config& config, const std::string& filepath) {
        try {
            config.clear();
            
            YAML::Node doc = YAML::LoadFile(filepath);
            
            if (!doc.IsMap() || !doc["nlm"].IsMap()) {
                NLM_LOG_ERROR("Invalid YAML configuration format: missing 'nlm' section", 
                             "SchemaValidation::importFromYAML");
                return false;
            }
            
            auto nlmNode = doc["nlm"];
            
            for (const auto& it : nlmNode) {
                std::string key = it.first.as<std::string>();
                std::string value = it.second.as<std::string>();
                
                // Convert value based on type
                if (key.find("learning_rate") != std::string::npos ||
                    key.find("time_constant") != std::string::npos ||
                    key.find("discount_factor") != std::string::npos) {
                    try {
                        double doubleValue = std::stod(value);
                        config.set(key, doubleValue, ConfigSource::File);
                    } catch (...) {
                        config.set(key, value, ConfigSource::File);
                    }
                } else if (key.find("seed") != std::string::npos ||
                          key.find("count") != std::string::npos ||
                          key.find("steps") != std::string::npos ||
                          key.find("width") != std::string::npos ||
                          key.find("height") != std::string::npos) {
                    try {
                        int64_t intValue = std::stoll(value);
                        config.set(key, intValue, ConfigSource::File);
                    } catch (...) {
                        config.set(key, value, ConfigSource::File);
                    }
                } else if (key.find("enabled") != std::string::npos ||
                          key.find("to_file") != std::string::npos) {
                    bool boolValue = (value == "true" || value == "1" || value == "yes");
                    config.set(key, boolValue, ConfigSource::File);
                } else {
                    config.set(key, value, ConfigSource::File);
                }
            }
            
            return true;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error importing from YAML: ") + e.what(), 
                         "SchemaValidation::importFromYAML");
            return false;
        }
    }

    // Import configuration from JSON format
    bool importFromJSON(Config& config, const std::string& filepath) {
        try {
            config.clear();
            
            std::ifstream file(filepath);
            if (!file.is_open()) {
                NLM_LOG_ERROR(std::string("Cannot open JSON configuration file: ") + filepath, 
                             "SchemaValidation::importFromJSON");
                return false;
            }
            
            nlohmann::json jsonConfig;
            file >> jsonConfig;
            
            if (!jsonConfig.contains("nlm") || !jsonConfig["nlm"].is_object()) {
                NLM_LOG_ERROR("Invalid JSON configuration format: missing 'nlm' section", 
                             "SchemaValidation::importFromJSON");
                return false;
            }
            
            auto nlmNode = jsonConfig["nlm"];
            
            for (const auto& it : nlmNode.items()) {
                std::string key = it.key();
                std::string value = it.value().dump();
                
                // Remove JSON-specific quotes
                if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
                    value = value.substr(1, value.size() - 2);
                }
                
                // Convert value based on type
                if (key.find("learning_rate") != std::string::npos ||
                    key.find("time_constant") != std::string::npos ||
                    key.find("discount_factor") != std::string::npos) {
                    try {
                        double doubleValue = std::stod(value);
                        config.set(key, doubleValue, ConfigSource::File);
                    } catch (...) {
                        config.set(key, value, ConfigSource::File);
                    }
                } else if (key.find("seed") != std::string::npos ||
                          key.find("count") != std::string::npos ||
                          key.find("steps") != std::string::npos ||
                          key.find("width") != std::string::npos ||
                          key.find("height") != std::string::npos) {
                    try {
                        int64_t intValue = std::stoll(value);
                        config.set(key, intValue, ConfigSource::File);
                    } catch (...) {
                        config.set(key, value, ConfigSource::File);
                    }
                } else if (key.find("enabled") != std::string::npos ||
                          key.find("to_file") != std::string::npos) {
                    bool boolValue = (value == "true" || value == "1" || value == "yes");
                    config.set(key, boolValue, ConfigSource::File);
                } else {
                    config.set(key, value, ConfigSource::File);
                }
            }
            
            return true;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error importing from JSON: ") + e.what(), 
                         "SchemaValidation::importFromJSON");
            return false;
        }
    }

    // Get validation statistics
    struct ValidationStats {
        size_t totalKeys;
        size_t validKeys;
        size_t invalidKeys;
        double validationScore;
        std::vector<std::string> warnings;
    };

    ValidationStats getValidationStats(const Config& config) {
        ValidationStats stats;
        stats.totalKeys = config.getKeys().size();
        stats.validKeys = 0;
        stats.invalidKeys = 0;
        stats.validationScore = 0.0;
        
        auto errors = getAllValidationErrors(config);
        stats.invalidKeys = errors.size();
        stats.validKeys = stats.totalKeys - stats.invalidKeys;
        
        if (stats.totalKeys > 0) {
            stats.validationScore = static_cast<double>(stats.validKeys) / stats.totalKeys * 100.0;
        }
        
        // Generate warnings for potential issues
        if (config.has("log_level") && config.has("log_to_file")) {
            auto level = config.getOptional<std::string>("log_level");
            auto toFile = config.getOptional<bool>("log_to_file");
            if (level && toFile && *toFile) {
                stats.warnings.push_back("Debug logging to file may produce large log files");
            }
        }
        
        if (config.has("neuron_count") && config.has("connection_probability")) {
            auto count = config.getOptional<size_t>("neuron_count");
            auto prob = config.getOptional<double>("connection_probability");
            if (count && prob && *count > 10000 && *prob > 0.5) {
                stats.warnings.push_back("Large network with high connection probability may cause performance issues");
            }
        }
        
        return stats;
    }

} // namespace SchemaValidation

// Global configuration validation utilities
namespace ConfigValidation {

    // Validate configuration against all available schemas
    std::vector<std::string> validateConfiguration(const Config& config) {
        std::vector<std::string> errors;
        
        // Validate against JSON schema
        auto jsonErrors = SchemaValidation::validateWithJSONSchema(config);
        for (const auto& error : jsonErrors) {
            errors.push_back("JSON Schema: " + error.message);
        }
        
        // Validate against INI rules
        auto iniErrors = SchemaValidation::validateWithINIRules(config);
        for (const auto& error : iniErrors) {
            errors.push_back("INI Format: " + error.message);
        }
        
        // Get validation statistics
        auto stats = SchemaValidation::getValidationStats(config);
        
        // Add warnings to errors if score is low
        if (stats.validationScore < 50.0) {
            errors.push_back("Configuration validation score is low (" + 
                           std::to_string(stats.validationScore) + "%)");
        }
        
        return errors;
    }

    // Create configuration from YAML string
    bool createConfigFromYAMLString(Config& config, const std::string& yamlString) {
        try {
            YAML::Node doc = YAML::Load(yamlString);
            if (!doc.IsMap() || !doc["nlm"].IsMap()) {
                NLM_LOG_ERROR("Invalid YAML configuration format", 
                             "ConfigValidation::createConfigFromYAMLString");
                return false;
            }
            
            config.clear();
            auto nlmNode = doc["nlm"];
            
            for (const auto& it : nlmNode) {
                std::string key = it.first.as<std::string>();
                std::string value = it.second.as<std::string>();
                config.set(key, value, ConfigSource::File);
            }
            
            return true;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error parsing YAML string: ") + e.what(), 
                         "ConfigValidation::createConfigFromYAMLString");
            return false;
        }
    }

    // Create configuration from JSON string
    bool createConfigFromJSONString(Config& config, const std::string& jsonString) {
        try {
            nlohmann::json jsonConfig = nlohmann::json::parse(jsonString);
            
            if (!jsonConfig.contains("nlm") || !jsonConfig["nlm"].is_object()) {
                NLM_LOG_ERROR("Invalid JSON configuration format", 
                             "ConfigValidation::createConfigFromJSONString");
                return false;
            }
            
            config.clear();
            auto nlmNode = jsonConfig["nlm"];
            
            for (const auto& it : nlmNode.items()) {
                std::string key = it.key();
                std::string value = it.value().dump();
                
                // Remove JSON-specific quotes
                if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
                    value = value.substr(1, value.size() - 2);
                }
                
                config.set(key, value, ConfigSource::File);
            }
            
            return true;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error parsing JSON string: ") + e.what(), 
                         "ConfigValidation::createConfigFromJSONString");
            return false;
        }
    }

    // Get configuration validation summary
    std::string getConfigurationSummary(const Config& config) {
        auto stats = SchemaValidation::getValidationStats(config);
        std::stringstream summary;
        
        summary << "Configuration Summary:\n";
        summary << "  Total keys: " << stats.totalKeys << "\n";
        summary << "  Valid keys: " << stats.validKeys << "\n";
        summary << "  Invalid keys: " << stats.invalidKeys << "\n";
        summary << "  Validation score: " << std::fixed << std::setprecision(1) 
                << stats.validationScore << "%\n";
        
        if (!stats.warnings.empty()) {
            summary << "  Warnings:\n";
            for (const auto& warning : stats.warnings) {
                summary << "    - " << warning << "\n";
            }
        }
        
        return summary.str();
    }

    // Check if configuration is ready for use
    bool isConfigurationReady(const Config& config) {
        auto errors = validateConfiguration(config);
        
        // Check for critical missing keys
        std::vector<std::string> criticalKeys = {
            "random_seed",
            "simulation_timestep",
            "neuron_count",
            "region_count",
            "connection_probability"
        };
        
        for (const auto& key : criticalKeys) {
            if (!config.has(key)) {
                errors.push_back("Missing critical configuration key: " + key);
            }
        }
        
        return errors.empty();
    }

} // namespace ConfigValidation
