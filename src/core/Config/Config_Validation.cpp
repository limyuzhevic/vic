// Configuration validation and schema definition implementation
// This file extends the existing Config.cpp with validation capabilities

#include "Config.hpp"
#include <filesystem>
#include <algorithm>
#include <unordered_set>
#include <sstream>

namespace nlm {

// Configuration schema definition
struct ConfigSchema {
    std::string key;
    ConfigSource source;
    bool required;
    std::string description;
    std::string type; // "int", "double", "bool", "string", "list"
    
    // Validation constraints
    std::optional<int> minInt;
    std::optional<int> maxInt;
    std::optional<double> minDouble;
    std::optional<double> maxDouble;
    std::vector<std::string> allowedValues;
    
    ConfigSchema() : key(), source(ConfigSource::Default), required(false), description() {}
};

class ConfigValidator {
public:
    ConfigValidator() = default;
    
    // Validate configuration against a schema
    static bool validate(const Config& config, const std::vector<ConfigSchema>& schema) {
        std::vector<std::string> errors;
        bool valid = true;
        
        // Check required fields
        for (const auto& entry : schema) {
            if (entry.required && !config.has(entry.key)) {
                errors.push_back("Missing required field: " + entry.key);
                valid = false;
            }
        }
        
        // Validate field types and constraints
        for (const auto& entry : schema) {
            if (!config.has(entry.key)) {
                continue; // Skip optional fields that are not present
            }
            
            std::string error = validateField(config, entry.key, entry);
            if (!error.empty()) {
                errors.push_back(error);
                valid = false;
            }
        }
        
        // Log errors if validation failed
        if (!valid && !errors.empty()) {
            std::ostringstream oss;
            oss << "Configuration validation failed:";
            for (const auto& error : errors) {
                oss << "\n  - " << error;
            }
            NLM_LOG_ERROR(oss.str());
        }
        
        return valid;
    }
    
    // Validate a single field
    static std::string validateField(const Config& config, const std::string& key, const ConfigSchema& schema) {
        auto value = config.get<std::string>(key);
        if (!value.has_value()) {
            return "Field " + key + " not found in configuration";
        }
        
        // Basic type validation based on schema type
        if (schema.type == "int") {
            auto intVal = config.get<int>(key);
            if (!intVal.has_value()) {
                return "Field " + key + " should be an integer";
            }
            
            if (schema.minInt.has_value() && *intVal < *schema.minInt) {
                return "Field " + key + " must be >= " + std::to_string(*schema.minInt);
            }
            
            if (schema.maxInt.has_value() && *intVal > *schema.maxInt) {
                return "Field " + key + " must be <= " + std::to_string(*schema.maxInt);
            }
            
            if (!schema.allowedValues.empty() && 
                std::find(schema.allowedValues.begin(), schema.allowedValues.end(), std::to_string(*intVal)) == schema.allowedValues.end()) {
                return "Field " + key + " has invalid value: " + std::to_string(*intVal);
            }
        }
        else if (schema.type == "double") {
            auto doubleVal = config.get<double>(key);
            if (!doubleVal.has_value()) {
                return "Field " + key + " should be a double";
            }
            
            if (schema.minDouble.has_value() && *doubleVal < *schema.minDouble) {
                return "Field " + key + " must be >= " + std::to_string(*schema.minDouble);
            }
            
            if (schema.maxDouble.has_value() && *doubleVal > *schema.maxDouble) {
                return "Field " + key + " must be <= " + std::to_string(*schema.maxDouble);
            }
        }
        else if (schema.type == "bool") {
            auto boolVal = config.get<bool>(key);
            if (!boolVal.has_value()) {
                return "Field " + key + " should be a boolean";
            }
        }
        else if (schema.type == "string") {
            if (!schema.allowedValues.empty() && 
                std::find(schema.allowedValues.begin(), schema.allowedValues.end(), *value) == schema.allowedValues.end()) {
                return "Field " + key + " has invalid value: " + *value;
            }
        }
        
        return "";
    }
    
    // Get default schema for NLM
    static std::vector<ConfigSchema> getDefaultSchema() {
        std::vector<ConfigSchema> schema;
        
        // Brain configuration
        {
            ConfigSchema entry;
            entry.key = "brain.neuron_count";
            entry.required = true;
            entry.type = "int";
            entry.minInt = 1;
            entry.maxInt = 1000000;
            entry.description = "Number of neurons in the brain";
            schema.push_back(entry);
        }
        
        {
            ConfigSchema entry;
            entry.key = "brain.synapse_density";
            entry.required = false;
            entry.type = "double";
            entry.minDouble = 0.0;
            entry.maxDouble = 1.0;
            entry.description = "Synapse connectivity density (0.0 to 1.0)";
            schema.push_back(entry);
        }
        
        {
            ConfigSchema entry;
            entry.key = "brain.connection_probability";
            entry.required = false;
            entry.type = "double";
            entry.minDouble = 0.0;
            entry.maxDouble = 1.0;
            entry.description = "Probability of connection between neurons (0.0 to 1.0)";
            schema.push_back(entry);
        }
        
        // Plasticity configuration
        {
            ConfigSchema entry;
            entry.key = "plasticity.stdp.enable";
            entry.required = false;
            entry.type = "bool";
            entry.description = "Enable Spike-Timing-Dependent Plasticity (STDP)";
            schema.push_back(entry);
        }
        
        {
            ConfigSchema entry;
            entry.key = "plasticity.stdp.learning_rate";
            entry.required = false;
            entry.type = "double";
            entry.minDouble = 0.0;
            entry.description = "STDP learning rate";
            schema.push_back(entry);
        }
        
        // Neuromodulation configuration
        {
            ConfigSchema entry;
            entry.key = "neuromod.curiosity.enable";
            entry.required = false;
            entry.type = "bool";
            entry.description = "Enable curiosity-driven exploration";
            schema.push_back(entry);
        }
        
        {
            ConfigSchema entry;
            entry.key = "neuromod.novelty.enable";
            entry.required = false;
            entry.type = "bool";
            entry.description = "Enable novelty detection";
            schema.push_back(entry);
        }
        
        // Development configuration
        {
            ConfigSchema entry;
            entry.key = "development_synaptogenesis_rate";
            entry.required = false;
            entry.type = "double";
            entry.minDouble = 0.0;
            entry.description = "Synaptogenesis rate during development";
            schema.push_back(entry);
        }
        
        // Simulation configuration
        {
            ConfigSchema entry;
            entry.key = "simulation_timestep";
            entry.required = true;
            entry.type = "double";
            entry.minDouble = 0.0;
            entry.description = "Simulation time step in seconds";
            schema.push_back(entry);
        }
        
        {
            ConfigSchema entry;
            entry.key = "max_simulation_steps";
            entry.required = false;
            entry.type = "int";
            entry.minInt = 0;
            entry.description = "Maximum number of simulation steps";
            schema.push_back(entry);
        }
        
        return schema;
    }
    
    // Validate configuration file format
    static bool validateFileFormat(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        int lineNumber = 0;
        bool validFormat = true;
        
        while (std::getline(file, line)) {
            lineNumber++;
            line = trim(line);
            
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue; // Skip comments and empty lines
            }
            
            // Check for key=value format
            size_t pos = line.find('=');
            if (pos == std::string::npos) {
                std::cout << "Invalid format at line " << lineNumber << ": " << line << std::endl;
                validFormat = false;
                continue;
            }
            
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            if (key.empty()) {
                std::cout << "Missing key at line " << lineNumber << std::endl;
                validFormat = false;
            }
            
            // Check value format
            if (value.empty()) {
                std::cout << "Missing value at line " << lineNumber << std::endl;
                validFormat = false;
            }
        }
        
        return validFormat;
    }
    
    // Validate configuration against default schema
    static bool validateDefault(const Config& config) {
        auto schema = getDefaultSchema();
        return validate(config, schema);
    }
    
    // Get validation error messages
    static std::vector<std::string> getValidationErrors(const Config& config, const std::vector<ConfigSchema>& schema) {
        std::vector<std::string> errors;
        
        for (const auto& entry : schema) {
            if (entry.required && !config.has(entry.key)) {
                errors.push_back("Missing required field: " + entry.key);
            }
            
            if (config.has(entry.key)) {
                std::string error = validateField(config, entry.key, entry);
                if (!error.empty()) {
                    errors.push_back(error);
                }
            }
        }
        
        return errors;
    }
    
private:
    static std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }
};

// Configuration validation functions (to be added to Config.hpp)
namespace ConfigValidation {
    
    // Validate configuration against default schema
    inline bool validate(const Config& config) {
        return ConfigValidator::validateDefault(config);
    }
    
    // Validate configuration against custom schema
    inline bool validate(const Config& config, const std::vector<ConfigSchema>& schema) {
        return ConfigValidator::validate(config, schema);
    }
    
    // Get validation errors
    inline std::vector<std::string> getErrors(const Config& config) {
        return ConfigValidator::getValidationErrors(config, ConfigValidator::getDefaultSchema());
    }
    
    // Get validation errors with custom schema
    inline std::vector<std::string> getErrors(const Config& config, const std::vector<ConfigSchema>& schema) {
        return ConfigValidator::getValidationErrors(config, schema);
    }
    
    // Check if configuration is valid
    inline bool isValid(const Config& config) {
        return validate(config);
    }
    
    // Validate configuration file
    inline bool validateFile(const std::string& filepath) {
        return ConfigValidator::validateFileFormat(filepath);
    }
    
    // Get default schema
    inline std::vector<ConfigSchema> getDefaultSchema() {
        return ConfigValidator::getDefaultSchema();
    }
    
    // Add validation to Config::loadFromFile
    inline bool loadFromFileWithValidation(Config& config, const std::string& filepath) {
        if (!ConfigValidator::validateFile(filepath)) {
            std::cout << "Invalid configuration file format: " << filepath << std::endl;
            return false;
        }
        
        bool loaded = config.loadFromFile(filepath);
        if (loaded && !validate(config)) {
            std::cout << "Configuration loaded but validation failed: " << filepath << std::endl;
            return false;
        }
        
        return loaded;
    }
}

} // namespace nlm

// Add explicit instantiation for validation functions
namespace nlm {
    // Template instantiations for Config::get
    template std::optional<int> Config::get<int>(const std::string&) const;
    template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
    template std::optional<double> Config::get<double>(const std::string&) const;
    template std::optional<bool> Config::get<bool>(const std::string&) const;
    template std::optional<std::string> Config::get<std::string>(const std::string&) const;
    
    template int Config::getOr<int>(const std::string&, const int&) const;
    template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
    template double Config::getOr<double>(const std::string&, const double&) const;
    template bool Config::getOr<bool>(const std::string&, const bool&) const;
    template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;
}