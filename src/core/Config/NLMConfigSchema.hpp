// Default NLM Configuration Schema Implementation
#ifndef NLM_CONFIG_SCHEMA_HPP
#define NLM_CONFIG_SCHEMA_HPP

#include "ConfigurationSchema.hpp"
#include "ValidationError.hpp"
#include <memory>

namespace nlm {

// Default configuration schema for NLM project
class NLMConfigSchema {
public:
    static std::shared_ptr<ConfigurationSchema> createDefaultSchema() {
        auto schema = std::make_shared<ConfigurationSchema>();
        
        // Random seed configuration
        schema->addParameter(ParameterDefinition(
            "random_seed", 
            ParameterType::Integer, 
            ConfigValue(42),
            "Random seed for reproducible experiments"
        ));
        
        // Simulation configuration
        schema->addParameter(ParameterDefinition(
            "simulation_timestep",
            ParameterType::Float,
            ConfigValue(0.001),
            "Simulation timestep in seconds"
        ));
        
        // Brain configuration
        schema->addParameter(ParameterDefinition(
            "neuron_count",
            ParameterType::Integer,
            ConfigValue(1000),
            "Number of neurons in the brain"
        ));
        
        schema->addParameter(ParameterDefinition(
            "region_count",
            ParameterType::Integer,
            ConfigValue(2),
            "Number of brain regions"
        ));
        
        // Connectivity configuration
        schema->addParameter(ParameterDefinition(
            "connection_probability",
            ParameterType::Float,
            ConfigValue(0.1),
            "Connection probability for random connectivity"
        ));
        
        // Plasticity configuration
        schema->addParameter(ParameterDefinition(
            "plasticity_learning_rate",
            ParameterType::Float,
            ConfigValue(0.01),
            "Plasticity learning rate"
        ));
        
        schema->addParameter(ParameterDefinition(
            "stdp_ltp_weight",
            ParameterType::Float,
            ConfigValue(0.01),
            "STDP LTP weight"
        ));
        
        schema->addParameter(ParameterDefinition(
            "stdp_ltd_weight",
            ParameterType::Float,
            ConfigValue(0.012),
            "STDP LTD weight"
        ));
        
        schema->addParameter(ParameterDefinition(
            "stdp_time_constant",
            ParameterType::Float,
            ConfigValue(20.0),
            "STDP time constant"
        ));
        
        // Development configuration
        schema->addParameter(ParameterDefinition(
            "development_synaptogenesis_rate",
            ParameterType::Float,
            ConfigValue(0.001),
            "Development synaptogenesis rate"
        ));
        
        schema->addParameter(ParameterDefinition(
            "development_pruning_rate",
            ParameterType::Float,
            ConfigValue(0.0001),
            "Development pruning rate"
        ));
        
        // Neuromodulation configuration
        schema->addParameter(ParameterDefinition(
            "dopamine_baseline",
            ParameterType::Float,
            ConfigValue(0.1),
            "Dopamine baseline level"
        ));
        
        // Reward configuration
        schema->addParameter(ParameterDefinition(
            "reward_discount_factor",
            ParameterType::Float,
            ConfigValue(0.99),
            "Reward discount factor"
        ));
        
        // Environment configuration
        schema->addParameter(ParameterDefinition(
            "environment_name",
            ParameterType::String,
            ConfigValue("GridWorld"),
            "Environment name"
        ));
        
        schema->addParameter(ParameterDefinition(
            "environment_width",
            ParameterType::Integer,
            ConfigValue(10),
            "Environment width"
        ));
        
        schema->addParameter(ParameterDefinition(
            "environment_height",
            ParameterType::Integer,
            ConfigValue(10),
            "Environment height"
        ));
        
        // Logging configuration
        schema->addParameter(ParameterDefinition(
            "log_level",
            ParameterType::String,
            ConfigValue("INFO"),
            "Log level (DEBUG, INFO, WARN, ERROR)"
        ));
        
        schema->addParameter(ParameterDefinition(
            "log_to_file",
            ParameterType::Boolean,
            ConfigValue(false),
            "Whether to log to file"
        ));
        
        schema->addParameter(ParameterDefinition(
            "log_filename",
            ParameterType::String,
            ConfigValue("nlm.log"),
            "Log filename"
        ));
        
        // Simulation settings
        schema->addParameter(ParameterDefinition(
            "max_simulation_steps",
            ParameterType::Integer,
            ConfigValue(10000),
            "Maximum simulation steps"
        ));
        
        schema->addParameter(ParameterDefinition(
            "simulation_time_limit",
            ParameterType::Float,
            ConfigValue(0.0),
            "Simulation time limit (0 = unlimited)"
        ));
        
        // Visualization configuration
        schema->addParameter(ParameterDefinition(
            "visualization_enabled",
            ParameterType::Boolean,
            ConfigValue(false),
            "Whether visualization is enabled"
        ));
        
        schema->addParameter(ParameterDefinition(
            "visualization_update_rate",
            ParameterType::Float,
            ConfigValue(30.0),
            "Visualization update rate (Hz)"
        ));
        
        // Configuration parameters with cross-parameter constraints
        // Example: environment dimensions should be positive
        auto widthValidator = std::make_shared<RangeValidationRule>(0.0, 10000.0);
        auto heightValidator = std::make_shared<RangeValidationRule>(0.0, 10000.0);
        
        auto widthParam = schema->getParameter("environment_width").value();
        widthParam.validationRules.push_back(widthValidator);
        schema->addParameter(widthParam);
        
        auto heightParam = schema->getParameter("environment_height").value();
        heightParam.validationRules.push_back(heightValidator);
        schema->addParameter(heightParam);
        
        // Add cross-parameter validation (ensure environment is square for some scenarios)
        auto crossValidator = std::make_shared<CrossParameterValidationRule>(
            "environment_height",
            [](const ConfigValue& height, const ConfigValue& width) {
                if (std::holds_alternative<int>(height) && std::holds_alternative<int>(width)) {
                    return std::get<int>(height) == std::get<int>(width);
                }
                return true;
            }
        );
        
        auto crossParam = schema->getParameter("environment_width").value();
        crossParam.validationRules.push_back(crossValidator);
        schema->addParameter(crossParam);
        
        return schema;
    }
    
    static std::shared_ptr<ConfigurationSchema> createMinimalSchema() {
        auto schema = std::make_shared<ConfigurationSchema>();
        
        // Minimal required parameters
        schema->addParameter(ParameterDefinition(
            "neuron_count",
            ParameterType::Integer,
            ConfigValue(1000),
            "Number of neurons in the brain"
        ));
        
        return schema;
    }
    
    static std::shared_ptr<ConfigurationSchema> createDevelopmentSchema() {
        auto schema = createDefaultSchema();
        
        // Add development-specific parameters
        schema->addParameter(ParameterDefinition(
            "enable_development",
            ParameterType::Boolean,
            ConfigValue(true),
            "Whether to enable developmental processes"
        ));
        
        return schema;
    }
    
    static std::shared_ptr<ConfigurationSchema> createOptimizedSchema() {
        auto schema = createDefaultSchema();
        
        // For optimized performance, disable logging and visualization
        auto logLevelParam = schema->getParameter("log_level").value();
        logLevelParam.defaultValue = ConfigValue("ERROR");
        schema->addParameter(logLevelParam);
        
        auto logToFileParam = schema->getParameter("log_to_file").value();
        logToFileParam.defaultValue = ConfigValue(false);
        schema->addParameter(logToFileParam);
        
        auto visEnabledParam = schema->getParameter("visualization_enabled").value();
        visEnabledParam.defaultValue = ConfigValue(false);
        schema->addParameter(visEnabledParam);
        
        return schema;
    }
};

} // namespace nlm

#endif // NLM_CONFIG_SCHEMA_HPP
