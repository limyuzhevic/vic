// NLM (熙然) - Neural Learning Machine
// Configuration Validation and JSON Parsing
// Enhanced configuration system with validation and JSON support

#pragma once

#include <string>
#include <vector>
#include <map>
#include <variant>
#include <optional>
#include <stdexcept>
#include <sstream>
#include <iostream>

namespace nlm {

// Forward declarations
class Config;

// Configuration validation exceptions
class ConfigValidationError : public std::runtime_error {
public:
    ConfigValidationError(const std::string& message, const std::string& key = "")
        : std::runtime_error("Configuration validation error" + (key.empty() ? "" : " for key: " + key) + ": " + message),
          key_(key) {}
    
    const std::string& getKey() const { return key_; }
    
private:
    std::string key_;
};

class ConfigParseError : public std::runtime_error {
public:
    ConfigParseError(const std::string& message, const std::string& filepath = "")
        : std::runtime_error("Configuration parse error" + (filepath.empty() ? "" : " in file: " + filepath) + ": " + message),
          filepath_(filepath) {}
    
    const std::string& getFilepath() const { return filepath_; }
    
private:
    std::string filepath_;
};

// Configuration schema validation
struct ConfigSchema {
    std::string name;
    std::string description;
    std::map<std::string, std::string> typeDescriptions;
    std::map<std::string, std::string> defaultValues;
    std::map<std::string, std::string> constraints;
    std::vector<std::string> requiredKeys;
    std::map<std::string, std::string> dependencies;
    
    ConfigSchema(const std::string& schemaName, const std::string& schemaDescription)
        : name(schemaName), description(schemaDescription) {}
    
    void addField(const std::string& key, const std::string& type, const std::string& description,
                  const std::string& defaultValue = "") {
        typeDescriptions[key] = type;
        defaultValues[key] = defaultValue;
    }
    
    void addConstraint(const std::string& key, const std::string& constraint) {
        constraints[key] = constraint;
    }
    
    void addDependency(const std::string& key, const std::string& dependsOn) {
        dependencies[key] = dependsOn;
    }
    
    void addRequired(const std::string& key) {
        requiredKeys.push_back(key);
    }
    
    bool validate(const std::map<std::string, std::string>& config) const {
        std::stringstream errors;
        bool valid = true;
        
        // Check required keys
        for (const auto& key : requiredKeys) {
            if (config.find(key) == config.end()) {
                errors << "Missing required key: " << key << std::endl;
                valid = false;
            }
        }
        
        // Check type constraints
        for (const auto& [key, typeDesc] : typeDescriptions) {
            auto it = config.find(key);
            if (it != config.end()) {
                if (!validateType(key, it->second, typeDesc)) {
                    errors << "Invalid type for key '" << key << "': expected " << typeDesc 
                           << ", got '" << it->second << "'" << std::endl;
                    valid = false;
                }
            }
        }
        
        // Check constraints
        for (const auto& [key, constraint] : constraints) {
            auto it = config.find(key);
            if (it != config.end()) {
                if (!validateConstraint(key, it->second, constraint)) {
                    errors << "Constraint violation for key '" << key << "': '" 
                           << it->second << "' does not satisfy constraint: " << constraint << std::endl;
                    valid = false;
                }
            }
        }
        
        if (!valid && !quietValidation) {
            std::cerr << "Configuration validation failed:" << std::endl;
            std::cerr << errors.str();
        }
        
        return valid;
    }
    
    bool validateType(const std::string& key, const std::string& value, const std::string& expectedType) const {
        if (expectedType == "int") {
            try {
                std::stoi(value);
                return true;
            } catch (...) { return false; }
        } else if (expectedType == "float") {
            try {
                std::stof(value);
                return true;
            } catch (...) { return false; }
        } else if (expectedType == "bool") {
            return value == "true" || value == "false" || value == "1" || value == "0";
        } else if (expectedType == "string") {
            return true; // Any string is valid
        } else if (expectedType == "vector") {
            return isVectorFormat(value);
        } else {
            return true; // Unknown type, assume valid
        }
    }
    
    bool validateConstraint(const std::string& key, const std::string& value, const std::string& constraint) const {
        if (constraint.find("range") != std::string::npos) {
            // Parse range like "range(0.0, 1.0)"
            size_t pos1 = constraint.find('(');
            size_t pos2 = constraint.find(',', pos1);
            size_t pos3 = constraint.find(')', pos2);
            
            if (pos1 != std::string::npos && pos2 != std::string::npos && pos3 != std::string::npos) {
                double min = std::stod(constraint.substr(pos1 + 1, pos2 - pos1 - 1));
                double max = std::stod(constraint.substr(pos2 + 1, pos3 - pos2 - 1));
                double val = std::stod(value);
                return val >= min && val <= max;
            }
        } else if (constraint == "positive") {
            try {
                double val = std::stod(value);
                return val > 0.0;
            } catch (...) { return false; }
        } else if (constraint == "nonnegative") {
            try {
                double val = std::stod(value);
                return val >= 0.0;
            } catch (...) { return false; }
        } else if (constraint == "string") {
            return true; // Always valid for strings
        }
        return true; // Unknown constraint, assume valid
    }
    
    bool isVectorFormat(const std::string& value) const {
        // Check if value looks like a vector: "1,2,3" or "[1,2,3]"
        if (value.find('[') == 0 && value.back() == ']') {
            // Remove brackets and check comma separation
            std::string content = value.substr(1, value.size() - 2);
            return !content.empty(); // Basic check - could be enhanced
        } else {
            // Check comma-separated format
            return value.find(',') != std::string::npos;
        }
    }
    
    // Schema registry for predefined schemas
    static ConfigSchema& getPhase2Schema() {
        static ConfigSchema schema("Phase2", "NLM Phase 2 Configuration Schema");
        
        if (schema.typeDescriptions.empty()) {
            schema.addField("random_seed", "int", "Random seed for reproducible experiments");
            schema.addField("simulation_timestep", "float", "Simulation timestep in seconds");
            schema.addField("neuron_count", "int", "Number of neurons in the brain");
            schema.addField("region_count", "int", "Number of brain regions");
            schema.addField("connection_probability", "float", "Connection probability for random connectivity");
            
            schema.addField("stdp_ltp_weight", "float", "STDP Long-Term Potentiation weight");
            schema.addField("stdp_ltd_weight", "float", "STDP Long-Term Depression weight");
            schema.addField("stdp_tau", "float", "STDP time constant");
            
            schema.addField("synaptogenesis_rate", "float", "Structural synaptogenesis rate");
            schema.addField("pruning_rate", "float", "Structural pruning rate");
            
            schema.addField("log_level", "string", "Logging level: TRACE, DEBUG, INFO, WARN, ERROR, FATAL");
            schema.addField("log_to_file", "bool", "Whether to log to file");
            schema.addField("log_filename", "string", "Log file name");
            
            schema.addField("max_simulation_steps", "int", "Maximum simulation steps");
            schema.addField("simulation_time_limit", "float", "Simulation time limit in seconds");
            
            // Add constraints
            schema.addConstraint("simulation_timestep", "range(0.0001, 1.0)");
            schema.addConstraint("neuron_count", "range(10, 100000)");
            schema.addConstraint("region_count", "range(1, 100)");
            schema.addConstraint("connection_probability", "range(0.0, 1.0)");
            schema.addConstraint("stdp_ltp_weight", "range(0.0, 0.5)");
            schema.addConstraint("stdp_ltd_weight", "range(0.0, 0.5)");
            schema.addConstraint("stdp_tau", "range(1.0, 100.0)");
            schema.addConstraint("synaptogenesis_rate", "range(0.0, 0.1)");
            schema.addConstraint("pruning_rate", "range(0.0, 0.1)");
            
            // Add required keys
            schema.addRequired("random_seed");
            schema.addRequired("simulation_timestep");
            schema.addRequired("neuron_count");
            schema.addRequired("region_count");
            schema.addRequired("connection_probability");
        }
        
        return schema;
    }
    
    static ConfigSchema& getPhase1Schema() {
        static ConfigSchema schema("Phase1", "NLM Phase 1 Configuration Schema");
        
        if (schema.typeDescriptions.empty()) {
            schema.addField("random_seed", "int", "Random seed for reproducible experiments");
            schema.addField("simulation_timestep", "float", "Simulation timestep in seconds");
            schema.addField("neuron_count", "int", "Number of neurons in the brain");
            schema.addField("region_count", "int", "Number of brain regions");
            schema.addField("connection_probability", "float", "Connection probability for random connectivity");
            
            schema.addField("plasticity_learning_rate", "float", "Plasticity learning rate");
            schema.addField("stdp_ltp_weight", "float", "STDP Long-Term Potentiation weight");
            schema.addField("stdp_ltd_weight", "float", "STDP Long-Term Depression weight");
            schema.addField("stdp_time_constant", "float", "STDP time constant");
            
            schema.addField("development_synaptogenesis_rate", "float", "Development synaptogenesis rate");
            schema.addField("development_pruning_rate", "float", "Development pruning rate");
            
            schema.addField("dopamine_baseline", "float", "Dopamine baseline value");
            schema.addField("reward_discount_factor", "float", "Reward discount factor");
            
            schema.addField("environment_name", "string", "Environment name");
            schema.addField("environment_width", "int", "Environment width");
            schema.addField("environment_height", "int", "Environment height");
            
            schema.addField("log_level", "string", "Logging level");
            schema.addField("log_to_file", "bool", "Whether to log to file");
            schema.addField("log_filename", "string", "Log file name");
            
            schema.addField("max_simulation_steps", "int", "Maximum simulation steps");
            schema.addField("simulation_time_limit", "float", "Simulation time limit");
            
            schema.addField("visualization_enabled", "bool", "Whether visualization is enabled");
            schema.addField("visualization_update_rate", "float", "Visualization update rate");
            
            // Add constraints
            schema.addConstraint("simulation_timestep", "range(0.0001, 1.0)");
            schema.addConstraint("neuron_count", "range(10, 100000)");
            schema.addConstraint("region_count", "range(1, 100)");
            schema.addConstraint("connection_probability", "range(0.0, 1.0)");
            schema.addConstraint("plasticity_learning_rate", "range(0.0, 1.0)");
            schema.addConstraint("stdp_ltp_weight", "range(0.0, 0.5)");
            schema.addConstraint("stdp_ltd_weight", "range(0.0, 0.5)");
            schema.addConstraint("stdp_time_constant", "range(1.0, 100.0)");
            schema.addConstraint("development_synaptogenesis_rate", "range(0.0, 0.1)");
            schema.addConstraint("development_pruning_rate", "range(0.0, 0.1)");
            schema.addConstraint("dopamine_baseline", "range(-1.0, 10.0)");
            schema.addConstraint("reward_discount_factor", "range(0.0, 1.0)");
            schema.addConstraint("environment_width", "range(1, 1000)");
            schema.addConstraint("environment_height", "range(1, 1000)");
            schema.addConstraint("visualization_update_rate", "range(0.0, 60.0)");
            
            // Add required keys
            schema.addRequired("random_seed");
            schema.addRequired("simulation_timestep");
            schema.addRequired("neuron_count");
            schema.addRequired("region_count");
            schema.addRequired("connection_probability");
        }
        
        return schema;
    }
    
    // Global flag to control validation output
    static bool quietValidation;
};

// JSON configuration parser
class JSONConfigParser {
public:
    static std::map<std::string, std::string> parse(const std::string& jsonContent) {
        std::map<std::string, std::string> config;
        
        // Simple JSON parser - in production, use a proper JSON library like nlohmann::json
        // This is a simplified implementation for demonstration
        
        size_t pos = 0;
        while (pos < jsonContent.size()) {
            // Find next key
            size_t keyStart = jsonContent.find('"', pos);
            if (keyStart == std::string::npos) break;
            
            size_t keyEnd = jsonContent.find('"', keyStart + 1);
            if (keyEnd == std::string::npos) break;
            
            std::string key = jsonContent.substr(keyStart + 1, keyEnd - keyStart - 1);
            pos = keyEnd + 1;
            
            // Skip whitespace
            while (pos < jsonContent.size() && (jsonContent[pos] == ' ' || jsonContent[pos] == '\n' || jsonContent[pos] == '\t')) {
                pos++;
            }
            
            // Skip colon
            if (pos < jsonContent.size() && jsonContent[pos] == ':') {
                pos++;
            }
            
            // Skip whitespace after colon
            while (pos < jsonContent.size() && (jsonContent[pos] == ' ' || jsonContent[pos] == '\n' || jsonContent[pos] == '\t')) {
                pos++;
            }
            
            // Parse value based on first character
            if (pos < jsonContent.size()) {
                if (jsonContent[pos] == '"') {
                    // String value
                    size_t valueStart = pos + 1;
                    size_t valueEnd = jsonContent.find('"', valueStart);
                    if (valueEnd != std::string::npos) {
                        config[key] = jsonContent.substr(valueStart, valueEnd - valueStart);
                        pos = valueEnd + 1;
                    }
                } else if (jsonContent[pos] == '{' || jsonContent[pos] == '[') {
                    // Object or array - convert to string representation
                    size_t bracePos = jsonContent.find_first_of("}", pos);
                    if (bracePos == std::string::npos) {
                        bracePos = jsonContent.find_first_of("]", pos);
                    }
                    if (bracePos != std::string::npos) {
                        config[key] = jsonContent.substr(pos, bracePos - pos + 1);
                        pos = bracePos + 1;
                    }
                } else {
                    // Number or boolean value
                    size_t valueEnd = jsonContent.find_first_of(",}", pos);
                    if (valueEnd != std::string::npos) {
                        config[key] = jsonContent.substr(pos, valueEnd - pos);
                        pos = valueEnd;
                    }
                }
            }
            
            // Skip comma if present
            while (pos < jsonContent.size() && (jsonContent[pos] == ',' || jsonContent[pos] == ' ' || 
                                                jsonContent[pos] == '\n' || jsonContent[pos] == '\t')) {
                pos++;
            }
        }
        
        return config;
    }
    
    static std::string generateDefaultJSON(const ConfigSchema& schema) {
        std::stringstream json;
        json << "{\n";
        
        bool first = true;
        for (const auto& [key, defaultValue] : schema.defaultValues) {
            if (defaultValue.empty()) continue;
            
            if (!first) json << ",\n";
            first = false;
            
            json << "  \"" << key << "\": \"" << defaultValue << "\"";
        }
        
        json << "\n}";
        return json.str();
    }
};

// Configuration validator utilities
class ConfigValidator {
public:
    static bool validateConfig(const std::map<std::string, std::string>& config, const std::string& schemaName = "Phase2") {
        if (schemaName == "Phase2") {
            return ConfigSchema::getPhase2Schema().validate(config);
        } else if (schemaName == "Phase1") {
            return ConfigSchema::getPhase1Schema().validate(config);
        }
        return false;
    }
    
    static std::vector<std::string> getValidationErrors(const std::map<std::string, std::string>& config, 
                                                       const std::string& schemaName = "Phase2") {
        std::vector<std::string> errors;
        
        if (schemaName == "Phase2") {
            const auto& schema = ConfigSchema::getPhase2Schema();
            
            // Check required keys
            for (const auto& key : schema.requiredKeys) {
                if (config.find(key) == config.end()) {
                    errors.push_back("Missing required key: " + key);
                }
            }
            
            // Check type constraints
            for (const auto& [key, typeDesc] : schema.typeDescriptions) {
                auto it = config.find(key);
                if (it != config.end()) {
                    if (!schema.validateType(key, it->second, typeDesc)) {
                        errors.push_back("Invalid type for key '" + key + "': expected " + 
                                       typeDesc + ", got '" + it->second + "'");
                    }
                }
            }
            
            // Check constraints
            for (const auto& [key, constraint] : schema.constraints) {
                auto it = config.find(key);
                if (it != config.end()) {
                    if (!schema.validateConstraint(key, it->second, constraint)) {
                        errors.push_back("Constraint violation for key '" + key + "': '" + 
                                       it->second + "' does not satisfy constraint: " + constraint);
                    }
                }
            }
        }
        
        return errors;
    }
    
    static std::string generateConfigTemplate(const std::string& schemaName = "Phase2") {
        if (schemaName == "Phase2") {
            return JSONConfigParser::generateDefaultJSON(ConfigSchema::getPhase2Schema());
        } else if (schemaName == "Phase1") {
            return JSONConfigParser::generateDefaultJSON(ConfigSchema::getPhase1Schema());
        }
        return "{}";
    }
};

// Global configuration schema registry
class ConfigSchemaRegistry {
public:
    static ConfigSchema& getSchema(const std::string& name) {
        auto it = schemas.find(name);
        if (it != schemas.end()) {
            return *it->second;
        }
        throw std::runtime_error("Schema not found: " + name);
    }
    
    static void registerSchema(const std::string& name, std::unique_ptr<ConfigSchema> schema) {
        schemas[name] = std::move(schema);
    }
    
    static bool hasSchema(const std::string& name) {
        return schemas.find(name) != schemas.end();
    }
    
    static std::vector<std::string> getAvailableSchemas() {
        std::vector<std::string> names;
        for (const auto& pair : schemas) {
            names.push_back(pair.first);
        }
        return names;
    }
    
private:
    static std::map<std::string, std::unique_ptr<ConfigSchema>> schemas;
};

// Global instance for schema registration
inline std::map<std::string, std::unique_ptr<ConfigSchema>> ConfigSchemaRegistry::schemas;

// Configuration validation helper functions
inline bool validateConfigWithSchema(const std::map<std::string, std::string>& config, 
                                   const std::string& schemaName = "Phase2") {
    return ConfigValidator::validateConfig(config, schemaName);
}

inline std::vector<std::string> getConfigValidationErrors(const std::map<std::string, std::string>& config,
                                                         const std::string& schemaName = "Phase2") {
    return ConfigValidator::getValidationErrors(config, schemaName);
}

inline std::string generateConfigTemplate(const std::string& schemaName = "Phase2") {
    return ConfigValidator::generateConfigTemplate(schemaName);
}

} // namespace nlm
