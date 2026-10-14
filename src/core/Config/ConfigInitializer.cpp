#include "ConfigInitializer.hpp"
#include "Config.hpp"
#include <sstream>
#include <unordered_map>
#include <limits>

namespace nlm {

static ConfigSchema globalSchema;
static ConfigValidator globalValidator;

ConfigInitializer::ConfigInitializer() {
    initializeAllParameters();
}

ConfigSchema& ConfigInitializer::getSchema() {
    return globalSchema;
}

ConfigValidator& ConfigInitializer::getValidator() {
    return globalValidator;
}

void ConfigInitializer::initializeAllParameters() {
    // Clear any existing parameters
    globalSchema = ConfigSchema();
    
    // Register all configuration parameters with their validation rules
    
    // Basic Simulation Parameters
    globalSchema.registerParameter(ConfigParameter(
        "random_seed",
        "Random seed for reproducible experiments",
        42,
        createPositiveValidator(),
        std::make_pair(0.0, std::numeric_limits<int64_t>::max()),
        true,
        "Basic Simulation"
    ));
    
    globalSchema.registerParameter(ConfigParameter(
        "simulation_timestep",
        "Simulation timestep in seconds",
        0.001,
        createRangeValidator(0.0001, 0.1),
        std::make_pair(0.0001, 0.1),
        true,
        "Basic Simulation"
    ));
    
    globalSchema.registerParameter(ConfigParameter(
        "neuron_count",
        "Number of neurons in the brain",
        1000LL,
        createRangeValidator(1.0, 100000.0),
        std::make_pair(1.0, 100000.0),
        true,
        "Basic Simulation"
    ));
    
    globalSchema.registerParameter(ConfigParameter(
        "region_count",
        "Number of brain regions",
        2LL,
        createRangeValidator(1.0, 10.0),
        std::make_pair(1.0, 10.0),
        true,
        "Basic Simulation"
    ));
    
    globalSchema.registerParameter(ConfigParameter(
        "connection_probability",
        "Connection probability for random connectivity",
        0.1f,
        createProbabilityValidator(),
        std::make_pair(0.0, 1.0),
        true,
        "Basic Simulation"
    ));
    
    // Plasticity Parameters
    globalSchema.registerParameter(ConfigParameter(
        "plasticity_learning_rate",
        "Plasticity learning rate",
        0.01f,
        createRangeValidator(0.0, 1.0),
        std::make_pair(0.0, 1.0),
        true,
        "Plasticity"
    ));
    
    globalSchema.registerParameter(ConfigParameter(
        "stdp_ltp_weight",
        "STDP long-term potentiation weight",
        0.01f,
        createRangeValidator(0.0, 0.1),
        std::make_pair(0.0, 0.1),
        true,
        "Plasticity"
    ));
    
    globalSchema.registerParameter(ConfigParameter(
        "stdp_ltd_weight",
        "STDP long-term depression weight",
        0.012f,
        createRangeValidator(0.0, 0.1),
        std::make_pair(0.0, 0.1),
        true,
        "Plasticity"
    ));
    
    globalSchema.registerParameter(ConfigParameter(
        "stdp_tau",
        "STDP time constant (tau)",
        20.0f,
        createRangeValidator(1.0, 100.0),
        std::make_pair(1.0, 100.0),
        true,
        "Plasticity"
    ));
    
    // Development Parameters
    globalSchema.registerParameter(ConfigParameter(
        "development_synaptogenesis_rate",
        "Development synaptogenesis rate",
        0.001f,
        createRangeValidator(0.0, 0.1),
        std::make_pair(0.0, 0.1),
        true,
        "Development"
    ));
    
    globalSchema.registerParameter(ConfigParameter(
        "development_pruning_rate",
        "Development pruning rate",
        0.0001f,
        createRangeValidator(0.0, 0.01),
        std::make_pair(0.0, 0.01),
        true,
        "Development"
    ));
    
    // Neuromodulation Parameters
    globalSchema.registerParameter(ConfigParameter(
        "dopamine_baseline",
        "Baseline dopamine level",
        0.1f,
        createRangeValidator(0.0, 1.0),
        std::make_pair(0.0, 1.0),
        true,
        "Neuromodulation"
    ));
    
    // Reward Parameters
    globalSchema.registerParameter(ConfigParameter(
        "reward_discount_factor",
        "Reward discount factor",
        0.99f,
        createRangeValidator(0.0, 1.0),
        std::make_pair(0.0, 1.0),
        true,
        "Reward"
    ));
    
    // Environment Parameters
    globalSchema.registerParameter(ConfigParameter(
        "environment_name",
        "Environment name",
        std::string("GridWorld"),
        createEnvironmentNameValidator(),
        std::nullopt,
        true,
        "Environment"
    ));
    
    globalSchema.registerParameter(ConfigParameter(
        "environment_width",
        "Environment width",
        10,
        createRangeValidator(1.0, 1000.0),
        std::make_pair(1.0, 1000.0),
        true,
        "Environment"
    ));
    
    globalSchema.registerParameter(ConfigParameter(
        "environment_height",
        "Environment height",
        10,
        createRangeValidator(1.0, 1000.0),
        std::make_pair(1.0, 1000.0),
        true,
        "Environment"
    ));
    
    // Logging Parameters
    globalSchema.registerParameter(ConfigParameter(
        "log_level",
        "Log level (DEBUG, INFO, WARNING, ERROR)",
        std::string("INFO"),
        createLogLevelValidator(),
        std::nullopt,
        true,
        "Logging"
    ));
    
    globalSchema.registerParameter(ConfigParameter(
        "log_to_file",
        "Whether to log to file",
        false,
        nullptr,
        std::nullopt,
        true,
        "Logging"
    ));
    
    globalSchema.registerParameter(ConfigParameter(
        "log_filename",
        "Log filename",
        std::string("nlm.log"),
        nullptr,
        std::nullopt,
        true,
        "Logging"
    ));
    
    // Simulation Parameters
    globalSchema.registerParameter(ConfigParameter(
        "max_simulation_steps",
        "Maximum simulation steps",
        10000,
        createPositiveValidator(),
        std::make_pair(1.0, 1000000.0),
        true,
        "Simulation"
    ));
    
    globalSchema.registerParameter(ConfigParameter(
        "simulation_time_limit",
        "Simulation time limit (0 = unlimited)",
        0.0,
        createRangeValidator(0.0, 100000.0),
        std::make_pair(0.0, 100000.0),
        true,
        "Simulation"
    ));
    
    // Visualization Parameters
    globalSchema.registerParameter(ConfigParameter(
        "visualization_enabled",
        "Whether visualization is enabled",
        false,
        nullptr,
        std::nullopt,
        true,
        "Visualization"
    ));
    
    globalSchema.registerParameter(ConfigParameter(
        "visualization_update_rate",
        "Visualization update rate",
        30.0f,
        createRangeValidator(1.0, 1000.0),
        std::make_pair(1.0, 1000.0),
        true,
        "Visualization"
    ));
    
    // Set up the validator
    globalValidator.addSchema("default", globalSchema);
    globalValidator.setStrictMode(false); // Allow warnings for now
}

std::string ConfigInitializer::getParameterDescriptions() {
    return globalSchema.getSchemaDescription();
}

bool ConfigInitializer::validateConfiguration(const Config& config) {
    std::unordered_map<std::string, std::any> configMap;
    
    // Convert config entries to any map for validation
    for (const auto& entry : config.getKeys()) {
        auto value = config.get<std::any>(entry);
        if (value.has_value()) {
            configMap[entry] = value.value();
        }
    }
    
    std::vector<ValidationError> errors = globalValidator.validate(configMap);
    
    return errors.empty();
}

std::vector<ValidationError> ConfigInitializer::getValidationErrors() {
    return globalValidator.getValidationErrors();
}

void ConfigInitializer::reset() {
    globalSchema = ConfigSchema();
    globalValidator = ConfigValidator();
    initializeAllParameters();
}

// Helper function implementations

std::function<bool(const std::any&)> ConfigInitializer::createRangeValidator(double min, double max) {
    return [min, max](const std::any& value) {
        double val = getNumericValue(value);
        return val >= min && val <= max;
    };
}

std::function<bool(const std::any&)> ConfigInitializer::createPositiveValidator() {
    return [](const std::any& value) {
        double val = getNumericValue(value);
        return val > 0.0;
    };
}

std::function<bool(const std::any&)> ConfigInitializer::createNonNegativeValidator() {
    return [](const std::any& value) {
        double val = getNumericValue(value);
        return val >= 0.0;
    };
}

std::function<bool(const std::any&)> ConfigInitializer::createProbabilityValidator() {
    return [](const std::any& value) {
        double val = getNumericValue(value);
        return val >= 0.0 && val <= 1.0;
    };
}

std::function<bool(const std::any&)> ConfigInitializer::createLogLevelValidator() {
    std::vector<std::string> validLevels = {"DEBUG", "INFO", "WARNING", "ERROR"};
    return createEnumValidator(validLevels);
}

std::function<bool(const std::any&)> ConfigInitializer::createEnvironmentNameValidator() {
    std::vector<std::string> validEnvironments = {"GridWorld", "Maze", "OpenField", "Custom"};
    return createEnumValidator(validEnvironments);
}

std::function<bool(const std::any&)> ConfigInitializer::createEnumValidator(const std::vector<std::string>& validValues) {
    return [validValues](const std::any& value) {
        if (value.type() != typeid(std::string)) {
            return false;
        }
        std::string val = std::any_cast<std::string>(value);
        return std::find(validValues.begin(), validValues.end(), val) != validValues.end();
    };
}

double ConfigInitializer::getNumericValue(const std::any& value) {
    if (value.type() == typeid(int)) {
        return static_cast<double>(std::any_cast<int>(value));
    } else if (value.type() == typeid(int64_t)) {
        return static_cast<double>(std::any_cast<int64_t>(value));
    } else if (value.type() == typeid(double)) {
        return std::any_cast<double>(value);
    } else if (value.type() == typeid(float)) {
        return static_cast<double>(std::any_cast<float>(value));
    }
    return 0.0;
}

std::string ConfigInitializer::getTypeName(const std::any& value) {
    if (value.type() == typeid(int)) return "int";
    if (value.type() == typeid(int64_t)) return "int64_t";
    if (value.type() == typeid(double)) return "double";
    if (value.type() == typeid(float)) return "float";
    if (value.type() == typeid(bool)) return "bool";
    if (value.type() == typeid(std::string)) return "string";
    return "unknown";
}

} // namespace nlm