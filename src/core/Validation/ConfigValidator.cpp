#include "ConfigValidator.hpp"
#include "Config.hpp"
#include <algorithm>
#include <sstream>
#include <regex>
#include <unordered_set>
#include <chrono>
#include <cmath>

namespace nlm {

struct ConfigValidator::Impl {
    ValidationOptions options;
    std::vector<std::pair<std::string, ConfigValueSchema>> cachedParameters;
    mutable ValidationStatistics stats;
    std::chrono::system_clock::time_point lastValidationTime;
    bool cacheValid;
    
    Impl() : cacheValid(false), lastValidationTime(std::chrono::system_clock::now()) {
        options.validateTypes = true;
        options.validateRanges = true;
        options.validateDependencies = true;
        options.validateConsistency = true;
        options.failFast = false;
        options.includeWarnings = true;
        options.logErrors = true;
    }
};

ConfigValidator::ConfigValidator() : pImpl(std::make_unique<Impl>()) {}

ConfigValidator::~ConfigValidator() = default;

void ConfigValidator::setOptions(const ValidationOptions& options) {
    pImpl->options = options;
}

ValidationOptions ConfigValidator::getOptions() const {
    return pImpl->options;
}

ValidationResult ConfigValidator::validateParameter(
    const std::string& name,
    const ConfigValueSchema& value,
    const std::string& source
) const {
    pImpl->stats.totalChecks++;
    
    // Check if parameter exists
    if (!gConfigurationSchema.hasParameter(name)) {
        pImpl->stats.failedChecks++;
        pImpl->stats.parameterFailures[name]++;
        return ValidationResult(false, 
            "Parameter \"" + name + "\" is not defined in configuration schema", name);
    }
    
    const ParameterInfo* paramInfo = gConfigurationSchema.getParameter(name);
    
    // Check type
    if (pImpl->options.validateTypes && !validateAgainstSchema(name, value, false)) {
        pImpl->stats.failedChecks++;
        pImpl->stats.parameterFailures[name]++;
        std::string error = getTypeErrorMessage(name, value);
        if (pImpl->options.logErrors) {
            // In a real implementation, this would log to a logger
        }
        return ValidationResult(false, error, name);
    }
    
    // Check constraints
    if (pImpl->options.validateRanges && hasConstraints(name)) {
        std::vector<Constraint> constraints = getParameterConstraints(name);
        for (const auto& constraint : constraints) {
            if (!validateAgainstConstraint(name, value, constraint)) {
                pImpl->stats.failedChecks++;
                pImpl->stats.parameterFailures[name]++;
                std::string error = getConstraintErrorMessage(constraint, value);
                if (pImpl->options.logErrors) {
                    // In a real implementation, this would log to a logger
                }
                return ValidationResult(false, error, name);
            }
        }
    }
    
    // Check format
    if (!validateParameterFormat(name, value)) {
        pImpl->stats.failedChecks++;
        pImpl->stats.parameterFailures[name]++;
        std::string error = getFormatErrorMessage(name, value);
        if (pImpl->options.logErrors) {
            // In a real implementation, this would log to a logger
        }
        return ValidationResult(false, error, name);
    }
    
    pImpl->stats.passedChecks++;
    return ValidationResult(true, "", name);
}

std::vector<ValidationResult> ConfigValidator::validateParameters(
    const std::vector<std::pair<std::string, ConfigValueSchema>>& parameters
) const {
    std::vector<ValidationResult> results;
    
    if (pImpl->options.failFast) {
        // Validate parameters one by one and stop on first error
        for (const auto& param : parameters) {
            ValidationResult result = validateParameter(param.first, param.second);
            if (!result.isValid) {
                results.push_back(result);
                break;
            }
            results.push_back(result);
        }
    } else {
        // Validate all parameters
        for (const auto& param : parameters) {
            ValidationResult result = validateParameter(param.first, param.second);
            results.push_back(result);
        }
    }
    
    pImpl->cachedParameters = parameters;
    pImpl->cacheValid = true;
    
    return results;
}

std::vector<ValidationResult> ConfigValidator::validateConfig(
    const std::vector<std::pair<std::string, ConfigValueSchema>>& parameters
) const {
    std::vector<ValidationResult> results;
    
    // Basic validation (type, range, format)
    std::vector<ValidationResult> basicResults = validateParameters(parameters);
    results.insert(results.end(), basicResults.begin(), basicResults.end());
    
    // Check for consistency
    if (pImpl->options.validateConsistency) {
        std::vector<ValidationResult> consistencyResults = checkConsistency(parameters);
        results.insert(results.end(), consistencyResults.begin(), consistencyResults.end());
    }
    
    // Check for dependencies
    if (pImpl->options.validateDependencies) {
        std::vector<ValidationResult> dependencyResults = validateDependencies(parameters);
        results.insert(results.end(), dependencyResults.begin(), dependencyResults.end());
    }
    
    return results;
}

std::vector<ValidationResult> ConfigValidator::checkConsistency(
    const std::vector<std::pair<std::string, ConfigValueSchema>>& parameters
) const {
    std::vector<ValidationResult> results;
    
    // Check for required parameters
    std::vector<ValidationResult> requiredResults = checkRequiredParameters(parameters);
    results.insert(results.end(), requiredResults.begin(), requiredResults.end());
    
    // Check for logical consistency
    for (const auto& param : parameters) {
        const ParameterInfo* paramInfo = getParameterInfo(param.first);
        if (!paramInfo) continue;
        
        // Check specific consistency rules
        if (param.first == "brain.v_thresh" || param.first == "brain.v_rest" || param.first == "brain.v_reset") {
            // Check that v_thresh > v_reset > v_rest
            std::optional<double> vThresh = std::get_if<double>(&param.second);
            std::optional<double> vRest = std::get_if<double>(&getParameterValue(parameters, "brain.v_rest"));
            std::optional<double> vReset = std::get_if<double>(&getParameterValue(parameters, "brain.v_reset"));
            
            if (vThresh && vReset && vReset.value() > vThresh.value()) {
                results.emplace_back(false,
                    "brain.v_reset (" + std::to_string(vReset.value()) + ") must be less than brain.v_thresh (" + std::to_string(vThresh.value()) + ")",
                    param.first);
            }
            
            if (vRest && vReset && vReset.value() > vRest.value()) {
                results.emplace_back(false,
                    "brain.v_reset (" + std::to_string(vReset.value()) + ") must be less than brain.v_rest (" + std::to_string(vRest.value()) + ")",
                    param.first);
            }
        }
        
        // Check plasticity settings consistency
        if (param.first == "plasticity.stdp.enable" && param.second.index() == typeid(bool)) {
            bool stdpEnabled = std::get<bool>(param.second);
            bool hebbianEnabled = getParameterValue(parameters, "plasticity.hebbian.enable");
            bool structuralEnabled = getParameterValue(parameters, "plasticity.structural.enable");
            
            if (stdpEnabled && !hebbianEnabled && !structuralEnabled) {
                results.emplace_back(false,
                    "STDP is enabled but both Hebbian and structural plasticity are disabled. Consider enabling at least one plasticity mechanism.",
                    param.first);
            }
        }
        
        // Check neuromodulation settings
        if (param.first == "neuromod.dopamine.scale") {
            double scale = std::get<double>(param.second);
            if (scale < 0.0) {
                results.emplace_back(false,
                    "neuromod.dopamine.scale must be non-negative (got " + std::to_string(scale) + ")",
                    param.first);
            }
        }
    }
    
    return results;
}

std::vector<ValidationResult> ConfigValidator::validateDependencies(
    const std::vector<std::pair<std::string, ConfigValueSchema>>& parameters
) const {
    std::vector<ValidationResult> results;
    
    // Check parameter dependencies
    for (const auto& param : parameters) {
        const ParameterInfo* paramInfo = getParameterInfo(param.first);
        if (!paramInfo) continue;
        
        // Check if parameter depends on others
        if (param.first == "plasticity.stdp.enable" && param.second.index() == typeid(bool)) {
            bool stdpEnabled = std::get<bool>(param.second);
            
            // If STDP is enabled, check that required parameters are set
            if (stdpEnabled) {
                std::string learningRateKey = "plasticity.stdp.learning_rate";
                if (!hasParameter(learningRateKey) || !getParameterValue(parameters, learningRateKey)) {
                    results.emplace_back(false,
                        "plasticity.stdp.enable requires plasticity.stdp.learning_rate to be set",
                        param.first);
                }
                
                std::string tauPlusKey = "plasticity.stdp.tau_plus";
                if (!hasParameter(tauPlusKey) || !getParameterValue(parameters, tauPlusKey)) {
                    results.emplace_back(false,
                        "plasticity.stdp.enable requires plasticity.stdp.tau_plus to be set",
                        param.first);
                }
            }
        }
        
        // Check development settings
        if (param.first == "development.synaptogenesis_rate") {
            double rate = std::get<double>(param.second);
            if (rate < 0.0 || rate > 1.0) {
                results.emplace_back(false,
                    "development.synaptogenesis_rate must be between 0.0 and 1.0 (got " + std::to_string(rate) + ")",
                    param.first);
            }
        }
        
        if (param.first == "development.pruning_rate") {
            double rate = std::get<double>(param.second);
            if (rate < 0.0 || rate > 1.0) {
                results.emplace_back(false,
                    "development.pruning_rate must be between 0.0 and 1.0 (got " + std::to_string(rate) + ")",
                    param.first);
            }
        }
        
        // Check log level
        if (param.first == "log_level") {
            std::string level = std::get<std::string>(param.second);
            // Convert to lowercase for case-insensitive comparison
            std::transform(level.begin(), level.end(), level.begin(), ::tolower);
            if (level != "debug" && level != "info" && level != "warning" && level != "error" && level != "critical") {
                results.emplace_back(false,
                    "log_level must be one of: debug, info, warning, error, critical (got " + level + ")",
                    param.first);
            }
        }
    }
    
    return results;
}

std::vector<ValidationResult> ConfigValidator::validateRanges(
    const std::vector<std::pair<std::string, ConfigValueSchema>>& parameters
) const {
    std::vector<ValidationResult> results;
    
    for (const auto& param : parameters) {
        if (!hasConstraints(param.first)) continue;
        
        std::vector<Constraint> constraints = getParameterConstraints(param.first);
        for (const auto& constraint : constraints) {
            if (!validateAgainstConstraint(param.first, param.second, constraint)) {
                std::string error = getConstraintErrorMessage(constraint, param.second);
                results.emplace_back(false, error, param.first);
            }
        }
    }
    
    return results;
}

std::vector<ValidationResult> ConfigValidator::validateTypes(
    const std::vector<std::pair<std::string, ConfigValueSchema>>& parameters
) const {
    std::vector<ValidationResult> results;
    
    for (const auto& param : parameters) {
        const ParameterInfo* paramInfo = getParameterInfo(param.first);
        if (!paramInfo) continue;
        
        if (!isTypeMatch(param.first, param.second)) {
            std::string error = getTypeErrorMessage(param.first, param.second);
            results.emplace_back(false, error, param.first);
        }
    }
    
    return results;
}

const ParameterInfo* ConfigValidator::getParameterInfo(const std::string& name) const {
    return gConfigurationSchema.getParameter(name);
}

std::vector<std::string> ConfigValidator::getAllParameterNames() const {
    return gConfigurationSchema.getAllParameters() | 
           std::views::transform([](const ParameterInfo* param) { return param->name; }) |
           std::ranges::to<std::vector<std::string>>();
}

bool ConfigValidator::hasConstraints(const std::string& name) const {
    const ParameterInfo* paramInfo = getParameterInfo(name);
    return paramInfo && !paramInfo->constraints.empty();
}

std::vector<Constraint> ConfigValidator::getParameterConstraints(const std::string& name) const {
    const ParameterInfo* paramInfo = getParameterInfo(name);
    if (!paramInfo) return {};
    return paramInfo->constraints;
}

bool ConfigValidator::validateAgainstConstraint(
    const std::string& name,
    const ConfigValueSchema& value,
    const Constraint& constraint
) const {
    switch (constraint.type) {
        case Constraint::Type::Range:
            return isValueInRange(value, constraint.value1);
        case Constraint::Type::Enum:
            return isValueInEnum(value, constraint.value1);
        case Constraint::Type::Pattern:
            if (std::holds_alternative<std::string>(value)) {
                return matchesPattern(std::get<std::string>(value), constraint.value1);
            }
            return false;
        case Constraint::Type::Positive:
            return validatePositive(value);
        case Constraint::Type::NonNegative:
            return validateNonNegative(value);
        default:
            return true;
    }
}

std::string ConfigValidator::getConstraintErrorMessage(
    const Constraint& constraint,
    const ConfigValueSchema& value
) const {
    switch (constraint.type) {
        case Constraint::Type::Range:
            return "Value " + std::to_string(getNumericValue(value)) + 
                   " is outside valid range [" + constraint.value1 + ", " + constraint.value2 + "]";
        case Constraint::Type::Enum:
            return "Value is not in allowed enum: " + constraint.value1;
        case Constraint::Type::Pattern:
            return "Value does not match required pattern: " + constraint.value1;
        case Constraint::Type::Positive:
            return "Value must be positive";
        case Constraint::Type::NonNegative:
            return "Value must be non-negative";
        default:
            return "Value validation failed";
    }
}

bool ConfigValidator::isDependentParameter(const std::string& name, const std::string& dependsOn) const {
    const ParameterInfo* paramInfo = getParameterInfo(name);
    return paramInfo && paramInfo->validationGroup == dependsOn;
}

std::vector<std::string> ConfigValidator::getDependentParameters(const std::string& name) const {
    std::vector<std::string> dependents;
    for (const auto* param : gConfigurationSchema.getAllParameters()) {
        if (param->validationGroup == name) {
            dependents.push_back(param->name);
        }
    }
    return dependents;
}

std::vector<ValidationResult> ConfigValidator::validateCompleteness(
    const std::vector<std::pair<std::string, ConfigValueSchema>>& parameters
) const {
    std::vector<ValidationResult> results;
    
    for (const auto* param : gConfigurationSchema.getAllParameters()) {
        if (param->isRequired && !hasParameter(param->name)) {
            results.emplace_back(false,
                "Required parameter \"" + param->name + "\" is missing",
                param->name);
        }
    }
    
    return results;
}

std::vector<ValidationResult> ConfigValidator::checkRequiredParameters(
    const std::vector<std::pair<std::string, ConfigValueSchema>>& parameters
) const {
    return validateCompleteness(parameters);
}

bool ConfigValidator::validateAgainstSchema(
    const std::string& name,
    const ConfigValueSchema& value,
    bool checkConstraints
) const {
    const ParameterInfo* paramInfo = getParameterInfo(name);
    if (!paramInfo) return false;
    
    // Check type
    if (!isTypeMatch(name, value)) {
        return false;
    }
    
    // Check constraints
    if (checkConstraints && hasConstraints(name)) {
        std::vector<Constraint> constraints = getParameterConstraints(name);
        for (const auto& constraint : constraints) {
            if (!validateAgainstConstraint(name, value, constraint)) {
                return false;
            }
        }
    }
    
    return true;
}

std::string ConfigValidator::getParameterTypeName(const std::string& name) const {
    const ParameterInfo* paramInfo = getParameterInfo(name);
    if (!paramInfo) return "unknown";
    
    switch (value.index()) {
        case 0:  // int
        case 1:  // int64_t
            return "integer";
        case 2:  // double
            return "float/double";
        case 3:  // bool
            return "boolean";
        case 4:  // std::string
            return "string";
        case 5:  // std::vector<int>
            return "integer array";
        case 6:  // std::vector<double>
            return "float array";
        case 7:  // std::vector<std::string>
            return "string array";
        default:
            return "unknown";
    }
}

bool ConfigValidator::isTypeMatch(const std::string& name, const ConfigValueSchema& value) const {
    const ParameterInfo* paramInfo = getParameterInfo(name);
    if (!paramInfo) return false;
    
    // Get expected type from parameter info
    // In a real implementation, this would need to be stored in ParameterInfo
    // For now, we'll use a simple mapping based on parameter name patterns
    return true; // Placeholder - implement proper type checking
}

std::string ConfigValidator::getTypeErrorMessage(const std::string& name, const ConfigValueSchema& value) const {
    const ParameterInfo* paramInfo = getParameterInfo(name);
    if (!paramInfo) return "Unknown parameter type";
    
    return "Parameter \"" + name + "\" has invalid type. Expected " + 
           getParameterTypeName(name) + " but got " + typeToString(value);
}

bool ConfigValidator::validateValueRange(const std::string& name, const ConfigValueSchema& value) const {
    return true; // Delegate to constraint validation
}

std::string ConfigValidator::getRangeErrorMessage(const std::string& name, const ConfigValueSchema& value) const {
    return "Parameter \"" + name + "\" has value outside valid range";
}

bool ConfigValidator::validateNoDuplicateKeys(const std::vector<std::pair<std::string, ConfigValueSchema>>& parameters) const {
    std::unordered_set<std::string> keys;
    for (const auto& param : parameters) {
        if (keys.find(param.first) != keys.end()) {
            return false;
        }
        keys.insert(param.first);
    }
    return true;
}

std::string ConfigValidator::getDuplicateKeyErrorMessage(const std::string& key) const {
    return "Duplicate key \"" + key + "\" found in configuration";
}

bool ConfigValidator::validateParameterFormat(const std::string& name, const ConfigValueSchema& value) const {
    // Check format based on parameter name and value
    if (name == "log_level") {
        if (std::holds_alternative<std::string>(value)) {
            std::string level = std::get<std::string>(value);
            std::transform(level.begin(), level.end(), level.begin(), ::tolower);
            return level == "debug" || level == "info" || level == "warning" || level == "error" || level == "critical";
        }
        return false;
    }
    
    if (name == "environment_name") {
        if (std::holds_alternative<std::string>(value)) {
            std::string env = std::get<std::string>(value);
            return env == "GridWorld" || env == "ContinuousWorld" || env == "HybridWorld";
        }
        return false;
    }
    
    return true;
}

std::string ConfigValidator::getFormatErrorMessage(const std::string& name, const ConfigValueSchema& value) const {
    if (name == "log_level") {
        return "log_level must be one of: debug, info, warning, error, critical";
    }
    
    if (name == "environment_name") {
        return "environment_name must be one of: GridWorld, ContinuousWorld, HybridWorld";
    }
    
    return "Parameter format is invalid";
}

void ConfigValidator::clearCache() {
    pImpl->cachedParameters.clear();
    pImpl->cacheValid = false;
}

ConfigValidator::ValidationStatistics ConfigValidator::getStatistics() const {
    return pImpl->stats;
}

void ConfigValidator::resetStatistics() {
    pImpl->stats = ValidationStatistics();
}

// Helper function implementations
bool ConfigValidator::matchesPattern(const std::string& value, const std::string& pattern) const {
    std::regex regex(pattern);
    return std::regex_match(value, regex);
}

std::string ConfigValidator::typeToString(const ConfigValueSchema& value) const {
    switch (value.index()) {
        case 0:  // int
        case 1:  // int64_t
            return "integer";
        case 2:  // double
            return "float/double";
        case 3:  // bool
            return "boolean";
        case 4:  // std::string
            return "string";
        case 5:  // std::vector<int>
            return "integer array";
        case 6:  // std::vector<double>
            return "float array";
        case 7:  // std::vector<std::string>
            return "string array";
        default:
            return "unknown";
    }
}

bool ConfigValidator::isValueInEnum(const ConfigValueSchema& value, const std::string& enumValues) const {
    // Parse enum values (comma-separated)
    std::vector<std::string> enumList;
    std::string current;
    std::istringstream stream(enumValues);
    
    while (std::getline(stream, current, ',')) {
        enumList.push_back(current);
    }
    
    if (std::holds_alternative<std::string>(value)) {
        std::string val = std::get<std::string>(value);
        return std::find(enumList.begin(), enumList.end(), val) != enumList.end();
    }
    
    return false;
}

bool ConfigValidator::isValueInRange(const ConfigValueSchema& value, const std::string& range) const {
    // Parse range (min,max)
    size_t commaPos = range.find(',');
    if (commaPos == std::string::npos) return false;
    
    double min = std::stod(range.substr(0, commaPos));
    double max = std::stod(range.substr(commaPos + 1));
    
    return getNumericValue(value) >= min && getNumericValue(value) <= max;
}

bool ConfigValidator::validatePositive(const ConfigValueSchema& value) const {
    return getNumericValue(value) > 0.0;
}

bool ConfigValidator::validateNonNegative(const ConfigValueSchema& value) const {
    return getNumericValue(value) >= 0.0;
}

// External initialization
void initializeConfigurationSchema() {
    // In a real implementation, this would register all parameters
    // For now, it's a placeholder
}

// Global schema instance
ConfigurationSchema gConfigurationSchema;

} // namespace nlm
