/**
 * Configuration validation implementation for NLM
 * 
 * This provides runtime validation of configuration parameters,
 * consistency checking, and better error reporting for configuration files.
 */

#include "ConfigValidation.hpp"
#include "Config.hpp"
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <numeric>

namespace nlm {

struct ConfigValidator::Impl {
    std::map<std::string, ConfigParameter> parameters;
    std::map<std::string, std::string> helpText;
};

ConfigValidator::ConfigValidator() : pImpl(std::make_unique<Impl>()) {}
ConfigValidator::~ConfigValidator() = default;

void ConfigValidator::addParameter(const ConfigParameter& param) {
    pImpl->parameters[param.key] = param;
}

ValidationResult ConfigValidator::validate(const std::string& key, const std::string& value) const {
    // Check if parameter exists
    auto it = pImpl->parameters.find(key);
    if (it == pImpl->parameters.end()) {
        return ValidationResult(false, "Unknown configuration parameter: " + key);
    }
    
    const ConfigParameter& param = it->second;
    std::vector<std::string> warnings;
    
    // Type validation
    if (!ConfigUtils::matchesType(value, param.type)) {
        return ValidationResult(false, 
            "Invalid value type for parameter '" + key + "': expected " + param.type + ", got " + ConfigUtils::detectType(value));
    }
    
    // Range validation for numeric types
    if (param.type == "int" || param.type == "double" || param.type == "float") {
        std::optional<double> numValue = ConfigUtils::stringToDouble(value);
        if (!numValue.has_value()) {
            return ValidationResult(false, "Cannot convert value to number: " + value);
        }
        
        if (numValue.value() < param.minValue || numValue.value() > param.maxValue) {
            return ValidationResult(false,
                "Value out of range for parameter '" + key + "': " + value +
                ". Must be between " + std::to_string(param.minValue) + 
                " and " + std::to_string(param.maxValue));
        }
    }
    
    // Enum validation
    if (!param.validValues.empty()) {
        if (std::find(param.validValues.begin(), param.validValues.end(), value) == param.validValues.end()) {
            std::string validList = "";
            for (size_t i = 0; i < param.validValues.size(); ++i) {
                if (i > 0) validList += ", ";
                validList += "'" + param.validValues[i] + "'";
            }
            return ValidationResult(false,
                "Invalid value for parameter '" + key + "': '" + value + ". "
                "Valid values: " + validList);
        }
    }
    
    // Special validation for specific parameters
    if (key == "simulation_timestep" || key == "timestep") {
        double timestep = std::stod(value);
        if (timestep <= 0.0) {
            warnings.push_back("Simulation timestep should be positive for stable integration");
        } else if (timestep > 1.0) {
            warnings.push_back("Large timestep may cause numerical instability");
        }
    }
    
    if (key == "connection_probability") {
        double prob = std::stod(value);
        if (prob < 0.0 || prob > 1.0) {
            return ValidationResult(false,
                "Connection probability must be between 0.0 and 1.0");
        }
    }
    
    if (key == "neuron_count" || key == "brain.neuron_count") {
        int count = std::stoi(value);
        if (count <= 0) {
            return ValidationResult(false, "Neuron count must be positive");
        } else if (count > 100000) {
            warnings.push_back("Very large neuron count may require significant memory");
        }
    }
    
    if (key == "region_count" || key == "brain.region_count") {
        int count = std::stoi(value);
        if (count <= 0) {
            return ValidationResult(false, "Region count must be positive");
        } else if (count > 100) {
            warnings.push_back("Many regions may increase computational complexity");
        }
    }
    
    // Consistency validation
    ValidationResult consistencyResult = validateConsistency(value, key);
    if (!consistencyResult.valid) {
        return consistencyResult;
    }
    
    if (!warnings.empty()) {
        return ValidationResult(true, "", warnings);
    }
    
    return ValidationResult(true);
}

ValidationResult ConfigValidator::validateAll(const Config& config) const {
    std::vector<std::string> allErrors;
    std::vector<std::string> allWarnings;
    
    // Check all configured parameters
    for (const auto& entry : pImpl->parameters) {
        const std::string& key = entry.first;
        
        if (config.has(key)) {
            // Get actual value from config (simplified - assumes string values)
            std::optional<std::string> valueOpt = config.get<std::string>(key);
            if (valueOpt.has_value()) {
                ValidationResult result = validate(key, valueOpt.value());
                if (!result.valid) {
                    allErrors.push_back(result.errorMessage);
                }
                allWarnings.insert(allWarnings.end(), result.warnings.begin(), result.warnings.end());
            }
        } else if (entry.second.required) {
            allErrors.push_back("Missing required configuration parameter: " + key);
        }
    }
    
    // Check for parameter consistency
    ValidationResult consistencyResult = validateConsistency(config);
    if (!consistencyResult.valid) {
        allErrors.push_back(consistencyResult.errorMessage);
        allWarnings.insert(allWarnings.end(), consistencyResult.warnings.begin(), consistencyResult.warnings.end());
    }
    
    if (allErrors.empty()) {
        return ValidationResult(true, "", allWarnings);
    } else {
        std::string errorMsg = "Configuration validation failed:\n";
        for (const auto& error : allErrors) {
            errorMsg += "  - " + error + "\n";
        }
        return ValidationResult(false, errorMsg, allWarnings);
    }
}

std::vector<std::string> ConfigValidator::getAllKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->parameters.size());
    for (const auto& pair : pImpl->parameters) {
        keys.push_back(pair.first);
    }
    return keys;
}

std::optional<ConfigParameter> ConfigValidator::getParameter(const std::string& key) const {
    auto it = pImpl->parameters.find(key);
    if (it == pImpl->parameters.end()) {
        return std::nullopt;
    }
    return it->second;
}

std::vector<ConfigParameter> ConfigValidator::getParametersByCategory(const std::string& category) const {
    std::vector<ConfigParameter> result;
    for (const auto& pair : pImpl->parameters) {
        if (pair.second.category == category) {
            result.push_back(pair.second);
        }
    }
    return result;
}

std::string ConfigValidator::generateDocumentation() const {
    std::ostringstream oss;
    oss << "NLM Configuration Parameters" << std::endl;
    oss << "=" << std::string(30, '=') << std::endl << std::endl;
    
    // Group by category
    std::map<std::string, std::vector<ConfigParameter>> byCategory;
    for (const auto& pair : pImpl->parameters) {
        byCategory[pair.second.category].push_back(pair.second);
    }
    
    for (const auto& categoryPair : byCategory) {
        oss << categoryPair.first << " Parameters" << std::endl;
        oss << std::string(categoryPair.first.size() + 4, '-') << std::endl << std::endl;
        
        for (const auto& param : categoryPair.second) {
            oss << param.key << " (type: " << param.type << ")" << std::endl;
            oss << "  Description: " << param.description << std::endl;
            if (!param.defaultValue.empty()) {
                oss << "  Default: " << param.defaultValue;
                if (!param.unit.empty()) {
                    oss << " " << param.unit;
                }
                oss << std::endl;
            }
            if (!param.validValues.empty()) {
                oss << "  Valid values: ";
                for (size_t i = 0; i < param.validValues.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << "'" << param.validValues[i] << "'";
                }
                oss << std::endl;
            }
            if (param.minValue != std::numeric_limits<double>::lowest() || 
                param.maxValue != std::numeric_limits<double>::max()) {
                oss << "  Range: " << param.minValue << " to " << param.maxValue << std::endl;
            }
            oss << std::endl;
        }
        oss << std::endl;
    }
    
    return oss.str();
}

void ConfigValidator::loadStandardParameters() {
    // General simulation parameters
    {
        ConfigParameter param;
        param.key = "random_seed";
        param.description = "Random seed for reproducible experiments";
        param.type = "int";
        param.defaultValue = "42";
        param.unit = "(integer)";
        param.minValue = 0;
        param.maxValue = 2147483647;
        param.required = false;
        param.category = "Simulation";
        addParameter(param);
    }
    
    {
        ConfigParameter param;
        param.key = "simulation_timestep";
        param.description = "Simulation timestep in seconds";
        param.type = "float";
        param.defaultValue = "0.001";
        param.unit = "(seconds)";
        param.minValue = 1e-6;
        param.maxValue = 0.1;
        param.required = true;
        param.category = "Simulation";
        addParameter(param);
    }
    
    {
        ConfigParameter param;
        param.key = "max_simulation_steps";
        param.description = "Maximum number of simulation steps";
        param.type = "int";
        param.defaultValue = "10000";
        param.unit = "(steps)";
        param.minValue = 1;
        param.maxValue = 1000000;
        param.required = false;
        param.category = "Simulation";
        addParameter(param);
    }
    
    // Brain parameters
    {
        ConfigParameter param;
        param.key = "neuron_count";
        param.description = "Total number of neurons in the brain";
        param.type = "int";
        param.defaultValue = "1000";
        param.unit = "(neurons)";
        param.minValue = 1;
        param.maxValue = 1000000;
        param.required = true;
        param.category = "Brain";
        addParameter(param);
    }
    
    {
        ConfigParameter param;
        param.key = "region_count";
        param.description = "Number of brain regions";
        param.type = "int";
        param.defaultValue = "1";
        param.unit = "(regions)";
        param.minValue = 1;
        param.maxValue = 100;
        param.required = false;
        param.category = "Brain";
        addParameter(param);
    }
    
    {
        ConfigParameter param;
        param.key = "connection_probability";
        param.description = "Probability of connection between neurons (0.0 to 1.0)";
        param.type = "float";
        param.defaultValue = "0.1";
        param.unit = "(0.0 to 1.0)";
        param.minValue = 0.0;
        param.maxValue = 1.0;
        param.required = false;
        param.category = "Brain";
        addParameter(param);
    }
    
    // Plasticity parameters
    {
        ConfigParameter param;
        param.key = "stdp_ltp_weight";
        param.description = "STDP Long-Term Potentiation weight factor";
        param.type = "float";
        param.defaultValue = "0.01";
        param.unit = "(weight)";
        param.minValue = 0.0;
        param.maxValue = 1.0;
        param.required = false;
        param.category = "Plasticity";
        addParameter(param);
    }
    
    {
        ConfigParameter param;
        param.key = "stdp_ltd_weight";
        param.description = "STDP Long-Term Depression weight factor";
        param.type = "float";
        param.defaultValue = "0.012";
        param.unit = "(weight)";
        param.minValue = 0.0;
        param.maxValue = 1.0;
        param.required = false;
        param.category = "Plasticity";
        addParameter(param);
    }
    
    {
        ConfigParameter param;
        param.key = "stdp_time_constant";
        param.description = "STDP time constant (in simulation steps)";
        param.type = "float";
        param.defaultValue = "20.0";
        param.unit = "(steps)";
        param.minValue = 1.0;
        param.maxValue = 1000.0;
        param.required = false;
        param.category = "Plasticity";
        addParameter(param);
    }
    
    // Development parameters
    {
        ConfigParameter param;
        param.key = "development_synaptogenesis_rate";
        param.description = "Rate of new synapse formation during development";
        param.type = "float";
        param.defaultValue = "0.001";
        param.unit = "(rate per step)";
        param.minValue = 0.0;
        param.maxValue = 0.1;
        param.required = false;
        param.category = "Development";
        addParameter(param);
    }
    
    {
        ConfigParameter param;
        param.key = "development_pruning_rate";
        param.description = "Rate of synapse pruning during development";
        param.type = "float";
        param.defaultValue = "0.0001";
        param.unit = "(rate per step)";
        param.minValue = 0.0;
        param.maxValue = 0.01;
        param.required = false;
        param.category = "Development";
        addParameter(param);
    }
    
    // Neuromodulation parameters
    {
        ConfigParameter param;
        param.key = "dopamine_baseline";
        param.description = "Baseline dopamine level (affects neural excitability)";
        param.type = "float";
        param.defaultValue = "0.1";
        param.unit = "(level)";
        param.minValue = 0.0;
        param.maxValue = 1.0;
        param.required = false;
        param.category = "Neuromodulation";
        addParameter(param);
    }
    
    // Environment parameters
    {
        ConfigParameter param;
        param.key = "environment_width";
        param.description = "Width of the simulation environment";
        param.type = "float";
        param.defaultValue = "16.0";
        param.unit = "(units)";
        param.minValue = 1.0;
        param.maxValue = 1000.0;
        param.required = false;
        param.category = "Environment";
        addParameter(param);
    }
    
    {
        ConfigParameter param;
        param.key = "environment_height";
        param.description = "Height of the simulation environment";
        param.type = "float";
        param.defaultValue = "16.0";
        param.unit = "(units)";
        param.minValue = 1.0;
        param.maxValue = 1000.0;
        param.required = false;
        param.category = "Environment";
        addParameter(param);
    }
    
    // Logging parameters
    {
        ConfigParameter param;
        param.key = "log_level";
        param.description = "Minimum log level to output";
        param.type = "string";
        param.defaultValue = "INFO";
        param.validValues = {"DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL"};
        param.required = false;
        param.category = "Logging";
        addParameter(param);
    }
    
    {
        ConfigParameter param;
        param.key = "log_to_file";
        param.description = "Whether to log to file in addition to console";
        param.type = "bool";
        param.defaultValue = "false";
        param.required = false;
        param.category = "Logging";
        addParameter(param);
    }
}

ValidationResult ConfigValidator::validateConsistency(const std::string& value, const std::string& key) const {
    // Check for consistency between related parameters
    if (key == "neuron_count" && pImpl->parameters.count("region_count")) {
        try {
            int neuronCount = std::stoi(value);
            int regionCount = std::stoi(pImpl->parameters.at("region_count").defaultValue);
            
            if (neuronCount > 10000 && regionCount > 10) {
                return ValidationResult(false, 
                    "Many neurons with many regions may be computationally expensive");
            }
        } catch (...) {
            // Ignore conversion errors
        }
    }
    
    return ValidationResult(true);
}

ValidationResult ConfigValidator::validateConsistency(const class Config& config) const {
    return ValidationResult(true);
}

// ConfigHelp implementation
struct ConfigHelp::Impl {
    std::map<std::string, std::string> helpText;
};

ConfigHelp::ConfigHelp() : pImpl(std::make_unique<Impl>()) {}
ConfigHelp::~ConfigHelp() = default;

std::string ConfigHelp::getHelp(const std::string& key) const {
    auto it = pImpl->helpText.find(key);
    if (it == pImpl->helpText.end()) {
        return "No help available for parameter: " + key;
    }
    return it->second;
}

std::string ConfigHelp::getFullHelp() const {
    std::ostringstream oss;
    oss << "NLM Configuration Help" << std::endl;
    oss << "=" << std::string(25, '=') << std::endl << std::endl;
    oss << "Use --help <parameter> for details on specific parameters." << std::endl;
    oss << "Use --list for all available parameters." << std::endl << std::endl;
    oss << "Commonly used parameters:" << std::endl;
    oss << "  --neuron_count <n>      Set number of neurons (default: 500)" << std::endl;
    oss << "  --simulation_timestep <t>  Set simulation timestep (default: 0.001)" << std::endl;
    oss << "  --region_count <n>       Set number of brain regions (default: 1)" << std::endl;
    oss << "  --random_seed <seed>      Set random seed for reproducibility" << std::endl << std::endl;
    oss << "For more information, see the documentation." << std::endl;
    return oss.str();
}

std::vector<std::string> ConfigHelp::getCompletions(const std::string& partialKey) const {
    std::vector<std::string> completions;
    std::string lowerPartial = ConfigUtils::toLower(partialKey);
    
    for (const auto& pair : pImpl->helpText) {
        std::string key = pair.first;
        std::string lowerKey = ConfigUtils::toLower(key);
        
        if (lowerKey.find(lowerPartial) == 0) {
            completions.push_back(key);
        }
    }
    
    return completions;
}

std::vector<std::string> ConfigHelp::getCategories() const {
    std::vector<std::string> categories;
    for (const auto& pair : pImpl->helpText) {
        // Extract category from help text (simple implementation)
        if (pair.second.find("Simulation") != std::string::npos) {
            categories.push_back("Simulation");
        } else if (pair.second.find("Brain") != std::string::npos) {
            categories.push_back("Brain");
        } else if (pair.second.find("Plasticity") != std::string::npos) {
            categories.push_back("Plasticity");
        } else {
            categories.push_back("Other");
        }
    }
    
    // Remove duplicates
    std::sort(categories.begin(), categories.end());
    categories.erase(std::unique(categories.begin(), categories.end()), categories.end());
    
    return categories;
}

std::vector<std::string> ConfigHelp::findKeys(const std::string& pattern) const {
    std::vector<std::string> matches;
    std::string lowerPattern = ConfigUtils::toLower(pattern);
    
    for (const auto& pair : pImpl->helpText) {
        std::string key = pair.first;
        std::string lowerKey = ConfigUtils::toLower(key);
        
        if (lowerKey.find(lowerPattern) != std::string::npos) {
            matches.push_back(key);
        }
    }
    
    return matches;
}

void ConfigHelp::loadStandardHelp() {
    pImpl->helpText["random_seed"] = 
        "Random seed for reproducible experiments. Use an integer value for deterministic results.";
    
    pImpl->helpText["simulation_timestep"] = 
        "Simulation timestep in seconds. Smaller values give more accuracy but are slower.";
    
    pImpl->helpText["neuron_count"] = 
        "Total number of neurons in the brain. Affects computational complexity and memory usage.";
    
    pImpl->helpText["region_count"] = 
        "Number of brain regions. Each region can have different properties and connectivity patterns.";
    
    pImpl->helpText["connection_probability"] = 
        "Probability of connection between neurons when creating random connectivity. Values range from 0.0 (no connections) to 1.0 (fully connected).";
    
    pImpl->helpText["stdp_ltp_weight"] = 
        "STDP Long-Term Potentiation weight factor. Controls the strength of learning when a pre-synaptic spike precedes a post-synaptic spike.";
    
    pImpl->helpText["stdp_ltd_weight"] = 
        "STDP Long-Term Depression weight factor. Controls the strength of learning when a post-synaptic spike precedes a pre-synaptic spike.";
    
    pImpl->helpText["stdp_time_constant"] = 
        "STDP time constant in simulation steps. Controls how long spike traces last for STDP calculations.";
    
    pImpl->helpText["development_synaptogenesis_rate"] = 
        "Rate of new synapse formation during development. Higher values lead to more connections over time.";
    
    pImpl->helpText["development_pruning_rate"] = 
        "Rate of synapse pruning during development. Higher values lead to more connections being removed over time.";
    
    pImpl->helpText["dopamine_baseline"] = 
        "Baseline dopamine level. Affects neural excitability and plasticity. Higher values increase learning and exploration.";
    
    pImpl->helpText["environment_width"] = 
        "Width of the simulation environment for agent navigation and sensing.";
    
    pImpl->helpText["environment_height"] = 
        "Height of the simulation environment for agent navigation and sensing.";
    
    pImpl->helpText["log_level"] = 
        "Minimum log level to output. Available levels: DEBUG, INFO, WARNING, ERROR, CRITICAL.";
    
    pImpl->helpText["log_to_file"] = 
        "Whether to log to file in addition to console output. Useful for long-running experiments.";
    
    pImpl->helpText["log_filename"] = 
        "Filename for log output when log_to_file is enabled. Logs are appended to existing files.";
    
    pImpl->helpText["max_simulation_steps"] = 
        "Maximum number of simulation steps to run. Set to 0 for unlimited duration.";
}

// ConfigError implementation
ConfigError::ConfigError(ErrorCode code, const std::string& message)
    : std::runtime_error(message), errorCode_(code), key_(""), value_("") {}

ConfigError::ConfigError(ErrorCode code, const std::string& key, const std::string& value)
    : std::runtime_error("Configuration error for parameter '" + key + "': " + value), 
      errorCode_(code), key_(key), value_(value) {}

// ConfigUtils implementation
template<typename T>
std::optional<T> ConfigUtils::parseValue(const std::string& str, const std::string& type) {
    try {
        if (type == "int") {
            return std::stoi(str);
        } else if (type == "float" || type == "double") {
            return std::stod(str);
        } else if (type == "bool") {
            std::string lower = toLower(str);
            if (lower == "true" || lower == "yes" || lower == "1") return true;
            if (lower == "false" || lower == "no" || lower == "0") return false;
            return std::nullopt;
        } else if (type == "string") {
            return str;
        }
    } catch (...) {
        // Ignore conversion errors
    }
    return std::nullopt;
}

template<typename T>
std::string ConfigUtils::formatValue(const T& value, const std::string& type) {
    return std::to_string(value);
}

bool ConfigUtils::matchesType(const std::string& value, const std::string& type) {
    if (type == "int") {
        return isInteger(value);
    } else if (type == "float" || type == "double") {
        return isFloat(value);
    } else if (type == "bool") {
        return isBool(value);
    } else if (type == "string") {
        return true; // Any string is valid
    }
    return false;
}

std::optional<double> ConfigUtils::stringToDouble(const std::string& str) {
    try {
        return std::stod(str);
    } catch (...) {
        return std::nullopt;
    }
}

std::optional<int> ConfigUtils::stringToInt(const std::string& str) {
    try {
        return std::stoi(str);
    } catch (...) {
        return std::nullopt;
    }
}

std::optional<bool> ConfigUtils::stringToBool(const std::string& str) {
    std::string lower = toLower(str);
    if (lower == "true" || lower == "yes" || lower == "1") return true;
    if (lower == "false" || lower == "no" || lower == "0") return false;
    return std::nullopt;
}

std::string ConfigUtils::detectType(const std::string& value) {
    if (isInteger(value)) return "int";
    if (isFloat(value)) return "float";
    if (isBool(value)) return "bool";
    return "string";
}

std::string ConfigUtils::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string ConfigUtils::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

bool ConfigUtils::isInteger(const std::string& str) {
    if (str.empty()) return false;
    size_t start = 0;
    if (str[0] == '+' || str[0] == '-') start = 1;
    return std::all_of(str.begin() + start, str.end(), ::isdigit);
}

bool ConfigUtils::isFloat(const std::string& str) {
    try {
        std::stod(str);
        return true;
    } catch (...) {
        return false;
    }
}

bool ConfigUtils::isBool(const std::string& str) {
    std::string lower = toLower(str);
    return lower == "true" || lower == "false" || 
           lower == "yes" || lower == "no" ||
           lower == "1" || lower == "0";
}

} // namespace nlm