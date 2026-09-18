// nlm - Advanced Configuration Implementation with JSON Support
#include "ConfigAdvanced.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <regex>

namespace nlm {

// ConfigValidator implementation
ConfigValidator::ConfigValidator() {
    initializeDefaultRules();
    initializeDefaultConfig();
}

void ConfigValidator::initializeDefaultRules() {
    // Neural system rules
    rules_.push_back({
        "neuron_count",
        "Total number of neurons in the brain",
        [](const ConfigValue& v) { return std::holds_alternative<double>(v) && std::get<double>(v) > 0; },
        "neuron_count must be a positive number"
    });
    
    rules_.push_back({
        "connection_probability",
        "Probability of connection between neurons (0.0 to 1.0)",
        [](const ConfigValue& v) {
            if (!std::holds_alternative<double>(v)) return false;
            double val = std::get<double>(v);
            return val >= 0.0 && val <= 1.0;
        },
        "connection_probability must be between 0.0 and 1.0"
    });
    
    rules_.push_back({
        "v_thresh",
        "Neuron threshold potential (mV)",
        [](const ConfigValue& v) {
            if (!std::holds_alternative<double>(v)) return false;
            double val = std::get<double>(v);
            return val < 0.0;  // Threshold is typically negative
        },
        "v_thresh must be a negative value (typically -50.0 mV)"
    });
    
    // Plasticity rules
    rules_.push_back({
        "stdp_learning_rate",
        "STDP learning rate",
        [](const ConfigValue& v) {
            if (!std::holds_alternative<double>(v)) return false;
            double val = std::get<double>(v);
            return val >= 0.0 && val <= 1.0;
        },
        "stdp_learning_rate must be between 0.0 and 1.0"
    });
    
    rules_.push_back({
        "synaptogenesis_rate",
        "Synaptogenesis rate (neurons per second)",
        [](const ConfigValue& v) {
            if (!std::holds_alternative<double>(v)) return false;
            double val = std::get<double>(v);
            return val >= 0.0 && val <= 0.01;
        },
        "synaptogenesis_rate must be between 0.0 and 0.01"
    });
    
    // World parameters
    rules_.push_back({
        "world_width",
        "World width",
        [](const ConfigValue& v) {
            if (!std::holds_alternative<double>(v)) return false;
            double val = std::get<double>(v);
            return val > 0.0;
        },
        "world_width must be positive"
    });
    
    rules_.push_back({
        "world_height",
        "World height",
        [](const ConfigValue& v) {
            if (!std::holds_alternative<double>(v)) return false;
            double val = std::get<double>(v);
            return val > 0.0;
        },
        "world_height must be positive"
    });
    
    // Simulation parameters
    rules_.push_back({
        "simulation_timestep",
        "Simulation timestep in seconds",
        [](const ConfigValue& v) {
            if (!std::holds_alternative<double>(v)) return false;
            double val = std::get<double>(v);
            return val > 0.0 && val <= 0.1;  // Should be small (max 100ms)
        },
        "simulation_timestep must be between 0.0 and 0.1 seconds"
    });
    
    rules_.push_back({
        "random_seed",
        "Random seed for reproducibility",
        [](const ConfigValue& v) {
            if (std::holds_alternative<int64_t>(v)) {
                return std::get<int64_t>(v) >= 0;
            } else if (std::holds_alternative<uint64_t>(v)) {
                return true;
            }
            return false;
        },
        "random_seed must be a non-negative integer"
    });
}

void ConfigValidator::initializeDefaultConfig() {
    defaults_.neuron_count = 1000.0;
    defaults_.connection_probability = 0.1;
    defaults_.initial_weight_mean = 0.5;
    defaults_.initial_weight_std = 0.1;
    defaults_.v_thresh = -50.0;
    defaults_.v_rest = -70.0;
    defaults_.v_reset = -75.0;
    defaults_.tau_mem = 20.0;
    defaults_.tau_ref = 2.0;
    
    defaults_.stdp_learning_rate = 0.001;
    defaults_.stdp_tau_plus = 20.0;
    defaults_.stdp_tau_minus = 20.0;
    defaults_.synaptogenesis_rate = 0.0001;
    defaults_.pruning_rate = 0.00001;
    
    defaults_.world_width = 20.0;
    defaults_.world_height = 20.0;
    defaults_.world_vision_width = 16;
    defaults_.world_vision_height = 16;
    
    defaults_.simulation_timestep = 0.001;
    defaults_.random_seed = 42;
}

void ConfigValidator::addRule(const ValidationRule& rule) {
    rules_.push_back(rule);
}

void ConfigValidator::addDefaults(const DefaultConfig& defaults) {
    // Merge defaults
    if (defaults.neuron_count > 0) defaults_.neuron_count = defaults.neuron_count;
    if (defaults.connection_probability >= 0.0 && defaults.connection_probability <= 1.0) 
        defaults_.connection_probability = defaults.connection_probability;
    // ... apply all other defaults similarly
    defaults_.synaptogenesis_rate = defaults.synaptogenesis_rate;
    defaults_.pruning_rate = defaults.pruning_rate;
    defaults_.random_seed = defaults.random_seed;
}

ValidationResult ConfigValidator::validate(const std::map<std::string, ConfigValue>& config) const {
    ValidationResult result;
    result.valid = true;
    
    // Check all rules
    for (const auto& rule : rules_) {
        auto it = config.find(rule.key);
        if (it != config.end()) {
            if (!rule.validator(it->second)) {
                result.valid = false;
                result.errors.push_back(rule.key + ": " + rule.errorMessage);
            }
        }
    }
    
    // Check for missing required keys
    const std::vector<std::string> requiredKeys = {
        "neuron_count",
        "connection_probability", 
        "v_thresh",
        "v_rest",
        "tau_mem",
        "world_width",
        "world_height",
        "simulation_timestep"
    };
    
    for (const auto& key : requiredKeys) {
        if (config.find(key) == config.end()) {
            result.valid = false;
            result.errors.push_back("Missing required configuration key: " + key);
        }
    }
    
    return result;
}

void ConfigValidator::applyDefaults(std::map<std::string, ConfigValue>& config) const {
    // Convert defaults to ConfigValue and apply if key not present
    const auto& toConfigValue = [](const auto& value) {
        if constexpr (std::is_same_v<decltype(value), double>) {
            return ConfigValue(value);
        } else if constexpr (std::is_same_v<decltype(value), int64_t>) {
            return ConfigValue(static_cast<long long>(value));
        } else if constexpr (std::is_same_v<decltype(value), int>) {
            return ConfigValue(static_cast<long long>(value));
        } else if constexpr (std::is_same_v<decltype(value), bool>) {
            return ConfigValue(value);
        } else if constexpr (std::is_same_v<decltype(value), std::string>) {
            return ConfigValue(value);
        } else if constexpr (std::is_same_v<decltype(value), size_t>) {
            return ConfigValue(static_cast<long long>(value));
        } else {
            return ConfigValue(std::string()); // Default to empty string
        }
    };
    
    // Helper function to check if default is valid
    auto isDefaultValid = [](const auto& defaultVal, bool (*validator)(const ConfigValue&)) -> bool {
        return validator(toConfigValue(defaultVal));
    };
    
    // Apply defaults only if key doesn't exist or default is more appropriate
    auto applyDefault = [&](const std::string& key, const auto& defaultValue, 
                          bool (*validator)(const ConfigValue&)) {
        auto it = config.find(key);
        if (it == config.end()) {
            if (isDefaultValid(defaultValue, validator)) {
                config[key] = toConfigValue(defaultValue);
            }
        }
    };
    
    // Apply neural system defaults
    applyDefault("neuron_count", defaults_.neuron_count, 
                 [](const ConfigValue& v) { return std::holds_alternative<double>(v) && std::get<double>(v) > 0; });
    
    applyDefault("connection_probability", defaults_.connection_probability,
                 [](const ConfigValue& v) {
                     if (!std::holds_alternative<double>(v)) return false;
                     double val = std::get<double>(v);
                     return val >= 0.0 && val <= 1.0;
                 });
    
    applyDefault("v_thresh", defaults_.v_thresh,
                 [](const ConfigValue& v) {
                     if (!std::holds_alternative<double>(v)) return false;
                     return std::get<double>(v) < 0.0;
                 });
    
    applyDefault("v_rest", defaults_.v_rest,
                 [](const ConfigValue& v) {
                     if (!std::holds_alternative<double>(v)) return false;
                     return std::get<double>(v) < 0.0;
                 });
    
    applyDefault("tau_mem", defaults_.tau_mem,
                 [](const ConfigValue& v) {
                     if (!std::holds_alternative<double>(v)) return false;
                     return std::get<double>(v) > 0.0;
                 });
    
    // Apply world defaults
    applyDefault("world_width", defaults_.world_width,
                 [](const ConfigValue& v) {
                     if (!std::holds_alternative<double>(v)) return false;
                     return std::get<double>(v) > 0.0;
                 });
    
    applyDefault("world_height", defaults_.world_height,
                 [](const ConfigValue& v) {
                     if (!std::holds_alternative<double>(v)) return false;
                     return std::get<double>(v) > 0.0;
                 });
    
    // Apply simulation defaults
    applyDefault("simulation_timestep", defaults_.simulation_timestep,
                 [](const ConfigValue& v) {
                     if (!std::holds_alternative<double>(v)) return false;
                     double val = std::get<double>(v);
                     return val > 0.0 && val <= 0.1;
                 });
    
    applyDefault("random_seed", defaults_.random_seed,
                 [](const ConfigValue& v) {
                     if (std::holds_alternative<int64_t>(v)) {
                         return std::get<int64_t>(v) >= 0;
                     } else if (std::holds_alternative<uint64_t>(v)) {
                         return true;
                     }
                     return false;
                 });
}

std::string ConfigValidator::configValueToString(const ConfigValue& value) {
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

ConfigValue ConfigValidator::stringToConfigValue(const std::string& str) {
    // Try to convert string to appropriate type
    if (str == "true") return true;
    if (str == "false") return false;
    if (str.empty()) return std::string();
    
    // Check for numeric values
    size_t pos;
    try {
        // Try double
        double d = std::stod(str, &pos);
        if (pos == str.length()) {
            return d;
        }
        
        // Try int64_t
        long long i = std::stoll(str, &pos);
        if (pos == str.length()) {
            return i;
        }
    } catch (...) {
        // If parsing fails, return as string
    }
    
    return str;
}

// Config implementation with JSON support
struct Config::Impl {
    std::map<std::string, ConfigValue> entries;
    std::shared_ptr<ConfigValidator> validator;
    std::vector<std::string> validationErrors;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->validator = std::make_shared<ConfigValidator>();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;
Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // First try to load as JSON
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json jsonData;
        file >> jsonData;
        
        // Clear existing config
        pImpl->entries.clear();
        
        // Parse JSON
        fromJson(jsonData);
        
        // Validate
        validate();
        
        return true;
    } catch (const nlohmann::json::parse_error& e) {
        // If JSON parsing fails, try fallback to simple key=value format
        file.clear();
        file.seekg(0);
        
        std::string line;
        while (std::getline(file, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                
                // Remove quotes if present
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                // Try to parse value based on content
                ConfigValue configValue;
                if (value == "true") {
                    configValue = true;
                } else if (value == "false") {
                    configValue = false;
                } else if (value[0] == '"' || value[0] == '\'') {
                    configValue = value.substr(1, value.length() - 2);
                } else {
                    // Try numeric
                    try {
                        size_t posFloat;
                        float f = std::stof(value, &posFloat);
                        if (posFloat == value.length()) {
                            configValue = f;
                        } else {
                            size_t posInt;
                            long long i = std::stoll(value, &posInt);
                            if (posInt == value.length()) {
                                configValue = i;
                            } else {
                                configValue = value;
                            }
                        }
                    } catch (...) {
                        configValue = value;
                    }
                }
                
                pImpl->entries[key] = configValue;
            }
        }
        
        return true;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    // Same implementation as before
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Save as JSON
    nlohmann::json jsonData;
    
    for (const auto& pair : pImpl->entries) {
        jsonData[pair.first] = pair.second;
    }
    
    file << jsonData.dump(2); // Pretty print with 2-space indentation
    
    return true;
}

bool Config::validate() {
    if (!pImpl->validator) {
        return true;
    }
    
    ValidationResult result = pImpl->validator->validate(pImpl->entries);
    pImpl->validationErrors = result.errors;
    
    return result.valid;
}

std::vector<std::string> Config::getValidationErrors() const {
    return pImpl->validationErrors;
}

void Config::applyDefaults() {
    if (pImpl->validator) {
        pImpl->validator->applyDefaults(pImpl->entries);
    }
}

nlohmann::json Config::toJson() const {
    nlohmann::json jsonData;
    
    for (const auto& pair : pImpl->entries) {
        jsonData[pair.first] = pair.second;
    }
    
    return jsonData;
}

bool Config::fromJson(const nlohmann::json& json) {
    try {
        pImpl->entries.clear();
        
        for (auto it = json.begin(); it != json.end(); ++it) {
            pImpl->entries[it.key()] = it.value();
        }
        
        return true;
    } catch (...) {
        return false;
    }
}

void Config::merge(const std::map<std::string, ConfigValue>& other) {
    pImpl->entries.insert(other.begin(), other.end());
}

// Helper method implementations
template<typename T>
std::optional<T> Config::getImpl(const std::string& key) const {
    auto it = pImpl->entries.find(key);
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    try {
        return std::get<T>(it->second);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

// Template method implementations
#define IMPLEMENT_GET_METHOD(Type)
#define IMPLEMENT_GET_OR_METHOD(Type)

#define IMPLEMENT_TEMPLATE_METHODS() \
IMPLEMENT_GET_METHOD(int) \
IMPLEMENT_GET_METHOD(int64_t) \
IMPLEMENT_GET_METHOD(double) \
IMPLEMENT_GET_METHOD(bool) \
IMPLEMENT_GET_METHOD(std::string) \
IMPLEMENT_GET_OR_METHOD(int) \
IMPLEMENT_GET_OR_METHOD(int64_t) \
IMPLEMENT_GET_OR_METHOD(double) \
IMPLEMENT_GET_OR_METHOD(bool) \
IMPLEMENT_GET_OR_METHOD(std::string)

IMPLEMENT_TEMPLATE_METHODS()

} // namespace nlm
