// Configuration Validation System Implementation

#include "core/Config/ConfigValidator.hpp"
#include <limits>
#include <algorithm>
#include <unordered_set>

namespace nlm {

ConfigValidator::ConfigValidator() {}

void ConfigValidator::addRule(const std::string& parameter, 
                            ValidationFunc validator,
                            const std::string& description,
                            ValidationSeverity severity) {
    // Remove existing rule for this parameter to avoid duplicates
    rules_.erase(std::remove_if(rules_.begin(), rules_.end(),
        [&parameter](const ValidationRule& rule) { return rule.parameter == parameter; }),
        rules_.end());
    
    rules_.push_back({parameter, validator, description, severity});
}

void ConfigValidator::addIntRule(const std::string& parameter,
                                std::function<bool(int64_t)> validator,
                                const std::string& description,
                                ValidationSeverity severity,
                                int64_t minValue,
                                int64_t maxValue) {
    addRule(parameter, 
        [validator, minValue, maxValue](const std::string& valueStr) -> std::optional<std::string> {
            try {
                int64_t value = std::stoll(valueStr);
                if (!validator(value)) return "Value " + valueStr + " is invalid for parameter '" + parameter + "'";
                if (value < minValue || value > maxValue) {
                    return "Value " + valueStr + " is out of range [" + std::to_string(minValue) + ", " + std::to_string(maxValue) + "] for parameter '" + parameter + "'";
                }
                return std::nullopt;
            } catch (const std::exception&) {
                return "Invalid integer value for parameter '" + parameter + "'";
            }
        },
        description, severity);
}

void ConfigValidator::addFloatRule(const std::string& parameter,
                                  std::function<bool(double)> validator,
                                  const std::string& description,
                                  ValidationSeverity severity,
                                  double minValue,
                                  double maxValue) {
    addRule(parameter,
        [validator, minValue, maxValue](const std::string& valueStr) -> std::optional<std::string> {
            try {
                double value = std::stod(valueStr);
                if (!validator(value)) return "Value " + valueStr + " is invalid for parameter '" + parameter + "'";
                if (value < minValue || value > maxValue) {
                    return "Value " + valueStr + " is out of range [" + std::to_string(minValue) + ", " + std::to_string(maxValue) + "] for parameter '" + parameter + "'";
                }
                return std::nullopt;
            } catch (const std::exception&) {
                return "Invalid float value for parameter '" + parameter + "'";
            }
        },
        description, severity);
}

void ConfigValidator::addBoolRule(const std::string& parameter,
                                 const std::string& description,
                                 ValidationSeverity severity) {
    addRule(parameter,
        [parameter](const std::string& valueStr) -> std::optional<std::string> {
            if (valueStr == "true" || valueStr == "false") {
                return std::nullopt;
            }
            return "Invalid boolean value for parameter '" + parameter + "'. Must be 'true' or 'false'";
        },
        description, severity);
}

void ConfigValidator::addProbabilityRule(const std::string& parameter,
                                        const std::string& description,
                                        ValidationSeverity severity) {
    addFloatRule(parameter,
        [parameter](double value) -> bool {
            return value >= 0.0 && value <= 1.0;
        },
        description, severity, 0.0, 1.0);
}

void ConfigValidator::validate(const std::string& parameter, const std::string& valueStr) const {
    for (const auto& rule : rules_) {
        if (rule.parameter == parameter) {
            auto error = rule.validator(valueStr);
            if (error.has_value()) {
                if (rule.severity == ValidationSeverity::Error) {
                    throw ValidationError(error.value(), parameter, ValidationSeverity::Error);
                } else {
                    // For warnings, we could log them instead of throwing
                    // For now, just throw with Warning severity
                    throw ValidationError(error.value(), parameter, ValidationSeverity::Warning);
                }
            }
            return;
        }
    }
    // No rule found for this parameter - allow it
}

std::vector<ValidationError> ConfigValidator::validateAll(const std::unordered_map<std::string, std::string>& config) const {
    std::vector<ValidationError> errors;
    for (const auto& [parameter, valueStr] : config) {
        try {
            validate(parameter, valueStr);
        } catch (const ValidationError& e) {
            errors.push_back(e);
        }
    }
    return errors;
}

std::vector<std::string> ConfigValidator::getParameterNames() const {
    std::vector<std::string> names;
    std::unordered_set<std::string> seen;
    for (const auto& rule : rules_) {
        if (seen.find(rule.parameter) == seen.end()) {
            names.push_back(rule.parameter);
            seen.insert(rule.parameter);
        }
    }
    return names;
}

std::string ConfigValidator::getDescription(const std::string& parameter) const {
    for (const auto& rule : rules_) {
        if (rule.parameter == parameter) {
            return rule.description;
        }
    }
    return "";
}

std::optional<ConfigValidator::ValidationFunc> ConfigValidator::getRule(const std::string& parameter) const {
    for (const auto& rule : rules_) {
        if (rule.parameter == parameter) {
            return rule.validator;
        }
    }
    return std::nullopt;
}

void ConfigValidator::removeRule(const std::string& parameter) {
    rules_.erase(std::remove_if(rules_.begin(), rules_.end(),
        [&parameter](const ValidationRule& rule) { return rule.parameter == parameter; }),
        rules_.end());
}

void ConfigValidator::clearRules() {
    rules_.clear();
}

// Static instance
ConfigValidator& ConfigValidator::getInstance() {
    static ConfigValidator instance;
    return instance;
}

// NLMConfigValidator implementation
void NLMConfigValidator::initializeRules(ConfigValidator& validator) {
    addCoreRules(validator);
    addSTDPParameterRules(validator);
    addStructuralPlasticityRules(validator);
    addEnvironmentRules(validator);
    addVisionRules(validator);
    addAgentRules(validator);
}

void NLMConfigValidator::addCoreRules(ConfigValidator& validator) {
    // Core neural parameters
    validator.addIntRule("neuron_count", validatePositive,
        "Number of neurons in the brain network", ValidationSeverity::Error, 1, 100000);
    
    validator.addIntRule("region_count", validatePositive,
        "Number of neural regions", ValidationSeverity::Error, 1, 100);
    
    validator.addFloatRule("connection_probability", validateProbability,
        "Probability of connections between neurons (0.0 to 1.0)", ValidationSeverity::Error);
    
    validator.addIntRule("random_seed", [](int64_t) { return true; },  // Accept any seed
        "Random seed for reproducible simulations", ValidationSeverity::Warning, 0, std::numeric_limits<int64_t>::max());
    
    validator.addFloatRule("simulation_timestep", validateNonNegative,
        "Simulation timestep in seconds", ValidationSeverity::Error, 0.0, 1.0);
}

void NLMConfigValidator::addSTDPParameterRules(ConfigValidator& validator) {
    // STDP parameters
    validator.addFloatRule("stdp_ltp_weight", validatePositive,
        "STDP Long-Term Potentiation weight", ValidationSeverity::Error, 0.0, 1.0);
    
    validator.addFloatRule("stdp_ltd_weight", validatePositive,
        "STDP Long-Term Depression weight", ValidationSeverity::Error, 0.0, 1.0);
    
    validator.addFloatRule("stdp_tau", validatePositive,
        "STDP time constant", ValidationSeverity::Error, 0.0, 1000.0);
}

void NLMConfigValidator::addStructuralPlasticityRules(ConfigValidator& validator) {
    // Structural plasticity parameters
    validator.addFloatRule("synaptogenesis_rate", validateNonNegative,
        "Rate of new synapse formation", ValidationSeverity::Warning, 0.0, 1.0);
    
    validator.addFloatRule("pruning_rate", validateNonNegative,
        "Rate of synapse pruning", ValidationSeverity::Warning, 0.0, 1.0);
}

void NLMConfigValidator::addEnvironmentRules(ConfigValidator& validator) {
    // Environment parameters
    validator.addIntRule("environment_width", validatePositive,
        "Width of the simulation environment", ValidationSeverity::Error, 1, 1000);
    
    validator.addIntRule("environment_height", validatePositive,
        "Height of the simulation environment", ValidationSeverity::Error, 1, 1000);
}

void NLMConfigValidator::addVisionRules(ConfigValidator& validator) {
    // Vision parameters
    validator.addIntRule("vision_width", validatePositive,
        "Width of visual field (number of vision neurons)", ValidationSeverity::Error, 1, 1000);
    
    validator.addIntRule("vision_height", validatePositive,
        "Height of visual field (number of vision neurons)", ValidationSeverity::Error, 1, 1000);
}

void NLMConfigValidator::addAgentRules(ConfigValidator& validator) {
    // Agent-specific parameters
    validator.addBoolRule("agent_enabled", 
        "Whether the agent system is enabled", ValidationSeverity::Warning);
}

// Static helper function implementations
bool NLMConfigValidator::validatePositive(const std::string& param, const std::string& valueStr) {
    try {
        double value = std::stod(valueStr);
        return value > 0.0;
    } catch (const std::exception&) {
        return false;
    }
}

bool NLMConfigValidator::validateNonNegative(const std::string& param, const std::string& valueStr) {
    try {
        double value = std::stod(valueStr);
        return value >= 0.0;
    } catch (const std::exception&) {
        return false;
    }
}

bool NLMConfigValidator::validateProbability(const std::string& param, const std::string& valueStr) {
    try {
        double value = std::stod(valueStr);
        return value >= 0.0 && value <= 1.0;
    } catch (const std::exception&) {
        return false;
    }
}

bool NLMConfigValidator::validateFloatRange(const std::string& param, const std::string& valueStr, double min, double max) {
    try {
        double value = std::stod(valueStr);
        return value >= min && value <= max;
    } catch (const std::exception&) {
        return false;
    }
}

bool NLMConfigValidator::validateIntRange(const std::string& param, const std::string& valueStr, int64_t min, int64_t max) {
    try {
        int64_t value = std::stoll(valueStr);
        return value >= min && value <= max;
    } catch (const std::exception&) {
        return false;
    }
}

// ConfigValidationHelper implementation
bool ConfigValidationHelper::strictValidation_ = false;

bool ConfigValidationHelper::validateConfig(const std::unordered_map<std::string, std::string>& config) {
    auto errors = validateConfigWithErrors(config);
    if (!errors.empty()) {
        for (const auto& error : errors) {
            if (error.getSeverity() == ValidationSeverity::Error || strictValidation_) {
                // In strict mode, even warnings are treated as errors
                std::cerr << "CONFIG ERROR: " << error.what() << std::endl;
            } else {
                // Just warning - print to stderr but don't abort
                std::cerr << "CONFIG WARNING: " << error.what() << std::endl;
            }
        }
        return errors.size() == 0 || (strictValidation_ && !errors.empty());
    }
    return true;
}

std::vector<ValidationError> ConfigValidationHelper::validateConfigWithErrors(const std::unordered_map<std::string, std::string>& config) {
    ConfigValidator validator;
    NLMConfigValidator::initializeRules(validator);
    return validator.validateAll(config);
}