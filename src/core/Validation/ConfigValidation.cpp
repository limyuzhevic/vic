// Implementation of configuration validation framework

#include "ConfigValidation.hpp"
#include "Config.hpp"
#include <fstream>
#include <filesystem>
#include <cmath>
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace nlm {

void ConfigSchema::addParameter(const ConfigParameter& param) {
    // Check for duplicate keys
    if (keyIndex_.find(param.key) != keyIndex_.end()) {
        throw std::runtime_error("Duplicate configuration key: " + param.key);
    }
    
    parameters_.push_back(param);
    keyIndex_[param.key] = parameters_.size() - 1;
}

bool ConfigSchema::isValid(const std::string& key) const {
    return keyIndex_.find(key) != keyIndex_.end();
}

bool ConfigSchema::isValid(const Config& config) const {
    return validate(config).empty();
}

std::vector<ValidationError> ConfigSchema::validate(const Config& config) const {
    std::vector<ValidationError> errors;
    
    for (const auto& param : parameters_) {
        if (param.required && !config.has(param.key)) {
            errors.emplace_back(ValidationErrorType::MissingRequired, param.key,
                              "Required configuration parameter is missing",
                              "Please add " + param.key + " to your configuration file");
        } else if (config.has(param.key)) {
            auto valueOpt = config.get<std::string>(param.key);
            if (valueOpt) {
                ConfigValue value;
                // Convert string value to appropriate type for validation
                // This is a simplified conversion - in practice, we'd need
                // to handle the actual type stored in the config
                std::vector<ValidationError> valErrors = validate(param.key, value);
                errors.insert(errors.end(), valErrors.begin(), valErrors.end());
            }
        }
    }
    
    return errors;
}

std::vector<ValidationError> ConfigSchema::validate(const std::string& key, 
                                                    const ConfigValue& value) const {
    std::vector<ValidationError> errors;
    
    auto paramIt = keyIndex_.find(key);
    if (paramIt == keyIndex_.end()) {
        errors.emplace_back(ValidationErrorType::InvalidType, key,
                          "Unknown configuration key",
                          "Use one of: " + getKeyList());
        return errors;
    }
    
    size_t paramIndex = paramIt->second;
    const ConfigParameter& param = parameters_[paramIndex];
    
    // Validate type
    if (!validateType(value, param.type)) {
        errors.emplace_back(ValidationErrorType::InvalidType, key,
                          "Invalid type for parameter " + key + ". " +
                          getParameterDescription(key) + " expects " +
                          typeToString(param.type) + ".",
                          "Ensure the value is of the correct type");
    }
    
    // Validate range (if applicable)
    if (param.range.has_value() && !validateRange(value, param.range.value())) {
        std::ostringstream oss;
        oss << "Value " << valueToString(value) << " is outside valid range. "
            << "Valid range is [" << param.range->first << ", " 
            << param.range->second << "]";
        
        errors.emplace_back(ValidationErrorType::OutOfRange, key,
                          oss.str(),
                          "Adjust the value to be within the specified range");
    }
    
    // Validate size (if applicable for list types)
    if (param.minSize.has_value() || param.maxSize.has_value()) {
        bool sizeValid = true;
        if (param.minSize.has_value()) {
            size_t minSize = param.minSize.value();
            if (!valueSize(value) || valueSize(value) < minSize) {
                sizeValid = false;
            }
        }
        if (param.maxSize.has_value() && sizeValid) {
            size_t maxSize = param.maxSize.value();
            if (!valueSize(value) || valueSize(value) > maxSize) {
                sizeValid = false;
            }
        }
        
        if (!sizeValid) {
            std::ostringstream oss;
            oss << "Value size is invalid. ";
            if (param.minSize.has_value()) {
                oss << "Minimum size is " << param.minSize->first << ". ";
            }
            if (param.maxSize.has_value()) {
                oss << "Maximum size is " << param.maxSize->first << ". ";
            }
            
            errors.emplace_back(ValidationErrorType::OutOfRange, key,
                              oss.str(),
                              "Adjust the value size to be within the specified bounds");
        }
    }
    
    // Apply custom validator if provided
    if (param.validator.has_value() && !param.validator.value()(value)) {
        errors.emplace_back(ValidationErrorType::InvalidFormat, key,
                          "Custom validation failed for parameter " + key,
                          "Check the format or content of the parameter value");
    }
    
    return errors;
}

std::string ConfigSchema::getParameterDescription(const std::string& key) const {
    auto paramIt = keyIndex_.find(key);
    if (paramIt == keyIndex_.end()) {
        return "Unknown parameter";
    }
    
    return parameters_[paramIt->second].description;
}

std::vector<std::string> ConfigSchema::getAllKeys() const {
    std::vector<std::string> keys;
    keys.reserve(parameters_.size());
    for (const auto& param : parameters_) {
        keys.push_back(param.key);
    }
    return keys;
}

std::vector<ConfigParameter> ConfigSchema::getParameters() const {
    return parameters_;
}

const ConfigParameter* ConfigSchema::getParameter(const std::string& key) const {
    auto paramIt = keyIndex_.find(key);
    if (paramIt == keyIndex_.end()) {
        return nullptr;
    }
    return &parameters_[paramIt->second];
}

ConfigSchema ConfigSchema::createStandardSchema() {
    ConfigSchema schema;
    
    // Brain parameters
    schema.addParameter(ConfigParameter(
        "brain.neuron_count", ConfigParameterType::Integer,
        1000, "Total number of neurons in the brain"
    ));
    
    schema.addParameter(ConfigParameter(
        "brain.synapse_density", ConfigParameterType::Double,
        0.1, "Synapse connectivity density (0.0 to 1.0)"
    ));
    
    schema.addParameter(ConfigParameter(
        "brain.connection_probability", ConfigParameterType::Double,
        0.05, "Probability of connection between neurons (0.0 to 1.0)"
    ));
    
    schema.addParameter(ConfigParameter(
        "brain.initial_weight_mean", ConfigParameterType::Double,
        0.5, "Mean initial synaptic weight (-1.0 to 1.0)"
    ));
    
    schema.addParameter(ConfigParameter(
        "brain.initial_weight_std", ConfigParameterType::Double,
        0.1, "Standard deviation of initial weights (0.0 to 1.0)"
    ));
    
    schema.addParameter(ConfigParameter(
        "brain.v_thresh", ConfigParameterType::Double,
        -50.0, "Neuron threshold potential (mV)"
    ));
    
    schema.addParameter(ConfigParameter(
        "brain.v_rest", ConfigParameterType::Double,
        -70.0, "Resting potential (mV)"
    ));
    
    schema.addParameter(ConfigParameter(
        "brain.v_reset", ConfigParameterType::Double,
        -75.0, "Reset potential after spike (mV)"
    ));
    
    schema.addParameter(ConfigParameter(
        "brain.tau_mem", ConfigParameterType::Double,
        20.0, "Membrane time constant (ms)"
    ));
    
    schema.addParameter(ConfigParameter(
        "brain.tau_ref", ConfigParameterType::Double,
        2.0, "Refractory period (ms)"
    ));
    
    // Plasticity parameters
    schema.addParameter(ConfigParameter(
        "plasticity.stdp.enable", ConfigParameterType::Boolean,
        true, "Enable spike-timing dependent plasticity (STDP)"
    ));
    
    schema.addParameter(ConfigParameter(
        "plasticity.stdp.learning_rate", ConfigParameterType::Double,
        0.001, "STDP learning rate (0.0 to 0.1)"
    ));
    
    schema.addParameter(ConfigParameter(
        "plasticity.stdp.tau_plus", ConfigParameterType::Double,
        20.0, "STDP time constant for LTP (ms)"
    ));
    
    schema.addParameter(ConfigParameter(
        "plasticity.stdp.tau_minus", ConfigParameterType::Double,
        20.0, "STDP time constant for LTD (ms)"
    ));
    
    schema.addParameter(ConfigParameter(
        "plasticity.hebbian.enable", ConfigParameterType::Boolean,
        true, "Enable Hebbian learning"
    ));
    
    schema.addParameter(ConfigParameter(
        "plasticity.hebbian.learning_rate", ConfigParameterType::Double,
        0.01, "Hebbian learning rate (0.0 to 0.1)"
    ));
    
    schema.addParameter(ConfigParameter(
        "plasticity.structural.enable", ConfigParameterType::Boolean,
        true, "Enable structural plasticity (synaptogenesis/pruning)"
    ));
    
    schema.addParameter(ConfigParameter(
        "plasticity.structural.synaptogenesis_rate", ConfigParameterType::Double,
        0.001, "Rate of new synapse formation (0.0 to 0.01)"
    ));
    
    schema.addParameter(ConfigParameter(
        "plasticity.structural.pruning_rate", ConfigParameterType::Double,
        0.0001, "Rate of synapse pruning (0.0 to 0.001)"
    ));
    
    // Neuromodulation parameters
    schema.addParameter(ConfigParameter(
        "neuromod.dopamine.scale", ConfigParameterType::Double,
        1.0, "Dopamine modulation scale (0.0 to 2.0)"
    ));
    
    schema.addParameter(ConfigParameter(
        "neuromod.curiosity.enable", ConfigParameterType::Boolean,
        true, "Enable curiosity-driven exploration"
    ));
    
    schema.addParameter(ConfigParameter(
        "neuromod.novelty.enable", ConfigParameterType::Boolean,
        true, "Enable novelty detection"
    ));
    
    // Development parameters
    schema.addParameter(ConfigParameter(
        "development.synaptogenesis_rate", ConfigParameterType::Double,
        0.001, "Developmental synaptogenesis rate (0.0 to 0.01)"
    ));
    
    schema.addParameter(ConfigParameter(
        "development.pruning_rate", ConfigParameterType::Double,
        0.0001, "Developmental pruning rate (0.0 to 0.001)"
    ));
    
    // Reward parameters
    schema.addParameter(ConfigParameter(
        "reward.discount_factor", ConfigParameterType::Double,
        0.99, "Reward discount factor (0.0 to 1.0)"
    ));
    
    // Environment parameters
    schema.addParameter(ConfigParameter(
        "environment.name", ConfigParameterType::String,
        "GridWorld", "Environment name"
    ));
    
    schema.addParameter(ConfigParameter(
        "environment.width", ConfigParameterType::Integer,
        10, "Environment width (cells)"
    ));
    
    schema.addParameter(ConfigParameter(
        "environment.height", ConfigParameterType::Integer,
        10, "Environment height (cells)"
    ));
    
    // Logging parameters
    schema.addParameter(ConfigParameter(
        "logging.level", ConfigParameterType::String,
        "INFO", "Logging level (DEBUG, INFO, WARNING, ERROR)"
    ));
    
    schema.addParameter(ConfigParameter(
        "logging.to_file", ConfigParameterType::Boolean,
        false, "Log to file"
    ));
    
    schema.addParameter(ConfigParameter(
        "logging.filename", ConfigParameterType::String,
        "nlm.log", "Log filename"
    ));
    
    // Simulation parameters
    schema.addParameter(ConfigParameter(
        "simulation.timestep", ConfigParameterType::Double,
        0.001, "Simulation timestep (s)"
    ));
    
    schema.addParameter(ConfigParameter(
        "simulation.max_steps", ConfigParameterType::Integer,
        10000, "Maximum simulation steps"
    ));
    
    schema.addParameter(ConfigParameter(
        "simulation.time_limit", ConfigParameterType::Double,
        0.0, "Simulation time limit (s, 0 = unlimited)"
    ));
    
    return schema;
}

bool ConfigSchema::validateType(const ConfigValue& value, ConfigParameterType type) {
    return std::visit([](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int> && type == ConfigParameterType::Integer) return true;
        else if constexpr (std::is_same_v<T, int64_t> && type == ConfigParameterType::Integer64) return true;
        else if constexpr (std::is_same_v<T, double> && type == ConfigParameterType::Double) return true;
        else if constexpr (std::is_same_v<T, bool> && type == ConfigParameterType::Boolean) return true;
        else if constexpr (std::is_same_v<T, std::string> && type == ConfigParameterType::String) return true;
        else if constexpr (std::is_same_v<T, std::vector<int>> && type == ConfigParameterType::IntList) return true;
        else if constexpr (std::is_same_v<T, std::vector<double>> && type == ConfigParameterType::DoubleList) return true;
        else if constexpr (std::is_same_v<T, std::vector<std::string>> && type == ConfigParameterType::StringList) return true;
        else return false;
    }, value);
}

bool ConfigSchema::validateRange(const ConfigValue& value, const std::pair<double, double>& range) {
    return std::visit([range](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_arithmetic_v<T>) {
            return arg >= range.first && arg <= range.second;
        } else {
            return false; // Non-numeric types cannot be validated against range
        }
    }, value);
}

bool ConfigSchema::validateSize(const ConfigValue& value, size_t minSize, size_t maxSize) {
    return std::visit([minSize, maxSize](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::vector<int>> ||
                     std::is_same_v<T, std::vector<double>> ||
                     std::is_same_v<T, std::vector<std::string>>) {
            return arg.size() >= minSize && arg.size() <= maxSize;
        } else {
            return false; // Non-list types cannot be validated for size
        }
    }, value);
}

std::string ConfigSchema::errorMessage(ValidationErrorType type, const std::string& key,
                                       const std::string& value, const ConfigParameter& param) {
    switch (type) {
        case ValidationErrorType::InvalidType:
            return "Invalid type for parameter '" + key + "'. " + value +
                   " is not compatible with parameter type " + typeToString(param.type);
        case ValidationErrorType::OutOfRange:
            return "Value " + value + " for parameter '" + key + "' is outside valid range";
        case ValidationErrorType::MissingRequired:
            return "Required parameter '" + key + "' is missing";
        case ValidationErrorType::DuplicateKey:
            return "Duplicate configuration key '" + key + "'";
        case ValidationErrorType::InvalidFormat:
            return "Invalid format for parameter '" + key + "'";
        default:
            return "Unknown validation error for parameter '" + key + "'";
    }
}

std::string ConfigSchema::getKeyList() const {
    std::ostringstream oss;
    bool first = true;
    for (const auto& key : getAllKeys()) {
        if (!first) oss << ", ";
        oss << key;
        first = false;
    }
    return oss.str();
}

std::string ConfigSchema::typeToString(ConfigParameterType type) {
    switch (type) {
        case ConfigParameterType::Integer: return "integer";
        case ConfigParameterType::Integer64: return "int64_t";
        case ConfigParameterType::Double: return "double";
        case ConfigParameterType::Boolean: return "boolean";
        case ConfigParameterType::String: return "string";
        case ConfigParameterType::StringList: return "string list";
        case ConfigParameterType::DoubleList: return "double list";
        case ConfigParameterType::IntList: return "int list";
        default: return "unknown";
    }
}

std::string ConfigSchema::valueToString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

size_t ConfigSchema::valueSize(const ConfigValue& value) {
    return std::visit([](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::vector<int>> ||
                     std::is_same_v<T, std::vector<double>> ||
                     std::is_same_v<T, std::vector<std::string>>) {
            return arg.size();
        } else {
            return size_t(0); // Non-list types have size 0
        }
    }, value);
}

std::vector<ValidationError> ConfigValidator::validate(const Config& config) const {
    errors_.clear();
    
    for (const auto& entry : config.getKeys()) {
        auto valueOpt = config.get<std::string>(entry);
        if (valueOpt) {
            ConfigValue value;
            // In a real implementation, we would convert the string back to the actual type
            // For now, we'll just do basic validation
            std::vector<ValidationError> valErrors = validate(entry, value);
            errors_.insert(errors_.end(), valErrors.begin(), valErrors.end());
        }
    }
    
    return errors_;
}

std::vector<ValidationError> ConfigValidator::validate(const std::string& key, 
                                                       const ConfigValue& value) const {
    std::vector<ValidationError> errors;
    
    addError(ValidationErrorType::InvalidType, key,
             "Placeholder validation - full implementation needed", "");
    
    return errors;
}

std::vector<std::string> ConfigValidator::getSuggestions() const {
    std::vector<std::string> suggestions;
    
    for (const auto& error : errors_) {
        if (error.type == ValidationErrorType::MissingRequired) {
            suggestions.push_back("Add the missing required parameter: " + error.key);
        } else if (error.type == ValidationErrorType::OutOfRange) {
            suggestions.push_back("Adjust the value for parameter " + error.key + " to be within the valid range");
        } else if (error.type == ValidationErrorType::InvalidType) {
            suggestions.push_back("Fix the type of parameter " + error.key + ". Check the documentation for the expected type");
        } else if (error.type == ValidationErrorType::InvalidFormat) {
            suggestions.push_back("Fix the format of parameter " + error.key + ". Check the documentation for examples");
        }
    }
    
    return suggestions;
}

void ConfigValidator::addError(ValidationErrorType type, const std::string& key,
                               const std::string& message, const std::string& suggestion) const {
    errors_.emplace_back(type, key, message, suggestion);
}

bool ConfigValidationHelpers::validateBrainParameters(const Config& config, 
                                                       std::vector<ValidationError>& errors) {
    bool isValid = true;
    
    // Validate brain.neuron_count
    if (config.has("brain.neuron_count")) {
        auto count = config.get<int64_t>("brain.neuron_count");
        if (count && (*count <= 0 || *count > 100000)) {
            errors.emplace_back(ValidationErrorType::OutOfRange, "brain.neuron_count",
                              "Neuron count must be between 1 and 100000", "Set brain.neuron_count to a reasonable value");
            isValid = false;
        }
    }
    
    // Validate brain.synapse_density
    if (config.has("brain.synapse_density")) {
        auto density = config.get<double>("brain.synapse_density");
        if (density && (*density < 0.0 || *density > 1.0)) {
            errors.emplace_back(ValidationErrorType::OutOfRange, "brain.synapse_density",
                              "Synapse density must be between 0.0 and 1.0", "Set brain.synapse_density to a value between 0.0 and 1.0");
            isValid = false;
        }
    }
    
    // Validate brain.connection_probability
    if (config.has("brain.connection_probability")) {
        auto prob = config.get<double>("brain.connection_probability");
        if (prob && (*prob < 0.0 || *prob > 1.0)) {
            errors.emplace_back(ValidationErrorType::OutOfRange, "brain.connection_probability",
                              "Connection probability must be between 0.0 and 1.0", "Set brain.connection_probability to a value between 0.0 and 1.0");
            isValid = false;
        }
    }
    
    return isValid;
}

bool ConfigValidationHelpers::validatePlasticityParameters(const Config& config, 
                                                            std::vector<ValidationError>& errors) {
    bool isValid = true;
    
    // Validate plasticity.stdp.learning_rate
    if (config.has("plasticity.stdp.learning_rate")) {
        auto rate = config.get<double>("plasticity.stdp.learning_rate");
        if (rate && (*rate < 0.0 || *rate > 0.1)) {
            errors.emplace_back(ValidationErrorType::OutOfRange, "plasticity.stdp.learning_rate",
                              "STDP learning rate must be between 0.0 and 0.1", "Set plasticity.stdp.learning_rate to a value between 0.0 and 0.1");
            isValid = false;
        }
    }
    
    // Validate plasticity.hebbian.learning_rate
    if (config.has("plasticity.hebbian.learning_rate")) {
        auto rate = config.get<double>("plasticity.hebbian.learning_rate");
        if (rate && (*rate < 0.0 || *rate > 0.1)) {
            errors.emplace_back(ValidationErrorType::OutOfRange, "plasticity.hebbian.learning_rate",
                              "Hebbian learning rate must be between 0.0 and 0.1", "Set plasticity.hebbian.learning_rate to a value between 0.0 and 0.1");
            isValid = false;
        }
    }
    
    return isValid;
}

bool ConfigValidationHelpers::validateNeuromodulationParameters(const Config& config, 
                                                                   std::vector<ValidationError>& errors) {
    bool isValid = true;
    
    // Validate neuromod.dopamine.scale
    if (config.has("neuromod.dopamine.scale")) {
        auto scale = config.get<double>("neuromod.dopamine.scale");
        if (scale && (*scale < 0.0 || *scale > 2.0)) {
            errors.emplace_back(ValidationErrorType::OutOfRange, "neuromod.dopamine.scale",
                              "Dopamine scale must be between 0.0 and 2.0", "Set neuromod.dopamine.scale to a value between 0.0 and 2.0");
            isValid = false;
        }
    }
    
    return isValid;
}

bool ConfigValidationHelpers::validateDevelopmentParameters(const Config& config, 
                                                              std::vector<ValidationError>& errors) {
    bool isValid = true;
    
    // Validate development.synaptogenesis_rate
    if (config.has("development.synaptogenesis_rate")) {
        auto rate = config.get<double>("development.synaptogenesis_rate");
        if (rate && (*rate < 0.0 || *rate > 0.01)) {
            errors.emplace_back(ValidationErrorType::OutOfRange, "development.synaptogenesis_rate",
                              "Synaptogenesis rate must be between 0.0 and 0.01", "Set development.synaptogenesis_rate to a value between 0.0 and 0.01");
            isValid = false;
        }
    }
    
    // Validate development.pruning_rate
    if (config.has("development.pruning_rate")) {
        auto rate = config.get<double>("development.pruning_rate");
        if (rate && (*rate < 0.0 || *rate > 0.001)) {
            errors.emplace_back(ValidationErrorType::OutOfRange, "development.pruning_rate",
                              "Pruning rate must be between 0.0 and 0.001", "Set development.pruning_rate to a value between 0.0 and 0.001");
            isValid = false;
        }
    }
    
    return isValid;
}

bool ConfigValidationHelpers::validateWorldParameters(const Config& config, 
                                                       std::vector<ValidationError>& errors) {
    bool isValid = true;
    
    // Validate environment.width
    if (config.has("environment.width")) {
        auto width = config.get<int64_t>("environment.width");
        if (width && (*width <= 0 || *width > 1000)) {
            errors.emplace_back(ValidationErrorType::OutOfRange, "environment.width",
                              "Environment width must be between 1 and 1000", "Set environment.width to a reasonable value");
            isValid = false;
        }
    }
    
    // Validate environment.height
    if (config.has("environment.height")) {
        auto height = config.get<int64_t>("environment.height");
        if (height && (*height <= 0 || *height > 1000)) {
            errors.emplace_back(ValidationErrorType::OutOfRange, "environment.height",
                              "Environment height must be between 1 and 1000", "Set environment.height to a reasonable value");
            isValid = false;
        }
    }
    
    return isValid;
}

bool ConfigValidationHelpers::validateSimulationParameters(const Config& config, 
                                                            std::vector<ValidationError>& errors) {
    bool isValid = true;
    
    // Validate simulation.timestep
    if (config.has("simulation.timestep")) {
        auto timestep = config.get<double>("simulation.timestep");
        if (timestep && (*timestep <= 0.0 || *timestep > 1.0)) {
            errors.emplace_back(ValidationErrorType::OutOfRange, "simulation.timestep",
                              "Simulation timestep must be between 0.0 and 1.0", "Set simulation.timestep to a value between 0.0 and 1.0");
            isValid = false;
        }
    }
    
    // Validate simulation.max_steps
    if (config.has("simulation.max_steps")) {
        auto maxSteps = config.get<int64_t>("simulation.max_steps");
        if (maxSteps && (*maxSteps <= 0 || *maxSteps > 1000000)) {
            errors.emplace_back(ValidationErrorType::OutOfRange, "simulation.max_steps",
                              "Maximum steps must be between 1 and 1000000", "Set simulation.max_steps to a reasonable value");
            isValid = false;
        }
    }
    
    return isValid;
}

bool ConfigValidationHelpers::validateAllParameters(const Config& config, 
                                                     std::vector<ValidationError>& errors) {
    bool isValid = true;
    
    isValid &= validateBrainParameters(config, errors);
    isValid &= validatePlasticityParameters(config, errors);
    isValid &= validateNeuromodulationParameters(config, errors);
    isValid &= validateDevelopmentParameters(config, errors);
    isValid &= validateWorldParameters(config, errors);
    isValid &= validateSimulationParameters(config, errors);
    
    return isValid;
}

std::string ConfigValidationHelpers::getParameterInfo(const std::string& key) {
    // Return parameter information based on key
    if (key == "brain.neuron_count") {
        return "Total number of neurons (1-100000)";
    } else if (key == "brain.synapse_density") {
        return "Synapse connectivity density (0.0-1.0)";
    } else if (key == "brain.connection_probability") {
        return "Connection probability (0.0-1.0)";
    } else if (key == "brain.v_thresh") {
        return "Neuron threshold potential (mV)";
    } else if (key == "brain.v_rest") {
        return "Resting potential (mV)";
    } else if (key == "brain.v_reset") {
        return "Reset potential after spike (mV)";
    } else if (key == "plasticity.stdp.learning_rate") {
        return "STDP learning rate (0.0-0.1)";
    } else if (key == "plasticity.hebbian.learning_rate") {
        return "Hebbian learning rate (0.0-0.1)";
    } else if (key == "neuromod.dopamine.scale") {
        return "Dopamine scale (0.0-2.0)";
    } else if (key == "development.synaptogenesis_rate") {
        return "Synaptogenesis rate (0.0-0.01)";
    } else if (key == "development.prunning_rate") {
        return "Pruning rate (0.0-0.001)";
    } else if (key == "simulation.timestep") {
        return "Simulation timestep (0.0-1.0)";
    } else if (key == "simulation.max_steps") {
        return "Maximum simulation steps (1-1000000)";
    }
    
    return "Unknown parameter";
}

std::unordered_map<std::string, std::pair<double, double>> ConfigValidationHelpers::getParameterRanges() {
    std::unordered_map<std::string, std::pair<double, double>> ranges;
    
    ranges["brain.neuron_count"] = std::make_pair(1.0, 100000.0);
    ranges["brain.synapse_density"] = std::make_pair(0.0, 1.0);
    ranges["brain.connection_probability"] = std::make_pair(0.0, 1.0);
    ranges["brain.initial_weight_mean"] = std::make_pair(-1.0, 1.0);
    ranges["brain.initial_weight_std"] = std::make_pair(0.0, 1.0);
    ranges["brain.v_thresh"] = std::make_pair(-100.0, 0.0);
    ranges["brain.v_rest"] = std::make_pair(-100.0, 0.0);
    ranges["brain.v_reset"] = std::make_pair(-100.0, 0.0);
    ranges["plasticity.stdp.learning_rate"] = std::make_pair(0.0, 0.1);
    ranges["plasticity.hebbian.learning_rate"] = std::make_pair(0.0, 0.1);
    ranges["plasticity.stdp.tau_plus"] = std::make_pair(0.0, 100.0);
    ranges["plasticity.stdp.tau_minus"] = std::make_pair(0.0, 100.0);
    ranges["neuromod.dopamine.scale"] = std::make_pair(0.0, 2.0);
    ranges["development.synaptogenesis_rate"] = std::make_pair(0.0, 0.01);
    ranges["development.pruning_rate"] = std::make_pair(0.0, 0.001);
    ranges["reward.discount_factor"] = std::make_pair(0.0, 1.0);
    ranges["simulation.timestep"] = std::make_pair(0.0, 1.0);
    ranges["simulation.max_steps"] = std::make_pair(1.0, 1000000.0);
    
    return ranges;
}

std::unordered_map<std::string, std::string> ConfigValidationHelpers::getParameterDescriptions() {
    std::unordered_map<std::string, std::string> descriptions;
    
    descriptions["brain.neuron_count"] = "Total number of neurons in the brain. Typical values: 100-10000.";
    descriptions["brain.synapse_density"] = "Synapse connectivity density. Typical values: 0.05-0.2.";
    descriptions["brain.connection_probability"] = "Probability of connection between neurons. Typical values: 0.05-0.2.";
    descriptions["brain.initial_weight_mean"] = "Mean initial synaptic weight. Typical values: -0.1 to 0.1.";
    descriptions["brain.initial_weight_std"] = "Standard deviation of initial weights. Typical values: 0.1 to 0.2.";
    descriptions["brain.v_thresh"] = "Neuron threshold potential (mV). Typical values: -50 to -40 mV.";
    descriptions["brain.v_rest"] = "Resting potential (mV). Typical values: -70 to -65 mV.";
    descriptions["brain.v_reset"] = "Reset potential after spike (mV). Typical values: -80 to -75 mV.";
    descriptions["plasticity.stdp.learning_rate"] = "STDP learning rate. Typical values: 0.001 to 0.01.";
    descriptions["plasticity.hebbian.learning_rate"] = "Hebbian learning rate. Typical values: 0.01 to 0.1.";
    descriptions["plasticity.stdp.tau_plus"] = "STDP time constant for LTP (ms). Typical values: 10 to 50 ms.";
    descriptions["plasticity.stdp.tau_minus"] = "STDP time constant for LTD (ms). Typical values: 10 to 50 ms.";
    descriptions["neuromod.dopamine.scale"] = "Dopamine modulation scale. Typical values: 0.5 to 1.0.";
    descriptions["development.synaptogenesis_rate"] = "Rate of new synapse formation. Typical values: 0.001 to 0.01.";
    descriptions["development.pruning_rate"] = "Rate of synapse pruning. Typical values: 0.0001 to 0.001.";
    descriptions["reward.discount_factor"] = "Reward discount factor. Typical values: 0.9 to 0.99.";
    descriptions["simulation.timestep"] = "Simulation timestep (s). Typical values: 0.001 to 0.01 s.";
    descriptions["simulation.max_steps"] = "Maximum simulation steps. Typical values: 1000 to 100000.";
    
    return descriptions;
}

// ValidatedConfig implementation
ValidatedConfig::ValidatedConfig(const ConfigSchema& schema)
    : schema_(schema), validator_(schema) {}

bool ValidatedConfig::loadFromFile(const std::string& filepath) {
    if (!config_.loadFromFile(filepath)) {
        return false;
    }
    
    updateValidationCache();
    return true;
}

bool ValidatedConfig::loadFromArgs(int argc, char** argv) {
    if (!config_.loadFromArgs(argc, argv)) {
        return false;
    }
    
    updateValidationCache();
    return true;
}

void ValidatedConfig::set(const std::string& key, const ConfigValue& value,
                         ConfigSource source) {
    config_.set(key, value, source);
    applyParameterValidation(key, value);
}

bool ValidatedConfig::setWithValidation(const std::string& key, const ConfigValue& value,
                                       ConfigSource source,
                                       std::vector<ValidationError>* errors) {
    // First, check if the key exists in schema
    if (!schema_.isValid(key)) {
        if (errors) {
            errors->emplace_back(ValidationErrorType::InvalidType, key,
                               "Unknown configuration key", "Use one of: " + getValidKeyList());
        }
        return false;
    }
    
    // Get parameter info for error messages
    const ConfigParameter* param = schema_.getParameter(key);
    if (!param) {
        if (errors) {
            errors->emplace_back(ValidationErrorType::InvalidType, key,
                               "Unknown configuration parameter");
        }
        return false;
    }
    
    // Validate the value
    std::vector<ValidationError> validationErrors = schema_.validate(key, value);
    
    if (!validationErrors.empty()) {
        if (errors) {
            errors->insert(errors->end(), validationErrors.begin(), validationErrors.end());
        }
        return false;
    }
    
    // If validation passes, set the value
    config_.set(key, value, source);
    return true;
}

bool ValidatedConfig::validate(std::vector<ValidationError>* errors) {
    validator_.reset();
    
    // Validate against schema
    std::vector<ValidationError> schemaErrors = validator_.validate(config_);
    
    if (errors) {
        errors->insert(errors->end(), schemaErrors.begin(), schemaErrors.end());
    }
    
    // Also run parameter-specific validation
    std::vector<ValidationError> paramErrors;
    ConfigValidationHelpers::validateAllParameters(config_, paramErrors);
    
    if (errors) {
        errors->insert(errors->end(), paramErrors.begin(), paramErrors.end());
    }
    
    // Update validator with additional errors
    for (const auto& error : paramErrors) {
        validator_.addError(error.type, error.key, error.message, error.suggestion);
    }
    
    return validator_.hasErrors();
}

std::vector<std::string> ValidatedConfig::getValidationSuggestions() const {
    return validator_.getSuggestions();
}

void ValidatedConfig::applyParameterValidation(const std::string& key, const ConfigValue& value) {
    // Apply parameter-specific validation
    std::vector<ValidationError> errors;
    ConfigValidationHelpers::validateAllParameters(config_, errors);
    
    // Update validator with new errors
    validator_.reset();
    for (const auto& error : errors) {
        validator_.addError(error.type, error.key, error.message, error.suggestion);
    }
}

void ValidatedConfig::updateValidationCache() {
    validator_.reset();
}

std::string ValidatedConfig::getValidKeyList() const {
    std::vector<std::string> keys = schema_.getAllKeys();
    std::ostringstream oss;
    for (size_t i = 0; i < keys.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << keys[i];
    }
    return oss.str();
}

} // namespace nlm