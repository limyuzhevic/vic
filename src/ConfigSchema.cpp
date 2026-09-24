#include "ConfigSchema.hpp"
#include <iostream>

namespace nlm {
    
    // Initialize static members
    std::map<std::string, ConfigParameter> ConfigSchema::parameters_;
    bool ConfigSchema::initialized_ = false;
    
    void ConfigSchema::initializeSchema() {
        if (initialized_) return;
        
        // Core simulation parameters
        parameters_["random_seed"] = ConfigParameter(
            "random_seed", 
            "Random seed for reproducible experiments",
            42, false
        );
        
        parameters_["simulation_timestep"] = ConfigParameter(
            "simulation_timestep",
            "Simulation timestep in seconds",
            0.001, 0.0, 1.0
        );
        
        parameters_["neuron_count"] = ConfigParameter(
            "neuron_count",
            "Number of neurons in the brain",
            500, false
        );
        
        parameters_["region_count"] = ConfigParameter(
            "region_count",
            "Number of brain regions",
            1, false
        );
        
        parameters_["connection_probability"] = ConfigParameter(
            "connection_probability",
            "Connection probability for random connectivity",
            0.15, 0.0, 1.0
        );
        
        // STDP parameters
        parameters_["stdp_ltp_weight"] = ConfigParameter(
            "stdp_ltp_weight",
            "STDP LTP weight",
            0.02, 0.0, 1.0
        );
        
        parameters_["stdp_ltd_weight"] = ConfigParameter(
            "stdp_ltd_weight",
            "STDP LTD weight",
            0.015, 0.0, 1.0
        );
        
        parameters_["stdp_tau"] = ConfigParameter(
            "stdp_tau",
            "STDP time constant in ms",
            20.0, 0.0, 1000.0
        );
        
        // Structural plasticity parameters
        parameters_["synaptogenesis_rate"] = ConfigParameter(
            "synaptogenesis_rate",
            "Synaptogenesis rate",
            0.0001f, 0.0, 1.0
        );
        
        parameters_["pruning_rate"] = ConfigParameter(
            "pruning_rate",
            "Pruning rate",
            0.00001f, 0.0, 1.0
        );
        
        // Log configuration parameters
        parameters_["log_level"] = ConfigParameter(
            "log_level",
            "Log level: debug, info, warning, error, critical",
            std::string("info"),
            std::vector<std::string>{"debug", "info", "warning", "error", "critical"}
        );
        
        parameters_["log_file"] = ConfigParameter(
            "log_file",
            "Log output file path",
            std::string(),
            false
        );
        
        parameters_["verbose"] = ConfigParameter(
            "verbose",
            "Enable verbose output",
            false, false
        );
        
        // Simulation parameters
        parameters_["max_simulation_steps"] = ConfigParameter(
            "max_simulation_steps",
            "Maximum number of simulation steps",
            10000, false
        );
        
        parameters_["simulation_time_limit"] = ConfigParameter(
            "simulation_time_limit",
            "Simulation time limit in seconds (0 = unlimited)",
            0.0, 0.0, 1000000.0
        );
        
        parameters_["environment_name"] = ConfigParameter(
            "environment_name",
            "Environment name",
            std::string("GridWorld"),
            false
        );
        
        initialized_ = true;
    }
    
    ConfigParameter ConfigSchema::getParameter(const std::string& name) {
        if (!initialized_) initializeSchema();
        
        auto it = parameters_.find(name);
        if (it == parameters_.end()) {
            throw ConfigValidationError("Unknown configuration parameter", name);
        }
        return it->second;
    }
    
    void ConfigSchema::validate(const std::string& key, const std::string& value,
                               ConfigSource source) {
        if (!initialized_) initializeSchema();
        
        try {
            ConfigParameter param = getParameter(key);
            
            // Basic validation - just check that key exists
            // In a real implementation, you'd add type checking here
            if (param.typeName == "string" && !param.allowedValues.empty()) {
                bool valid = false;
                for (const auto& allowed : param.allowedValues) {
                    if (value == allowed) {
                        valid = true;
                        break;
                    }
                }
                if (!valid) {
                    throw ConfigValidationError(
                        "Invalid value for parameter '" + key + "'",
                        key, value
                    );
                }
            }
            
        } catch (const ConfigValidationError& e) {
            throw;
        } catch (const std::exception& e) {
            throw ConfigValidationError(
                "Error validating parameter '" + key + "': " + e.what(),
                key, value
            );
        }
    }
    
    std::vector<std::string> ConfigSchema::getAllParameterNames() {
        if (!initialized_) initializeSchema();
        std::vector<std::string> names;
        names.reserve(parameters_.size());
        for (const auto& pair : parameters_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    bool ConfigSchema::hasParameter(const std::string& name) {
        if (!initialized_) initializeSchema();
        return parameters_.find(name) != parameters_.end();
    }
    
    std::string ConfigSchema::getDescription(const std::string& name) {
        try {
            return getParameter(name).description;
        } catch (const ConfigValidationError&) {
            return "Unknown parameter";
        }
    }
    
    std::string ConfigSchema::getDefaultString(const std::string& name) {
        try {
            return getParameter(name).getDefaultString();
        } catch (const ConfigValidationError&) {
            return "";
        }
    }
    
    void ConfigSchema::applyDefaults(Config& config) {
        if (!initialized_) initializeSchema();
        
        for (const auto& pair : parameters_) {
            const auto& param = pair.second;
            if (!config.has(param.name)) {
                ConfigValue configValue;
                if (param.typeName == "int") {
                    configValue = param.getDefaultAs<int>();
                } else if (param.typeName == "int64_t") {
                    configValue = param.getDefaultAs<int64_t>();
                } else if (param.typeName == "double") {
                    configValue = param.getDefaultAs<double>();
                } else if (param.typeName == "bool") {
                    configValue = param.getDefaultAs<bool>();
                } else if (param.typeName == "string") {
                    configValue = param.getDefaultAs<std::string>();
                }
                config.set(param.name, configValue, ConfigSource::Default);
            }
        }
    }
    
} // namespace nlm
