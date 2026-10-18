#include "ConfigValidator.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <regex>

namespace nlm {

struct ConfigValidator::Impl {
    std::unordered_map<std::string, ConfigSchema> registeredSchemas;
    std::vector<TypeValidation> typeValidations;
    std::vector<ParameterDependency> parameterDependencies;
    std::vector<ValidationRule> validationRules;
    std::unordered_map<std::string, Config> profiles;
    
    // Mode-specific validation rules
    std::unordered_map<ValidationMode, std::vector<ValidationRule>> modeRules;
    
    // Special validation functions
    std::function<bool(const Config&)> neuralValidator;
    std::function<bool(const Config&)> plasticityValidator;
    std::function<bool(const Config&)> neuromodulationValidator;
    std::function<bool(const Config&)> developmentValidator;
    std::function<bool(const Config&)> environmentValidator;
    std::function<bool(const Config&)> loggingValidator;
    std::function<bool(const Config&)> simulationValidator;
    
    // Range constraints
    std::unordered_map<std::string, std::pair<double, double>> neuralRanges;
    std::unordered_map<std::string, std::pair<double, double>> plasticityRanges;
    std::unordered_map<std::string, std::pair<double, double>> neuromodulationRanges;
    std::unordered_map<std::string, std::pair<double, double>> developmentRanges;
};

ConfigValidator::ConfigValidator() : pImpl(std::make_unique<Impl>()) {
    // Initialize with default validation rules
    setupDefaultValidationRules();
}

ConfigValidator::~ConfigValidator() = default;

ConfigValidator::ConfigValidator(ConfigValidator&&) noexcept = default;

ConfigValidator& ConfigValidator::operator=(ConfigValidator&&) noexcept = default;

void ConfigValidator::setupDefaultValidationRules() {
    // Setup mode-specific rules
    setupNeuralValidation();
    setupPlasticityValidation();
    setupNeuromodulationValidation();
    setupDevelopmentValidation();
    setupEnvironmentValidation();
    setupLoggingValidation();
    setupSimulationValidation();
}

void ConfigValidator::registerSchema(const ConfigSchema& schema) {
    pImpl->registeredSchemas[schema.name] = schema;
}

void ConfigValidator::registerTypeValidation(const TypeValidation& validation) {
    pImpl->typeValidations.push_back(validation);
}

void ConfigValidator::registerParameterDependency(const ParameterDependency& dependency) {
    pImpl->parameterDependencies.push_back(dependency);
}

void ConfigValidator::registerValidationRule(const ValidationRule& rule) {
    pImpl->validationRules.push_back(rule);
    
    // Add to mode-specific rules
    pImpl->modeRules[mode_].push_back(rule);
}

std::vector<ValidationError> ConfigValidator::validate(const Config& config) const {
    std::vector<ValidationError> errors;
    
    updateStats(true); // This will be updated properly after validation
    
    // Validate against all registered schemas
    for (const auto& pair : pImpl->registeredSchemas) {
        std::vector<ValidationError> schemaErrors = 
            validateAgainstSchema(config, pair.first);
        errors.insert(errors.end(), schemaErrors.begin(), schemaErrors.end());
    }
    
    // Validate against type validations
    for (const auto& typeValidation : pImpl->typeValidations) {
        if (!validateType(config, typeValidation.parameter)) {
            errors.emplace_back(typeValidation.parameter,
                               "Type validation failed",
                               ValidationSeverity::Error);
        }
    }
    
    // Validate dependencies
    std::vector<ValidationError> depErrors = validateDependencies(config);
    errors.insert(errors.end(), depErrors.begin(), depErrors.end());
    
    // Validate against registered rules
    for (const auto& rule : pImpl->validationRules) {
        if (!rule.rule(config)) {
            errors.emplace_back(
                "Validation rule failed: " + rule.name,
                ValidationSeverity::Error);
        }
    }
    
    return errors;
}

std::vector<ValidationError> ConfigValidator::validate(const Config& config, 
                                                       const std::string& group) const {
    std::vector<ValidationError> errors;
    
    // Validate only parameters in the specified group
    std::vector<ConfigEntry> groupEntries;
    for (const auto& entry : config.getEntries(group)) {
        groupEntries.push_back(entry);
    }
    
    for (const auto& entry : groupEntries) {
        std::vector<ValidationError> entryErrors = validateConfigEntry(entry);
        errors.insert(errors.end(), entryErrors.begin(), entryErrors.end());
    }
    
    return errors;
}

bool ConfigValidator::validateParameter(const Config& config, const std::string& key,
                                       const std::string& group) const {
    auto it = std::find_if(config.getEntries(group).begin(), 
                          config.getEntries(group).end(),
                          [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == config.getEntries(group).end()) {
        return false;
    }
    
    std::vector<ValidationError> errors = validateConfigEntry(*it);
    return errors.empty();
}

std::vector<ValidationError> ConfigValidator::validateForMode(const Config& config,
                                                             ValidationMode mode) const {
    std::vector<ValidationError> errors;
    
    // Validate against mode-specific rules
    auto modeIt = pImpl->modeRules.find(mode);
    if (modeIt != pImpl->modeRules.end()) {
        for (const auto& rule : modeIt->second) {
            if (!rule.rule(config)) {
                errors.emplace_back(
                    "Mode-specific validation failed: " + rule.name,
                    ValidationSeverity::Error);
            }
        }
    }
    
    return errors;
}

std::vector<ValidationError> ConfigValidator::validateForProfile(const Config& config,
                                                                 const std::string& profile) const {
    std::vector<ValidationError> errors;
    
    auto profileIt = pImpl->profiles.find(profile);
    if (profileIt != pImpl->profiles.end()) {
        // Compare with profile template
        const Config& profileTemplate = profileIt->second;
        
        // Check for missing parameters
        for (const auto& param : profileTemplate.getKeys()) {
            if (!config.has(param)) {
                errors.emplace_back(param,
                                   "Required parameter is missing",
                                   ValidationSeverity::Error);
            }
        }
        
        // Check for invalid values
        for (const auto& param : config.getKeys()) {
            if (profileTemplate.has(param)) {
                auto profileVal = profileTemplate.get<std::string>(param);
                auto configVal = config.get<std::string>(param);
                if (profileVal && configVal) {
                    if (*profileVal != *configVal) {
                        errors.emplace_back(param,
                                           "Value differs from profile template",
                                           ValidationSeverity::Warning);
                    }
                }
            }
        }
    }
    
    return errors;
}

std::vector<ValidationError> ConfigValidator::validateGroup(const Config& config,
                                                            const std::string& group) const {
    std::vector<ValidationError> errors;
    
    // Validate only parameters in the specified group
    std::vector<ConfigEntry> groupEntries;
    for (const auto& entry : config.getEntries(group)) {
        groupEntries.push_back(entry);
    }
    
    for (const auto& entry : groupEntries) {
        std::vector<ValidationError> entryErrors = validateConfigEntry(entry);
        errors.insert(errors.end(), entryErrors.begin(), entryErrors.end());
    }
    
    return errors;
}

std::vector<ValidationError> ConfigValidator::validateNeuralParameters(const Config& config) const {
    std::vector<ValidationError> errors;
    
    // Validate neural parameters using special validator if available
    if (pImpl->neuralValidator) {
        if (!pImpl->neuralValidator(config)) {
            errors.emplace_back("neural",
                               "Neural parameter validation failed",
                               ValidationSeverity::Error);
        }
    }
    
    // Use type validations as fallback
    for (const auto& validation : pImpl->typeValidations) {
        if (validation.parameter.find("neuron") != std::string::npos ||
            validation.parameter.find("region") != std::string::npos ||
            validation.parameter.find("connection") != std::string::npos) {
            
            if (!validateType(config, validation.parameter)) {
                errors.emplace_back(validation.parameter,
                                   "Neural parameter type validation failed",
                                   ValidationSeverity::Error);
            }
        }
    }
    
    return errors;
}

std::vector<ValidationError> ConfigValidator::validatePlasticityParameters(const Config& config) const {
    std::vector<ValidationError> errors;
    
    // Validate plasticity parameters using special validator if available
    if (pImpl->plasticityValidator) {
        if (!pImpl->plasticityValidator(config)) {
            errors.emplace_back("plasticity",
                               "Plasticity parameter validation failed",
                               ValidationSeverity::Error);
        }
    }
    
    // Use type validations as fallback
    for (const auto& validation : pImpl->typeValidations) {
        if (validation.parameter.find("plasticity") != std::string::npos ||
            validation.parameter.find("stdp") != std::string::npos) {
            
            if (!validateType(config, validation.parameter)) {
                errors.emplace_back(validation.parameter,
                                   "Plasticity parameter type validation failed",
                                   ValidationSeverity::Error);
            }
        }
    }
    
    return errors;
}

std::vector<ValidationError> ConfigValidator::validateNeuromodulationParameters(const Config& config) const {
    std::vector<ValidationError> errors;
    
    // Validate neuromodulation parameters using special validator if available
    if (pImpl->neuromodulationValidator) {
        if (!pImpl->neuromodulationValidator(config)) {
            errors.emplace_back("neuromodulation",
                               "Neuromodulation parameter validation failed",
                               ValidationSeverity::Error);
        }
    }
    
    // Use type validations as fallback
    for (const auto& validation : pImpl->typeValidations) {
        if (validation.parameter.find("dopamine") != std::string::npos ||
            validation.parameter.find("curiosity") != std::string::npos ||
            validation.parameter.find("novelty") != std::string::npos ||
            validation.parameter.find("reward") != std::string::npos) {
            
            if (!validateType(config, validation.parameter)) {
                errors.emplace_back(validation.parameter,
                                   "Neuromodulation parameter type validation failed",
                                   ValidationSeverity::Error);
            }
        }
    }
    
    return errors;
}

std::vector<ValidationError> ConfigValidator::validateDevelopmentParameters(const Config& config) const {
    std::vector<ValidationError> errors;
    
    // Validate development parameters using special validator if available
    if (pImpl->developmentValidator) {
        if (!pImpl->developmentValidator(config)) {
            errors.emplace_back("development",
                               "Development parameter validation failed",
                               ValidationSeverity::Error);
        }
    }
    
    // Use type validations as fallback
    for (const auto& validation : pImpl->typeValidations) {
        if (validation.parameter.find("development") != std::string::npos ||
            validation.parameter.find("synaptogenesis") != std::string::npos ||
            validation.parameter.find("pruning") != std::string::npos) {
            
            if (!validateType(config, validation.parameter)) {
                errors.emplace_back(validation.parameter,
                                   "Development parameter type validation failed",
                                   ValidationSeverity::Error);
            }
        }
    }
    
    return errors;
}

std::vector<ValidationError> ConfigValidator::validateNeuralRanges(const Config& config) const {
    std::vector<ValidationError> errors;
    
    for (const auto& pair : pImpl->neuralRanges) {
        const std::string& param = pair.first;
        double min = pair.second.first;
        double max = pair.second.second;
        
        if (config.has(param)) {
            auto valueOpt = config.get<double>(param);
            if (valueOpt) {
                double val = *valueOpt;
                if (min > -1e9 && val < min) {
                    errors.emplace_back(param,
                                       "Value is below minimum for neural parameters",
                                       ValidationSeverity::Error);
                }
                if (max < 1e9 && val > max) {
                    errors.emplace_back(param,
                                       "Value exceeds maximum for neural parameters",
                                       ValidationSeverity::Error);
                }
            }
        }
    }
    
    return errors;
}

std::vector<ValidationError> ConfigValidator::validatePlasticityRanges(const Config& config) const {
    std::vector<ValidationError> errors;
    
    for (const auto& pair : pImpl->plasticityRanges) {
        const std::string& param = pair.first;
        double min = pair.second.first;
        double max = pair.second.second;
        
        if (config.has(param)) {
            auto valueOpt = config.get<double>(param);
            if (valueOpt) {
                double val = *valueOpt;
                if (min > -1e9 && val < min) {
                    errors.emplace_back(param,
                                       "Value is below minimum for plasticity parameters",
                                       ValidationSeverity::Error);
                }
                if (max < 1e9 && val > max) {
                    errors.emplace_back(param,
                                       "Value exceeds maximum for plasticity parameters",
                                       ValidationSeverity::Error);
                }
            }
        }
    }
    
    return errors;
}

std::vector<ValidationError> ConfigValidator::validateNeuromodulationRanges(const Config& config) const {
    std::vector<ValidationError> errors;
    
    for (const auto& pair : pImpl->neuromodulationRanges) {
        const std::string& param = pair.first;
        double min = pair.second.first;
        double max = pair.second.second;
        
        if (config.has(param)) {
            auto valueOpt = config.get<double>(param);
            if (valueOpt) {
                double val = *valueOpt;
                if (min > -1e9 && val < min) {
                    errors.emplace_back(param,
                                       "Value is below minimum for neuromodulation parameters",
                                       ValidationSeverity::Error);
                }
                if (max < 1e9 && val > max) {
                    errors.emplace_back(param,
                                       "Value exceeds maximum for neuromodulation parameters",
                                       ValidationSeverity::Error);
                }
            }
        }
    }
    
    return errors;
}

std::vector<ValidationError> ConfigValidator::validateDevelopmentRanges(const Config& config) const {
    std::vector<ValidationError> errors;
    
    for (const auto& pair : pImpl->developmentRanges) {
        const std::string& param = pair.first;
        double min = pair.second.first;
        double max = pair.second.second;
        
        if (config.has(param)) {
            auto valueOpt = config.get<double>(param);
            if (valueOpt) {
                double val = *valueOpt;
                if (min > -1e9 && val < min) {
                    errors.emplace_back(param,
                                       "Value is below minimum for development parameters",
                                       ValidationSeverity::Error);
                }
                if (max < 1e9 && val > max) {
                    errors.emplace_back(param,
                                       "Value exceeds maximum for development parameters",
                                       ValidationSeverity::Error);
                }
            }
        }
    }
    
    return errors;
}

std::vector<ValidationError> ConfigValidator::validateDependencies(const Config& config) const {
    std::vector<ValidationError> errors;
    
    for (const auto& dep : pImpl->parameterDependencies) {
        if (!checkDependency(config, dep)) {
            errors.emplace_back(dep.requiredParameter,
                               dep.errorMessage,
                               ValidationSeverity::Error);
        }
    }
    
    return errors;
}

std::vector<ValidationError> ConfigValidator::validateParameterDependencies(const Config& config,
                                                                          const std::string& group) const {
    std::vector<ValidationError> errors;
    
    // Filter dependencies for the specified group
    for (const auto& dep : pImpl->parameterDependencies) {
        // Check if dependency is related to the group
        if (group.empty() || 
            dep.requiredParameter.find(group) != std::string::npos) {
            
            if (!checkDependency(config, dep)) {
                errors.emplace_back(dep.requiredParameter,
                                   dep.errorMessage,
                                   ValidationSeverity::Error);
            }
        }
    }
    
    return errors;
}

std::vector<ValidationError> ConfigValidator::validateAllTypes(const Config& config) const {
    std::vector<ValidationError> errors;
    
    for (const auto& validation : pImpl->typeValidations) {
        if (!validateType(config, validation.parameter)) {
            errors.emplace_back(validation.parameter,
                               "Type validation failed",
                               ValidationSeverity::Error);
        }
    }
    
    return errors;
}

bool ConfigValidator::validateType(const Config& config, const std::string& key,
                                  const std::string& group) const {
    for (const auto& validation : pImpl->typeValidations) {
        if (validation.parameter == key &&
            !validation.validate(config.get<ConfigValue>(key))) {
            return false;
        }
    }
    
    return true;
}

std::vector<ValidationError> ConfigValidator::validateAgainstSchema(const Config& config,
                                                                   const std::string& schemaName) const {
    std::vector<ValidationError> errors;
    
    auto schemaIt = pImpl->registeredSchemas.find(schemaName);
    if (schemaIt == pImpl->registeredSchemas.end()) {
        errors.emplace_back(schemaName,
                           "Schema not found",
                           ValidationSeverity::Error);
        return errors;
    }
    
    const ConfigSchema& schema = schemaIt->second;
    
    // Check required parameters
    for (const auto& param : schema.parameters) {
        if (param.required && !config.has(param.key)) {
            errors.emplace_back(param.key,
                               "Required parameter is missing",
                               ValidationSeverity::Error);
        }
    }
    
    // Validate existing parameters
    for (const auto& entry : config.getEntries()) {
        auto paramIt = std::find_if(schema.parameters.begin(), schema.parameters.end(),
                                    [&entry](const ParameterMetadata& p) { return p.key == entry.key; });
        if (paramIt != schema.parameters.end()) {
            // Validate value
            ParameterConstraint constraint = paramIt->constraint;
            if (!validateValue(entry.value, constraint)) {
                errors.emplace_back(entry.key,
                                   "Value validation failed for parameter",
                                   ValidationSeverity::Error);
            }
        }
    }
    
    return errors;
}

bool ConfigValidator::checkDependency(const Config& config, const ParameterDependency& dep) const {
    if (!config.has(dep.requiredParameter)) {
        return false;
    }
    
    auto valueOpt = config.get<ConfigValue>(dep.requiredParameter);
    if (!valueOpt) {
        return false;
    }
    
    return dep.dependencyValidator(*valueOpt);
}

void ConfigValidator::setupNeuralValidation() {
    // Setup neural-specific validation rules
    pImpl->neuralValidator = [this](const Config& config) -> bool {
        bool isValid = true;
        
        // Check neuron count is reasonable
        if (config.has("neuron_count")) {
            auto count = config.get<size_t>("neuron_count");
            if (count && (*count < 1 || *count > 1000000)) {
                isValid = false;
            }
        }
        
        // Check connection probability is between 0 and 1
        if (config.has("connection_probability")) {
            auto prob = config.get<double>("connection_probability");
            if (prob && (*prob < 0.0 || *prob > 1.0)) {
                isValid = false;
            }
        }
        
        return isValid;
    };
    
    // Setup range constraints
    pImpl->neuralRanges["neuron_count"] = std::make_pair(1.0, 1000000.0);
    pImpl->neuralRanges["connection_probability"] = std::make_pair(0.0, 1.0);
    
    // Register validation rules
    registerValidationRule(ValidationRule("neural_neuron_count",
        [this](const Config& config) -> bool {
            if (!config.has("neuron_count")) return true;
            auto count = config.get<size_t>("neuron_count");
            return count && *count >= 10;
        },
        ValidationSeverity::Error,
        "Neuron count must be at least 10"));
    
    registerValidationRule(ValidationRule("neural_connection_prob",
        [this](const Config& config) -> bool {
            if (!config.has("connection_probability")) return true;
            auto prob = config.get<double>("connection_probability");
            return prob && *prob >= 0.0 && *prob <= 1.0;
        },
        ValidationSeverity::Error,
        "Connection probability must be between 0 and 1"));
}

void ConfigValidator::setupPlasticityValidation() {
    // Setup plasticity-specific validation rules
    pImpl->plasticityValidator = [this](const Config& config) -> bool {
        bool isValid = true;
        
        // Check STDP weights are reasonable
        if (config.has("stdp_ltp_weight")) {
            auto ltp = config.get<double>("stdp_ltp_weight");
            if (ltp && (*ltp < 0.0 || *ltp > 0.5)) {
                isValid = false;
            }
        }
        
        if (config.has("stdp_ltd_weight")) {
            auto ltd = config.get<double>("stdp_ltd_weight");
            if (ltd && (*ltd < 0.0 || *ltd > 0.5)) {
                isValid = false;
            }
        }
        
        // Check plasticity learning rate
        if (config.has("plasticity_learning_rate")) {
            auto rate = config.get<double>("plasticity_learning_rate");
            if (rate && (*rate < 0.0 || *rate > 1.0)) {
                isValid = false;
            }
        }
        
        return isValid;
    };
    
    // Setup range constraints
    pImpl->plasticityRanges["stdp_ltp_weight"] = std::make_pair(0.0, 0.5);
    pImpl->plasticityRanges["stdp_ltd_weight"] = std::make_pair(0.0, 0.5);
    pImpl->plasticityRanges["plasticity_learning_rate"] = std::make_pair(0.0, 1.0);
    
    // Register validation rules
    registerValidationRule(ValidationRule("plasticity_stdp_weights",
        [this](const Config& config) -> bool {
            if (config.has("stdp_ltp_weight") && config.has("stdp_ltd_weight")) {
                auto ltp = config.get<double>("stdp_ltp_weight");
                auto ltd = config.get<double>("stdp_ltd_weight");
                if (ltp && ltd) {
                    return *ltp <= *ltd; // LTP should not be greater than LTD
                }
            }
            return true;
        },
        ValidationSeverity::Error,
        "STDP LTP weight should not exceed LTD weight"));
}

void ConfigValidator::setupNeuromodulationValidation() {
    // Setup neuromodulation-specific validation rules
    pImpl->neuromodulationValidator = [this](const Config& config) -> bool {
        bool isValid = true;
        
        // Check dopamine baseline is reasonable
        if (config.has("dopamine_baseline")) {
            auto dopamine = config.get<double>("dopamine_baseline");
            if (dopamine && (*dopamine < -1.0 || *dopamine > 1.0)) {
                isValid = false;
            }
        }
        
        // Check curiosity strength
        if (config.has("curiosity_strength")) {
            auto curiosity = config.get<double>("curiosity_strength");
            if (curiosity && (*curiosity < 0.0 || *curiosity > 10.0)) {
                isValid = false;
            }
        }
        
        // Check novelty sensitivity
        if (config.has("novelty_sensitivity")) {
            auto novelty = config.get<double>("novelty_sensitivity");
            if (novelty && (*novelty < 0.0 || *novelty > 10.0)) {
                isValid = false;
            }
        }
        
        return isValid;
    };
    
    // Setup range constraints
    pImpl->neuromodulationRanges["dopamine_baseline"] = std::make_pair(-1.0, 1.0);
    pImpl->neuromodulationRanges["curiosity_strength"] = std::make_pair(0.0, 10.0);
    pImpl->neuromodulationRanges["novelty_sensitivity"] = std::make_pair(0.0, 10.0);
}

void ConfigValidator::setupDevelopmentValidation() {
    // Setup development-specific validation rules
    pImpl->developmentValidator = [this](const Config& config) -> bool {
        bool isValid = true;
        
        // Check synaptogenesis rate
        if (config.has("synaptogenesis_rate")) {
            auto rate = config.get<double>("synaptogenesis_rate");
            if (rate && (*rate < 0.0 || *rate > 0.01)) {
                isValid = false;
            }
        }
        
        // Check pruning rate
        if (config.has("pruning_rate")) {
            auto rate = config.get<double>("pruning_rate");
            if (rate && (*rate < 0.0 || *rate > 0.01)) {
                isValid = false;
            }
        }
        
        return isValid;
    };
    
    // Setup range constraints
    pImpl->developmentRanges["synaptogenesis_rate"] = std::make_pair(0.0, 0.01);
    pImpl->developmentRanges["pruning_rate"] = std::make_pair(0.0, 0.01);
}

void ConfigValidator::setupEnvironmentValidation() {
    pImpl->environmentValidator = [this](const Config& config) -> bool {
        bool isValid = true;
        
        // Check environment dimensions
        if (config.has("environment_width")) {
            auto width = config.get<size_t>("environment_width");
            if (width && (*width < 1 || *width > 1000)) {
                isValid = false;
            }
        }
        
        if (config.has("environment_height")) {
            auto height = config.get<size_t>("environment_height");
            if (height && (*height < 1 || *height > 1000)) {
                isValid = false;
            }
        }
        
        // Check environment name
        if (config.has("environment_name")) {
            auto name = config.get<std::string>("environment_name");
            if (name && (*name != "GridWorld" && *name != "Continuous" && *name != "Custom")) {
                isValid = false;
            }
        }
        
        return isValid;
    };
    
    // Register validation rules
    registerValidationRule(ValidationRule("environment_dimensions",
        [this](const Config& config) -> bool {
            if (config.has("environment_width") && config.has("environment_height")) {
                auto width = config.get<size_t>("environment_width");
                auto height = config.get<size_t>("environment_height");
                return width && height && (*width > 0) && (*height > 0);
            }
            return true;
        },
        ValidationSeverity::Error,
        "Environment dimensions must be positive"));
}

void ConfigValidator::setupLoggingValidation() {
    pImpl->loggingValidator = [this](const Config& config) -> bool {
        bool isValid = true;
        
        // Check log level
        if (config.has("log_level")) {
            auto level = config.get<std::string>("log_level");
            if (level && (*level != "DEBUG" && *level != "INFO" &&
                         *level != "WARNING" && *level != "ERROR" &&
                         *level != "CRITICAL")) {
                isValid = false;
            }
        }
        
        return isValid;
    };
}

void ConfigValidator::setupSimulationValidation() {
    pImpl->simulationValidator = [this](const Config& config) -> bool {
        bool isValid = true;
        
        // Check simulation timestep
        if (config.has("simulation_timestep")) {
            auto timestep = config.get<double>("simulation_timestep");
            if (timestep && (*timestep <= 0.0 || *timestep > 1.0)) {
                isValid = false;
            }
        }
        
        // Check max simulation steps
        if (config.has("max_simulation_steps")) {
            auto steps = config.get<size_t>("max_simulation_steps");
            if (steps && (*steps < 1 || *steps > 1000000)) {
                isValid = false;
            }
        }
        
        return isValid;
    };
}

std::string ConfigValidator::getParameterCategoryDescription(ParameterCategory category) const {
    switch (category) {
        case ParameterCategory::Neural: return "Neural system parameters";
        case ParameterCategory::Plasticity: return "Synaptic plasticity parameters";
        case ParameterCategory::Neuromodulation: return "Neuromodulation parameters";
        case ParameterCategory::Development: return "Development parameters";
        case ParameterCategory::Reward: return "Reward parameters";
        case ParameterCategory::Environment: return "Environment parameters";
        case ParameterCategory::Logging: return "Logging parameters";
        case ParameterCategory::Simulation: return "Simulation parameters";
        case ParameterCategory::Visualization: return "Visualization parameters";
        case ParameterCategory::Memory: return "Memory parameters";
        case ParameterCategory::Cognition: return "Cognition parameters";
        case ParameterCategory::Performance: return "Performance parameters";
        case ParameterCategory::Custom: return "Custom parameters";
        default: return "Unknown parameter category";
    }
}

std::string ConfigValidator::getErrorMessageForParameter(const std::string& key) const {
    auto it = pImpl->typeValidations.begin();
    for (; it != pImpl->typeValidations.end(); ++it) {
        if (it->parameter == key) {
            return "Parameter " + key + " has type validation: expected " +
                   std::to_string(static_cast<int>(it->expectedType));
        }
    }
    
    return "Parameter " + key + " validation failed";
}

bool ConfigValidator::isValidParameterType(const ConfigValue& value, ParameterType type) const {
    switch (type) {
        case ParameterType::Integer: return std::holds_alternative<int>(value) ||
                                       std::holds_alternative<int64_t>(value);
        case ParameterType::Float: return std::holds_alternative<double>(value);
        case ParameterType::Boolean: return std::holds_alternative<bool>(value);
        case ParameterType::String: return std::holds_alternative<std::string>(value);
        case ParameterType::IntegerArray: return std::holds_alternative<std::vector<int>>(value);
        case ParameterType::FloatArray: return std::holds_alternative<std::vector<double>>(value);
        case ParameterType::StringArray: return std::holds_alternative<std::vector<std::string>>(value);
        case ParameterType::Object: return !std::holds_alternative<int>(value) &&
                                       !std::holds_alternative<double>(value) &&
                                       !std::holds_alternative<bool>(value) &&
                                       !std::holds_alternative<std::string>(value) &&
                                       !std::holds_alternative<std::vector<int>>(value) &&
                                       !std::holds_alternative<std::vector<double>>(value) &&
                                       !std::holds_alternative<std::vector<std::string>>(value);
        default: return false;
    }
}

void ConfigValidator::addProfile(const std::string& name, const Config& configTemplate) {
    pImpl->profiles[name] = configTemplate;
}

std::vector<std::string> ConfigValidator::getProfileNames() const {
    std::vector<std::string> names;
    for (const auto& pair : pImpl->profiles) {
        names.push_back(pair.first);
    }
    return names;
}

std::optional<Config> ConfigValidator::getProfile(const std::string& name) const {
    auto it = pImpl->profiles.find(name);
    if (it != pImpl->profiles.end()) {
        return it->second;
    }
    return std::nullopt;
}

void ConfigValidator::updateStats(bool passed) const {
    pImpl->stats_.totalValidations++;
    if (passed) {
        pImpl->stats_.passedValidations++;
    } else {
        pImpl->stats_.failedValidations++;
    }
}

} // namespace nlm