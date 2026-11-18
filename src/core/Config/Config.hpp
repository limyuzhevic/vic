#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <regex>
#include <unordered_map>
#include <chrono>
#include <ctime>

namespace nlm {

// Configuration value types with better support
using ConfigValue = std::variant<
    int,
    int64_t,
    uint64_t,
    float,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>
>;

// Configuration validation result
struct ValidationResult {
    bool valid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    ValidationResult() : valid(true) {}
    ValidationResult(bool v, const std::string& error) : valid(v) {
        errors.push_back(error);
    }
    
    void addError(const std::string& error) {
        errors.push_back(error);
        valid = false;
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
    
    std::string getSummary() const {
        std::ostringstream oss;
        if (!valid) {
            oss << "Configuration validation FAILED: ";
            for (size_t i = 0; i < errors.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << errors[i];
            }
        }
        if (!warnings.empty()) {
            if (!valid) oss << " (with warnings): ";
            else oss << "Warnings: ";
            for (size_t i = 0; i < warnings.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << warnings[i];
            }
        }
        if (valid && warnings.empty()) {
            oss << "Configuration is valid";
        }
        return oss.str();
    }
};

// Enhanced configuration schema with validation rules
struct ConfigSchemaEntry {
    std::string key;
    ConfigValue defaultValue;
    std::string type;  // "int", "double", "string", "bool", "vector<int>", etc.
    std::string description;
    ConfigSource source;
    bool required;
    
    // Validation rules
    std::optional<std::pair<double, double>> range;      // min, max for numeric types
    std::optional<size_t> vectorMinSize;
    std::optional<size_t> vectorMaxSize;
    std::optional<std::regex> pattern;                   // for string validation
    std::optional<std::string> allowedValues;            // comma-separated
    
    ConfigSchemaEntry(const std::string& k, const ConfigValue& defaultVal, 
                     const std::string& t, const std::string& desc, 
                     ConfigSource s = ConfigSource::Default, bool req = false)
        : key(k), defaultValue(defaultVal), type(t), description(desc), 
          source(s), required(req) {}
    
    bool validate(const ConfigValue& value, ValidationResult& result) const {
        bool isValid = true;
        
        // Type checking
        if (type == "int" && !std::holds_alternative<int>(value)) {
            result.addError("Key \"" + key + "\" expects int type, got " + getTypeName(value));
            isValid = false;
        } else if (type == "int64_t" && !std::holds_alternative<int64_t>(value)) {
            result.addError("Key \"" + key + "\" expects int64_t type, got " + getTypeName(value));
            isValid = false;
        } else if (type == "uint64_t" && !std::holds_alternative<uint64_t>(value)) {
            result.addError("Key \"" + key + "\" expects uint64_t type, got " + getTypeName(value));
            isValid = false;
        } else if (type == "float" && !std::holds_alternative<float>(value)) {
            result.addError("Key \"" + key + "\" expects float type, got " + getTypeName(value));
            isValid = false;
        } else if (type == "double" && !std::holds_alternative<double>(value)) {
            result.addError("Key \"" + key + "\" expects double type, got " + getTypeName(value));
            isValid = false;
        } else if (type == "bool" && !std::holds_alternative<bool>(value)) {
            result.addError("Key \"" + key + "\" expects bool type, got " + getTypeName(value));
            isValid = false;
        } else if (type == "string" && !std::holds_alternative<std::string>(value)) {
            result.addError("Key \"" + key + "\" expects string type, got " + getTypeName(value));
            isValid = false;
        }
        
        // Range validation for numeric types
        if (isValid && range.has_value()) {
            double val = getNumericValue(value);
            double min = range->first;
            double max = range->second;
            if (val < min || val > max) {
                result.addError("Key \"" + key + "\" value " + std::to_string(val) +
                              " is outside range [" + std::to_string(min) + ", " + 
                              std::to_string(max) + "]");
                isValid = false;
            }
        }
        
        // Vector size validation
        if (isValid && vectorMinSize.has_value()) {
            size_t size = getVectorSize(value);
            if (size < vectorMinSize.value()) {
                result.addError("Key \"" + key + "\" vector size " + std::to_string(size) +
                              " is less than minimum " + std::to_string(vectorMinSize.value()));
                isValid = false;
            }
        }
        
        if (isValid && vectorMaxSize.has_value()) {
            size_t size = getVectorSize(value);
            if (size > vectorMaxSize.value()) {
                result.addError("Key \"" + key + "\" vector size " + std::to_string(size) +
                              " exceeds maximum " + std::to_string(vectorMaxSize.value()));
                isValid = false;
            }
        }
        
        // Pattern validation for strings
        if (isValid && pattern.has_value() && type == "string") {
            std::string str = std::get<std::string>(value);
            if (!std::regex_match(str, pattern.value())) {
                result.addError("Key \"" + key + "\" string \"" + str + \"\" does not match pattern");
                isValid = false;
            }
        }
        
        // Allowed values validation
        if (isValid && !allowedValues.empty() && type == "string") {
            std::string str = std::get<std::string>(value);
            std::string allowed = allowedValues.value();
            std::vector<std::string> allowedList;
            std::stringstream ss(allowed);
            std::string item;
            while (std::getline(ss, item, ',')) {
                allowedList.push_back(item);
            }
            if (std::find(allowedList.begin(), allowedList.end(), str) == allowedList.end()) {
                result.addError("Key \"" + key + "\" value \"" + str + \"\" is not in allowed values: " + allowed);
                isValid = false;
            }
        }
        
        return isValid;
    }
    
private:
    std::string getTypeName(const ConfigValue& value) const {
        if (std::holds_alternative<int>(value)) return "int";
        if (std::holds_alternative<int64_t>(value)) return "int64_t";
        if (std::holds_alternative<uint64_t>(value)) return "uint64_t";
        if (std::holds_alternative<float>(value)) return "float";
        if (std::holds_alternative<double>(value)) return "double";
        if (std::holds_alternative<bool>(value)) return "bool";
        if (std::holds_alternative<std::string>(value)) return "string";
        if (std::holds_alternative<std::vector<int>>(value)) return "vector<int>";
        if (std::holds_alternative<std::vector<double>>(value)) return "vector<double>";
        if (std::holds_alternative<std::vector<std::string>>(value)) return "vector<string>";
        return "unknown";
    }
    
    double getNumericValue(const ConfigValue& value) const {
        if (std::holds_alternative<int>(value)) return static_cast<double>(std::get<int>(value));
        if (std::holds_alternative<int64_t>(value)) return static_cast<double>(std::get<int64_t>(value));
        if (std::holds_alternative<uint64_t>(value)) return static_cast<double>(std::get<uint64_t>(value));
        if (std::holds_alternative<float>(value)) return static_cast<double>(std::get<float>(value));
        if (std::holds_alternative<double>(value)) return std::get<double>(value);
        return 0.0;
    }
    
    size_t getVectorSize(const ConfigValue& value) const {
        if (std::holds_alternative<std::vector<int>>(value)) return std::get<std::vector<int>>(value).size();
        if (std::holds_alternative<std::vector<double>>(value)) return std::get<std::vector<double>>(value).size();
        if (std::holds_alternative<std::vector<std::string>>(value)) return std::get<std::vector<std::string>>(value).size();
        return 0;
    }
};

class EnhancedConfig {
public:
    EnhancedConfig() {
        // Initialize schema with validation rules
        initializeSchema();
    }
    
    // Load configuration with validation
    bool loadFromFile(const std::string& filepath, ValidationResult& validationResult) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            validationResult.addError("Cannot open configuration file: " + filepath);
            return false;
        }
        
        std::string line;
        int lineNumber = 0;
        while (std::getline(file, line)) {
            ++lineNumber;
            line = trim(line);
            
            // Skip empty lines and comments
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            // Parse key=value pairs with better error handling
            size_t pos = line.find('=');
            if (pos == std::string::npos) {
                validationResult.addWarning("Invalid line " + std::to_string(lineNumber) + ": no '=' found");
                continue;
            }
            
            std::string key = trim(line.substr(0, pos));
            std::string valueStr = trim(line.substr(pos + 1));
            
            // Remove quotes
            if (valueStr.size() >= 2 && 
                ((valueStr.front() == '"' && valueStr.back() == '"') ||
                 (valueStr.front() == '\'' && valueStr.back() == '\''))) {
                valueStr = valueStr.substr(1, valueStr.size() - 2);
            }
            
            // Convert string value to appropriate type based on schema
            bool converted = setValidated(key, valueStr, ConfigSource::File, validationResult);
            if (!converted) {
                validationResult.addWarning("Failed to convert value for key \"" + key + "\" in line " + std::to_string(lineNumber));
            }
        }
        
        return true;
    }
    
    // Load from command line with validation
    bool loadFromArgs(int argc, char** argv, ValidationResult& validationResult) {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            // Handle --key=value format
            if (arg.substr(0, 2) == "--") {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    bool converted = setValidated(key, value, ConfigSource::CommandLine, validationResult);
                    if (!converted) {
                        validationResult.addWarning("Invalid command line value for key \"" + key + "\"");
                    }
                } else {
                    validationResult.addWarning("Invalid command line argument format: " + arg);
                }
            }
            // Handle -key value format
            else if (arg[0] == '-' && i + 1 < argc) {
                std::string key = arg.substr(1);
                std::string value = argv[++i];
                bool converted = setValidated(key, value, ConfigSource::CommandLine, validationResult);
                if (!converted) {
                    validationResult.addWarning("Invalid command line value for key \"" + key + "\"");
                }
            }
        }
        return true;
    }
    
    // Set value with validation
    template<typename T>
    void set(const std::string& key, const T& value, ConfigSource source = ConfigSource::Runtime) {
        ValidationResult tempResult;
        setValidated(key, value, source, tempResult);
        
        // If validation fails for schema, we still allow it for backward compatibility
        // but warn about it
        if (!tempResult.valid) {
            for (const auto& warning : tempResult.warnings) {
                std::cerr << "Warning: " << warning << std::endl;
            }
        }
    }
    
    // Get value with type safety
    template<typename T>
    std::optional<T> get(const std::string& key) const {
        auto it = std::find_if(entries.begin(), entries.end(),
                               [&key](const ConfigEntry& e) { return e.key == key; });
        if (it == entries.end()) {
            return std::nullopt;
        }
        
        return convertToType<T>(it->value);
    }
    
    // Get value with default
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const {
        auto val = get<T>(key);
        return val.has_value() ? val.value() : defaultValue;
    }
    
    // Check if key exists
    bool has(const std::string& key) const {
        return std::any_of(entries.begin(), entries.end(),
                          [&key](const ConfigEntry& e) { return e.key == key; });
    }
    
    // Get all keys
    std::vector<std::string> getKeys() const {
        std::vector<std::string> keys;
        keys.reserve(entries.size());
        for (const auto& entry : entries) {
            keys.push_back(entry.key);
        }
        return keys;
    }
    
    // Validate all entries against schema
    ValidationResult validateAll() const {
        ValidationResult result;
        
        for (const auto& entry : entries) {
            // Check if key is in schema
            auto schemaIt = schema.find(entry.key);
            if (schemaIt == schema.end()) {
                result.addWarning("Key \"" + entry.key + "\" is not in configuration schema");
                continue;
            }
            
            // Validate against schema
            if (!schemaIt->second.validate(entry.value, result)) {
                // Error already added to result
            }
            
            // Check if required key is missing
            if (schemaIt->second.required && !has(entry.key)) {
                result.addError("Required key \"" + entry.key + "\" is missing");
            }
        }
        
        // Check for missing required keys
        for (const auto& [key, schemaEntry] : schema) {
            if (schemaEntry.required && !has(key)) {
                result.addError("Required key \"" + key + "\" is missing");
            }
        }
        
        return result;
    }
    
    // Get configuration summary
    std::string summary() const {
        std::ostringstream oss;
        oss << "=== Enhanced Configuration Summary ===\n";
        oss << "Total entries: " << entries.size() << "\n";
        oss << "Total schema entries: " << schema.size() << "\n";
        oss << "\n";
        
        ValidationResult validation = validateAll();
        oss << "Validation: " << validation.getSummary() << "\n\n";
        
        oss << "=== Configuration Entries ===\n";
        for (const auto& entry : entries) {
            oss << entry.key << " = [";
            oss << getTypeName(entry.value) << "] (" << static_cast<int>(entry.source) << ")\n";
            oss << "    Description: " << getDescription(entry.key) << "\n";
        }
        
        return oss.str();
    }
    
    // Save configuration to file
    bool saveToFile(const std::string& filepath) const {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << "# Enhanced NLM Configuration File\n";
        file << "# Generated on " << getCurrentTimestamp() << "\n\n";
        
        for (const auto& entry : entries) {
            file << "# " << getDescription(entry.key) << "\n";
            file << "# Source: " << static_cast<int>(entry.source) << "\n";
            file << entry.key << " = " << getValueString(entry.value) << "\n\n";
        }
        
        return true;
    }
    
    // Get description for key
    std::string getDescription(const std::string& key) const {
        auto schemaIt = schema.find(key);
        if (schemaIt != schema.end()) {
            return schemaIt->second.description;
        }
        return "Unknown key: " + key;
    }
    
    // Get configuration value as string
    std::string getValueString(const ConfigValue& value) const {
        std::ostringstream oss;
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else {
                oss << arg;
            }
        }, value);
        return oss.str();
    }
    
    // Initialize configuration schema
    void initializeSchema() {
        // Basic configuration
        schema["random_seed"] = ConfigSchemaEntry(
            "random_seed", 42, "int64_t",
            "Random seed for reproducible experiments", ConfigSource::Default, true
        );
        
        schema["simulation_timestep"] = ConfigSchemaEntry(
            "simulation_timestep", 0.001, "double",
            "Simulation timestep in seconds", ConfigSource::Default, true
        );
        schema["simulation_timestep"].range = std::make_pair(0.0001, 1.0);
        
        schema["neuron_count"] = ConfigSchemaEntry(
            "neuron_count", 500, "uint64_t",
            "Number of neurons in the brain", ConfigSource::Default, true
        );
        schema["neuron_count"].range = std::make_pair(10, 10000);
        
        schema["region_count"] = ConfigSchemaEntry(
            "region_count", 1, "uint64_t",
            "Number of brain regions", ConfigSource::Default, true
        );
        schema["region_count"].range = std::make_pair(1, 10);
        
        schema["connection_probability"] = ConfigSchemaEntry(
            "connection_probability", 0.15f, "float",
            "Connection probability for random connectivity", ConfigSource::Default, true
        );
        schema["connection_probability"].range = std::make_pair(0.0f, 1.0f);
        
        // Plasticity settings
        schema["stdp_ltp_weight"] = ConfigSchemaEntry(
            "stdp_ltp_weight", 0.02f, "float",
            "STDP long-term potentiation weight", ConfigSource::Default, true
        );
        schema["stdp_ltp_weight"].range = std::make_pair(0.0f, 0.1f);
        
        schema["stdp_ltd_weight"] = ConfigSchemaEntry(
            "stdp_ltd_weight", 0.015f, "float",
            "STDP long-term depression weight", ConfigSource::Default, true
        );
        schema["stdp_ltd_weight"].range = std::make_pair(0.0f, 0.1f);
        
        schema["stdp_tau"] = ConfigSchemaEntry(
            "stdp_tau", 20.0, "double",
            "STDP time constant", ConfigSource::Default, true
        );
        schema["stdp_tau"].range = std::make_pair(1.0, 100.0);
        
        // Structural plasticity
        schema["synaptogenesis_rate"] = ConfigSchemaEntry(
            "synaptogenesis_rate", 0.0001f, "float",
            "Synaptogenesis rate", ConfigSource::Default, true
        );
        schema["synaptogenesis_rate"].range = std::make_pair(0.0f, 0.01f);
        
        schema["pruning_rate"] = ConfigSchemaEntry(
            "pruning_rate", 0.00001f, "float",
            "Pruning rate", ConfigSource::Default, true
        );
        schema["pruning_rate"].range = std::make_pair(0.0f, 0.001f);
        
        // Neuromodulation
        schema["dopamine_baseline"] = ConfigSchemaEntry(
            "dopamine_baseline", 0.1, "double",
            "Dopamine baseline level", ConfigSource::Default, true
        );
        schema["dopamine_baseline"].range = std::make_pair(0.0, 1.0);
        
        // Reward
        schema["reward_discount_factor"] = ConfigSchemaEntry(
            "reward_discount_factor", 0.99, "double",
            "Reward discount factor", ConfigSource::Default, true
        );
        schema["reward_discount_factor"].range = std::make_pair(0.9, 1.0);
        
        // Environment
        schema["environment_name"] = ConfigSchemaEntry(
            "environment_name", std::string("GridWorld"), "string",
            "Environment name", ConfigSource::Default, true
        );
        schema["environment_name"].allowedValues = "GridWorld,Random,Custom";
        
        schema["environment_width"] = ConfigSchemaEntry(
            "environment_width", 10, "int",
            "Environment width", ConfigSource::Default, true
        );
        schema["environment_width"].range = std::make_pair(5, 100);
        
        schema["environment_height"] = ConfigSchemaEntry(
            "environment_height", 10, "int",
            "Environment height", ConfigSource::Default, true
        );
        schema["environment_height"].range = std::make_pair(5, 100);
        
        // Logging
        schema["log_level"] = ConfigSchemaEntry(
            "log_level", std::string("INFO"), "string",
            "Log level", ConfigSource::Default, true
        );
        schema["log_level"].allowedValues = "DEBUG,INFO,WARN,ERROR,FATAL";
        
        schema["log_to_file"] = ConfigSchemaEntry(
            "log_to_file", false, "bool",
            "Log to file", ConfigSource::Default, true
        );
        
        schema["log_filename"] = ConfigSchemaEntry(
            "log_filename", std::string("nlm.log"), "string",
            "Log filename", ConfigSource::Default, true
        );
        
        // Simulation
        schema["max_simulation_steps"] = ConfigSchemaEntry(
            "max_simulation_steps", 10000, "uint64_t",
            "Maximum simulation steps", ConfigSource::Default, true
        );
        schema["max_simulation_steps"].range = std::make_pair(100, 1000000);
        
        // Performance
        schema["visualization_enabled"] = ConfigSchemaEntry(
            "visualization_enabled", false, "bool",
            "Enable visualization", ConfigSource::Default, true
        );
        
        schema["visualization_update_rate"] = ConfigSchemaEntry(
            "visualization_update_rate", 30.0, "double",
            "Visualization update rate (Hz)", ConfigSource::Default, true
        );
        schema["visualization_update_rate"].range = std::make_pair(1.0, 1000.0);
    }
    
private:
    struct ConfigEntry {
        std::string key;
        ConfigValue value;
        ConfigSource source;
    };
    
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, ConfigSchemaEntry> schema;
    
    // Internal helper methods
    bool setValidated(const std::string& key, const std::string& valueStr, 
                     ConfigSource source, ValidationResult& validationResult) {
        // Find or create entry
        auto it = std::find_if(entries.begin(), entries.end(),
                               [&key](const ConfigEntry& e) { return e.key == key; });n        
        if (it == entries.end()) {
            // Create new entry
            ConfigEntry newEntry;
            newEntry.key = key;
            newEntry.source = source;
            
            // Try to convert to appropriate type based on schema
            newEntry.value = convertStringToConfigValue(valueStr, validationResult);
            entries.push_back(newEntry);
        } else {
            // Update existing entry
            it->source = source;
            it->value = convertStringToConfigValue(valueStr, validationResult);
        }
        
        return !validationResult.valid || validationResult.errors.empty();
    }
    
    template<typename T>
    std::optional<T> convertToType(const ConfigValue& value) const {
        if constexpr (std::is_same_v<T, int>) {
            if (std::holds_alternative<int>(value)) {
                return std::get<int>(value);
            } else if (std::holds_alternative<int64_t>(value)) {
                return static_cast<int>(std::get<int64_t>(value));
            } else if (std::holds_alternative<double>(value)) {
                return static_cast<int>(std::get<double>(value));
            } else if (std::holds_alternative<float>(value)) {
                return static_cast<int>(std::get<float>(value));
            }
        } else if constexpr (std::is_same_v<T, int64_t>) {
            if (std::holds_alternative<int64_t>(value)) {
                return std::get<int64_t>(value);
            } else if (std::holds_alternative<int>(value)) {
                return static_cast<int64_t>(std::get<int>(value));
            } else if (std::holds_alternative<double>(value)) {
                return static_cast<int64_t>(std::get<double>(value));
            } else if (std::holds_alternative<float>(value)) {
                return static_cast<int64_t>(std::get<float>(value));
            }
        } else if constexpr (std::is_same_v<T, uint64_t>) {
            if (std::holds_alternative<uint64_t>(value)) {
                return std::get<uint64_t>(value);
            } else if (std::holds_alternative<int64_t>(value)) {
                return static_cast<uint64_t>(std::get<int64_t>(value));
            } else if (std::holds_alternative<int>(value)) {
                return static_cast<uint64_t>(std::get<int>(value));
            } else if (std::holds_alternative<double>(value)) {
                return static_cast<uint64_t>(std::get<double>(value));
            } else if (std::holds_alternative<float>(value)) {
                return static_cast<uint64_t>(std::get<float>(value));
            }
        } else if constexpr (std::is_same_v<T, float>) {
            if (std::holds_alternative<float>(value)) {
                return std::get<float>(value);
            } else if (std::holds_alternative<double>(value)) {
                return static_cast<float>(std::get<double>(value));
            } else if (std::holds_alternative<int>(value)) {
                return static_cast<float>(std::get<int>(value));
            } else if (std::holds_alternative<int64_t>(value)) {
                return static_cast<float>(std::get<int64_t>(value));
            }
        } else if constexpr (std::is_same_v<T, double>) {
            if (std::holds_alternative<double>(value)) {
                return std::get<double>(value);
            } else if (std::holds_alternative<float>(value)) {
                return static_cast<double>(std::get<float>(value));
            } else if (std::holds_alternative<int>(value)) {
                return static_cast<double>(std::get<int>(value));
            } else if (std::holds_alternative<int64_t>(value)) {
                return static_cast<double>(std::get<int64_t>(value));
            }
        } else if constexpr (std::is_same_v<T, bool>) {
            if (std::holds_alternative<bool>(value)) {
                return std::get<bool>(value);
            }
        } else if constexpr (std::is_same_v<T, std::string>) {
            if (std::holds_alternative<std::string>(value)) {
                return std::get<std::string>(value);
            }
        }
        
        return std::nullopt;
    }
    
    ConfigValue convertStringToConfigValue(const std::string& valueStr, ValidationResult& validationResult) {
        // Try to convert based on schema if available
        bool foundSchema = false;
        std::string targetType;
        
        // Check if key has schema and get expected type
        // For simplicity, we'll try all types in order
        
        // Try boolean
        if (valueStr == "true" || valueStr == "1") {
            return true;
        } else if (valueStr == "false" || valueStr == "0") {
            return false;
        }
        
        // Try integer
        try {
            size_t pos;
            int intVal = std::stoi(valueStr, &pos);
            if (pos == valueStr.size()) {
                return intVal;
            }
        } catch (...) {}
        
        // Try int64_t
        try {
            size_t pos;
            int64_t int64Val = std::stoll(valueStr, &pos);
            if (pos == valueStr.size()) {
                return int64Val;
            }
        } catch (...) {}
        
        // Try uint64_t
        try {
            size_t pos;
            uint64_t uint64Val = std::stoull(valueStr, &pos);
            if (pos == valueStr.size()) {
                return uint64Val;
            }
        } catch (...) {}
        
        // Try float
        try {
            size_t pos;
            float floatVal = std::stof(valueStr, &pos);
            if (pos == valueStr.size()) {
                return floatVal;
            }
        } catch (...) {}
        
        // Try double
        try {
            size_t pos;
            double doubleVal = std::stod(valueStr, &pos);
            if (pos == valueStr.size()) {
                return doubleVal;
            }
        } catch (...) {}
        
        // Treat as string
        return valueStr;
    }
    
    static std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }
    
    static std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
};

// Enhanced Config class that maintains backward compatibility
class Config {
public:
    Config() : enhancedConfig(std::make_unique<EnhancedConfig>()) {}
    ~Config() = default;
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept = default;
    Config& operator=(Config&&) noexcept = default;
    
    // Load from file with validation
    bool loadFromFile(const std::string& filepath) {
        ValidationResult validationResult;
        bool success = enhancedConfig->loadFromFile(filepath, validationResult);
        if (!validationResult.errors.empty()) {
            std::cerr << "Configuration validation errors:" << std::endl;
            for (const auto& error : validationResult.errors) {
                std::cerr << "  ERROR: " << error << std::endl;
            }
        }
        if (!validationResult.warnings.empty()) {
            std::cout << "Configuration validation warnings:" << std::endl;
            for (const auto& warning : validationResult.warnings) {
                std::cout << "  WARNING: " << warning << std::endl;
            }
        }
        return success;
    }
    
    // Load from command line with validation
    bool loadFromArgs(int argc, char** argv) {
        ValidationResult validationResult;
        bool success = enhancedConfig->loadFromArgs(argc, argv, validationResult);
        if (!validationResult.errors.empty()) {
            std::cerr << "Command line argument validation errors:" << std::endl;
            for (const auto& error : validationResult.errors) {
                std::cerr << "  ERROR: " << error << std::endl;
            }
        }
        if (!validationResult.warnings.empty()) {
            std::cout << "Command line argument validation warnings:" << std::endl;
            for (const auto& warning : validationResult.warnings) {
                std::cout << "  WARNING: " << warning << std::endl;
            }
        }
        return success;
    }
    
    // Set values with validation
    template<typename T>
    void set(const std::string& key, const T& value, ConfigSource source = ConfigSource::Runtime) {
        enhancedConfig->set(key, value, source);
    }
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const {
        return enhancedConfig->get<T>(key);
    }
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const {
        return enhancedConfig->getOr<T>(key, defaultValue);
    }
    
    // Check existence
    bool has(const std::string& key) const {
        return enhancedConfig->has(key);
    }
    
    // Get all keys
    std::vector<std::string> getKeys() const {
        return enhancedConfig->getKeys();
    }
    
    // Clear all
    void clear() {
        enhancedConfig->clear();
    }
    
    // Get configuration summary with validation
    std::string summary() const {
        return enhancedConfig->summary();
    }
    
    // Validate configuration
    ValidationResult validate() const {
        return enhancedConfig->validateAll();
    }
    
    // Save to file
    bool saveToFile(const std::string& filepath) const {
        return enhancedConfig->saveToFile(filepath);
    }
    
    // Get the enhanced config for advanced usage
    EnhancedConfig* getEnhancedConfig() {
        return enhancedConfig.get();
    }
    
private:
    std::unique_ptr<EnhancedConfig> enhancedConfig;
};

} // namespace nlm