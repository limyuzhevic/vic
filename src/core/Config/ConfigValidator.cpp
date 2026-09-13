// ConfigValidator.cpp
#include "ConfigValidator.h"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <sstream>
#include <fstream>
#include <algorithm>
#include <iostream>

namespace nlm {

struct ConfigValidator::Impl {
    std::unordered_map<std::string, ConfigParameter> parameters;
    std::shared_ptr<Config> defaultConfig;
};

ConfigValidator::ConfigValidator() : pImpl(std::make_unique<Impl>()) {
    initializeDefaultParameters();
}

void ConfigValidator::initializeDefaultParameters() {
    // Define standard parameters based on configs/default.cfg
    
    // Basic parameters
    registerParameter({
        "random_seed",
        "Random seed for reproducible experiments",
        ConfigSource::Default,
        "42",
        [](const ConfigValue& v) {
            if (std::holds_alternative<int>(v) || std::holds_alternative<int64_t>(v)) {
                int64_t seed = std::holds_alternative<int>(v) ? 
                    std::get<int>(v) : std::get<int64_t>(v);
                return seed >= 0 && seed <= UINT64_MAX;
            }
            return false;
        },
        false,
        {}
    });
    
    registerParameter({
        "simulation_timestep",
        "Simulation timestep in seconds",
        ConfigSource::Default,
        "0.001",
        [](const ConfigValue& v) {
            if (std::holds_alternative<double>(v) || std::holds_alternative<float>(v)) {
                double timestep = std::holds_alternative<double>(v) ? 
                    std::get<double>(v) : std::get<float>(v);
                return timestep > 0.0 && timestep < 1.0;
            }
            return false;
        },
        false,
        {}
    });
    
    // Neuron parameters
    registerParameter({
        "neuron_count",
        "Number of neurons in the brain",
        ConfigSource::Default,
        "1000",
        [](const ConfigValue& v) {
            if (std::holds_alternative<int>(v) || std::holds_alternative<int64_t>(v) || 
                std::holds_alternative<size_t>(v)) {
                int64_t count = std::holds_alternative<int>(v) ? 
                    std::get<int>(v) : (std::holds_alternative<int64_t>(v) ? 
                    std::get<int64_t>(v) : std::get<size_t>(v));
                return count > 0 && count < 1000000; // Reasonable upper bound
            }
            return false;
        },
        false,
        {}
    });
    
    registerParameter({
        "region_count",
        "Number of brain regions",
        ConfigSource::Default,
        "2",
        [](const ConfigValue& v) {
            if (std::holds_alternative<int>(v) || std::holds_alternative<int64_t>(v) || 
                std::holds_alternative<size_t>(v)) {
                int64_t count = std::holds_alternative<int>(v) ? 
                    std::get<int>(v) : (std::holds_alternative<int64_t>(v) ? 
                    std::get<int64_t>(v) : std::get<size_t>(v));
                return count >= 1 && count <= 100;
            }
            return false;
        },
        false,
        {}
    });
    
    registerParameter({
        "connection_probability",
        "Connection probability for random connectivity (0.0 to 1.0)",
        ConfigSource::Default,
        "0.1",
        [](const ConfigValue& v) {
            if (std::holds_alternative<double>(v) || std::holds_alternative<float>(v)) {
                double prob = std::holds_alternative<double>(v) ? 
                    std::get<double>(v) : std::get<float>(v);
                return prob >= 0.0 && prob <= 1.0;
            }
            return false;
        },
        false,
        {}
    });
    
    // Plasticity parameters
    registerParameter({
        "plasticity_learning_rate",
        "Global plasticity learning rate",
        ConfigSource::Default,
        "0.01",
        [](const ConfigValue& v) {
            if (std::holds_alternative<double>(v) || std::holds_alternative<float>(v)) {
                double rate = std::holds_alternative<double>(v) ? 
                    std::get<double>(v) : std::get<float>(v);
                return rate >= 0.0 && rate <= 1.0;
            }
            return false;
        },
        false,
        {}
    });
    
    registerParameter({
        "stdp_ltp_weight",
        "STDP Long-Term Potentiation weight",
        ConfigSource::Default,
        "0.01",
        [](const ConfigValue& v) {
            if (std::holds_alternative<double>(v) || std::holds_alternative<float>(v)) {
                double weight = std::holds_alternative<double>(v) ? 
                    std::get<double>(v) : std::get<float>(v);
                return weight >= 0.0 && weight <= 1.0;
            }
            return false;
        },
        false,
        {}
    });
    
    registerParameter({
        "stdp_ltd_weight",
        "STDP Long-Term Depression weight",
        ConfigSource::Default,
        "0.012",
        [](const ConfigValue& v) {
            if (std::holds_alternative<double>(v) || std::holds_alternative<float>(v)) {
                double weight = std::holds_alternative<double>(v) ? 
                    std::get<double>(v) : std::get<float>(v);
                return weight >= 0.0 && weight <= 1.0;
            }
            return false;
        },
        false,
        {}
    });
    
    registerParameter({
        "stdp_tau",
        "STDP time constant (tau)",
        ConfigSource::Default,
        "20.0",
        [](const ConfigValue& v) {
            if (std::holds_alternative<double>(v) || std::holds_alternative<float>(v)) {
                double tau = std::holds_alternative<double>(v) ? 
                    std::get<double>(v) : std::get<float>(v);
                return tau > 0.0 && tau < 1000.0;
            }
            return false;
        },
        false,
        {}
    });
    
    // Development parameters
    registerParameter({
        "synaptogenesis_rate",
        "Synaptogenesis rate",
        ConfigSource::Default,
        "0.001",
        [](const ConfigValue& v) {
            if (std::holds_alternative<double>(v) || std::holds_alternative<float>(v)) {
                double rate = std::holds_alternative<double>(v) ? 
                    std::get<double>(v) : std::get<float>(v);
                return rate >= 0.0 && rate <= 1.0;
            }
            return false;
        },
        false,
        {}
    });
    
    registerParameter({
        "pruning_rate",
        "Pruning rate",
        ConfigSource::Default,
        "0.0001",
        [](const ConfigValue& v) {
            if (std::holds_alternative<double>(v) || std::holds_alternative<float>(v)) {
                double rate = std::holds_alternative<double>(v) ? 
                    std::get<double>(v) : std::get<float>(v);
                return rate >= 0.0 && rate <= 1.0;
            }
            return false;
        },
        false,
        {}
    });
    
    // Neuromodulation parameters
    registerParameter({
        "dopamine_baseline",
        "Dopamine baseline level",
        ConfigSource::Default,
        "0.1",
        [](const ConfigValue& v) {
            if (std::holds_alternative<double>(v) || std::holds_alternative<float>(v)) {
                double level = std::holds_alternative<double>(v) ? 
                    std::get<double>(v) : std::get<float>(v);
                return level >= 0.0 && level <= 10.0;
            }
            return false;
        },
        false,
        {}
    });
    
    // Reward parameters
    registerParameter({
        "reward_discount_factor",
        "Reward discount factor (0.0 to 1.0)",
        ConfigSource::Default,
        "0.99",
        [](const ConfigValue& v) {
            if (std::holds_alternative<double>(v) || std::holds_alternative<float>(v)) {
                double factor = std::holds_alternative<double>(v) ? 
                    std::get<double>(v) : std::get<float>(v);
                return factor >= 0.0 && factor <= 1.0;
            }
            return false;
        },
        false,
        {}
    });
    
    // Environment parameters
    registerParameter({
        "environment_name",
        "Environment name",
        ConfigSource::Default,
        "GridWorld",
        [](const ConfigValue& v) {
            return std::holds_alternative<std::string>(v);
        },
        false,
        {"GridWorld", "Empty", "Maze", "Custom"}
    });
    
    registerParameter({
        "environment_width",
        "Environment width",
        ConfigSource::Default,
        "10",
        [](const ConfigValue& v) {
            if (std::holds_alternative<int>(v) || std::holds_alternative<int64_t>(v) || 
                std::holds_alternative<size_t>(v)) {
                int64_t width = std::holds_alternative<int>(v) ? 
                    std::get<int>(v) : (std::holds_alternative<int64_t>(v) ? 
                    std::get<int64_t>(v) : std::get<size_t>(v));
                return width >= 1 && width <= 1000;
            }
            return false;
        },
        false,
        {}
    });
    
    registerParameter({
        "environment_height",
        "Environment height",
        ConfigSource::Default,
        "10",
        [](const ConfigValue& v) {
            if (std::holds_alternative<int>(v) || std::holds_alternative<int64_t>(v) || 
                std::holds_alternative<size_t>(v)) {
                int64_t height = std::holds_alternative<int>(v) ? 
                    std::get<int>(v) : (std::holds_alternative<int64_t>(v) ? 
                    std::get<int64_t>(v) : std::get<size_t>(v));
                return height >= 1 && height <= 1000;
            }
            return false;
        },
        false,
        {}
    });
    
    // Logging parameters
    registerParameter({
        "log_level",
        "Log level",
        ConfigSource::Default,
        "INFO",
        [](const ConfigValue& v) {
            if (std::holds_alternative<std::string>(v)) {
                std::string level = std::get<std::string>(v);
                return level == "DEBUG" || level == "INFO" || level == "WARN" || 
                       level == "ERROR" || level == "FATAL";
            }
            return false;
        },
        false,
        {"DEBUG", "INFO", "WARN", "ERROR", "FATAL"}
    });
    
    registerParameter({
        "log_to_file",
        "Log to file",
        ConfigSource::Default,
        "false",
        [](const ConfigValue& v) {
            return std::holds_alternative<bool>(v);
        },
        false,
        {}
    });
    
    // Simulation parameters
    registerParameter({
        "max_simulation_steps",
        "Maximum simulation steps",
        ConfigSource::Default,
        "10000",
        [](const ConfigValue& v) {
            if (std::holds_alternative<int>(v) || std::holds_alternative<int64_t>(v) || 
                std::holds_alternative<size_t>(v)) {
                int64_t steps = std::holds_alternative<int>(v) ? 
                    std::get<int>(v) : (std::holds_alternative<int64_t>(v) ? 
                    std::get<int64_t>(v) : std::get<size_t>(v));
                return steps > 0 && steps < 1000000000;
            }
            return false;
        },
        false,
        {}
    });
    
    registerParameter({
        "simulation_time_limit",
        "Simulation time limit (seconds)",
        ConfigSource::Default,
        "0.0",
        [](const ConfigValue& v) {
            if (std::holds_alternative<double>(v) || std::holds_alternative<float>(v)) {
                double limit = std::holds_alternative<double>(v) ? 
                    std::get<double>(v) : std::get<float>(v);
                return limit >= 0.0 && limit <= 1000000.0;
            }
            return false;
        },
        false,
        {}
    });
    
    // Visualization parameters
    registerParameter({
        "visualization_enabled",
        "Enable visualization",
        ConfigSource::Default,
        "false",
        [](const ConfigValue& v) {
            return std::holds_alternative<bool>(v);
        },
        false,
        {}
    });
    
    registerParameter({
        "visualization_update_rate",
        "Visualization update rate (Hz)",
        ConfigSource::Default,
        "30.0",
        [](const ConfigValue& v) {
            if (std::holds_alternative<double>(v) || std::holds_alternative<float>(v)) {
                double rate = std::holds_alternative<double>(v) ? 
                    std::get<double>(v) : std::get<float>(v);
                return rate > 0.0 && rate <= 1000.0;
            }
            return false;
        },
        false,
        {}
    });
    
    // Consistency checks and derived parameters
    registerParameter({
        "plasticity_parameters_consistent",
        "Check consistency between plasticity parameters",
        ConfigSource::Runtime,
        "true",
        [](const ConfigValue& v) {
            return std::holds_alternative<bool>(v);
        },
        true,
        {}
    });
}

void ConfigValidator::registerParameter(const ConfigParameter& param) {
    pImpl->parameters[param.key] = param;
    
    // Update default config
    if (pImpl->defaultConfig) {
        pImpl->defaultConfig->set(param.key, ConfigValue(param.defaultValue), ConfigSource::Default);
    }
}

ValidationResult ConfigValidator::validate(const std::shared_ptr<Config>& config) const {
    ValidationResult result;
    result.isValid = true;
    
    if (!config) {
        result.isValid = false;
        result.errors.push_back("Configuration is null");
        return result;
    }
    
    // Check all registered parameters
    for (const auto& [key, param] : pImpl->parameters) {
        // Check if parameter exists in config
        if (config->has(key)) {
            auto valueOpt = config->get<std::string>(key);
            if (!valueOpt) {
                result.isValid = false;
                result.errors.push_back("Parameter '" + key + "' has invalid type");
                continue;
            }
            
            // Get the actual value from config (in its native type)
            auto configValueOpt = config->getAny(key);
            if (!configValueOpt) {
                result.isValid = false;
                result.errors.push_back("Parameter '" + key + "' has invalid type");
                continue;
            }
            
            // Validate against parameter definition
            auto errors = validateParameter(key, *configValueOpt);
            result.errors.insert(result.errors.end(), errors.begin(), errors.end());
            
            // Check valid values if specified
            if (!param.validValues.empty()) {
                auto valueOptAny = config->getAny(key);
                if (valueOptAny) {
                    std::string valueStr;
                    std::visit([&valueStr](auto&& arg) {
                        using T = std::decay_t<decltype(arg)>;
                        if constexpr (std::is_same_v<T, std::string>) {
                            valueStr = arg;
                        } else if constexpr (std::is_same_v<T, int> || 
                                            std::is_same_v<T, int64_t> ||
                                            std::is_same_v<T, size_t>) {
                            valueStr = std::to_string(arg);
                        } else if constexpr (std::is_same_v<T, double> || 
                                            std::is_same_v<T, float>) {
                            valueStr = std::to_string(arg);
                        } else if constexpr (std::is_same_v<T, bool>) {
                            valueStr = arg ? "true" : "false";
                        }
                    }, *valueOptAny);
                    
                    bool validValue = false;
                    for (const auto& validValue : param.validValues) {
                        if (valueStr == validValue) {
                            validValue = true;
                            break;
                        }
                    }
                    
                    if (!validValue) {
                        result.isValid = false;
                        result.errors.push_back("Parameter '" + key + "' has invalid value: '" + 
                                               valueStr + "'. Valid values: " + 
                                               joinStrings(param.validValues, ", "));
                    }
                }
            }
        } else if (param.required) {
            result.isValid = false;
            result.errors.push_back("Required parameter '" + key + "' is missing");
        }
    }
    
    // Run cross-parameter validation
    if (config->has("synaptogenesis_rate") && config->has("pruning_rate")) {
        auto synOpt = config->get<double>("synaptogenesis_rate");
        auto pruneOpt = config->get<double>("pruning_rate");
        if (synOpt && pruneOpt) {
            if (*synOpt < 0.0 || *synOpt > 1.0) {
                result.warnings.push_back("synaptogenesis_rate (" + std::to_string(*synOpt) + ") is outside expected range [0.0, 1.0]");
            }
            if (*pruneOpt < 0.0 || *pruneOpt > 1.0) {
                result.warnings.push_back("pruning_rate (" + std::to_string(*pruneOpt) + ") is outside expected range [0.0, 1.0]");
            }
            if (*synOpt < *pruneOpt) {
                result.warnings.push_back("synaptogenesis_rate should typically be larger than pruning_rate for network stability");
            }
        }
    }
    
    if (config->has("stdp_ltp_weight") && config->has("stdp_ltd_weight")) {
        auto ltpOpt = config->get<double>("stdp_ltp_weight");
        auto ltdOpt = config->get<double>("stdp_ltd_weight");
        if (ltpOpt && ltdOpt) {
            if (*ltpOpt <= 0.0) {
                result.warnings.push_back("stdp_ltp_weight should be positive");
            }
            if (*ltdOpt <= 0.0) {
                result.warnings.push_back("stdp_ltd_weight should be positive");
            }
            if (*ltdOpt >= *ltpOpt) {
                result.warnings.push_back("stdp_ltd_weight should typically be less than stdp_ltp_weight");
            }
        }
    }
    
    if (config->has("neuron_count") && config->has("region_count")) {
        auto neuronOpt = config->get<size_t>("neuron_count");
        auto regionOpt = config->get<size_t>("region_count");
        if (neuronOpt && regionOpt && *regionOpt > 0) {
            size_t neuronsPerRegion = *neuronOpt / *regionOpt;
            if (neuronsPerRegion < 10) {
                result.warnings.push_back("Very few neurons per region (" + std::to_string(neuronsPerRegion) + "). Consider increasing neuron_count or decreasing region_count");
            }
        }
    }
    
    return result;
}

std::optional<ConfigParameter> ConfigValidator::getParameter(const std::string& key) const {
    auto it = pImpl->parameters.find(key);
    if (it != pImpl->parameters.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::vector<std::string> ConfigValidator::getParameterKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->parameters.size());
    for (const auto& pair : pImpl->parameters) {
        keys.push_back(pair.first);
    }
    return keys;
}

std::vector<std::string> ConfigValidator::validateParameter(const std::string& key, const ConfigValue& value) const {
    std::vector<std::string> errors;
    
    auto paramOpt = getParameter(key);
    if (!paramOpt) {
        errors.push_back("Unknown parameter: '" + key + "'");
        return errors;
    }
    
    const auto& param = *paramOpt;
    
    // Validate type using the parameter's validator function
    if (param.validator && !param.validator(value)) {
        errors.push_back("Parameter '" + key + "' has invalid value type or range");
    }
    
    return errors;
}

void ConfigValidator::clearParameters() {
    pImpl->parameters.clear();
    pImpl->defaultConfig = nullptr;
}

std::shared_ptr<Config> ConfigValidator::getDefaultConfig() const {
    if (!pImpl->defaultConfig) {
        pImpl->defaultConfig = std::make_shared<Config>();
        
        // Set all default values
        for (const auto& [key, param] : pImpl->parameters) {
            pImpl->defaultConfig->set(key, ConfigValue(param.defaultValue), param.defaultSource);
        }
    }
    
    return pImpl->defaultConfig;
}

std::string ConfigValidator::getDocumentation() const {
    std::stringstream ss;
    
    ss << "NLM Configuration Parameters\n";
    ss << "==========================\n\n";
    
    for (const auto& [key, param] : pImpl->parameters) {
        ss << "Key: " << key << "\n";
        ss << "Description: " << param.description << "\n";
        ss << "Default: " << param.defaultValue << " (" << static_cast<int>(param.defaultSource) << ")\n";
        ss << "Required: " << (param.required ? "Yes" : "No") << "\n";
        
        if (!param.validValues.empty()) {
            ss << "Valid values: ";
            for (size_t i = 0; i < param.validValues.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << "\"" << param.validValues[i] << "\"";
            }
            ss << "\n";
        }
        
        ss << "\n";
    }
    
    return ss.str();
}

// Global validator instance
std::unique_ptr<ConfigValidator> g_configValidator;

// Helper functions
ValidationResult validateConfig(const std::shared_ptr<Config>& config) {
    if (!g_configValidator) {
        g_configValidator = std::make_unique<ConfigValidator>();
    }
    return g_configValidator->validate(config);
}

bool isValidParameterType(const std::string& key, const ConfigValue& value) {
    if (!g_configValidator) {
        g_configValidator = std::make_unique<ConfigValidator>();
    }
    
    auto paramOpt = g_configValidator->getParameter(key);
    if (!paramOpt) {
        return false;
    }
    
    return paramOpt->validator ? paramOpt->validator(value) : true;
}

std::string getParameterType(const std::string& key) {
    if (!g_configValidator) {
        g_configValidator = std::make_unique<ConfigValidator>();
    }
    
    auto paramOpt = g_configValidator->getParameter(key);
    if (!paramOpt) {
        return "unknown";
    }
    
    return paramOpt->description;
}

// Helper function to join strings
std::string joinStrings(const std::vector<std::string>& strings, const std::string& delimiter) {
    std::stringstream ss;
    for (size_t i = 0; i < strings.size(); ++i) {
        if (i > 0) ss << delimiter;
        ss << strings[i];
    }
    return ss.str();
}

} // namespace nlm
