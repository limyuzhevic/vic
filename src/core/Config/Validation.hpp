# Configuration Validation System for NLM
# Validates NLM configuration files against schema and provides detailed error reporting

#include "core/Config/Config.hpp"
#include <fstream>
#include <sstream>
#include <regex>
#include <unordered_map>
#include <iostream>
#include <vector>
#include <set>

using namespace nlm;

// NLM Configuration Schema
struct NLMConfigSchema {
    struct Field {
        std::string name;
        std::string type;  // int, int64, double, float, bool, string, vector<int>, vector<double>, vector<string>
        std::string defaultValue;
        std::string description;
        bool required;
        std::string validationPattern;  // Regex for string validation
        std::string validationRange;    // "min:max" for numeric types
        
        Field() : required(false) {}
        Field(const std::string& n, const std::string& t, const std::string& desc = "",
              bool req = false, const std::string& pattern = "", const std::string& range = "")
            : name(n), type(t), description(desc), required(req), 
              validationPattern(pattern), validationRange(range) {}
    };
    
    std::vector<Field> fields;
    std::vector<std::string> warnings;
    std::set<std::string> deprecatedFields;
    
    NLMConfigSchema() {
        // Initialize with known NLM configuration fields
        initializeFields();
    }
    
private:
    void initializeFields() {
        // Random seed and simulation settings
        fields.emplace_back("random_seed", "int64", "42", "Random seed for reproducible experiments", true, "^\\d+$", "0:");
        fields.emplace_back("simulation_timestep", "double", "0.001", "Time step for simulation in seconds", true, "^0\\.(\\d+)*$", "0.0:1.0");
        fields.emplace_back("neuron_count", "int64", "500", "Number of neurons in the brain", true, "^\\d+$", "1:");
        fields.emplace_back("region_count", "int64", "1", "Number of brain regions", true, "^\\d+$", "1:");
        fields.emplace_back("connection_probability", "float", "0.15", "Connection probability for random connectivity", true, "^(0|0\\.(\\d+)*|1|1\\.0*)$", "0:1");
        
        // Plasticity settings
        fields.emplace_back("plasticity_learning_rate", "float", "0.01", "Plasticity learning rate", false, "^(0|0\\.(\\d+)*|1|1\\.0*)$", "0:1");
        fields.emplace_back("stdp_ltp_weight", "float", "0.02", "STDP Long-Term Potentiation weight", false, "^(0|0\\.(\\d+)*|1|1\\.0*)$", "0:1");
        fields.emplace_back("stdp_ltd_weight", "float", "0.015", "STDP Long-Term Depression weight", false, "^(0|0\\.(\\d+)*|1|1\\.0*)$", "0:1");
        fields.emplace_back("stdp_time_constant", "double", "20.0", "STDP time constant", false, "^\\d+(\\.\\d+)?$", "0:");
        
        // Structural plasticity settings
        fields.emplace_back("synaptogenesis_rate", "float", "0.0001", "Synaptogenesis rate", false, "^(0|0\\.(\\d+)*)$", "0:1");
        fields.emplace_back("pruning_rate", "float", "0.00001", "Pruning rate", false, "^(0|0\\.(\\d+)*)$", "0:1");
        
        // Neuromodulation settings
        fields.emplace_back("dopamine_baseline", "float", "0.1", "Dopamine baseline level", false, "^(0|0\\.(\\d+)*)$", "0:1");
        fields.emplace_back("curiosity_enabled", "bool", "false", "Enable curiosity-driven exploration", false);
        fields.emplace_back("novelty_enabled", "bool", "false", "Enable novelty detection", false);
        
        // Memory system settings
        fields.emplace_back("working_memory_capacity", "int64", "100", "Working memory capacity", false, "^\\d+$", "0:");
        fields.emplace_back("episodic_memory_max_episodes", "int64", "100", "Maximum number of episodic memory episodes", false, "^\\d+$", "0:");
        fields.emplace_back("associative_memory_pattern_count", "int64", "50", "Number of associative memory patterns", false, "^\\d+$", "0:");
        
        // Prediction system settings
        fields.emplace_back("prediction_system_enabled", "bool", "true", "Enable prediction system", false);
        fields.emplace_back("prediction_horizon", "int64", "5", "Prediction horizon in time steps", false, "^\\d+$", "0:");
        
        // Cognition system settings
        fields.emplace_back("planner_enabled", "bool", "true", "Enable neural planner", false);
        fields.emplace_back("concept_formation_enabled", "bool", "true", "Enable concept formation", false);
        fields.emplace_back("attention_enabled", "bool", "true", "Enable attentional selection", false);
        
        // Development system settings
        fields.emplace_back("development_enabled", "bool", "true", "Enable developmental changes", false);
        fields.emplace_back("critical_period_enabled", "bool", "true", "Enable critical period learning", false);
        fields.emplace_back("development_synaptogenesis_rate", "float", "0.001", "Development synaptogenesis rate", false, "^(0|0\\.(\\d+)*)$", "0:1");
        fields.emplace_back("development_pruning_rate", "float", "0.0001", "Development pruning rate", false, "^(0|0\\.(\\d+)*)$", "0:1");
        
        // Environment settings
        fields.emplace_back("environment_name", "string", "GridWorld", "Environment name", false);
        fields.emplace_back("environment_width", "int64", "10", "Environment width", false, "^\\d+$", "1:");
        fields.emplace_back("environment_height", "int64", "10", "Environment height", false, "^\\d+$", "1:");
        
        // Vision settings
        fields.emplace_back("vision_width", "int64", "16", "Vision sensor width", false, "^\\d+$", "1:");
        fields.emplace_back("vision_height", "int64", "16", "Vision sensor height", false, "^\\d+$", "1:");
        
        // Audio settings
        fields.emplace_back("audio_enabled", "bool", "false", "Enable audio processing", false);
        
        // Internal signals settings
        fields.emplace_back("internal_signals_enabled", "bool", "true", "Enable internal signal processing", false);
        
        // Motor system settings
        fields.emplace_back("motor_system_enabled", "bool", "true", "Enable motor system", false);
        
        // Logging settings
        fields.emplace_back("log_level", "string", "INFO", "Log level (DEBUG, INFO, WARNING, ERROR, CRITICAL)", false);
        fields.emplace_back("log_to_file", "bool", "false", "Log to file instead of console", false);
        fields.emplace_back("log_filename", "string", "nlm.log", "Log filename", false);
        
        // Simulation settings
        fields.emplace_back("max_simulation_steps", "int64", "10000", "Maximum simulation steps", false, "^\\d+$", "0:");
        fields.emplace_back("simulation_time_limit", "double", "0.0", "Simulation time limit in seconds (0 = unlimited)", false, "^\\d+(\\.\\d+)?$", "0:");
        
        // Visualization settings
        fields.emplace_back("visualization_enabled", "bool", "false", "Enable visualization", false);
        fields.emplace_back("visualization_update_rate", "double", "30.0", "Visualization update rate in Hz", false, "^\\d+(\\.\\d+)?$", "0:");
        
        // Performance and monitoring
        fields.emplace_back("checkpointing_enabled", "bool", "false", "Enable checkpointing functionality", false);
        fields.emplace_back("checkpoint_interval", "int64", "1000", "Checkpoint interval in steps", false, "^\\d+$", "0:");
        fields.emplace_back("enable_replay", "bool", "false", "Enable memory replay functionality", false);
        
        // Advanced features
        fields.emplace_back("enable_curiosity", "bool", "false", "Enable curiosity-driven exploration", false);
        fields.emplace_back("enable_reflex_modulation", "bool", "false", "Enable reflex modulation", false);
        fields.emplace_back("enable_wta_dynamics", "bool", "false", "Enable winner-take-all dynamics", false);
        fields.emplace_back("enable_planner_override", "bool", "false", "Enable planner override of reflexes", false);
        
        // Warnings for deprecated fields
        deprecatedFields.insert("plasticity_learning_rate");
        
        // Add warnings
        warnings.push_back("The 'plasticity_learning_rate' field is deprecated and will be removed in a future version.");
        warnings.push_back("The 'dopamine_baseline' field had a duplicate entry in previous versions. Use only one value.");
    }
    
    // Validate configuration file
    bool validateFile(const std::string& filepath, std::vector<std::string>& errors, std::vector<std::string>& warnings) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            errors.push_back("Cannot open configuration file: " + filepath);
            return false;
        }
        
        std::string line;
        int lineNumber = 0;
        std::unordered_map<std::string, bool> foundFields;
        
        while (std::getline(file, line)) {
            lineNumber++;
            line = trim(line);
            
            // Skip comments and empty lines
            if (line.empty() || line.find('#') === 0 || line.find('//') === 0) {
                continue;
            }
            
            // Parse key = value format
            size_t pos = line.find('=');
            if (pos === std::string::npos) {
                errors.push_back("Line " + std::to_string(lineNumber) + ": Invalid format - missing '=' operator: " + line);
                continue;
            }
            
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Check for duplicate key
            if (foundFields.find(key) !== foundFields.end()) {
                errors.push_back("Line " + std::to_string(lineNumber) + ": Duplicate key: " + key);
                continue;
            }
            foundFields[key] = true;
            
            // Validate field against schema
            bool valid = validateField(key, value, errors, lineNumber);
            if (!valid) {
                continue;
            }
        }
        
        // Check for missing required fields
        for (const auto& field : fields) {
            if (field.required && foundFields.find(field.name) === foundFields.end()) {
                warnings.push_back("Field '" + field.name + "' not found in configuration file. Using default value.");
            }
        }
        
        // Check for unknown fields
        for (const auto& foundField : foundFields) {
            bool known = false;
            for (const auto& field : fields) {
                if (field.name === foundField.first) {
                    known = true;
                    break;
                }
            }
            if (!known && deprecatedFields.find(foundField.first) === deprecatedFields.end()) {
                warnings.push_back("Unknown field: '" + foundField.first + "'. This field may be ignored or deprecated.");
            }
        }
        
        file.close();
        return errors.empty();
    }
    
    // Validate individual field
    bool validateField(const std::string& key, const std::string& value, std::vector<std::string>& errors, int lineNumber) {
        // Find field definition in schema
        Field* field = nullptr;
        for (auto& f : fields) {
            if (f.name === key) {
                field = &f;
                break;
            }
        }
        
        if (!field) {
            // Check if deprecated
            if (deprecatedFields.find(key) !== deprecatedFields.end()) {
                errors.push_back("Line " + std::to_string(lineNumber) + ": Deprecated field: " + key);
                return false;
            }
            // Unknown field - just warn, don't error
            return true;
        }
        
        // Validate based on field type
        if (field->type === "int64") {
            try {
                long long int val = std::stoll(value);
                if (!field->validationRange.empty()) {
                    validateNumberRange(val, field->validationRange, errors, lineNumber, key);
                }
            } catch (const std::exception& e) {
                errors.push_back("Line " + std::to_string(lineNumber) + ": Field '" + key + "' must be a valid int64 value: " + value);
                return false;
            }
        } else if (field->type === "int") {
            try {
                int val = std::stoi(value);
                if (!field->validationRange.empty()) {
                    validateNumberRange(val, field->validationRange, errors, lineNumber, key);
                }
            } catch (const std::exception& e) {
                errors.push_back("Line " + std::to_string(lineNumber) + ": Field '" + key + "' must be a valid int value: " + value);
                return false;
            }
        } else if (field->type === "double") {
            try {
                double val = std::stod(value);
                if (!field->validationRange.empty()) {
                    validateNumberRange(val, field->validationRange, errors, lineNumber, key);
                }
            } catch (const std::exception& e) {
                errors.push_back("Line " + std::to_string(lineNumber) + ": Field '" + key + "' must be a valid double value: " + value);
                return false;
            }
        } else if (field->type === "float") {
            try {
                float val = std::stof(value);
                if (!field->validationRange.empty()) {
                    validateNumberRange(val, field->validationRange, errors, lineNumber, key);
                }
            } catch (const std::exception& e) {
                errors.push_back("Line " + std::to_string(lineNumber) + ": Field '" + key + "' must be a valid float value: " + value);
                return false;
            }
        } else if (field->type === "bool") {
            if (value !== "true" && value !== "false") {
                errors.push_back("Line " + std::to_string(lineNumber) + ": Field '" + key + "' must be 'true' or 'false': " + value);
                return false;
            }
        } else if (field->type === "string") {
            if (field->validationPattern.empty()) {
                // Basic validation - just check it's not empty
                if (value.empty()) {
                    errors.push_back("Line " + std::to_string(lineNumber) + ": Field '" + key + "' string value cannot be empty");
                    return false;
                }
            } else {
                // Validate with regex pattern
                std::regex pattern(field->validationPattern);
                if (!std::regex_match(value, pattern)) {
                    errors.push_back("Line " + std::to_string(lineNumber) + ": Field '" + key + "' value '" + value + "' does not match pattern " + field->validationPattern);
                    return false;
                }
            }
        }
        
        return true;
    }
    
    // Validate numeric value range
    template<typename T>
    void validateNumberRange(T value, const std::string& range, std::vector<std::string>& errors, int lineNumber, const std::string& key) {
        size_t colonPos = range.find(':');
        if (colonPos === std::string::npos) {
            return; // Invalid range format, skip validation
        }
        
        std::string minStr = range.substr(0, colonPos);
        std::string maxStr = range.substr(colonPos + 1);
        
        T min = (minStr.empty() ? std::numeric_limits<T>::lowest() : std::stod(minStr));
        T max = (maxStr.empty() ? std::numeric_limits<T>::max() : std::stod(maxStr));
        
        if (value < min || value > max) {
            errors.push_back("Line " + std::to_string(lineNumber) + ": Field '" + key + "' value " + std::to_string(value) + " is outside valid range [" + 
                           (minStr.empty() ? "-inf" : minStr) + ":" + (maxStr.empty() ? "inf" : maxStr) + "]");
        }
    }
    
    // Trim whitespace from string
    std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\n\r");
        if (first === std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t\n\r");
        return str.substr(first, (last - first + 1));
    }
    
    // Generate schema documentation
    std::string generateSchemaDocumentation() {
        std::stringstream doc;
        doc << "NLM Configuration Schema" << std::endl;
        doc << "=======================" << std::endl << std::endl;
        
        for (const auto& field : fields) {
            doc << "Field: " << field.name << std::endl;
            doc << "  Type: " << field.type << std::endl;
            doc << "  Description: " << field.description << std::endl;
            doc << "  Required: " << (field.required ? "Yes" : "No") << std::endl;
            if (!field.defaultValue.empty()) {
                doc << "  Default: " << field.defaultValue << std::endl;
            }
            if (!field.validationRange.empty()) {
                doc << "  Range: " << field.validationRange << std::endl;
            }
            if (!field.validationPattern.empty()) {
                doc << "  Pattern: " << field.validationPattern << std::endl;
            }
            doc << std::endl;
        }
        
        if (!warnings.empty()) {
            doc << "Warnings:" << std::endl;
            for (const auto& warning : warnings) {
                doc << "  - " << warning << std::endl;
            }
            doc << std::endl;
        }
        
        if (!deprecatedFields.empty()) {
            doc << "Deprecated Fields:" << std::endl;
            for (const auto& deprecated : deprecatedFields) {
                doc << "  - " << deprecated << std::endl;
            }
            doc << std::endl;
        }
        
        return doc.str();
    }
};

// Global configuration validator instance
NLMConfigSchema gConfigSchema;

// Configuration validation function
bool validateConfigFile(const std::string& filepath, std::vector<std::string>& errors, std::vector<std::string>& warnings) {
    return gConfigSchema.validateFile(filepath, errors, warnings);
}

// Generate configuration schema documentation
std::string generateConfigSchemaDocumentation() {
    return gConfigSchema.generateSchemaDocumentation();
}

// Validate configuration against config object
bool validateConfig(const Config& config, std::vector<std::string>& errors) {
    auto keys = config.getKeys();
    for (const auto& key : keys) {
        if (!gConfigSchema.validateField(key, "", errors, 0)) {
            return false;
        }
    }
    return true;
}