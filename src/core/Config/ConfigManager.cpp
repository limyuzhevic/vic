// ConfigManager.cpp - Implementation of improved configuration system
#include "ConfigManager.hpp"
#include "src/core/Config/Config.hpp"
#include "src/utils/ErrorHandlingUtils.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <iomanip>

namespace nlm {

namespace {
    // Helper functions
    std::string toLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), 
                      [](unsigned char c) { return std::tolower(c); });
        return result;
    }
    
    std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }
    
    bool stringToBool(const std::string& str) {
        std::string lower = toLower(trim(str));
        return lower == "true" || lower == "1" || lower == "yes" || lower == "on";
    }
    
    int stringToInt(const std::string& str) {
        int value = std::stoi(str);
        return value;
    }
    
    int64_t stringToInt64(const std::string& str) {
        int64_t value = std::stoll(str);
        return value;
    }
    
    double stringToDouble(const std::string& str) {
        double value = std::stod(str);
        return value;
    }
    
    std::vector<int> stringToIntVector(const std::string& str) {
        std::vector<int> result;
        std::stringstream ss(str);
        std::string token;
        while (std::getline(ss, token, ',')) {
            try {
                result.push_back(stringToInt(trim(token)));
            } catch (...) {
                // Skip invalid entries
            }
        }
        return result;
    }
    
    std::vector<double> stringToDoubleVector(const std::string& str) {
        std::vector<double> result;
        std::stringstream ss(str);
        std::string token;
        while (std::getline(ss, token, ',')) {
            try {
                result.push_back(stringToDouble(trim(token)));
            } catch (...) {
                // Skip invalid entries
            }
        }
        return result;
    }
    
    std::vector<std::string> stringToStringVector(const std::string& str) {
        std::vector<std::string> result;
        std::stringstream ss(str);
        std::string token;
        while (std::getline(ss, token, ',')) {
            result.push_back(trim(token));
        }
        return result;
    }
}

// ConfigValidator implementation
std::map<std::string, ConfigSchemaEntry> ConfigValidator::schemaMap_;

bool ConfigValidator::validate(const std::string& key, const ConfigValue& value,
                              ErrorCategory category) {
    auto it = schemaMap_.find(key);
    if (it == schemaMap_.end()) {
        // No schema defined, use basic validation
        return true;
    }
    
    const ConfigSchemaEntry& schema = it->second;
    
    // Check type compatibility
    bool typeMatches = false;
    if (std::holds_alternative<int>(value) && 
        (std::is_same_v<int, decltype(schema.defaultValue)> || 
         std::is_same_v<int64_t, decltype(schema.defaultValue)>)) {
        typeMatches = true;
    } else if (std::holds_alternative<int64_t>(value) && 
               (std::is_same_v<int, decltype(schema.defaultValue)> || 
                std::is_same_v<int64_t, decltype(schema.defaultValue)>)) {
        typeMatches = true;
    } else if (std::holds_alternative<double>(value) && 
               (std::is_same_v<double, decltype(schema.defaultValue)> || 
                std::is_same_v<int, decltype(schema.defaultValue)> || 
                std::is_same_v<int64_t, decltype(schema.defaultValue)>)) {
        typeMatches = true;
    } else if (std::holds_alternative<bool>(value) && 
               std::is_same_v<bool, decltype(schema.defaultValue)>) {
        typeMatches = true;
    } else if (std::holds_alternative<std::string>(value) && 
               std::is_same_v<std::string, decltype(schema.defaultValue)>) {
        typeMatches = true;
    } else if (std::holds_alternative<std::vector<int>>(value) && 
               std::is_same_v<std::vector<int>, decltype(schema.defaultValue)>) {
        typeMatches = true;
    } else if (std::holds_alternative<std::vector<double>>(value) && 
               std::is_same_v<std::vector<double>, decltype(schema.defaultValue)>) {
        typeMatches = true;
    } else if (std::holds_alternative<std::vector<std::string>>(value) && 
               std::is_same_v<std::vector<std::string>, decltype(schema.defaultValue)>) {
        typeMatches = true;
    }
    
    if (!typeMatches) {
        NLM_THROW(category, "Type mismatch for configuration key: " + key);
        return false;
    }
    
    // Run custom validator if provided
    if (schema.validator && !schema.validator(value)) {
        std::string errorMsg = "Validation failed for key: " + key;
        if (!schema.validationMessages.empty()) {
            auto msgIt = schema.validationMessages.find("validation");
            if (msgIt != schema.validationMessages.end()) {
                errorMsg = msgIt->second;
            }
        }
        NLM_THROW(category, errorMsg);
        return false;
    }
    
    return true;
}

bool ConfigValidator::validateAll(const Config& config,
                                 ErrorCategory category) {
    bool allValid = true;
    for (const auto& entry : config.getKeys()) {
        if (!validate(entry, config.get<ConfigValue>(entry).value(), category)) {
            allValid = false;
        }
    }
    return allValid;
}

std::string ConfigValidator::getValidationError(const std::string& key, const ConfigValue& value) {
    return "Validation failed for key: " + key;
}

void ConfigValidator::registerSchema(const ConfigSchemaEntry& schema) {
    schemaMap_[schema.key] = schema;
}

std::optional<ConfigSchemaEntry> ConfigValidator::getSchema(const std::string& key) {
    auto it = schemaMap_.find(key);
    if (it != schemaMap_.end()) {
        return it->second;
    }
    return std::nullopt;
}

bool ConfigValidator::hasSchema(const std::string& key) {
    return schemaMap_.find(key) != schemaMap_.end();
}

void ConfigValidator::loadDefaultSchema() {
    // Register default schema entries
    std::vector<ConfigSchemaEntry> defaultSchema = {
        {
            "neuron_count",
            ConfigValue(static_cast<int64_t>(1000)),
            ConfigSource::Default,
            "Number of neurons in the network",
            ConfigCategories::Neural,
            true,
            [](const ConfigValue& v) { return std::holds_alternative<int64_t>(v) && std::get<int64_t>(v) > 0; },
            {
                {"validation", "Neuron count must be a positive integer"},
                {"range", "1 to 10000"}
            }
        },
        {
            "connection_probability",
            ConfigValue(0.1f),
            ConfigSource::Default,
            "Probability of connection between neurons",
            ConfigCategories::Neural,
            false,
            [](const ConfigValue& v) {
                if (!std::holds_alternative<float>(v)) return false;
                float p = std::get<float>(v);
                return p >= 0.0f && p <= 1.0f;
            },
            {
                {"validation", "Connection probability must be between 0.0 and 1.0"},
                {"range", "0.0 to 1.0"}
            }
        },
        {
            "simulation_timestep",
            ConfigValue(0.001),
            ConfigSource::Default,
            "Simulation time step in seconds",
            ConfigCategories::Simulation,
            false,
            [](const ConfigValue& v) {
                if (!std::holds_alternative<double>(v)) return false;
                double dt = std::get<double>(v);
                return dt > 0.0 && dt < 1.0;
            },
            {
                {"validation", "Simulation timestep must be positive and less than 1 second"},
                {"range", "0.0001 to 1.0"}
            }
        },
        {
            "random_seed",
            ConfigValue(static_cast<int64_t>(42)),
            ConfigSource::Default,
            "Random seed for reproducibility",
            ConfigCategories::General,
            false,
            [](const ConfigValue& v) { return std::holds_alternative<int64_t>(v); },
            {}
        },
        {
            "stdp_ltp_weight",
            ConfigValue(0.01f),
            ConfigSource::Default,
            "STDP long-term potentiation weight",
            ConfigCategories::Plasticity,
            false,
            [](const ConfigValue& v) {
                if (!std::holds_alternative<float>(v)) return false;
                return std::get<float>(v) >= 0.0f;
            },
            {}
        },
        {
            "stdp_ltd_weight",
            ConfigValue(0.012f),
            ConfigSource::Default,
            "STDP long-term depression weight",
            ConfigCategories::Plasticity,
            false,
            [](const ConfigValue& v) {
                if (!std::holds_alternative<float>(v)) return false;
                return std::get<float>(v) >= 0.0f;
            },
            {}
        },
        {
            "stdp_tau",
            ConfigValue(20.0f),
            ConfigSource::Default,
            "STDP time constant in milliseconds",
            ConfigCategories::Plasticity,
            false,
            [](const ConfigValue& v) {
                if (!std::holds_alternative<float>(v)) return false;
                return std::get<float>(v) > 0.0f;
            },
            {}
        }
    };
    
    for (const auto& entry : defaultSchema) {
        registerSchema(entry);
    }
}

void ConfigValidator::clearSchema() {
    schemaMap_.clear();
}

// EnvironmentVariableLoader implementation
bool EnvironmentVariableLoader::loadFromEnvironment(Config& config,
                                                   const std::string& prefix,
                                                   ConfigSource source) {
    bool success = true;
    
    // Common environment variable names
    std::vector<std::string> commonVars = {
        "NLM_NEURON_COUNT",
        "NLM_CONNECTION_PROBABILITY",
        "NLM_SIMULATION_TIMESTEP",
        "NLM_RANDOM_SEED",
        "NLM_STDP_LTP_WEIGHT",
        "NLM_STDP_LTD_WEIGHT",
        "NLM_STDP_TAU"
    };
    
    for (const auto& varName : commonVars) {
        if (hasEnvVar(varName)) {
            std::string value = getEnvVar(varName);
            std::string key = varName.substr(prefix.size()); // Remove prefix
            
            // Convert value to appropriate type
            try {
                // Try to parse as int first
                try {
                    int64_t intVal = stringToInt64(value);
                    config.set(key, intVal, source);
                } catch (...) {
                    // Try to parse as float
                    try {
                        float floatVal = static_cast<float>(stringToDouble(value));
                        config.set(key, floatVal, source);
                    } catch (...) {
                        // Try to parse as bool
                        try {
                            bool boolVal = stringToBool(value);
                            config.set(key, boolVal, source);
                        } catch (...) {
                            // Keep as string if all else fails
                            config.set(key, value, source);
                        }
                    }
                }
                
                NLM_LOG_INFO(std::string("Loaded environment variable: ") + varName + 
                           " = " + value + " for key: " + key);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Failed to parse environment variable ") + varName + 
                           ": " + e.what());
                success = false;
            }
        }
    }
    
    return success;
}

template<typename T>
T EnvironmentVariableLoader::convertEnvValue(const std::string& str) {
    // Implementation for specific types
    if constexpr (std::is_same_v<T, int>) {
        return stringToInt(str);
    } else if constexpr (std::is_same_v<T, int64_t>) {
        return stringToInt64(str);
    } else if constexpr (std::is_same_v<T, double>) {
        return stringToDouble(str);
    } else if constexpr (std::is_same_v<T, float>) {
        return static_cast<float>(stringToDouble(str));
    } else if constexpr (std::is_same_v<T, bool>) {
        return stringToBool(str);
    } else if constexpr (std::is_same_v<T, std::string>) {
        return str;
    } else {
        throw std::runtime_error("Unsupported type for environment variable conversion");
    }
}

bool EnvironmentVariableLoader::hasEnvVar(const std::string& name) {
    const char* value = std::getenv(name.c_str());
    return value != nullptr && std::strlen(value) > 0;
}

std::string EnvironmentVariableLoader::getEnvVar(const std::string& name) {
    const char* value = std::getenv(name.c_str());
    if (value != nullptr) {
        return std::string(value);
    }
    throw std::runtime_error("Environment variable not found: " + name);
}

// ConfigManager implementation
struct ConfigManager::Impl {
    std::vector<std::string> loadedFiles;
    std::map<std::string, ConfigSource> sourceCounts;
    bool isValidated = false;
};

ConfigManager::ConfigManager() : pImpl(std::make_unique<Impl>()) {
    // Initialize default schema
    ConfigValidator::loadDefaultSchema();
    
    // Initialize categories
    categoryIndex_[ConfigCategories::General] = {
        "random_seed", "simulation_timestep"
    };
    
    categoryIndex_[ConfigCategories::Simulation] = {
        "simulation_timestep", "neuron_count", "region_count"
    };
    
    categoryIndex_[ConfigCategories::Neural] = {
        "neuron_count", "region_count", "connection_probability"
    };
    
    categoryIndex_[ConfigCategories::Plasticity] = {
        "stdp_ltp_weight", "stdp_ltd_weight", "stdp_tau",
        "synaptogenesis_rate", "pruning_rate"
    };
    
    categoryIndex_[ConfigCategories::Memory] = {
        "working_memory_capacity", "max_episodes"
    };
    
    categoryIndex_[ConfigCategories::Development] = {
        "developmental_stage_start", "plasticity_modulation_rate"
    };
    
    categoryIndex_[ConfigCategories::Neuromodulation] = {
        "dopamine_level", "curiosity_strength", "novelty_sensitivity"
    };
    
    categoryIndex_[ConfigCategories::Cognition] = {
        "planning_depth", "attention_inhibition", "attention_excitation"
    };
}

ConfigManager::~ConfigManager() = default;

ConfigManager::ConfigManager(ConfigManager&&) noexcept = default;

ConfigManager& ConfigManager::operator=(ConfigManager&&) noexcept = default;

bool ConfigManager::loadConfiguration(const std::string& configFile,
                                     const std::vector<std::string>& commandLineArgs) {
    try {
        // Start with a fresh configuration
        Config newConfig;
        
        // Load from file if specified
        if (!configFile.empty()) {
            NLM_LOG_INFO("Loading configuration from file: " + configFile);
            if (!newConfig.loadFromFile(configFile)) {
                NLM_LOG_WARNING("Failed to load configuration from file: " + configFile);
            } else {
                pImpl->loadedFiles.push_back(configFile);
            }
        }
        
        // Load from command line arguments
        if (!commandLineArgs.empty()) {
            NLM_LOG_INFO("Loading configuration from command line arguments");
            newConfig.loadFromArgs(commandLineArgs.size(), const_cast<char**>(commandLineArgs.data()));
        }
        
        // Apply environment variable overrides
        applyEnvironmentVariableOverrides(newConfig);
        
        // Apply default values
        applyDefaultValues(newConfig);
        
        // Validate configuration
        if (!validateAndNormalize(newConfig)) {
            NLM_LOG_ERROR("Configuration validation failed: " + validationError_);
            return false;
        }
        
        // Merge with existing configuration
        if (!mergeConfiguration(newConfig)) {
            return false;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading configuration: ") + e.what());
        return false;
    }
}

bool ConfigManager::mergeConfiguration(const Config& otherConfig,
                                      ConfigSource source) {
    try {
        for (const auto& key : otherConfig.getKeys()) {
            auto value = otherConfig.get<ConfigValue>(key);
            if (value.has_value()) {
                getConfig().set(key, value.value(), source);
                pImpl->sourceCounts[key]++;
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception merging configuration: ") + e.what());
        return false;
    }
}

bool ConfigManager::applyConfiguration(const Config& newConfig) {
    try {
        clear();
        return mergeConfiguration(newConfig);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception applying configuration: ") + e.what());
        return false;
    }
}

bool ConfigManager::validateAndNormalize(Config& config) const {
    std::string errors;
    
    // Validate all configuration values
    for (const auto& key : config.getKeys()) {
        auto value = config.get<ConfigValue>(key);
        if (value.has_value() && !validateSingleValue(key, value.value(), ErrorCategory::Validation)) {
            errors += "Key '" + key + "': " + validationError_ + "\n";
        }
    }
    
    if (!errors.empty()) {
        validationError_ = "Configuration validation failed:\n" + errors;
        return false;
    }
    
    // Normalize values
    for (auto& key : config.getKeys()) {
        auto value = config.get<ConfigValue>(key);
        if (value.has_value()) {
            // Normalize value based on schema
            auto schemaOpt = ConfigValidator::getSchema(key);
            if (schemaOpt.has_value()) {
                const ConfigSchemaEntry& schema = schemaOpt.value();
                
                // Apply normalization based on type
                if (std::holds_alternative<int64_t>(value.value()) &&
                    std::holds_alternative<int64_t>(schema.defaultValue)) {
                    int64_t val = std::get<int64_t>(value.value());
                    int64_t defaultVal = std::get<int64_t>(schema.defaultValue);
                    
                    // Clamp to reasonable range based on default
                    if (key == "neuron_count" && (val < 1 || val > 100000)) {
                        val = std::clamp(val, static_cast<int64_t>(1), static_cast<int64_t>(100000));
                        config.set(key, val, config.get<ConfigEntry>(key).source);
                    }
                }
                else if (std::holds_alternative<float>(value.value()) &&
                         std::holds_alternative<float>(schema.defaultValue)) {
                    float val = std::get<float>(value.value());
                    float defaultVal = std::get<float>(schema.defaultValue);
                    
                    if (key == "connection_probability") {
                        val = std::clamp(val, 0.0f, 1.0f);
                        config.set(key, val, config.get<ConfigEntry>(key).source);
                    } else if (key == "simulation_timestep") {
                        val = std::clamp(val, 0.0001f, 1.0f);
                        config.set(key, val, config.get<ConfigEntry>(key).source);
                    }
                }
            }
        }
    }
    
    pImpl->isValidated = true;
    validationError_.clear();
    return true;
}

template<typename T>
std::optional<T> ConfigManager::getValidated(const std::string& key) const {
    auto value = getConfig().get<T>(key);
    if (!value.has_value()) {
        return std::nullopt;
    }
    
    // Validate the value
    if (!validateSingleValue(key, value.value(), ErrorCategory::Validation)) {
        return std::nullopt;
    }
    
    return value;
}

template<typename T>
T ConfigManager::getValidatedOr(const std::string& key, const T& defaultValue) const {
    auto value = getValidated<T>(key);
    return value.has_value() ? value.value() : defaultValue;
}

void ConfigManager::resetToDefaults() {
    clear();
    
    // Re-apply default values
    Config config;
    applyDefaultValues(config);
    
    // Merge with the manager
    mergeConfiguration(config, ConfigSource::Default);
}

void ConfigManager::clear() {
    pImpl->loadedFiles.clear();
    pImpl->sourceCounts.clear();
    pImpl->isValidated = false;
    
    // Clear the underlying Config
    Config config;
    config.clear();
}

size_t ConfigManager::getConfigurationSourceCount() const {
    return pImpl->sourceCounts.size();
}

size_t ConfigManager::getConfigurationEntryCount() const {
    return getConfig().getKeys().size();
}

void ConfigManager::updateCategoryIndex() const {
    // Rebuild category index
    categoryIndex_.clear();
    
    for (const auto& entry : getConfig().getKeys()) {
        auto schemaOpt = ConfigValidator::getSchema(entry);
        if (schemaOpt.has_value()) {
            const std::string& category = schemaOpt.value().category;
            categoryIndex_[category].push_back(entry);
        }
    }
}

bool ConfigManager::validateSingleValue(const std::string& key, const ConfigValue& value,
                                       ErrorCategory category) const {
    // Use ConfigValidator to validate
    return ConfigValidator::validate(key, value, category);
}

void ConfigManager::applyDefaultValues(Config& config) const {
    // Apply default values from schema
    for (const auto& [key, schema] : ConfigValidator::schemaMap_) {
        if (!config.has(key)) {
            config.set(key, schema.defaultValue, schema.defaultSource);
        }
    }
}

void ConfigManager::applyEnvironmentVariableOverrides(Config& config) const {
    // Load from environment variables
    EnvironmentVariableLoader::loadFromEnvironment(config, "NLM_");
}

// Explicit template instantiations
template std::optional<int> ConfigManager::getValidated<int>(const std::string& key) const;
template std::optional<int64_t> ConfigManager::getValidated<int64_t>(const std::string& key) const;
template std::optional<double> ConfigManager::getValidated<double>(const std::string& key) const;
template std::optional<float> ConfigManager::getValidated<float>(const std::string& key) const;
template std::optional<bool> ConfigManager::getValidated<bool>(const std::string& key) const;
template std::optional<std::string> ConfigManager::getValidated<std::string>(const std::string& key) const;

// EnvironmentVariableLoader template instantiations

template int EnvironmentVariableLoader::convertEnvValue<int>(const std::string& str);
template int64_t EnvironmentVariableLoader::convertEnvValue<int64_t>(const std::string& str);
template double EnvironmentVariableLoader::convertEnvValue<double>(const std::string& str);
template float EnvironmentVariableLoader::convertEnvValue<float>(const std::string& str);
template bool EnvironmentVariableLoader::convertEnvValue<bool>(const std::string& str);
template std::string EnvironmentVariableLoader::convertEnvValue<std::string>(const std::string& str);

} // namespace nlm