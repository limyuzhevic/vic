#include "Config/ConfigValidator.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>

namespace nlm {

struct ConfigValidator::Impl {
    struct ValidationRule {
        std::string key;
        std::function<bool(const ConfigValue&)> validator;
        std::string errorMessage;
        ValidationResult result;
        std::string suggestion;
    };
    
    std::vector<ValidationRule> rules;
    std::map<std::string, std::string> descriptions;
    std::map<std::string, std::string> types;
    std::map<std::string, std::string> units;
    std::map<std::string, std::string> examples;
};

ConfigValidator::ConfigValidator() : pImpl(std::make_unique<Impl>()) {
    // Setup default configuration metadata
    setupDefaultMetadata();
}

void ConfigValidator::addValidation(const std::string& key, 
                                   const std::function<bool(const ConfigValue&)>& validator,
                                   const std::string& errorMessage,
                                   ValidationResult result,
                                   const std::string& suggestion) {
    Impl::ValidationRule rule;
    rule.key = key;
    rule.validator = validator;
    rule.errorMessage = errorMessage;
    rule.result = result;
    rule.suggestion = suggestion;
    
    pImpl->rules.push_back(rule);
}

std::vector<ValidationError> ConfigValidator::validate(const Config& config) const {
    std::vector<ValidationError> errors;
    
    for (const auto& rule : pImpl->rules) {
        if (config.has(rule.key)) {
            auto valueOpt = config.get(rule.key);
            if (valueOpt) {
                if (!rule.validator(*valueOpt)) {
                    ValidationError error(rule.key, rule.errorMessage, rule.result, rule.suggestion);
                    errors.push_back(error);
                }
            }
        }
    }
    
    return errors;
}

std::string ConfigValidator::getErrorSummary(const std::vector<ValidationError>& errors) const {
    std::ostringstream oss;
    oss << "Configuration validation found " << ConfigErrorReporter::countErrors(errors) << " error";
    if (ConfigErrorReporter::countErrors(errors) != 1) {
        oss << "s";
    }
    oss << ".";
    return oss.str();
}

std::string ConfigValidator::getWarningSummary(const std::vector<ValidationError>& errors) const {
    std::ostringstream oss;
    oss << "Configuration validation found " << ConfigErrorReporter::countWarnings(errors) << " warning";
    if (ConfigErrorReporter::countWarnings(errors) != 1) {
        oss << "s";
    }
    oss << ".";
    return oss.str();
}

bool ConfigValidator::validatePositiveInt(const ConfigValue& value) {
    try {
        int val = std::get<int>(value);
        return val > 0;
    } catch (...) {
        return false;
    }
}

bool ConfigValidator::validatePositiveDouble(const ConfigValue& value) {
    try {
        double val = std::get<double>(value);
        return val > 0.0;
    } catch (...) {
        return false;
    }
}

bool ConfigValidator::validateNonNegativeInt(const ConfigValue& value) {
    try {
        int val = std::get<int>(value);
        return val >= 0;
    } catch (...) {
        return false;
    }
}

bool ConfigValidator::validateProbability(const ConfigValue& value) {
    try {
        double val = std::get<double>(value);
        return val >= 0.0 && val <= 1.0;
    } catch (...) {
        return false;
    }
}

bool ConfigValidator::validateStringNotEmpty(const ConfigValue& value) {
    try {
        std::string val = std::get<std::string>(value);
        return !val.empty();
    } catch (...) {
        return false;
    }
}

bool ConfigValidator::validateRange(const ConfigValue& value, double min, double max) {
    try {
        double val = std::get<double>(value);
        return val >= min && val <= max;
    } catch (...) {
        return false;
    }
}

bool ConfigValidator::validateEnum(const ConfigValue& value, 
                                 const std::vector<std::string>& validValues) {
    try {
        std::string val = std::get<std::string>(value);
        return std::find(validValues.begin(), validValues.end(), val) != validValues.end();
    } catch (...) {
        return false;
    }
}

bool ConfigValidator::hasMetadata(const std::string& key) {
    // This would need access to a global validator instance
    // For now return false
    return false;
}

std::string ConfigValidator::getDescription(const std::string& key) {
    // This would need access to a global validator instance
    // For now return empty string
    return "";
}

std::string ConfigValidator::getType(const std::string& key) {
    // This would need access to a global validator instance
    // For now return empty string
    return "";
}

std::string ConfigValidator::getUnits(const std::string& key) {
    // This would need access to a global validator instance
    // For now return empty string
    return "";
}

std::string ConfigValidator::getExample(const std::string& key) {
    // This would need access to a global validator instance
    // For now return empty string
    return "";
}

void ConfigValidator::setupDefaultMetadata() {
    // Brain parameters
    pImpl->descriptions["neuron_count"] = "Number of neurons in the brain";
    pImpl->types["neuron_count"] = "int";
    pImpl->units["neuron_count"] = "neurons";
    pImpl->examples["neuron_count"] = "100, 500, 1000";
    
    pImpl->descriptions["region_count"] = "Number of brain regions";
    pImpl->types["region_count"] = "int";
    pImpl->units["region_count"] = "regions";
    pImpl->examples["region_count"] = "1, 2, 4";
    
    pImpl->descriptions["connection_probability"] = "Probability of connections between neurons";
    pImpl->types["connection_probability"] = "float";
    pImpl->units["connection_probability"] = "probability [0.0, 1.0]";
    pImpl->examples["connection_probability"] = "0.1, 0.2, 0.5";
    
    pImpl->descriptions["simulation_timestep"] = "Time step for simulation";
    pImpl->types["simulation_timestep"] = "float";
    pImpl->units["simulation_timestep"] = "seconds";
    pImpl->examples["simulation_timestep"] = "0.001, 0.01, 0.1";
    
    // Plasticity parameters
    pImpl->descriptions["stdp_ltp_weight"] = "STDP long-term potentiation weight";
    pImpl->types["stdp_ltp_weight"] = "float";
    pImpl->units["stdp_ltp_weight"] = "weight [0.0, 1.0]";
    pImpl->examples["stdp_ltp_weight"] = "0.01, 0.02, 0.05";
    
    pImpl->descriptions["stdp_ltd_weight"] = "STDP long-term depression weight";
    pImpl->types["stdp_ltd_weight"] = "float";
    pImpl->units["stdp_ltd_weight"] = "weight [0.0, 1.0]";
    pImpl->examples["stdp_ltd_weight"] = "0.01, 0.02, 0.05";
    
    pImpl->descriptions["stdp_tau"] = "STDP time constant";
    pImpl->types["stdp_tau"] = "float";
    pImpl->units["stdp_tau"] = "milliseconds";
    pImpl->examples["stdp_tau"] = "10, 20, 50";
    
    // Development parameters
    pImpl->descriptions["synaptogenesis_rate"] = "Rate of synapse formation";
    pImpl->types["synaptogenesis_rate"] = "float";
    pImpl->units["synaptogenesis_rate"] = "rate per step";
    pImpl->examples["synaptogenesis_rate"] = "0.0001, 0.001, 0.01";
    
    pImpl->descriptions["pruning_rate"] = "Rate of synapse removal";
    pImpl->types["pruning_rate"] = "float";
    pImpl->units["pruning_rate"] = "rate per step";
    pImpl->examples["pruning_rate"] = "0.00001, 0.0001, 0.001";
    
    // Random seed
    pImpl->descriptions["random_seed"] = "Random seed for reproducibility";
    pImpl->types["random_seed"] = "int64_t";
    pImpl->units["random_seed"] = "seed value";
    pImpl->examples["random_seed"] = "42, 12345, 0";
}

// ConfigErrorReporter implementation
std::string ConfigErrorReporter::formatError(const ValidationError& error) {
    return formatError(error, true);
}

std::string ConfigErrorReporter::formatError(const ValidationError& error, bool includeSuggestion) {
    std::ostringstream oss;
    oss << "Error: ";
    
    switch (error.result) {
        case ValidationResult::Invalid:
            oss << "Invalid value for '" << error.key << "': " << error.message;
            break;
        case ValidationResult::Warning:
            oss << "Warning: Invalid value for '" << error.key << "': " << error.message;
            break;
        case ValidationResult::Valid:
            oss << "Value for '" << error.key << "' is valid: " << error.message;
            break;
    }
    
    if (includeSuggestion && !error.suggestion.empty()) {
        oss << "\n  Suggestion: " << error.suggestion;
    }
    
    return oss.str();
}

void ConfigErrorReporter::printErrors(const std::vector<ValidationError>& errors, 
                                      std::ostream& out) {
    for (const auto& error : errors) {
        if (error.result == ValidationResult::Invalid) {
            out << formatError(error) << std::endl;
        }
    }
}

void ConfigErrorReporter::printWarnings(const std::vector<ValidationError>& errors,
                                        std::ostream& out) {
    for (const auto& error : errors) {
        if (error.result == ValidationResult::Warning) {
            out << formatError(error) << std::endl;
        }
    }
}

bool ConfigErrorReporter::hasErrors(const std::vector<ValidationError>& errors) {
    return std::any_of(errors.begin(), errors.end(),
                      [](const ValidationError& e) { return e.result == ValidationResult::Invalid; });
}

bool ConfigErrorReporter::hasWarnings(const std::vector<ValidationError>& errors) {
    return std::any_of(errors.begin(), errors.end(),
                      [](const ValidationError& e) { return e.result == ValidationResult::Warning; });
}

size_t ConfigErrorReporter::countErrors(const std::vector<ValidationError>& errors) {
    return std::count_if(errors.begin(), errors.end(),
                        [](const ValidationError& e) { return e.result == ValidationResult::Invalid; });
}

size_t ConfigErrorReporter::countWarnings(const std::vector<ValidationError>& errors) {
    return std::count_if(errors.begin(), errors.end(),
                        [](const ValidationError& e) { return e.result == ValidationResult::Warning; });
}

// Configuration validation helpers implementation
namespace ConfigValidation {

std::vector<ValidationError> validateBasicConfiguration(const Config& config) {
    std::vector<ValidationError> errors;
    
    // Validate neuron count
    if (config.has("neuron_count")) {
        auto neuronsOpt = config.get<size_t>("neuron_count");
        if (neuronsOpt && *neuronsOpt < 10) {
            errors.emplace_back("neuron_count", 
                               "Neuron count should be at least 10 for meaningful simulation",
                               ValidationResult::Warning,
                               "Try increasing neuron_count to 100 or more");
        }
    }
    
    // Validate connection probability
    if (config.has("connection_probability")) {
        auto probOpt = config.get<double>("connection_probability");
        if (probOpt && (*probOpt < 0.0 || *probOpt > 1.0)) {
            errors.emplace_back("connection_probability",
                               "Connection probability must be between 0.0 and 1.0",
                               ValidationResult::Invalid,
                               "Try setting connection_probability to 0.1 for sparse connectivity");
        }
    }
    
    // Validate simulation timestep
    if (config.has("simulation_timestep")) {
        auto timestepOpt = config.get<double>("simulation_timestep");
        if (timestepOpt && *timestepOpt <= 0.0) {
            errors.emplace_back("simulation_timestep",
                               "Simulation timestep must be positive",
                               ValidationResult::Invalid,
                               "Try setting simulation_timestep to 0.001");
        }
    }
    
    return errors;
}

std::vector<ValidationError> validateBrainConfiguration(const Config& config) {
    std::vector<ValidationError> errors;
    
    // Validate neuron count vs region count ratio
    if (config.has("neuron_count") && config.has("region_count")) {
        auto neuronsOpt = config.get<size_t>("neuron_count");
        auto regionsOpt = config.get<size_t>("region_count");
        if (neuronsOpt && regionsOpt && *regionsOpt > 0) {
            double neuronsPerRegion = static_cast<double>(*neuronsOpt) / *regionsOpt;
            if (neuronsPerRegion < 10) {
                errors.emplace_back("neuron_count/region_count",
                                   "Too few neurons per region for meaningful dynamics",
                                   ValidationResult::Warning,
                                   "Increase neuron_count or decrease region_count");
            }
        }
    }
    
    return errors;
}

std::vector<ValidationError> validatePlasticityConfiguration(const Config& config) {
    std::vector<ValidationError> errors;
    
    // Validate STDP parameters
    if (config.has("stdp_ltp_weight") && config.has("stdp_ltd_weight")) {
        auto ltpOpt = config.get<double>("stdp_ltp_weight");
        auto ltdOpt = config.get<double>("stdp_ltd_weight");
        if (ltpOpt && ltdOpt) {
            if (*ltpOpt <= 0 || *ltdOpt <= 0) {
                errors.emplace_back("STDP weights",
                                   "STDP weights must be positive",
                                   ValidationResult::Invalid,
                                   "Try setting both stdp_ltp_weight and stdp_ltd_weight to positive values");
            }
        }
    }
    
    return errors;
}

std::vector<ValidationError> validateMemoryConfiguration(const Config& config) {
    std::vector<ValidationError> errors;
    // Memory configuration validation would go here
    return errors;
}

std::vector<ValidationError> validateNeuromodulationConfiguration(const Config& config) {
    std::vector<ValidationError> errors;
    // Neuromodulation configuration validation would go here
    return errors;
}

std::string getConfigurationRecommendations(const Config& config) {
    std::ostringstream oss;
    oss << "Configuration Recommendations:\n";
    oss << "  - For research simulations: neuron_count=1000, region_count=2\n";
    oss << "  - For real-time applications: neuron_count=100, region_count=1\n";
    oss << "  - For educational demonstrations: neuron_count=50, region_count=1\n";
    oss << "  - For detailed connectivity: connection_probability=0.2\n";
    oss << "  - For sparse connectivity: connection_probability=0.1\n";
    oss << "  - For fast simulations: simulation_timestep=0.01\n";
    oss << "  - For detailed simulations: simulation_timestep=0.001\n";
    
    return oss.str();
}

std::string getPerformanceRecommendations(const Config& config) {
    std::ostringstream oss;
    oss << "Performance Recommendations:\n";
    
    if (config.has("neuron_count")) {
        auto neuronsOpt = config.get<size_t>("neuron_count");
        if (neuronsOpt) {
            if (*neuronsOpt > 5000) {
                oss << "  - Consider using simulation_timestep=0.01 for better performance\n";
            }
        }
    }
    
    oss << "  - Monitor memory usage during long simulations\n";
    oss << "  - Use checkpoint files to save/restore brain state\n";
    oss << "  - Consider running with --benchmark for performance analysis\n";
    
    return oss.str();
}

std::string getMemoryRecommendations(const Config& config) {
    std::ostringstream oss;
    oss << "Memory Recommendations:\n";
    
    if (config.has("neuron_count")) {
        auto neuronsOpt = config.get<size_t>("neuron_count");
        if (neuronsOpt) {
            oss << "  - For " << *neuronsOpt << " neurons: expect ~" << (*neuronsOpt * 100) / 1024 << " KB memory usage\n";
        }
    }
    
    oss << "  - Use checkpointing for long-running simulations\n";
    oss << "  - Monitor memory usage during learning phases\n";
    
    return oss.str();
}

void setupDefaultValidations(ConfigValidator& validator) {
    // Add basic configuration validations
    validator.addValidation("neuron_count", ConfigValidator::validatePositiveInt,
                          "Neuron count must be positive", ValidationResult::Invalid,
                          "Try setting neuron_count to 100");
    
    validator.addValidation("region_count", ConfigValidator::validatePositiveInt,
                          "Region count must be positive", ValidationResult::Invalid,
                          "Try setting region_count to 1");
    
    validator.addValidation("connection_probability", ConfigValidator::validateProbability,
                          "Connection probability must be between 0.0 and 1.0", ValidationResult::Invalid,
                          "Try setting connection_probability to 0.1");
    
    validator.addValidation("simulation_timestep", ConfigValidator::validatePositiveDouble,
                          "Simulation timestep must be positive", ValidationResult::Invalid,
                          "Try setting simulation_timestep to 0.001");
    
    validator.addValidation("stdp_ltp_weight", ConfigValidator::validatePositiveDouble,
                          "STDP LTP weight must be positive", ValidationResult::Warning,
                          "Try setting stdp_ltp_weight to 0.01");
    
    validator.addValidation("stdp_ltd_weight", ConfigValidator::validatePositiveDouble,
                          "STDP LTD weight must be positive", ValidationResult::Warning,
                          "Try setting stdp_ltd_weight to 0.01");
    
    validator.addValidation("synaptogenesis_rate", ConfigValidator::validatePositiveDouble,
                          "Synaptogenesis rate must be positive", ValidationResult::Warning,
                          "Try setting synaptogenesis_rate to 0.0001");
    
    validator.addValidation("pruning_rate", ConfigValidator::validatePositiveDouble,
                          "Pruning rate must be positive", ValidationResult::Warning,
                          "Try setting pruning_rate to 0.00001");
}

void setupBrainValidations(ConfigValidator& validator) {
    validator.addValidation("neuron_count", ConfigValidator::validatePositiveInt,
                          "Neuron count must be positive", ValidationResult::Invalid,
                          "Try setting neuron_count to 100");
    
    validator.addValidation("region_count", ConfigValidator::validatePositiveInt,
                          "Region count must be positive", ValidationResult::Invalid,
                          "Try setting region_count to 1");
}

void setupLearningValidations(ConfigValidator& validator) {
    validator.addValidation("stdp_ltp_weight", ConfigValidator::validatePositiveDouble,
                          "STDP LTP weight must be positive", ValidationResult::Warning,
                          "Try setting stdp_ltp_weight to 0.01");
    
    validator.addValidation("stdp_ltd_weight", ConfigValidator::validatePositiveDouble,
                          "STDP LTD weight must be positive", ValidationResult::Warning,
                          "Try setting stdp_ltd_weight to 0.01");
}

} // namespace ConfigValidation

} // namespace nlm
