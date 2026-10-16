// NLM Configuration Schema and Validation
// Provides configuration validation, schema definition, and preset management

#pragma once

#include "core/Config/Config.hpp"
#include <string>
#include <vector>
#include <map>
#include <unordered_set>
#include <functional>

namespace nlm {

// Configuration validation errors
enum class ConfigValidationError {
    SUCCESS,
    INVALID_KEY,
    INVALID_TYPE,
    OUT_OF_RANGE,
    MISSING_REQUIRED,
    DUPLICATE_ENTRY,
    INVALID_FORMAT
};

// Configuration schema definition
class ConfigSchema {
public:
    struct Parameter {
        std::string name;
        std::string type;  // "int", "float", "bool", "string", "vector"
        std::string description;
        std::string defaultValue;
        bool required = false;
        std::vector<std::string> allowedValues;
        std::pair<double, double> range = {-1e9, 1e9};
        
        Parameter() : name(), type("string"), description(), defaultValue(), 
                     required(false), range({-1e9, 1e9}) {}
        
        Parameter(const std::string& n, const std::string& t, const std::string& desc,
                 const std::string& def = "", bool req = false)
            : name(n), type(t), description(desc), defaultValue(def), 
              required(req), range({-1e9, 1e9}) {}
    };
    
    // Configuration presets for different modes
    struct Preset {
        std::string name;
        std::string description;
        std::map<std::string, std::string> parameters;
    };
    
private:
    std::map<std::string, Parameter> parameters_;
    std::map<std::string, Preset> presets_;
    std::unordered_set<std::string> allowedTopLevelKeys_;
    std::function<bool(const std::string&, const ConfigValue&)> validator_;
    
public:
    ConfigSchema() {
        initializeDefaultSchema();
    }
    
    void initializeDefaultSchema() {
        // Brain parameters
        parameters_["brain.neuron_count"] = Parameter(
            "brain.neuron_count", "int", 
            "Total number of neurons in the brain",
            "1000", true, {}, {1, 100000});
        
        parameters_["brain.synapse_density"] = Parameter(
            "brain.synapse_density", "float",
            "Synaptic connectivity density (0.0 to 1.0)",
            "0.1", false, {}, {0.0, 1.0});
        
        parameters_["brain.connection_probability"] = Parameter(
            "brain.connection_probability", "float",
            "Probability of connection between neurons (0.0 to 1.0)",
            "0.05", false, {}, {0.0, 1.0});
        
        parameters_["brain.initial_weight_mean"] = Parameter(
            "brain.initial_weight_mean", "float",
            "Mean initial synaptic weight",
            "0.5", false, {}, {0.0, 1.0});
        
        parameters_["brain.initial_weight_std"] = Parameter(
            "brain.initial_weight_std", "float",
            "Standard deviation of initial weights",
            "0.1", false, {}, {0.0, 1.0});
        
        parameters_["brain.v_thresh"] = Parameter(
            "brain.v_thresh", "float",
            "Neuron threshold potential (mV)",
            "-50.0", false, {}, {-100.0, -20.0});
        
        parameters_["brain.v_rest"] = Parameter(
            "brain.v_rest", "float",
            "Resting potential (mV)",
            "-70.0", false, {}, {-90.0, -50.0});
        
        parameters_["brain.v_reset"] = Parameter(
            "brain.v_reset", "float",
            "Reset potential after spike (mV)",
            "-75.0", false, {}, {-80.0, -70.0});
        
        parameters_["brain.tau_mem"] = Parameter(
            "brain.tau_mem", "float",
            "Membrane time constant (ms)",
            "20.0", false, {}, {1.0, 100.0});
        
        parameters_["brain.tau_ref"] = Parameter(
            "brain.tau_ref", "float",
            "Refractory period (ms)",
            "2.0", false, {}, {1.0, 10.0});
        
        // Plasticity parameters
        parameters_["plasticity.stdp.enable"] = Parameter(
            "plasticity.stdp.enable", "bool",
            "Enable Spike-Timing-Dependent Plasticity",
            "true", false);
        
        parameters_["plasticity.stdp.learning_rate"] = Parameter(
            "plasticity.stdp.learning_rate", "float",
            "STDP learning rate",
            "0.001", false, {}, {0.0, 1.0});
        
        parameters_["plasticity.stdp.tau_plus"] = Parameter(
            "plasticity.stdp.tau_plus", "float",
            "STDP time constant for LTP (ms)",
            "20.0", false, {}, {1.0, 100.0});
        
        parameters_["plasticity.stdp.tau_minus"] = Parameter(
            "plasticity.stdp.tau_minus", "float",
            "STDP time constant for LTD (ms)",
            "20.0", false, {}, {1.0, 100.0});
        
        parameters_["plasticity.hebbian.enable"] = Parameter(
            "plasticity.hebbian.enable", "bool",
            "Enable Hebbian learning",
            "true", false);
        
        parameters_["plasticity.structural.enable"] = Parameter(
            "plasticity.structural.enable", "bool",
            "Enable structural plasticity",
            "true", false);
        
        parameters_["plasticity.synaptogenesis_rate"] = Parameter(
            "plasticity.synaptogenesis_rate", "float",
            "Rate of new synapse formation",
            "0.0001", false, {}, {0.0, 0.01});
        
        parameters_["plasticity.pruning_rate"] = Parameter(
            "plasticity.pruning_rate", "float",
            "Rate of synapse pruning",
            "0.00001", false, {}, {0.0, 0.001});
        
        // Neuromodulation parameters
        parameters_["neuromod.dopamine.scale"] = Parameter(
            "neuromod.dopamine.scale", "float",
            "Dopamine modulation scale",
            "1.0", false, {}, {0.0, 10.0});
        
        parameters_["neuromod.curiosity.enable"] = Parameter(
            "neuromod.curiosity.enable", "bool",
            "Enable curiosity-driven exploration",
            "true", false);
        
        parameters_["neuromod.novelty.enable"] = Parameter(
            "neuromod.novelty.enable", "bool",
            "Enable novelty detection",
            "true", false);
        
        // Environment parameters
        parameters_["environment.width"] = Parameter(
            "environment.width", "int",
            "World width in cells",
            "20", false, {}, {1, 1000});
        
        parameters_["environment.height"] = Parameter(
            "environment.height", "int",
            "World height in cells",
            "20", false, {}, {1, 1000});
        
        parameters_["environment.vision_width"] = Parameter(
            "environment.vision_width", "int",
            "Vision width in cells",
            "8", false, {}, {1, 100});
        
        parameters_["environment.vision_height"] = Parameter(
            "environment.vision_height", "int",
            "Vision height in cells",
            "8", false, {}, {1, 100});
        
        // Development parameters
        parameters_["development.enable"] = Parameter(
            "development.enable", "bool",
            "Enable developmental changes over time",
            "true", false);
        
        parameters_["development.initial_plasticity"] = Parameter(
            "development.initial_plasticity", "float",
            "Initial plasticity rate at birth",
            "1.0", false, {}, {0.1, 5.0});
        
        parameters_["development.plasticity_decay_rate"] = Parameter(
            "development.plasticity_decay_rate", "float",
            "Plasticity decay rate per unit time",
            "0.001", false, {}, {0.0, 0.1});
        
        // Experiment parameters
        parameters_["experiment.max_steps"] = Parameter(
            "experiment.max_steps", "int",
            "Maximum simulation steps",
            "1000", false, {}, {1, 1000000});
        
        parameters_["experiment.checkpoint_path"] = Parameter(
            "experiment.checkpoint_path", "string",
            "Path for checkpoint files",
            "/tmp/nlm_checkpoint.bin");
        
        // Phase presets
        presets_["phase1"] = {
            "phase1", "Phase 1: Project skeleton and basic components",
            {
                {"brain.neuron_count", "100"},
                {"brain.synapse_density", "0.05"},
                {"plasticity.stdp.enable", "false"},
                {"plasticity.hebbian.enable", "false"},
                {"development.enable", "false"}
            }
        };
        
        presets_["phase2"] = {
            "phase2", "Phase 2: Real neural computation",
            {
                {"brain.neuron_count", "500"},
                {"brain.synapse_density", "0.1"},
                {"brain.connection_probability", "0.15"},
                {"plasticity.stdp.enable", "true"},
                {"plasticity.stdp.learning_rate", "0.02"},
                {"plasticity.hebbian.enable", "true"},
                {"plasticity.structural.enable", "true"},
                {"development.enable", "false"}
            }
        };
        
        presets_["phase3"] = {
            "phase3", "Phase 3: World interaction",
            {
                {"brain.neuron_count", "500"},
                {"brain.synapse_density", "0.1"},
                {"brain.connection_probability", "0.1"},
                {"environment.width", "20"},
                {"environment.height", "20"},
                {"environment.vision_width", "16"},
                {"environment.vision_height", "16"},
                {"plasticity.stdp.enable", "true"},
                {"plasticity.stdp.learning_rate", "0.01"},
                {"plasticity.hebbian.enable", "true"},
                {"neuromod.dopamine.scale", "1.0"},
                {"neuromod.curiosity.enable", "true"},
                {"development.enable", "false"}
            }
        };
        
        presets_["phase4"] = {
            "phase4", "Phase 4: Emerging cognition",
            {
                {"brain.neuron_count", "1000"},
                {"brain.synapse_density", "0.1"},
                {"brain.connection_probability", "0.1"},
                {"plasticity.stdp.enable", "true"},
                {"plasticity.stdp.learning_rate", "0.01"},
                {"plasticity.hebbian.enable", "true"},
                {"plasticity.structural.enable", "true"},
                {"neuromod.dopamine.scale", "1.0"},
                {"neuromod.curiosity.enable", "true"},
                {"neuromod.novelty.enable", "true"},
                {"development.enable", "true"},
                {"development.initial_plasticity", "1.5"}
            }
        };
        
        presets_["phase5"] = {
            "phase5", "Phase 5: Performance optimizations",
            {
                {"brain.neuron_count", "2000"},
                {"brain.synapse_density", "0.08"},
                {"brain.connection_probability", "0.08"},
                {"plasticity.stdp.enable", "true"},
                {"plasticity.stdp.learning_rate", "0.005"},
                {"plasticity.hebbian.enable", "true"},
                {"plasticity.structural.enable", "true"},
                {"neuromod.dopamine.scale", "1.0"},
                {"neuromod.curiosity.enable", "true"},
                {"neuromod.novelty.enable", "true"},
                {"development.enable", "true"},
                {"development.initial_plasticity", "1.2"}
            }
        };
        
        presets_["phase6"] = {
            "phase6", "Phase 6: Final integration",
            {
                {"brain.neuron_count", "500"},
                {"brain.synapse_density", "0.1"},
                {"brain.connection_probability", "0.15"},
                {"plasticity.stdp.enable", "true"},
                {"plasticity.stdp.learning_rate", "0.01"},
                {"plasticity.hebbian.enable", "true"},
                {"plasticity.structural.enable", "true"},
                {"neuromod.dopamine.scale", "1.0"},
                {"neuromod.curiosity.enable", "true"},
                {"neuromod.novelty.enable", "true"},
                {"development.enable", "true"},
                {"development.initial_plasticity", "1.3"},
                {"experiment.max_steps", "2000"},
                {"experiment.checkpoint_path", "/tmp/nlm_checkpoint.bin"}
            }
        };
        
        // Initialize allowed keys for validation
        allowedTopLevelKeys_ = {
            "brain", "plasticity", "neuromod", "environment", "development", "experiment"
        };
    }
    
    // Add a parameter to the schema
    void addParameter(const Parameter& param) {
        parameters_[param.name] = param;
    }
    
    // Add a preset
    void addPreset(const Preset& preset) {
        presets_[preset.name] = preset;
    }
    
    // Get parameter description
    std::string getParameterDescription(const std::string& name) const {
        auto it = parameters_.find(name);
        if (it != parameters_.end()) {
            return it->second.description + " (type: " + it->second.type + ")";
        }
        return "Unknown parameter";
    }
    
    // Validate configuration
    ConfigValidationError validate(const Config& config) const {
        // Check required parameters
        for (const auto& pair : parameters_) {
            const auto& param = pair.second;
            if (param.required && !config.has(pair.first)) {
                return ConfigValidationError::MISSING_REQUIRED;
            }
        }
        
        // Validate parameter types and ranges
        for (const auto& pair : config.getKeys()) {
            if (parameters_.find(pair) == parameters_.end()) {
                // Skip unknown parameters (they might be added dynamically)
                continue;
            }
            
            const auto& param = parameters_.find(pair)->second;
            
            // Validate type based on parameter type
            bool typeValid = false;
            if (param.type == "int") {
                if (config.get<int>(pair).has_value()) {
                    typeValid = true;
                    // Check range
                    auto val = config.get<int>(pair).value();
                    if (val < param.range.first || val > param.range.second) {
                        return ConfigValidationError::OUT_OF_RANGE;
                    }
                }
            } else if (param.type == "float") {
                if (config.get<double>(pair).has_value()) {
                    typeValid = true;
                    // Check range
                    auto val = config.get<double>(pair).value();
                    if (val < param.range.first || val > param.range.second) {
                        return ConfigValidationError::OUT_OF_RANGE;
                    }
                }
            } else if (param.type == "bool") {
                if (config.get<bool>(pair).has_value()) {
                    typeValid = true;
                }
            } else if (param.type == "string") {
                if (config.get<std::string>(pair).has_value()) {
                    typeValid = true;
                }
            }
            
            if (!typeValid) {
                return ConfigValidationError::INVALID_TYPE;
            }
            
            // Check allowed values if specified
            if (!param.allowedValues.empty()) {
                std::string strValue;
                if (param.type == "string") {
                    strValue = config.get<std::string>(pair).value();
                } else if (param.type == "int") {
                    strValue = std::to_string(config.get<int>(pair).value());
                } else if (param.type == "float") {
                    strValue = std::to_string(config.get<double>(pair).value());
                } else if (param.type == "bool") {
                    strValue = config.get<bool>(pair).value() ? "true" : "false";
                }
                
                bool valueAllowed = false;
                for (const auto& allowed : param.allowedValues) {
                    if (strValue == allowed) {
                        valueAllowed = true;
                        break;
                    }
                }
                
                if (!valueAllowed) {
                    return ConfigValidationError::INVALID_FORMAT;
                }
            }
        }
        
        return ConfigValidationError::SUCCESS;
    }
    
    // Apply preset to configuration
    void applyPreset(Config& config, const std::string& presetName) {
        auto it = presets_.find(presetName);
        if (it == presets_.end()) {
            throw std::runtime_error("Unknown preset: " + presetName);
        }
        
        const auto& preset = it->second;
        
        // Clear existing configuration
        config.clear();
        
        // Apply preset parameters
        for (const auto& pair : preset.parameters) {
            const std::string& key = pair.first;
            const std::string& value = pair.second;
            
            // Convert string value to appropriate type based on parameter schema
            auto paramIt = parameters_.find(key);
            if (paramIt != parameters_.end()) {
                const auto& param = paramIt->second;
                
                if (param.type == "int") {
                    try {
                        config.set(key, std::stoi(value), ConfigSource::File);
                    } catch (...) {
                        config.set(key, std::stoi(value), ConfigSource::File);
                    }
                } else if (param.type == "float") {
                    try {
                        config.set(key, std::stod(value), ConfigSource::File);
                    } catch (...) {
                        config.set(key, std::stod(value), ConfigSource::File);
                    }
                } else if (param.type == "bool") {
                    bool boolValue = (value == "true" || value == "1" || value == "yes");
                    config.set(key, boolValue, ConfigSource::File);
                } else {
                    config.set(key, value, ConfigSource::File);
                }
            } else {
                // Unknown parameter, store as string
                config.set(key, value, ConfigSource::File);
            }
        }
        
        NLM_LOG_INFO("Applied preset: " + presetName);
    }
    
    // Get all preset names
    std::vector<std::string> getPresetNames() const {
        std::vector<std::string> names;
        names.reserve(presets_.size());
        for (const auto& pair : presets_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    // Get preset description
    std::string getPresetDescription(const std::string& presetName) const {
        auto it = presets_.find(presetName);
        if (it != presets_.end()) {
            return it->second.description;
        }
        return "Unknown preset";
    }
    
    // Get all parameter names
    std::vector<std::string> getParameterNames() const {
        std::vector<std::string> names;
        names.reserve(parameters_.size());
        for (const auto& pair : parameters_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    // Generate configuration summary
    std::string generateSummary(const Config& config) const {
        std::ostringstream oss;
        oss << "NLM Configuration Schema\n";
        oss << "======================\n\n";
        
        oss << "Parameters:\n";
        for (const auto& pair : parameters_) {
            const auto& param = pair.second;
            bool hasValue = config.has(pair.first);
            std::string valueStr = hasValue ? "(set)" : "(not set)";
            std::string requiredStr = param.required ? " (required)" : "";
            oss << "  " << pair.first << " - " << param.description << requiredStr << " - " << valueStr << "\n";
        }
        
        oss << "\nPresets:\n";
        for (const auto& pair : presets_) {
            oss << "  " << pair.first << ": " << pair.second.description << "\n";
        }
        
        oss << "\nConfiguration:\n";
        for (const auto& key : config.getKeys()) {
            std::string valueStr;
            if (parameters_.find(key) != parameters_.end()) {
                const auto& param = parameters_.find(key)->second;
                if (param.type == "int") {
                    auto val = config.get<int>(key).value();
                    valueStr = std::to_string(val);
                } else if (param.type == "float") {
                    auto val = config.get<double>(key).value();
                    valueStr = std::to_string(val);
                } else if (param.type == "bool") {
                    auto val = config.get<bool>(key).value();
                    valueStr = val ? "true" : "false";
                } else if (param.type == "string") {
                    auto val = config.get<std::string>(key).value();
                    valueStr = val;
                }
            } else {
                // Unknown parameter, use generic display
                auto val = config.get<std::string>(key);
                if (val.has_value()) {
                    valueStr = val.value();
                } else {
                    valueStr = "<unknown>";
                }
            }
            oss << "  " << key << " = " << valueStr << "\n";
        }
        
        return oss.str();
    }
    
    // Validate configuration path (keys should follow schema)
    bool validateKeyPath(const std::string& key) const {
        // Check if key has a valid top-level category
        size_t dotPos = key.find('.');
        if (dotPos == std::string::npos) {
            return false;
        }
        
        std::string category = key.substr(0, dotPos);
        return allowedTopLevelKeys_.find(category) != allowedTopLevelKeys_.end();
    }
};