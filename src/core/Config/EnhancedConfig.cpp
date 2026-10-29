// Enhanced Configuration System Implementation
// ============================================

#include "EnhancedConfig.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <json/json.hpp> // Assuming JSON library is available

using json = nlohmann::json;

namespace nlm {

struct EnhancedConfig::Impl {
    // Legacy config for migration
    std::shared_ptr<Config> legacyConfig;
    
    // Parameter lookup for validation
    std::map<std::string, std::function<bool(const ConfigValue&)>> validators;
    
    // Performance cache
    mutable std::map<std::string, bool> validationCache;
    mutable std::map<std::string, std::string> validationErrors;
};

EnhancedConfig::EnhancedConfig() : pImpl(std::make_unique<Impl>()) {
    initializeSchema();
}

EnhancedConfig::~EnhancedConfig() = default;

EnhancedConfig::EnhancedConfig(EnhancedConfig&& other) noexcept : pImpl(std::move(other.pImpl)) {
}

EnhancedConfig& EnhancedConfig::operator=(EnhancedConfig&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

void EnhancedConfig::initializeSchema() {
    // Brain Configuration Parameters
    addParameter("neuron_count", "Number of neurons in the network", ConfigValue(1000), ConfigSource::Default);
    addParameter("region_count", "Number of neural regions", ConfigValue(1), ConfigSource::Default);
    addParameter("connection_probability", "Probability of connection between neurons", ConfigValue(0.1f), ConfigSource::Default);
    
    // Neuron Parameters
    addParameter("neuron_threshold", "Neuron firing threshold (mV)", ConfigValue(-55.0f), ConfigSource::Default);
    addParameter("neuron_resting_potential", "Neuron resting potential (mV)", ConfigValue(-70.0f), ConfigSource::Default);
    addParameter("neuron_refractory_period", "Neuron refractory period (steps)", ConfigValue(5), ConfigSource::Default);
    addParameter("neuron_leak_conductance", "Neuron leak conductance (nS)", ConfigValue(10.0f), ConfigSource::Default);
    
    // Dynamics Parameters
    addParameter("simulation_timestep", "Simulation timestep (s)", ConfigValue(0.001), ConfigSource::Default);
    addParameter("time_constant", "Membrane time constant (ms)", ConfigValue(20.0f), ConfigSource::Default);
    addParameter("membrane_capacitance", "Membrane capacitance (nF)", ConfigValue(1.0f), ConfigSource::Default);
    
    // Plasticity Parameters
    addParameter("stdp_ltp_weight", "STDP LTP weight", ConfigValue(0.02f), ConfigSource::Default);
    addParameter("stdp_ltd_weight", "STDP LTD weight", ConfigValue(0.015f), ConfigSource::Default);
    addParameter("stdp_tau", "STDP time constant (ms)", ConfigValue(20.0f), ConfigSource::Default);
    addParameter("synaptogenesis_rate", "Synapse formation rate", ConfigValue(0.0001f), ConfigSource::Default);
    addParameter("pruning_rate", "Synapse pruning rate", ConfigValue(0.00001f), ConfigSource::Default);
    
    // Memory and Consolidation
    addParameter("replay_interval", "Memory replay interval (steps)", ConfigValue(100), ConfigSource::Default);
    addParameter("consolidation_interval", "Memory consolidation interval (steps)", ConfigValue(1000), ConfigSource::Default);
    addParameter("checkpoint_dir", "Checkpoint directory path", ConfigValue("./checkpoints"), ConfigSource::Default);
    
    // Configuration Options
    addParameter("random_seed", "Random seed for reproducible experiments", ConfigValue(42), ConfigSource::Default);
    addParameter("enable_checkpointing", "Enable checkpoint saving", ConfigValue(true), ConfigSource::Default);
    addParameter("enable_replay", "Enable memory replay", ConfigValue(true), ConfigSource::Default);
    addParameter("enable_development", "Enable developmental changes", ConfigValue(true), ConfigSource::Default);
}

bool EnhancedConfig::loadFromFile(const std::string& filepath) {
    // Try JSON first
    try {
        std::ifstream file(filepath);
        if (file.is_open()) {
            json j;
            file >> j;
            return loadFromJSON(j.dump());
        }
    } catch (...) {
        // Fall back to simple format
    }
    
    // Use legacy file loading for backward compatibility
    if (pImpl->legacyConfig) {
        return pImpl->legacyConfig->loadFromFile(filepath);
    }
    
    return false;
}

bool EnhancedConfig::loadFromArgs(int argc, char** argv) {
    if (pImpl->legacyConfig) {
        return pImpl->legacyConfig->loadFromArgs(argc, argv);
    }
    return false;
}

bool EnhancedConfig::loadFromJSON(const std::string& jsonContent) {
    try {
        json j = json::parse(jsonContent);
        
        for (auto it = j.begin(); it != j.end(); ++it) {
            std::string key = it.key();
            const json& value = *it;
            
            ConfigValue configValue;
            
            if (value.is_number_integer()) {
                configValue = value.get<int64_t>();
            } else if (value.is_number_unsigned()) {
                configValue = value.get<uint64_t>();
            } else if (value.is_number_float()) {
                configValue = value.get<double>();
            } else if (value.is_boolean()) {
                configValue = value.get<bool>();
            } else if (value.is_string()) {
                configValue = value.get<std::string>();
            } else if (value.is_array()) {
                // Handle vector types
                if (value.empty()) {
                    configValue = std::vector<int>{};
                } else if (value[0].is_number_integer()) {
                    std::vector<int64_t> vec = value.get<std::vector<int64_t>>();
                    configValue = vec;
                } else if (value[0].is_number_float()) {
                    std::vector<double> vec = value.get<std::vector<double>>();
                    configValue = vec;
                }
            }
            
            set(key, configValue, ConfigSource::File);
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool EnhancedConfig::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    json j;
    
    // Convert all configuration entries to JSON
    for (const auto& entry : pImpl->legacyConfig->getKeys()) {
        auto value = pImpl->legacyConfig->get<ConfigValue>(entry);
        if (value) {
            // Convert ConfigValue to JSON
            std::visit([&](const auto& val) {
                j[entry] = val;
            }, *value);
        }
    }
    
    file << j.dump(2);
    return true;
}

std::string EnhancedConfig::toJSON() const {
    if (!pImpl->legacyConfig) {
        return "{}";
    }
    
    json j;
    for (const auto& entry : pImpl->legacyConfig->getKeys()) {
        auto value = pImpl->legacyConfig->get<ConfigValue>(entry);
        if (value) {
            std::visit([&](const auto& val) {
                j[entry] = val;
            }, *value);
        }
    }
    
    return j.dump(2);
}

// Template implementations for EnhancedConfig methods
template<typename T>
std::optional<T> EnhancedConfig::get(const std::string& key) const {
    if (pImpl->legacyConfig) {
        return pImpl->legacyConfig->get<T>(key);
    }
    return std::nullopt;
}

template<typename T>
T EnhancedConfig::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

template<typename T>
T EnhancedConfig::getRequired(const std::string& key) const {
    auto val = get<T>(key);
    if (!val.has_value()) {
        throw std::runtime_error("Required configuration parameter missing: " + key);
    }
    return val.value();
}

bool EnhancedConfig::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    if (pImpl->legacyConfig) {
        bool result = pImpl->legacyConfig->has(key);
        pImpl->legacyConfig->set(key, value, source);
        
        // Clear validation cache for this key
        validationCache_.erase(key);
        validationErrors_.erase(key);
        
        return true;
    }
    return false;
}

bool EnhancedConfig::set(const std::string& key, const std::string& value, ConfigSource source) {
    return set(key, ConfigValue(value), source);
}

bool EnhancedConfig::set(const std::string& key, int value, ConfigSource source) {
    return set(key, ConfigValue(value), source);
}

bool EnhancedConfig::set(const std::string& key, double value, ConfigSource source) {
    return set(key, ConfigValue(value), source);
}

bool EnhancedConfig::set(const std::string& key, bool value, ConfigSource source) {
    return set(key, ConfigValue(value), source);
}

bool EnhancedConfig::has(const std::string& key) const {
    return pImpl->legacyConfig && pImpl->legacyConfig->has(key);
}

std::string EnhancedConfig::getTypeName(const std::string& key) const {
    auto schemaIt = schema_.find(key);
    if (schemaIt != schema_.end()) {
        return schemaIt->second.typeName;
    }
    
    // Infer type from current value
    if (pImpl->legacyConfig && pImpl->legacyConfig->has(key)) {
        auto value = pImpl->legacyConfig->get<ConfigValue>(key);
        if (value) {
            return std::visit([](const auto& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, int64_t>) return "int64_t";
                else if constexpr (std::is_same_v<T, double>) return "double";
                else if constexpr (std::is_same_v<T, bool>) return "bool";
                else if constexpr (std::is_same_v<T, std::string>) return "string";
                else if constexpr (std::is_same_v<T, std::vector<int64_t>>) return "vector<int64_t>";
                else if constexpr (std::is_same_v<T, std::vector<double>>) return "vector<double>";
                else if constexpr (std::is_same_v<T, std::vector<std::string>>) return "vector<string>";
                else return "unknown";
            }, *value);
        }
    }
    
    return "unknown";
}

std::string EnhancedConfig::getDescription(const std::string& key) const {
    auto schemaIt = schema_.find(key);
    if (schemaIt != schema_.end()) {
        return schemaIt->second.description;
    }
    return "";
}

bool EnhancedConfig::validate(const std::string& key) const {
    // Check cache first
    auto cacheIt = validationCache_.find(key);
    if (cacheIt != validationCache_.end()) {
        return cacheIt->second;
    }
    
    bool isValid = true;
    std::string error;
    
    if (!has(key)) {
        isValid = false;
        error = "Parameter missing";
    } else {
        auto value = pImpl->legacyConfig->get<ConfigValue>(key);
        if (!value) {
            isValid = false;
            error = "Parameter value inaccessible";
        } else {
            // Validate against schema
            auto schemaIt = schema_.find(key);
            if (schemaIt != schema_.end()) {
                const auto& param = schemaIt->second;
                
                // Validate type
                isValid = validateValueType(*value, param.typeName);
                if (!isValid) {
                    error = "Type mismatch: expected " + param.typeName;
                } else {
                    // Validate range
                    isValid = validateRange(*value, param);
                    if (!isValid) {
                        error = "Value out of range";
                    } else {
                        // Validate custom constraints
                        if (param.customValidator && !param.customValidator(*value)) {
                            isValid = false;
                            error = "Custom validation failed";
                        } else {
                            // Validate dependencies
                            isValid = validateDependencies(key);
                            if (!isValid) {
                                error = "Dependency validation failed";
                            }
                        }
                    }
                }
            }
        }
    }
    
    // Cache result
    validationCache_[key] = isValid;
    if (!isValid) {
        validationErrors_[key] = error;
    }
    
    return isValid;
}

bool EnhancedConfig::validateAll() const {
    bool allValid = true;
    for (const auto& key : pImpl->legacyConfig->getKeys()) {
        if (!validate(key)) {
            allValid = false;
        }
    }
    return allValid;
}

std::string EnhancedConfig::summary() const {
    if (!pImpl->legacyConfig) {
        return "No configuration loaded";
    }
    
    std::ostringstream oss;
    oss << "Enhanced Configuration Summary:" << std::endl;
    oss << "Total parameters: " << pImpl->legacyConfig->getKeys().size() << std::endl;
    
    for (const auto& key : pImpl->legacyConfig->getKeys()) {
        oss << "  " << key << " = ";
        
        auto value = pImpl->legacyConfig->get<ConfigValue>(key);
        if (value) {
            std::visit([&oss](const auto& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    oss << "\"" << val << "\"";
                } else {
                    oss << val;
                }
            }, *value);
        }
        
        oss << " (" << (validate(key) ? "VALID" : "INVALID") << ")" << std::endl;
    }
    
    return oss.str();
}

std::string EnhancedConfig::validationSummary() const {
    std::ostringstream oss;
    oss << "Validation Status:" << std::endl;
    
    int validCount = 0;
    int invalidCount = 0;
    
    for (const auto& key : pImpl->legacyConfig->getKeys()) {
        if (validate(key)) {
            validCount++;
        } else {
            invalidCount++;
            oss << "  " << key << ": " << validationErrors_[key] << std::endl;
        }
    }
    
    oss << "Valid: " << validCount << ", Invalid: " << invalidCount << std::endl;
    return oss.str();
}

void EnhancedConfig::addParameter(const std::string& key,
                                 const std::string& description,
                                 const ConfigValue& defaultValue,
                                 ConfigSource source) {
    ConfigParameter param;
    param.key = key;
    param.description = description;
    param.defaultValue = defaultValue;
    param.defaultSource = source;
    
    // Set type name based on default value
    param.typeName = std::visit([](const auto& val) {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, int64_t>) return "int64_t";
        else if constexpr (std::is_same_v<T, double>) return "double";
        else if constexpr (std::is_same_v<T, bool>) return "bool";
        else if constexpr (std::is_same_v<T, std::string>) return "string";
        else if constexpr (std::is_same_v<T, std::vector<int64_t>>) return "vector<int64_t>";
        else if constexpr (std::is_same_v<T, std::vector<double>>) return "vector<double>";
        else if constexpr (std::is_same_v<T, std::vector<std::string>>) return "vector<string>";
        else return "unknown";
    }, defaultValue);
    
    schema_[key] = param;
}

void EnhancedConfig::clearValidationCache() {
    validationCache_.clear();
    validationErrors_.clear();
}

bool EnhancedConfig::loadPreset(const std::string& presetName) {
    if (presetName == "default") {
        // Set default values for all parameters
        for (const auto& param : schema_) {
            set(param.first, param.second.defaultValue, param.second.defaultSource);
        }
        return true;
    } else if (presetName == "small") {
        set("neuron_count", ConfigValue(100), ConfigSource::File);
        set("region_count", ConfigValue(1), ConfigSource::File);
        return true;
    } else if (presetName == "large") {
        set("neuron_count", ConfigValue(10000), ConfigSource::File);
        set("region_count", ConfigValue(4), ConfigSource::File);
        return true;
    }
    
    return false;
}

bool EnhancedConfig::savePreset(const std::string& presetName) const {
    std::string filename = presetName + ".cfg";
    return saveToFile(filename);
}

std::vector<std::string> EnhancedConfig::getAvailablePresets() const {
    return {"default", "small", "large"};
}

bool EnhancedConfig::validateDependencies(const std::string& key) const {
    auto schemaIt = schema_.find(key);
    if (schemaIt == schema_.end()) {
        return true;
    }
    
    const auto& param = schemaIt->second;
    
    // Check if required dependencies are satisfied
    for (const auto& depKey : param.dependsOn) {
        if (!has(depKey) || !validate(depKey)) {
            return false;
        }
    }
    
    // Call custom dependency validator if provided
    if (param.dependencyValidator) {
        ConfigValue currentValue;
        if (pImpl->legacyConfig && pImpl->legacyConfig->has(key)) {
            currentValue = *pImpl->legacyConfig->get<ConfigValue>(key);
        }
        return param.dependencyValidator(currentValue, *pImpl->legacyConfig);
    }
    
    return true;
}

std::vector<std::string> EnhancedConfig::getDependencies(const std::string& key) const {
    auto schemaIt = schema_.find(key);
    if (schemaIt != schema_.end()) {
        return schemaIt->second.dependsOn;
    }
    return {};
}

bool EnhancedConfig::migrateFromLegacy(const Config& legacyConfig) {
    try {
        pImpl->legacyConfig = std::make_shared<Config>(legacyConfig);
        
        // Apply schema constraints
        for (const auto& param : schema_) {
            if (!has(param.first)) {
                set(param.first, param.second.defaultValue, param.second.defaultSource);
            }
        }
        
        return true;
    } catch (...) {
        return false;
    }
}

// Template instantiations for common types
template std::optional<int> EnhancedConfig::get<int>(const std::string&) const;
template std::optional<int64_t> EnhancedConfig::get<int64_t>(const std::string&) const;
template std::optional<double> EnhancedConfig::get<double>(const std::string&) const;
template std::optional<bool> EnhancedConfig::get<bool>(const std::string&) const;
template std::optional<std::string> EnhancedConfig::get<std::string>(const std::string&) const;

template int EnhancedConfig::getOr<int>(const std::string&, const int&) const;
template int64_t EnhancedConfig::getOr<int64_t>(const std::string&, const int64_t&) const;
template double EnhancedConfig::getOr<double>(const std::string&, const double&) const;
template bool EnhancedConfig::getOr<bool>(const std::string&, const bool&) const;
template std::string EnhancedConfig::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm