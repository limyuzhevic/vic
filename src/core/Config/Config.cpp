#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <vector>
#include <stdexcept>

namespace nlm {

struct ConfigSchema {
    std::string key;
    std::string type; // "int", "int64", "double", "float", "bool", "string"
    std::string description;
    std::string defaultValue;
    std::string minValue;
    std::string maxValue;
    std::string validationRule;
    bool required;
    std::string phase; // "phase1", "phase2", "phase3", "phase4", "phase5", "phase6"
    
    ConfigSchema(std::string k, std::string t, std::string desc = "") 
        : key(std::move(k)), type(std::move(t)), description(std::move(desc)), 
          required(false), phase("phase1") {}
};

class ConfigValidator {
public:
    ConfigValidator() {
        initializeSchema();
    }
    
    std::vector<std::string> validate(const Config& config) {
        std::vector<std::string> errors;
        
        for (const auto& entry : schema_) {
            if (entry.required && !config.has(entry.key)) {
                errors.push_back("Required configuration key missing: " + entry.key);
                continue;
            }
            
            if (config.has(entry.key)) {
                std::string error = validateEntry(config, entry);
                if (!error.empty()) {
                    errors.push_back("Validation error for key '" + entry.key + "': " + error);
                }
            }
        }
        
        return errors;
    }
    
    void addSchemaEntry(const ConfigSchema& schema) {
        schema_.push_back(schema);
    }
    
    std::vector<ConfigSchema> getSchema() const {
        return schema_;
    }
    
    std::vector<std::string> getPhaseKeys(const std::string& phase) {
        std::vector<std::string> keys;
        for (const auto& entry : schema_) {
            if (entry.phase == phase) {
                keys.push_back(entry.key);
            }
        }
        return keys;
    }
    
private:
    std::vector<ConfigSchema> schema_;
    
    void initializeSchema() {
        // Phase 1 basic configuration
        addSchemaEntry(ConfigSchema("random_seed", "int64", "Random seed for reproducible experiments", "42", "0", "2147483647", "", true, "phase1"));
        addSchemaEntry(ConfigSchema("simulation_timestep", "double", "Simulation timestep in seconds", "0.001", "0.000001", "1.0", "> 0", true, "phase1"));
        addSchemaEntry(ConfigSchema("neuron_count", "int64", "Number of neurons in the brain", "1000", "1", "100000", "> 0", true, "phase1"));
        addSchemaEntry(ConfigSchema("region_count", "int64", "Number of brain regions", "2", "1", "100", "> 0", true, "phase1"));
        
        // Connectivity
        addSchemaEntry(ConfigSchema("connection_probability", "float", "Connection probability for random connectivity", "0.1", "0.0", "1.0", "between 0 and 1", true, "phase1"));
        
        // Plasticity
        addSchemaEntry(ConfigSchema("plasticity_learning_rate", "float", "Hebbian plasticity learning rate", "0.01", "0.0", "10.0", ">= 0", true, "phase1"));
        addSchemaEntry(ConfigSchema("stdp_ltp_weight", "float", "STDP LTP weight", "0.01", "0.0", "10.0", "> 0", true, "phase1"));
        addSchemaEntry(ConfigSchema("stdp_ltd_weight", "float", "STDP LTD weight", "0.012", "0.0", "10.0", "> 0", true, "phase1"));
        addSchemaEntry(ConfigSchema("stdp_time_constant", "float", "STDP time constant", "20.0", "1.0", "1000.0", "> 0", true, "phase1"));
        
        // Development
        addSchemaEntry(ConfigSchema("development_synaptogenesis_rate", "float", "Synaptogenesis rate", "0.001", "0.0", "0.1", ">= 0", true, "phase1"));
        addSchemaEntry(ConfigSchema("development_pruning_rate", "float", "Pruning rate", "0.0001", "0.0", "0.01", ">= 0", true, "phase1"));
        
        // Neuromodulation
        addSchemaEntry(ConfigSchema("dopamine_baseline", "float", "Baseline dopamine level", "0.0", "0.0", "10.0", ">= 0", true, "phase1"));
        
        // Reward
        addSchemaEntry(ConfigSchema("reward_discount_factor", "float", "Reward discount factor", "0.99", "0.0", "1.0", "between 0 and 1", true, "phase1"));
        
        // Environment
        addSchemaEntry(ConfigSchema("environment_name", "string", "Environment name", "GridWorld", "", "", "", true, "phase1"));
        addSchemaEntry(ConfigSchema("environment_width", "int64", "Environment width", "10", "1", "1000", "> 0", true, "phase1"));
        addSchemaEntry(ConfigSchema("environment_height", "int64", "Environment height", "10", "1", "1000", "> 0", true, "phase1"));
        
        // Logging
        addSchemaEntry(ConfigSchema("log_level", "string", "Log level (Debug, Info, Warning, Error, Critical)", "INFO", "", "", "one of: Debug, Info, Warning, Error, Critical", true, "phase1"));
        addSchemaEntry(ConfigSchema("log_to_file", "bool", "Log to file", "false", "", "", "", true, "phase1"));
        addSchemaEntry(ConfigSchema("log_filename", "string", "Log filename", "nlm.log", "", "", "", true, "phase1"));
        
        // Simulation
        addSchemaEntry(ConfigSchema("max_simulation_steps", "int64", "Maximum simulation steps", "10000", "1", "1000000", "> 0", true, "phase1"));
        addSchemaEntry(ConfigSchema("simulation_time_limit", "double", "Simulation time limit in seconds", "0.0", "0.0", "10000.0", ">= 0", true, "phase1"));
        
        // Visualization
        addSchemaEntry(ConfigSchema("visualization_enabled", "bool", "Enable visualization", "false", "", "", "", true, "phase1"));
        addSchemaEntry(ConfigSchema("visualization_update_rate", "float", "Visualization update rate (Hz)", "30.0", "1.0", "1000.0", "> 0", true, "phase1"));
        
        // Phase 2 specific
        addSchemaEntry(ConfigSchema("neuron_type_count", "int64", "Number of neuron types", "5", "1", "20", "> 0", false, "phase2"));
        addSchemaEntry(ConfigSchema("enable_stdp", "bool", "Enable STDP plasticity", "true", "", "", "", false, "phase2"));
        addSchemaEntry(ConfigSchema("stdp_learning_rate", "float", "STDP learning rate", "0.02", "0.0", "1.0", ">= 0", false, "phase2"));
        
        // Phase 3 specific
        addSchemaEntry(ConfigSchema("enable_reward_modulation", "bool", "Enable reward modulation", "true", "", "", "", false, "phase3"));
        addSchemaEntry(ConfigSchema("enable_structural_plasticity", "bool", "Enable structural plasticity", "true", "", "", "", false, "phase3"));
        addSchemaEntry(ConfigSchema("enable_development", "bool", "Enable development", "true", "", "", "", false, "phase3"));
        addSchemaEntry(ConfigSchema("enable_curiosity", "bool", "Enable curiosity-driven exploration", "true", "", "", "", false, "phase3"));
        addSchemaEntry(ConfigSchema("curiosity_threshold", "float", "Curiosity threshold for exploration", "0.2", "0.0", "1.0", "between 0 and 1", false, "phase3"));
        
        // Phase 4 specific
        addSchemaEntry(ConfigSchema("enable_prediction_system", "bool", "Enable prediction system", "true", "", "", "", false, "phase4"));
        addSchemaEntry(ConfigSchema("prediction_error_threshold", "float", "Prediction error threshold", "0.1", "0.0", "1.0", "between 0 and 1", false, "phase4"));
        addSchemaEntry(ConfigSchema("novelty_threshold", "float", "Novelty threshold for learning", "0.3", "0.0", "1.0", "between 0 and 1", false, "phase4"));
        addSchemaEntry(ConfigSchema("enable_cognition_systems", "bool", "Enable cognitive mechanisms", "true", "", "", "", false, "phase4"));
        
        // Phase 5 specific
        addSchemaEntry(ConfigSchema("enable_memory_systems", "bool", "Enable memory systems", "true", "", "", "", false, "phase5"));
        addSchemaEntry(ConfigSchema("memory_capacity_factor", "float", "Memory capacity scaling factor", "1.0", "0.1", "10.0", "> 0", false, "phase5"));
        addSchemaEntry(ConfigSchema("replay_consolidation_strength", "float", "Replay consolidation strength", "0.5", "0.0", "1.0", "between 0 and 1", false, "phase5"));
        
        // Phase 6 specific
        addSchemaEntry(ConfigSchema("enable_neuromodulation", "bool", "Enable neuromodulation system", "true", "", "", "", false, "phase6"));
        addSchemaEntry(ConfigSchema("neuromodulation_level", "float", "Neuromodulation level scaling", "1.0", "0.0", "2.0", "> 0", false, "phase6"));
        addSchemaEntry(ConfigSchema("enable_attention", "bool", "Enable attention mechanisms", "true", "", "", "", false, "phase6"));
        addSchemaEntry(ConfigSchema("attention_focus_strength", "float", "Attention focus strength", "0.8", "0.0", "1.0", "between 0 and 1", false, "phase6"));
        
        // Vision and sensory parameters
        addSchemaEntry(ConfigSchema("vision_width", "int64", "Vision width in pixels", "16", "1", "256", "> 0", false, "phase1"));
        addSchemaEntry(ConfigSchema("vision_height", "int64", "Vision height in pixels", "16", "1", "256", "> 0", false, "phase1"));
        addSchemaEntry(ConfigSchema("audio_sample_rate", "int64", "Audio sample rate", "22050", "1", "192000", "> 0", false, "phase1"));
        addSchemaEntry(ConfigSchema("internal_signals_count", "int64", "Number of internal signals", "10", "1", "100", "> 0", false, "phase1"));
        
        // Motor and action parameters
        addSchemaEntry(ConfigSchema("action_update_rate", "float", "Action update rate", "10.0", "1.0", "100.0", "> 0", false, "phase1"));
        addSchemaEntry(ConfigSchema("energy_decay_rate", "float", "Energy decay rate per timestep", "0.01", "0.0", "1.0", "between 0 and 1", false, "phase1"));
        addSchemaEntry(ConfigSchema("max_energy", "float", "Maximum energy capacity", "100.0", "1.0", "1000.0", "> 0", false, "phase1"));
    }
    
    std::string validateEntry(const Config& config, const ConfigSchema& schema) {
        if (!config.has(schema.key)) {
            return "Key not found in configuration";
        }
        
        auto value = getConfigValueAsString(config, schema.key);
        
        // Type validation
        if (!validateType(value, schema.type)) {
            return "Type mismatch. Expected " + schema.type + ", got string value";
        }
        
        // Range validation
        if (!validateRange(value, schema)) {
            return "Value out of range. Min: " + schema.minValue + ", Max: " + schema.maxValue;
        }
        
        // Custom validation rule
        if (!schema.validationRule.empty() && !validateCustom(value, schema.validationRule)) {
            return "Custom validation failed: " + schema.validationRule;
        }
        
        return "";
    }
    
    std::string getConfigValueAsString(const Config& config) const {
        std::string result;
        for (const auto& entry : config.pImpl->entries) {
            std::visit([&result](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    result = arg;
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    result = std::to_string(arg);
                } else if constexpr (std::is_same_v<T, double>) {
                    result = std::to_string(arg);
                } else if constexpr (std::is_same_v<T, float>) {
                    result = std::to_string(arg);
                } else if constexpr (std::is_same_v<T, bool>) {
                    result = arg ? "true" : "false";
                }
            }, entry.value);
        }
        return result;
    }
    
    std::string getConfigValueAsString(const Config& config, const std::string& key) const {
        auto it = std::find_if(config.pImpl->entries.begin(), config.pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it == config.pImpl->entries.end()) {
            return "";
        }
        
        std::string result;
        std::visit([&result](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                result = arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                result = std::to_string(arg);
            } else if constexpr (std::is_same_v<T, double>) {
                result = std::to_string(arg);
            } else if constexpr (std::is_same_v<T, float>) {
                result = std::to_string(arg);
            } else if constexpr (std::is_same_v<T, bool>) {
                result = arg ? "true" : "false";
            }
        }, it->value);
        
        return result;
    }
    
    bool validateType(const std::string& value, const std::string& expectedType) const {
        if (expectedType == "int64" || expectedType == "int") {
            try {
                std::stoll(value);
                return true;
            } catch (...) {
                return false;
            }
        } else if (expectedType == "double") {
            try {
                std::stod(value);
                return true;
            } catch (...) {
                return false;
            }
        } else if (expectedType == "float") {
            try {
                std::stof(value);
                return true;
            } catch (...) {
                return false;
            }
        } else if (expectedType == "bool") {
            return value == "true" || value == "false" || value == "1" || value == "0";
        } else if (expectedType == "string") {
            return !value.empty();
        }
        return false;
    }
    
    bool validateRange(const std::string& value, const ConfigSchema& schema) const {
        if (schema.minValue.empty() || schema.maxValue.empty()) {
            return true; // No range constraints
        }
        
        try {
            double val = std::stod(value);
            double minVal = std::stod(schema.minValue);
            double maxVal = std::stod(schema.maxValue);
            return val >= minVal && val <= maxVal;
        } catch (...) {
            return false;
        }
    }
    
    bool validateCustom(const std::string& value, const std::string& rule) const {
        if (rule.find("between") != std::string::npos && rule.find("and") != std::string::npos) {
            // Parse "between X and Y" rule
            size_t andPos = rule.find(" and ");
            if (andPos != std::string::npos) {
                std::string between = rule.substr(0, andPos);
                size_t betweenPos = between.find("between ");
                if (betweenPos != std::string::npos) {
                    std::string range = between.substr(betweenPos + 7);
                    size_t andPos2 = range.find(" and ");
                    if (andPos2 != std::string::npos) {
                        double minVal = std::stod(range.substr(0, andPos2));
                        double maxVal = std::stod(range.substr(andPos2 + 5));
                        double val = std::stod(value);
                        return val >= minVal && val <= maxVal;
                    }
                }
            }
        } else if (rule.find("one of:") != std::string::npos) {
            // Parse "one of: val1, val2, val3" rule
            size_t colonPos = rule.find(":");
            if (colonPos != std::string::npos) {
                std::string options = rule.substr(colonPos + 2);
                std::stringstream ss(options);
                std::string option;
                while (std::getline(ss, option, ',')) {
                    option = trim(option);
                    if (option == value) {
                        return true;
                    }
                }
            }
        } else if (rule.find(">= ") != std::string::npos) {
            // Parse ">= X" rule
            size_t spacePos = rule.find(">= ");
            if (spacePos != std::string::npos) {
                double minVal = std::stod(rule.substr(spacePos + 3));
                double val = std::stod(value);
                return val >= minVal;
            }
        } else if (rule.find("> ") != std::string::npos) {
            // Parse "> X" rule
            size_t spacePos = rule.find("> ");
            if (spacePos != std::string::npos) {
                double minVal = std::stod(rule.substr(spacePos + 2));
                double val = std::stod(value);
                return val > minVal;
            }
        }
        
        return true; // If we can't parse the rule, assume it's valid
    }
    
    std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }
};

} // namespace nlm
