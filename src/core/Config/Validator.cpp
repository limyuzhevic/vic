#include "Validator.hpp"
#include <cmath>
#include <regex>
#include <algorithm>
#include <numeric>

namespace nlm {

struct ConfigValidator::Impl {
    std::unordered_map<std::string, std::vector<ValidationRule>> rules;
    std::unordered_map<std::string, std::string> descriptions;
    std::unordered_set<std::string> categories;
    
    // Dependency rules
    std::unordered_map<std::string, std::vector<std::string>> dependencies;
    std::unordered_map<std::string, std::string> dependencyErrors;
};

ConfigValidator::ConfigValidator() : pImpl(std::make_unique<Impl>()) {
    initializeDefaultRules();
}

ConfigValidator::~ConfigValidator() = default;

std::vector<std::string> ConfigValidator::validate(const Config& config) const {
    std::vector<std::string> errors;
    
    // Check all keys in config
    for (const auto& key : config.getKeys()) {
        auto value = config.get<ConfigValue>(key);
        if (value) {
            std::vector<std::string> keyErrors = validateKey(key, *value);
            errors.insert(errors.end(), keyErrors.begin(), keyErrors.end());
        }
    }
    
    // Check for missing required keys
    for (const auto& [key, rule] : pImpl->rules) {
        if (rule.required && !config.has(key)) {
            errors.push_back("Required configuration key missing: " + key);
        }
    }
    
    // Validate dependencies
    std::string dependencyError;
    if (!validateDependencies(config, dependencyError)) {
        errors.push_back(dependencyError);
    }
    
    return errors;
}

std::vector<std::string> ConfigValidator::validateKey(const std::string& key, const ConfigValue& value) const {
    std::vector<std::string> errors;
    
    if (!hasValidationRules(key)) {
        return errors;  // No rules to validate against
    }
    
    const auto& rules = pImpl->rules.at(key);
    
    for (const auto& rule : rules) {
        std::string error;
        
        if (!validateType(key, value, error)) {
            errors.push_back(error);
        }
        
        if (!validateRange(key, value, error)) {
            errors.push_back(error);
        }
        
        if (!validateFormat(key, value, error)) {
            errors.push_back(error);
        }
    }
    
    return errors;
}

void ConfigValidator::addValidationRule(const std::string& key,
                                       const std::string& type,
                                       const std::string& validation,
                                       const std::string& category,
                                       double minValue,
                                       double maxValue,
                                       bool required,
                                       const std::string& description) {
    ValidationRule rule;
    rule.type = type;
    rule.validation = validation;
    rule.category = category;
    rule.minValue = minValue;
    rule.maxValue = maxValue;
    rule.required = required;
    rule.description = description;
    
    pImpl->rules[key].push_back(rule);
    
    if (!description.empty()) {
        pImpl->descriptions[key] = description;
    }
    
    if (!category.empty()) {
        pImpl->categories.insert(category);
    }
}

std::vector<std::string> ConfigValidator::getValidationRules(const std::string& key) const {
    std::vector<std::string> rules;
    
    if (pImpl->rules.find(key) != pImpl->rules.end()) {
        for (const auto& rule : pImpl->rules.at(key)) {
            std::string ruleStr = rule.type;
            if (!rule.validation.empty()) {
                ruleStr += "(";
                ruleStr += rule.validation;
                ruleStr += ")";
            }
            if (!rule.category.empty()) {
                ruleStr += " [" + rule.category + "]";
            }
            rules.push_back(ruleStr);
        }
    }
    
    return rules;
}

std::unordered_map<std::string, std::vector<std::string>> ConfigValidator::getAllValidationRules() const {
    return pImpl->rules;
}

std::string ConfigValidator::getErrorMessage(const std::string& key, const std::string& errorType,
                                             const ConfigValue& value) const {
    std::string message = "Configuration error for key '" + key + "': ";
    
    if (errorType == "type") {
        message += "Invalid type. Expected ";
        auto rules = getValidationRules(key);
        if (!rules.empty()) {
            message += rules[0];
        } else {
            message += "valid value type";
        }
    } else if (errorType == "range") {
        message += "Value out of range. Expected between ";
        auto rules = getValidationRules(key);
        if (!rules.empty()) {
            size_t pos = rules[0].find('(');
            if (pos != std::string::npos) {
                message += rules[0].substr(pos + 1, rules[0].size() - pos - 2);
            }
        }
    } else if (errorType == "format") {
        message += "Invalid format. Expected pattern: ";
        auto rules = getValidationRules(key);
        if (!rules.empty()) {
            size_t pos = rules[0].find('(');
            if (pos != std::string::npos) {
                message += rules[0].substr(pos + 1, rules[0].size() - pos - 2);
            }
        }
    } else {
        message += "Validation failed.";
    }
    
    message += ". Current value: ";
    
    std::visit([&message](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            message += "\"" + arg + "\"";
        } else {
            message += std::to_string(arg);
        }
    }, value);
    
    return message;
}

bool ConfigValidator::hasValidationRules(const std::string& key) const {
    return pImpl->rules.find(key) != pImpl->rules.end();
}

std::vector<std::string> ConfigValidator::getCategories() const {
    return std::vector<std::string>(pImpl->categories.begin(), pImpl->categories.end());
}

std::string ConfigValidator::getDescription(const std::string& key) const {
    auto it = pImpl->descriptions.find(key);
    if (it != pImpl->descriptions.end()) {
        return it->second;
    }
    return "";
}

void ConfigValidator::initializeDefaultRules() {
    // Brain configuration rules
    addValidationRule("brain.neuron_count", "int", "[100, 100000]", "brain",
                      100.0, 100000.0, true, "Number of neurons in the brain");
    
    addValidationRule("brain.region_count", "int", "[1, 100]", "brain",
                      1.0, 100.0, true, "Number of brain regions");
    
    addValidationRule("brain.connection_probability", "double", "[0.0, 1.0]", "brain",
                      0.0, 1.0, true, "Connection probability for random connectivity");
    
    addValidationRule("brain.initial_weight_mean", "double", "[-1.0, 1.0]", "brain",
                      -1.0, 1.0, true, "Mean initial synaptic weight");
    
    addValidationRule("brain.initial_weight_std", "double", "[0.0, 2.0]", "brain",
                      0.0, 2.0, true, "Standard deviation of initial weights");
    
    addValidationRule("brain.v_thresh", "double", "[-100.0, -20.0]", "brain",
                      -100.0, -20.0, true, "Neuron threshold potential (mV)");
    
    addValidationRule("brain.v_rest", "double", "[-100.0, 0.0]", "brain",
                      -100.0, 0.0, true, "Resting potential (mV)");
    
    addValidationRule("brain.v_reset", "double", "[-100.0, 0.0]", "brain",
                      -100.0, 0.0, true, "Reset potential after spike (mV)");
    
    addValidationRule("brain.tau_mem", "double", "[1.0, 100.0]", "brain",
                      1.0, 100.0, true, "Membrane time constant (ms)");
    
    addValidationRule("brain.tau_ref", "double", "[0.0, 20.0]", "brain",
                      0.0, 20.0, true, "Refractory period (ms)");
    
    // Plasticity configuration rules
    addValidationRule("plasticity.stdp.enable", "bool", "", "plasticity",
                      0.0, 1.0, true, "Enable STDP plasticity");
    
    addValidationRule("plasticity.stdp.learning_rate", "double", "[0.0, 1.0]", "plasticity",
                      0.0, 1.0, true, "STDP learning rate");
    
    addValidationRule("plasticity.stdp.tau_plus", "double", "[1.0, 100.0]", "plasticity",
                      1.0, 100.0, true, "STDP time constant for potentiation (ms)");
    
    addValidationRule("plasticity.stdp.tau_minus", "double", "[1.0, 100.0]", "plasticity",
                      1.0, 100.0, true, "STDP time constant for depression (ms)");
    
    addValidationRule("plasticity.hebbian.enable", "bool", "", "plasticity",
                      0.0, 1.0, true, "Enable Hebbian plasticity");
    
    addValidationRule("plasticity.hebbian.learning_rate", "double", "[0.0, 1.0]", "plasticity",
                      0.0, 1.0, true, "Hebbian learning rate");
    
    addValidationRule("plasticity.structural.enable", "bool", "", "plasticity",
                      0.0, 1.0, true, "Enable structural plasticity");
    
    addValidationRule("plasticity.structural.synaptogenesis_rate", "double", "[0.0, 1.0]", "plasticity",
                      0.0, 1.0, true, "Synaptogenesis rate");
    
    addValidationRule("plasticity.structural.pruning_rate", "double", "[0.0, 1.0]", "plasticity",
                      0.0, 1.0, true, "Pruning rate");
    
    // Neuromodulation configuration rules
    addValidationRule("neuromod.dopamine.scale", "double", "[0.0, 10.0]", "neuromodulation",
                      0.0, 10.0, true, "Dopamine modulation scale");
    
    addValidationRule("neuromod.curiosity.enable", "bool", "", "neuromodulation",
                      0.0, 1.0, true, "Enable curiosity-driven exploration");
    
    addValidationRule("neuromod.novelty.enable", "bool", "", "neuromodulation",
                      0.0, 1.0, true, "Enable novelty detection");
    
    addValidationRule("neuromod.reward.scale", "double", "[0.0, 10.0]", "neuromodulation",
                      0.0, 10.0, true, "Reward modulation scale");
    
    // Development configuration rules
    addValidationRule("development.enable", "bool", "", "development",
                      0.0, 1.0, true, "Enable developmental processes");
    
    addValidationRule("development.critical_period_length", "double", "[0.0, 1000.0]", "development",
                      0.0, 1000.0, true, "Critical period length (ms)");
    
    addValidationRule("development.maturation_rate", "double", "[0.0, 1.0]", "development",
                      0.0, 1.0, true, "Brain maturation rate");
    
    addValidationRule("development.aging_rate", "double", "[0.0, 0.1]", "development",
                      0.0, 0.1, true, "Brain aging rate");
    
    // Agent configuration rules
    addValidationRule("agent.enable_reward_modulation", "bool", "", "agent",
                      0.0, 1.0, true, "Enable reward modulation");
    
    addValidationRule("agent.enable_structural_plasticity", "bool", "", "agent",
                      0.0, 1.0, true, "Enable structural plasticity");
    
    addValidationRule("agent.enable_development", "bool", "", "agent",
                      0.0, 1.0, true, "Enable development");
    
    addValidationRule("agent.enable_curiosity", "bool", "", "agent",
                      0.0, 1.0, true, "Enable curiosity");
    
    addValidationRule("agent.exploration_rate", "double", "[0.0, 1.0]", "agent",
                      0.0, 1.0, true, "Agent exploration rate");
    
    // Environment configuration rules
    addValidationRule("environment.name", "string", "", "environment",
                      0.0, std::numeric_limits<double>::max(), true, "Environment name");
    
    addValidationRule("environment.width", "int", "[1, 1000]", "environment",
                      1.0, 1000.0, true, "Environment width");
    
    addValidationRule("environment.height", "int", "[1, 1000]", "environment",
                      1.0, 1000.0, true, "Environment height");
    
    addValidationRule("environment.max_energy", "double", "[0.0, 10000.0]", "environment",
                      0.0, 10000.0, true, "Maximum energy in environment");
    
    addValidationRule("environment.vision_width", "int", "[1, 100]", "environment",
                      1.0, 100.0, true, "Agent vision width");
    
    addValidationRule("environment.vision_height", "int", "[1, 100]", "environment",
                      1.0, 100.0, true, "Agent vision height");
    
    // Simulation configuration rules
    addValidationRule("simulation.max_steps", "int", "[1, 1000000]", "simulation",
                      1.0, 1000000.0, true, "Maximum simulation steps");
    
    addValidationRule("simulation.time_limit", "double", "[0.0, 1000.0]", "simulation",
                      0.0, 1000.0, true, "Simulation time limit (s)");
    
    addValidationRule("simulation.verbose", "bool", "", "simulation",
                      0.0, 1.0, true, "Enable verbose logging");
    
    // Legacy configuration rules (for backward compatibility)
    addValidationRule("random_seed", "int", "[0, 2147483647]", "legacy",
                      0.0, 2147483647.0, true, "Random seed for reproducible experiments");
    
    addValidationRule("simulation_timestep", "double", "[0.0001, 1.0]", "legacy",
                      0.0001, 1.0, true, "Simulation timestep in seconds");
    
    addValidationRule("neuron_count", "int", "[10, 100000]", "legacy",
                      10.0, 100000.0, true, "Number of neurons in the brain");
    
    addValidationRule("region_count", "int", "[1, 100]", "legacy",
                      1.0, 100.0, true, "Number of brain regions");
    
    addValidationRule("connection_probability", "double", "[0.0, 1.0]", "legacy",
                      0.0, 1.0, true, "Connection probability for random connectivity");
    
    // Set dependency rules
    pImpl->dependencies["plasticity.stdp.tau_plus"] = {"plasticity.stdp.enable"};
    pImpl->dependencies["plasticity.stdp.tau_minus"] = {"plasticity.stdp.enable"};
    pImpl->dependencies["plasticity.hebbian.learning_rate"] = {"plasticity.hebbian.enable"};
    pImpl->dependencies["plasticity.structural.synaptogenesis_rate"] = {"plasticity.structural.enable"};
    pImpl->dependencies["plasticity.structural.pruning_rate"] = {"plasticity.structural.enable"};
    
    pImpl->dependencyErrors["plasticity.stdp.enable"] = "STDP time constants require STDP to be enabled";
    pImpl->dependencyErrors["plasticity.hebbian.enable"] = "Hebbian learning rate requires Hebbian learning to be enabled";
    pImpl->dependencyErrors["plasticity.structural.enable"] = "Structural plasticity rates require structural plasticity to be enabled";
}

bool ConfigValidator::validateType(const std::string& key, const ConfigValue& value, std::string& error) const {
    auto it = pImpl->rules.find(key);
    if (it == pImpl->rules.end()) {
        return true;  // No rules, skip validation
    }
    
    const std::string& expectedType = it->second[0].type;
    
    bool isValid = false;
    
    if (expectedType == "int") {
        isValid = std::holds_alternative<int>(value) || 
                  std::holds_alternative<int64_t>(value);
    } else if (expectedType == "double") {
        isValid = std::holds_alternative<double>(value);
    } else if (expectedType == "string") {
        isValid = std::holds_alternative<std::string>(value);
    } else if (expectedType == "bool") {
        isValid = std::holds_alternative<bool>(value);
    }
    
    if (!isValid) {
        error = getErrorMessage(key, "type", value);
    }
    
    return isValid;
}

bool ConfigValidator::validateRange(const std::string& key, const ConfigValue& value, std::string& error) const {
    auto it = pImpl->rules.find(key);
    if (it == pImpl->rules.end()) {
        return true;  // No rules, skip validation
    }
    
    double actualValue = 0.0;
    
    if (std::holds_alternative<int>(value)) {
        actualValue = static_cast<double>(std::get<int>(value));
    } else if (std::holds_alternative<int64_t>(value)) {
        actualValue = static_cast<double>(std::get<int64_t>(value));
    } else if (std::holds_alternative<double>(value)) {
        actualValue = std::get<double>(value);
    } else if (std::holds_alternative<bool>(value)) {
        actualValue = std::get<bool>(value) ? 1.0 : 0.0;
    }
    
    bool inRange = (actualValue >= it->second[0].minValue && 
                   actualValue <= it->second[0].maxValue);
    
    if (!inRange) {
        error = getErrorMessage(key, "range", value);
    }
    
    return inRange;
}

bool ConfigValidator::validateFormat(const std::string& key, const ConfigValue& value, std::string& error) const {
    auto it = pImpl->rules.find(key);
    if (it == pImpl->rules.end()) {
        return true;  // No rules, skip validation
    }
    
    const std::string& validation = it->second[0].validation;
    if (validation.empty()) {
        return true;  // No format validation
    }
    
    std::string actualValueStr;
    if (std::holds_alternative<std::string>(value)) {
        actualValueStr = std::get<std::string>(value);
    } else if (std::holds_alternative<int>(value)) {
        actualValueStr = std::to_string(std::get<int>(value));
    } else if (std::holds_alternative<int64_t>(value)) {
        actualValueStr = std::to_string(std::get<int64_t>(value));
    } else if (std::holds_alternative<double>(value)) {
        actualValueStr = std::to_string(std::get<double>(value));
    } else if (std::holds_alternative<bool>(value)) {
        actualValueStr = std::get<bool>(value) ? "true" : "false";
    }
    
    bool isValid = false;
    
    if (validation == "[0.0, 1.0]") {
        // Range validation (already done in range validation)
        isValid = true;
    } else if (validation.rfind("[", 0) == 0 && validation.back() == ']') {
        // Generic range pattern
        isValid = true;  // Range validation already done
    } else if (validation == "") {
        isValid = true;
    }
    
    if (!isValid) {
        error = getErrorMessage(key, "format", value);
    }
    
    return isValid;
}

bool ConfigValidator::validateDependencies(const Config& config, std::string& error) const {
    for (const auto& [dependentKey, dependencies] : pImpl->dependencies) {
        for (const auto& depKey : dependencies) {
            if (config.has(depKey)) {
                auto depValue = config.get<ConfigValue>(depKey);
                if (depValue) {
                    if (std::holds_alternative<bool>(*depValue) && !std::get<bool>(*depValue)) {
                        error = "Dependency violation: " + dependentKey + 
                               " requires " + depKey + " to be enabled";
                        return false;
                    }
                }
            }
        }
    }
    
    return true;
}

} // namespace nlm
