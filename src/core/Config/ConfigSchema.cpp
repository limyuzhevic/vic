#pragma once

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct ConfigSchema::Impl {
    std::vector<ConfigField> fields_;
};

ConfigSchema::ConfigSchema() : pImpl(std::make_unique<Impl>()) {}

void ConfigSchema::addField(const std::string& key, const std::string& type,
                            const std::string& description, bool required,
                            const std::string& defaultValue,
                            const std::string& validation) {
    pImpl->fields_.emplace_back(key, type, description, required, defaultValue, validation);
}

const std::vector<ConfigField>& ConfigSchema::getFields() const {
    return pImpl->fields_;
}

const ConfigField* ConfigSchema::getField(const std::string& key) const {
    auto it = std::find_if(pImpl->fields_.begin(), pImpl->fields_.end(),
        [&key](const ConfigField& field) { return field.key == key; });
    return (it != pImpl->fields_.end()) ? &(*it) : nullptr;
}

std::string ConfigSchema::getFieldType(const std::string& key) const {
    const ConfigField* field = getField(key);
    return field ? field->type : "";
}

std::vector<std::string> ConfigSchema::validate(const Config& config) const {
    std::vector<std::string> errors;
    
    for (const auto& field : pImpl->fields_) {
        if (field.required && !config.has(field.key)) {
            errors.push_back("Required field missing: " + field.key);
        }
        
        if (config.has(field.key)) {
            // Type validation
            std::optional<ConfigValue> valueOpt = getConfigValueFromConfig(config, field.key);
            if (valueOpt) {
                const ConfigValue& value = *valueOpt;
                
                // Basic type checking
                bool typeMatches = false;
                if (field.type == "int") {
                    typeMatches = std::holds_alternative<int>(value);
                } else if (field.type == "int64_t") {
                    typeMatches = std::holds_alternative<int64_t>(value);
                } else if (field.type == "double") {
                    typeMatches = std::holds_alternative<double>(value);
                } else if (field.type == "bool") {
                    typeMatches = std::holds_alternative<bool>(value);
                } else if (field.type == "string") {
                    typeMatches = std::holds_alternative<std::string>(value);
                } else if (field.type == "std::vector<int>") {
                    typeMatches = std::holds_alternative<std::vector<int>>(value);
                } else if (field.type == "std::vector<double>") {
                    typeMatches = std::holds_alternative<std::vector<double>>(value);
                } else if (field.type == "std::vector<std::string>") {
                    typeMatches = std::holds_alternative<std::vector<std::string>>(value);
                }
                
                if (!typeMatches) {
                    errors.push_back("Type mismatch for field '" + field.key + "': expected " + field.type + ", got " + configTypeToString(value));
                }
            }
        }
    }
    
    return errors;
}

// Helper function to get ConfigValue from Config
std::optional<ConfigValue> getConfigValueFromConfig(const Config& config, const std::string& key) {
    // This is a helper function - in real implementation, Config would expose this
    // For now, we need to add a method to Config to get ConfigValue directly
    // For this example, we'll skip this and use the existing get<T> method
    return std::nullopt;
}

ConfigSchema createDefaultNLMConfigSchema() {
    ConfigSchema schema;
    
    // Brain configuration fields
    schema.addField("brain.neuron_count", "int", "Total number of neurons", true, "1000", "min=1,max=1000000");
    schema.addField("brain.synapse_density", "double", "Synapse connectivity density", false, "0.1", "min=0.0,max=1.0");
    schema.addField("brain.connection_probability", "double", "Probability of connection between neurons", false, "0.05", "min=0.0,max=1.0");
    schema.addField("brain.initial_weight_mean", "double", "Mean initial synaptic weight", false, "0.5", "min=0.0,max=1.0");
    schema.addField("brain.initial_weight_std", "double", "Standard deviation of initial weights", false, "0.1", "min=0.0,max=1.0");
    schema.addField("brain.v_thresh", "double", "Neuron threshold potential (mV)", false, "-50.0", "range=-100,-20");
    schema.addField("brain.v_rest", "double", "Resting potential (mV)", false, "-70.0", "range=-100,-20");
    schema.addField("brain.v_reset", "double", "Reset potential after spike (mV)", false, "-75.0", "range=-100,-20");
    schema.addField("brain.tau_mem", "double", "Membrane time constant (ms)", false, "20.0", "min=1.0,max=100.0");
    schema.addField("brain.tau_ref", "double", "Refractory period (ms)", false, "2.0", "min=0.1,max=10.0");
    
    // Plasticity configuration
    schema.addField("plasticity.stdp.enable", "bool", "Enable STDP", false, "true");
    schema.addField("plasticity.stdp.learning_rate", "double", "STDP learning rate", false, "0.001", "min=0.0,max=0.1");
    schema.addField("plasticity.stdp.tau_plus", "double", "STDP time constant (ms)", false, "20.0", "min=1.0,max=100.0");
    schema.addField("plasticity.stdp.tau_minus", "double", "STDP time constant (ms)", false, "20.0", "min=1.0,max=100.0");
    schema.addField("plasticity.hebbian.enable", "bool", "Enable Hebbian learning", false, "true");
    schema.addField("plasticity.structural.enable", "bool", "Enable structural plasticity", false, "true");
    
    // Neuromodulation configuration
    schema.addField("neuromod.dopamine.scale", "double", "Dopamine modulation scale", false, "1.0", "min=0.0,max=2.0");
    schema.addField("neuromod.curiosity.enable", "bool", "Enable curiosity-driven exploration", false, "true");
    schema.addField("neuromod.novelty.enable", "bool", "Enable novelty detection", false, "true");
    
    // Simulation configuration
    schema.addField("simulation.timestep", "double", "Simulation timestep (s)", false, "0.001", "min=0.0001,max=0.1");
    schema.addField("simulation.random_seed", "int64_t", "Random seed for reproducibility", false, "42");
    schema.addField("simulation.max_steps", "int64_t", "Maximum simulation steps", false, "10000", "min=0");
    
    // Development configuration
    schema.addField("development.critical_period_length", "double", "Length of critical period (s)", false, "100.0", "min=0.0,max=1000.0");
    schema.addField("development.plasticity_decay_rate", "double", "Rate of plasticity decay over time", false, "0.001", "min=0.0,max=0.01");
    
    // Memory configuration
    schema.addField("memory.working_capacity", "int", "Working memory capacity (number of traces)", false, "100", "min=0");
    schema.addField("memory.episodic_max_episodes", "int", "Maximum number of episodic memory episodes", false, "1000", "min=0");
    
    // Output configuration
    schema.addField("output.log_level", "string", "Log level (Debug, Info, Warning, Error)", false, "Info", "values=Debug,Info,Warning,Error");
    schema.addField("output.checkpoint_interval", "int64_t", "Checkpoint save interval (steps)", false, "1000", "min=0");
    
    return schema;
}

} // namespace nlm
