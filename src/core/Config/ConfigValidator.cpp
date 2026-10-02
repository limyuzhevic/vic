#include "ConfigValidator.hpp"
#include <sstream>
#include <algorithm>
#include <cmath>
#include <limits>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>
#include "../Logger/Logger.hpp"

namespace nlm {

ConfigValidator::ValidationResult ConfigValidator::validate(const Config& config) const {
    ValidationResult result;
    result.isValid = true;
    
    // Validate all entries
    for (const auto& entry : config.pImpl->entries) {
        if (!validateParameter(entry.key, entry.value)) {
            result.isValid = false;
        }
    }
    
    return result;
}

bool ConfigValidator::validateParameter(const std::string& key, const ConfigValue& value) const {
    auto it = parameterDefinitions_.find(key);
    if (it == parameterDefinitions_.end()) {
        // Unknown parameter - log warning
        return true;  // Allow unknown parameters for flexibility
    }
    
    const auto& paramInfo = it->second;
    
    // Check if parameter has valid values
    if (!paramInfo.validValues.empty()) {
        bool valueIsValid = false;
        std::visit([&](auto&& arg) {
            std::string strValue;
            if constexpr (std::is_same_v<decltype(arg), int>) {
                strValue = std::to_string(arg);
            } else if constexpr (std::is_same_v<decltype(arg), int64_t>) {
                strValue = std::to_string(arg);
            } else if constexpr (std::is_same_v<decltype(arg), double>) {
                strValue = std::to_string(arg);
            } else if constexpr (std::is_same_v<decltype(arg), bool>) {
                strValue = arg ? "true" : "false";
            } else if constexpr (std::is_same_v<decltype(arg), std::string>) {
                strValue = arg;
            }
            
            valueIsValid = std::find(paramInfo.validValues.begin(), paramInfo.validValues.end(), strValue) != paramInfo.validValues.end();
        }, value);
        
        if (!valueIsValid) {
            std::stringstream ss;
            ss << "Parameter \"" << key << "\" value \"";
            std::visit([&](auto&& arg) {
                std::stringstream valss;
                if constexpr (std::is_same_v<decltype(arg), int>) {
                    valss << arg;
                } else if constexpr (std::is_same_v<decltype(arg), int64_t>) {
                    valss << arg;
                } else if constexpr (std::is_same_v<decltype(arg), double>) {
                    valss << arg;
                }
                ss << valss.str();
            }, value);
            ss << "\" is outside valid range [" << paramInfo.range.first << ", " << paramInfo.range.second << "]";
            
            NLM_LOG_WARNING(ss.str());
            return false;
        }
    }
    
    // Check numeric range if applicable
    if (paramInfo.range.first != paramInfo.range.second) {
        bool valueInRange = false;
        std::visit([&](auto&& arg) {
            if constexpr (std::is_same_v<decltype(arg), int>) {
                valueInRange = arg >= paramInfo.range.first && arg <= paramInfo.range.second;
            } else if constexpr (std::is_same_v<decltype(arg), int64_t>) {
                valueInRange = arg >= paramInfo.range.first && arg <= paramInfo.range.second;
            } else if constexpr (std::is_same_v<decltype(arg), double>) {
                valueInRange = arg >= paramInfo.range.first && arg <= paramInfo.range.second;
            }
        }, value);
        
        if (!valueInRange) {
            std::stringstream ss;
            ss << "Parameter \"" << key << "\" value \"";
            std::visit([&](auto&& arg) {
                std::stringstream valss;
                if constexpr (std::is_same_v<decltype(arg), int>) {
                    valss << arg;
                } else if constexpr (std::is_same_v<decltype(arg), int64_t>) {
                    valss << arg;
                } else if constexpr (std::is_same_v<decltype(arg), double>) {
                    valss << arg;
                }
                ss << valss.str();
            }, value);
            ss << "\" is outside valid range [" << paramInfo.range.first << ", " << paramInfo.range.second << "]";
            
            NLM_LOG_WARNING(ss.str());
            return false;
        }
    }
    
    return true;
}

template<typename T>
bool ConfigValidator::validateType(const std::string& key, const ConfigValue& value) const {
    try {
        std::get<T>(value);
        return true;
    } catch (const std::bad_variant_access&) {
        return false;
    }
}

void ConfigValidator::initializeParameterDefinitions() {
    // Initialize parameter definitions for common configuration parameters
    // Random seed
    parameterDefinitions_["random_seed"] = {
        "random_seed",
        "int",
        "Random seed for reproducible experiments",
        false,
        {},
        {-1, std::numeric_limits<int64_t>::max()}
    };
    
    // Simulation timestep
    parameterDefinitions_["simulation_timestep"] = {
        "simulation_timestep",
        "double",
        "Simulation timestep in seconds",
        false,
        {},
        {0.0, 10.0}
    };
    
    // Neuron count
    parameterDefinitions_["neuron_count"] = {
        "neuron_count",
        "int64_t",
        "Number of neurons in the brain",
        false,
        {},
        {1, std::numeric_limits<int64_t>::max()}
    };
    
    // Region count
    parameterDefinitions_["region_count"] = {
        "region_count",
        "int64_t",
        "Number of brain regions",
        false,
        {},
        {1, 100}
    };
    
    // Connection probability
    parameterDefinitions_["connection_probability"] = {
        "connection_probability",
        "double",
        "Connection probability for random connectivity",
        false,
        {},
        {0.0, 1.0}
    };
    
    // Log level
    parameterDefinitions_["log_level"] = {
        "log_level",
        "string",
        "Log level (DEBUG, INFO, WARNING, ERROR, CRITICAL)",
        false,
        {"DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL"},
        {-1, -1}
    };
    
    // Log to file
    parameterDefinitions_["log_to_file"] = {
        "log_to_file",
        "bool",
        "Whether to log to file",
        false,
        {},
        {-1, -1}
    };
    
    // Visualization enabled
    parameterDefinitions_["visualization_enabled"] = {
        "visualization_enabled",
        "bool",
        "Whether visualization is enabled",
        false,
        {},
        {-1, -1}
    };
}

} // namespace nlm
