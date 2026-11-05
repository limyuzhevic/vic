#pragma once

/**
 * Unified Configuration System for NLM
 * 
 * Provides:
 * - Type-safe configuration parameters
 * - Validation and type checking
 * - Support for file-based configuration
 * - Command-line argument overrides
 * - Runtime configuration changes
 */

#include <string>
#include <map>
#include <vector>
#include <variant>
#include <memory>
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <chrono>
#include <ctime>

namespace nlm {

// Configuration value types
using ConfigValue = std::variant<
    int64_t,
    double,
    float,
    bool,
    std::string,
    std::vector<int64_t>,
    std::vector<double>,
    std::vector<std::string>
>;

/**
 * Configuration parameter metadata
 */
struct ConfigParameter {
    std::string name;
    ConfigValue defaultValue;
    std::string description;
    bool required = false;
    bool validated = false;
    
    ConfigParameter(const std::string& n, const ConfigValue& def, 
                   const std::string& desc, bool req = false)
        : name(n), defaultValue(def), description(desc), required(req) {}
};

/**
 * Configuration validation error
 */
class ConfigValidationError : public std::runtime_error {
public:
    ConfigValidationError(const std::string& message)
        : std::runtime_error(message) {}
};

/**
 * Main configuration class
 */
class Config {
private:
    std::map<std::string, ConfigValue> values_;
    std::vector<ConfigParameter> parameters_;
    std::map<std::string, std::string> sourceTrace_;
    bool validateOnSet_ = true;
    
public:
    Config() {
        initializeDefaultParameters();
    }
    
    /**
     * Get configuration value with type safety
     */
    template<typename T>
    T get(const std::string& key) const {
        auto it = values_.find(key);
        if (it == values_.end()) {
            throw ConfigValidationError("Configuration key not found: " + key);
        }
        try {
            return std::get<T>(it->second);
        } catch (const std::bad_variant_access&) {
            throw ConfigValidationError("Configuration value type mismatch for key: " + key);
        }
    }
    
    /**
     * Get configuration value with default
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const {
        auto it = values_.find(key);
        if (it == values_.end()) {
            return defaultValue;
        }
        try {
            return std::get<T>(it->second);
        } catch (const std::bad_variant_access&) {
            return defaultValue;
        }
    }
    
    /**
     * Set configuration value with validation
     */
    template<typename T>
    void set(const std::string& key, const T& value, const std::string& source = "unknown") {
        // Check if parameter exists
        auto paramIt = std::find_if(parameters_.begin(), parameters_.end(),
            [&](const ConfigParameter& p) { return p.name == key; });
        
        if (paramIt != parameters_.end()) {
            // Validate parameter
            if (paramIt->validated && values_.find(key) != values_.end()) {
                // Re-validation check
                T oldValue = get<T>(key);
                if (validateValue(key, oldValue, value)) {
                    values_[key] = value;
                    sourceTrace_[key] = source;
                }
            } else {
                values_[key] = value;
                sourceTrace_[key] = source;
                paramIt->validated = true;
            }
        } else {
            // Add new parameter
            values_[key] = value;
            sourceTrace_[key] = source;
            parameters_.emplace_back(key, ConfigValue(value), "Dynamic parameter");
        }
    }
    
    /**
     * Load configuration from file
     */
    bool loadFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            // Skip comments
            if (line.empty() || line[0] == '#') continue;
            
            size_t pos = line.find('=');
            if (pos == std::string::npos) continue;
            
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            
            // Trim whitespace
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            
            // Parse value based on type
            try {
                // Try to detect type
                if (value == "true" || value == "false") {
                    bool boolValue = (value == "true");
                    set<bool>(key, boolValue, "file:" + filename);
                } else if (value.find('.') != std::string::npos) {
                    double doubleValue = std::stod(value);
                    set<double>(key, doubleValue, "file:" + filename);
                } else {
                    try {
                        int64_t intValue = std::stoll(value);
                        set<int64_t>(key, intValue, "file:" + filename);
                    } catch (...) {
                        // Try to parse as float
                        try {
                            float floatValue = std::stof(value);
                            set<float>(key, floatValue, "file:" + filename);
                        } catch (...) {
                            // String value
                            set<std::string>(key, value, "file:" + filename);
                        }
                    }
                }
            } catch (const std::exception& e) {
                std::cerr << "Warning: Failed to parse config value for key '" 
                         << key << "': " << e.what() << std::endl;
            }
        }
        
        file.close();
        return true;
    }
    
    /**
     * Load configuration from command line arguments
     */
    void loadFromArgs(int argc, char* argv[]) {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            // Support --key=value or --key value format
            if (arg.find("--") == 0) {
                if (arg.find('=') != std::string::npos) {
                    // --key=value format
                    size_t eqPos = arg.find('=');
                    std::string key = arg.substr(2, eqPos - 2);
                    std::string value = arg.substr(eqPos + 1);
                    setFromString(key, value, "command_line");
                } else if (i + 1 < argc && argv[i + 1][0] != '-') {
                    // --key value format
                    std::string key = arg.substr(2);
                    std::string value = argv[++i];
                    setFromString(key, value, "command_line");
                }
            }
        }
    }
    
    /**
     * Validate configuration value
     */
    bool validateValue(const std::string& key, const ConfigValue& oldValue, const ConfigValue& newValue) {
        // Basic validation - can be extended for specific parameter types
        if (std::holds_alternative<bool>(newValue)) {
            return true; // Boolean values are always valid
        } else if (std::holds_alternative<int64_t>(newValue)) {
            int64_t val = std::get<int64_t>(newValue);
            if (key == "neuron_count" && (val < 1 || val > 100000)) return false;
            if (key == "region_count" && (val < 1 || val > 1000)) return false;
            return true;
        } else if (std::holds_alternative<double>(newValue)) {
            double val = std::get<double>(newValue);
            if (key == "simulation_timestep" && (val <= 0.0 || val > 1.0)) return false;
            if (key == "connection_probability" && (val < 0.0 || val > 1.0)) return false;
            return true;
        } else if (std::holds_alternative<float>(newValue)) {
            float val = std::get<float>(newValue);
            if (key == "random_seed" && val < 0.0f) return false;
            return true;
        }
        return true;
    }
    
    /**
     * Set value from string (with type conversion)
     */
    void setFromString(const std::string& key, const std::string& valueStr, const std::string& source) {
        // Try different types
        if (valueStr == "true" || valueStr == "false") {
            bool val = (valueStr == "true");
            set<bool>(key, val, source);
        } else if (valueStr.find('.') != std::string::npos) {
            double val = std::stod(valueStr);
            set<double>(key, val, source);
        } else {
            try {
                int64_t val = std::stoll(valueStr);
                set<int64_t>(key, val, source);
            } catch (...) {
                try {
                    float val = std::stof(valueStr);
                    set<float>(key, val, source);
                } catch (...) {
                    set<std::string>(key, valueStr, source);
                }
            }
        }
    }
    
    /**
     * Get configuration source trace
     */
    std::string getSource(const std::string& key) const {
        auto it = sourceTrace_.find(key);
        return it != sourceTrace_.end() ? it->second : "default";
    }
    
    /**
     * Check if configuration key exists
     */
    bool hasKey(const std::string& key) const {
        return values_.find(key) != values_.end();
    }
    
    /**
     * Get all configuration keys
     */
    std::vector<std::string> getKeys() const {
        std::vector<std::string> keys;
        for (const auto& pair : values_) {
            keys.push_back(pair.first);
        }
        return keys;
    }
    
    /**
     * Export configuration to string
     */
    std::string toString() const {
        std::stringstream ss;
        for (const auto& pair : values_) {
            ss << pair.first << " = ";
            exportValue(pair.second, ss);
            ss << std::endl;
        }
        return ss.str();
    }
    
    /**
     * Export configuration to JSON
     */
    std::string toJSON() const {
        std::stringstream ss;
        ss << "{\n";
        bool first = true;
        for (const auto& pair : values_) {
            if (!first) ss << ",\n";
            first = false;
            ss << "  \"" << pair.first << "\": ";
            exportJSON(pair.second, ss);
        }
        ss << "\n}";
        return ss.str();
    }
    
    /**
     * Export configuration to YAML
     */
    std::string toYAML() const {
        std::stringstream ss;
        for (const auto& pair : values_) {
            ss << pair.first << ": ";
            exportYAML(pair.second, ss);
            ss << "\n";
        }
        return ss.str();
    }
    
private:
    void initializeDefaultParameters() {
        // Core parameters
        parameters_.emplace_back("random_seed", int64_t(42), "Random seed for reproducibility");
        parameters_.emplace_back("simulation_timestep", 0.001, "Simulation timestep in seconds");
        parameters_.emplace_back("neuron_count", int64_t(1000), "Number of neurons in the brain");
        parameters_.emplace_back("region_count", int64_t(2), "Number of brain regions");
        parameters_.emplace_back("connection_probability", 0.1, "Connection probability for random connectivity");
        
        // Plasticity parameters
        parameters_.emplace_back("plasticity_learning_rate", 0.01, "Base learning rate for plasticity");
        parameters_.emplace_back("stdp_ltp_weight", 0.01, "STDP long-term potentiation weight");
        parameters_.emplace_back("stdp_ltd_weight", 0.012, "STDP long-term depression weight");
        parameters_.emplace_back("stdp_time_constant", 20.0, "STDP time constant");
        
        // Development parameters
        parameters_.emplace_back("development_synaptogenesis_rate", 0.001, "Synaptogenesis rate during development");
        parameters_.emplace_back("development_pruning_rate", 0.0001, "Pruning rate during development");
        
        // Neuromodulation parameters
        parameters_.emplace_back("dopamine_baseline", 0.1, "Baseline dopamine level");
        parameters_.emplace_back("dopamine_decay_rate", 0.95, "Dopamine decay rate");
        
        // Reward parameters
        parameters_.emplace_back("reward_discount_factor", 0.99, "Reward discount factor");
        parameters_.emplace_back("reward_decay_rate", 0.98, "Reward decay rate");
        
        // Environment parameters
        parameters_.emplace_back("environment_width", 10, "Width of the environment");
        parameters_.emplace_back("environment_height", 10, "Height of the environment");
        parameters_.emplace_back("vision_width", 16, "Vision sensor width");
        parameters_.emplace_back("vision_height", 16, "Vision sensor height");
        
        // Simulation parameters
        parameters_.emplace_back("max_simulation_steps", 10000, "Maximum simulation steps");
        parameters_.emplace_back("simulation_time_limit", 0.0, "Simulation time limit (0 = no limit)");
        
        // Logging parameters
        parameters_.emplace_back("log_level", std::string("INFO"), "Log level (DEBUG, INFO, WARNING, ERROR)");
        parameters_.emplace_back("log_to_file", false, "Whether to log to file");
        parameters_.emplace_back("log_filename", std::string("nlm.log"), "Log filename");
        
        // Visualization parameters
        parameters_.emplace_back("visualization_enabled", false, "Whether visualization is enabled");
        parameters_.emplace_back("visualization_update_rate", 30.0, "Visualization update rate (Hz)");
    }
    
    void exportValue(const ConfigValue& value, std::stringstream& ss) const {
        if (std::holds_alternative<int64_t>(value)) {
            ss << std::get<int64_t>(value);
        } else if (std::holds_alternative<double>(value)) {
            ss << std::get<double>(value);
        } else if (std::holds_alternative<float>(value)) {
            ss << std::get<float>(value);
        } else if (std::holds_alternative<bool>(value)) {
            ss << (std::get<bool>(value) ? "true" : "false");
        } else if (std::holds_alternative<std::string>(value)) {
            ss << std::get<std::string>(value);
        } else if (std::holds_alternative<std::vector<int64_t>>(value)) {
            ss << "[";
            const auto& vec = std::get<std::vector<int64_t>>(value);
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << vec[i];
            }
            ss << "]";
        } else if (std::holds_alternative<std::vector<double>>(value)) {
            ss << "[";
            const auto& vec = std::get<std::vector<double>>(value);
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << vec[i];
            }
            ss << "]";
        } else if (std::holds_alternative<std::vector<std::string>>(value)) {
            ss << "[";
            const auto& vec = std::get<std::vector<std::string>>(value);
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << "\"" << vec[i] << "\"";
            }
            ss << "]";
        }
    }
    
    void exportJSON(const ConfigValue& value, std::stringstream& ss) const {
        if (std::holds_alternative<int64_t>(value)) {
            ss << std::get<int64_t>(value);
        } else if (std::holds_alternative<double>(value)) {
            ss << std::get<double>(value);
        } else if (std::holds_alternative<float>(value)) {
            ss << std::get<float>(value);
        } else if (std::holds_alternative<bool>(value)) {
            ss << (std::get<bool>(value) ? "true" : "false");
        } else if (std::holds_alternative<std::string>(value)) {
            ss << "\"" << std::get<std::string>(value) << "\"";
        } else if (std::holds_alternative<std::vector<int64_t>>(value)) {
            ss << "[";
            const auto& vec = std::get<std::vector<int64_t>>(value);
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << vec[i];
            }
            ss << "]";
        } else if (std::holds_alternative<std::vector<double>>(value)) {
            ss << "[";
            const auto& vec = std::get<std::vector<double>>(value);
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << vec[i];
            }
            ss << "]";
        } else if (std::holds_alternative<std::vector<std::string>>(value)) {
            ss << "[";
            const auto& vec = std::get<std::vector<std::string>>(value);
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << "\"" << vec[i] << "\"";
            }
            ss << "]";
        }
    }
    
    void exportYAML(const ConfigValue& value, std::stringstream& ss) const {
        if (std::holds_alternative<int64_t>(value)) {
            ss << std::get<int64_t>(value);
        } else if (std::holds_alternative<double>(value)) {
            ss << std::get<double>(value);
        } else if (std::holds_alternative<float>(value)) {
            ss << std::get<float>(value);
        } else if (std::holds_alternative<bool>(value)) {
            ss << (std::get<bool>(value) ? "true" : "false");
        } else if (std::holds_alternative<std::string>(value)) {
            ss << std::get<std::string>(value);
        } else if (std::holds_alternative<std::vector<int64_t>>(value)) {
            ss << "[";
            const auto& vec = std::get<std::vector<int64_t>>(value);
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << vec[i];
            }
            ss << "]";
        } else if (std::holds_alternative<std::vector<double>>(value)) {
            ss << "[";
            const auto& vec = std::get<std::vector<double>>(value);
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << vec[i];
            }
            ss << "]";
        } else if (std::holds_alternative<std::vector<std::string>>(value)) {
            ss << "[";
            const auto& vec = std::get<std::vector<std::string>>(value);
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << "\"" << vec[i] << "\"";
            }
            ss << "]";
        }
    }
};