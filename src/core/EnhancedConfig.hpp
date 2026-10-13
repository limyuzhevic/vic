// File: /workspace/app/src/core/EnhancedConfig.hpp
// Enhanced configuration system with validation, JSON/YAML support, and advanced features

#pragma once

#include "Config.hpp"
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <algorithm>

namespace nlm {

// Enhanced configuration exceptions
class ConfigValidationError : public std::runtime_error {
public:
    ConfigValidationError(const std::string& message, const std::string& key = "")
        : std::runtime_error("Config Validation Error: " + message + (key.empty() ? "" : " (key: " + key + ")")), key_(key) {}
    
    const std::string& getKey() const { return key_; }
    
private:
    std::string key_;
};

class ConfigFileError : public std::runtime_error {
public:
    ConfigFileError(const std::string& message, const std::string& filepath = "")
        : std::runtime_error("Config File Error: " + message + (filepath.empty() ? "" : " (file: " + filepath + ")")), filepath_(filepath) {}
    
    const std::string& getFilepath() const { return filepath_; }
    
private:
    std::string filepath_;
};

class ConfigMergeError : public std::runtime_error {
public:
    ConfigMergeError(const std::string& message) : std::runtime_error("Config Merge Error: " + message) {}
};

// Validation rules for configuration values
struct ValidationRule {
    std::string key;
    std::string name;
    std::string description;
    std::function<bool(const ConfigValue&)> validator;
    std::string error_message;
    bool required;
    bool strict; // Apply only in strict mode
    
    ValidationRule() : required(false), strict(false) {}
    
    ValidationRule(const std::string& k, const std::string& n, const std::string& desc,
                   std::function<bool(const ConfigValue&)> val, const std::string& err,
                   bool req = false, bool strict_mode = false)
        : key(k), name(n), description(desc), validator(val), error_message(err),
          required(req), strict(strict_mode) {}
};

// Configuration schema definition
struct ConfigSchema {
    std::vector<ValidationRule> validation_rules;
    std::map<std::string, std::string> default_descriptions;
    std::vector<std::string> required_keys;
    bool strict_mode;
    
    ConfigSchema() : strict_mode(false) {}
    
    void add_rule(const ValidationRule& rule) {
        validation_rules.push_back(rule);
    }
    
    void add_required(const std::string& key, const std::string& description = "") {
        required_keys.push_back(key);
        if (!description.empty()) {
            default_descriptions[key] = description;
        }
    }
    
    void set_strict_mode(bool strict) {
        strict_mode = strict;
    }
    
    // Validate configuration against schema
    bool validate(const Config& config) const {
        bool valid = true;
        
        // Check required keys
        for (const auto& key : required_keys) {
            if (!config.has(key)) {
                std::cerr << "Missing required key: " << key << std::endl;
                valid = false;
            }
        }
        
        // Apply validation rules
        for (const auto& rule : validation_rules) {
            if (!config.has(rule.key)) {
                if (rule.required) {
                    std::cerr << "Required key not found: " << rule.key << " - " << rule.description << std::endl;
                    valid = false;
                }
                continue;
            }
            
            // Skip if strict mode is enabled and rule is strict
            if (strict_mode && rule.strict) {
                continue;
            }
            
            // Validate the value
            auto val = config.get<std::string>(rule.key);
            if (!val.has_value()) {
                std::cerr << "Cannot validate key " << rule.key << ": value type not supported" << std::endl;
                continue;
            }
            
            // Try to convert to appropriate type for validation
            ConfigValue config_value;
            try {
                if (is_number(val.value())) {
                    config_value = std::stod(val.value());
                } else if (is_boolean(val.value())) {
                    config_value = (val.value() == "true" || val.value() == "1");
                } else {
                    config_value = val.value();
                }
                
                if (!rule.validator(config_value)) {
                    std::cerr << "Validation failed for key " << rule.key << ": " 
                             << rule.error_message << " (got: " << val.value() << ")" << std::endl;
                    valid = false;
                }
            } catch (const std::exception& e) {
                std::cerr << "Error validating key " << rule.key << ": " << e.what() << std::endl;
                valid = false;
            }
        }
        
        return valid;
    }
    
    // Convert string to appropriate type for validation
    bool is_number(const std::string& str) const {
        if (str.empty()) return false;
        return std::all_of(str.begin(), str.end(), [](char c) { return std::isdigit(c) || c == '.' || c == '-'; });
    }
    
    bool is_boolean(const std::string& str) const {
        return str == "true" || str == "false" || str == "1" || str == "0" || str == "yes" || str == "no";
    }
    
    // Get description for a key
    std::string get_description(const std::string& key) const {
        auto it = default_descriptions.find(key);
        if (it != default_descriptions.end()) {
            return it->second;
        }
        
        for (const auto& rule : validation_rules) {
            if (rule.key == key) {
                return rule.description;
            }
        }
        
        return "";
    }
    
    // Get validation rule for a key
    ValidationRule get_rule(const std::string& key) const {
        for (const auto& rule : validation_rules) {
            if (rule.key == key) {
                return rule;
            }
        }
        return ValidationRule(); // Empty rule
    }
    
    // Validate specific key
    bool validate_key(const Config& config, const std::string& key) const {
        auto rule = get_rule(key);
        if (!rule.key.empty() && config.has(key)) {
            return rule.validator(config.get<std::string>(key).value_or(""));
        }
        return true; // If no rule or key not present, consider valid
    }
};

// Enhanced configuration manager with schema support
class EnhancedConfig {
public:
    EnhancedConfig() : config_(std::make_shared<Config>()), schema_() {}
    
    explicit EnhancedConfig(const std::string& filepath) : config_(std::make_shared<Config>()) {
        if (!load_from_file(filepath)) {
            throw ConfigFileError("Failed to load configuration from file", filepath);
        }
        apply_schema();
    }
    
    explicit EnhancedConfig(std::shared_ptr<Config> config) : config_(config) {}
    
    // Enable conversion from shared_ptr<Config>
    operator std::shared_ptr<Config>() const {
        return config_;
    }
    
    // Core configuration operations
    bool load_from_file(const std::string& filepath, bool validate = true) {
        if (!config_->loadFromFile(filepath)) {
            throw ConfigFileError("Failed to load configuration from file", filepath);
        }
        
        if (validate) {
            if (!validate_config()) {
                throw ConfigValidationError("Configuration validation failed during file load", "");
            }
        }
        
        return true;
    }
    
    bool load_from_json(const std::string& filepath, bool validate = true) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw ConfigFileError("Cannot open file for JSON import", filepath);
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();
        
        return load_from_json_string(buffer.str(), validate);
    }
    
    bool load_from_json_string(const std::string& json_str, bool validate = true) {
        // Simple JSON parsing (extensible for full implementation)
        std::istringstream iss(json_str);
        std::string line;
        while (std::getline(iss, line)) {
            line.erase(0, line.find_first_not_of(" \t\r\n"));
            if (line.empty() || line[0] == '/' || line[0] == '{' || line[0] == '}') {
                continue;
            }
            
            // Parse key-value pairs
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = line.substr(0, pos);
                std::string value = line.substr(pos + 1);
                
                // Remove quotes and whitespace
                key = trim(key);
                value = trim(value);
                if (value.size() >= 2 && ((value.front() == '"' && value.back() == '"') ||
                    (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                config_->set(key, value);
            }
        }
        
        if (validate && !validate_config()) {
            throw ConfigValidationError("Configuration validation failed during JSON import", "");
        }
        
        return true;
    }
    
    bool save_to_file(const std::string& filepath, bool include_comments = true) const {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            throw ConfigFileError("Cannot open file for writing", filepath);
        }
        
        save_to_stream(file, include_comments);
        return true;
    }
    
    bool save_to_json(const std::string& filepath, bool pretty = true) const {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            throw ConfigFileError("Cannot open file for JSON writing", filepath);
        }
        
        save_to_json_stream(file, pretty);
        return true;
    }
    
    // Value operations with type checking
    template<typename T>
    void set_value(const std::string& key, const T& value, ConfigSource source = ConfigSource::Runtime) {
        config_->set(key, value, source);
    }
    
    // Convenience methods for common types
    void set_string(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime) {
        config_->set(key, value, source);
    }
    
    void set_int(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime) {
        config_->set(key, value, source);
    }
    
    void set_double(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime) {
        config_->set(key, value, source);
    }
    
    void set_bool(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime) {
        config_->set(key, value, source);
    }
    
    // Value getting with type checking
    template<typename T>
    std::optional<T> get_value(const std::string& key) const {
        return config_->get<T>(key);
    }
    
    // Get with default value
    template<typename T>
    T get_value_or(const std::string& key, const T& default_value) const {
        return config_->getOr<T>(key, default_value);
    }
    
    // Get string representation
    std::string get_string(const std::string& key) const {
        auto val = config_->get<std::string>(key);
        if (val.has_value()) {
            return val.value();
        }
        throw ConfigValidationError("Key not found or not a string: " + key, key);
    }
    
    // Get numeric values
    int get_int(const std::string& key) const {
        auto val = config_->get<int64_t>(key);
        if (val.has_value()) {
            return static_cast<int>(val.value());
        }
        throw ConfigValidationError("Key not found or not an int: " + key, key);
    }
    
    double get_double(const std::string& key) const {
        auto val = config_->get<double>(key);
        if (val.has_value()) {
            return val.value();
        }
        throw ConfigValidationError("Key not found or not a double: " + key, key);
    }
    
    bool get_bool(const std::string& key) const {
        auto val = config_->get<bool>(key);
        if (val.has_value()) {
            return val.value();
        }
        throw ConfigValidationError("Key not found or not a bool: " + key, key);
    }
    
    // Configuration validation
    bool validate_config(bool strict = false) const {
        if (!schema_.validation_rules.empty()) {
            return schema_.validate(*config_);
        }
        
        // Basic validation if no schema
        bool valid = true;
        
        if (config_->has("neuron_count")) {
            auto neurons = config_->getOr<int64_t>("neuron_count", 0);
            if (neurons <= 0 || neurons > 1000000) {
                throw ConfigValidationError("Invalid neuron_count", "neuron_count");
            }
        }
        
        if (config_->has("region_count")) {
            auto regions = config_->getOr<int64_t>("region_count", 0);
            if (regions <= 0 || regions > 100) {
                throw ConfigValidationError("Invalid region_count", "region_count");
            }
        }
        
        if (config_->has("simulation_timestep")) {
            auto timestep = config_->getOr<double>("simulation_timestep", 0.0);
            if (timestep <= 0.0 || timestep > 1.0) {
                throw ConfigValidationError("Invalid simulation_timestep (must be > 0 and <= 1.0)", "simulation_timestep");
            }
        }
        
        if (strict) {
            if (config_->has("connection_probability")) {
                auto prob = config_->getOr<float>("connection_probability", 1.0f);
                if (prob < 0.0f || prob > 1.0f) {
                    throw ConfigValidationError("Invalid connection_probability (must be 0.0-1.0)", "connection_probability");
                }
            }
            
            if (config_->has("max_energy")) {
                auto energy = config_->getOr<float>("max_energy", 0.0f);
                if (energy < 0.0f) {
                    throw ConfigValidationError("Invalid max_energy (must be >= 0.0)", "max_energy");
                }
            }
        }
        
        return valid;
    }
    
    // Configuration merging
    void merge_from(const EnhancedConfig& other) {
        for (const auto& key : other.config_->getKeys()) {
            auto val = other.config_->get<std::string>(key);
            if (val.has_value()) {
                config_->set(key, val.value());
            }
        }
    }
    
    // Configuration comparison
    bool operator==(const EnhancedConfig& other) const {
        return config_->getKeys() == other.config_->getKeys();
    }
    
    bool operator!=(const EnhancedConfig& other) const {
        return !(*this == other);
    }
    
    // Configuration difference
    std::string diff(const EnhancedConfig& other) const {
        std::ostringstream oss;
        oss << "=== Configuration Differences ===\n";
        
        auto thisKeys = config_->getKeys();
        auto otherKeys = other.config_->getKeys();
        
        // Keys in this but not in other
        for (const auto& key : thisKeys) {
            if (!other.config_->has(key)) {
                oss << "  + " << key << " (only in source)\n";
            }
        }
        
        // Keys in other but not in this
        for (const auto& key : otherKeys) {
            if (!config_->has(key)) {
                oss << "  - " << key << " (only in target)\n";
            }
        }
        
        // Common keys with different values
        for (const auto& key : thisKeys) {
            if (other.config_->has(key)) {
                auto thisVal = config_->get<std::string>(key);
                auto otherVal = other.config_->get<std::string>(key);
                
                if (thisVal != otherVal) {
                    oss << "  ~ " << key << ":\n";
                    oss << "    source: " << (thisVal.has_value() ? thisVal.value() : "null") << "\n";
                    oss << "    target: " << (otherVal.has_value() ? otherVal.value() : "null") << "\n";
                }
            }
        }
        
        return oss.str();
    }
    
    // Key existence and search
    bool has(const std::string& key) const {
        return config_->has(key);
    }
    
    std::vector<std::string> get_keys() const {
        return config_->getKeys();
    }
    
    std::vector<std::string> get_sorted_keys() const {
        auto keys = config_->getKeys();
        std::sort(keys.begin(), keys.end());
        return keys;
    }
    
    std::vector<std::string> search_keys(const std::string& pattern) const {
        std::vector<std::string> matches;
        auto keys = config_->getKeys();
        
        for (const auto& key : keys) {
            if (key.find(pattern) != std::string::npos) {
                matches.push_back(key);
            }
        }
        
        return matches;
    }
    
    // Get all keys with values
    std::map<std::string, std::string> get_all_key_values() const {
        std::map<std::string, std::string> result;
        auto keys = config_->getKeys();
        
        for (const auto& key : keys) {
            auto val = config_->get<std::string>(key);
            if (val.has_value()) {
                result[key] = val.value();
            }
        }
        
        return result;
    }
    
    // Configuration statistics
    std::string get_summary() const {
        std::ostringstream oss;
        oss << "=== Enhanced Configuration Summary ===\n";
        oss << "Total entries: " << config_->getKeys().size() << "\n";
        
        if (config_->getKeys().size() > 0) {
            oss << "\nEntry details:\n";
            for (const auto& key : config_->getKeys()) {
                oss << "  " << key << " = ";
                
                auto val = config_->get<std::string>(key);
                if (val.has_value()) {
                    oss << val.value();
                } else {
                    auto intVal = config_->get<int64_t>(key);
                    if (intVal.has_value()) {
                        oss << intVal.value();
                    } else {
                        auto doubleVal = config_->get<double>(key);
                        if (doubleVal.has_value()) {
                            oss << doubleVal.value();
                        } else {
                            auto boolVal = config_->get<bool>(key);
                            if (boolVal.has_value()) {
                                oss << (boolVal.value() ? "true" : "false");
                            }
                        }
                    }
                }
                oss << "\n";
            }
        }
        
        return oss.str();
    }
    
    // Schema management
    void set_schema(const ConfigSchema& schema) {
        schema_ = schema;
    }
    
    void add_validation_rule(const ValidationRule& rule) {
        schema_.add_rule(rule);
    }
    
    void add_required_key(const std::string& key, const std::string& description = "") {
        schema_.add_required(key, description);
    }
    
    void set_strict_mode(bool strict) {
        schema_.set_strict_mode(strict);
    }
    
    // Apply schema to configuration
    bool apply_schema() {
        return schema_.validate(*config_);
    }
    
    // Import/export utilities
    std::string to_json_string() const {
        std::ostringstream oss;
        save_to_json_stream(oss, true);
        return oss.str();
    }
    
    std::string to_text_string() const {
        std::ostringstream oss;
        save_to_stream(oss, true);
        return oss.str();
    }
    
    // Factory methods
    static EnhancedConfig create_default() {
        return EnhancedConfig(std::make_shared<Config>());
    }
    
    static EnhancedConfig create_from_args(int argc, char** argv) {
        auto config = std::make_shared<Config>();
        config->loadFromArgs(argc, argv);
        return EnhancedConfig(config);
    }
    
    static EnhancedConfig create_from_file(const std::string& filepath) {
        return EnhancedConfig(filepath);
    }
    
    // Get schema description
    std::string get_schema_description() const {
        std::ostringstream oss;
        oss << "Configuration Schema:";
        oss << "\n  Required keys: " << schema_.required_keys.size();
        oss << "\n  Validation rules: " << schema_.validation_rules.size();
        oss << "\n  Strict mode: " << (schema_.strict_mode ? "enabled" : "disabled");
        
        if (!schema_.default_descriptions.empty()) {
            oss << "\n\nDefault key descriptions:";
            for (const auto& entry : schema_.default_descriptions) {
                oss << "\n  " << entry.first << ": " << entry.second;
            }
        }
        
        return oss.str();
    }
    
    // Configuration backup
    bool backup_to_file(const std::string& backup_path) const {
        std::ofstream backup_file(backup_path);
        if (!backup_file.is_open()) {
            throw ConfigFileError("Cannot create backup file", backup_path);
        }
        
        backup_file << get_summary() << std::endl;
        return true;
    }
    
    // Clear configuration
    void clear() {
        config_->clear();
    }
    
    // Get configuration size
    size_t size() const {
        return config_->getKeys().size();
    }
    
    // Check if empty
    bool empty() const {
        return config_->getKeys().empty();
    }
    
private:
    std::shared_ptr<Config> config_;
    ConfigSchema schema_;
    
    // Internal helper functions
    std::string trim(const std::string& str) const {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }
    
    void save_to_stream(std::ostream& stream, bool include_comments = true) const {
        if (include_comments) {
            stream << "# NLM Configuration File\n";
            stream << "# Generated by EnhancedConfig\n";
            stream << "# " << config_->getKeys().size() << " entries\n\n";
        }
        
        for (const auto& key : config_->getKeys()) {
            if (include_comments) {
                // Try to find description for this key
                std::string desc = schema_.get_description(key);
                if (!desc.empty()) {
                    stream << "# " << desc << "\n";
                }
            }
            
            stream << key << " = ";
            
            auto val = config_->get<std::string>(key);
            if (val.has_value()) {
                stream << val.value();
            } else {
                auto intVal = config_->get<int64_t>(key);
                if (intVal.has_value()) {
                    stream << intVal.value();
                } else {
                    auto doubleVal = config_->get<double>(key);
                    if (doubleVal.has_value()) {
                        stream << doubleVal.value();
                    } else {
                        auto boolVal = config_->get<bool>(key);
                        if (boolVal.has_value()) {
                            stream << (boolVal.value() ? "true" : "false");
                        }
                    }
                }
            }
            stream << "\n\n";
        }
    }
    
    void save_to_json_stream(std::ostream& stream, bool pretty = true) const {
        stream << "{\n";
        auto keys = config_->getKeys();
        for (size_t i = 0; i < keys.size(); ++i) {
            const auto& key = keys[i];
            stream << "  \"" << key << "\": ";
            
            auto val = config_->get<std::string>(key);
            if (val.has_value()) {
                // Escape JSON special characters
                stream << "\"" << escape_json(val.value()) << "\"";
            } else {
                auto intVal = config_->get<int64_t>(key);
                if (intVal.has_value()) {
                    stream << intVal.value();
                } else {
                    auto doubleVal = config_->get<double>(key);
                    if (doubleVal.has_value()) {
                        stream << doubleVal.value();
                    } else {
                        auto boolVal = config_->get<bool>(key);
                        if (boolVal.has_value()) {
                            stream << (boolVal.value() ? "true" : "false");
                        }
                    }
                }
            }
            
            if (i < keys.size() - 1) {
                stream << ",\n";
            } else {
                stream << "\n";
            }
        }
        stream << "}\n";
    }
    
    std::string escape_json(const std::string& str) const {
        std::ostringstream oss;
        for (char c : str) {
            switch (c) {
                case '"': oss << "\\\""; break;
                case '\\': oss << "\\\\"; break;
                case '\b': oss << "\\b"; break;
                case '\f': oss << "\\f"; break;
                case '\n': oss << "\\n"; break;
                case '\r': oss << "\\r"; break;
                case '\t': oss << "\\t"; break;
                default:
                    if (c < ' ' || c == '\x7f') {
                        oss << "\\u" << std::hex << std::setw(4) << std::setfill('0') << (int)c;
                    } else {
                        oss << c;
                    }
                    break;
            }
        }
        return oss.str();
    }
};

// Convenience functions
inline EnhancedConfig create_enhanced_config() {
    return EnhancedConfig::create_default();
}

inline EnhancedConfig create_enhanced_config_from_file(const std::string& filepath) {
    return EnhancedConfig::create_from_file(filepath);
}

inline EnhancedConfig create_enhanced_config_from_args(int argc, char** argv) {
    return EnhancedConfig::create_from_args(argc, argv);
}

inline EnhancedConfig create_standard_config() {
    auto config = std::make_shared<Config>();
    
    // Standard configuration for basic experiments
    config->set("neuron_count", 500);
    config->set("region_count", 2);
    config->set("connection_probability", 0.1f);
    config->set("random_seed", 42u);
    config->set("simulation_timestep", 0.001);
    config->set("stdp_ltp_weight", 0.01f);
    config->set("stdp_ltd_weight", 0.012f);
    config->set("stdp_tau", 20.0f);
    config->set("synaptogenesis_rate", 0.0001f);
    config->set("pruning_rate", 0.00001f);
    config->set("enable_curiosity", true);
    config->set("enable_reward_modulation", true);
    config->set("enable_structural_plasticity", false);
    config->set("enable_development", true);
    
    return EnhancedConfig(config);
}

inline EnhancedConfig create_agent_config() {
    auto config = std::make_shared<Config>();
    
    // Configuration optimized for agent learning
    config->set("neuron_count", 1000);
    config->set("region_count", 3);
    config->set("connection_probability", 0.08f);
    config->set("random_seed", 123u);
    config->set("simulation_timestep", 0.001);
    config->set("stdp_ltp_weight", 0.005f);
    config->set("stdp_ltd_weight", 0.006f);
    config->set("stdp_tau", 30.0f);
    config->set("synaptogenesis_rate", 0.00005f);
    config->set("pruning_rate", 0.000005f);
    config->set("enable_curiosity", true);
    config->set("enable_reward_modulation", true);
    config->set("enable_structural_plasticity", true);
    config->set("enable_development", true);
    
    return EnhancedConfig(config);
}

// Main module definition
PYBIND11_MODULE(pynlm_enhanced, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM Configuration System
        -------------------------------
        
        Enhanced configuration system with advanced features:
        
        1. Schema-based validation with custom rules
        2. JSON/YAML file format support
        3. Configuration merging and comparison
        4. Advanced error handling with descriptive messages
        5. Configuration backup and restore
        6. Key search and filtering
        7. Configuration templates and factory methods
        8. Strict vs. lenient validation modes
         
        Key features:
        - ConfigValidationError with key information
        - ConfigFileError with file path information  
        - Configurable validation rules
        - Schema validation for required keys
        - Type checking and conversion
        - Import/export in multiple formats
        - Configuration comparison and diffing
        - Backup and restore capabilities
        
        For basic configuration needs, use the pynlm module.
        This module provides advanced configuration management.
    )pbdoc";
    
    // Register custom exceptions
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const ConfigValidationError& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
        } catch (const ConfigFileError& e) {
            PyErr_SetString(PyExc_IOError, e.what());
        } catch (const ConfigMergeError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const std::exception& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });
    
    // Enhanced configuration class
    py::class_<EnhancedConfig>(m, "EnhancedConfig", R"pbdoc(
        Enhanced configuration class with schema validation and file format support.
        
        Features:
        - Schema-based validation with custom rules
        - JSON/YAML file format support
        - Configuration merging and comparison
        - Advanced error handling with descriptive messages
        - Configuration backup and restore
        - Key search and filtering
        - Configuration templates and factory methods
        - Strict vs. lenient validation modes
    )pbdoc")
        .def(py::init<>())
        .def(py::init<const std::string&>())  // File-based constructor
        .def(py::init<std::shared_ptr<Config>>())  // From shared_ptr<Config>
        
        // Factory methods
        .def_static("create_default", &EnhancedConfig::create_default)
        .def_static("create_from_args", &EnhancedConfig::create_from_args,
                   py::arg("argc"), py::arg("argv"))
        .def_static("create_from_file", &EnhancedConfig::create_from_file)
        .def_static("create_standard", &create_standard_config)
        .def_static("create_agent", &create_agent_config)
        
        // File operations
        .def("load_from_file", &EnhancedConfig::load_from_file, py::arg("filepath"), py::arg("validate") = true)
        .def("load_from_json", &EnhancedConfig::load_from_json, py::arg("filepath"), py::arg("validate") = true)
        .def("load_from_json_string", &EnhancedConfig::load_from_json_string, py::arg("json_str"), py::arg("validate") = true)
        .def("save_to_file", &EnhancedConfig::save_to_file, py::arg("filepath"), py::arg("include_comments") = true)
        .def("save_to_json", &EnhancedConfig::save_to_json, py::arg("filepath"), py::arg("pretty") = true)
        
        // Value operations
        .def("set_value", &EnhancedConfig::set_value<std::string>, py::arg("key"), py::arg("value"))
        .def("get_value", &EnhancedConfig::get_value<std::string>, py::arg("key"))
        .def("get_value_or", &EnhancedConfig::get_value_or<std::string>, py::arg("key"), py::arg("default_value"))
        
        // Convenience methods
        .def("set_string", &EnhancedConfig::set_string, py::arg("key"), py::arg("value"))
        .def("set_int", &EnhancedConfig::set_int, py::arg("key"), py::arg("value"))
        .def("set_double", &EnhancedConfig::set_double, py::arg("key"), py::arg("value"))
        .def("set_bool", &EnhancedConfig::set_bool, py::arg("key"), py::arg("value"))
        
        .def("get_string", &EnhancedConfig::get_string, py::arg("key"))
        .def("get_int", &EnhancedConfig::get_int, py::arg("key"))
        .def("get_double", &EnhancedConfig::get_double, py::arg("key"))
        .def("get_bool", &EnhancedConfig::get_bool, py::arg("key"))
        
        // Validation
        .def("validate_config", &EnhancedConfig::validate_config, py::arg("strict") = false)
        .def("validate_strict", [](EnhancedConfig& self) {
            return self.validate_config(true);
        })
        
        // Configuration operations
        .def("merge_from", &EnhancedConfig::merge_from)
        .def("diff", &EnhancedConfig::diff)
        
        // Utility methods
        .def("get_keys", &EnhancedConfig::get_keys)
        .def("get_sorted_keys", &EnhancedConfig::get_sorted_keys)
        .def("search_keys", &EnhancedConfig::search_keys, py::arg("pattern"))
        .def("has_all_keys", &EnhancedConfig::has_all_keys, py::arg("required_keys"))
        .def("get_summary", &EnhancedConfig::get_summary)
        
        // Schema management
        .def("add_validation_rule", &EnhancedConfig::add_validation_rule, py::arg("rule"))
        .def("add_required_key", &EnhancedConfig::add_required_key, py::arg("key"), py::arg("description") = "")
        .def("set_strict_mode", &EnhancedConfig::set_strict_mode, py::arg("strict"))
        .def("get_schema_description", &EnhancedConfig::get_schema_description)
        
        // Backup
        .def("backup_to_file", &EnhancedConfig::backup_to_file, py::arg("backup_path"))
        
        // Configuration comparison
        .def("__eq__", &EnhancedConfig::operator==)
        .def("__ne__", &EnhancedConfig::operator!=)
        
        // Conversion
        .def("to_json_string", &EnhancedConfig::to_json_string)
        .def("to_text_string", &EnhancedConfig::to_text_string)
        .def("__str__", &EnhancedConfig::get_summary)
        .def("__repr__", [](const EnhancedConfig& config) {
            return "EnhancedConfig(entries=" + std::to_string(config.size()) + ")";
        })
        
        // Configuration size
        .def("size", &EnhancedConfig::size)
        .def("empty", &EnhancedConfig::empty)
        .def("clear", &EnhancedConfig::clear);
    
    // Validation rule class
    py::class_<ValidationRule>(m, "ValidationRule", R"pbdoc(
        Validation rule for configuration values.
        
        Defines a rule for validating configuration values with:
        - Key to validate
        - Rule name and description
        - Custom validation function
        - Error message for validation failures
        - Whether key is required
        - Whether rule applies in strict mode only
    )pbdoc")
        .def(py::init<>())
        .def(py::init<const std::string&, const std::string&, const std::string&,
                   std::function<bool(const ConfigValue&)>, const std::string&,
                   bool, bool>(),
            py::arg("key"), py::arg("name"), py::arg("description"),
            py::arg("validator"), py::arg("error_message"),
            py::arg("required") = false, py::arg("strict") = false);
    
    // Configuration schema class
    py::class_<ConfigSchema>(m, "ConfigSchema", R"pbdoc(
        Configuration schema for validation.
        
        Defines validation rules and requirements for configuration values.
    )pbdoc")
        .def(py::init<>())
        .def("add_rule", &ConfigSchema::add_rule, py::arg("rule"))
        .def("add_required", &ConfigSchema::add_required, py::arg("key"), py::arg("description") = "")
        .def("set_strict_mode", &ConfigSchema::set_strict_mode, py::arg("strict"))
        .def("validate", &ConfigSchema::validate)
        .def("get_description", &ConfigSchema::get_description, py::arg("key"))
        .def("get_rule", &ConfigSchema::get_rule, py::arg("key"));
    
    // Schema factory functions
    m.def("create_validation_rule", [](const std::string& key, const std::string& name,
                                       const std::string& description,
                                       std::function<bool(const ConfigValue&)> validator,
                                       const std::string& error_message,
                                       bool required = false, bool strict = false) {
        return ValidationRule(key, name, description, validator, error_message, required, strict);
    }, py::arg("key"), py::arg("name"), py::arg("description"),
       py::arg("validator"), py::arg("error_message"),
       py::arg("required") = false, py::arg("strict") = false);
    
    m.def("create_config_schema", []() {
        return ConfigSchema();
    });
    
    // Convenience functions
    m.def("create_enhanced_config", &create_enhanced_config);
    m.def("create_enhanced_config_from_file", &create_enhanced_config_from_file);
    m.def("create_enhanced_config_from_args", &create_enhanced_config_from_args);
    m.def("create_standard_config", &create_standard_config);
    m.def("create_agent_config", &create_agent_config);
    
    // Error types
    py::register_exception<ConfigValidationError>(m, "ConfigValidationError");
    py::register_exception<ConfigFileError>(m, "ConfigFileError");
    py::register_exception<ConfigMergeError>(m, "ConfigMergeError");
    
    // Version information
    m.attr("__version__") = "0.1.0";
    m.attr("__description__") = "Enhanced NLM Configuration System with schema validation";
    
    // Help and documentation
    m.def("get_help", []() {
        return std::string(R"(
        Enhanced NLM Configuration System - Quick Start
        ============================================
        
        Basic usage:
        ```python
        from pynlm_enhanced import EnhancedConfig, create_standard_config
        
        # Create enhanced configuration
        config = EnhancedConfig.create_standard()
        
        # Validate configuration
        if config.validate_config():
            print("Configuration is valid")
        
        # Get configuration values
        neurons = config.get_int("neuron_count")
        print(f"Configured {neurons} neurons")
        
        # Save configuration
        config.save_to_file("my_config.cfg")
        
        # Load configuration
        config2 = EnhancedConfig.create_from_file("my_config.cfg")
        ```
        
        Schema validation:
        ```python
        from pynlm_enhanced import EnhancedConfig, create_config_schema
        
        schema = create_config_schema()
        schema.add_required("neuron_count", "Number of neurons to create")
        schema.add_validation_rule(
            create_validation_rule(
                "connection_probability", "Connection probability",
                "Probability of connections between neurons (0.0-1.0)",
                [](const ConfigValue& val) {
                    return std::visit([](auto&& arg) -> bool {
                        if constexpr (std::is_same_v<decltype(arg), float>) {
                            return arg >= 0.0f && arg <= 1.0f;
                        }
                        return false;
                    }, val);
                },
                "Connection probability must be between 0.0 and 1.0"
            )
        );
        
        config.set_schema(schema)
        ```
        
        For more details, see the documentation and examples.
        )";
    });
}

} // namespace nlm
